/*
 * Copyright 1992 by Kevin E. Martin, Chapel Hill, North Carolina.
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
 * KEVIN E. MARTIN, RICKARD E. FAITH, SCOTT LAIRD, AND TIAGO GONS DISCLAIM
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * KEVIN E. MARTIN, RICKARD E. FAITH, SCOTT LAIRD, OR TIAGO GONS BE LIABLE
 * FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Further modifications by Scott Laird (lair@kimbark.uchicago.edu)
 * and Tiago Gons (tiago@comosjn.hobby.nl)
 */

/* $XFree86: mit/server/ddx/x386/accel/mach8/mach8.h,v 2.10 1994/02/01 13:51:26 dawes Exp $ */

#ifndef MACH8_H
#define MACH8_H

#define MACH8_PATCHLEVEL "0"

#include "X.h"
#include "misc.h"
#include "x386.h"
#include "regionstr.h"

extern Bool mach8Probe();
extern Bool mach8Initialize();
extern void mach8EnterLeaveVT();
extern void mach8PrintIdent();
extern void mach8Adjust();
extern Bool mach8SwitchMode();
extern Bool mach8ClockSelect();

extern void mach8calcvmode();

extern void mach8Init();
extern Bool mach8ScreenInit();
extern void mach8InitEnvironment();
extern void mach8CleanUp(void);

extern Bool mach8SaveScreen();
extern Bool mach8CloseScreen();

extern void mach8RestoreDACvalues();
extern int mach8ListInstalledColormaps();
extern void mach8StoreColors();
extern void mach8InstallColormap();
extern void mach8UninstallColormap();
extern void mach8RestoreColor0();

extern Bool mach8RealizeFont();
extern Bool mach8UnrealizeFont();

extern void mach8ImageFill();

extern void mach8ImageRead();
extern void mach8ImageWrite();
extern void mach8RealImageFill();
extern void mach8ImageStipple();
extern void mach8ImageReadDram();
extern void mach8ImageWriteDram();
extern void mach8RealImageFillDram();
extern void mach8ImageStippleDram();

extern void mach8CacheInit();
extern void mach8FontCache8Init();
extern void mach8ImageInit();

extern int mach8CacheTile();
extern int mach8CacheStipple();
extern int mach8CacheOpStipple();
extern void mach8CImageFill();
extern void mach8CImageStipple();
extern void mach8CImageOpStipple();
extern void mach8CacheFreeSlot();

extern void *mach8CacheFont8();
extern void mach8UnCacheFont8();

extern void mach8PolyPoint();
extern void mach8Line();
extern void mach8Segment();

extern void mach8DashLine();
extern void mach8DoubleDashLine();
extern void mach8DashSegment();
extern void mach8DoubleDashSegment();

extern void mach8SetSpans();
extern void mach8GetSpans();

extern void mach8SolidFSpans();
extern void mach8TiledFSpans();
extern void mach8StipFSpans();
extern void mach8OStipFSpans();

extern void mach8PolyFillRect();

extern int mach8PolyText8();
extern void mach8ImageText8();

extern void mach8FindOrdering();
extern RegionPtr mach8CopyArea();
extern RegionPtr mach8CopyPlane();
extern void mach8CopyWindow();
extern void mach8FindOrdering();

extern void mach8SaveAreas();
extern void mach8RestoreAreas();

extern Bool mach8CreateGC();

extern void mach8GetImage();

extern ScrnInfoRec mach8InfoRec;

extern short mach8alu[];

extern int mach8ValidTokens[];

extern void (*mach8ImageWriteFunc)();
extern void (*mach8ImageReadFunc)();
extern void (*mach8RealImageFillFunc)();
extern void (*mach8ImageStippleFunc)();

#endif /* MACH8_H */
