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
   xdiary -- An X application which displays a calendar in one of two formats
             and/or an interactive daily dairy.

   Author: Jason Baietto (jason@ssd.csd.harris.com)
   Date:   October 10, 1990
*/

/*==========================================================================*/
/*                              Header Files:                               */
/*==========================================================================*/
#include <stdio.h>
#include <ctype.h>
#include <pwd.h>
#include <X11/Xos.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/AsciiText.h>
#include "Calendar.h"
#include "main.h"
#include "diary.h"
#include "template.h"
#include "cal.icon"
#include "diary.icon"
#include "arrow_r.xbm"
#include "arrow_l.xbm"
#include "resource.h"
#include "common.h"
#include "patchlevel.h"


/*==========================================================================*/
/*                            Forward References:                           */
/*==========================================================================*/
void quit();
void day_inc();
void day_dec();
void month_inc();
void month_dec();
void year_inc();
void year_dec();
void entry_inc();
void entry_dec();
void warp_to_dialog();
void warp_to_start();
void dialog_done();
void flip_calendar();
void set_the_date();
void init_bitmaps();
void init_dialog();
void init_button_box();
void init_menu();
void init_single_calendar();
void init_full_calendar();
void process_options();
Widget make_inc_dec_button();
void change_to_full();
void change_to_single();
void initialize_diary_widgets();
void toggle_diary();
void save_diary_callback();
void calendar_callback();
void show_new_diary_entry();
void save_old_diary_entry();
char * get_default_diary_file_name();
char * get_default_template_file_name();
void flash();
void flash_back();
void show_pretty_date();
void sensitize();
void set_initial_time_interval();


/*==========================================================================*/
/*                                 Globals:                                 */
/*==========================================================================*/
Widget top_level_widget;
Widget single_popup_shell_widget;
Widget full_popup_shell_widget;
Widget button_box_widget;
Widget option_button_widget;
Widget single_form_widget;
Widget single_calendar_widget;
Widget full_form_widget;
Widget full_calendar_widgets[12];
Widget dialog_popup_shell_widget;
Widget dialog_widget;
Widget text_widget;
Widget menu_popup_shell_widget;
Widget menu_today_widget;
Widget menu_warp_widget;
Widget menu_diary_widget;
Widget menu_full_widget;
Widget menu_single_widget;
Widget menu_save_widget;
Widget menu_quit_widget;
Widget diary_popup_shell_widget;
Widget diary_top_paned_widget;
Widget diary_label_widget;
Widget diary_form_widget;
Widget diary_text_widget;
Widget diary_text_source_widget;

XtActionsRec xdiary_actions[] = {
   { "dialog_done",     dialog_done },
};

char text_translation_table[] = "#override\n\
   <Key>Return:   dialog_done()";

XtTranslations text_translations;

Date current_date;

XrmOptionDescRec command_line_table[] = 
{
   { "-iconic",          "*iconic",             XrmoptionNoArg, "True" },
   { "-linewidth",       "*Calendar*lineWidth", XrmoptionSepArg, NULL  },
   { "-fullgeometry",    "*full.geometry",      XrmoptionSepArg, NULL  },
   { "-singlegeometry",  "*single.geometry",    XrmoptionSepArg, NULL  },
   { "-diarygeometry",   "*diary.geometry",     XrmoptionSepArg, NULL  },
   { "-digitfont",       "*single*digitFont",   XrmoptionSepArg, NULL  },
   { "-weekdayfont",     "*single*weekdayFont", XrmoptionSepArg, NULL  },
   { "-titlefont",       "*single*titleFont",   XrmoptionSepArg, NULL  },
   { "-digitgravity",    "*digitGravity",       XrmoptionSepArg, NULL  },
   { "-startingweekday", "*startingWeekday",    XrmoptionSepArg, NULL  },
   { "-language",        "*language",           XrmoptionSepArg, NULL  },
   { "-diaryfile",       "*diaryFile",          XrmoptionSepArg, NULL  },
   { "-templatefile",    "*templateFile",       XrmoptionSepArg, NULL  },
   { "-nocalendar",      "*noCalendar",         XrmoptionNoArg, "True" },
   { "-nodiary",         "*noDiary",            XrmoptionNoArg, "True" },
   { "-long",            "*long",               XrmoptionNoArg, "True" },
   { "-abbrev",          "*abbrev",             XrmoptionSepArg, NULL  },
   { "-fullyear",        "*fullYear",           XrmoptionNoArg, "True" },
   { "-singlemonth",     "*singleMonth",        XrmoptionNoArg, "True" }
   
};

app_data_type application_data;

char help[] = "\n\
Usage: xdiary [-linewidth <pixels>]\n\
              [-fullgeometry <geometry>]\n\
              [-singlegeometry <geometry>]\n\
              [-diarygeometry <geometry>]\n\
              [-digitfont <font_name>]\n\
              [-weekdayfont <font_name>]\n\
              [-titlefont <font_name>]\n\
              [-digitgravity <gravity>]\n\
              [-startingweekday <day_name>]\n\
              [-language <lang_name>]\n\
              [-diaryfile <diary_filename>]\n\
              [-templatefile <template_filename>]\n\
              [-nocalendar]\n\
              [-nodiary]\n\
              [-long]\n\
              [-abbrev <letter_count>]\n\
              [-fullyear]\n\
              [-singlemonth]\n\
              [<date_string>]\n\n";


/* True after initialized. */
int full_initialized = FALSE;
int single_initialized = FALSE;
int diary_initialized = FALSE;

/* True if any changes were made. */
int diary_changed = FALSE;

/* True after flash, until flash_back */
int currently_flashing;


/*==========================================================================*/
/*                            Argument Processing:                          */
/*==========================================================================*/
typedef enum { none, single, full } mode;
mode display_mode = single;
int diary_showing = FALSE;
int lang_opt = ENGLISH;
int lang_specified = FALSE;
int date_opt = FALSE;
int long_opt = FALSE;
int abbrev_opt = 3;
int diary_opt = TRUE;
int fly_opt = FALSE;
int arg_error = FALSE;
int forward = TRUE;
int interval = 0;
int using_template = FALSE;
XtIntervalId timeout_id;
char * date_string = NULL;
char * diary_file_name = NULL;
char * template_file_name = NULL;

/* Ignores case.  Allows partial abbreviations or garbage at end. */
int check_option(string_ptr, option_ptr)
char * string_ptr;
char * option_ptr;
{
   while (*string_ptr && *option_ptr) {
      if (DOWNCASE(*string_ptr) == *option_ptr) {
         /* Letters match */
         string_ptr++;
         option_ptr++;
      } else {
         /* Doesn't match. */
         return FALSE;
      }
   }
   return TRUE;
}



#define OPTION(opt) (check_option(argv[count],(opt)))
#define ARG  (argv[count])
#define NEXT \
   if (++count == argc) { \
      fprintf(stderr, "xdiary: \"%s\" requires argument\n", argv[count-1]); \
      return FALSE; \
   }



int parse_args(argc, argv)
int argc;
char *argv[];
{
   int count;
   for (count=1; count < argc; count++) {

      /* Undocumented, used for testing. */
      if (OPTION("-flies")) { 
         fly_opt = TRUE;
         NEXT;
         interval = atoi(ARG);
         if (interval < 0) {
            interval = abs(interval);
            forward  = FALSE;
         }
         continue; 
      }

      if (OPTION("-")) {
         fprintf(stderr, "xdiary: unknown option \"%s\"\n", ARG);
         arg_error = TRUE;
         continue;
      }

      if (!date_opt++) {
         date_string = ARG;
         continue;
      }

      /* No valid options match. */
      return FALSE;
   }

   /* All options present and accounted for. */
   return TRUE;
}





#define RESOURCE(field) (application_data.field)
#define EQUAL(res, str) (check_option((res),(str)))

void process_resources()
{
   if (RESOURCE(language)) {
      lang_specified = TRUE;
      if (EQUAL(RESOURCE(language), "english")) lang_opt = ENGLISH; else
      if (EQUAL(RESOURCE(language), "spanish")) lang_opt = SPANISH; else
      if (EQUAL(RESOURCE(language), "french"))  lang_opt = FRENCH;  else
      if (EQUAL(RESOURCE(language), "german"))  lang_opt = GERMAN;  else {
         lang_specified = FALSE;
         fprintf(
            stderr,
            "xdiary: unknown language \"%s\"\n",
            RESOURCE(language)
         );
      }
   }

   if (RESOURCE(diaryfile)) {
      diary_file_name = RESOURCE(diaryfile);
   }

   if (RESOURCE(templatefile)) {
      template_file_name = RESOURCE(templatefile);
   }

   if (RESOURCE(nocalendar)) {
      display_mode = none;
   }

   if (RESOURCE(nodiary)) {
      if (display_mode == none) {
         fprintf(
            stderr, 
            "xdiary: nocalendar and nodiary can't both be specified.\n"
         );
      } else {
         diary_opt = FALSE;
      }
   }

   if (RESOURCE(long_opt)) {
      long_opt = TRUE;
      lang_specified = TRUE;
   }

   if (RESOURCE(abbrev)) {
      abbrev_opt = RESOURCE(abbrev);
      if (abbrev_opt < 0) {
         abbrev_opt = 0;
      }
      lang_specified = TRUE;
   }

   if (RESOURCE(fullyear)) {
      display_mode = full;
   }

   if (RESOURCE(singlemonth)) {
      display_mode = single;
   }

}




void process_options()
{
   int month;
   int weekday;

   /* Abbreviate the weekday names as specified. */
   if (lang_specified && !long_opt) {
      for (weekday=0; weekday < 7; weekday++) {
         if (strlen(weekday_names[lang_opt][weekday]) > abbrev_opt) {
            weekday_names[lang_opt][weekday][abbrev_opt] = NULL;
         }
      }
   }

   if (display_mode == full) {
      init_full_calendar();
   } else {
      init_single_calendar();
   }

   /* Realize a calendar. */
   if (display_mode == single) {
      XtPopup(single_popup_shell_widget, XtGrabNone);
      if (!diary_showing) {
         sensitize(menu_single_widget, False);
      }
   } else if (display_mode == full) {
      XtPopup(full_popup_shell_widget, XtGrabNone);
      if (!diary_showing) {
         sensitize(menu_full_widget, False);
      }
   } else {
      /* Realize it, but don't pop it up. */
      XtRealizeWidget(single_popup_shell_widget);
      sensitize(menu_diary_widget, False);
   }

   if (date_opt) {
      /* A date string was specified. */
      if (display_mode == full) {
         current_date = DateConverter(full_calendar_widgets[0], date_string);
       } else {
         current_date = DateConverter(single_calendar_widget, date_string);
      }
   }

   if (!date_opt || current_date.year == 0) {
      /* No date was specified or the specified date was bogus, */
      /* so calendar widget chose its own date.  Get that date. */
      if (display_mode == full) {
         CalendarGetDate(full_calendar_widgets[0], &current_date);
      } else {
         CalendarGetDate(single_calendar_widget, &current_date);
      }
   }

   set_the_date(current_date);

   /* The undocumentd fly option -- used for testing. */
   if (fly_opt) {
      timeout_id = XtAddTimeOut(interval, flip_calendar, NULL);
   } else {
      /* Make calendar flip with the date. */
      set_initial_time_interval();
      timeout_id = XtAddTimeOut(interval, flip_calendar, NULL);
      interval = TWENTY_FOUR_HOURS;
   }

   if (!diary_file_name) {
      diary_file_name = get_default_diary_file_name();
   }

   if (template_file_name) {
      using_template = init_template(template_file_name);
      if (!using_template) {
         fprintf(stderr, "xdiary: unable to open template file\n");
      }
   } else {
      /* No template was specified. */
      template_file_name = get_default_template_file_name();
      using_template = init_template(template_file_name);
   }

   if (diary_opt) {
      toggle_diary();
   }

}





/*==========================================================================*/
/*                                  Main:                                   */
/*==========================================================================*/
main(argc, argv)
int argc;
char *argv[];
{
   /* Never Realized */
   top_level_widget = XtInitialize(
      argv[0],                       /* application name */
      "Xdiary",                      /* application class */
      command_line_table,            /* resource manager options */
      XtNumber(command_line_table),  /* number of resource manager options */
      &argc,                         /* number of command line args */
      argv                           /* array of command line args */
   );

   XtGetApplicationResources(
      top_level_widget,
      &application_data,
      application_resources,
      XtNumber(application_resources),
      NULL,
      0
   );

   XtAddActions(xdiary_actions, XtNumber(xdiary_actions));

   process_resources();

   if (!parse_args(argc, argv) || arg_error) {
      fprintf(stderr, help);
      XtDestroyWidget(top_level_widget);
      exit(0);
   }
   
   init_bitmaps();

   init_dialog();

   init_menu();

   process_options();

   XtMainLoop();
   
   return(1);
}




/*==========================================================================*/
/*                                 Bitmaps:                                 */
/*==========================================================================*/
Pixmap calendar_icon;
Pixmap diary_icon;
Pixmap right_arrow;
Pixmap left_arrow;
void init_bitmaps()
{
   calendar_icon = XCreateBitmapFromData(
      XtDisplay(top_level_widget),
      RootWindowOfScreen(XtScreen(top_level_widget)),
      calendar_bits,
      calendar_width,
      calendar_height
   );

   diary_icon = XCreateBitmapFromData(
      XtDisplay(top_level_widget),
      RootWindowOfScreen(XtScreen(top_level_widget)),
      diary_bits,
      diary_width,
      diary_height
   );

   left_arrow = XCreateBitmapFromData(
      XtDisplay(top_level_widget),
      RootWindowOfScreen(XtScreen(top_level_widget)),
      left_arrow_bits,
      left_arrow_width,
      left_arrow_height
   );

   right_arrow = XCreateBitmapFromData(
      XtDisplay(top_level_widget),
      RootWindowOfScreen(XtScreen(top_level_widget)),
      right_arrow_bits,
      right_arrow_width,
      right_arrow_height
   );
}



/*==========================================================================*/
/*                               Buttonbox:                                 */
/*==========================================================================*/
void init_button_box(parent_widget, mask)
Widget parent_widget;
int mask;
{
   Widget widget_list[MAX_WIDGETS];
   int num_to_manage = 0;

   /*VARARGS0*/
   button_box_widget = XtVaCreateManagedWidget (
      "buttons",           /* widget name */
      boxWidgetClass,      /* widget class */
      parent_widget,       /* parent widget */
      XtNorientation,      XtEhorizontal,
      XtNborderWidth,      0,
      XtNshowGrip,         False,
      XtNskipAdjust,       True,
      NULL                 /* end args */
   );
 
   /*VARARGS0*/
   widget_list[num_to_manage++] = option_button_widget = XtVaCreateWidget (
      "option",               /* widget name */
      menuButtonWidgetClass,  /* widget class */
      button_box_widget,      /* parent widget */
      XtNlabel,               option_label[lang_opt],
      XtNmenuName,            "menu",
      NULL                    /* end args */
   );

   if (mask & DAY) {
      widget_list[num_to_manage++] = make_inc_dec_button (
         "day",                  /* widget name */
         button_box_widget,      /* parent widget */
         day_label[lang_opt],    /* label */
         day_dec,                /* dec callback */
         day_inc                 /* inc callback */
      );
   }
    
   if (mask & MONTH) {
      widget_list[num_to_manage++] = make_inc_dec_button (
         "month",                /* widget name */
         button_box_widget,      /* parent widget */
         month_label[lang_opt],  /* label */
         month_dec,              /* dec callback */
         month_inc               /* inc callback */
      );
   }
   
   if (mask & YEAR) {
      widget_list[num_to_manage++] = make_inc_dec_button (
         "year",                 /* widget name */
         button_box_widget,      /* parent widget */
         year_label[lang_opt],   /* label */
         year_dec,               /* dec callback */
         year_inc                /* inc callback */
      );
   }
   
   if (mask & ENTRY) {
      widget_list[num_to_manage++] = make_inc_dec_button (
         "entry",                 /* widget name */
         button_box_widget,       /* parent widget */
         entry_label[lang_opt],   /* label */
         entry_dec,               /* dec callback */
         entry_inc                /* inc callback */
      );
   }

   /* Manage the children of the top form widget defined above. */
   XtManageChildren(widget_list, num_to_manage);
}




/*==========================================================================*/
/*                                 Dialog:                                  */
/*==========================================================================*/
void init_dialog()
{
   /*VARARGS0*/
   dialog_popup_shell_widget = XtVaCreatePopupShell(
      "dialog",
      transientShellWidgetClass,
      top_level_widget,
      NULL
   );

   /*VARARGS0*/
   dialog_widget = XtVaCreateManagedWidget (
      "dialog",                  /* widget name */
      dialogWidgetClass,         /* widget class */
      dialog_popup_shell_widget, /* parent widget */
      XtNlabel,                  dialog_label[lang_opt],
      XtNvalue,                  "",
      NULL                       /* end args */
   );
   text_widget = XtNameToWidget(dialog_widget, "value");
   text_translations  = XtParseTranslationTable(text_translation_table);
   XtOverrideTranslations(text_widget, text_translations);
}





/*==========================================================================*/
/*                                  Menu:                                   */
/*==========================================================================*/
void init_menu()
{
   Widget widget_list[MAX_WIDGETS];
   int num_to_manage = 0;

   /*VARARGS0*/
   menu_popup_shell_widget = XtVaCreatePopupShell(
      "menu",
      simpleMenuWidgetClass,
      top_level_widget,
      NULL
   );

   /* TODAY */
   /*VARARGS0*/
   widget_list[num_to_manage++] = menu_today_widget = XtVaCreateWidget (
      "menu_today",              /* widget name */
      smeBSBObjectClass,         /* widget class */
      menu_popup_shell_widget,   /* parent widget */
      XtNlabel,                  today_label[lang_opt],
      NULL                       /* end args */
   );
   XtAddCallback(menu_today_widget, XtNcallback, warp_to_start, 0);

   /* LINE */
   /*VARARGS0*/
   widget_list[num_to_manage++] = XtVaCreateWidget (
      "line",                    /* widget name */
      smeLineObjectClass,        /* widget class */
      menu_popup_shell_widget,   /* parent widget */
      NULL                       /* end args */
   );

   /* WARP */
   /*VARARGS0*/
   widget_list[num_to_manage++] = menu_warp_widget = XtVaCreateWidget (
      "menu_warp",               /* widget name */
      smeBSBObjectClass,         /* widget class */
      menu_popup_shell_widget,   /* parent widget */
      XtNlabel,                  warp_label[lang_opt],
      NULL                       /* end args */
   );
   XtAddCallback(menu_warp_widget, XtNcallback, warp_to_dialog, 0);

   /* LINE */
   /*VARARGS0*/
   widget_list[num_to_manage++] = XtVaCreateWidget (
      "line",                    /* widget name */
      smeLineObjectClass,        /* widget class */
      menu_popup_shell_widget,   /* parent widget */
      NULL                       /* end args */
   );

   /* DIARY */
   /*VARARGS0*/
   widget_list[num_to_manage++] = menu_diary_widget = XtVaCreateWidget (
      "menu_diary",              /* widget name */
      smeBSBObjectClass,         /* widget class */
      menu_popup_shell_widget,   /* parent widget */
      XtNlabel,                  diary_label[lang_opt],
      NULL                       /* end args */
   );
   XtAddCallback(menu_diary_widget, XtNcallback, toggle_diary, 0);

   /* LINE */
   /*VARARGS0*/
   widget_list[num_to_manage++] = XtVaCreateWidget (
      "line",                    /* widget name */
      smeLineObjectClass,        /* widget class */
      menu_popup_shell_widget,   /* parent widget */
      NULL                       /* end args */
   );

   /* FULL */
   /*VARARGS0*/
   widget_list[num_to_manage++] = menu_full_widget = XtVaCreateWidget (
      "menu_full",               /* widget name */
      smeBSBObjectClass,         /* widget class */
      menu_popup_shell_widget,   /* parent widget */
      XtNlabel,                  full_label[lang_opt],
      NULL                       /* end args */
   );
   XtAddCallback(menu_full_widget, XtNcallback, change_to_full, 0);

   /* LINE */
   /*VARARGS0*/
   widget_list[num_to_manage++] = XtVaCreateWidget (
      "line",                    /* widget name */
      smeLineObjectClass,        /* widget class */
      menu_popup_shell_widget,   /* parent widget */
      NULL                       /* end args */
   );

   /* SINGLE */
   /*VARARGS0*/
   widget_list[num_to_manage++] = menu_single_widget = XtVaCreateWidget (
      "menu_single",             /* widget name */
      smeBSBObjectClass,         /* widget class */
      menu_popup_shell_widget,   /* parent widget */
      XtNlabel,                  single_label[lang_opt],
      NULL                       /* end args */
   );
   XtAddCallback(menu_single_widget, XtNcallback, change_to_single, 0);

   /* LINE */
   /*VARARGS0*/
   widget_list[num_to_manage++] = XtVaCreateWidget (
      "line",                    /* widget name */
      smeLineObjectClass,        /* widget class */
      menu_popup_shell_widget,   /* parent widget */
      NULL                       /* end args */
   );

   /* SAVE */
   /*VARARGS0*/
   widget_list[num_to_manage++] = menu_save_widget = XtVaCreateWidget (
      "menu_save",               /* widget name */
      smeBSBObjectClass,         /* widget class */
      menu_popup_shell_widget,   /* parent widget */
      XtNlabel,                  save_label[lang_opt],
      NULL                       /* end args */
   );
   XtAddCallback(menu_save_widget, XtNcallback, save_diary_callback, 0);

   /* LINE */
   /*VARARGS0*/
   widget_list[num_to_manage++] = XtVaCreateWidget (
      "line",                    /* widget name */
      smeLineObjectClass,        /* widget class */
      menu_popup_shell_widget,   /* parent widget */
      NULL                       /* end args */
   );

   /* QUIT */
   /*VARARGS0*/
   widget_list[num_to_manage++] = menu_quit_widget = XtVaCreateWidget (
      "menu_quit",               /* widget name */
      smeBSBObjectClass,         /* widget class */
      menu_popup_shell_widget,   /* parent widget */
      XtNlabel,                  quit_label[lang_opt],
      NULL                       /* end args */
   );
   XtAddCallback(menu_quit_widget, XtNcallback, quit, 0);
   
   /* Manage the children of the top form widget defined above. */
   XtManageChildren(widget_list, num_to_manage);
}




/*==========================================================================*/
/*                             Single Calendar:                             */
/*==========================================================================*/
void init_single_calendar() 
{
   Widget single_paned_widget;
   int linewidth;

   if (single_initialized) return;
   single_initialized = TRUE;

   /*VARARGS0*/
   single_popup_shell_widget = XtVaCreatePopupShell(
      "single",
      topLevelShellWidgetClass,
      top_level_widget,
      XtNiconPixmap,    calendar_icon,
      XtNiconName,      "calendar",
      NULL
   );

   /*VARARGS0*/
   single_paned_widget = XtVaCreateManagedWidget (
      "paned",                   /* widget name */
      panedWidgetClass,          /* widget class */
      single_popup_shell_widget, /* parent widget */
      NULL                       /* end args */
   );

   init_button_box(single_paned_widget, MONTH | YEAR);

   /*VARARGS0*/
   single_form_widget = XtVaCreateManagedWidget (
      "form",               /* widget name */
      formWidgetClass,      /* widget class */
      single_paned_widget,  /* parent widget */
      XtNshowGrip,          False,
      XtNborderWidth,       0,
      NULL                  /* end args */
   );

   /*VARARGS0*/
   single_calendar_widget = XtVaCreateManagedWidget (
      "calendar",          /* widget name */
      calendarWidgetClass, /* widget class */
      single_form_widget,  /* parent widget */
      XtNtop,              XtChainTop,
      XtNbottom,           XtChainBottom,
      XtNright,            XtChainRight,
      XtNleft,             XtChainLeft,
      XtNborderWidth,      0,
      NULL                 /* end args */
   );
   XtAddCallback(single_calendar_widget, XtNcallback, calendar_callback, 0);

   /* Get the border and the grid to be the same thickness */
   /*VARARGS0*/
   XtVaGetValues(
      single_calendar_widget,
      XtNlineWidth, &linewidth,
      NULL
   );

   /*VARARGS0*/
   XtVaSetValues(
      single_calendar_widget,
      XtNborderWidth,  linewidth/2,
      NULL
   );

   if (lang_specified) {
      /*VARARGS0*/
      XtVaSetValues(
         single_calendar_widget,
         XtNmonthNames,   (XtPointer) month_names[lang_opt],
         XtNweekdayNames, (XtPointer) weekday_names[lang_opt],
         NULL
      );
   }
}



/*==========================================================================*/
/*                              Full Calendar:                              */
/*==========================================================================*/
void init_full_calendar() 
{
   Widget full_paned_widget;
   Widget widget_list[MAX_WIDGETS];
   int num_to_manage = 0;
   int month;
   int linewidth;

   if (full_initialized) return;
   full_initialized = TRUE;

   /*VARARGS0*/
   full_popup_shell_widget = XtVaCreatePopupShell (
      "full",
      topLevelShellWidgetClass,
      top_level_widget,
      XtNiconPixmap,    calendar_icon,
      XtNiconName,      "calendar",
      NULL
   );

   /*VARARGS0*/
   full_paned_widget = XtVaCreateManagedWidget (
      "paned",                 /* widget name */
      panedWidgetClass,        /* widget class */
      full_popup_shell_widget, /* parent widget */
      NULL                     /* end args */
   );

   init_button_box(full_paned_widget, YEAR);

   /*VARARGS0*/
   full_form_widget = XtVaCreateManagedWidget (
      "form",                  /* widget name */
      formWidgetClass,         /* widget class */
      full_paned_widget,       /* parent widget */
      XtNshowGrip,             False,
      XtNborderWidth,          1,
      NULL                     /* end args */
   );

   /*VARARGS0*/
   widget_list[num_to_manage++] = full_calendar_widgets[0] = XtVaCreateWidget (
      "january",           /* widget name */
      calendarWidgetClass, /* widget class */
      full_form_widget,    /* parent widget */
      XtNtop,              XtChainTop,
      XtNleft,             XtChainLeft,
      XtNborderWidth,      0,
      NULL                 /* end args */
   );

   /*VARARGS0*/
   widget_list[num_to_manage++] = full_calendar_widgets[1] = XtVaCreateWidget (
      "february",          /* widget name */
      calendarWidgetClass, /* widget class */
      full_form_widget,    /* parent widget */
      XtNfromHoriz,        full_calendar_widgets[0],
      XtNtop,              XtChainTop,
      XtNborderWidth,      0,
      NULL                 /* end args */
   );

   /*VARARGS0*/
   widget_list[num_to_manage++] = full_calendar_widgets[2] = XtVaCreateWidget (
      "march",             /* widget name */
      calendarWidgetClass, /* widget class */
      full_form_widget,    /* parent widget */
      XtNfromHoriz,        full_calendar_widgets[1],
      XtNtop,              XtChainTop,
      XtNright,            XtChainRight,
      XtNborderWidth,      0,
      NULL                 /* end args */
   );

   /*VARARGS0*/
   widget_list[num_to_manage++] = full_calendar_widgets[3] = XtVaCreateWidget (
      "april",             /* widget name */
      calendarWidgetClass, /* widget class */
      full_form_widget,    /* parent widget */
      XtNfromVert,         full_calendar_widgets[0],
      XtNleft,             XtChainLeft,
      XtNborderWidth,      0,
      NULL                 /* end args */
   );

   /*VARARGS0*/
   widget_list[num_to_manage++] = full_calendar_widgets[4] = XtVaCreateWidget (
      "may",               /* widget name */
      calendarWidgetClass, /* widget class */
      full_form_widget,    /* parent widget */
      XtNfromVert,         full_calendar_widgets[0],
      XtNfromHoriz,        full_calendar_widgets[3],
      XtNborderWidth,      0,
      NULL                 /* end args */
   );

   /*VARARGS0*/
   widget_list[num_to_manage++] = full_calendar_widgets[5] = XtVaCreateWidget (
      "june",          /* widget name */
      calendarWidgetClass, /* widget class */
      full_form_widget,    /* parent widget */
      XtNfromVert,         full_calendar_widgets[0],
      XtNfromHoriz,        full_calendar_widgets[4],
      XtNright,            XtChainRight,
      XtNborderWidth,      0,
      NULL                 /* end args */
   );

   /*VARARGS0*/
   widget_list[num_to_manage++] = full_calendar_widgets[6] = XtVaCreateWidget (
      "july",          /* widget name */
      calendarWidgetClass, /* widget class */
      full_form_widget,    /* parent widget */
      XtNfromVert,         full_calendar_widgets[3],
      XtNleft,             XtChainLeft,
      XtNborderWidth,      0,
      NULL                 /* end args */
   );

   /*VARARGS0*/
   widget_list[num_to_manage++] = full_calendar_widgets[7] = XtVaCreateWidget (
      "august",          /* widget name */
      calendarWidgetClass, /* widget class */
      full_form_widget,    /* parent widget */
      XtNfromVert,         full_calendar_widgets[3],
      XtNfromHoriz,        full_calendar_widgets[6],
      XtNborderWidth,      0,
      NULL                 /* end args */
   );

   /*VARARGS0*/
   widget_list[num_to_manage++] = full_calendar_widgets[8] = XtVaCreateWidget (
      "september",          /* widget name */
      calendarWidgetClass, /* widget class */
      full_form_widget,    /* parent widget */
      XtNfromVert,         full_calendar_widgets[3],
      XtNfromHoriz,        full_calendar_widgets[7],
      XtNright,            XtChainRight,
      XtNborderWidth,      0,
      NULL                 /* end args */
   );

   /*VARARGS0*/
   widget_list[num_to_manage++] = full_calendar_widgets[9] = XtVaCreateWidget (
      "october",          /* widget name */
      calendarWidgetClass, /* widget class */
      full_form_widget,    /* parent widget */
      XtNfromVert,         full_calendar_widgets[6],
      XtNbottom,           XtChainBottom,
      XtNleft,             XtChainLeft,
      XtNborderWidth,      0,
      NULL                 /* end args */
   );

   /*VARARGS0*/
   widget_list[num_to_manage++] = full_calendar_widgets[10] = XtVaCreateWidget (
      "november",          /* widget name */
      calendarWidgetClass, /* widget class */
      full_form_widget,    /* parent widget */
      XtNfromVert,         full_calendar_widgets[6],
      XtNfromHoriz,        full_calendar_widgets[9],
      XtNbottom,           XtChainBottom,
      XtNborderWidth,      0,
      NULL                 /* end args */
   );

   /*VARARGS0*/
   widget_list[num_to_manage++] = full_calendar_widgets[11] = XtVaCreateWidget (
      "december",          /* widget name */
      calendarWidgetClass, /* widget class */
      full_form_widget,    /* parent widget */
      XtNfromVert,         full_calendar_widgets[6],
      XtNfromHoriz,        full_calendar_widgets[10],
      XtNbottom,           XtChainBottom,
      XtNright,            XtChainRight,
      XtNborderWidth,      0,
      NULL                 /* end args */
   );

   /* Get the border and the grid to be the same thickness */
   /*VARARGS0*/
   XtVaGetValues(
      full_calendar_widgets[0],
      XtNlineWidth, &linewidth,
      NULL
   );

   for (month=0; month < 12; month++) {
      XtAddCallback(
         full_calendar_widgets[month],
         XtNcallback,
         calendar_callback,
         0
      );

      /*VARARGS0*/
      XtVaSetValues(
         full_calendar_widgets[month],
         XtNborderWidth,  linewidth/2,
         NULL
      );

      if (lang_specified) {
         /*VARARGS0*/
         XtVaSetValues(
            full_calendar_widgets[month],
            XtNmonthNames,   (XtPointer) month_names[lang_opt],
            XtNweekdayNames, (XtPointer) weekday_names[lang_opt],
            NULL
         );
      }
   }

   /* Manage all the children of the form widget */
   XtManageChildren(widget_list, num_to_manage);
}



/*==========================================================================*/
/*                            Make Inc/Dec Button:                          */
/*==========================================================================*/
Widget make_inc_dec_button(name, parent_widget, label, callback1, callback2)
char * name;
Widget parent_widget;
char * label;
void (*callback1)();
void (*callback2)();
{
   Widget form_widget;
   Widget button1;
   Widget button2;
   Widget label_widget;
   Widget widget_list[MAX_WIDGETS];
   int num_to_manage = 0;

   /*VARARGS0*/
   form_widget = XtVaCreateManagedWidget (
      "form",              /* widget name */
      formWidgetClass,     /* widget class */
      parent_widget,       /* parent widget */
      XtNtop,               XtChainTop,
      XtNbottom,            XtChainBottom,
      XtNleft,              XtChainLeft,
      XtNright,             XtChainRight,
      XtNdefaultDistance,   0,
      XtNborderWidth,       0,
      NULL                 /* end args */
   );

   /*VARARGS0*/
   widget_list[num_to_manage++] = label_widget = XtVaCreateWidget (
      name,                /* widget name */
      labelWidgetClass,    /* widget class */
      form_widget,         /* parent widget */
      XtNlabel,            label,
      XtNtop,              XtChainTop,
      XtNbottom,           XtChainTop,
      XtNleft,             XtChainLeft,
      XtNright,            XtChainLeft,
      XtNborderWidth,      0,
      NULL                 /* end args */
   );
   
   /*VARARGS0*/
   widget_list[num_to_manage++] = button1 = XtVaCreateWidget (
      "dec",               /* widget name */
      commandWidgetClass,  /* widget class */
      form_widget,         /* parent widget */
      XtNfromHoriz,        label_widget,
      XtNtop,              XtChainTop,
      XtNbottom,           XtChainTop,
      XtNleft,             XtChainLeft,
      XtNright,            XtChainLeft,
      XtNbitmap,           left_arrow,
      XtNborderWidth,      1,
      NULL                 /* end args */
   );
   XtAddCallback(button1, XtNcallback, callback1, NULL);

   /*VARARGS0*/
   widget_list[num_to_manage++] = button2 = XtVaCreateWidget (
      "inc",               /* widget name */
      commandWidgetClass,  /* widget class */
      form_widget,         /* parent widget */
      XtNfromHoriz,        button1,
      XtNtop,              XtChainTop,
      XtNbottom,           XtChainTop,
      XtNleft,             XtChainLeft,
      XtNright,            XtChainLeft,
      XtNbitmap,           right_arrow,
      XtNborderWidth,      1,
      NULL                 /* end args */
   );
   XtAddCallback(button2, XtNcallback, callback2, NULL);

   /* Manage all the children of the form widget */
   XtManageChildren(widget_list, num_to_manage);

   return (form_widget);
}



/*==========================================================================*/
/*                          Miscellaneous Functions:                        */
/*==========================================================================*/
void set_the_date(new_date)
Date new_date;
{
   int month;
   Date temp_date;

   if (display_mode == full) {

      temp_date.year = new_date.year;

      /* Only change the calendar date if a day was specified. */
      if (new_date.day) {
         for (month=0; month < 12; month ++) {
            /* Set date to highlight for each calendar. */
            CalendarSetDate(full_calendar_widgets[month], new_date);
            current_date = new_date;
         }
      }

      for (month=0; month < 12; month ++) {
         /* Set month to show for each calendar. */
         temp_date.month = month+1;
         CalendarShowMonth(full_calendar_widgets[month], temp_date);
      }

   } else if (display_mode == single) {

      /* Only change the calendar date if a day was specified. */
      if (new_date.day) {
         CalendarSetDate(single_calendar_widget, new_date);
         current_date = new_date;
      }
      CalendarShowMonth(single_calendar_widget, new_date);

   } else {

      /* No calendar is displayed. */
      if (new_date.day) {
         CalendarSetDate(single_calendar_widget, new_date);
         current_date = new_date;
      }
   }

}




void flip_calendar()
{
   if (fly_opt) {
      if (forward) {
         day_inc();
      } else {
         day_dec();
      }
   } else {
      warp_to_start(NULL, NULL, NULL);
   }
   timeout_id = XtAddTimeOut(interval, flip_calendar, NULL);
}




char * get_default_diary_file_name()
{
   static char file_name[MAXPATHLEN];
   uid_t uid = getuid();
   struct passwd * pwent = getpwuid(uid);
   strcpy(file_name, pwent->pw_dir);
   strcat(file_name, DIARY_FILE);
   return(file_name);
}




char * get_default_template_file_name()
{
   static char file_name[MAXPATHLEN];
   uid_t uid = getuid();
   struct passwd * pwent = getpwuid(uid);
   strcpy(file_name, pwent->pw_dir);
   strcat(file_name, TEMPLATE_FILE);
   return(file_name);
}




/* Determine micro seconds til midnight. */
void set_initial_time_interval()
{
   time_t seconds = time(NULL);
   struct tm * tm_ptr = localtime(&seconds);
   int minutes = tm_ptr->tm_hour * 60 + tm_ptr->tm_min;

   /* Wait until 5 minutes after midnight and then flip */
   interval = TWENTY_FOUR_HOURS + FIVE_MINUTES - (1000 * 60 * minutes);
}





/*==========================================================================*/
/*                               Callbacks:                                 */
/*==========================================================================*/
/*ARGSUSED*/
void quit(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
   save_old_diary_entry();
   XtDestroyWidget(top_level_widget);
   if (diary_changed) {
      save_diary(diary_file_name);
      diary_changed = FALSE;
   }
   XtRemoveTimeOut(timeout_id);
   exit(0);
}



/*ARGSUSED*/
void month_inc(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
   /* Only called from single calendar mode. */
   CalendarIncMonth(single_calendar_widget);
}




/*ARGSUSED*/
void month_dec(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
   /* Only called from single calendar mode. */
   CalendarDecMonth(single_calendar_widget);
}




/*ARGSUSED*/
void year_inc(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
   int month;
   if (display_mode == full) {
      for (month=0; month < 12; month++) {
         CalendarIncYear(full_calendar_widgets[month]);
      }
   } else {
      CalendarIncYear(single_calendar_widget);
   }
}



/*ARGSUSED*/
void year_dec(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
   int month;
   if (display_mode == full) {
      for (month=0; month < 12; month++) {
         CalendarDecYear(full_calendar_widgets[month]);
      }
   } else {
      CalendarDecYear(single_calendar_widget);
   }
}




/*ARGSUSED*/
void warp_to_dialog(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
   Position x, y;

   XtTranslateCoords(
      widget,
      0, 0,
      &x, &y
   );

   /*VARARGS0*/
   XtVaSetValues(
      dialog_popup_shell_widget,
      XtNx, x+6,
      XtNy, y-6,
      NULL
   );

   /*VARARGS0*/
   XtVaSetValues(
      dialog_widget,
      XtNvalue, "",
      NULL
   );
   XtPopup(dialog_popup_shell_widget, XtGrabExclusive);
}




/*ARGSUSED*/
void warp_to_start(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
   Date todays_date;
   todays_date = GetTodaysDate();
   save_old_diary_entry();
   set_the_date(todays_date);
   show_new_diary_entry();
}





/*ARGSUSED*/
void day_inc(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
   int month;
   Date temp_date;

   save_old_diary_entry();

   if (display_mode == full) {
      
      for (month=0; month < 12; month++) {
         CalendarIncDay(full_calendar_widgets[month], False);
      }

      CalendarGetDate(full_calendar_widgets[0], &current_date);

      temp_date.year = current_date.year;
      for (month=0; month < 12; month++) {
         temp_date.month = month+1;
         CalendarShowMonth(full_calendar_widgets[month], temp_date);
      }

   } else if (display_mode == single) {

      CalendarIncDay(single_calendar_widget, True);
      CalendarGetDate(single_calendar_widget, &current_date);
      CalendarShowMonth(single_calendar_widget, current_date);
   
   } else {

      /* No calendar is displayed. */
      CalendarIncDay(single_calendar_widget, True);
      CalendarGetDate(single_calendar_widget, &current_date);
   }

   show_new_diary_entry();
}




/*ARGSUSED*/
void day_dec(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
   int month;
   Date temp_date;

   save_old_diary_entry();

   if (display_mode == full) {
      
      for (month=0; month < 12; month++) {
         CalendarDecDay(full_calendar_widgets[month], False);
      }

      CalendarGetDate(full_calendar_widgets[0], &current_date);

      temp_date.year = current_date.year;
      for (month=0; month < 12; month++) {
         temp_date.month = month+1;
         CalendarShowMonth(full_calendar_widgets[month], temp_date);
      }

   } else if (display_mode == single) {

      CalendarDecDay(single_calendar_widget, True);
      CalendarGetDate(single_calendar_widget, &current_date);
      CalendarShowMonth(single_calendar_widget, current_date);

   } else {

      /* No calendar is displayed. */
      CalendarDecDay(single_calendar_widget, True);
      CalendarGetDate(single_calendar_widget, &current_date);
   }

   show_new_diary_entry();
}




/*ARGSUSED*/
void change_to_full(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
   if (display_mode == full) {
      if (diary_showing) {
         XtPopdown(full_popup_shell_widget);
         sensitize(menu_diary_widget, False);
         sensitize(menu_full_widget, True);
         sensitize(menu_single_widget, True);
         display_mode = none;
      } else {
         /* Can't hide full if no diary is showing. */
      }
      return;
   }

   if (display_mode == single) {
      XtPopdown(single_popup_shell_widget);
   }

   if (!full_initialized) {
      init_full_calendar();
   }

   if (!diary_showing) {
      sensitize(menu_full_widget, False);
      sensitize(menu_single_widget, True);
   } else {
      sensitize(menu_diary_widget, True);
   }

   XtPopup(full_popup_shell_widget, XtGrabNone);
   display_mode = full;
   set_the_date(current_date);
}




/*ARGSUSED*/
void change_to_single(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
   if (display_mode == single) {
      if (diary_showing) {
         XtPopdown(single_popup_shell_widget);
         sensitize(menu_diary_widget, False);
         sensitize(menu_full_widget, True);
         sensitize(menu_single_widget, True);
         display_mode = none;
      } else {
         /* Can't hide single if no diary is showing. */
      }
      return;
   } 

   if (display_mode == full) {
      XtPopdown(full_popup_shell_widget);
   }

   if (!single_initialized) {
      init_single_calendar();
   }

   if (!diary_showing) {
      sensitize(menu_single_widget, False);
      sensitize(menu_full_widget, True);
   } else {
      sensitize(menu_diary_widget, True);
   }

   XtPopup(single_popup_shell_widget, XtGrabNone);
   display_mode = single;
   set_the_date(current_date);
}




/*==========================================================================*/
/*                                Actions:                                  */
/*==========================================================================*/
/*ARGSUSED*/
void dialog_done(w, event, params, num_params)
Widget w;
XButtonEvent *event;
String *params;
Cardinal *num_params;
{
   char * string;
   Date new_date;

   XtPopdown(dialog_popup_shell_widget);
   string = XawDialogGetValueString(dialog_widget);
   if (!strcmp(string, "")) {
      return;
   }

   if (display_mode == full) {
      new_date = DateConverter(full_calendar_widgets[0], string);
   } else {
      new_date = DateConverter(single_calendar_widget, string);
   }

   if (new_date.year) {
      /* The date was valid. */
      if (diary_showing) {
         save_old_diary_entry();
         set_the_date(new_date);
         show_new_diary_entry();
      } else {
         set_the_date(new_date);
      }
   }
}




/*==========================================================================*/
/*                                  Diary:                                  */
/*==========================================================================*/
void initialize_diary_widgets()
{
   diary_initialized = TRUE;

   init_diary(diary_file_name);

   /*VARARGS0*/
   diary_popup_shell_widget = XtVaCreatePopupShell(
      "diary",
      topLevelShellWidgetClass,
      top_level_widget,
      XtNiconPixmap,    diary_icon,
      XtNiconName,      "diary",
      XtNwaitForWm,     False,
      NULL
   );

   /*VARARGS0*/
   diary_top_paned_widget = XtVaCreateManagedWidget (
      "paned",                   /* widget name */
      panedWidgetClass,          /* widget class */
      diary_popup_shell_widget,  /* parent widget */
      NULL                       /* end args */
   );

   init_button_box(diary_top_paned_widget, DAY | ENTRY);

   /*VARARGS0*/
   diary_label_widget = XtVaCreateManagedWidget (
     "label",                   /* widget name */
      labelWidgetClass,         /* widget class */
      diary_top_paned_widget,   /* parent widget */
      XtNlabel,                 "",
      XtNshowGrip,              False,
      XtNskipAdjust,            True,
      NULL                      /* end args */
   );

   /*VARARGS0*/
   diary_form_widget = XtVaCreateManagedWidget (
      "form",                  /* widget name */
      formWidgetClass,         /* widget class */
      diary_top_paned_widget,  /* parent widget */
      XtNshowGrip,             False,
      NULL                     /* end args */
   );

   /*VARARGS0*/
   diary_text_widget = XtVaCreateManagedWidget(
      "text",                  /* widget name */
      asciiTextWidgetClass,    /* widget class */
      diary_form_widget,       /* parent widget */
      XtNeditType,             XtEtextEdit,
      XtNtop,                  XtChainTop,
      XtNbottom,               XtChainBottom,
      XtNright,                XtChainRight,
      XtNleft,                 XtChainLeft,
      NULL                     /* end args */
   );

   /*VARARGS0*/
   XtVaGetValues(
      diary_text_widget,
      XtNtextSource, &diary_text_source_widget,
      NULL
   );
}




void toggle_diary()
{
   if (!diary_initialized) {
      initialize_diary_widgets();
   }

   if (!diary_showing) {
      /* Display the diary. */
      XtPopup(diary_popup_shell_widget, XtGrabNone);
      diary_showing = TRUE;
      sensitize(menu_diary_widget, True);
      if (display_mode != none) {
         sensitize(menu_full_widget, True);
         sensitize(menu_single_widget, True);
      } else {
         sensitize(menu_diary_widget, False);
      }
      show_new_diary_entry();
   } else if (display_mode != none) {
      /* Hide the diary. */
      XtPopdown(diary_popup_shell_widget);
      diary_showing = FALSE;
      if (display_mode == single) {
          sensitize(menu_single_widget, False);
      } else {
         sensitize(menu_full_widget, False);
      }
      save_old_diary_entry();
   } 
}



void show_new_diary_entry()
{
   char * pretty_string;
   char * entry_text;

   if (!diary_showing) {
      return;
   }

   entry_text = retrieve(current_date);

   if (using_template && !entry_text[0]) {
      /* Initialize the entry to the template. */
      entry_text = template;
   }

   /*VARARGS0*/ 
   XtVaSetValues(
      diary_text_widget,
      XtNstring, entry_text,
      NULL
   );

   if (display_mode == full) {
      pretty_string = CalendarPrettyDate(full_calendar_widgets[0]);
   } else {
      pretty_string = CalendarPrettyDate(single_calendar_widget);
   }

   /*VARARGS0*/ 
   XtVaSetValues(
      diary_label_widget,
      XtNlabel,             pretty_string,
      NULL                  /* end args */
   );
}



void save_old_diary_entry()
{
   char * old_text;
   char * copy;

   if (!diary_initialized) {
      return;
   }

   if (XawAsciiSourceChanged(diary_text_source_widget)) {

      diary_changed = TRUE;

      /*VARARGS0*/ 
      XtVaGetValues(
         diary_text_widget,
         XtNstring, &old_text,
         NULL
      );

      if (using_template) {
         if (entry_modified(old_text)) {
            copy = XtNewString(old_text);
            install(current_date, copy, TRUE);
         } else {
            /* Only whitespace changes were made to the template */
            install(current_date, "", FALSE);
         }
      } else {
         copy = XtNewString(old_text);
         install(current_date, copy, TRUE);
      }
   }
}




void sensitize(widget, flag)
Widget widget;
Boolean flag;
{
   /*VARARGS0*/
   XtVaSetValues(
      widget,
      XtNsensitive, flag,
      NULL
   );
}




/*==========================================================================*/
/*                             Diary Callbacks:                             */
/*==========================================================================*/
/*ARGSUSED*/
void entry_inc(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
   Date next_date;
   if (currently_flashing) {
      return;
   }
   next_date = next_entry(current_date);
   if (next_date.year) {
      save_old_diary_entry();
      set_the_date(next_date);
      show_new_diary_entry();
   } else {
      flash("No following diary entry exists");
   }
}




/*ARGSUSED*/
void entry_dec(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
   Date prev_date;
   if (currently_flashing) {
      return;
   }
   prev_date = prev_entry(current_date);
   if (prev_date.year) {
      save_old_diary_entry();
      set_the_date(prev_date);
      show_new_diary_entry();
   } else {
      flash("No previous diary entry exists");
   }
}




/*ARGSUSED*/
void save_diary_callback(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
   save_old_diary_entry();
   if (diary_changed) {
      save_diary(diary_file_name);
      diary_changed = FALSE;
   }
}



/*ARGSUSED*/
void calendar_callback(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
   Date * date_ptr = (Date *) call_data;

   if (diary_showing) {
      save_old_diary_entry();
      set_the_date(*date_ptr);
      show_new_diary_entry();
   } else {
      set_the_date(*date_ptr);
   }
}




void flash(string)
char * string;
{
   currently_flashing = TRUE;
   /*VARARGS0*/
   XtVaSetValues(
      diary_label_widget,
      XtNlabel,             string,
      NULL                  /* end args */
   );
   XtAddTimeOut(ONE_SECOND, flash_back, NULL);
}




void flash_back()
{
   char * pretty_string;
 
   if (display_mode == full) {
      pretty_string = CalendarPrettyDate(full_calendar_widgets[0]);
   } else {
      pretty_string = CalendarPrettyDate(single_calendar_widget);
   }

   /*VARARGS0*/
   XtVaSetValues(
      diary_label_widget,
      XtNlabel,             pretty_string,
      NULL                  /* end args */
   );
   currently_flashing = FALSE;
}
