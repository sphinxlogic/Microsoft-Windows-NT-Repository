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
/* $XFree86: mit/server/ddx/mfb/mfbtile.c,v 1.4 1993/03/27 09:01:06 dawes Exp $ */
/* $XConsortium: mfbtile.c,v 5.3 90/05/15 18:38:21 keith Exp $ */
#define BANKING_MODS
#include "X.h"

#include "windowstr.h"
#include "regionstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"

#include "mfb.h"
#include "maskbits.h"

#include "mergerop.h"
/* 

   the boxes are already translated.

   NOTE:
   iy = ++iy < tileHeight ? iy : 0
is equivalent to iy%= tileheight, and saves a division.
*/

/* 
    tile area with a 32 bit wide pixmap 
*/
void
MROP_NAME(mfbTileArea32)(pDraw, nbox, pbox, alu, ptile)
    DrawablePtr pDraw;
    int nbox;
    BoxPtr pbox;
    int alu;
    PixmapPtr ptile;
{
    register unsigned int *psrc;
			/* pointer to bits in tile, if needed */
    int tileHeight;	/* height of the tile */
    register unsigned int srcpix;	

    int nlwidth;	/* width in longwords of the drawable */
    int w;		/* width of current box */
    MROP_DECLARE_REG ()
    register int h;	/* height of current box */
    register int nlw;	/* loop version of nlwMiddle */
    register unsigned int *p;	/* pointer to bits we're writing */
    int startmask;
    int endmask;	/* masks for reggedy bits at either end of line */
    int nlwMiddle;	/* number of longwords between sides of boxes */
    int nlwExtra;	/* to get from right of box to left of next span */
    
    register int iy;	/* index of current scanline in tile */


    unsigned int *pbits;	/* pointer to start of drawable */

    if (pDraw->type == DRAWABLE_WINDOW)
    {
	pbits = (unsigned int *)
		(((PixmapPtr)(pDraw->pScreen->devPrivate))->devPrivate.ptr);
	nlwidth = (int)
		(((PixmapPtr)(pDraw->pScreen->devPrivate))->devKind) >> 2;
    }
    else
    {
	pbits = (unsigned int *)(((PixmapPtr)pDraw)->devPrivate.ptr);
	nlwidth = (int)(((PixmapPtr)pDraw)->devKind) >> 2;
    }

    BANK_FLAG(pbits)

    MROP_INITIALIZE(alu,~0)

    tileHeight = ptile->drawable.height;
    psrc = (unsigned int *)(ptile->devPrivate.ptr);

    while (nbox--)
    {
	w = pbox->x2 - pbox->x1;
	h = pbox->y2 - pbox->y1;
	iy = pbox->y1 % tileHeight;
	p = pbits + (pbox->y1 * nlwidth) + (pbox->x1 >> 5);
        SETRW(p);

	if ( ((pbox->x1 & 0x1f) + w) < 32)
	{
	    maskpartialbits(pbox->x1, w, startmask);
	    nlwExtra = nlwidth;
	    while (h--)
	    {
		srcpix = psrc[iy];
		iy++;
		if (iy == tileHeight)
		    iy = 0;
		*MAPW(p) = MROP_MASK(srcpix,*MAPR(p),startmask);
		p += nlwExtra; CHECKRWO(p);
	    }
	}
	else
	{
	    maskbits(pbox->x1, w, startmask, endmask, nlwMiddle);
	    nlwExtra = nlwidth - nlwMiddle;

	    if (startmask && endmask)
	    {
		nlwExtra -= 1;
		while (h--)
		{
		    srcpix = psrc[iy];
		    iy++;
		    if (iy == tileHeight)
			iy = 0;
		    nlw = nlwMiddle;
		    *MAPW(p) = MROP_MASK (srcpix,*MAPR(p),startmask);
		    p++; CHECKRWO(p);
		    while (nlw--)
		    {
			*MAPW(p) = MROP_SOLID(srcpix,*MAPR(p));
			p++; CHECKRWO(p);
		    }

		    *MAPW(p) = MROP_MASK(srcpix,*MAPR(p),endmask);
		    p += nlwExtra; CHECKRWO(p);
		}
	    }
	    else if (startmask && !endmask)
	    {
		nlwExtra -= 1;
		while (h--)
		{
		    srcpix = psrc[iy];
		    iy++;
		    if (iy == tileHeight)
			iy = 0;
		    nlw = nlwMiddle;
		    *MAPW(p) = MROP_MASK(srcpix,*MAPR(p),startmask);
		    p++; CHECKRWO(p);
		    while (nlw--)
		    {
			*MAPW(p) = MROP_SOLID(srcpix,*MAPR(p));
			p++; CHECKRWO(p);
		    }
		    p += nlwExtra; CHECKRWO(p);
		}
	    }
	    else if (!startmask && endmask)
	    {
		while (h--)
		{
		    srcpix = psrc[iy];
		    iy++;
		    if (iy == tileHeight)
			iy = 0;
		    nlw = nlwMiddle;
		    while (nlw--)
		    {
			*MAPW(p) = MROP_SOLID(srcpix,*MAPR(p));
			p++; CHECKRWO(p);
		    }

		    *MAPW(p) = MROP_MASK(srcpix,*MAPR(p),endmask);
		    p += nlwExtra; CHECKRWO(p);
		}
	    }
	    else /* no ragged bits at either end */
	    {
		while (h--)
		{
		    srcpix = psrc[iy];
		    iy++;
		    if (iy == tileHeight)
			iy = 0;
		    nlw = nlwMiddle;
		    while (nlw--)
		    {
			*MAPW(p) = MROP_SOLID (srcpix,*MAPR(p));
			p++; CHECKRWO(p);
		    }
		    p += nlwExtra; CHECKRWO(p);
		}
	    }
	}
        pbox++;
    }
}

#if (MROP) == 0
void
mfbTileArea32 (pDraw, nbox, pbox, alu, ptile)
    DrawablePtr pDraw;
    int nbox;
    BoxPtr pbox;
    int alu;
    PixmapPtr ptile;
{
    void    (*f)(), mfbTileArea32Copy(), mfbTileArea32General();
    
    if (alu == GXcopy)
	f = mfbTileArea32Copy;
    else
	f = mfbTileArea32General;
    (*f) (pDraw, nbox, pbox, alu, ptile);
}
#endif
