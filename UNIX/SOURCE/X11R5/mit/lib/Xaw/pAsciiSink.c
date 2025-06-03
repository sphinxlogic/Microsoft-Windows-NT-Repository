/* libXaw : pAsciiSink.c
 * ---------------------
 *
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xaw/pAsciiSink.c,v 1.2 1993/03/27 09:11:03 dawes Exp $
 */

#include <X11/IntrinsicP.h>
#include <X11/Xaw/AsciiSinkP.h>

#ifdef SVR3SHLIB
#undef asciiSinkObjectClass
WidgetClass asciiSinkObjectClass = (WidgetClass)&asciiSinkClassRec;
#endif
