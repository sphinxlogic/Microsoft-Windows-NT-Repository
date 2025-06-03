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

DIGITAL AND KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL DIGITAL OR KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL,
INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)

******************************************************************/
/* $XConsortium: cfbline.c,v 1.19 91/08/13 18:48:42 keith Exp $ */

/* s3dline.c from s3line.c with help from cfbresd.c and cfbline.c - Jon */
/* $XFree86: mit/server/ddx/x386/accel/s3/s3dline.c,v 2.11 1993/12/25 13:59:50 dawes Exp $ */

#include "X.h"

#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "scrnintstr.h"
#include "mistruct.h"

#include "cfb.h"
#include "cfbmskbits.h"
#include "misc.h"
#include "x386.h"
#include "s3.h"
#include "regs3.h"

/*
 * single-pixel lines on a color frame buffer
 * 
 * NON-SLOPED LINES horizontal lines are always drawn left to right; we have to
 * move the endpoints right by one after they're swapped. horizontal lines
 * will be confined to a single band of a region.  the code finds that band
 * (giving up if the lower bound of the band is above the line we're
 * drawing); then it finds the first box in that band that contains part of
 * the line.  we clip the line to subsequent boxes in that band. vertical
 * lines are always drawn top to bottom (y-increasing.) this requires adding
 * one to the y-coordinate of each endpoint after swapping.
 * 
 * SLOPED LINES when clipping a sloped line, we bring the second point inside
 * the clipping box, rather than one beyond it, and then add 1 to the length
 * of the line before drawing it.  this lets us use the same box for finding
 * the outcodes for both endpoints.  since the equation for clipping the
 * second endpoint to an edge gives us 1 beyond the edge, we then have to
 * move the point towards the first point by one step on the major axis.
 * eventually, there will be a diagram here to explain what's going on.  the
 * method uses Cohen-Sutherland outcodes to determine outsideness, and a
 * method similar to Pike's layers for doing the actual clipping.
 * 
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

#define NextDash {\
    dashIndexTmp++; \
    if (dashIndexTmp == numInDashList) \
        dashIndexTmp = 0; \
    dashRemaining = pDash[dashIndexTmp]; \
    thisDash = dashRemaining; \
    }

#define FillDashPat {\
   int i; \
\
   for (i = 0; i < 16; i++) {\
      dashPat <<= 1;\
      if (tmp + i < len) {\
	 if (!(dashIndexTmp & 1))\
	    dashPat |= 1;\
	 if (--thisDash == 0)\
	    NextDash\
      }\
   }\
}
/*
 * Dashed lines through the graphics engine.
 * Known Bugs: Jon 13/9/93
 * - Dash offset isn't caclulated correctly for clipped lines. [fixed?]
 * - Dash offset isn't updated correctly. [fixed?]
 * - Dash patters which are a power of 2 and < 16 can be done faster through
 *   the color compare register.
 * - DoubleDashed lines are are probably very incorrect.
 * - line caps are possible wrong too.
 * - Caclulating the dashes could probably be done more optimally,
 *   e.g. We could producing the pattern stipple before hand?
 */
void
s3Dline(pDrawable, pGC, mode, npt, pptInit)
     DrawablePtr pDrawable;
     GCPtr pGC;
     int   mode;		/* Origin or Previous */
     int   npt;			/* number of points */
     DDXPointPtr pptInit;
{
   int   nboxInit;
   register int nbox;
   BoxPtr pboxInit;
   register BoxPtr pbox;
   register DDXPointPtr ppt;	/* pointer to list of translated points */

   unsigned int oc1;		/* outcode of point 1 */
   unsigned int oc2;		/* outcode of point 2 */

   int   xorg, yorg;		/* origin of window */

   int   adx;			/* abs values of dx and dy */
   int   ady;
   int   signdx;		/* sign of dx and dy */
   int   signdy;
   int   e, e1, e2;		/* bresenham error and increments */
   int   len;			/* length of segment */
   int   axis;			/* major axis */
   short direction;
   unsigned char *pDash;
   int   dashOffset;   
   int numInDashList;
   int dashIndex;
   int dashIndexTmp, dashOffsetTmp, thisDash, dashRemaining;
   int unclippedlen;
   short dashPat;
 /* a bunch of temporaries */
   int   tmp;
   register int y1, y2;
   register int x1, x2;
   RegionPtr cclip;
   cfbPrivGCPtr devPriv;
   short fix;

   if (!x386VTSema)
   {
      cfbLineSD(pDrawable, pGC, mode, npt, pptInit);
      return;
   }

   devPriv = (cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr);
   cclip = devPriv->pCompositeClip;
   pboxInit = REGION_RECTS(cclip);
   nboxInit = REGION_NUM_RECTS(cclip);

   BLOCK_CURSOR;
   WaitQueue(6);
   S3_OUTW(FRGD_MIX, FSS_FRGDCOL | s3alu[pGC->alu]);
   if (pGC->lineStyle == LineDoubleDash) {
      S3_OUTW(BKGD_COLOR, (short)pGC->bgPixel);
      S3_OUTW(BKGD_MIX, BSS_BKGDCOL | s3alu[pGC->alu]);      
   } else
      S3_OUTW(BKGD_MIX, BSS_BKGDCOL | MIX_DST);
   S3_OUTW(WRT_MASK, (short)pGC->planemask);
   S3_OUTW(FRGD_COLOR, (short)pGC->fgPixel);
   S3_OUTW (MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_EXPPC | COLCMPOP_F);
   
   xorg = pDrawable->x;
   yorg = pDrawable->y;
   ppt = pptInit;

   x2 = ppt->x + xorg;
   y2 = ppt->y + yorg;

   pDash = (unsigned char *) pGC->dash;
   numInDashList = pGC->numInDashList;
    
   dashIndex = 0;
   dashOffset = 0;
   miStepDash ((int)pGC->dashOffset, &dashIndex, pDash,
                numInDashList, &dashOffset);

   dashRemaining = pDash[dashIndex] - dashOffset;
   thisDash = dashRemaining ;
		  
   
   while (--npt) {
      nbox = nboxInit;
      pbox = pboxInit;

      x1 = x2;
      y1 = y2;
      ++ppt;
      if (mode == CoordModePrevious) {
	 xorg = x1;
	 yorg = y1;
      }
      x2 = ppt->x + xorg;
      y2 = ppt->y + yorg;

#ifdef fastaxislinesfixed
      if (x1 == x2) {

	 if (y1 > y2) {
	    unclippedlen = y1 = y2;
	 } else {
	    direction |= INC_X;
	    unclippedlen = y2 - y1;	    
	 }
	 
       /* get to first band that might contain part of line */
	 while ((nbox) && (pbox->y2 <= y1)) {
	    pbox++;
	    nbox--;
	 }

	 if (nbox) {
	  /* stop when lower edge of box is beyond end of line */
	    while ((nbox) && (y2 >= pbox->y1)) {
	       if ((x1 >= pbox->x1) && (x1 < pbox->x2)) {
		  int   y1t, y2t;


		/* this box has part of the line in it */
		  y1t = max(y1, pbox->y1);
		  y2t = min(y2, pbox->y2);
		  if (y1t != y2t) {
		     /* use tmp dash index and offsets */
		     dashIndexTmp = dashIndex;    
		     dashOffsetTmp = dashOffset; 

		     if (y1t != y1) { /* advance the dash index */
			miStepDash (y1t - y1, &dashIndexTmp, pDash,
				 numInDashList, &dashOffsetTmp);
		     }
		     dashRemaining = pDash[dashIndexTmp] - dashOffsetTmp;
		     thisDash = dashRemaining ;
		     
		     WaitQueue(4);
		     S3_OUTW(CUR_X, (short)x1);
		     S3_OUTW(CUR_Y, (short)y1t);
		     len = y2t - y1t;
		     S3_OUTW(MAJ_AXIS_PCNT, (short)(len - 1));
		     S3_OUTW(CMD, CMD_LINE | DRAW | LINETYPE | PLANAR |
			   PCDATA | _16BIT |WRTDATA | (6 << 5));

		     for (tmp = 0 ; tmp < len; tmp+=16) {
			FillDashPat;			
			S3_OUTW(PIX_TRANS, dashPat);
		     }		  
		  }
	       }
	       nbox--;
	       pbox++;
	    }
	 }
	 y2 = ppt->y + yorg;
      } else if (y1 == y2) {

       /*
        * force line from left to right, keeping endpoint semantics
        */
	 if (x1 > x2) {
	    register int tmp;

	    tmp = x2;
	    x2 = x1 + 1;
	    x1 = tmp + 1;
	 }
	 unclippedlen = x2 - x1;
       /* find the correct band */
	 while ((nbox) && (pbox->y2 <= y1)) {
	    pbox++;
	    nbox--;
	 }

       /* try to draw the line, if we haven't gone beyond it */
	 if ((nbox) && (pbox->y1 <= y1)) {
	  /* when we leave this band, we're done */
	    tmp = pbox->y1;
	    while ((nbox) && (pbox->y1 == tmp)) {
	       int   x1t, x2t;

	       if (pbox->x2 <= x1) {
		/* skip boxes until one might contain start point */
		  nbox--;
		  pbox++;
		  continue;
	       }
	     /* stop if left of box is beyond right of line */
	       if (pbox->x1 >= x2) {
		  nbox = 0;
		  break;
	       }
	       x1t = max(x1, pbox->x1);
	       x2t = min(x2, pbox->x2);	       
	       if (x1t != x2t) {
		  dashIndexTmp = dashIndex;    
		  dashOffsetTmp = dashOffset; 

		  if (x1t != x1) { /* advance the dash index */
		     miStepDash (x1t - x1, &dashIndexTmp, pDash,
				 numInDashList, &dashOffsetTmp);
		  }
		  dashRemaining = pDash[dashIndexTmp] - dashOffsetTmp;
		  thisDash = dashRemaining ;
		  
		  WaitQueue(4);
		  S3_OUTW(CUR_X, (short)x1t);
		  S3_OUTW(CUR_Y, (short)y1);
		  len = x2t - x1t;
		  S3_OUTW(MAJ_AXIS_PCNT, (short)(len - 1));
		  S3_OUTW(CMD, CMD_LINE | DRAW | LINETYPE | PLANAR |
			PCDATA | _16BIT | WRTDATA);
		  for (tmp = 0 ; tmp < len; tmp+=16) {
		     FillDashPat;
		     S3_OUTW(PIX_TRANS, dashPat);
		  }		 
	       }
	       nbox--;
	       pbox++;
	    }
	 }
	 x2 = ppt->x + xorg;
      }
      else
#endif
      {			/* sloped line */
	 direction = 0x0000;
	 signdx = 1;
	 if ((adx = x2 - x1) < 0) {
	    adx = -adx;
	    signdx = -1;
	    fix = 0;
	 } else {
	    direction |= INC_X;
	    fix = -1;
	 }
	 signdy = 1;
	 if ((ady = y2 - y1) < 0) {
	    ady = -ady;
	    signdy = -1;
	 } else {
	    direction |= INC_Y;
	 }

	 if (adx > ady) {
	    axis = X_AXIS;
	    e1 = ady << 1;
	    e2 = e1 - (adx << 1);
	    e = e1 - adx;
	    unclippedlen = adx;
	 } else {
	    axis = Y_AXIS;
	    e1 = adx << 1;
	    e2 = e1 - (ady << 1);
	    e = e1 - ady;
	    direction |= YMAJAXIS;
	    unclippedlen = ady;	    
	 }

       /*
        * we have bresenham parameters and two points. all we have to do now
        * is clip and draw.
        */

	 while (nbox--) {
	    oc1 = 0;
	    oc2 = 0;
	    OUTCODES(oc1, x1, y1, pbox);
	    OUTCODES(oc2, x2, y2, pbox);
	    if ((oc1 | oc2) == 0) {
	       if (axis == X_AXIS) 
		  len = adx;
	       else 
		  len = ady;
	     dashIndexTmp = dashIndex;    
	     dashOffsetTmp = dashOffset;
	     /* No need to adjust dash offset */
	     /*
	      * NOTE:  The 8514/A hardware routines for generating lines do
	      * not match the software generated lines of mi, cfb, and mfb.
	      * This is a problem, and if I ever get time, I'll figure out
	      * the 8514/A algorithm and implement it in software for mi,
	      * cfb, and mfb.
	      * 2-sep-93 TCG: apparently only change needed is
	      * addition of 'fix' stuff in cfbline.c
	      */
	       WaitQueue(7);
	       S3_OUTW(CUR_X, (short)x1);
	       S3_OUTW(CUR_Y, (short)y1);
	       S3_OUTW(ERR_TERM, (short)(e + fix));
	       S3_OUTW(DESTY_AXSTP, (short)e1);
	       S3_OUTW(DESTX_DIASTP, (short)e2);
	       S3_OUTW(MAJ_AXIS_PCNT, (short)len);
	       S3_OUTW(CMD, CMD_LINE | DRAW | LASTPIX | PLANAR | direction |
		       PCDATA | _16BIT | WRTDATA);
	       for (tmp = 0 ; tmp < len; tmp+=16) {
		  FillDashPat;
		  S3_OUTW(PIX_TRANS, dashPat);
	       }
	       break;
	    } else if (oc1 & oc2) {
	       pbox++;
	    } else {

	     /*
	      * let the mfb helper routine do our work; better than
	      * duplicating code...
	      */
	       BoxRec box;
	       DDXPointRec pt1Copy;	/* clipped start point */
	       DDXPointRec pt2Copy;	/* clipped end point */
	       int   err;	/* modified bresenham error term */
	       int   clip1, clip2;	/* clippedness of the endpoints */

	       int   clipdx, clipdy;	/* difference between clipped and
					 * unclipped start point */
	       DDXPointRec pt1;
	       int dlen;

	       pt1.x = pt1Copy.x = x1;
	       pt1.y = pt1Copy.y = y1;
	       pt2Copy.x = x2;
	       pt2Copy.y = y2;
	       box.x1 = pbox->x1;
	       box.y1 = pbox->y1;
	       box.x2 = pbox->x2 - 1;
	       box.y2 = pbox->y2 - 1;
	       clip1 = 0;
	       clip2 = 0;

	       if (mfbClipLine(pbox, box,
			       &pt1, &pt1Copy, &pt2Copy,
			       adx, ady, signdx, signdy, axis,
			       &clip1, &clip2) == 1) {
		  dashIndexTmp = dashIndex;
		  dashOffsetTmp = dashOffset; 
			       
		  if (axis == X_AXIS) {
		     dlen = abs(pt1Copy.x - x1);
		     len = abs(pt2Copy.x - pt1Copy.x);
		  } else {
		     dlen = abs(pt1Copy.y - y1);
		     len = abs(pt2Copy.y - pt1Copy.y);
		  }

		  len += (clip2 != 0);
		  if (len) {
		   /* unwind bresenham error term to first point */
		     if (clip1) {
			clipdx = abs(pt1Copy.x - x1);
			clipdy = abs(pt1Copy.y - y1);
			if (axis == X_AXIS)
			   err = e + ((clipdy * e2) + ((clipdx - clipdy) * e1));
			else
			   err = e + ((clipdx * e2) + ((clipdy - clipdx) * e1));
		     } else
			err = e;

		     /*
		      * Here is a problem, the unwound error terms could be
		      * upto 16bit now. The poor S3 is only 12 or 13 bit.
		      * The rounding error is probably small I favor scaling
		      * the error terms, although re-evaluation is also an
		      * option I think it might give visable errors
		      * - Jon 12/9/93.
		      */
		      
		     if (abs(err) > 4096  || abs(e1) > 4096 || abs(e2) > 4096) {
#if 1
			int div;

			if (abs(err) > abs(e1))
			    div = (abs(err) > abs(e2)) ?
			    (abs(err) + 4095)/ 4096 : (abs(e2) + 4095)/ 4096;
			else
			    div = (abs(e1) > abs(e2)) ?
			    (abs(e1) + 4095)/ 4096 : (abs(e2) + 4095)/ 4096;

			err /= div;
			e1 /= div;
			e2 /= div;
#else
			int minor;
			if (axis == X_AXIS) {			
			   minor = abs(pt2Copy.y - pt1Copy.y);
			   err = 2 * minor - len;			   
			} else {
			   minor = abs(pt2Copy.x - pt1Copy.x);
			   err = 2 * minor - len - 1;	   
			}
			e1 = minor << 1;
			e2 = e1 - (len << 1);
#endif
		     }
		     miStepDash (dlen, &dashIndexTmp, pDash,
				 numInDashList, &dashOffsetTmp);
		     WaitQueue(7);
		     S3_OUTW(CUR_X, (short)pt1Copy.x);
		     S3_OUTW(CUR_Y, (short)pt1Copy.y);
		     S3_OUTW(ERR_TERM, (short)(err + fix));
		     S3_OUTW(DESTY_AXSTP, (short)e1);
		     S3_OUTW(DESTX_DIASTP, (short)e2);
		     S3_OUTW(MAJ_AXIS_PCNT, (short)len);
		     S3_OUTW(CMD, CMD_LINE | DRAW | LASTPIX | PLANAR |
			     direction | PCDATA | _16BIT | WRTDATA);

		     for (tmp = 0 ; tmp < len; tmp+=16) {
			FillDashPat;
			S3_OUTW(PIX_TRANS, dashPat);
		     }
		  }
	       }
	       pbox++;
	    }
	 }			/* while (nbox--) */
      }/* sloped line */
      miStepDash (unclippedlen, &dashIndex, pDash,
		  numInDashList, &dashOffset);
   } /* while (nline--) */

 /*
  * paint the last point if the end style isn't CapNotLast. (Assume that a
  * projecting, butt, or round cap that is one pixel wide is the same as the
  * single pixel of the endpoint.)
  */

   if ((pGC->capStyle != CapNotLast) &&
       ((ppt->x + xorg != pptInit->x + pDrawable->x) ||
	(ppt->y + yorg != pptInit->y + pDrawable->y) ||
	(ppt == pptInit + 1))) {
      nbox = nboxInit;
      pbox = pboxInit;
      while (nbox--) {
	 if ((x2 >= pbox->x1) &&
	     (y2 >= pbox->y1) &&
	     (x2 < pbox->x2) &&
	     (y2 < pbox->y2)) {
	    WaitQueue(4);
	    S3_OUTW(CUR_X, (short)x2);
	    S3_OUTW(CUR_Y, (short)y2);
	    S3_OUTW(MAJ_AXIS_PCNT, 0);
	    S3_OUTW(CMD, CMD_LINE | DRAW | LINETYPE | PLANAR | WRTDATA);

	    break;
	 } else
	    pbox++;
      }
   }
   WaitQueue(3);
   S3_OUTW(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
   S3_OUTW(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
   S3_OUTW (MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_FRGDMIX | COLCMPOP_F);  
   UNBLOCK_CURSOR;
}
