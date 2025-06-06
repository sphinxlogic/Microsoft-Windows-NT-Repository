/* machine description file for tek4300.
   Copyright (C) 1988 Free Software Foundation, Inc.

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


/* The following line tells the configuration script what sort of 
   operating system this machine is likely to run.
   USUAL-OPSYS="bsd4-3"  */

/* The following three symbols give information on
 the size of various data types.  */

#define SHORTBITS 16		/* Number of bits in a short */

#define INTBITS 32		/* Number of bits in an int */

#define LONGBITS 32		/* Number of bits in a long */

/* 68000 has lowest-numbered byte as most significant */

/* This value matches the value the system gives it.  */
#define BIG_ENDIAN 4321

/* Define NO_ARG_ARRAY if you cannot take the address of the first of a
 * group of arguments and treat it as an array of the arguments.  */

#undef NO_ARG_ARRAY

/* Define WORD_MACHINE if addresses and such have
 * to be corrected before they can be used as byte counts.  */

#undef WORD_MACHINE

/* Now define a symbol for the cpu type, if your compiler
   does not define it automatically.  */

#define tek4300

/* Use type int rather than a union, to represent Lisp_Object */

#define NO_UNION_TYPE

/* Data type of load average, as read out of kmem.  */

#define LOAD_AVE_TYPE long

/* Convert that into an integer that is 100 for a load average of 1.0  */

#define LOAD_AVE_CVT(x) (x)

/* Define NO_REMAP if memory segmentation makes it not work well
   to change the boundary between the text section and data section
   when Emacs is dumped.  If you define this, the preloaded Lisp
   code will not be sharable; but that's better than failing completely.  */

#define NO_REMAP

/* Define C_ALLOCA if this machine does not support a true alloca
   and the one written in C should be used instead. */

#define C_ALLOCA

/* setjmp and longjmp can safely replace _setjmp and _longjmp, */

#define _longjmp longjmp
#define _setjmp setjmp

/* The text segment always starts at a fixed address.
   This way we don't need to have a label _start defined.  */

#define TEXT_START 0

/* The Tektronix exec struct for ZMAGIC files is struct zexec */

#define EXEC_HDR_TYPE struct zexec

/* The entry-point label (start of text segment) is `start', not `__start'.  */

#define DEFAULT_ENTRY_ADDRESS start

/* Use the system's malloc calls, gmalloc.c won't work for us. */

#define SYSTEM_MALLOC

/* In building xmakefile, "cc -E -g" forcibly reads from stdin.  Since we
   can't remove the CFLAGS from that "cc -E" invocation, make sure we
   never pass -g.  If you want to debug, remove the following, and fix
   src/Makefile.in so it doesn't pass ${CFLAGS} when creating xmakefile. */

#define C_DEBUG_SWITCH

/* eirik@elf.ithaca.ny.us said this was needed in 19.22.  */
#define NO_MODE_T
