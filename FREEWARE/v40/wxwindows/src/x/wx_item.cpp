/*
 * File:      wx_item.cc
 * Purpose:     Panel item implementation (X version)
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	April 1995
 * RCS_ID:      $Id: wx_item.cc,v 1.3 1994/08/14 21:28:43 edz Exp $
 * Copyright:   (c) 1993, AIAI, University of Edinburgh
 */

static const char sccsid[] = "@(#)wx_item.cc	1.2 5/9/94";

#ifdef __GNUG__
#pragma implementation
#pragma implementation "wx_item.h"
#endif

#include <stdlib.h>
#include "wx_defs.h"
#include "wx_utils.h"
#include "wx_privt.h"
#include "wx_item.h"

#ifdef wx_xview
#include "wx_buttn.h"
#endif

#ifdef wx_motif
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>

void wxPanelItemEventHandler (Widget    wid,
                              XtPointer client_data,
                              XEvent*   event,
                              Boolean *continueToDispatch);
#endif

#ifdef wx_xview
#include <xview/panel.h>
//#include <xview/openmenu.h>
//#include <xview/svrimage.h>

#if USE_SCROLLBAR
static Bool wxScrollbarLeftButtonAction (wxPanel * panel, Event * x_event);
#include "wx_scrol.h"
#endif

#endif

// See above notes
char *
fillCopy (const char *str)
{
  size_t len = strlen (str);
  char *ptr = new char[len + 1];
  // memset should be the fastest
  memset (ptr, MEANING_CHARACTER, len);
  ptr[len] = '\0';
  return ptr;
}

/* When implementing a new item, be sure to:

 * - add the item to the parent panel
 * - set window_parent to the parent
 * - NULL any extra child window pointers not created for this item
 *   (e.g. label control that wasn't needed)
 * - delete any extra child windows in the destructor (e.g. label control)
 * - implement GetSize and SetSize
 * - to find panel position if coordinates are (-1, -1), use GetPosition
 * - call AdvanceCursor after creation, for panel layout mechanism.
 *
 */

/*
   Motif notes

   A panel is a form.
   Each item is created on a RowColumn or Form of its own, to allow a label to
   be positioned. wxListBox and wxMultiText have forms, all the others have RowColumns.
   This is to allow labels to be positioned to the top left (can't do it with a
   RowColumn as far as I know).
   AttachWidget positions widgets relative to one another (left->right, top->bottom)
   unless the x, y coordinates are given (more than -1).
 */


// Item members
wxItem::wxItem (void)
{
#ifdef wx_motif
  formWidget = 0;
  labelWidget = 0;
  rowNumber = 0;
  colNumber = 0;
  SetLabelPosition(wxHORIZONTAL);
  oldTranslations = 0;
  canAddEventHandler = FALSE;
  inSetValue = FALSE;
#endif
#ifdef wx_x
  actualLabel = NULL;
#endif
}

wxItem::~wxItem (void)
{
#ifdef wx_motif
    if (CanAddEventHandler())
    {
      XtRemoveEventHandler((Widget)handle,
         ButtonPressMask | ButtonReleaseMask | PointerMotionMask, // | KeyPressMask,
         False,
         wxPanelItemEventHandler,
         (XtPointer)this);
      if (labelWidget && labelWidget != (Widget)handle)
      {
        XtRemoveEventHandler(labelWidget,
           ButtonPressMask | ButtonReleaseMask | PointerMotionMask, // | KeyPressMask,
           False,
           wxPanelItemEventHandler,
           (XtPointer)this);
        wxDeleteWindowFromTable(labelWidget);
      }
    }

  if (labelWidget && (labelWidget != (Widget) handle))
    XtDestroyWidget (labelWidget);

  if (formWidget)
    {
      wxDeleteWindowFromTable(formWidget);

      if (handle)
        wxDeleteWindowFromTable((Widget)handle);

      if (formWidget)
	XtDestroyWidget (formWidget);

      // Necessary, to avoid ~wxWindow to (re)delete the Widget!!
      // (because all formWidget subtree has been deleted)
      handle = (char *) NULL;
    }
#endif
#ifdef wx_xview
  if (actualLabel)
    delete[] actualLabel;
#endif
}

#ifdef wx_motif
void wxItem::RemoveTranslations(Bool remove)
{
  Widget theWidget = (Widget)handle;
  if (theWidget)
  {
    if (remove && !oldTranslations)
    {
      XtVaGetValues(theWidget, XmNtranslations, &oldTranslations, NULL);
      XtUninstallTranslations(theWidget);
    }
    else if (!remove && oldTranslations)
    {
      XtVaSetValues(theWidget, XmNtranslations, oldTranslations, NULL);
      oldTranslations = 0;
    }
  }
}
#endif

void wxItem::GetSize (int *width, int *height)
{
#ifdef wx_motif
  Dimension xx, yy;
  XtVaGetValues (formWidget, XmNwidth, &xx, XmNheight, &yy, NULL);
  *width = xx;
  *height = yy;
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) handle;
  Rect *rect = (Rect *) xv_get (item, XV_RECT);

  *height = rect->r_height;
  *width = rect->r_width;
#endif
}

void wxItem::GetPosition (int *x, int *y)
{
#ifdef wx_motif
  Position xx, yy;
  XtVaGetValues (formWidget, XmNx, &xx, XmNy, &yy, NULL);
  *x = xx;
  *y = yy;
#endif
#ifdef wx_xview
  Xv_opaque x_win = (Xv_opaque) handle;

  *x = (int) xv_get (x_win, XV_X);
  *y = (int) xv_get (x_win, XV_Y);
#endif
}

void wxItem::SetSize (int x, int y, int width, int height, int sizeFlags)
{
#ifdef wx_motif
  Boolean is_managed = XtIsManaged(formWidget);
  if (is_managed)
    XtUnmanageChild (formWidget);

  if (x > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    XtVaSetValues (formWidget, XmNleftAttachment, XmATTACH_SELF,
		   XmNx, x,
		   NULL);
  if (y > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    XtVaSetValues (formWidget, XmNtopAttachment, XmATTACH_SELF,
		   XmNy, y,
		   NULL);
  if (width > -1)
    XtVaSetValues (formWidget, XmNwidth, width, NULL);
  if (height > -1)
    XtVaSetValues (formWidget, XmNheight, height, NULL);

  if (is_managed)
    XtManageChild (formWidget);

  GetEventHandler()->OnSize (width, height);
#endif
#ifdef wx_xview
  Xv_opaque x_win = (Xv_opaque) handle;

  if (x > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    (void) xv_set (x_win, XV_X, x, NULL);

  if (y > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    (void) xv_set (x_win, XV_Y, y, NULL);

  if (width > -1)
    (void) xv_set (x_win, XV_WIDTH, width, NULL);

  if (height > -1)
    (void) xv_set (x_win, XV_HEIGHT, height, NULL);
  GetEventHandler()->OnSize (width, height);
#endif
}

void wxItem::SetLabel (char *label)
{
  char buf[400];
  wxStripMenuCodes(label, buf);
#ifdef wx_motif
  if (!labelWidget)
    return;
  XmString text = XmStringCreateSimple (buf);
  XtVaSetValues (labelWidget,
		 XmNlabelString, text,
		 NULL);
  XmStringFree (text);
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) handle;
  xv_set (item, PANEL_LABEL_STRING, buf, NULL);
#endif
}

char *wxItem::GetLabel (void)
{
#ifdef wx_motif
  if (!labelWidget)
    return NULL;

  XmString text;
  char *s;
  XtVaGetValues (labelWidget,
		 XmNlabelString, &text,
		 NULL);

  if (XmStringGetLtoR (text, XmSTRING_DEFAULT_CHARSET, &s))
    {
      strcpy (wxBuffer, s);
      XtFree (s);
      XmStringFree(text);
      return wxBuffer;
    }
  else
    {
      XmStringFree(text);
      return NULL;
    }
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) handle;
  strcpy (wxBuffer, (char *) xv_get (item, PANEL_LABEL_STRING));
  return wxBuffer;
#endif
}

void wxItem::SetFocus (void)
{
#ifdef wx_motif
  wxWindow::SetFocus ();
#endif
#ifdef wx_xview
#endif
}

Bool wxItem::Show (Bool show)
{
  isShown = show;
#ifdef wx_motif
  if (formWidget)
    {
      if (show)
	XtManageChild (formWidget);
      else
	XtUnmanageChild (formWidget);
    }
#endif
#ifdef wx_xview
  Xv_opaque window = (Xv_opaque) handle;
  xv_set (window, XV_SHOW, show, NULL);
#endif
  return TRUE;
}

void wxItem::Enable (Bool enable)
{
#ifdef wx_motif
  wxWindow::Enable(enable);
#endif
#ifdef wx_xview
  Xv_opaque window = (Xv_opaque) handle;
  if (window)
    xv_set (window, PANEL_INACTIVE, !enable, NULL);
#endif
}

float wxItem::GetCharHeight (void)
{
#ifdef wx_motif
  return 0.0;
#endif
#ifdef wx_xview
  Xv_Font the_font;

  if (!(font && font->x_font))
    {
      Xv_opaque thing = (Xv_opaque) (GetParent ()->handle);
      the_font = (Xv_Font) xv_get (thing, XV_FONT);
    }
  else
    the_font = font->x_font;

  return (float) xv_get (the_font, FONT_DEFAULT_CHAR_HEIGHT);
#endif
}

float wxItem::GetCharWidth (void)
{
#ifdef wx_motif
  return 0.0;
#endif
#ifdef wx_xview
  Xv_Font the_font;

  if (!(font && font->x_font))
    {
      Xv_opaque thing = (Xv_opaque) (GetParent ()->handle);
      the_font = (Xv_Font) xv_get (thing, XV_FONT);
    }
  else
    the_font = font->x_font;

  return (float) xv_get (the_font, FONT_DEFAULT_CHAR_WIDTH);
#endif
}

#ifdef wx_motif
// Find the letter corresponding to the mnemonic, for Motif
char wxFindMnemonic (char *s)
{
  char mnem = 0;
  int len = strlen (s);
  int i;
  for (i = 0; i < len; i++)
    {
      if (s[i] == '&')
	{
	  // Carefully handle &&
	  if ((i + 1) <= len && s[i + 1] == '&')
	    i++;
	  else
	    {
	      mnem = s[i + 1];
	      break;
	    }
	}
    }
  return mnem;
}
#endif

#ifdef wx_xview
void wxPanelEventProc (Panel x_panel, Event * x_event)
{
  wxPanel *panel = (wxPanel *) xv_get (x_panel, WIN_CLIENT_DATA);
  if (panel)
    {
#if USE_SCROLLBAR
    if (wxScrollbarLeftButtonAction (panel, x_event))
      return;
#endif
      if (event_id (x_event) == KBD_USE)
	panel->GetEventHandler()->OnSetFocus ();
      else if (event_id (x_event) == KBD_DONE)
	panel->GetEventHandler()->OnKillFocus ();
      else if (x_event->ie_xevent->xany.type == KeyPress)
      {
        KeySym keySym;
        (void) XLookupString ((XKeyEvent *) x_event->ie_xevent, wxBuffer, 20, &keySym, NULL);
        int id = CharCodeXToWX (keySym);

        // Find the default item, if any, and execute its callback
	Panel_item item = (Panel_item) xv_get (x_panel, PANEL_DEFAULT_ITEM);
	wxButton *wx_item = NULL;
	if (item)
	  wx_item = (wxButton *) xv_get (item, PANEL_CLIENT_DATA);

        if ((id == 13) && wx_item)
        {
          wxCommandEvent event (wxEVENT_TYPE_BUTTON_COMMAND);
          event.eventObject = wx_item;
          wx_item->ProcessCommand (event);
	}
        else if ((id > -1) && (id != WXK_SHIFT) && (id != WXK_CONTROL))
	{
          wxKeyEvent event (wxEVENT_TYPE_CHAR);
          if (event_shift_is_down (x_event))
            event.shiftDown = TRUE;
          if (event_ctrl_is_down (x_event))
            event.controlDown = TRUE;
          event.keyCode = id;
          event.eventObject = panel;

          event.x = event_x (x_event);
          event.y = event_y (x_event);
        }
      }
      else
	{
	  WXTYPE eventType = 0;

	  if ((event_id (x_event) == LOC_DRAG) || (event_id (x_event) == LOC_MOVE))
	    eventType = wxEVENT_TYPE_MOTION;
	  else if (event_is_button (x_event))
	    {
	      if (event_is_down (x_event))
		{
		  if (event_id (x_event) == BUT (1))
		    eventType = wxEVENT_TYPE_LEFT_DOWN;
		  else if (event_id (x_event) == BUT (2))
		    eventType = wxEVENT_TYPE_MIDDLE_DOWN;
		  else if (event_id (x_event) == BUT (3))
		    eventType = wxEVENT_TYPE_RIGHT_DOWN;
		}
	      else if (event_is_up (x_event))
		{
		  if (event_id (x_event) == BUT (1))
		    eventType = wxEVENT_TYPE_LEFT_UP;
		  else if (event_id (x_event) == BUT (2))
		    eventType = wxEVENT_TYPE_MIDDLE_UP;
		  else if (event_id (x_event) == BUT (3))
		    eventType = wxEVENT_TYPE_RIGHT_UP;
		}
	    }
	  wxMouseEvent event (eventType);

	  event.eventObject = panel;
	  event.controlDown = event_ctrl_is_down (x_event);
	  event.shiftDown = event_shift_is_down (x_event);
	  event.altDown = event_alt_is_down (x_event);
	  event.leftDown = event_left_is_down (x_event);
	  event.middleDown = event_middle_is_down (x_event);
	  event.rightDown = event_right_is_down (x_event);
	  event.eventHandle = (char *) x_event;

	  event.x = event_x (x_event);
	  event.y = event_y (x_event);

	  panel->GetEventHandler()->OnEvent (event);
	}
    }
}

#endif

#ifdef wx_motif
char * wxFindAccelerator (char *s)
{
// The accelerator text is after the \t char.
  while (*s && *s != '\t')
    s++;
  if (*s == '\0')
    return (NULL);
  s++;
/*
   Now we need to format it as X standard:

   input            output

   F7           --> <Key>F7
   Ctrl+N       --> Ctrl<Key>N
   Alt+k        --> Meta<Key>k
   Ctrl+Shift+A --> Ctrl Shift<Key>A

 */

  wxBuffer[0] = '\0';
  char *tmp = copystring (s);
  s = tmp;
  char *p = s;

  while (1)
    {
      while (*p && *p != '+')
	p++;
      if (*p)
	{
	  *p = '\0';
	  if (wxBuffer[0])
	    strcat (wxBuffer, " ");
	  if (strcmp (s, "Alt"))
	    strcat (wxBuffer, s);
	  else
	    strcat (wxBuffer, "Meta");
	  s = p + 1;
	  p = s;
	}
      else
	{
	  strcat (wxBuffer, "<Key>");
	  strcat (wxBuffer, s);
	  break;
	}
    }
  delete[]tmp;
  return wxBuffer;
}

XmString wxFindAcceleratorText (char *s)
{
// The accelerator text is after the \t char.
  while (*s && *s != '\t')
    s++;
  if (*s == '\0')
    return (NULL);
  s++;
  XmString text = XmStringCreateSimple (s);
  return text;
}
#endif

#ifdef wx_motif
void wxPanelItemEventHandler (Widget    wid,
                              XtPointer client_data,
                              XEvent*   event,
                              Boolean *continueToDispatch)
{
  // Widget can be a label or the actual widget.

  wxItem *item = (wxItem *)wxWidgetHashTable->Get((long)wid);
  if (item)
  {
    wxMouseEvent wxevent(0);
    if (wxTranslateMouseEvent(wxevent, item, wid, event))
      item->GetEventHandler()->OnEvent(wxevent);
  }
  *continueToDispatch = True;
}

#endif

/*
    If scrollbar is a chid of panel it can't receive any events
    from left mouse button (BUT1). Events are sending to the
    panel (strange! why only left button?). May be because of 
    scrollbar isn't standard XView "PANEL ITEM". Function
    wxScrollbarLeftButtonAction corrects this problem.
    I tested it on Linux, X11R5, XView 3.0.
*/

#ifdef wx_xview
#if USE_SCROLLBAR
static Bool wxScrollbarLeftButtonAction (wxPanel * panel, Event * x_event)
{
  static wxWindow *inDrag = NULL;
  if ((event_id (x_event) == LOC_DRAG && event_left_is_down (x_event) && inDrag) ||
         (event_is_button (x_event) && event_id (x_event) == BUT (1))) {
    for(wxNode *node = panel->GetChildren()->First(); node; node = node->Next()) {
        wxWindow *win = (wxWindow *)node->Data();
    	if (win->IsKindOf(CLASSINFO(wxScrollBar)) &&
    			xv_get((Xv_Window) win->handle, XV_SHOW)) {
          int sb_w, sb_h;
          win->GetSize(&sb_w, &sb_h);
          Window x_win  = win->GetXWindow();
          XButtonEvent * x_but_event = (XButtonEvent *) event_xevent(x_event);
          Display *display = panel->GetXDisplay();
          int event_x, event_y;
          Window childWindow;
          XTranslateCoordinates(display, panel->GetXWindow(), x_win, x_but_event->x,
                  x_but_event->y, &event_x, &event_y, &childWindow);
          if (inDrag==win || (event_x >= 0 && event_x < sb_w && 
          			     event_y >= 0 && event_y < sb_h)) {
          	if (event_is_down (x_event))
          		inDrag = win;
          	else if (event_is_up (x_event))
          		inDrag = NULL;
            x_but_event->window = x_win;
            x_but_event->x = event_x;
            x_but_event->y = event_y;
            XPutBackEvent(display, (XEvent *) x_but_event);
            return TRUE;
          }
        }
    }
  }
  inDrag = NULL;
  return FALSE;
}
#endif
#endif

void wxItem::ChangeColour(void)
{
}

extern int str16len(const char *);

void wxItem::GetTextExtent (const char *string,
                            float *x,
                            float *y,
                            float *descent,
                            float *externalLeading,
                            wxFont * fontparm, Bool use16)
{
  wxFont * the_font = fontparm ? fontparm : this->font;
  if (!the_font)
  {
    cerr << "wxWindows warning - GetTextExtent called with NULL font\n";
    *x = *y = -1;
    return;
  }
#ifdef wx_motif
  XFontStruct *fontStruct = NULL;
  the_font->GetInternalFont(wxGetDisplay(), &fontStruct);

  if (!fontStruct)
  {
    int res_x = 100;
    int res_y = 100;
    fontStruct = wxFontPool->
                   FindNearestFont(
                                   the_font->GetFontId(),
                                   the_font->GetStyle(),
                                   the_font->GetWeight(),
                                   (int)(10*(the_font->GetPointSize()+0.5)),
                                   the_font->GetUnderlined(),
                                   res_x,
                                   res_y, wxGetDisplay());
  }
#endif
#ifdef wx_xview
  Xv_Font xvfont;
  if (the_font->x_font)
  {
    xvfont = the_font->x_font;
  }
  else
  {
    xvfont = wxFontPool->FindNearestFont(the_font->GetFontId(),
                                         the_font->GetStyle(),
                                         the_font->GetWeight(),
                                         the_font->GetPointSize(),
                                         the_font->GetUnderlined(),
                                         0,
                                         0, wxGetDisplay());
  }
  XFontStruct *fontStruct = (XFontStruct *)xv_get(xvfont, FONT_INFO);
#endif
  int direction, ascent, descent2;
  XCharStruct overall;
  if (use16)
    XTextExtents16(fontStruct, (XChar2b *)string, str16len(string), 
		   &direction, &ascent, &descent2, &overall);
  else
    XTextExtents(fontStruct,
               string,
               strlen(string),
               &direction,
               &ascent,
               &descent2,
               &overall);
  *x = overall.width;
  *y = ascent + descent2;
  if (descent)
  {
    *descent = (float) descent2;
  }
  if (externalLeading)
  {
    *externalLeading = 0.0;
  }
}
