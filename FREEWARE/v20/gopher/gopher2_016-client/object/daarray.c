/********************************************************************
 * lindner
 * 3.2
 * 1993/03/26 19:52:10
 * /home/mudhoney/GopherSrc/CVS/gopher+/object/DAarray.c,v
 * Exp
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: DAarray.c
 * Dynamic Array Implementation
 *********************************************************************
 * Revision History:
 * DAarray.c,v
 * Revision 3.2  1993/03/26  19:52:10  lindner
 * Fixed off by one prob in DApop
 *
 * Revision 3.1.1.1  1993/02/11  18:03:01  lindner
 * Gopher+1.2beta release
 *
 * Revision 1.3  1992/12/21  21:14:57  lindner
 * Fixed bug in DAcpy, initfn wasn't being copied right.
 *
 * Revision 1.2  1992/12/21  20:04:04  lindner
 * Added DAcpy()
 *
 * Revision 1.1  1992/12/10  23:27:52  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/


#include "DAarray.h"

#include "Malloc.h"

/*
 * Create a new dynamic array
 *
 * size      -- the initial number of elements
 * newfn     -- creates new object
 * initfn    -- initializes the object
 * destroyfn -- performs clean up and frees objs memory
 * copyfn    -- copys one obj to the other, like strcpy
 */

DynArray *
DAnew(size, newfn, initfn, destroyfn, copyfn)
  int size;
  char * (*newfn)();
  void   (*initfn)();
  void   (*destroyfn)();
  char * (*copyfn)();
{
     DynArray *temp;
     int i;
     
     temp = (DynArray*) malloc(sizeof(DynArray));
     temp->objects = (char **) malloc(size * sizeof(char *));
     temp->newfn     = newfn;
     temp->initfn    = initfn;
     temp->destroyfn = destroyfn;
     temp->copyfn    = copyfn;
     temp->Top       = 0;

     if (copyfn == NULL)
	  /** Can't work without this!!! ***/
	  perror("Egad, no copy function in DAnew()!!");

     if (newfn != NULL)
	  for (i = 0; i < size; i++)
	       temp->objects[i] = (char *) newfn();

     temp->maxsize = size;
     return(temp);
}


void DAdestroy(da)
  DynArray *da;
{
     int i;
     
     if (da->destroyfn != NULL)
	  for (i = 0; i< da->maxsize; i++)
	       da->destroyfn(da->objects[i]);

     free(da->objects);
     free(da);
}


void
DAinit(da)
  DynArray *da;
{
     int i;
     
     if (da->initfn!=NULL)
	  for (i=0; i<da->maxsize; i++)
	       da->initfn(DAgetEntry(da, i));

     DAsetTop(da, 0);
}

void
DAgrow(da, size)
  DynArray *da;
  int size;
{
     char **temp;
     int i;

     if (size < da->maxsize)
	  return; /** Size is smaller than requested **/

     temp = (char **) realloc(da->objects, size*sizeof(char*));
 
     if (temp == NULL)
	  perror("Out of memory!!!\n"), exit(-1);

     if (temp != da->objects) {
	  da->objects = temp;
     }

     /** Initialize the new objects.  **/

     for (i= da->maxsize; i< size; i++)
	  da->objects[i] = da->newfn();
     
     da->maxsize = size;
     return;
}

/*
 * Tacks an item on the end of the array, grows it if necessary..
 */

void
DApush(da, obj)
  DynArray *da;
  char *obj;
{
     int top;

     top = DAgetTop(da);
     
     if (top == da->maxsize)
	  DAgrow(da, da->maxsize*2);

     da->copyfn(DAgetEntry(da, top), obj);

     DAsetTop(da, top+1);	        	/* update end of list */
}

char *
DApop(da)
  DynArray *da;
{
     int top;
     char *newobj;
     
     top = DAgetTop(da);

     if (top == 0)
	  return(NULL);  /** Nothing to pop! **/

     newobj = da->newfn();
     da->initfn(newobj);
     
     top--;
     da->copyfn(newobj, DAgetEntry(da, top));

     DAsetTop(da, top);

     return(newobj);
}

void 
DAsort(da, sortfn)
  DynArray *da;
  int (*sortfn)();
{
     char *moo;

     moo = (char *) &((da->objects)[0]);
     
     qsort(moo, da->Top,
	   sizeof(char *), sortfn);
}


void
DAcpy(dest, orig)
  DynArray *dest;
  DynArray *orig;
{
     int i;

     DAsetTop(dest, 0);
     dest->newfn = orig->newfn;
     dest->initfn = orig->initfn;
     dest->destroyfn = orig->destroyfn;
     dest->copyfn = dest->copyfn;

     for (i=0; i<DAgetTop(orig); i++) {
	  DApush(dest, DAgetEntry(orig,i));
     }
}
#ifdef DA_TEST

#include <stdio.h>
#include <string.h>

char *makespace()
{
     return(malloc(256));
}

int DEBUG = 1;

int
moostrcmp(s1, s2)
  char **s1, **s2;
{
     printf("Comparing %s, %s\n", *s1,*s2);
     return(strcmp(*s1,*s2));
}

main()
{
     DynArray *test;
     int i;
     char tempstr[100];

     printf("Testing Dynamic Arrays...\n\n");
     
     test = DAnew(5, makespace, NULL, NULL, strcpy);

     DApush(test, "ziggy\n");
     DApush(test, "iggy\n");
     
     for (i= 10; i >0; i--) {
	  sprintf(tempstr, "Moocow #%d\n", i);
	  printf(tempstr);
	  DApush(test, tempstr);
     }

     for (i=0; i< 10; i++) {
	  printf(DAgetEntry(test, i));
     }

     DAsort(test, moostrcmp);

     for (i=0; i< 10; i++) {
	  printf(DAgetEntry(test, i));
     }

}

#endif
