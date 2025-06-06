/* $XConsortium: List.c,v 1.35 91/10/16 21:35:06 eswu Exp $ */

/*
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/*
 * List.c - List widget
 *
 * This is the List widget, it is useful to display a list, without the
 * overhead of having a widget for each item in the list.  It allows 
 * the user to select an item in a list and notifies the application through
 * a callback function.
 *
 *	Created: 	8/13/88
 *	By:		Chris D. Peterson
 *                      MIT X Consortium
 */


/*
 * this code was modified in 1/95 by Johannes Plass,
 * plass@dipmza.physik.uni-mainz.de
 * search for _USE_UNMODIFIED_XAW_ to find the change
 *
*/ 

#include <stdio.h>
#include <ctype.h>

/*
#define MESSAGES
#define MESSAGES1
*/
#include "message.h"

#ifdef VMS
#   include <X11_DIRECTORY/IntrinsicP.h>
#   include <X11_DIRECTORY/StringDefs.h>
#   include <XMU_DIRECTORY/Drawing.h>
#   include <XAW_DIRECTORY/XawInit.h>
#   include <XAW_DIRECTORY/ListP.h>
#else
#   include <X11/IntrinsicP.h>
#   include <X11/StringDefs.h>
#   include <X11/Xmu/Drawing.h>
#   include <X11/Xaw3d/XawInit.h>
#   include <X11/Xaw3d/ListP.h>
#endif

/* 
 * Default Translation table.
 */

static char defaultTranslations[] =  
  "<Btn1Down>:   Set()\n\
   <Btn1Up>:     Notify()";

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

#define offset(field) XtOffset(ListWidget, field)

static XtResource resources[] = {
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	offset(list.foreground), XtRString, XtDefaultForeground},
    {XtNcursor, XtCCursor, XtRCursor, sizeof(Cursor),
       offset(simple.cursor), XtRString, "left_ptr"},
    {XtNfont,  XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	offset(list.font),XtRString, XtDefaultFont},
    {XtNlist, XtCList, XtRPointer, sizeof(char **),
       offset(list.list), XtRString, NULL},
    {XtNdefaultColumns, XtCColumns, XtRInt,  sizeof(int),
	offset(list.default_cols), XtRImmediate, (XtPointer)2},
    {XtNlongest, XtCLongest, XtRInt,  sizeof(int),
	offset(list.longest), XtRImmediate, (XtPointer)0},
    {XtNnumberStrings, XtCNumberStrings, XtRInt,  sizeof(int),
	offset(list.nitems), XtRImmediate, (XtPointer)0},
    {XtNpasteBuffer, XtCBoolean, XtRBoolean,  sizeof(Boolean),
	offset(list.paste), XtRImmediate, (XtPointer) False},
    {XtNforceColumns, XtCColumns, XtRBoolean,  sizeof(Boolean),
	offset(list.force_cols), XtRImmediate, (XtPointer) False},
    {XtNverticalList, XtCBoolean, XtRBoolean,  sizeof(Boolean),
	offset(list.vertical_cols), XtRImmediate, (XtPointer) False},
    {XtNinternalWidth, XtCWidth, XtRDimension,  sizeof(Dimension),
	offset(list.internal_width), XtRImmediate, (XtPointer)4},
    {XtNinternalHeight, XtCHeight, XtRDimension, sizeof(Dimension),
	offset(list.internal_height), XtRImmediate, (XtPointer)2},
    {XtNcolumnSpacing, XtCSpacing, XtRDimension,  sizeof(Dimension),
	offset(list.column_space), XtRImmediate, (XtPointer)6},
    {XtNrowSpacing, XtCSpacing, XtRDimension,  sizeof(Dimension),
	offset(list.row_space), XtRImmediate, (XtPointer)2},
    {XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer),
        offset(list.callback), XtRCallback, NULL},
};

static void Initialize();
static void ChangeSize();
static void Resize();
static void Redisplay();
static void Destroy();
static Boolean Layout();
static XtGeometryResult PreferredGeom();
static Boolean SetValues();
static void Notify(), Set(), Unset();

static XtActionsRec actions[] = {
      {"Notify",         Notify},
      {"Set",            Set},
      {"Unset",          Unset},
};

ListClassRec listClassRec = {
  {
/* core_class fields */	
    /* superclass	  	*/	(WidgetClass) &simpleClassRec,
    /* class_name	  	*/	"List",
    /* widget_size	  	*/	sizeof(ListRec),
    /* class_initialize   	*/	XawInitializeWidgetSet,
    /* class_part_initialize	*/	NULL,
    /* class_inited       	*/	FALSE,
    /* initialize	  	*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize		  	*/	XtInheritRealize,
    /* actions		  	*/	actions,
    /* num_actions	  	*/	XtNumber(actions),
    /* resources	  	*/	resources,
    /* num_resources	  	*/	XtNumber(resources),
    /* xrm_class	  	*/	NULLQUARK,
    /* compress_motion	  	*/	TRUE,
    /* compress_exposure  	*/	FALSE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest	  	*/	FALSE,
    /* destroy		  	*/	Destroy,
    /* resize		  	*/	Resize,
    /* expose		  	*/	Redisplay,
    /* set_values	  	*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus	 	*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private   	*/	NULL,
    /* tm_table		   	*/	defaultTranslations,
   /* query_geometry		*/      PreferredGeom,
  },
/* Simple class fields initialization */
  {
    /* change_sensitive		*/	XtInheritChangeSensitive
  },
/* List class fields initialization */
  {
    /* not used			*/	0
  }
};

WidgetClass listWidgetClass = (WidgetClass)&listClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static void GetGCs(w)
Widget w;
{
    XGCValues	values;
    ListWidget lw = (ListWidget) w;    

    values.foreground	= lw->list.foreground;
    values.font		= lw->list.font->fid;
    lw->list.normgc = XtGetGC(w, (unsigned) GCForeground | GCFont,
				 &values);

    values.foreground	= lw->core.background_pixel;
    lw->list.revgc = XtGetGC(w, (unsigned) GCForeground | GCFont,
				 &values);

    values.tile       = XmuCreateStippledPixmap(XtScreen(w), 
						lw->list.foreground,
						lw->core.background_pixel,
						lw->core.depth);
    values.fill_style = FillTiled;

    lw->list.graygc = XtGetGC(w, (unsigned) GCFont | GCTile | GCFillStyle,
			      &values);
}

/*	Function Name: ResetList
 *	Description: Resets the new list when important things change.
 *	Arguments: w - the widget.
 *                 changex, changey - allow the height or width to change?
 *	Returns: none.
 */

static void
ResetList(w, changex, changey)
Widget w;
Boolean changex, changey;
{
    ListWidget lw = (ListWidget) w;
    Dimension width = w->core.width;
    Dimension height = w->core.height;
    register int i, len;

    BEGINMESSAGE(ResetList)
/*
 * If list is NULL then the list will just be the name of the widget.
 */

    if (lw->list.list == NULL) {
      lw->list.list = &(lw->core.name);
      lw->list.nitems = 1;
    }

    if (lw->list.nitems == 0)	    /* Get number of items. */
        for ( ; lw->list.list[lw->list.nitems] != NULL ; lw->list.nitems++);

    if (lw->list.longest == 0) /* Get column width. */
        for ( i = 0 ; i < lw->list.nitems; i++) {
	    len = XTextWidth(lw->list.font, lw->list.list[i],
			     strlen(lw->list.list[i]));
	    if (len > lw->list.longest)
	        lw->list.longest = len;
	}

    lw->list.col_width = lw->list.longest + lw->list.column_space;

    if (Layout(w, changex, changey, &width, &height))
      ChangeSize(w, width, height);
    ENDMESSAGE(ResetList)

}

/*	Function Name: ChangeSize.
 *	Description: Laysout the widget.
 *	Arguments: w - the widget to try change the size of.
 *	Returns: none.
 */

static void
ChangeSize(w, width, height)
Widget w;
Dimension width, height;
{
    XtWidgetGeometry request, reply;

    BEGINMESSAGE(ChangeSize)
    INFSMESSAGE(changing size of list,XtName(w))
    request.request_mode = CWWidth | CWHeight;
    request.width = width;
    request.height = height;

    INFIIMESSAGE(current size,w->core.width,w->core.height)
    INFIIMESSAGE(requesting,width,height)
    switch ( XtMakeGeometryRequest(w, &request, &reply) ) {
    case XtGeometryYes:
        INFMESSAGE(reply on geometry request: Yes)
        break;
    case XtGeometryNo:
        INFMESSAGE(reply on geometry request: No)
        break;
    case XtGeometryAlmost:
        INFMESSAGE(reply on geometry request: Almost)
	Layout(w, (request.height != reply.height),
	          (request.width != reply.width),
	       &(reply.width), &(reply.height));
	request = reply;
	switch (XtMakeGeometryRequest(w, &request, &reply) ) {
	case XtGeometryYes:
            INFMESSAGE(reply on new geometry request: Yes)
	    break;
	case XtGeometryNo:
            INFMESSAGE(reply on new geometry request: No)
	    break;
	case XtGeometryAlmost:
            INFMESSAGE(reply on new geometry request: Almost)
	    request = reply;
	    if (Layout(w, FALSE, FALSE,
		       &(request.width), &(request.height))) {
	      char buf[BUFSIZ];
	      (void) sprintf(buf, "List Widget: %s %s",
		      "Size Changed when it shouldn't have",
		      "when computing layout");
	      XtAppWarning(XtWidgetToApplicationContext(w), buf);
	    }
	    request.request_mode = CWWidth | CWHeight;
            INFMESSAGE(making last geometry request)
	    XtMakeGeometryRequest(w, &request, &reply);
	    break;
	default:
	  XtAppWarning(XtWidgetToApplicationContext(w),
		       "List Widget: Unknown geometry return.");
	  break;
	}
	break;
    default:
	XtAppWarning(XtWidgetToApplicationContext(w),
		     "List Widget: Unknown geometry return.");
	break;
    }
    INFIIMESSAGE(current size,w->core.width,w->core.height)
    ENDMESSAGE(ChangeSize)

}

/*	Function Name: Initialize
 *	Description: Function that initilizes the widget instance.
 *	Arguments: junk - NOT USED.
 *                 new  - the new widget.
 *	Returns: none
 */

/* ARGSUSED */
static void 
Initialize(junk, new, args, num_args)
Widget junk, new;
ArgList args;
Cardinal *num_args;
{
    ListWidget lw = (ListWidget) new;

/* 
 * Initialize all private resources.
 */
    BEGINMESSAGE(Initialize)
    GetGCs(new);

    /* Set row height. */
    lw->list.row_height = lw->list.font->max_bounds.ascent
			+ lw->list.font->max_bounds.descent
			+ lw->list.row_space;

    ResetList(new, (new->core.width == 0), (new->core.height == 0));

    lw->list.highlight = lw->list.is_highlighted = NO_HIGHLIGHT;
    ENDMESSAGE(Initialize)

} /* Initialize */

/*	Function Name: CvtToItem
 *	Description: Converts Xcoord to item number of item containing that
 *                   point.
 *	Arguments: w - the list widget.
 *                 xloc, yloc - x location, and y location.
 *	Returns: the item number.
 */

static int
CvtToItem(w, xloc, yloc, item)
Widget w;
int xloc, yloc;
int *item;
{
    int one, another;
    ListWidget lw = (ListWidget) w;
    int ret_val = OKAY;

    if (lw->list.vertical_cols) {
        one = lw->list.nrows * ((xloc - (int) lw->list.internal_width)
	    / lw->list.col_width);
        another = (yloc - (int) lw->list.internal_height) 
	        / lw->list.row_height;
	 /* If out of range, return minimum possible value. */
	if (another >= lw->list.nrows) {
	    another = lw->list.nrows - 1;
	    ret_val = OUT_OF_RANGE;
	}
    }
    else {
        one = (lw->list.ncols * ((yloc - (int) lw->list.internal_height) 
              / lw->list.row_height)) ;
	/* If in right margin handle things right. */
        another = (xloc - (int) lw->list.internal_width) / lw->list.col_width;
	if (another >= lw->list.ncols) {
	    another = lw->list.ncols - 1; 
	    ret_val = OUT_OF_RANGE;
	}
    }  
    if ((xloc < 0) || (yloc < 0))
        ret_val = OUT_OF_RANGE;
    if (one < 0) one = 0;
    if (another < 0) another = 0;
    *item = one + another;
    if (*item >= lw->list.nitems) return(OUT_OF_RANGE);
    return(ret_val);
}

/*	Function Name: FindCornerItems.
 *	Description: Find the corners of the rectangle in item space.
 *	Arguments: w - the list widget.
 *                 event - the event structure that has the rectangle it it.
 *                 ul_ret, lr_ret - the corners ** RETURNED **.
 *	Returns: none.
 */

static void
FindCornerItems(w, event, ul_ret, lr_ret)
Widget w;
XEvent * event;
int *ul_ret, *lr_ret;
{
    int xloc, yloc;

    xloc = event->xexpose.x;
    yloc = event->xexpose.y;
    CvtToItem(w, xloc, yloc, ul_ret);
    xloc += event->xexpose.width;
    yloc += event->xexpose.height;
    CvtToItem(w, xloc, yloc, lr_ret);
}

/*	Function Name: ItemInRectangle
 *	Description: returns TRUE if the item passed is in the given rectangle.
 *	Arguments: w - the list widget.
 *                 ul, lr - corners of the rectangle in item space.
 *                 item - item to check.
 *	Returns: TRUE if the item passed is in the given rectangle.
 */

static Boolean
ItemInRectangle(w, ul, lr, item)
Widget w;
int ul, lr, item;
{
    ListWidget lw = (ListWidget) w;
    register int mod_item;
    int things;
    
    if (item < ul || item > lr) 
        return(FALSE);
    if (lw->list.vertical_cols)
        things = lw->list.nrows;
    else
        things = lw->list.ncols;

    mod_item = item % things;
    if ( (mod_item >= ul % things) && (mod_item <= lr % things ) )
        return(TRUE);
    return(FALSE);
}

/*	Function Name: HighlightBackground
 *	Description: paints the color of the background for the given item.
 *	Arguments: w - the widget.
 *                 x, y - ul corner of the area item occupies.
 *                 item - the item we are dealing with.
 *                 gc - the gc that is used to paint this rectangle
 *	Returns: 
 */

static void
HighlightBackground(w, x, y, item, gc)
Widget w;
int x, y, item;
GC gc;
{
    ListWidget lw = (ListWidget) w;
    int hl_x, hl_y, width, height;

    hl_x = x - lw->list.column_space/2;
    width = XTextWidth(lw->list.font, lw->list.list[item],
			 strlen(lw->list.list[item])) + lw->list.column_space;
    hl_y = y - lw->list.row_space/2;
    height = lw->list.row_height + lw->list.row_space;

    XFillRectangle(XtDisplay(w), XtWindow(w), gc, hl_x, hl_y, width, height);
}

/*	Function Name: PaintItemName
 *	Description: paints the name of the item in the appropriate location.
 *	Arguments: w - the list widget.
 *                 item - the item to draw.
 *	Returns: none.
 *
 *      NOTE: no action taken on an unrealized widget.
 */

static void
PaintItemName(w, item)
Widget w;
int item;
{
    char * str;
    GC gc;
    int x, y, str_y;
    ListWidget lw = (ListWidget) w;

    if (!XtIsRealized(w)) return; /* Just in case... */
   
    if (lw->list.vertical_cols) {
	x = lw->list.col_width * (item / lw->list.nrows)
	  + lw->list.internal_width;
        y = lw->list.row_height * (item % lw->list.nrows)
	  + lw->list.internal_height;
    }
    else {
        x = lw->list.col_width * (item % lw->list.ncols)
	  + lw->list.internal_width;
        y = lw->list.row_height * (item / lw->list.ncols)
	  + lw->list.internal_height;
    }

    str_y = y + lw->list.font->max_bounds.ascent;

    if (item == lw->list.is_highlighted) {
        if (item == lw->list.highlight) {
            gc = lw->list.revgc;
	    HighlightBackground(w, x, y, item, lw->list.normgc);
	}
        else {
	    if (XtIsSensitive(w)) 
	        gc = lw->list.normgc;
	    else
	        gc = lw->list.graygc;
	    HighlightBackground(w, x, y, item, lw->list.revgc);
	    lw->list.is_highlighted = NO_HIGHLIGHT;
        }
    }
    else {
        if (item == lw->list.highlight) {
            gc = lw->list.revgc;
	    HighlightBackground(w, x, y, item, lw->list.normgc);
	    lw->list.is_highlighted = item;
	}
	else {
	    if (XtIsSensitive(w)) 
	        gc = lw->list.normgc;
	    else
	        gc = lw->list.graygc;
	}
    }

    str =  lw->list.list[item];	/* draw it */
    XDrawString(XtDisplay(w), XtWindow(w), gc, x, str_y, str, strlen(str));
}
    
/*	Function Name: Redisplay
 *	Description: Repaints the widget window on expose events.
 *	Arguments: w - the list widget.
 *                 event - the expose event for this repaint.
 *                 junk - NOT USED.
 *	Returns: 
 */

/* ARGSUSED */
static void 
Redisplay(w, event, junk)
Widget w;
XEvent *event;
Region junk;
{
    int item;			/* an item to work with. */
    int ul_item, lr_item;       /* corners of items we need to paint. */
    ListWidget lw = (ListWidget) w;

    BEGINMESSAGE1(Redisplay)
    INFSMESSAGE1(redisplaying list,XtName(w))
    if (event == NULL) {	/* repaint all. */
        ul_item = 0;
	lr_item = lw->list.nrows * lw->list.ncols - 1;
	XClearWindow(XtDisplay(w), XtWindow(w));
    }
    else
        FindCornerItems(w, event, &ul_item, &lr_item);
    
    for (item = ul_item; (item <= lr_item && item < lw->list.nitems) ; item++)
      if (ItemInRectangle(w, ul_item, lr_item, item))
	PaintItemName(w, item);
    ENDMESSAGE1(Redisplay)
}

/*	Function Name: PreferredGeom
 *	Description: This tells the parent what size we would like to be
 *                   given certain constraints.
 *	Arguments: w - the widget.
 *                 intended - what the parent intends to do with us.
 *                 requested - what we want to happen.
 *	Returns: none.
 */

static XtGeometryResult 
PreferredGeom(w, intended, requested)
Widget w;
XtWidgetGeometry *intended, *requested;
{
    Dimension new_width, new_height;
    Boolean change, width_req, height_req;

    BEGINMESSAGE(PreferredGeom)
    width_req = intended->request_mode & CWWidth;
    height_req = intended->request_mode & CWHeight;

#if _USE_UNMODIFIED_XAW_ /*###jp###*/

    if (width_req)
      new_width = intended->width;
    else
      new_width = w->core.width;

    if (height_req)
      new_height = intended->height;
    else
      new_height = w->core.height;

    requested->request_mode = 0;

/*
 * We only care about our height and width.
 */

    if ( !width_req && !height_req) {
       INFMESSAGE(not a width or height query)
       ENDMESSAGE(PreferredGeom)
       return(XtGeometryYes);
    }
    
    change = Layout(w, !width_req, !height_req, &new_width, &new_height);
    INFIIMESSAGE(wanted size,new_width,new_height)

    requested->request_mode |= CWWidth;
    requested->width = new_width;
    requested->request_mode |= CWHeight;
    requested->height = new_height;

    if (change) {
        INFMESSAGE(returning: Almost) ENDMESSAGE(PreferredGeom)
        return(XtGeometryAlmost);
    }

#else /*_USE_UNMODIFIED_XAW_*/

    /*
       Introduced this change since a mode bit not set
       in 'intended' indicates that we are free to change
       the corresponding geometry field ###jp###
    */

    if (width_req) new_width = intended->width;
    else           new_width = w->core.width;

    if (height_req) new_height = intended->height;
    else            new_height = w->core.height;

#ifdef MESSAGES
    if (width_req) {
       INFIMESSAGE(parent intends,intended->width)
    } else {
       INFMESSAGE(parent does not intend width)
    }
    if (width_req) {
       INFIMESSAGE(parent intends,intended->height)
    } else {
       INFMESSAGE(parent does not intend height)
    }
#endif

    Layout(w, !width_req, !height_req, &new_width, &new_height);
    INFIIMESSAGE(current:,w->core.width,w->core.height)
    INFIIMESSAGE(calculated preferred:,new_width,new_height)
    requested->request_mode = (CWWidth|CWHeight);
    requested->width  = new_width;
    requested->height = new_height;

    if ( !width_req    ||
         !height_req   ||
         (width_req &&  (requested->width  != intended->width))  ||
         (height_req && (requested->height != intended->height))
       ) {
        INFMESSAGE(returning: Almost) ENDMESSAGE(PreferredGeom)
        return(XtGeometryAlmost);
    }

    if ( (requested->width  == w->core.width)  &&
         (requested->height == w->core.height)
       ) {
       /*
          if the preferred geometry equals the current geometry
          we return XtGeometryNo
       */
       INFMESSAGE(preferred geometry equals current geometry)
       ENDMESSAGE(PreferredGeom)
       return(XtGeometryNo);
    }

#endif /*_USE_UNMODIFIED_XAW_*/

    INFMESSAGE(returning: Yes)
    ENDMESSAGE(PreferredGeom)
    return(XtGeometryYes);
}

/*	Function Name: Resize
 *	Description: resizes the widget, by changing the number of rows and
 *                   columns.
 *	Arguments: w - the widget.
 *	Returns: none.
 */

static void
Resize(w)
    Widget w;
{
    Dimension width, height;

    BEGINMESSAGE(Resize)
    width = w->core.width;
    height = w->core.height;

    if (Layout(w, FALSE, FALSE, &width, &height))
	XtAppWarning(XtWidgetToApplicationContext(w),
	   "List Widget: Size changed when it shouldn't have when resising.");
    ENDMESSAGE(Resize)
}

/*	Function Name: Layout
 *	Description: lays out the item in the list.
 *	Arguments: w - the widget.
 *                 xfree, yfree - TRUE if we are free to resize the widget in
 *                                this direction.
 *                 width, height - the is the current width and height that 
 *                                 we are going to layout the list widget to,
 *                                 depending on xfree and yfree of course.
 *                               
 *	Returns: TRUE if width or height have been changed.
 */

static Boolean
Layout(w, xfree, yfree, width, height)
Widget w;
Boolean xfree, yfree;
Dimension *width, *height;
{
    ListWidget lw = (ListWidget) w;
    Boolean change = FALSE;

    BEGINMESSAGE(Layout)
    INFSMESSAGE(layouting list,XtName(w))
    
/* 
 * If force columns is set then always use number of columns specified
 * by default_cols.
 */

    if (lw->list.force_cols) {
        INFMESSAGE(forcing columns)
        lw->list.ncols = lw->list.default_cols;
	if (lw->list.ncols <= 0) lw->list.ncols = 1;
	/* 12/3 = 4 and 10/3 = 4, but 9/3 = 3 */
	lw->list.nrows = ( ( lw->list.nitems - 1) / lw->list.ncols) + 1 ;
	if (xfree) {		/* If allowed resize width. */
            INFMESSAGE(free to change width)
	    *width = lw->list.ncols * lw->list.col_width 
	           + 2 * lw->list.internal_width;
	    change = TRUE;
	}
	if (yfree) {		/* If allowed resize height. */
            INFMESSAGE(free to change height)
	    *height = (lw->list.nrows * lw->list.row_height)
                    + 2 * lw->list.internal_height;
	    change = TRUE;
	}
        ENDMESSAGE(Layout)
	return(change);
    }

/*
 * If both width and height are free to change the use default_cols
 * to determine the number columns and set new width and height to
 * just fit the window.
 */

    if (xfree && yfree) {
        INFMESSAGE(free to change width and height)
        lw->list.ncols = lw->list.default_cols;
	if (lw->list.ncols <= 0) lw->list.ncols = 1;
	lw->list.nrows = ( ( lw->list.nitems - 1) / lw->list.ncols) + 1 ;
        *width = lw->list.ncols * lw->list.col_width
	       + 2 * lw->list.internal_width;
	*height = (lw->list.nrows * lw->list.row_height)
                + 2 * lw->list.internal_height;
	change = TRUE;
    }
/* 
 * If the width is fixed then use it to determine the number of columns.
 * If the height is free to move (width still fixed) then resize the height
 * of the widget to fit the current list exactly.
 */
    else if (!xfree) {
        INFMESSAGE(width is fixed)
        lw->list.ncols = ( (int)(*width - 2 * lw->list.internal_width)
	                    / (int)lw->list.col_width);
	if (lw->list.ncols <= 0) lw->list.ncols = 1;
	lw->list.nrows = ( ( lw->list.nitems - 1) / lw->list.ncols) + 1 ;
	if ( yfree ) {
            INFMESSAGE(height is free)
  	    *height = (lw->list.nrows * lw->list.row_height)
		    + 2 * lw->list.internal_height;
	    change = TRUE;
	}
    }
/* 
 * The last case is xfree and !yfree we use the height to determine
 * the number of rows and then set the width to just fit the resulting
 * number of columns.
 */
    else if (!yfree) {		/* xfree must be TRUE. */
        INFMESSAGE(width is free but height is fixed)
        lw->list.nrows = (int)(*height - 2 * lw->list.internal_height) 
	                 / (int)lw->list.row_height;
	if (lw->list.nrows <= 0) lw->list.nrows = 1;
	lw->list.ncols = (( lw->list.nitems - 1 ) / lw->list.nrows) + 1;
	*width = lw->list.ncols * lw->list.col_width 
	       + 2 * lw->list.internal_width;
	change = TRUE;
    }      
    ENDMESSAGE(Layout)
    return(change);
}

/*	Function Name: Notify
 *	Description: Notifies the user that a button has been pressed, and
 *                   calles the callback, if the XtNpasteBuffer resource
 *                   is true then the name of the item is also put in the
 *                   X cut buffer ( buf (0) ).
 *	Arguments: w - the widget that the notify occured in.
 *                 event - event that caused this notification.
 *                 params, num_params - not used.
 *	Returns: none.
 */

/* ARGSUSED */
static void
Notify(w, event, params, num_params)
Widget w;
XEvent * event;
String * params;
Cardinal *num_params;
{
    ListWidget lw = ( ListWidget ) w;
    int item, item_len;
    XawListReturnStruct ret_value;

/* 
 * Find item and if out of range then unhighlight and return. 
 * 
 * If the current item is unhighlighted then the user has aborted the
 * notify, so unhighlight and return.
 */

    if ( ((CvtToItem(w, event->xbutton.x, event->xbutton.y, &item))
	  == OUT_OF_RANGE) || (lw->list.highlight != item) ) {
        XawListUnhighlight(w);
        return;
    }

    item_len = strlen(lw->list.list[item]);

    if ( lw->list.paste )	/* if XtNpasteBuffer set then paste it. */
        XStoreBytes(XtDisplay(w), lw->list.list[item], item_len);

/* 
 * Call Callback function.
 */

    ret_value.string = lw->list.list[item];
    ret_value.list_index = item;
    
    XtCallCallbacks( w, XtNcallback, (XtPointer) &ret_value);
}

/*	Function Name: Unset
 *	Description: unhighlights the current element.
 *	Arguments: w - the widget that the event occured in.
 *                 event - not used.
 *                 params, num_params - not used.
 *	Returns: none.
 */

/* ARGSUSED */
static void
Unset(w, event, params, num_params)
Widget w;
XEvent * event;
String * params;
Cardinal *num_params;
{
  XawListUnhighlight(w);
}

/*	Function Name: Set
 *	Description: Highlights the current element.
 *	Arguments: w - the widget that the event occured in.
 *                 event - event that caused this notification.
 *                 params, num_params - not used.
 *	Returns: none.
 */

/* ARGSUSED */
static void
Set(w, event, params, num_params)
Widget w;
XEvent * event;
String * params;
Cardinal *num_params;
{
  int item;
  ListWidget lw = (ListWidget) w;

  if ( (CvtToItem(w, event->xbutton.x, event->xbutton.y, &item))
      == OUT_OF_RANGE)
    XawListUnhighlight(w);		        /* Unhighlight current item. */
  else if ( lw->list.is_highlighted != item )   /* If this item is not */
    XawListHighlight(w, item);	                /* highlighted then do it. */
}

/*
 * Set specified arguments into widget
 */

static Boolean 
SetValues(current, request, new, args, num_args)
Widget current, request, new;
ArgList args;
Cardinal *num_args;
{
    ListWidget cl = (ListWidget) current;
    ListWidget rl = (ListWidget) request;
    ListWidget nl = (ListWidget) new;
    Boolean redraw = FALSE;

    BEGINMESSAGE(SetValues)
    INFSMESSAGE(setting values of list,XtName(new))
    if ((cl->list.foreground != rl->list.foreground) ||
	(cl->core.background_pixel != rl->core.background_pixel) ||
	(cl->list.font != rl->list.font) ) {
	XGCValues values;
	XGetGCValues(XtDisplay(current), cl->list.graygc, GCTile, &values);
	XmuReleaseStippledPixmap(XtScreen(current), values.tile);
	XtReleaseGC(current, cl->list.graygc);
	XtReleaseGC(current, cl->list.revgc);
	XtReleaseGC(current, cl->list.normgc);
        GetGCs(new);
        redraw = TRUE;
    }

    /* Reset row height. */

    if ((cl->list.row_space != rl->list.row_space) ||
	(cl->list.font != rl->list.font)) 
        nl->list.row_height = nl->list.font->max_bounds.ascent
	                    + nl->list.font->max_bounds.descent
			    + nl->list.row_space;
    
    if ((cl->core.width != rl->core.width)                     ||
	(cl->core.height != rl->core.height)                   ||
	(cl->list.internal_width != rl->list.internal_width)   ||
	(cl->list.internal_height != rl->list.internal_height) ||
	(cl->list.column_space != rl->list.column_space)       ||
	(cl->list.row_space != rl->list.row_space)             ||
	(cl->list.default_cols != rl->list.default_cols)       ||
	(  (cl->list.force_cols != rl->list.force_cols) &&
	   (rl->list.force_cols != rl->list.ncols) )           ||
	(cl->list.vertical_cols != rl->list.vertical_cols)     ||
	(cl->list.longest != rl->list.longest)                 ||
	(cl->list.nitems != rl->list.nitems)                   ||
	(cl->list.font != rl->list.font)                       ||
	(cl->list.list != rl->list.list)                        ) {

      ResetList(new, TRUE, TRUE);
      redraw = TRUE;
    }

    if (cl->list.list != rl->list.list)
	nl->list.is_highlighted = nl->list.highlight = NO_HIGHLIGHT;

    if ((cl->core.sensitive != rl->core.sensitive) ||
	(cl->core.ancestor_sensitive != rl->core.ancestor_sensitive)) {
        nl->list.highlight = NO_HIGHLIGHT;
	redraw = TRUE;
    }
    
    if (!XtIsRealized(current)) {
      INFMESSAGE(not realize) ENDMESSAGE(SetValues)
      return(FALSE);
    }

    ENDMESSAGE(SetValues)
    return(redraw);
}

static void Destroy(w)
    Widget w;
{
    ListWidget lw = (ListWidget) w;
    XGCValues values;

    BEGINMESSAGE(Destroy)
    XGetGCValues(XtDisplay(w), lw->list.graygc, GCTile, &values);
    XmuReleaseStippledPixmap(XtScreen(w), values.tile);
    XtReleaseGC(w, lw->list.graygc);
    XtReleaseGC(w, lw->list.revgc);
    XtReleaseGC(w, lw->list.normgc);
    ENDMESSAGE(Destroy)
}

/* Exported Functions */

/*	Function Name: XawListChange.
 *	Description: Changes the list being used and shown.
 *	Arguments: w - the list widget.
 *                 list - the new list.
 *                 nitems - the number of items in the list.
 *                 longest - the length (in Pixels) of the longest element
 *                           in the list.
 *                 resize - if TRUE the the list widget will
 *                          try to resize itself.
 *	Returns: none.
 *      NOTE:      If nitems of longest are <= 0 then they will be calculated.
 *                 If nitems is <= 0 then the list needs to be NULL terminated.
 */

void
#if NeedFunctionPrototypes
XawListChange(Widget w, char ** list, int nitems, int longest,
#if NeedWidePrototypes
	      int resize_it)
#else
	      Boolean resize_it)
#endif
#else
XawListChange(w, list, nitems, longest, resize_it)
Widget w;
char ** list;
int nitems, longest;
Boolean resize_it;
#endif
{
    ListWidget lw = (ListWidget) w;

    BEGINMESSAGE(XawListChange)
    lw->list.list = list;

    if (nitems <= 0) nitems = 0;
    lw->list.nitems = nitems;
    if (longest <= 0) longest = 0;
    lw->list.longest = longest;

    ResetList(w, resize_it, resize_it);
    lw->list.is_highlighted = lw->list.highlight = NO_HIGHLIGHT;
    if ( XtIsRealized(w) )
      Redisplay(w, (XEvent *)NULL, (Region)NULL);
    ENDMESSAGE(XawListChange)
}

/*	Function Name: XawListUnhighlight
 *	Description: unlights the current highlighted element.
 *	Arguments: w - the widget.
 *	Returns: none.
 */

void
#if NeedFunctionPrototypes
XawListUnhighlight(Widget w)
#else
XawListUnhighlight(w)
Widget w;
#endif
{
    ListWidget lw = ( ListWidget ) w;

    lw->list.highlight = NO_HIGHLIGHT;
    if (lw->list.is_highlighted != NO_HIGHLIGHT)
        PaintItemName(w, lw->list.is_highlighted); /* unhighlight this one. */
}

/*	Function Name: XawListHighlight
 *	Description: Highlights the given item.
 *	Arguments: w - the list widget.
 *                 item - the item to hightlight.
 *	Returns: none.
 */

void
#if NeedFunctionPrototypes
XawListHighlight(Widget w, int item)
#else
XawListHighlight(w, item)
Widget w;
int item;
#endif
{
    ListWidget lw = ( ListWidget ) w;
    
    if (XtIsSensitive(w)) {
        lw->list.highlight = item;
        if (lw->list.is_highlighted != NO_HIGHLIGHT)
            PaintItemName(w, lw->list.is_highlighted);  /* Unhighlight. */
	PaintItemName(w, item); /* HIGHLIGHT this one. */ 
    }
}

/*	Function Name: XawListShowCurrent
 *	Description: returns the currently highlighted object.
 *	Arguments: w - the list widget.
 *	Returns: the info about the currently highlighted object.
 */

XawListReturnStruct *
#if NeedFunctionPrototypes
XawListShowCurrent(Widget w)
#else
XawListShowCurrent(w)
Widget w;
#endif
{
    ListWidget lw = ( ListWidget ) w;
    XawListReturnStruct * ret_val;

    ret_val = (XawListReturnStruct *) XtMalloc (sizeof (XawListReturnStruct));
    
    ret_val->list_index = lw->list.highlight;
    if (ret_val->list_index == XAW_LIST_NONE)
      ret_val->string = "";
    else
      ret_val->string = lw->list.list[ ret_val->list_index ];

    return(ret_val);
}

