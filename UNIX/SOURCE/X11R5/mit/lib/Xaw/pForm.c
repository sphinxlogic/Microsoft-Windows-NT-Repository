/* libXaw : pForm.c
 * ----------------
 *
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xaw/pForm.c,v 1.2 1993/03/27 09:11:14 dawes Exp $
 */

#include <X11/IntrinsicP.h>
#include <X11/Xaw/FormP.h>

#ifdef SVR3SHLIB
#undef formWidgetClass
WidgetClass formWidgetClass = (WidgetClass)&formClassRec;
#endif
