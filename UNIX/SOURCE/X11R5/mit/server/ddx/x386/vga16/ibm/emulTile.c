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

/* $XFree86: mit/server/ddx/x386/vga16/ibm/emulTile.c,v 2.1 1994/02/01 13:54:05 dawes Exp $ */
/* Header: /andrew/X11/r3src/release/server/ddx/ibm/ppc/RCS/emulTile.c,v 30.0 89/01/23 19:28:31 paul Exp */
/* Source: /andrew/X11/r3src/release/server/ddx/ibm/ppc/RCS/emulTile.c,v */

/* ppc Tile
 * P. Shupak 11/87
 * Modified From original ppc Tile
 * T. Paquin 9/87
 * Uses private imageFill a bunch of times
 */

#include "X.h"
#include "pixmapstr.h"
#include "pixmap.h"
#include "scrnintstr.h"
#include "gcstruct.h"
#include "gc.h"

#include "OScompiler.h"

/* #include "ppcGCstr.h" */
#include "windowstr.h"	/* GJA */
/* #include "ppcScrn.h" */
#include "ppc.h"	/* GJA */

#include "ibmTrace.h"

extern void ErrorF() ;

static void
DrawFirstTile( pTile, x, y, w, h, alu, planes, xOffset, yOffset )
register PixmapPtr pTile ;
register int x ;
register int y ;
int w ;
int h ;
int alu ;
unsigned long int planes ;
int xOffset ;
int yOffset ;
{
register int htarget ;
register int vtarget ;

	if ( xOffset ) { /* Not X-Aligned */
		if ( yOffset ) { /* Nor Y-Aligned */
			htarget = MIN( pTile->drawable.width - xOffset, w ),
			vtarget = MIN( pTile->drawable.height - yOffset, h ),
			yOffset *= pTile->devKind ;
			vgaDrawColorImage( x, y,
				 htarget,
				 vtarget,
				 pTile->devPrivate.ptr + yOffset + xOffset,
				 pTile->devKind,
				 alu, planes ) ;
			if ( w > htarget ) {
				w = MIN( w, pTile->drawable.width ) ;
				if ( h > vtarget ) {
					h = MIN( h, pTile->drawable.height ) ;
					vgaDrawColorImage( x, y + vtarget,
						 htarget,
						 h - vtarget,
						 pTile->devPrivate.ptr + xOffset,
						 pTile->devKind,
						 alu, planes ) ;
					vgaDrawColorImage( x + htarget, y,
						 w - htarget,
						 vtarget,
						 pTile->devPrivate.ptr + yOffset,
						 pTile->devKind,
						 alu, planes ) ;
					vgaDrawColorImage( x + htarget,
						 y + vtarget,
						 w - htarget,
						 h - vtarget,
						 pTile->devPrivate.ptr,
						 pTile->devKind,
						 alu, planes ) ;
				}
				else { /* h <= vtarget */
					vgaDrawColorImage( x + htarget, y,
						 w - htarget,
						 vtarget,
						 pTile->devPrivate.ptr + yOffset,
						 pTile->devKind,
						 alu, planes ) ;
				}
			}
			else if ( h > vtarget ) {
				vgaDrawColorImage( x, y + vtarget,
					 htarget,
					 MIN( h, pTile->drawable.height ) - vtarget,
					 pTile->devPrivate.ptr + xOffset,
					 pTile->devKind,
					 alu, planes ) ;
				vtarget = pTile->drawable.height ;
			}
		}
		else { /* No Y Offset */
			vgaDrawColorImage( x, y,
				 htarget = MIN( pTile->drawable.width - xOffset, w ),
				 vtarget = MIN( pTile->drawable.height, h ),
				 pTile->devPrivate.ptr + xOffset,
				 pTile->devKind,
				 alu, planes ) ;
			if ( w > htarget ) {
				vgaDrawColorImage( x + htarget, y,
					 MIN( pTile->drawable.width, w ) - htarget,
					 vtarget,
					 pTile->devPrivate.ptr,
					 pTile->devKind,
					 alu, planes ) ;
			}
		}
	}
	else if ( yOffset ) {
		vgaDrawColorImage( x, y,
			 htarget = MIN( pTile->drawable.width, w ),
			 vtarget = MIN( pTile->drawable.height - yOffset, h ),
			 pTile->devPrivate.ptr + ( yOffset * pTile->devKind ),
			 pTile->devKind,
			 alu, planes ) ;
		if ( h > vtarget ) {
			vgaDrawColorImage( x, y + vtarget,
				 htarget,
				 MIN( pTile->drawable.height, h ) - vtarget,
				 pTile->devPrivate.ptr,
				 pTile->devKind,
				 alu, planes ) ;
		}
	}
	else { /* NO Offset */
		vgaDrawColorImage( x, y,
			 htarget = MIN( pTile->drawable.width, w ),
			 vtarget = MIN( pTile->drawable.height, h ),
			 pTile->devPrivate.ptr,
			 pTile->devKind,
			 alu, planes ) ;
	}

	return ;
}

/* GJA --
 * After finding three kinds of errors in this single function,
 * (requiring modifications to be made at at least 10 places,
 * I decided to REWRITE the body of the ppcTileRect function from scratch.
 * This version simply computes all relevant margins in advance, and does
 * not try to reuse temporary variables. I leave that to the compiler.
 * (This was a maintenance and robustness nightmare anyway.)
 * The code is pretty obvious: all margins, coordinates, and numbers of tiles
 * are computed before drawing starts.
 * Notice that the margins consist of incompletely drawn tiles. Therefore
 * we need offsets in the data for the left and upper margins.
 * The right and lower margins are also incomplete, but start at offset 0
 * in the data. They just end at awkward offsets. 
 * The center block, by definition, consists of fully drawn tiles.
 * Perhaps we could leave out some if's. But why bother? It would decrease
 * robustness.
 */
void
ppcTileRect( pTile, alu, planes, x0, y0, w, h, xSrc, ySrc )
register PixmapPtr pTile ;
const int alu ;
const unsigned long int planes ;
register int x0, y0, w, h ;
int xSrc ;
int ySrc ;
{
ScreenPtr pScreen ;
int xOffset ;
int yOffset ;
int width, height;

TRACE( ( "ppcTileRect(pTile=x%x,alu=x%x,planes=x%02x,x0=%d,y0=%d,w=%d,h=%d,xSrc=%d,ySrc=%d\n",
		pTile, alu, planes, x0, y0, w, h, xSrc, ySrc ) ) ;

     pScreen = pTile->drawable.pScreen ;
     switch ( alu ) {
	case GXclear:		/* 0x0 Zero 0 */
	case GXinvert:		/* 0xa NOT dst */
	case GXset:		/* 0xf 1 */
		vgaFillSolid
			( 0xFF, alu, planes, x0, y0, w, h ) ;
	case GXnoop:		/* 0x5 dst */
		return ;
	default:
		break ;
}

    width = pTile->drawable.width;
    if ( ( xOffset = ( x0 - xSrc ) ) > 0 )
	xOffset %= width ;
    else
	xOffset = width - (( - xOffset ) % width ) ;
    if ( xOffset == width ) xOffset = 0; /* For else case */

    height = pTile->drawable.height;
    if ( ( yOffset = ( y0 - ySrc ) ) > 0 )
	yOffset %= height ;
    else
	yOffset = height - (( - yOffset ) % height ) ;
    if ( yOffset == height ) yOffset = 0; /* For else case */

     switch ( alu ) {
	case GXcopyInverted:	/* 0xc NOT src */
	case GXcopy:		/* 0x3 src */
		/* Special Case Code */
		DrawFirstTile( pTile, x0, y0, w, h,
			       alu, planes, xOffset, yOffset ) ;
		/* Here We Double The Size Of The BLIT Each Iteration */
		ppcReplicateArea(
			    x0, y0, planes, w, h,
			    MIN( w, pTile->drawable.width ), MIN( h, pTile->drawable.height ), pScreen ) ;
		break ;
	case GXnor:		/* 0x8 NOT src AND NOT dst */
	case GXandReverse:	/* 0x2 src AND NOT dst */
	case GXorReverse:	/* 0xb src OR NOT dst */
	case GXnand:		/* 0xe NOT src OR NOT dst */
	case GXandInverted:	/* 0x4 NOT src AND dst */
	case GXand:		/* 0x1 src AND dst */
	case GXequiv:		/* 0x9 NOT src XOR dst */
	case GXxor:		/* 0x6 src XOR dst */
	case GXorInverted:	/* 0xd NOT src OR dst */
	case GXor:		/* 0x7 src OR dst */
	default:
		{
		register unsigned char *data ;
		register int hcount, vcount ; /* Number of tiles in center */
		int xcount, ycount;	/* Temporaries */
		int x1, y1;	/* Left upper corner of center */
		int x2, y2;	/* Left upper corner of lower right margin */
		int leftmgn, rightmgn, topmgn, botmgn; /* Margins */

		int htarget, vtarget ;

		data = pTile->devPrivate.ptr;
	
		/* Compute the various sizes and coordinates. */
		leftmgn = MIN( w, width - xOffset ) ;
		x1 = x0 + leftmgn;
		topmgn = MIN( h, height - yOffset ) ;
		y1 = y0 + topmgn;

		rightmgn = (w - leftmgn) % width;
		hcount = (w - leftmgn) / width;
		x2 = x0 + w - rightmgn;
		botmgn = (h - topmgn) % height;
		vcount = (h - topmgn) / height;
		y2 = y0 + h - botmgn;
		
		/* We'll use yOffset as offset in data.
		 * This requires yOffset != height (ditto xOffset).
                 */
		yOffset *= pTile->devKind;

		/* Draw top margin, including corners */
		if ( topmgn ) {
			if ( leftmgn ) {
				vgaDrawColorImage( x0, y0, leftmgn, topmgn,
					 data + yOffset + xOffset,
					 pTile->devKind, alu, planes ) ;
			}
			for (	xcount = hcount, htarget = x1;
				xcount ;
				xcount--, htarget += width )
			{
				vgaDrawColorImage( htarget, y0, width, topmgn,
					 data + yOffset,
					 pTile->devKind, alu, planes ) ;
			}
			if ( rightmgn ) {
				vgaDrawColorImage( x2, y0, rightmgn, topmgn,
					 data + yOffset,
					 pTile->devKind, alu, planes ) ;
			}
		}
	
		/* Draw bottom margin, including corners */
		if ( botmgn ) {
			if ( leftmgn ) {
				vgaDrawColorImage( x0, y2, leftmgn, botmgn,
					 data + xOffset,
					 pTile->devKind, alu, planes ) ;
			}
			for (	xcount = hcount, htarget = x1;
				xcount ;
				xcount--, htarget += width )
			{
				vgaDrawColorImage( htarget, y2, width, botmgn,
					 data,
					 pTile->devKind, alu, planes ) ;
			}
			if ( rightmgn ) {
				vgaDrawColorImage( x2, y2, rightmgn, botmgn,
					 data,
					 pTile->devKind, alu, planes ) ;
			}
		}
	
		/* Draw left margin, excluding corners */
		if ( leftmgn ) {
			for (	ycount = vcount, vtarget = y1 ;
				ycount ;
				ycount--, vtarget += height )
			{
				vgaDrawColorImage( x0, vtarget, leftmgn, height,
					 data + xOffset,
					 pTile->devKind, alu, planes ) ;
			}
		}

		/* Draw right margin, excluding corners */
		if ( rightmgn ) {
			for (	ycount = vcount, vtarget = y1 ;
				ycount ;
				ycount--, vtarget += height )
			{
				vgaDrawColorImage( x2, vtarget, rightmgn, height,
					 data,
					 pTile->devKind, alu, planes ) ;
			}
		}

		/* Draw center consisting of full tiles */
		for (		ycount = vcount, vtarget = y1 ;
				ycount ;
				ycount--, vtarget += height )
		{
			for (	xcount = hcount, htarget = x1 ;
				xcount ;
				xcount--, htarget += width )
			{
				vgaDrawColorImage( htarget, vtarget, width, height,
					 data,
					 pTile->devKind, alu, planes ) ;
				
			}
		}
	} } /* Block + switch */
}
