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
 * KEVIN E. MARTIN, RICKARD E. FAITH, AND TIAGO GONS DISCLAIM
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * KEVIN E. MARTIN, RICKARD E. FAITH, OR TIAGO GONS BE LIABLE
 * FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Further modifications by Tiago Gons (tiago@comosjn.hobby.nl)
 * Adapted to use accelerated text output without font cache.
 * Hans Nasten (nasten@everyware.se)
 *
 */

/* $XFree86: mit/server/ddx/x386/accel/mach8/mach8text.c,v 2.5 1994/01/09 03:30:30 dawes Exp $ */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"misc.h"
#include	"gcstruct.h"
#include	"fontstruct.h"
#include        "scrnintstr.h"
#include        "pixmapstr.h"
#include	"dixfontstr.h"
#include	"mi.h"
#include	"mach8.h"

extern int mach8CPolyText8();
extern int mach8CImageText8();

int
mach8PolyText8(pDraw, pGC, x, y, count, chars)
    DrawablePtr pDraw;
    GCPtr	pGC;
    int		x, y;
    int 	count;
    char	*chars;
{
    void *vpnt;

/* 11-jun-93 TCG : is VT visible */
    if (!x386VTSema)
    {
        return miPolyText8(pDraw, pGC, x, y, count, chars);
    }

    if (((vpnt = mach8CacheFont8(pGC->font)) == NULL) ||
	(pGC->fillStyle != FillSolid))
	return miPolyText8(pDraw, pGC, x, y, count, chars);
    else
	return mach8CPolyText8(pDraw, pGC, x, y, count, chars, vpnt);
}


int
mach8PolyText16(pDraw, pGC, x, y, count, chars)
    DrawablePtr pDraw;
    GCPtr	pGC;
    int		x, y;
    int		count;
    unsigned short *chars;
{
    return miPolyText16(pDraw, pGC, x, y, count, chars);
}


void
mach8ImageText8(pDraw, pGC, x, y, count, chars)
    DrawablePtr pDraw;
    GCPtr	pGC;
    int		x, y;
    int		count;
    char	*chars;
{
/* 11-jun-93 TCG : is VT visible */
    if (!x386VTSema)
    {
        miImageText8(pDraw, pGC, x, y, count, chars);
        return;
    }

    /* Don't need to check fill style here - it isn't used in image text */

    mach8CImageText8( pDraw, pGC, x, y, count, chars,
		       mach8CacheFont8(pGC->font) );

}


void
mach8ImageText16(pDraw, pGC, x, y, count, chars)
    DrawablePtr pDraw;
    GCPtr	pGC;
    int		x, y;
    int		count;
    unsigned short *chars;
{
    miImageText16(pDraw, pGC, x, y, count, chars);
}
