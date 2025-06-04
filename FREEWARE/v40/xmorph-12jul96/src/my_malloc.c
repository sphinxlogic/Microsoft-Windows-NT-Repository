/* my_malloc.c : memory allocation routines with error checking
**
** Written and Copyright (C) 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#include <stdio.h>
#include <stdlib.h>
#ifndef VMS
#include <malloc.h>
#endif
#include "my_malloc.h"

#ifdef NOTDEF
char *my_malloc(unsigned size, char *proc)
{
  char *mem;
#ifdef SUNOS
  malloc_verify();
#endif
  if((mem=malloc(size))==NULL) {
    fprintf(stderr, "Bad Alloc: %li in %s\n", size, proc);
  }

#ifdef SUNOS
  malloc_verify();
#endif
  return(mem);
}
#endif


#define EXTRA (nelem)

char *my_calloc(unsigned nelem, unsigned elsize, char *proc)
{
  char *mem;
#ifdef SUNOS
  malloc_verify();
#endif
  if((mem=calloc(nelem + EXTRA, elsize))==NULL) {
    fprintf(stderr, "Bad Alloc: %li x %li in %s\n", nelem, elsize, proc);
  }

#ifdef DEBUG_ALLOC
  printf("allocated %li at %p from %s\n", nelem*elsize, mem, proc);
  fflush(stdout);
#endif

#ifdef SUNOS
  malloc_verify();
#endif
  return(mem);
}

void my_free(void *ptr, char *proc)
{
#ifdef SUNOS
  malloc_verify();
#endif

#ifdef DEBUG_ALLOC
  printf("freeing %p from %s\n", ptr, proc);
  fflush(stdout);
#endif

  free(ptr);
#ifdef SUNOS
  malloc_verify();
#endif
  return;
}
