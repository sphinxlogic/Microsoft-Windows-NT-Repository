/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "buffer.h"
#include "ins.h"
#include "misc.h"
#include "paste.h"
#include "undo.h"
#include "line.h"

/* breaks line at current point leaving point at end of current line */

int split_line (void)
{
    int size;
   
   if (CLine == NULL)
     {
	exit_error("split_line: Null Line", 1);
     }
   size = CLine->len - Point;

   if (NULL == make_line(size + 1))
     {
	exit_error("Allocation Failure in split_line", 0);
     }

   MEMCPY((char *) CLine->data, (char *) (CLine->prev->data + Point), size);
   CLine->len = size;
   CLine = CLine->prev;  LineNum--;
   CLine->len = Point;
   remake_line(Point + 1);
   /* now update the marks */
   update_marks(NLINSERT, 1);
   record_newline_insertion();
   return(0);
}

void splice_line (void)
{
   int n1, n2;

   if (CLine->next == NULL)
     {
	exit_error("splice line: next line is Null", 1);
     }
   push_spot();
   n1 = CLine->len;
   n2 = CLine->next->len;
   
   if (n1 + n2 > CLine->space) remake_line(n1 + n2 + 1);
   MEMCPY((char *) (CLine->data + Point), (char *) CLine->next->data, n2);
   CLine->len = n1 + n2;

   CLine = CLine->next;  LineNum++;
   update_marks(NLDELETE, 1);
   delete_line();
   pop_spot();
}

