/* $XConsortium: CurUtil.h,v 1.3 91/07/22 23:45:39 converse Exp $
 *
 * Copyright 1988 by the Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided 
 * that the above copyright notice appear in all copies and that both that 
 * copyright notice and this permission notice appear in supporting 
 * documentation, and that the name of M.I.T. not be used in advertising
 * or publicity pertaining to distribution of the software without specific, 
 * written prior permission. M.I.T. makes no representations about the 
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * The X Window System is a Trademark of MIT.
 *
 * The interfaces described by this header file are for miscellaneous utilities
 * and are not part of the Xlib standard.
 */

#ifndef _XMU_CURUTIL_H_
#define _XMU_CURUTIL_H_

#ifdef VMS
#   include <X11_DIRECTORY/Xfuncproto.h>
#else
#   include <X11/Xfuncproto.h>
#endif

_XFUNCPROTOBEGIN

extern int XmuCursorNameToIndex(
#if NeedFunctionPrototypes
    _Xconst char*	/* name */
#endif
);

_XFUNCPROTOEND

#endif /* _XMU_CURUTIL_H_ */
