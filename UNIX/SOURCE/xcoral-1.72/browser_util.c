/*
** Copyright 1989, 1992 by Lionel Fournigault
**
** Permission to use, copy, and distribute for non-commercial purposes,
** is hereby granted without fee, providing that the above copyright
** notice appear in all copies and that both the copyright notice and this
** permission notice appear in supporting documentation.
** The software may be modified for your own purposes, but modified versions
** may not be distributed.
** This software is provided "as is" without any expressed or implied warranty.
**
**
*/

#include "result_types.h"
#include "browser_util.h"
#include <stdio.h>
#include <string.h>
#ifdef apollo
#include <stdlib.h>
#else
#include <malloc.h>
#endif

/*------------------------------------------------------------------------------
*/
char* xmalloc(size)
  int size;
{
  char* result;
   
  size = (size + 15) & 0xfffffff0;

  result = (char *) malloc(size);
  if (result == Null) {
    fprintf(stderr, ">>>>>>  Browser Error\n");
    fprintf(stderr, "    >>  Memory exhausted\n");
  }
  return(result);
}


/*------------------------------------------------------------------------------
*/
int sort_util(i,j)
  char** i;
  char** j;
{
  return(strcmp(*i, *j));
}


