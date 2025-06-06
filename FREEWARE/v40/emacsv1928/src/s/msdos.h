/* System description file for MS-DOS

   Copyright (C) 1993 Free Software Foundation, Inc.

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

/* Note: lots of stuff here was taken from s-msdos.h in demacs. */


/*
 *	Define symbols to identify the version of Unix this is.
 *	Define all the symbols that apply correctly.
 */

/* #define UNIPLUS */
/* #define USG5 */
/* #define USG */
/* #define HPUX */
/* #define UMAX */
/* #define BSD4_1 */
/* #define BSD4_2 */
/* #define BSD4_3 */
/* #define BSD */
/* #define VMS */
#ifndef MSDOS
#define MSDOS
#endif
#undef BSD
#undef VMS

/* SYSTEM_TYPE should indicate the kind of system you are using.
 It sets the Lisp variable system-type.  */

#define SYSTEM_TYPE "ms-dos"

#define SYMS_SYSTEM syms_of_dosfns()

/* NOMULTIPLEJOBS should be defined if your system's shell
 does not have "job control" (the ability to stop a program,
 run some other program, then continue the first one).  */

#define NOMULTIPLEJOBS

/* Emacs can read input using SIGIO and buffering characters itself,
   or using CBREAK mode and making C-g cause SIGINT.
   The choice is controlled by the variable interrupt_input.
   Define INTERRUPT_INPUT to make interrupt_input = 1 the default (use SIGIO)

   SIGIO can be used only on systems that implement it (4.2 and 4.3).
   CBREAK mode has two disadvatages
     1) At least in 4.2, it is impossible to handle the Meta key properly.
        I hear that in system V this problem does not exist.
     2) Control-G causes output to be discarded.
        I do not know whether this can be fixed in system V.

   Another method of doing input is planned but not implemented.
   It would have Emacs fork off a separate process
   to read the input and send it to the true Emacs process
   through a pipe.
*/

/* #define INTERRUPT_INPUT */

/* Letter to use in finding device name of first pty,
  if system supports pty's.  'a' means it is /dev/ptya0  */

/* #define FIRST_PTY_LETTER 'a' */

/*
 *	Define HAVE_TIMEVAL if the system supports the BSD style clock values.
 *	Look in <sys/time.h> for a timeval structure.
 */

#define HAVE_TIMEVAL

/*
 *	Define HAVE_SELECT if the system supports the `select' system call.
 */

/* #define HAVE_SELECT */

/*
 *	Define HAVE_PTYS if the system supports pty devices.
 */

/* #define HAVE_PTYS */

/*
 *	Define NONSYSTEM_DIR_LIBRARY to make Emacs emulate
 *      The 4.2 opendir, etc., library functions.
 */

/* #define NONSYSTEM_DIR_LIBRARY */

#define SYSV_SYSTEM_DIR

/* Define this symbol if your system has the functions bcopy, etc. */

#define BSTRING

/* Define this is the compiler understands `volatile'.  */
#define HAVE_VOLATILE


/* subprocesses should be defined if you want to
   have code for asynchronous subprocesses
   (as used in M-x compile and M-x shell).
   This is generally OS dependent, and not supported
   under most USG systems. */

#undef subprocesses

/* If your system uses COFF (Common Object File Format) then define the
   preprocessor symbol "COFF". */

#define COFF

/* define MAIL_USE_FLOCK if the mailer uses flock
   to interlock access to /usr/spool/mail/$USER.
   The alternative is that a lock file named
   /usr/spool/mail/$USER.lock.  */

/* #define MAIL_USE_FLOCK */

/* Define CLASH_DETECTION if you want lock files to be written
   so that Emacs can tell instantly when you try to modify
   a file that someone else has modified in his Emacs.  */

/* #define CLASH_DETECTION */

/* Here, on a separate page, add any special hacks needed
   to make Emacs work on this system.  For example,
   you might define certain system call names that don't
   exist on your system, or that do different things on
   your system and must be used only through an encapsulation
   (Which you should place, by convention, in sysdep.c).  */

/* Some compilers tend to put everything declared static
   into the initialized data area, which becomes pure after dumping Emacs.
   On these systems, you must #define static as nothing to foil this.
   Note that emacs carefully avoids static vars inside functions.  */

/* #define static */

/* we use djgcc's malloc */
/* #define SYSTEM_MALLOC */
/* setjmp and longjmp can safely replace _setjmp and _longjmp,
   but they will run slower.  */

#define _setjmp setjmp
#define _longjmp longjmp

#define NO_MODE_T

/* we can use dj's getpagesize() */
#define HAVE_GETPAGESIZE

/* New chdir () routine. */
#ifdef chdir
#undef chdir
#endif
#define chdir sys_chdir

#define LIBS_SYSTEM -lpc

/* This somehow needs to be defined even though we use COFF.  */
#define TEXT_START -1

#define ORDINARY_LINK

/* command.com does not under stand `...` so we define this.  */
#define LIB_GCC -Lgcc
#define DONT_NEED_ENVIRON
#define SEPCHAR ';'

#define NULL_DEVICE "nul"
#define EXEC_SUFFIXES ".exe:.com:.bat:"

#define O_RDONLY        0x0001
#define O_WRONLY        0x0002
#define O_RDWR          0x0004
#define O_CREAT         0x0100
#define O_TRUNC         0x0200
#define O_EXCL          0x0400
#define O_APPEND        0x0800
#define O_TEXT          0x4000
#define O_BINARY        0x8000

#define HAVE_INVERSE_HYPERBOLIC
#define FLOAT_CHECK_DOMAIN
#define NO_MATHERR

/* When $TERM is "internal" then this is substituted:  */
#define INTERNAL_TERMINAL "pc|bios|IBM PC with colour display:\
:co#80:li#25:km:\
:cm=\E@%.%.:\
:do=^J:le=^H:up=\EU:ri=\ER:\
:ti=\EA\027:te=\EA\007\EE:\
:so=\EA\077:se=\EA\027:\
:ms:mb=\EX\200:md=\EX\010:mk=\EA\161:me=\EA\027:\
:cl=\EC:ce=\EE:\
:vb=\EB\140:bl=\007:"
#define fflush internal_flush

/* Define this to a function (Fdowncase, Fupcase) if your file system
   likes that */
#define FILE_SYSTEM_CASE Fdowncase

/* bcopy under djgpp is quite safe */
#define GAP_USE_BCOPY
#define BCOPY_UPWARD_SAFE 1
#define BCOPY_DOWNWARD_SAFE 1

/* We need a little extra space, see ../../lisp/loadup.el */
#define PURESIZE 240000

/* We have (the code to control) a mouse.  */
#define HAVE_MOUSE
