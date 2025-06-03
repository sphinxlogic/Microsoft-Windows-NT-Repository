/* libXaw : pClock.c
 * -----------------
 *
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xaw/pClock.c,v 1.2 1993/03/27 09:11:09 dawes Exp $
 */

#include        <X11/IntrinsicP.h>
#include        <X11/Xaw/ClockP.h>

#ifdef SVR3SHLIB
#undef clockWidgetClass
WidgetClass clockWidgetClass = (WidgetClass) &clockClassRec;
#endif
