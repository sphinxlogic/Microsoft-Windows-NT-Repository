/*	xcscrpt.c -- script interpreter module for XC");
	This file uses 4-character tabstops
	Author: larry gensch, December 1987
	Major rewrite: fred buck, Jan 1989
	Binding code: larry gensch, September 1991
	This code is released to the public domain
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/param.h>
#include <ctype.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/stat.h>
#include "xc.h"

jmp_buf			here;
extern FILE		*cfp;
short			ttyflag, debugflag, linkflag, scriptflag,
				mrbstart,	/* ring buffer start pointer */
				mrbcount,	/* ring buffer counter */
				BREAK = 0;	/* a hook for a later 'trap' keyword */
extern short	eofflag;
extern int		redial(), s_set(), s_exit(), xmitbrk();
extern void		divert(), set_onoff(), xcdial();

int				my_escape = 1;	/* Control-A; resettable at run-time */

char			mringbuf[LG_BUFF];	/* ring buffer for modem input */
static			S_abort();
static void		unsetall(), S_bombout(), S_call();
static char		*NO_ARG = "%s command must have an argument";

static bindfunc_t   find_function(); /* bindfunc_t : see xc.h */

typedef struct bindstruct {
	int					bs_c;		/* Character prefix */
	int					bs_function;/* Function code */
	char				*bs_string;	/* String/script to emit */
	struct bindstruct	*bs_next;	/* Pointer to next entry */
} binding_t;
static binding_t *first_binding = NIL(binding_t);

typedef struct {
	bindfunc_t	bf_function;/* Function code */
	char		*bf_name;	/* bind_function() name */
	char		*bf_string;	/* String/script assigned */
} bindstr_t;

static bindstr_t function_list[] = {
	{ENDCHAR, "endchar", "Exit terminal mode"},
	{QUITCHR, "quitchr", "Quit program"},
	{CAPTYES, "captyes", "Turn on terminal mode capture"},
	{CAPTEND, "captend", "Turn off terminal mode capture"},
	{DIVCHAR, "divchar", "Send file through modem"},
	{DIALCHR, "dialchr", "Dial from phonelist"},
	{HUPCHAR, "hupchar", "Hang up modem"},
	{SCRPCHR, "scrpchr", "Prompt for script file"},
	{HLPCHAR, "hlpchar", "Display terminal mode key bindings"},
	{BRKCHAR, "brkchar", "Send modem BREAK signal"},
	{EMITSTR, "emitstr", "Emit string"},
	{DOSCRPT, "doscrpt", "Execute script file"}
};

#define FUNCTION_COUNT (sizeof(function_list) / sizeof(function_list[0]))

static void 
newsigint(junk)
int junk;
{
	eofflag++;
	show_abort();
	S_bombout();
	longjmp(here,1);
}

void 
do_script(file)
char *file;
{
	void (*oldvec)();

	capture = eofflag = FALSE;
	ttyflag = scriptflag = TRUE;

	oldvec = signal(SIGINT, newsigint);
	if (!setjmp(here))
		S_call(file);

	unsetall();
	if (capture)
		capture = FALSE,
		fclose(cfp);

	linkflag = scriptflag = FALSE;

	signal(SIGINT, oldvec);
}

static 
k_seen(bytes,fword)
long bytes;
char *fword;
{
	int i, j, k;
	char *cptr;

	cptr = fword;

	if (!fword || !*fword)
		sprintf(Msg,NO_ARG,"SEEN"),
		S2(Msg);

	j = mrbstart - 1;
	if (bytes<=0 || bytes>LG_BUFF)
		bytes = LG_BUFF;
	k = mrbcount - bytes;	/* check only most recent 'bytes' bytes */
	i = 0;
	while ((i++)<mrbcount){
		++j;
		j = j % LG_BUFF;
		if (i<k)
			continue;
		if (mringbuf[j] != *cptr){
			cptr = fword;
			continue;
		}
		if (*(++cptr)=='\0')
			return SUCCESS;
	}
	return FAILURE;
}

k_waitfor(interval,fword)
long interval;
char *fword;
{
	register c, i = -1 ;
	register long limit, waitfor_msec = 0;
	char *ptr;
	struct tms tbuf;

	mrbstart = mrbcount = 0;
	sprintf(line,"\"%s\"",fword);
	lptr = line;
	getword();
	lc_word(word);
	ptr = word;

	if (interval < -1){
		waitfor_msec = -interval;
		goto SPITOUT;
	}
	if (!fword || word[0] == '\0'){
		sprintf(Msg,NO_ARG,"WAITFOR");
		S2(Msg);
		return FAILURE;
	}

	waitfor_msec = 1000 * ((interval > 0) ? interval : 30);
	eofflag = FALSE;

SPITOUT: limit = times(&tbuf) + (HZ * waitfor_msec)/1000;
	while (limit >= times(&tbuf) && !eofflag){
		if ((c = readbyte(1)) == -1)
			continue;

		if (cismode && c==ENQ){
			s_cis();
			goto SPITOUT;
		}

		++i;
		i = i % LG_BUFF;
		mringbuf[i] = c;
		mrbstart = mrbstart % LG_BUFF;
		if (mrbcount<LG_BUFF)
			++mrbcount;
		else
			++mrbstart,
			mrbstart = mrbstart % LG_BUFF;

		if (ttyflag)
			fputc(c,tfp);

		if (capture && c != '\r')
			fputc(c,cfp);

		if (tolower(c) != *ptr){
			ptr = word;
			continue;
		}

		if (*++ptr == '\0')
			return SUCCESS;
	}
	return FAILURE;
}

static 
k_transmit(junk,fword)
long junk;
char *fword;
{
	sprintf(line,"\"%s\"",fword);
	lptr = line;
	getword();
	if (!fword || word[0] == '\0'){
		sprintf(Msg,NO_ARG,"TRANSMIT");
		S2(Msg);
		return FAILURE;
	}
	send_string(word);
	return SUCCESS;
}

static 
k_pause(pause_time,junk)
long pause_time;
char *junk;
{
	pause_time = pause_time ? pause_time : 5;
	sleep((unsigned)pause_time);
	return SUCCESS;
}

static 
k_dial(junk,fword)
long junk;
char *fword;
{
	sprintf(line,"%s",fword);
	lptr = line;
	getword();
	if (!fword || word[0] == '\0'){
		sprintf(Msg,NO_ARG,"DIAL");
		S2(Msg);
		return FAILURE;
	}
	xcdial(word);
	return SUCCESS;
}

static 
k_capture(junk,fword)
long junk;
char *fword;
{
	int val = capture;

	sprintf(word,"capture");
	sprintf(line,"%s",fword);
	lptr = line;
	set_onoff(&capture);

	if (val == capture)
		return SUCCESS;

	if (!capture)
		fclose(cfp);
	else {
		if (!(cfp = fopen(captfile, "a"))){
			sprintf(Msg,"Can't open capture file %s",captfile);
			S2(Msg);
			eofflag++;
			return FAILURE;
		}
		setbuf(cfp,NIL(char));
	}
	return SUCCESS;
}

static 
k_debug(junk,fword)
long junk;
char *fword;
{
	sprintf(word,"debug");
	sprintf(line,"%s",fword);
	lptr = line;
	set_onoff(&debugflag);
	return SUCCESS;
}

static 
k_tty(junk,fword)
long junk;
char *fword;
{
	sprintf(word,"tty");
	sprintf(line,"%s",fword);
	lptr = line;
	set_onoff(&ttyflag);
	return SUCCESS;
}

static 
k_type(junk,fword)
long junk;
char *fword;
{
	sprintf(line,"%s",fword);
	lptr = line;
	getword();
	if (!fword || word[0] == '\0'){
		sprintf(Msg,NO_ARG,"TYPE");
		S2(Msg);
		return FAILURE;
	}
	divert(TRUE);
	return SUCCESS;
}

static 
k_linked()
{
	return linkflag;
}

/*	unbind_key() removes the binding attached to the keycode specified by (c).*/
static void 
unbind_key(c)
int c;
{
	binding_t *ptr = first_binding, *prev = NIL(binding_t);
	if (!ptr)
		return;

	while (ptr) {
		if (ptr->bs_c == c) {
			if (ptr->bs_string)
				free(ptr->bs_string);
			if (prev)
				prev->bs_next = ptr->bs_next;
			else
				first_binding->bs_next = ptr->bs_next;
			free(ptr);
			return;
		}
		prev = ptr;
		ptr = ptr->bs_next;
	}
}

/*	bind_key() binds the key whose ASCII code is specified by (c) to the
	function whose code is specified by (function). Emit strings
	(for EMITSTR) and script names (for DOSCRPT) are specified by (string).
*/
static void 
bind_key(c,function,string)
int c;
int function;
char *string;
{
	binding_t *ptr = (binding_t *) malloc(sizeof(binding_t)), *curr, *prev;

	if (!ptr) {
			S2("BIND_KEY allocation error");
			S_abort();
	}

	unbind_key(c);

	ptr->bs_c = c;
	ptr->bs_function = function;
	ptr->bs_string = strdup(string);

	/* The following is an insertion sort to ensure that the bindings are
	   stored in ascending sequence by key code.  This makes
	   show_bindings()'s display easier to read. -lg
	*/

	for (prev = NIL(binding_t), curr = first_binding;
		curr != NIL(binding_t);
		prev = curr, curr = curr->bs_next) {
		if (ptr->bs_c < curr->bs_c) {
			if (!prev) {
				ptr->bs_next = first_binding;
				first_binding = ptr;
				break;
			} else {
				ptr->bs_next = curr;
				prev->bs_next = ptr;
				break;
			}
		}
	}

	if (curr == NIL(binding_t)) {
		if (!prev)
			first_binding = ptr;
		else
			prev->bs_next = ptr;
		ptr->bs_next = NIL(binding_t);
	}
}

/*	bind_function() Binds the key whose code is represented by the value in
	(n) to execute the XC builtin function whose name is pointed to be (cptr).
	Any previous binding of the specified keycode is forgotten.
	This routine is designed to be an XC script builtin.
*/
static 
bind_function(n,cptr)
long n;
char *cptr;
{
	int c;
	bindfunc_t code;

	if (n < 1L || n > 127L) {
		sprintf(Msg,"Invalid key code %d in BIND_FUNCTION command",n);
		S2(Msg);
		return FAILURE;
	}

	c = tolower((int) n);

	sprintf(line, "\"%s\"", cptr);
	lptr = line;
	getword();
	if (cptr && cptr[0] != '\0') {
		lc_word(word);
		code = find_function(word);
		if (code == BADFUNC) {
			sprintf(Msg,"Invalid function name '%s' in BIND_FUNCTION command",
			word);
			S2(Msg);
			return FAILURE;
		}
		bind_key(c, code, word);
	} else {
		sprintf(Msg,NO_ARG,"BIND_FUNCTION");
		S2(Msg);
		return FAILURE;
	}

	return SUCCESS;
}

/*	bind_string() binds the key whose code is represented by the
	decimal value in (n) to emit the string pointed to by (cptr).
	Any previous binding of the specified keycode is forgotten.
	This routine is designed to be an XC script builtin.
*/
static 
bind_string(n,cptr)
long n;
char *cptr;
{
	int c;

	if (n < 1L || n > 127L) {
		sprintf(Msg,"Invalid key code %d in BIND_STRING command",n);
		S2(Msg);
		return FAILURE;
	}

	c = tolower((int) n);

	sprintf(line, "\"%s\"", cptr);
	lptr = line;
	getword();
	if (cptr && cptr[0] != '\0')
		bind_key(c, EMITSTR, word);
	else {
		sprintf(Msg,NO_ARG,"BIND_STRING");
		S2(Msg);
		return FAILURE;
	}

	return SUCCESS;
}

/*	bind_script() binds the key whose code is represented by the value in
	(n) to execute the XC script whose name is pointed to by (cptr).
	Any previous binding of the specified keycode is forgotten.
	This routine is designed to be an XC script builtin.
*/
static 
bind_script(n,cptr)
long n;
char *cptr;
{
	int c;

	if (n < 1L || n > 127L) {
		sprintf(Msg,"Invalid key code %d in BIND_STRING command",n);
		S2(Msg);
		return FAILURE;
	}

	c = tolower((int) n);

	sprintf(line, "\"%s\"", cptr);
	lptr = line;
	getword();
	if (cptr && cptr[0] != '\0')
		bind_key(c, DOSCRPT, word);
	else {
		sprintf(Msg,NO_ARG,"BIND_SCRIPT");
		S2(Msg);
		return FAILURE;
	}

	return SUCCESS;
}

/*	Variables Section */
/*	Most of the variable-handling logic is credit: Steve Manes 1987 */
#define VNAMELEN	16	/* maximum name length for variables */
#define VMAXSIZE	256	/* maximum length for CHAR variable */
#define VMAXVARS	30	/* maximum number of user variables */
#define VCHAR		'C'	/* CHARACTER variable type */
#define VNUM		'N'	/* NUMERIC variable type (always 'long') */

/* Variable structure */
typedef struct var {
	char name[VNAMELEN+1];	/* variable name */
	struct var	*next;		/* ptr to next structure in var_list */
	char type;				/* variable type */
	union {					/* pointer to CHAR or NUM/DATE */
		char str[VMAXSIZE+1];
		long num;
	} u;
} VAR;

static VAR	*Varlist = NIL(VAR);	/* top of variable list */
static VAR	*Lastvar = NIL(VAR);	/* bottom of variable list */

/* Valid variable name characters */
unchar 
OKname[]= {
	 /* control characters */
	  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	 /* ! " # $ % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ */
		1,0,1,0,0,0,0,0,0,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
	 /* A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ \ ] ^ _ ` */
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,
	 /* a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~	   */
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0
};

/*	Return pointer to VAR structure for User or System variable
	'name', otherwise return NIL(VAR).
	Variable contents in vp->u.[str|num]
*/
static VAR *
findvar(name)
char *name;
{
	static	VAR *vp;	/* pointer to output structure */

	if (!name || !*name || !Varlist)
		return NIL(VAR);
	vp = Varlist;
	while (vp){
		if (!strncmp(name, vp->name, VNAMELEN) )
			return vp;
		vp = vp->next;
	}
	return NIL(VAR);				/* not found */
}

/*	Delete User variable 'name' from VAR list.
	If variable doesn't exist, no error is returned
*/
static void 
unsetvar(name)
char *name;
{
	VAR *p;
	VAR *lastp;

	if (!name || !*name)
		return;
	for (p=Varlist; p; lastp = p, p = p->next){
		if (!strncmp(name, p->name, VNAMELEN) ){	/* name match? */
			if (Varlist == Lastvar)					/* only 1 variable */
				Varlist = Lastvar = NIL(VAR);		/* in list */
			else if (p == Varlist)					/* first variable */
				Varlist = p->next;
			else {
				lastp->next = p->next;				/* dump variable in middle
													   of list */
				if (p == Lastvar)					/* or last object */
					Lastvar = lastp;				/* in list */
			}
			free(p);			/* reclaim memory */
			break;
		}
	}
}

/*	Set the value of User variable 'name' to 'val' with 'type'.
	If variable exists, change its contents. Otherwise, create it.
	Returns: FAILURE or SUCCESS
*/
static 
setvar(name,type,str,val)
char *name;
char type;
char *str;
long *val;
{
	VAR *vp;
	short i;

	if (!name || !*name)
		return FAILURE;
	if (!(vp = findvar(name))){ /* create new variable */
		for (i=0; i < VNAMELEN && name[i]; i++){
			if( !OKname[(name[i] & 0x7F)] || isdigit(name[0]) ){
				sprintf(Msg,"Illegal variable name '%s'",name);
				S_abort();
			}
		}
		if (!(vp = (VAR *)malloc(sizeof(VAR)))){
			sprintf(Msg,"%s: allocation error",name);
			S2(Msg);
			return FAILURE;
		}
		lc_word(name);
		strncpy(vp->name, name, VNAMELEN);	/* set vari name */
		vp->next = NIL(VAR);				/* flag 'no next' */
		if (!Varlist)
			Varlist = vp;					/* first variable */
		else
			Lastvar->next = vp;				/* add this to the list */
		Lastvar = vp;						/* set 'last' pointer */
	}

	if (type == VCHAR)
		strncpy(vp->u.str, str, VMAXSIZE);
	else
		vp->u.num = *val;
	vp->type = type;
	return SUCCESS;
}

/*	Unset all user variables, deallocating memory.
	No error returned
*/
static void 
unsetall()
{
	VAR *p;
	VAR *nextp;

	if (!Varlist)
		return;
	p = Varlist;
	while (p->next){
		nextp = p->next;
		free(p);
		p = nextp;
	}
	Varlist = Lastvar = NIL(VAR);
}
/*	end variables section */

/*	Action Primitives */
static struct {
	char *name;
	int (*funcptr)();
} s_acttab[] = {
	{"beep",		beep},
	{"bind_function",	bind_function},
	{"bind_script",	bind_script},
	{"bind_string",	bind_string},
	{"capture",		k_capture},
	{"debug",		k_debug},
	{"dial",		k_dial},
	{"hangup",		hangup},
	{"linked",		k_linked},
	{"pause",		k_pause},
	{"quit",		s_exit},
	{"redial",		redial},
	{"seen",		k_seen},
	{"xmitbrk",		xmitbrk},
	{"transmit",	k_transmit},
	{"tty",			k_tty},
	{"type",		k_type},
	{"waitfor",		k_waitfor},
	{NIL(char),			0}
};
/*	end of primitives */

/*	token types */
typedef enum {
	NULLTOK,	/* terminating '\0' in script buffer */
	ACTION,		/* an action (primitive or script cmd) */
	AFFIRM,		/* script 'affirm' */
	BACKQUOT,	/* script command substitution */
	SBREAK,		/* script 'break' */
	CALL,		/* script 'call' */
	COMMENT,	/* comment */
	SCONTNUE,	/* script 'continue' */
	DECR,		/* script 'decr' */
	DO,			/* script 'do' */
	DONE,		/* script 'done' */
	ECHOS,		/* script 'echo' */
	EFLAG,		/* '-n' switch for script 'echo' cmd */
	ELSE,		/* script 'else' */
	ENDIF,		/* script 'endif' */
	ENDTRAP,	/* script 'endtrap' */
	EQ,			/* operator "equals" */
	EXIT,		/* script 'exit' */
	SFILE,		/* script 'file' */
	SFALSE,		/* script 'false' */
	IF,			/* script 'if' */
	INCR,		/* script 'incr' */
	LESSTHAN,	/* operator "less than" */
	LITERAL,	/* a literal string (e.g. "abcde") */
	MORETHAN,	/* operator "greater than" */
	NEGATE,		/* negation operator for comparisons */
	NEQ,		/* operator "not equal to" */
	NUMBER,		/* a numeric constant (e.g. 12345) */
	PIPE,		/* script 'pipe' */
	READ,		/* script 'read' */
	SHELL,		/* script 'shell' */
	SET,		/* script 'assign' */
	STRAP,		/* script 'trap' */
	TERMINAT,	/* statement terminators (';' and '\n') */
	THEN,		/* script 'then' */
	STRUE,		/* script 'true' */
	UNSET,		/* script 'unset' */
	UNTRAP,		/* script 'untrap' */
	XCSET,		/* xc 'set' command */
	VARNAME,	/* a variable name */
	WHILE,		/* script 'while' */
	TTERROR,	/* unrecognizable token */
	TIMEOUT		/* script 'timeout' */
} TOK_TYPE;

/*	token table */
static struct {
	char *name;
	TOK_TYPE token;
} s_toktab[] = {
	{"NULLTOK",		NULLTOK},
	{"ACTION",		ACTION},
	{"affirm",		AFFIRM},
	{"BACKQUOT",	BACKQUOT},
	{"break",		SBREAK},
	{"call",		CALL},
	{"COMMENT",		COMMENT},
	{"continue",	SCONTNUE},
	{"decr",		DECR},
	{"do",			DO},
	{"done",		DONE},
	{"echo",		ECHOS},
	{"-n",			EFLAG},
	{"else",		ELSE},
	{"endif",		ENDIF},
	{"fi",			ENDIF},
	{"ENDTRAP",		ENDTRAP},
	{"eq",			EQ},
	{"exit",		EXIT},
	{"false",		SFALSE},
	{"file",		SFILE},
	{"if",			IF},
	{"incr",		INCR},
	{"lessthan",	LESSTHAN},
	{"LITERAL",		LITERAL},
	{"morethan",	MORETHAN},
	{"!",			NEGATE},
	{"neq",			NEQ},
	{"NUMBER",		NUMBER},
	{"pipe",		PIPE},
	{"read",		READ},
	{"assign",		SET},
	{"shell",		SHELL},
	{"TRAP",		STRAP},		/* 'trap' keyword left for later dev't */
	{"TERMINAT",	TERMINAT},
	{"then",		THEN},
	{"timeout",		TIMEOUT},
	{"true",		STRUE},
	{"unassign",	UNSET},
	{"UNTRAP",		UNTRAP},
	{"set",			XCSET},
	{"VARNAME",		VARNAME},
	{"while",		WHILE},
	{"\0",			TTERROR}
};
/*	end token types */

/*	tok_value is set by lexan() in the following instances:
	 TOK_TYPE NUMBER:	 (long) value of number
	 TOK_TYPE LITERAL:	 pointer to beginning of quoted string
	 TOK_TYPE ACTION:	 function pointer to appropriate vector
	 TOK_TYPE VARNAME:	 pointer to VAR struct
	 TOK_TYPE TTERROR:	 pointer to strange construction in script
	All other values of TOK_TYPE don't require further information.
*/
static union {
	long numval;		/* numbers */
	char *strptr;		/* for literal strings (points to initial '"') */
	int (*funcptr)();	/* vector for primitives */
	VAR *varptr;		/* for variables */
} tok_value;


/*	lexan() is the lexical analyzer, which translates words
	into token types and sets tok_value appropriately. It's
	called repeatedly by the language parser, S_parse().
*/
static TOK_TYPE
lexan(pcptr)
char **pcptr;			/* address of script program counter */
{
	long nvalue, negpos = 1;
	VAR *varptr;
	FILE *bqpipe;
	int i, c;				/* really a char, but 'int' to spot EOF */
	static char *cptr, *lasttok,
				latoken[VNAMELEN+1], temp[VMAXSIZE+1], bqcmd[VMAXSIZE+1];
	extern FILE *popen();

	/* if in debug mode, echo script line to tfp */
	if (debugflag && *pcptr>lasttok){
		cptr = *pcptr - 1;
		while (*cptr==' ' || *cptr=='\t') --cptr;
		if (*cptr=='\n'){
			fputs("+ ",tfp);
			++cptr;
			while (*cptr!='\n' && *cptr)
				fputc(*(cptr++),tfp);
			fputc('\r',tfp),
			fputc('\n',tfp);
		}
	}

	/* skip to beginning of next token */
	while (**pcptr==' ' || **pcptr=='\t') ++(*pcptr);
	tok_value.strptr = cptr = lasttok = *pcptr;				/* save place */

									/* negation operator for comparisons */
	if (*cptr=='!' && (*(cptr+1)==' ' || *(cptr+1)=='\t')){
		++cptr;
		*pcptr = cptr;
		return NEGATE;
	}
									/* comment in script */
	if (*cptr=='#'){
		while (*cptr && *cptr!='\n') ++cptr;
		*pcptr = cptr;
		return TERMINAT;
	}
									/* statement terminator */
	if (*cptr==';' || *cptr=='\n'){
		++cptr;
		*pcptr = cptr;
		return TERMINAT;
	}
									/* end of script */
	if (*cptr=='\0')
		return NULLTOK;
									/* quoted literal string */
	if (*cptr=='"'){
		++cptr;
		while (*cptr && *cptr!='\n' && !(*cptr=='"' && *(cptr-1)!='\\'))
			++cptr;
		if (*cptr=='"'){
			++cptr;
			*pcptr = cptr;
			return LITERAL;
		}
		sprintf(Msg,"Unmatched quote");
		S_abort();
	}
							/* environment variable (treat as a literal) */
	if (*cptr=='$'){
		++cptr;
		for (i=0; i<VMAXSIZE; ++i){
			if (!*cptr || *cptr==' ' || *cptr=='\t' || *cptr=='\n'
				|| *cptr=='\r' || *cptr==';')
					break;
			temp[i] = *(cptr++);
		}
		temp[i] = '\0';
		tok_value.strptr = getenv(temp);
		if (!tok_value.strptr)
			tok_value.strptr = temp,
			sprintf(Msg,"%s: no such environment variable",temp),
			S2(Msg),
			tok_value.strptr = "";
		*pcptr = cptr;
		return LITERAL;
	}
						/* back-quoted shell command (treat like env var) */
	if (*cptr=='`'){
		++cptr;
		i = 0;
		while (*cptr && *cptr!='\n' && *cptr!='`' && (++i)<VMAXSIZE)
			++cptr;
		if (*cptr=='`'){
			for (i=0; i<VMAXSIZE; ++i){			/* tok_value ptr points */
				bqcmd[i] = tok_value.strptr[i+1];	/* to leading '`' */
				if (bqcmd[i]=='`'){
					bqcmd[i] = '\0';
					break;
				}
			}
			bqcmd[i] = '\0';
			signal(SIGCLD,SIG_DFL);
			if (!(bqpipe=popen(bqcmd,"r"))){
				sprintf(Msg,"%s: cannot create pipe",bqcmd);
				S_abort();
			}
			else {
				temp[0] = '\0';
				i = 0;
				while (i<=VMAXSIZE && (c=fgetc(bqpipe))!=EOF && c!='\n')
					temp[i++] = c;
				fflush(bqpipe);
				pclose(bqpipe);
				temp[i] = '\0';
				tok_value.strptr = temp;
				*pcptr = cptr + 1;
				return LITERAL;
			}
		}
		else {
			sprintf(Msg,"Unmatched back-quote:");
			S_abort();
		}
	}
								/* dialout port name */
	if (!strncmp(cptr,"portname",8)){
		tok_value.strptr =mport(NIL(char));
		*pcptr += 8;
		return LITERAL;
	}
								/* leading hyphen, maybe a negative number? */
	if (*cptr=='-')
		negpos = (-1),
		++cptr;
								/* string beginning with a digit */
	if (isdigit(*cptr)){
		nvalue = (*cptr - '0') * negpos;
		while (*(++cptr)){
			if (isdigit(*cptr)){
				nvalue *= 10;
				nvalue += (*cptr - '0');
				if (nvalue>0) nvalue *= negpos;
				continue;
			}
			else if (strchr(" \t\n;",*cptr)){
				tok_value.numval = nvalue;
				*pcptr = cptr;
				return NUMBER;
			}
			sprintf(Msg,"Variable name cannot begin with a digit: ");
			S_abort();
		}
		tok_value.numval = nvalue;
		*pcptr = cptr;
		return NUMBER;
	}
					/* check for '-n' switch for echo (type EFLAG) */
	if (negpos<0){
		if (*cptr=='N' || *cptr=='n'){
			while (*cptr && !strchr(" \t\n;",*cptr)) ++cptr;
			*pcptr = cptr;
			return EFLAG;
		}
		sprintf(Msg,"Bad option to ECHO command");
		S_abort();
	}
							/* impermissible initial character */
	if (!isalpha(*cptr)){
		sprintf(Msg,"bad initial character: %c", *cptr);
		S_abort();
	}

		/* remember that tok_value.strptr points to start of token */
	for (i=1; i<(VNAMELEN+1); ++i){		/* jump to next field separator */
		++cptr;
		if (*cptr=='\0' || strchr(" \t\n;",*cptr))
			break;
	}
	if (i>VNAMELEN){						/* word too long */
		sprintf(Msg,"Variable name too long");
		S_abort();
	}
	strncpy(latoken,tok_value.strptr,i);	/* copy word to array 'latoken' */
	latoken[i] = '\0';
	lc_word(latoken);						/* cvt to lowercase */
											/* script keywords */
					/* scan table for keyword match */
	for (i=0; *(s_toktab[i].name) && strcmp(latoken,s_toktab[i].name); ++i)
		;
	if (*s_toktab[i].name)	{				/* lc keyword recognized */
		if (s_toktab[i].token==STRUE)
			tok_value.numval = TRUE;
		if (s_toktab[i].token==SFALSE)
			tok_value.numval = FALSE;
		*pcptr = cptr;
		return s_toktab[i].token;
	}
									/* system primitive (ACTION) */
									/* scan table for keyword match */
	for (i=0;s_acttab[i].name && strcmp(latoken,s_acttab[i].name);++i)
		;
	if (s_acttab[i].name){			/* primitive recognized */
		tok_value.funcptr = s_acttab[i].funcptr;
		*pcptr = cptr;
		return ACTION;
	}
									/* user variable name */
	if ((varptr=findvar(latoken))){	/* existing user variable */
		tok_value.varptr = varptr;
		*pcptr = cptr;
		return VARNAME;
	}
				/* could this be the name of a new variable? */
	tok_value.strptr = latoken;	/* just in case this is 'on', 'off', etc. */
	if (!setvar(latoken,VCHAR,"",0))	/* can't create it */
		return TTERROR;
	if (!(varptr=findvar(latoken)))	/* can't retrieve it */
		return TTERROR;
	else {							/* got it */
		tok_value.varptr = varptr;
		*pcptr = cptr;
		return VARNAME;
	}
}

/*		utility routines called by S_parse() */

/*	S_affirm is a placeholder. It's the function to get a yes or no
	response from the user.
*/
static 
S_affirm()
{
	char c, junk;

	c = getchar();
	fputc(c,tfp);
	while ((junk=getchar()) !='\n' && junk !='\r')
		fputc(junk,tfp);

	fputc('\r',tfp),
	fputc('\n',tfp);
	return(c=='y' || c=='Y');
}

/*	S_addsub increments or decrements a numeric variable.
	It assumes that since the INCR or DECR directives call
	lexan() for the variable just before coming here, the
	tok_value structure contains a pointer to the variable
	whose value is to be changed.
*/
static 
S_addsub(direction)
int direction;
{
	long oldval = tok_value.varptr->u.num;
	oldval += direction;

	if (!setvar(tok_value.varptr->name,VNUM,"",&oldval)){
		sprintf(Msg,"Error setting variable '%s'", tok_value.varptr->name);
		S_abort();
	}
	return(tok_value.varptr->u.num ? SUCCESS : FAILURE);
}

/*	S_qstrip returns a pointer to a (static) string with leading and
	trailing double-quote marks removed. If the string happens to
	lack a leading or trailing double-quote mark, then the string
	will be returned with its beginning unchanged, and its length
	equal to VMAXSIZE or the length of the string, whichever is
	shorter. Double-quote marks escaped with a backslash are included
	in the returned string and the backslash is excised.
*/
static char *
S_qstrip(strptr)
char *strptr;
{
	int i;
	static char strbuf[VMAXSIZE+2];

	if (*strptr=='"')
		++strptr;
	for (i=0; i<VMAXSIZE+1; ++i){
		if (*strptr=='\\' && *(strptr+1)=='"' && *(strptr-1)!='\\')
			++strptr;
		if ((*strptr=='"' && *(strptr-1)!='\\') || !*strptr || *strptr=='\n')
			break;
		strbuf[i] = *strptr;
		++strptr;
	}
	strbuf[i] = '\0';
	return strbuf;
}

/*	S_read does the parsing grunts for the script 'read' directive.	On
	entry, the 'read' token has been parsed, but that's all.
*/
static 
S_read(pcptr)
char **pcptr;
{
	int i;
	VAR *varptr1;
	static char strbuf[VMAXSIZE+2];

	if (lexan(pcptr)!=VARNAME)
		S_abort();
	varptr1 = tok_value.varptr;
	strbuf[0] = '\0';
	for (i=0; i<VMAXSIZE; ++i){
		strbuf[i] = getchar();
		if (strbuf[i]==BS){
			if (i>0)
				fputs("\b \b",tfp),
				i -= 2;
			else
				i = -1;
			continue;
		}
		fputc(strbuf[i],tfp);
		if (strbuf[i]=='\n' || strbuf[i]=='\r'){
			strbuf[i] = '\0';
			break;
		}
	}
	strbuf[VMAXSIZE] = '\0';
	fputc('\r',tfp),
	fputc('\n',tfp);
	return(setvar(varptr1->name,VCHAR,strbuf,0));
}

/*	S_perform invokes a system primitive and returns SUCCESS if the
	primitive succeeds, FAILURE if it fails. On entry, only the ACTION
	token has been parsed.
*/
static 
S_perform(pcptr)
char **pcptr;
{
	int (*fptr)();
	char *cptr;
	VAR *varptr1;
	long n = -1;

	fptr = tok_value.funcptr;
	cptr = NIL(char);
	while (TRUE){
		switch (lexan(pcptr)){
			case TERMINAT:
				break;
			case NUMBER:
				if (n != -1)
					S_abort();
				n = tok_value.numval;
				continue;
			case LITERAL:
			case TTERROR:
				cptr = S_qstrip(tok_value.strptr);
				continue;
			case VARNAME:
				varptr1 = tok_value.varptr;
				if (varptr1->type==VCHAR){
						cptr = varptr1->u.str;
						break;
				}
				if (n != -1)
					S_abort();
				n = varptr1->u.num;
				continue;
			default:
				S_abort();
		}
		break;
	}
	return (*fptr)(n,cptr);
}

/*	S_set does the parsing grunts for the script 'assign' directive. It's
	a separate function mostly to keep from cluttering up S_parse()
	too much.	On entry, only the 'set' token has been received from
	the directive containing it.
*/
static
S_set(pcptr)
char **pcptr;				/* S_parse()'s program counter (p_pc) */
{
	TOK_TYPE nexttype;
	VAR *varptr1, *varptr2;
	char *setstr;

	if ((nexttype=lexan(pcptr))!=VARNAME)
		S_abort();
	varptr1 = tok_value.varptr;
	if ((nexttype=lexan(pcptr))!=EQ)
		S_abort();
	switch (nexttype = lexan(pcptr)){
		case LITERAL:
			setstr = S_qstrip(tok_value.strptr);
			return(setvar(varptr1->name,VCHAR,setstr,0));
		case ACTION:
		case AFFIRM:
			if (nexttype==ACTION)
				tok_value.numval = (long) S_perform(pcptr);
			else
				tok_value.numval = (long) S_affirm();
		case NUMBER:
		case STRUE:
		case SFALSE:
			return(setvar(varptr1->name,VNUM,"",&tok_value.numval));
		case VARNAME:
			varptr2 = tok_value.varptr;
			switch (varptr2->type){
				case VCHAR:
					return(setvar(varptr1->name,VCHAR,varptr2->u.str,0));
				default:
					return(setvar(varptr1->name,VNUM,"",&(varptr2->u.num)));
			}
		default:
			S_abort();
	}
}

/*	S_varcmp() compares a variable's value with a string or numeric
	literal, or with the value of a second variable. Once again,
	this function does parsing grunts for S_parse().
*/
static
S_varcmp(varptr1, pcptr)
VAR *varptr1;
char **pcptr;
{
	TOK_TYPE compmode;
	long testnum;
	static char strbuf[VMAXSIZE+1];
	char *cmpstr;
	int status, numvar;
	VAR *varptr2;

	numvar = (varptr1->type!=VCHAR);
	compmode = lexan(pcptr);
	switch (compmode){
		case EQ:
		case NEQ:
		case MORETHAN:
		case LESSTHAN:
			break;
		case TERMINAT:
			if (numvar)
				return(varptr1->u.num ? TRUE : FALSE);
			else
				return(*varptr1->u.str ? TRUE : FALSE);
		default:
			S_abort();
	}
	switch (lexan(pcptr)){
		case LITERAL:
			if (numvar){
				sprintf(Msg,"Error: %s is a numeric variable",varptr1->name);
				S_abort();
			}
			++tok_value.strptr;
			strncpy(strbuf,tok_value.strptr,VMAXSIZE);
			*(strchr(strbuf,'"')) = '\0';
			cmpstr = strbuf;
			break;
		case NUMBER:
		case STRUE:
		case SFALSE:
			if (!numvar){
				sprintf(Msg,"Error: %s is a string variable",varptr1->name);
				S_abort();
			}
			testnum = tok_value.numval;
			break;
		case VARNAME:
			varptr2 = tok_value.varptr;
			if (numvar && varptr2->type==VCHAR){
				sprintf(Msg,"Error: %s and %s are of different types",
					varptr1->name, varptr2->name);
				S_abort();
			}
			if (numvar)
				testnum = varptr2->u.num;
			else
				cmpstr = varptr2->u.str;
			break;
		default:
			S_abort();
	}
	if (numvar){
		status = (varptr1->u.num==testnum);
		if (compmode==EQ)
			return status;
		if (compmode==NEQ)
			return !status;
		status = (varptr1->u.num > testnum);
		if (compmode==MORETHAN)
			return status;
		else
			return !status;
	}
	status = strcmp(varptr1->u.str,cmpstr);
	if (compmode==EQ)
		return(status==0);
	if (compmode==NEQ)
		return status;
	if (compmode==MORETHAN)
		return(status>0);
	else
		return(status<0);
}

static char *
S_construct(pcptr)
char **pcptr;
{
	char *cptr;
	static char newstring[VMAXSIZE+10];
	TOK_TYPE nexttype;

	newstring[0] = '\0';
	cptr = newstring;
	while ((nexttype=lexan(pcptr))!=NULLTOK){
		if (strlen(newstring)>VMAXSIZE){
			newstring[VMAXSIZE] = '\0';
			break;
		}
		switch (nexttype){
			case TERMINAT:
				break;
			case NUMBER:
				sprintf(cptr,"%ld",tok_value.numval);
				cptr += strlen(cptr);
				continue;
			case LITERAL:
				sprintf(cptr,"%s",S_qstrip(tok_value.strptr));
				cptr += strlen(cptr);
				continue;
			case VARNAME:
				if (tok_value.varptr->type != VCHAR)
					sprintf(cptr,"%ld",tok_value.varptr->u.num);
				else
					sprintf(cptr,"%s",tok_value.varptr->u.str);
				cptr += strlen(cptr);
				continue;
			default:
				S_abort();
		}
		break;
	}
	return newstring;
}

/*		stack protection and break/continue stuff */

static int	nest_while,		/* number of nested loops */
			nest_parse,		/* number of nested calls to S_parse() */
			nest_cmd;		/* number of nested commands */

static long deadline;		/* deadline for 'timeout' keyword */

jmp_buf env;				/* cell for environment, setjmp */

#define CMDNEST		8		/* max number of nested scripts */
#define PARSNEST	50		/* max number of nested calls to parser */

static char *areas[CMDNEST];

#define DNP		--nest_parse
#define BCCHK(x) if(x==SBREAK||x==SCONTNUE){DNP;return(nest_while?x:S_abort());}

/* cleanup any debris left after a non-trapped keyboard interrupt */
static void 
S_bombout()
{
	int i;

	for (i=0; i<nest_cmd; ++i){
		if (areas[i])
			free(areas[i]),
			areas[i] = NIL(char);
	}
	nest_cmd = nest_while = nest_parse = 0;
	deadline = 0;
}

/*	S_parse() */
static char	*intercom,		/* last previous value of S_parse program ctr */
			*tvector;		/* trap vector */
static FILE *savetfp;		/* to stash tfp when tfp redirected */
static 
S_parse(p_pc, t_invoke)
char *p_pc;
TOK_TYPE t_invoke;
{
	long n;					/* "leading" number for primitives */
	int i,
		status,				/* status of last performed operation */
		retval,				/* value to be returned by this function */
		testing,			/* flag set if within 'if' or 'while' clause */
		direction,			/* if 1, increment, if -1, decrement */
		w_status,			/* used to correct nest_parse in 'while' */
		counter,			/* for WHILE and IF, to track keywords */
		negating;			/* ! operator in effect for comparisons */
	char *cptr, *S_WHILE, *S_DO, *S_DONE;
	TOK_TYPE nexttype;
	VAR *varptr1;

	++nest_parse;
	testing = (t_invoke==THEN || t_invoke==DO);
	retval = status = negating = FALSE;
	n = -1;
	counter = 0;
	while (TRUE){
		/* we come through here only at the beginning of expressions */
		if (deadline){
			if (time(0)>deadline){		/* deadline; exit current script */
				deadline = 0;
				longjmp(env,TIMEOUT);
			}
		}
		if (BREAK && tvector && t_invoke!=ENDTRAP)	/* interrupt trap */
			BREAK = FALSE,
			cptr = tvector,
			status = S_parse(cptr,ENDTRAP),
			DNP;

		BREAK = FALSE;
		retval = testing ? (retval || status) : status;
		direction = 1;
		intercom = p_pc;
		nexttype = lexan(&p_pc);
				/* check for list terminator */
		if (nexttype==t_invoke || (t_invoke==ENDIF && nexttype==ELSE)){
			if (debugflag && testing)
				fprintf(tfp,"\r\n\t\t\t\t\t\t\tCondition: %s\r\n",
					retval ? "TRUE" : "FALSE");
			return retval;
		}
		switch (nexttype){
			case NULLTOK:		/* inconsistent list terminators */
			case DO:								/**/
			case DONE:								/**/
			case THEN:								/**/
			case ELSE:								/**/
			case ENDIF:								/**/
			case TTERROR:							/**/
			case EFLAG:			/* not at beginning of expressions */
			case EQ:								/**/
			case NEQ:								/**/
			case MORETHAN:							/**/
			case LESSTHAN:							/**/
				S_abort();
			case LITERAL:
				if (!testing)
					S_abort();
				status = !!strlen(tok_value.strptr);
				if (negating)
					status = !status;
				continue;
			case NEGATE:
				if (!testing || negating)
					S_abort();
				negating = 1;
				continue;
			case NUMBER:
				if (n!=(-1))
					S_abort();
				n = tok_value.numval;
				continue;
			case TERMINAT:
				negating = 0;
				continue;
			case ACTION:
				status = S_perform(&p_pc);
				if (negating)
					status = !status;
				break;
			case AFFIRM:
				status = S_affirm();
				if (negating)
					status = !status;
				break;
			case SBREAK:
			case SCONTNUE:
				if (testing || t_invoke==NULLTOK)
					S_abort();
				return(nexttype==SBREAK ? SBREAK : SCONTNUE);
			case CALL:
				lexan(&p_pc);
				i = nest_while;
				nest_while = 0;
				S_call(S_qstrip(tok_value.strptr));
				DNP;
				nest_while = i;
				break;
			case COMMENT:
				continue;
			case DECR:
				direction = (-1);		/* and fall through to... */
			case INCR:
				if ((nexttype=lexan(&p_pc))!=VARNAME)
					S_abort();
				if ((tok_value.varptr->type) != VNUM){
					sprintf(Msg,"Error: %s is not a numeric variable",
						tok_value.varptr->name);
					S_abort();
				}
				status = S_addsub(direction);
				if (negating) status =
					!status;
				break;
			case ECHOS:
				status = 1;
				if ((nexttype = lexan(&p_pc))==EFLAG)
					status = 0;
				else
					p_pc = intercom,
					lexan(&p_pc);
				cptr = S_construct(&p_pc);
				fprintf(tfp,"%s",cptr);
				if (status)
					if (tfp != cfp)
						fputc('\r',tfp),
					fputc('\n',tfp);
				else
					status = 1;
				break;
			case EXIT:
				longjmp(env,EXIT);
			case READ:
				status = S_read(&p_pc);
				break;
			case SET:
				status = S_set(&p_pc);
				if (negating)
					status = !status;
				break;
			case SHELL:
				sprintf(word,"!");
				sprintf(line,"%s",S_construct(&p_pc));
				if (tfp==cfp)
					sprintf(&line[strlen(line)]," >>%s",captfile);
				lptr = line;
				status = !s_shell();
				if (negating)
					status = !status;
				break;
			case PIPE:
				sprintf(word,"$");
				sprintf(line,"%s",S_construct(&p_pc));
				lptr = line;
				status = !s_shell();
				if (negating)
					status = !status;
				break;
			case SFILE:
				savetfp = tfp;
				if (!capture){
					S2("Capture option not on");
					while ((nexttype=lexan(&p_pc))!=TERMINAT &&
						nexttype != NULLTOK);
					break;
				}
				tfp = cfp;
				status = S_parse(p_pc,TERMINAT);
				if (negating)
					status = !status;
				DNP;
				while ((nexttype=lexan(&p_pc)) !=TERMINAT &&
					nexttype != NULLTOK)
					;
				tfp = savetfp;
				fseek(cfp,0L,2);
				break;
			case STRUE:
			case SFALSE:
				status = (nexttype==STRUE);
				intercom = p_pc;
				if ((nexttype=(lexan(&p_pc)))!=TERMINAT && nexttype!=NULLTOK)
						S_abort();
				if (negating)
					status = !status;
				break;
			case STRAP:
				tvector = p_pc;
				cptr = tvector;
				while ((nexttype=lexan(&p_pc))!=ENDTRAP)
					if (nexttype==NULLTOK)
						S_abort();
				break;
			case TIMEOUT:
				if ((nexttype=lexan(&p_pc))!=NUMBER)
					S_abort();
				if (tok_value.numval>=0){
					deadline = 0;
					if (tok_value.numval)
						deadline = time(0) + (tok_value.numval*60);
				}
				while ((nexttype=lexan(&p_pc))!=TERMINAT && nexttype!=NULLTOK)
					;
				break;
			case UNSET:
				if ((nexttype=lexan(&p_pc))!=VARNAME)
					S_abort();
				status = 1;
				unsetvar(tok_value.varptr->name);
				break;
			case UNTRAP:
				tvector = NIL(char);
				if ((nexttype=lexan(&p_pc))!=TERMINAT)
					S_abort();
				break;
			case XCSET:
				i = 0;
				line[0] = '\0';
				while (*p_pc!='\n')
					line[i++] = *(p_pc++);
				line[i] = '\0';
				lptr = line;
				s_set();
				break;
			case VARNAME:
				varptr1 = tok_value.varptr;
				if (!testing){
					if (varptr1->type==VCHAR || n!=(-1))
						S_abort();
					n = varptr1->u.num;
					continue;
				}
				status = S_varcmp(varptr1,&p_pc);
				if (negating)
					status = !status;
				break;
			case IF:
				if (nest_parse > PARSNEST){
					sprintf(Msg,"Nesting level too deep");
					S_abort();
				}
				status = S_parse(p_pc,THEN);
				DNP;
				if (status==TRUE){
					lexan(&intercom);
					p_pc = intercom;
					status = S_parse(p_pc,ENDIF);
					BCCHK(status)
					DNP;
					cptr = intercom;
					nexttype = lexan(&cptr);
					p_pc = cptr;
					if (nexttype==ELSE){
						counter = 0;
						while (TRUE){
							switch ((nexttype=lexan(&cptr))){
								case IF:
									++counter;
									continue;
								case ENDIF:
									if (counter){
										--counter;
										continue;
									}
									p_pc = cptr;
									break;
								case NULLTOK:
									S_abort();
								default:
									continue;
							}
							break;
						} /* now p_pc points to just after matching 'endif' */
					}
				}
				else {	/* intercom is now the THEN token */
					counter = 0;
					cptr = intercom;
					while (TRUE){
						switch ((nexttype=lexan(&cptr))){
							case IF:
								++counter;
								continue;
							case ELSE:
							case ENDIF:
								if (counter){
									--counter;
									continue;
								}
								p_pc = cptr;
								break;
							case NULLTOK:
								S_abort();
							default:
								continue;
						}
						break;
					}
					if (nexttype==ELSE){
						status = S_parse(p_pc,ENDIF);
						BCCHK(status)
						DNP;
						p_pc = intercom;
						lexan(&p_pc);
					}
					/* p_pc now points to just after ENDIF */
				}
				break;
			case WHILE:
				if (nest_parse > PARSNEST){
					sprintf(Msg,"Nesting level too deep");
					S_abort();
				}
				S_WHILE = p_pc;
				S_DO = S_DONE = NIL(char);
				while ((w_status=S_parse(S_WHILE,DO))==TRUE){
					DNP;
					--nest_while;
					if (!S_DO)
						lexan(&intercom),
						S_DO = intercom;
					status = S_parse(S_DO,DONE);
					DNP;
					--nest_while;
					if (status == SBREAK){
						status = 0;
						break;
					}					/* note SCONTNUE is automatic */
					if (!S_DONE && status!=SCONTNUE)
						lexan(&intercom),
						S_DONE = intercom;
				}
				if (S_DONE)
					p_pc = S_DONE;
				else {
					cptr = S_DO ? S_DO : S_WHILE;
					while (TRUE){
						switch ((nexttype=lexan(&cptr))){
							case WHILE:
								++counter;
								continue;
							case DONE:
								if (counter){
									--counter;
									continue;
								}
								p_pc = cptr;
								break;
							case NULLTOK:
								S_abort();
							default:
								continue;
						}
						break;
					}
					/* p_pc now points to just after matching 'done' */
				}
				if (w_status==FALSE)
					DNP;
				break;
		} /* end of main switch, whew */
		if (t_invoke==TERMINAT)
			return status;
		n = -1;
	}
}

/* load a script and call S_parse to run it */
static void 
S_call(scriptname)
char *scriptname;
{
	int i;
	jmp_buf senv;
	char *oldtvec, *newptr, *oldptr, script[SM_BUFF];
	long filesize;
	FILE *scriptfp;
	static struct stat statbuf;

	strcpy(script, scriptname);
	memset(Msg, 0, SM_BUFF);
	if (++nest_cmd>CMDNEST){
		S2("Too many nested scripts");
		--nest_cmd;
		return;
	}
	if (!(scriptfp = openfile(script))){
		sprintf(Msg,"Can't open '%s'",script);
		S2(Msg);
		--nest_cmd;
		return;
	}

	/* this succeeds, openfile() has called isregfile() to stat the file */
	fstat(fileno(scriptfp),&statbuf);
	filesize = statbuf.st_size;

	areas[nest_cmd - 1] = NIL(char);
	if (!(areas[nest_cmd-1]=(char*)calloc((unsigned)filesize+10,1))){
		sprintf(Msg,"%s: allocation error",script);
		S2(Msg);
		fclose(scriptfp);
		--nest_cmd;
		return;
	}
	if (strcmp(STARTUP,script))
		sprintf(Msg,"Running %s",script),
		S2(Msg);
	*(areas[nest_cmd - 1]) = '\n';
	fread((areas[nest_cmd - 1] + 1),filesize,1,scriptfp);
	*(areas[nest_cmd - 1] + filesize + 1) = '\0';
	fclose(scriptfp);

	oldtvec = tvector;
	tvector = NIL(char);
	newptr = (char *)senv;
	oldptr = (char *)env;
	for (i=0; i<sizeof(env); ++i)
		*(newptr++) = *(oldptr++);
	if ((i=setjmp(env))==0)
		S_parse(areas[nest_cmd - 1],NULLTOK);
	else if (i==TTERROR)
		S2("Abnormal script termination");
	else if (i==TIMEOUT)
		S2("Timeout");
	else if (i==EXIT)
		S2("Script encountered 'exit'");

	tvector = oldtvec;
	newptr = (char *)env;
	oldptr = (char *)senv;
	for (i=0; i<sizeof(env); ++i)
		*(newptr++) = *(oldptr++);
	--nest_cmd;
	if (areas[nest_cmd])
		free(areas[nest_cmd]);

	if (strcmp(STARTUP,script))
		sprintf(Msg,"%s COMPLETE",script),
		S2(Msg);
}

static 
S_abort()
{
	char *cptr;

	if (*Msg)
		S2(Msg);

	cptr = intercom;
	while (*cptr && *cptr!='\n')
		--cptr;

	++cptr;
	while (*cptr && *cptr!='\n')
		fputc(*(cptr++),tfp);
	fputc('\r',tfp),
	fputc('\n',tfp);

	if (tfp==cfp)
		tfp = savetfp;
	unsetall();
	longjmp(env,TTERROR);
}

/*	get_bound_char() get a character from the user's keyboard.  If the terminal
	mode escape character is seen, look ahead to the next character and act on
	it. If unrecognized, simply swallow the escape character and return the
	second character. Returns ASCII character code, or XCBIND function code
*/
get_bound_char()
{
	int			c, lc;
	binding_t	*ptr;
	static char	*emit_string = NIL(char);

	if (emit_string) {
		c = *(emit_string++);
		if (!c)
			emit_string = NIL(char);
	}

	if (!emit_string) {
		c = getchar();
		if (c == my_escape) {
			lc = tolower(c=getchar());
			for (ptr = first_binding; ptr; ptr = ptr->bs_next){
				if (ptr->bs_c == lc){
					switch (ptr->bs_function){
					case EMITSTR:
						emit_string = ptr->bs_string;
						return *(emit_string++);
					case DOSCRPT:
						strcpy(ddsname, ptr->bs_string);
						return DOSCRPT;
					default:
						return ptr->bs_function;
					}
				}
			}
		}
	}
	return c;
}

/*	default_bindings restores XC to its default key bindings.
	Uppercase keys are mapped to lower case.
*/
void 
default_bindings()
{
	bind_key('/', HLPCHAR, "");
	bind_key('?', HLPCHAR, "");
	bind_key('b', BRKCHAR, "");
	bind_key('d', DIALCHR, "");
	bind_key('f', DIVCHAR, "");
	bind_key('h', HUPCHAR, "");
	bind_key('n', CAPTEND, "");
	bind_key('q', QUITCHR, "");
	bind_key('s', SCRPCHR, "");
	bind_key('x', ENDCHAR, "");
	bind_key('y', CAPTYES, "");
}

/*	show_emit() performs an unctrl() operation on an entire buffer. */
static void 
show_emit(str)
char *str;
{
	while (*str)
		fprintf(tfp,"%s", unctrl(*str++));
}

/*	get_function() returns the description corresponding to the function code
	specified by (code).
*/
static char *
get_function(code)
int code;
{
	bindstr_t *ptr = function_list;
	int i;

	for (i = 0; i < FUNCTION_COUNT; i++, ptr++)
		if ((int) ptr->bf_function == code)
			return ptr->bf_string;

	return "???";
}

show_bindings()
{
	binding_t *ptr = first_binding;
	char *escape_str = strdup(unctrl(my_escape));
	int curline = 0;

	cls();

	fprintf(tfp,"\tTERMINAL mode escape character is %s.\r\n\r\n", escape_str);
	curline += 2;

	while (ptr){
		if (curline >= LI - 2){
			S0("PRESS ENTER");
			getline();
			cls();
			curline = 0;
		}

		fprintf(tfp,"\t%s - %-3.3s %s", escape_str, unctrl(ptr->bs_c),
			get_function(ptr->bs_function));

		switch (ptr->bs_function){
		case EMITSTR:
		case DOSCRPT:
			fputc(' ',tfp);
			fputc('"',tfp);
			show_emit(ptr->bs_string);
			fputc('"',tfp);
			break;
		}
		fputc('\r',tfp);
		fputc('\n',tfp);
		curline++;

		ptr = ptr->bs_next;
	}

	free(escape_str);
}

/*	find_function() returns the function code referenced by the name
	pointed to by (name).  Returns BADFUNC if the name is unrecognized.
*/
static 
bindfunc_t find_function(name)
char *name;
{
	bindstr_t *ptr = function_list;
	int i;

	for (i = 0; i < FUNCTION_COUNT; i++, ptr++)
		if (strcmp(ptr->bf_name, name) == 0)
			return ptr->bf_function;

	return BADFUNC;
}
