/*
 * $XConsortium: XawInit.c,v 1.2 89/10/09 15:48:42 jim Exp $
 *
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * 
 * 			    XawInitializeWidgetSet
 * 
 * This routine forces a reference to vendor shell so that the one in this
 * widget is installed.  Any other cross-widget set initialization should be
 * done here as well.  All Athena widgets should include "XawInit.h" and
 * call this routine from their ClassInitialize procs (this routine may be
 * used as the class init proc).
 */

#include <stdio.h> /*for message.h ###jp###*/

#ifdef VMS
#   include <X11_DIRECTORY/IntrinsicP.h>
#   include <X11_DIRECTORY/Vendor.h>
#   include <XAW_DIRECTORY/XawInit.h>
#   include <X11_DIRECTORY/StringDefs.h>
#   include <XMU_DIRECTORY/Converters.h>
#else
#   include <X11/Intrinsic.h>
#   include <X11/Vendor.h>
#   include <X11/Xaw3d/XawInit.h>
#endif

/*
#define MESSAGES
#define MESSAGE_NO_ESC
*/
#include "message.h"

#ifdef VMS
static void VMS_SetupXawConverters()
{
    static XtConvertArgRec screenConvertArg[] = {
        {XtWidgetBaseOffset, (XtPointer) XtOffsetOf(WidgetRec, core.screen),
	     sizeof(Screen *)}
    };
    BEGINMESSAGE(VMS_SetupXawConverters)
    XtAddConverter(XtRString, XtRCursor, XmuCvtStringToCursor,
		   screenConvertArg, XtNumber(screenConvertArg));

    XtAddConverter(XtRString, XtRBitmap, XmuCvtStringToBitmap,
		   screenConvertArg, XtNumber(screenConvertArg));

    XtAddConverter(XtRString, XtRPixmap, XmuCvtStringToPixmap,
		   screenConvertArg, XtNumber(screenConvertArg));

    ENDMESSAGE(VMS_SetupXawConverters)
}
#endif

void XawInitializeWidgetSet()
{
    static int firsttime = 1;

    if (firsttime) {
        BEGINMESSAGE(XawInitializeWidgetSet)
	firsttime = 0;
	XtInitializeWidgetClass (vendorShellWidgetClass);
#       ifdef VMS
            VMS_SetupXawConverters();
#       endif
        ENDMESSAGE(XawInitializeWidgetSet)
    }
}
