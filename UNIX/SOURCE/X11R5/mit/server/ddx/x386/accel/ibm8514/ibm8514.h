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
 * KEVIN E. MARTIN AND TIAGO GONS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL KEVIN E. MARTIN OR TIAGO GONS BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified by Tiago Gons (tiago@comosjn.hobby.nl)
 */

/* $XFree86: mit/server/ddx/x386/accel/ibm8514/ibm8514.h,v 2.6 1993/12/25 13:57:09 dawes Exp $ */

#ifndef IBM8514_H
#define IBM8514_H

#define IBM8514_PATCHLEVEL "0"

#include "X.h"
#include "misc.h"
#include "x386.h"
#include "regionstr.h"

extern Bool ibm8514Probe();
extern Bool ibm8514Initialize();
extern void ibm8514EnterLeaveVT();
extern void ibm8514PrintIdent();

extern Bool ibm8514SaveScreen();
extern Bool ibm8514CloseScreen();
extern Bool ibm8514ScreenInit();

extern void ibm8514RestoreDACvalues();
extern int ibm8514ListInstalledColormaps();
extern void ibm8514StoreColors();
extern void ibm8514InstallColormap();
extern void ibm8514UninstallColormap();
extern void ibm8514RestoreColor0();

extern Bool ibm8514RealizeFont();
extern Bool ibm8514UnrealizeFont();

extern void ibm8514ImageRead();
extern void ibm8514ImageWrite();
extern void ibm8514ImageFill();
extern void ibm8514ImageStipple();

extern void ibm8514CacheInit();
extern void ibm8514FontCache8Init();
extern void ibm8514ImageInit();

extern int ibm8514CacheTile();
extern int ibm8514CacheStipple();
extern int ibm8514CacheOpStipple();
extern void ibm8514CImageFill();
extern void ibm8514CImageStipple();
extern void ibm8514CImageOpStipple();
extern void ibm8514CacheFreeSlot();

extern int ibm8514CacheFont8();
extern void ibm8514UnCacheFont8();

extern void ibm8514PolyPoint();
extern void ibm8514Line();
extern void ibm8514Segment();

extern void ibm8514DashLine();
extern void ibm8514DoubleDashLine();
extern void ibm8514DashSegment();
extern void ibm8514DoubleDashSegment();

extern void ibm8514SetSpans();
extern void ibm8514GetSpans();

extern void ibm8514SolidFSpans();
extern void ibm8514TiledFSpans();
extern void ibm8514StipFSpans();
extern void ibm8514OStipFSpans();

extern void ibm8514PolyFillRect();

extern int ibm8514PolyText8();
extern void ibm8514ImageText8();

extern void ibm8514FindOrdering();
extern RegionPtr ibm8514CopyArea();
extern RegionPtr ibm8514CopyPlane();
extern void ibm8514CopyWindow();

extern void ibm8514SaveAreas();
extern void ibm8514RestoreAreas();

extern Bool ibm8514CreateGC();

extern void ibm8514GetImage();

extern ScrnInfoRec ibm8514InfoRec;

extern short ibm8514alu[];

extern int ibm8514ValidTokens[];

#endif /* IBM8514_H */
