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

#ifndef _CalendarWidgetP_h
#define _CalendarWidgetP_h

/* CalendarP.h -- Private Include File. */

#include "Gravity.h"
#include <X11/CoreP.h>
#include <values.h>
#include "Calendar.h"
#include "DayName.h"
#include "StrTable.h"
#include "Date.h"

/* Widget's Class Part: */
typedef struct {
   int make_compiler_happy;
} CalendarClassPart;

/* Widget's Full Class Record: */
typedef struct _CalendarClassRec {
   CoreClassPart core_class;
   CalendarClassPart calendar_class;
} CalendarClassRec;

/* State info to avoid scrogging the calendar. */
typedef enum {
   plasma,       /* Before init -- i.e. uninitialized.       */
   foggy,        /* Just after initialize or resize method.  */
   solid         /* Just after expose until next resize.     */
} CalendarState;

#define DAYS_IN_WEEK (7)
#define MAX_DAYS_IN_MONTH (31)
#define MONTHS_IN_YEAR (12)
#define MAX_YEAR_LEN (10)

#define ROWS (8)
#define COLS (DAYS_IN_WEEK)

#define HORIZ_SEGMENTS (ROWS-2)
#define VERTI_SEGMENTS (COLS-1)
#define BORDER_SEGMENTS (4)
#define TOTAL_SEGMENTS (HORIZ_SEGMENTS+VERTI_SEGMENTS+BORDER_SEGMENTS)

/* Calendar Widget's Instance Part: */
typedef struct {

/* Public Resources */

   /* Callback list for all buttons */
   XtCallbackList callback;

   /* Line width of calendar lines */
   int            line_width;

   Pixel          foreground;
   Pixel          background;

   Font           digit_font;
   Font           weekday_font;
   Font           title_font;
   Font           info_font;
   
   /* Gravity of the digits in their boxes */
   L_XtGravity      digit_gravity;

   /* String table for names of digits. */
   StringTable    digit_names;

   /* String table for names of days of the week. */
   StringTable    weekday_names;

   /* String table for names of months. */
   StringTable    month_names;

   /* True means highlight specified day (if any) */
   Boolean        highlight;

   /* True means show year along with month in title. */
   Boolean        show_year;

   /* Weekday for leftmost column in calendar. 0=Sunday, 6=Saturday. */
   int            starting_weekday;

/* Private */
   
   /* Date for calendar to display month for.  Initially current. */
   Date          date;

   /* The current highlighted date.  Initially current. */
   Date          highlight_date;

   /* Current cell dimensions */
   float         real_cell_width;
   float         real_cell_height;

   /* Minimum cell dimensions based on current font extents */
   float         min_cell_width;
   float         min_cell_height;
   
   /* Cell matrix of XRectangles */
   XRectangle    cell_geometry[ROWS][COLS];

   /* Currently selected cell coordinate */
   int           current_x_cell;
   int           current_y_cell;

   /* Previous core width (updated after resize) */
   int           old_window_width;
   int           old_window_height;

   /* Month data for the showing date's year. */
   int           month_starting_weekdays[MONTHS_IN_YEAR];
   int           days_in_february;

   /* Month data for the highlight_date's year. */
   int           highlight_days_in_february;

   /* Simplicity variables to avoid recomputing. */
   int           month_start_cellnum;
   int           month_end_cellnum;

   /* lengths of strings frequently drawn in calendar */
   int           digit_name_lengths[MAX_DAYS_IN_MONTH];
   int           weekday_name_lengths[DAYS_IN_WEEK];
   int           month_name_lengths[MONTHS_IN_YEAR];

   /* Year information for the current calendar */
   char          year_string[MAX_YEAR_LEN];
   int           year_string_length;
   int           current_year;

   /* Title information for the current calendar */
   char *        title_string;
   int           title_string_length;
   XRectangle    title_geometry;

   /* Widget state info.  Used to avoid changing the date between the */
   /* resize or initialization method and the expose method.          */

   /* True if a valid cell has been selected with the pointer. */
   Boolean        cell_selected;

   /* Last method that has been run. */
   CalendarState  state;

   /* Segments for drawing the grid. */
   XSegment       segments[TOTAL_SEGMENTS];

   /* The number of segments currently in the segments array. */
   int            number_segments;

   /* GCs for line/box drawing */
   GC            draw_gc;
   GC            undraw_gc;
   GC            invert_gc;

   /* GCs and font structs for text drawing */
   GC            digit_draw_gc;
   GC            digit_undraw_gc;
   XFontStruct * digit_fsp;

   GC            weekday_draw_gc;
   GC            weekday_undraw_gc;
   XFontStruct * weekday_fsp;

   GC            title_draw_gc;
   GC            title_undraw_gc;
   XFontStruct * title_fsp;

   GC            info_draw_gc;
   GC            info_undraw_gc;
   XFontStruct * info_fsp;

} CalendarPart;

/* Widget's Full Instance Record: */
typedef struct _CalendarRec {
   CorePart     core;
   CalendarPart calendar;
} CalendarRec;

extern CalendarClassRec calendarClassRec;

#define SUNDAY    0
#define MONDAY    1
#define TUESDAY   2
#define WEDNESDAY 3
#define THURSDAY  4
#define FRIDAY    5
#define SATURDAY  6

#define XtNDefaultWeekdayNames \
    "\"Sun\",\"Mon\",\"Tue\",\"Wed\",\"Thu\",\"Fri\",\"Sat\""

#define XtNDefaultMonthNames \
    "\"January\",\"February\",\"March\",\"April\", \
     \"May\",\"June\",\"July\",\"August\",\"September\", \
     \"October\",\"November\",\"December\""

#define XtNDefaultDigitNames \
   "\"1\",\"2\",\"3\",\"4\",\"5\",\"6\",\"7\",\"8\",\"9\",\"10\", \
   \"11\",\"12\",\"13\",\"14\",\"15\",\"16\",\"17\",\"18\",\"19\",\"20\", \
   \"21\",\"22\",\"23\",\"24\",\"25\",\"26\",\"27\",\"28\",\"29\",\"30\", \
   \"31\""

#define OFF (0)
#define ON (1)

#define WINDOW_HEIGHT(cell_height) ((int)((cell_height)*ROWS))
#define WINDOW_WIDTH(cell_width)   ((int)((cell_width )*COLS))

#define MIN_LINE_WIDTH (1)
#define MAX_LINE_WIDTH (10)
#define DEFAULT_LINE_WIDTH (1)

#define INVERT_BORDER (1)

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MAX3(a,b,c) (MAX(MAX((a),(b)),(c)))

#define MAX_TITLE_LEN (100)

/* This assumes integer data types */
#define range_check(name, val, min, max) \
{ \
   if ((int)(val) < (int)(min)) { \
      fprintf(stderr, "Warning: %s=%d too small (using %d)\n", name, val, min); \
      (val) = (min); \
   } else if ((int)(val) > (int)(max)) { \
      fprintf(stderr, "Warning: %s=%d too large (using %d)\n", name, val, max); \
      (val) = (max); \
   } \
}

#define bound(val, min, max) \
   ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))

#define low_bound(val, min) \
   ((val) < (min) ? (min) : (val))

#define high_bound(val, max) \
   ((val) > (max) ? (max) : (val))


#define CELLXYtoCELLNUM(x,y) (((y)-2)*COLS+(x))

#define CELLNUMtoCELLXY(n,x,y) \
   ( \
     (x) = (n)%7, \
     (y) = ((n)/7)+2 \
   )

#define CELLNUMtoDAYNUM(cn) \
   ( \
      ( (cn) < widget->calendar.month_start_cellnum || \
        (cn) > widget->calendar.month_end_cellnum ) \
      ? 0 \
      : (cn) - widget->calendar.month_start_cellnum + 1 \
   )

#define CELLXYtoDAYNUM(x,y) (CELLNUMtoDAYNUM(CELLXYtoCELLNUM((x),(y))))
   
#define JANUARY 0
#define FEBRUARY 1
#define MARCH 2
#define APRIL 3
#define MAY 4
#define JUNE 5
#define JULY 6
#define AUGUST 7
#define SEPTEMBER 8
#define OCTOBER 9 
#define NOVEMBER 10
#define DECEMBER 11

#define A_LEAP_YEAR(year) (year%4 == 0 && !(year%100 == 0 && year%400 != 0))

/* 0=Sunday, 6=Saturday */
#define DAYStoWEEKDAY(days) ((((days)%7)+6)%7)

#define ROTATE(widget, weekday) \
   ( ((weekday) + (7 - (widget)->calendar.starting_weekday)) % 7 )

#define DAYS_IN_MONTH(widget) \
   ( (widget)->calendar.date.month == FEBRUARY+1 \
     ? (widget)->calendar.days_in_february \
     : days_in_month[(widget)->calendar.date.month - 1] \
   )

#define DAYS_IN_HIGHLIGHT_MONTH(widget) \
   ( (widget)->calendar.highlight_date.month == FEBRUARY+1 \
     ? (widget)->calendar.highlight_days_in_february \
     : days_in_month[(widget)->calendar.highlight_date.month - 1] \
   )

#define WIDGETtoCELLXY(widget,x,y) \
   ( (widget)->calendar.highlight_date.day \
     ? ( \
         (x) = ( (widget)->calendar.month_start_cellnum   \
                 + (widget)->calendar.highlight_date.day - 1 ) % 7, \
         (y) = ( ( (widget)->calendar.month_start_cellnum   \
                   + (widget)->calendar.highlight_date.day - 1 ) / 7 ) + 2 \
       ) \
     : ( \
         (x) = 0 , \
         (y) = 0   \
       ) \
   )

#define GEOMETRY(widget,x,y) \
   ((widget)->calendar.cell_geometry[(y)][(x)])

#define SYNC(widget) \
   ((widget)->calendar.date.month == (widget)->calendar.highlight_date.month \
    && (widget)->calendar.date.year == (widget)->calendar.highlight_date.year)
 
#define MAX_PRETTY_DATE_STRING_LENGTH 256

#define CALENDAR_DEFAULT_FONT "fixed"
 
#endif /* _CalendarWidgetP_h */
