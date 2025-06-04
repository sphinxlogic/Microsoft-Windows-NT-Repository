/* Definitions file for GNU Emacs running on the GNU Hurd.
   Copyright (C) 1994 Free Software Foundation, Inc.

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


/* Get most of the stuff from bsd4.3 */
#include "bsd4-3.h"

/* For mem-limits.h.  */
#define BSD4_2

#undef SYSTEM_TYPE
#define SYSTEM_TYPE "gnu"

/* XXX should getloadavg be in libc?  Should we have a libutil?
#define HAVE_GETLOADAVG */

#define HAVE_UNION_WAIT

#define SIGNALS_VIA_CHARACTERS

#define HAVE_TERMIOS
#define NO_TERMIO

#define LIBS_DEBUG

/* XXX emacs should not expect TAB3 to be defined.  */
#define TABDLY OXTABS
#define TAB3 OXTABS


#define SYSV_SYSTEM_DIR

/* GNU has POSIX-style pgrp behavior.  */
#undef BSD_PGRPS

/* Reread the time zone on startup.  */
#define LOCALTIME_CACHE

#define HAVE_WAIT_HEADER

/* GNU needs its own crt0, and libc defines data_start.  */
#define ORDINARY_LINK
#define DATA_START ({ extern int data_start; (char *) &data_start; })

/* It is not safe to move the text/data boundary because in GNU the C
   library will need some data and bss of its own, but which must come
   after Emacs's data because it is linked in to satisfy references Emacs
   makes.  Fortunately, in GNU it is also no loss for the pure Lisp data to
   be in data instead of text, because it's all mapped copy-on-write.  */
#define NO_REMAP

/* For the i386-mach a.out format: */
#define A_TEXT_OFFSET(x) (sizeof (struct exec))
#define A_TEXT_SEEK(hdr) (N_TXTOFF(hdr) + A_TEXT_OFFSET(hdr))


/* ptys are not done yet.  */
#undef HAVE_PTYS

/* Until we finish the network.  */
#undef HAVE_SOCKETS
