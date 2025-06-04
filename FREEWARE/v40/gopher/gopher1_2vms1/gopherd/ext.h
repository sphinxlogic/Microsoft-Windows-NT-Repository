/********************************************************************
 * $Author: lindner $
 * $Revision: 1.1 $
 * $Date: 1992/12/10 23:13:27 $
 * $Source: /home/mudhoney/GopherSrc/release1.11/gopherd/RCS/ext.h,v $
 * $Status: $
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: ext.h
 * Header file and access methods for filename extension routines
 *********************************************************************
 * Revision History:
 * $Log: ext.h,v $
 * Revision 1.1  1992/12/10  23:13:27  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/


#ifndef EXT_H
#define EXT_H

#include "DAarray.h"
#include "STRstring.h"
#include "boolean.h"

/****************************************************
 * A structure for mapping filenames to gopher types, and G+types
 */

struct Ext_struct {
     char   Objtype;
     String *GplusType;
     String *Prefix;
     String *ext;
};

typedef struct Ext_struct Ext;
typedef DynArray ExtArray;

#define ExtgetEntry(a,b) (Ext *)(DAgetEntry((DynArray*)a,b))
#define ExtArrDestroy(a) (DAdestroy(a))

ExtArray *ExtArrayNew();
void      ExtAdd(/*extarr, objtype, gplustype, prefix, fileext */);
void      ExtGet(/*extarr, fileext, objtype, gplustype, prefix */);
boolean   ExtProcessLine(/*extarr, inputline*/);

#endif /** EXT_H **/

