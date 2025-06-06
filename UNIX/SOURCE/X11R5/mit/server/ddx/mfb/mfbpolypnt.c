/* Combined Purdue/PurduePlus patches, level 2.0, 1/17/89 */
/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $XFree86: mit/server/ddx/mfb/mfbpolypnt.c,v 1.4 1993/03/27 09:01:00 dawes Exp $ */
/* $XConsortium: mfbpolypnt.c,v 5.3 89/09/11 10:12:05 rws Exp $ */

#define BANKING_MODS
#include "X.h"
#include "Xprotostr.h"
#include "pixmapstr.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "miscstruct.h"
#include "regionstr.h"
#include "scrnintstr.h"

#include "mfb.h"
#include "maskbits.h"

void
mfbPolyPoint(pDrawable, pGC, mode, npt, pptInit)
    register DrawablePtr pDrawable;
    GCPtr	pGC;
    int		mode;		/* Origin or Previous */
    int		npt;
    xPoint 	*pptInit;
{

    register BoxPtr pbox;
    register int nbox;

    register int *addrl;
    int nlwidth;

    int nptTmp;
    register xPoint *ppt;

    register int x;
    register int y;
    register int rop;
    mfbPrivGC	*pGCPriv;

    if (!(pGC->planemask & 1))
	return;

    pGCPriv = (mfbPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr;
    rop = pGCPriv->rop;

    if (pDrawable->type == DRAWABLE_WINDOW)
    {
	addrl = (int *)
	        (((PixmapPtr)(pDrawable->pScreen->devPrivate))->devPrivate.ptr);
	nlwidth = (int)
		  (((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind) >> 2;
    }
    else
    {
	addrl = (int *)(((PixmapPtr)pDrawable)->devPrivate.ptr);
	nlwidth = (int)(((PixmapPtr)pDrawable)->devKind) >> 2;
    }

    BANK_FLAG(addrl)

    if ((mode == CoordModePrevious) && (npt > 1))
    {
	for (ppt = pptInit + 1, nptTmp = npt - 1; --nptTmp >= 0; ppt++)
	{
	    ppt->x += (ppt-1)->x;
	    ppt->y += (ppt-1)->y;
	}
    }

    nbox = REGION_NUM_RECTS(pGCPriv->pCompositeClip);
    pbox = REGION_RECTS(pGCPriv->pCompositeClip);
    for (; --nbox >= 0; pbox++)
    {
        int *tmpaddr;
	if (rop == RROP_BLACK)
	{
	    for (ppt = pptInit, nptTmp = npt; --nptTmp >= 0; ppt++)
	    {
		x = ppt->x + pDrawable->x;
		y = ppt->y + pDrawable->y;
		if ((x >= pbox->x1) && (x < pbox->x2) &&
		    (y >= pbox->y1) && (y < pbox->y2))
                {
                    tmpaddr = addrl + (y * nlwidth) + (x >> 5);
                    SETRW(tmpaddr);
		    *MAPRW(tmpaddr) &= rmask[x & 0x1f];
                }
	    }
	}
	else if (rop == RROP_WHITE)
	{
	    for (ppt = pptInit, nptTmp = npt; --nptTmp >= 0; ppt++)
	    {
		x = ppt->x + pDrawable->x;
		y = ppt->y + pDrawable->y;
		if ((x >= pbox->x1) && (x < pbox->x2) &&
		    (y >= pbox->y1) && (y < pbox->y2))
                {
		    tmpaddr = addrl + (y * nlwidth) + (x >> 5);
                    SETRW(tmpaddr);
		    *MAPRW(tmpaddr) |= mask[x & 0x1f];
                }
	    }
	}
	else if (rop == RROP_INVERT)
	{
	    for (ppt = pptInit, nptTmp = npt; --nptTmp >= 0; ppt++)
	    {
		x = ppt->x + pDrawable->x;
		y = ppt->y + pDrawable->y;
		if ((x >= pbox->x1) && (x < pbox->x2) &&
		    (y >= pbox->y1) && (y < pbox->y2))
                {
		    tmpaddr = addrl + (y * nlwidth) + (x >> 5);
                    SETRW(tmpaddr);
		    *MAPRW(tmpaddr) ^= mask[x & 0x1f];
                }
	    }
	}
    }
}
