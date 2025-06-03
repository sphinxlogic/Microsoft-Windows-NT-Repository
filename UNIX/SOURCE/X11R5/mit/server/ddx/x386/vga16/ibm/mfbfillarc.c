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

/* $XFree86: mit/server/ddx/x386/vga16/ibm/mfbfillarc.c,v 2.0 1994/02/25 15:02:47 dawes Exp $ */
/* $XConsortium: mfbfillarc.c,v 5.7 90/10/06 13:58:08 rws Exp $ */

#define BANKING_MODS
#include "X.h"
#include "Xprotostr.h"
#include "miscstruct.h"
#include "gcstruct.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "mfb.h"
#include "maskbits.h"
#include "mifillarc.h"
#include "OScompiler.h"	/* GJA */
#include "wm3.h"	/* GJA */

extern void miPolyFillArc();

static void
v16FillEllipseSolid(pDraw, arc, rop)
    DrawablePtr pDraw;
    xArc *arc;
    register int rop;
{
    int x, y, e;
    int yk, xk, ym, xm, dx, dy, xorg, yorg;
    register int slw;
    miFillArcRec info;
    int *addrlt, *addrlb;
    register int *addrl;
    register int n;
    int nlwidth;
    register int xpos;
    int startmask, endmask, nlmiddle;

    if (pDraw->type == DRAWABLE_WINDOW)
    {
	addrlt = (int *)
	       (((PixmapPtr)(pDraw->pScreen->devPrivate))->devPrivate.ptr);
	nlwidth = (int)
	       (((PixmapPtr)(pDraw->pScreen->devPrivate))->devKind) >> 2;
    }
    else
    {
	addrlt = (int *)(((PixmapPtr)pDraw)->devPrivate.ptr);
	nlwidth = (int)(((PixmapPtr)pDraw)->devKind) >> 2;
    }

/*    BANK_FLAG(addrlt) */

    miFillArcSetup(arc, &info);
    MIFILLARCSETUP();
    xorg += pDraw->x;
    yorg += pDraw->y;
    addrlb = addrlt;
    addrlt += nlwidth * (yorg - y);
    addrlb += nlwidth * (yorg + y + dy);
    while (y)
    {
	addrlt += nlwidth;
	addrlb -= nlwidth;
	MIFILLARCSTEP(slw);
	if (!slw)
	    continue;
	xpos = xorg - x;
	addrl = addrlt + (xpos >> 5);
	VSETRW(addrl);
	if (((xpos & 0x1f) + slw) < 32)
	{
	    maskpartialbits(xpos, slw, startmask);
	    UPDRW(VMAPRW(addrl),startmask);
	    if (miFillArcLower(slw))
	    {
		addrl = addrlb + (xpos >> 5);
		VSETRW(addrl);
		UPDRW(VMAPRW(addrl),startmask);
	    }
	    continue;
	}
	maskbits(xpos, slw, startmask, endmask, nlmiddle);
	if (startmask)
	{
	    UPDRW(VMAPRW(addrl),startmask); addrl++; VCHECKRWO(addrl);
	}
	n = nlmiddle;
	while (n--) {
	    UPDRW(VMAPRW(addrl),~0); addrl++; VCHECKRWO(addrl);
	}
	if (endmask)
	{
	    UPDRW(VMAPRW(addrl),endmask);
	}
	if (!miFillArcLower(slw))
	    continue;
	addrl = addrlb + (xpos >> 5);
	VSETRW(addrl);
	if (startmask)
	{
	    UPDRW(VMAPRW(addrl),startmask); addrl++; VCHECKRWO(addrl);
	}
	n = nlmiddle;
	while (n--) {
	    UPDRW(VMAPRW(addrl),~0); addrl++; VCHECKRWO(addrl);
	}
	if (endmask)
	{
	    UPDRW(VMAPRW(addrl),endmask);
	}
    }
}

#define FILLSPAN(xl,xr,addr) \
    if (xr >= xl) \
    { \
	width = xr - xl + 1; \
	addrl = addr + (xl >> 5); \
	VSETRW(addrl); \
	if (((xl & 0x1f) + width) < 32) \
	{ \
	    maskpartialbits(xl, width, startmask); \
	    UPDRW(VMAPRW(addrl),startmask); \
	} \
	else \
	{ \
	    maskbits(xl, width, startmask, endmask, nlmiddle); \
	    if (startmask) \
	    { \
		UPDRW(VMAPRW(addrl),startmask); addrl++; VCHECKRWO(addrl);\
	    } \
	    n = nlmiddle; \
		while (n--) { \
		    UPDRW(VMAPRW(addrl),~0); addrl++; VCHECKRWO(addrl);\
		} \
	    if (endmask) \
	    { \
		UPDRW(VMAPRW(addrl),endmask); \
	    } \
	} \
    }

#define FILLSLICESPANS(flip,addr) \
    if (!flip) \
    { \
	FILLSPAN(xl, xr, addr); \
    } \
    else \
    { \
	xc = xorg - x; \
	FILLSPAN(xc, xr, addr); \
	xc += slw - 1; \
	FILLSPAN(xl, xc, addr); \
    }

static void
v16FillArcSliceSolidCopy(pDraw, pGC, arc, rop)
    DrawablePtr pDraw;
    GCPtr pGC;
    xArc *arc;
    register int rop;
{
    register int *addrl;
    register int n;
    int yk, xk, ym, xm, dx, dy, xorg, yorg, slw;
    register int x, y, e;
    miFillArcRec info;
    miArcSliceRec slice;
    int xl, xr, xc;
    int *addrlt, *addrlb;
    int nlwidth;
    int width;
    int startmask, endmask, nlmiddle;

    if (pDraw->type == DRAWABLE_WINDOW)
    {
	addrlt = (int *)
	       (((PixmapPtr)(pDraw->pScreen->devPrivate))->devPrivate.ptr);
	nlwidth = (int)
	       (((PixmapPtr)(pDraw->pScreen->devPrivate))->devKind) >> 2;
    }
    else
    {
	addrlt = (int *)(((PixmapPtr)pDraw)->devPrivate.ptr);
	nlwidth = (int)(((PixmapPtr)pDraw)->devKind) >> 2;
    }

/*    BANK_FLAG(addrlt) */

    miFillArcSetup(arc, &info);
    miFillArcSliceSetup(arc, &slice, pGC);
    MIFILLARCSETUP();
    xorg += pDraw->x;
    yorg += pDraw->y;
    addrlb = addrlt;
    addrlt += nlwidth * (yorg - y);
    addrlb += nlwidth * (yorg + y + dy);
    slice.edge1.x += pDraw->x;
    slice.edge2.x += pDraw->x;
    while (y > 0)
    {
	addrlt += nlwidth;
	addrlb -= nlwidth;
	MIFILLARCSTEP(slw);
	MIARCSLICESTEP(slice.edge1);
	MIARCSLICESTEP(slice.edge2);
	if (miFillSliceUpper(slice))
	{
	    MIARCSLICEUPPER(xl, xr, slice, slw);
	    FILLSLICESPANS(slice.flip_top, addrlt);
	}
	if (miFillSliceLower(slice))
	{
	    MIARCSLICELOWER(xl, xr, slice, slw);
	    FILLSLICESPANS(slice.flip_bot, addrlb);
	}
    }
}

void
v16PolyFillArcSolid(pDraw, pGC, narcs, parcs)
    register DrawablePtr pDraw;
    GCPtr	pGC;
    int		narcs;
    xArc	*parcs;
{
    mfbPrivGC *priv;
    register xArc *arc;
    register int i;
    BoxRec box;
    RegionPtr cclip;
    int rop;

    priv = (mfbPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr;
    rop = priv->rop;
#if 0
    if ((rop == RROP_NOP) || !(pGC->planemask & 1))
#else
    if ( !(pGC->planemask & 0x0F))
#endif
	return;
    cclip = priv->pCompositeClip;
    for (arc = parcs, i = narcs; --i >= 0; arc++)
    {
	if (miFillArcEmpty(arc))
	    continue;
	if (miCanFillArc(arc))
	{
	    box.x1 = arc->x + pDraw->x;
	    box.y1 = arc->y + pDraw->y;
	    box.x2 = box.x1 + (int)arc->width + 1;
	    box.y2 = box.y1 + (int)arc->height + 1;
	    if ((*pDraw->pScreen->RectIn)(cclip, &box) == rgnIN)
	    {
		if ((arc->angle2 >= FULLCIRCLE) ||
		    (arc->angle2 <= -FULLCIRCLE))
		    DO_WM3(pGC,v16FillEllipseSolid(pDraw, arc, rop))
		else
		    DO_WM3(pGC,v16FillArcSliceSolidCopy(pDraw, pGC, arc, rop)) 
		continue;
	    }
	}
	miPolyFillArc(pDraw, pGC, 1, arc);
    }
}

void
v16PolyFillArc(pDraw, pGC, narcs, parcs)
    register DrawablePtr pDraw;
    GCPtr	pGC;
    int		narcs;
    xArc	*parcs;
{
    extern int x386VTSema;

    if ( !x386VTSema || (pGC->fillStyle != FillSolid) ) {
	miPolyFillArc(pDraw, pGC, narcs, parcs);
    } else {
        v16PolyFillArcSolid(pDraw, pGC, narcs, parcs);
    }
}
