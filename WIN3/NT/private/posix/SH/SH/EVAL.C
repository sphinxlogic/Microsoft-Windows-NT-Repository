/*
 * Expansion - quoting, separation, substitution, globbing
 */

static char *RCSid = "$Id: eval.c,v 3.4 89/03/27 15:49:55 egisin Exp $";

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#if 0 && XN
#include <fcntl.h>
#endif
#include "sh.h"
#include "lex.h"
#include "tree.h"
#include "table.h"
#include "expand.h"

/*
 * string expansion
 *
 * first pass: quoting, IFS separation, ${} and $() substitution.
 * second pass: filename expansion (*?[]~).
 */

/* expansion generator state */
typedef struct Expand {
	/* int  type; */	/* see expand() */
	char   *str;		/* string */
	union {
		char  **strv;	/* string[] */
		FILE   *file;	/* file */
	} u;			/* source */
	short	split;		/* split "$@"*/
} Expand;

#define	XBASE	0		/* scanning original */
#define	XSUB	1		/* expanding ${} string */
#define	XARGSEP	2		/* ifs0 between "$@" */
#define	XARG	3		/* expanding $*, $@ */
#define	XCOM	4		/* expanding $() */

#if 1
extern	int	gmatch ARGS((register char *, register char *)); /* in misc.c */

#endif /* Xn 1992-07-20 */
static	void	expand ARGS((char *, XPtrV *, int));
static	int	comsub ARGS((Expand *, char *comm));
static	int	varsub ARGS((Expand *, char *name, int stype));
static	void	glob ARGS((char *cp, XPtrV *wp));
static	void	globit ARGS((char *ds, char *dp, char *sp, XPtrV *wp, int check));
static	char   *tilde ARGS((char *wp));
static	char   *trimsub ARGS((char *str, char *pat, int how));

int	ifs0 = ' ';		/* todo: first char of $IFS */

/* compile and expand word */
char *
#if __STDC__
substitute(char Const *cp, int f)
#else
substitute(cp, f)
	char Const *cp;
	int f;
#endif /* Xn 1992-07-17 */
{
	struct source *s, *sold;

	sold = source;
	s = pushs(SWSTR);
	s->str = (char *) cp;
	source = s;
	if (yylex(ONEWORD) != LWORD)
		errorf("eval:substitute error\n");
	source = sold;
	return evalstr(yylval.cp, f);
}

/*
 * expand arg-list
 */
char **
#if __STDC__
eval(register char **ap, int f)
#else
eval(ap, f)
	register char **ap;
	int f;
#endif /* Xn 1992-07-17 */
{
	XPtrV w;

	if (*ap == NULL)
		return ap;
	XPinit(w, 32);
	XPput(w, NULL);		/* space for shell name */
#ifdef	SHARPBANG
	XPput(w, NULL);		/* and space for one arg */
#endif
	while (*ap != NULL)
		expand(*ap++, &w, f);
	XPput(w, NULL);
#ifdef	SHARPBANG
	return (char **) XPclose(w) + 2;
#else
	return (char **) XPclose(w) + 1;
#endif
}

/*
 * expand string
 */
char *
#if __STDC__
evalstr(register char *cp, int f)
#else
evalstr(cp, f)
	register char *cp;
	int f;
#endif /* Xn 1992-07-17 */
{
	XPtrV w;

	XPinit(w, 1);
	expand(cp, &w, f);
	cp = (XPsize(w) == 0) ? "" : (char*) *XPptrv(w);
	XPfree(w);
	return cp;
}

/* for nested substitution: ${var:=$var2} */
typedef struct SubType {
	short	type;		/* [=+-?%#] action after expanded word */
	short	base;		/* begin position of expanded word */
	char   *name;		/* name for ${var=word} */
} SubType;

static void
#if __STDC__
expand(char *cp, register XPtrV *wp, int f)
#else
expand(cp, wp, f)
	char *cp;		/* input word */
	register XPtrV *wp;	/* output words */
	int f;			/* DO* flags */
#endif /* Xn 1992-07-17 */
{
	register int c;
	register int type = XBASE; /* expansion type */
	register int quote = 0;	/* quoted */
	int quotestack[11];	/* Keep this bigger than the subtype stack */
	register int *qst = quotestack + 11;	/* This too, of course */
	XString ds;		/* destination string */
	register char *dp, *sp;	/* dest., source */
	int fdo, word, combase;	/* second pass flags; have word */
	Expand x;		/* expansion variables */
	SubType subtype [10];	/* substitution type stack */
	register SubType *st = subtype + 10;
	int newlines;		/* For trailing newlines in COMSUB */

	if (cp == NULL)
		errorf("eval:expand(NULL)\n");
#if 0 && XN
	(void) fprintf(stderr, "cp: \"%s\"\n", cp);
	(void) fflush(stderr);
#endif
	if (flag[FNOGLOB])
		f &= ~ DOGLOB;

	Xinit(ds, dp, 128);	/* init dest. string */
	type = XBASE;
	sp = cp;
	fdo = 0;
	word = !(f&DOBLANK);

#if 0
	while (1) {
#else
	for (;;) {
#endif /* Xn 1992-07-20 */
		Xcheck(ds, dp);

		switch (type) {
		  case XBASE:	/* original prefixed string */
			c = *sp++;
			switch (c) {
			  case EOS:
				c = 0;
				break;
			  case CHAR:
				c = *sp++;
				break;
			  case QCHAR:
				quote |= 2; /* temporary quote */
				c = *sp++;
				break;
			  case OQUOTE:
				word = quote = 1;
				continue;
			  case CQUOTE:
				quote = 0;
				continue;
			  case COMSUB:
				type = comsub(&x, sp);
				sp = strchr(sp, 0) + 1;
				combase = Xsavepos(ds, dp);
				newlines = 0;
				continue;
			  case OSUBST: /* ${var{:}[=+-?]word} */
				cp = sp; 		/* variable */
				sp = strchr(sp, 0) + 1;	/* skip variable */
				c = (*sp == CSUBST) ? 0 : *sp++;
				if ((c&0x7F) == '#' || (c&0x7F) == '%') {
					if (flag[FNOUNSET] &&
					    strval(global(cp)) == null)
						errorf("%s: unset variable\n", cp);
					f |= DOPAT;
					type = XBASE;
					*--qst = quote;
					quote = 0;
				} else
					type = varsub(&x, cp, c);
				if (type == XBASE) {	/* expand? */
					if (st == subtype)
						errorf("ridiculous ${} nesting\n");
					--st;
#if 0
					st->type = c;
					st->base = Xsavepos(ds, dp);
#else
					st->type = (short)c;
					st->base = (short)Xsavepos(ds, dp);
#endif /* Xn 1992-07-20 */
					st->name = cp;
				} else
					sp = wdscan(sp, CSUBST); /* skip word */
				continue;
			  case CSUBST: /* only get here if expanding word */
				*dp = 0;
				if (f&DOGLOB)
					f &= ~DOPAT;
				switch (st->type&0x7F) {
				  case '#':
				  case '%':
					*dp = 0;
					dp = Xrestpos(ds, dp, st->base);
					quote = *qst++;
					x.str = trimsub(strval(global(st->name)),
						dp, st->type);
					type = XSUB;
					continue;
				  case '=':
#if 0
					if ((x.u.vp->flag&RDONLY))
						errorf("cannot set readonly %s\n", cp);
#endif
					setstr(global(st->name), Xrestpos(ds, dp, st->base));
					break;
				  case '?':
					if (dp == Xrestpos(ds, dp, st->base))
						errorf("missing value for %s\n", cp);
					else
						errorf("%s\n", Xrestpos(ds, dp, st->base));
				}
				st++;
				type = XBASE;
				continue;
			}
			break;

		  case XSUB:
			if ((c = *x.str++) == 0) {
				type = XBASE;
				continue;
			}
			break;

		  case XARGSEP:
			type = XARG;
			quote = 1;
		  case XARG:
			if ((c = *x.str++) == 0) {
				if ((x.str = *x.u.strv++) == NULL) {
					type = XBASE;
					continue;
				} else if (quote && x.split) {
					/* terminate word for "$@" */
					type = XARGSEP;
					quote = 0;
				}
				c = ifs0;
			}
			break;

		  case XCOM:
			if (newlines) {		/* Spit out saved nl's */
				c = '\n';
				--newlines;
			} else {
				while ((c = getc(x.u.file)) == '\n')
					newlines++;	/* Save newlines */
				if (newlines && c != EOF) {
					ungetc(c, x.u.file);
					c = '\n';
					--newlines;
				}
			}
			if (c == EOF) {
				cp = Xrestpos(ds, sp, combase);
				newlines = 0;
				fclose(x.u.file);
				if (x.split)
					waitlast();
				type = XBASE;
				continue;
			}
			break;
		}

		/* check for end of word or IFS separation */
		if (c == 0 || !quote && (f&DOBLANK) && ctype(c, C_IFS)) {
			if (word) {
				*dp++ = 0;
				cp = Xclose(ds, dp);
				if (fdo&DOTILDE)
					cp = tilde(cp);
				if (fdo&DOGLOB)
					glob(cp, wp);
				else
					{XPput(*wp, cp);}
				fdo = word = 0;
				if (c != 0)
					Xinit(ds, dp, 128);
			} else
				; /* ignore IFS */
			if (c == 0)
				return;
		} else {
			/* mark any special second pass chars */
			if (!quote)
				switch (c) {
				  case '*':
				  case '?':
				  case '[':
					if (f&(DOPAT|DOGLOB)) {
						fdo |= (f&DOGLOB);
						*dp++ = MAGIC;
					}
					break;
				  case NOT:
					if ((f&(DOPAT|DOGLOB)) &&
					    dp[-1] == '[' && dp[-2] == MAGIC) {
						*dp++ = MAGIC;
					}
					break;
				  case '~':
					if ((f&DOTILDE) && dp == Xstring(ds, dp) ||
					    !(f&DOBLANK) && 
					    (dp[-1] == '=' || dp[-1] == ':')) {
						fdo |= DOTILDE;
						*dp++ = MAGIC;
					}
					break;
				}
			else
				quote &= ~2; /* undo temporary */

			word = 1;
#if 0
			*dp++ = c; /* save output char */
#else
			*dp++ = (char)c; /* save output char */
#endif /* Xn 1992-07-20 */
		}
	}
}

/*
 * Prepare to generate the string returned by ${} substitution.
 */
static int
#if __STDC__
varsub(register Expand *xp, register char *sp, int stype)
#else
varsub(xp, sp, stype)
	register Expand *xp;
	register char *sp;
	int stype;
#endif /* Xn 1992-07-17 */
{
	register int c;
	int type;

	/* ${#var}, string length or argc */
	if (sp[0] == '#' && (c = sp[1]) != 0) {
		c = (c == '*' || c == '@') ? e.loc->argc :
			strlen(strval(global(sp+1)));
		xp->str = strsave(ulton((unsigned long)c, 10), ATEMP);
		return XSUB;
	}

	c = sp[0];
	if (c == '*' || c == '@') {
		if (e.loc->argc == 0) {
			xp->str = null;
			type = XSUB;
		} else {
			xp->u.strv = e.loc->argv + 1;
			xp->str = *xp->u.strv++;
#if 0
			xp->split = c == '@'; /* $@ */
#else
			xp->split = (short)(c == '@'); /* $@ */
#endif /* Xn 1992-07-20 */
			type = XARG;
		}
	} else {
		xp->str = strval(global(sp));
		type = XSUB;
	}

	c = stype&0x7F;
	/* test the compiler's code generator */
	if (c == '%' || c == '#' ||
	    (((stype&0x80) ? *xp->str=='\0' : xp->str==null) ? /* undef? */
	     c == '=' || c == '-' || c == '?' : c == '+'))
		type = XBASE;	/* expand word instead of variable value */
	if (type != XBASE && flag[FNOUNSET] && xp->str == null && c != '+')
		errorf("%s: unset variable\n", sp);
	return type;
}

/*
 * Run the command in $(...) and read its output.
 */
static int
#if __STDC__
comsub(register Expand *xp, char *cp)
#else
comsub(xp, cp)
	register Expand *xp;
	char *cp;
#endif /* Xn 1992-07-17 */
{
	Source *s;
	register struct op *t;
	FILE *fi;

#if 0 && XN
	(void) fprintf(stderr, "comsub - cp: \"%s\"\n", cp);
	(void) fflush(stderr);
#endif
	s = pushs(SSTRING);
	s->str = cp;
	t = compile(s);

	if (t != NULL && t->type == TCOM && /* $(<file) */
	    *t->args == NULL && *t->vars == NULL && t->ioact != NULL) {
		register struct ioword *io = *t->ioact;

#if 0 && XN
		(void) fprintf(stderr, "comsub - if\n", cp);
		(void) fflush(stderr);
#endif
		if ((io->flag&IOTYPE) != IOREAD)
			errorf("funny $() command\n");
		fi = fopen(evalstr(io->name, DOTILDE), "r");
		if (fi != NULL)
#if WIN_NT
			_fileno(fi) = savefd(_fileno(fi));
#else
			fileno(fi) = savefd(fileno(fi));
#endif /* Xn 1993-04-02 */
		xp->split = 0;	/* no waitlast() */
	} else {
		int ofd1, pv[2];

#if 0 && XN
		(void) fprintf(stderr, "comsub - else\n", cp);
		(void) fflush(stderr);
#endif
		openpipe(pv);
		fi = fdopen(pv[0], "r");
#if 0 && XN
{
		int fd;

		(void) fprintf(stderr, "comsub - 1 (%d)\n", fcntl(1, F_GETFD));
		ofd1 = savefd(1);
#if 0 && WIN_NT
		(void) fcntl(ofd1, F_SETFD, fcntl(ofd1, F_GETFD) & ~FD_CLOEXEC);
#endif
		(void) fprintf(stderr, "comsub - ofd1: %d (%d)\n", ofd1, fcntl(ofd1, F_GETFD));
		fd = dup2(pv[1], 1);
		(void) fprintf(stderr, "comsub - dup2: %d (%d)\n", fd, fcntl(fd, F_GETFD));
		(void) fflush(stderr);
}
#else
		ofd1 = savefd(1);
		dup2(pv[1], 1);
#endif
		close(pv[1]);
#if 0
		exchild(t, XXCOM|XPIPEO);
#else
		execute(t, XFORK|XXCOM|XPIPEO);
#endif
		dup2(ofd1, 1);
		close(ofd1);
		xp->split = 1;	/* waitlast() */
	}	

	if (fi == NULL)
		errorf("cannot open $() input\n");
	setvbuf(fi, (char *)NULL, _IOFBF, BUFSIZ);
	xp->u.file = fi;
	return XCOM;
}

/*
 * perform #pattern and %pattern substitution in ${}
 */

static char *
#if __STDC__
trimsub(register char *str, char *pat, int how)
#else
trimsub(str, pat, how)
	register char *str;
	char *pat;
	int how;
#endif /* Xn 1992-07-17 */
{
	register char *end = strchr(str, 0);
	register char *p, c, *match;

	switch (how&0xff) {	/* UCHAR_MAX maybe? */
	case '#':		/* shortest at begin */
		for (p = str; p <= end; p++) {
			c = *p; *p = '\0';
			if (gmatch(str, pat)) {
				*p = c;
				return p;
			}
			*p = c;
		}
		break;
	case '#'|0x80:		/* longest match at begin */
		for (p = end; p >= str; p--) {
			c = *p; *p = '\0';
			if (gmatch(str, pat)) {
				*p = c;
				return p;
			}
			*p = c;
		}
		break;
	case '%':		/* shortest match at end */
		for (p = end; p >= str; p--) {
			if (gmatch(p, pat)) {
				c = *p; *p = '\0';
				match = strsave( str, APERM );	/* APERM? */
				*p = c;
				return match;
			}
		}
		break;
	case '%'|0x80:	/* longest match at end */
		for (p = str; p <= end; p++) {
			if (gmatch(p, pat)) {
				c = *p; *p = '\0';
				match = strsave( str, ATEMP );	/* APERM? */
				*p = c;
				return match;
			}
		}
		break;
	}

	return str;		/* no match, return string */
}

/*
 * glob
 * Name derived from V6's /etc/glob, the program that expanded filenames.
 */

#if 0
static	char   *debunk();
#else
static	char   *debunk ARGS((char *));
#endif /* Xn 1992-07-17 */

static void 
#if __STDC__
glob(char *cp, register XPtrV *wp)
#else
glob(cp, wp)
	char *cp;
	register XPtrV *wp;
#endif /* Xn 1992-07-17 */
{
	char path [PATH];
	register char *sp = cp;
	int oldsize;

	oldsize = XPsize(*wp);
	globit(path, path, sp, wp, 0);

	if (XPsize(*wp) == oldsize)
		{XPput(*wp, debunk(cp));}
	else
		qsortp(XPptrv(*wp) + oldsize, (size_t)(XPsize(*wp) - oldsize), xstrcmp);
}

static void
#if __STDC__
globit(char *ds, char *dp, char *sp, register XPtrV *wp, int check)
#else
globit(ds, dp, sp, wp, check)
	char *ds;		/* dest path */
	char *dp;		/* dest end */
	char *sp;		/* source path */
	register XPtrV *wp;	/* output list */
	int check;		/* check dest existence */
#endif /* Xn 1992-07-17 */
{
	register char *np;	/* next source component */
	register char *tsp, *tdp;

	if (sp == NULL) {	/* end of source path */
#if _POSIX_SOURCE
		if (check && eaccess(ds, F_OK) < 0)
#else
		if (check && eaccess(ds, 0) < 0)
#endif /* Xn 1992-07-31 */
			return;
		XPput(*wp, strsave(ds, ATEMP));
		return;
	}

	if (dp > ds)
		*dp++ = '/';
	while (*sp == '/')
		*dp++ = *sp++;
	np = strchr(sp, '/');
	if (np != NULL)
		*np++ = 0;

	*dp = 0;
	if (strchr(sp, MAGIC) == NULL) { /* contains no pattern? */
		tdp = dp; tsp = sp;
		while ((*tdp++ = *tsp++) != 0)
			;
		--tdp;
		globit(ds, tdp, np, wp, check);
	} else {
		DIR *dirp;
		struct dirent *d;

		/* ToDo:
		 * should not attemp to open() special files: /dev/ttyd0/*
		 * opendir should do this check, but Doug Gwyn's does not.
		 */
		dirp = opendir((*ds == 0) ? "." : ds);
		if (dirp == NULL)
			goto Nodir;
		while ((d = readdir(dirp)) != NULL) {
			tsp = d->d_name;
			if (tsp[0] == '.' &&
			    (tsp[1] == 0 || tsp[1] == '.' && tsp[2] == 0))
				continue; /* always ignore . and .. */
			if (*tsp == '.' && *sp != '.' || !gmatch(tsp, sp))
				continue;

			tdp = dp;
			while ((*tdp++ = *tsp++) != 0)
				;
			--tdp;
			globit(ds, tdp, np, wp, np != NULL);
		}
		closedir(dirp);
	  Nodir:;
	}

	if (np != NULL)
		*--np = '/';
}

/* remove MAGIC from string */
static char *
#if __STDC__
debunk(char *cp)
#else
debunk(cp)
	char *cp;
#endif /* Xn 1992-07-17 */
{
	register char *dp, *sp;

	for (dp = sp = cp; *sp != 0; sp++)
		if (*sp != MAGIC)
			*dp++ = *sp;
	*dp = 0;
	return cp;
}

/*
 * tilde expansion
 *
 * based on a version by Arnold Robbins
 */

static char *homedir ARGS((char *)); /* Xn 1992-08-19 */

static char *
#if __STDC__
tilde(char *acp)
#else
tilde(acp)
	char *acp;
#endif /* Xn 1992-07-17 */
{
	register int c;
	char path [PATH+1];
	register char *cp = acp, *wp = path, *dp;
	char userid [16+1];

  Again:
#if 0
	while (1) {
#else
	for (;;) {
#endif /* Xn 1992-07-20 */
		if ((c = *cp++) == 0) {
			*wp = 0;
			afree((Void*)acp, ATEMP);
			return strsave(path, ATEMP);
		} else if (c == MAGIC && *cp == '~')
			break;
		else
#if 0
			*wp++ = c;
#else
			*wp++ = (char)c;
#endif /* Xn 1992-07-20 */
	}

	dp = NULL;	/* no output substitution */
	if (cp[1] == 0 || cp[1] == '/' || cp[1] == ':') /* ~ or ~/ */
		dp = strval(global("HOME")), cp += 1;
	else if (cp[1] == '+' && (cp[2] == '/' || cp[2] == ':' || cp[2] == 0))
		dp = strval(global("PWD")), cp += 2;
	else if (cp[1] == '-' && (cp[2] == '/' || cp[2] == ':' || cp[2] == 0))
		dp = strval(global("OLDPWD")), cp += 2;
	else if (letter(cp[1])) {
		char *save = cp;
		for (dp = userid, cp++; letnum(*cp) && dp < userid+16; )
			*dp++ = *cp++;
		*dp = 0;
		dp = homedir(userid);
		if (dp == NULL)
			cp = save;
	}
	/* substitute */
	if (dp != NULL)
		while (*dp != 0)
			*wp++ = *dp++;
	goto Again;
}

/*
 * map userid to user's home directory.
 * todo: implement a cache with the "homedirs" table.
 * note that 4.3's getpw adds more than 6K to the shell,
 * and the YP version probably adds much more.
 * we might consider our own version of getpwnam() to keep the size down.
 */

static char *
#if __STDC__
homedir(char *name)
#else
homedir(name)
	char *name;
#endif /* Xn 1992-07-17 */
{
	register struct tbl *ap;
	register struct passwd *pw;
	extern struct passwd *getpwnam();

	ap = tsearch(&homedirs, name, hash(name));
#if 0 && XN
	(void) fprintf(stderr, "homedir(");
	if (name == NULL)
		(void) fprintf(stderr, "NULL");
	else
		(void) fprintf(stderr, "\"%s\"", name);
	(void) fprintf(stderr, ") - ap: %p", ap);
	if (ap != NULL)
	{
		(void) fprintf(stderr, "; ap->val.s: ");
		if (ap->val.s == NULL)
			(void) fprintf(stderr, "NULL");
		else
			(void) fprintf(stderr, "\"%s\"", ap->val.s);
	}
	(void) fprintf(stderr, "\n");
	(void) fflush(stderr);
#endif
	if ((ap != NULL && (ap->flag&ISSET)))
		return ap->val.s;
	pw = getpwnam(name);
#if 0 && XN
	(void) fprintf(stderr, "pw: %p", pw);
	if (pw != NULL)
	{
		(void) fprintf(stderr, "; pw->pw_dir: ");
		if (pw->pw_dir == NULL)
			(void) fprintf(stderr, "NULL");
		else
			(void) fprintf(stderr, "\"%s\"", pw->pw_dir);
	}
	(void) fprintf(stderr, "\n");
	(void) fflush(stderr);
#endif
	if (pw == NULL)
		return NULL;
	return pw->pw_dir;
}
