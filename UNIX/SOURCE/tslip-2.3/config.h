/* config.h.  Generated automatically by configure.  */
/* config.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
#ifndef _ALL_SOURCE
/* #undef _ALL_SOURCE */
#endif

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to the type of elements in the array set by `getgroups'.
   Usually this is either `int' or `gid_t'.  */
/* #undef GETGROUPS_T */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef gid_t */

/* Define if you have alloca.h and it should be used (not Ultrix).  */
/* #undef HAVE_ALLOCA_H */

/* Define if you have unistd.h.  */
#define HAVE_UNISTD_H 1

/* Define if you have vfork.h.  */
/* #undef HAVE_VFORK_H */

/* Define if on MINIX.  */
/* #undef _MINIX */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef mode_t */

/* Define to `long' if <sys/types.h> doesn't define.  */
/* #undef off_t */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef pid_t */

/* Define if the system does not provide POSIX.1 features except
   with this defined.  */
/* #undef _POSIX_1_SOURCE */

/* Define if you need to in order for stat and other things to work.  */
/* #undef _POSIX_SOURCE */

/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE void

/* Define if the setvbuf function takes the buffering type as its second
   argument and the buffer pointer as the third, as on System V
   before release 3.  */
/* #undef SETVBUF_REVERSED */

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown
 */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef uid_t */

/* Define vfork as fork if vfork does not work.  */
/* #undef vfork */

/* This appears to have been left out of the standard acconfig.h, as the
 * standard AC_ALLOCA references it:
 */
/* #undef CRAY_STACKSEG_END */

#define HAVE_ANSI_PROTOS 1

/* #undef HAVE_RESOLVER */

#define HAVE_VOID 1

#define HAVE_VOID_POINTER 1

/* This also appears to have been left out of the standard acconfig.h */
/* #undef HAVE_TIME_WITH_SYS_TIME */

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <memory.h> header file.  */
#define HAVE_MEMORY_H 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <sys/wait.h> header file.  */
#define HAVE_SYS_WAIT_H 1

/* Define if you have the <sys/time.h> header file.  */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <termio.h> header file.  */
#define HAVE_TERMIO_H 1

/* Define if you have the <termios.h> header file.  */
#define HAVE_TERMIOS_H 1
