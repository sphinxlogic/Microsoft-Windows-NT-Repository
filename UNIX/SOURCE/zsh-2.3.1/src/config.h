/* this file is created automatically by buildzsh */

/* define this if you are sysvish */
#define SYSV
#define SYSVR4

#define TERMIOS
/* #define TTY_NEEDS_DRAINING */
/* #define CLOBBERS_TYPEAHEAD */

#define HAS_DIRENT

#define HAS_UNISTD

#define HAS_STDLIB

#define HAS_STRING

#define HAS_MEMORY

#define HAS_LOCALE

#define HAS_UTMPX

#define UTMP_HOST

/*#define HAS_TIME*/

#define HAS_WAIT

/* define this if you have WAITPID */
#define WAITPID

/* define this if you have SELECT */
#define HAS_SELECT

/* define this if you have <sys/select.h> */
#define HAS_SYS_SELECT

/* we can't just test for S_IFIFO or check to see if the mknod worked,
   because the NeXTs sold by a vendor which will remain nameless will
   happily create the FIFO for you, and then panic when you try to do
	something weird with them, because they aren't supported by the OS. */

/* #define NO_FIFOS */

/* define this if you have strftime() */
#define HAS_STRFTIME

#define HAS_TCSETPGRP

#define HAS_TCCRAP

#define HAS_SETPGID

#define HAS_SIGRELSE

/* define this if you have RFS */
/* #define HAS_RFS */

/* define this if you have a working getrusage and wait3 */
#define HAS_RUSAGE
/* define this if you use NIS for your passwd map */
/* #define HAS_NIS_PASSWD */

/* define this if your signal handlers return void */
#define SIGVOID
#ifdef sgi
#undef SIGVOID
#endif

/* define this if signal handlers need to be reset each time */
/* #define RESETHANDNEEDED */

#ifdef SIGVOID
#define HANDTYPE void
#else
#define HANDTYPE int
#define INTHANDTYPE
#endif

/* a string corresponding to the host type */
#define HOSTTYPE "i386"

/* the default editor for the fc builtin */
#define DEFFCEDIT "vi"

/* default prefix for temporary files */
#define DEFTMPPREFIX "/tmp/zsh"

/* define if you prefer "suspended" to "stopped" */
#define USE_SUSPENDED

/* the file to source absolutely first whenever zsh is run; if undefined,
      don't source anything */
#define GLOBALZSHENV "/etc/zshenv"

/* the file to source whenever zsh is run; if undefined, don't source
	anything */
#define GLOBALZSHRC "/etc/zshrc"

/* the file to source whenever zsh is run as a login shell; if
	undefined, don't source anything */
#define GLOBALZLOGIN "/etc/zlogin"

/* the file to source whenever zsh is run as a login shell, before
	zshrc is read; if undefined, don't source anything */
#define GLOBALZPROFILE "/etc/zprofile"

/* the default HISTSIZE */
#define DEFAULT_HISTSIZE 30

#define _BSD_SIGNALS   /* this could be an iris, you never know */
#define _BSD           /* this could be HP-UX, you never know */
#define _BSD_INCLUDES  /* this could be AIX, you never know */
#define _BBN_POSIX_SUPPORT	/* this could be nX, you never know */

/* if your compiler doesn't like void *, change this to char *
	and ignore all the warnings.
*/

typedef void *vptr;

#define JOB_CONTROL
