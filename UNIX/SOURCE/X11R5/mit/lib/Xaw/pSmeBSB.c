/* libXaw : pSmeBSB.c
 * ------------------
 *
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xaw/pSmeBSB.c,v 1.2 1993/03/27 09:11:37 dawes Exp $
 */

#include <X11/IntrinsicP.h>
#include <X11/Xaw/SmeBSBP.h>

#ifdef SVR3SHLIB
#undef smeBSBObjectClass
WidgetClass smeBSBObjectClass = (WidgetClass) &smeBSBClassRec;
#endif
