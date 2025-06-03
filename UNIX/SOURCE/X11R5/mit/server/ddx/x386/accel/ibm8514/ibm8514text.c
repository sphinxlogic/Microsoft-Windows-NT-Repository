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
 * KEVIN E. MARTIN AND TIAGO GONS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL KEVIN E. MARTIN OR TIAGO GONS BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified by Tiago Gons (tiago@comosjn.hobby.nl)
 */

/* $XFree86: mit/server/ddx/x386/accel/ibm8514/ibm8514text.c,v 2.3 1993/12/25 13:57:37 dawes Exp $ */

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
#include	"ibm8514.h"

extern int ibm8514CPolyText8();
extern int ibm8514CImageText8();

int
ibm8514PolyText8(pDraw, pGC, x, y, count, chars)
    DrawablePtr pDraw;
    GCPtr	pGC;
    int		x, y;
    int 	count;
    char	*chars;
{
    int plane;

/* 4-5-93 TCG : is VT visible */
    if (!x386VTSema)
    {
        return miPolyText8(pDraw, pGC, x, y, count, chars);
    }

    if (((plane = ibm8514CacheFont8(pGC->font)) == -1) ||
	(pGC->fillStyle != FillSolid))
	return miPolyText8(pDraw, pGC, x, y, count, chars);
    else
	return ibm8514CPolyText8(pDraw, pGC, x, y, count, chars, plane);
}


int
ibm8514PolyText16(pDraw, pGC, x, y, count, chars)
    DrawablePtr pDraw;
    GCPtr	pGC;
    int		x, y;
    int		count;
    unsigned short *chars;
{
    return miPolyText16(pDraw, pGC, x, y, count, chars);
}


void
ibm8514ImageText8(pDraw, pGC, x, y, count, chars)
    DrawablePtr pDraw;
    GCPtr	pGC;
    int		x, y;
    int		count;
    char	*chars;
{
    int plane;

/* 4-5-93 TCG : is VT visible */
    if (!x386VTSema)
    {
        miImageText8(pDraw, pGC, x, y, count, chars);
        return;
    }

    /* Don't need to check fill style here - it isn't used in image text */
    if ((plane = ibm8514CacheFont8(pGC->font)) == -1)
	miImageText8(pDraw, pGC, x, y, count, chars);
    else
	ibm8514CImageText8(pDraw, pGC, x, y, count, chars, plane);
}


void
ibm8514ImageText16(pDraw, pGC, x, y, count, chars)
    DrawablePtr pDraw;
    GCPtr	pGC;
    int		x, y;
    int		count;
    unsigned short *chars;
{
    miImageText16(pDraw, pGC, x, y, count, chars);
}
