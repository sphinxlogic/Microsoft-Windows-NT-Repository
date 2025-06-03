
/* pVendor.c
 * ---------
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xt/pVendor.c,v 1.2 1993/03/27 09:13:05 dawes Exp $
 */

#include "IntrinsicI.h"
#include "StringDefs.h"
#include "Shell.h"
#include "ShellP.h"
#include "Vendor.h"
#include "VendorP.h"
#include <stdio.h>

#ifdef SVR3SHLIB

#undef vendorShellWidgetClass

externaldef(vendorshellwidgetclass) WidgetClass vendorShellWidgetClass =
        (WidgetClass) (&vendorShellClassRec);

#endif

