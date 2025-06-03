/************************************************************
Copyright 1989 by The Massachusetts Institute of Technology

Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and without
fee is hereby granted, provided that the above copyright
no- tice appear in all copies and that both that copyright
no- tice and this permission notice appear in supporting
docu- mentation, and that the name of MIT not be used in
advertising or publicity pertaining to distribution of the
software without specific prior written permission.
M.I.T. makes no representation about the suitability of
this software for any purpose. It is provided "as is"
without any express or implied warranty.

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)

KEVIN E. MARTIN, RICKARD E. FAITH, AND TIAGO GONS DISCLAIM
ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
KEVIN E. MARTIN, RICKARD E. FAITH, OR TIAGO GONS BE LIABLE
FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
Further modifications by Tiago Gons (tiago@comosjn.hobby.nl)

********************************************************/

/* $XFree86: mit/server/ddx/x386/accel/mach8/mach8plypt.c,v 2.3 1993/09/04 09:03:08 dawes Exp $ */

/* $XConsortium: cfbpolypnt.c,v 5.13 91/07/14 13:51:14 keith Exp $ */

#include "X.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "scrnintstr.h"
#include "cfb.h"
#include "cfbmskbits.h"
#include "regmach8.h"
#include "mach8.h"

#define isClipped(c,ul,lr)  ((((c) - (ul)) | ((lr) - (c))) & ClipMask)

void
mach8PolyPoint(pDrawable, pGC, mode, npt, pptInit)
    DrawablePtr pDrawable;
    GCPtr pGC;
    int mode;
    int npt;
    xPoint *pptInit;
{
    register long   pt;
    register long   c1, c2;
    register unsigned long   ClipMask = 0x80008000;
    register long   *ppt;
    RegionPtr	    cclip;
    int		    nbox;
    register int    i;
    register BoxPtr pbox;
    int		    off;
    cfbPrivGCPtr    devPriv;
    xPoint	    *pptPrev;

/* 11-jun-93 TCG : is VT visible */
    if (!x386VTSema)
    {
        cfbPolyPoint(pDrawable, pGC, mode, npt, pptInit);
	return;
    }

    devPriv = (cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr); 
    if (pGC->alu == GXnoop)
	return;
    cclip = devPriv->pCompositeClip;
    if ((mode == CoordModePrevious) && (npt > 1))
    {
	for (pptPrev = pptInit + 1, i = npt - 1; --i >= 0; pptPrev++)
	{
	    pptPrev->x += (pptPrev-1)->x;
	    pptPrev->y += (pptPrev-1)->y;
	}
    }
    off = *((int *) &pDrawable->x);
    off -= (off & 0x8000) << 1;

    WaitQueue(4);
    outw(FRGD_MIX, FSS_FRGDCOL | mach8alu[pGC->alu]);
    outw(WRT_MASK, (short)pGC->planemask);
    outw(FRGD_COLOR, (short)pGC->fgPixel);
    outw(MAJ_AXIS_PCNT, 0);

    for (nbox = REGION_NUM_RECTS(cclip), pbox = REGION_RECTS(cclip);
	 --nbox >= 0;
	 pbox++)
    {
	c1 = *((long *) &pbox->x1) - off;
	c2 = *((long *) &pbox->x2) - off - 0x00010001;
	for (ppt = (long *) pptInit, i = npt; --i >= 0;)
	{
	    pt = *ppt++;
	    if (!isClipped(pt,c1,c2)) {
		WaitQueue(3);
		outw(CUR_X, (short)(intToX(pt)+pDrawable->x));
		outw(CUR_Y, (short)(intToY(pt)+pDrawable->y));
		outw(CMD, CMD_LINE | DRAW | LINETYPE | PLANAR | WRTDATA);
	    }
	}
    }

    WaitQueue(2);
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
    outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
}
