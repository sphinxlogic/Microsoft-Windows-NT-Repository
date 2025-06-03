/********************************************************************
 * $Author: lindner $
 * $Revision: 1.1 $
 * $Date: 1992/12/10 23:27:52 $
 * $Source: /home/mudhoney/GopherSrc/release1.11/object/RCS/boolean.h,v $
 * $Status: $
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: boolean.h
 * Header file that defines a boolean type.
 *********************************************************************
 * Revision History:
 * $Log: boolean.h,v $
 * Revision 1.1  1992/12/10  23:27:52  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/


/*
 * Simple boolean type
 */

#ifndef BOOLEAN_H
#define BOOLEAN_H

#undef boolean  /** Just in case it's defined **/
typedef int boolean;
typedef int BOOLEAN;

#if !defined(TRUE) && !defined(FALSE)
#define TRUE (1)
#define FALSE (0)
#endif

#endif /* BOOLEAN_H */
