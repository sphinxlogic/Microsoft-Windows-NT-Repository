/* libXaw : libXaw.c
 * exported data
 * Template created by mkshtmpl.sh 1.4
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 * Conception derived from work of Thomas Roell
 *
 * $XFree86: mit/lib/Xaw/globals.c,v 1.3 1993/03/20 03:33:49 dawes Exp $
 */

#include <X11/IntrinsicP.h>
#include <X11/Xmu/WidgetNode.h>
#include <X11/Xaw/AllWidgets.h>
#include <X11/Xaw/TextP.h>

#ifdef SVR3SHLIB

#ifdef __STDC__
#define Const const
#else
#define Const /**/
#endif

#define VoidInit(var) void *var = 0

Const char *_shared_libXaw_version = SOXAWREV;  char __libXaw_sp1__[16] = "";

unsigned long FMT8BIT = 0L;

extern char __XawDefaultTextTranslations1[];
extern char __XawDefaultTextTranslations2[];
extern char __XawDefaultTextTranslations3[];

char *_XawDefaultTextTranslations1 = __XawDefaultTextTranslations1;
char *_XawDefaultTextTranslations2 = __XawDefaultTextTranslations2;
char *_XawDefaultTextTranslations3 = __XawDefaultTextTranslations3;

Cardinal _XawTextActionsTableCount = 0;

VoidInit(_libXaw__aux__);
VoidInit(_libXaw_XShapeCombineMask);
VoidInit(_libXaw_XShapeQueryExtension);
VoidInit(_libXaw__ctype);
VoidInit(_libXaw__iob);
VoidInit(_libXaw_asctime);
VoidInit(_libXaw_atof);
VoidInit(_libXaw_atoi);
VoidInit(_libXaw_bcopy);
VoidInit(_libXaw_close);
VoidInit(_libXaw_creat);
VoidInit(_libXaw_errno);
VoidInit(_libXaw_exit);
VoidInit(_libXaw_fclose);
VoidInit(_libXaw_fopen);
VoidInit(_libXaw_fprintf);
VoidInit(_libXaw_fread);
VoidInit(_libXaw_fseek);
VoidInit(_libXaw_ftell);
VoidInit(_libXaw_getlogin);
VoidInit(_libXaw_getpwuid);
VoidInit(_libXaw_getuid);
VoidInit(_libXaw_localtime);
VoidInit(_libXaw_memset);
VoidInit(_libXaw_open);
VoidInit(_libXaw_printf);
VoidInit(_libXaw_read);
VoidInit(_libXaw_sprintf);
VoidInit(_libXaw_stat);
VoidInit(_libXaw_strcat);
VoidInit(_libXaw_strchr);
VoidInit(_libXaw_strcmp);
VoidInit(_libXaw_strcpy);
VoidInit(_libXaw_strncpy);
VoidInit(_libXaw_sys_errlist);
VoidInit(_libXaw_sys_nerr);
VoidInit(_libXaw_system);
VoidInit(_libXaw_time);
VoidInit(_libXaw_tmpnam);
VoidInit(_libXaw_toupper);
VoidInit(_libXaw_write);
VoidInit(_libXaw__XawTextActionsTableCount);

VoidInit(_libXaw_asciiSinkObjectClass);
VoidInit(_libXaw_asciiSrcObjectClass);
VoidInit(_libXaw_asciiTextWidgetClass);
VoidInit(_libXaw_boxWidgetClass);
VoidInit(_libXaw_clockWidgetClass);
VoidInit(_libXaw_commandWidgetClass);
VoidInit(_libXaw_dialogWidgetClass);
VoidInit(_libXaw_formWidgetClass);
VoidInit(_libXaw_gripWidgetClass);
VoidInit(_libXaw_labelWidgetClass);
VoidInit(_libXaw_listWidgetClass);
VoidInit(_libXaw_logoWidgetClass);
VoidInit(_libXaw_mailboxWidgetClass);
VoidInit(_libXaw_menuButtonWidgetClass);
VoidInit(_libXaw_panedWidgetClass);
VoidInit(_libXaw_pannerWidgetClass);
VoidInit(_libXaw_portholeWidgetClass);
VoidInit(_libXaw_repeaterWidgetClass);
VoidInit(_libXaw_scrollbarWidgetClass);
VoidInit(_libXaw_simpleWidgetClass);
VoidInit(_libXaw_simpleMenuWidgetClass);
VoidInit(_libXaw_smeObjectClass);
VoidInit(_libXaw_smeBSBObjectClass);
VoidInit(_libXaw_smeLineObjectClass);
VoidInit(_libXaw_stripChartWidgetClass);
VoidInit(_libXaw_textWidgetClass);
VoidInit(_libXaw_textSinkObjectClass);
VoidInit(_libXaw_textSrcObjectClass);
VoidInit(_libXaw_toggleWidgetClass);
VoidInit(_libXaw_treeWidgetClass);
VoidInit(_libXaw_vendorShellWidgetClass);
VoidInit(_libXaw_viewportWidgetClass);

VoidInit(_libXaw_transientShellWidgetClass);

VoidInit(_libXaw__XA_CHARACTER_POSITION);
VoidInit(_libXaw__XA_COMPOUND_TEXT);
VoidInit(_libXaw__XA_DELETE);
VoidInit(_libXaw__XA_LENGTH);
VoidInit(_libXaw__XA_LIST_LENGTH);
VoidInit(_libXaw__XA_NULL);
VoidInit(_libXaw__XA_SPAN);
VoidInit(_libXaw__XA_TARGETS);
VoidInit(_libXaw__XA_TEXT);

void *__libXaw_p__[FUTURE_WIDGET_COUNT] = {0};


#ifndef __GNUC__
/*
 * If we are working with floating point aritmetic, stock AT&T cc generates
 * an unresolved reference to __fltused. But we want to make a shared lib from
 * this here and don't want to reference /lib/libc_s.a, just define this sym as
 * (shared lib) static.
 * The trick is that while building the shared lib all references to this
 * symbol are resolved internally. But the symbol will be outside only visible
 * as a static one, so preventing a name conflict with other shared libs.
 */
long __fltused = 0;
#endif

/* A dummy function for free branchtab-slots.
 * This would be a great place for an error-check mechanism for shared libs.
 * (Print error message if not existent function is called.)
 */
extern void _libXaw_dummy() {};


/*
 * This function is for future versions of the library which could
 * want initialize pointers to default values, which clients linked with
 * the old version don't initialize. It's called from inside the
 * .init section of the clients.
 */
extern Cardinal __XawTextActionsTableCount;

void __default_libXaw_init_()
{

  _XawTextActionsTableCount = __XawTextActionsTableCount;

}

#endif

