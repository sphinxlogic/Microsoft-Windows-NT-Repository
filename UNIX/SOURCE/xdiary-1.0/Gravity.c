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
#include "GravityP.h"
#include "common.h"

#define MAX_STRING_LEN 256

/*==========================================================================*/
/*                                 Quarks:                                  */
/*==========================================================================*/
XrmQuark XtQNorthWest;
XrmQuark XtQNorth;
XrmQuark XtQNorthEast;
XrmQuark XtQWest;
XrmQuark XtQCenter;
XrmQuark XtQEast;
XrmQuark XtQSouthWest;
XrmQuark XtQSouth;
XrmQuark XtQSouthEast;

XrmQuark XtQNW;
XrmQuark XtQN;
XrmQuark XtQNE;
XrmQuark XtQW;
XrmQuark XtQC;
XrmQuark XtQE;
XrmQuark XtQSW;
XrmQuark XtQS;
XrmQuark XtQSE;


/*==========================================================================*/
/*                         Gravity Type Converter:                          */
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
void GravityConverter(args, num_args, fromVal, toVal)
XrmValuePtr args;
Cardinal    *num_args;
XrmValuePtr fromVal;
XrmValuePtr toVal;
{
   static int initialized = FALSE;
   static L_XtGravity gravity;
   XrmQuark q;
   char lowerName[MAX_STRING_LEN];

   if (!initialized) {
      /* Create quarks the first time we're called. */
      XtQNorthWest  = XrmStringToQuark(XtNnorthWest);
      XtQNorth      = XrmStringToQuark(XtNnorth);
      XtQNorthEast  = XrmStringToQuark(XtNnorthEast);
      XtQWest       = XrmStringToQuark(XtNwest);
      XtQCenter     = XrmStringToQuark(XtNcenter);
      XtQEast       = XrmStringToQuark(XtNeast);
      XtQSouthWest  = XrmStringToQuark(XtNsouthWest);
      XtQSouth      = XrmStringToQuark(XtNsouth);
      XtQSouthEast  = XrmStringToQuark(XtNsouthEast);

      XtQNW  = XrmStringToQuark(XtNnorthWest2);
      XtQN   = XrmStringToQuark(XtNnorth2);
      XtQNE  = XrmStringToQuark(XtNnorthEast2);
      XtQW   = XrmStringToQuark(XtNwest2);
      XtQC   = XrmStringToQuark(XtNcenter2);
      XtQE   = XrmStringToQuark(XtNeast2);
      XtQSW  = XrmStringToQuark(XtNsouthWest2);
      XtQS   = XrmStringToQuark(XtNsouth2);
      XtQSE  = XrmStringToQuark(XtNsouthEast2);
   }

   downcase_string((char*)fromVal->addr, lowerName);
   q = XrmStringToQuark(lowerName);

   toVal->size = sizeof(L_XtGravity);
   toVal->addr = (XtPointer) &gravity;

   if (q == XtQNorthWest || q == XtQNW) {
      gravity = NorthWest;
   } else if (q == XtQNorth || q == XtQN) {
      gravity = North;
   } else if (q == XtQNorthEast || q == XtQNE) {
      gravity = NorthEast;
   } else if (q == XtQWest || q == XtQW) {
      gravity = West;
   } else if (q == XtQCenter || q == XtQC) {
      gravity = Center;
   } else if (q == XtQEast || q == XtQE) {
      gravity = East;
   } else if (q == XtQSouthWest || q == XtQSW) {
      gravity = SouthWest;
   } else if (q == XtQSouth || q == XtQS) {
      gravity = South;
   } else if (q == XtQSouthEast || q == XtQSE) {
      gravity = SouthEast;
   } else {
      XtStringConversionWarning(fromVal->addr, "XtRGravity");
      toVal->addr = NULL;
      toVal->size = 0;
   }
}

