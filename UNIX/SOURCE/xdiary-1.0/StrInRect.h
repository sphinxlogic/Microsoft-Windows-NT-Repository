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

#ifndef _DrawStringInRect_h
#define _DrawStringInRect_h

#include "Gravity.h"

/*
   Author: Jason Baietto
   Date:   9/22/90

   The DrawStringInRect routine draws text in a specified XtRectangle at the
   location specified by the XtGravity parameter.  

   The DrawStringsInRects routine does the same thing, but for multiple
   strings.  It is passed a pointer to an array of strings, an array of string
   lengths, and an array of rectangles, thus making it possible for each
   string to specify a different rectangle.

   Care must be taken to ensure that the XFontStruct passed in matches the
   font that is specified in the GC that is passed in, otherwise the extent
   measurements will be incorrect and the string will be drawn incorrectly.

   Absolutely no error checking is done.  The application is responsible for
   providing a rectangle large enough for the specified string.

   You must include this file in your widget or application to use the
   DrawStringInRect routines.
*/

extern void DrawStringInRect();
extern void DrawStringsInRects();

#endif /* _DrawStringInRect_h */
