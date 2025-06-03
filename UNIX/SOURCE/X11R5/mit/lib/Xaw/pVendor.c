/* libXaw : pVendor.c
 * ------------------
 *
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xaw/pVendor.c,v 1.2 1993/03/27 09:11:49 dawes Exp $
 */

#include <X11/IntrinsicP.h>
#include <X11/Shell.h>
#include <X11/ShellP.h>
#include <X11/Vendor.h>
#include <X11/VendorP.h>

#ifdef SVR3SHLIB
#undef vendorShellWidgetClass
WidgetClass vendorShellWidgetClass = (WidgetClass) &vendorShellClassRec;
#endif
