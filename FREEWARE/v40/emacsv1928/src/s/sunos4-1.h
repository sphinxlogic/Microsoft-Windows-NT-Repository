#include "sunos4-0.h"

/* 4.1.1 makes these system calls interruptible.  */

#define read sys_read
#define write sys_write
#define open sys_open
#define close sys_close

#define INTERRUPTIBLE_OPEN
#define INTERRUPTIBLE_CLOSE
#define INTERRUPTIBLE_IO

/* Cause the compilation of oldxmenu to use the right -I option.  */
#define OLDXMENU_OPTIONS CFLAGS=C_SWITCH_SYSTEM

#if 0 /* This isn't right.  Apparently some sites do have -lresolv
	 but don't use that.  On those systems, the code below loses.
	 There's no way to win automatically unless someone
	 figures out a way of determining automatically which way is right
	 on any given system.  */
/* Some systems do not run the Network Information Service, but have
   modified the shared C library to include resolver support without
   also changing the C archive library (/usr/lib/libc.a).  If we
   detect the presence of libresolv.a, use -lresolv to supplement libc.a.

   We used to have #ifdef HAVE_GETHOSTNAME is to prevent configure from
   setting libsrc_libs to -lresolv in lib-src/Makefile.  But nowadays
   configure is smarter about computing libsrc_libs, and would not
   be fooled.  Anyway, why not use -lresolv in lib-src?  */
/* #ifdef HAVE_GETHOSTNAME */
#ifdef HAVE_LIBRESOLV
#define LIBS_SYSTEM -lresolv
#endif
/* #endif */
#endif

#if 0 /* Not necessary, since SYSTEM_MALLOC is defined in sunos4-0.h.  */
/* Tell GNU malloc to compensate for a bug in localtime.  */
#define SUNOS_LOCALTIME_BUG
#endif
