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
#include <ctype.h>
#include <values.h>
#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>
#include "DateP.h"
#include "StrTable.h"
#include "common.h"

static void process_digit_token();
static void process_alpha_token();
static int convert_month_string_to_number();
static int compare_string();
static void construct_date();

char * default_month_names[] = {
   "january", "february", "march", "april", "may", "june", "july",
   "august", "september", "october", "november", "december" };

static int days_in_month[] = 
   /* jan feb mar apr may jun jul aug sep oct nov dec */
   {  0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31  };


static StringTable month_names = default_month_names;

static int token[3];
static int state[3];
static int count;

#define STRING (0)
#define NUMBER (1)

static char * beg_ptr;
static char * cur_ptr;
static Date date;

static int european;

Date convert_string_to_date(table, string)
StringTable table;
char * string;
{
   int in_alpha_token = FALSE;
   int in_digit_token = FALSE;
   date.month = date.day = date.year = 0;
   beg_ptr = cur_ptr = string;
   count = 0;
   european = FALSE;

   /* Was a string table specified? */
   if (table) {
      month_names = table;
   }

   while (*cur_ptr) {
      if (isalpha(*cur_ptr)) {
         if (in_digit_token) {
            /* Finished processing a digit token. */
            process_digit_token();
            in_digit_token = FALSE;
         } else if (in_alpha_token) {
            /* Still scanning an alpha token. */
         } else {
            /* Start processing an alpha token. */
            in_alpha_token = TRUE;
            beg_ptr = cur_ptr;
         }
      } else if (isdigit(*cur_ptr)) {
         if (in_alpha_token) {
            /* Finished processing a digit token. */
            process_digit_token();
            in_digit_token = FALSE;
         } else if (in_digit_token) {
            /* Still scannin a digit token. */
         } else {
            /* Start processing a digit token. */
            in_digit_token = TRUE;
            beg_ptr = cur_ptr;
         }
      } else {
         if (in_alpha_token) {
            /* Finished processing an alpha token. */
            process_alpha_token();
            in_alpha_token = FALSE;
         } else if (in_digit_token) {
            /* Finished processing a digit token. */
            process_digit_token();
            in_digit_token = FALSE;
         }

         /* Ignore garbage between tokens, but look for periods. */
         if (*cur_ptr == '.') {
            european = TRUE;
         }
      }
      cur_ptr++;
   }/*while*/

   if (in_alpha_token) {
      process_alpha_token();
   } else if (in_digit_token) {
      process_digit_token();
   }

   construct_date();

   if (!is_valid_date(&date)) {
      date.month = 0;
      date.day   = 0;
      date.year  = 0;
   }

   return date;
}



static void process_alpha_token()
{
   if (count < 3) {
      token[count] = convert_month_string_to_number();
      state[count] = STRING;
   }
   count++;
}



static void process_digit_token()
{
   if (count < 3) {
      token[count] = atoi(beg_ptr);
      if (token[count] > MAXSHORT) {
         token[count] = 0;
      }
      state[count] = NUMBER;
   }
   count++;
}



static int convert_month_string_to_number()
{
   int month;
   for (month = 0; month < 12; month ++) {
      if (compare_string(month_names[month])) {
         return month+1;
      }
   }

   /* Nothing matched. */
   return 0;
}



static int compare_string(string)
char * string;
{
   char * token_ptr = beg_ptr;
   char * string_ptr = string;

   while (token_ptr < cur_ptr) {
      if (DOWNCASE(*string_ptr) == DOWNCASE(*token_ptr)) {
         /* Letters match (case ignored) */
         token_ptr++;
         string_ptr++;
      } else {
         /* Doesn't match. */
         break;
      }
   }

   if (token_ptr == cur_ptr) {
      /* We got to the end without a mismatch. */
      return TRUE;
   } else {
      /* String and token didn't match. */
      return FALSE;
   }
   
}




static void construct_date()
{
   date.day   = 0;
   date.month = 0;
   date.year  = 0;


   if (count == 1) {

      if (state[0] == NUMBER) {
         /* number, done */
      } else {
         /* string, error */
         return;
      }

      date.day   = 0;
      date.month = 1;
      date.year  = token[0];

   } else if (count == 2) {

      if (state[0] == STRING) {
         /* string */
         if (state[1] == NUMBER) {
            /* string number, done */
         } else {
            /* string string, error */
            return;
         }
      } else {
         /* number */
         if (state[1] == NUMBER) {
            /* number number, done */
         } else {
            /* number string, error */
            return;
         }
      }

      /* Month Year */
      date.day   = 0;
      date.month = token[0];
      date.year  = token[1];

   } else if (count == 3) {

      if (state[0] == STRING) {
         /* string */
         if (state[1] == NUMBER) {
            /* string number */
            if (state[2] == NUMBER) {
               /* string number number, done */
            } else {
               /* string number string, error */
               return;
            }
         } else {
            /* string string, error */
            return;
         }

         /* Month Day Year */
         date.month = token[0];
         date.day   = token[1];
         date.year  = token[2];

      } else {

         /* number */
         if (state[1] == STRING) {
            /* number string */
            if (state[2] == NUMBER) {
               /* number string number, done */
               /* Day Month Year */
               date.day   = token[0];
               date.month = token[1];
               date.year  = token[2];
            } else {
               /* number string string, error */
               return;
            }
         } else {
            /* number number */
            if (state[2] == NUMBER) {
               /* number number number, done */
               if (european) {
                  /* Day Month Year */
                  date.day   = token[0];
                  date.month = token[1];
                  date.year  = token[2];
               } else {
                  /* Month Day Year */
                  date.month = token[0];
                  date.day   = token[1];
                  date.year  = token[2];
               }
            } else {
               /* number number string, error */
               return;
            }
         }
      }
   }
}



int is_valid_date(date_ptr)
Date * date_ptr;
{
   if (date_ptr->month < 1 || date_ptr->month > 12) {
      /* None or an invalid month was specified. */
      return FALSE;
   }

   if (date_ptr->year <= 0) {
      /* No year was specified. */
      return FALSE;
   }

   if (date_ptr->year < 100) {
      /* Bump a two digit year into the 20th century. */
      date_ptr->year += 1900;
   }

   if (!date_ptr->day) {
      /* It's okay if no day is specified. */
      return TRUE;
   } else {
      if (date_ptr->day < 0 || date_ptr->day > days_in_month[date_ptr->month]) {
         /* An invalid day was specified. */
         return FALSE;
      } else {
         /* One last error check. */
         if (date_ptr->month == FEBRUARY 
             && date_ptr->day == 29 
             && !A_LEAP_YEAR(date_ptr->year)) {
            return FALSE;
         } else {
            /* A valid day was specified. */
            return TRUE;
         }
      }
   }
}
