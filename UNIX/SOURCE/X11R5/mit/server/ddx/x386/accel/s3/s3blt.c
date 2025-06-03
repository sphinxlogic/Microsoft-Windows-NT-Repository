/*
 * Copyright 1989 by the Massachusetts Institute of Technology
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 * 
 * Author: Keith Packard
 * 
 * Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * 
 * KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 * 
 * $XConsortium: cfbbitblt.c,v 5.39 91/05/24 16:33:25 keith Exp $
 */

/*
 * Modified by Amancio Hasty and Jon Tombs
 * 
 * Id: s3blt.c,v 2.2 1993/06/22 20:54:09 jon Exp jon
 */

/* $XFree86: mit/server/ddx/x386/accel/s3/s3blt.c,v 2.15 1994/02/25 14:58:44 dawes Exp $ */

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

#include	"misc.h"
#include	"x386.h"
#include	"s3.h"
#include	"regs3.h"
#include        "s3linear.h"

extern int s3MAX_SLOTS;
void  s3FindOrdering();
extern RegionPtr cfbBitBlt();

RegionPtr
s3CopyArea(pSrcDrawable, pDstDrawable,
	   pGC, srcx, srcy, width, height, dstx, dsty)
     register DrawablePtr pSrcDrawable;
     register DrawablePtr pDstDrawable;
     GC   *pGC;
     int   srcx, srcy;
     int   width, height;
     int   dstx, dsty;
{
   RegionPtr prgnSrcClip;	/* may be a new region, or just a copy */
   Bool  freeSrcClip = FALSE;

   RegionPtr prgnExposed;
   RegionRec rgnDst;
   register BoxPtr pbox;
   int   i;
   register int dx;
   register int dy;
   xRectangle origSource;
   DDXPointRec origDest;
   int   numRects;
   BoxRec fastBox;
   int   fastClip = 0;		/* for fast clipping with pixmap source */
   int   fastExpose = 0;	/* for fast exposures with pixmap source */

   if (!x386VTSema || 
       (pSrcDrawable->type != DRAWABLE_WINDOW &&
                      pDstDrawable->type != DRAWABLE_WINDOW))
      return cfbCopyArea(pSrcDrawable, pDstDrawable, pGC,
                          srcx, srcy, width, height, dstx, dsty);

   origSource.x = srcx;
   origSource.y = srcy;
   origSource.width = width;
   origSource.height = height;
   origDest.x = dstx;
   origDest.y = dsty;

   if ((pSrcDrawable != pDstDrawable) &&
       pSrcDrawable->pScreen->SourceValidate) {
      (*pSrcDrawable->pScreen->SourceValidate) (pSrcDrawable, srcx, srcy, width, height);
   }
   srcx += pSrcDrawable->x;
   srcy += pSrcDrawable->y;

 /* clip the source */

   if (pSrcDrawable->type == DRAWABLE_PIXMAP) {
      if ((pSrcDrawable == pDstDrawable) &&
	  (pGC->clientClipType == CT_NONE)) {
	 prgnSrcClip = ((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;
      } else {
	 fastClip = 1;
      }
   } else {
      if (pGC->subWindowMode == IncludeInferiors) {
	 if (!((WindowPtr) pSrcDrawable)->parent) {

	  /*
	   * special case bitblt from root window in IncludeInferiors mode;
	   * just like from a pixmap
	   */
	    fastClip = 1;
	 } else if ((pSrcDrawable == pDstDrawable) &&
		    (pGC->clientClipType == CT_NONE)) {
	    prgnSrcClip = ((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;
	 } else {
	    prgnSrcClip = NotClippedByChildren((WindowPtr) pSrcDrawable);
	    freeSrcClip = TRUE;
	 }
      } else {
	 prgnSrcClip = &((WindowPtr) pSrcDrawable)->clipList;
      }
   }

   fastBox.x1 = srcx;
   fastBox.y1 = srcy;
   fastBox.x2 = srcx + width;
   fastBox.y2 = srcy + height;

 /* Don't create a source region if we are doing a fast clip */
   if (fastClip) {

      fastExpose = 1;

    /*
     * clip the source; if regions extend beyond the source size, make sure
     * exposure events get sent
     */
      if (fastBox.x1 < pSrcDrawable->x) {
	 fastBox.x1 = pSrcDrawable->x;
	 fastExpose = 0;
      }
      if (fastBox.y1 < pSrcDrawable->y) {
	 fastBox.y1 = pSrcDrawable->y;
	 fastExpose = 0;
      }
      if (fastBox.x2 > pSrcDrawable->x + (int)pSrcDrawable->width) {
	 fastBox.x2 = pSrcDrawable->x + (int)pSrcDrawable->width;
	 fastExpose = 0;
      }
      if (fastBox.y2 > pSrcDrawable->y + (int)pSrcDrawable->height) {
	 fastBox.y2 = pSrcDrawable->y + (int)pSrcDrawable->height;
	 fastExpose = 0;
      }
   } else {
      (*pGC->pScreen->RegionInit) (&rgnDst, &fastBox, 1);
      (*pGC->pScreen->Intersect) (&rgnDst, &rgnDst, prgnSrcClip);
   }

   dstx += pDstDrawable->x;
   dsty += pDstDrawable->y;

   if (pDstDrawable->type == DRAWABLE_WINDOW) {
      if (!((WindowPtr) pDstDrawable)->realized) {
	 if (!fastClip)
	    (*pGC->pScreen->RegionUninit) (&rgnDst);
	 if (freeSrcClip)
	    (*pGC->pScreen->RegionDestroy) (prgnSrcClip);
	 return NULL;
      }
   }
   dx = srcx - dstx;
   dy = srcy - dsty;

 /* Translate and clip the dst to the destination composite clip */
   if (fastClip) {
      RegionPtr cclip;

    /* Translate the region directly */
      fastBox.x1 -= dx;
      fastBox.x2 -= dx;
      fastBox.y1 -= dy;
      fastBox.y2 -= dy;

    /*
     * If the destination composite clip is one rectangle we can do the clip
     * directly.  Otherwise we have to create a full blown region and call
     * intersect
     */

    /*
     * XXX because CopyPlane uses this routine for 8-to-1 bit copies, this
     * next line *must* also correctly fetch the composite clip from an mfb
     * gc
     */

      cclip = ((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;
      if (REGION_NUM_RECTS(cclip) == 1) {
	 BoxPtr pBox = REGION_RECTS(cclip);

	 if (fastBox.x1 < pBox->x1)
	    fastBox.x1 = pBox->x1;
	 if (fastBox.x2 > pBox->x2)
	    fastBox.x2 = pBox->x2;
	 if (fastBox.y1 < pBox->y1)
	    fastBox.y1 = pBox->y1;
	 if (fastBox.y2 > pBox->y2)
	    fastBox.y2 = pBox->y2;

       /* Check to see if the region is empty */
	 if (fastBox.x1 >= fastBox.x2 || fastBox.y1 >= fastBox.y2)
	    (*pGC->pScreen->RegionInit) (&rgnDst, NullBox, 0);
	 else
	    (*pGC->pScreen->RegionInit) (&rgnDst, &fastBox, 1);
      } else {

       /*
        * We must turn off fastClip now, since we must create a full blown
        * region.  It is intersected with the composite clip below.
        */
	 fastClip = 0;
	 (*pGC->pScreen->RegionInit) (&rgnDst, &fastBox, 1);
      }
   } else {
      (*pGC->pScreen->TranslateRegion) (&rgnDst, -dx, -dy);
   }

   if (!fastClip) {
      (*pGC->pScreen->Intersect) (&rgnDst,
				  &rgnDst,
				  ((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);
   }
 /* Do bit blitting */
   numRects = REGION_NUM_RECTS(&rgnDst);
   if (numRects && width && height) {
      pbox = REGION_RECTS(&rgnDst);

      if (pSrcDrawable->type == DRAWABLE_WINDOW && pDstDrawable->type == DRAWABLE_WINDOW) {
       /* Window --> Window */
	 unsigned int *ordering;
	 BoxPtr prect;
	 short direction = 0;

	 ordering = (unsigned int *)ALLOCATE_LOCAL(numRects * sizeof(unsigned int));

	 if (!ordering) {
	    DEALLOCATE_LOCAL(ordering);
	    return (RegionPtr) NULL;
	 }
	 s3FindOrdering(pSrcDrawable, pDstDrawable, pGC, numRects, pbox, srcx,
			srcy, dstx, dsty, ordering);

	 if (dx > 0)
	    direction |= INC_X;
	 if (dy > 0)
	    direction |= INC_Y;

	 BLOCK_CURSOR;
	 WaitQueue(3);
	 S3_OUTW(FRGD_MIX, FSS_BITBLT | s3alu[pGC->alu]);
	 S3_OUTW(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
	 S3_OUTW(WRT_MASK, pGC->planemask);

	 if (direction == (INC_X | INC_Y)) {
	    for (i = 0; i < numRects; i++) {
	       prect = &pbox[ordering[i]];

	       WaitQueue(7);
	       S3_OUTW(CUR_X, (short)(prect->x1 + dx));
	       S3_OUTW(CUR_Y, (short)(prect->y1 + dy));
	       S3_OUTW(DESTX_DIASTP, (short)(prect->x1));
	       S3_OUTW(DESTY_AXSTP, (short)(prect->y1));
	       S3_OUTW(MAJ_AXIS_PCNT, (short)(prect->x2 - prect->x1 - 1));
	       S3_OUTW(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(prect->y2 - prect->y1 - 1));
	       S3_OUTW(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
	    }
	 } else if (direction == INC_X) {
	    for (i = 0; i < numRects; i++) {
	       prect = &pbox[ordering[i]];

	       WaitQueue(7);
	       S3_OUTW(CUR_X, (short)(prect->x1 + dx));
	       S3_OUTW(CUR_Y, (short)(prect->y2 + dy - 1));
	       S3_OUTW(DESTX_DIASTP, (short)(prect->x1));
	       S3_OUTW(DESTY_AXSTP, (short)(prect->y2 - 1));
	       S3_OUTW(MAJ_AXIS_PCNT, (short)(prect->x2 - prect->x1 - 1));
	       S3_OUTW(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(prect->y2 - prect->y1 - 1));
	       S3_OUTW(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
	    }
	 } else if (direction == INC_Y) {
	    for (i = 0; i < numRects; i++) {
	       prect = &pbox[ordering[i]];

	       WaitQueue(7);
	       S3_OUTW(CUR_X, (short)(prect->x2 + dx - 1));
	       S3_OUTW(CUR_Y, (short)(prect->y1 + dy));
	       S3_OUTW(DESTX_DIASTP, (short)(prect->x2 - 1));
	       S3_OUTW(DESTY_AXSTP, (short)(prect->y1));
	       S3_OUTW(MAJ_AXIS_PCNT, (short)(prect->x2 - prect->x1 - 1));
	       S3_OUTW(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(prect->y2 - prect->y1 - 1));
	       S3_OUTW(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
	    }
	 } else {
	    for (i = 0; i < numRects; i++) {
	       prect = &pbox[ordering[i]];

	       WaitQueue(7);
	       S3_OUTW(CUR_X, (short)(prect->x2 + dx - 1));
	       S3_OUTW(CUR_Y, (short)(prect->y2 + dy - 1));
	       S3_OUTW(DESTX_DIASTP, (short)(prect->x2 - 1));
	       S3_OUTW(DESTY_AXSTP, (short)(prect->y2 - 1));
	       S3_OUTW(MAJ_AXIS_PCNT, (short)(prect->x2 - prect->x1 - 1));
	       S3_OUTW(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(prect->y2 - prect->y1 - 1));
	       S3_OUTW(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
	    }
	 }

	 WaitQueue(3);
	 S3_OUTW(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
	 S3_OUTW(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
	 S3_OUTW(WRT_MASK, 0xffff);
	 UNBLOCK_CURSOR;
	 DEALLOCATE_LOCAL(ordering);
      } else if (pSrcDrawable->type == DRAWABLE_WINDOW &&
		 pDstDrawable->type != DRAWABLE_WINDOW) {
       /* Window --> Pixmap */
	 int   pixWidth = PixmapBytePad(pDstDrawable->width, pDstDrawable->depth);
	 unsigned char *pdst = ((PixmapPtr) pDstDrawable)->devPrivate.ptr;

	 for (i = numRects; --i >= 0; pbox++)
	    (s3ImageReadFunc) (pbox->x1 + dx, pbox->y1 + dy,
			       pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
			       pdst, pixWidth, pbox->x1, pbox->y1,
			       pGC->planemask);
      } else if (pSrcDrawable->type != DRAWABLE_WINDOW &&
		 pDstDrawable->type == DRAWABLE_WINDOW) {
       /* Pixmap --> Window */
	 int   pixWidth = PixmapBytePad(pSrcDrawable->width, pSrcDrawable->depth);
	 unsigned char *psrc = ((PixmapPtr) pSrcDrawable)->devPrivate.ptr;

	 for (i = numRects; --i >= 0; pbox++)
	    (s3ImageWriteFunc) (pbox->x1, pbox->y1,
				pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
				psrc, pixWidth, pbox->x1 + dx, pbox->y1 + dy,
				s3alu[pGC->alu], pGC->planemask);
      } else {
       /* Pixmap --> Pixmap */
	 ErrorF("s3CopyArea:  Tried to do a Pixmap to Pixmap copy\n");
      }

    /*
     * (*localDoBitBlt) (pSrcDrawable, pDstDrawable, pGC->alu, &rgnDst,
     * pptSrc, pGC->planemask);
     */
   }
   prgnExposed = NULL;
   if (((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->fExpose) {
      extern RegionPtr miHandleExposures();

    /* Pixmap sources generate a NoExposed (we return NULL to do this) */
      if (!fastExpose)
	 prgnExposed =
	    miHandleExposures(pSrcDrawable, pDstDrawable, pGC,
			      origSource.x, origSource.y,
			      (int)origSource.width,
			      (int)origSource.height,
			      origDest.x, origDest.y, 0);
   }
   (*pGC->pScreen->RegionUninit) (&rgnDst);
   if (freeSrcClip)
      (*pGC->pScreen->RegionDestroy) (prgnSrcClip);
   return prgnExposed;
}

void
s3FindOrdering(pSrcDrawable, pDstDrawable, pGC, numRects, boxes, srcx, srcy, dstx, dsty, ordering)
     DrawablePtr pSrcDrawable;
     DrawablePtr pDstDrawable;
     GC   *pGC;
     int   numRects;
     BoxPtr boxes;
     int   srcx;
     int   srcy;
     int   dstx;
     int   dsty;
     unsigned int *ordering;
{
   int   i, j, y;
   int   xMax, yMin, yMax;

 /*
  * If not the same drawable then order of move doesn't matter. Following
  * assumes that boxes are sorted from top to bottom and left to right.
  */
   if ((pSrcDrawable != pDstDrawable) &&
       ((pGC->subWindowMode != IncludeInferiors) ||
	(pSrcDrawable->type == DRAWABLE_PIXMAP) ||
	(pDstDrawable->type == DRAWABLE_PIXMAP))) {
      for (i = 0; i < numRects; i++)
	 ordering[i] = i;
   } else {			/* within same drawable, must sequence moves
				 * carefully! */
      if (dsty <= srcy) {	/* Scroll up or stationary vertical. Vertical
				 * order OK */
	 if (dstx <= srcx) {	/* Scroll left or stationary horizontal.
				 * Horizontal order OK as well */
	    for (i = 0; i < numRects; i++)
	       ordering[i] = i;
	 } else {		/* scroll right. must reverse horizontal
				 * banding of rects. */
	    for (i = 0, j = 1, xMax = 0; i < numRects; j = i + 1, xMax = i) {
	     /* find extent of current horizontal band */
	       y = boxes[i].y1;	/* band has this y coordinate */
	       while ((j < numRects) && (boxes[j].y1 == y))
		  j++;
	     /* reverse the horizontal band in the output ordering */
	       for (j--; j >= xMax; j--, i++)
		  ordering[i] = j;
	    }
	 }
      } else {			/* Scroll down. Must reverse vertical
				 * banding. */
	 if (dstx < srcx) {	/* Scroll left. Horizontal order OK. */
	    for (i = numRects - 1, j = i - 1, yMin = i, yMax = 0; i >= 0; j = i - 1, yMin = i) {
	     /* find extent of current horizontal band */
	       y = boxes[i].y1;	/* band has this y coordinate */
	       while ((j >= 0) && (boxes[j].y1 == y))
		  j--;
	     /* reverse the horizontal band in the output ordering */
	       for (j++; j <= yMin; j++, i--, yMax++)
		  ordering[yMax] = j;
	    }
	 } else {		/* Scroll right or horizontal stationary.
				 * Reverse horizontal order as well (if
				 * stationary, horizontal order can be
				 * swapped without penalty and this is faster
				 * to compute). */
	    for (i = 0, j = numRects - 1; i < numRects; i++, j--)
	       ordering[i] = j;
	 }
      }
   }
}

RegionPtr
s3CopyPlane(pSrcDrawable, pDstDrawable,
            pGC, srcx, srcy, width, height, dstx, dsty, bitPlane)
    DrawablePtr              pSrcDrawable;
    DrawablePtr              pDstDrawable;
    GCPtr            pGC;
    int                      srcx, srcy;
    int                      width, height;
    int                      dstx, dsty;
    unsigned long    bitPlane;
{
   PixmapPtr pBitmap = NULL;
   RegionPtr prgnSrcClip;	/* may be a new region, or just a copy */
   Bool  freeSrcClip = FALSE;

   RegionPtr prgnExposed;
   RegionRec rgnDst;
   register BoxPtr pbox;
   int   i;
   register int dx;
   register int dy;
   xRectangle origSource;
   DDXPointRec origDest;
   int   numRects;
   BoxRec fastBox;
   int   fastClip = 0;		/* for fast clipping with pixmap source */
   int   fastExpose = 0;	/* for fast exposures with pixmap source */

   /*
    * If switched away, or doing pixmap->pixmap copy, just use cfb.
    */
   if (!x386VTSema || 
       (pSrcDrawable->type != DRAWABLE_WINDOW &&
	pDstDrawable->type != DRAWABLE_WINDOW))
   {
      return cfbCopyPlane(pSrcDrawable, pDstDrawable, pGC,
                      srcx, srcy, width, height, dstx, dsty, bitPlane);
   }

   /*
    * If we are using a linear mapped framebuffer, using direct memory
    * access is faster than using the graphics engine to do stipple
    * operations.
    */
   if (s3LinearAperture) {
      /* 
       * Fall through to below for window->window moves, since the graphics
       * engine is the fastest way to do this.  Otherwise, use the cfb code
       * on the linear region, because that's faster than the graphics engine
       * for stipple-type operations.
       */
      if ((pSrcDrawable->type != DRAWABLE_WINDOW) ||
	  (pDstDrawable->type != DRAWABLE_WINDOW)) {
         RegionPtr retVal;
	 BLOCK_CURSOR;
         s3EnableLinear();
         retVal = cfbCopyPlane(pSrcDrawable, pDstDrawable, pGC, srcx, srcy, 
			       width, height, dstx, dsty, bitPlane);
         s3DisableLinear();
	 UNBLOCK_CURSOR;
         return retVal;
      }
   } else {
      if ((pSrcDrawable->type != DRAWABLE_WINDOW) &&
	  (pDstDrawable->type == DRAWABLE_WINDOW) &&
	  (pSrcDrawable->bitsPerPixel == 8)) {
	 /*
	  * Shortcut - we can do Pixmap->Window when the source depth is
	  * 8 by using the handy-dandy cfbCopyPlane8to1 to create a bitmap
	  * for us to use.
	  */
	 GCPtr pGC1;
         extern cfbCopyPlane8to1();

	 pBitmap=(*pSrcDrawable->pScreen->CreatePixmap)(pSrcDrawable->pScreen, 
						        width, height, 1);
	 if (!pBitmap)
	    return(NULL);
	 pGC1 = GetScratchGC(1, pSrcDrawable->pScreen);
	 if (!pGC1) {
	    (*pSrcDrawable->pScreen->DestroyPixmap)(pBitmap);
	    return(NULL);
	 }
	 ValidateGC((DrawablePtr)pBitmap, pGC1);
	 (void) cfbBitBlt(pSrcDrawable, (DrawablePtr)pBitmap, pGC1, srcx, srcy,
			  width, height, 0, 0, cfbCopyPlane8to1, bitPlane);
	 pSrcDrawable = (DrawablePtr)pBitmap;
      }
      else if ((pSrcDrawable->type == DRAWABLE_WINDOW) &&
	       (pDstDrawable->type != DRAWABLE_WINDOW)) {
	 /*
	  * Shortcut - we can do Window->Pixmap by copying the window to
	  * a pixmap, then we have a Pixmap->Pixmap operation
	  */
	 GCPtr pGC1;
	 RegionPtr retval;
	 PixmapPtr pPixmap;

	 pPixmap=(*pSrcDrawable->pScreen->CreatePixmap)(pSrcDrawable->pScreen, 
						        width, height, 8);
	 if (!pPixmap)
	    return(NULL);
	 pGC1 = GetScratchGC(8, pSrcDrawable->pScreen);
	 if (!pGC1) {
	    (*pSrcDrawable->pScreen->DestroyPixmap)(pPixmap);
	    return(NULL);
	 }
	 ValidateGC((DrawablePtr)pPixmap, pGC1);
	 s3CopyArea(pSrcDrawable, pPixmap, pGC1, srcx, srcy, width, height,
		    0, 0);
	 retval = cfbCopyPlane((DrawablePtr)pPixmap, pDstDrawable, pGC,
                             0, 0, width, height, dstx, dsty, bitPlane);
	 (*pSrcDrawable->pScreen->DestroyPixmap)(pPixmap);
	 return(retval);
      }
      else if (((pSrcDrawable->type == DRAWABLE_WINDOW) && 
                (pDstDrawable->type != DRAWABLE_WINDOW)) ||
	       ((pSrcDrawable->type != DRAWABLE_WINDOW) &&
		(pDstDrawable->type == DRAWABLE_WINDOW) &&
		(pSrcDrawable->bitsPerPixel != 1))) {
         /*
          * Cases we can't handle, and must do another way:
          *	- copy Window->Pixmap
	  *	- copy Pixmap->Window, depth != {1,8}
          *
          * Simplest thing to do is to let miCopyPlane deal with it for us.
          */
         return miCopyPlane(pSrcDrawable, pDstDrawable, pGC, srcx, srcy, 
			    width, height, dstx, dsty, bitPlane);
      }
   }

   origSource.x = srcx;
   origSource.y = srcy;
   origSource.width = width;
   origSource.height = height;
   origDest.x = dstx;
   origDest.y = dsty;

   if ((pSrcDrawable != pDstDrawable) &&
       pSrcDrawable->pScreen->SourceValidate) {
      (*pSrcDrawable->pScreen->SourceValidate)(pSrcDrawable, srcx, srcy, 
					       width, height);
   }
   srcx += pSrcDrawable->x;
   srcy += pSrcDrawable->y;

   /* clip the source */
   if (pSrcDrawable->type == DRAWABLE_PIXMAP) {
      if ((pSrcDrawable == pDstDrawable) &&
	  (pGC->clientClipType == CT_NONE)) {
	 prgnSrcClip = ((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;
      } else {
	 fastClip = 1;
      }
   } else {
      if (pGC->subWindowMode == IncludeInferiors) {
	 if (!((WindowPtr) pSrcDrawable)->parent) {

	    /*
	     * special case bitblt from root window in IncludeInferiors mode;
	     * just like from a pixmap
	     */
	    fastClip = 1;
	 } else if ((pSrcDrawable == pDstDrawable) &&
		    (pGC->clientClipType == CT_NONE)) {
	    prgnSrcClip = ((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;
	 } else {
	    prgnSrcClip = NotClippedByChildren((WindowPtr) pSrcDrawable);
	    freeSrcClip = TRUE;
	 }
      } else {
	 prgnSrcClip = &((WindowPtr) pSrcDrawable)->clipList;
      }
   }

   fastBox.x1 = srcx;
   fastBox.y1 = srcy;
   fastBox.x2 = srcx + width;
   fastBox.y2 = srcy + height;

   /* Don't create a source region if we are doing a fast clip */
   if (fastClip) {
      fastExpose = 1;

      /*
       * clip the source; if regions extend beyond the source size, make sure
       * exposure events get sent
       */
      if (fastBox.x1 < pSrcDrawable->x) {
	 fastBox.x1 = pSrcDrawable->x;
	 fastExpose = 0;
      }
      if (fastBox.y1 < pSrcDrawable->y) {
	 fastBox.y1 = pSrcDrawable->y;
	 fastExpose = 0;
      }
      if (fastBox.x2 > pSrcDrawable->x + (int)pSrcDrawable->width) {
	 fastBox.x2 = pSrcDrawable->x + (int)pSrcDrawable->width;
	 fastExpose = 0;
      }
      if (fastBox.y2 > pSrcDrawable->y + (int)pSrcDrawable->height) {
	 fastBox.y2 = pSrcDrawable->y + (int)pSrcDrawable->height;
	 fastExpose = 0;
      }
   } else {
      (*pGC->pScreen->RegionInit) (&rgnDst, &fastBox, 1);
      (*pGC->pScreen->Intersect) (&rgnDst, &rgnDst, prgnSrcClip);
   }

   dstx += pDstDrawable->x;
   dsty += pDstDrawable->y;

   if (pDstDrawable->type == DRAWABLE_WINDOW) {
      if (!((WindowPtr) pDstDrawable)->realized) {
	 if (!fastClip)
	    (*pGC->pScreen->RegionUninit) (&rgnDst);
	 if (freeSrcClip)
	    (*pGC->pScreen->RegionDestroy) (prgnSrcClip);
	 return NULL;
      }
   }
   dx = srcx - dstx;
   dy = srcy - dsty;

   /* Translate and clip the dst to the destination composite clip */
   if (fastClip) {
      RegionPtr cclip;

      /* Translate the region directly */
      fastBox.x1 -= dx;
      fastBox.x2 -= dx;
      fastBox.y1 -= dy;
      fastBox.y2 -= dy;

      /*
       * If the destination composite clip is one rectangle we can do the clip
       * directly.  Otherwise we have to create a full blown region and call
       * intersect
       */
      cclip = ((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;
      if (REGION_NUM_RECTS(cclip) == 1) {
	 BoxPtr pBox = REGION_RECTS(cclip);

	 if (fastBox.x1 < pBox->x1)
	    fastBox.x1 = pBox->x1;
	 if (fastBox.x2 > pBox->x2)
	    fastBox.x2 = pBox->x2;
	 if (fastBox.y1 < pBox->y1)
	    fastBox.y1 = pBox->y1;
	 if (fastBox.y2 > pBox->y2)
	    fastBox.y2 = pBox->y2;

       /* Check to see if the region is empty */
	 if (fastBox.x1 >= fastBox.x2 || fastBox.y1 >= fastBox.y2)
	    (*pGC->pScreen->RegionInit) (&rgnDst, NullBox, 0);
	 else
	    (*pGC->pScreen->RegionInit) (&rgnDst, &fastBox, 1);
      } else {
         /*
          * We must turn off fastClip now, since we must create a full blown
          * region.  It is intersected with the composite clip below.
          */
	 fastClip = 0;
	 (*pGC->pScreen->RegionInit) (&rgnDst, &fastBox, 1);
      }
   } else {
      (*pGC->pScreen->TranslateRegion) (&rgnDst, -dx, -dy);
   }

   if (!fastClip) {
      (*pGC->pScreen->Intersect) (&rgnDst,
				  &rgnDst,
				  ((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);
   }

   /* Do bit blitting */
   numRects = REGION_NUM_RECTS(&rgnDst);
   if (numRects && width && height) {
      pbox = REGION_RECTS(&rgnDst);

      if (pSrcDrawable->type == DRAWABLE_WINDOW && 
	  pDstDrawable->type == DRAWABLE_WINDOW) {
         /* Window --> Window */
	 unsigned int *ordering;
	 BoxPtr prect;
	 short direction = 0;

	 ordering = (unsigned int *)ALLOCATE_LOCAL(numRects * 
						   sizeof(unsigned int));

	 if (!ordering) {
	    DEALLOCATE_LOCAL(ordering);
	    return (RegionPtr) NULL;
	 }
	 s3FindOrdering(pSrcDrawable, pDstDrawable, pGC, numRects, pbox, 
			srcx, srcy, dstx, dsty, ordering);

	 if (dx > 0)
	    direction |= INC_X;
	 if (dy > 0)
	    direction |= INC_Y;

	 BLOCK_CURSOR;
	 WaitQueue(6);
	 S3_OUTW(FRGD_MIX, FSS_FRGDCOL | s3alu[pGC->alu]);
	 S3_OUTW(RD_MASK, (unsigned short)bitPlane);
	 S3_OUTW(WRT_MASK, pGC->planemask);
	 S3_OUTW(FRGD_COLOR, (short)pGC->fgPixel);
	 S3_OUTW(BKGD_COLOR, (short)pGC->bgPixel);
	 S3_OUTW(MULTIFUNC_CNTL, PIX_CNTL|MIXSEL_EXPBLT);

	 if (direction == (INC_X | INC_Y)) {
	    for (i = 0; i < numRects; i++) {
	       prect = &pbox[ordering[i]];

	       WaitQueue(7);
	       S3_OUTW(CUR_X, (short)(prect->x1 + dx));
	       S3_OUTW(CUR_Y, (short)(prect->y1 + dy));
	       S3_OUTW(DESTX_DIASTP, (short)(prect->x1));
	       S3_OUTW(DESTY_AXSTP, (short)(prect->y1));
	       S3_OUTW(MAJ_AXIS_PCNT, (short)(prect->x2 - prect->x1 - 1));
	       S3_OUTW(MULTIFUNC_CNTL, 
		    MIN_AXIS_PCNT | (short)(prect->y2 - prect->y1 - 1));
	       S3_OUTW(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
	    }
	 } else if (direction == INC_X) {
	    for (i = 0; i < numRects; i++) {
	       prect = &pbox[ordering[i]];

	       WaitQueue(7);
	       S3_OUTW(CUR_X, (short)(prect->x1 + dx));
	       S3_OUTW(CUR_Y, (short)(prect->y2 + dy - 1));
	       S3_OUTW(DESTX_DIASTP, (short)(prect->x1));
	       S3_OUTW(DESTY_AXSTP, (short)(prect->y2 - 1));
	       S3_OUTW(MAJ_AXIS_PCNT, (short)(prect->x2 - prect->x1 - 1));
	       S3_OUTW(MULTIFUNC_CNTL, 
		    MIN_AXIS_PCNT | (short)(prect->y2 - prect->y1 - 1));
	       S3_OUTW(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
	    }
	 } else if (direction == INC_Y) {
	    for (i = 0; i < numRects; i++) {
	       prect = &pbox[ordering[i]];

	       WaitQueue(7);
	       S3_OUTW(CUR_X, (short)(prect->x2 + dx - 1));
	       S3_OUTW(CUR_Y, (short)(prect->y1 + dy));
	       S3_OUTW(DESTX_DIASTP, (short)(prect->x2 - 1));
	       S3_OUTW(DESTY_AXSTP, (short)(prect->y1));
	       S3_OUTW(MAJ_AXIS_PCNT, (short)(prect->x2 - prect->x1 - 1));
	       S3_OUTW(MULTIFUNC_CNTL, 
		    MIN_AXIS_PCNT | (short)(prect->y2 - prect->y1 - 1));
	       S3_OUTW(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
	    }
	 } else {
	    for (i = 0; i < numRects; i++) {
	       prect = &pbox[ordering[i]];

	       WaitQueue(7);
	       S3_OUTW(CUR_X, (short)(prect->x2 + dx - 1));
	       S3_OUTW(CUR_Y, (short)(prect->y2 + dy - 1));
	       S3_OUTW(DESTX_DIASTP, (short)(prect->x2 - 1));
	       S3_OUTW(DESTY_AXSTP, (short)(prect->y2 - 1));
	       S3_OUTW(MAJ_AXIS_PCNT, (short)(prect->x2 - prect->x1 - 1));
	       S3_OUTW(MULTIFUNC_CNTL, 
		    MIN_AXIS_PCNT | (short)(prect->y2 - prect->y1 - 1));
	       S3_OUTW(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
	    }
	 }

	 WaitQueue(4);
	 S3_OUTW(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
	 S3_OUTW(RD_MASK, 0xffff);
	 S3_OUTW(WRT_MASK, 0xffff);
	 S3_OUTW(MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_FRGDMIX | COLCMPOP_F);
	 UNBLOCK_CURSOR;
	 DEALLOCATE_LOCAL(ordering);
      } else if (pSrcDrawable->type == DRAWABLE_WINDOW && 
		 pDstDrawable->type != DRAWABLE_WINDOW) {
         /* Window --> Pixmap */
	 /* THIS IS NOT IMPLEMENTED - IT GETS PASSED TO miCopyPlane ABOVE */
      } else if (pSrcDrawable->type != DRAWABLE_WINDOW && 
		 pDstDrawable->type == DRAWABLE_WINDOW) {
         /* Pixmap --> Window */
         PixmapPtr pix = (PixmapPtr) pSrcDrawable;
	 int   pixWidth;
	 unsigned char *psrc;

	 pixWidth = PixmapBytePad(pSrcDrawable->width, pSrcDrawable->depth);
	 psrc = pix->devPrivate.ptr;
	 
	 if (s3MAX_SLOTS && s3CacheOpStipple(pix, (pbox->x2 - pbox->x1) -
						  pix->drawable.width)) {
	    for (i = numRects; --i >= 0; pbox++) {
	       s3CImageOpStipple(pix->slot, pbox->x1, pbox->y1,
				 pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
				 -dx, -dy, pGC->fgPixel, pGC->bgPixel, 
				 s3alu[pGC->alu], (short)pGC->planemask);
	    }
	 } else {
	    for (i = numRects; --i >= 0; pbox++) {
	       s3ImageOpStipple(pbox->x1, pbox->y1,
				pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
				psrc, pixWidth,
				pix->drawable.width, pix->drawable.height,
				-dx, -dy, pGC->fgPixel, pGC->bgPixel, 
				s3alu[pGC->alu], (short)pGC->planemask);
	    }
	 }
      } else {
         /* Pixmap --> Pixmap */
	 ErrorF("s3CopyArea:  Tried to do a Pixmap to Pixmap copy\n");
      }
   }
   prgnExposed = NULL;
   if (((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->fExpose) {
      extern RegionPtr miHandleExposures();

      /* Pixmap sources generate a NoExposed (we return NULL to do this) */
      if (!fastExpose)
	 prgnExposed =
	    miHandleExposures(pSrcDrawable, pDstDrawable, pGC,
			      origSource.x, origSource.y,
			      (int)origSource.width,
			      (int)origSource.height,
			      origDest.x, origDest.y, 0);
   }
   (*pGC->pScreen->RegionUninit) (&rgnDst);
   if (freeSrcClip)
      (*pGC->pScreen->RegionDestroy) (prgnSrcClip);
   if (pBitmap)
      (*pSrcDrawable->pScreen->DestroyPixmap) (pBitmap);
   return prgnExposed;
}
