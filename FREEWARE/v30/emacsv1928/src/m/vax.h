/* machine description file for vax.
   Copyright (C) 1985, 1986 Free Software Foundation, Inc.

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
   USUAL-OPSYS="note"

NOTE-START
The vax (-machine=vax) runs zillions of different operating systems.

Vax running Berkeley Unix (-opsystem=bsd4-1, -opsystem=bsd4-2 or
			   -opsystem=bsd4-3)

  Works.

Vax running Ultrix (-opsystem=bsd4-2)

  Works.  See under Ultrix in share-lib/MACHINES for problems using X
  windows on Ultrix.

Vax running System V rel 2 (-opsystem=usg5-2)

  18.27 Works.

Vax running System V rel 0 (-opsystem=usg5-0)

  Works as of 18.36.

Vax running VMS (-opsystem=vms)

  18.36 believed to work.  Addition of features is necessary to make
  this Emacs version more usable.

NOTE-END  */

/* The following three symbols give information on
 the size of various data types.  */

#define SHORTBITS 16		/* Number of bits in a short */

#define INTBITS 32		/* Number of bits in an int */

#define LONGBITS 32		/* Number of bits in a long */

/* Vax is not big-endian: lowest numbered byte is least significant. */

/* #undef BIG_ENDIAN */

/* #define vax    -- appears to be done automatically  */

/* Use type int rather than a union, to represent Lisp_Object */

#define NO_UNION_TYPE

/* crt0.c should use the vax-bsd style of entry, with no dummy args.  */

#define CRT0_DUMMIES

/* crt0.c should define a symbol `start' and do .globl with a dot.  */

#define DOT_GLOBAL_START

#ifdef BSD
/* USG systems I know of running on Vaxes do not actually
   support the load average, so disable it for them.  */

/* Data type of load average, as read out of kmem.  */

#define LOAD_AVE_TYPE double

/* Convert that into an integer that is 100 for a load average of 1.0  */

#define LOAD_AVE_CVT(x) ((int) ((x) * 100.0))

#endif /* BSD */

#ifdef VMS

/* Data type of load average, as read out of driver.  */

#define LOAD_AVE_TYPE float

/* Convert that into an integer that is 100 for a load average of 1.0  */

#define LOAD_AVE_CVT(x) ((int) ((x) * 100.0))

#endif /* VMS */

/* Vax sysV has alloca in the PW library.  */

#ifdef USG
#define LIB_STANDARD -lPW -lc
#define HAVE_ALLOCA

/* There is some bug in unexec in for usg 5.2 on a vax
   which nobody who runs such a system has yet tracked down. */
#ifndef USG5_0
#define NO_REMAP
#endif /* USG 5_0 */

#define TEXT_START 0
#endif /* USG */

#ifdef BSD
#define HAVE_ALLOCA
#endif /* BSD */

#ifdef VMS
#ifdef __DECC
/* It's incredibble what number of error messages you get if you don't
   have this... */
#define C_SWITCH_SYSTEM /standard=vaxc \
/WARNING=(DISABLE=(ADDRCONSTEXT,GLOBALEXT,LONGEXTERN)) /prefix=all
#if 0
#define LIBS_MACHINE sys$share:decc$shr/share
#endif
#else /* not __DECC */
#define LIBS_MACHINE sys$share:vaxcrtl/share
#endif /* __DECC */
#endif /* VMS */

#ifdef BSD4_2
#define HAVE_FTIME
#endif
