/*
 * Author: Jason Baietto, jason@ssd.csd.harris.com
 * xdiary Copyright 1990 Harris Computer Systems Division
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

#ifndef _DayNameConverter_h
#define _DayNameConverter_h

extern void DayNameConverter();

#define XtRDayName "DayName"

typedef enum {
   Sunday,
   Monday,
   Tuesday,
   Wednesday,
   Thursday,
   Friday,
   Saturday
} XtDayName;

#endif /* _DayNameConverter_h */
