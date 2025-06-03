/* main.c - main() and some logging routines for expect

Written by: Don Libes, NIST, 2/6/90

Design and implementation of this program was paid for by U.S. tax
dollars.  Therefore it is public domain.  However, the author and NIST
would appreciate credit if this program or parts of it are used.
*/

#include <stdio.h>
#ifndef M_XENIX
  /* netdb.h is not available on SCO XENIX 386, unless you bought
     the TCP/IP Development Package.  <pf@artcom0.north.de> 26-Jan-92 : */
#include <netdb.h>
#endif
#include <sys/types.h>
#ifdef CRAY
#include <sys/inode.h>	/* Cray 5.1 needs this, I don't know why */
#endif
#include <sys/file.h>
#include <sys/ioctl.h>
#include <signal.h>
#ifndef NO_STRING_H
#include <string.h>
#else
char *strchr();
#endif
#include <sys/time.h>
#include <errno.h>
#ifdef EXTERN_ERRNO
extern int errno;
#endif
#include "term.h"
#include "tcl.h"
#include "tclInt.h"

#ifndef va_dcl
/* unfortunately, tclInt.h includes varargs, which on some systems, namely */
/* SunOS 4.0.3, does protect against reinclusion.  Do so here. */
#include <varargs.h>
#endif
#include "global.h"
#include "translate.h"
#include "command.h"

#ifdef __SABER__
#undef	VERSION
#define	VERSION		"Saber"
#undef	SCRIPTDIR
#define SCRIPTDIR	"test/"
#endif
static char exp_version[] = VERSION;
#define VERSION_VARNAME   "expect_version"
#define NEED_TCL_MAJOR		6
#define NEED_TCL_MINOR		1
#define SCRIPTDIR_VARNAME "expect_library"

Tcl_Interp *interp;		/* Tcl instantiation */
int loguser = TRUE;		/* if TRUE, expect/spawn may write to stdout */
int logfile_all = FALSE;	/* if TRUE, write log of all interactions */
				/* despite value of loguser. */
FILE *logfile = 0;
FILE *cmdfile = 0;
FILE *debugfile = 0;
int is_debugging = FALSE;
void init_cmd_interpreter();
void exp_init_pty();
void init_interact();
void init_expect();
void init_spawn();
void init_trap();
void init_unit_random();

void
usage()
{
	errorlog(stderr,"usage: expect [-di] [-c cmds] [[-f] cmdfile] [args]\r\n");
	bye(-1);
}

int is_raw = FALSE;
int is_noecho = FALSE;

extern int disconnected;
extern int dev_tty;

exp_tty tty_current, tty_cooked;
int ioctled_devtty = FALSE;

/* if set == 1, set it to raw, else unset it */
void
tty_raw(set)
int set;
{
	if (set == 1) {
		is_raw = TRUE;
#if defined(SYSV3) || defined(POSIX)
		tty_current.c_iflag = 0;
		tty_current.c_oflag = 0;
		tty_current.c_lflag &= ECHO; /* disable everything except echo */
		tty_current.c_cc[VMIN] = 1;
		tty_current.c_cc[VTIME] = 0;
	} else {
		tty_current.c_iflag = tty_cooked.c_iflag;
		tty_current.c_oflag = tty_cooked.c_oflag;
		tty_current.c_lflag = tty_cooked.c_lflag;
		tty_current.c_cc[VMIN] = tty_cooked.c_cc[VMIN];
		tty_current.c_cc[VTIME] = tty_cooked.c_cc[VTIME];
#else
		tty_current.sg_flags |= RAW;
	} else {
		tty_current.sg_flags = tty_cooked.sg_flags;
#endif
		is_raw = FALSE;
	}
}
	
void
tty_echo(set)
int set;
{
	if (set == 1) {
		is_noecho = FALSE;
#if defined(SYSV3) || defined(POSIX)
		tty_current.c_lflag |= ECHO;
	} else {
		tty_current.c_lflag &= ~ECHO;
#else
		
		tty_current.sg_flags |= ECHO;
	} else {
		tty_current.sg_flags &= ~ECHO;
#endif
		is_noecho = TRUE;
	}
}

/* returns 0 if nothing changed */
/* if something changed, the out parameters are changed as well */
int
tty_raw_noecho(tty_old,was_raw,was_echo)
exp_tty *tty_old;
int *was_raw, *was_echo;
{
	if (disconnected) return(0);
	if (is_raw && is_noecho) return(0);
	if (dev_tty == -1) return(0);

	*tty_old = tty_current;		/* save old parameters */
	*was_raw = is_raw;
	*was_echo = !is_noecho;
	debuglog("tty_raw_noecho: was raw = %d  echo = %d\r\n",is_raw,!is_noecho);

	tty_raw(1);
	tty_echo(-1);

#ifdef POSIX
 	if (tcsetattr(dev_tty, TCSADRAIN, &tty_current) == -1) {
#else
#	ifdef SYSV3
        if (ioctl(dev_tty, TCSETAW, &tty_current) == -1) {
#	else
	if (ioctl(dev_tty, TIOCSETP, &tty_current) == -1) {
#	endif
#endif
		errorlog("ioctl(raw): %s\r\n",sys_errlist[errno]);
		bye(-1);
	}

	ioctled_devtty = TRUE;
	return(1);
}

/* returns 0 if nothing changed */
/* if something changed, the out parameters are changed as well */
int
tty_cooked_echo(tty_old,was_raw,was_echo)
exp_tty *tty_old;
int *was_raw, *was_echo;
{
	if (disconnected) return(0);
	if (!is_raw && !is_noecho) return(0);
	if (dev_tty == -1) return(0);

	*tty_old = tty_current;		/* save old parameters */
	*was_raw = is_raw;
	*was_echo = !is_noecho;
	debuglog("tty_cooked_echo: was raw = %d  echo = %d\r\n",is_raw,!is_noecho);

	tty_raw(-1);
	tty_echo(1);

#ifdef POSIX
 	if (tcsetattr(dev_tty, TCSADRAIN, &tty_current) == -1) {
#else
#    ifdef SYSV3
        if (ioctl(dev_tty, TCSETAW, &tty_current) == -1) {
#    else
	if (ioctl(dev_tty, TIOCSETP, &tty_current) == -1) {
#    endif
#endif
		errorlog("ioctl(noraw): %s\r\n",sys_errlist[errno]);
		bye(-1);
	}
	ioctled_devtty = TRUE;

	return(1);
}

void
tty_set(tty,raw,echo)
exp_tty *tty;
int raw;
int echo;
{
#ifdef POSIX
 	if (tcsetattr(dev_tty, TCSADRAIN, tty) == -1) {
#else
#	ifdef SYSV3
        if (ioctl(dev_tty, TCSETAW, tty) == -1) {
#	else
	if (ioctl(dev_tty, TIOCSETP, tty) == -1) {
#	endif
#endif
		errorlog("ioctl(set): %s\r\n",sys_errlist[errno]);
		bye(-1);
	}
	is_raw = raw;
	is_noecho = !echo;
	tty_current = *tty;
	debuglog("tty_set: raw = %d, echo = %d\r\n",is_raw,!is_noecho);
	ioctled_devtty = TRUE;
}	

init_tty()
{
	extern exp_tty exp_tty_original;

	/* save original user tty-setting in 'cooked', just in case user */
	/* asks for it without earlier telling us what cooked means to them */
	tty_cooked = exp_tty_original;

	/* save our current idea of the terminal settings */
	tty_current = exp_tty_original;
}

void generic_sighandler();

void
bye(status)
int status;
{
	extern int disconnected;
	extern int forked;
	extern exp_tty exp_tty_original;
	static int bye_in_progress = FALSE;

	/* prevent recursion */
	if (bye_in_progress) {
		errorlog("bye recursed\r\n");
		exit(status);
	}
	bye_in_progress = TRUE;

	/* called user-defined exit routine if one exists */
	generic_sighandler(0);

	flush_streams();	/* need to do this before tty is reset, */
	/* because formatting earlier was done sensitive to tty mode */

	if (!disconnected && !forked
	    && (dev_tty != -1) && isatty(dev_tty) && ioctled_devtty) {
		tty_set(&exp_tty_original,dev_tty,0);
	}
	/* all other files either don't need to be flushed or will be
	   implicitly closed at exit.  Spawned processes are free to continue
	   running, however most will shutdown after seeing EOF on stdin.
	   Some systems also deliver SIGHUP and other sigs to idle processes
	   which will blow them away if not prepared.
	*/
	exit(status);
}

/* Following this are several functions that log the conversation. */
/* Most of them have multiple calls to printf-style functions.  */
/* At first glance, it seems stupid to reformat the same arguments again */
/* but we have no way of telling how long the formatted output will be */
/* and hence cannot allocate a buffer to do so. */
/* Fortunately, in production code, most of the duplicate reformatting */
/* will be skipped, since it is due to handling errors and debugging. */

/* send to log if open */
/* send to stderr if debugging enabled */
/* use this for logging everything but the parent/child conversation */
/* (this turns out to be almost nothing) */
/* uppercase L differentiates if from math function of same name */
#define LOGUSER		(loguser || force_stdout)
/*VARARGS*/
void
Log(va_alist)
va_dcl
{
	int force_stdout;
	char *fmt;
	va_list args;

	va_start(args);
	force_stdout = va_arg(args,int);
	fmt = va_arg(args,char *);
	if (debugfile) vfprintf(debugfile,fmt,args);
	if (logfile_all || (LOGUSER && logfile)) vfprintf(logfile,fmt,args);
	if (LOGUSER) vfprintf(stdout,fmt,args);
	va_end(args);
}

/* just like log but does no formatting */
/* send to log if open */
/* use this function for logging the parent/child conversation */
void
nflog(buf,force_stdout)
char *buf;
int force_stdout;	/* override value of loguser */
{
	int length = strlen(buf);

	if (debugfile) fwrite(buf,1,length,debugfile);
	if (logfile_all || (LOGUSER && logfile)) fwrite(buf,1,length,logfile);
	if (LOGUSER) fwrite(buf,1,length,stdout);
}
#undef LOGUSER

/* send to log if open and debugging enabled */
/* send to stderr if debugging enabled */
/* use this function for recording unusual things in the log */
/*VARARGS*/
void
debuglog(va_alist)
va_dcl
{
	char *fmt;
	va_list args;

	va_start(args);
	fmt = va_arg(args,char *);
	if (debugfile) vfprintf(debugfile,fmt,args);
	if (is_debugging) {
		vfprintf(stderr,fmt,args);
		if (logfile) vfprintf(logfile,fmt,args);
	}

	va_end(args);
}

/* send to log if open */
/* send to stderr */
/* use this function for error conditions */
/*VARARGS*/
void
errorlog(va_alist)
va_dcl
{
	char *fmt;
	va_list args;

	va_start(args);
	fmt = va_arg(args,char *);
	vfprintf(stderr,fmt,args);
	if (debugfile) vfprintf(debugfile,fmt,args);
	if (logfile) vfprintf(logfile,fmt,args);
	va_end(args);
}

/* just like errorlog but does no formatting */
/* send to log if open */
/* use this function for logging the parent/child conversation */
/*ARGSUSED*/
void
nferrorlog(buf,force_stdout)
char *buf;
int force_stdout;	/* not used, only declared here for compat with */
			/* nflog() */
{
	int length = strlen(buf);
	fwrite(buf,1,length,stderr);
	if (debugfile) fwrite(buf,1,length,debugfile);
	if (logfile) fwrite(buf,1,length,logfile);
}

/* this stupidity because Tcl needs commands in writable space */
char prompt1[] = "prompt1";
char prompt2[] = "prompt2";

static char *prompt2_default = "+> ";
static char *prompt1_default = "expect%d.%d> ";
static char *prompt1_default_out;
#define PROMPT1_DEFAULT_MAXSIZE 40	/* enough for prompt and two ints */

/*ARGSUSED*/
int
cmdPrompt1(clientData, interp, argc, argv)
ClientData clientData;
Tcl_Interp *interp;
int argc;
char **argv;
{
	Interp *iPtr = (Interp *)interp;

	sprintf(prompt1_default_out,prompt1_default,
		iPtr->numLevels,iPtr->curEventNum+1);			
	Tcl_Return(interp,prompt1_default_out,TCL_STATIC);
	return(TCL_OK);
}

/*ARGSUSED*/
int
cmdPrompt2(clientData, interp, argc, argv)
ClientData clientData;
Tcl_Interp *interp;
int argc;
char **argv;
{
	Tcl_Return(interp,prompt2_default,TCL_STATIC);
	return(TCL_OK);
}

/* user has pressed escape char from interact or somehow requested expect.
If a user-supplied command returns:

TCL_ERROR,	assume user is experimenting and reprompt
TCL_OK,		ditto
TCL_RETURN,	return TCL_OK (assume user just wants to escape() to return)
TCL_RETURN_TCL,	return TCL_RETURN
anything else	return it
*/
int
escape(interp)
Tcl_Interp *interp;
{
	int rc;
	char *ccmd;		/* pointer to complete command */
	char line[BUFSIZ];	/* space for partial command */
	int newcmd = TRUE;
	Tcl_CmdBuf buffer;
	Interp *iPtr = (Interp *)interp;
	int tty_changed = FALSE;

	exp_tty tty_old;
	int was_raw, was_echo;

	if (!(buffer = Tcl_CreateCmdBuf())) {
		Tcl_Return(interp,"no more space for cmd buffer\r\n",TCL_STATIC);
		return(TCL_ERROR);
	}
	newcmd = TRUE;
	while (TRUE) {
		/* force terminal state */
		tty_changed = tty_cooked_echo(&tty_old,&was_raw,&was_echo);

#if 0
		if (newcmd) Log(1,"expect%d.%d> ",
			iPtr->numLevels,iPtr->curEvent);
		else nflog("+> ",1);
#endif

		if (newcmd) {
			rc = Tcl_Eval(interp,prompt1,0,(char **)0);
			if (rc == TCL_OK) Log(1,"%s",interp->result);
			else Log(1,prompt1_default,iPtr->numLevels,
						   iPtr->curEventNum+1);
		} else {
			rc = Tcl_Eval(interp,prompt2,0,(char **)0);
			if (rc == TCL_OK) Log(1,"%s",interp->result);
			else Log(1,prompt2_default,1);
		}
		flush_streams();

		if (fgets(line,BUFSIZ,stdin) == NULL) {
			if (newcmd) bye(0); /* user sent EOF or disappeared */
			line[0] = 0;
		}
		if (debugfile) fwrite(line,1,strlen(line),debugfile);
		/* intentionally always write to logfile */
		if (logfile) fwrite(line,1,strlen(line),logfile);
		/* no need to write to stdout, since they will see it */
		/* just from it having been echoed as they are typing it */

		if (NULL == (ccmd = Tcl_AssembleCmd(buffer,line))) {
			newcmd = FALSE;
			continue;	/* continue collecting command */
		}
		newcmd = TRUE;

		if (tty_changed) tty_set(&tty_old,was_raw,was_echo);
		switch (rc = Tcl_RecordAndEval(interp,ccmd,0)) {
		case TCL_OK:
			if (*interp->result != 0)
				Log(0,"%s\r\n",cook(interp->result,(int *)0));
			continue;
		case TCL_ERROR:
			errorlog("%s\r\n",cook(get_var("errorInfo"),(int *)0));
#if 0
			if (*interp->result) {
				/* split into 2 errorlog calls, only because */
				/* cook uses a static buffer */
				/* note that ccmd has a trailing newline */
				errorlog("error in command: %s",
					cook(ccmd,(int *)0));
				errorlog("%s\r\n",
					cook(interp->result,(int *)0));
			}
			/* since user is typing by hand, we expect lots
			   of errors, and want to give another chance */
#endif
			continue;
#define finish(x)	{rc = x; goto done;}
		case TCL_BREAK:
		case TCL_CONTINUE:
			finish(rc);
		case TCL_RETURN_TCL:
			finish(TCL_RETURN);
		case TCL_RETURN:
			finish(TCL_OK);
		default:
			/* note that ccmd has trailing newline */
			errorlog("error %d: %s\r\n",rc,ccmd);
			continue;
		}
	}
	/* cannot fall thru here, must jump to label */
 done:
	if (tty_changed) tty_set(&tty_old,was_raw,was_echo);

	/* currently, code guarantees buffer is valid */
	Tcl_DeleteCmdBuf(buffer);

	return(rc);
}

static char *argv0;

void
main(argc, argv)
int argc;
char *argv[];
{
	extern int optind;
	extern char *optarg;
	char *args;		/* ptr to string-rep of all args */

	int interactive = FALSE;
	int cmdlinecmds = FALSE;
	char *cmdfilename = 0;
	int newcmd;
	Tcl_CmdBuf buffer;

	int c;
	int rc;

	int sys_rc = TRUE;	/* read system rc file */
	int my_rc = TRUE;	/* read personal rc file */

	int tcl_major = atoi(TCL_VERSION);
	int tcl_minor;
	char *dot = strchr(TCL_VERSION,'.');

	tcl_minor = atoi(dot+1);

	if (tcl_major < NEED_TCL_MAJOR || tcl_minor < NEED_TCL_MINOR) {
	       fprintf(stderr,
		       "Expect compiled with Tcl %d.%d but needs at least Tcl %d.%d\n",
		       tcl_major,tcl_minor,NEED_TCL_MAJOR,NEED_TCL_MINOR);
	       exit(-1);
	}

	argv0 = argv[0];

	init_cmd_interpreter();	/* Tcl interpreter */
	exp_init_pty();
	init_tty(); /* do this only now that pty's looked at orig tty state */

	/* initialize commands */
	init_expect();
	init_interact();
	init_spawn();
	init_trap();
	init_unit_random();

	Tcl_SetVar(interp,SCRIPTDIR_VARNAME,SCRIPTDIR,0);
	Tcl_SetVar(interp,VERSION_VARNAME,VERSION,0);

	prompt1_default_out = malloc(PROMPT1_DEFAULT_MAXSIZE);
	if (!prompt1_default_out) {
		errorlog("expect: malloc failed at startup\n");
		bye(-1);
	}

	while ((c = getopt(argc, argv, "c:df:inN")) != EOF) {
		switch(c) {
		case 'c': /* command */
			cmdlinecmds = TRUE;
			rc = Tcl_Eval(interp,optarg,0,(char **)0);

			if (rc != TCL_OK) {
#if 0
			    errorlog("error in command: %s\r\n",
				cook(optarg,(int *)0));
		    	    if (rc != TCL_ERROR)
				errorlog("Tcl_Eval = %d\r\n",rc);
			    if (*interp->result != 0) {
				errorlog("%s\r\n",
					cook(interp->result,(int *)0));
			    }
#endif
			    errorlog("%s\r\n",cook(get_var("errorInfo"),(int *)0));
			}
			break;
		case 'd': is_debugging = TRUE;
			debuglog("expect version %s\r\n",VERSION);
			break;
		case 'f': /* name of cmd file */
			cmdfilename = optarg;
			break;
		case 'i': /* interactive */
			interactive = TRUE;
			break;
		case 'n': /* don't read personal rc file */
			my_rc = FALSE;
			break;
		case 'N': /* don't read system-wide rc file */
			sys_rc = FALSE;
			break;
		default: usage();
		}
	}

	for (c = 0;c<argc;c++) {
		debuglog("argv[%d] = %s  ",c,argv[c]);
	}
	debuglog("\r\n");

	/* get cmd file name, if we haven't got it already */
	if (!cmdfilename && (optind < argc)) {
		cmdfilename = argv[optind];
		optind++;
	}
	if (cmdfilename) {
		if (streq(cmdfilename,"-")) {
			cmdfile = stdin;
		} else if (NULL == (cmdfile = fopen(cmdfilename,"r"))) {
			errorlog("%s: %s\r\n",cmdfilename,sys_errlist[errno]);
			bye(-1);
		}
	}

	if (cmdfile && interactive) {
		errorlog("cannot read commands from both file and keyboard\r\n");
		bye(-1);
	}

	/* collect remaining args and make into an argv */
	/* Tcl expects argv[0] to be the command name, but it doesn't do */
	/* anything useful with it anyway, so just back up the pointer */
	/* actually, back it up twice, so we can make the resultant [0] */
	/* be the program name */
	/* Oh, and we add to argc for the same reason */
	/* If no cmdfilename, there certainly can't be any args! */
	if (cmdfilename) {
		argv[optind-1] = cmdfilename;
		Tcl_ListCmd((ClientData)0,interp,2+argc-optind,argv+optind-2);
		args = interp->result;
	} else args = "";

	debuglog("set argv {%s}\r\n",args);
	Tcl_SetVar(interp,"argv",args,0);

	/* read rc files */
	if (sys_rc) {
	    char file[200];
	    int fd;

	    sprintf(file,"%sexpect.rc",SCRIPTDIR);
	    if (-1 != (fd = open(file,0))) {
		if (TCL_ERROR == (rc = Tcl_EvalFile(interp,file))) {
		    errorlog("error executing system initialization file: %s\r\n",file);
		    if (rc != TCL_ERROR) errorlog("Tcl_Eval = %d\r\n",rc);
		    if (*interp->result != 0) errorlog("%s\r\n",interp->result);
		    bye(-1);
		}
		close(fd);
	    }
	}
	if (my_rc) {
	    char file[200];
	    char *home;
	    int fd;
	    char *getenv();

	    if (NULL != (home = getenv("HOME"))) {
		sprintf(file,"%s/.expect.rc",home);
		if (-1 != (fd = open(file,0))) {
		    if (TCL_ERROR == (rc = Tcl_EvalFile(interp,file))) {
			errorlog("error executing file: %s\r\n",file);
			if (rc != TCL_ERROR) errorlog("Tcl_Eval = %d\r\n",rc);
			if (*interp->result != 0) errorlog("%s\r\n",interp->result);
			bye(-1);
		    }
		    close(fd);
	        }
	    }
	}


	/* become interactive if user requested or "nothing to do" */
	if (interactive || (!cmdfile && !cmdlinecmds)) (void) escape(interp);

	if (!cmdfile) bye(0);

	debuglog("executing commands from command file\r\n");

	if (!(buffer = Tcl_CreateCmdBuf())) {
		errorlog("no more space for cmd buffer\r\n");
		bye(0);
	}
	newcmd = TRUE;
	while (1) {
		char line[BUFSIZ];/* buffer for partial Tcl command */
		char *ccmd;	/* pointer to complete Tcl command */

		if (fgets(line,BUFSIZ,cmdfile) == NULL) {
			if (newcmd) bye(0);
			else line[0] = 0;
		}
		if (NULL == (ccmd = Tcl_AssembleCmd(buffer,line))) {
			newcmd = FALSE;
			continue;	/* continue collecting command */
		}
		newcmd = TRUE;


		rc = Tcl_Eval(interp,ccmd,0,(char **)0);

		if (rc != TCL_OK) {
		    /* no \n at end, since ccmd will already have one. */
		    /* Actually, this is not true if command is last in */
		    /* file and has no newline after it, oh well */
#if 0
		    errorlog("error in command: %s",cook(ccmd,(int *)0));
		    if (rc != TCL_ERROR) errorlog("Tcl_Eval = %d\r\n",rc);
		    if (*interp->result != 0) {
			errorlog("%s\r\n",cook(interp->result,(int *)0));
		    }
#endif
		    errorlog("%s\r\n",cook(get_var("errorInfo"),(int *)0));
		}
	}
	/*NOTREACHED*/
}

/*ARGSUSED*/
int
cmdExpectVersion(clientData, interp, argc, argv)
ClientData clientData;
Tcl_Interp *interp;
int argc;
char **argv;
{
	int emajor, umajor;
	char *user_version;	/* user-supplied version string */

	if (argc == 1) {
		Tcl_SetResult(interp,exp_version,TCL_STATIC);
		return(TCL_OK);
	}
	if (argc > 3) {
		tcl_error("usage: expect_version [[-exit] version]");
		return(TCL_ERROR);
	}

	user_version = argv[argc==2?1:2];
	emajor = atoi(exp_version);
	umajor = atoi(user_version);

	/* first check major numbers */
	if (emajor > umajor) return(TCL_OK);
	else if (emajor == umajor) {
		int u, e;

		/* now check minor numbers */
		char *dot = strchr(user_version,'.');
		/* if user did not supply minor number, let it go */
		if (dot) {
			u = atoi(dot+1);
			dot = strchr(exp_version,'.');
			e = atoi(dot+1);
			if (e >= u) return(TCL_OK);
		}
	}

	if (argc == 2) {
		tcl_error("requires Expect version %s (but using %s)",
			argv0,user_version,exp_version);
		return(TCL_ERROR);
	}
	errorlog("%s: requires Expect version %s (but using %s)\r\n",
		argv0,user_version,exp_version);
	bye(-1);
	/*NOTREACHED*/
}
