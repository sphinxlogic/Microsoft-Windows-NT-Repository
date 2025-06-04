/* my_malloc.h : memory allocation routines with error checking
**
** Written and Copyright (C) 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#ifndef _MY_MALLOC_H__INCLUDED_
#define _MY_MALLOC_H__INCLUDED_

#ifdef NOTDEF
extern char * my_malloc(unsigned size, char *proc);
#endif

extern char * my_calloc(unsigned nelem, unsigned elsize, char *proc);

extern void my_free(void *ptr, char *proc);

#define MY_CALLOC(nelem,type,proc) ((type*)my_calloc(nelem,sizeof(type),proc))

#endif
