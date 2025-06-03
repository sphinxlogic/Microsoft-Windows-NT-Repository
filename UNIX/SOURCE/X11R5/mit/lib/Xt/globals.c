/* libXt : libXt.c
 * exported data
 * Template created by mkshtmpl.sh 1.4
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 * Conception derived partially from work of Thomas Roell
 *
 * $XFree86: mit/lib/Xt/globals.c,v 1.3 1993/03/20 03:40:55 dawes Exp $
 */

#include "IntrinsicI.h"         /* to get XrmQuark,      */
                                /* TMGlobalRec, String   */

#ifdef SVR3SHLIB

#ifdef __STDC__
#define Const const
#else
#define Const /**/
#endif

#define VoidInit(var) void *var = 0

Const char *_shared_libXt_version = SOXTREV;  char __libXt__sp1__[16] = "";

String XtCXtToolkitError = "XtToolkitError";  char __libXt__sp2__[32] = "";
TMGlobalRec _XtGlobalTM = {0};
int _XtInheritTranslations = 0;
XrmQuark  _XtQString = 0;

VoidInit(_libXt__aux__);
VoidInit(_libXt__ctype);
VoidInit(_libXt__iob);
VoidInit(_libXt_access);
VoidInit(_libXt_atof);
VoidInit(_libXt_calloc);
VoidInit(_libXt_errno);
VoidInit(_libXt_exit);
VoidInit(_libXt_fclose);
VoidInit(_libXt_fopen);
VoidInit(_libXt_fprintf);
VoidInit(_libXt_free);
VoidInit(_libXt_getenv);
VoidInit(_libXt_gethostname);
VoidInit(_libXt_getpwnam);
VoidInit(_libXt_getpwuid);
VoidInit(_libXt_gettimeofday);
VoidInit(_libXt_getuid);
VoidInit(_libXt_malloc);
VoidInit(_libXt_memset);
VoidInit(_libXt_printf);
VoidInit(_libXt_qsort);
VoidInit(_libXt_realloc);
VoidInit(_libXt_select);
VoidInit(_libXt_sprintf);
VoidInit(_libXt_stat);
VoidInit(_libXt_strcat);
VoidInit(_libXt_strchr);
VoidInit(_libXt_strcmp);
VoidInit(_libXt_strcpy);
VoidInit(_libXt_strncpy);
VoidInit(_libXt_strrchr);
VoidInit(_libXt__XtInherit);	/* must be imported */
VoidInit(_libXt__XtCXtToolkitError);
VoidInit(_libXt__XtGlobalTM);
/* VoidInit(_libXt__XtInheritTranslations); */
VoidInit(_libXt__XtQString);
/*
VoidInit(_libXt_XtShellStrings);
VoidInit(_libXt_XtStrings);
*/

/*
 * Composite.c
 */
VoidInit(_libXt_compositeWidgetClass);

/*
 * Constraint.c
 */
VoidInit(_libXt_constraintWidgetClass);

/*
 * Core.c
 */
VoidInit(_libXt_coreWidgetClass);

/*
 * Object.c
 */
/* objectClass not referenced in .text section of any libXt Source */

/*
 * RectObj.c
 */
VoidInit(_libXt_rectObjClass);

/*
 * Shell.c
 */
VoidInit(_libXt_shellWidgetClass);
VoidInit(_libXt_overrideShellWidgetClass);
VoidInit(_libXt_wmShellWidgetClass);
VoidInit(_libXt_transientShellWidgetClass);
VoidInit(_libXt_topLevelShellWidgetClass);
VoidInit(_libXt_applicationShellWidgetClass);

/*
 * Vendor.c
 */
VoidInit(_libXt_vendorShellWidgetClass);



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
extern void _libXt_dummy() {};

/*
 * This function is for future versions of the library which could
 * want initialize pointers to default values, which clients linked with
 * the old version don't initialize. It's called from inside the
 * .init section of the clients.
 */
void __default_libXt_init_()
{

}

#endif


