/********************************************************************
 * lindner
 * 3.6
 * 1994/03/17 04:37:12
 * /home/mudhoney/GopherSrc/CVS/gopher+/object/Dirent.h,v
 * Exp
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: Dirent.h
 * Portably include dir.h dirent.h, etc.
 *********************************************************************
 * Revision History:
 * Dirent.h,v
 * Revision 3.6  1994/03/17  04:37:12  lindner
 * Lame VMS port
 *
 * Revision 3.5  1993/12/27  16:12:07  lindner
 * Mods for Apollo Domain systems
 *
 * Revision 3.4  1993/09/30  23:18:25  lindner
 * pl8
 *
 * Revision 3.3  1993/07/13  04:03:59  lindner
 * added 386BSD
 *
 * Revision 3.2  1993/05/25  22:02:10  lindner
 * Fix problems with CDC epix systems
 *
 * Revision 3.1.1.1  1993/02/11  18:03:02  lindner
 * Gopher+1.2beta release
 *
 * Revision 1.1  1992/12/10  23:27:52  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/

#if !defined(GDIRENT_H) && !defined(VMS)
#define GDIRENT_H

/*** These need dir.h for sure ***/

#if defined(NeXT) || defined(n16) || defined(sequent) || defined(apollo)|| defined(NO_DIRENT)
#  include <sys/types.h>
#  include <sys/dir.h>
#  define  dirent direct
#  if defined(apollo)
#     include <sys/stat.h>
#  else
#    define S_ISDIR(m)      (((m)&S_IFMT) == S_IFDIR)
#    define S_ISREG(m)      (((m)&S_IFMT) == S_IFREG)
#  endif

/*** These can use dirent for sure ***/

#else
#  if defined(sun) || defined(ultrix) || defined(hpux) || defined(__sgi)
#    undef NO_DIRENT
#  endif


#  if defined(_AIX) || defined(_SYSV_SOURCE) || defined(sgi) || defined(cray)
#    undef NO_DIRENT
#  endif

#  if defined (_POSIX_SOURCE) || defined(_SEQUENT_) || defined(M_XENIX)
#    undef NO_DIRENT
#  endif

#  if defined(__386BSD__)
#    undef NO_DIRENT
#  endif

/********************************************************************/

#  if !defined(NO_DIRENT) 
#      include <dirent.h>

/*** Everyone else gets dir.h, plus some funky definitions ***/

#  else
#    include <sys/types.h>
#    include <sys/dir.h>
#    define  dirent direct
#    define S_ISDIR(m)      (((m)&S_IFMT) == S_IFDIR)
#    define S_ISREG(m)      (((m)&S_IFMT) == S_IFREG)
#  endif
#endif


#endif /* GDIRENT_H */
