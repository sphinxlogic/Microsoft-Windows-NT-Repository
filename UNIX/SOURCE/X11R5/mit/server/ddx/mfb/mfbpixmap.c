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

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $XFree86: mit/server/ddx/mfb/mfbpixmap.c,v 2.0 1993/07/24 07:13:42 dawes Exp $ */
/* $XConsortium: mfbpixmap.c,v 5.5 89/07/28 11:59:59 rws Exp $ */

/* pixmap management
   written by drewry, september 1986

   on a monchrome device, a pixmap is a bitmap.
*/

#include "Xmd.h"
#include "mfb.h"
#include "mi.h"

#include "pixmapstr.h"
#include "maskbits.h"

#include "servermd.h"

PixmapPtr
mfbCreatePixmap (pScreen, width, height, depth)
    ScreenPtr	pScreen;
    int		width;
    int		height;
    int		depth;
{
    register PixmapPtr pPixmap;
    int size;

    if (depth != 1)
	return NullPixmap;

    size = PixmapBytePad(width, 1);
    pPixmap = (PixmapPtr)xalloc(sizeof(PixmapRec) + (height * size));
    if (!pPixmap)
	return NullPixmap;
    pPixmap->drawable.type = DRAWABLE_PIXMAP;
    pPixmap->drawable.class = 0;
    pPixmap->drawable.pScreen = pScreen;
    pPixmap->drawable.depth = 1;
    pPixmap->drawable.bitsPerPixel = 1;
    pPixmap->drawable.id = 0;
    pPixmap->drawable.serialNumber = NEXT_SERIAL_NUMBER;
    pPixmap->drawable.x = 0;
    pPixmap->drawable.y = 0;
    pPixmap->drawable.width = width;
    pPixmap->drawable.height = height;
    pPixmap->slot = -1;
    pPixmap->cacheId = 0;
    pPixmap->devKind = size;
    pPixmap->refcnt = 1;
    pPixmap->devPrivate.ptr = (pointer)(pPixmap + 1);
    return pPixmap;
}

Bool
mfbDestroyPixmap(pPixmap)
    PixmapPtr pPixmap;
{
    if(--pPixmap->refcnt)
	return TRUE;
    xfree(pPixmap);
    return TRUE;
}


PixmapPtr
mfbCopyPixmap(pSrc)
    register PixmapPtr	pSrc;
{
    register PixmapPtr	pDst;
    int		size;

    size = pSrc->drawable.height * pSrc->devKind;
    pDst = (PixmapPtr)xalloc(sizeof(PixmapRec) + size);
    if (!pDst)
	return NullPixmap;
    pDst->drawable = pSrc->drawable;
    pDst->drawable.id = 0;
    pDst->drawable.serialNumber = NEXT_SERIAL_NUMBER;
    pDst->slot = -1;
    pDst->cacheId = 0;
    pDst->devKind = pSrc->devKind;
    pDst->refcnt = 1;
    pDst->devPrivate.ptr = (pointer)(pDst + 1);
    bcopy((char *)pSrc->devPrivate.ptr, (char *)pDst->devPrivate.ptr, size);
    return pDst;
}


/* replicates a pattern to be a full 32 bits wide.
   relies on the fact that each scnaline is longword padded.
   doesn't do anything if pixmap is not a factor of 32 wide.
   changes width field of pixmap if successful, so that the fast
	XRotatePixmap code gets used if we rotate the pixmap later.

   calculate number of times to repeat
   for each scanline of pattern
      zero out area to be filled with replicate
      left shift and or in original as many times as needed
*/
void
mfbPadPixmap(pPixmap)
    PixmapPtr pPixmap;
{
    register int width = pPixmap->drawable.width;
    register int h;
    register int mask;
    register unsigned int *p;
    register unsigned int bits;	/* real pattern bits */
    register int i;
    int rep;			/* repeat count for pattern */

    if (width >= 32)
	return;

    rep = 32/width;
    if (rep*width != 32)
	return;

    mask = endtab[width];

    p = (unsigned int *)(pPixmap->devPrivate.ptr);
    for (h=0; h < pPixmap->drawable.height; h++)
    {
	*p &= mask;
	bits = *p;
	for(i=1; i<rep; i++)
	{
	    bits = SCRRIGHT(bits, width);
	    *p |= bits;
	}
	p++;
    }
    pPixmap->drawable.width = 32;
}

/* Rotates pixmap pPix by w pixels to the right on the screen. Assumes that
 * words are 32 bits wide, and that the least significant bit appears on the
 * left.
 */
void
mfbXRotatePixmap(pPix, rw)
    PixmapPtr	pPix;
    register int rw;
{
    register long	*pw, *pwFinal;
    register unsigned long	t;

    if (pPix == NullPixmap)
        return;

    pw = (long *)pPix->devPrivate.ptr;
    rw %= (int)pPix->drawable.width;
    if (rw < 0)
	rw += (int)pPix->drawable.width;
    if(pPix->drawable.width == 32)
    {
        pwFinal = pw + pPix->drawable.height;
	while(pw < pwFinal)
	{
	    t = *pw;
	    *pw++ = SCRRIGHT(t, rw) | 
		    (SCRLEFT(t, (32-rw)) & endtab[rw]);
	}
    }
    else
    {
	/* We no longer do this.  Validate doesn't try to rotate odd-size
	 * tiles or stipples.  mfbUnnatural<tile/stipple>FS works directly off
	 * the unrotate tile/stipple in the GC
	 */
        ErrorF("X internal error: trying to rotate odd-sized pixmap.\n");
    }

}

/* Rotates pixmap pPix by h lines.  Assumes that h is always less than
   pPix->height
   works on any width.
 */
void
mfbYRotatePixmap(pPix, rh)
    register PixmapPtr	pPix;
    int	rh;
{
    int nbyDown;	/* bytes to move down to row 0; also offset of
			   row rh */
    int nbyUp;		/* bytes to move up to line rh; also
			   offset of first line moved down to 0 */
    char *pbase;
    char *ptmp;
    int	height;

    if (pPix == NullPixmap)
	return;
    height = (int) pPix->drawable.height;
    rh %= height;
    if (rh < 0)
	rh += height;

    pbase = (char *)pPix->devPrivate.ptr;

    nbyDown = rh * pPix->devKind;
    nbyUp = (pPix->devKind * height) - nbyDown;
    if(!(ptmp = (char *)ALLOCATE_LOCAL(nbyUp)))
	return;

    bcopy(pbase, ptmp, nbyUp);		/* save the low rows */
    bcopy(pbase+nbyUp, pbase, nbyDown);	/* slide the top rows down */
    bcopy(ptmp, pbase+nbyDown, nbyUp);	/* move lower rows up to row rh */
    DEALLOCATE_LOCAL(ptmp);
}

void
mfbCopyRotatePixmap(psrcPix, ppdstPix, xrot, yrot)
    register PixmapPtr psrcPix, *ppdstPix;
    int	xrot, yrot;
{
    register PixmapPtr pdstPix;

    if ((pdstPix = *ppdstPix) &&
	(pdstPix->devKind == psrcPix->devKind) &&
	(pdstPix->drawable.height == psrcPix->drawable.height))
    {
	bcopy((char *)psrcPix->devPrivate.ptr,
	      (char *)pdstPix->devPrivate.ptr,
	      psrcPix->drawable.height * psrcPix->devKind);
	pdstPix->drawable.width = psrcPix->drawable.width;
	pdstPix->drawable.serialNumber = NEXT_SERIAL_NUMBER;
	pdstPix->slot = -1;
	pdstPix->cacheId = 0;
    }
    else
    {
	if (pdstPix)
	    mfbDestroyPixmap(pdstPix);
	*ppdstPix = pdstPix = mfbCopyPixmap(psrcPix);
	if (!pdstPix)
	    return;
    }
    mfbPadPixmap(pdstPix);
    if (xrot)
	mfbXRotatePixmap(pdstPix, xrot);
    if (yrot)
	mfbYRotatePixmap(pdstPix, yrot);
}
