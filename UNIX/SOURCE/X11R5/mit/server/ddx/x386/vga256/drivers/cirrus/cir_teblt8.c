/*
 * TEGblt - ImageText expanded glyph fonts only.  For
 * 8 bit displays, in Copy mode with no clipping.
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
*/

/* $XConsortium: cfbteblt8.c,v 5.14 91/04/10 11:41:38 keith Exp $ */
/* $XFree86: mit/server/ddx/x386/vga256/drivers/cirrus/cir_teblt8.c,v 2.3 1994/03/06 06:46:26 dawes Exp $ */

/*
 * Modified for Cirrus by Harm Hanemaayer (hhanemaa@cs.ruu.nl).
 *
 * We accelerate straightforward text writing for fonts with widths up to 16
 * pixels. Added CirrusPolyGlyphBlt for transparent text.
 */
 

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"servermd.h"
#include	"cfb.h"
#include	"fontstruct.h"
#include	"dixfontstr.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"
#include	"cfbmskbits.h"
#include	"cfb8bit.h"
#include	"vga.h"	/* For vgaBase. */
#include        "vgaBank.h"
/* #include        "vgaFasm.h" */
#include        "cfbfuncs.h"

#include "compiler.h"

#include "cir_driver.h"
#include "cir_blitter.h"


extern void speedupcfbTEGlyphBlt8();	/* Doesn't support clipping. */
extern void cfbImageGlyphBlt8();
extern void miPolyGlyphBlt();


void CirrusTransferTextWidth8();
void CirrusTransferTextWidth6();
void CirrusTransferText();		/* General, for widths <= 16. */


void CirrusImageGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC 		*pGC;
    int 	xInit, yInit;
    unsigned int nglyph;
    CharInfoPtr *ppci;		/* array of character info */
    unsigned char *pglyphBase;	/* start of array of glyphs */
{
    FontPtr		pfont = pGC->font;
    unsigned long	*pdstBase;
    int			widthDst;
    int			widthGlyph;
    int			h;
    int			x, y;
    BoxRec		bbox;		/* for clipping */

    	int glyphWidth;		/* Character width in pixels. */
    	int glyphWidthBytes;	/* Character width in bytes (padded). */
	int i;
	/* Characters are padded to 4 bytes. */
	unsigned long **glyphp;
	int shift, line;
	unsigned dworddata;
	int destaddr, blitwidth;

	glyphWidth = FONTMAXBOUNDS(pfont,characterWidth);
	glyphWidthBytes = GLYPHWIDTHBYTESPADDED(*ppci);

#if 0	
	ErrorF("CirrusImageGlyphBlt: glyphWidthBytes = %d, glyphWidth = %d\n",
		glyphWidthBytes, glyphWidth);
#endif		

    h = FONTASCENT(pfont) + FONTDESCENT(pfont);

    if ((h | glyphWidth) == 0) return;

    x = xInit + FONTMAXBOUNDS(pfont,leftSideBearing) + pDrawable->x;
    y = yInit - FONTASCENT(pfont) + pDrawable->y;
    bbox.x1 = x;
    bbox.x2 = x + (glyphWidth * nglyph);
    bbox.y1 = y;
    bbox.y2 = y + h;

    switch ((*pGC->pScreen->RectIn)(
                ((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip, &bbox))
    {
      case rgnPART:
	cfbImageGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
      case rgnOUT:
	return;
    }

	/* We only accelerate fonts 16 or less pixels wide. */
	if (glyphWidthBytes != 4 || glyphWidth > 16) {
#if 0		/* There seems to be a problem using speedup. */
		/* Use speedup. */
	        speedupcfbTEGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
#else
	        cfbImageGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
#endif
		return;	        
	}


    if (!cfb8CheckPixels (pGC->fgPixel, pGC->bgPixel))
	cfb8SetPixels (pGC->fgPixel, pGC->bgPixel);

    cfbGetLongWidthAndPointer(pDrawable, widthDst, pdstBase)
    widthDst *= 4;	/* Convert to bytes. */

#if 0
	ErrorF("CirrusImageGlyphBlt: widthDst = %d, size = (%d, %d), bg = %d, fg = %d\n",
		widthDst, glyphWidth * nglyph, h, pGC->bgPixel, pGC->fgPixel);
#endif		

	destaddr = y * widthDst + x;
	SETDESTADDR(destaddr);
	SETDESTPITCH(widthDst);
	SETSRCADDR(0);
	SETSRCPITCH(0);
	blitwidth = glyphWidth * nglyph;
	SETWIDTH(blitwidth);
	SETHEIGHT(h);

	SETBACKGROUNDCOLOR(pGC->bgPixel);
	SETFOREGROUNDCOLOR(pGC->fgPixel);

	SETBLTMODE(SYSTEMSRC | COLOREXPAND);
	SETROP(CROP_SRC);
	STARTBLT();

	/* Problem: must synthesize bitmap. The current code works reasonably
	 * efficiently for 6 and 8 pixel wide fonts, other widths (up to 16)
	 * are less efficiently handled.
	 */

	/* Collect list of pointers to glyph bitmaps. */
	glyphp = ALLOCATE_LOCAL(nglyph * sizeof(unsigned long *));
	for (i = 0; i < nglyph; i++ ) {
		glyphp[i] = (unsigned long *)FONTGLYPHBITS(pglyphBase, *ppci++);
	}

	/* Write bitmap to video memory (for BitBlt engine to process). */
	/* Gather bytes until we have a dword to write. Doubleword is   */
	/* LSByte first, and MSBit first in each byte, as required for  */
	/* the blit data. */

	switch (glyphWidth) {
	case 8 :
		/* 8 pixel wide font, easier and faster. */
		CirrusTransferTextWidth8(nglyph, h, glyphp);
		break;
	case 6 :
		CirrusTransferTextWidth6(nglyph, h, glyphp);
		break;
	default :
		CirrusTransferText(nglyph, h, glyphp, glyphWidth, vgaBase);
		break;
	}

	WAITUNTILFINISHED();

	SETBACKGROUNDCOLOR(0x0f);
	SETFOREGROUNDCOLOR(0);
	
	DEALLOCATE_LOCAL(glyphp);
}


#if 0

void CirrusPolyGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC 		*pGC;
    int 	xInit, yInit;
    unsigned int nglyph;
    CharInfoPtr *ppci;		/* array of character info */
    unsigned char *pglyphBase;	/* start of array of glyphs */
{
    FontPtr		pfont = pGC->font;
    unsigned long	*pdstBase;
    int			widthDst;
    int			widthGlyph;
    int			h;
    int			x, y;
    BoxRec		bbox;		/* for clipping */

    	int glyphWidth;		/* Character width in pixels. */
    	int glyphWidthBytes;	/* Character width in bytes (padded). */
	int i;
	/* Characters are padded to 4 bytes. */
	unsigned long **glyphp;
	int shift, line;
	unsigned dworddata;
	int destaddr, blitwidth;
	void (*PolyGlyph)();

	PolyGlyph = NULL;
	if (pGC->fillStyle != FillSolid)
		PolyGlyph = miPolyGlyphBlt;

        if (FONTMAXBOUNDS(pGC->font,rightSideBearing) -
            FONTMINBOUNDS(pGC->font,leftSideBearing) > 32 ||
	    FONTMINBOUNDS(pGC->font,characterWidth) < 0)
		PolyGlyph = miPolyGlyphBlt;

	/* Special raster ops that don't seem to work. */
	if (pGC->alu == GXset || pGC->alu == GXcopyInverted)
		PolyGlyph = cfbPolyGlyphRop8;

	if (PolyGlyph != NULL) {
		(*PolyGlyph)(pDrawable, pGC, xInit, yInit, nglyph, ppci,
			pglyphBase);
		return;
	}

	glyphWidth = FONTMAXBOUNDS(pfont,characterWidth);
	glyphWidthBytes = GLYPHWIDTHBYTESPADDED(*ppci);

    h = FONTASCENT(pfont) + FONTDESCENT(pfont);

    if ((h | glyphWidth) == 0) return;

    x = xInit + FONTMAXBOUNDS(pfont,leftSideBearing) + pDrawable->x;
    y = yInit - FONTASCENT(pfont) + pDrawable->y;
    bbox.x1 = x;
    bbox.x2 = x + (glyphWidth * nglyph);
    bbox.y1 = y;
    bbox.y2 = y + h;

    switch ((*pGC->pScreen->RectIn)(
                ((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip, &bbox))
    {
      case rgnPART:
        if (pGC->alu == GXcopy)
        	PolyGlyph = cfbPolyGlyphBlt8;
        else
        	PolyGlyph = cfbPolyGlyphRop8;
	(*PolyGlyph)(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
      case rgnOUT:
	return;
    }

	/* We only accelerate fonts 16 or less pixels wide. */
	if (glyphWidthBytes != 4 || glyphWidth > 16) {
	        if (pGC->alu == GXcopy)
        		PolyGlyph = cfbPolyGlyphBlt8;
	        else
        		PolyGlyph = cfbPolyGlyphRop8;
		(*PolyGlyph)(pDrawable, pGC, xInit, yInit, nglyph, ppci,
			pglyphBase);
		return;	        
	}


    if (!cfb8CheckPixels (pGC->fgPixel, pGC->bgPixel))
	cfb8SetPixels (pGC->fgPixel, pGC->bgPixel);

    cfbGetLongWidthAndPointer(pDrawable, widthDst, pdstBase)
    widthDst *= 4;	/* Convert to bytes. */

	destaddr = y * widthDst + x;
	SETDESTADDR(destaddr);
	SETDESTPITCH(widthDst);
	SETSRCADDR(0);
	SETSRCPITCH(0);
	blitwidth = glyphWidth * nglyph;
	SETWIDTH(blitwidth);
	SETHEIGHT(h);

	SETBACKGROUNDCOLOR(pGC->bgPixel);
	SETFOREGROUNDCOLOR(pGC->fgPixel);
	SETTRANSPARENCYCOLOR(pGC->bgPixel);

	SETBLTMODE(SYSTEMSRC | COLOREXPAND | TRANSPARENCYCOMPARE);
	SETROP(cirrus_rop[pGC->alu]);
	STARTBLT();

	/* Problem: must synthesize bitmap. The current code works reasonably
	 * efficiently for 6 and 8 pixel wide fonts, other widths (up to 16)
	 * are less efficiently handled.
	 */

	/* Collect list of pointers to glyph bitmaps. */
	glyphp = ALLOCATE_LOCAL(nglyph * sizeof(unsigned long *));
	for (i = 0; i < nglyph; i++ ) {
		glyphp[i] = (unsigned long *)FONTGLYPHBITS(pglyphBase, *ppci++);
	}

	/* Write bitmap to video memory (for BitBlt engine to process). */
	/* Gather bytes until we have a dword to write. Doubleword is   */
	/* LSByte first, and MSBit first in each byte, as required for  */
	/* the blit data. */

	switch (glyphWidth) {
	case 8 :
		/* 8 pixel wide font, easier and faster. */
		CirrusTransferTextWidth8(nglyph, h, glyphp);
		break;
	case 6 :
		CirrusTransferTextWidth6(nglyph, h, glyphp);
		break;
	default :
		CirrusTransferText(nglyph, h, glyphp, glyphWidth, vgaBase);
		break;
	}

	WAITUNTILFINISHED();

	SETBACKGROUNDCOLOR(0x0f);
	SETFOREGROUNDCOLOR(0);
	
	DEALLOCATE_LOCAL(glyphp);
}

#endif



/*
 * Low-level text transfer routines.
 */

void CirrusTransferTextWidth8(nglyph, h, glyphp)
	int nglyph;
	int h;
	unsigned long **glyphp;
{
	int shift;
	unsigned long dworddata;
	int i, line;

	shift = 0;
	dworddata = 0;
	line = 0;
	while (line < h) {
		i = 0;
		/* Unroll loop. */
		while (shift == 0 && nglyph - i >= 8) {
			/* This only gets used if the text width is a */
			/* multiple of 4 characters. Should do 'alignment' */
			/* for shift values 8, 16 and 24, and then this. */
			dworddata = byte_reversed[glyphp[i][line]];
			dworddata += byte_reversed[glyphp[i + 1][line]] << 8;
			dworddata += byte_reversed[glyphp[i + 2][line]] << 16;
			dworddata += byte_reversed[glyphp[i + 3][line]] << 24;
			*(unsigned long *)vgaBase = dworddata;
			dworddata = byte_reversed[glyphp[i + 4][line]];
			dworddata += byte_reversed[glyphp[i + 5][line]] << 8;
			dworddata += byte_reversed[glyphp[i + 6][line]] << 16;
			dworddata += byte_reversed[glyphp[i + 7][line]] << 24;
			*(unsigned long *)vgaBase = dworddata;
			i += 8;
			dworddata = 0;
		}

		while (i < nglyph) {
			dworddata += byte_reversed[glyphp[i][line]] << shift;
			shift += 8;
			if (shift == 32) {
				/* Write the dword. */
				*(unsigned long *)vgaBase = dworddata;
				shift = 0;
				dworddata = 0;
			}
			i++;
		}
		line++;
	}
	if (shift != 0)
		*(unsigned long *)vgaBase = dworddata;
}


void CirrusTransferTextWidth6(nglyph, h, glyphp)
	int nglyph;
	int h;
	unsigned long **glyphp;
{
	int shift;
	unsigned long dworddata;
	int i, line;

	/* Special case for fixed font. Tricky, bit order is very awkward. */
	/* We maintain a word straightforwardly LSB, and do the bit order  */
	/* converting when writing 16-bit words. */

	dworddata = 0;
	line = 0;
	shift = 0;
	while (line < h) {
		i = 0;
		while (i < nglyph) {
			if (shift == 0 && nglyph - i >= 8) {
				/* Speed up 8 character chunks. */
				/* Bit order conversion is done directly. */
				/* 3 16-bit words written (48 bits). */
				unsigned byte2, byte3, byte6, byte7;
				dworddata = byte_reversed[glyphp[i][line]];
				byte2 = byte_reversed[glyphp[i + 1][line]];
				dworddata += byte2 >> 6;
				dworddata += (byte2 & 0x3c) << 10;
				byte3 = byte_reversed[glyphp[i + 2][line]];
				dworddata += (byte3 & 0xf0) << 4;
				dworddata += (byte3 & 0x0c) << 20;
				dworddata += byte_reversed[glyphp[i + 3][line]] << 14;
				dworddata += byte_reversed[glyphp[i + 4][line]] << 24;
				byte6 = byte_reversed[glyphp[i + 5][line]];
				dworddata += (byte6 & 0xc0) << 18;
				*(unsigned long *)vgaBase = dworddata;
				dworddata = (byte6 & 0x3c) << 2;
				byte7 = byte_reversed[glyphp[i + 6][line]];
				dworddata += (byte7 & 0xf0) >> 4;
				dworddata += (byte7 & 0x0c) << 12;
				dworddata += byte_reversed[glyphp[i + 7][line]] << 6;
				*(unsigned short *)vgaBase = dworddata;
				i += 8;
				dworddata = 0;
			}
			else {
				dworddata += glyphp[i][line] << shift;
				shift += 6;
				i++;
			}
			if (shift >= 16) {
				/* Write a 16-bit word. */
				*(unsigned short *)vgaBase =
					byte_reversed[dworddata & 0xff] +
					(byte_reversed[(dworddata & 0xff00) >> 8] << 8);
				shift -= 16;
				dworddata >>= 16;
			}
		}
		if (shift > 0) {
			/* Make sure last bits of scanline are padded to byte
			 * boundary. */
			shift = (shift + 7) & ~7;
			if (shift >= 16) {
				/* Write a 16-bit word. */
				*(unsigned short *)vgaBase =
					byte_reversed[dworddata & 0xff] +
					(byte_reversed[(dworddata & 0xff00) >> 8] << 8);
				shift -= 16;
				dworddata >>= 16;
			}
		}
		line++;
	}

	{
		/* There are (shift) bits left. */
		unsigned data;
		int bytes;
		data = byte_reversed[dworddata & 0xff] +
			(byte_reversed[(dworddata & 0xff00) >> 8] << 8);
		/* Number of bytes of real bitmap data. */
		bytes = ((nglyph * 6 + 7) >> 3) * h;
		/* We must transfer a multiple of 4 bytes in total. */
		if ((bytes - ((shift + 7) >> 3)) & 2)
			*(unsigned short *)vgaBase = data;
		else
			if (shift != 0)
				*(unsigned long *)vgaBase = data;
	}
}


#if 0

void CirrusTransferText(nglyph, h, glyphp, glyphwidth, base)
	int nglyph;
	int h;
	unsigned long **glyphp;
	unsigned char *base;
{
	int shift;
	unsigned long dworddata;
	int i, line;

	/* Other character widths. Tricky, bit order is very awkward.  */
	/* We maintain a word straightforwardly LSB, and do the */
	/* bit order converting when writing 16-bit words. */

	dworddata = 0;
	line = 0;
	shift = 0;
	while (line < h) {
		i = 0;
		while (i < nglyph) {
			dworddata += glyphp[i][line] << shift;
			shift += glyphwidth;
			if (shift >= 16) {
				/* Write a 16-bit word. */
				*(unsigned short *)base =
					byte_reversed[dworddata & 0xff] +
					(byte_reversed[(dworddata & 0xff00) >> 8] << 8);
				shift -= 16;
				dworddata >>= 16;
			}
			i++;
		}
		if (shift > 0) {
			/* Make sure last bits of scanline are padded to byte
			 * boundary. */
			shift = (shift + 7) & ~7;
			if (shift >= 16) {
				/* Write a 16-bit word. */
				*(unsigned short *)base =
					byte_reversed[dworddata & 0xff] +
					(byte_reversed[(dworddata & 0xff00) >> 8] << 8);
				shift -= 16;
				dworddata >>= 16;
			}
		}
		line++;
	}

	{
		/* There are (shift) bits left. */
		unsigned data;
		int bytes;
		data = byte_reversed[dworddata & 0xff] +
			(byte_reversed[(dworddata & 0xff00) >> 8] << 8);
		/* Number of bytes of real bitmap data. */
		bytes = ((nglyph * glyphwidth + 7) >> 3) * h;
		/* We must transfer a multiple of 4 bytes in total. */
		if ((bytes - ((shift + 7) >> 3)) & 2)
			*(unsigned short *)base = data;
		else
			if (shift != 0)
				*(unsigned long *)base = data;
	}
}

#endif
