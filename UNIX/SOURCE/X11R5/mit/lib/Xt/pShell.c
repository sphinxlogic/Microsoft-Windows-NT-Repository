
/* pShell.c
 * --------
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xt/pShell.c,v 1.2 1993/03/27 09:13:04 dawes Exp $
 */

#define SHELL

#ifndef DEFAULT_WM_TIMEOUT
#define DEFAULT_WM_TIMEOUT 5000
#endif

#include "IntrinsicI.h"
#include "StringDefs.h"
#include "Shell.h"
#include "ShellP.h"
#include "Vendor.h"
#include "VendorP.h"
#include <X11/Xatom.h>
#include <X11/Xlocale.h>
#include <stdio.h>

#ifdef SVR3SHLIB

#undef shellWidgetClass
#undef overrideShellWidgetClass
#undef wmShellWidgetClass
#undef transientShellWidgetClass
#undef topLevelShellWidgetClass
#undef applicationShellWidgetClass

externaldef(shellwidgetclass) WidgetClass shellWidgetClass =
	(WidgetClass) (&shellClassRec);

externaldef(overrideshellwidgetclass) WidgetClass overrideShellWidgetClass =
        (WidgetClass) (&overrideShellClassRec);

externaldef(wmshellwidgetclass) WidgetClass wmShellWidgetClass =
	(WidgetClass) (&wmShellClassRec);

externaldef(transientshellwidgetclass) WidgetClass transientShellWidgetClass =
        (WidgetClass) (&transientShellClassRec);

externaldef(toplevelshellwidgetclass) WidgetClass topLevelShellWidgetClass =
        (WidgetClass) (&topLevelShellClassRec);

externaldef(applicationshellwidgetclass) WidgetClass applicationShellWidgetClass =
        (WidgetClass) (&applicationShellClassRec);

#endif
