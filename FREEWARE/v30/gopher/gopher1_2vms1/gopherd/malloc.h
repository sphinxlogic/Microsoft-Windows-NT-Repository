/********************************************************************
 * $Author: lindner $
 * $Revision: 1.1 $
 * $Date: 1992/12/10 23:27:52 $
 * $Source: /home/mudhoney/GopherSrc/release1.11/object/RCS/Malloc.h,v $
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
 * $Log: Malloc.h,v $
 * Revision 1.1  1992/12/10  23:27:52  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/



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

#ifndef NULL
#define NULL ((void *) 0)
#endif
