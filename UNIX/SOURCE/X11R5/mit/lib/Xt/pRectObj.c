
/* pRectObj.c
 * ----------
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xt/pRectObj.c,v 1.2 1993/03/27 09:13:02 dawes Exp $
 */

#define RECTOBJ
#include "IntrinsicI.h"
#include "StringDefs.h"

#ifdef SVR3SHLIB

#undef rectObjClass

externaldef(rectObjClass)
WidgetClass rectObjClass = (WidgetClass)&rectObjClassRec;


#endif

