
/* pObject.c
 * ---------
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xt/pObject.c,v 1.2 1993/03/27 09:13:00 dawes Exp $
 */

#define OBJECT
#include "IntrinsicI.h"
#include "StringDefs.h"

#ifdef SVR3SHLIB

#undef objectClass

externaldef(objectClass) WidgetClass objectClass
	 = (WidgetClass)&objectClassRec;

#endif

