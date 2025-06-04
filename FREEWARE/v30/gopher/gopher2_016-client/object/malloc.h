/********************************************************************
 * lindner
 * 3.5
 * 1994/05/24 05:42:17
 * /home/mudhoney/GopherSrc/CVS/gopher+/object/Malloc.h,v
 * $Status: $
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: Malloc.h
 * Portable method of getting malloc definitions
 *********************************************************************
 * Revision History:
 * Malloc.h,v
 * Revision 3.5  1994/05/24  05:42:17  lindner
 * Fix for old field version of DECC
 *
 * Revision 3.4  1994/05/19  14:25:24  lindner
 * faster VMS VAXC malloc
 *
 * Revision 3.3  1993/10/19  20:46:35  lindner
 * use stdio.h in Malloc.h to get NULL if necessary
 *
 * Revision 3.2  1993/10/11  17:23:32  lindner
 * Better malloc fix for systems without NULL
 *
 * Revision 3.1.1.1  1993/02/11  18:03:03  lindner
 * Gopher+1.2beta release
 *
 * Revision 1.1  1992/12/10  23:27:52  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/

#ifndef G_MALLOC_H

/*
 * Just enough to get us memory allocation and NULL
 * 
 * Ick, portable code is ugly!
 */

#include "Stdlib.h"

/*
 * Make sure we have NULL
 */

#if defined(NeXT) || defined(__STDC__)
#include <stdlib.h>
#endif

/* Usually can get NULL from stdio.h */
#ifndef NULL
#include <stdio.h>
#endif

/* Punt! */
#ifndef NULL  
#define NULL ((void*) 0)
#endif


/* memory management for VAXC */

#if defined(VMS) && defined(VAXC) && !defined(__DECC)
#  include <stdlib.h>
#  define malloc  VAXC$MALLOC_OPT
#  define calloc  VAXC$CALLOC_OPT
#  define free    VAXC$FREE_OPT
#  define cfree   VAXC$CFREE_OPT
#  define realloc VAXC$REALLOC_OPT
#endif /* VMS and VAXC */

#endif /* G_MALLOC_H */
