
/* pConstraint.c
 * -------------
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xt/pConstraint.c,v 1.2 1993/03/27 09:12:59 dawes Exp $
 */
#define CONSTRAINT
#include "IntrinsicI.h"
#include "StringDefs.h"

#ifdef SVR3SHLIB

#undef constraintWidgetClass

externaldef(constraintwidgetclass) WidgetClass constraintWidgetClass =
        (WidgetClass) &constraintClassRec;


#endif

