/************************************************************
Copyright 1987 by Sun Microsystems, Inc. Mountain View, CA.

                    All Rights Reserved

Permission  to  use,  copy,  modify,  and  distribute   this
software  and  its documentation for any purpose and without
fee is hereby granted, provided that the above copyright no-
tice  appear  in all copies and that both that copyright no-
tice and this permission notice appear in  supporting  docu-
mentation,  and  that the names of Sun or MIT not be used in
advertising or publicity pertaining to distribution  of  the
software  without specific prior written permission. Sun and
M.I.T. make no representations about the suitability of this
software for any purpose. It is provided "as is" without any
express or implied warranty.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)

KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

********************************************************/
/* $XConsortium: cfbscrinit.c,v 5.18 90/09/24 17:46:11 rws Exp $ */

/*
 * Modified by Amancio Hasty and Jon Tombs
 * 
 * Id: s3scrin.c,v 2.2 1993/06/22 20:54:09 jon Exp jon
 */

/* $XFree86: mit/server/ddx/x386/accel/s3/s3scrin.c,v 2.5 1994/02/25 14:58:49 dawes Exp $ */

#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "resource.h"
#include "colormap.h"
#include "colormapst.h"
#include "cfb.h"
#include "mi.h"
#include "mistruct.h"
#include "dix.h"
#include "cfbmskbits.h"
#include "mibstore.h"
#include "s3.h"

extern RegionPtr mfbPixmapToRegion();
extern Bool mfbAllocatePrivates();

extern int defaultColorVisualClass;
#define _BP 6			/**** VGA ****/
#define _RZ ((PSZ + 2) / 3)
#define _RS 0
#define _RM ((1 << _RZ) - 1)
#define _GZ ((PSZ - _RZ + 1) / 2)
#define _GS _RZ
#define _GM (((1 << _GZ) - 1) << _GS)
#define _BZ (PSZ - _RZ - _GZ)
#define _BS (_RZ + _GZ)
#define _BM (((1 << _BZ) - 1) << _BS)
#define _CE (1 << _RZ)

static VisualRec visuals[] =
{
/* vid  class        bpRGB cmpE nplan rMask gMask bMask oRed oGreen oBlue */
#ifndef STATIC_COLOR
   0, PseudoColor, _BP, 1 << PSZ, PSZ, 0, 0, 0, 0, 0, 0,
   0, DirectColor, _BP, _CE, PSZ, _RM, _GM, _BM, _RS, _GS, _BS,
   0, GrayScale, _BP, 1 << PSZ, PSZ, 0, 0, 0, 0, 0, 0,
   0, StaticGray, _BP, 1 << PSZ, PSZ, 0, 0, 0, 0, 0, 0,
#endif
   0, StaticColor, _BP, 1 << PSZ, PSZ, _RM, _GM, _BM, _RS, _GS, _BS,
   0, TrueColor, _BP, _CE, PSZ, _RM, _GM, _BM, _RS, _GS, _BS
};

#define	NUMVISUALS	((sizeof visuals)/(sizeof visuals[0]))

static VisualID VIDs[NUMVISUALS];

static DepthRec depths[] =
{
/* depth	numVid		vids */
   1, 0, NULL,
   8, NUMVISUALS, VIDs
};

#define NUMDEPTHS	((sizeof depths)/(sizeof depths[0]))

static unsigned long cfbGeneration = 0;

miBSFuncRec s3BSFuncRec =
{
   s3SaveAreas,
   s3RestoreAreas,
   (void (*)())0,
   (PixmapPtr(*)())0,
   (PixmapPtr(*)())0,
};

/* dts * (inch/dot) * (25.4 mm / inch) = mm */
Bool
s3ScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width)
     register ScreenPtr pScreen;
     pointer pbits;		/* pointer to screen bitmap */
     int   xsize, ysize;	/* in pixels */
     int   dpix, dpiy;		/* dots per inch */
     int   width;		/* pixel width of frame buffer */
{
   int   i;

   if (cfbGeneration != serverGeneration) {
    /* Set up the visual IDs */
      for (i = 0; i < NUMVISUALS; i++) {
	 visuals[i].vid = FakeClientID(0);
	 VIDs[i] = visuals[i].vid;
	 if (s3DAC8Bit) {
	    visuals[i].bitsPerRGBValue = 8;
	 }
      }
      cfbGeneration = serverGeneration;
   }
   if (!mfbAllocatePrivates(pScreen,
			    &cfbWindowPrivateIndex, &cfbGCPrivateIndex))
      return FALSE;
   if (!AllocateWindowPrivate(pScreen, cfbWindowPrivateIndex,
			      sizeof(cfbPrivWin)) ||
       !AllocateGCPrivate(pScreen, cfbGCPrivateIndex, sizeof(cfbPrivGC)))
      return FALSE;
   if (defaultColorVisualClass < 0) {
      i = 0;
   } else {
      for (i = 0;
	   (i < NUMVISUALS) && (visuals[i].class != defaultColorVisualClass);
	   i++) ;
      if (i >= NUMVISUALS)
	 i = 0;
   }
   pScreen->defColormap = FakeClientID(0);
 /* let CreateDefColormap do whatever it wants for pixels */
   pScreen->blackPixel = pScreen->whitePixel = (Pixel) 0;
   pScreen->QueryBestSize = mfbQueryBestSize;
 /* SaveScreen */
   pScreen->GetImage = s3GetImage;
   pScreen->GetSpans = s3GetSpans;
   pScreen->CreateWindow = cfbCreateWindow;
   pScreen->DestroyWindow = cfbDestroyWindow;
   pScreen->PositionWindow = cfbPositionWindow;
   pScreen->ChangeWindowAttributes = cfbChangeWindowAttributes;
   pScreen->RealizeWindow = cfbMapWindow;
   pScreen->UnrealizeWindow = cfbUnmapWindow;
   pScreen->PaintWindowBackground = miPaintWindow;
   pScreen->PaintWindowBorder = miPaintWindow;
   pScreen->CopyWindow = s3CopyWindow;
   pScreen->CreatePixmap = cfbCreatePixmap;
   pScreen->DestroyPixmap = cfbDestroyPixmap;
   pScreen->RealizeFont = s3RealizeFont;
   pScreen->UnrealizeFont = s3UnrealizeFont;
   pScreen->CreateGC = s3CreateGC;
   pScreen->CreateColormap = cfbInitializeColormap;
   pScreen->DestroyColormap = NoopDDA;
#ifdef	STATIC_COLOR
   pScreen->InstallColormap = cfbInstallColormap;
   pScreen->UninstallColormap = cfbUninstallColormap;
   pScreen->ListInstalledColormaps = cfbListInstalledColormaps;
   pScreen->StoreColors = NoopDDA;
#else
   pScreen->InstallColormap = s3InstallColormap;
   pScreen->UninstallColormap = s3UninstallColormap;
   pScreen->ListInstalledColormaps = s3ListInstalledColormaps;
   pScreen->StoreColors = s3StoreColors;
#endif
   pScreen->ResolveColor = cfbResolveColor;
   pScreen->BitmapToRegion = mfbPixmapToRegion;
   mfbRegisterCopyPlaneProc(pScreen, s3CopyPlane);   
   return miScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width,
		       8, NUMDEPTHS, depths,
		       visuals[i].vid, NUMVISUALS, visuals,
		       &s3BSFuncRec);
}
