/************************************************************
Copyright 1989 by The Massachusetts Institute of Technology

Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and without
fee is hereby granted, provided that the above copyright
notice appear in all copies and that both that copyright
notice and this permission notice appear in supporting
documentation, and that the name of MIT not be used in
advertising or publicity pertaining to distribution of the
software without specific prior written permission.
M.I.T. makes no representation about the suitability of
this software for any purpose. It is provided "as is"
without any express or implied warranty.

********************************************************/
/* GJA -- Took mfb code and modified it. */

/* $XFree86: mit/server/ddx/x386/vga16/ibm/mfbzerarc.c,v 2.0 1994/02/25 15:02:48 dawes Exp $ */
/* $XConsortium: mfbzerarc.c,v 5.10 89/09/20 18:55:33 rws Exp $ */

/* Derived from:
 * "Algorithm for drawing ellipses or hyperbolae with a digital plotter"
 * by M. L. V. Pitteway
 * The Computer Journal, November 1967, Volume 10, Number 3, pp. 282-289
 */

#define BANKING_MODS
#include "X.h"
#include "Xprotostr.h"
#include "miscstruct.h"
#include "gcstruct.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "mfb.h"
#include "maskbits.h"
#include "mizerarc.h"
#include "OScompiler.h"	/* GJA */
#include "wm3.h"	/* GJA */

extern void miPolyArc(), miZeroPolyArc();
/*
 * Note, LEFTMOST must be the bit leftmost in the actual screen
 * representation.  This depends on both BITMAP_BIT_ORDER and
 * IMAGE_BYTE_ORDER
 * DHD 10/92
 */

#if (BITMAP_BIT_ORDER == MSBFirst)
#if (IMAGE_BYTE_ORDER == MSBFirst)
#define LEFTMOST	((unsigned int) 0x80000000)
#else
#define LEFTMOST	((unsigned int) 0x80)
#endif
#else
#if (IMAGE_BYTE_ORDER == LSBFirst)
#define LEFTMOST	((unsigned int) 1)
#else
#define LEFTMOST	((unsigned int) 0x1000000)
#endif
#endif

#define PixelateWhite(addr,off) \
{ \
    register int *tmpaddr = &((addr)[(off)>>5]); \
    VSETRW(tmpaddr); \
    UPDRW(VMAPRW(tmpaddr),SCRRIGHT (LEFTMOST, ((off) & 0x1f))); \
}
#define PixelateBlack(addr,off) \
{ \
    register int *tmpaddr = &((addr)[(off)>>5]); \
    VSETRW(tmpaddr); \
    UPDRW(VMAPRW(tmpaddr),~(SCRRIGHT (LEFTMOST, ((off) & 0x1f)))); \
}

#define Pixelate(base,off) \
{ \
    paddr = base + ((off)>>5); \
    pmask = SCRRIGHT(LEFTMOST, (off) & 0x1f); \
    VSETRW(paddr); \
    UPDRW(VMAPW(paddr),(pixel & pmask)); \
}

#define DoPix(bit,base,off) if (mask & bit) Pixelate(base,off);

static void
v16ZeroArcSS(pDraw, pGC, arc)
    DrawablePtr pDraw;
    GCPtr pGC;
    xArc *arc;
{
    miZeroArcRec info;
    Bool do360;
    register int x, y, a, b, d, mask;
    register int k1, k3, dx, dy;
    int *addrl;
    int *yorgl, *yorgol;
    unsigned long pixel;
    int nlwidth, yoffset, dyoffset;
    int pmask;
    register int *paddr;

    if (((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->rop ==
	RROP_BLACK)
	pixel = 0;
    else
	pixel = ~0L;

    if (pDraw->type == DRAWABLE_WINDOW)
    {
	addrl = (int *)
		(((PixmapPtr)(pDraw->pScreen->devPrivate))->devPrivate.ptr);
	nlwidth = (int)
		(((PixmapPtr)(pDraw->pScreen->devPrivate))->devKind) >> 2;
    }
    else
    {
	addrl = (int *)(((PixmapPtr)pDraw)->devPrivate.ptr);
	nlwidth = (int)(((PixmapPtr)pDraw)->devKind) >> 2;
    }

/*    BANK_FLAG(addrl) */

    do360 = miZeroArcSetup(arc, &info, TRUE);
    yorgl = addrl + ((info.yorg + pDraw->y) * nlwidth);
    yorgol = addrl + ((info.yorgo + pDraw->y) * nlwidth);
    info.xorg += pDraw->x;
    info.xorgo += pDraw->x;
    MIARCSETUP();
    yoffset = y ? nlwidth : 0;
    dyoffset = 0;
    mask = info.initialMask;
    if (!(arc->width & 1))
    {
	DoPix(2, yorgl, info.xorgo);
	DoPix(8, yorgol, info.xorgo);
    }
    if (!info.end.x || !info.end.y)
    {
	mask = info.end.mask;
	info.end = info.altend;
    }
    if (do360 && (arc->width == arc->height) && !(arc->width & 1))
    {
	int xoffset = nlwidth;
	int *yorghl = yorgl + (info.h * nlwidth);
	int xorghp = info.xorg + info.h;
	int xorghn = info.xorg - info.h;

        while (1)
        {
	    PixelateWhite(yorgl + yoffset, info.xorg + x);
	    PixelateWhite(yorgl + yoffset, info.xorg - x);
	    PixelateWhite(yorgol- yoffset, info.xorg - x);
	    PixelateWhite(yorgol - yoffset, info.xorg + x);
	    if (a < 0)
	        break;
	    PixelateWhite(yorghl - xoffset, xorghp - y);
	    PixelateWhite(yorghl - xoffset, xorghn + y);
	    PixelateWhite(yorghl + xoffset, xorghn + y);
	    PixelateWhite(yorghl + xoffset, xorghp - y);
	    xoffset += nlwidth;
	    MIARCCIRCLESTEP(yoffset += nlwidth;);
        }
	x = info.w;
	yoffset = info.h * nlwidth;
    }
    else if (do360)
    {
	while (y < info.h || x < info.w)
	{
	    MIARCOCTANTSHIFT(dyoffset = nlwidth;);
	    Pixelate(yorgl + yoffset, info.xorg + x);
	    Pixelate(yorgl + yoffset, info.xorgo - x);
	    Pixelate(yorgol - yoffset, info.xorgo - x);
	    Pixelate(yorgol - yoffset, info.xorg + x);
	    MIARCSTEP(yoffset += dyoffset;, yoffset += nlwidth;);
	}
    }
    else
    {
	while (y < info.h || x < info.w)
	{
	    MIARCOCTANTSHIFT(dyoffset = nlwidth;);
	    if ((x == info.start.x) || (y == info.start.y))
	    {
		mask = info.start.mask;
		info.start = info.altstart;
	    }
	    DoPix(1, yorgl + yoffset, info.xorg + x);
	    DoPix(2, yorgl + yoffset, info.xorgo - x);
	    DoPix(4, yorgol - yoffset, info.xorgo - x);
	    DoPix(8, yorgol - yoffset, info.xorg + x);
	    if ((x == info.end.x) || (y == info.end.y))
	    {
		mask = info.end.mask;
		info.end = info.altend;
	    }
	    MIARCSTEP(yoffset += dyoffset;, yoffset += nlwidth;);
	}
    }
    if ((x == info.start.x) || (y == info.start.y))
	mask = info.start.mask;
    DoPix(1, yorgl + yoffset, info.xorg + x);
    DoPix(4, yorgol - yoffset, info.xorgo - x);
    if (arc->height & 1)
    {
	DoPix(2, yorgl + yoffset, info.xorgo - x);
	DoPix(8, yorgol - yoffset, info.xorg + x);
    }
}

void
v16ZeroPolyArcSS(pDraw, pGC, narcs, parcs)
    DrawablePtr	pDraw;
    GCPtr	pGC;
    int		narcs;
    xArc	*parcs;
{
    register xArc *arc;
    register int i;
    BoxRec box;
    RegionPtr cclip;

    if (!pGC->planemask & 0x0F)
	return;
    cclip = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip;
    for (arc = parcs, i = narcs; --i >= 0; arc++)
    {
	if (miCanZeroArc(arc))
	{
	    box.x1 = arc->x + pDraw->x;
	    box.y1 = arc->y + pDraw->y;
	    box.x2 = box.x1 + (int)arc->width + 1;
	    box.y2 = box.y1 + (int)arc->height + 1;
	    if ((*pDraw->pScreen->RectIn)(cclip, &box) == rgnIN)
		v16ZeroArcSS(pDraw, pGC, arc);
	    else
		miZeroPolyArc(pDraw, pGC, 1, arc);
	}
	else
	    miPolyArc(pDraw, pGC, 1, arc);
    }
}

void
v16ZeroPolyArc(pDraw, pGC, narcs, parcs)
    DrawablePtr	pDraw;
    GCPtr	pGC;
    int		narcs;
    xArc	*parcs;
{
    extern int x386VTSema;

    if ( !x386VTSema ) {
	miZeroPolyArc(pDraw, pGC, narcs, parcs);
    } else {
        DO_WM3(pGC,v16ZeroPolyArcSS(pDraw, pGC, narcs, parcs));
    }
}
