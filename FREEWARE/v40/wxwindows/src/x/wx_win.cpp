/*
 * File:	wx_win.cc
 * Purpose:	wxWindow class implementation (X version)
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_win.cc,v 1.5 1994/08/14 21:28:43 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

static const char sccsid[] = "@(#)wx_win.cc	1.2 5/9/94";

#ifdef __GNUG__
#pragma implementation
#endif

#include <X11/keysym.h>

#include "wx_defs.h"
#include "wx_win.h"
#include "wx_main.h"
#include "wx_utils.h"
#include "wx_privt.h"
#include "wx_gdi.h"
#include "wx_canvs.h"
#include "wx_menu.h"
#include "wx_panel.h"
#include "wx_dialg.h"
#include "wx_text.h"
#include "wx_cmdlg.h"

#ifdef wx_motif

#define event_left_is_down(x) (x->xbutton.state & Button1Mask)
#define event_middle_is_down(x) (x->xbutton.state & Button2Mask)
#define event_right_is_down(x) (x->xbutton.state & Button3Mask)

#include "wx_dialg.h"
#include <Xm/RowColumn.h>
#endif

#ifdef wx_xview
#include <xview/wmgr.h>
#include <xview/svrimage.h>
extern Xv_Server xview_server;
// Intercept focus events
extern "C" int win_set_kbd_focus(Xv_Window, XID);
extern void wxMenuProc (Menu x_menu, Menu_item menu_item);
#endif

// Constructor
wxWindow::wxWindow(void)
{
  currentWindowCursor = 0;
#ifdef wx_motif
  wxType = 0;
  button1Pressed = FALSE;
  button2Pressed = FALSE;
  button3Pressed = FALSE;
#endif
#ifdef wx_xview
  dropSite = 0;
#endif
}

// Destructor
wxWindow::~wxWindow(void)
{
  if (window_parent)
  {
    window_parent->RemoveChild(this);

    if (window_parent->IsKindOf(CLASSINFO(wxPanel)))
    {
      wxPanel *panel = (wxPanel *) GetParent ();
      // Must reset the panel since we may have dangling pointers, etc.

      panel->new_line = FALSE;
      panel->label_position = wxHORIZONTAL;

      panel->hSpacing = PANEL_HSPACING;
      panel->vSpacing = PANEL_VSPACING;
      panel->initial_hspacing = panel->hSpacing;
      panel->initial_vspacing = panel->vSpacing;
      panel->current_hspacing = panel->hSpacing;
      panel->current_vspacing = panel->vSpacing;
#ifdef wx_motif
      panel->cursor_x = PANEL_LEFT_MARGIN;
      panel->cursor_y = PANEL_TOP_MARGIN;
      panel->max_height = 0;
      panel->max_line_height = 0;
      panel->max_width = 0;
      panel->firstRowWidget = 0;
      panel->currentRow = 0;
      panel->currentCol = 0;
      panel->last_created = 0;
#endif
    }
  }
#ifdef wx_motif
  if (handle)
  {
    DestroyChildren();

    wxDeleteWindowFromTable((Widget)handle);
    Widget w = (Widget)handle;
    if (w)
      XtDestroyWidget(w);

    PostDestroyChildren();

  }
#endif
#ifdef wx_xview
  if (handle)
  {
    // Reset client data
    xv_set((Xv_opaque)handle, WIN_CLIENT_DATA, NULL, NULL);
    DestroyChildren();
    xv_destroy_safe((Xv_opaque)handle);
  }
#endif
  delete children;
  children = NULL;
}

void wxWindow::SetFocus(void)
{
#ifdef wx_motif
  XmProcessTraversal((Widget)handle, XmTRAVERSE_CURRENT);
  XmProcessTraversal((Widget)handle, XmTRAVERSE_CURRENT);
#endif
#ifdef wx_xview
  Xv_opaque win = (Xv_opaque)handle;

  if (this->IsKindOf(CLASSINFO(wxCanvas)))
    win = canvas_paint_window(win);
    
  Xv_opaque id = xv_get(win, XV_XID);
  if (id)
    win_set_kbd_focus(win, id);
#endif
}

void wxWindow::CaptureMouse(void)
{
  if (winCaptured)
    return;
    
#ifdef wx_motif
  if (handle)
    XtAddGrab((Widget)handle, TRUE, FALSE);
#endif
#ifdef wx_xview
  if (handle)
  {
    Xv_opaque win = (Xv_opaque)handle;
    xv_set(win, WIN_GRAB_ALL_INPUT, TRUE, NULL);
  }
#endif
  winCaptured = TRUE;
}

void wxWindow::ReleaseMouse(void)
{
  if (!winCaptured)
    return;
    
#ifdef wx_motif
  if (handle)
    XtRemoveGrab((Widget)handle);
#endif
#ifdef wx_xview
  if (handle)
  {
    Xv_opaque win = (Xv_opaque)handle;
    xv_set(win, WIN_GRAB_ALL_INPUT, FALSE, NULL);
  }
#endif
  winCaptured = FALSE;
}

void wxWindow::Enable(Bool enable)
{
#ifdef wx_motif
  if (handle)
  {
    XtSetSensitive((Widget)handle, enable);
    XmUpdateDisplay((Widget)handle);
  }
#endif
#ifdef wx_xview
#endif
}

Window wxWindow::GetXWindow(void)
{
#ifdef wx_motif
  return XtWindow((Widget)handle);
#endif
#ifdef wx_xview
  Xv_opaque x_win = (Xv_opaque)handle;
  Window thisWindow = 0;
  if (this->IsKindOf(CLASSINFO(wxCanvas)))
  {
    Xv_Window win = xv_get(x_win, CANVAS_NTH_PAINT_WINDOW, 0);
    thisWindow = xv_get(win, XV_XID);
  }
  else
  {
    thisWindow = xv_get(x_win, XV_XID);
  }
  return thisWindow;
#endif
}

Display *wxWindow::GetXDisplay(void)
{
#ifdef wx_motif
  return XtDisplay((Widget)handle);
#endif
#ifdef wx_xview
  return wxGetDisplay();
#endif
}

void wxWindow::Refresh(Bool eraseBack, wxRectangle *rect)
{
#ifdef wx_motif
  Display *display = XtDisplay((Widget)handle);
  Window thisWindow = XtWindow((Widget)handle);
#endif
#ifdef wx_xview
  Xv_opaque x_win = (Xv_opaque)handle;
  wmgr_refreshwindow(x_win); // Refresh Window manager

  Window thisWindow = GetXWindow();
  Display *display = GetXDisplay(); // Get display for the window
/*
  Xv_opaque x_win = (Xv_opaque)handle;
  wmgr_refreshwindow(x_win); // Refresh Window manager

  Xv_Window win = 0;
  Display *display = GetXDisplay(); // Get display for the window
  Window thisWindow = 0;
  if (this->IsKindOf(CLASSINFO(wxCanvas)) && !this->IsKindOf(CLASSINFO(wxPanel)))
  {
    win = xv_get(x_win, CANVAS_NTH_PAINT_WINDOW, 0);
    thisWindow = xv_get(win, XV_XID);
  }
  else
  {
    thisWindow = xv_get(x_win, XV_XID);
  }
*/
#endif
  XExposeEvent dummyEvent;
  int width, height;
  GetSize(&width, &height);

  dummyEvent.type = Expose;
  dummyEvent.display = display;
  dummyEvent.send_event = True;
  dummyEvent.window = thisWindow;
  if (rect)
  {
    dummyEvent.x = rect->x;
    dummyEvent.y = rect->y;
    dummyEvent.width = rect->width;
    dummyEvent.height = rect->height;
  }
  else
  {
    dummyEvent.x = 0;
    dummyEvent.y = 0;
    dummyEvent.width = width;
    dummyEvent.height = height;
  }
  dummyEvent.count = 0;

 /* 1996 12 03
     Bart JOURQUIN jourquin@message.fucam.ac.be
     The following Xview conditionnal compilation is introduce in order
     to obtain consistant UpdateRectangle coordinates for Xview.
     Remark: this doesn't solve the inconsistancy that exists between
     UpdateRecangle coordinates for Windows/Motif and XView. */

#ifdef wx_xview
  wxCanvas *cnvas = (wxCanvas*) this;
  if (cnvas->GetDC())
      {
	int UpperLeftX, UpperLeftY;
	cnvas->ViewStart(&UpperLeftX, &UpperLeftY);
	int x_units, y_units;
	cnvas->GetScrollPixelsPerUnit(&x_units, &y_units);
	dummyEvent.x += UpperLeftX*x_units;
	dummyEvent.y += UpperLeftY*y_units;
      }
	
#endif

  // 1996 12 03 Bart JOURQUIN - END - 
  
  // What about clearing background?
  // Ok, here it is, for wxCanvas derivatives (incl. wxPanel)
  if (eraseBack && this->IsKindOf(CLASSINFO(wxCanvas)))
  {
    wxCanvas *canvas = (wxCanvas *)this;
    if (canvas->GetDC())
     canvas->GetDC()->Clear();
  }

  XSendEvent(display, thisWindow, False, ExposureMask, (XEvent *)&dummyEvent);
}

void wxWindow::DragAcceptFiles(Bool accept)
{
}

// Get total size
void wxWindow::GetSize(int *x, int *y)
{
#ifdef wx_motif
  Widget widget = (Widget)handle;
  Dimension xx, yy;
  XtVaGetValues(widget, XmNwidth, &xx, XmNheight, &yy, NULL);
  *x = xx; *y = yy;
#endif
#ifdef wx_xview
  Xv_opaque x_win = (Xv_opaque)handle;

  *x = (int)xv_get(x_win, XV_WIDTH);
  *y = (int)xv_get(x_win, XV_HEIGHT);
#endif
}

void wxWindow::GetPosition(int *x, int *y)
{
#ifdef wx_motif
  Widget widget = (Widget)handle;
  Position xx, yy;
  XtVaGetValues(widget, XmNx, &xx, XmNy, &yy, NULL);
  *x = xx; *y = yy;
#endif
#ifdef wx_xview
  Xv_opaque x_win = (Xv_opaque)handle;

  *x = (int)xv_get(x_win, XV_X);
  *y = (int)xv_get(x_win, XV_Y);
#endif
}

void wxWindow::ClientToScreen(int *x, int *y)
{
#ifdef wx_motif
  Display *display = XtDisplay((Widget)handle);
  Window rootWindow = RootWindowOfScreen(XtScreen((Widget)handle));
  Window thisWindow;
  if (this->IsKindOf(CLASSINFO(wxFrame)))
  {
    wxFrame *fr = (wxFrame *)this;
    thisWindow = XtWindow(fr->clientArea);
  }
  else
    thisWindow = XtWindow((Widget)handle);
#endif
#ifdef wx_xview
  Xv_opaque x_win = (Xv_opaque)handle;
  Xv_Window win = 0;
  Xv_Screen screen = xv_get(xview_server, SERVER_NTH_SCREEN, 0);
  Xv_Window root_window = xv_get(screen, XV_ROOT);
  Display *display = (Display *)xv_get(root_window, XV_DISPLAY);
  Window rootWindow = xv_get(root_window, XV_XID);
  Window thisWindow = 0;
  if (this->IsKindOf(CLASSINFO(wxCanvas)))
  {
    win = xv_get(x_win, CANVAS_NTH_PAINT_WINDOW, 0);
    thisWindow = xv_get(win, XV_XID);
  }
  else if (this->IsKindOf(CLASSINFO(wxFrame)))
  {
    int xp, yp;
    GetPosition(&xp, &yp);
    *x = *x + xp;
    *y = *y + yp;
    return;
  }
  else
  {
    thisWindow = xv_get(x_win, XV_XID);
  }
#endif
  Window childWindow;
  int xx = *x;
  int yy = *y;
  XTranslateCoordinates(display, thisWindow, rootWindow, xx, yy, x, y, &childWindow);
}

void wxWindow::ScreenToClient(int *x, int *y)
{
#ifdef wx_motif
  Display *display = XtDisplay((Widget)handle);
  Window rootWindow = RootWindowOfScreen(XtScreen((Widget)handle));
  Window thisWindow;
  if (this->IsKindOf(CLASSINFO(wxFrame)))
  {
    wxFrame *fr = (wxFrame *)this;
    thisWindow = XtWindow(fr->clientArea);
  }
  else
    thisWindow = XtWindow((Widget)handle);
#endif
#ifdef wx_xview
  Xv_opaque x_win = (Xv_opaque)handle;
  Xv_Window win = 0;
  Xv_Screen screen = xv_get(xview_server, SERVER_NTH_SCREEN, 0);
  Xv_Window root_window = xv_get(screen, XV_ROOT);
  Display *display = (Display *)xv_get(root_window, XV_DISPLAY);
  Window rootWindow = xv_get(root_window, XV_XID);
  Window thisWindow = 0;

  if (this->IsKindOf(CLASSINFO(wxCanvas)))
  {
    win = xv_get(x_win, CANVAS_NTH_PAINT_WINDOW, 0);
    thisWindow = xv_get(win, XV_XID);
  }
  else if (this->IsKindOf(CLASSINFO(wxFrame)))
  {
    int xp, yp;
    GetPosition(&xp, &yp);
    *x = *x - xp;
    *y = *y - yp;
    return;
  }
  else
  {
    thisWindow = xv_get(x_win, XV_XID);
  }
#endif
  Window childWindow;
  int xx = *x;
  int yy = *y;
  XTranslateCoordinates(display, rootWindow, thisWindow, xx, yy, x, y, &childWindow);
}

wxCursor *wxWindow::SetCursor(wxCursor *cursor)
{
  wxCursor *old_cursor = wx_cursor;
//  wx_cursor = cursor;
#ifdef wx_motif
  /* MATTHEW: [4] Get cursor for this display */
  if (cursor)
  {
    Display *dpy = GetXDisplay();
    Cursor x_cursor = cursor->GetXCursor(dpy);

    Widget w = (Widget)handle;
    Window win = XtWindow(w);
    XDefineCursor(dpy, win, x_cursor);
  }
#endif
#ifdef wx_xview
  if (cursor && cursor->x_cursor)
  {
    if (cursor->use_raw_x_cursor)
    {
      Display *dpy = GetXDisplay();
      Window win2 = GetXWindow();

      XDefineCursor(dpy, win2, cursor->x_cursor);
    }
    else
    {
      if (this->IsKindOf(CLASSINFO(wxCanvas)))
      {
        Xv_opaque x_win = (Xv_opaque)handle;
        Xv_Window win2 = xv_get(x_win, CANVAS_NTH_PAINT_WINDOW, 0);

        xv_set(win2, WIN_CURSOR, cursor->x_cursor, NULL);
      }
      else
        xv_set((Xv_opaque)handle, WIN_CURSOR, cursor->x_cursor, NULL);
    }
  }
#endif
  // Removed, because this causes a lot of unexpected effects
  // if cursor are changed dynamicaly into ::OnMouseEvent() method...
  // [receiving OnMouseEvent() calls before current call is completed!]
  //wxFlushEvents();

  wx_cursor = cursor;
 
  return old_cursor;
}

void wxWindow::SetColourMap(wxColourMap *cmap)
{
  Display *display = GetXDisplay();
  Window win = GetXWindow();
  /* MATTHEW: Use display-specific colormap */
  XSetWindowColormap(display, win, cmap->GetXColormap(display));
}

// Get size *available for subwindows* i.e. excluding menu bar etc.
// For XView, this is the same as GetSize
void wxWindow::GetClientSize(int *x, int *y)
{
#ifdef wx_motif
  Widget widget = (Widget)handle;
  Dimension xx, yy;
  XtVaGetValues(widget, XmNwidth, &xx, XmNheight, &yy, NULL);
  *x = xx; *y = yy;
#endif
#ifdef wx_xview
  Xv_opaque x_win = (Xv_opaque)handle;

  *x = (int)xv_get(x_win, XV_WIDTH);
  *y = (int)xv_get(x_win, XV_HEIGHT);
#endif
}

void wxWindow::SetSize(int x, int y, int width, int height, int sizeFlags)
{
#ifdef wx_motif
  Widget widget = (Widget)handle;

  if (x > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    XtVaSetValues(widget, XmNx, x, NULL);
  if (y > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    XtVaSetValues(widget, XmNy, y, NULL);
  if (width > -1)
    XtVaSetValues(widget, XmNwidth, width, NULL);
  if (height > -1)
    XtVaSetValues(widget, XmNheight, height, NULL);
  GetEventHandler()->OnSize(width, height);
#endif
#ifdef wx_xview
  Xv_opaque x_win = (Xv_opaque)handle;

  if ((x == -1 || y == -1) && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
  {
    int xx,yy ;
    GetPosition(&xx, &yy);
    if (x == -1) x = xx ;
    if (y == -1) y = yy ;
  }

  if (width == -1 || height == -1)
  {
    int ww,hh ;
    GetSize(&ww, &hh);
    if (width == -1) width = ww ;
    if (height == -1) height = hh ;
  }

  // The reason we set these all at once (and not for each dimension
  // ignoring -1's) is that XView requires to do it all in one call, for optimum
  // results.
  (void)xv_set(x_win, XV_X, x, XV_Y, y, XV_WIDTH, width, XV_HEIGHT, height, NULL);

  GetEventHandler()->OnSize(width, height);
#endif
}

void wxWindow::SetClientSize(int width, int height)
{
#ifdef wx_motif
  Widget widget = (Widget)handle;

  if (width > -1)
    XtVaSetValues(widget, XmNwidth, width, NULL);
  if (height > -1)
    XtVaSetValues(widget, XmNheight, height, NULL);
  GetEventHandler()->OnSize(width, height);
#endif
#ifdef wx_xview
  Xv_opaque x_win = (Xv_opaque)handle;

  if (width > -1 && height > -1)
    (void)xv_set(x_win, XV_WIDTH, width, XV_HEIGHT, height, NULL);

  GetEventHandler()->OnSize(width, height);
#endif
}

Bool wxWindow::Show(Bool show)
{
#ifdef wx_motif
  Window xwin=GetXWindow();        //*** all this part
  Display *xdisp=GetXDisplay();    //*** added by
  if(show)                         //*** chubraev
          XMapWindow(xdisp,xwin);  //***
  else                             //***
          XUnmapWindow(xdisp,xwin);//***
#endif
#ifdef wx_xview
  Xv_opaque window = (Xv_opaque)handle;
  xv_set(window, XV_SHOW, show, NULL);
#endif
  return TRUE;
}

Bool wxWindow::IsShown(void)
{
#ifdef wx_motif
  return isShown;
#endif
#ifdef wx_xview
  Xv_opaque window = (Xv_opaque)handle;
  return (Bool)xv_get(window, XV_SHOW);
#endif
  return TRUE;
}

float wxWindow::GetCharHeight(void)
{
#ifdef wx_motif
  return 0.0;
#endif
#ifdef wx_xview
  Xv_Font the_font;

  if (!(font && font->x_font))
  {
    Xv_opaque thing = (Xv_opaque)handle;
    the_font = (Xv_Font)xv_get(thing, XV_FONT);
  }
  else the_font = font->x_font;

  return (float)xv_get(the_font, FONT_DEFAULT_CHAR_HEIGHT);
#endif
}

float wxWindow::GetCharWidth(void)
{
#ifdef wx_motif
  return 0.0;
#endif
#ifdef wx_xview
  Xv_Font the_font;

  if (!(font && font->x_font))
  {
    Xv_opaque thing = (Xv_opaque)handle;
    the_font = (Xv_Font)xv_get(thing, XV_FONT);
  }
  else the_font = font->x_font;

  return (float)xv_get(the_font, FONT_DEFAULT_CHAR_WIDTH);
#endif
}

/* MATTHEW: Helper function for 16-bit fonts */
int str16len(const char *s)
{
  int count = 0;

  while (s[0] && s[1]) {
    count++;
    s += 2;
  }

  return count;
}

void wxWindow::GetTextExtent(const char *string, float *x, float *y,
                              float *descent, float *externalLeading, wxFont *theFont, Bool use16)
{
  wxFont *fontToUse = theFont;

  if (!fontToUse)
    fontToUse = font;

  /* MATTHEW: Use GetInternalFont */
  if (!fontToUse) {
    cerr << "wxWindows warning - set a font before calling GetTextExtent!\n";
    *x = -1;
    *y = -1;
    return;
  }

  XFontStruct *fontStruct;
  (void)fontToUse->GetInternalFont(GetXDisplay(), &fontStruct);

  int direction, ascent, descent2;
  XCharStruct overall;

  /* MATTHEW: Helper 16-bit fonts */
  if (use16)
    XTextExtents16(fontStruct, (XChar2b *)string, str16len(string), 
		   &direction, &ascent, &descent2, &overall);
  else
    XTextExtents(fontStruct, string, strlen(string), &direction, &ascent,
		 &descent2, &overall);
  *x = overall.width;
  *y = ascent + descent2;
  if (descent)
    *descent = (float)descent2;
  if (externalLeading)
    *externalLeading = 0.0;
}

// Taken from 1.66B until we get definitive answer from Nick L.
Bool wxWindow:: PopupMenu (wxMenu * menu, float x, float y)
{
#ifdef wx_motif
  Widget widget = (Widget)handle;

  /* MATTHEW: [11] safety (revised) */
  /* Always popup menus in the frame. Dunno why, but this seems
     to solve the problem described below for FakePopupMenu(). */
  /* HACK: The menuId field seems to be usused, so we'll use it to
     indicate whether a menu is popped up or not:
        0: Not currently created as a popup
       -1: Created as a popup, but not active
        1: Active popup.
   */

  if (menu->GetParent() && (menu->menuId != -1))
    return FALSE;

  if (menu->handle) {
    wxList *list = menu->GetParent()->GetChildren();
    int dd = list->destroy_data;

    list->DeleteContents(FALSE);
    list->DeleteObject(menu);
    list->DeleteContents(dd);

    menu->DestroyMenu(TRUE);
  }

  wxWindow *parent = this;
  while (parent->window_parent)
    parent = parent->window_parent;

  menu->menuId = 1; /* Mark as popped-up */
  menu->CreateMenu(NULL, (Widget)parent->handle, menu);
  menu->SetParent(parent);
  parent->children->Append(menu);  // Store menu for later deletion

  Widget menuWidget = (Widget) menu->handle;

  int rootX = 0;
  int rootY = 0;

  int deviceX = (int) x;
  int deviceY = (int) y;

  if (this->IsKindOf(CLASSINFO(wxCanvas)))
  {
    wxCanvas *canvas = (wxCanvas *) this;
    deviceX = canvas->GetDC ()->LogicalToDeviceX (x);
    deviceY = canvas->GetDC ()->LogicalToDeviceY (y);
  }

  Display *display = XtDisplay ((Widget)parent->handle);
  Window rootWindow = RootWindowOfScreen (XtScreen((Widget)parent->handle));
  Window thisWindow = XtWindow ((Widget)parent->handle);
  Window childWindow;
  XTranslateCoordinates (display, thisWindow, rootWindow, (int) deviceX, (int) deviceY,
			 &rootX, &rootY, &childWindow);

  XButtonPressedEvent event;
  event.type = ButtonPress;
  event.button = 1;

  event.x = (int) deviceX;
  event.y = (int) deviceY;

  event.x_root = rootX;
  event.y_root = rootY;

  XmMenuPosition (menuWidget, &event);
  XtManageChild (menuWidget);

  return TRUE;
#endif
#ifdef wx_xview
  int deviceX = (int) x;
  int deviceY = (int) y;

  if (this->IsKindOf(CLASSINFO(wxCanvas)))
    {
      wxCanvas *canvas = (wxCanvas *) this;
      int vs_x, vs_y;
      canvas->ViewStart (&vs_x, &vs_y);

      // Under XView, LogicalToDevice doesn't deal with scroll position
      deviceX = canvas->GetDC ()->LogicalToDeviceX (x) - vs_x * canvas->horiz_units;
      deviceY = canvas->GetDC ()->LogicalToDeviceY (y) - vs_y * canvas->vert_units;
    }

  xv_set ((Menu) this->handle,
	  MENU_NOTIFY_PROC, wxMenuProc,
	  NULL);

  Event event;
  event.ie_locx = (int) deviceX;
  event.ie_locy = (int) deviceY;
  event.ie_xevent = NULL;
  event.ie_win = (Xv_opaque) handle;
  event.ie_string = NULL;
  event.ie_code = 0;
  event.ie_flags = 0;
  event.ie_shiftmask = 0;
  event.action = 0;

  menu_show ((Menu) menu->handle,
	     (Xv_opaque) handle,
	     &event, NULL);

  return TRUE;
#endif
}

// Old TS fix: replace with PopupMenu from 1.66B until
// we get wx_win.cpp from Nick L. (may be identical to 1.66B code)
#if 0
Bool wxWindow:: PopupMenu (wxMenu * menu, float x, float y)
{
#ifdef wx_motif
   
    // still visible, do nothing
    if (menu->menuId == 1) {
	menu->HidePopup ();
	return TRUE;
    }

    // determine position of popup
    int xr = (int)x, yr = (int)y, dummyInt;
    Window dummyWin;
    XQueryPointer (XtDisplay((Widget)handle), XtWindow((Widget)handle),
		   &dummyWin, &dummyWin, &xr, &yr, &dummyInt,
		   &dummyInt, (unsigned*)&dummyInt);
    
    // hidden, show it
    if (menu->menuId == 2){		
	menu->ShowPopup (xr, yr);
	return TRUE;
    }
    
    // unconstructed, construct it
    if (menu->menuId == 0) {
	menu->handle = handle;
	menu->CreatePopup ((Widget)handle, xr, yr); 
	return TRUE;
    }
    return TRUE;

#endif
#ifdef wx_xview
  int deviceX = (int) x;
  int deviceY = (int) y;

  if (this->IsKindOf(CLASSINFO(wxCanvas)))
    {
      wxCanvas *canvas = (wxCanvas *) this;
      int vs_x, vs_y;
      canvas->ViewStart (&vs_x, &vs_y);

      // Under XView, LogicalToDevice doesn't deal with scroll position
      deviceX = canvas->GetDC ()->LogicalToDeviceX (x) - vs_x * canvas->horiz_units;
      deviceY = canvas->GetDC ()->LogicalToDeviceY (y) - vs_y * canvas->vert_units;
    }

  xv_set ((Menu) this->handle,
	  MENU_NOTIFY_PROC, wxMenuProc,
	  NULL);

  Event event;
  event.ie_locx = (int) deviceX;
  event.ie_locy = (int) deviceY;
  event.ie_xevent = NULL;
  event.ie_win = (Xv_opaque) handle;
  event.ie_string = NULL;
  event.ie_code = 0;
  event.ie_flags = 0;
  event.ie_shiftmask = 0;
  event.action = 0;

  menu_show ((Menu) menu->handle,
	     (Xv_opaque) handle,
	     &event, NULL);

  return TRUE;
#endif
}
#endif

#ifdef wx_motif
// Sometimes in Motif there are problems popping up a menu
// (for unknown reasons); use this instead when this happens.
// Works for one-level popups only (ignores submenus).
Bool wxWindow::FakePopupMenu(wxMenu *menu, float x, float y)
{
  // Find true dialog box parent
  wxWindow *trueParent = NULL;
  if (this->IsKindOf(CLASSINFO(wxCanvas)) ||
      this->IsKindOf(CLASSINFO(wxPanel)) ||
      this->IsKindOf(CLASSINFO(wxTextWindow)))
    trueParent = GetParent();
  else
    trueParent = this;

  // Find true screen x and y position
  int trueX = (int)x;
  int trueY = (int)y;

  ClientToScreen(&trueX, &trueY);

  // Create array of strings to pass to choose dialog
  int noStrings = menu->menuItems.Number();
  char **theStrings = new char *[noStrings];
  // Have an array of menu ids so can map position in
  // chooser list to menu id
  int *menuIds = new int[noStrings];

  int i = 0;
  
  for (wxNode * node = menu->menuItems.First (); node; node = node->Next ())
    {
      wxMenuItem *item = (wxMenuItem *) node->Data ();
      if ((item->itemId != -2) && (item->itemName && !item->subMenu) &&
          (item->isEnabled))
      {
        theStrings[i] = item->itemName;
        menuIds[i] = item->itemId;
        i ++;
      }
    }
  int stringIndex = wxGetSingleChoiceIndex("Please choose an option",
   "Menu", i, theStrings, trueParent, trueX, trueY);

  int menuId = 0;
  if (stringIndex > -1)
    menuId = menuIds[stringIndex];

  delete[] theStrings;
  delete[] menuIds;
  if (stringIndex > -1)
  {
    wxCommandEvent event (wxEVENT_TYPE_MENU_COMMAND);
    event.eventObject = menu;
    event.commandInt = menuId;
    menu->ProcessCommand (event);
  }
  return TRUE;
}
#endif

int CharCodeXToWX(KeySym keySym)
{
  int id;
  switch (keySym) {
    case XK_Shift_L:
    case XK_Shift_R:
      id = WXK_SHIFT; break;
    case XK_Control_L:
    case XK_Control_R:
      id = WXK_CONTROL; break;
    case XK_BackSpace:
      id = WXK_BACK; break;
    case XK_Delete:
      id = WXK_DELETE; break;
    case XK_Clear:
      id = WXK_CLEAR; break;
    case XK_Tab:
      id = WXK_TAB; break;
    case XK_numbersign:
      id = '#'; break;
    case XK_Return:
      id = WXK_RETURN; break;
    case XK_Escape:
      id = WXK_ESCAPE; break;
    case XK_Pause:
    case XK_Break:
      id = WXK_PAUSE; break;
    case XK_Num_Lock:
      id = WXK_NUMLOCK; break;
    case XK_Scroll_Lock:
      id = WXK_SCROLL; break;

    case XK_Home:
      id = WXK_HOME; break;
    case XK_End:
      id = WXK_END; break;
    case XK_Left:
      id = WXK_LEFT; break;
    case XK_Right:
      id = WXK_RIGHT; break;
    case XK_Up:
      id = WXK_UP; break;
    case XK_Down:
      id = WXK_DOWN; break;
    case XK_Next:
      id = WXK_NEXT; break;
    case XK_Prior:
      id = WXK_PRIOR; break;
    case XK_Menu:
      id = WXK_MENU; break;
    case XK_Select:
      id = WXK_SELECT; break;
    case XK_Cancel:
      id = WXK_CANCEL; break;
    case XK_Print:
      id = WXK_PRINT; break;
    case XK_Execute:
      id = WXK_EXECUTE; break;
    case XK_Insert:
      id = WXK_INSERT; break;
    case XK_Help:
      id = WXK_HELP; break;

    case XK_KP_Multiply:
      id = WXK_MULTIPLY; break;
    case XK_KP_Add:
      id = WXK_ADD; break;
    case XK_KP_Subtract:
      id = WXK_SUBTRACT; break;
    case XK_KP_Divide:
      id = WXK_DIVIDE; break;
    case XK_KP_Decimal:
      id = WXK_DECIMAL; break;
    case XK_KP_Equal:
      id = '='; break;
    case XK_KP_Space:
      id = ' '; break;
    case XK_KP_Tab:
      id = WXK_TAB; break;
    case XK_KP_Enter:
      id = WXK_RETURN; break;
    case XK_KP_0:
      id = WXK_NUMPAD0; break;
    case XK_KP_1:
      id = WXK_NUMPAD1; break;
    case XK_KP_2:
      id = WXK_NUMPAD2; break;
    case XK_KP_3:
      id = WXK_NUMPAD3; break;
    case XK_KP_4:
      id = WXK_NUMPAD4; break;
    case XK_KP_5:
      id = WXK_NUMPAD5; break;
    case XK_KP_6:
      id = WXK_NUMPAD6; break;
    case XK_KP_7:
      id = WXK_NUMPAD7; break;
    case XK_KP_8:
      id = WXK_NUMPAD8; break;
    case XK_KP_9:
      id = WXK_NUMPAD9; break;
    case XK_F1:
      id = WXK_F1; break;
    case XK_F2:
      id = WXK_F2; break;
    case XK_F3:
      id = WXK_F3; break;
    case XK_F4:
      id = WXK_F4; break;
    case XK_F5:
      id = WXK_F5; break;
    case XK_F6:
      id = WXK_F6; break;
    case XK_F7:
      id = WXK_F7; break;
    case XK_F8:
      id = WXK_F8; break;
    case XK_F9:
      id = WXK_F9; break;
    case XK_F10:
      id = WXK_F10; break;
    case XK_F11:
      id = WXK_F11; break;
    case XK_F12:
      id = WXK_F12; break;
    case XK_F13:
      id = WXK_F13; break;
    case XK_F14:
      id = WXK_F14; break;
    case XK_F15:
      id = WXK_F15; break;
    case XK_F16:
      id = WXK_F16; break;
    case XK_F17:
      id = WXK_F17; break;
    case XK_F18:
      id = WXK_F18; break;
    case XK_F19:
      id = WXK_F19; break;
    case XK_F20:
      id = WXK_F20; break;
    case XK_F21:
      id = WXK_F21; break;
    case XK_F22:
      id = WXK_F22; break;
    case XK_F23:
      id = WXK_F23; break;
    case XK_F24:
      id = WXK_F24; break;
    default:
      id = (keySym <= 255) ? (int)keySym : -1;
  } // switch
  return id;
}

KeySym CharCodeWXToX(int id)
{
  KeySym keySym;

  switch (id) {
    case WXK_CANCEL:            keySym = XK_Cancel; break;
    case WXK_BACK:              keySym = XK_BackSpace; break;
    case WXK_TAB:	        keySym = XK_Tab; break;
    case WXK_CLEAR:		keySym = XK_Clear; break;
    case WXK_RETURN:		keySym = XK_Return; break;
    case WXK_SHIFT:		keySym = XK_Shift_L; break;
    case WXK_CONTROL:		keySym = XK_Control_L; break;
    case WXK_MENU :		keySym = XK_Menu; break;
    case WXK_PAUSE:		keySym = XK_Pause; break;
    case WXK_ESCAPE:		keySym = XK_Escape; break;
    case WXK_SPACE:		keySym = ' '; break;
    case WXK_PRIOR:		keySym = XK_Prior; break;
    case WXK_NEXT :		keySym = XK_Next; break;
    case WXK_END:		keySym = XK_End; break;
    case WXK_HOME :		keySym = XK_Home; break;
    case WXK_LEFT :		keySym = XK_Left; break;
    case WXK_UP:		keySym = XK_Up; break;
    case WXK_RIGHT:		keySym = XK_Right; break;
    case WXK_DOWN :		keySym = XK_Down; break;
    case WXK_SELECT:		keySym = XK_Select; break;
    case WXK_PRINT:		keySym = XK_Print; break;
    case WXK_EXECUTE:		keySym = XK_Execute; break;
    case WXK_INSERT:		keySym = XK_Insert; break;
    case WXK_DELETE:		keySym = XK_Delete; break;
    case WXK_HELP :		keySym = XK_Help; break;
    case WXK_NUMPAD0:		keySym = XK_KP_0; break;
    case WXK_NUMPAD1:		keySym = XK_KP_1; break;
    case WXK_NUMPAD2:		keySym = XK_KP_2; break;
    case WXK_NUMPAD3:		keySym = XK_KP_3; break;
    case WXK_NUMPAD4:		keySym = XK_KP_4; break;
    case WXK_NUMPAD5:		keySym = XK_KP_5; break;
    case WXK_NUMPAD6:		keySym = XK_KP_6; break;
    case WXK_NUMPAD7:		keySym = XK_KP_7; break;
    case WXK_NUMPAD8:		keySym = XK_KP_8; break;
    case WXK_NUMPAD9:		keySym = XK_KP_9; break;
    case WXK_MULTIPLY:		keySym = XK_KP_Multiply; break;
    case WXK_ADD:		keySym = XK_KP_Add; break;
    case WXK_SUBTRACT:		keySym = XK_KP_Subtract; break;
    case WXK_DECIMAL:		keySym = XK_KP_Decimal; break;
    case WXK_DIVIDE:		keySym = XK_KP_Divide; break;
    case WXK_F1:		keySym = XK_F1; break;
    case WXK_F2:		keySym = XK_F2; break;
    case WXK_F3:		keySym = XK_F3; break;
    case WXK_F4:		keySym = XK_F4; break;
    case WXK_F5:		keySym = XK_F5; break;
    case WXK_F6:		keySym = XK_F6; break;
    case WXK_F7:		keySym = XK_F7; break;
    case WXK_F8:		keySym = XK_F8; break;
    case WXK_F9:		keySym = XK_F9; break;
    case WXK_F10:		keySym = XK_F10; break;
    case WXK_F11:		keySym = XK_F11; break;
    case WXK_F12:		keySym = XK_F12; break;
    case WXK_F13:		keySym = XK_F13; break;
    case WXK_F14:		keySym = XK_F14; break;
    case WXK_F15:		keySym = XK_F15; break;
    case WXK_F16:		keySym = XK_F16; break;
    case WXK_F17:		keySym = XK_F17; break;
    case WXK_F18:		keySym = XK_F18; break;
    case WXK_F19:		keySym = XK_F19; break;
    case WXK_F20:		keySym = XK_F20; break;
    case WXK_F21:		keySym = XK_F21; break;
    case WXK_F22:		keySym = XK_F22; break;
    case WXK_F23:		keySym = XK_F23; break;
    case WXK_F24:		keySym = XK_F24; break;
    case WXK_NUMLOCK:		keySym = XK_Num_Lock; break;
    case WXK_SCROLL:		keySym = XK_Scroll_Lock; break;
    default:                    keySym = id <= 255 ? (KeySym)id : 0;
  } // switch
  return keySym;
}

#ifdef wx_motif
Bool wxWindow::PreResize(void)
{
  return TRUE;
}

// All widgets should have this as their resize proc.
// OnSize sent to wxWindow via client data.
void wxWidgetResizeProc(Widget w, XConfigureEvent *event, String args[], int *num_args)
{
  wxWindow *win = (wxWindow *)wxWidgetHashTable->Get((long)w);
  if (!win)
    return;
  if (win->PreResize()) {
    int width, height;
    win->GetSize(&width, &height);
    win->GetEventHandler()->OnSize(width, height);
  }
}
#endif

void wxWindow::SetFont(wxFont *f)
{
#ifdef wx_motif
  if (f) 
    XtVaSetValues ((Widget)handle,
		   XmNfontList, f->GetInternalFont (XtDisplay((Widget)handle)),
		   NULL);
#endif
#ifdef wx_xview
#endif
}

#ifdef wx_motif
Bool wxTranslateMouseEvent(wxMouseEvent& wxevent, wxWindow *win, Widget widget, XEvent *xevent)
{
  switch (xevent->xany.type)
  {
    case EnterNotify:
    case LeaveNotify:
    case ButtonPress:
    case ButtonRelease:
    case MotionNotify:
      {
	WXTYPE eventType = 0;

        if (xevent->xany.type == LeaveNotify)
	{
          win->button1Pressed = FALSE;
          win->button2Pressed = FALSE;
          win->button3Pressed = FALSE;
          return FALSE;
	}
	else if (xevent->xany.type == MotionNotify)
	  {
	    eventType = wxEVENT_TYPE_MOTION;
	  }
	else if (xevent->xany.type == ButtonPress)
	  {
//            cout << "Button press\n";
	    if (xevent->xbutton.button == Button1)
	      {
		eventType = wxEVENT_TYPE_LEFT_DOWN;
		win->button1Pressed = TRUE;
//                cout << "Left button press\n";
	      }
	    else if (xevent->xbutton.button == Button2)
	      {
		eventType = wxEVENT_TYPE_MIDDLE_DOWN;
		win->button2Pressed = TRUE;
	      }
	    else if (xevent->xbutton.button == Button3)
	      {
		eventType = wxEVENT_TYPE_RIGHT_DOWN;
		win->button3Pressed = TRUE;
	      }
	  }
	else if (xevent->xany.type == ButtonRelease)
	  {
//            cout << "Button release\n";
	    if (xevent->xbutton.button == Button1)
	      {
		eventType = wxEVENT_TYPE_LEFT_UP;
		win->button1Pressed = FALSE;
//                cout << "Left button release\n";
	      }
	    else if (xevent->xbutton.button == Button2)
	      {
		eventType = wxEVENT_TYPE_MIDDLE_UP;
		win->button2Pressed = FALSE;
	      }
	    else if (xevent->xbutton.button == Button3)
	      {
		eventType = wxEVENT_TYPE_RIGHT_UP;
		win->button3Pressed = FALSE;
	      }
            else return FALSE;
	  }
          else return FALSE;

	wxevent.eventHandle = (char *)xevent;
        wxevent.eventType = eventType;

        Position x1, y1;
        XtVaGetValues(widget, XmNx, &x1, XmNy, &y1, NULL);

        int x2, y2;
        win->GetPosition(&x2, &y2);

        // The button x/y must be translated to wxWindows
        // window space - the widget might be a label or button,
        // within a form.
        int dx = 0;
        int dy = 0;
        if (widget != (Widget)win->handle)
        {
          dx = x1;
          dy = y1;
	}

        wxevent.x = xevent->xbutton.x + dx;
	wxevent.y = xevent->xbutton.y + dy;

/*
	wxevent.leftDown = win->button1Pressed;
	wxevent.middleDown = win->button2Pressed;
	wxevent.rightDown = win->button3Pressed;
*/
	wxevent.leftDown = ((eventType == wxEVENT_TYPE_LEFT_DOWN)
			    || (event_left_is_down (xevent) 
				&& (eventType != wxEVENT_TYPE_LEFT_UP)));
	wxevent.middleDown = ((eventType == wxEVENT_TYPE_MIDDLE_DOWN)
			      || (event_middle_is_down (xevent) 
				  && (eventType != wxEVENT_TYPE_MIDDLE_UP)));
	wxevent.rightDown = ((eventType == wxEVENT_TYPE_RIGHT_DOWN)
			     || (event_right_is_down (xevent) 
				 && (eventType != wxEVENT_TYPE_RIGHT_UP)));

	wxevent.shiftDown = xevent->xbutton.state & ShiftMask;
	wxevent.controlDown = xevent->xbutton.state & ControlMask;
        return TRUE;
    }
  }
  return FALSE;
}

Bool wxTranslateKeyEvent(wxKeyEvent& wxevent, wxWindow *win, Widget widget, XEvent *xevent)
{
  switch (xevent->xany.type)
  {
    case KeyPress:
      {
        char buf[20];
        
	KeySym keySym;
//	XComposeStatus compose;
//	(void) XLookupString ((XKeyEvent *) xevent, buf, 20, &keySym, &compose);
	(void) XLookupString ((XKeyEvent *) xevent, buf, 20, &keySym, NULL);
	int id = CharCodeXToWX (keySym);

	if (xevent->xkey.state & ShiftMask)
	  wxevent.shiftDown = TRUE;
	if (xevent->xkey.state & ControlMask)
	  wxevent.controlDown = TRUE;
	if (xevent->xkey.state & Mod3Mask)
	  wxevent.altDown = TRUE;
	if (xevent->xkey.state & Mod1Mask)
	  wxevent.metaDown = TRUE;
	wxevent.eventObject = win;
	wxevent.keyCode = id;
        wxevent.SetTimestamp(xevent->xkey.time);

        wxevent.x = xevent->xbutton.x;
	wxevent.y = xevent->xbutton.y;

        if (win->IsKindOf(CLASSINFO(wxCanvas)))
        {
          wxCanvas *canvas = (wxCanvas *)win;
  	  if (canvas->GetDC ())
	  {
	    wxevent.x = canvas->GetDC ()->DeviceToLogicalX (xevent->xbutton.x);
	    wxevent.y = canvas->GetDC ()->DeviceToLogicalY (xevent->xbutton.y);
	  }
        }
	if (id > -1)
	  return TRUE;
        else
          return FALSE;
	break;
      }
    default:
      break;
  }
  return FALSE;
}
#endif

#ifdef wx_motif
Bool wxAddWindowToTable(Widget w, wxWindow *win)
{
  wxWindow *oldItem = NULL;
#if DEBUG
//  printf("Adding widget %ld, name = %s\n", w, win->GetClassInfo()->GetClassName());
#endif
  if (oldItem = (wxWindow *)wxWidgetHashTable->Get ((long) w))
  {
    char buf[300];
    sprintf(buf, "Widget table clash: new widget is %ld, %s", (long)w, win->GetClassInfo()->GetClassName());
    wxError (buf);
    fflush(stderr);
    sprintf(buf, "Old widget was %s", oldItem->GetClassInfo()->GetClassName());
    wxError (buf);
    return FALSE;
  }

  wxWidgetHashTable->Put ((long) w, win);
  return TRUE;
}

wxWindow *wxGetWindowFromTable(Widget w)
{
  return (wxWindow *)wxWidgetHashTable->Get ((long) w);
}

void wxDeleteWindowFromTable(Widget w)
{
#if DEBUG
//  printf("Deleting widget %ld\n", w);
#endif
//  wxWindow *win = (wxWindow *)wxWidgetHashTable->Get ((long) w);
/*
#if DEBUG
  if (!win)
    printf("Did not find a window for widget %ld\n", w);
#endif
*/
  wxWidgetHashTable->Delete((long)w);
}
  
#endif

/*
 * Set min/max size
 */
 
void wxWindow::SetSizeHints(int minW, int minH, int maxW, int maxH, int incW, int incH)
{
  if (!this->IsKindOf(CLASSINFO(wxFrame)))
    return;

  wxFrame *frame = (wxFrame *)this;

#ifdef wx_motif
 if (minW > -1)
    XtVaSetValues(frame->frameShell, XmNminWidth, minW, NULL);
  if (minH > -1)
    XtVaSetValues(frame->frameShell, XmNminHeight, minH, NULL);
  if (maxW > -1)
    XtVaSetValues(frame->frameShell, XmNmaxWidth, maxW, NULL);
  if (maxH > -1)
    XtVaSetValues(frame->frameShell, XmNmaxHeight, maxH, NULL);
  if (incW > -1)
    XtVaSetValues(frame->frameShell, XmNwidthInc, incW, NULL);
  if (incH > -1)
    XtVaSetValues(frame->frameShell, XmNheightInc, incH, NULL);
#endif
#ifdef wx_xview
  XSizeHints* sizeHints=XAllocSizeHints();
  if (!sizeHints) return;
  long flags=0;
  if (minW>-1 && minH >-1 ) flags=PMinSize;
  if (maxW>-1 && maxH >-1 ) flags=flags | PMaxSize;
  if (incW>-1 && incH >-1 ) flags=flags | PResizeInc;
  sizeHints->flags     = flags;
  sizeHints->min_width = minW;
  sizeHints->min_height= minH;
  sizeHints->max_width = maxW;
  sizeHints->max_height= maxH;
  sizeHints->width_inc = incW;
  sizeHints->height_inc= incH;
  XSetWMNormalHints(frame->GetXDisplay(),frame->GetXWindow(),sizeHints);
  XFree((char *)sizeHints);
#endif
}
