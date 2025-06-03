/************************************************************
Copyright 1987 by Sun Microsystems, Inc. Mountain View, CA.

                    All Rights Reserved

Permission  to  use,  copy,  modify,  and  distribute   this
software  and  its documentation for any purpose and without
fee is hereby granted, provided that the above copyright no-
tice  appear  in all copies and that both that copyright no-
tice and this permission notice appear in  supporting  docu-
mentation,  and  that the names of Sun or MIT not be used in
advertising or publicity pertaining to distribution  of  the
software  without specific prior written permission. Sun and
M.I.T. make no representations about the suitability of this
software for any purpose. It is provided "as is" without any
express or implied warranty.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

********************************************************/

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

Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)

KEVIN E. MARTIN AND TIAGO GONS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL KEVIN E. MARTIN OR TIAGO GONS BE LIABLE FOR ANY SPECIAL,
INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Modified by Tiago Gons (tiago@comosjn.hobby.nl)

******************************************************************/

/* $XFree86: mit/server/ddx/x386/accel/ibm8514/ibm8514fs.c,v 2.3 1993/11/24 12:33:10 dawes Exp $ */

#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include "cfb.h"

#include "reg8514.h"
#include "ibm8514.h"

void
ibm8514SolidFSpans (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GCPtr       pGC;
    int         nInit;                  /* number of spans to fill */
    DDXPointPtr pptInit;                /* pointer to list of start points */
    int         *pwidthInit;            /* pointer to list of n widths */
    int         fSorted;
{
    int n;                      /* number of spans to fill */
    register DDXPointPtr ppt;   /* pointer to list of start points */
    register int *pwidth;       /* pointer to list of n widths */

/* 4-5-93 TCG : is VT visible */
    if (!x386VTSema)
    {
        cfbSolidSpansGeneral(pDrawable, pGC, 
			     nInit, pptInit, pwidthInit, fSorted);
	return;
    }

    if (pDrawable->type != DRAWABLE_WINDOW) {
        switch (pDrawable->depth) {
            case 1:
		ErrorF("should call mfbSolidFillSpans\n");
		return;
                break;
            case 8:
		ErrorF("should call cfbSolidFillSpans\n");
		return;
                break;
            default:
                ErrorF("Unsupported pixmap depth\n");
                return;
                break;
        }
    }

    if (!(pGC->planemask))
        return;

    n = nInit * miFindMaxBand(((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);
    pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
        if (ppt) DEALLOCATE_LOCAL(ppt);
        if (pwidth) DEALLOCATE_LOCAL(pwidth);
        return;
    }
    n = miClipSpans(((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip,
                     pptInit, pwidthInit, nInit,
                     ppt, pwidth, fSorted);

    WaitQueue(3);
    outw(FRGD_COLOR, (short)(pGC->fgPixel));
    outw(FRGD_MIX, FSS_FRGDCOL | ibm8514alu[pGC->alu]);
    outw(WRT_MASK, (short)pGC->planemask);

    while (n--) {
	WaitQueue(5);
	outw(CUR_X, (short)(ppt->x));
	outw(CUR_Y, (short)(ppt->y));
	outw(MAJ_AXIS_PCNT, ((short)*pwidth)-1);
	outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | 0);
	outw(CMD, CMD_RECT | INC_Y | INC_X | DRAW | PLANAR | WRTDATA);

	ppt++;
	pwidth++;
    }

    WaitQueue(2);
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
    outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);

    DEALLOCATE_LOCAL(ppt);
    DEALLOCATE_LOCAL(pwidth);
}

void
ibm8514TiledFSpans (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GCPtr       pGC;
    int         nInit;                  /* number of spans to fill */
    DDXPointPtr pptInit;                /* pointer to list of start points */
    int         *pwidthInit;            /* pointer to list of n widths */
    int         fSorted;
{
    int n;                      /* number of spans to fill */
    register DDXPointPtr ppt;   /* pointer to list of start points */
    register int *pwidth;       /* pointer to list of n widths */
    int xrot, yrot, width, height, pixWidth;
    PixmapPtr pPix = pGC->tile.pixmap;

/* 4-5-93 TCG : is VT visible */
    if (!x386VTSema)
    {
        cfbUnnaturalTileFS(pDrawable, pGC, 
			   nInit, pptInit, pwidthInit, fSorted);
	return;
    }

    if (pDrawable->type != DRAWABLE_WINDOW) {
        switch (pDrawable->depth) {
            case 1:
                ErrorF("should call mfbTiledFillSpans\n");
		return;
                break;
            case 8:
                ErrorF("should call cfbTiledFillSpans\n");
		return;
                break;
            default:
                ErrorF("Unsupported pixmap depth\n");
                return;
                break;
        }
    }

    if (!(pGC->planemask))
        return;

    n = nInit * miFindMaxBand(((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);
    pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
        if (ppt) DEALLOCATE_LOCAL(ppt);
        if (pwidth) DEALLOCATE_LOCAL(pwidth);
        return;
    }
    n = miClipSpans(((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip,
                     pptInit, pwidthInit, nInit,
                     ppt, pwidth, fSorted);

    xrot = pDrawable->x + pGC->patOrg.x;
    yrot = pDrawable->y + pGC->patOrg.y;

    if (pPix == (PixmapPtr)0) {
	ErrorF("ibm8514TiledFSpans:  PixmapPtr tile.pixmap == NULL\n");
	return;
    }

    width = pPix->drawable.width;
    height = pPix->drawable.height;
    pixWidth = PixmapBytePad(width, pPix->drawable.depth);

    if (ibm8514CacheTile(pPix)) {
	while (n--) {
	    ibm8514CImageFill(pPix->slot,
			      ppt->x, ppt->y, *pwidth, 1, xrot, yrot,
			      ibm8514alu[pGC->alu], pGC->planemask);
	    ppt++;
	    pwidth++;
    	}
    } else {
	while (n--) {
	    ibm8514ImageFill(ppt->x, ppt->y, *pwidth, 1,
			     pPix->devPrivate.ptr, pixWidth,
			     width, height, xrot, yrot,
			     ibm8514alu[pGC->alu], pGC->planemask);
	    ppt++;
	    pwidth++;
    	}
    }

    DEALLOCATE_LOCAL(ppt);
    DEALLOCATE_LOCAL(pwidth);
}

void
ibm8514StipFSpans (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GCPtr       pGC;
    int         nInit;                  /* number of spans to fill */
    DDXPointPtr pptInit;                /* pointer to list of start points */
    int         *pwidthInit;            /* pointer to list of n widths */
    int         fSorted;
{
    int n;                      /* number of spans to fill */
    register DDXPointPtr ppt;   /* pointer to list of start points */
    register int *pwidth;       /* pointer to list of n widths */
    int xrot, yrot, width, height, pixWidth;
    PixmapPtr pPix = pGC->stipple;

/* 4-5-93 TCG : is VT visible */
    if (!x386VTSema)
    {
        cfbUnnaturalStippleFS(pDrawable, pGC, 
			      nInit, pptInit, pwidthInit, fSorted);
	return;
    }

    if (pDrawable->type != DRAWABLE_WINDOW) {
        switch (pDrawable->depth) {
            case 1:
                ErrorF("should call mfbStippleFillSpans\n");
		return;
                break;
            case 8:
                ErrorF("should call cfbStippleFillSpans\n");
		return;
                break;
            default:
                ErrorF("Unsupported pixmap depth\n");
                return;
                break;
        }
    }

    if (!(pGC->planemask))
        return;

    n = nInit * miFindMaxBand(((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);
    pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
        if (ppt) DEALLOCATE_LOCAL(ppt);
        if (pwidth) DEALLOCATE_LOCAL(pwidth);
        return;
    }
    n = miClipSpans(((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip,
                     pptInit, pwidthInit, nInit,
                     ppt, pwidth, fSorted);

    xrot = pDrawable->x + pGC->patOrg.x;
    yrot = pDrawable->y + pGC->patOrg.y;

    if (pPix == (PixmapPtr)0) {
	ErrorF("ibm8514StipFSpans:  PixmapPtr stipple == NULL\n");
	return;
    }

    width = pPix->drawable.width;
    height = pPix->drawable.height;
    pixWidth = PixmapBytePad(width, pPix->drawable.depth);

    if (ibm8514CacheStipple(pPix)) {
	while (n--) {
	    ibm8514CImageStipple(pPix->slot,
				   ppt->x, ppt->y, *pwidth, 1, xrot, yrot,
				   pGC->fgPixel,
				   ibm8514alu[pGC->alu], pGC->planemask);
	    ppt++;
	    pwidth++;
    	}
    } else {
	while (n--) {
	    ibm8514ImageStipple(ppt->x, ppt->y, *pwidth, 1,
				pPix->devPrivate.ptr, pixWidth, width, height,
				xrot, yrot, pGC->fgPixel, 0,
				ibm8514alu[pGC->alu], pGC->planemask, 0);
	    ppt++;
	    pwidth++;
    	}
    }

    DEALLOCATE_LOCAL(ppt);
    DEALLOCATE_LOCAL(pwidth);
}

void
ibm8514OStipFSpans (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GCPtr       pGC;
    int         nInit;                  /* number of spans to fill */
    DDXPointPtr pptInit;                /* pointer to list of start points */
    int         *pwidthInit;            /* pointer to list of n widths */
    int         fSorted;
{
    int n;                      /* number of spans to fill */
    register DDXPointPtr ppt;   /* pointer to list of start points */
    register int *pwidth;       /* pointer to list of n widths */
    int xrot, yrot, width, height, pixWidth;
    PixmapPtr pPix = pGC->stipple;

/* 4-5-93 TCG : is VT visible */
    if (!x386VTSema)
    {
        cfbUnnaturalStippleFS(pDrawable, pGC, 
			      nInit, pptInit, pwidthInit, fSorted);
	return;
    }

    if (pDrawable->type != DRAWABLE_WINDOW) {
        switch (pDrawable->depth) {
            case 1:
                ErrorF("should call mfbOpStippleFillSpans\n");
		return;
                break;
            case 8:
                ErrorF("should call cfbOpStippleFillSpans\n");
		return;
                break;
            default:
                ErrorF("Unsupported pixmap depth\n");
                return;
                break;
        }
    }

    if (!(pGC->planemask))
        return;

    n = nInit * miFindMaxBand(((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);
    pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
        if (ppt) DEALLOCATE_LOCAL(ppt);
        if (pwidth) DEALLOCATE_LOCAL(pwidth);
        return;
    }
    n = miClipSpans(((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip,
                     pptInit, pwidthInit, nInit,
                     ppt, pwidth, fSorted);

    xrot = pDrawable->x + pGC->patOrg.x;
    yrot = pDrawable->y + pGC->patOrg.y;

    if (pPix == (PixmapPtr)0) {
	ErrorF("ibm8514StipFSpans:  PixmapPtr stipple == NULL\n");
	return;
    }

    width = pPix->drawable.width;
    height = pPix->drawable.height;
    pixWidth = PixmapBytePad(width, pPix->drawable.depth);

    if (ibm8514CacheOpStipple(pPix)) {
	while (n--) {
	    ibm8514CImageOpStipple(pPix->slot,
				   ppt->x, ppt->y, *pwidth, 1, xrot, yrot,
				   pGC->fgPixel, pGC->bgPixel,
				   ibm8514alu[pGC->alu], pGC->planemask);
	    ppt++;
	    pwidth++;
    	}
    } else {
	while (n--) {
	    ibm8514ImageStipple(ppt->x, ppt->y, *pwidth, 1,
				pPix->devPrivate.ptr, pixWidth,
				width, height,
				xrot, yrot, pGC->fgPixel, pGC->bgPixel,
				ibm8514alu[pGC->alu], pGC->planemask, 1);
	    ppt++;
	    pwidth++;
    	}
    }

    DEALLOCATE_LOCAL(ppt);
    DEALLOCATE_LOCAL(pwidth);
}
