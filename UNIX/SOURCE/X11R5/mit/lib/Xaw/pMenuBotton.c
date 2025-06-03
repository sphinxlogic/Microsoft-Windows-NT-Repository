/* libXaw : p.MenuBotton.c
 * -----------------------
 *
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xaw/pMenuBotton.c,v 1.2 1993/03/27 09:11:23 dawes Exp $
 */

#include <X11/IntrinsicP.h>
#include <X11/Xaw/MenuButtoP.h>

#ifdef SVR3SHLIB
#undef menuButtonWidgetClass
WidgetClass menuButtonWidgetClass = (WidgetClass) &menuButtonClassRec;
#endif
