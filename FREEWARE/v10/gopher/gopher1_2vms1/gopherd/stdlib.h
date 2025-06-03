/********************************************************************
 * $Author: lindner $
 * $Revision: 1.2 $
 * $Date: 1992/12/29 21:00:59 $
 * $Source: /home/mudhoney/GopherSrc/release1.11/object/RCS/Stdlib.h,v $
 * $State: Rel $
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: Stdlib.h
 * Include stdlib.h on systems that have it.
 *********************************************************************
 * Revision History:
 * $Log: Stdlib.h,v $
 * Revision 1.2  1992/12/29  21:00:59  lindner
 * Added changes necessary for VMS
 *
 * Revision 1.1  1992/12/10  23:27:52  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/


#if defined (NeXT) || defined(__STDC__)
#  ifndef sony_news   /** Ack! sonynews is STDC but no stdlib! **/
#    include <stdlib.h>
#  endif
#else

#  if !defined(mips) && !defined(sequent) && !defined(n16) && !defined(NeXT) &&   !defined(ultrix) && !defined(UMAX43) && !defined(sony_news)

#    ifndef VMS
#      include <stdlib.h>
#    else
#      include stdlib
#    endif

#  endif
#endif
