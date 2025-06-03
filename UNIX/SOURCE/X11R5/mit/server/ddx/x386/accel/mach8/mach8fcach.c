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
 * KEVIN E. MARTIN AND RICKARD E. FAITH DISCLAIM ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL KEVIN E. MARTIN OR RICKARD E. FAITH BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 *
 * Modified to use a dynamic font cache.
 * Accelerated text display when no font cache is available added.
 * Now using a linked list to store the CacheFont8Rec structs instead
 * of a pointer array.
 * Hans Nasten. (nasten@everyware.se)
 */

/* $XFree86: mit/server/ddx/x386/accel/mach8/mach8fcach.c,v 2.17 1994/02/12 11:05:43 dawes Exp $ */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"cfb.h"
#include	"misc.h"
#include	"windowstr.h"
#include	"gcstruct.h"
#include	"fontstruct.h"
#include	"dixfontstr.h"
#include	"regmach8.h"
#include	"mach8.h"

#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"
extern Bool x386Verbose;

extern void QueryGlyphExtents();

#define CACHECELL_WIDTH  48
#define CACHECELL_HEIGHT 51
#define FC_MAX_WIDTH	 CACHECELL_WIDTH/2
#define FC_MAX_HEIGHT	 CACHECELL_HEIGHT

struct cachecell {
    struct cachecell	*next;
    unsigned short	x;
    unsigned short	y;
    unsigned short	plane;
};

struct font_pos {
    unsigned short	x;
    unsigned short	y;
    unsigned short	planemask;
};

typedef struct CFRec{
    struct CFRec	*next;
    FontPtr		font;
    struct cachecell	*cells;
    struct font_pos	pos[256];
    CharInfoPtr		pci[256];
} CacheFont8Rec;

CacheFont8Rec	*mach8FontCache;
short		mach8ReadMask[8] = { 2, 4, 8, 16, 32, 64, 128, 1 };

int			mach8FCacheInit = 0;
struct cachecell	*mach8CacheFreeList;
int			mach8FreeCacheCells;
int			mach8TotalCacheCells;
int			mach8NumberOfCachedFonts;

/*
 * mach8FreeCellChain
 *      Add a linked list of cell structs to the front of the free list.
 */

void
mach8FreeCellChain( chain )
struct cachecell *chain;

{
struct cachecell *lastlink;
int n_cells;

    if( chain != NULL ) {
	n_cells = 1;
	for( lastlink = chain;
	     lastlink->next != NULL;
	     lastlink = lastlink->next )
	    n_cells++;

	lastlink->next = mach8CacheFreeList;
	mach8CacheFreeList = chain;
	mach8FreeCacheCells += n_cells;
    }
}


/*
 * mach8AllocCellChain
 *      Allocate a linked list of cell structs from the front of the free list.
 */

struct cachecell *
mach8AllocCellChain( n_cells )
int n_cells;

{
struct cachecell *cellpnt, *retpnt;
int nc;

    if( mach8FreeCacheCells < n_cells )
	return( NULL );

    nc = n_cells;
    retpnt = cellpnt = mach8CacheFreeList;
    while( --nc > 0 ) {
	if( cellpnt == NULL )
	    ErrorF( "Severe cache panic. Reached NULL with n_cells > 0\n" );

	cellpnt = cellpnt->next;
    }

    mach8CacheFreeList = cellpnt->next;
    cellpnt->next = NULL;
    mach8FreeCacheCells -= n_cells;
    return( retpnt );

}


/*
 * mach8AddCacheArea
 *      Add the specified screen memory to the cache free list.
 */

void
mach8AddCacheArea( x, y, width, height )
int x, y, width, height;
{
struct cachecell *chain, *lastcell, *newcell;
int h, w, x2, y2;
unsigned short bitplane;

    chain = lastcell = NULL;
    for( bitplane = 0; bitplane < 8; bitplane++ ) {
	y2 = y;
	h = height;
	while( h >= CACHECELL_HEIGHT ) {
	    x2 = x;
	    w = width;
	    while( w >= CACHECELL_WIDTH ) {
		if( ( newcell = (struct cachecell *)
			    (Xalloc( sizeof( struct cachecell ))) ) == NULL ) {
		   ErrorF("Out of memory when Xallocing cache cell structs\n");
		   return;
		}
		newcell->plane = bitplane;
		newcell->x = x2;
		newcell->y = y2;
		newcell->next = NULL;
		if( lastcell == NULL )
		    chain = lastcell = newcell;
		else {
		    lastcell->next = newcell;
		    lastcell = newcell;
		}
		x2 += CACHECELL_WIDTH;
		w -= CACHECELL_WIDTH;
	    }
	    y2 += CACHECELL_HEIGHT;
	    h -= CACHECELL_HEIGHT;
	}
    }
    if( chain != NULL )
	mach8FreeCellChain( chain );

}

/*
 * mach8FontCache8Init --
 *      initialize Mach8 font cache.  We need to set the mach8FC*
 *      variables to the proper values so we don't interfere with
 *      the display or the pixmap cache.
 */

void
mach8FontCache8Init()
{
    int totaly;
    int first = 1;
    CacheFont8Rec *fpnt, *fpnt2;

  
    /*
     * If this is a call due to a VT switch, uncache all fonts.
     * I belive this is needed at least when someone starts 2 servers
     * on different VT:s.
     */
    if( mach8FCacheInit ) {
	for (fpnt = mach8FontCache; fpnt != NULL; fpnt = fpnt2) {
	    fpnt2 = fpnt->next;
	    Xfree( fpnt );
	    first = 0;
	}
    }
    mach8FCacheInit = 1;
    mach8FontCache = NULL;
    mach8CacheFreeList = NULL;
    mach8FreeCacheCells = 0;
    mach8NumberOfCachedFonts = 0;
    totaly=(mach8InfoRec.videoRam*1024) / mach8InfoRec.virtualX; 
    if( totaly > 1024 )
	totaly = 1024;
    if( mach8InfoRec.videoRam > 512 ) {
	mach8AddCacheArea( 256, mach8InfoRec.virtualY, 768,
			   1024-mach8InfoRec.virtualY > 256 ? 256 :
						1024-mach8InfoRec.virtualY);
	if( mach8InfoRec.virtualX < 1024 )
	    mach8AddCacheArea( mach8InfoRec.virtualX, 0,
			       1024-mach8InfoRec.virtualX,
			       mach8InfoRec.virtualY );
    }
    else {
	mach8AddCacheArea( 256, mach8InfoRec.virtualY,
			   mach8InfoRec.virtualX-256,
			   totaly-mach8InfoRec.virtualY > 256 ? 256 :
						totaly-mach8InfoRec.virtualY);
    }
    mach8TotalCacheCells = mach8FreeCacheCells;
    if( x386Verbose && first )
	ErrorF( "%s %s: Total number of font cache cells (%dx%d) %d\n",
		XCONFIG_PROBED, mach8InfoRec.name,
		CACHECELL_WIDTH, CACHECELL_HEIGHT, mach8FreeCacheCells );


}

void
mach8UnCacheFont8(font)
    FontPtr font;
{
    int i;
    CacheFont8Rec *fpnt, *fpnt2;

    if( mach8FontCache != NULL ) {
	fpnt = mach8FontCache;
	if( fpnt->font == font ) {
	    mach8FontCache = fpnt->next;
	    mach8FreeCellChain( fpnt->cells );
	    Xfree( fpnt );
	    mach8NumberOfCachedFonts--;
	}
	else {
	    while( fpnt->next != NULL ) {
		fpnt2 = fpnt->next;
		if (fpnt2->font == font) {
		    fpnt->next = fpnt2->next;
		    mach8FreeCellChain( fpnt2->cells );
		    Xfree( fpnt2 );
		    mach8NumberOfCachedFonts--;
		    break;
		}
		fpnt = fpnt2;
	    }
	}
    }

}

void *
mach8CacheFont8(font)
    FontPtr font;
{
    int i, j, c;
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
    struct cachecell *cell;
    int glyphs_cell, n_cells;
    int cell_x, cell_y, cell_width, cell_height;
    CacheFont8Rec *fpnt;


    for (fpnt = mach8FontCache; fpnt != NULL; fpnt = fpnt->next) {
	if( fpnt->font == font )
	    return( (void *)(fpnt) );
    }
    width = FONTMAXBOUNDS(font,rightSideBearing) -
	    FONTMINBOUNDS(font,leftSideBearing);
    height = FONTMAXBOUNDS(font,ascent) + FONTMAXBOUNDS(font,descent);
    fpnt = NULL;
    if ((width <= FC_MAX_WIDTH) && (height <= FC_MAX_HEIGHT) &&
	(FONTFIRSTROW(font) == 0) && (FONTLASTROW(font) == 0) &&
	(FONTLASTCOL(font) < 256)) {
	nbyLine = PixmapBytePad(width, 1);
	pbits = (unsigned char *)ALLOCATE_LOCAL(height*nbyLine);
	if (!pbits)
	    return( NULL );

	glyphs_cell = (CACHECELL_WIDTH/width)*(CACHECELL_HEIGHT/height);
	n_cells = 256 / glyphs_cell;
	if( ( 256 % glyphs_cell ) != 0 )
	    n_cells++;

	/*
	 * Free old fonts until enough cache cells are available.
	 */
	while( ( cell = mach8AllocCellChain(n_cells) ) == NULL ) {
	    if( mach8FontCache == NULL ) {
		DEALLOCATE_LOCAL(pbits);
		return( NULL );	/* No fonts cached and no cache cells free. */
	    }
	    /*
	     * Find oldest cached font. ( last in linked list ).
	     */
	    for( fpnt = mach8FontCache; fpnt->next != NULL; fpnt = fpnt->next )
		;

	    /*
	     * Uncache oldest cached font.
	     */
	    mach8UnCacheFont8( fpnt->font );
	}
	/*
	 * Allocate a font cache record and link it first in the chain.
	 */
	if( ( fpnt = (CacheFont8Rec *)
				 (Xalloc( sizeof(CacheFont8Rec)))) == NULL ) {
	    DEALLOCATE_LOCAL(pbits);
	    return( NULL );
	}
	fpnt->next = mach8FontCache;
	mach8FontCache = fpnt;
	mach8NumberOfCachedFonts++;

	/*
	 * Now we cache the font.
	 */
	fpnt->cells = cell;
	fpnt->font = font;
	cell_x = cell_y = 0;
	cell_width = CACHECELL_WIDTH;
	cell_height = CACHECELL_HEIGHT;
	for (c = 0; c < 256; c++) {
	    fpnt->pos[c].x = cell->x + cell_x;
	    fpnt->pos[c].y = cell->y + cell_y;
	    fpnt->pos[c].planemask = mach8ReadMask[cell->plane];
	    chr = (unsigned char)c;
	    GetGlyphs(font, 1, &chr, Linear8Bit, &n, &pci);
	    if (n == 0) {
		fpnt->pci[c] = NULL;
	    } else {
		fpnt->pci[c] = pci;
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
		    (mach8ImageStippleFunc)(cell->x + cell_x,
					    cell->y + cell_y,
					    gWidth, gHeight,
					    pb, nbyGlyphWidth, gWidth, gHeight,
					    cell->x + cell_x,
					    cell->y + cell_y,
					    0xff, 0, mach8alu[GXcopy],
					    (1 << cell->plane), 1);
		}
	    }
	    /*
	     * Advance the cache cell pointers from left to right,
	     * top to bottom, cell by cell.
	     */
	    cell_x += width;
	    cell_width -= width;
	    if( cell_width < width ) {
		/*
		 * At right edge. Try to put another row below this one.
		 */
		cell_width = CACHECELL_WIDTH;
		cell_x = 0;
		cell_y += height;
		cell_height -= height;
		if( cell_height < height ) {
		    /*
		     * Use next cache cell.
		     */
		    cell_height = CACHECELL_HEIGHT;
		    cell_y = 0;
		    cell = cell->next;
		}
	    }
	}
	DEALLOCATE_LOCAL(pbits);
    }
    return( (void *)(fpnt) );
}

int
mach8CPolyText8(pDraw, pGC, x, y, count, chars, vpnt)
    DrawablePtr pDraw;
    GCPtr pGC;
    int x;
    int y;
    int count;
    unsigned char *chars;
    void *vpnt;
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
    CacheFont8Rec *mach8FCP = (CacheFont8Rec *)(vpnt);
    CacheFont8Rec *fpnt;
    int         ret_x;
    CharInfoPtr	pci;
    int		ci;
    struct font_pos *pos;
    unsigned short pmsk, width, height, gwidth, gheight;


    /*
     * If miPolyText8() is to be believed, the returned new X value is
     * completely independent of what happens during rendering.
     */
    ret_x = x;
    for (i = 0; i < count; i++)
	ret_x += mach8FCP->pci[(int)chars[i]] ?
		 mach8FCP->pci[(int)chars[i]]->metrics.characterWidth : 0;

    /*
     * Make sure this font is first in the linked list.
     */
    if( mach8FCP != mach8FontCache ) {
	for( fpnt = mach8FontCache; fpnt->next != mach8FCP; fpnt = fpnt->next )
	    ;
	fpnt->next = mach8FCP->next;
	mach8FCP->next = mach8FontCache;
	mach8FontCache = mach8FCP;
    }
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

    WaitQueue(5);
    outw(FRGD_COLOR, (short)pGC->fgPixel);
    outw(MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_EXPBLT | COLCMPOP_F);
    outw(FRGD_MIX, FSS_FRGDCOL | mach8alu[pGC->alu]);
    outw(BKGD_MIX, BSS_BKGDCOL | MIX_DST);
    outw(WRT_MASK, (short)pGC->planemask);
    for (xorig = x; --numRects >= 0; ++pBox, x = xorig) {
	WaitQueue(4);
	outw(MULTIFUNC_CNTL, SCISSORS_L | (short)pBox->x1);
	outw(MULTIFUNC_CNTL, SCISSORS_T | (short)pBox->y1);
	outw(MULTIFUNC_CNTL, SCISSORS_R | (short)(pBox->x2-1));
	outw(MULTIFUNC_CNTL, SCISSORS_B | (short)(pBox->y2-1));
	pmsk = width = height = 0;
	for (i = 0; i < count; i++) {
	    ci = (int)(chars[i]);
	    pci = mach8FCP->pci[ci];
	    pos = &mach8FCP->pos[ci];
	    if (pci != NULL) {
		if( ( gheight = (short)(GLYPHHEIGHTPIXELS(pci)) )
		&& ( gwidth = (short)(GLYPHWIDTHPIXELS(pci)) ) ) {
		    gheight--;
		    gwidth--;
		    WaitQueue(8);
		    if( !pmsk || pmsk != pos->planemask ) {
			pmsk = pos->planemask;
			outw(RD_MASK, pmsk);
		    }
		    outw(CUR_X, pos->x);
		    outw(CUR_Y, pos->y);
		    outw(DESTX_DIASTP, (short)(x + pci->metrics.leftSideBearing));
		    outw(DESTY_AXSTP, (short)(y - pci->metrics.ascent));
		    if( !width || width != gwidth ) {
			width = gwidth;
			outw(MAJ_AXIS_PCNT, width);
		    }
		    if( !height || height != gheight ){
			height = gheight;
			outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | height);
		    }
		    outw(CMD,
			 CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);
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
mach8AccPolyText8(pDraw, pGC, x, y, count, chars)
    DrawablePtr pDraw;
    GCPtr	pGC;
    int		x, y;
    int		count;
    char	*chars;
{
    int i, j;
    FontPtr font = pGC->font;
    unsigned long n;
    int nbyLine;
    unsigned char *pb, *pbits;
    CharInfoPtr pci;
    unsigned char *pglyph;
    int gWidth, gHeight;
    int nbyGlyphWidth;
    int nbyPadGlyph;
    int		maxAscent, maxDescent;
    int		minLeftBearing;
    BoxPtr	pBox;
    int		numRects;
    RegionPtr	pRegion;
    int		yBand;
    int		xorig;
    int		clipx, clipy, clipw, cliph, xoff, yoff;


    gWidth = FONTMAXBOUNDS(font,rightSideBearing) -
	    FONTMINBOUNDS(font,leftSideBearing);
    gHeight = FONTMAXBOUNDS(font,ascent) + FONTMAXBOUNDS(font,descent);
    nbyLine = PixmapBytePad(gWidth, 1);
    pbits = (unsigned char *)ALLOCATE_LOCAL(gHeight*nbyLine);
    if (!pbits)
	return;

    x += pDraw->x;
    y += pDraw->y;

    maxAscent = FONTMAXBOUNDS(font,ascent);
    maxDescent = FONTMAXBOUNDS(font,descent);
    minLeftBearing = FONTMINBOUNDS(font,leftSideBearing);

    pRegion = ((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;

    pBox = REGION_RECTS(pRegion);
    numRects = REGION_NUM_RECTS (pRegion);
    while (numRects && pBox->y2 <= y - maxAscent)
    {
	++pBox;
	--numRects;
    }
    if (!numRects || pBox->y1 >= y + maxDescent) {
	DEALLOCATE_LOCAL(pbits);
	return;
    }
    yBand = pBox->y1;
    while (numRects && pBox->y1 == yBand && pBox->x2 <= x + minLeftBearing)
    {
	++pBox;
	--numRects;
    }
    if (!numRects) {
	DEALLOCATE_LOCAL(pbits);
	return;
    }
    for (xorig = x; --numRects >= 0; ++pBox, x = xorig) {
	for (i = 0; i < count; i++) {
	    GetGlyphs(font, 1, &chars[i], Linear8Bit, &n, &pci);
	    if (n != 0) {
		pglyph = FONTGLYPHBITS(pglyphBase, pci);
		gWidth = GLYPHWIDTHPIXELS(pci);
		gHeight = GLYPHHEIGHTPIXELS(pci);
		clipx = x + pci->metrics.leftSideBearing;
		clipy = y - pci->metrics.ascent;
		if( gWidth && gHeight
		&& clipx + gWidth > pBox->x1 && clipx < pBox->x2
		&& clipy + gHeight > pBox->y1 && clipy < pBox->y2 ) {
		    xoff = ( clipx < pBox->x1 ? pBox->x1 - clipx : 0 );
		    if( clipx + gWidth > pBox->x2 )
			clipw = gWidth - xoff - ((clipx + gWidth) - pBox->x2);
		    else
			clipw = gWidth - xoff;

		    yoff = ( clipy < pBox->y1 ? pBox->y1 - clipy : 0 );
		    if( clipy + gHeight > pBox->y2 )
			cliph = gHeight-yoff - ((clipy + gHeight) - pBox->y2);
		    else
			cliph = gHeight - yoff;

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
		    (mach8ImageStippleFunc)( clipx + xoff,
					     clipy + yoff,
					     clipw, cliph,
					     pb, nbyGlyphWidth,
					     gWidth, gHeight,
					     clipx, clipy,
					     pGC->fgPixel, pGC->bgPixel,
					     mach8alu[pGC->alu],
					     pGC->planemask, 1);
		}
		x += pci->metrics.characterWidth;
	    }
	}
    }
    DEALLOCATE_LOCAL(pbits);

}


void
mach8CImageText8(pDraw, pGC, x, y, count, chars, vpnt)
    DrawablePtr pDraw;
    GCPtr pGC;
    int x;
    int y;
    int count;
    unsigned char *chars;
    void *vpnt;
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
    if( vpnt == NULL )
	mach8AccPolyText8(pDraw, pGC, x, y, count, chars );
    else
	(void)mach8CPolyText8(pDraw, pGC, x, y, count, chars, vpnt);

    /* put all the toys away when done playing */
    gcvals[0] = oldAlu;
    gcvals[1] = oldFG;
    gcvals[2] = oldFS;
    DoChangeGC(pGC, GCFunction|GCForeground|GCFillStyle, gcvals, 0);
}
