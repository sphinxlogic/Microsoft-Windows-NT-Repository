/* * Last edited: Aug 29 14:37 1991 (mallet) */
#ifndef lint
static char rcsid[] = "$Id: StrToPmap.c,v 1.1 1991/09/06 07:48:33 mallet Exp $";
#endif /* lint */

/*
 * Copyright 1991 Lionel Mallet
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Lionel MALLET not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Lionel MALLET makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Lionel MALLET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL Lionel MALLET BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Lionel Mallet - SIMULOG
 */


#ifdef VMS
#   include <X11_DIRECTORY/Intrinsic.h>
#   include <X11_DIRECTORY/StringDefs.h>
#   include <Xmu_DIRECTORY/Converters.h>
#   include <Xmu_DIRECTORY/Drawing.h>
#else
#   include <X11/Intrinsic.h>
#   include <X11/StringDefs.h>
#   include <X11/Xmu/Converters.h>
#   include <X11/Xmu/Drawing.h>
#endif

/*
 * XmuConvertStringToPixmap:
 *
 * creates a depth-1 Pixmap suitable for window manager icons.
 * "string" represents a pixmap(1) filename which may be absolute,
 * or relative to the global resource pixmapFilePath, class
 * PixmapFilePath.  If the resource is not defined, the default
 * value is the build symbol PIXMAPDIR.
 *
 * To use, include the following in your ClassInitialize procedure:

static XtConvertArgRec screenConvertArg[] = {
    {XtBaseOffset, (caddr_t) XtOffset(Widget, core.screen), sizeof(Screen *)}
};

    XtAddConverter("String", "Pixmap", XmuCvtStringToPixmap,
		   screenConvertArg, XtNumber(screenConvertArg));
 *
 */

#define	done(address, type) \
	{ (*toVal).size = sizeof(type); (*toVal).addr = (caddr_t) address; }


/*ARGSUSED*/
void XmuCvtStringToPixmap(args, num_args, fromVal, toVal)
    XrmValuePtr args;
    Cardinal    *num_args;
    XrmValuePtr	fromVal;
    XrmValuePtr	toVal;
{
    static Pixmap pixmap;		/* static for cvt magic */
    char *name = (char *)fromVal->addr;

    if (*num_args != 1)
     XtErrorMsg("wrongParameters","cvtStringToPixmap","XtToolkitError",
             "String to pixmap conversion needs screen argument",
              (String *)NULL, (Cardinal *)NULL);

/* DRM, 25-JAN-2000, some applications use "unspecified_pixmap" */
    if ( (strcmp(name, "None") == 0) ||
         (strcmp(name, "unspecified_pixmap") == 0)  ) {
	pixmap = None;
	done(&pixmap, Pixmap);
	return;
    }

    if (strcmp(name, "ParentRelative") == 0) {
	pixmap = ParentRelative;
	done(&pixmap, Pixmap);
	return;
    }

    pixmap = (Pixmap) XmuLocatePixmapFile(*((Screen **)args[0].addr),name,0,0, 0, 0,0,0,0,0,0);/*###jp### changed 23.10.94*/

    if (pixmap != None) {
	done (&pixmap, Pixmap);
    } else {/*###jp### show some warning */
       XtStringConversionWarning(name, "Pixmap (converter: XmuCvtStringToPixmap)");
       return;
    }
}

