/*
 * Author: Jason Baietto, jason@ssd.csd.harris.com
 * xdiary Copyright 1990 Harris Corporation
 *
 * Permission to use, copy, modify, and distribute, this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holder be used in
 * advertising or publicity pertaining to distribution of the software with
 * specific, written prior permission, and that no fee is charged for further
 * distribution of this software, or any modifications thereof.  The copyright
 * holder makes no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, AND IN NO
 * EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM ITS USE,
 * LOSS OF DATA, PROFITS, QPA OR GPA, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH
 * THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>
#include "StrTableP.h"

/* Copy a string, replacing two consecutive double quotes with a single. */
static void string_copy(dest_string, source_string, length)
char * dest_string;
char * source_string;
int length;
{
   char * dest_ptr = dest_string;
   char * src_ptr = source_string;
   char * end_ptr = src_ptr + length;

   while (src_ptr < end_ptr) {
      if (*src_ptr == QUOTE) {
         src_ptr++;
      }
      *dest_ptr = *src_ptr;
      dest_ptr++;
      src_ptr++;
   }
   
   /* Null terminate the string */
   *dest_ptr = NULL;
}


/* Convert a string containing quoted strings into a string table.   */
/* Two consecutive quotes inside a "string" become a single quote.   */
/* This algorithm just ignores all characters between each "string". */
/* Not the most robust thing I ever wrote, but it'll do the job.     */

/*ARGSUSED*/
void StringTableConverter(args, num_args, fromVal, toVal)
XrmValuePtr args;
Cardinal    *num_args;
XrmValuePtr fromVal;
XrmValuePtr toVal;
{
   static char ** array;
   int num_allocated;
   int token_len;
   char * beg_ptr = (char *) fromVal->addr;
   char * cur_ptr = beg_ptr;
   int token_num = 0;
   int in_token = FALSE;

   num_allocated = INIT_SIZE;
   array = (char **) XtMalloc(num_allocated*sizeof(char *));

   while (*cur_ptr) {
      if (*cur_ptr == QUOTE) {
         if (in_token) {
            if (*(cur_ptr+sizeof(char)) == QUOTE) {
               /* Two consecutive quotes become one quote. */
               cur_ptr++;
            } else {
               /* Finished with this token, ignore final quote. */
               token_len = cur_ptr - beg_ptr;
               array[token_num] = (char*)XtMalloc((token_len+1)*sizeof(char));
               string_copy(array[token_num], beg_ptr, token_len);
               in_token = FALSE;
               token_num++;

               /* Allocate more array space if necessary. */
               if (token_num == num_allocated) {
                  num_allocated += GROW_BY;
                  array = (char**)XtRealloc(array, num_allocated*sizeof(char*));
               }
            }

         } else {
            /* We've got a new token, skip leading quote */
            beg_ptr = cur_ptr;
            beg_ptr++;
            in_token = TRUE;
         }
      }
      cur_ptr++;
   }

   if (in_token) {
      /* Exited above loop still in a token. */
      /*VARARGS*/
      fprintf(stderr,
         "StringTableConverter: Unmatched quote in resource string.\n"
      );
      toVal->addr = NULL;
      toVal->size = 0;
   } else {
      /* Null terminate the array. */
      array[token_num] = NULL;

      /* Load the return value. */
      toVal->addr = (caddr_t) &array;
      toVal->size = sizeof(array);
   }
}


/* Return the number of strings in a string table. */
int StringTableNumber(table)
StringTable table;
{
   int number = -1;
   while (table[++number]);
   return number;
}


/* Dump the specified string table.  For debugging only. */
void StringTableDump(table)
StringTable table;
{
   int number = -1;
   while (table[++number]) {
      /*VARARGS*/
      printf("%d: %s\n", number, table[number]);
   }
}



/* Create new array with old contents. */
StringTable StringTableCopy(old)
StringTable old;
{
   int i;
   int length = StringTableNumber(old) + 1;
   StringTable new = (StringTable) XtMalloc(length * sizeof(char *));

   for (i=0; i < length; i++) {   
      new[i] = old[i];
   }

   return new;
}
