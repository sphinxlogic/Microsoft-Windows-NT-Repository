/* src/config.h.  Generated automatically by configure.  */
/* GNU Emacs site configuration template file.  -*- C -*-
   Copyright (C) 1988, 1993 Free Software Foundation, Inc.

This file is part of GNU Emacs.

GNU Emacs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.  No author or distributor
accepts responsibility to anyone for the consequences of using it
or for whether it serves any particular purpose or works at all,
unless he says so in writing.  Refer to the GNU Emacs General Public
License for full details.

Everyone is granted permission to copy, modify and redistribute
GNU Emacs, but only under the conditions described in the
GNU Emacs General Public License.   A copy of this license is
supposed to have been given to you along with GNU Emacs so you
can know your rights and responsibilities.  It should be in a
file named COPYING.  Among other things, the copyright notice
and this notice must be preserved on all copies.  */


/* No code in Emacs #includes config.h twice, but some of the code
   intended to work with other packages as well (like gmalloc.c) 
   think they can include it as many times as they like.  */
#ifndef EMACS_CONFIG_H
#define EMACS_CONFIG_H


/* These are all defined in the top-level Makefile by configure.
   They're here only for reference.  */

/* Define LISP_FLOAT_TYPE if you want emacs to support floating-point
   numbers. */
#define LISP_FLOAT_TYPE 1

/* Define GNU_MALLOC if you want to use the *new* GNU memory allocator. */
#define GNU_MALLOC 1

/* Define REL_ALLOC if you want to use the relocating allocator for
   buffer space. */
#define REL_ALLOC 1
  
/* Define HAVE_X_WINDOWS if you want to use the X window system.  */
#define HAVE_X_WINDOWS 1

/* Define HAVE_X11 if you want to use version 11 of X windows.
   Otherwise, Emacs expects to use version 10.  */
#define HAVE_X11 1

/* Define this if you're using XFree386.  */
/* #undef HAVE_XFREE386 */

/* Define HAVE_X_MENU if you want to use the X window menu system.
   This appears to work on some machines that support X
   and not on others.  */
#define HAVE_X_MENU 1

/* If we're using any sort of window system, define MULTI_FRAME.  */
#ifdef HAVE_X_WINDOWS
#define MULTI_FRAME
#endif

/* Define USE_TEXT_PROPERTIES to support visual and other properties
   on text. */
#define USE_TEXT_PROPERTIES

/* Define USER_FULL_NAME to return a string
   that is the user's full name.
   It can assume that the variable `pw'
   points to the password file entry for this user.

   At some sites, the pw_gecos field contains
   the user's full name.  If neither this nor any other
   field contains the right thing, use pw_name,
   giving the user's login name, since that is better than nothing.  */
#define USER_FULL_NAME pw->pw_gecos

/* Define AMPERSAND_FULL_NAME if you use the convention
   that & in the full name stands for the login id.  */
/* #undef AMPERSAND_FULL_NAME */

/* Some things figured out by the configure script, grouped as they are in
   configure.in.  */
#define HAVE_SYS_TIMEB_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_UNISTD_H 1
#define STDC_HEADERS 1
#define TIME_WITH_SYS_TIME 1

/* #undef HAVE_LIBDNET */

/* #undef HAVE_ALLOCA_H */

#define HAVE_GETTIMEOFDAY 1
#define HAVE_GETHOSTNAME 1
#define HAVE_DUP2 1
#define HAVE_RENAME 1
#define HAVE_CLOSEDIR 1

/* #undef TM_IN_SYS_TIME */
/* #undef HAVE_TM_ZONE */
#define HAVE_TZNAME 1

/* #undef const */

#define HAVE_LONG_FILE_NAMES 1

/* #undef CRAY_STACKSEG_END */
/* #undef STACK_DIRECTION */

#define UNEXEC_SRC unexelf.c

/* #undef HAVE_LIBXBSD */
#define HAVE_XRMSETDATABASE 1
#define HAVE_XSCREENRESOURCESTRING 1
#define HAVE_XSCREENNUMBEROFSCREEN 1

#define HAVE_MKDIR 1
#define HAVE_RMDIR 1
#define HAVE_RANDOM 1
#define HAVE_BCOPY 1
#define HAVE_LOGB 1
#define HAVE_FREXP 1
/* #undef HAVE_FTIME */
/* #undef HAVE_RES_INIT */ /* For -lresolv on Suns.  */

/* #undef HAVE_AIX_SMT_EXP */

/* If using GNU, then support inline function declarations. */
#ifdef __GNUC__
#define INLINE __inline__
#else
#define INLINE
#endif

/* The configuration script defines opsysfile to be the name of the
   s/*.h file that describes the system type you are using.  The file
   is chosen based on the configuration name you give.

   See the file ../etc/MACHINES for a list of systems and the
   configuration names to use for them.

   See s/template.h for documentation on writing s/*.h files.  */
#define config_opsysfile "s/usg5-4-2.h" 
#include config_opsysfile

/* The configuration script defines machfile to be the name of the
   m/*.h file that describes the machine you are using.  The file is
   chosen based on the configuration name you give.

   See the file ../etc/MACHINES for a list of machines and the
   configuration names to use for them.

   See m/template.h for documentation on writing m/*.h files.  */
#define config_machfile "m/intel386.h"
#include config_machfile

/* Load in the conversion definitions if this system
   needs them and the source file being compiled has not
   said to inhibit this.  There should be no need for you
   to alter these lines.  */

#ifdef SHORTNAMES
#ifndef NO_SHORTNAMES
#include "../shortnames/remap.h"
#endif /* not NO_SHORTNAMES */
#endif /* SHORTNAMES */

/* Define `subprocesses' should be defined if you want to
   have code for asynchronous subprocesses
   (as used in M-x compile and M-x shell).
   These do not work for some USG systems yet;
   for the ones where they work, the s/*.h file defines this flag.  */

#ifndef VMS
#ifndef USG
/* #define subprocesses */
#endif
#endif

/* Define LD_SWITCH_SITE to contain any special flags your loader may need.  */
/* #undef LD_SWITCH_SITE */

/* Define C_SWITCH_SITE to contain any special flags your compiler needs.  */
/* #undef C_SWITCH_SITE */

/* Define LD_SWITCH_X_SITE to contain any special flags your loader
   may need to deal with X Windows.  For instance, if you've defined
   HAVE_X_WINDOWS above and your X libraries aren't in a place that
   your loader can find on its own, you might want to add "-L/..." or
   something similar.  */
#define LD_SWITCH_X_SITE -L/usr/X/lib -lXbsd

/* Define C_SWITCH_X_SITE to contain any special flags your compiler
   may need to deal with X Windows.  For instance, if you've defined
   HAVE_X_WINDOWS above and your X include files aren't in a place
   that your compiler can find on its own, you might want to add
   "-I/..." or something similar.  */
#define C_SWITCH_X_SITE -I/opt/include

/* Define the return type of signal handlers if the s-xxx file
   did not already do so.  */
#define RETSIGTYPE void

/* SIGTYPE is the macro we actually use.  */
#ifndef SIGTYPE
#define SIGTYPE RETSIGTYPE
#endif

/* The rest of the code currently tests the CPP symbol BSTRING.
   Override any claims made by the system-description files.  */
/* #undef BSTRING */
#ifdef HAVE_BCOPY
#define BSTRING
#endif

/* Non-ANSI C compilers usually don't have volatile.  */
#ifndef HAVE_VOLATILE
#ifndef __STDC__
#define volatile
#endif
#endif

#if 0 /* This should be taken care of by autoconf now.  */
/* joe@zircon.uucp says that in order to use XFree386, you have to
   link against -lXbsd, which insists on defining the random
   function.
   faith@cs.unc.edu says this is bogus for Linux and several other OS's.
   Eventually, we should have autoconf test for this.  Anyone want to
   submit a patch?  */
#if defined (HAVE_XFREE386) && !defined (LINUX)
#define LIBX11_SYSTEM -lXbsd
#define HAVE_RANDOM
#endif
#endif

/* Some of the files of Emacs which are intended for use with other
   programs assume that if you have a config.h file, you must declare
   the type of getenv.

   This declaration shouldn't appear when alloca.s or ymakefile
   includes config.h.  */
#ifndef NOT_C_CODE
extern char *getenv ();
#endif

#endif /* EMACS_CONFIG_H */
