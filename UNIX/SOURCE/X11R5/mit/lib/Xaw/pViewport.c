/* libXaw : pViewport.c
 * --------------------
 *
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xaw/pViewport.c,v 1.2 1993/03/27 09:11:51 dawes Exp $
 */

#include <X11/IntrinsicP.h>
#include <X11/Xaw/ViewportP.h>

#ifdef SVR3SHLIB
#undef viewportWidgetClass
WidgetClass viewportWidgetClass = (WidgetClass)&viewportClassRec;
#endif
