
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

/* $XFree86: mit/server/ddx/x386/vga16/ibm/ppcIO.c,v 2.3 1994/03/08 04:51:08 dawes Exp $ */

#include "X.h"
#include "resource.h"
#include "scrnintstr.h"
#include "servermd.h"	/* GJA */
#include "mi.h" /* GJA */

#include "compiler.h"	/* rvb */
#include "vgaVideo.h"

#include "ibmTrace.h"
#include "windowstr.h"
#include "ppc.h"

#include "mistruct.h"
#include "mfb.h"

extern void miRecolorCursor() ;
extern void NoopDDA() ;
/* extern void ibmAbort(); */

static PixmapPtr BogusPixmap = (PixmapPtr) 1 ;
extern ScreenRec vgaScreenRec ; /* Forward Declaration Here */

PixmapFormatRec	vgaFormats[] = { { 4, 8, 32 } } ;

VisualRec vgaVisuals[] = {
	{
	0,			/* unsigned long	vid */
	PseudoColor,		/* short       class */
	6,			/* short       bitsPerRGBValue */
	1 << VGA_MAXPLANES,	/* short	ColormapEntries */
	VGA_MAXPLANES,		/* short	nplanes */
	0,			/* unsigned long	redMask */
	0,			/* unsigned long	greenMask */
	0,			/* unsigned long	blueMask */
	0,			/* int		offsetRed */
	0,			/* int		offsetGreen */
	0			/* int		offsetBlue */
	}
} ;

#define NUM_VISUALS  (sizeof vgaVisuals/sizeof (VisualRec))

unsigned long int vgaDepthVIDs[NUM_VISUALS];

DepthRec vgaDepths[] = {
/*	depth		numVid	vids */
    {	1,		0,	NULL	},
    {	VGA_MAXPLANES,	NUM_VISUALS,	vgaDepthVIDs }
} ;

#define NUM_DEPTHS  (sizeof vgaDepths/sizeof (DepthRec))

NeverCalled()
{
	ErrorF("NeverCalled was nevertheless called\n");
	abort();
}

Bool ppcInitializeColormap();	/* GJAHACK */

miBSFuncRec ppcBSFuncRec = {
    ppcSaveAreas,
    ppcRestoreAreas,
    (void (*)()) 0,
    (PixmapPtr (*)()) 0,
    (PixmapPtr (*)()) 0,
};


/* Global Variables */
int BYTES_PER_LINE, MAX_ROW, MAX_COLUMN;

/*ARGSUSED*/
Bool
vgaScreenClose( index, pScreen )
register const int index ;
register ScreenPtr const pScreen ;
{
	pScreen->defColormap = 0 ;
}


GCPtr sampleGCperDepth[MAXFORMATS+1] = { 0 };
PixmapPtr samplePixmapPerDepth[1] = { 0 };

void
Init16Output( pScreen, virtx, virty )
    ScreenPtr pScreen;
    int virtx, virty;
{
  extern int defaultColorVisualClass;
#ifdef UNBANKED_VGA16
  extern pointer vgaBase; /* GJAHACK */
#else
  extern pointer vgaVirtBase; /* GJAHACK */
#endif

  /*
   * patch the original structures
   */
  BYTES_PER_LINE = virtx / 8;
  MAX_ROW       = virty - 1;
  MAX_COLUMN    = virtx - 1;

  if ( defaultColorVisualClass < 0 ) {
    vgaVisuals[0].class = PseudoColor;
  } else {
    vgaVisuals[0].class = defaultColorVisualClass;
  }

  pScreen-> id = 0;
  pScreen->defColormap = FakeClientID(0);
  pScreen-> whitePixel = VGA_WHITE_PIXEL;
  pScreen-> blackPixel = VGA_BLACK_PIXEL;
  pScreen-> rgf = 0;
  *(pScreen-> GCperDepth) = *(sampleGCperDepth);
  *(pScreen-> PixmapPerDepth) = *(samplePixmapPerDepth);
  pScreen-> numVisuals = sizeof vgaVisuals/sizeof (VisualRec);
  pScreen-> visuals = &vgaVisuals[0];
  pScreen-> CloseScreen = vgaScreenClose;
  pScreen-> QueryBestSize = ppcQueryBestSize;
  pScreen-> GetImage = ppcGetImage;
  pScreen-> GetSpans = ppcGetSpans;
  pScreen-> CreateWindow = ppcCreateWindowForXYhardware;
  pScreen-> DestroyWindow = ppcDestroyWindow;
  pScreen-> PositionWindow = ppcPositionWindow;
  pScreen-> ChangeWindowAttributes = mfbChangeWindowAttributes;
  pScreen-> RealizeWindow = mfbMapWindow;
  pScreen-> UnrealizeWindow = mfbUnmapWindow;
  pScreen-> PaintWindowBackground = ppcPaintWindow;
  pScreen-> PaintWindowBorder = ppcPaintWindow;
  pScreen-> CopyWindow = ppcCopyWindow;
  pScreen-> CreatePixmap = ppcCreatePixmap;
  pScreen-> DestroyPixmap = mfbDestroyPixmap;
  pScreen-> SaveDoomedAreas = NoopDDA;
  pScreen-> RestoreAreas = (RegionPtr(*)())ppcRestoreAreas;
  pScreen-> ExposeCopy = NoopDDA;
  pScreen-> TranslateBackingStore = (RegionPtr(*)())NoopDDA;
  pScreen-> ClearBackingStore = (RegionPtr(*)())NoopDDA;
  pScreen-> DrawGuarantee = NoopDDA;
  pScreen-> RealizeFont = mfbRealizeFont;
  pScreen-> UnrealizeFont = mfbUnrealizeFont;
  pScreen-> CreateGC = ppcCreateGC;
  pScreen-> CreateColormap = (Bool (*)())ppcInitializeColormap;
  pScreen-> DestroyColormap = (void (*)())NoopDDA;
  pScreen-> ResolveColor = ppcResolveColor;
  pScreen-> BitmapToRegion = mfbPixmapToRegion;

  if (!mfbAllocatePrivates(pScreen, (int*)NULL, (int*)NULL))
	return ;

#ifdef UNBANKED_VGA16
  miScreenInit(pScreen, vgaBase, virtx, virty, 75, 75, virtx,
#else
  miScreenInit(pScreen, vgaVirtBase, virtx, virty, 75, 75, virtx,
#endif
	1, NUM_DEPTHS, vgaDepths, 0 /* See above */,
	NUM_VISUALS, vgaVisuals, NULL);
  pScreen-> rootDepth = VGA_MAXPLANES;

  miInitializeBackingStore (pScreen, &ppcBSFuncRec);
  TRACE(("ppcCommonScreenInit returns\n"));

  mfbRegisterCopyPlaneProc(pScreen,miCopyPlane); /* GJA -- R4->R5 */
}
