/* This file contains a number of definitions for Signals that are found
on Unix systems but not on OpenVMS (at least up to 6.1).  */

/*Definitions are stolen from <signal.h> on a Digital Unix system */
/* 960422 MPJZ */

#define SIGSTOP   17    /* (@) stop (cannot be caught or ignored) */
#define SIGTSTP   18    /* (@) interactive stop */
#define SIGXCPU   24    /* cpu time limit exceeded (see setrlimit()) */
