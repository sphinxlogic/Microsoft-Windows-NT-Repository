/* GJA -- this file was machine generated from the .c files */
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

Copyright 1987 by the Regents of the University of California
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

			All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.
******************************************************************/

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

/* $XFree86: mit/server/ddx/x386/vga16/ibm/vgaProcs.h,v 2.1 1993/09/23 15:49:07 dawes Exp $ */

#if __STDC__ || defined(__cplusplus)
#define P_(s) s
#else
#define P_(s) ()
#endif

/* vgaAddr.c */
/* vgaBitBlt.c */
void vgaBitBlt P_((const int alu, const int readplanes, const int writeplanes, register int x0, int y0, register int x1, int y1, register int w, register int h));
/* vgaData.c */
int NeverCalled P_((void));
/* vgaGC.c */
Mask vgaChangeGCtype P_((register GC *pGC, register ppcPrivGCPtr devPriv));
Mask vgaChangeWindowGC P_((register GC *pGC, register Mask changes));
/* vgaIO.c */
Bool vga16ScreenInit P_((register ScreenPtr pScreen));
Bool vgaScreenClose P_((register int index, register ScreenPtr pScreen));
void Init16Output P_((ScreenPtr pScreen, int virtx, int virty));
/* vgaImages.c */
void vgaDrawColorImage P_((int x, int y, register int w, register int h, register const unsigned char *data, register int RowIncrement, const int alu, const unsigned long int planes));
void vgaReadColorImage P_((int x, int y, int lx, int ly, register unsigned char *data, int RowIncrement));
/* vgaLine.c */
void vgaHorzLine P_((unsigned long int color, int alu, unsigned long int mask, int x, int y, int w));
void vgaVertLine P_((unsigned long int color, int alu, unsigned long int mask, int x, int y, int h));
void vgaBresLine P_((unsigned long int color, int alu, unsigned long int planes, int signdx, int signdy, int axis, int x, int y, int et, int e1, int e2, unsigned long int len));
/* vgaOSD.c */
int vga16Probe P_((int virtx, int virty));
/* vgaSolid.c */
void vgaFillSolid P_((unsigned long int color, const int alu, unsigned long int planes, register int x0, register const int y0, register int lx, register const int ly));
/* vgaStipple.c */
void vgaDrawMonoImage P_((unsigned char *data, int x, int y, int w, int h, unsigned long int fg, int alu, unsigned long int planes));
void vgaFillStipple P_((register PixmapPtr pStipple, unsigned long int fg, int alu, unsigned long int planes, int x, int y, int w, int h, int xSrc, int ySrc));
/* offscreen.c */
int vgaSaveScreenPix P_((ScreenPtr pScreen, PixmapPtr ppix));
int vgaRestoreScreenPix P_((ScreenPtr pScreen, PixmapPtr ppix));
int do_rop P_((int src, int dst, int alu, int planes));
void offBitBlt P_((const int alu, const int readplanes, const int writeplanes, register int x0, int y0, register int x1, int y1, register int w, register int h));
void offDrawColorImage P_((int x, int y, register int w, register int h, register const unsigned char *data, register int RowIncrement, const int alu, const unsigned long int planes));
void offReadColorImage P_((int x, int y, int lx, int ly, register unsigned char *data, int RowIncrement));
void offBresLine P_((unsigned long int color, int alu, unsigned long int planes, int signdx, int signdy, int axis, int x, int y, int et, int e1, int e2, unsigned long int len));
void offFillSolid P_((unsigned long int color, const int alu, unsigned long int planes, register int x0, register const int y0, register int lx, register const int ly));
void offDrawMonoImage P_((unsigned char *data, int x, int y, int w, int h, unsigned long int fg, int alu, unsigned long int planes));
void offFillStipple P_((register PixmapPtr const pStipple, unsigned long int fg, const int alu, unsigned long int planes, int x, int y, int w, int h, const int xSrc, const int ySrc));

#undef P_
