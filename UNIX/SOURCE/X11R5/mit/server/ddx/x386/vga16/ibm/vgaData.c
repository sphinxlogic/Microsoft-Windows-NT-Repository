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

/* $XFree86: mit/server/ddx/x386/vga16/ibm/vgaData.c,v 2.1 1993/10/02 16:08:21 dawes Exp $ */

#include "X.h"
#include "Xproto.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "dixfont.h" /* GJA */
#include "pixmapstr.h"
#include "window.h"
#include "gcstruct.h"
#include "colormapst.h"
#include "cursorstr.h"

#include "mistruct.h"
#include "mfb.h"

/* #include "OSio.h" */
/* #include "OScursor.h" */

#include "ppc.h"
#include "ppcProcs.h"

#include "compiler.h"
#include "vgaVideo.h"

#include "vgaProcs.h"

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

#define vgaGCInterestValidateMask \
( GCLineStyle | GCLineWidth | GCJoinStyle | GCBackground | GCForeground	\
| GCFunction | GCPlaneMask | GCFillStyle | GC_CALL_VALIDATE_BIT		\
| GCClipXOrigin | GCClipYOrigin | GCClipMask | GCSubwindowMode )

/* START GJA */
/* int ibmSaveScreen(); /* declaration -- GJA */

NeverCalled()
{
	ErrorF("NeverCalled was nevertheless called\n");
	abort();
}

int Unused;
/* END GJA */

ppcPrivGC vgaPrototypeGCPriv = {
	GXcopy,	/* unsigned char	rop */
	0,	/* unsigned char	ropOpStip */
	0,	/* unsigned char	ropFillArea */
	TRUE,		/* short	fExpose */
	FALSE,		/* short	freeCompClip */
	NullPixmap,	/* PixmapPtr	pRotatedPixmap */
	0,	/* RegionPtr	pCompositeClip */
	ppcAreaFill,	/* void 	(* FillArea)() */
		{
		    VGA_ALLPLANES,	/* unsigned long	planemask */
		    1,			/* unsigned long	fgPixel */
		    0,			/* unsigned long	bgPixel */
		    GXcopy,		/* int			alu */
		    FillSolid,		/* int			fillStyle */
		}, /* ppcReducedRrop	colorRrop  */
	-1,	/* short lastDrawableType */
	-1,	/* short lastDrawableDepth */
	(void (*)())NeverCalled, /* GJA -- void (* cacheIGBlt)() */
	(void (*)())NeverCalled, /* GJA -- void (* cachePGBlt) () */
	0	/* pointer devPriv */
} ;

GCFuncs vgaGCFuncs = {
ppcValidateGC,
NoopDDA,
NoopDDA,
ppcDestroyGC,
ppcChangeClip,
ppcDestroyClip,
ppcCopyClip
};

GCOps vgaGCOps = {
	ppcSolidWindowFS,	/*  void (* FillSpans)() */
	ppcSetSpans,		/*  void (* SetSpans)()	 */
	miPutImage,		/*  void (* PutImage)()	 */
	ppcCopyArea,		/*  RegionPtr (* CopyArea)()	 */
	miCopyPlane,		/*  void (* CopyPlane)() */
	ppcPolyPoint,		/*  void (* PolyPoint)() */
	ppcScrnZeroLine,	/*  void (* Polylines)() */
	ppcScrnZeroSegs,	/*  void (* PolySegment)() */
	miPolyRectangle,	/*  void (* PolyRectangle)() */
	ppcPolyZeroArc,		/*  void (* PolyArc)()	 */
	miFillPolygon,		/*  void (* FillPolygon)() */
	miPolyFillRect,		/*  void (* PolyFillRect)() */
	miPolyFillArc,		/*  void (* PolyFillArc)() */
	miPolyText8,		/*  int (* PolyText8)()	 */
	miPolyText16,		/*  int (* PolyText16)() */
	miImageText8,		/*  void (* ImageText8)() */
	miImageText16,		/*  void (* ImageText16)() */
	(void (*)()) miImageGlyphBlt,	/*  GJA -- void (* ImageGlyphBlt)() */
	(void (*)()) miPolyGlyphBlt,	/*  GJA -- void (* PolyGlyphBlt)() */
	miPushPixels,		/*  void (* PushPixels)() */
	miMiter			/*  void (* LineHelper)() */
};

GC vgaPrototypeGC = {
	&vgaScreenRec,		/*  ScreenPtr	pScreen	 */
	VGA_MAXPLANES,		/*  int         depth	 */
	GXcopy,			/*  int		alu	 */
	0,			/*  int		lineWidth */
	0,			/*  int		dashOffset */
	0,			/*  int		numInDashList */
	0,			/*  unsigned char *dash	 */
	LineSolid,		/*  int		lineStyle */
	CapButt,		/*  int		capStyle */
	JoinMiter,		/*  int		joinStyle */
	FillSolid,		/*  int		fillStyle */
	EvenOddRule,		/*  int		fillRule */
	ArcPieSlice,		/*  int		arcMode	 */
	ClipByChildren,		/*  int		subWindowMode */
	TRUE,			/*  Bool	graphicsExposures */
	CT_NONE,		/*  int		clientClipType */
	1,			/*  int		miTranslate:1 */
	1,			/*  unsigned int tileIsPixel:1 */
	0,			/*  unused:16 */
	VGA_ALLPLANES,		/*  unsigned long	planemask */
	VGA_BLACK_PIXEL,	/*  unsigned long	fgPixel */
	VGA_WHITE_PIXEL,	/*  unsigned long	bgPixel */
	{ 0, },                  /*  PixUnion	tile	 */
	0,			/*  PixmapPtr	stipple	 */
	{ 0, 0 },		/*  DDXPointRec	patOrg	 */
	0,			/*  FontPtr	font	 */
	{ 0, 0 },		/*  DDXPointRec	clipOrg	 */
	{ 0, 0 },		/*  DDXPointRec	lastWinOrg */
	NULL,			/*  pointer	clientClip */
	(1<<(GCLastBit+1))-1,	/*  unsigned long	stateChanges */
	0,			/*  unsigned long        serialNumber */
	(GCFuncs *)&vgaGCFuncs,	/*  pointer	funcs */
	(GCOps *)&vgaGCOps,	/*  pointer	ops */
	(DevUnion *)&vgaPrototypeGCPriv /*  DevUnion *devPrivates */
} ;

ppcScrnPriv vgaScrnPriv = {
	{
		{
			DRAWABLE_PIXMAP,/* unsigned char	type */
			0,		/* unsigned char	class */
			VGA_MAXPLANES,	/* unsigned char         depth */
			1,		/* unsigned char	bitsPerPixel */
			0,		/* unsigned long        id */
			0,		/* short        x */
			0,		/* short        y */
			640,	        /* short        width */
			480,	        /* short        height */
			0,		/* ScreenPtr    pScreen */
			0,		/* unsigned long        serialNumber */
		},		/* DrawableRec drawable */
		1,		/* int refcnt */
		60,	        /* int devKind */
		{ 0 }		/* DevUnion devPrivate */
	},			/* PixmapRec	pixmap */
	0,			/* void	(* devHardwareInitBB)() */
	0,			/* void	(* devHardwareInitAB)() */
	&vgaPrototypeGC,	/* GCPtr	devPrototypeGC */
	&vgaPrototypeGCPriv,	/* ppcPrivGC 	*devPrototypeGCPriv */
	vgaChangeGCtype,	/* Mask		(* changeGCtype)() */
	vgaChangeWindowGC,	/* Mask		(* changeWindowGC)() */
	0,			/* Colormap	InstalledColormap */
	vgaBitBlt,		/* void	(* blit)() */
	vgaFillSolid,		/* void	(* solidFill)() */
	ppcTileRect,		/* void	(* tileFill)() */
	vgaFillStipple,		/* void	(* stipFill)() */
	ppcOpaqueStipple,	/* void	(* opstipFill)() */
	vgaDrawColorImage,	/* void	(* imageFill)() */
	vgaReadColorImage,	/* void	(* imageRead)() */
	vgaBresLine,		/* void	(* lineBres)() */
	vgaHorzLine,		/* void	(* lineHorz)() */
	vgaVertLine,		/* void	(* lineVert)() */
	(void (*)())NeverCalled,	/* GJA -- void	(* setColor)() */
	(void (*)())NeverCalled,	/* void	(* RecolorCursor)() */
	vgaDrawMonoImage,	/* void	(* monoFill)() */
	(void (*)())NeverCalled,		/* void	(* glyphFill)() */
	(unsigned long *(*)()) NoopDDA, /* unsigned long *((* planeRead)()) */
	ppcReplicateArea,	/* void	(* replicateArea)() */
	NoopDDA,		/* void	(* DestroyGCPriv)() */
/* High Level Software Cursor Support !! */
	&Unused,	/* int	* CursorSemaphore */
	NeverCalled,		/* int	(* CheckCursor)() */
	(void (*)())NeverCalled	/* void	(* ReplaceCursor)() */
/*	,0			/* ppcCacheInfoPtr cacheInfo */
} ;

ScreenRec vgaScreenRec = {
	0,			/* int			myNum */
	0,			/* ATOM id */
	640,		        /* short		width */
	480,		        /* short		height */
	240,                    /* short		mmWidth */
	180,	                /* short		mmHeight */
	sizeof vgaDepths/sizeof (DepthRec), /* short	numDepths */
	VGA_MAXPLANES,		/* short   	    	rootDepth */
	vgaDepths,		/* DepthPtr       	allowedDepths */
	0,			/* unsigned long      	rootVisual */
	0,			/* unsigned long	defColormap */
	1,			/* short		minInstalledCmaps */
	1,			/* short		maxInstalledCmaps */
	Always,			/* char                backingStoreSupport */
	NotUseful,		/* char                saveUnderSupport */
	VGA_WHITE_PIXEL,	/* unsigned long	whitePixel */
	VGA_BLACK_PIXEL,	/* unsigned long	blackPixel */
	0,			/* unsigned long	rgf */
	{ 0 },			/* GCPtr	GCperDepth[MAXFORMATS+1] */
	{ 0 },			/* PixmapPtr		PixmapPerDepth[1] */
	(pointer) &vgaScrnPriv,	/* pointer		devPrivate */
	sizeof vgaVisuals/sizeof (VisualRec), /* short  numVisuals */
	&vgaVisuals[0],		/* VisualPtr		visuals */
	0,			/* int			WindowPrivateLength */
	0,			/* unsigned 		*WindowPrivateSizes */
	0,			/* unsigned 		totalWindowSize */
	0,			/* int			GCPrivateLen */
	0,			/* unsigned 		*GCPrivateSizes */
	0,			/* unsigned 		totalGCSize */
/* Random screen procedures */
	vgaScreenClose,		/* Bool (* CloseScreen)() */
	ppcQueryBestSize,	/* void (* QueryBestSize)() */
	NeverCalled,		/* GJA -- Bool (* SaveScreen)() */
	ppcGetImage,		/* void (* GetImage)() */
	ppcGetSpans, /* unsigned int  *(* GetSpans)() */
	NoopDDA,		/* void (* PointerNonInterestBox)() */
	(void (*)())NULL,	/* void (* SourceValidate)() */
/* Window Procedures */
	ppcCreateWindowForXYhardware,	/* Bool (* CreateWindow)() */
	ppcDestroyWindow,	/* Bool (* DestroyWindow)() */
	ppcPositionWindow,	/* Bool (* PositionWindow)() */
	mfbChangeWindowAttributes,	/* Bool (* ChangeWindowAttributes)() */
	mfbMapWindow,		/* Bool (* RealizeWindow)() */
	mfbUnmapWindow,		/* Bool (* UnrealizeWindow)() */
	miValidateTree,		/* int  (* ValidateTree)() */
        NoopDDA,                /* void  (* PostValidateTree)() -- GJA */
	miWindowExposures,	/* void (* WindowExposures)() */
	ppcPaintWindow,		/* void (* PaintWindowBackground)() */
	ppcPaintWindow,		/* void (* PaintWindowBorder)() */
	ppcCopyWindow,		/* void (* CopyWindow)() */
	miClearToBackground,	/* void (* ClearToBackground)() */
        NoopDDA,                /* void (* ClipNotify)() -- GJA */
/* Pixmap procedures */
	ppcCreatePixmap,	/* PixmapPtr (* CreatePixmap)() */
	mfbDestroyPixmap,	/* Bool (* DestroyPixmap)() */
/* Backing store procedures */
	NoopDDA,		/* void (* SaveDoomedAreas)() */
	(RegionPtr(*)())ppcRestoreAreas,	 /* RegionPtr (* RestoreAreas)() */
	NoopDDA,		/* void	 (* ExposeCopy)() */
	(RegionPtr(*)())NoopDDA,		/* RegionPtr (* TranslateBackingStore)() */
	(RegionPtr(*)())NoopDDA,		/* RegionPtr (* ClearBackingStore)() */
	NoopDDA,		/* void (* DrawGuarantee)() */
/* Font procedures */
	mfbRealizeFont,		/* Bool (* RealizeFont)() */
	mfbUnrealizeFont,	/* Bool (* UnrealizeFont)() */
/* Cursor Procedures */
	(void (*)())NeverCalled,	/* void (* ConstrainCursor)() */
	(void (*)())NeverCalled,	/* void (* CursorLimits)() */
	(Bool (*)())NeverCalled,	/* Bool (* DisplayCursor)() */
	(Bool (*)())NeverCalled,	/* Bool (* RealizeCursor)() */
	(Bool (*)())NeverCalled,	/* Bool (* UnrealizeCursor)() */
	(void (*)())NeverCalled,	/* void (* RecolorCursor)() */
	(Bool (*)())NeverCalled,	/* Bool (* SetCursorPosition)() */
/* GC procedures */
	ppcCreateGC,		/* Bool (* CreateGC)() */
/* Colormap procedures */
	(Bool (*)())NoopDDA,		/* GJA(?) -- Bool (* CreateColormap)() */
	(void (*)())NoopDDA,		/* void (* DestroyColormap)() */
	(void (*)())NeverCalled,	/* GJA -- void (* InstallColormap)() */
	(void (*)())NeverCalled,	/* GJA -- void (* UninstallColormap)() */
	(int (*)())NeverCalled,	/* GJA -- int (* ListInstalledColormaps) () */
	(void (*)())NeverCalled,		/* GJA -- void (* StoreColors)() */
	ppcResolveColor,	/* GJA -- void (* ResolveColor)() */
/* Region procedures */
	miRegionCreate,		/* RegionPtr (* RegionCreate)() */
	miRegionInit,		/* void (* RegionInit)() */
	miRegionCopy,		/* Bool (* RegionCopy)() */
	miRegionDestroy,	/* void (* RegionDestroy)() */
	miRegionUninit,		/* void (* RegionUninit)() */
	miIntersect,		/* int (* Intersect)() */
	miUnion,		/* int (* Union)() */
	miSubtract,		/* int (* Subtract)() */
	miInverse,		/* int (* Inverse)() */
	miRegionReset,		/* void (* RegionReset)() */
	miTranslateRegion,	/* void (* TranslateRegion)() */
	miRectIn,		/* int (* RectIn)() */
	miPointInRegion,	/* Bool (* PointInRegion)() */
	miRegionNotEmpty,	/* Bool (* RegionNotEmpty)() */
	miRegionEmpty,		/* void (* RegionEmpty)() */
	miRegionExtents,	/* BoxPtr (*RegionExtents)() */
	miRegionAppend,		/* Bool (*RegionAppend)() */
	miRegionValidate,	/* Bool (*RegionValidate)() */
	mfbPixmapToRegion,	/* RegionPtr (*BitmapToRegion)() */
	miRectsToRegion,	/* RegionPtr (*RectsToRegion)() */
	miSendGraphicsExpose,	/* void	(*SendGraphicsExpose)() */
/* os layer procedures */
	NoopDDA,		/* GJA -- void (* BlockHandler)() */
	NoopDDA,		/* GJA -- void (* WakeupHandler)() */
	(pointer) 0,		/* pointer blockData */
	(pointer) 0,		/* pointer wakeupData */
	0			/* DevUnion *devPrivates */
} ;
