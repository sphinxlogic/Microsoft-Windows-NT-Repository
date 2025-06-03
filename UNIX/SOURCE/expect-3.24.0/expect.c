/* expect.c - expect and trap commands

Written by: Don Libes, NIST, 2/6/90

Design and implementation of this program was paid for by U.S. tax
dollars.  Therefore it is public domain.  However, the author and NIST
would appreciate credit if this program or parts of it are used.

$Revision: 1.1 $
$Date: 1992/02/21 19:05:45 $

*/

#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#ifndef NO_STRING_H
#include <string.h>
#else
char *strchr();
#endif
#include <varargs.h>
#include <errno.h>
#ifdef EXTERN_ERRNO
extern int errno;
#endif
#include <ctype.h>	/* for isspace */
#include <time.h>	/* for time(3) */
#include <setjmp.h>
#include <sys/wait.h>
#include "tcl.h"
extern char *tclRegexpError;	/* declared in tclInt.h */
#include "regexp.h"
#include "translate.h"
#include "global.h"
#include "command.h"

/* initial length of strings that we can guarantee patterns can match */
int default_match_max =		2000;
#define INIT_EXPECT_TIMEOUT	"10"	/* in seconds */

/* user variable names */
#define EXPECT_TIMEOUT		"timeout"
#define EXPECT_MATCH_MAX	"match_max"
#define EXPECT_OUT		"expect_out"
#define SPAWN_ID_ANY_VARNAME	"any_spawn_id"
#define SPAWN_ID_ANY_LIT	"-1"
#define SPAWN_ID_ANY		-1

int i_read_errno;	/* place to save errno, if i_read() == -1, so it
			   doesn't get overwritten before we get to read it */
static jmp_buf env;	/* for interruptable read() */
static int env_valid = FALSE;	/* whether we can longjmp or not */
static int timeout;	/* seconds */
#define TIME_INFINITY	-1

void lowmemcpy();

/*ARGSUSED*/
static void
sigalarm_handler(n)
int n;		       	/* unused, for compatibility with STDC */
{
#ifdef REARM_SIG
	signal(SIGALRM,sigalarm_handler);
#endif

	/* check env_valid first to protect us from the alarm occurring */
	/* in the window between i_read and alarm(0) */
	if (env_valid) longjmp(env,1);
}

/* upon interrupt, act like timeout */
/*ARGSUSED*/
static void
sigint_handler(n)
int n;			/* unused, for compatibility with STDC */
{
#ifdef REARM_SIG
	signal(SIGINT,sigint_handler);/* not nec. for BSD, but doesn't hurt */
#endif

	/* longjmp if we are executing a read inside of expect command */
	if (env_valid) longjmp(env,1);

	/* if anywhere else in code, prepare to exit */
	bye(0);
}

/* 1 ecase struct is reserved for each case in the expect command.  Note that
eof/timeout don't use any of theirs, but the algorithm is simpler this way. */

struct ecase {	/* case for expect command */
	int m;		/* master */
	int n;		/* number of patterns */
	char *pat;	/* original pattern spec */
	char **patn;	/* if multiple patterns in list */
	char *body;	/* ptr to body to be executed upon match */
#define PAT_EOF		1
#define PAT_TIMEOUT	2
#define PAT_DEFAULT	3
#define PAT_FULLBUFFER	4
#define PAT_GLOB	5 /* glob-style pattern list */
#define PAT_RE		6 /* regular expression */
	int use;	/* PAT_XXX */
	int glob_start;	/* start of string matched when use == PAT_GLOB */
	int transfer;	/* if false, leave matched chars in input stream */
#define CASE_UNKNOWN	0
#define CASE_NORM	1
#define CASE_LOWER	2
	int Case;	/* convert case before doing match? */
	regexp *re;	/* if this is 0, then pattern match via glob */
#define EXP_BEFORE	1
#define EXP_DURING	2
#define EXP_AFTER	3
	int owner;	/* one of the above */
};

struct expect_special
	before = {"expect_before ", NULL, 0, NULL, 0, EXP_BEFORE},
	after  = {"expect_after ",  NULL, 0, NULL, 0, EXP_AFTER};

/* remove nulls from s.  Initially, the number of chars in s is c, */
/* not strlen(s).  This count does not include the trailing null. */
/* returns number of nulls removed. */
int
rm_nulls(s,c)
char *s;
int c;
{
	char *s2 = s;	/* points to place in original string to put */
			/* next non-null character */
	int count = 0;
	int i;

	for (i=0;i<c;i++,s++) {
		if (0 == *s) {
			count++;
			continue;
		}
		if (count) *s2 = *s;
		s2++;
	}
	return(count);
}

/* generate printable versions of random ASCII strings.  This is used by */
/* cmdExpect when -d forces it to print strings it is examining. */
char *
printify(s)
char *s;
{
	static int destlen = 0;
	static char *dest = 0;
	char *d;		/* ptr into dest */
	unsigned int need;

	if (s == 0) return("<null>");

	/* worst case is every character takes 3 to printify */
	need = strlen(s)*3 + 1;
	if (need > destlen) {
		if (dest) free(dest);
		if (!(dest = malloc(need))) {
			destlen = 0;
			return("malloc failed in printify");
		}
		destlen = need;
	}

	for (d = dest;*s;s++) {
		if (*s == '\r') {
			strcpy(d,"\\r");		d += 2;
		} else if (*s == '\n') {
			strcpy(d,"\\n");		d += 2;
		} else if (*s == '\t') {
			strcpy(d,"\\t");		d += 2;
		} else if ((unsigned)*s < 0x20) { /* unsigned strips parity */
			sprintf(d,"\\C%c",*s + '`');	d += 3;
		} else if (*s == 0x7f) {
			/* not syntactically correct, but you get the point */
			strcpy(d,"\\7f");		d += 3;
		} else {
			*d = *s;			d += 1;
		}
	}
	*d = '\0';
	return(dest);
}

/* free up any argv structures in the ecases */
void
free_ecases(ecases,ecases_inuse,me)
struct ecase *ecases;
int ecases_inuse;
int me;
{
	int i;
	struct ecase *ec;

	for (ec=ecases,i=0;i<ecases_inuse;i++,ec++) {
		if (ec->re) free((char *)ec->re);
		/* individual elements of each list don't have to be freed */
		/* because SplitList allocates them all from single blocks! */
		if (ec->patn) free((char *)ec->patn);
		if (me != EXP_DURING) {
			if (ec->pat) free(ec->pat);
			if (ec->body) free(ec->body);
		}
	}
	if (ecases) free((char *)ecases);
}

/* no standard defn for this, and some systems don't even have if, so avoid */
/* the whole quagmire by calling it something else */
char *Strdup(s)
char *s;
{
	char *news = malloc(strlen(s) + 1);
	if (news) strcpy(news,s);
	return(news);
}

/* In many places, there is no need to malloc a copy of a string, since it */
/* will be freed before we return to Tcl */
int
save_str(lhs,rhs,me)
char **lhs;	/* left hand side */
char *rhs;	/* right hand side */
int me;
{
	if ((me == EXP_DURING) || (rhs == 0)) *lhs = rhs;
	else if (!(*lhs = Strdup(rhs))) {
		tcl_error("no space to save: %s",rhs);
		return(TCL_ERROR);
	}
	return(TCL_OK);
}

/* union together two arrays, ending up with unique array (b) */
void
union_arrays(a,b,acount,bcount)
int *a, *b;
int acount, *bcount;
{
	int i, j;

	for (i=0; i < acount ;i++) {
		/* check this one against all so far */
		for (j=0;j < *bcount;j++) {
			if (a[i] == b[j]) break;
		}
		/* if not found, add to array */
		if (j== *bcount) {
			b[j] = a[i];
			(*bcount)++;
		}
	}
}

/* called to execute a command of only one argument - a hack to commands */
/* to be called with all args surrounded by an outer set of braces */
/* returns TCL_whatever */
/*ARGSUSED*/
int
exp_eval_with_one_arg(clientData,argc,argv)
ClientData clientData;
int argc;
char **argv;
{
	char *buf;
	int rc;
	char *a;

	/* + 2 is for blank separating cmd and null at end */
	if (!(buf = malloc(strlen(argv[0]) + strlen(argv[1]) + 2))) {
		tcl_error("%s: no space to save arguments",argv[0]);
		return(TCL_ERROR);
	}
	/* replace top-level newlines with blanks */
	for (a=argv[1];*a;) {
		extern char *TclWordEnd();

		for (;isspace(*a);a++) {
			if (*a == '\n') *a = ' ';
		}
		a = TclWordEnd(a,0);
	}

	/* recreate statement */
	sprintf(buf,"%s %s",argv[0],argv[1]);

	rc = Tcl_Eval(interp,buf,0,(char **)NULL);
	free(buf);
	return(rc);

#if 0
	int i;
	int len = strlen(argv[0]) + strlen(argv[1]) + 2;

	for (i=0;i<len;i++) {
		if (buf[i] == '{' && just_saw_space) {
			for (;i<len;i++) {
				if (buf[i] == '}') break;
			}
		} else if (buf[i] == '[') {
			for (;i<len;i++) {
				if (buf[i] == ']') break;
			}
		} else if (buf[i] == '"' && just_saw_space) {
			for (;i<len;i++) {
				if (buf[i] == '"') break;
		} else {
			if (is_space(buf[i])) {
				int just_saw_space = TRUE;
				if (buf[i] == '\n') buf[i] = ' ';
			}
		} else just_saw_space = FALSE;
	}


	rc = Tcl_Eval(interp,buf,0,(char **)NULL);
	free(buf);
	return(rc);
#endif
}

extern int expectCD_user;
#define EXPECT_USER	(clientData == &expectCD_user)

/* parse the arguments to expect or it's variants */
/* returns TCL_OK or TCL_ERROR */
int
parse_expect_args(clientData,argc,argv,ecases,ecases_inuse,masters,mcount,me)
ClientData clientData;
int argc;
char **argv;
struct ecase **ecases;
int *ecases_inuse;
int **masters;		/* array of unique masters in ecases */
int *mcount;		/* count of masters */
int me;
{
	int size;
	int i;
	struct ecase *ec;
	int m;

#if 0
	char **oldargv = 0;	/* save original argv here if we split it */
#endif
	int case_master;	/* master to assign to next case as it is */
				/* being parsed */

	argv++;
	argc--;

	/* if we want to listen to user, use fd 0 */
	if (EXPECT_USER) m = 0;	/* true if we were called as expect_user */
	else {
/*		if (0 == update_master(&m)) return(TCL_ERROR);*/
		update_master(&m);	/* it'll be checked later, if used */
	}

	*ecases_inuse = (1+argc)/2;	/* estimate of number of patterns */

	/* This takes into account optional final action */
	/* If flags are used, this will be too high but it's not worth the */
	/* trouble of making two passes, so we'll just adjust the count */
	/* later when we find out the real amount */

	if (*ecases) free((char *)*ecases);	/* for before/after cases */
	if (*ecases_inuse) {
		if (0 == (*ecases = (struct ecase *)malloc(*ecases_inuse *
						sizeof(struct ecase)))) {
			tcl_error("malloc(%d ecases)",*ecases_inuse);
			goto error;
		}
	} else *ecases = NULL;

	/* zero them out so that just in case we get an error in the middle */
	/* of SplitList, we can deallocate them cleanly */
	for (i = 0, ec = *ecases;i<argc;i+=2,ec++) {
		ec->patn = 0;	/* necessary? - makes freeing code simpler? */
		ec->pat = 0;
		ec->body = 0;
		ec->transfer = 1;
		ec->re = 0;
		ec->Case = CASE_NORM;
		ec->owner = me;
		ec->use = PAT_GLOB;
	}

	/* forget old estimate of cases and prepare to calculate true number */
	*ecases_inuse = 0;
	ec = *ecases;
	case_master = m;
	for (i = 0;i<argc;i++) {
		if (streq(argv[i],"-n")) {
			ec->transfer = 0;
			continue;
		} else if (streq(argv[i],"-re")) {
			ec->use = PAT_RE;
			continue;
		} else if (streq(argv[i],"-nocase")) {
			ec->Case = CASE_LOWER;
			continue;
		} else if (streq(argv[i],"-i")) {
			i++;
			if (i>=argc) {
				tcl_error("-i requires following spawn_id");
				goto error;
			}
			case_master = atoi(argv[i]);
			continue;
		}

		ec->m = case_master;

		/* save original pattern spec */
		if (TCL_ERROR == save_str(&ec->pat,argv[i],me)) goto error;

		if (save_str(&ec->body,argv[i+1],me) == TCL_ERROR)
			goto error;

		if (streq(argv[i],"timeout")) {
			ec->use = PAT_TIMEOUT;
		} else if (streq(argv[i],"eof")) {
			ec->use = PAT_EOF;
		} else if (streq(argv[i],"full_buffer")) {
			ec->use = PAT_FULLBUFFER;
		} else if (streq(argv[i],"default")) {
			ec->use = PAT_DEFAULT;
		} else { /* pattern */
			if (ec->use == PAT_RE) {
			  ec->n = 1;
			  tclRegexpError = 0;
			  if (!(ec->re = regcomp(argv[i]))) {
				tcl_error("bad regular expression: %s",
							tclRegexpError);
				goto error;
			  }
		        } else {
			  ec->use = PAT_GLOB;
			  /* break up list into individual patterns */
			  
			  if (TCL_OK != Tcl_SplitList(interp,argv[i],
						&ec->n,&ec->patn)) {
				errorlog("%s\r\n",interp->result);
				tcl_error("expect*: failed to parse pattern: %s",argv[i]);
				goto error;
			  }
			}
		}
		i++; ec++; (*ecases_inuse)++;
	}

	/* build a list of masters for later use by ready() */
	size = *ecases_inuse;
	if (me == EXP_DURING) {
		size += 1 + before.mcount + after.mcount;
		/* "+1" just in case no patterns given, need placeholder */
		/* for default pattern */
	}
	if (0 == (*masters = (int *)malloc(size*sizeof(int)))) {
		tcl_error("malloc(%d spawn_id's)",size);
		goto error;
	}

	*mcount = 0;	/* initially empty */
	for (i=0,ec= *ecases; i < *ecases_inuse ;i++,ec++) {
		int j;

		if (ec->m == SPAWN_ID_ANY) continue;

		/* check this one against all so far */
		for (j=0;j < *mcount;j++) {
			if (ec->m == (*masters)[j]) break;
		}
		/* if not found, add to array */
		if (j== *mcount) {
			(*masters)[j] = ec->m;
			(*mcount)++;
		}
	}

	/* add before/after masters to candidates to pass to ready */
	if (me == EXP_DURING) {
		if (*mcount == 0) {
			/* if no patterns given, force pattern default */
			/* with current master */
			(*masters)[0] = m;
			(*mcount)++;
		}
		union_arrays(before.masters,*masters,before.mcount,mcount);
		union_arrays(after.masters,*masters,after.mcount,mcount);
	}

	return(TCL_OK);

 error:
	if (*ecases) free_ecases(*ecases,*ecases_inuse,me);
	*ecases = NULL;
	*ecases_inuse = 0;
#if 0
	/* if we split argv ourselves, cleanup */
	if (oldargv) free((char *)argv);
#endif
	return(TCL_ERROR);
}

#define EXP_EOF		0
#define EXP_ABEOF	-1	/* abnormal eof */
#define EXP_TIMEOUT	-2
#define EXP_TCLERROR	-3
#define EXP_FULLBUFFER	-5
#define EXP_MATCH	-6
#define EXP_NOMATCH	-7
#define EXP_IS_DEFAULT(x)	((x) == EXP_TIMEOUT || (x) == EXP_EOF)

static char yes[] = "yes\r\n";
static char no[] = "no\r\n";

struct eval_out {
	struct ecase *e;		/* ecase */
	struct f *f;
	char *buffer;
	int match;
};

/* like eval_cases, but handles only a single cases that needs a real */
/* string match */
/* returns EXP_X where X is MATCH, NOMATCH, FULLBUFFER, TCLERRROR */
int
eval_case_string(e,m,o,last_f,last_case)
struct ecase *e;
int m;
struct eval_out *o;		/* 'output' - i.e., final case of interest */
/* next two args are for debugging, when they change, reprint buffer */
struct f **last_f;
int *last_case;
{
	struct f *f = fs + m;
	char *buffer;

	if (!f->buffer) {
		/* not even initialized */
		/* even though no I/O has yet occurred, force a buffer to */
		/* exist so that "expect *" will succeed */
		if (TCL_ERROR == f_adjust(f)) return(EXP_TCLERROR);
#if 0
		return(EXP_NOMATCH);
#endif
	}

	/* if -nocase, use the lowerized buffer */
	buffer = ((e->Case == CASE_NORM)?f->buffer:f->lower);

	/* if master or case changed, redisplay debug-buffer */
	if ((f != *last_f) || e->Case != *last_case) {
		debuglog("expect: does {%s} (spawn_id %d) match pattern ",
				dprintify(buffer),f-fs);
		*last_f = f;
		*last_case = e->Case;
	}

	if (e->use == PAT_RE) {
		debuglog("{%s}? ",dprintify(e->pat));
		tclRegexpError = 0;
		if (buffer && regexec(e->re,buffer)) {
			o->e = e;
			o->match = e->re->endp[0]-buffer;
			o->buffer = buffer;
			o->f = f;
			debuglog(yes);
			return(EXP_MATCH);
		} else {
			debuglog(no);
			if (tclRegexpError) {
			    tcl_error("r.e. match failed: %s",tclRegexpError);
			    return(EXP_TCLERROR);
		        }
		    }
	} else if (e->use == PAT_GLOB) {
		int j;
		int match; /* # of chars that matched */

		for (j=0;j<e->n;j++) {
			/* skip place-holder for spawn_id with no pattern */
			if (e->patn[j][0] == '\0') continue;
		        debuglog("{%s}? ",dprintify(e->patn[j]));
			if (buffer && (-1 != (match = Exp_StringMatch(
					buffer,e->patn[j],&e->glob_start)))) {
				o->e = e;
				o->match = match;
				o->buffer = buffer;
				o->f = f;
				debuglog(yes);
				return(EXP_MATCH);
			} else debuglog(no);
		}
	} else if ((f->size == f->msize) && (f->size > 0)) {
		debuglog("%s? ",e->pat);
		o->e = e;
		o->match = f->umsize;
		o->buffer = f->buffer;
		o->f = f;
		return(EXP_FULLBUFFER);
	}
	return(EXP_NOMATCH);
}

/* sets o.e if successfully finds a matching pattern, eof, timeout or deflt */
/* returns original status arg or EXP_TCLERROR */
eval_cases(ecs_in,ecases_inuse,m,o,last_f,last_case,status,masters,mcount)
struct ecase *ecs_in;
int ecases_inuse;
int m;
struct eval_out *o;		/* 'output' - i.e., final case of interest */
/* next two args are for debugging, when they change, reprint buffer */
struct f **last_f;
int *last_case;
int status;
int *masters;
int mcount;
{
	int i;
	struct ecase *e;

	if (o->e || status == EXP_TCLERROR) return(status);

	if (status == EXP_TIMEOUT) {
		for (i=0, e=ecs_in;i<ecases_inuse;i++,e++) {
			if (e->use == PAT_TIMEOUT || e->use == PAT_DEFAULT) {
				o->e = e;
				break;
			}
		}
		return(status);
	} else if (status == EXP_EOF) {
		for (i=0, e=ecs_in;i<ecases_inuse;i++,e++) {
			if (e->use == PAT_EOF || e->use == PAT_DEFAULT) {
				if (e->m == SPAWN_ID_ANY || e->m == m) {
					o->e = e;
					break;
				}
			}
		}
		return(status);
	}

	/* the top loops are split from the bottom loop only because I can't */
	/* split'em further. */

	/* The bufferful condition does not prevent a pattern match from */
	/* occurring and vice versa, so it is scanned with patterns */
	for (i=0, e=ecs_in;i<ecases_inuse;i++,e++) {
		int j;

		if (e->use == PAT_TIMEOUT ||
		    e->use == PAT_DEFAULT ||
		    e->use == PAT_EOF) continue;

		/* if m == SPAWN_ID_ANY, then we have not yet read from any */
		/* master, so check every case against its master */
		/* if e->m == ditto, then user is explicitly asking every */
		/* case to be checked against every master */
		if (e->m == SPAWN_ID_ANY) {
			/* test against each spawn_id */
			for (j=0;j<mcount;j++) {
				status = eval_case_string(e,masters[j],o,last_f,last_case);
				if (status != EXP_NOMATCH) return(status);
			}
		} else if (m == SPAWN_ID_ANY) {
			/* test against its own spawn_id */
			status = eval_case_string(e,e->m,o,last_f,last_case);
			if (status != EXP_NOMATCH) return(status);
		} else {
			/* reject things immediately from wrong spawn_id */
			if (e->m != m) continue;

			status = eval_case_string(e,m,o,last_f,last_case);
			if (status != EXP_NOMATCH) return(status);
		}
	}
	return(EXP_NOMATCH);
}

/* This function handles the work of Expect_Before and After depending */
/* upon the first argument */
/*ARGSUSED*/
int
cmdExpectGlobal(clientData, interp, argc, argv)
ClientData clientData;
Tcl_Interp *interp;
int argc;
char **argv;
{
	struct expect_special *e;

	if (argc == 2 && strchr(argv[1],'\n')) {
		return(exp_eval_with_one_arg(clientData,argc,argv));
	}

	e = (struct expect_special *) clientData;
	return(parse_expect_args(clientData,argc,argv,&e->ecases,
		&e->ecount,&e->masters,&e->mcount,e->me));
}

/* adjusts file according to user's size request */
/* return TCL_ERROR or TCL_OK */
int
f_adjust(f)
struct f *f;
{
	int new_msize;
	char *new_buf;

	/* get the latest buffer size.  Double the user input for */
	/* two reasons.  1) Need twice the space in case the match */
	/* straddles two bufferfuls, 2) easier to hack the division */
	/* by two when shifting the buffers later on.  The extra  */
	/* byte in the malloc's is just space for a null we can slam on the */
	/* end.  It makes the logic easier later.  The -1 here is so that */
	/* requests actually come out to even/word boundaries (if user */
	/* gives "reasonable" requests) */
	new_msize = f->umsize*2 - 1;
	if (new_msize != f->msize) {
		if (!f->buffer) {
			/* allocate buffer space for 1st time */
			f->lower = malloc((unsigned)new_msize+1);
			f->buffer = malloc((unsigned)new_msize+1);
			if ((!f->buffer) && (!f->lower)) {
				tcl_error("out of space - failed to malloc initial match buffer");
				if (f->lower) {free(f->lower); f->lower = 0;}
				return(TCL_ERROR);
			}
			f->size = 0;
		} else {
			/* buffer already exists - resize */
			if (0 == (new_buf = realloc(f->buffer,new_msize+1))) {
				tcl_error("failed to grow match buf to %d bytes",f->umsize);
				return(TCL_ERROR);
			}
			f->buffer = new_buf;
			if (0 == (new_buf = realloc(f->lower,new_msize+1))) {
				tcl_error("failed to grow match buf to %d bytes",f->umsize);
				/* no need to free other buffer */
				/* - code will still work even if */
				/* buffer is larger than necessary */
				return(TCL_ERROR);
			}
			f->lower = new_buf;
			/* if truncated, forget about some data */
			if (f->size >= f->msize) f->size = f->msize-1;
		}
		f->msize = new_msize;
		f->buffer[f->size] = '\0';
	}
	return(TCL_OK);
}

/* these should really be local to i_read, however the longjmp could then */
/* clobber them */
#define cc i_read_cc
static int cc;
static int masters_max_out;

/* returns # of chars read or (non-positive) error of form EXP_XXX */
int
i_read(masters,masters_max,m,timeout)
int *masters;
int masters_max;
int *m;				/* new master */
int timeout;
{
	struct f *f;
	cc = EXP_TIMEOUT;
	masters_max_out = 1;	/* way to get address of 1 */

	if (1 != setjmp(env)) {
		env_valid = TRUE;

		/* convert fs into ready list */
		if (TCL_ERROR == ready(masters,masters_max,m,&masters_max_out,
								&timeout)) {
			return(EXP_TCLERROR);
		}
		if (masters_max_out == 0) return(EXP_TIMEOUT);	/* timed out */

		f = fs + *m;

		/* when buffer fills, copy second half over first and */
		/* continue, so we can do matches over multiple buffers */
		if (f->size == f->msize) {
			int half = f->size/2;
			memcpy(f->buffer,f->buffer+half,half);
			memcpy(f->lower, f->lower +half,half);
			f->size = half;
		}
		if (timeout) alarm(timeout);
#if MAJOR_DEBUGGING
debuglog("read(fd=%d,buffer=%x,length=%d)",
*m,f->buffer+f->size, f->msize-f->size);
#endif
		cc = read(*m,f->buffer+f->size, f->msize-f->size);
#if MAJOR_DEBUGGING
debuglog("= %d\r\n",cc);
#endif
		i_read_errno = errno;	/* errno can be overwritten by the */
					/* time we return */
		if (timeout) alarm(0);
	}
	/* setjmp returned, which means alarm went off or ^C pressed */
	env_valid = FALSE;
	return(cc);
}

/* variables predefined by expect are retrieved using this routine
which looks in the global space if they are not in the local space.
This allows the user to localize them if desired, and also to
avoid having to put "global" in procedure definitions.
*/
char *
get_var(var)
char *var;
{
	char *val;

	if (NULL != (val = Tcl_GetVar(interp,var,0 /* local */)))
		return(val);
	return(Tcl_GetVar(interp,var,TCL_GLOBAL_ONLY));
}

int
get_timeout()
{
	char *t;

	if (NULL != (t = get_var(EXPECT_TIMEOUT))) timeout = atoi(t);
	return(timeout);
}

/* unfinished_thoughts_on_SIGWINCH */
#if defined(SIGWINCH) && defined(TIOCGWINSZ)
static void
sigwinch_handler()
{
#if
	signal(SIGWINCH,sinwinch_handler);
#endif
	ioctl(dev_tty,TIOCSWINSZ,
#endif

void
init_expect()
{
	Tcl_SetVar(interp,EXPECT_TIMEOUT,	INIT_EXPECT_TIMEOUT,0);
	Tcl_SetVar(interp,SPAWN_ID_ANY_VARNAME,	SPAWN_ID_ANY_LIT,0);

	signal(SIGALRM,sigalarm_handler);
	signal(SIGINT,sigint_handler);
#if 0
#if defined(SIGWINCH) && defined(TIOCGWINSZ)
	signal(SIGWINCH,sinwinch_handler);
#endif
#endif
}

/*ARGSUSED*/
int
cmdExpect(clientData, interp, argc, argv)
ClientData clientData;
Tcl_Interp *interp;
int argc;
char **argv;
{
	int cc;			/* number of chars returned in a single read */
				/* or negative EXP_whatever */
	int oldrc;
	int m;			/* before doing an actual read, attempt */
				/* to match upon any spawn_id */
	struct f *f;		/* file associated with master */

	int i;			/* trusty temporary */
	struct ecase *ecases;
	int ecases_inuse;	/* number of ecases to use */
	int *masters;		/* array of masters to watch */
	int mcount;		/* number of masters to watch */

	struct eval_out eo;	/* final case of interest */

	int result;		/* Tcl result */

	time_t start_time_total;/* time at beginning of this procedure */
	time_t start_time = 0;	/* time when restart label hit */
	time_t current_time = 0;/* current time (when we last looked)*/
	time_t end_time;	/* future time at which to give up */
	time_t elapsed_time_total;/* time from now to match/fail/timeout */
	time_t elapsed_time;	/* time from restart to (ditto) */

	struct f *last_f;	/* for differentiating when multiple f's */
				/* to print out better debugging messages */
	int last_case;		/* as above but for case */
	int do_flush;		/* if anything queued up, should be flushed */
	int first_time = 1;	/* if not "restarted" */

	if (argc == 2 && strchr(argv[1],'\n')) {
		return(exp_eval_with_one_arg(clientData,argc,argv));
	}

	time(&start_time_total);
	start_time = start_time_total;
 restart:
	if (first_time) first_time = 0;
	else time(&start_time);

	cc = EXP_NOMATCH;
	m = SPAWN_ID_ANY;
	ecases = 0;
	masters = 0;
	mcount = 0;
	result = TCL_OK;
	last_f = 0;
	do_flush = FALSE;
	/* end of restart code */

	eo.e = 0;		/* no final case yet */
	eo.f = 0;		/* no final file selected yet */
	eo.match = 0;		/* nothing matched yet */

	/* get the latest timeout */
	(void) get_timeout();

	/* make arg list for processing cases */
	/* do it dynamically, since expect can be called recursively */
	if (TCL_ERROR == parse_expect_args(clientData,argc,argv,
			&ecases,&ecases_inuse,
			&masters,&mcount,
			EXP_DURING)) return(TCL_ERROR);

	/* if anything queued up unwritten (from interact), flush it */
	for (i=0;i<mcount;i++) {
		int write_count;

		if (!(f = fd_to_f(masters[i],"expect"))) return(TCL_ERROR);
		if (!f->buffer) continue;

		if (f->size) write_count = f->size - f->printed;
		else write_count = 0;

		if (write_count) {
			do_flush = TRUE;

			/* This logic is identical to what happens */
			/* after a read() below */

			if (logfile_all || (loguser && logfile)) {
				fwrite(f->buffer + f->printed,1,write_count,
								logfile);
			}
			/* don't write to user if they're seeing it already, */
			/* that is, typing it! */
			if (loguser && !is_user(m)) fwrite(f->buffer + f->printed,
						1,write_count,stdout);
			if (debugfile) fwrite(f->buffer + f->printed,
						1,write_count,debugfile);
			f->printed = 0;
		}
	}

	/* if we wrote to any logs, flush them */
	if (do_flush && ((loguser && (logfile || !is_user(m)))
			    || debugfile
			    || logfile_all)) {
		flush_streams();
	}

	/* code works even if ==, however this avoids pointless system call */
	if (timeout != TIME_INFINITY) {
		time(&current_time);
		end_time = current_time + timeout;
	}

	for (;;) {
	     /* if first time thru, try to match against */
	     /* old buffered data before reading more */
	    if (m != SPAWN_ID_ANY) {
		/* Ignore Purify errors about following line.  They errors */
		/* are correct but don't hurt the program.  And the code is */
		/* easier to read this way. */
		int remtime = end_time - current_time;  /* max time to read */
		/* This accounts for systems which wait in ready, and */
		/* ancient ones which can't, and have to wait in read() */
	
		if (timeout == TIME_INFINITY || remtime > 0) {
			if (timeout == TIME_INFINITY) remtime = -1;
			cc = i_read(masters,mcount,&m,remtime);
		} else {
			cc = EXP_TIMEOUT;
		}

		if (cc == EXP_EOF) {		/* normal EOF */
		} else if (cc == EXP_ABEOF) {	/* abnormal EOF */
			/* On many systems, ptys produce EIO upon EOF - sigh */
			if (i_read_errno == EIO) {
				/* Sun, Cray, BSD, and others */
				cc = EXP_EOF;
			} else if (i_read_errno == 0) {
				/* AIX */
				cc = EXP_EOF;
			} else {
				if (i_read_errno == EBADF) {
					tcl_error("bad spawn_id (process died earlier?)");
				} else {
					tcl_error("i_read(spawn_id=%d): %s",
						m,sys_errlist[errno]);
					fd_close(m);
				}
				goto error;
			}
		} else if (cc == EXP_TIMEOUT) {
			debuglog("expect: timed out\r\n");
		} else if (cc == EXP_TCLERROR) {
			goto error;
		} else {
			/* got data! */

			f = fs + m;
			oldrc = f->size;
			f->size += cc;

			if (logfile_all || (loguser && logfile)) {
				fwrite(f->buffer + oldrc,1,cc,logfile);
			}
			/* don't write to user if they are seeing it */
			/* already, that is, typing it! */
			if (loguser && !is_user(m))
				fwrite(f->buffer + oldrc,1,cc,stdout);
			if (debugfile)
				fwrite(f->buffer + oldrc,1,cc,debugfile);

			/* if we wrote to any logs, flush them */
			if (   (loguser && (logfile || !is_user(m)))
			    || debugfile
			    || logfile_all) {
				flush_streams();
			}

			/* remove nulls from input, since there is no way */
			/* for Tcl to deal with such strings.  Doing it here */
			/* lets them be sent to the screen, just in case */
			/* they are involved in formatting operations */
			f->size -= rm_nulls(f->buffer + oldrc,cc);
			f->printed = f->size; /* count'm even if not logging */
			f->buffer[f->size] = '\0';
			/* below here, cc as general status */
			cc = EXP_NOMATCH;

			/* copy to lowercase buffer */
			lowmemcpy(f->lower+oldrc,f->buffer+oldrc,1 + f->size - oldrc);

			/* force redisplay of buffer when debugging */
			last_f = 0;
		}
	     }

	     cc = eval_cases(before.ecases,before.ecount,
			m,&eo,&last_f,&last_case,cc,masters,mcount);
	     cc = eval_cases(ecases,ecases_inuse,
			m,&eo,&last_f,&last_case,cc,masters,mcount);
	     cc = eval_cases(after.ecases,after.ecount,
			m,&eo,&last_f,&last_case,cc,masters,mcount);
	     if (cc == EXP_TCLERROR) goto error;
	     /* special eof code that cannot be done in eval_cases */
	     /* or above, because it would then be executed several times */
	     if (cc == EXP_EOF) {
			eo.f = fs + m;
			eo.match = eo.f->size;
			eo.buffer = eo.f->buffer;
			debuglog("expect: read eof\r\n");
			break;
	     } else if (cc == EXP_TIMEOUT) break;
	     /* break out if timeout or eof and failed to find a case for it */

	     if (eo.e) break;

	     /* if we've not yet actually read anything, don't update time */
	     /* this forces at least one read to be done if timeout > 0 */
	     if (m != SPAWN_ID_ANY)
		if (timeout != TIME_INFINITY) time(&current_time);

	     m = 0; /* as long as we assign m any non-SPAWN_ID_ANY value */
		/* we will be assured that the next time through the loop */
		/* it will indeed try to ready from all the masters */
		/* At that time, m will be given a new value */
	}

	goto done;

error:
	result = TCL_ERROR;
 done:
#define out(i,val)  debuglog("expect: set %s(%s) {%s}\r\n",EXPECT_OUT,i, \
						dprintify(val)); \
		    if (!Tcl_SetVar2(interp,EXPECT_OUT,i,val,0)) \
				{result = TCL_ERROR; goto cleanup;}
	{
		char value[20];

		time(&current_time);
		elapsed_time = current_time - start_time;
		elapsed_time_total = current_time - start_time_total;
		sprintf(value,"%d",elapsed_time);
		out("seconds",value);
		sprintf(value,"%d",elapsed_time_total);
		out("seconds_total",value);
	}

	if (result != TCL_ERROR) {
		char *body = 0;
		char *buffer;	/* pointer to normal or lowercased data */
		struct ecase *e = 0;	/* points to current ecase */
		int match = -1;		/* characters matched */
		char match_char;	/* place to hold char temporarily */
					/* uprooted by a NULL */

		if (eo.e) {
			e = eo.e;
			body = e->body;
			if (e->use != PAT_TIMEOUT) {
				f = eo.f;
				match = eo.match;
				buffer = eo.buffer;
			}
		} else if (cc == EXP_EOF) {
			/* read an eof but no user-supplied case */
			f = eo.f;
			match = eo.match;
			buffer = eo.buffer;
		}			

		if (match >= 0) {
			char name[20], value[20];
			char *str;

			if (e && e->use == PAT_RE) {
				for (i=0;i<NSUBEXP;i++) {
					int offset;

					if (e->re->startp[i] == 0) break;

					/* start index */
					sprintf(name,"%d,start",i);
					offset = e->re->startp[i]-buffer;
					sprintf(value,"%d",offset);
					out(name,value);

					/* end index */
					sprintf(name,"%d,end",i);
					sprintf(value,"%d",
						e->re->endp[i]-buffer-1);
					out(name,value);

					/* string itself */
					sprintf(name,"%d,string",i);
					str = f->buffer + offset;
					/* temporarily null-terminate in */
					/* middle */
					match_char = *e->re->endp[i];
					*e->re->endp[i] = 0;
					out(name,e->re->startp[i]);
					*e->re->endp[i] = match_char;
				}
				/* redefine length of string that */
				/* matched for later extraction */
				match = e->re->endp[0]-buffer;
			} else if (e && e->use == PAT_GLOB) {
				/* start index */
				sprintf(value,"%d",e->glob_start);
				out("0,start",value);

				/* end index */
				sprintf(value,"%d",e->glob_start + match - 1);
				out("0,end",value);

				/* string itself */
				str = f->buffer + e->glob_start;
				/* temporarily null-terminate in middle */
				match_char = str[match];
				str[match] = 0;
				out("0,string",str);
				str[match] = match_char;

				/* redefine length of string that */
				/* matched for later extraction */
				match += e->glob_start;
			} else if (e && e->use == PAT_FULLBUFFER) {
				debuglog("expect: full buffer\r\n");
			}
		}

		/* this is broken out of (match > 0) (above) since it can */
		/* that an EOF occurred with match == 0 */
		if (eo.f) {
			char spawn_id[10];	/* enough for a %d */

			sprintf(spawn_id,"%d",f-fs);
			out("spawn_id",spawn_id);

			/* save buf[0..match] */
			/* temporarily null-terminate string in middle */
			match_char = f->buffer[match];
			f->buffer[match] = 0;
			out("buffer",f->buffer);
			/* remove middle-null-terminator */
			f->buffer[match] = match_char;

			/* "!e" means no case matched - transfer by default */
			if (!e || e->transfer) {
				/* delete matched chars from input buffer */
				f->size -= match;
				f->printed -= match;
				if (f->size != 0) {
				   memcpy(f->buffer,f->buffer+match,f->size);
				   memcpy(f->lower,f->lower+match,f->size);
				}
				f->buffer[f->size] = '\0';
				f->lower[f->size] = '\0';
			}

			if (cc == EXP_EOF) fd_close(f - fs);

		}

		if (body) result = Tcl_Eval(interp,body,0,(char **) NULL);
	}

 cleanup:
	free_ecases(ecases,ecases_inuse,EXP_DURING);
	if (masters) free((char *)masters);

	if (result == TCL_CONTINUE_EXPECT) {
		debuglog("expect: continuing expect\r\n");
		goto restart;
	}

	return(result);
}

/* lowmemcpy - like memcpy but it lowercases result */
void
lowmemcpy(dest,src,n)
char *dest;
char *src;
int n;
{
	for (;n>0;n--) {
		*dest = (isupper(*src)?tolower(*src):*src);
		src++;	dest++;
	}
}

/* The following functions implement expect's glob-style string matching */
/* Exp_StringMatch allow's implements the unanchored front (or conversely */
/* the '^') feature.  Exp_StringMatch2 does the rest of the work. */
int	/* returns # of chars that matched */
Exp_StringMatch(string, pattern,offset)
char *string;
char *pattern;
int *offset;	/* offset from beginning of string where pattern matches */
{
	char *s;
	int sm;	/* count of chars matched or -1 */
	int caret = FALSE;

	*offset = 0;

	if (pattern[0] == '^') {
		caret = TRUE;
		pattern++;
	}

	sm = Exp_StringMatch2(string,pattern);
	if (sm >= 0) return(sm);

	if (caret) return(-1);

	if (pattern[0] == '*') return(-1);

	for (s = string;*s;s++) {
 		sm = Exp_StringMatch2(s,pattern);
		if (sm != -1) {
			*offset = s-string;
			return(sm);
		}
	}
	return(-1);
}

/* Exp_StringMatch2 --

Like Tcl_StringMatch except that
1) returns number of characters matched, -1 if failed.
	(Can return 0 on patterns like "" or "$")
2) does not require pattern to match to end of string
3) Original code is stolen from Tcl_StringMatch
*/

int Exp_StringMatch2(string,pattern)
    register char *string;	/* String. */
    register char *pattern;	/* Pattern, which may contain
				 * special characters. */
{
    char c2;
    int match = 0;	/* # of chars matched */

    while (1) {
	/* See if we're at the end of both the pattern and the string.
	 * If so, we succeeded.  If we're at the end of the pattern
	 * but not at the end of the string, we failed.
	 */
	
	if (*pattern == 0) {
		/* removed test for end of string - DEL */
		return match;
	}

	if ((*string == 0) && (*pattern != '*')) {
	    return -1;
	}

	/* Check for a "*" as the next pattern character.  It matches
	 * any substring.  We handle this by calling ourselves
	 * recursively for each postfix of string, until either we
	 * match or we reach the end of the string.
	 */
	
	if (*pattern == '*') {
	    pattern += 1;
	    if (*pattern == 0) {
		return(strlen(string)+match); /* DEL */
	    }
	    while (*string != 0) {
		int rc;					/* DEL */

		if (-1 != (rc = Exp_StringMatch2(string, pattern))) {
		    return rc+match;		/* DEL */
		}
		string += 1;
		match++;				/* DEL */
	    }
	    return -1;					/* DEL */
	}
    
	/* Check for a "?" as the next pattern character.  It matches
	 * any single character.
	 */

	if (*pattern == '?') {
	    goto thisCharOK;
	}

	/* Check for a "[" as the next pattern character.  It is followed
	 * by a list of characters that are acceptable, or by a range
	 * (two characters separated by "-").
	 */
	
	if (*pattern == '[') {
	    pattern += 1;
	    while (1) {
		if ((*pattern == ']') || (*pattern == 0)) {
		    return 0;
		}
		if (*pattern == *string) {
		    break;
		}
		if (pattern[1] == '-') {
		    c2 = pattern[2];
		    if (c2 == 0) {
			return -1;		/* DEL */
		    }
		    if ((*pattern <= *string) && (c2 >= *string)) {
			break;
		    }
		    if ((*pattern >= *string) && (c2 <= *string)) {
			break;
		    }
		    pattern += 2;
		}
		pattern += 1;
	    }
	    while ((*pattern != ']') && (*pattern != 0)) {
		pattern += 1;
	    }
	    goto thisCharOK;
	}
    
	/* If the last pattern character is '$', verify that the entire
	 * string has been matched. - DEL 
	 */

	if ((*pattern == '$') && (pattern[1] == 0)) {
		if (*string == 0) return(0);
		else return(-1);		
	}

	/* If the next pattern character is '/', just strip off the '/'
	 * so we do exact matching on the character that follows.
	 */
	
	if (*pattern == '\\') {
	    pattern += 1;
	    if (*pattern == 0) {
		return -1;
	    }
	}

	/* There's no special character.  Just make sure that the next
	 * characters of each string match.
	 */
	
	if (*pattern != *string) {
	    return -1;
	}

	thisCharOK: pattern += 1;
	string += 1;
	match++;
    }
}


/* Tcl statements to execute upon various signals */
/* Each is handled by the same "generic_sighandler (below)" which */
/* looks them up here */
struct {	/* one per signal */
	char *action;		/* Tcl command to execute upon sig */
	char *name;		/* name of C macro */
} signals[NSIG];

void
init_trap()
{
	int i;

	for (i=0;i<NSIG;i++) {
		signals[i].name = 0;
		signals[i].action = 0;
	}

	/* defined by C standard */
#if defined(SIGABRT)
	/* unbelievable but some systems don't support this (e.g. SunOS 3.5) */
	signals[SIGABRT].name = "SIGABRT";
#endif
	signals[SIGFPE ].name = "SIGFPE";
	signals[SIGILL ].name = "SIGILL";
	signals[SIGINT ].name = "SIGINT";
	signals[SIGSEGV].name = "SIGSEGV";
	signals[SIGTERM].name = "SIGTERM";

	/* our own extension */
	signals[0].name = "ONEXIT";

	/* nonstandard but common */
#if defined(SIGHUP)		/* hangup */
	signals[SIGHUP ].name = "SIGHUP";
#endif
#if defined(SIGQUIT)		/* quit */
	signals[SIGQUIT].name = "SIGQUIT";
#endif
#if defined(SIGTRAP)		/* trace trap (not reset when caught) */
	signals[SIGTRAP].name = "SIGTRAP";
#endif
#if defined(SIGIOT)		/* IOT instruction */
	signals[SIGIOT ].name = "SIGIOT";
#endif
#if defined(SIGEMT)		/* EMT instruction */
	signals[SIGEMT ].name = "SIGEMT";
#endif
#if defined(SIGKILL)		/* kill (cannot be caught or ignored) */
	signals[SIGKILL].name = "SIGKILL";
#endif
#if defined(SIGBUS)		/* bus error */
	signals[SIGBUS ].name = "SIGBUS";
#endif
#if defined(SIGSYS)		/* bad argument to system call */
	signals[SIGSYS ].name = "SIGSYS";
#endif
#if defined(SIGPIPE)		/* write on a pipe with no one to read it */
	signals[SIGPIPE].name = "SIGPIPE";
#endif
#if defined(SIGALRM)		/* alarm clock */
	signals[SIGALRM].name = "*SIGALRM";
#endif
#if defined(SIGCLD)		/* Like SIGCHLD.  */
	signals[SIGCLD ].name = "SIGCLD";
#endif
#if defined(SIGPWR)		/* imminent power failure */
	signals[SIGPWR ].name = "SIGPWR";
#endif
#if defined(SIGPOLL)		/* For keyboard input?  */
	signals[SIGPOLL].name = "SIGPOLL";
#endif
#if defined(SIGURG)		/* urgent condition on IO channel */
	signals[SIGURG ].name = "SIGURG";
#endif
#if defined(SIGSTOP)		/* sendable stop signal not from tty */
	signals[SIGSTOP].name = "SIGSTOP";
#endif
#if defined(SIGTSTP)		/* stop signal from tty */
	signals[SIGTSTP].name = "SIGTSTP";
#endif
#if defined(SIGCONT)		/* continue a stopped process */
	signals[SIGCONT].name = "SIGCONT";
#endif
#if defined(SIGCHLD)		/* to parent on child stop or exit */
	signals[SIGCHLD].name = "SIGCHLD";
#endif
#if defined(SIGTTIN)		/* to readers pgrp upon background tty read */
	signals[SIGTTIN].name = "SIGTTIN";
#endif
#if defined(SIGTTOU)		/* like TTIN for output if (tp->t_local&LTOSTOP) */
	signals[SIGTTOU].name = "SIGTTOU";
#endif
#if defined(SIGIO)		/* input/output signal */
	signals[SIGIO  ].name = "SIGIO";
#endif
#if defined(SIGXCPU)		/* exceeded CPU time limit */
	signals[SIGXCPU].name = "SIGXCPU";
#endif
#if defined (SIGXFSZ)		/* exceeded file size limit */
	signals[SIGXFSZ].name = "SIGXFSZ";
#endif
#if defined(SIGVTALRM)		/* virtual time alarm */
	signals[SIGVTALRM].name = "SIGVTALRM";
#endif
#if defined(SIGPROF)		/* profiling time alarm */
	signals[SIGPROF].name = "SIGPROF";
#endif
#if defined(SIGWINCH)		/* window changed */
	signals[SIGWINCH].name = "SIGWINCH";
#endif
#if defined(SIGLOST)		/* resource lost (eg, record-lock lost) */
	signals[SIGLOST].name = "SIGLOST";
#endif
#if defined(SIGUSR1)		/* user defined signal 1 */
	signals[SIGUSR1].name = "SIGUSR1";
#endif
#if defined(SIGUSR2)		/* user defined signal 2 */
	signals[SIGUSR2].name = "SIGUSR2";
#endif

#if 0
#ifdef HPUX
	/* initially forced to catch & discard SIGCLD to collect wait status */
	(void) Tcl_Eval(interp,"trap SIG_DFL SIGCHLD",0,(char **)0);
	/* no point in checking for errors here, since it is so early on */
	/* something else will be sure to fail before application begins */

	/* note that SIGCHLD is used rather than SIGCLD since HPUX defines */
	/* them both, but expect can only handle one, and it handles the */
	/* "wrong" one, first */
#endif
#endif
}

/* reserved to us if name begins with asterisk */
#define SIG_RESERVED(x)	(signals[x].name[0] == '*')

char *
signal_to_string(sig)
int sig;
{
	if (sig < 0 || sig > NSIG) {
		return("SIGNAL OUT OF RANGE");
	} else if (!signals[sig].name) {
		return("SIGNAL UNKNOWN");
	} else return(signals[sig].name + SIG_RESERVED(sig));
}

void
print_signal(sig)
int sig;
{
	if (signals[sig].action) Log(0,"%s (%d): %s\r\n",
		signal_to_string(sig),sig,signals[sig].action);
}

/* given signal index or name as string, */
/* returns signal index or -1 if bad arg */
int
string_to_signal(s)
char *s;
{
	int sig;
	char *name;

	/* try interpreting as an integer */
	if (1 == sscanf(s,"%d",&sig)) return(sig);

	/* try interpreting as a string */
	for (sig=0;sig<NSIG;sig++) {
		name = signals[sig].name;
		if (SIG_RESERVED(sig)) name++;
		if (streq(s,name) || streq(s,name+3))
			return(sig);
	}
	return(-1);
}

/* called upon receipt of a user-declared signal */
void
generic_sighandler(sig)
int sig;
{
	int proc_valid = TRUE;

	debuglog("generic_sighandler: handling signal(%d)\r\n",sig);

	if (sig < 0 || sig >= NSIG) {
		errorlog("caught impossible signal\r\n",sig);
	} else if (!signals[sig].action) {
		/* In this one case, we let ourselves be called when no */
		/* signaler predefined, since we are calling explicitly */
		/* from another part of the program, and it is just simpler */
		if (sig == 0) return;
		errorlog("caught unexpected signal: %s (%d)\r\n",
			signal_to_string(sig),sig);
	} else {
		int rc;

#ifdef REARM_SIG
#ifdef SYSV3
		/* assume no wait() occurs between SIGCLD and */
		/* this code */
		if (sig == SIGCLD) {
			int i, pid;
			int status;
			extern int fd_max;

			pid = wait(&status);
			for (i=0;i<=fd_max;i++) {
				if (fs[i].pid == pid) break;
			}
			if (i>fd_max || !(fs[i].flags & FD_VALID)) {
				debuglog("received SIGCLD from unknown pid %d\r\n",pid);
				proc_valid = FALSE;
			} else {
				fs[i].flags |= FD_SYSWAITED;
				fs[i].wait = status;
			}
		}
#endif
		if (sig != 0) signal(sig,generic_sighandler);
#endif

		debuglog("generic_sighandler: Tcl_Eval(%s)\r\n",signals[sig].action);
		if (proc_valid) {
		  rc = Tcl_Eval(interp,signals[sig].action,0,(char **)0);
		  if (rc != TCL_OK) {
		    errorlog("caught %s (%d): error in command: %s\r\n",
			signal_to_string(sig),sig,signals[sig].action);
		    if (rc != TCL_ERROR) errorlog("Tcl_Eval = %d\r\n",rc);
		    if (*interp->result != 0) {
			errorlog("%s\r\n",interp->result);
		    }
	          }
		}
	}

	/* if we are doing an i_read, restart it */
	if (env_valid) longjmp(env,2);
}

/* reset signal to default */
void
sig_reset(sig)
int sig;
{
#ifdef SIG_FN_RETURN
	SIG_FN_RETURN (*default_proc)();
#else
	void (*default_proc)();
#endif

	signals[sig].action = 0;  /* should've been free'd by now if nec. */

	/* SIGINT defaults to timeout/exit routine */
	/* Ultrix 1.3 compiler can't handle this */
	/* default_proc = (sig == SIGINT?sigint_handler:SIG_DFL);*/
	if (sig == SIGINT) default_proc = sigint_handler;
	else default_proc = SIG_DFL;

	signal(sig,default_proc);
}

/*ARGSUSED*/
int
cmdMatchMax(clientData,interp,argc,argv)
ClientData clientData;
Tcl_Interp *interp;
int argc;
char **argv;
{
	int size = -1;
	int m;
	struct f *f;
	char buf[30];	/* big enough for an int, eh? */
	int Default = FALSE;

	argc--; argv++;

	if ((argc >= 1) && streq(argv[0],"-d")) Default = TRUE;
	else {
		if (!(f = update_master(&m))) return(TCL_ERROR);
	}

	if (argc == 0) {
		size = f->umsize;
	} else if ((argc == 1) && Default) {
		size = default_match_max;
	}
	if (size >= 0) {
		sprintf(buf,"%d",size);
		Tcl_Return(interp,buf,TCL_VOLATILE);
		return(TCL_OK);
	}

	/* all that's left is to set the size */
	size = atoi(argv[argc-1]);
	if (size <= 0) {
		tcl_error("%s must be positive",EXPECT_MATCH_MAX);
		return(TCL_ERROR);
	}

	if (Default) default_match_max = size;
	else f->umsize = size;

	return(TCL_OK);
}

/* following is only used as arg to tcl_error */
char trap_usage[] = "usage: trap [[arg] {list of signals}]";

/*ARGSUSED*/
int
cmdTrap(clientData, interp, argc, argv)
ClientData clientData;
Tcl_Interp *interp;
int argc;
char **argv;
{
	char *action = 0;
	int n;		/* number of signals in list */
	char **list;	/* list of signals */
	int len;	/* length of action */
	int i;
	int rc = TCL_OK;

	if (argc > 3) {
		tcl_error(trap_usage);
		return(TCL_ERROR);
	}

	if (argc == 1) {
		for (i=0;i<NSIG;i++) if (signals[i].action) print_signal(i);
		return(TCL_OK);
	}

	if (argc == 3) action = argv[1];

	/* argv[argc-1] is the list of signals */
	/* first extract it */
	if (TCL_OK != Tcl_SplitList(interp,argv[argc-1],&n,&list)) {
		errorlog("%s\r\n",interp->result);
		tcl_error(trap_usage);
		return(TCL_ERROR);
	}

	for (i=0;i<n;i++) {
		int sig = string_to_signal(list[i]);
		if (sig < 0 || sig >= NSIG) {
			tcl_error("trap: invalid signal %s",list[i]);
			rc = TCL_ERROR;
			break;
		}

		if (!action) action = "SIG_DFL";
/* {
			print_signal(sig);
			continue;
		}
*/

#if 0
#ifdef HPUX
		if (sig == SIGCLD && streq(action,"SIG_DFL")) {
			action = "";
		}
#endif
#endif

		if (sig == SIGALRM) {
			/* SIGALRM reserved to us, for expect command */
			tcl_error("trap: cannot trap SIGALRM (%d)",SIGALRM);
			rc = TCL_ERROR;
			break;
		}

		debuglog("trap: setting up signal %d (\"%s\")\r\n",sig,list[i]);

		if (signals[sig].action) free(signals[sig].action);

		if (streq(action,"SIG_DFL")) {
			if (sig != 0) sig_reset(sig);
		} else {
			len = 1 + strlen(action);
			if (0 == (signals[sig].action = malloc(len))) {
				tcl_error("trap: malloc failed");
				if (sig != 0) sig_reset(sig);
				rc = TCL_ERROR;
				break;
			}
			memcpy(signals[sig].action,action,len);
			if (sig == 0) continue;
			if (streq(action,"SIG_IGN")) {
				signal(sig,SIG_IGN);
			} else signal(sig,generic_sighandler);
		}
	}
	free((char *)list);
	return(rc);
}
