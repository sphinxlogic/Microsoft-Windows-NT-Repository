
/* pComposite.c
 * ------------
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xt/pComposite.c,v 1.2 1993/03/27 09:12:57 dawes Exp $
 */
#define COMPOSITE
#include "IntrinsicI.h"
#include "StringDefs.h"

#ifdef SVR3SHLIB

#undef compositeWidgetClass

externaldef(compositewidgetclass) WidgetClass compositeWidgetClass = (WidgetClass) &compositeClassRec;


#endif

