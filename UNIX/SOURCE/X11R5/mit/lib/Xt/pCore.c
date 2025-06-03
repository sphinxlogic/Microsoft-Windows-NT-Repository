
/* pCore.c
 * -------
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xt/pCore.c,v 1.3 1993/03/20 03:40:59 dawes Exp $
 */

#define _XT_CORE_C

#include "IntrinsicP.h"
#include "EventI.h"
#include "TranslateI.h"
#include "ResourceI.h"
#include "RectObj.h"
#include "RectObjP.h"
#include "StringDefs.h"

#ifdef SVR3SHLIB

externalref WidgetClass _libXtCore_unNamedObjClassRec;

#undef widgetClass
#undef coreWidgetClass

externaldef(WidgetClass) WidgetClass _libXtCore_unNamedObjClass =
        (WidgetClass) &_libXtCore_unNamedObjClassRec;

externaldef (WidgetClass) WidgetClass widgetClass = &widgetClassRec;

externaldef (WidgetClass) WidgetClass coreWidgetClass = &widgetClassRec;

#endif

