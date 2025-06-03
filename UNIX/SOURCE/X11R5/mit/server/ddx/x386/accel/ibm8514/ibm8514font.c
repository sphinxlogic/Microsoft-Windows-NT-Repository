/*
 * Copyright 1992 by Kevin E. Martin, Chapel Hill, North Carolina.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Kevin E. Martin not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Kevin E. Martin makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: mit/server/ddx/x386/accel/ibm8514/ibm8514font.c,v 2.0 1993/08/06 11:48:39 dawes Exp $ */

#include "X.h"
#include "Xmd.h"
#include "Xproto.h"
#include "mfb.h"
#include "fontstruct.h"
#include "dixfontstr.h"
#include "scrnintstr.h"
#include "ibm8514.h"

Bool
ibm8514RealizeFont(pScreen, font)
    ScreenPtr	pScreen;
    FontPtr	font;
{
    /* We _should_ probably be caching things here */
    /* (void)ibm8514CacheFont8(font); */
    return mfbRealizeFont(pScreen, font);
}

Bool
ibm8514UnrealizeFont(pScreen, font)
    ScreenPtr	pScreen;
    FontPtr	font;
{
    ibm8514UnCacheFont8(font);
    return mfbUnrealizeFont(pScreen, font);
}
