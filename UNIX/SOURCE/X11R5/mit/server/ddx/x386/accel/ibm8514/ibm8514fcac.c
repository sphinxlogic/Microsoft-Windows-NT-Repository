/*
 * Copyright 1992 by Kevin E. Martin, Chapel Hill, North Carolina.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Kevin E. Martin not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Kevin E. Martin makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: mit/server/ddx/x386/accel/ibm8514/ibm8514fcac.c,v 2.7 1993/12/25 13:57:17 dawes Exp $ */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"cfb.h"
#include	"misc.h"
#include	"windowstr.h"
#include	"gcstruct.h"
#include	"fontstruct.h"
#include	"dixfontstr.h"
#include	"reg8514.h"
#include	"ibm8514.h"

#define NUM_FONTS      	8
#define FC_MAX_WIDTH	24
#define FC_MAX_HEIGHT	32

extern void QueryGlyphExtents();

typedef struct {
    FontPtr		font;
    unsigned int	lru;
    CharInfoPtr		pci[256];
} CacheFont8Rec;

CacheFont8Rec	ibm8514FontCache[NUM_FONTS];
int		ibm8514FC_X, ibm8514FC_Y;
short		ibm8514ReadMask[8] = { 2, 4, 8, 16, 32, 64, 128, 1 };

void
ibm8514FontCache8Init(x,y)
    int x, y;
{
    int i;

    ibm8514FC_X = 256;  /* To the right of the pixmap cache */
    ibm8514FC_Y = y;

    for (i = 0; i < NUM_FONTS; i++) {
	ibm8514FontCache[i].font = (FontPtr)0;
	ibm8514FontCache[i].lru = 0xffffffff;
    }
}

void
ibm8514UnCacheFont8(font)
    FontPtr font;
{
    int i;

    for (i = 0; i < NUM_FONTS; i++)
	if (ibm8514FontCache[i].font == font) {
	    ibm8514FontCache[i].font = (FontPtr)0;
	    ibm8514FontCache[i].lru = 0xffffffff;
	    return;
	}
}

int
ibm8514CacheFont8(font)
    FontPtr font;
{
    int i, j, c;
    int ret = -1;
    unsigned long n;
    unsigned char chr;
    int width, height;
    int nbyLine;
    unsigned char *pb, *pbits;
    CharInfoPtr pci;
    unsigned char *pglyph;
    int gWidth, gHeight;
    int nbyGlyphWidth;
    int nbyPadGlyph;

    for (i = 0; i < NUM_FONTS; i++)
	if (ibm8514FontCache[i].font == font)
	    ret = i;

    width = FONTMAXBOUNDS(font,rightSideBearing) -
	    FONTMINBOUNDS(font,leftSideBearing);
    height = FONTMAXBOUNDS(font,ascent) + FONTMAXBOUNDS(font,descent);

    if ((ret == -1) && (width <= FC_MAX_WIDTH) && (height <= FC_MAX_HEIGHT) &&
	(FONTFIRSTROW(font) == 0) && (FONTLASTROW(font) == 0) &&
	(FONTLASTCOL(font) < 256)) {
	ret = 0;
	for (i = 1; i < NUM_FONTS; i++)
	    if (ibm8514FontCache[i].lru > ibm8514FontCache[ret].lru)
		ret = i;

	nbyLine = PixmapBytePad(width, 1);
	pbits = (unsigned char *)ALLOCATE_LOCAL(height*nbyLine);
	if (!pbits) {
	    return -1;
	}

	ibm8514FontCache[ret].font = font;
	ibm8514FontCache[ret].lru = 0;

	for (c = 0; c < 256; c++) {
	    chr = (unsigned char)c;
	    GetGlyphs(font, 1, &chr, Linear8Bit, &n, &pci);
	    if (n == 0) {
		ibm8514FontCache[ret].pci[c] = NULL;
	    } else {
		ibm8514FontCache[ret].pci[c] = pci;
		pglyph = FONTGLYPHBITS(pglyphBase, pci);
		gWidth = GLYPHWIDTHPIXELS(pci);
		gHeight = GLYPHHEIGHTPIXELS(pci);
		if (gWidth && gHeight) {
		    nbyGlyphWidth = GLYPHWIDTHBYTESPADDED(pci);
		    nbyPadGlyph = PixmapBytePad(gWidth, 1);
		    
		    if (nbyGlyphWidth == nbyPadGlyph
#if GLYPHPADBYTES != 4
			&& (((int) pglyph) & 3) == 0
#endif
			) {
			pb = pglyph;
		    } else {
			for (i = 0, pb = pbits;
			     i < gHeight;
			     i++, pb = pbits+(i*nbyPadGlyph))
			    for (j = 0; j < nbyGlyphWidth; j++)
				*pb++ = *pglyph++;
			pb = pbits;
		    }
		    ibm8514ImageStipple(ibm8514FC_X+(c%32)*FC_MAX_WIDTH,
					ibm8514FC_Y+(c/32)*FC_MAX_HEIGHT,
					gWidth, gHeight,
					pb, nbyGlyphWidth, gWidth, gHeight,
					ibm8514FC_X+(c%32)*FC_MAX_WIDTH,
					ibm8514FC_Y+(c/32)*FC_MAX_HEIGHT,
					0xff, 0, ibm8514alu[GXcopy],
					(1 << ret), 1);
		}
	    }
	}
    }

    return ret;
}

int
ibm8514CPolyText8(pDraw, pGC, x, y, count, chars, plane)
    DrawablePtr pDraw;
    GCPtr pGC;
    int x;
    int y;
    int count;
    unsigned char *chars;
    int plane;
{
    int		i;
    BoxPtr	pBox;
    int		numRects;
    RegionPtr	pRegion;
    int		yBand;
    int		maxAscent, maxDescent;
    int		minLeftBearing;
    FontPtr	pfont = pGC->font;
    int		xorig;
    CharInfoPtr	pci;
    CacheFont8Rec *ibm8514FCP = &(ibm8514FontCache[plane]);
    int         ret_x;

    /*
     * If miPolyText8() is to be believed, the returned new X value is
     * completely independent of what happens during rendering.
     */
    ret_x = x;
    for (i = 0; i < count; i++)
	ret_x += ibm8514FCP->pci[(int)chars[i]] ?
		 ibm8514FCP->pci[(int)chars[i]]->metrics.characterWidth : 0;

    for (i = 0; i < NUM_FONTS; i++)
	if (i != plane && ibm8514FontCache[i].lru != 0xffffffff)
	    ibm8514FontCache[i].lru++;

    x += pDraw->x;
    y += pDraw->y;

    maxAscent = FONTMAXBOUNDS(pfont,ascent);
    maxDescent = FONTMAXBOUNDS(pfont,descent);
    minLeftBearing = FONTMINBOUNDS(pfont,leftSideBearing);

    pRegion = ((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;

    pBox = REGION_RECTS(pRegion);
    numRects = REGION_NUM_RECTS (pRegion);
    while (numRects && pBox->y2 <= y - maxAscent)
    {
	++pBox;
	--numRects;
    }
    if (!numRects || pBox->y1 >= y + maxDescent)
	return ret_x;
    yBand = pBox->y1;
    while (numRects && pBox->y1 == yBand && pBox->x2 <= x + minLeftBearing)
    {
	++pBox;
	--numRects;
    }
    if (!numRects)
	return ret_x;

    WaitQueue(6);
    outw(FRGD_COLOR, (short)pGC->fgPixel);
    outw(MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_EXPBLT | COLCMPOP_F);
    outw(RD_MASK, ibm8514ReadMask[plane]);
    outw(FRGD_MIX, FSS_FRGDCOL | ibm8514alu[pGC->alu]);
    outw(BKGD_MIX, BSS_BKGDCOL | MIX_DST);
    outw(WRT_MASK, (short)pGC->planemask);

    for (xorig = x; --numRects >= 0; ++pBox, x = xorig) {
	WaitQueue(4);
	outw(MULTIFUNC_CNTL, SCISSORS_L | (short)pBox->x1);
	outw(MULTIFUNC_CNTL, SCISSORS_T | (short)pBox->y1);
	outw(MULTIFUNC_CNTL, SCISSORS_R | (short)(pBox->x2-1));
	outw(MULTIFUNC_CNTL, SCISSORS_B | (short)(pBox->y2-1));

	for (i = 0; i < count; i++) {
	    pci = ibm8514FCP->pci[(int)chars[i]];

	    if (pci != NULL) {
		if (GLYPHHEIGHTPIXELS(pci) && GLYPHWIDTHPIXELS(pci))
		{
		    WaitQueue(7);
		    outw(CUR_X, (short)(ibm8514FC_X+(((int)chars[i])%32)*FC_MAX_WIDTH));
		    outw(CUR_Y, (short)(ibm8514FC_Y+(((int)chars[i])/32)*FC_MAX_HEIGHT));
		    outw(DESTX_DIASTP, (short)(x + pci->metrics.leftSideBearing));
		    outw(DESTY_AXSTP, (short)(y - pci->metrics.ascent));
		    outw(MAJ_AXIS_PCNT, (short)(GLYPHWIDTHPIXELS(pci)-1));
		    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT |
		         (short)(GLYPHHEIGHTPIXELS(pci)-1));
		    outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);
		}
		x += pci->metrics.characterWidth;
	    }
	}
    }

    WaitQueue(8);
    outw(MULTIFUNC_CNTL, SCISSORS_T | 0);
    outw(MULTIFUNC_CNTL, SCISSORS_L | 0);
    outw(MULTIFUNC_CNTL, SCISSORS_R | 1023);
    outw(MULTIFUNC_CNTL, SCISSORS_B | 1023);
    outw(RD_MASK, 0xff);
    outw(MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_FRGDMIX | COLCMPOP_F);
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
    outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);

    return ret_x;
}


void
ibm8514CImageText8(pDraw, pGC, x, y, count, chars, plane)
    DrawablePtr pDraw;
    GCPtr pGC;
    int x;
    int y;
    int count;
    unsigned char *chars;
    int plane;
{
    ExtentInfoRec info;		/* used by QueryGlyphExtents() */
    XID gcvals[3];
    int oldAlu, oldFS;
    unsigned long oldFG;
    xRectangle backrect;
    CharInfoPtr *ppci;
    unsigned long n;

    if(!(ppci = (CharInfoPtr *)ALLOCATE_LOCAL(count*sizeof(CharInfoPtr))))
	return;

    GetGlyphs(pGC->font, (unsigned long)count, (unsigned char *)chars,
	      Linear8Bit, &n, ppci);

    QueryGlyphExtents(pGC->font, ppci, n, &info);

    DEALLOCATE_LOCAL(ppci);

    if (info.overallWidth >= 0)
    {
    	backrect.x = x;
    	backrect.width = info.overallWidth;
    }
    else
    {
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
    DoChangeGC(pGC, GCFunction|GCForeground|GCFillStyle, gcvals, 0);
    ValidateGC(pDraw, pGC);
    (*pGC->ops->PolyFillRect)(pDraw, pGC, 1, &backrect);

    /* put down the glyphs */
    gcvals[0] = oldFG;
    DoChangeGC(pGC, GCForeground, gcvals, 0);
    ValidateGC(pDraw, pGC);
    (void)ibm8514CPolyText8(pDraw, pGC, x, y, count, chars, plane);

    /* put all the toys away when done playing */
    gcvals[0] = oldAlu;
    gcvals[1] = oldFG;
    gcvals[2] = oldFS;
    DoChangeGC(pGC, GCFunction|GCForeground|GCFillStyle, gcvals, 0);
}
