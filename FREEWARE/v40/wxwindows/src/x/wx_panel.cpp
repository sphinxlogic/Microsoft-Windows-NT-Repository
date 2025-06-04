/*
 * File:      wx_panel.cc
 * Purpose:     wxPanel class implementation (X version)
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_panel.cc,v 1.3 1994/08/14 21:28:43 edz Exp $
 * Copyright:   (c) 1993, AIAI, University of Edinburgh
 */

static const char sccsid[] = "@(#)wx_panel.cc	1.2 5/9/94";

#ifdef __GNUG__
#pragma implementation
#endif

#include "wx_defs.h"
#include "wx_panel.h"
#include "wx_frame.h"
#include "wx_utils.h"
#include "wx_item.h"
#include "wx_dcpan.h"
#include "wx_privt.h"
#include "wx_dialg.h"

#ifdef wx_xview
#include "wx_text.h"
#endif

#if USE_EXTENDED_STATICS
#include "wx_stat.h"
#endif

#ifdef wx_motif
#include <Xm/Form.h>
#include <Xm/BulletinB.h>
#include <Xm/PushB.h>
#include <Xm/Frame.h>
#include <Xm/RowColumn.h>
extern void wxPanelItemEventHandler (Widget    wid,
                              XtPointer client_data,
                              XEvent*   event,
                              Boolean *continueToDispatch);
#endif

#ifdef wx_xview
#include <xview/panel.h>
#include <xview/scrollbar.h>
#include <xview/dragdrop.h>
extern void wxPanelEventProc (Panel panel, Event * event);	// See wx_item.cc
void wxPanelInterposeProc(Panel panel, Event *event, Notify_arg arg, Notify_event_type type);
#endif

// Constructor
wxPanel::wxPanel (void)
{
  new_line = FALSE;
  SetLabelPosition(wxHORIZONTAL);

  hSpacing = PANEL_HSPACING;
  vSpacing = PANEL_VSPACING;
  initial_hspacing = hSpacing;
  initial_vspacing = vSpacing;
  current_hspacing = hSpacing;
  current_vspacing = vSpacing;

  panelBackgroundBrush = NULL;

  labelFont = NULL;
  buttonFont = NULL;
  backColour = *wxWHITE;
  labelColour = *wxBLACK;
  buttonColour = *wxBLACK;

#ifdef wx_motif
  // For absolute-positioning auto layout
  cursor_x = PANEL_LEFT_MARGIN;
  cursor_y = PANEL_TOP_MARGIN;
  max_height = 0;
  max_line_height = 0;
  max_width = 0;
  last_created = 0;
  borderWidget = 0;
  new_line = FALSE;

  firstRowWidget = NULL;
  lastWidget = NULL;
  panelWidget = NULL;
  allRelative = TRUE;
  currentRow = 0;
  currentCol = 0;
  handle = NULL;

  // Focus processing
  previousFocus = NULL;
  manualChange = FALSE;
#endif
  has_child = FALSE;
  wx_dc = NULL;

}

wxPanel::wxPanel (wxWindow *parent, int x, int y, int width, int height,
	 long style, Constdata char *name):
wxbPanel (parent, x, y, width, height, style, name)
{
  Create (parent, x, y, width, height, style, name);
}

Bool wxPanel::
Create (wxWindow *parent, int x, int y, int width, int height,
	long style, Constdata char *name)
{
  wx_dc = NULL;

  SetName(name);
  new_line = FALSE;
  

  if (!parent)
    return FALSE;

  window_parent = parent;

  panelBackgroundBrush = NULL;

  has_child = FALSE;
  windowStyle = style;

  if (windowStyle & wxVERTICAL_LABEL)
    SetLabelPosition(wxVERTICAL);
  else if (windowStyle & wxHORIZONTAL_LABEL)
    SetLabelPosition(wxHORIZONTAL);
  else
    SetLabelPosition(wxHORIZONTAL);

  hSpacing = PANEL_HSPACING;
  vSpacing = PANEL_VSPACING;

  initial_hspacing = hSpacing;
  initial_vspacing = vSpacing;

  current_hspacing = hSpacing;
  current_vspacing = vSpacing;

  labelFont = NULL;
  buttonFont = NULL;

  backColour = *wxWHITE;
  labelColour = *wxBLACK;
  buttonColour = *wxBLACK;

#ifdef wx_motif
  // Focus processing
  previousFocus = NULL;
  manualChange = FALSE;

  // For absolute-positioning auto layout
  cursor_x = PANEL_LEFT_MARGIN;
  cursor_y = PANEL_TOP_MARGIN;
  max_height = 0;
  max_line_height = 0;
  max_width = 0;

  borderWidget = 0;

  firstRowWidget = NULL;
  lastWidget = NULL;
  allRelative = TRUE;
  panelWidget = NULL;

  currentRow = 0;
  currentCol = 0;

  last_created = 0;

  // Must create panel big for Motif to resize properly later
  if (width == -1)
    width = 1000;
  if (height == -1)
    height = 1000;

  Widget parentWidget = 0;
  if (parent->IsKindOf(CLASSINFO(wxFrame)))
    parentWidget = ((wxFrame *)parent)->clientArea;
  else if (parent->IsKindOf(CLASSINFO(wxPanel)))
    parentWidget = (Widget)parent->handle;
  else
  {
    wxError("Panel subwindow must be a child of either a frame or panel!");
    return FALSE;
  }

  wxCanvas::Create(parent, x, y, width, height, style, name);
  panelWidget = (Widget)handle;

  if (parent->IsKindOf(CLASSINFO(wxPanel)))
  {
    wxPanel *parentPanel = (wxPanel *)parent;
    parentPanel->AttachWidget (this, panelWidget, x, y, width, height);
    parentPanel->GetValidPosition(&x,&y) ;

    labelFont = parentPanel->labelFont ;
    buttonFont = parentPanel->buttonFont ;
    backColour = parentPanel->backColour ;
    labelColour = parentPanel->labelColour ;
    buttonColour = parentPanel->buttonColour ;
  }

  // Construct a new brush that takes on the
  // real background colour of this panel.
  panelBackgroundBrush = new wxBrush;
  Pixel thePix;
  XtVaGetValues(panelWidget, XmNbackground, &thePix, NULL);
  panelBackgroundBrush->colour.pixel = thePix;
  GetPanelDC()->SetBackground(panelBackgroundBrush);
#endif // wx_motif
#ifdef wx_xview
  // Compute my creation pos.
  if (parent->IsKindOf(CLASSINFO(wxPanel)))
  {
    wxPanel *father = (wxPanel *)parent;
    father->GetValidPosition (&x, &y);
  }

  wxWindow *father = parent;
  while (!father->IsKindOf(CLASSINFO(wxFrame)))
    {
      int xx, yy;
      father->GetPosition(&xx, &yy);
      x += xx;
      y += yy;
      father = father->GetParent ();
    }
  int y_offset = ((wxFrame *)father)->y_offset;
  if ((y > -1) && (parent == father)) // If direct child of frame, add offset,
                                      // else will have been dealt with already
    y += y_offset;	// Allow for possible menu bar

  Bool scrollable = style & wxVSCROLL;

  Frame x_frame = (Frame) (father->GetHandle ());
  Panel x_panel;
  
  if (!(father->GetWindowStyleFlag() & wxPUSH_PIN) ||
       parent->IsKindOf(CLASSINFO(wxPanel)))
    x_panel = (Panel) xv_create (x_frame, scrollable ? SCROLLABLE_PANEL : PANEL, NULL);
  else 
    x_panel = (Panel) xv_get (x_frame,  // We should have a panel already
                           FRAME_CMD_PANEL);

  xv_set(x_panel,   PANEL_BACKGROUND_PROC, wxPanelEventProc,
		    PANEL_ACCEPT_KEYSTROKE, TRUE,
		    WIN_CLIENT_DATA, (char *) this,
		    PANEL_LAYOUT, PANEL_HORIZONTAL, XV_SHOW, FALSE,
		    NULL);

  notify_interpose_event_func(x_panel, (Notify_func)wxPanelInterposeProc, NOTIFY_SAFE);

  Scrollbar sb;
  if (scrollable)
    sb = xv_create (x_panel, SCROLLBAR,
		    NULL);

  if (x > -1)
    {
      xv_set (x_panel, XV_X, x, NULL);
    }

  if (y > -1)
    xv_set (x_panel, XV_Y, y, NULL);

  // If width = -1, will extend to edge of frame
  xv_set (x_panel, XV_WIDTH, width, NULL);

  if (height > -1)
    xv_set (x_panel, XV_HEIGHT, height, NULL);

  xv_set (x_panel, XV_SHOW, TRUE, NULL);

  handle = (char *) x_panel;
  xv_set(x_panel, PANEL_REPAINT_PROC, wxPanelRepaintProc, NULL);
  xv_set(x_panel, XV_KEY_DATA, PANEL_STATIC_ITEM, this, NULL);
  if (parent)
    parent->AddChild (this);
  wx_dc = new wxPanelDC(this); // Not necessary in Motif section, done in wx_canvs.cc
#endif
  return TRUE;
}

wxPanel::~wxPanel (void)
{
  if (panelBackgroundBrush)
  {
    if (wx_dc)
      wx_dc->SetBackground(NULL);
    delete panelBackgroundBrush;
  }
#ifdef wx_motif
  DestroyChildren ();
#endif
#ifdef wx_xview
  if (handle)
    xv_set((Panel)handle, PANEL_REPAINT_PROC, NULL, NULL);
#endif
}

void wxPanel:: SetSize (int x, int y, int w, int h, int sizeFlags)
{
#ifdef wx_motif
  wxCanvas::SetSize(x, y, w, h, sizeFlags);
#endif
#ifdef wx_xview
  if (x == -1 || y == -1)
    {
      int xx, yy;
      GetPosition (&xx, &yy);
      if (x == -1)
	x = xx;
      if (y == -1)
	y = yy;
    }

  if (w == -1 || h == -1)
    {
      int ww, hh;
      GetSize (&ww, &hh);
      if (w == -1)
	w = ww;
      if (h == -1)
	h = hh;
    }

  wxWindow *father = GetParent();
  while (!father->IsKindOf(CLASSINFO(wxFrame)))
    {
      int xx, yy;
      father->GetPosition(&xx, &yy);
      x += xx;
      y += yy;
      father = (wxPanel *) (father->GetParent ());
    }
  int y_offset = ((wxFrame *)father)->y_offset;
  if ((y > -1) && (GetParent() == father)) // If direct child of frame, add offset,
                                      // else will have been dealt with already
    y += y_offset;	// Allow for possible menu bar

//  if (window_parent && window_parent->IsKindOf(CLASSINFO(wxFrame)))
//    real_y += ((wxFrame *) window_parent)->y_offset;	// Allow for possible menu bar

  Xv_opaque object = (Xv_opaque) handle;

  (void) xv_set (object, XV_X, x, XV_Y, y, NULL);

  (void) xv_set (object, XV_WIDTH, w, XV_HEIGHT, h, NULL);
#endif
  GetEventHandler()->OnSize (w, h);
}

void wxPanel:: SetClientSize (int w, int h)
{
#ifdef wx_motif
  SetSize (-1, -1, w, h);
#endif
#ifdef wx_xview
  SetSize (-1, -1, w, h);
#endif
}

void wxPanel:: GetPosition (int *x, int *y)
{
#ifdef wx_motif
 wxCanvas::GetPosition(x, y);
#endif
#ifdef wx_xview
  if (!GetParent() || GetParent()->IsKindOf(CLASSINFO(wxFrame)))
  {
    wxWindow::GetPosition (x, y);
    if (GetParent())
      *y = *y - ((wxFrame *) GetParent())->y_offset;	// Allow for menu bar
  }
  else
  {
    wxWindow *parent = GetParent();
    int parentX = (int)xv_get((Xv_opaque)parent->handle, XV_X);
    int parentY = (int)xv_get((Xv_opaque)parent->handle, XV_Y);
    int thisX = (int)xv_get((Xv_opaque)handle, XV_X);
    int thisY = (int)xv_get((Xv_opaque)handle, XV_Y);
    *x = thisX - parentX;
    *y = thisY - parentY;
  }
#endif
}

/*****************************************************************
 * ITEM PLACEMENT FUNCTIONS
 *****************************************************************/


// Start a new line
#ifdef wx_motif
void wxPanel:: RealNewLine (void)
{
  //cursor_x = PANEL_LEFT_MARGIN;
  cursor_x = initial_hspacing;
  if (max_line_height == 0)
    {
      cursor_y += current_vspacing;
    }
  else
    cursor_y += current_vspacing + max_line_height;
  max_line_height = 0;
  new_line = FALSE;
}
#endif

void wxPanel:: NewLine (void)
{
#ifdef wx_motif
  if (new_line)
    current_vspacing += vSpacing;
  else
    current_vspacing = vSpacing;
  new_line = TRUE;
#endif
#ifdef wx_xview
  Panel panel = (Panel) handle;
  if (new_line)
    {
      (void) xv_create (panel, PANEL_MESSAGE,
			PANEL_LAYOUT, PANEL_HORIZONTAL,
			PANEL_NEXT_ROW, vSpacing, NULL);
    }
  new_line = TRUE;
#endif
}

void wxPanel:: NewLine (int pixels)
{
#ifdef wx_motif
  if (new_line)
    current_vspacing += pixels;
  else
    current_vspacing = pixels;
  new_line = TRUE;
#endif
#ifdef wx_xview
  Panel panel = (Panel) handle;
  if (new_line)
    {
      (void) xv_create (panel, PANEL_MESSAGE,
			PANEL_LAYOUT, PANEL_HORIZONTAL,
			PANEL_NEXT_ROW, pixels, NULL);
    }
  new_line = TRUE;
#endif
}

void wxPanel:: Tab (void)
{
#ifdef wx_motif
  current_hspacing += hSpacing;
#endif
#ifdef wx_xview
  current_hspacing += hSpacing;
  Panel panel = (Panel) handle;
  (void) xv_set (panel, PANEL_ITEM_X_GAP, current_hspacing, NULL);
#endif
}

void wxPanel:: Tab (int pixels)
{
#ifdef wx_motif
  current_hspacing += pixels;
#endif
#ifdef wx_xview
  current_hspacing += pixels;
  Panel panel = (Panel) handle;
  (void) xv_set (panel, PANEL_ITEM_X_GAP, current_hspacing, NULL);
#endif
}

void wxPanel:: GetCursor (int *x, int *y)
{
#ifdef wx_motif
  *x = cursor_x;
  *y = cursor_y;
#endif
#ifdef wx_xview
  Xv_opaque object = (Xv_opaque) handle;
  *x = (int) xv_get (object, PANEL_ITEM_X);
  *y = (int) xv_get (object, PANEL_ITEM_Y);
#endif
}

// Set/get horizontal spacing
void wxPanel:: SetHorizontalSpacing (int sp)
{
#ifdef wx_motif
  hSpacing = sp;
  current_hspacing = sp;
/*
   XtVaSetValues((Widget)handle,
   XmNmarginWidth, sp,
   NULL);
 */
#endif
#ifdef wx_xview
  Xv_opaque object = (Xv_opaque) handle;
  (void) xv_set (object, PANEL_ITEM_X_GAP, sp, NULL);
#endif
}

int wxPanel:: GetHorizontalSpacing (void)
{
#ifdef wx_motif
  return hSpacing;
#endif
#ifdef wx_xview
  Xv_opaque object = (Xv_opaque) handle;
  return (int) xv_get (object, PANEL_ITEM_X_GAP);
#endif
}

// Set/get vertical spacing
void wxPanel:: SetVerticalSpacing (int sp)
{
#ifdef wx_motif
  vSpacing = sp;
  current_vspacing = sp;
/*
   XtVaSetValues((Widget)handle,
   XmNmarginHeight, sp,
   NULL);
 */
#endif
#ifdef wx_xview
  Xv_opaque object = (Xv_opaque) handle;
  (void) xv_set (object, PANEL_ITEM_Y_GAP, sp, NULL);
#endif
}

int wxPanel:: GetVerticalSpacing (void)
{
#ifdef wx_motif
  return vSpacing;
#endif
#ifdef wx_xview
  Xv_opaque object = (Xv_opaque) handle;
  return (int) xv_get (object, PANEL_ITEM_Y_GAP);
#endif
}

// Fits the panel around the items
void wxPanel:: Fit (void)
{
#ifdef wx_motif
  RealAdvanceCursor ();
  SetClientSize (max_width + initial_hspacing, max_height + initial_vspacing);
#endif
#ifdef wx_xview
//  Xv_opaque object = (Xv_opaque)handle;
  // Pre-fit
  //  window_fit(object); // Doesn't seem to work always
  int maxX = 0;
  int maxY = 0;
  for(wxNode *node = GetChildren ()->First (); node; node = node->Next())
    {
      wxWindow *win = (wxWindow *) node->Data ();
      if ((win->IsKindOf(CLASSINFO(wxPanel)) &&
	   !win->IsKindOf(CLASSINFO(wxDialogBox))) ||
	  win->IsKindOf(CLASSINFO(wxTextWindow)) ||
	  win->IsKindOf(CLASSINFO(wxItem)) ||
	  win->IsKindOf(CLASSINFO(wxCanvas)))
	{
	  int x, y, w, h;
	  win->GetPosition (&x, &y);
	  win->GetSize (&w, &h);
	  if ((x + w) > maxX)
	    maxX = x + w;
	  if ((y + h) > maxY)
	    maxY = y + h;
	}
    }

/*
 * JACS - don't think this is strictly necessary
  // Add subpanels
  wxNode *child = children->First ();
//  int ww,hh ;
  //  GetSize(&ww,&hh) ;
  while (child)
    {
      wxPanel *panel = (wxPanel *) child->Data ();
      child = child->Next ();
      if (!panel->IsKindOf(CLASSINFO(wxPanel)))
	continue;
      panel->Fit ();
      int cw, ch;
      cw += panel->x_create;
      ch += panel->y_create;

      if (cw > maxX)
	maxX = cw;
      if (ch > maxY)
	maxY = ch;
    }
*/
  SetClientSize (maxX, maxY);
#endif
}

#ifdef wx_motif
// Update next cursor position
void wxPanel:: RealAdvanceCursor (void)
{
  wxWindow *item = last_created;
  if (item)
    {
      int width, height;
      int x, y;
      item->GetSize (&width, &height);
      item->GetPosition (&x, &y);

      if ((x + width) > max_width)
	max_width = x + width;
      if ((y + height) > max_height)
	max_height = y + height;
      if (height > max_line_height)
	max_line_height = height;
/*
   fprintf(stderr,"maxwidth %d maxheight %d maklineheight %d\n",
   max_width,max_height,max_line_height);
   fprintf(stderr,"Newline %d\n",new_line) ;
 */
      cursor_x = x + width + current_hspacing;
      cursor_y = y;
      last_created = NULL;
    }
  // Don't give a newline if we're starting the top row
  if (new_line)
    RealNewLine ();
}
#endif

// Update next cursor position
void wxPanel:: AdvanceCursor (wxWindow * item)
{
#ifdef wx_motif
  // For calculating positions on a bulletin board.
  /*
     int width, height;
     int x, y;
     item->GetSize(&width, &height);
     item->GetPosition(&x, &y);

     if ((x + width) > max_width)
     max_width = x + width;
     if ((y + height) > max_height)
     max_height = y + height;
     if (height > max_line_height)
     max_line_height = height;

     cursor_x = x + width + hSpacing;
     cursor_y = y;
   */
  last_created = item;
#endif
#ifdef wx_xview
#endif
}

// If x or y are not specified (i.e. < 0), supply
// values based on left to right, top to bottom layout.
// Internal use only.
void wxPanel:: GetValidPosition (int *x, int *y)
{
#ifdef wx_motif
  // For calculating positions on a bulletin board.
  if (*x < 0)
    *x = cursor_x;

  if (*y < 0)
    *y = cursor_y;
#endif
#ifdef wx_xview
  Xv_opaque object = (Xv_opaque) handle;
  if (*x < 0)
    *x = (int) xv_get (object, PANEL_ITEM_X);
  if (*y < 0)
    *y = (int) xv_get (object, PANEL_ITEM_Y);
#endif
}

#ifdef wx_xview
void wxPanel:: DragAcceptFiles (Bool accept)
{

/*
   if (accept)
   {
   if (dropSite)
   xv_destroy(dropSite);

   Xv_Window pw = xv_get((Panel)handle, PANEL_FIRST_PAINT_WINDOW, NULL);
   dropSite = xv_create(pw, DROP_SITE_ITEM,
   DROP_SITE_ID, NewId(),
   DROP_SITE_REGION, xv_get(pw, WIN_RECT),
   DROP_SITE_EVENT_MASK, DND_ENTERLEAVE,
   NULL);
   }
   else if (dropSite) xv_destroy(dropSite);
 */
}
#endif


#ifdef wx_motif
void wxPanel::
AttachWidget (wxPanel * panel, Widget formWidget,
	      int x, int y, int width, int height)
{

//fprintf(stderr,"Subpanel %x Panel %x\n",this,panel) ;
  //fprintf(stderr,"Panel: ih %d iv %d ch %d cv %d hs %d vs %d\n",
  //      panel->initial_hspacing,panel->initial_vspacing,
  //      panel->current_hspacing,panel->current_vspacing,
  //      panel->hSpacing,panel->vSpacing) ;

  if ((x > -1) || (y > -1))
    panel->allRelative = FALSE;

  //itemOrientation = panel->label_position;

//  wxAddWindowToTable(formWidget, this);

  XtTranslations ptr;
  XtOverrideTranslations (formWidget,
		   ptr = XtParseTranslationTable ("<Configure>: resize()"));
  XtFree ((char *) ptr);

  SetSize (x, y, width, height);
}
#endif

void wxPanel:: Centre (int direction)
{
  int x, y, width, height, panel_width, panel_height, new_x, new_y;

  wxWindow *father = GetParent ();
  if (!father)
    return;

  father->GetClientSize (&panel_width, &panel_height);
  GetSize (&width, &height);
  GetPosition (&x, &y);

  new_x = -1;
  new_y = -1;

  if (direction & wxHORIZONTAL)
    new_x = (int) ((panel_width - width) / 2);

  if (direction & wxVERTICAL)
    new_y = (int) ((panel_height - height) / 2);

  SetSize (new_x, new_y, -1, -1);

}

void wxPanel:: Enable (Bool Flag)
{
#ifdef wx_motif
  if (panelWidget)
    XtSetSensitive (panelWidget, Flag);
#endif
}

void wxPanel:: AddChild (wxObject * child)
{

  if (!has_child)
    {
      initial_hspacing = hSpacing;
      initial_vspacing = vSpacing;
#ifdef wx_motif
/*
   if (borderWidget)
   {
   XtVaSetValues(borderWidget,
   XmNmarginHeight,initial_vspacing,
   XmNmarginWidth,initial_hspacing,
   NULL) ;
   initial_hspacing = 0 ;
   initial_vspacing = 0 ;
   }
 */
#endif
    }
  has_child = TRUE;

#ifdef wx_motif
  // Wolrfram Gloger Jan 1996
//  cursor_x = hSpacing;
//  cursor_y = vSpacing;
  RealAdvanceCursor ();
#endif

#ifdef wx_xview
  (void) xv_set ((Xv_opaque)handle,
                 PANEL_ITEM_X_GAP, current_hspacing, 
                 PANEL_ITEM_Y_GAP, current_vspacing, 
                 NULL);
#endif

  current_hspacing = hSpacing;
  current_vspacing = vSpacing;

  children->Append (child);
}

void wxPanel:: ChangeColour (void)
{
#ifdef wx_motif
  if (!(GetWindowStyleFlag() & wxUSER_COLOURS))
    return;

  int change = wxComputeColors (XtDisplay(panelWidget), GetBackgroundColour(), GetButtonColour());

  if (change == wxBACK_COLORS)
    {
      if (borderWidget)
	XtVaSetValues (borderWidget,
		       XmNbackground, itemColors[wxBACK_INDEX].pixel,
		       XmNtopShadowColor, itemColors[wxTOPS_INDEX].pixel,
		       XmNbottomShadowColor, itemColors[wxBOTS_INDEX].pixel,
		       XmNforeground, itemColors[wxFORE_INDEX].pixel,
		       XmNborderColor, itemColors[wxFORE_INDEX].pixel,
		       NULL);
      XtVaSetValues (panelWidget,
		     XmNbackground, itemColors[wxBACK_INDEX].pixel,
		     XmNtopShadowColor, itemColors[wxTOPS_INDEX].pixel,
		     XmNbottomShadowColor, itemColors[wxBOTS_INDEX].pixel,
		     XmNforeground, itemColors[wxFORE_INDEX].pixel,
		     XmNborderColor, itemColors[wxFORE_INDEX].pixel,
		     NULL);
    }
  else if (change == wxFORE_COLORS)
    {
      if (borderWidget)
	XtVaSetValues (borderWidget,
		       XmNforeground, itemColors[wxFORE_INDEX].pixel,
		       XmNborderColor, itemColors[wxFORE_INDEX].pixel,
		       NULL);
      XtVaSetValues (panelWidget,
		     XmNforeground, itemColors[wxFORE_INDEX].pixel,
		     XmNborderColor, itemColors[wxFORE_INDEX].pixel,
		     NULL);
    }

#endif
}


/*
 * Attach new widget to last widget, or below previous widget of
 * previous row if we've had a new line
 */

#ifdef wx_motif
void wxPanel::AttachWidget (wxWindow *item, Widget formWidget,
	      int x, int y, int width, int height)
{
  if ((x > -1) || (y > -1))
    allRelative = FALSE;

  if (item->IsKindOf(CLASSINFO(wxItem)))
  {
    wxItem *panelItem = (wxItem *)item;

    if (panelItem->CanAddEventHandler())
    {
      XtAddEventHandler((Widget)item->handle,
        ButtonPressMask | ButtonReleaseMask | PointerMotionMask, // | KeyPressMask,
        False,
        wxPanelItemEventHandler,
        (XtPointer)panelItem);
#if USE_GADGETS
      if (panelItem->labelWidget && (panelItem->labelWidget != (Widget)item->handle))
      {
          wxAddWindowToTable(panelItem->labelWidget, panelItem);
          XtAddEventHandler((Widget)panelItem->labelWidget,
            ButtonPressMask | ButtonReleaseMask | PointerMotionMask, // | KeyPressMask,
            False,
            wxPanelItemEventHandler,
            (XtPointer)panelItem);
      }
#endif
    }
  }

  if (formWidget)
  {
    if (!wxAddWindowToTable(formWidget, item))
      return;

    XtTranslations ptr;
    XtOverrideTranslations (formWidget,
  		   ptr = XtParseTranslationTable ("<Configure>: resize()"));
    XtFree ((char *) ptr);
  }
  
  GetValidPosition (&x, &y);
  item->SetSize (x, y, width, height);
  AdvanceCursor (item);

  current_hspacing = hSpacing;
  current_vspacing = vSpacing;
}

void wxPanel::OptimizeLayout (void)
{
}
#endif

Window wxPanel::GetXWindow(void)
{
#ifdef wx_motif
  return (Window)XtWindow((Widget)handle);
#endif
#ifdef wx_xview
  return wxWindow::GetXWindow();
#endif
}

#if USE_EXTENDED_STATICS

#define LOH_MAX(a,b) ((a > b) ? a : b)
#define LOH_MIN(a,b) ((a < b) ? a : b)

Bool IsBoxCovered(XRectangle *r_list,int count,XRectangle rect)
   {
     if (count == 0)
       return TRUE;
       
     int llp, x_min, x_max, y_min, y_max;

     int rx1 = rect.x;
     int rx2 = rx1 + rect.width;
     int ry1 = rect.y;
     int ry2 = ry1 + rect.height;

     int cx1,cx2,cy1,cy2;

     for(llp = 0;llp < count;llp++)
        {
          cx1 = r_list[llp].x;
          cx2 = cx1 + r_list[llp].width;
          cy1 = r_list[llp].y;
          cy2 = cy1 + r_list[llp].height;
          
          x_min = LOH_MIN(rx1,cx1);
          x_max = LOH_MAX(rx2,cx2);
          y_min = LOH_MIN(ry1,cy1);
          y_max = LOH_MAX(ry2,cy2);

          if (((x_max - x_min) <= (rect.width + r_list[llp].width)) &&
              ((y_max - y_min) <= (rect.height + r_list[llp].height)))
             return TRUE;

        }
     return FALSE;
   }

void wxPanel::DrawAllStaticItems(XRectangle *rect,int count)
{
  XRectangle xr;
  int x,y,w,h;
  if (staticItems.Number() > 0)
  {
    wxNode *node = staticItems.First();
    wxStaticItem *item;
    while(node)
    {
      item = (wxStaticItem *)node -> Data();
      item -> GetDrawingSize(&x,&y,&w,&h);
      xr.x = x;
      xr.y = y;
      xr.width = w; 
      xr.height = h;
      if (IsBoxCovered(rect,count,xr) && item -> IsShow()) 
      {
        item -> Draw(rect,count);
      }
      node = node -> Next();
    }
  }
}
#endif

#ifdef wx_xview
void wxPanelRepaintProc(Panel x_panel, Xv_Window pw, Rectlist *p_area)
{
  wxPanel *panel = (wxPanel *) xv_get(x_panel, XV_KEY_DATA,
                                         PANEL_STATIC_ITEM);

  if (panel && panel->GetDC())
  {
    Display *display = panel -> GetXDisplay();
    int count = 0;
    Rectnode *node = p_area -> rl_head;
    while (node)
    {
      count ++;
      node = node->rn_next;
    }
    XRectangle *xrect = NULL;
    if (count > 0)
    {
      xrect = new XRectangle[count];
      node = p_area -> rl_head;
      int i = 0;
      while (node)
      {
        xrect[i].x = node->rn_rect.r_left;
        xrect[i].y = node->rn_rect.r_top;
        xrect[i].width = node->rn_rect.r_width;
        xrect[i].height = node->rn_rect.r_height;
        node = node -> rn_next;
        i ++;
      }
    }

    XSetClipRectangles(display, panel -> GetDC() -> gc, p_area -> rl_x,
                             p_area -> rl_y, xrect, count, Unsorted);
    panel->DoPaint(xrect,count);

    if (xrect)
      delete xrect;

    // An attempt to reset the clipping region back to nothing
    // in case this is what is causing Clear() not to work.
    XGCValues gc_val;
    gc_val.clip_mask = None;
    XChangeGC(display, panel -> GetDC() -> gc, GCClipMask, &gc_val);
  }
}

#endif

void wxPanel::DoPaint(XRectangle *xrect, int n)
{
  updateRects.Clear();
  int i;
  for (i = 0; i < n; i++)
  {
    wxRectangle *rect = new wxRectangle;
    rect->x = xrect[i].x;
    rect->y = xrect[i].y;
    rect->width = xrect[i].width;
    rect->height = xrect[i].height;
    updateRects.Append(rect);
  }
#ifdef wx_motif
  DoRefresh();
#endif
#ifdef wx_xview
  GetEventHandler()->OnPaint();
#endif
  updateRects.Clear();
}

void wxPanel::OnPaint(void)
{
#if USE_EXTENDED_STATICS
  DrawAllStaticItems(NULL, 0);
#endif
  PaintSelectionHandles();
}

#ifdef wx_xview
void wxPanelInterposeProc(Panel x_panel, Event *event, Notify_arg arg, Notify_event_type type)
{
  wxPanel *panel = (wxPanel *)xv_get(x_panel, WIN_CLIENT_DATA);

  if (!panel)
  {
    notify_next_event_func(x_panel, (Notify_event)event, arg, type);
    return;
  }
  
  // Check for keypress and implement wxPanel:::OnChar, wxFrame::OnCharHook
  if (event && event->ie_xevent->xany.type == KeyPress)
  {
    KeySym keySym;
//    XComposeStatus compose;
//    (void) XLookupString ((XKeyEvent *) event->ie_xevent, wxBuffer, 20, &keySym, &compose);
    (void) XLookupString ((XKeyEvent *) event->ie_xevent, wxBuffer, 20, &keySym, NULL);
    int id = CharCodeXToWX (keySym);

    wxKeyEvent wxevent (wxEVENT_TYPE_CHAR);
    if (event_shift_is_down (event))
      wxevent.shiftDown = TRUE;
    if (event_ctrl_is_down (event))
      wxevent.controlDown = TRUE;
    if (event_meta_is_down (event))
      wxevent.metaDown = TRUE;
    if (event_alt_is_down (event))
      wxevent.altDown = TRUE;
    wxevent.keyCode = id;
    wxevent.eventObject = panel;
    wxevent.SetTimestamp(1000 * event_time(event).tv_sec 
                          + event_time(event).tv_usec / 1000);

    wxevent.x = event_x (event);
    wxevent.y = event_y (event);

    if ((id > -1) && (id != WXK_SHIFT) && (id != WXK_CONTROL))
    {
      // Implement wxDialogBox/wxFrame::OnCharHook by checking ancestor.
      wxWindow *parent = NULL;
      if (panel->IsKindOf(CLASSINFO(wxDialogBox)))
        parent = panel;
      else
      {
        parent = panel->GetParent();
        while (parent && !parent->IsKindOf(CLASSINFO(wxFrame)))
          parent = parent->GetParent();
      }
            
      if (parent)
      {
        if (parent->GetEventHandler()->OnCharHook(wxevent))
          return;
      }
      panel->GetEventHandler()->OnChar(wxevent);
    }    
  }

  // Why call panel/item OnEvent only in edit mode? Why not always get events,
  // in addition to those processed by panel items?
  // Now, first send OnEvent to the relevant item or the wxPanel,
  // and then only let default processing happen if we're NOT in user edit mode.
//  if (panel->GetUserEditMode())

  if (event)
  {
    int eventX = event->ie_locx;
    int eventY = event->ie_locy;

    wxItem *wxitem = NULL;
    Panel_item item;
    Rect *rect;
    PANEL_EACH_ITEM(x_panel, item);
      rect = (Rect *)xv_get(item, XV_RECT);
      if (rect_includespoint(rect, event->ie_locx, event->ie_locy))
      {
        eventX = eventX - rect->r_left;
        eventY = eventY - rect->r_top;
        wxitem = (wxItem *)xv_get(item, PANEL_CLIENT_DATA);
      }
    PANEL_END_EACH

/*
    if (wxitem)
      cout << "Item event\n";
    else
      cout << "Panel event\n";
*/

    if (event_id(event) == LOC_DRAG || event_id(event) == LOC_MOVE ||
        event_is_button(event))
    {
	  WXTYPE eventType = 0;

	  if ((event_id (event) == LOC_DRAG) || (event_id (event) == LOC_MOVE))
	    eventType = wxEVENT_TYPE_MOTION;
	  else if (event_is_button (event))
	    {
	      if (event_is_down (event))
		{
		  if (event_id (event) == BUT (1))
		    eventType = wxEVENT_TYPE_LEFT_DOWN;
		  else if (event_id (event) == BUT (2))
		    eventType = wxEVENT_TYPE_MIDDLE_DOWN;
		  else if (event_id (event) == BUT (3))
		    eventType = wxEVENT_TYPE_RIGHT_DOWN;
		}
	      else if (event_is_up (event))
		{
		  if (event_id (event) == BUT (1))
		    eventType = wxEVENT_TYPE_LEFT_UP;
		  else if (event_id (event) == BUT (2))
		    eventType = wxEVENT_TYPE_MIDDLE_UP;
		  else if (event_id (event) == BUT (3))
		    eventType = wxEVENT_TYPE_RIGHT_UP;
		}
	    }
	  wxMouseEvent mouseEvent (eventType);

	  if (wxitem)
 	    mouseEvent.eventObject = wxitem;
          else
            mouseEvent.eventObject = panel;
            
	  mouseEvent.controlDown = event_ctrl_is_down (event);
	  mouseEvent.shiftDown = event_shift_is_down (event);
	  mouseEvent.altDown = event_alt_is_down (event);
	  mouseEvent.metaDown = event_meta_is_down (event);
	  mouseEvent.leftDown = event_left_is_down (event);
	  mouseEvent.middleDown = event_middle_is_down (event);
	  mouseEvent.rightDown = event_right_is_down (event);
	  mouseEvent.eventHandle = (char *) event;

	  mouseEvent.x = eventX;
	  mouseEvent.y = eventY;

          if (wxitem)
            wxitem->GetEventHandler()->OnEvent(mouseEvent);
          else
            panel->GetEventHandler()->OnEvent(mouseEvent);
            
          if (panel->GetUserEditMode())
            return;
          else
            notify_next_event_func(x_panel, (Notify_event)event, arg, type);
    }
    else   
      notify_next_event_func(x_panel, (Notify_event)event, arg, type);
  }
}
#endif

#ifdef wx_motif
// Override edit mode so we can remove translations etc.
void wxPanel::SetUserEditMode(Bool edit)
{
  editUIMode = edit;
  wxNode *node = GetChildren()->First();
  while (node)
  {
    wxWindow *win = (wxWindow *)node->Data();
    if (win->IsKindOf(CLASSINFO(wxItem)))
    {
      wxItem *item = (wxItem *)win;
      item->RemoveTranslations(edit);
    }
    node = node->Next();
  }
}
#endif
