/* $XFree86: mit/server/ddx/x386/vga256/cfb.banked/cfbbitblt.c,v 2.0 1993/08/18 06:48:21 dawes Exp $ */
/*
 * cfb copy area
 */

/*
Copyright 1989 by the Massachusetts Institute of Technology

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of M.I.T. not be used in
advertising or publicity pertaining to distribution of the software
without specific, written prior permission.  M.I.T. makes no
representations about the suitability of this software for any
purpose.  It is provided "as is" without express or implied warranty.

Author: Keith Packard

*/
/* $XConsortium: cfbbitblt.c,v 5.39 91/05/24 16:33:25 keith Exp $ */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"
#include	"cfb.h"
#include	"cfbmskbits.h"
#include	"cfb8bit.h"
#include	"fastblt.h"
#include        "vgaBank.h"
#include	"cfbfuncs.h"

extern int  cfbDoBitbltXor();
extern int  cfbDoBitbltOr();
extern int  cfbDoBitbltGeneral();

cfbDoBitblt (pSrc, pDst, alu, prgnDst, pptSrc, planemask)
    DrawablePtr	    pSrc, pDst;
    int		    alu;
    RegionPtr	    prgnDst;
    DDXPointPtr	    pptSrc;
    unsigned long   planemask;
{
    int	(*blt)() = cfbDoBitbltGeneral;
    if ((planemask & PMSK) == PMSK) {
	switch (alu) {
	case GXcopy:
	    blt = cfbLowlevFuncs.doBitbltCopy;
	    break;
	case GXxor:
	    blt = cfbDoBitbltXor;
	    break;
	case GXor:
	    blt = cfbDoBitbltOr;
	    break;
	}
    }
    return (*blt) (pSrc, pDst, alu, prgnDst, pptSrc, planemask);
}

static int (*doBitBlt)() = NULL;
static unsigned long bitBltPlane = 0;

RegionPtr
cfbCopyArea(pSrcDrawable, pDstDrawable,
            pGC, srcx, srcy, width, height, dstx, dsty)
    register DrawablePtr pSrcDrawable;
    register DrawablePtr pDstDrawable;
    GC *pGC;
    int srcx, srcy;
    int width, height;
    int dstx, dsty;
{
    RegionPtr prgnSrcClip;	/* may be a new region, or just a copy */
    Bool freeSrcClip = FALSE;

    RegionPtr prgnExposed;
    RegionRec rgnDst;
    DDXPointPtr pptSrc;
    register DDXPointPtr ppt;
    register BoxPtr pbox;
    int i;
    register int dx;
    register int dy;
    xRectangle origSource;
    DDXPointRec origDest;
    int numRects;
    BoxRec fastBox;
    int fastClip = 0;		/* for fast clipping with pixmap source */
    int fastExpose = 0;		/* for fast exposures with pixmap source */
    int (*localDoBitBlt)();
    unsigned long localBitBltPlane;

    origSource.x = srcx;
    origSource.y = srcy;
    origSource.width = width;
    origSource.height = height;
    origDest.x = dstx;
    origDest.y = dsty;

    localDoBitBlt = doBitBlt ? doBitBlt : cfbLowlevFuncs.doBitbltCopy;
    localBitBltPlane = bitBltPlane;
    doBitBlt = cfbLowlevFuncs.doBitbltCopy;
    bitBltPlane = 0;

    if ((pSrcDrawable != pDstDrawable) &&
	pSrcDrawable->pScreen->SourceValidate)
    {
	(*pSrcDrawable->pScreen->SourceValidate) (pSrcDrawable, srcx, srcy, width, height);
    }

    if (localDoBitBlt == cfbLowlevFuncs.doBitbltCopy)
    {
	if (pGC->alu != GXcopy || (pGC->planemask & PMSK) != PMSK)
	{
	    localDoBitBlt = cfbDoBitbltGeneral;
	    if ((pGC->planemask & PMSK) == PMSK)
	    {
		switch (pGC->alu) {
		case GXxor:
		    localDoBitBlt = cfbDoBitbltXor;
		    break;
		case GXor:
		    localDoBitBlt = cfbDoBitbltOr;
		    break;
		}
	    }
	}
    }

    srcx += pSrcDrawable->x;
    srcy += pSrcDrawable->y;

    /* clip the source */

    if (pSrcDrawable->type == DRAWABLE_PIXMAP)
    {
	if ((pSrcDrawable == pDstDrawable) &&
	    (pGC->clientClipType == CT_NONE))
	{
	    prgnSrcClip = ((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;
	}
	else
	{
	    fastClip = 1;
	}
    }
    else
    {
	if (pGC->subWindowMode == IncludeInferiors)
	{
	    if (!((WindowPtr) pSrcDrawable)->parent)
	    {
		/*
		 * special case bitblt from root window in
		 * IncludeInferiors mode; just like from a pixmap
		 */
		fastClip = 1;
	    }
	    else if ((pSrcDrawable == pDstDrawable) &&
		(pGC->clientClipType == CT_NONE))
	    {
		prgnSrcClip = ((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;
	    }
	    else
	    {
		prgnSrcClip = NotClippedByChildren((WindowPtr)pSrcDrawable);
		freeSrcClip = TRUE;
	    }
	}
	else
	{
	    prgnSrcClip = &((WindowPtr)pSrcDrawable)->clipList;
	}
    }

    fastBox.x1 = srcx;
    fastBox.y1 = srcy;
    fastBox.x2 = srcx + width;
    fastBox.y2 = srcy + height;

    /* Don't create a source region if we are doing a fast clip */
    if (fastClip)
    {
	fastExpose = 1;
	/*
	 * clip the source; if regions extend beyond the source size,
 	 * make sure exposure events get sent
	 */
	if (fastBox.x1 < pSrcDrawable->x)
	{
	    fastBox.x1 = pSrcDrawable->x;
	    fastExpose = 0;
	}
	if (fastBox.y1 < pSrcDrawable->y)
	{
	    fastBox.y1 = pSrcDrawable->y;
	    fastExpose = 0;
	}
	if (fastBox.x2 > pSrcDrawable->x + (int) pSrcDrawable->width)
	{
	    fastBox.x2 = pSrcDrawable->x + (int) pSrcDrawable->width;
	    fastExpose = 0;
	}
	if (fastBox.y2 > pSrcDrawable->y + (int) pSrcDrawable->height)
	{
	    fastBox.y2 = pSrcDrawable->y + (int) pSrcDrawable->height;
	    fastExpose = 0;
	}
    }
    else
    {
	(*pGC->pScreen->RegionInit)(&rgnDst, &fastBox, 1);
	(*pGC->pScreen->Intersect)(&rgnDst, &rgnDst, prgnSrcClip);
    }

    dstx += pDstDrawable->x;
    dsty += pDstDrawable->y;

    if (pDstDrawable->type == DRAWABLE_WINDOW)
    {
	if (!((WindowPtr)pDstDrawable)->realized)
	{
	    if (!fastClip)
		(*pGC->pScreen->RegionUninit)(&rgnDst);
	    if (freeSrcClip)
		(*pGC->pScreen->RegionDestroy)(prgnSrcClip);
	    return NULL;
	}
    }

    dx = srcx - dstx;
    dy = srcy - dsty;

    /* Translate and clip the dst to the destination composite clip */
    if (fastClip)
    {
	RegionPtr cclip;

        /* Translate the region directly */
        fastBox.x1 -= dx;
        fastBox.x2 -= dx;
        fastBox.y1 -= dy;
        fastBox.y2 -= dy;

	/* If the destination composite clip is one rectangle we can
	   do the clip directly.  Otherwise we have to create a full
	   blown region and call intersect */

	/* XXX because CopyPlane uses this routine for 8-to-1 bit
	 * copies, this next line *must* also correctly fetch the
	 * composite clip from an mfb gc
	 */

	cclip = ((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;
        if (REGION_NUM_RECTS(cclip) == 1)
        {
	    BoxPtr pBox = REGION_RECTS(cclip);

	    if (fastBox.x1 < pBox->x1) fastBox.x1 = pBox->x1;
	    if (fastBox.x2 > pBox->x2) fastBox.x2 = pBox->x2;
	    if (fastBox.y1 < pBox->y1) fastBox.y1 = pBox->y1;
	    if (fastBox.y2 > pBox->y2) fastBox.y2 = pBox->y2;

	    /* Check to see if the region is empty */
	    if (fastBox.x1 >= fastBox.x2 || fastBox.y1 >= fastBox.y2)
		(*pGC->pScreen->RegionInit)(&rgnDst, NullBox, 0);
	    else
		(*pGC->pScreen->RegionInit)(&rgnDst, &fastBox, 1);
	}
        else
	{
	    /* We must turn off fastClip now, since we must create
	       a full blown region.  It is intersected with the
	       composite clip below. */
	    fastClip = 0;
	    (*pGC->pScreen->RegionInit)(&rgnDst, &fastBox,1);
	}
    }
    else
    {
        (*pGC->pScreen->TranslateRegion)(&rgnDst, -dx, -dy);
    }

    if (!fastClip)
    {
	(*pGC->pScreen->Intersect)(&rgnDst,
				   &rgnDst,
				 ((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);
    }

    /* Do bit blitting */
    numRects = REGION_NUM_RECTS(&rgnDst);
    if (numRects && width && height)
    {
	if(!(pptSrc = (DDXPointPtr)ALLOCATE_LOCAL(numRects *
						  sizeof(DDXPointRec))))
	{
	    (*pGC->pScreen->RegionUninit)(&rgnDst);
	    if (freeSrcClip)
		(*pGC->pScreen->RegionDestroy)(prgnSrcClip);
	    return NULL;
	}
	pbox = REGION_RECTS(&rgnDst);
	ppt = pptSrc;
	for (i = numRects; --i >= 0; pbox++, ppt++)
	{
	    ppt->x = pbox->x1 + dx;
	    ppt->y = pbox->y1 + dy;
	}

	(*localDoBitBlt) (pSrcDrawable, pDstDrawable, pGC->alu, &rgnDst, pptSrc, pGC->planemask);
	DEALLOCATE_LOCAL(pptSrc);
    }

    prgnExposed = NULL;
    if (((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->fExpose)
    {
	extern RegionPtr    miHandleExposures();

        /* Pixmap sources generate a NoExposed (we return NULL to do this) */
        if (!fastExpose)
	    prgnExposed =
		miHandleExposures(pSrcDrawable, pDstDrawable, pGC,
				  origSource.x, origSource.y,
				  (int)origSource.width,
				  (int)origSource.height,
				  origDest.x, origDest.y, localBitBltPlane);
    }
    (*pGC->pScreen->RegionUninit)(&rgnDst);
    if (freeSrcClip)
	(*pGC->pScreen->RegionDestroy)(prgnSrcClip);
    return prgnExposed;
}

#if (PPW == 4)

cfbCopyPlane1to8 (pSrcDrawable, pDstDrawable, rop, prgnDst, pptSrc, planemask)
    DrawablePtr pSrcDrawable;
    DrawablePtr pDstDrawable;
    int	rop;
    unsigned long planemask;
    RegionPtr prgnDst;
    DDXPointPtr pptSrc;
{
    int	srcx, srcy, dstx, dsty, width, height;
    int xoffSrc, xoffDst;
    unsigned long *psrcBase, *pdstBase;
    int	widthSrc, widthDst;
    unsigned long *psrcLine, *pdstLine;
    register unsigned long *psrc, *pdst;
    register unsigned long bits, tmp;
    register int leftShift, rightShift;
    unsigned long startmask, endmask;
    register int nl, nlMiddle;
    int firstoff, secondoff;
    unsigned long    src;
    int nbox;
    BoxPtr  pbox;

    cfbGetLongWidthAndPointer (pSrcDrawable, widthSrc, psrcBase)

    cfbGetLongWidthAndPointer (pDstDrawable, widthDst, pdstBase)

    BANK_FLAG(pdstBase)

#ifndef MACH386
    if (psrcBase >= (unsigned long *)VGABASE) ErrorF("1to8 src onscreen\n");
#endif /* MACH386 */

    nbox = REGION_NUM_RECTS(prgnDst);
    pbox = REGION_RECTS(prgnDst);
    while (nbox--)
    {
	dstx = pbox->x1;
	dsty = pbox->y1;
	srcx = pptSrc->x;
	srcy = pptSrc->y;
	width = pbox->x2 - pbox->x1;
	height = pbox->y2 - pbox->y1;
	pbox++;
	pptSrc++;
	psrcLine = psrcBase + srcy * widthSrc + (srcx >> 5);
	pdstLine = pdstBase + dsty * widthDst + (dstx >> 2);
	xoffSrc = srcx & 0x1f;
	xoffDst = dstx & 0x3;
	if (xoffDst + width < 4)
	{
	    maskpartialbits(dstx, width, startmask);
	    endmask = 0;
	    nlMiddle = 0;
	}
	else
	{
	    maskbits(dstx, width, startmask, endmask, nlMiddle);
	}
	/*
	 * compute constants for the first four bits to be
	 * copied.  This avoids troubles with partial first
	 * writes, and difficult shift computation
	 */
	if (startmask)
	{
	    firstoff = xoffSrc - xoffDst;
	    if (firstoff > 28)
		secondoff = 32 - firstoff;
	    if (xoffDst)
	    {
	    	srcx += (4-xoffDst);
	    	dstx += (4-xoffDst);
	    	xoffSrc = srcx & 0x1f;
	    }
	}
	leftShift = xoffSrc;
	rightShift = 32 - leftShift;
	if (cfb8StippleRRop == GXcopy)
	{
	    while (height--)
	    {
	    	psrc = psrcLine;
	    	pdst = pdstLine;
		SETRW(pdst);
	    	psrcLine += widthSrc;
	    	pdstLine += widthDst;
	    	bits = *psrc++;
	    	if (startmask)
	    	{
		    if (firstoff < 0)
		    	tmp = BitRight (bits, -firstoff);
		    else
		    {
		    	tmp = BitLeft (bits, firstoff);
			/*
			 * need a more cautious test for partialmask
			 * case...
			 */
		    	if (firstoff >= 28)
		    	{
			    bits = *psrc++;
			    if (firstoff != 28)
				tmp |= BitRight (bits, secondoff);
		    	}
		    }
		    *pdst = *pdst & ~startmask | GetFourPixels(tmp) & startmask;
		    pdst++;
		    CHECKRWO(pdst);
	    	}
	    	nl = nlMiddle;
	    	while (nl >= 8)
	    	{
		    nl -= 8;
		    tmp = BitLeft(bits, leftShift);
		    bits = *psrc++;
		    if (rightShift != 32)
		    	tmp |= BitRight(bits, rightShift);

#ifdef FAST_CONSTANT_OFFSET_MODE
# define StorePixels(pdst,o,pixels)	(pdst)[o] = (pixels)
# define EndStep(pdst,o)		(pdst) += (o)
# define StoreRopPixels(pdst,o,and,xor)	(pdst)[o] = DoRRop((pdst)[o],and,xor);
#else
# define StorePixels(pdst,o,pixels)	*(pdst)++ = (pixels); CHECKRWO(pdst)
# define EndStep(pdst,o)
# define StoreRopPixels(pdst,o,and,xor)	*(pdst) = DoRRop(*(pdst),and,xor); (pdst)++; CHECKRWO(pdst);
#endif

#define Step(c)			NextFourBits(c);
#define StoreBitsPlain(o,c)	StorePixels(pdst,o,GetFourPixels(c))
#define StoreRopBitsPlain(o,c)	StoreRopPixels(pdst,o,\
					cfb8StippleAnd[GetFourBits(c)], \
					cfb8StippleXor[GetFourBits(c)])
#define StoreBits0(c)		StoreBitsPlain(0,c)
#define StoreRopBits0(c)	StoreRopBitsPlain(0,c)

#if (BITMAP_BIT_ORDER == MSBFirst)
# define StoreBits(o,c)	StoreBitsPlain(o,c)
# define StoreRopBits(o,c)  StoreRopBitsPlain(o,c)
# define FirstStep(c)	Step(c)
#else
# define StoreBits(o,c)	StorePixels(pdst,o,*((unsigned long *)\
			    (((char *) cfb8Pixels) + (c & 0x3c))))
# define StoreRopBits(o,c)  StoreRopPixels(pdst,o, \
	    *((unsigned long *) (((char *) cfb8StippleAnd) + (c & 0x3c))), \
	    *((unsigned long *) (((char *) cfb8StippleXor) + (c & 0x3c))))
# define FirstStep(c)	c = BitLeft (c, 2);
#endif

		    StoreBits0(tmp);	FirstStep(tmp);
		    StoreBits(1,tmp);	Step(tmp);
		    StoreBits(2,tmp);	Step(tmp);
		    StoreBits(3,tmp);	Step(tmp);
		    StoreBits(4,tmp);	Step(tmp);
		    StoreBits(5,tmp);	Step(tmp);
		    StoreBits(6,tmp);	Step(tmp);
		    StoreBits(7,tmp);   EndStep (pdst,8);
	    	}
	    	if (nl || endmask)
	    	{
		    tmp = BitLeft(bits, leftShift);
		    /*
		     * better condition needed -- mustn't run
		     * off the end of the source...
		     */
		    if (rightShift != 32)
		    {
		    	bits = *psrc++;
		    	tmp |= BitRight (bits, rightShift);
		    }
		    EndStep (pdst, nl);
		    switch (nl)
		    {
		    case 7:
			StoreBitsPlain(-7,tmp);	Step(tmp);
		    case 6:
			StoreBitsPlain(-6,tmp);	Step(tmp);
		    case 5:
			StoreBitsPlain(-5,tmp);	Step(tmp);
		    case 4:
			StoreBitsPlain(-4,tmp);	Step(tmp);
		    case 3:
			StoreBitsPlain(-3,tmp);	Step(tmp);
		    case 2:
			StoreBitsPlain(-2,tmp);	Step(tmp);
		    case 1:
			StoreBitsPlain(-1,tmp);	Step(tmp);
		    }
		    if (endmask)
		    	*pdst = *pdst & ~endmask | GetFourPixels(tmp) & endmask;
	    	}
	    }
	}
	else
	{
	    while (height--)
	    {
	    	psrc = psrcLine;
	    	pdst = pdstLine;
		SETRW(pdst);
	    	psrcLine += widthSrc;
	    	pdstLine += widthDst;
	    	bits = *psrc++;
	    	if (startmask)
	    	{
		    if (firstoff < 0)
		    	tmp = BitRight (bits, -firstoff);
		    else
		    {
		    	tmp = BitLeft (bits, firstoff);
		    	if (firstoff >= 28)
		    	{
			    bits = *psrc++;
			    if (firstoff != 28)
				tmp |= BitRight (bits, secondoff);
		    	}
		    }
		    src = GetFourBits(tmp);
		    *pdst = MaskRRopPixels (*pdst, src, startmask);
		    pdst++;
		    CHECKRWO(pdst);
	    	}
	    	nl = nlMiddle;
		while (nl >= 8)
		{
		    nl -= 8;
		    tmp = BitLeft(bits, leftShift);
		    bits = *psrc++;
		    if (rightShift != 32)
			tmp |= BitRight(bits, rightShift);
		    StoreRopBits0(tmp);	FirstStep(tmp);
		    StoreRopBits(1,tmp);	Step(tmp);
		    StoreRopBits(2,tmp);	Step(tmp);
		    StoreRopBits(3,tmp);	Step(tmp);
		    StoreRopBits(4,tmp);	Step(tmp);
		    StoreRopBits(5,tmp);	Step(tmp);
		    StoreRopBits(6,tmp);	Step(tmp);
		    StoreRopBits(7,tmp);	EndStep(pdst,8);
		}
	    	if (nl || endmask)
	    	{
		    tmp = BitLeft(bits, leftShift);
		    /*
		     * better condition needed -- mustn't run
		     * off the end of the source...
		     */
		    if (rightShift != 32)
		    {
		    	bits = *psrc++;
		    	tmp |= BitRight (bits, rightShift);
		    }
		    while (nl--)
		    {
			src = GetFourBits (tmp);
			*pdst = RRopPixels (*pdst, src);
		    	pdst++;
			CHECKRWO(pdst);
			NextFourBits(tmp);
		    }
		    if (endmask)
		    {
			src = GetFourBits (tmp);
			*pdst = MaskRRopPixels (*pdst, src, endmask);
		    }
	    	}
	    }
	}
    }
}

#endif

RegionPtr cfbCopyPlane(pSrcDrawable, pDstDrawable,
	    pGC, srcx, srcy, width, height, dstx, dsty, bitPlane)
    DrawablePtr 	pSrcDrawable;
    DrawablePtr		pDstDrawable;
    GCPtr		pGC;
    int 		srcx, srcy;
    int 		width, height;
    int 		dstx, dsty;
    unsigned long	bitPlane;
{
    RegionPtr	ret;
    extern RegionPtr    miHandleExposures();

#if (PPW == 4)
    extern unsigned long cfbCopyPlaneBitPlane;
    extern cfbCopyPlane8to1();

    if (pSrcDrawable->depth == 1 && pDstDrawable->depth == 8)
    {
    	if (bitPlane == 1)
	{
       	    doBitBlt = cfbCopyPlane1to8;
	    bitBltPlane = bitPlane;
	    cfb8CheckOpaqueStipple (pGC->alu,
				    pGC->fgPixel, pGC->bgPixel,
				    pGC->planemask);
    	    ret = cfbCopyArea (pSrcDrawable, pDstDrawable,
	    	    pGC, srcx, srcy, width, height, dstx, dsty);
	}
	else
	    ret = miHandleExposures (pSrcDrawable, pDstDrawable,
	    	pGC, srcx, srcy, width, height, dstx, dsty, bitPlane);
    }
    else if (pSrcDrawable->depth == 8 && pDstDrawable->depth == 1)
    {
	extern	int InverseAlu[16];
	int oldalu;

	oldalu = pGC->alu;
    	if (pGC->fgPixel == 0 && pGC->bgPixel == 1)
	    pGC->alu = InverseAlu[pGC->alu];
    	else if (pGC->fgPixel == pGC->bgPixel)
	    pGC->alu = mfbReduceRop(pGC->alu, pGC->fgPixel);
	cfbCopyPlaneBitPlane = bitPlane;
	doBitBlt = cfbCopyPlane8to1;
	bitBltPlane = bitPlane;
	ret = cfbCopyArea (pSrcDrawable, pDstDrawable,
		    pGC, srcx, srcy, width, height, dstx, dsty);
	pGC->alu = oldalu;
    }
    else
    {
	PixmapPtr	pBitmap;
	ScreenPtr	pScreen = pSrcDrawable->pScreen;
	GCPtr		pGC1;
	unsigned long	fg, bg;

	pBitmap = (*pScreen->CreatePixmap) (pScreen, width, height, 1);
	if (!pBitmap)
	    return NULL;
	pGC1 = GetScratchGC (1, pScreen);
	if (!pGC1)
	{
	    (*pScreen->DestroyPixmap) (pBitmap);
	    return NULL;
	}
	/*
	 * don't need to set pGC->fgPixel,bgPixel as copyPlane8to1
	 * ignores pixel values, expecting the rop to "do the
	 * right thing", which GXcopy will.
	 */
	ValidateGC ((DrawablePtr) pBitmap, pGC1);
	cfbCopyPlaneBitPlane = bitPlane;
	doBitBlt = cfbCopyPlane8to1;
	bitBltPlane = bitPlane;
	/* no exposures here, scratch GC's don't get graphics expose */
	(void) cfbCopyArea (pSrcDrawable, (DrawablePtr) pBitmap,
			    pGC1, srcx, srcy, width, height, 0, 0);
	cfb8CheckOpaqueStipple (pGC->alu,
				pGC->fgPixel, pGC->bgPixel,
				pGC->planemask);
	doBitBlt = cfbCopyPlane1to8;
	bitBltPlane = 1;
	/* no exposures here, copy bits from inside a pixmap */
	(void) cfbCopyArea ((DrawablePtr) pBitmap, pDstDrawable, pGC,
			    0, 0, width, height, dstx, dsty);
	FreeScratchGC (pGC1);
	(*pScreen->DestroyPixmap) (pBitmap);
	/* compute resultant exposures */
	ret = miHandleExposures (pSrcDrawable, pDstDrawable, pGC,
				 srcx, srcy, width, height,
				 dstx, dsty, bitPlane);
    }
    return ret;
#else
    return miCopyPlane (pSrcDrawable, pDstDrawable,
	    pGC, srcx, srcy, width, height, dstx, dsty, bitPlane);
#endif
}
