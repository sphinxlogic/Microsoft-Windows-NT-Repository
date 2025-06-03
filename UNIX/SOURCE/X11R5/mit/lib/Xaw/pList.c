/* libXaw : pList.c
 * ----------------
 *
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xaw/pList.c,v 1.2 1993/03/27 09:11:18 dawes Exp $
 */

#include <X11/IntrinsicP.h>
#include <X11/Xaw/ListP.h>

#ifdef SVR3SHLIB
#undef listWidgetClass
WidgetClass listWidgetClass = (WidgetClass)&listClassRec;
#endif
