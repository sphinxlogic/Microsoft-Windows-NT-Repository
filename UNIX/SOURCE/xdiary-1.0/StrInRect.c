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
#include "StrInRectP.h"

/*==========================================================================*/
/*                        DrawStringInRect Routines:                        */
/*==========================================================================*/
void DrawStringInRect(display,
                      drawable,
                      gc,
                      font,
                      string,
                      length,
                      rectangle,
                      gravity,
                      xpad,
                      ypad)
Display      * display;
Drawable       drawable;
GC             gc;
XFontStruct  * font;
char         * string;
int            length;       
XRectangle   * rectangle;
L_XtGravity      gravity;
int            xpad;
int            ypad;
{

   int ascent        = font->ascent;
   int descent       = font->descent;
   int string_width  = XTextWidth(font, string, length);
   int string_height = ascent + descent;
   int x,y;

   switch (gravity) {
      case NorthWest:
         x = rectangle->x + xpad;
         y = rectangle->y + ascent + ypad;
         break;
      case North:
         x = rectangle->x + (rectangle->width - string_width)/2;
         y = rectangle->y + ascent + ypad;
         break;
      case NorthEast:
         x = (rectangle->x + rectangle->width - 1) - string_width - xpad;
         y = rectangle->y + ascent + ypad;
         break;
      case West:
         x = rectangle->x + xpad;
         y = rectangle->y + (rectangle->height - string_height)/2 + ascent;
         break;
      case Center:
         x = rectangle->x + (rectangle->width - string_width)/2;
         y = rectangle->y + (rectangle->height - string_height)/2 + ascent;
         break;
      case East:
         x = (rectangle->x + rectangle->width - 1) - string_width - xpad;
         y = rectangle->y + (rectangle->height - string_height)/2 + ascent;
         break;
      case SouthWest:
         x = rectangle->x + xpad;
         y = (rectangle->y + rectangle->height - 1) - descent - 1 - ypad;
         break;
      case South:
         x = rectangle->x + (rectangle->width - string_width)/2;
         y = (rectangle->y + rectangle->height - 1) - descent - 1 - ypad;
         break;
      case SouthEast:
         x = (rectangle->x + rectangle->width - 1) - string_width - xpad;
         y = (rectangle->y + rectangle->height - 1) - descent - 1 - ypad;
         break;
      default:
         fprintf(stderr, "DrawStringInRect: unknown gravity\n");
         break;
   }

   XDrawString(
      display,
      drawable,
      gc,
      x,y,
      string,
      length
   );
}




void DrawStringsInRects(display,
                        drawable,
                        gc,
                        font,
                        strings,
                        lengths,
                        rectangles,
                        count,
                        gravity,
                        xpad,
                        ypad)
Display      * display;
Drawable       drawable;
GC             gc;
XFontStruct  * font;
StringTable    strings;
int          * lengths;
XRectangle   * rectangles;
int            count;
L_XtGravity      gravity;
int            xpad;
int            ypad;
{

   int ascent        = font->ascent;
   int descent       = font->descent;
   int string_height = ascent + descent;
   int string_width;
   int x,y;
   int i;

   for (i=0; i < count; i++) {

      string_width  = XTextWidth(font, strings[i], lengths[i]);

      switch (gravity) {
         case NorthWest:
            x = rectangles[i].x + xpad;
            y = rectangles[i].y + ascent + ypad;
            break;
         case North:
            x = rectangles[i].x + (rectangles[i].width - string_width)/2;
            y = rectangles[i].y + ascent + ypad;
            break;
         case NorthEast:
            x = (rectangles[i].x + rectangles[i].width - 1) - string_width - xpad;
            y = rectangles[i].y + ascent + ypad;
            break;
         case West:
            x = rectangles[i].x + xpad;
            y = rectangles[i].y + (rectangles[i].height - string_height)/2 + ascent;
            break;
         case Center:
            x = rectangles[i].x + (rectangles[i].width - string_width)/2;
            y = rectangles[i].y + (rectangles[i].height - string_height)/2 + ascent;
            break;
         case East:
            x = (rectangles[i].x + rectangles[i].width - 1) - string_width - xpad;
            y = rectangles[i].y + (rectangles[i].height - string_height)/2 + ascent;
            break;
         case SouthWest:
            x = rectangles[i].x + xpad;
            y = (rectangles[i].y + rectangles[i].height - 1) - descent - 1 - ypad;
            break;
         case South:
            x = rectangles[i].x + (rectangles[i].width - string_width)/2;
            y = (rectangles[i].y + rectangles[i].height - 1) - descent - 1 - ypad;
            break;
         case SouthEast:
            x = (rectangles[i].x + rectangles[i].width - 1) - string_width - xpad;
            y = (rectangles[i].y + rectangles[i].height - 1) - descent - 1 - ypad;
            break;
         default:
            fprintf(stderr, "DrawStringsInRects: unknown gravity\n");
            break;
      }

      XDrawString(
         display,
         drawable,
         gc,
         x,y,
         strings[i],
         lengths[i]
      );
   }
}
