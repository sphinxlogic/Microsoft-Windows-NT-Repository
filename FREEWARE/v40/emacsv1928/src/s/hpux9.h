/* System description file for hpux version 9.  */

#include "hpux8.h"

#define HPUX9

/* If Emacs doesn't seem to work when built to use GNU malloc, you
   probably need to get the latest patches to the HP/UX compiler.
   See `etc/MACHINES' for more information.  */
#if 0
#define SYSTEM_MALLOC 1
#undef GNU_MALLOC
#undef REL_ALLOC
#endif

#ifndef __GNUC__
/* Make room for enough symbols, so dispnew.c does not fail.  */
#define C_SWITCH_SYSTEM -Wp,-H200000 -D_BSD
#endif

#if 0 /* These definitions run into a bug in hpux
	 whereby trying to disable the vdsusp character has no effect.
	 supposedly there is no particular need for this.  */
/* neal@ctd.comsat.com */
#undef HAVE_TERMIO
#define HAVE_TERMIOS
#define NO_TERMIO
#endif
