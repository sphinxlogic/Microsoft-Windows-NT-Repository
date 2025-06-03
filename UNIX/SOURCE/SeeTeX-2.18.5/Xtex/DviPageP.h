/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 */ 

#ifndef _DviPageP_h
#define _DviPageP_h

#include "DviPage.h"
/* include superclass private header file */

#include <X11/CoreP.h>
#include <X11/Xaw/Label.h>
#include <X11/Shell.h>

#ifdef HAVE_DPS
#  include <DPS/XDPSlib.h>
#  include <DPS/dpsXclient.h>
#  include <DPS/dpsexcept.h>
#endif

#ifdef HAVE_NEWS
#  include <NeWS/psio.h>
#  include <xvps/pscanvas.h>
#endif

#ifdef HAVE_GHOSTSCRIPT
#include <X11/Xmu/Atoms.h>
#include <X11/Xmu/CharSet.h>
#endif

typedef struct {
#ifdef HAVE_GHOSTSCRIPT
    AtomPtr ghostview;
    AtomPtr next;
    AtomPtr page;
    AtomPtr done;
#else
    int empty;
#endif
} DviPageClassPart;

typedef struct _DviPageClassRec {
    CoreClassPart	core_class;
    DviPageClassPart	dviPage_class;
} DviPageClassRec;

extern DviPageClassRec dviPageClassRec;


typedef struct {
	i32	stack_hh;
	i32	stack_vv;
	i32	h;		/* the saved h */
	i32	v;		/* the saved v */
	i32	w;		/* etc */
	i32	x;
	i32	y;
	i32	z;
} LocalDviStack;

typedef struct DviButtonStruct DviButton;

#define DVI_REGION_STRLEN 128
struct DviButtonStruct {
    XPoint upperLeft;
    XPoint lowerRight;
    char action[ DVI_REGION_STRLEN ];
    char *longerAction;
    DviButton *next;
};

typedef struct {
    Pixel	background;
    Pixel	foreground;
    Pixel	boxColor;
    
    int useBackingStore;	/* backing store type */
    Bool haveBackingStore;
    
    Bool reverseVideo;
    
    XFontStruct *font;
    
    /* private state */
    
    GC invertGC;
    GC paintGC;
    GC clearGC;
    
    Bool redisplay;
    
    char *page;
    
    Dimension pixelsWide;
    Dimension pixelsHigh;
    
    Dimension pixelsPerInchHoriz;
    Dimension pixelsPerInchVert;
    
    float paperWidth;
    float paperHeight;
    
    float topOffset;
    float leftOffset;
    
    char *maxDriftString;	/* in points */
    
    XFontStruct **xFonts;
    
    struct PostAmbleInfo *pai;
    DviFontInfo *dviFonts;
    
    int userMag;
    
    double dpi;
    char *dpiString;
    
    float dpiHoriz;
    float dpiVert;
    
    unsigned long spPerDot;
    
    long fastFromSpValueHoriz;
    long fastFromSpValueVert;
    
    Region updateRegion;
    
    LocalDviStack *dviStack;
    LocalDviStack *dviStackPointer;
    int dviStackSize;
    
    /* for anchors */
    Bool anchored;
    int anchorFromX;
    int anchorFromY;
    int anchorToX;
    int anchorToY;
    
    Widget scalePopup;
    LabelWidget scaleLabel;
    
    ScaleUnits units;
    char *scaleUnitsString;
    
    /* for tpic specials */
#define	MAXPOINTS	300	/* Max points in a path */
    int xx[MAXPOINTS];
    int yy[MAXPOINTS];
    int pathLen;
    int penSize;
    Bool whiten;
    Bool shade;
    Bool blacken;
    
#if defined(HAVE_DPS) || defined(HAVE_NEWS) || defined(HAVE_GHOSTSCRIPT)
    float trueDpi;
#endif

#if defined(HAVE_DPS) || defined(HAVE_NEWS) 
    char *dpsPreamble;
#endif

#ifdef HAVE_DPS
    /* Display PostScript Extensions */
    Bool dpsVisible;
    DPSContext ctx;

#endif	/* HAVE_DPS */
    
#ifdef HAVE_NEWS
    /* NeWS extensions */

    PSFILE *newsfile;
    NeWStoken *newstoken;
    
#endif  /* HAVE_NEWS */

#ifdef HAVE_GHOSTSCRIPT
    char *gsPreamble;
    String interpreter;			/* ghostscript to use */
    String ghostSearchPath;	/* file search path for gs */

    int ghostPID;		/* PID of forked ghostscript process */
    Window ghostMsgWin;		/* ghostscript window to send msgs to */

    /* Following for handling text i/o */
    XtInputId ghostInputId, ghostOutputId, ghostErrorId;
    int ghostInputFD, ghostOutputFD, ghostErrorFD;
    XtCallbackList ghostOutput;

    /* Following for handling picture output */
    Pixmap ghostPixmap;
    int ghostPixWidth, ghostPixHeight, ghostPixDepth;
#endif

    /* for XTeX action buttons */
    DviButton *buttons;
    
} DviPagePart;

typedef struct _DviPageRec {
    CorePart		core;
    DviPagePart	dviPage;
} DviPageRec;


#define fastFromSpVert(w, x) ((x) / (w -> dviPage.fastFromSpValueVert))
#define fastFromSpHoriz(w, x) ((x) / (w -> dviPage.fastFromSpValueHoriz))

#endif  _DviPageP_h
