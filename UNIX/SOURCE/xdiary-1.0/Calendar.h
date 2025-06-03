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

#ifndef _CalendarWidget_h
#define _CalendarWidget_h

#include <X11/Core.h>
#include "StrInRect.h"
#include "Date.h"

/* 
   Calendar.h -- Public Include File.
   Author: Jason Baietto
   Date:   September 16, 1990
*/

/* Public functions: */
extern void CalendarSetDate();
extern void CalendarShowMonth();
extern void CalendarGetDate();
extern void CalendarSetMonthInfo();
extern void CalendarIncMonth();
extern void CalendarDecMonth();
extern void CalendarIncYear();
extern void CalendarDecYear();
extern void CalendarIncDay();
extern void CalendarDecDay();
extern Date GetTodaysDate();
extern Date DateConverter();
extern char * CalendarPrettyDate();

typedef struct {
   unsigned int mask;
   union {
      char * string;
      Pixmap pixmap;
   } info[9]; /* one per gravity */
} DayInfo;

typedef DayInfo MonthInfo[31];

/* Resource Names: */
#ifndef XtNlineWidth
#define XtNlineWidth        "lineWidth"
#endif
#define XtNdigitFont        "digitFont"
#define XtNweekdayFont      "weekdayFont"
#define XtNtitleFont        "titleFont"
#define XtNinfoFont         "infoFont"
#define XtNdigitGravity     "digitGravity"
#define XtNdigitNames       "digitNames"
#define XtNweekdayNames     "weekdayNames"
#define XtNmonthNames       "monthNames"
#ifndef XtNhighlight
#define XtNhighlight        "highlight"
#endif
#define XtNshowYear         "showYear"
#define XtNstartingWeekday  "startingWeekday"

/* Resource Classes: */
#ifndef XtCLineWidth
#define XtCLineWidth        "LineWidth"
#endif
#define XtCCalendarFont     "CalendarFont"
#define XtCDigitGravity     "DigitGravity"
#define XtCDigitNames       "DigitNames"
#define XtCWeekdayNames     "WeekdayNames"
#define XtCMonthNames       "MonthNames"
#ifndef XtCHighlight
#define XtCHighlight        "Highlight"
#endif
#define XtCShowYear         "ShowYear"
#define XtCStartingWeekday  "StartingWeekday"

extern WidgetClass calendarWidgetClass;

typedef struct _CalendarClassRec *CalendarWidgetClass;
typedef struct _CalendarRec      *CalendarWidget;

#define DIstringNW  (1L<<0)
#define DIstringN   (1L<<1)
#define DIstringNE  (1L<<2)
#define DIstringW   (1L<<3)
#define DIstringC   (1L<<4)
#define DIstringE   (1L<<5)
#define DIstringSW  (1L<<6)
#define DIstringS   (1L<<7)
#define DIstringSE  (1L<<8)

#define DIpixmapNW  (1L<<9)
#define DIpixmapN   (1L<<10)
#define DIpixmapNE  (1L<<11)
#define DIpixmapW   (1L<<12)
#define DIpixmapC   (1L<<13)
#define DIpixmapE   (1L<<14)
#define DIpixmapSW  (1L<<15)
#define DIpixmapS   (1L<<16)
#define DIpixmapSE  (1L<<17)


#define XtRcalendarDefaultTranslations \
   "<Btn1Down>:    select()\n\
    <Btn1Motion>:  select()\n\
    <Btn1Up>:      notify()"

#endif /* _CalendarWidget_h */
