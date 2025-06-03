/********************************************************************
  Changes from Alan Coopersmith's patches:
    - add Location item to gopherdir struct to keep track of where
	the directory came from
 ********************************************************************/

/********************************************************************
 * lindner
 * 3.4
 * 1994/01/21 04:47:20
 * /home/mudhoney/GopherSrc/CVS/gopher+/object/GDgopherdir.h,v
 * Exp
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
 * GDgopherdir.h,v
 * Revision 3.4  1994/01/21  04:47:20  lindner
 * Add function prototypes
 *
 * Revision 3.3  1994/01/10  03:29:07  lindner
 * New Method GDsetNumitems()
 *
 * Revision 3.2  1993/04/15  21:35:59  lindner
 * Changed protos
 *
 * Revision 3.1.1.1  1993/02/11  18:03:03  lindner
 * Gopher+1.2beta release
 *
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

     GopherObj *Location;

     DynArray *Gophers;
     
     int currentitem;
};

typedef struct g_dir_struct GopherDirObj;
typedef struct g_dir_struct *GopherDirObjp;

#define GDgetEntry(a,b)    ((GopherObj *)(DAgetEntry((a)->Gophers,(b))))
#define GDgetTitle(a)      (STRget((a)->Title))
#define GDsetTitle(a,b)    (STRset((a)->Title,b))
#define GDgetLocation(a) ((a)->Location)
#define GDgetNumitems(a)   (DAgetNumitems((a)->Gophers))
#define GDsetNumitems(a,b) (DAsetTop((a)->Gophers,(b)))

#define GDsetCurrentItem(a,b) ((a)->currentitem=b)
#define GDgetCurrentItem(a) ((a)->currentitem)

/*** Real live functions declared in GDgopherdir.c ***/

GopherDirObj *GDnew();
void         GDdestroy();
void         GDinit();
void	     GDsetLocation();
void         GDaddGS();
void         GDaddGSmerge();
void         GDsort();
void         GDtoNet();
void         GDplustoNet();
void         GDfromNet();
int          GDplusfromNet();
void         GDgrow();
void         GDfromLink();
void         GDtoLink();
GopherDirObj *GDdeleteGS();
#endif /* GDGOPHERDIR_H */

