/* libXaw : pLabel.c
 * -----------------
 *
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xaw/pLabel.c,v 1.3 1993/05/22 06:43:37 dawes Exp $
 */

#include <X11/IntrinsicP.h>
#include <X11/Xaw/LabelP.h>

#ifdef SVR3SHLIB
#undef labelWidgetClass
WidgetClass labelWidgetClass = (WidgetClass)&labelClassRec;
#endif
