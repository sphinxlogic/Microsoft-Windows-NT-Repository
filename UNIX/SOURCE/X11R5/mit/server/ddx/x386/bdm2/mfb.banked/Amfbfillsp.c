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
/* $XFree86: mit/server/ddx/x386/bdm2/mfb.banked/Amfbfillsp.c,v 2.0 1993/08/30 15:22:50 dawes Exp $ */
/* $XConsortium: mfbfillsp.c,v 5.7 90/05/15 18:38:15 keith Exp $ */
#define BANKING_MODS
#include "X.h"
#include "Xmd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "mfb.h"
#include "maskbits.h"

#include "mergerop.h"

#include "servermd.h"

#if (BITMAP_SCANLINE_UNIT < 32)
#undef BITMAP_SCANLINE_UNIT
#define BITMAP_SCANLINE_UNIT 32
#endif

/* scanline filling for monochrome frame buffer
   written by drewry, oct 1986

   these routines all clip.  they assume that anything that has called
them has already translated the points (i.e. pGC->miTranslate is
non-zero, which is howit gets set in mfbCreateGC().)

   the number of new scnalines created by clipping ==
MaxRectsPerBand * nSpans.

    FillSolid is overloaded to be used for OpaqueStipple as well,
if fgPixel == bgPixel.  


    FillTiled is overloaded to be used for OpaqueStipple, if
fgPixel != bgPixel.  based on the fill style, it uses
{RotatedPixmap, gc.alu} or {RotatedPixmap, PrivGC.ropOpStip}
*/


void mfbBlackSolidFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int		nInit;			/* number of spans to fill */
    DDXPointPtr pptInit;		/* pointer to list of start points */
    int		*pwidthInit;		/* pointer to list of n widths */
    int 	fSorted;
{
				/* next three parameters are post-clip */
    int n;			/* number of spans to fill */
    register DDXPointPtr ppt;	/* pointer to list of start points */
    register int *pwidth;	/* pointer to list of n widths */
    int *addrlBase;		/* pointer to start of bitmap */
    int nlwidth;		/* width in longwords of bitmap */
    register int *addrl;	/* pointer to current longword in bitmap */
    register int nlmiddle;
    register int startmask;
    register int endmask;
    int *pwidthFree;		/* copies of the pointers to free */
    DDXPointPtr pptFree;

    if (!(pGC->planemask & 1))
	return;

    n = nInit * miFindMaxBand(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip);
    pwidthFree = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    pptFree = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!pptFree || !pwidthFree)
    {
	if (pptFree) DEALLOCATE_LOCAL(pptFree);
	if (pwidthFree) DEALLOCATE_LOCAL(pwidthFree);
	return;
    }
    pwidth = pwidthFree;
    ppt = pptFree;
    n = miClipSpans(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip,
		    pptInit, pwidthInit, nInit,
		    ppt, pwidth, fSorted);

    if (pDrawable->type == DRAWABLE_WINDOW)
    {
	addrlBase = (int *)
		(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devPrivate.ptr);
	nlwidth = (int)
		  (((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind) >> 2;
    }
    else
    {
	addrlBase = (int *)(((PixmapPtr)pDrawable)->devPrivate.ptr);
	nlwidth = (int)(((PixmapPtr)pDrawable)->devKind) >> 2;
    }

    BANK_FLAG(addrlBase)

    while (n--)
    {
        addrl = addrlBase + (ppt->y * nlwidth) + (ppt->x >> 5);
        SETRW(addrl);

	if (*pwidth)
	{
	    if ( ((ppt->x & 0x1f) + *pwidth) < 32)
	    {
		/* all bits inside same longword */
		maskpartialbits(ppt->x, *pwidth, startmask);
		    *MAPRW(addrl) &= ~startmask;
	    }
	    else
	    {
		maskbits(ppt->x, *pwidth, startmask, endmask, nlmiddle);
		if (startmask) {
		    *MAPRW(addrl) &= ~startmask; addrl++;
#ifndef LINE_BANK_ALIGNED
			CHECKRWO(addrl);
#endif
		  }
#ifdef LINE_BANK_ALIGNED
		Duff (nlmiddle, *MAPW(addrl) = 0x0; addrl++;);
#else /* LINE_BANK_ALIGNED */
		Duff (nlmiddle, *MAPW(addrl) = 0x0; addrl++; CHECKRWO(addrl));
#endif /* LINE_BANK_ALIGNED */
		if (endmask)
		    *MAPRW(addrl) &= ~endmask;
	    }
	}
	pwidth++;
	ppt++;
    }
    DEALLOCATE_LOCAL(pptFree);
    DEALLOCATE_LOCAL(pwidthFree);
}



void mfbWhiteSolidFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int		nInit;			/* number of spans to fill */
    DDXPointPtr pptInit;		/* pointer to list of start points */
    int		*pwidthInit;		/* pointer to list of n widths */
    int 	fSorted;
{
				/* next three parameters are post-clip */
    int n;			/* number of spans to fill */
    register DDXPointPtr ppt;	/* pointer to list of start points */
    register int *pwidth;	/* pointer to list of n widths */
    int *addrlBase;		/* pointer to start of bitmap */
    int nlwidth;		/* width in longwords of bitmap */
    register int *addrl;	/* pointer to current longword in bitmap */
    register int nlmiddle;
    register int startmask;
    register int endmask;
    int *pwidthFree;		/* copies of the pointers to free */
    DDXPointPtr pptFree;

    if (!(pGC->planemask & 1))
	return;

    n = nInit * miFindMaxBand(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip);
    pwidthFree = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    pptFree = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!pptFree || !pwidthFree)
    {
	if (pptFree) DEALLOCATE_LOCAL(pptFree);
	if (pwidthFree) DEALLOCATE_LOCAL(pwidthFree);
	return;
    }
    pwidth = pwidthFree;
    ppt = pptFree;
    n = miClipSpans(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip,
		    pptInit, pwidthInit, nInit,
		    ppt, pwidth, fSorted);

    if (pDrawable->type == DRAWABLE_WINDOW)
    {
	addrlBase = (int *)
		(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devPrivate.ptr);
	nlwidth = (int)
		 (((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind) >> 2;
    }
    else
    {
	addrlBase = (int *)(((PixmapPtr)pDrawable)->devPrivate.ptr);
	nlwidth = (int)(((PixmapPtr)pDrawable)->devKind) >> 2;
    }

    BANK_FLAG(addrlBase)

    while (n--)
    {
        addrl = addrlBase + (ppt->y * nlwidth) + (ppt->x >> 5);
        SETRW(addrl);

	if (*pwidth)
	{
	    if ( ((ppt->x & 0x1f) + *pwidth) < 32)
	    {
		/* all bits inside same longword */
		maskpartialbits(ppt->x, *pwidth, startmask);
		*MAPRW(addrl) |= startmask;
	    }
	    else
	    {
		maskbits(ppt->x, *pwidth, startmask, endmask, nlmiddle);
		if (startmask) {
		    *MAPRW(addrl) |= startmask; addrl++;
#ifndef LINE_BANK_ALIGNED
			CHECKRWO(addrl);
#endif
		  }
#ifdef LINE_BANK_ALIGNED
		Duff (nlmiddle,
                      *MAPW(addrl) = 0xffffffff; addrl++;);
#else /* LINE_BANK_ALIGNED */
		Duff (nlmiddle,
                      *MAPW(addrl) = 0xffffffff; addrl++;CHECKRWO(addrl));
#endif /* LINE_BANK_ALIGNED */
		if (endmask)
		    *MAPRW(addrl) |= endmask;
	    }
	}
	pwidth++;
	ppt++;
    }
    DEALLOCATE_LOCAL(pptFree);
    DEALLOCATE_LOCAL(pwidthFree);
}



void mfbInvertSolidFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int		nInit;			/* number of spans to fill */
    DDXPointPtr pptInit;		/* pointer to list of start points */
    int		*pwidthInit;		/* pointer to list of n widths */
    int 	fSorted;
{
				/* next three parameters are post-clip */
    int n;			/* number of spans to fill */
    register DDXPointPtr ppt;	/* pointer to list of start points */
    register int *pwidth;	/* pointer to list of n widths */
    int *addrlBase;		/* pointer to start of bitmap */
    int nlwidth;		/* width in longwords of bitmap */
    register int *addrl;	/* pointer to current longword in bitmap */
    register int nlmiddle;
    register int startmask;
    register int endmask;
    int *pwidthFree;		/* copies of the pointers to free */
    DDXPointPtr pptFree;

    if (!(pGC->planemask & 1))
	return;

    n = nInit * miFindMaxBand(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip);
    pwidthFree = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    pptFree = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!pptFree || !pwidthFree)
    {
	if (pptFree) DEALLOCATE_LOCAL(pptFree);
	if (pwidthFree) DEALLOCATE_LOCAL(pwidthFree);
	return;
    }
    pwidth = pwidthFree;
    ppt = pptFree;
    n = miClipSpans(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip,
		    pptInit, pwidthInit, nInit,
		    ppt, pwidth, fSorted);

    if (pDrawable->type == DRAWABLE_WINDOW)
    {
	addrlBase = (int *)
		(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devPrivate.ptr);
	nlwidth = (int)
		  (((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind) >> 2;
    }
    else
    {
	addrlBase = (int *)(((PixmapPtr)pDrawable)->devPrivate.ptr);
	nlwidth = (int)(((PixmapPtr)pDrawable)->devKind) >> 2;
    }

    BANK_FLAG(addrlBase)

    while (n--)
    {
        addrl = addrlBase + (ppt->y * nlwidth) + (ppt->x >> 5);
        SETRW(addrl);

	if (*pwidth)
	{
	    if ( ((ppt->x & 0x1f) + *pwidth) < 32)
	    {
		/* all bits inside same longword */
		maskpartialbits(ppt->x, *pwidth, startmask);
		*MAPRW(addrl) ^= startmask;
	    }
	    else
	    {
		maskbits(ppt->x, *pwidth, startmask, endmask, nlmiddle);
		if (startmask) {
		    *MAPRW(addrl) ^= startmask; addrl++;
#ifndef LINE_BANK_ALIGNED
		    CHECKRWO(addrl);
#endif
		}
#ifdef LINE_BANK_ALIGNED
		Duff (nlmiddle,
                      *MAPRW(addrl) ^= 0xffffffff; addrl++;);
#else
		Duff (nlmiddle,
                      *MAPRW(addrl) ^= 0xffffffff; addrl++; CHECKRWO(addrl));
#endif
		if (endmask)
		    *MAPRW(addrl) ^= endmask;
	    }
	}
	pwidth++;
	ppt++;
    }
    DEALLOCATE_LOCAL(pptFree);
    DEALLOCATE_LOCAL(pwidthFree);
}


void 
mfbWhiteStippleFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
DrawablePtr pDrawable;
GC *pGC;
int nInit;			/* number of spans to fill */
DDXPointPtr pptInit;		/* pointer to list of start points */
int *pwidthInit;		/* pointer to list of n widths */
int fSorted;
{
				/* next three parameters are post-clip */
    int n;			/* number of spans to fill */
    register DDXPointPtr ppt;	/* pointer to list of start points */
    register int *pwidth;	/* pointer to list of n widths */
    int *addrlBase;		/* pointer to start of bitmap */
    int nlwidth;		/* width in longwords of bitmap */
    register int *addrl;	/* pointer to current longword in bitmap */
    register int src;
    register int nlmiddle;
    register int startmask;
    register int endmask;
    PixmapPtr pStipple;
    int *psrc;
    int tileHeight;
    int *pwidthFree;		/* copies of the pointers to free */
    DDXPointPtr pptFree;

    if (!(pGC->planemask & 1))
	return;

    n = nInit * miFindMaxBand(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip);
    pwidthFree = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    pptFree = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!pptFree || !pwidthFree)
    {
	if (pptFree) DEALLOCATE_LOCAL(pptFree);
	if (pwidthFree) DEALLOCATE_LOCAL(pwidthFree);
	return;
    }
    pwidth = pwidthFree;
    ppt = pptFree;
    n = miClipSpans(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip,
		    pptInit, pwidthInit, nInit, 
		    ppt, pwidth, fSorted);

    if (pDrawable->type == DRAWABLE_WINDOW)
    {
	addrlBase = (int *)
		(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devPrivate.ptr);
	nlwidth = (int)
		  (((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind) >> 2;
    }
    else
    {
	addrlBase = (int *)(((PixmapPtr)pDrawable)->devPrivate.ptr);
	nlwidth = (int)(((PixmapPtr)pDrawable)->devKind) >> 2;
    }

    BANK_FLAG(addrlBase)

    pStipple = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pRotatedPixmap;
    tileHeight = pStipple->drawable.height;
    psrc = (int *)(pStipple->devPrivate.ptr);

    while (n--)
    {
        addrl = addrlBase + (ppt->y * nlwidth) + (ppt->x >> 5);
        SETRW(addrl);
	src = psrc[ppt->y % tileHeight];

        /* all bits inside same longword */
        if ( ((ppt->x & 0x1f) + *pwidth) < 32)
        {
	    maskpartialbits(ppt->x, *pwidth, startmask);
	    *MAPRW(addrl) |= (src & startmask);
        }
        else
        {
	    maskbits(ppt->x, *pwidth, startmask, endmask, nlmiddle);
	    if (startmask) {
		*MAPRW(addrl) |= (src & startmask); addrl++;
#ifndef LINE_BANK_ALIGNED
		CHECKRWO(addrl);
#endif
	      }
#ifdef LINE_BANK_ALIGNED
	    Duff (nlmiddle, *MAPRW(addrl) |= src; addrl++;);
#else
	    Duff (nlmiddle, *MAPRW(addrl) |= src; addrl++; CHECKRWO(addrl));
#endif
	    if (endmask)
		*MAPRW(addrl) |= (src & endmask);
        }
	pwidth++;
	ppt++;
    }
    DEALLOCATE_LOCAL(pptFree);
    DEALLOCATE_LOCAL(pwidthFree);
}


void 
mfbBlackStippleFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
DrawablePtr pDrawable;
GC *pGC;
int nInit;			/* number of spans to fill */
DDXPointPtr pptInit;		/* pointer to list of start points */
int *pwidthInit;		/* pointer to list of n widths */
int fSorted;
{
				/* next three parameters are post-clip */
    int n;			/* number of spans to fill */
    register DDXPointPtr ppt;	/* pointer to list of start points */
    register int *pwidth;	/* pointer to list of n widths */
    int *addrlBase;		/* pointer to start of bitmap */
    int nlwidth;		/* width in longwords of bitmap */
    register int *addrl;	/* pointer to current longword in bitmap */
    register int src;
    register int nlmiddle;
    register int startmask;
    register int endmask;
    PixmapPtr pStipple;
    int *psrc;
    int tileHeight;
    int *pwidthFree;		/* copies of the pointers to free */
    DDXPointPtr pptFree;

    if (!(pGC->planemask & 1))
	return;

    n = nInit * miFindMaxBand(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip);
    pwidthFree = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    pptFree = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!pptFree || !pwidthFree)
    {
	if (pptFree) DEALLOCATE_LOCAL(pptFree);
	if (pwidthFree) DEALLOCATE_LOCAL(pwidthFree);
	return;
    }
    pwidth = pwidthFree;
    ppt = pptFree;
    n = miClipSpans(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip,
		    pptInit, pwidthInit, nInit, 
		    ppt, pwidth, fSorted);

    if (pDrawable->type == DRAWABLE_WINDOW)
    {
	addrlBase = (int *)
		(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devPrivate.ptr);
	nlwidth = (int)
		  (((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind) >> 2;
    }
    else
    {
	addrlBase = (int *)(((PixmapPtr)pDrawable)->devPrivate.ptr);
	nlwidth = (int)(((PixmapPtr)pDrawable)->devKind) >> 2;
    }

    BANK_FLAG(addrlBase)

    pStipple = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pRotatedPixmap;
    tileHeight = pStipple->drawable.height;
    psrc = (int *)(pStipple->devPrivate.ptr);

    while (n--)
    {
        addrl = addrlBase + (ppt->y * nlwidth) + (ppt->x >> 5);
        SETRW(addrl);
	src = psrc[ppt->y % tileHeight];

        /* all bits inside same longword */
        if ( ((ppt->x & 0x1f) + *pwidth) < 32)
        {
	    maskpartialbits(ppt->x, *pwidth, startmask);
	    *MAPRW(addrl) &= ~(src & startmask);
        }
        else
        {
	    maskbits(ppt->x, *pwidth, startmask, endmask, nlmiddle);
	    if (startmask) {
		*MAPRW(addrl) &= ~(src & startmask); addrl++;
#ifndef LINE_BANK_ALIGNED
		CHECKRWO(addrl);
#endif
	      }
#ifdef LINE_BANK_ALIGNED
	    Duff (nlmiddle, *MAPRW(addrl) &= ~src; addrl++;);
#else
	    Duff (nlmiddle, *MAPRW(addrl) &= ~src; addrl++; CHECKRWO(addrl));
#endif
	    if (endmask)
		*MAPRW(addrl) &= ~(src & endmask);
        }
	pwidth++;
	ppt++;
    }
    DEALLOCATE_LOCAL(pptFree);
    DEALLOCATE_LOCAL(pwidthFree);
}


void 
mfbInvertStippleFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
DrawablePtr pDrawable;
GC *pGC;
int nInit;			/* number of spans to fill */
DDXPointPtr pptInit;		/* pointer to list of start points */
int *pwidthInit;		/* pointer to list of n widths */
int fSorted;
{
				/* next three parameters are post-clip */
    int n;			/* number of spans to fill */
    register DDXPointPtr ppt;	/* pointer to list of start points */
    register int *pwidth;	/* pointer to list of n widths */
    int *addrlBase;		/* pointer to start of bitmap */
    int nlwidth;		/* width in longwords of bitmap */
    register int *addrl;	/* pointer to current longword in bitmap */
    register int src;
    register int nlmiddle;
    register int startmask;
    register int endmask;
    PixmapPtr pStipple;
    int *psrc;
    int tileHeight;
    int *pwidthFree;		/* copies of the pointers to free */
    DDXPointPtr pptFree;

    if (!(pGC->planemask & 1))
	return;

    n = nInit * miFindMaxBand(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip);
    pwidthFree = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    pptFree = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!pptFree || !pwidthFree)
    {
	if (pptFree) DEALLOCATE_LOCAL(pptFree);
	if (pwidthFree) DEALLOCATE_LOCAL(pwidthFree);
	return;
    }
    pwidth = pwidthFree;
    ppt = pptFree;
    n = miClipSpans(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip,
		    pptInit, pwidthInit, nInit, 
		    ppt, pwidth, fSorted);

    if (pDrawable->type == DRAWABLE_WINDOW)
    {
	addrlBase = (int *)
		(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devPrivate.ptr);
	nlwidth = (int)
		  (((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind) >> 2;
    }
    else
    {
	addrlBase = (int *)(((PixmapPtr)pDrawable)->devPrivate.ptr);
	nlwidth = (int)(((PixmapPtr)pDrawable)->devKind) >> 2;
    }

    BANK_FLAG(addrlBase)

    pStipple = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pRotatedPixmap;
    tileHeight = pStipple->drawable.height;
    psrc = (int *)(pStipple->devPrivate.ptr);

    while (n--)
    {
        addrl = addrlBase + (ppt->y * nlwidth) + (ppt->x >> 5);
        SETRW(addrl);
	src = psrc[ppt->y % tileHeight];

        /* all bits inside same longword */
        if ( ((ppt->x & 0x1f) + *pwidth) < 32)
        {
	    maskpartialbits(ppt->x, *pwidth, startmask);
	    *MAPRW(addrl) ^= (src & startmask);
        }
        else
        {
	    maskbits(ppt->x, *pwidth, startmask, endmask, nlmiddle);
	    if (startmask) {
		*MAPRW(addrl) ^= (src & startmask); addrl++;
#ifndef LINE_BANK_ALIGNED
		CHECKRWO(addrl);
#endif
	      }
#ifdef LINE_BANK_ALIGNED
	    Duff(nlmiddle, *MAPRW(addrl) ^= src; addrl++;);
#else
	    Duff(nlmiddle, *MAPRW(addrl) ^= src; addrl++; CHECKRWO(addrl));
#endif
	    if (endmask)
		*MAPRW(addrl) ^= (src & endmask);
        }
	pwidth++;
	ppt++;
    }
    DEALLOCATE_LOCAL(pptFree);
    DEALLOCATE_LOCAL(pwidthFree);
}


/* this works with tiles of width == 32 */
#ifdef LINE_BANK_ALIGNED
#define FILLSPAN32(ROP) \
    while (n--) \
    { \
	if (*pwidth) \
	{ \
            addrl = addrlBase + (ppt->y * nlwidth) + (ppt->x >> 5); \
            SETRW(addrl); \
	    src = psrc[ppt->y % tileHeight]; \
            if ( ((ppt->x & 0x1f) + *pwidth) < 32) \
            { \
	        maskpartialbits(ppt->x, *pwidth, startmask); \
	        *MAPW(addrl) = (*MAPR(addrl) & ~startmask) | \
		         (ROP(src, *MAPR(addrl)) & startmask); \
            } \
            else \
            { \
	        maskbits(ppt->x, *pwidth, startmask, endmask, nlmiddle); \
	        if (startmask) \
	        { \
	            *MAPW(addrl) = (*MAPR(addrl) & ~startmask) | \
			     (ROP(src, *MAPR(addrl)) & startmask); \
		    addrl++; \
	        } \
	        while (nlmiddle--) \
	        { \
		    *MAPW(addrl) = ROP(src, *MAPR(addrl)); \
		    addrl++; \
	        } \
	        if (endmask) \
	            *MAPW(addrl) = (*MAPR(addrl) & ~endmask) | \
			     (ROP(src, *MAPR(addrl)) & endmask); \
            } \
	} \
	pwidth++; \
	ppt++; \
    }
#else /* LINE_BANK_ALIGNED */
#define FILLSPAN32(ROP) \
    while (n--) \
    { \
	if (*pwidth) \
	{ \
            addrl = addrlBase + (ppt->y * nlwidth) + (ppt->x >> 5); \
            SETRW(addrl); \
	    src = psrc[ppt->y % tileHeight]; \
            if ( ((ppt->x & 0x1f) + *pwidth) < 32) \
            { \
	        maskpartialbits(ppt->x, *pwidth, startmask); \
	        *MAPW(addrl) = (*MAPR(addrl) & ~startmask) | \
		         (ROP(src, *MAPR(addrl)) & startmask); \
            } \
            else \
            { \
	        maskbits(ppt->x, *pwidth, startmask, endmask, nlmiddle); \
	        if (startmask) \
	        { \
	            *MAPW(addrl) = (*MAPR(addrl) & ~startmask) | \
			     (ROP(src, *MAPR(addrl)) & startmask); \
		    addrl++; CHECKRWO(addrl); \
	        } \
	        while (nlmiddle--) \
	        { \
		    *MAPW(addrl) = ROP(src, *MAPR(addrl)); \
		    addrl++; CHECKRWO(addrl); \
	        } \
	        if (endmask) \
	            *MAPW(addrl) = (*MAPR(addrl) & ~endmask) | \
			     (ROP(src, *MAPR(addrl)) & endmask); \
            } \
	} \
	pwidth++; \
	ppt++; \
    }
#endif /* LINE_BANK_ALIGNED */


void mfbTileFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
DrawablePtr pDrawable;
GC *pGC;
int nInit;			/* number of spans to fill */
DDXPointPtr pptInit;		/* pointer to list of start points */
int *pwidthInit;		/* pointer to list of n widths */
int fSorted;
{
				/* next three parameters are post-clip */
    int n;			/* number of spans to fill */
    register DDXPointPtr ppt;	/* pointer to list of start points */
    register int *pwidth;	/* pointer to list of n widths */
    int *addrlBase;		/* pointer to start of bitmap */
    int nlwidth;		/* width in longwords of bitmap */
    register int *addrl;	/* pointer to current longword in bitmap */
    register int src;
    register int nlmiddle;
    register int startmask;
    register int endmask;
    PixmapPtr pTile;
    int *psrc;
    int tileHeight;
    int rop;
    int *pwidthFree;		/* copies of the pointers to free */
    DDXPointPtr pptFree;
    unsigned long   flip;


    if (!(pGC->planemask & 1))
	return;

    n = nInit * miFindMaxBand(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip);
    pwidthFree = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    pptFree = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!pptFree || !pwidthFree)
    {
	if (pptFree) DEALLOCATE_LOCAL(pptFree);
	if (pwidthFree) DEALLOCATE_LOCAL(pwidthFree);
	return;
    }
    pwidth = pwidthFree;
    ppt = pptFree;
    n = miClipSpans(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip,
		    pptInit, pwidthInit, nInit, 
		    ppt, pwidth, fSorted);

    if (pDrawable->type == DRAWABLE_WINDOW)
    {
	addrlBase = (int *)
		(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devPrivate.ptr);
	nlwidth = (int)
		  (((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind) >> 2;
    }
    else
    {
	addrlBase = (int *)(((PixmapPtr)pDrawable)->devPrivate.ptr);
	nlwidth = (int)(((PixmapPtr)pDrawable)->devKind) >> 2;
    }

    BANK_FLAG(addrlBase)

    pTile = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pRotatedPixmap;
    tileHeight = pTile->drawable.height;
    psrc = (int *)(pTile->devPrivate.ptr);
    if (pGC->fillStyle == FillTiled)
	rop = pGC->alu;
    else
	rop = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->ropOpStip;

    flip = 0;
    switch(rop)
    {
      case GXcopyInverted:  /* for opaque stipples */
	flip = ~0;
      case GXcopy:
	{

#define DoMaskCopyRop(src,dst,mask)	((dst) & ~(mask) | (src) & (mask))

	    while (n--)
	    {
	    	if (*pwidth)
	    	{
            	    addrl = addrlBase + (ppt->y * nlwidth) + (ppt->x >> 5);
                    SETRW(addrl);
	    	    src = psrc[ppt->y % tileHeight] ^ flip;
            	    if ( ((ppt->x & 0x1f) + *pwidth) < 32)
            	    {
	            	maskpartialbits(ppt->x, *pwidth, startmask);
			*MAPW(addrl) = DoMaskCopyRop (src, *MAPR(addrl), startmask);
            	    }
            	    else
            	    {
	            	maskbits(ppt->x, *pwidth, startmask, endmask, nlmiddle);
	            	if (startmask)
	            	{
			    *MAPW(addrl) = DoMaskCopyRop (src, *MAPR(addrl), startmask);
		    	    addrl++;
#ifndef LINE_BANK_ALIGNED
				CHECKRWO(addrl);
#endif
	            	}
	            	while (nlmiddle--)
	            	{
			    *MAPW(addrl) = src;
		    	    addrl++;
#ifndef LINE_BANK_ALIGNED
				CHECKRWO(addrl);
#endif
	            	}
	            	if (endmask)
			    *MAPW(addrl) = DoMaskCopyRop (src, *MAPR(addrl), endmask);
            	    }
	    	}
	    	pwidth++;
	    	ppt++;
	    }
	}
	break;
      default:
	{
	    register DeclareMergeRop ();

	    InitializeMergeRop(rop,~0);
	    while (n--)
	    {
	    	if (*pwidth)
	    	{
            	    addrl = addrlBase + (ppt->y * nlwidth) + (ppt->x >> 5);
                    SETRW(addrl);
	    	    src = psrc[ppt->y % tileHeight];
            	    if ( ((ppt->x & 0x1f) + *pwidth) < 32)
            	    {
	            	maskpartialbits(ppt->x, *pwidth, startmask);
			*MAPW(addrl) = DoMaskMergeRop (src, *MAPR(addrl), startmask);
            	    }
            	    else
            	    {
	            	maskbits(ppt->x, *pwidth, startmask, endmask, nlmiddle);
	            	if (startmask)
	            	{
			    *MAPW(addrl) = DoMaskMergeRop (src, *MAPR(addrl), startmask);
		    	    addrl++;
#ifndef LINE_BANK_ALIGNED
				CHECKRWO(addrl);
#endif
	            	}
	            	while (nlmiddle--)
	            	{
			    *MAPW(addrl) = DoMergeRop (src, *MAPR(addrl));
		    	    addrl++;
#ifndef LINE_BANK_ALIGNED
				CHECKRWO(addrl);
#endif
	            	}
	            	if (endmask)
			    *MAPW(addrl) = DoMaskMergeRop (src, *MAPR(addrl), endmask);
            	    }
	    	}
	    	pwidth++;
	    	ppt++;
	    }
	}
	break;
    }
    DEALLOCATE_LOCAL(pptFree);
    DEALLOCATE_LOCAL(pwidthFree);
}


/* Fill spans with tiles that aren't 32 bits wide */
void
mfbUnnaturalTileFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
DrawablePtr pDrawable;
GC		*pGC;
int		nInit;		/* number of spans to fill */
DDXPointPtr pptInit;		/* pointer to list of start points */
int *pwidthInit;		/* pointer to list of n widths */
int fSorted;
{
    int		iline;		/* first line of tile to use */
				/* next three parameters are post-clip */
    int n;			/* number of spans to fill */
    register DDXPointPtr ppt;	/* pointer to list of start points */
    register int *pwidth;	/* pointer to list of n widths */
    unsigned int *addrlBase;	/* pointer to start of bitmap */
    int		 nlwidth;	/* width in longwords of bitmap */
    register unsigned int *pdst;/* pointer to current word in bitmap */
    register unsigned int *psrc;/* pointer to current word in tile */
    register int nlMiddle;
    register int rop, nstart;
    unsigned int startmask;
    PixmapPtr	pTile;		/* pointer to tile we want to fill with */
    int		w, width, x, xSrc, ySrc, srcStartOver, nend;
    int 	tlwidth, rem, tileWidth, tileHeight, endinc;
    unsigned int      endmask, *psrcT;
    int *pwidthFree;		/* copies of the pointers to free */
    DDXPointPtr pptFree;

    if (!(pGC->planemask & 1))
	return;

    n = nInit * miFindMaxBand(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip);
    pwidthFree = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    pptFree = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!pptFree || !pwidthFree)
    {
	if (pptFree) DEALLOCATE_LOCAL(pptFree);
	if (pwidthFree) DEALLOCATE_LOCAL(pwidthFree);
	return;
    }
    pwidth = pwidthFree;
    ppt = pptFree;
    n = miClipSpans(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip,
		    pptInit, pwidthInit, nInit, 
		    ppt, pwidth, fSorted);

    if (pGC->fillStyle == FillTiled)
    {
	pTile = pGC->tile.pixmap;
	tlwidth = pTile->devKind >> 2;
	rop = pGC->alu;
    }
    else
    {
	pTile = pGC->stipple;
	tlwidth = pTile->devKind >> 2;
	rop = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->ropOpStip;
    }

    xSrc = pDrawable->x;
    ySrc = pDrawable->y;

    if (pDrawable->type == DRAWABLE_WINDOW)
    {
	addrlBase = (unsigned int *)
		(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devPrivate.ptr);
	nlwidth = (int)
		  (((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind) >> 2;
    }
    else
    {
	addrlBase = (unsigned int *)(((PixmapPtr)pDrawable)->devPrivate.ptr);
	nlwidth = (int)(((PixmapPtr)pDrawable)->devKind) >> 2;
    }

    BANK_FLAG(addrlBase)

    tileWidth = pTile->drawable.width;
    tileHeight = pTile->drawable.height;

    /* this replaces rotating the tile. Instead we just adjust the offset
     * at which we start grabbing bits from the tile.
     * Ensure that ppt->x - xSrc >= 0 and ppt->y - ySrc >= 0,
     * so that iline and rem always stay within the tile bounds.
     */
    xSrc += (pGC->patOrg.x % tileWidth) - tileWidth;
    ySrc += (pGC->patOrg.y % tileHeight) - tileHeight;

    while (n--)
    {
	iline = (ppt->y - ySrc) % tileHeight;
        pdst = addrlBase + (ppt->y * nlwidth) + (ppt->x >> 5);
        SETRW(pdst);
        psrcT = (unsigned int *) pTile->devPrivate.ptr + (iline * tlwidth);
	x = ppt->x;

	if (*pwidth)
	{
	    width = *pwidth;
	    while(width > 0)
	    {
		psrc = psrcT;
	        w = min(tileWidth, width);
		if((rem = (x - xSrc)  % tileWidth) != 0)
		{
		    /* if we're in the middle of the tile, get
		       as many bits as will finish the span, or
		       as many as will get to the left edge of the tile,
		       or a longword worth, starting at the appropriate
		       offset in the tile.
		    */
		    w = min(min(tileWidth - rem, width), BITMAP_SCANLINE_UNIT);
		    endinc = rem / BITMAP_SCANLINE_UNIT;
		    getandputrop((psrc+endinc), (rem&0x1f), (x & 0x1f), w, pdst, rop);
		    if((x & 0x1f) + w >= 0x20)
			pdst++; CHECKRWO(pdst);
		}
		else if(((x & 0x1f) + w) < 32)
		{
		    /* doing < 32 bits is easy, and worth special-casing */
		    putbitsrop(*psrc, x & 0x1f, w, pdst, rop);
		}
		else
		{
		    /* start at the left edge of the tile,
		       and put down as much as we can
		    */
		    maskbits(x, w, startmask, endmask, nlMiddle);

	            if (startmask)
		        nstart = 32 - (x & 0x1f);
	            else
		        nstart = 0;
	            if (endmask)
	                nend = (x + w)  & 0x1f;
	            else
		        nend = 0;

	            srcStartOver = nstart > 31;

		    if(startmask)
		    {
			putbitsrop(*psrc, (x & 0x1f), nstart, pdst, rop);
			pdst++;
#ifndef LINE_BANK_ALIGNED
			CHECKRWO(pdst);
#endif
			if(srcStartOver)
			    psrc++;
		    }
		     
		    while(nlMiddle--)
		    {
			    getandputrop0(psrc, nstart, 32, pdst, rop);
			    pdst++;
#ifndef LINE_BANK_ALIGNED
			    CHECKRWO(pdst);
#endif
			    psrc++;
		    }
		    if(endmask)
		    {
			getandputrop0(psrc, nstart, nend, pdst, rop);
		    }
		 }
		 x += w;
		 width -= w;
	    }
	}
	ppt++;
	pwidth++;
    }
    DEALLOCATE_LOCAL(pptFree);
    DEALLOCATE_LOCAL(pwidthFree);
}


/* Fill spans with stipples that aren't 32 bits wide */
void
mfbUnnaturalStippleFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
DrawablePtr pDrawable;
GC		*pGC;
int		nInit;		/* number of spans to fill */
DDXPointPtr pptInit;		/* pointer to list of start points */
int *pwidthInit;		/* pointer to list of n widths */
int fSorted;
{
				/* next three parameters are post-clip */
    int n;			/* number of spans to fill */
    register DDXPointPtr ppt;	/* pointer to list of start points */
    register int *pwidth;	/* pointer to list of n widths */
    int		iline;		/* first line of tile to use */
    int		*addrlBase;	/* pointer to start of bitmap */
    int		 nlwidth;	/* width in longwords of bitmap */
    register int *pdst;		/* pointer to current word in bitmap */
    register int *psrc;		/* pointer to current word in tile */
    register int nlMiddle;
    register int rop, nstart;
    int startmask;
    PixmapPtr	pTile;		/* pointer to tile we want to fill with */
    int		w, width,  x, xSrc, ySrc, srcStartOver, nend;
    int 	endmask, tlwidth, rem, tileWidth, *psrcT, endinc;
    int		tileHeight;
    int *pwidthFree;		/* copies of the pointers to free */
    DDXPointPtr pptFree;

    if (!(pGC->planemask & 1))
	return;

    n = nInit * miFindMaxBand(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip);
    pwidthFree = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    pptFree = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!pptFree || !pwidthFree)
    {
	if (pptFree) DEALLOCATE_LOCAL(pptFree);
	if (pwidthFree) DEALLOCATE_LOCAL(pwidthFree);
	return;
    }
    pwidth = pwidthFree;
    ppt = pptFree;
    n = miClipSpans(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip,
		    pptInit, pwidthInit, nInit, 
		    ppt, pwidth, fSorted);

    pTile = pGC->stipple;
    rop = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->rop;
    tlwidth = pTile->devKind >> 2;
    xSrc = pDrawable->x;
    ySrc = pDrawable->y;
    if (pDrawable->type == DRAWABLE_WINDOW)
    {
	addrlBase = (int *)
		(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devPrivate.ptr);
	nlwidth = (int)
		  (((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind) >> 2;
    }
    else
    {
	addrlBase = (int *)(((PixmapPtr)pDrawable)->devPrivate.ptr);
	nlwidth = (int)(((PixmapPtr)pDrawable)->devKind) >> 2;
    }

    BANK_FLAG(addrlBase)

    tileWidth = pTile->drawable.width;
    tileHeight = pTile->drawable.height;

    /* this replaces rotating the stipple.  Instead, we just adjust the offset
     * at which we start grabbing bits from the stipple.
     * Ensure that ppt->x - xSrc >= 0 and ppt->y - ySrc >= 0,
     * so that iline and rem always stay within the tile bounds.
     */
    xSrc += (pGC->patOrg.x % tileWidth) - tileWidth;
    ySrc += (pGC->patOrg.y % tileHeight) - tileHeight;
    while (n--)
    {
	iline = (ppt->y - ySrc) % tileHeight;
        pdst = addrlBase + (ppt->y * nlwidth) + (ppt->x >> 5);
        SETRW(pdst);
        psrcT = (int *) pTile->devPrivate.ptr + (iline * tlwidth);
	x = ppt->x;

	if (*pwidth)
	{
	    width = *pwidth;
	    while(width > 0)
	    {
		psrc = psrcT;
	        w = min(tileWidth, width);
		if((rem = (x - xSrc) % tileWidth) != 0)
		{
		    /* if we're in the middle of the tile, get
		       as many bits as will finish the span, or
		       as many as will get to the left edge of the tile,
		       or a longword worth, starting at the appropriate
		       offset in the tile.
		    */
		    w = min(min(tileWidth - rem, width), BITMAP_SCANLINE_UNIT);
		    endinc = rem / BITMAP_SCANLINE_UNIT;
		    getandputrrop((psrc + endinc), (rem & 0x1f), (x & 0x1f),
				 w, pdst, rop)
		    if((x & 0x1f) + w >= 0x20)
			pdst++;
#ifndef LINE_BANK_ALIGNED
		    CHECKRWO(pdst);
#endif
		}

		else if(((x & 0x1f) + w) < 32)
		{
		    /* doing < 32 bits is easy, and worth special-casing */
		    putbitsrrop(*psrc, x & 0x1f, w, pdst, rop);
		}
		else
		{
		    /* start at the left edge of the tile,
		       and put down as much as we can
		    */
		    maskbits(x, w, startmask, endmask, nlMiddle);

	            if (startmask)
		        nstart = 32 - (x & 0x1f);
	            else
		        nstart = 0;
	            if (endmask)
	                nend = (x + w)  & 0x1f;
	            else
		        nend = 0;

	            srcStartOver = nstart > 31;

		    if(startmask)
		    {
			putbitsrrop(*psrc, (x & 0x1f), nstart, pdst, rop);
			pdst++;
#ifndef LINE_BANK_ALIGNED
			CHECKRWO(pdst);
#endif
			if(srcStartOver)
			    psrc++;
		    }
		     
		    while(nlMiddle--)
		    {
			    getandputrrop0(psrc, nstart, 32, pdst, rop);
			    pdst++;
#ifndef LINE_BANK_ALIGNED
			    CHECKRWO(pdst);
#endif
			    psrc++;
		    }
		    if(endmask)
		    {
			getandputrrop0(psrc, nstart, nend, pdst, rop);
		    }
		 }
		 x += w;
		 width -= w;
	    }
	}
	ppt++;
	pwidth++;
    }
    DEALLOCATE_LOCAL(pptFree);
    DEALLOCATE_LOCAL(pwidthFree);
}
