/* global.h - global definitions

Written by: Don Libes, NIST, 2/6/90

Design and implementation of this program was paid for by U.S. tax
dollars.  Therefore it is public domain.  However, the author and NIST
would appreciate credit if this program or parts of it are used.
*/

#define FALSE 0
#define TRUE 1

extern char *sys_errlist[];
extern int errno;

/* if you get errors from the compiler here, add -DNOSTDLIB to Makefile */
#ifdef NOSTDLIB
	char* malloc();
	char* realloc();
#ifndef _TCLINT
	void exit();
#endif
#else
#include <stdlib.h>
#endif

/* yes, I have a weak mind */
#define streq(x,y)	(0 == strcmp((x),(y)))

#ifdef NO_MEMCPY
#define memcpy(x,y,len) bcopy(y,x,len)
#endif

#ifndef TERM
#  ifdef POSIX
#    define TERM termios
#  endif
#endif

#ifndef TERM
#  ifdef SYSV3
#    ifdef HPUX
#      define TERM termio
#    else
#      define TERM termios
#    endif
#  else
#      define TERM sgttyb
#  endif
#endif

typedef struct TERM exp_tty;

char *cook();
char *printify();
#define dprintify(x)	((is_debugging || debugfile)?printify(x):0)
/* in circumstances where "debuglog(printify(...))" is written, call */
/* dprintify instead.  This will avoid doing any formatting that would */
/* occur before debuglog got control and decided not to do anything */
/* because (is_debugging || debugfile) was false. */

void Log();
void errorlog();
void exp_debuglog();
void nflog();
void nferrorlog();
void tty_raw();
void tty_echo();
void tty_set();
void flush_streams();
void bye();

extern Tcl_Interp *interp;
extern FILE *cmdfile;
extern FILE *debugfile;
extern FILE *logfile;
extern int logfile_all;
extern int loguser;

extern int is_debugging;	/* useful to know for avoid debug calls */

/* yet more TCL return codes */
/* Tcl does not safely provide a way to define the values of these, so */
/* use ridiculously numbers for safety */
#define TCL_CONTINUE_EXPECT	-101	/* continue expect command itself */
#define TCL_RETURN_TCL		-102	/* converted by interact, interpeter */
					/* from "return -tcl" into TCL_RETURN*/
