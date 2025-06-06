/* system description header for VMS
   Copyright (C) 1986 Free Software Foundation, Inc.

This file is part of GNU Emacs.

GNU Emacs is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Emacs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Emacs; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*
 *	Define symbols to identify the version of Unix this is.
 *	Define all the symbols that apply correctly.
 */

#ifndef VMS		    /* Decus cpp doesn't define this but VAX C does */
#define VMS
#endif /* VMS */
/* Note that this file is used indirectly via vms4-0.h, or some other
   such file.  These other files define a symbol VMS4_0, VMS4_2, etc.  */

/* SYSTEM_TYPE should indicate the kind of system you are using.
 It sets the Lisp variable system-type.  */

#define SYSTEM_TYPE "vax-vms"

/* NOMULTIPLEJOBS should be defined if your system's shell
 does not have "job control" (the ability to stop a program,
 run some other program, then continue the first one).  */

/* #define NOMULTIPLEJOBS */

/* INTERRUPT_INPUT controls a default for Unix systems.
   VMS uses a separate mechanism.  */

/* #define INTERRUPT_INPUT */

/* Letter to use in finding device name of first pty,
  if system supports pty's.  'a' means it is /dev/ptya0  */

#define FIRST_PTY_LETTER 'a'

/*
 *	Define HAVE_TIMEVAL if the system supports the BSD style clock values.
 *	Look in <sys/time.h> for a timeval structure.
 */

/* #define HAVE_TIMEVAL */

/*
 *	Define HAVE_SELECT if the system supports the `select' system call.
 */

#define HAVE_SELECT		/* There is an emulation in vmsproc.c */

/*
 *	Define HAVE_PTYS if the system supports pty devices.
 */

#define HAVE_PTYS

/* Define HAVE_SOCKETS if system supports 4.2-compatible sockets.  */

#ifdef MULTINET
/* We're to use the old multinet style.  Some newer versions want
   this macro set to understand that.  */
#define MULTINET_OLD_STYLE
#define BSTRING
#endif

#if defined(UCX) || defined(MULTINET)
#define HAVE_SOCKETS
/* #define HAVE_GETHOSTNAME /* Now decided by configure... */ 
#endif

#if defined(UCX) && defined(MULTINET)
#error It is not permitted to use more than one TCP/IP package.
#endif

/*
 *	Define NONSYSTEM_DIR_LIBRARY to make Emacs emulate
 *      The 4.2 opendir, etc., library functions.
 */

#define NONSYSTEM_DIR_LIBRARY

/* Define this symbol if your system has the functions bcopy, etc. */

/* #define BSTRING */

/* subprocesses should be defined if you want to
   have code for asynchronous subprocesses
   (as used in M-x compile and M-x shell).
   This is generally OS dependent, and not supported
   under most USG systems. */

#define subprocesses

/* If your system uses COFF (Common Object File Format) then define the
   preprocessor symbol "COFF". */

/* #define COFF */

/* define MAIL_USE_FLOCK if the mailer uses flock
   to interlock access to /usr/spool/mail/$USER.
   The alternative is that a lock file named
   /usr/spool/mail/$USER.lock.  */

/* #define MAIL_USE_FLOCK */

/* Define CLASH_DETECTION if you want lock files to be written
   so that Emacs can tell instantly when you try to modify
   a file that someone else has modified in his Emacs.  */

/* #define CLASH_DETECTION */

/* Define the maximum record length for print strings, if needed. */

#define MAX_PRINT_CHARS 300

/* Define the path separator character */

#ifdef SEPCHAR
#undef SEPCHAR
#endif
#define SEPCHAR ','


/* Here, on a separate page, add any special hacks needed
   to make Emacs work on this system.  For example,
   you might define certain system call names that don't
   exist on your system, or that do different things on
   your system and must be used only through an encapsulation
   (Which you should place, by convention, in sysdep.c).  */

/* Do you have the shareable library bug?  If you link with a shareable
   library that contains psects with the NOSHR attribute and also refer to
   those psects in your program, the linker give you a private version of
   the psect which is not related to the version used by the shareable
   library.  The end result is that your references to variables in that
   psect have absolutely nothing to do with library references to what is
   supposed to be the same variable.  If you intend to link with the standard
   C library (NOT the shareable one) you don't need to define this.  (This
   is NOT fixed in V4.4...) */

#define SHAREABLE_LIB_BUG

/* Partially due to the above mentioned bug and also so that we don't need
   to require that people have a shareable C library, the default for Emacs
   is to link with the non-shared library.  If you want to link with the
   shared library, define this and remake xmakefile and fileio.c. This allows
   us to ship a guaranteed executable image. */

#define LINK_CRTL_SHARE

/* Define this if you want to read the file SYS$SYSTEM:SYSUAF.DAT for user
   information.  If you do use this, you must either make SYSUAF.DAT world 
   readable or install Emacs with SYSPRV.  */

/* #define READ_SYSUAF */

/* These are some special definitions that are needed */
#ifdef UCX
#ifdef __DECC
#define LIBS_SYSTEM sys$share:ucx$ipc_shr/share
#else /* for some reason, the VAX C compiled version can't stand
	 the shareable lib */
#define LIBS_SYSTEM sys$library:ucx$ipc/library
#endif
#endif

#ifdef MULTINET
#define LIBS_SYSTEM multinet:multinet_socket_library/share
#endif

#define C_DEBUG_SWITCH /debug
#define C_OPTIMIZE_SWITCH /optimize

#define LIB_MATH
#define LIB_STANDARD
#define LIB_X11_LIB sys$share:decw$xlibshr/share
#define UNEXEC , vmsmap.obj
#ifdef UNEXEC_SRC
#undef UNEXEC_SRC
#endif
#define UNEXEC_SRC $(srcdir)vmsmap.c
#define UNEXEC_HDRS $(srcdir)getpagesize.h
#define ORDINARY_LINK

#ifndef NOT_C_CODE
/* On VMS these have a different name */

#define index strchr
#define rindex strrchr
#define unlink delete

#ifndef _GNUC_
extern double mth$dmod(double*, double*);
#define drem(x,y) mth$dmod(&(x),&(y))
#endif

/* Some time rountines are missing in the VAX C RTL, or needs some
   extra bit of code */
#define tzset sys_tzset
#define localtime sys_localtime
#define gmtime sys_gmtime

/* On later versions of VMS these exist in the C run time library, but
   we are using our own implementations.  Hide their names to avoid
   linker errors */
#define rename sys_rename
#define execvp sys_execvp
#define system sys_system

/* select() is in the multinet and the UCX socket library, but are
   inappropriate here */
#define select sys_select

#ifdef UCX
/* connect() and socket() are defined in the UCX socket library, but they
   won't return a channel number we could use for SYS$QIO, so we redefine
   them */
#define connect sys_connect
#define socket sys_socket
#endif

#if 0 /* DEC C undefines these macros in STDLIB.H, which is included
	 in some of the C files.  This makes Emacs on AXP/OpenVMS crash.
	 So we'll get some linker warnings... so what? */
/* Hide these names so that we don't get linker errors */
#define malloc sys_malloc
#define free sys_free
#define cfree sys_cfree
#define realloc sys_realloc
#define calloc sys_calloc
#define cfree sys_cfree
#endif /* 0 */

#ifndef GNU_MALLOC
/* Don't use the standard brk and sbrk */
#define sbrk sys_sbrk
#define brk sys_brk
#endif

/* On VMS we want to avoid reading and writing very large amounts of
   data at once, so we redefine read and write here. */

#define read sys_read
#define write sys_write

/* sys_creat just calls the real creat with additional args of
   "rfm=var", "rat=cr" to get "normal" VMS files... */
#define creat sys_creat

/* fwrite forces an RMS PUT on every call.  This is abysmally slow, so
   we emulate fwrite with fputc, which forces buffering and is much
   faster! */
#define fwrite sys_fwrite

/* getuid only returns the member number, which is not unique on most VMS
   systems.  We emulate it with (getgid()<<16 | getuid()). */
#define getuid sys_getuid

/* If user asks for TERM, check first for EMACS_TERM.  */
#define getenv sys_getenv
  
/* Standard C abort is less useful than it should be. */
#define abort sys_abort

/* Name conflict.  Let's take the Emacs version.  */
#define getppid sys_getppid

/* Case conflicts with C library fread. */
#define Fread F_read

/* Case conflicts with C library fabs. */
#define Fabs F_abs

/* Case conflicts with C library fabs. */
#define Fmod F_mod

/* Case conflicts with C library srandom. */
#define Srandom S_random

/* Case conflicts with C library srandom. */
#define Ssignal S_signal

/* Case conflicts with Xlib XFree. */
#define xfree emacs_xfree

/* variable length too long... maybe */
#if 0
#define do_line_insertion_deletion_costs do_line_insertion_deletion_cost
#endif

/* Cause initialization of vmsfns.c to be run.  */
#define SYMS_SYSTEM syms_of_vmsfns ()

/* VAXCRTL access doesn't deal with SYSPRV very well (among other oddites...)
   Here, we use $CHKPRO to really determine access. */
#define access sys_access

#define	PAGESIZE	512

#define	_longjmp	longjmp
#define	_setjmp		setjmp

#ifdef __GNUC__
/* GAS uses up 7 of the available 30 characters in an external name
   to distinguish the case of the letters.
   So make these lower case, so we have all 30 for distinguishing.  */
#define Vminibuffer_completion_table vminibuffer_completion_table
#define Qminibuffer_completion_table qminibuffer_completion_table
#define Vminibuffer_completion_predicate vminibuffer_completion_predicate
#define Qminibuffer_completion_predicate qminibuffer_completion_predicate
#define Vminibuffer_completion_confirm vminibuffer_completion_confirm
#define Qminibuffer_completion_confirm qminibuffer_completion_confirm
#endif

#ifdef __GNUC__
#if 0
extern char sdata[] asm("_$$PsectAttributes_NOOVR$$$D$ATA") ;
extern char edata[] asm("_$$PsectAttributes_NOOVR$$__DATA") ;
#endif
#define DATA_START (((int) sdata + 8191) & ~511)
#define DATA_END ((int) edata + 511)
#else
#if 0
globalref char sdata[];
globalref char edata[];
#endif
#define DATA_START (((int) sdata + 511) & ~511)
#define DATA_END ((int) edata + 511)
#endif
#define TEXT_START 512

/* Baud-rate values from tty status are not standard.  */

#define BAUD_CONVERT  \
{ 0, 50, 75, 110, 134, 150, 300, 600, 1200, 1800, \
  2000, 2400, 3600, 4800, 7200, 9600, 19200 }

#define PURESIZE 330000

/* Stdio FILE type has extra indirect on VMS, so must alter this macro.  */

#define PENDING_OUTPUT_COUNT(FILE) ((*(FILE))->_ptr - (*(FILE))->_base)

#define NULL_DEVICE "NLA0:"

#define EXEC_SUFFIXES ".exe:.com"

/* Case conflict with Xlib XFree () */
#define xfree emacs_xfree

#endif /* NOT_C_CODE */
