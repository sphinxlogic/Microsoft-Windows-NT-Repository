/********************************************************************
 * $Author: lindner $
 * $Revision: 1.2 $
 * $Date: 1992/12/21 20:04:04 $
 * $Source: /home/mudhoney/GopherSrc/release1.11/object/RCS/DAarray.h,v $
 * $State: Rel $
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: DAarray.h
 * Dynamic Array Header file/abstraction
 *********************************************************************
 * Revision History:
 * $Log: DAarray.h,v $
 * Revision 1.2  1992/12/21  20:04:04  lindner
 * Added DAcpy()
 *
 * Revision 1.1  1992/12/10  23:27:52  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/


#ifndef DAARRAY_H
#define DAARRAY_H

/*
 *  A dynamic array class
 */ 

struct da_struct {
     char **objects;  /** Should be void** perhaps */
     
     int Top;
     int maxsize;

     char * (*newfn)();
     void   (*initfn)();
     void   (*destroyfn)();
     char * (*copyfn)();
};

typedef struct da_struct DynArray;

#define DAgetEntry(a,b)   (((a)->objects[b]))
#define DAgetTop(a)       ((a)->Top)
#define DAsetTop(a,b)     ((a)->Top=(b))
#define DAgetNumitems(a)   ((a)->Top)

DynArray *DAnew();
void     DAdestroy();
void     DAinit();
void     DApush();
char *   DApop();
void     DAsort();
void     DAgrow();
void     DAsort();
void     DAcpy(/* dest, orig */);
#endif

