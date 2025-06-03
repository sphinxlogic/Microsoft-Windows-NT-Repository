/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "buffer.h"
#include "replace.h"
#include "search.h"
#include "screen.h"
#include "ins.h"

int replace_next(char *old, char *neew)
{
   int n;

    if (search(old, 1, 0) == 0) return(0);

   n = strlen(old);
   deln(&n);
   ins_chars((unsigned char *) neew, strlen(neew));
   return(1);
}

