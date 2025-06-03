*** /usr/include/sys/signal.h~	Tue Nov 17 20:38:55 1992
--- /usr/include/sys/signal.h   Tue Jan 25 19:44:30 1994
***************
*** 83,89 ****
  #define SIGIGNORE	0x800
  #define SIGPAUSE	0x1000
  
! #if (__STDC__ - 0 == 0) || defined(_POSIX_SOURCE) || defined(_XOPEN_SOURCE)
  typedef struct {		/* signal set type */
  	unsigned long	sa_sigbits[4];
  } sigset_t;
--- 83,89 ----
  #define SIGIGNORE	0x800
  #define SIGPAUSE	0x1000
  
! #if defined(__STDC__) || defined(_POSIX_SOURCE) || defined(_XOPEN_SOURCE)
  typedef struct {		/* signal set type */
  	unsigned long	sa_sigbits[4];
  } sigset_t;
*** 99,105 ****
  #define SA_NOCLDSTOP  0x00020000      /* don't send job control SIGCLD's */
  #endif
  
! #if (__STDC__) && !defined(_POSIX_SOURCE)
                        /* non-comformant ANSI compilation      */
  
  /* definitions for the sa_flags field */
--- 99,105 ----
  #define SA_NOCLDSTOP  0x00020000      /* don't send job control SIGCLD's */
  #endif
  
! #if defined(__STDC__) && !defined(_POSIX_SOURCE)
                        /* non-comformant ANSI compilation      */
  
  /* definitions for the sa_flags field */
