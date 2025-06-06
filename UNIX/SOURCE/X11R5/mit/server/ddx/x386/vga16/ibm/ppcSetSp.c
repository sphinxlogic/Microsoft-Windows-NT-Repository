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

/* $XFree86: mit/server/ddx/x386/vga16/ibm/ppcSetSp.c,v 2.1 1994/02/01 13:54:32 dawes Exp $ */

#include "X.h"
#include "Xmd.h"

#include "servermd.h"

#include "misc.h"
#include "regionstr.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"

#include "OScompiler.h"
#include "ppc.h"
extern int mfbGCPrivateIndex ;

/* SetScanline -- copies the bits from psrc to the drawable starting at
 * (xStart, y) and continuing to (xEnd, y).  xOrigin tells us where psrc 
 * starts on the scanline. (I.e., if this scanline passes through multiple
 * boxes, we may not want to start grabbing bits at psrc but at some offset
 * further on.) 
 */
static void
ppcSetScanline( pixCount, psrc, pdst, pm, alu )
    register int		pixCount;	/* width of scanline in bits */
    register unsigned char	*psrc;
    register unsigned char	*pdst;		/* where to put the bits */
    register const int		pm;		/* plane mask */
    const int			alu;		/* raster op */
{
register const int npm = ~pm ;	/* inverted plane mask */
register char tmpx ;

switch ( alu ) {
	case GXclear:		/* 0x0 Zero 0 */
		while ( pixCount-- )
			*pdst++ &= npm ;
		break ;
	case GXand:		/* 0x1 src AND dst */
		while ( pixCount-- )
			*pdst++ &= *psrc++ | npm ;
		break ;
	case GXandReverse:	/* 0x2 src AND NOT dst */
		for ( ; pixCount-- ; pdst++, psrc++ ) {
			tmpx = *pdst;
			*pdst = ( tmpx & npm ) | ( pm & *psrc & ~tmpx ) ;
		}
		break ;
	case GXcopy:		/* 0x3 src */
		for ( ; pixCount-- ; pdst++, psrc++ )
			*pdst = ( *pdst & npm ) | ( pm & *psrc ) ;
		break ;
	case GXandInverted:	/* 0x4 NOT src AND dst */
		while ( pixCount-- )
			*pdst++ &= npm | ~*psrc++ ;
		break ;
	case GXnoop:		/* 0x5 dst */
		break ;
	case GXxor:		/* 0x6 src XOR dst */
		while ( pixCount-- )
			*pdst++ ^= pm & *psrc++ ;
		break ;
	case GXor:		/* 0x7 src OR dst */
		while ( pixCount-- )
			*pdst++ |= *psrc++ & pm ;
		break ;
	case GXnor:		/* 0x8 NOT src AND NOT dst */
		for ( ; pixCount-- ; pdst++, psrc++ ) {
			tmpx = *pdst;
			*pdst = ( tmpx & npm ) | ( pm & ~( tmpx | *psrc ) ) ;
		}
		break ;
	case GXequiv:		/* 0x9 NOT src XOR dst */
		while ( pixCount-- )
			*pdst++ ^= pm & ~ *psrc++ ;
		break ;
	case GXorReverse:	/* 0xb src OR NOT dst */
		for ( ; pixCount-- ; pdst++, psrc++ ) {
			tmpx = *pdst;
			*pdst = ( tmpx & npm ) | ( pm & ( *psrc | ~tmpx ) ) ;
		}
		break ;
	case GXinvert:		/* 0xa NOT dst */
		while ( pixCount-- )
			*pdst++ ^= pm ;
		break ;
	case GXcopyInverted:	/* 0xc NOT src */
		for ( ; pixCount-- ; pdst++, psrc++ )
			*pdst = ( *pdst & npm ) | ( pm & ~ *psrc ) ;
		break ;
	case GXorInverted:	/* 0xd NOT src OR dst */
		while ( pixCount-- )
			*pdst++ |= pm & ~ *psrc++ ;
		break ;
	case GXnand:		/* 0xe NOT src OR NOT dst */
		for ( ; pixCount-- ; pdst++, psrc++ ) {
			tmpx = *pdst;
			*pdst = ( tmpx & npm ) | ( pm & ~( tmpx & *psrc ) ) ;
		}
		break ;
	case GXset:		/* 0xf 1 */
		while ( pixCount-- )
			*pdst++ |= pm ;
		break ;
	default:
		ErrorF( "ppcSetScanLine: bad alu value == 0x%02X\n", alu ) ;
		break ;
}

return ;
}

/* SetSpans -- for each span copy pwidth[i] bits from psrc to pDrawable at
 * ppt[i] using the raster op from the GC.  If fSorted is TRUE, the scanlines
 * are in increasing Y order.
 * Source bit lines are server scanline padded so that they always begin
 * on a word boundary.
 */ 
void
ppcSetSpans( pDrawable, pGC, psrc, ppt, pwidth, nspans, fSorted )
    register const DrawablePtr	pDrawable ;
    const GCPtr			pGC ;
    unsigned char		*psrc ;
    register DDXPointPtr	ppt ;
    int				*pwidth ;
    int				nspans ;
    int				fSorted ;
{
    unsigned char	*pdstBase ;	/* start of dst bitmap */
    int 		widthDst ;	/* width of bitmap in words */
    register BoxPtr 	pbox, pboxLast, pboxTest ;
    register DDXPointPtr pptLast ;
    RegionPtr 		prgnDst ;
    register int	width ;
    int			xStart, xEnd ;
    int			yMax ;
    int			alu ;
    int			pm ;

    extern void mfbSetSpans() ;

    /* allow for 1-deep windows on nfb machines (eg apa8, aed) */
    if ( ( pDrawable->depth == 1 ) && ( pDrawable->type == DRAWABLE_PIXMAP ) ) {
	mfbSetSpans( pDrawable, pGC, psrc, ppt, pwidth, nspans, fSorted ) ;
	return ;
    }

    if ( !( pm = pGC->planemask & ~( (~0) << pDrawable->depth ) )
      || ( ( alu = pGC->alu ) == GXnoop ) )
	return ;

    prgnDst = ( (ppcPrivGC *) (pGC->devPrivates[mfbGCPrivateIndex].ptr) )->pCompositeClip ;

    if ( ! REGION_NUM_RECTS(prgnDst))
	return ;

    pboxLast = ( pbox = REGION_RECTS(prgnDst) ) + REGION_NUM_RECTS(prgnDst);
    pptLast = ppt + nspans ;

    if ( pDrawable->type == DRAWABLE_WINDOW ) {
	yMax = (int) pDrawable->height + pDrawable->y ;
    }
    else {
	pdstBase = (unsigned char *) ( (PixmapPtr) pDrawable )->devPrivate.ptr ;
	widthDst = (int) ( (PixmapPtr) pDrawable )->devKind ;
	yMax = pDrawable->height ;
    }

    if ( fSorted ) {
    /* scan lines sorted in ascending order. Because they are sorted, we
     * don't have to check each scanline against each clip box.  We can be
     * sure that this scanline only has to be clipped to boxes at or after the
     * beginning of this y-band 
     */
	for ( pboxTest = pbox ;
	      ( ppt < pptLast ) && ( ppt->y < yMax ) ;
	      ppt++, pwidth++,
	      psrc += PixmapBytePad( width, pDrawable->depth ) ) {
	    width = *pwidth ;
	    for ( pbox = pboxTest ;
	          pbox < pboxLast ;
		  pbox++ ) {
		if ( pbox->y2 <= ppt->y ) {
		    /* clip box is before scanline */
		    pboxTest = pbox + 1 ;
		}
		else if ( ( pbox->y1 > ppt->y )
		       || ( pbox->x1 > ppt->x + width ) )
		    break ; /* scanline before clip box or left of clip box */
		else if ( pbox->x2 > ppt->x ) {
		    /* some of the scanline is in the current clip box */
		    xStart = MAX( pbox->x1, ppt->x ) ;
		    xEnd = MIN( ppt->x + width, pbox->x2 ) ;
		    if ( pDrawable->type == DRAWABLE_PIXMAP )
			    ppcSetScanline( xEnd - xStart,
					    psrc + ( xStart - ppt->x ),
					    pdstBase + xStart
					    + ( ppt->y * widthDst ),
				            pm, alu ) ;
		    else
			vgaDrawColorImage
			    ( xStart, ppt->y, xEnd - xStart, 1,
			      psrc + ( xStart - ppt->x ), xEnd - xStart,
			      alu, pm ) ;
		    if ( ppt->x + width <= pbox->x2 )
		        break ; /* End of the line, as it were */
		}
	    }
	    /* We've tried this line against every box ; it must be outside them
	     * all.  move on to the next point */
	}
    }
    else {
    /* scan lines not sorted. We must clip each line against all the boxes */
	for ( ;
	      ppt < pptLast ;
	      ppt++, pwidth++,
	      psrc += PixmapBytePad( width, pDrawable->depth ) ) {
	    width = *pwidth ;
	    if ( ppt->y >= 0 && ppt->y < yMax ) {
		for ( pbox = REGION_RECTS(prgnDst) ; pbox < pboxLast ; pbox++ ) {
		    if ( pbox->y1 > ppt->y )
			break ; /* rest of clip region is above this scanline */
		    else if ( ( pbox->y2 > ppt->y )
			   && ( pbox->x1 <= ppt->x + width )
			   && ( pbox->x2 > ppt->x ) ) {
			xStart = MAX( pbox->x1, ppt->x ) ;
			xEnd = MIN( pbox->x2, ppt->x + width ) ;
    			if ( pDrawable->type == DRAWABLE_PIXMAP )
				ppcSetScanline( xEnd - xStart,
						psrc + ( xStart - ppt->x ),
						/* ^ GJA */
						( ( pdstBase
						 + ( ppt->y * widthDst ) )
						 + xStart ),
						pm, alu ) ;
    			else	/* pDrawable->type == DRAWABLE_WINDOW */
			    vgaDrawColorImage
				( xStart, ppt->y, xEnd - xStart, 1,
				  psrc + ( xStart - ppt->x ), xEnd - xStart,
					/* GJA ^ */
				  alu, pm ) ;
		    }

		}
	    }
	}
    }
    return ;
}
