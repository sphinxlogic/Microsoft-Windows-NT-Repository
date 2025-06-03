/*
 *	string-convert.c : A kludgy way to prevent getting a warning
 *		message of the form "can't convert NULL to type Widget".
 *
 *	George Ferguson, ferguson@cs.rochester.edu, 8 Feb 1991.
 *
 *	This code is lifted from lib/Xt/Converters.c in the X11R4 source
 *	tree. I would register my own converter for string to widget, but
 *	there's no way to get the default converter, so I'd end up copying
 *	or rewriting even more code. By the way, this works because the
 *	converter returns NULL when it fails, which is what we want.
 *
 *	$Id: string-convert.c,v 2.0 91/02/08 15:55:32 ferguson Exp $
 */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

/* from IntrinsicI.h */
static String XtNconversionError = "conversionError";
extern String XtCXtToolkitError;

void XtStringConversionWarning(from, toType)
/* In the Destiny X11/Intrinsic.h this function is defined with these parameters
 * (Greg Lehey, LEMIS, 18 May 1993). Don't know if it will work, but since we
 * have the function available in libXt, it's not clear why we need this at all */
#ifdef __svr4__
    const char *from, *toType;
#else
    String from, toType;
#endif
{
	String params[2];
	Cardinal num_params = 2;

	if (strcasecmp(from,"NULL") == 0 &&
	    strcasecmp(toType,XtRWidget) == 0)
	    return;

	params[0] = from;
	params[1] = toType;
	XtWarningMsg(XtNconversionError,"string",XtCXtToolkitError,
		    "Cannot convert string \"%s\" to type %s",
		    params,&num_params);
}
