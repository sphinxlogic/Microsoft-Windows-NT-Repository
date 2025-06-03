/*
 * Copyright 1992,1993 by Kevin E. Martin, Chapel Hill, North Carolina.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Kevin E. Martin not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Kevin E. Martin makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * KEVIN E. MARTIN AND RICKARD E. FAITH DISCLAIM ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 */

/* $XFree86: mit/server/ddx/x386/accel/mach32/mach32.h,v 2.13 1994/03/01 09:51:30 dawes Exp $ */

#ifndef MACH32_H
#define MACH32_H

#define MACH32_PATCHLEVEL "0"

#define MACH32_CURSBYTES	1024
#define MACH32_CURSMAX		64

#define NEW_MEM_DETECT

#include "X.h"
#include "misc.h"
#include "x386.h"
#include "regionstr.h"
#include "regmach32.h"

extern Bool mach32Probe();
extern Bool mach32Initialize();
extern void mach32EnterLeaveVT();
extern void mach32AdjustFrame();
extern Bool mach32SwitchMode();
extern void mach32PrintIdent();
extern Bool mach32ClockSelect();

extern void mach32Dline();
extern void mach32Dsegment();

extern Bool mach32SaveScreen();
extern Bool mach32CloseScreen();

extern Bool mach32ScreenInit();
extern void mach32InitDisplay();
extern void mach32InitAperture();
extern void mach32InitEnvironment();
extern void mach32CalcCRTCRegs();
extern void mach32SetCRTCRegs();
extern void mach32SaveLUT();
extern void mach32RestoreLUT();
extern void mach32CleanUp();
extern void mach32SetRamdac();

extern int mach32ListInstalledColormaps();
extern int mach32GetInstalledColormaps();
extern void mach32StoreColors();
extern void mach32InstallColormap();
extern void mach32UninstallColormap();
extern void mach32RestoreColor0();

extern Bool mach32RealizeFont();
extern Bool mach32UnrealizeFont();

extern void (*mach32ImageReadFunc)();
extern void (*mach32ImageWriteFunc)();
extern void (*mach32ImageFillFunc)();
extern void mach32ImageStipple();
extern void mach32ImageOpStipple();

extern void mach32CacheInit();
extern void mach32FontCache8Init();
extern void mach32ImageInit();

extern int mach32CacheTile();
extern int mach32CacheStipple();
extern int mach32CacheOpStipple();
extern Bool mach32CachableTile();
extern Bool mach32CachableStipple();
extern Bool mach32CachableOpStipple();
extern void mach32CImageFill();
extern void mach32CImageStipple();
extern void mach32CImageOpStipple();
extern void mach32CacheFreeSlot();

extern int mach32CacheFont8();
extern void mach32UnCacheFont8();
extern int mach32CPolyText8();
extern void mach32CImageText8();

extern void mach32PolyPoint();
extern void mach32Line();
extern void mach32Line1Rect();
extern void mach32Segment();

extern void mach32SetSpans();
extern void mach32GetSpans();

extern void mach32SolidFSpans();
extern void mach32TiledFSpans();
extern void mach32StipFSpans();
extern void mach32OStipFSpans();

extern void mach32PolyFillRect();

extern int mach32PolyText8();
extern void mach32ImageText8();
extern int mach32PolyText16();
extern void mach32ImageText16();

extern void mach32FindOrdering();
extern RegionPtr mach32CopyArea();
extern RegionPtr mach32CopyPlane();
extern void mach32CopyWindow();
extern void mach32GetImage();

extern void mach32PaintWindow();
extern void mach32FillBoxSolid();

extern void mach32SaveAreas();
extern void mach32RestoreAreas();

extern Bool mach32CreateGC();

extern Bool mach32CursorInit();
extern void mach32RestoreCursor();
extern void mach32CursorOff();
extern void mach32RenewCursorColor();

extern int mach32GetMemSize();

extern pointer mach32VideoMem;
extern pointer vgaBase;
extern Bool x386VTSema;
extern short mach32MaxX, mach32MaxY;
extern short mach32VirtX, mach32VirtY;
extern Bool mach32Use4MbAperture;
extern Bool mach32DAC8Bit;

extern int mach32Ramdac;
extern int mach32BusType;

/* == 0 --> no boundary crossing on this line */
/*  > 0 --> x value of first pixel in video aperture */
extern int mach32VideoPageBoundary[MACH32_MAX_Y+1];

extern ScrnInfoRec mach32InfoRec;
extern int mach32ValidTokens[];

extern short mach32alu[];
#endif
