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

/******************************************************************
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

/* $XFree86: mit/server/ddx/x386/vga16/ibm/ppcWindowFS.c,v 2.1 1994/02/01 13:54:36 dawes Exp $ */
/* Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcWindowFS.c,v 1.1 89/05/07 15:23:36 jeff Exp */
/* Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcWindowFS.c,v */

#include "X.h"
#include "misc.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include "mfb.h"
#include "maskbits.h"

#include "OScompiler.h"

#include "ppc.h"
#include "ppcSpMcro.h"

#include "ibmTrace.h"
extern int mfbGCPrivateIndex;

#define LeftMostBitInScreenLongWord SCRLEFT( 0xFFFFFFFF, 31 )
/*
********** ********** ********** ********** ********** ********** **********
   these routines all clip.  they assume that anything that has called
them has already translated the points (i.e. pGC->miTranslate is
non-zero, which is howit gets set in mfbCreateGC().)

   the number of new scanlines created by clipping ==
MaxRectsPerBand * nSpans.
********** ********** ********** ********** ********** ********** **********
*/

void
ppcSolidWindowFS( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted )
    DrawablePtr pDrawable ;
    GCPtr	pGC ;
    int		nInit ;			/* number of spans to fill */
    DDXPointPtr pptInit ;		/* pointer to list of start points */
    int		*pwidthInit ;		/* pointer to list of n widths */
    int 	fSorted ;
{
    register unsigned long int pm ;
    register unsigned long int fg ;
    register int alu ;
				/* next three parameters are post-clip */
    int n ;			/* number of spans to fill */
    register DDXPointPtr ppt ;	/* pointer to list of start points */
    register int *pwidth ;	/* pointer to list of n widths */
    int *pwidthFree ;		/* copies of the pointers to free */
    DDXPointPtr pptFree ;

    TRACE( ( "ppcSolidWindowFS(pDrawable=0x%x,pGC=0x%x,nInit=%d,pptInit=0x%x,pwidthInit=0x%x,fSorted=%d)\n",
	    pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted ) ) ;

    if ( pDrawable->type != DRAWABLE_WINDOW ) {
	ErrorF( "ppcSolidWindowFS: drawable is not a window\n") ;
	return ;
    }

    if ( ( alu = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.alu ) == GXnoop )
	return ;

    n = nInit * miFindMaxBand( ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->pCompositeClip ) ;
    if ( !( pwidth = (int *) ALLOCATE_LOCAL( n * sizeof( int ) ) ) )
	return ;
    pwidthFree = pwidth ;

    if ( !( ppt = (DDXPointRec *)
			ALLOCATE_LOCAL( n * sizeof( DDXPointRec ) ) ) ) {
	DEALLOCATE_LOCAL( pwidth ) ;
	return ;
    }
    pptFree = ppt ;

    n = miClipSpans( ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->pCompositeClip,
		     pptInit, pwidthInit, nInit, ppt, pwidth, fSorted ) ;

    pm = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.planemask ;
    fg = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.fgPixel ;

    for ( ; n-- ; ppt++, pwidth++ )
	if ( *pwidth )
	    vgaFillSolid( fg, alu, pm, ppt->x, ppt->y, *pwidth, 1 ) ;

    DEALLOCATE_LOCAL( pptFree ) ;
    DEALLOCATE_LOCAL( pwidthFree ) ;
    return ;
}

void
ppcStippleWindowFS( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted )
DrawablePtr pDrawable ;
register GC *pGC ;
int nInit ;			/* number of spans to fill */
DDXPointPtr pptInit ;		/* pointer to list of start points */
int *pwidthInit ;		/* pointer to list of n widths */
int fSorted ;
{
    register unsigned long int pm ;
    register unsigned long int fg ;
    register int alu ;
				/* next three parameters are post-clip */
    int n ;			/* number of spans to fill */
    register DDXPointPtr ppt ;	/* pointer to list of start points */
    register int *pwidth ;	/* pointer to list of n widths */
    PixmapPtr	pTile ;		/* pointer to tile we want to fill with */
    int xSrc ;
    int ySrc ;
    int *pwidthFree ;		/* copies of the pointers to free */
    DDXPointPtr pptFree ;

    TRACE( ( "ppcStippleWindowFS(pDrawable=0x%x,pGC=0x%x,nInit=%d,pptInit=0x%x,pwidthInit=0x%x,fSorted=%d)\n",
	   pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted ) ) ;

    if ( pDrawable->type != DRAWABLE_WINDOW ) {
	ErrorF( "ppcStippleWindowFS: drawable is not a window\n" ) ;
	return ;
    }

    if ( pGC->stipple->drawable.depth != 1 ) {
	ErrorF("ppcStippleFS: bad depth\ntype = %d, depth = %d\n",
		pDrawable->type, pGC->stipple->drawable.depth ) ;
	return ;
    }

    if ( ( alu = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.alu ) == GXnoop )
	return ;

    SETSPANPTRS( nInit, n, pwidthInit, pwidth, pptInit,
		 ppt, pwidthFree, pptFree, fSorted ) ;

    pm = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.planemask ;
    fg = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.fgPixel ;

    xSrc = pGC->patOrg.x + pDrawable->x ;
    ySrc = pGC->patOrg.y + pDrawable->y ;
    pTile = pGC->stipple ;

    for ( ; n-- ; ppt++, pwidth++ )
	vgaFillStipple( pTile, fg, alu, pm, ppt->x, ppt->y, *pwidth, 1, xSrc, ySrc ) ;

    DEALLOCATE_LOCAL( pptFree ) ;
    DEALLOCATE_LOCAL( pwidthFree ) ;

    return ;
}

void
ppcOpStippleWindowFS( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted )
DrawablePtr pDrawable ;
register GC *pGC ;
int nInit ;			/* number of spans to fill */
DDXPointPtr pptInit ;		/* pointer to list of start points */
int *pwidthInit ;		/* pointer to list of n widths */
int fSorted ;
{
    register DDXPointPtr ppt ;	/* pointer to list of start points */
    register int *pwidth ;	/* pointer to list of n widths */
    int n ;			/* number of spans to fill */
    int xSrc ;
    int ySrc ;
    unsigned long int pm ;
    unsigned long int fg, bg ;
    int alu ;
    int *pwidthFree ;		/* copies of the pointers to free */
    DDXPointPtr pptFree ;

    TRACE( ( "ppcOpStippleWindowFS(pDrawable=0x%x,pGC=0x%x,nInit=%d,pptInit=0x%x,pwidthInit=0x%x,fSorted=%d)\n",
	    pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted ) ) ;

    if ( pGC->stipple->drawable.depth != 1 ) {
	ErrorF( "ppcOpStippleWindowFS: bad depth\ntype = %d, depth = %d\n",
		pDrawable->type, pGC->stipple->drawable.depth ) ;
	return ;
    }

    if ( ( alu = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.alu ) == GXnoop )
	return ;

    SETSPANPTRS( nInit, n, pwidthInit, pwidth, pptInit,
		 ppt, pwidthFree, pptFree, fSorted ) ;

    pm = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.planemask ;
    fg = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.fgPixel ;
    bg = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.bgPixel ;

    xSrc = pGC->patOrg.x + pDrawable->x ;
    ySrc = pGC->patOrg.y + pDrawable->y ;

    for ( ; n-- ; ppt++, pwidth++ )
	ppcOpaqueStipple( pGC->stipple, fg, bg, alu, pm,
		ppt->x, ppt->y, *pwidth, 1,
		xSrc, ySrc ) ;

    DEALLOCATE_LOCAL( pptFree ) ;
    DEALLOCATE_LOCAL( pwidthFree ) ;
    return ;
}

void
ppcTileWindowFS( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted )
DrawablePtr pDrawable ;
register GC *pGC ;
int nInit ;			/* number of spans to fill */
DDXPointPtr pptInit ;		/* pointer to list of start points */
int *pwidthInit ;		/* pointer to list of n widths */
int fSorted ;
{
				/* next three parameters are post-clip */
    register DDXPointPtr ppt ;	/* pointer to list of start points */
    register int *pwidth ;	/* pointer to list of n widths */
    int n ;			/* number of spans to fill */
    unsigned char pm ;
    int alu ;
    int xSrc ;
    int ySrc ;
    int *pwidthFree ;		/* copies of the pointers to free */
    DDXPointPtr pptFree ;

    TRACE( ( "ppcTileWindowFS(pDrawable=0x%x,pGC=0x%x,nInit=%d,pptInit=0x%x,pwidthInit=0x%x,fSorted=%d)\n",
	    pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted ) ) ;

    if ( ( alu = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.alu ) == GXnoop )
	return ;

    SETSPANPTRS( nInit, n, pwidthInit, pwidth, pptInit,
		 ppt, pwidthFree, pptFree, fSorted ) ;

    xSrc = pGC->patOrg.x + pDrawable->x ;
    ySrc = pGC->patOrg.y + pDrawable->y ;
    pm = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.planemask ;

    for ( ; n-- ; ppt++, pwidth++ )
	    ppcTileRect( pGC->tile.pixmap, alu, pm,
		     ppt->x, ppt->y, *pwidth, 1, xSrc, ySrc ) ;

    DEALLOCATE_LOCAL( pptFree ) ;
    DEALLOCATE_LOCAL( pwidthFree ) ;
    return ;
}
