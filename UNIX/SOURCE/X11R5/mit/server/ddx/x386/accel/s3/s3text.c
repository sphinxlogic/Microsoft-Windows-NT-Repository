/*
 * Copyright 1992 by Kevin E. Martin, Chapel Hill, North Carolina.
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Kevin E. Martin not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Kevin E. Martin makes no
 * representations about the suitability of this software for any purpose. It
 * is provided "as is" without express or implied warranty.
 * 
 * KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 * 
 */

/*
 * Modified by Amancio Hasty and Jon Tombs
 * 
 * Id: s3text.c,v 2.3 1993/08/09 06:17:57 jon Exp jon
 */

/* $XFree86: mit/server/ddx/x386/accel/s3/s3text.c,v 2.7 1993/12/25 14:00:04 dawes Exp $ */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"cfb.h"
#include	"misc.h"
#include	"x386.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"fontstruct.h"
#include	"dixfontstr.h"
#include	"mi.h"
#include	"s3.h"
#include	"regs3.h"
#include	"s3bcach.h"

int s3NoCPolyText();
int s3NoCImageText();
extern CacheFont8Ptr s3CacheFont8();
extern unsigned char s3SwapBits[256];

int
s3PolyText8(pDraw, pGC, x, y, count, chars)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x, y;
     int   count;
     unsigned char *chars;
{
   CacheFont8Ptr ret;

   if (!x386VTSema)
   {
      return(miPolyText8(pDraw, pGC, x, y, count, chars));
   }

   /*
    * The S3 graphics engine apparently can't handle these ROPs for the
    * BLT operations used to render text.  The 8514 and ATI don't have a
    * problem using the same code that S3 uses.  The common feature of these
    * ROPs is that they don't reference the source pixel.
    */
   if ((pGC->fillStyle != FillSolid) || 
       (pGC->alu == GXclear || pGC->alu == GXinvert || pGC->alu == GXset)) {
      return miPolyText8(pDraw, pGC, x, y, count, chars);       
   } else {
      if ((ret = s3CacheFont8(pGC->font)) == NULL)
	 return s3NoCPolyText(pDraw, pGC, x, y, count, chars, TRUE);
      else
	 return s3CPolyText8(pDraw, pGC, x, y, count, chars, ret);
   }
}

int
s3PolyText16(pDraw, pGC, x, y, count, chars)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x, y;
     int   count;
     unsigned short *chars;
{

   if (!x386VTSema || 
       (pGC->fillStyle != FillSolid) || 
       (pGC->alu == GXclear || pGC->alu == GXinvert ||	pGC->alu == GXset)) {
      return miPolyText16(pDraw, pGC, x, y, count, chars);
  }
   return s3NoCPolyText(pDraw, pGC, x, y, count, chars, FALSE);
}

void
s3ImageText8(pDraw, pGC, x, y, count, chars)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x, y;
     int   count;
     char *chars;
{
   CacheFont8Ptr ret;

   if (!x386VTSema)
   {
      miImageText8(pDraw, pGC, x, y, count, chars);
      return;
   } 

   /* Don't need to check fill style here - it isn't used in image text */
   if ((ret = s3CacheFont8(pGC->font)) == NULL)
     s3NoCImageText(pDraw, pGC, x, y, count, chars, TRUE);
   else
      s3CImageText8(pDraw, pGC, x, y, count, chars, ret);
}

void
s3ImageText16(pDraw, pGC, x, y, count, chars)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x, y;
     int   count;
     unsigned short *chars;
{
   if (!x386VTSema) 
	miImageText16(pDraw, pGC, x, y, count, chars);
   else
        s3NoCImageText(pDraw, pGC, x, y, count, chars, FALSE);
}


/*
 * The guts of this should possibly be tidied up and put in s3im.c.
 * The generic Stipple functions in s3im.c have quite a large unnecessary
 * overhead for bitmap copies in the case that we are doing an exacy copy
 * with no tiling and starting from (0,0) in the source bitmap. - Jon.
 */
__inline__ static void
s3PolyGlyphBlt(pDrawable, pGC, x, y, nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC 		*pGC;
    int 	x, y;
    unsigned int nglyph;
    CharInfoPtr *ppci;		/* array of character info */
    unsigned char *pglyphBase;	/* start of array of glyphs */
{
    int width, height;
    int nbyLine;			/* bytes per line of padded pixmap */
    FontPtr pfont;
    register int i;
    register int j;
    unsigned char *pbits;		/* buffer for PutImage */
    register unsigned char *pb;		/* temp pointer into buffer */
    register CharInfoPtr pci;		/* currect char info */
    register unsigned char *pglyph;	/* pointer bits in glyph */
    int gWidth, gHeight;		/* width and height of glyph */
    register int nbyGlyphWidth;		/* bytes per scanline of glyph */
    int nbyPadGlyph;			/* server padded line of glyph */
    


    pfont = pGC->font;
    width = FONTMAXBOUNDS(pfont,rightSideBearing) - 
	    FONTMINBOUNDS(pfont,leftSideBearing);
    height = FONTMAXBOUNDS(pfont,ascent) +
	     FONTMAXBOUNDS(pfont,descent);

    nbyLine = PixmapBytePad(width, 1);
    pbits = (unsigned char *)ALLOCATE_LOCAL(height*nbyLine);
    if (!pbits)
    {
        return;
    }
    while(nglyph--)
    {
	pci = *ppci++;
	pglyph = FONTGLYPHBITS(pglyphBase, pci);
	gWidth = GLYPHWIDTHPIXELS(pci);
	gHeight = GLYPHHEIGHTPIXELS(pci);
	if (gWidth && gHeight)
	{
	    nbyGlyphWidth = GLYPHWIDTHBYTESPADDED(pci);
	    nbyPadGlyph = PixmapBytePad(gWidth, 1);

	    if (nbyGlyphWidth == nbyPadGlyph
#if GLYPHPADBYTES != 4
	        && (((int) pglyph) & 3) == 0
#endif
		)
	    {
		pb = pglyph;
	    }
	    else
	    {
		for (i=0, pb = pbits; i<gHeight; i++, pb = pbits+(i*nbyPadGlyph))
		    for (j = 0; j < nbyGlyphWidth; j++)
			*pb++ = *pglyph++;
		pb = pbits;
	    }

	    BLOCK_CURSOR;
	    WaitQueue(5)
	    S3_OUTW (CUR_X, (short) x + pci->metrics.leftSideBearing);
	    S3_OUTW (CUR_Y, (short) y - pci->metrics.ascent);
	    S3_OUTW (MAJ_AXIS_PCNT, (short) (gWidth - 1));
	    S3_OUTW (MULTIFUNC_CNTL, MIN_AXIS_PCNT | (gHeight-1));   
	    S3_OUTW (CMD, CMD_RECT | PCDATA | _16BIT | INC_Y | INC_X |
	     DRAW | PLANAR | WRTDATA);

	    
	    { /* The stipple code */
#define SWPBIT(s) (s3SwapBits[pb[(s)]])

	       int h, pix;
	       unsigned short getbuf;

	       for (h = 0; h < gHeight; h++) {
		  pix = 0;
	       
		  for (i = 0; i < gWidth; i += 16) {
		     getbuf = (getbuf << 8) | SWPBIT (pix++);
		     getbuf = (getbuf << 8) | SWPBIT (pix++);
		     S3_OUTW (PIX_TRANS, getbuf);		  
		  }
		  pb += nbyPadGlyph;
	       }
	    }
	    UNBLOCK_CURSOR;
	}

	x += pci->metrics.characterWidth;
    }
    DEALLOCATE_LOCAL(pbits);
}

int
s3NoCPolyText(pDraw, pGC, x, y, count, chars, is8bit)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x;
     int   y;
     int   count;
     unsigned char *chars;
     Bool is8bit;
{
   int   i;
   BoxPtr pBox;
   int   numRects;
   RegionPtr pRegion;
   int   yBand;
   int   maxAscent, maxDescent;
   int   minLeftBearing;
   FontPtr pfont = pGC->font;
   int ret_x, n;
   CharInfoPtr *charinfo;


    if(!(charinfo = (CharInfoPtr *)ALLOCATE_LOCAL(count*sizeof(CharInfoPtr ))))
	return x ;

    if (is8bit)
      GetGlyphs(pGC->font, (unsigned long)count, (unsigned char *)chars,
	      Linear8Bit, &n, charinfo);
    else
      GetGlyphs(pGC->font, (unsigned long)count, (unsigned char *)chars,
		(FONTLASTROW(pGC->font) == 0) ? Linear16Bit : TwoD16Bit,
		&n, charinfo);

   /*
    * If miPolyText8() is to be believed, the returned new X value is
    * completely independent of what happens during rendering.
    */
   ret_x = x;
   for (i = 0; i < n; i++) {
      ret_x += charinfo[i]->metrics.characterWidth;
   }

   if (n == 0) {
      DEALLOCATE_LOCAL(charinfo);
      return ret_x;
   }
   
   x += pDraw->x;
   y += pDraw->y;
   maxAscent = FONTMAXBOUNDS(pfont, ascent);
   maxDescent = FONTMAXBOUNDS(pfont, descent);
   minLeftBearing = FONTMINBOUNDS(pfont, leftSideBearing);
   pRegion = ((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;

   pBox = REGION_RECTS(pRegion);
   numRects = REGION_NUM_RECTS(pRegion);
   while (numRects && pBox->y2 <= y - maxAscent) {
      ++pBox;
      --numRects;
   }
   if (!numRects || pBox->y1 >= y + maxDescent) {
      DEALLOCATE_LOCAL(charinfo);
      return ret_x;
   }
   yBand = pBox->y1;
   while (numRects && pBox->y1 == yBand && pBox->x2 <= x + minLeftBearing) {
      ++pBox;
      --numRects;
   }
   if (!numRects) {
      DEALLOCATE_LOCAL(charinfo);   
      return ret_x;
   }

   BLOCK_CURSOR;
   WaitQueue (5);
   S3_OUTW (WRT_MASK, pGC->planemask);   
   S3_OUTW (FRGD_MIX, FSS_FRGDCOL | s3alu[pGC->alu]);
   S3_OUTW (BKGD_MIX, BSS_BKGDCOL | MIX_DST);
   S3_OUTW (MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_EXPPC | COLCMPOP_F);
   S3_OUTW (FRGD_COLOR, (short) pGC->fgPixel);

   for (; --numRects >= 0; ++pBox) {
      WaitQueue(4);
      S3_OUTW (MULTIFUNC_CNTL, SCISSORS_L | (short)pBox->x1);
      S3_OUTW(MULTIFUNC_CNTL, SCISSORS_T | (short)pBox->y1);
      S3_OUTW(MULTIFUNC_CNTL, SCISSORS_R | (short)(pBox->x2 - 1));
      S3_OUTW(MULTIFUNC_CNTL, SCISSORS_B | (short)(pBox->y2 - 1));

      s3PolyGlyphBlt(pDraw, pGC, x, y, n, charinfo, FONTGLYPHS(pGC->font));

   }

   WaitQueue(7);
   S3_OUTW (FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
   S3_OUTW (BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
   S3_OUTW (MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_FRGDMIX | COLCMPOP_F);
   S3_OUTW(MULTIFUNC_CNTL, SCISSORS_T | 0);
   S3_OUTW(MULTIFUNC_CNTL, SCISSORS_L | 0);
   S3_OUTW(MULTIFUNC_CNTL, SCISSORS_R | (s3DisplayWidth-1));
   S3_OUTW(MULTIFUNC_CNTL, SCISSORS_B | s3ScissB);
   UNBLOCK_CURSOR;

   return ret_x;
}


/*
 * Imagetext is all in one function to avoid calling GetGlyphs() twice.
 * - Jon.
 */
 
int
s3NoCImageText(pDraw, pGC, x, y, count, chars, is8bit)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x;
     int   y;
     int   count;
     unsigned char *chars;
     Bool is8bit;
{
   ExtentInfoRec info;		/* used by QueryGlyphExtents() */
   XID   gcvals[3];
   int   oldAlu, oldFS;
   unsigned long oldFG;
   xRectangle backrect;
   CharInfoPtr *charinfo;
   unsigned long n;

   if (!(charinfo = (CharInfoPtr *) ALLOCATE_LOCAL(count * sizeof(CharInfoPtr))))
      return 0;


    if (is8bit)
      GetGlyphs(pGC->font, (unsigned long)count, (unsigned char *)chars,
	      Linear8Bit, &n, charinfo);
    else
      GetGlyphs(pGC->font, (unsigned long)count, (unsigned char *)chars,
		(FONTLASTROW(pGC->font) == 0) ? Linear16Bit : TwoD16Bit,
		&n, charinfo);


   QueryGlyphExtents(pGC->font, charinfo, n, &info);

   if (info.overallWidth >= 0) {
      backrect.x = x;
      backrect.width = info.overallWidth;
   } else {
      backrect.x = x + info.overallWidth;
      backrect.width = -info.overallWidth;
   }
   backrect.y = y - FONTASCENT(pGC->font);
   backrect.height = FONTASCENT(pGC->font) + FONTDESCENT(pGC->font);

   oldAlu = pGC->alu;
   oldFG = pGC->fgPixel;
   oldFS = pGC->fillStyle;

 /* fill in the background */
   gcvals[0] = GXcopy;
   gcvals[1] = pGC->bgPixel;
   gcvals[2] = FillSolid;
   DoChangeGC(pGC, GCFunction | GCForeground | GCFillStyle, gcvals, 0);
   ValidateGC(pDraw, pGC);
   (*pGC->ops->PolyFillRect) (pDraw, pGC, 1, &backrect);

 /* put down the glyphs */
   gcvals[0] = oldFG;
   DoChangeGC(pGC, GCForeground, gcvals, 0);
   ValidateGC(pDraw, pGC);

   /* begin the font blitting */
   {
      BoxPtr pBox;
      int   numRects;
      RegionPtr pRegion;
      int   maxAscent, maxDescent;
      int   yBand;
      int   minLeftBearing;
      FontPtr pfont = pGC->font;
      
   x += pDraw->x;
   y += pDraw->y;
   maxAscent = FONTMAXBOUNDS(pfont, ascent);
   maxDescent = FONTMAXBOUNDS(pfont, descent);
   minLeftBearing = FONTMINBOUNDS(pfont, leftSideBearing);
   pRegion = ((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;

   pBox = REGION_RECTS(pRegion);
   numRects = REGION_NUM_RECTS(pRegion);
   while (numRects && pBox->y2 <= y - maxAscent) {
      ++pBox;
      --numRects;
   }
   if (!numRects || pBox->y1 >= y + maxDescent) {
      DEALLOCATE_LOCAL(charinfo);
      return 0;
   }
   yBand = pBox->y1;
   while (numRects && pBox->y1 == yBand && pBox->x2 <= x + minLeftBearing) {
      ++pBox;
      --numRects;
   }
   if (!numRects) {
      DEALLOCATE_LOCAL(charinfo);   
      return 0;
   }

   BLOCK_CURSOR;
   WaitQueue (5);
   S3_OUTW (WRT_MASK, pGC->planemask);   
   S3_OUTW (FRGD_MIX, FSS_FRGDCOL | s3alu[pGC->alu]);
   S3_OUTW (BKGD_MIX, BSS_BKGDCOL | MIX_DST);
   S3_OUTW (MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_EXPPC | COLCMPOP_F);
   S3_OUTW (FRGD_COLOR, (short) pGC->fgPixel);

   for (; --numRects >= 0; ++pBox) {
      WaitQueue(4);
      S3_OUTW (MULTIFUNC_CNTL, SCISSORS_L | (short)pBox->x1);
      S3_OUTW(MULTIFUNC_CNTL, SCISSORS_T | (short)pBox->y1);
      S3_OUTW(MULTIFUNC_CNTL, SCISSORS_R | (short)(pBox->x2 - 1));
      S3_OUTW(MULTIFUNC_CNTL, SCISSORS_B | (short)(pBox->y2 - 1));

      s3PolyGlyphBlt(pDraw, pGC, x, y, n, charinfo, FONTGLYPHS(pGC->font));

   }

   WaitQueue(7);
   S3_OUTW (FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
   S3_OUTW (BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
   S3_OUTW (MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_FRGDMIX | COLCMPOP_F);
   S3_OUTW(MULTIFUNC_CNTL, SCISSORS_T | 0);
   S3_OUTW(MULTIFUNC_CNTL, SCISSORS_L | 0);
   S3_OUTW(MULTIFUNC_CNTL, SCISSORS_R | (s3DisplayWidth-1));
   S3_OUTW(MULTIFUNC_CNTL, SCISSORS_B | s3ScissB);
   UNBLOCK_CURSOR;
   }
   
 /* put all the toys away when done playing */
   gcvals[0] = oldAlu;
   gcvals[1] = oldFG;
   gcvals[2] = oldFS;
   DoChangeGC(pGC, GCFunction | GCForeground | GCFillStyle, gcvals, 0);
   return 0;
}
