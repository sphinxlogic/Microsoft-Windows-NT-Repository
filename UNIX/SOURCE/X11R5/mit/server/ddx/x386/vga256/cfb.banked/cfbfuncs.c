/*
 * cfbfuncs.c
 *
 * Initialise low level cfb functions
 */

/* $XFree86: mit/server/ddx/x386/vga256/cfb.banked/cfbfuncs.c,v 2.0 1993/08/18 06:48:31 dawes Exp $ */

#include "cfbfuncs.h"

extern void vgaBitBlt();
extern int cfbDoBitbltCopy();
extern void cfbFillRectSolidCopy();
extern void cfb8FillRectTransparentStippled32();
extern void cfb8FillRectOpaqueStippled32();
extern void cfbSegmentSS();
extern void cfbLineSS();
extern void cfbFillBoxSolid();
extern void cfbTEGlyphBlt8();

CfbfuncRec cfbLowlevFuncs = {
    vgaBitBlt,				/* default to the 2-bank version */
    cfbDoBitbltCopy,
    cfbFillRectSolidCopy,
    cfb8FillRectTransparentStippled32,
    cfb8FillRectOpaqueStippled32,
    cfbSegmentSS,
    cfbLineSS,
    cfbFillBoxSolid,
    cfbTEGlyphBlt8,
};

