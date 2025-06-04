/********************************************************************
 Changes from Alan Coopersmith's patches:
    - add sequent to list of machines without waitpid()
    - use POSIX macros if present
 ********************************************************************/

/********************************************************************
 * lindner
 * 3.7
 * 1994/04/14 15:44:22
 * /home/mudhoney/GopherSrc/CVS/gopher+/object/Wait.h,v
 * Exp
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: Wait.h
 * Routines that portably define waitpid()
 *********************************************************************
 * Revision History:
 * Wait.h,v
 * Revision 3.7  1994/04/14  15:44:22  lindner
 * Add Gwaitstatus for VMS
 *
 * Revision 3.6  1994/04/08  19:15:46  lindner
 * Fix for old union wait stuff
 *
 * Revision 3.5  1994/03/08  03:23:23  lindner
 * Waitpid for vms
 *
 * Revision 3.4  1993/11/02  06:16:58  lindner
 * Fix for epix
 *
 * Revision 3.3  1993/07/20  23:19:41  lindner
 * Use waitpid, not wait3
 *
 * Revision 3.2  1993/05/05  18:40:16  lindner
 * Solaris mods
 *
 * Revision 3.1.1.1  1993/02/11  18:03:04  lindner
 * Gopher+1.2beta release
 *
 * Revision 1.1  1992/12/10  23:27:52  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/


/*
 * A header file to portably include the stuff for doing waitpid() etal
 */

#include "Stdlib.h"		/* for pid_t */

#ifdef VMS
#  define NO_WAITPID_WAIT3
#  include <processes.h>
#  define Portawait int
   pid_t waitpid();
#  define Gwaitstatus(a) (a)


#else

#  include <sys/ioctl.h>
#  include <sys/wait.h>
#  include <signal.h>

   /** These don't know what waitpid() is..  Naughty! **/

#  if defined(NeXT) || defined(SYSTYPE_BSD43) || defined(NO_WAITPID) || defined(sequent)
     typedef union wait Portawait;
#    define waitpid(a,b,c) wait3(b,c,NULL)
#    define pid_t int
#    define Gwaitstatus(a) ((a).w_status)
#  else
     /*** Everything else mostly does.. ***/
     typedef int Portawait;
#    if defined(WIFEXITED) && defined(WEXITSTATUS) && defined(WIFSIGNALED) && defined(WTERMSIG)
#      define Gwaitstatus(a) (WIFEXITED(a) ? WEXITSTATUS(a) : (WIFSIGNALED(a) ? WTERMSIG(a) : 0))
#     else
#      define Gwaitstatus(a) (a)
#    endif
#  endif
#endif /* VMS */

