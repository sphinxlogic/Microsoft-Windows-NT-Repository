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
#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>
#include "DayNameP.h"
#include "common.h"

#define MAX_STRING_LEN 256

/*==========================================================================*/
/*                                 Quarks:                                  */
/*==========================================================================*/
XrmQuark XtQSunday;
XrmQuark XtQMonday;
XrmQuark XtQTuesday;
XrmQuark XtQWednesday;
XrmQuark XtQThursday;
XrmQuark XtQFriday;
XrmQuark XtQSaturday;


/*==========================================================================*/
/*                         DayName Type Converter:                          */
/*==========================================================================*/
static void downcase_string(source, dest)
char * source;
char * dest;
{
   for (; *source != 0; source++, dest++) {
      *dest = DOWNCASE(*source);
   }
   *dest = 0;
}



/*ARGSUSED*/
void DayNameConverter(args, num_args, fromVal, toVal)
XrmValuePtr args;
Cardinal    *num_args;
XrmValuePtr fromVal;
XrmValuePtr toVal;
{
   static int initialized = FALSE;
   static XtDayName DayName;
   XrmQuark q;
   char lowerName[MAX_STRING_LEN];

   if (!initialized) {
      /* Create quarks the first time we're called. */
      XtQSunday     = XrmStringToQuark(XtNsunday);
      XtQMonday     = XrmStringToQuark(XtNmonday);
      XtQTuesday    = XrmStringToQuark(XtNtuesday);
      XtQWednesday  = XrmStringToQuark(XtNwednesday);
      XtQThursday   = XrmStringToQuark(XtNthursday);
      XtQFriday     = XrmStringToQuark(XtNfriday);
      XtQSaturday   = XrmStringToQuark(XtNsaturday);
   }

   downcase_string((char*)fromVal->addr, lowerName);
   q = XrmStringToQuark(lowerName);

   toVal->size = sizeof(XtDayName);
   toVal->addr = (XtPointer) &DayName;

   if (q == XtQSunday) {
      DayName = Sunday;
   } else if (q == XtQMonday) {
      DayName = Monday;
   } else if (q == XtQTuesday) {
      DayName = Tuesday;
   } else if (q == XtQWednesday) {
      DayName = Wednesday;
   } else if (q == XtQThursday) {
      DayName = Thursday;
   } else if (q == XtQFriday) {
      DayName = Friday;
   } else if (q == XtQSaturday) {
      DayName = Saturday;
   } else {
      XtStringConversionWarning(fromVal->addr, "XtRDayName");
      toVal->addr = NULL;
      toVal->size = 0;
   }
}

