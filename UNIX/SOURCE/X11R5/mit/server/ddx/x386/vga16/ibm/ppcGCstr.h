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
		Copyright IBM Corporation 1988

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $XFree86: mit/server/ddx/x386/vga16/ibm/ppcGCstr.h,v 2.0 1993/08/19 16:08:47 dawes Exp $ */
/* Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcGCstr.h,v 1.2 89/04/29 21:25:43 jeff Exp Locker: jeff */
/* Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcGCstr.h,v */

typedef struct {
    unsigned long	planemask ;
    unsigned long	fgPixel ;
    unsigned long	bgPixel ;
    int			alu ;
    int			fillStyle ;
    } ppcReducedRrop ;

/* ************************************************************************ */

/* private field of GC */
typedef struct {
/* The Next eleven (11) fields MUST CORRESPOND to
 * the fields of a "mfbPrivGC" struct
 * ----- BEGINNING OF "DO-NOT-CHANGE" REGION -----
 */
    unsigned char	rop ;		/* reduction of rasterop to 1 of 3 */
    unsigned char	ropOpStip ;	/* rop for opaque stipple */
    unsigned char	ropFillArea ;	/*  == alu, rop, or ropOpStip */
    unsigned	fExpose:1 ;		/* callexposure handling ? */
    unsigned	freeCompClip:1 ;
    PixmapPtr	pRotatedPixmap ;	/* tile/stipple  rotated to align */
    RegionPtr	pCompositeClip ;		/* free this based on freeCompClip
					   flag rather than NULLness */
    void 	(* FillArea)() ;		/* fills regions; look at the code */
/* ----- END OF "DO-NOT-CHANGE" REGION ----- */
    ppcReducedRrop	colorRrop ;
    short lastDrawableType ;	/* was last drawable a window or a pixmap? */
    short lastDrawableDepth ;	/* was last drawable 1 or 8 planes? */
    void (* cachedIGBlt)();	/* cached image glyph blit routine */
    void (* cachedPGBlt)();	/* cached poly glyph blit routine */
    pointer devPriv ;		/* Private area for device specific stuff */
    } ppcPrivGC ;
typedef ppcPrivGC *ppcPrivGCPtr ;
