/* libXaw : pPanner.c
 * ------------------
 *
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xaw/pPanner.c,v 1.2 1993/03/27 09:11:26 dawes Exp $
 */

#include <X11/IntrinsicP.h>
#include <X11/Xaw/PannerP.h>

#ifdef SVR3SHLIB
#undef pannerWidgetClass
WidgetClass pannerWidgetClass = (WidgetClass) &pannerClassRec;
#endif
