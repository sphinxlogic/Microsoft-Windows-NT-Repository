/*
 *
 * Copyright 1994 by H. Hanemaayer, Utrecht, The Netherlands
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of H. Hanemaayer not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  H. Hanemaayer makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * H. HANEMAAYER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL H. HANEMAAYER BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  H. Hanemaayer, <hhanemaa@cs.ruu.nl>
 *
 */

/* $XFree86: mit/server/ddx/x386/vga256/drivers/cirrus/cir_blitter.c,v 2.2 1994/03/01 09:57:06 dawes Exp $ */

/*
 * This file contains all low-level functions that use the BitBLT engine
 * on the 5426/5428/5434 (except for those that involve system memory,
 * they are in cir_im.c).
 */
 

#include "vga.h"	/* For vgaBase. */
#include "compiler.h"

#include "cir_driver.h"
#include "cir_blitter.h"


/* Cirrus raster operations. */

int cirrus_rop[16] = {
	CROP_0,			/* GXclear */
	CROP_AND,		/* GXand */
	CROP_SRC_AND_NOT_DST,	/* GXandReverse */
	CROP_SRC,		/* GXcopy */
	CROP_NOT_SRC_AND_DST,	/* GXandInverted */
	CROP_DST,		/* GXnoop */
	CROP_XOR,		/* GXxor */
	CROP_OR,		/* GXor */
	CROP_NOR,		/* GXnor */
	CROP_XNOR,		/* GXequiv */
	CROP_NOT_DST,		/* GXinvert */
	CROP_SRC_OR_NOT_DST,	/* GXorReverse */
	CROP_NOT_SRC,		/* GXcopyInverted */
	CROP_NOT_SRC_OR_DST,	/* GXorInverted */
	CROP_NAND,		/* GXnand */
	CROP_1			/* GXset */
};


/*
 * 8x8 pattern fill with color expanded pattern.
 *
 * patternword1 contains the first 32 pixels (first pixel in LSByte, MSBit),
 * patternword2 the second. 
 */
 
void CirrusBLTColorExpand8x8PatternFill(dstAddr, fgcolor, bgcolor,
fillWidth, fillHeight, dstPitch, rop, patternword1, patternword2)
     unsigned int dstAddr;
     int fgcolor, bgcolor;
     int fillHeight, fillWidth, dstPitch;
     int rop;
     unsigned long patternword1, patternword2;
{
  unsigned int srcAddr;
  int i;
  pointer pDst;
  extern int CirrusMemTop;

  if (!HAVE543X() && fillHeight > 1024) {
      /* Split into two for 5426 & 5428. */
      CirrusBLTColorExpand8x8PatternFill(dstAddr, fgcolor, bgcolor,
          fillWidth, 1024, dstPitch, rop, patternword1, patternword2);
      CirrusBLTColorExpand8x8PatternFill(dstAddr + dstPitch * 1024,
          fgcolor, bgcolor, fillWidth, fillHeight - 1024, dstPitch, rop,
          patternword1, patternword2);
      return;
  }

  /* NOTE:
   * I tried System memory source pattern fill, it's not explicitly
   * disallowed (nor documented) in the databook, but it doesn't work. 
   */

  /* Pattern fill with color expanded video memory source. */

  /* Write 8 byte monochrome pattern. */
  /* Because of 16K bank granularity and 32K window, we don't have to */
  /* check for bank boundaries. */
  srcAddr = CirrusMemTop;
  setwritebank(CirrusMemTop >> 14);
  *(unsigned long *)(vgaBase + 0x8000 + (srcAddr & 0x3fff)) =
  	patternword1;
  *(unsigned long *)(vgaBase + 0x8000 + ((srcAddr & 0x3fff) + 4)) =
  	patternword2;

  SETDESTADDR(dstAddr);
  SETSRCADDR(srcAddr);
  SETDESTPITCH(dstPitch);
  SETWIDTH(fillWidth);
  SETHEIGHT(fillHeight);

  /* 8x8 Pattern Copy, screen to screen blt, forwards, color expand. */
  SETBLTMODE(PATTERNCOPY | COLOREXPAND);
  SETROP(rop);
  SETFOREGROUNDCOLOR(fgcolor);
  SETBACKGROUNDCOLOR(bgcolor);

  /* Do it. */
  STARTBLT();

  WAITUNTILFINISHED();

  SETFOREGROUNDCOLOR(0);
}


/*
 * 8x8 pattern fill.
 *
 * Regular 8x8 byte-per-pixel pattern.
 * 
 */

void CirrusBLT8x8PatternFill(dstAddr, w, h, pattern, destpitch, rop)
     unsigned int dstAddr;
     int w, h, destpitch;
     int rop;
     void *pattern;
{
  unsigned int srcAddr;
  int i;
  pointer pDst;
  extern int CirrusMemTop;

  if (!HAVE543X() && h > 1024) {
      /* Split into two for 5426 & 5428. */
      CirrusBLT8x8PatternFill(dstAddr, w, 1024, pattern, destpitch, rop);
      CirrusBLT8x8PatternFill(dstAddr + destpitch * 1024, w, h - 1024,
          pattern, destpitch, rop);
      return;
  }

  /* Pattern fill with video memory source. */

  /* Write 64 byte pattern. */
  /* Because of 16K bank granularity and 32K window, we don't have to */
  /* check for bank boundaries. */
  srcAddr = CirrusMemTop;
  setwritebank(srcAddr >> 14);
  memcpy(vgaBase + 0x8000 + (srcAddr & 0x3fff), pattern, 64);

  SETDESTADDR(dstAddr);
  SETSRCADDR(srcAddr);
  SETDESTPITCH(destpitch);
  SETWIDTH(w);
  SETHEIGHT(h);

  /* 8x8 Pattern Copy, screen to screen blt, forwards. */
  SETBLTMODE(PATTERNCOPY);
  SETROP(rop);

  /* Do it. */
  STARTBLT();

  WAITUNTILFINISHED();
}


/*
 * 16x16 pattern fill.
 *
 * Uses two 16x8 byte-per-pixel pattern fills (meant for 8x8 16-bit pixel
 * fill), vertically interleaved.
 * 
 */

void CirrusBLT16x16PatternFill(dstAddr, w, h, pattern, destpitch, rop)
     unsigned int dstAddr;
     int w, h, destpitch;
     int rop;
     unsigned char *pattern;
{
  unsigned int srcAddr;
  int i;
  pointer pDst;
  int blith, blitpitch;
  extern int CirrusMemTop;

  if (!HAVE543X() && h > 1024) {
      /* Split into two for 5426 & 5428. */
      CirrusBLT16x16PatternFill(dstAddr, w, 1024, pattern, destpitch, rop);
      CirrusBLT16x16PatternFill(dstAddr + destpitch * 1024, w, h - 1024,
          pattern, destpitch, rop);
      return;
  }

  /* Pattern fill with video memory source. */

  /* Write 128 byte pattern (even lines). */
  /* Because of 16K bank granularity and 32K window, we don't have to */
  /* check for bank boundaries. */
  srcAddr = CirrusMemTop;
  setwritebank(srcAddr >> 14);
  for (i = 0; i < 8; i++)
      memcpy(vgaBase + 0x8000 + (srcAddr & 0x3fff) + i * 16,
          pattern + i * 32, 16);

  blitpitch = destpitch * 2;
  blith = (h / 2) + (h & 1);

  SETDESTADDR(dstAddr);
  SETSRCADDR(srcAddr);
  SETDESTPITCH(blitpitch);
  SETWIDTH(w);
  SETHEIGHT(blith);

  /* 8x8 Pattern Copy, 16-bit pixels, screen to screen blt, forwards. */
  SETBLTMODE(PATTERNCOPY | PIXELWIDTH16);
  SETROP(rop);

  /* Do it. */
  STARTBLT();

  WAITUNTILFINISHED();

  /* Now do uneven lines. */
  /* Write pattern. */
  for (i = 0; i < 8; i++)
	  memcpy(vgaBase + 0x8000 + (srcAddr & 0x3fff) + i * 16,
	  	pattern + i * 32 + 16, 16);

  dstAddr += destpitch;
/*  blitpitch = destpitch * 2; */
  blith = (h / 2);

  SETDESTADDR(dstAddr);
  SETSRCADDR(srcAddr);
/* SETDESTPITCH(blitpitch); */
  SETWIDTH(w);
  SETHEIGHT(blith);

  /* 8x8 Pattern Copy, 16-bit pixels, screen to screen blt, forwards. */
/*  SETBLTMODE(PATTERNCOPY | PIXELWIDTH); */
/*  SETROP(rop); */

  STARTBLT();

  WAITUNTILFINISHED();
}

#if 0

/*
 * 32x32 pattern fill for 5434.
 *
 * Uses four 32x8 byte-per-pixel pattern fills (meant for 8x8 32-bit pixel
 * fill), vertically interleaved.
 * This is speculative, and untested. I haven't checked with any official
 * register docs.
 *
 * Something like this would also work on the 5426/28 (by filling vertical
 * bands 16 pixels wide, 4-way interleaved) if the virtual screen width is
 * less than 1024 (i.e. rarely).
 */

void CirrusBLT32x32PatternFill(dstAddr, w, h, pattern, destpitch, rop)
     unsigned int dstAddr;
     int w, h, destpitch;
     int rop;
     void *pattern;
{
  unsigned int srcAddr;
  int i, k;
  int blith, blitpitch;
  extern int CirrusMemTop;

  /* No need to split into two for 5434 (handles heights up to 2048). */

  /* Pattern fill with video memory source. */

  /* Set up write bank for writing pattern. */
  srcAddr = CirrusMemTop;
  setwritebank(srcAddr >> 14);

  /* Set up BLT parameters that remain constant. */
  blitpitch = destpitch * 4;	/* Four-way interleaving. */
  SETDESTPITCH(blitpitch);
  /* 8x8 Pattern Copy, 32-bit pixels, screen to screen blt, forwards. */
  SETBLTMODE(PATTERNCOPY | PIXELWIDTH32);
  SETROP(rop);

  for (k = 0; k < 4; k++) {
      /* Do the lines for which [index % 4 == k]. */
      /* Write 32x8 pattern. */
      for (i = 0; i < 8; i++)
          memcpy(vgaBase + 0x8000 + (srcAddr & 0x3fff) + i * 32,
	  	pattern + i * 32 * 4 + 32 * k, 32);
      blith = h / 4;
      if (h & 3 > k)
  	  blith++;
      SETDESTADDR(dstAddr);
      SETSRCADDR(srcAddr);
      SETWIDTH(w);
      SETHEIGHT(blith);
      STARTBLT();
      WAITUNTILFINISHED();

      dstAddr += destpitch;
  }
}

#endif


/*
 * BitBLT. Direction can be forwards or backwards. Can be modified to
 * support special rops.
 */

void CirrusBLTBitBlt(dstAddr, srcAddr, dstPitch, srcPitch, w, h, dir)
     unsigned int dstAddr, srcAddr;
     int dstPitch, srcPitch;
     int w, h;
     int dir;			/* >0, increase adrresses, <0, decrease */
{
  if ((!HAVE543X()) && h > 1024) {
     /* Split into two. */
     if (dir > 0) {
         CirrusBLTBitBlt(dstAddr, srcAddr, dstPitch, srcPitch, w, 1024, dir);
         CirrusBLTBitBlt(dstAddr + dstPitch * 1024, srcAddr + srcPitch * 1024,
             dstPitch, srcPitch, w, h - 1024, dir);
     }
     else {
         CirrusBLTBitBlt(dstAddr, srcAddr, dstPitch, srcPitch, w, 1024, dir);
         CirrusBLTBitBlt(dstAddr - dstPitch * 1024, srcAddr - srcPitch * 1024,
             dstPitch, srcPitch, w, h - 1024, dir);
     }
     return;
  }

  SETSRCADDR(srcAddr);
  SETSRCPITCH(srcPitch);
  SETDESTADDR(dstAddr);
  SETDESTPITCH(dstPitch);
  SETWIDTH(w);
  SETHEIGHT(h);

  if (dir > 0) {
      SETBLTMODE(FORWARDS);
  }
  else {
      SETBLTMODE(BACKWARDS);
  }

  SETROP(CROP_SRC);

  STARTBLT();
  WAITUNTILFINISHED();
}


void CirrusBLTWaitUntilFinished() {
  int count, timeout;
  count = 0;
  timeout = 0;
  outb(0x3CE, 0x31);
  while (inb(0x3CF) & 1) {
    count++;
    if (count == 10000000) {
    	ErrorF("Cirrus: BitBLT engine time-out.\n");
    	*(unsigned long *)vgaBase = 0;
    	count = 9990000;
    	timeout++;
    	if (timeout == 8)
    		return;
    }
  }
}
