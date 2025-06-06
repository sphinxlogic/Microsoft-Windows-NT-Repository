/*	EVAL.C:	Expression evaluation functions for
		MicroEMACS

	written 1986 by Daniel Lawrence
 *
 * $Header: /usr2/foxharp/src/pgf/vile/RCS/eval.c,v 1.121 1995/08/04 23:12:06 pgf Exp $
 *
 */

#include	"estruct.h"
#include	"edef.h"
#include	"nevars.h"

#define	FUNC_NAMELEN	4

#define	ILLEGAL_NUM	-1
#define	MODE_NUM	-2
#define	USER_NUM	-3

/* When the command interpretor needs to get a variable's name, rather than its
 * value, it is passed back as a VDESC variable description structure.  The
 * v_num field is an index into the appropriate variable table.
 */

	/* macros for environment-variable switch */
	/*  (if your compiler balks with "non-constant case expression" */
#if SYS_VMS || HAVE_LOSING_SWITCH_WITH_STRUCTURE_OFFSET
#define	If(N)		if (vnum == N) {
#define	ElseIf(N)	} else If(N)
#define	Otherwise	} else {
#define	EndIf		}
#else			/* more compact if it works... */
#define	If(N)		switch (vnum) { case N: {
#define	ElseIf(N)	break; } case N: {
#define	Otherwise	break; } default: {
#define	EndIf		}}
#endif

#if OPT_EVAL
typedef struct	{
	int	v_type;	/* type of variable */
	int	v_num;	/* index, if it's an environment-variable */
	UVAR *	v_ptr;	/* pointer, if it's a user-variable */
	} VDESC;

static	char	**list_of_modes;	/* list for name-completion */

#if ENVFUNC
static	char *	DftEnv P(( char *, char * ));
static	char *	GetEnv P(( char * ));
static	void	SetEnv P(( char **, char * ));
#endif
#if OPT_SHOW_EVAL
static	char *	get_listvalue P(( char *, int ));
static	int	is_mode_name P(( char *, int, VALARGS * ));
static	void	makevarslist P(( int, void *));
#endif
static	SIZE_T	s2size P(( char * ));
static	char *	getkill P(( void ));
static	char *	gtfun P(( char * ));
static	char *	gtusr P(( char * ));
static	char *	ltos P(( int ));
static	char *	s2offset P(( char *, char * ));
static	int	PromptAndSet P(( char *, int, int ));
static	int	SetVarValue P(( VDESC *, char * ));
static	int	ernd P(( void ));
static	int	gtlbl P(( char * ));
static	int	l_strtol P(( char * ));
static	int	sindex P(( char *, char * ));
static	int	vars_complete P(( int, char *, int * ));
static	void	FindVar P(( char *, VDESC * ));
static	int	OnlyMode P(( char * ));
static	char ** AllModes P(( void ));
#endif

/*--------------------------------------------------------------------------*/

#if	ENVFUNC && OPT_EVAL
static char *
GetEnv(s)
	char	*s;
{
	register char *v = getenv(s);
	return v ? v : "";
}

static char *
DftEnv(name, dft)
register char	*name;
register char	*dft;
{
	name = GetEnv(name);
	return (name == 0) ? dft : name;
}

static void
SetEnv(namep, value)
char	**namep;
char	*value;
{
	FreeIfNeeded(*namep);
	*namep = strmalloc(value);
}
#else
#define	GetEnv(s)	""
#define	DftEnv(s,d)	d
#define	SetEnv(np,s)	(*(np) = strmalloc(s))
#endif

#if OPT_EVAL
static char *shell;	/* $SHELL environment is "$shell" variable */
static char *directory;	/* $TMP environment is "$directory" variable */
#endif

#if OPT_SHOW_EVAL
/* list the current vars into the current buffer */
/* ARGSUSED */
static void
makevarslist(dum1,ptr)
int dum1;
void *ptr;
{
	register UVAR *p;
	register int j;

	bprintf("--- Environment variables %*P\n", term.t_ncol-1, '-');
	bprintf("%s", (char *)ptr);
	for (p = user_vars, j = 0; p != 0; p = p->next) {
		if (!j++)
			bprintf("--- User variables %*P", term.t_ncol-1, '-');
		bprintf("\n%%%s = %s", p->u_name, p->u_value);
	}
}

/* Test a name to ensure that it is a mode-name, filtering out modish-stuff
 * such as "all" and "noview"
 */
static int
is_mode_name(name, showall, args)
char *name;
int showall;
VALARGS	*args;
{

	if (OnlyMode(name)) {
		if (find_mode(name, -TRUE, args)) {
			if (!showall && !strcmp(name, args->names->shortname))
				return FALSE;
			return TRUE;
		}
		return SORTOFTRUE;
	}
	return FALSE;
}

/* filter out mode-names that apply only to abbreviations */
static char *
get_listvalue(name, showall)
char *name;
int showall;
{
	VALARGS args;
	register int	s;

	if ((s = is_mode_name(name, showall, &args)) == TRUE)
		return string_mode_val(&args);
	else if (s == SORTOFTRUE)
		return gtenv(name);
	return 0;
}
#endif /* OPT_SHOW_EVAL */

/* ARGSUSED */
#if OPT_SHOW_EVAL
int
listvars(f, n)
int f,n;
{
	char *values;
	register WINDOW *wp = curwp;
	register int s;
	register ALLOC_T t;
	register char *v, *vv;
	static	char *fmt = "$%s = %*S\n";
	char	**Names = f ? AllModes() : envars;
	int	showall = f ? (n > 1) : FALSE;

	/* collect data for environment-variables, since some depend on window */
	for (s = t = 0; Names[s] != 0; s++) {
		if ((vv = get_listvalue(Names[s], showall)) != 0)
			t += strlen(Names[s]) + strlen(fmt) + strlen(vv);
	}
	if ((values = malloc(t)) == 0)
		return FALSE;

	for (s = 0, v = values; Names[s] != 0; s++) {
		if ((vv = get_listvalue(Names[s], showall)) != 0) {
			t = strlen(vv);
			if (t == 0) {
				t = 1;
				vv = "";
			} else if (vv[t-1] == '\n')
				t--;	/* suppress trailing newline */
			v = lsprintf(v, fmt, Names[s], t, vv);
		}
	}
	s = liststuff(VARIABLES_BufName, FALSE,
				makevarslist, 0, (void *)values);
	free(values);

	/* back to the buffer whose modes we just listed */
	swbuffer(wp->w_bufp);
	return s;
}
#endif /* OPT_SHOW_EVAL */

#if OPT_EVAL
static
char *gtfun(fname)	/* evaluate a function */
char *fname;		/* name of function to evaluate */
{
	register int fnum;		/* index to function to eval */
	char *it = errorm;
	char arg1[NSTRING];		/* value of first argument */
	char arg2[NSTRING];		/* value of second argument */
	char arg3[NSTRING];		/* value of third argument */
	static char result[2 * NSTRING];	/* string result */

	*arg1 = *arg2 = *arg3 = EOS;
	if (!fname[0])
		return(it);

	/* look the function up in the function table */
	mklower(fname)[FUNC_NAMELEN-1] = EOS; /* case-independent */
	for (fnum = 0; fnum < NFUNCS; fnum++)
		if (strncmp(fname, funcs[fnum].f_name, 
				strlen(funcs[fnum].f_name)) == 0)
			break;

	/* return errorm on a bad reference */
	if (fnum == NFUNCS)
		return(errorm);

	/* if needed, retrieve the first argument */
	if (funcs[fnum].f_type >= MONAMIC) {
		if (macarg(arg1) != TRUE)
			return(errorm);

		/* if needed, retrieve the second argument */
		if (funcs[fnum].f_type >= DYNAMIC) {
			if (macarg(arg2) != TRUE)
				return(errorm);

			/* if needed, retrieve the third argument */
			if (funcs[fnum].f_type >= TRINAMIC)
				if (macarg(arg3) != TRUE)
					return(errorm);
		}
	}

	/* and now evaluate it! */
	switch (fnum) {
	case UFADD:
		it = l_itoa(l_strtol(arg1) + l_strtol(arg2));
		break;
	case UFSUB:
		it = l_itoa(l_strtol(arg1) - l_strtol(arg2));
		break;
	case UFTIMES:
		it = l_itoa(l_strtol(arg1) * l_strtol(arg2));
		break;
	case UFDIV:
		it = l_itoa(l_strtol(arg1) / l_strtol(arg2));
		break;
	case UFMOD:
		it = l_itoa(l_strtol(arg1) % l_strtol(arg2));
		break;
	case UFNEG:
		it = l_itoa(-l_strtol(arg1));
		break;
	case UFCAT:
		it = strcat(strcpy(result, arg1), arg2);
		break;
	case UFLEFT:
		it = strncpy0(result, arg1, s2size(arg2)+1);
		break;
	case UFRIGHT:
		it = strcpy(result, s2offset(arg1,arg2));
		break;
	case UFMID:
		it = strncpy0(result, s2offset(arg1,arg2), s2size(arg3)+1);
		break;
	case UFNOT:
		it = ltos(stol(arg1) == FALSE);
		break;
	case UFEQUAL:
		it = ltos(l_strtol(arg1) == l_strtol(arg2));
		break;
	case UFLESS:
		it = ltos(l_strtol(arg1) < l_strtol(arg2));
		break;
	case UFGREATER:
		it = ltos(l_strtol(arg1) > l_strtol(arg2));
		break;
	case UFSEQUAL:
		it = ltos(strcmp(arg1, arg2) == 0);
		break;
	case UFSLESS:
		it = ltos(strcmp(arg1, arg2) < 0);
		break;
	case UFSGREAT:
		it = ltos(strcmp(arg1, arg2) > 0);
		break;
	case UFIND:
		it = tokval(arg1);
		break;
	case UFAND:
		it = ltos(stol(arg1) && stol(arg2));
		break;
	case UFOR:
		it = ltos(stol(arg1) || stol(arg2));
		break;
	case UFLENGTH:
		it = l_itoa((int)strlen(arg1));
		break;
	case UFUPPER:
		it = mkupper(arg1);
		break;
	case UFLOWER:
		it = mklower(arg1);
		break;
	case UFTRIM:
		it = mktrimmed(arg1);
		break;
	case UFTRUTH:
		/* is it the answer to everything? */
		it = ltos(l_strtol(arg1) == 42);
		break;
	case UFASCII:
		it = l_itoa((int)arg1[0]);
		break;
	case UFCHR:
		result[0] = (char)l_strtol(arg1);
		result[1] = EOS;
		it = result;
		break;
	case UFGTKEY:
		result[0] = (char)keystroke_raw8();
		result[1] = EOS;
		it = result;
		break;
	case UFRND:
		it = l_itoa((ernd() % absol(l_strtol(arg1))) + 1);
		break;
	case UFABS:
		it = l_itoa(absol(l_strtol(arg1)));
		break;
	case UFSINDEX:
		it = l_itoa(sindex(arg1, arg2));
		break;
	case UFENV:
		it = GetEnv(arg1);
		break;
	case UFBIND:
		it = prc2engl(arg1);
		break;
	case UFREADABLE:
		it = ltos(doglob(arg1)
			&& flook(arg1, FL_HERE|FL_READABLE) != NULL);
		break;
	case UFWRITABLE:
		it = ltos(doglob(arg1)
			&& flook(arg1, FL_HERE|FL_WRITEABLE) != NULL);
		break;
	default:
		it = errorm;
		break;
	}
	return it;
}

static char *
gtusr(vname)		/* look up a user var's value */
char *vname;		/* name of user variable to fetch */
{
	register UVAR	*p;

	if (vname[0] != EOS) {
		for (p = user_vars; p != 0; p = p->next)
			if (!strcmp(vname, p->u_name))
				return p->u_value;
	}
	return (errorm);
}

char *gtenv(vname)
char *vname;		/* name of environment variable to retrieve */
{
	register int vnum;	/* ordinal number of var referenced */
	register char *value = errorm;

	if (vname[0] != EOS) {

		/* scan the list, looking for the referenced name */
		for (vnum = 0; envars[vnum] != 0; vnum++)
			if (strcmp(vname, envars[vnum]) == 0)
				break;

		/* return errorm on a bad reference */
		if (envars[vnum] == 0) {
#if !SMALLER
			VALARGS	args;

			if (is_mode_name(vname, TRUE, &args) == TRUE)
				value = string_mode_val(&args);
#endif
			return (value);
		}


		/* otherwise, fetch the appropriate value */

		/* NOTE -- if you get a compiler error from this code, find
			the definitions of If and ElseIf up above, and add your
			system to the set of those with broken compilers that need
			to use ifs instead of a switch statement */

		If( EVPAGELEN )		value = l_itoa(term.t_nrow);
		ElseIf( EVCURCOL )	value = l_itoa(getccol(FALSE) + 1);
		ElseIf( EVCURLINE )	value = l_itoa(getcline());
#if OPT_RAMSIZE
		ElseIf( EVRAM )		value = l_itoa((int)(envram / 1024l));
#endif
		ElseIf( EVFLICKER )	value = ltos(flickcode);
		ElseIf( EVCURWIDTH )	value = l_itoa(term.t_ncol);
		ElseIf( EVCBUFNAME )	value = get_bname(curbp);
		ElseIf( EVABUFNAME )	value = get_bname(find_alt());
		ElseIf( EVCFNAME )	value = curbp ? curbp->b_fname : "";
		ElseIf( EVSRES )	value = sres;
		ElseIf( EVDEBUG )	value = ltos(macbug);
		ElseIf( EVSTATUS )	value = ltos(cmdstatus);
		ElseIf( EVPALETTE )	value = palstr;
		ElseIf( EVLASTKEY )	value = l_itoa(lastkey);
		ElseIf( EVCURCHAR )
			if (curbp && !is_empty_buf(curbp)) {
				value = is_at_end_of_line(DOT)
					? l_itoa('\n')
					: l_itoa(char_at(DOT));
			}

		ElseIf( EVDISCMD )	value = ltos(discmd);
		ElseIf( EVVERSION )	value = version;
		ElseIf( EVPROGNAME )	value = prognam;
		ElseIf( EVOS )		value = opersys;
		ElseIf( EVSEED )	value = l_itoa(seed);
		ElseIf( EVDISINP )	value = ltos(disinp);
		ElseIf( EVWLINE )	value = l_itoa(curwp->w_ntrows);
		ElseIf( EVCWLINE )	value = l_itoa(getwpos());
		ElseIf( EVSEARCH )	value = pat;
		ElseIf( EVREPLACE )	value = rpat;
		ElseIf( EVMATCH )	value = (patmatch == NULL) ? 
							"" : patmatch;
		ElseIf( EVMODE )	value = current_modename();
#if OPT_MLFORMAT
		ElseIf( EVMLFORMAT )
			if (modeline_format == 0)
				mlwrite("BUG: modeline_format uninitialized");
			value = modeline_format;
#endif

		ElseIf( EVMODIFIED )	value = ltos(curbp && b_is_changed(curbp));
		ElseIf( EVKILL )	value = getkill();
		ElseIf( EVTPAUSE )	value = l_itoa(term.t_pause);
		ElseIf( EVPENDING )	value = ltos(keystroke_avail());
		ElseIf( EVLLENGTH )	value = l_itoa(lLength(DOT.l));
		ElseIf( EVLINE )	value = getctext((CHARTYPE)0);
		ElseIf( EVWORD )	value = getctext(_nonspace);
		ElseIf( EVIDENTIF )	value = getctext(_ident);
		ElseIf( EVQIDENTIF )	value = getctext(_qident);
		ElseIf( EVPATHNAME )	value = getctext(_pathn);
#if SYS_UNIX
		ElseIf( EVPROCESSID )	value = l_itoa(getpid());
#endif
		ElseIf( EVCWD )		value = current_directory(FALSE);
		ElseIf( EVOCWD )	value = previous_directory();
#if OPT_PROCEDURES
		ElseIf( EVCDHOOK )	value = cdhook;
		ElseIf( EVRDHOOK )	value = readhook;
		ElseIf( EVWRHOOK )	value = writehook;
		ElseIf( EVBUFHOOK )	value = bufhook;
		ElseIf( EVEXITHOOK )	value = exithook;
#endif
#if DISP_X11
		ElseIf( EVFONT )	value = x_current_fontname();
		ElseIf( EVTITLE )	value = x_get_window_name();
		ElseIf( EVICONNAM )	value = x_get_icon_name();
#endif
		ElseIf( EVSHELL )
			if (shell == 0)
				SetEnv(&shell, DftEnv("SHELL", "/bin/sh"));
			value = shell;

		ElseIf( EVDIRECTORY )
			if (directory == 0)
				SetEnv(&directory, DftEnv("TMP", P_tmpdir));
			value = directory;

		ElseIf( EVNTILDES )	value = l_itoa(ntildes);


		EndIf
	}
	return value;
}

static char *
getkill()		/* return some of the contents of the kill buffer */
{
	register SIZE_T size;	/* max number of chars to return */
	static char value[NSTRING];	/* temp buffer for value */

	if (kbs[0].kbufh == NULL)
		/* no kill buffer....just a null string */
		value[0] = EOS;
	else {
		/* copy in the contents... */
		if (kbs[0].kused < NSTRING)
			size = kbs[0].kused;
		else
			size = NSTRING - 1;
		(void)strncpy(value, (char *)(kbs[0].kbufh->d_chunk), size);
		value[size] = EOS;
	}

	/* and return the constructed value */
	return(value);
}

static void
FindVar(var, vd)	/* find a variables type and name */
char *var;	/* name of var to get */
VDESC *vd;	/* structure to hold type and ptr */
{
	register UVAR *p;
	register int vnum;	/* subscript in variable arrays */
	register int vtype;	/* type to return */

fvar:
	vtype = vnum = ILLEGAL_NUM;
	vd->v_ptr = 0;

	if (!var[1]) {
		vd->v_type = vtype;
		return;
	}
	switch (var[0]) {

		case '$': /* check for legal enviromnent var */
			for (vnum = 0; envars[vnum] != 0; vnum++)
				if (strcmp(&var[1], envars[vnum]) == 0) {
					vtype = TKENV;
					break;
				}
#if !SMALLER
			if (vtype == ILLEGAL_NUM) {
				VALARGS	args;

				if (is_mode_name(&var[1], TRUE, &args) == TRUE) {
					vnum  = MODE_NUM;
					vtype = TKENV;
				}
			}
#endif
			break;

		case '%': /* check for existing legal user variable */
			for (p = user_vars; p != 0; p = p->next)
				if (!strcmp(var+1, p->u_name)) {
					vtype = TKVAR;
					vnum  = USER_NUM;
					vd->v_ptr = p;
					break;
				}
			if (vd->v_ptr == 0) {
				if ((p = typealloc(UVAR)) != 0
				 && (p->u_name = strmalloc(var+1)) != 0) {
					p->next    = user_vars;
					p->u_value = 0;
					user_vars  = vd->v_ptr = p;
					vtype = TKVAR;
					vnum  = USER_NUM;
				}
			}
			break;

		case '&':	/* indirect operator? */
			var[FUNC_NAMELEN] = EOS;
			if (strcmp(&var[1], "ind") == 0) {
				/* grab token, and eval it */
				execstr = token(execstr, var, EOS);
				(void)strcpy(var, tokval(var));
				goto fvar;
			}
	}

	/* return the results */
	vd->v_num = vnum;
	vd->v_type = vtype;
}

/*
 * Test for mode-names that we'll not show in the variable name-completion.
 */
static int
OnlyMode (name)
char *name;
{
	return (strncmp(name, "no", 2)
	   &&   strcmp(name, "all"));
}

/*
 * Return a list of only the modes that can be set with ":setv", ignoring
 * artifacts such as "all".
 */
static char **
AllModes ()
{
	if (list_of_modes == 0) {
		char **s, **d;
		size_t n;
		for (n = 0; all_modes[n] != 0; n++)
			;
		list_of_modes = typeallocn(char *, n + 1);
		for (s = all_modes, d = list_of_modes; (*d = *s) != 0; s++) {
			if (OnlyMode(*d)) {
				d++;
			}
		}
	}
	return list_of_modes;
}

/*
 * Handles name-completion for variables.  Note that since the first character
 * can be either a '$' or '%', we'll postpone name-completion until we've seen
 * a '$'.
 */
static int
vars_complete(c, buf, pos)
int	c;
char	*buf;
int	*pos;
{
	int	status;
	if (buf[0] == '$') {
		*pos -= 1;	/* account for leading '$', not in tables */
		status = kbd_complete(FALSE, c, buf+1, pos,
				(char *)AllModes(), sizeof(list_of_modes[0]));
		*pos += 1;
	} else {
		if (c != NAMEC) /* cancel the unget */
			unkeystroke(c);
		status = TRUE;
	}
	return status;
}

int
setvar(f, n)		/* set a variable */
int f;		/* default flag */
int n;		/* numeric arg (can overide prompted value) */
{
	register int status;	/* status return */
	static char var[NLINE+3];	/* name of variable to fetch */

	/* first get the variable to set.. */
	status = kbd_reply("Variable to set: ",
		var, NLINE,
		mode_eol, '=', KBD_NOEVAL|KBD_LOWERC, vars_complete);
	if (status != TRUE)
		return(status);
	return PromptAndSet(var, f, n);
}

static int
PromptAndSet(var, f, n)
char	*var;
int	f,n;
{
	register int status;	/* status return */
	VDESC vd;		/* variable num/type */
	char prompt[NLINE];
	char value[NLINE];	/* value to set variable to */

	/* check the legality and find the var */
	FindVar(var, &vd);

	/* if its not legal....bitch */
	if (vd.v_type == ILLEGAL_NUM) {
		mlforce("[No such variable as '%s']", var);
		return(FALSE);
	} else if (vd.v_type == TKENV && vd.v_num == MODE_NUM) {
		VALARGS	args;
		(void)find_mode(var+1, -TRUE, &args);
		set_end_string('=');
		return adjvalueset(var+1, TRUE, -TRUE, &args);
	}

	/* get the value for that variable */
	if (f == TRUE)
		(void)strcpy(value, l_itoa(n));
	else {
		value[0] = EOS;
		(void)lsprintf(prompt, "Value of %s: ", var);
		status = mlreply(prompt, value, sizeof(value));
		if (status != TRUE)
			return(status);
	}

	/* and set the appropriate value */
	status = SetVarValue(&vd, value);

	if (status == ABORT)
		mlforce("[Variable %s is readonly]", var);
	else if (status != TRUE)
		mlforce("[Cannot set %s to %s]", var, value);
	/* and return it */
	return(status);
}

/* entrypoint from modes.c, used to set environment variables */
#if OPT_EVAL
int
set_variable(name)
char	*name;
{
	char	temp[NLINE];
	if (*name != '$')
		name = strcat(strcpy(temp, "$"), name);
	return PromptAndSet(name, FALSE, 0);
}
#endif

static int
SetVarValue(var, value)	/* set a variable */
VDESC *var;	/* variable to set */
char *value;	/* value to set to */
{
	register UVAR *vptr;
	register int vnum;	/* ordinal number of var referenced */
	register int vtype;	/* type of variable to set */
	register int status;	/* status return */
	register int c;		/* translated character */

	/* simplify the vd structure (we are gonna look at it a lot) */
	vptr = var->v_ptr;
	vnum = var->v_num;
	vtype = var->v_type;

	/* and set the appropriate value */
	status = TRUE;
	switch (vtype) {
	case TKVAR: /* set a user variable */
		FreeIfNeeded(vptr->u_value);
		if ((vptr->u_value = strmalloc(value)) == 0)
			status = FALSE;
		break;

	case TKENV: /* set an environment variable */
		status = TRUE;	/* by default */
		If( EVCURCOL )
			status = gotocol(TRUE, atoi(value));

		ElseIf( EVCURLINE )
			status = gotoline(TRUE, atoi(value));

		ElseIf( EVFLICKER )
			flickcode = stol(value);

		ElseIf( EVCURWIDTH )
#if DISP_X11
			x_resize(atoi(value), term.t_nrow);
#else
			status = newwidth(TRUE, atoi(value));
#endif

		ElseIf( EVPAGELEN )
#if DISP_X11
			x_resize(term.t_ncol, atoi(value));
#else
			status = newlength(TRUE, atoi(value));
#endif

		ElseIf( EVCBUFNAME )
			if (curbp) {
				set_bname(curbp, value);
				curwp->w_flag |= WFMODE;
			}

		ElseIf( EVCFNAME )
			if (curbp) {
				ch_fname(curbp, value);
				curwp->w_flag |= WFMODE;
			}

		ElseIf( EVSRES )
			status = TTrez(value);

		ElseIf( EVDEBUG )
			macbug = stol(value);

		ElseIf( EVSTATUS )
			cmdstatus = stol(value);

		ElseIf( EVPALETTE )
			spal(strncpy0(palstr, value, (SIZE_T)(NSTRING)));

		ElseIf( EVLASTKEY )
			lastkey = l_strtol(value);

		ElseIf( EVCURCHAR )
			if (curbp == NULL || b_val(curbp,MDVIEW))
				status = rdonly();
			else {
				mayneedundo();
				(void)ldelete(1L, FALSE); /* delete 1 char */
				c = l_strtol(value);
				if (c == '\n')
					status = lnewline();
				else
					status = linsert(1, c);
				(void)backchar(FALSE, 1);
			}

		ElseIf( EVDISCMD )
			discmd = stol(value);

		ElseIf( EVSEED )
			seed = atoi(value);

		ElseIf( EVDISINP )
			disinp = stol(value);

		ElseIf( EVWLINE )
			status = resize(TRUE, atoi(value));

		ElseIf( EVCWLINE )
			status = forwline(TRUE, atoi(value) - getwpos());

#if OPT_MLFORMAT
		ElseIf( EVMLFORMAT )
			SetEnv(&modeline_format, value);
#endif

		ElseIf( EVSEARCH )
			(void)strcpy(pat, value);
			FreeIfNeeded(gregexp);
			gregexp = regcomp(pat, b_val(curbp, MDMAGIC));

		ElseIf( EVREPLACE )
			(void)strcpy(rpat, value);

		ElseIf( EVTPAUSE )
			term.t_pause = atoi(value);

		ElseIf( EVLINE )
			if (b_val(curbp,MDVIEW))
				status = rdonly();
			else {
				mayneedundo();
				status = putctext(value);
			}

		ElseIf( EVCWD )
			status = set_directory(value);
#if DISP_X11
		ElseIf( EVFONT ) status = x_setfont(value);
		ElseIf( EVTITLE ) x_set_window_name(value); status = TRUE;
		ElseIf( EVICONNAM ) x_set_icon_name(value); status = TRUE;
#endif
		ElseIf( EVSHELL )
			SetEnv(&shell, value);

		ElseIf( EVDIRECTORY )
			SetEnv(&directory, value);

#if OPT_PROCEDURES
		ElseIf( EVCDHOOK )     (void)strcpy(cdhook, value);
		ElseIf( EVRDHOOK )     (void)strcpy(readhook, value);
		ElseIf( EVWRHOOK )     (void)strcpy(writehook, value);
		ElseIf( EVBUFHOOK )    (void)strcpy(bufhook, value);
		ElseIf( EVEXITHOOK )   (void)strcpy(exithook, value);
#endif

		ElseIf( EVNTILDES )
			ntildes = atoi(value);
			if (ntildes > 100)
				ntildes = 100;

		Otherwise
			/* EVABUFNAME */
			/* EVPROGNAME */
			/* EVVERSION */
			/* EVMATCH */
			/* EVKILL */
			/* EVPENDING */
			/* EVLLENGTH */
			/* EVMODIFIED */
			/* EVPROCESSID */
			/* EVRAM */
			/* EVMODE */
			/* EVOS */
			status = ABORT;	/* must be readonly */
		EndIf
		break;
	}
#if	OPT_DEBUGMACROS
	/* if $debug == TRUE, every assignment will echo a statment to
	   that effect here. */

	if (macbug) {
		char	outline[NLINE];
		(void)strcpy(outline, "(((");

		/* assignment status */
		(void)strcat(outline, ltos(status));
		(void)strcat(outline, ":");

		/* variable name */
		if (var->v_type == TKENV) {
			(void)strcat(outline, "&");
			(void)strcat(outline, envars[var->v_num]);
		} else if (var->v_type == TKENV) {
			(void)strcat(outline, "%");
			(void)strcat(outline, var->v_ptr->u_name);
		}
		(void)strcat(outline, ":");

		/* and lastly the value we tried to assign */
		(void)strcat(outline, value);
		(void)strcat(outline, ")))");


		/* write out the debug line */
		mlforce("%s",outline);
		(void)update(TRUE);

		/* and get the keystroke to hold the output */
		if (ABORTED(keystroke())) {
			mlforce("[Macro aborted]");
			status = FALSE;
		}
	}
#endif
	return(status);
}
#endif

/*	l_itoa:	integer to ascii string.......... This is too
		inconsistent to use the system's	*/

char *l_itoa(i)
int i;	/* integer to translate to a string */
{
	static char result[INTWIDTH+1];	/* resulting string */
	(void)lsprintf(result,"%d",i);
	return result;
}


/* like strtol, but also allow character constants */
#if OPT_EVAL
static int
l_strtol(s)
char *s;
{
	if (s[0] == '\'' &&
	    s[2] == '\'' &&
	    s[3] == EOS)
	    return s[1];
	return (int)strtol(s, (char **)0, 0);
}
#endif

int toktyp(tokn)	/* find the type of a passed token */
char *tokn;	/* token to analyze */
{

	/* no blanks!!! */
	if (tokn[0] == EOS)
		return(TKNUL);

	/* a numeric literal? */
	if (isdigit(tokn[0]))
		return(TKLIT);

	if (tokn[0] == '"')
		return(TKSTR);

	/* if it's any other single char, it must be itself */
	if (tokn[1] == EOS)
		return(TKCMD);

#if ! SMALLER
	switch (tokn[0]) {
		case '"':	return(TKSTR);

		case '~':	return(TKDIR);
		case '@':	return(TKARG);
		case '<':	return(TKBUF);
		case '$':	return(TKENV);
		case '%':	return(TKVAR);
		case '&':	return(TKFUN);
		case '*':	return(TKLBL);

		default:	return(TKCMD);
	}
#else
	return(TKCMD);
#endif
}

char *
tokval(tokn)	/* find the value of a token */
char *tokn;		/* token to evaluate */
{
#if OPT_EVAL
	register int status;	/* error return */
	register BUFFER *bp;	/* temp buffer pointer */
	register SIZE_T blen;	/* length of buffer argument */
	register int distmp;	/* temporary discmd flag */
	int	oclexec;
	static char buf[NSTRING];/* string buffer for some returns */

	switch (toktyp(tokn)) {
		case TKNUL:	return("");

		case TKARG:	/* interactive argument */
				{
				static char tkargbuf[NSTRING];
			
				oclexec = clexec;

				(void)strcpy(tokn, tokval(&tokn[1]));
				distmp = discmd;	/* echo it always! */
				discmd = TRUE;
				clexec = FALSE;
				status = kbd_string(tokn, tkargbuf,
						sizeof(buf), '\n',
						KBD_EXPAND|KBD_QUOTES,
						no_completion);
				discmd = distmp;
				clexec = oclexec;

				if (status == ABORT)
					return(errorm);
				return(tkargbuf);
				}

		case TKBUF:	/* buffer contents fetch */

				/* grab the right buffer */
				(void)strcpy(tokn, tokval(&tokn[1]));
				if ((bp = find_b_name(tokn)) == NULL)
					return(errorm);

				/* if the buffer is displayed, get the window
				   vars instead of the buffer vars */
				if (bp->b_nwnd > 0) {
					curbp->b_dot = DOT;
				}

				/* make sure we are not at the end */
				if (is_header_line(bp->b_dot,bp))
					return(errorm);

				/* grab the line as an argument */
				blen = lLength(bp->b_dot.l);
				if (blen > bp->b_dot.o)
					blen -= bp->b_dot.o;
				else
					blen = 0;
				if (blen > NSTRING)
					blen = NSTRING;
				(void)strncpy(buf,
				    l_ref(bp->b_dot.l)->l_text + bp->b_dot.o,
					blen);
				buf[blen] = EOS;

				/* and step the buffer's line ptr
					ahead a line */
				bp->b_dot.l = lFORW(bp->b_dot.l);
				bp->b_dot.o = 0;

				/* if displayed buffer, reset window ptr vars*/
				if (bp->b_nwnd > 0) {
					DOT.l = curbp->b_dot.l;
					DOT.o = 0;
					curwp->w_flag |= WFMOVE;
				}

				/* and return the spoils */
				return(buf);

		case TKVAR:	return(gtusr(tokn+1));
		case TKENV:	return(gtenv(tokn+1));
		case TKFUN:	return(gtfun(tokn+1));
		case TKDIR:
#if SYS_UNIX
				return(lengthen_path(strcpy(buf,tokn)));
#else
				return(errorm);
#endif
		case TKLBL:	return(l_itoa(gtlbl(tokn)));
		case TKLIT:	return(tokn);
		case TKSTR:	return(tokn+1);
		case TKCMD:	return(tokn);
	}
	return errorm;
#else
	return (toktyp(tokn) == TKSTR) ? tokn+1 : tokn;
#endif
}

/*
 * Return true if the argument is one of the strings that we accept as a
 * synonym for "true".
 */
int
is_truem(val)
char *val;
{
	char	temp[8];
	(void)mklower(strncpy0(temp, val, sizeof(temp)));
	return (!strcmp(temp, "yes")
	   ||   !strcmp(temp, "true")
	   ||   !strcmp(temp, "t")
	   ||   !strcmp(temp, "y")
	   ||   !strcmp(temp, "on"));
}

/*
 * Return true if the argument is one of the strings that we accept as a
 * synonym for "false".
 */
int
is_falsem(val)
char *val;
{
	char	temp[8];
	(void)mklower(strncpy0(temp, val, sizeof(temp)));
	return (!strcmp(temp, "no")
	   ||   !strcmp(temp, "false")
	   ||   !strcmp(temp, "f")
	   ||   !strcmp(temp, "n")
	   ||   !strcmp(temp, "off"));
}

#if OPT_EVAL || DISP_X11
int stol(val)	/* convert a string to a numeric logical */
char *val;	/* value to check for stol */
{
	/* check for logical values */
	if (is_falsem(val))
		return(FALSE);
	if (is_truem(val))
		return(TRUE);

	/* check for numeric truth (!= 0) */
	return((atoi(val) != 0));
}
#endif

#if OPT_EVAL
/* use this as a wrapper when evaluating an array index, etc., that cannot
 * be negative.
 */
static SIZE_T
s2size(s)
char *s;
{
	int	n = atoi(s);
	if (n < 0)
		n = 0;
	return n;
}

/* use this to return a pointer to the string after the n-1 first characters */
static char *
s2offset(s, n)
char	*s;
char	*n;
{
	SIZE_T	len = strlen(s) + 1;
	UINT	off = s2size(n);
	if (off > len)
		off = len;
	if (off == 0)
		off = 1;
	return s + (off - 1);
}

/* ARGSUSED */
static int
gtlbl(tokn)	/* find the line number of the given label */
char *tokn;	/* label name to find */
{
	return(1);
}

static char *
ltos(val)	/* numeric logical to string logical */
int val;	/* value to translate */
{
	if (val)
		return(truem);
	else
		return(falsem);
}
#endif

#if OPT_EVAL || !SMALLER
char *mkupper(str)	/* make a string upper case */
char *str;		/* string to upper case */
{
	char *sp;

	sp = str;
	while (*sp) {
		if (islower(*sp))
			*sp += 'A' - 'a';
		++sp;
	}
	return(str);
}
#endif

char *mklower(str)	/* make a string lower case */
char *str;		/* string to lower case */
{
	char *sp;

	sp = str;
	while (*sp) {
		if (isupper(*sp))
			*sp += 'a' - 'A';
		++sp;
	}
	return(str);
}

char *mktrimmed(str)	/* trim whitespace */
register char *str;
{
	char *base = str;
	register char *dst = str;

	while (*str != EOS) {
		if (isspace(*str)) {
			if (dst != base)
				*dst++ = ' ';
			while (isspace(*str))
				str++;
		} else {
			*dst++ = *str++;
		}
	}
	if (dst != base
	 && isspace(dst[-1]))
	 	dst--;
	*dst = EOS;
	return base;
}

#if OPT_EVAL
int absol(x)	/* take the absolute value of an integer */
int x;
{
	return(x < 0 ? -x : x);
}
#endif

#if OPT_EVAL
static int
ernd()		/* returns a random integer */
{
	seed = absol(seed * 1721 + 10007);
	return(seed);
}

static int
sindex(sourc, pattern)	/* find pattern within source */
char *sourc;	/* source string to search */
char *pattern;	/* string to look for */
{
	int it = 0;	/* assume no match at all.. */
	char *sp;	/* ptr to current position to scan */
	char *csp;	/* ptr to source string during comparison */
	char *cp;	/* ptr to place to check for equality */

	/* scanning through the source string */
	sp = sourc;
	while (*sp) {
		/* scan through the pattern */
		cp = pattern;
		csp = sp;
		while (*cp) {
			if (!eq(*cp, *csp))
				break;
			++cp;
			++csp;
		}

		/* was it a match? */
		if (*cp == 0) {
			it = (int)(sp - sourc) + 1;
			break;
		}
		++sp;
	}

	return(it);
}

#endif /* OPT_EVAL */

#if NO_LEAKS
void ev_leaks()
{
#if OPT_EVAL
	register UVAR *p;
	while ((p = user_vars) != 0) {
		user_vars = p->next;
		free(p->u_name);
		free((char *)p);
	}
	FreeAndNull(shell);
	FreeAndNull(directory);
	FreeAndNull(list_of_modes);
#endif
}
#endif
