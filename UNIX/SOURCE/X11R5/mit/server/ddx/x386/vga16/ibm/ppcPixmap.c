/*
 * Copyright IBM Corporation 1987,1988,1989
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/

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

/* $XFree86: mit/server/ddx/x386/vga16/ibm/ppcPixmap.c,v 2.0 1993/08/19 16:08:55 dawes Exp $ */

#include "pixmapstr.h"
#include "servermd.h"
#include "mfb.h"

#include "OScompiler.h"
#include "ibmTrace.h"

PixmapPtr
ppcCreatePixmap( pScreen, width, height, depth )
    ScreenPtr	pScreen ;
    int		width ;
    int		height ;
    int		depth ;
{
    register PixmapPtr pPixmap ;
    int size ;

    TRACE(("ppcCreatePixmap(pScreen=0x%x, width=%d, height=%d, depth=%d)\n", pScreen, width, height, depth)) ;

    if ( depth > 8 )
	return (PixmapPtr) NULL ;

    size = PixmapBytePad(width, depth);
    pPixmap = (PixmapPtr) Xalloc( sizeof (PixmapRec) + (height * size) ) ;
    if ( !pPixmap )
	return (PixmapPtr) NULL ;
    pPixmap->drawable.type = DRAWABLE_PIXMAP ;
    pPixmap->drawable.class = 0 ;
    pPixmap->drawable.pScreen = pScreen ;
    pPixmap->drawable.depth = depth ;
    pPixmap->drawable.id = 0 ;
    pPixmap->drawable.bitsPerPixel = ( depth == 1 ) ? 1 : 8 ;
    pPixmap->drawable.serialNumber = NEXT_SERIAL_NUMBER ;
    pPixmap->drawable.x = 0 ;
    pPixmap->drawable.y = 0 ;
    pPixmap->drawable.width = width ;
    pPixmap->drawable.height = height ;
    pPixmap->devKind = size;
    pPixmap->refcnt = 1 ;
    size = height * pPixmap->devKind ;

    pPixmap->devPrivate.ptr = (pointer) (pPixmap + 1);
    bzero( (char *) pPixmap->devPrivate.ptr, size ) ;
    return pPixmap ;
}

PixmapPtr
ppcCopyPixmap(pSrc)
    register PixmapPtr	pSrc;
{
    register PixmapPtr	pDst;
    int		size;

    TRACE(("ppcCopyPixmap(pSrc=0x%x)\n", pSrc)) ;
    size = pSrc->drawable.height * pSrc->devKind;
    pDst = (PixmapPtr)xalloc(sizeof(PixmapRec) + size);
    if (!pDst)
	return NullPixmap;
    pDst->drawable = pSrc->drawable;
    pDst->drawable.id = 0;
    pDst->drawable.serialNumber = NEXT_SERIAL_NUMBER;
    pDst->devKind = pSrc->devKind;
    pDst->refcnt = 1;
    pDst->devPrivate.ptr = (pointer)(pDst + 1);
    MOVE( (char *)pSrc->devPrivate.ptr, (char *)pDst->devPrivate.ptr, size ) ;
    return pDst;
}

/* 
   see doc in server/ddx/mfb/mfbpixmap.c
*/
void
ppcPadPixmap(pPixmap)
    PixmapPtr pPixmap ;
{
    TRACE(("ppcPadPixmap(pPixmap=0x%x)\n", pPixmap)) ;
if ( pPixmap->drawable.depth == 1 )
	mfbPadPixmap( pPixmap ) ;
}

/* ppcdebugging routine -- 
*/
#ifdef DEBUG
void ppcDumpPixmap(pPix)
PixmapPtr pPix ;
{ 
unsigned char *psrc, *pline ;
int i, j ;
pline = pPix->devPrivate ;
for ( i = 0 ; i < pPix->height ; i++ )
	{
	psrc = pline ;
	for ( j = 0 ; j < pPix->width ; j++ )
		{
		ErrorF("%02x ",*psrc++) ;
		}
	ErrorF("\n") ;
	pline = pline + pPix->devKind ;
	}
return ;
}
#endif

/* Rotates deep pixmap pPix by w pixels to the right on the screen. Assumes 
 * that words are 32 bits wide, and that the least significant bit appears on 
 * the left.
 */
void ppcRotatePixmap(pPix, rw)
    PixmapPtr	pPix ;
    register int rw ;
{
    unsigned char *pTempSrc, *pTempDst, *pRealDst ;
    int		depth, h, i, wid, loc ;

    TRACE(("ppcRotatePixmap(pPix=0x%x, rw=%d)\n", pPix, rw)) ;
    if (pPix == NullPixmap)
        return ;
    depth = pPix->drawable.depth ;
    if ( depth == 1 ) {
	mfbXRotatePixmap(pPix,rw) ;
	return ;
    }
    if ( depth > 8 ) {
	ErrorF("ppcRotatePixmap: too deep (%d)\n",depth) ;
	return ;
    }

    wid = pPix->drawable.width ;
    rw %= wid ;
    h = pPix->drawable.height ;
    pTempDst = (unsigned char *) ALLOCATE_LOCAL(wid) ;
    pTempSrc = pRealDst = (unsigned char *) pPix->devPrivate.ptr ;
    while ( h-- ) {
	for ( i = 0 ; i < wid ; i++ ) {
		loc = i + rw ;
		if ( loc >= wid )
		     loc -= wid ;
		pTempDst[loc] = *pTempSrc++ ;
	}
	MOVE( (char *) pTempDst, (char *) pRealDst, wid) ;
	pRealDst += wid ;
    }
    DEALLOCATE_LOCAL(pTempDst) ;
    return ;
}
