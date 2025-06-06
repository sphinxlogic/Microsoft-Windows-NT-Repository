/*
 * built-in Korn commands: c_*
 */

static char *RCSid = "$Id: c_ksh.c,v 3.4 89/03/27 15:47:16 egisin Exp $";

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#if 1
#include <stdlib.h>
#include <unistd.h>
#endif /* Xn 1992-07-17 */
#include "sh.h"
#include "table.h"

#if 1
extern void flushcom ARGS((int)); /* in exec.c */
extern void cleanpath ARGS((char *, char *, char *)); /* in misc.c */
extern int fptreef ARGS((register FILE *f, char *fmt, ...)); /* in tree.c */
extern void j_jobs ARGS((void)); /* in jobs.c */
extern int j_lookup ARGS((char *)); /* in jobs.c */
extern void x_bind ARGS((char *, char *, int)); /* in emacs.c */

#endif /* Xn 1992-07-17 */
int
#if __STDC__
c_hash(register char **wp)
#else
c_hash(wp)
	register char **wp;
#endif /* Xn 1992-07-16 */
{
#if 0
	register int i;
#endif /* Xn 1992-07-17 */
	register struct tbl *tp, **p;

	wp++;
	if (*wp == NULL) {
		for (p = tsort(&commands); (tp = *p++) != NULL; )
			if ((tp->flag&ISSET))
				printf("%s\n", tp->val.s);
		return 0;
	}

	if (strcmp(*wp, "-r") == 0)
		flushcom(1);
	while (*wp != NULL)
		findcom(*wp++, 1);
	return 0;
}

int
#if __STDC__
c_cd(register char **wp)
#else
c_cd(wp)
	register char **wp;
#endif /* Xn 1992-07-16 */
{
	char path [PATH];
	char newd [PATH];
	register char *cp;
	register char *dir;
	register char *cdpath;
	register char *rep;
	register char *pwd = NULL, *oldpwd = NULL;
	register int done = 0;
	register int prt = 0;
	register struct tbl *v_pwd = NULL, *v_oldpwd = NULL;
#if 0
	extern Void cleanpath();
#endif /* Xn 1992-07-17 */

	if ((dir = wp[1]) == NULL && (dir = strval(global("HOME"))) == NULL)
		errorf("no home directory");

	v_pwd = global("PWD");
	if ((pwd = strval(v_pwd)) == null) {
		setstr(v_pwd, getcwd(path, (size_t)PATH));
		pwd = strval(v_pwd);
	}

	if (wp[1] != NULL && (rep = wp[2]) != NULL) {
		/*
		 * Two arg version: cd pat rep
		 */
		if (strlen(pwd) - strlen(dir) + strlen(rep) >= PATH)
			errorf("substitution too long\n");
		cp = strstr(pwd, dir);
		if (cp == NULL)
			errorf("substitution failed\n");
		strncpy(path, pwd, cp - pwd);		/* first part */
		strcpy(path + (cp - pwd), rep);		/* replacement */
		strcat(path, cp + strlen(dir)); 	/* last part */
		dir = strsave(path, ATEMP);
		prt = 1;
	} else if (dir[0] == '-' && dir[1] == '\0') {
		/*
		 * Change to previous dir: cd -
		 */
		dir = strval(v_oldpwd = global("OLDPWD"));
		prt = 1;
	}
	if (dir[0] == '/' || (dir[0] == '.' && (dir[1] == '/' ||
	    (dir[1] == '.' && dir[2] == '/')))) {
		/*
		 * dir is an explicitly named path, so no CDPATH search
		 */
		cleanpath(pwd, dir, newd);
		if (chdir(newd) < 0)
			errorf("%s: bad directory\n", newd);
		else if (prt)
			shellf("%s\n", newd);
		flushcom(0);
	} else {
		/*
		 * search CDPATH for dir
		 */
		cdpath = strval(global("CDPATH"));
#if 0
		while ( !done && cdpath != NULL ) {
			cp = path;
			while (*cdpath && *cdpath != ':')
				*cp++ = *cdpath++;
			if (*cdpath == '\0')
				cdpath = NULL;
			else
				cdpath++;
			if (prt = (cp > path)) {
#else
		do {
			cp = path;
			if (cdpath != NULL) {
				while (*cdpath && *cdpath != ':')
					*cp++ = *cdpath++;
				if (*cdpath == '\0')
					cdpath = NULL;
				else
					cdpath++;
			}
			if ((prt = (cp > path)) != 0) {
#endif /* Xn 1992-08-03 */
				*cp++ = '/';
				(void) strcpy( cp, dir );
				cp = path;
			} else
				cp = dir;

			cleanpath(pwd, cp, newd);
			if (chdir(newd) == 0)
				done = 1;
		} while (!done && cdpath != NULL);
		if (!done)
			errorf("%s: bad directory\n", dir);
		if (prt)
			shellf("%s\n", newd);
		flushcom(0);
	}

	/*
	 * Keep track of OLDPWD and PWD
	 */
	oldpwd = pwd;
	pwd = newd;
	if (!v_oldpwd)
		v_oldpwd = global("OLDPWD");
	if (oldpwd && *oldpwd)
		setstr(v_oldpwd, oldpwd);
	else
		unset(v_oldpwd);
	if (*pwd)
		setstr(v_pwd, pwd);
	else
		unset(v_pwd);

	return 0;
}

int
#if __STDC__
c_print(register char **wp)
#else
c_print(wp)
	register char **wp;
#endif /* Xn 1992-07-16 */
{
	int nl = 1;
	int expand = 1;
	FILE *f = stdout;

	for (wp++; *wp != NULL && **wp == '-'; wp++) {
		register char *s = *wp + 1;
		if (*s == '\0') {
			wp++;
			break;
		}
		while (*s) switch (*s++) {
		  case 'n':
			nl = 0;
			break;
		  case 'e':
			expand = 1;
			break;
		  case 'r':
			expand = 0;
			break;
		  case 'u':
			if (!digit(*s) || (f = shf[*s++-'0']) == NULL)
				errorf("bad -u argument\n");
			break;
		}
	}

	while (*wp != NULL) {
		register char *s = *wp;
		register int c;
		while ((c = *s++) != '\0')
			if (expand && c == '\\') {
				switch ((c = *s++)) {
				case 'b': c = '\b'; break;
				case 'c': nl = 0; continue; /* AT&T brain damage */
				case 'f': c = '\f'; break;
				case 'n': c = '\n'; break;
				case 'r': c = '\r'; break;
				case 't': c = '\t'; break;
				case 'v': c = 0x0B; break;
				case '0': case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
					c = c - '0';
					if (*s >= '0' && *s <= '7')
						c = 8*c + *s++ - '0';
					if (*s >= '0' && *s <= '7')
						c = 8*c + *s++ - '0';
					break;
				case '\\': break;
				default:
					putc('\\', f);
				}
				putc(c, f);
			} else
				putc(c, f);
		if (*++wp != NULL)
			putc(' ', f);
	}
	if (nl)
		putc('\n', f);
	return 0;
}

/* todo: handle case where id is both lexical and command */
int
#if __STDC__
c_whence(register char **wp)
#else
c_whence(wp)
	register char **wp;
#endif /* Xn 1992-07-16 */
{
	register struct tbl *tp;
	char *id;
	int vflag = 0;
	int ret = 0;

	for (wp++; (id = *wp) != NULL && *id == '-'; wp++)
		if (id[1] == 'v')
			vflag = 1;

	while ((id = *wp++) != NULL) {
		tp = tsearch(&lexicals, id, hash(id));
		if (tp == NULL)
			tp = findcom(id, 0);
		if (vflag)
			switch ((tp == NULL) ? CNONE : tp->type) {
			  case CNONE:
				printf("%s is unknown\n", id);
				ret = 1;
				break;
			  case CSHELL:
				printf("%s is a shell builtin\n", id);
				break;
			  case CFUNC:
				printf("%s is a function\n", id);
				fptreef(stdout, "function %s %T\n", id, tp->val.t);
				break;
			  case CEXEC:
				printf("%s is %s\n", id,
				       (tp->flag&ISSET) ? tp->val.s : "unknown");
				if (!(tp->flag&ISSET))
					ret = 1;
				break;
			  case CALIAS:
				printf("%s is the alias '%s'\n", id, tp->val.s);
				break;
			  case CKEYWD:
				printf("%s is a shell keyword\n", id);
				break;
			  default:
				printf("%s is *GOK*\n", id);
				break;
			}
		else
			switch ((tp == NULL) ? CNONE : tp->type) {
			  case CNONE:
				printf("\n");
				ret = 1;
				break;
			  case CSHELL:
				printf("builtin %s\n", id);
				break;
			  case CFUNC:
				printf("%s\n", id);
				break;
			  case CEXEC:
				printf("%s\n", (tp->flag&ISSET) ? tp->val.s : "");
				if (!(tp->flag&ISSET))
					ret = 1;
				break;
			  case CALIAS:
				printf("%s\n", tp->val.s);
				break;
			  case CKEYWD:
				printf("%s\n", id);
				break;
			  default:
				printf("*GOK*\n");
				break;
			}
	}
	return ret;
}

/* typeset, export, and readonly */
int
#if __STDC__
c_typeset(register char **wp)
#else
c_typeset(wp)
	register char **wp;
#endif /* Xn 1992-07-16 */
{
	register char *id;
	struct block *l = e.loc;
	register struct tbl *vp, **p;
	int fset = 0, fclr = 0;
	int thing = 0, func = 0, local = 0;

	switch (**wp) {
	  case 'e':		/* export */
		fset |= EXPORT;
		break;
	  case 'r':		/* readonly */
		fset |= RDONLY;
		break;
	  case 't':		/* typeset */
		local = 1;
		break;
	}

	for (wp++; (id = *wp) != NULL && (*id == '-' || *id == '+'); wp++) {
		int flag = 0;
		thing = *id;
		while (*++id != '\0') switch (*id) {
		  case 'f':
			flag |= FUNCT;
			func = 1;
			break;
		  case 'i':
			flag |= INTEGER;
			break;
#if WIN_NT
		  case 'p':
			flag |= POSIXIZE;
			break;
#endif /* Xn 1992-08-17 */
		  case 'r':
			flag |= RDONLY;
			break;
		  case 'x':
			flag |= EXPORT;
			break;
		  case 't':
			flag |= TRACE;
			break;
		  default:
			errorf("unknown flag -%c\n", *id);
		}
		if (flag != 0) { /* + or - with options */
			if (thing == '-')
				fset |= flag;
			else
				fclr |= flag;
			thing = 0;
		}
	}

	/* list variables and attributes */
	if (*wp == NULL) {
		for (l = e.loc; l != NULL; l = l->next) {
		    for (p = tsort((func==0) ? &l->vars : &l->funs);
			 (vp = *p++) != NULL; )
			if ((vp->flag&ISSET))
			    if (thing == 0 && fclr == 0 && fset == 0) {
				printf("typeset ");
				if ((vp->flag&INTEGER))
					printf("-i ");
				if ((vp->flag&EXPORT))
					printf("-x ");
#if WIN_NT
				if ((vp->flag&POSIXIZE))
					printf("-p ");
#endif /* Xn 1992-08-17 */
				if ((vp->flag&RDONLY))
					printf("-r ");
				if ((vp->flag&TRACE)) 
					printf("-t ");
				printf("%s\n", vp->name);
			    } else
			    if (thing == '+' ||
				fclr && (vp->flag&fclr) == fclr) {
				printf("%s\n", vp->name);
			    } else
			    if (thing == '-' ||
				fset && (vp->flag&fset) == fset) {
				if (fset&FUNCT)
				    printf("function %s\n", vp->name);
				else
				    printf("%s=%s\n", vp->name, strval(vp));
#if 0 && XN
				    printf("vp->val.s: %p [\"%s\"]\n", vp->val.s, vp->val.s);
#endif
			    }
		}
		return (0);
	}

	if (local)
		fset |= LOCAL;
	for (; *wp != NULL; wp++)
#if 0
		if (func) {
		} else
#endif
		if (typeset(*wp, fset, fclr) == NULL)
			errorf("%s: not identifier\n", *wp);
	return 0;
}
	
int
#if __STDC__
c_alias(register char **wp)
#else
c_alias(wp)
	register char **wp;
#endif /* Xn 1992-07-16 */
{
	register struct table *t = &lexicals;
	register struct tbl *ap, **p;
#if 0
	register int i;
#endif /* Xn 1992-07-17 */
	int rv = 0;

	if (*++wp != NULL && strcmp(*wp, "-d") == 0) {
		t = &homedirs;
		wp++;
	}

	if (*wp == NULL)
		for (p = tsort(t); (ap = *p++) != NULL; )
			if (ap->type == CALIAS && (ap->flag&DEFINED))
				printf("%s='%s'\n", ap->name, ap->val.s);

	for (; *wp != NULL; wp++) {
		register char *id = *wp;
		register char *val = strchr(id, '=');

		if (val == NULL) {
			ap = tsearch(t, id, hash(id));
			if (ap != NULL && ap->type == CALIAS && (ap->flag&DEFINED))
				printf("%s='%s'\n", ap->name, ap->val.s);
			else
				rv = 1;
		} else {
			*val++ = '\0';
			ap = tenter(t, id, hash(id));
			if (ap->type == CKEYWD)
				errorf("cannot alias keyword\n");
			if ((ap->flag&ALLOC)) {
				afree((Void*)ap->val.s, APERM);
				ap->flag &= ~(ALLOC|ISSET);
			}
			ap->type = CALIAS;
			ap->val.s = strsave(val, APERM);
			ap->flag |= DEFINED|ALLOC|ISSET;
		}
	}
	return rv;
}

int
#if __STDC__
c_unalias(register char **wp)
#else
c_unalias(wp)
	register char **wp;
#endif /* Xn 1992-07-16 */
{
	register struct table *t = &lexicals;
	register struct tbl *ap;

	if (*++wp != NULL && strcmp(*wp, "-d") == 0) {
		t = &homedirs;
		wp++;
	}

	for (; *wp != NULL; wp++) {
		ap = tsearch(t, *wp, hash(*wp));
		if (ap == NULL || ap->type != CALIAS)
			continue;
		if ((ap->flag&ALLOC))
			afree((Void*)ap->val.s, APERM);
		ap->flag &= ~(DEFINED|ISSET|ALLOC);
	}
	return 0;
}

int
#if __STDC__
c_let(char **wp)
#else
c_let(wp)
	char **wp;
#endif /* Xn 1992-07-16 */
{
	int rv = 1;

#if 0 && XN
{
	char **p;

	(void) fprintf(stderr, "c_ksh.c/c_let - wp:");
	for (p = wp; *p != NULL; ++p)
	{
		(void) fprintf(stderr, " \"%s\"", *p);
	}
	(void) fputc('\n', stderr);
	(void) fflush(stderr);
}
#endif /* Xn 1993-06-15 */
	for (wp++; *wp; wp++)
#if 0 && XN
	{
		(void) fprintf(stderr, "c_ksh.c/c_let - *wp: \"%s\"\n", *wp);
		(void) fflush(stderr);
#endif /* Xn 1993-06-15 */
		rv = evaluate(*wp) == 0;
#if 0 && XN
	}
#endif /* Xn 1993-06-15 */
	return rv;
}

int
#if __STDC__
c_jobs(char **wp)
#else
c_jobs(wp)
	char **wp;
#endif /* Xn 1992-07-16 */
{
	j_jobs();
#if 0
	return 0;
#else
	return (wp != wp);
#endif /* Xn 1992-07-17 */
}

#ifdef JOBS
int
#if __STDC__
c_fgbg(register char **wp)
#else
c_fgbg(wp)
	register char **wp;
#endif /* Xn 1992-07-16 */
{
	int bg = strcmp(*wp, "bg") == 0;

	if (!flag[FMONITOR])
		errorf("Job control not enabled\n");
	wp++;
	j_resume(j_lookup((*wp == NULL) ? "%" : *wp), bg);
	return 0;
}
#endif

int
#if __STDC__
c_kill(register char **wp)
#else
c_kill(wp)
	register char **wp;
#endif /* Xn 1992-07-16 */
{
	register char *cp;
	int sig = 15;		/* SIGTERM */
	int rv = 0;
	int n;
	int gotsig = FALSE;

	if (*++wp == NULL)
		errorf("Usage: kill [-l] [-signal] {pid|job} ...\n");
	if (strcmp(*wp, "-l") == 0) {
		register struct trap *p = sigtraps;
		for (sig = 0; sig < SIGNALS; sig++, p++)
			if (p->signal)
				printf("%2d %8s %s\n", p->signal, p->name, p->mess);
		return 0;
	}

	for (; (cp = *wp) != NULL; wp++)
		if (*cp == '-' && gotsig == FALSE && *(wp+1) != NULL) {
			struct trap *p;
			gotsig = FALSE;
			if (digit(*(cp+1))) {
				if ((n = atoi(cp+1)) < SIGNALS) {
					sig = n;
					gotsig = TRUE;
				} else if (kill(n, sig) < 0) {
					shellf("%s: %s\n", cp, strerror(errno));
					rv++;
				}
			} else {
				p = gettrap(cp+1);
				if (p == NULL)
					errorf("bad signal %s\n", cp+1);
				sig = p->signal;
				gotsig = TRUE;
			}
		} else {
			gotsig = FALSE;
			if (digit(*cp) || (*cp == '-' && digit(*(cp+1)))) {
				if (kill(atoi(cp), sig) < 0) {
					shellf("%s: %s\n", cp, strerror(errno));
					rv++;
				}
			} else
			if (*cp == '%')
				j_kill(j_lookup(cp), sig);
			else
				errorf("invalid argument\n");
		}
	return rv;
}

int
#if __STDC__
c_bind(register char **wp)
#else
c_bind(wp)
	register char **wp;
#endif /* Xn 1992-07-16 */
{
	int macro = 0;
	register char *cp;

	for (wp++; (cp = *wp) != NULL && *cp == '-'; wp++)
		if (cp[1] == 'm')
			macro = 1;

	if (*wp == NULL)	/* list all */
		x_bind((char*)NULL, (char*)NULL, 0);

	for (; *wp != NULL; wp++) {
		cp = strchr(*wp, '=');
		if (cp != NULL)
			*cp++ = 0;
		x_bind(*wp, cp, macro);
	}

	return 0;
}

extern	int	c_fc ARGS((register char **)); /* in history.c - Xn 1992-07-29 */
extern	int	c_getopts ARGS((char **)); /* in getopts.c - Xn 1992-07-29 */

Const struct builtin kshbuiltins [] = {
	{"cd", c_cd},
	{"print", c_print},
	{"getopts", c_getopts},
	{"=typeset", c_typeset},
	{"=export", c_typeset},
	{"=readonly", c_typeset},
	{"whence", c_whence},
	{"=alias", c_alias},
	{"unalias", c_unalias},
	{"hash", c_hash},
	{"let", c_let},
	{"fc", c_fc},
	{"jobs", c_jobs},
	{"kill", c_kill},
#ifdef JOBS
	{"fg", c_fgbg},
	{"bg", c_fgbg},
#endif
#ifdef EMACS
	{"bind", c_bind},
#endif
	{NULL, NULL}
};

