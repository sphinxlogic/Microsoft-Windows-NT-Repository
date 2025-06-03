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

/*
   template.c -- routines to read and manage a diary template.

   Author: Jason Baietto
     Date: November 9, 1990
*/


/*---------------------------------------------------------------------------*/
/*                               Header Files                                */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <ctype.h>
#include <values.h>
#include <X11/Intrinsic.h>


/*---------------------------------------------------------------------------*/
/*                          Global Types And Macros                          */
/*---------------------------------------------------------------------------*/
#define START_LENGTH 1000
#define GROW_LENGTH 1000
#define EMPTYSTRING ""



static template_length;

/*---------------------------------------------------------------------------*/
/*                              Static Routines                              */
/*---------------------------------------------------------------------------*/
static char * read_template(file_name)
char * file_name;
{
   int i = 0;
   int c = 0;
   char * char_ptr;
   long current_length = START_LENGTH;
   FILE * file;

   char_ptr = (char *) XtMalloc(current_length);

   file = fopen(file_name, "r");
   if (!file) {
      return ((char *) NULL);
   }

   /* Just read the whole thing into memory to minimize file I/O time */
   c = getc(file);
   while (c != EOF) {
      char_ptr[i++] = c;

      if (i >= current_length) {
         current_length += GROW_LENGTH;
         char_ptr = (char *) XtRealloc(char_ptr, current_length);
      }

      c = getc(file);
   }

   char_ptr[i] = NULL;   
   template_length = i;

   fclose(file);
   return(char_ptr);
}




static char * compressed_template;

static char * compress(uncompressed)
char * uncompressed;
{
   char * buffer = (char *) XtCalloc(template_length+1, sizeof(char));
   char * compressed = buffer;
   
   while (*uncompressed) {
      if (isspace(*uncompressed)) {
         uncompressed++;
      } else {
         *compressed = *uncompressed;
         compressed++;
         uncompressed++;
      }
   }

   return buffer;

}





/*---------------------------------------------------------------------------*/
/*                              Global Routines                              */
/*---------------------------------------------------------------------------*/
char * template;




int init_template(file_name)
char * file_name;
{
   template = read_template(file_name);

   if (!template) {
      /* The template specified does not exist. */
      return FALSE;
   }

   compressed_template = compress(template);
   return TRUE;
}




int entry_modified(entry)
char * entry;
{
   char * temp = compressed_template;

   while(*entry && *temp) {
      if (isspace(*entry)) {
         entry++;
      } else {
         if (*entry != *temp) {
            return TRUE;
         } else {
            entry++;
            temp++;
         }
      }
   }
   
   return FALSE;
}
