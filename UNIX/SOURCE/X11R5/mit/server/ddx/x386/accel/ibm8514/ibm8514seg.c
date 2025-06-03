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

DIGITAL, KEVIN E. MARTIN, AND TIAGO GONS DISCLAIM ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL DIGITAL, KEVIN E. MARTIN, OR TIAGO GONS BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
Further modifications by Tiago Gons (tiago@comosjn.hobby.nl)

******************************************************************/

/* $XFree86: mit/server/ddx/x386/accel/ibm8514/ibm8514seg.c,v 2.5 1993/12/25 13:57:34 dawes Exp $ */

/* $XConsortium: cfbline.c,v 1.19 91/08/13 18:48:42 keith Exp $ */

#include "X.h"

#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "scrnintstr.h"
#include "mistruct.h"

#include "cfb.h"
#include "cfbmskbits.h"
#include "reg8514.h"
#include "ibm8514.h"

/* single-pixel lines on a color frame buffer

   NON-SLOPED LINES
   horizontal lines are always drawn left to right; we have to
move the endpoints right by one after they're swapped.
   horizontal lines will be confined to a single band of a
region.  the code finds that band (giving up if the lower
bound of the band is above the line we're drawing); then it
finds the first box in that band that contains part of the
line.  we clip the line to subsequent boxes in that band.
   vertical lines are always drawn top to bottom (y-increasing.)
this requires adding one to the y-coordinate of each endpoint
after swapping.

   SLOPED LINES
   when clipping a sloped line, we bring the second point inside
the clipping box, rather than one beyond it, and then add 1 to
the length of the line before drawing it.  this lets us use
the same box for finding the outcodes for both endpoints.  since
the equation for clipping the second endpoint to an edge gives us
1 beyond the edge, we then have to move the point towards the
first point by one step on the major axis.
   eventually, there will be a diagram here to explain what's going
on.  the method uses Cohen-Sutherland outcodes to determine
outsideness, and a method similar to Pike's layers for doing the
actual clipping.

*/

#define OUTCODES(result, x, y, pbox) \
    if (x < pbox->x1) \
	result |= OUT_LEFT; \
    else if (x >= pbox->x2) \
	result |= OUT_RIGHT; \
    if (y < pbox->y1) \
	result |= OUT_ABOVE; \
    else if (y >= pbox->y2) \
	result |= OUT_BELOW;

void
ibm8514Segment(pDrawable, pGC, nseg, pSeg)
    DrawablePtr	pDrawable;
    GCPtr	pGC;
    int		nseg;
    register xSegment	*pSeg;
{
    int nboxInit;
    register int nbox;
    BoxPtr pboxInit;
    register BoxPtr pbox;

    unsigned int oc1;		/* outcode of point 1 */
    unsigned int oc2;		/* outcode of point 2 */

    int xorg, yorg;		/* origin of window */

    int adx;		/* abs values of dx and dy */
    int ady;
    int signdx;		/* sign of dx and dy */
    int signdy;
    int e, e1, e2;		/* bresenham error and increments */
    int len;			/* length of segment */
    int axis;			/* major axis */
    short cmd = CMD_LINE | DRAW | PLANAR | WRTDATA | LASTPIX;
    short cmd2;
    short fix;

				/* a bunch of temporaries */
    int tmp;
    register int y1, y2;
    register int x1, x2;
    RegionPtr cclip;
    cfbPrivGCPtr    devPriv;

/* 4-5-93 TCG : is VT visible */
    if (!x386VTSema)
    {
        cfbSegmentSS(pDrawable, pGC, nseg, pSeg);
	return;
    }

    devPriv = (cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr); 
    cclip = devPriv->pCompositeClip;
    pboxInit = REGION_RECTS(cclip);
    nboxInit = REGION_NUM_RECTS(cclip);

    WaitQueue(3);
    outw(FRGD_MIX, FSS_FRGDCOL | ibm8514alu[pGC->alu]);
    outw(WRT_MASK, (short)pGC->planemask);
    outw(FRGD_COLOR, (short)pGC->fgPixel);

    xorg = pDrawable->x;
    yorg = pDrawable->y;

    while (nseg--)
    {
	nbox = nboxInit;
	pbox = pboxInit;

	x1 = pSeg->x1 + xorg;
	y1 = pSeg->y1 + yorg;
	x2 = pSeg->x2 + xorg;
	y2 = pSeg->y2 + yorg;
	pSeg++;

	if (x1 == x2)
	{
	    /* make the line go top to bottom of screen, keeping
	       endpoint semantics
	    */
	    if (y1 > y2)
	    {
		register int tmp;

		tmp = y2;
		y2 = y1 + 1;
		y1 = tmp + 1;
		if (pGC->capStyle != CapNotLast)
		    y1--;
	    }
	    else if (pGC->capStyle != CapNotLast)
		y2++;
	    /* get to first band that might contain part of line */
	    while ((nbox) && (pbox->y2 <= y1))
	    {
		pbox++;
		nbox--;
	    }

	    if (nbox)
	    {
		/* stop when lower edge of box is beyond end of line */
		while((nbox) && (y2 >= pbox->y1))
		{
		    if ((x1 >= pbox->x1) && (x1 < pbox->x2))
		    {
			int y1t, y2t;
			/* this box has part of the line in it */
			y1t = max(y1, pbox->y1);
			y2t = min(y2, pbox->y2);
			if (y1t != y2t)
			{
			    WaitQueue(4);
			    outw(CUR_X, (short)x1);
			    outw(CUR_Y, (short)y1t);
			    outw(MAJ_AXIS_PCNT, (short)(y2t-y1t-1));
			    outw(CMD, CMD_LINE | DRAW | LINETYPE | PLANAR |
				  WRTDATA | (6 << 5));
			}
		    }
		    nbox--;
		    pbox++;
		}
	    }
	}
	else if (y1 == y2)
	{
	    /* force line from left to right, keeping
	       endpoint semantics
	    */
	    if (x1 > x2)
	    {
		register int tmp;

		tmp = x2;
		x2 = x1 + 1;
		x1 = tmp + 1;
		if (pGC->capStyle != CapNotLast)
		    x1--;
	    }
	    else if (pGC->capStyle != CapNotLast)
		x2++;

	    /* find the correct band */
	    while( (nbox) && (pbox->y2 <= y1))
	    {
		pbox++;
		nbox--;
	    }

	    /* try to draw the line, if we haven't gone beyond it */
	    if ((nbox) && (pbox->y1 <= y1))
	    {
		/* when we leave this band, we're done */
		tmp = pbox->y1;
		while((nbox) && (pbox->y1 == tmp))
		{
		    int	x1t, x2t;

		    if (pbox->x2 <= x1)
		    {
			/* skip boxes until one might contain start point */
			nbox--;
			pbox++;
			continue;
		    }

		    /* stop if left of box is beyond right of line */
		    if (pbox->x1 >= x2)
		    {
			nbox = 0;
			break;
		    }

		    x1t = max(x1, pbox->x1);
		    x2t = min(x2, pbox->x2);
		    if (x1t != x2t)
		    {
			WaitQueue(4);
			outw(CUR_X, (short)x1t);
			outw(CUR_Y, (short)y1);
			outw(MAJ_AXIS_PCNT, (short)(x2t-x1t-1));
			outw(CMD, CMD_LINE | DRAW | LINETYPE | PLANAR |
			      WRTDATA);
		    }
		    nbox--;
		    pbox++;
		}
	    }
	}
	else	/* sloped line */
	{
	    cmd2 = cmd;
	    signdx = 1;
	    if ((adx = x2 - x1) < 0)
	    {
		adx = -adx;
		signdx = -1;
		fix = 0;
	    }
	    else
	    {
		cmd2 |= INC_X;
		fix = -1;
	    }
	    signdy = 1;
	    if ((ady = y2 - y1) < 0)
	    {
		ady = -ady;
		signdy = -1;
	    }
	    else
	    {
		cmd2 |= INC_Y;
	    }

	    if (adx > ady)
	    {
		axis = X_AXIS;
		e1 = ady << 1;
		e2 = e1 - (adx << 1);
		e = e1 - adx;

	    }
	    else
	    {
		axis = Y_AXIS;
		e1 = adx << 1;
		e2 = e1 - (ady << 1);
		e = e1 - ady;
		cmd2 |= YMAJAXIS;
	    }

	    /* we have bresenham parameters and two points.
	       all we have to do now is clip and draw.
	    */

	    while(nbox--)
	    {
		oc1 = 0;
		oc2 = 0;
		OUTCODES(oc1, x1, y1, pbox);
		OUTCODES(oc2, x2, y2, pbox);
		if ((oc1 | oc2) == 0)
		{
		    if (axis == X_AXIS)
			len = adx;
		    else
			len = ady;
		    if (pGC->capStyle != CapNotLast)
			len++;

		    WaitQueue(7);
		    outw(CUR_X, (short)x1);
		    outw(CUR_Y, (short)y1);
		    outw(ERR_TERM, (short)(e + fix));
		    outw(DESTY_AXSTP, (short)e1);
		    outw(DESTX_DIASTP, (short)e2);
		    outw(MAJ_AXIS_PCNT, (short)len);
		    outw(CMD, cmd2);

		    break;
		}
		else if (oc1 & oc2)
		{
		    pbox++;
		}
		else
		{
	    	    /*
	     	     * let the mfb helper routine do our work;
	     	     * better than duplicating code...
	     	     */
	    	    BoxRec box;
    	    	    DDXPointRec pt1Copy;	/* clipped start point */
    	    	    DDXPointRec pt2Copy;	/* clipped end point */
    	    	    int err;			/* modified bresenham error term */
    	    	    int clip1, clip2;		/* clippedness of the endpoints */
    	    	
    	    	    int clipdx, clipdy;		/* difference between clipped and
				       	       	   unclipped start point */
		    DDXPointRec	pt1;
    	    	
    	
	    	    pt1.x = pt1Copy.x = x1;
		    pt1.y = pt1Copy.y = y1;
	    	    pt2Copy.x = x2;
		    pt2Copy.y = y2;
	    	    box.x1 = pbox->x1;
	    	    box.y1 = pbox->y1;
	    	    box.x2 = pbox->x2-1;
	    	    box.y2 = pbox->y2-1;
	    	    clip1 = 0;
	    	    clip2 = 0;
    	
		    if (mfbClipLine (pbox, box,
				     &pt1, &pt1Copy, &pt2Copy, 
				     adx, ady, signdx, signdy, axis,
				     &clip1, &clip2) == 1)
		    {
		    	if (axis == X_AXIS)
			    len = abs(pt2Copy.x - pt1Copy.x);
		    	else
			    len = abs(pt2Copy.y - pt1Copy.y);
    
		    	if (clip2 != 0 || pGC->capStyle != CapNotLast)
			    len++;
		    	if (len)
		    	{
			    /* unwind bresenham error term to first point */
			    if (clip1)
			    {
			    	clipdx = abs(pt1Copy.x - x1);
			    	clipdy = abs(pt1Copy.y - y1);
			    	if (axis == X_AXIS)
				    err = e+((clipdy*e2) + ((clipdx-clipdy)*e1));
			    	else
				    err = e+((clipdx*e2) + ((clipdy-clipdx)*e1));
			    }
			    else
			    	err = e;

			    WaitQueue(7);
			    outw(CUR_X, (short)pt1Copy.x);
			    outw(CUR_Y, (short)pt1Copy.y);
			    outw(ERR_TERM, (short)(err + fix));
			    outw(DESTY_AXSTP, (short)e1);
			    outw(DESTX_DIASTP, (short)e2);
			    outw(MAJ_AXIS_PCNT, (short)len);
			    outw(CMD, cmd2);
		    	}
		    }
		    pbox++;
		}
	    } /* while (nbox--) */
	} /* sloped line */
    } /* while (nline--) */

    WaitQueue(2);
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
    outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
}
