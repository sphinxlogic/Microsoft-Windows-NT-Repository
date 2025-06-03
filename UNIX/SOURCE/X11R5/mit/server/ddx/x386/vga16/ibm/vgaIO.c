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

/* $XFree86: mit/server/ddx/x386/vga16/ibm/vgaIO.c,v 2.0 1993/08/19 16:09:20 dawes Exp $ */

#include "X.h"
#include "resource.h"
#include "scrnintstr.h"
#include "servermd.h"	/* GJA */
#include "mi.h" /* GJA */

#include "OScompiler.h"

#include "vgaVideo.h"

#include "ibmTrace.h"

/* Global Variables */
extern int vgaDisplayTubeType ;
extern ScreenRec vgaScreenRec ;
extern VisualRec vgaVisuals[] ;

static int HardwareReady = 0 ;

Bool
vga16ScreenInit( pScreen )
register ScreenPtr const pScreen ;
{
	static int been_here = 0 ;

	TRACE( ( "vgaScreenInit(index=%d,pScreen=0x%x,argc=%d,argv=0x%x)\n",
		index, pScreen, argc, argv ) ) ;

	if ( !been_here ) {
/*		vgaInitFontCache() ;	MAYBE SOMEDAY */
		been_here = TRUE ;
	}

	vgaVisuals[0].class = ( vgaDisplayTubeType == COLOR_TUBE )
			    ? PseudoColor : GrayScale ;
	ppc16CommonScreenInit( pScreen, &vgaScreenRec ) ;

	return HardwareReady ;
}

/*ARGSUSED*/
Bool
vgaScreenClose( index, pScreen )
register const int index ;
register ScreenPtr const pScreen ;
{
TRACE( ( "vgaScreenClose(index=%d,pScreen=0x%x)\n", index, pScreen ) ) ;
ppcCommonScreenClose( pScreen, index, &vgaScreenRec ) ;
return 1 ;
}

void
Init16Output( pScreen, virtx, virty )
    ScreenPtr pScreen;
    int virtx, virty;
{
    extern vga16ScreenInit();

    vga16Probe(virtx,virty); /* For side effects. GJA */
    vga16ScreenInit(pScreen);

    mfbRegisterCopyPlaneProc(pScreen,miCopyPlane); /* GJA -- R4->R5 */
}
