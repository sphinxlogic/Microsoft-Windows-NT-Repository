/********************************************************************
 * $Author: lindner $
 * $Revision: 1.1 $
 * $Date: 1992/12/10 23:27:52 $
 * $Source: /home/mudhoney/GopherSrc/release1.11/object/RCS/GDgopherdir.h,v $
 * $Status: $
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: GDgopherdir.h
 * Header file/abstraction of a gopher directory
 *********************************************************************
 * Revision History:
 * $Log: GDgopherdir.h,v $
 * Revision 1.1  1992/12/10  23:27:52  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/

#ifndef GDGOPHERDIR_H
#define GDGOPHERDIR_H
#include "GSgopherobj.h"
#include "DAarray.h"

/****************************************************************
** A Gopher directory structure...
** Features dynamic growth among other things...
**
*****************************************************************/

struct g_dir_struct {
     String *Title;

     DynArray *Gophers;
     
     int currentitem;
};

typedef struct g_dir_struct GopherDirObj;
typedef struct g_dir_struct *GopherDirObjp;

#define GDgetEntry(a,b)    ((GopherObj *)(DAgetEntry((a)->Gophers,(b))))
#define GDgetTitle(a)      (STRget((a)->Title))
#define GDsetTitle(a,b)    (STRset((a)->Title,b))
#define GDgetNumitems(a)   (DAgetNumitems((a)->Gophers))

#define GDsetCurrentItem(a,b) ((a)->currentitem=b)
#define GDgetCurrentItem(a) ((a)->currentitem)

/*** Real live functions declared in GDgopherdir.c ***/

GopherDirObj *GDnew();
void         GDdestroy();
void         GDinit();
void         GDaddGS();
void         GDsort();
void         GDtoNet();
void         GDtoNetHTML();
int          GDfromNet();
void         GDgrow();
void         GDfromLink();
void         GDtoLink();
#endif /* GDGOPHERDIR_H */






