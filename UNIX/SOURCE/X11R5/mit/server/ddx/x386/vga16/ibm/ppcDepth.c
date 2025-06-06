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

/* $XFree86: mit/server/ddx/x386/vga16/ibm/ppcDepth.c,v 2.0 1993/08/19 16:08:43 dawes Exp $ */
/* Header: /andrew/X11/R3src/tape/server/ddx/ibm/ppc/RCS/ppcDepth.c,v 9.1 88/10/24 03:59:57 paul Exp */
/* Source: /andrew/X11/R3src/tape/server/ddx/ibm/ppc/RCS/ppcDepth.c,v */

/* Check to see if the alleged depth is acceptable for the Screen  
 *
 * T. Paquin 9/87
 *
 */

#include "X.h"
#include "scrnintstr.h"
#include "pixmapstr.h"

Bool
ppcDepthOK(pDraw,depth)
register DrawablePtr pDraw;
register int depth;
{
register ScreenPtr pScreen= pDraw->pScreen;
register int i = pScreen->numDepths;

    if ( ( pDraw->type == DRAWABLE_PIXMAP ) && ( depth == 1 ) )
	return TRUE ;

    while ( i-- )
	if ( depth == pScreen->allowedDepths[i].depth )
	    return TRUE ;

    return FALSE ;
}
