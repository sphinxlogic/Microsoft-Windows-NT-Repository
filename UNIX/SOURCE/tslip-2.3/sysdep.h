/* @(#)sysdep.h	1.2 (11 Oct 1993) */
/*..........................................................................
 . Copyright 1993, Jim Avera.  All Rights Reserved.
 . 
 . You are prohibited from copying, distributing, modifying, or using this
 . file except as described in the accompanying file "COPYRIGHT".
 ...........................................................................*/

/*
 * This file is supposed to encapsulate everything which might depend on the
 * compilation platform.  At first (9/93), most of the code ignores this, but
 * the intent is to eventually remove hard-coded dependencies in .c files and 
 * include this file instead, so as to make it easier to port to future systems.
 */

#include "config.h"	/* get advice from configure */

#define TSLIP 1

/* Some compilers require this to be the first thing (except #directives) */
#ifdef __GNUC__
# define alloca __builtin_alloca
#else
# if HAVE_ALLOCA_H
#  include <alloca.h>
# else
#  ifdef _AIX
    #pragma alloca  /* indent required so pre-ansi compilers ignore this */
#  else
    char *alloca();
#  endif
# endif
#endif
 
#ifdef const
# define volatile /* empty */
#endif

#if ! HAVE_VOID
#define void int
#endif

#if HAVE_VOID_POINTER
typedef void *pointer;
typedef const void *constpointer;
#else
typedef char *pointer;
typedef const char *constpointer;
#endif

#if HAVE_ANSI_PROTOS
#define P(x) x
#else
#define P(x) ()
#endif

#if STDC_HEADERS
# include <stdarg.h>
# define VARARGLIST1(t1,n1) (t1 n1,...)
# define VARARGLIST2(t1,n1,t2,n2) (t1 n1,t2 n2,...)
# define VARARGLIST3(t1,n1,t2,n2,t3,n3) (t1 n1,t2 n2,t3 n3,...)
# define VARARGLIST4(t1,n1,t2,n2,t3,n3,t4,n4) (t1 n1,t2 n2,t3 n3,t4 n4,...)
# define Va_start(ap,last_fixed_arg) va_start(ap,last_fixed_arg)
#else
# include <varargs.h>
# define VARARGLIST1(t1,n1) (n1, va_alist) t1 n1; va_dcl
# define VARARGLIST2(t1,n1,t2,n2) (n1, n2, va_alist) t1 n1; t2 n2; va_dcl
# define VARARGLIST3(t1,n1,t2,n2,t3,n3) (n1,n2,n3,va_alist) t1 n1; t2 n2; t3 n3; va_dcl
# define VARARGLIST4(t1,n1,t2,n2,t3,n3,t4,n4) (n1,n2,n3,n4,va_alist) t1 n1; t2 n2; t3 n3; t4 n4; va_dcl
# define Va_start(ap,last_fixed_arg) va_start(ap)
#endif
#define Va_list va_list
#define Va_end(ap) va_end(ap)
#define Va_arg(ap,t) va_arg(ap,t)

/* --- Define System V and BSD string/memory functions */
#if STDC_HEADERS || HAVE_STRING_H
#include <string.h>
/* An ANSI string.h and pre-ANSI memory.h might conflict */
#if !STDC_HEADERS && HAVE_MEMORY_H
#include <memory.h>
#endif /* not STDC_HEADERS and HAVE_MEMORY_H */
#define bcopy(s,d,n) memcopy((d),(s),(n))
#define bcmp(a,b,n) memcpy((a),(b),(n))
#define bzero(d,n) memset((d),0,(n))
#else
#include <strings.h>
/* memory.h and strings.h conflict on some systems */
#define strchr index
#define strrchr rindex
#define memcpy(d,s,n) bcopy((s),(d),(n))
#define memcmp(s1,d2,n) bcmp((s1),(s2),(n))
#endif /* not STDC_HEADERS and not HAVE_STRING_H */

#if STDC_HEADERS
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#else
extern char *getenv ();
extern pointer malloc ();
extern pointer realloc ();
#if __STDC__ || __GNUC__
#include "limits.h"
#else
#define SHRT_MIN        (-32768)
#define SHRT_MAX        32767
#define USHRT_MAX       65535
#define INT_MIN         (-2147483647-1)
#define INT_MAX         2147483647 
#define UINT_MAX        4294967295
#define LONG_MIN        (-2147483647-1)
#define LONG_MAX        2147483647
#define ULONG_MAX       4294967295
#define CHAR_BIT 8
#define WORD_BIT (sizeof(int) * CHAR_BIT)
#define LONG_BIT (sizeof(long) * CHAR_BIT)
#endif
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifndef R_OK /* access(2) "amode" values no in unistd.h */
# define	F_OK	0	/* mere existence */
# define	X_OK	1
# define	W_OK	2
# define	R_OK	4
#endif

#ifndef SEEK_SET /* lseek(2) "whence" values no in unistd.h */
# define SEEK_SET 0
# define SEEK_CUR 1
# define SEEK_END 2
#endif

#ifndef GF_PATH /* file paths in unistd.h */
# define GF_PATH "/etc/group"
# define PF_PATH "/etc/passwd"
# define IN_PATH "/usr/include"
#endif

#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#if HAVE_FCNTL_H
#include <fcntl.h>
#else
#include <sys/file.h>
#endif

#if HAVE_SYS_WAIT_H
#ifndef _POSIX_VERSION
/* Prevent the NeXT prototype using union wait from causing problems.  */
#define wait system_wait
#endif
#include <sys/wait.h>
#ifndef _POSIX_VERSION
#undef wait
#endif
#endif /* HAVE_SYS_WAIT_H */

#ifndef WEXITSTATUS
#define WEXITSTATUS(stat_val) ((unsigned)(stat_val) >> 8)
# define WEXITSTATUS(stat_val)	((int)(((stat_val)>>8)&255))
# define WTERMSIG(stat_val)	((int)(((stat_val)   )&127))
# define WSTOPSIG(stat_val)	((int)(((stat_val)>>8)&255))
#undef WIFEXITED		/* Avoid 4.3BSD incompatibility with Posix.  */
#endif
#ifndef WIFEXITED
#define WIFEXITED(stat_val)	(((stat_val) & 255) == 0)
#endif
#ifndef WCOREDUMP
# define WCOREDUMP(stat_val)	(((stat_val)&0200) != 0)
#endif
#ifndef WIFCONTINUED
# define WIFCONTINUED(stat_val) (((int)((stat_val))&0177777)==0177777)
#endif

#if HAVE_VFORK_H
#include <vfork.h>
#endif

#include <errno.h>
#ifndef EALREADY
# define EALREADY EAGAIN
#endif
#if !STDC_HEADERS
extern int errno;
#endif

#include <stdio.h>

#ifndef S_IFMT
# include <sys/stat.h>
#endif

#ifndef S_ISDIR	/* st_mode file type macros */
#define S_ISREG(m)	(((m) & S_IFMT)==S_IFREG)	/* regular file */
#define S_ISDIR(m)	(((m) & S_IFMT)==S_IFDIR)	/* directory */
#define S_ISCHR(m)	(((m) & S_IFMT)==S_IFCHR)	/* character special */
#define S_ISBLK(m)	(((m) & S_IFMT)==S_IFBLK)	/* block special */
#define S_ISFIFO(m)	(((m) & S_IFMT)==S_IFIFO)	/* pipe or FIFO */
#endif

/* st_mode file permission bits */
#ifndef S_IRUSR
# define	S_IRWXU	00700		/* read, write, execute: owner */
# define	S_IRUSR	00400		/* read permission: owner */
# define	S_IWUSR	00200		/* write permission: owner */
# define	S_IXUSR	00100		/* execute permission: owner */
# define	S_IRWXG	00070		/* read, write, execute: group */
# define	S_IRGRP	00040		/* read permission: group */
# define	S_IWGRP	00020		/* write permission: group */
# define	S_IXGRP	00010		/* execute permission: group */
# define	S_IROTH	00004		/* read permission: other */
# define	S_IWOTH	00002		/* write permission: other */
# define	S_IXOTH	00001		/* execute permission: other */
#endif

#include <signal.h>

extern RETSIGTYPE ( *signal P((int,RETSIGTYPE (*)(int))) ) P((int)) ;

#ifndef sigmask
# define sigmask(sig) (1L << ((sig)-1))
#endif

/* Pre-SVR3 setvbuf had middle args backwards from documentation */
#if SETVBUF_REVERSED
# define do_setvbuf(stream,buf,type,size) setvbuf(stream,type,buf,size)
#else
# define do_setvbuf(stream,buf,type,size) setvbuf(stream,buf,type,size)
#endif

#include <sys/param.h>

#if ((!defined(_MAXNAMLEN)) && defined(_MAXNAMLEN))
# define MAXNAMLEN _MAXNAMLEN	/* posix-compliant identifier */
#endif

#ifndef MAXPATHLEN
# define MAXPATHLEN ((unsigned)256)
#endif

/* Greg Lehey, 17 January 1994: SVR4.2 header files have difficulty working
   * out which way round its bytes are. Help like this. */
#ifdef SVR4
#ifdef i386
#ifndef BYTE_ORDER
#define	LITTLE_ENDIAN	1234				    /* least-significant byte first */
#define	BIG_ENDIAN	4321				    /* most-significant byte first */
#define	BYTE_ORDER	LITTLE_ENDIAN
#endif
#endif
#endif
