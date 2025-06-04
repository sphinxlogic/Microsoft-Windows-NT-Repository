/* slist.h */

#ifndef _SList_h
#define _Slist_h

/* Resource names */

#define SListNforeground                "foreground"
#define SListNpointerForeground         "pointerForeground"
#define SListNselectCallback            "selectCallback"
#define SListNdoubleClickCallback       "doubleClickCallback"
#define SListNcurrentPixmap             "currentPixmap"
#define SListNscrollbarLines            "scrollbarLines"
#define SListNstepperScrollFactor       "stepperScrollFactor"
#define SListNtextMarginWidth           "textMarginWidth"
#define SListNdoubleClickDelay          "doubleClickDelay"
#define SListNseparatorWidth            "separatorWidth"
#define SListNcols			"cols"
#define SListNrows			"rows"
#define SListNgetDataCallback		"getDataCallback"

/* Resource classes */

#define SListCForeground                "Foreground"
#define SListCPointerForeground         "PointerForeground"
#define SListCCurrentPixmap             "CurrentPixmap"
#define SListCScrollbarLines            "ScrollbarLines"
#define SListCScrollFactor              "ScrollFactor"
#define SListCTextMarginWidth           "TextMarginWidth"
#define SListCDoubleClickDelay		"DoubleClickDelay"
#define SListCSeparatorWidth            "SeparatorWidth"
#define SListCCols			"Cols"
#define SListCRows			"Rows"

/* Callback structures */

typedef struct {
    int         reason;
    XEvent      *event;
    int         item_number;            /* Item number selected */
    short	select_count;		/* Total number selected */
    short       item_count;             /* Number of items selected this time */
    Boolean     select;                 /* true for select,
					   false for deselect */
    char	**data;			/* pointer to data storage for
					   getData callbacks */
} SListCallbackStruct;

/* Callback reasons */

#define SListCRSelect           1
#define SListCRDoubleClick      2
#define SListCRGetData		3

void SListLoad(/* widget, lineCount */);
void SListSetTop(/* widget, item */);
int  SListGetTop(/* widget */);
int  SListGetRows(/* widget */);
int  SListGetBottom(/* widget */);
void SListUpdateLine(/* widget, line */);
void SListSelectAll(/* widget, select */);
void SListUnhighlight(/* widget */);
void SListHighlight(/* widget , line */);
int  SListGetHighlight(/* widget */);
Boolean SListIsSelected(/* widget, line */);
int SListGetSelectionCount(/* widget */);
Widget SListCreate(/*widget, name, args, argCnt */);
void SListInitializeForDRM();

#ifndef SLIST
typedef struct SListClassRec *slistwidgetclassrec;
externalref WidgetClass slistwidgetclass;
#endif

#endif /* _SList_h */
