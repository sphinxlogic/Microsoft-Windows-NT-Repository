  /* slistP.h */

#ifndef _SListp_h
#define _Slistp_h
#include "slist.h"

typedef struct {
    String		translations;
} SListClassPart;

typedef struct _SListClassRec {
    CoreClassPart       core_class;
#ifdef MOTIF
    XmPrimitiveClassPart primitive_class;
#endif
    SListClassPart      slist_class;
} SListClassRec, *SListClass;

typedef struct _SListPart
{
    /* Resource fields */
#ifdef MOTIF
    XmFontList          *fontRecP;      /* font */
#else
    DwtFontRec          *fontRecP;      /* font */
#endif
    Pixel               foreground;     /* foreground color */
    int                 textMarginWidth; /* text margin width in pixels */
    int                 cols;           /* initial number of columns */
    int                 rows;           /* initial number of rows */
#ifdef MOTIF
    XmAnyCallbackStruct selectCallback; /* Mb1 callback */
    XmAnyCallbackStruct doubleClickCallback; /* double click callback */
    XmAnyCallbackStruct getDataCallback;/* get data callback */
#else
    DwtCallbackStruct   selectCallback; /* Mb1 callback */
    DwtCallbackStruct   doubleClickCallback; /* double click callback */
    DwtCallbackStruct	getDataCallback;/* get data callback */
#endif

    /* Private fields */
    Widget              vScroll;        /* Vertical Scrollbar */
    XFontStruct         *fontP;         /* font */
    int                 cellHeight;     /* cell height in pixels */
    int                 cellWidth;      /* cell width in pixels */
    int                 yFontAdjust;    /* adjustment for text positioning */
    int			topLine;	/* top line of the window */
    int                 textX;          /* boundaries of text window */
    int                 textY;
    int                 textWidth;
    int                 textHeight;
#ifndef MOTIF
    int			parentWidth;	/* known parent size */
    int			parentHeight;	/* for conforming when resized */
#endif
    int                 leftMarginWidth; /* scrollbar plus left margin */
    GC                  normgc;         /* normal graphics context */
    GC                  normtxtgc;      /* text graphics context */
    GC                  invgc;          /* text reverse graphics context */
    GC                  flipgc;         /* invert graphics context */
    int                 knownHeight;    /* last known core.height */
    int                 knownWidth;     /* last known core.width */
    int                 LineCnt;        /* entries used in text array */
    int                 listIsGrabbed;  /* we are doing a drag select (T/F) */
    int                 mouseLine;      /* line where mouse last seen */
    int                 highlightedLine; /* line which is highlighted (or -1) */
    int                 YAdjust;        /* in pixels */
    int                 anyAreExposed;  /* some exposures are pending */
    char                *exposeMapP;    /* expose bytemap */
    char                *selectMapP;    /* select bytemap */
    int                 mapLen;         /* allocated bytemap length */
    int                 selectParity;   /* flavor of drag select (1/0) */
    int                 downLine;       /* line where drag select started */
    int                 upLine;         /* line where drag select ended */
    int                 ClickInterval;  /* double click delay */
    XtIntervalId        ClickTimerID;   /* id for click timer */
    int                 DownCount;      /* count of downclicks */
    Time                DownTime;       /* time of downclick */
    int                 ClickLine;      /* line number of first click */
} SListPart;

/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/
 
typedef struct _SListRec {
    CorePart            core;           /* Core widget fields */
#ifdef MOTIF
    XmPrimitivePart     primitive;      /* Primitive widget fields */
#endif
    SListPart           slist;          /* SList widget fields */
} SListWidgetRec, *SListWidget;

#endif /* _SListp_h */
