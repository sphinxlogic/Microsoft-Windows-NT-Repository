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

/* $XFree86: mit/server/ddx/x386/vga16/ibm/ppcProcs.h,v 2.1 1993/09/23 15:49:06 dawes Exp $ */

#if __STDC__ || defined(__cplusplus)
#define P_(s) s
#else
#define P_(s) ()
#endif

/* ppcArc.c */
void ppcPolyZeroArc P_((WindowPtr pWindow, GCPtr pGC, int narcs, xArc *parcs));
void ppcPolyFillArc P_((WindowPtr pWindow, GCPtr pGC, int narcs, xArc *parcs));
/* ppcArea.c */
void ppcAreaFill P_((register WindowPtr pWin, register int nboxes, register BoxPtr pBox, GCPtr pGC));
/* ppcBStore.c */
void ppcSaveAreas P_((register PixmapPtr pPixmap, RegionPtr prgnSave, int xorg, int yorg));
void ppcRestoreAreas P_((register PixmapPtr pPixmap, RegionPtr prgnRestore, int xorg, int yorg));
/* ppcBitmap.c */
void ppcQuickBlt P_((int *psrcBase, int *pdstBase, int xSrc, int ySrc, int xDst, int yDst, int w, int h, int wSrc, int wDst));
void ppcRotBitmapRight P_((register PixmapPtr pPix, register int rw));
void ppcRotBitmapDown P_((register PixmapPtr pPix, register int rh));
/* ppcCReduce.c */
void ppcReduceGeneral P_((register int alu, register unsigned long pm, register unsigned long fg, register unsigned long bg, register int fillStyle, int drawableDepth, ppcReducedRrop *returnLoc));
void ppcReduceColorRrop P_((ppcReducedRrop *initialLoc, int drawableDepth, ppcReducedRrop *returnLoc));
void ppcGetReducedColorRrop P_((GC *pGC, int drawableDepth, ppcReducedRrop *returnLoc));
/* ppcClip.c */
void ppcDestroyClip P_((GCPtr pGC));
void ppcChangeClip P_((GCPtr pGC, unsigned int type, pointer pvalue, int nrects));
void ppcCopyClip P_((GCPtr pgcDst, GCPtr pgcSrc));
/* ppcCpArea.c */
RegionPtr ppcCopyArea P_((register DrawablePtr pSrcDrawable, register DrawablePtr pDstDrawable, GC *pGC, int srcx, int srcy, int width, int height, int dstx, int dsty));
/* ppcCurs.c */
Bool ppcRealizeCursor P_((ScreenPtr pScr, CursorPtr pCurs));
Bool ppcUnrealizeCursor P_((register ScreenPtr pScr, register CursorPtr pCurs));
/* ppcDepth.c */
Bool ppcDepthOK P_((register DrawablePtr pDraw, register int depth));
/* ppcFillRct.c */
void ppcPolyFillRect P_((DrawablePtr pDrawable, GCPtr pGC, int nrectFill, xRectangle *prectInit));
/* ppcGC.c */
Bool ppcCreateGC P_((register GCPtr pGC));
void ppcDestroyGC P_((register GC *pGC));
Mask ppcChangePixmapGC P_((register GC *pGC, register Mask changes));
void ppcValidateGC P_((register GCPtr pGC, register Mask changes, DrawablePtr pDrawable));
/* ppcGetSp.c */
void ppcGetSpans P_((DrawablePtr pDrawable, int wMax, register DDXPointPtr ppt, int *pwidth, register int nspans, unsigned int *pdstStart));
/* ppcIO.c */
void ppc16CommonScreenInit P_((register ScreenPtr pScreen, ScreenPtr pScrnProto));
void ppcCommonScreenClose P_((register ScreenPtr pScreen, register int index, ScreenPtr pScrnProto));
Bool vga16CreateDefColormap P_((ScreenPtr pScreen));
/* ppcImg.c */
void ppcGetImage P_((DrawablePtr pDraw, int sx, int sy, int w, int h, unsigned int format, unsigned long int planeMask, pointer pdstLine));
/* ppcLine.c */
void ppcScrnZeroLine P_((DrawablePtr pDrawable, GCPtr pGC, int mode, int npt, DDXPointPtr pptInit));
void ppcScrnZeroDash P_((DrawablePtr pDrawable, GCPtr pGC, int mode, int npt, DDXPointPtr pptInit));
void ppcScrnZeroSegs P_((DrawablePtr pDrawable, GCPtr pGC, int nseg, xSegment *psgInit));
/* ppcPixmap.c */
PixmapPtr ppcCreatePixmap P_((ScreenPtr pScreen, int width, int height, int depth));
PixmapPtr ppcCopyPixmap P_((register PixmapPtr pSrc));
void ppcPadPixmap P_((PixmapPtr pPixmap));
void ppcRotatePixmap P_((PixmapPtr pPix, register int rw));
/* ppcPixmapFS.c */
int mod P_((int n1, int n2));
void ppcSolidPixmapFS P_((DrawablePtr pDrawable, GCPtr pGC, int nInit, DDXPointPtr pptInit, int *pwidthInit, int fSorted));
void ppcStipplePixmapFS P_((register DrawablePtr pDrawable, GC *pGC, int nInit, DDXPointPtr pptInit, int *pwidthInit, int fSorted));
void ppcOpStipplePixmapFS P_((DrawablePtr pDrawable, GC *pGC, int nInit, DDXPointPtr pptInit, int *pwidthInit, int fSorted));
void ppcTilePixmapFS P_((register DrawablePtr pDrawable, GC *pGC, int nInit, DDXPointPtr pptInit, int *pwidthInit, int fSorted));
/* ppcPntWin.c */
void ppcPaintWindow P_((WindowPtr pWin, RegionPtr pRegion, int what));
void ppcPaintWindowSolid P_((register WindowPtr pWin, register RegionPtr pRegion, int what));
void ppcPaintWindowTile P_((register WindowPtr pWin, register RegionPtr pRegion, int what));
/* ppcPolyPnt.c */
void ppcPolyPoint P_((DrawablePtr pDrawable, GCPtr pGC, int mode, int npt, xPoint *pptInit));
/* ppcPolyRec.c */
void ppcPolyRectangle P_((DrawablePtr pDraw, GCPtr pGC, int nrects, xRectangle *pRects));
/* ppcPushPxl.c */
void ppcPushPixels P_((GCPtr pGC, PixmapPtr pBitMap, DrawablePtr pDrawable, int dx, int dy, int xOrg, int yOrg));
/* ppcQuery.c */
void ppcQueryBestSize P_((register int class, register short *pwidth, register short *pheight));
/* ppcRot.c */
void ppcRotZ8mapUp P_((register PixmapPtr pSource, register PixmapPtr pDest, register int shift));
void ppcRotZ8mapLeft P_((register PixmapPtr pSource, register PixmapPtr pDest, register int shift));
void ppcClipZ8Pixmap P_((register PixmapPtr pSource, register PixmapPtr pDest));
PixmapPtr ppcClipBitmap P_((PixmapPtr pStipple, int w, int h));
void ppcRotBitmapUp P_((register PixmapPtr pSource, register PixmapPtr pDest, register int shift));
void ppcRotBitmapLeft P_((register PixmapPtr pSource, register PixmapPtr pDest, register int shift));
void ppcClipZ1Pixmap P_((register PixmapPtr pSource, register PixmapPtr pDest));
void ppcEndWorldHunger P_((void));
void ppcRefinanceNationalDebt P_((void));
void ppcEndWarForever P_((void));
void ppcPayServerHackersWhatTheyreWorth P_((void));
/* ppcRslvC.c */
void ppcResolveColor P_((register unsigned short *pRed, register unsigned short *pGreen, register unsigned short *pBlue, register VisualPtr pVisual));
/* ppcSetSp.c */
void ppcSetSpans P_((register DrawablePtr pDrawable, GCPtr pGC, unsigned char *psrc, register DDXPointPtr ppt, int *pwidth, int nspans, int fSorted));
/* ppcSinTab.c */
/* ppcWindow.c */
void ppcCopyWindow P_((register WindowPtr pWin, DDXPointRec ptOldOrg, RegionPtr prgnSrc));
void ppcCopyWindowForXYhardware P_((register WindowPtr pWin, DDXPointRec ptOldOrg, RegionPtr prgnSrc));
Bool ppcPositionWindow P_((register WindowPtr pWin, register int x, register int y));
Bool ppcUnrealizeWindow P_((register WindowPtr pWindow, register int x, register int y));
Bool ppcRealizeWindow P_((register WindowPtr pWindow));
Bool ppcDestroyWindow P_((register WindowPtr pWin));
Bool ppcCreateWindow P_((register WindowPtr pWin));
Bool ppcCreateWindowForXYhardware P_((register WindowPtr pWin));
/* ppcWindowFS.c */
void ppcSolidWindowFS P_((DrawablePtr pDrawable, GCPtr pGC, int nInit, DDXPointPtr pptInit, int *pwidthInit, int fSorted));
void ppcStippleWindowFS P_((DrawablePtr pDrawable, register GC *pGC, int nInit, DDXPointPtr pptInit, int *pwidthInit, int fSorted));
void ppcOpStippleWindowFS P_((DrawablePtr pDrawable, register GC *pGC, int nInit, DDXPointPtr pptInit, int *pwidthInit, int fSorted));
void ppcTileWindowFS P_((DrawablePtr pDrawable, register GC *pGC, int nInit, DDXPointPtr pptInit, int *pwidthInit, int fSorted));
/* emulOpStip.c */
void ppcOpaqueStipple P_((register PixmapPtr pStipple, unsigned long int fg, unsigned long int bg, int alu, unsigned long int planes, register int x, register int y, register int w, register int h, int xSrc, int ySrc));
/* emulRepAre.c */
void ppcReplicateArea P_((register int x, register int y, register int planeMask, int goalWidth, int goalHeight, int currentHoriz, int currentVert, ScreenPtr pScrn));
/* emulStip.c */
void ppcStipple P_((PixmapPtr pStipple, unsigned long int fg, int merge, unsigned long int planes, int x, int y, int w, int h, int xSrc, int ySrc));
/* emulTile.c */
void ppcTileRect P_((register PixmapPtr pTile, int alu, unsigned long int planes, register int x0, register int y0, register int w, register int h, int xSrc, int ySrc));

#undef P_
