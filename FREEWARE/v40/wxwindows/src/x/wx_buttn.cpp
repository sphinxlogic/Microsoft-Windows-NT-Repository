/*
 * File:        wx_buttn.cc
 * Purpose:     Button implementation (X version)
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	April 1995
 * Copyright:   (c) 1995, AIAI, University of Edinburgh
 */

#ifdef __GNUG__
#pragma implementation
#pragma implementation "wx_buttn.h"
#endif

#include <stdlib.h>
#include "wx_defs.h"
#include "wx_utils.h"
#include "wx_buttn.h"
#include "wx_privt.h"

#ifdef wx_xview
// Button item notifier proc
void wxItemProc (Panel_item item, Event * event);
#endif

#ifdef wx_motif
#include <Xm/PushBG.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>

#if !USE_IMAGE_PIXMAP_METHOD
#ifdef wx_motif
Pixmap
XCreateInsensitivePixmap( Display *display, Pixmap pixmap );
#endif
#endif

void 
wxButtonCallback (Widget w, XtPointer clientData, XtPointer ptr)
{
  if (!wxWidgetHashTable->Get ((long) w))
    // Widget has been deleted!
    return;

  wxButton *item = (wxButton *) clientData;
  wxCommandEvent event (wxEVENT_TYPE_BUTTON_COMMAND);
  event.eventObject = item;
  item->ProcessCommand (event);
}
#endif

// Button item notifier

#ifdef wx_xview
void 
wxItemProc (Panel_item x_item, Event * x_event)
{
  wxButton *item = (wxButton *) xv_get (x_item, PANEL_CLIENT_DATA);

  // Added by Lindsay Pender
  xv_set (x_item,                             // Don't dismiss unpinned window
        PANEL_NOTIFY_STATUS, XV_ERROR,
        NULL);

  wxCommandEvent event (wxEVENT_TYPE_BUTTON_COMMAND);
  event.eventObject = item;
  item->ProcessCommand (event);
}
#endif

wxButton::wxButton (void)
{
  buttonBitmap = NULL;
}

wxButton::wxButton (wxPanel * panel, wxFunction Function, char *label,
	  int x, int y, int width, int height, long style, char *name):
wxbButton (panel, Function, label, x, y, width, height, style, name)
{
  Create (panel, Function, label, x, y, width, height, style, name);
}

wxButton::wxButton (wxPanel * panel, wxFunction Function, wxBitmap * bitmap,
	  int x, int y, int width, int height, long style, char *name):
wxbButton (panel, Function, bitmap, x, y, width, height, style, name)
{
  Create (panel, Function, bitmap, x, y, width, height, style, name);
}

Bool wxButton::Create (wxPanel * panel, wxFunction Function, char *label,
	int x, int y, int width, int height, long style, char *name)
{
  isBitmap = FALSE;
  SetName(name);
  if (panel)
    panel->AddChild (this);
  buttonFont = panel->buttonFont;
  labelFont = panel->labelFont;
  backColour = panel->backColour;
  labelColour = panel->labelColour;
  buttonColour = panel->buttonColour;
  buttonBitmap = NULL;
  window_parent = panel;
  windowStyle = style;

  Callback (Function);

  SetLabelPosition(wxHORIZONTAL);

#ifdef wx_motif
  canAddEventHandler = TRUE;
  SetName(name);

  Widget panelForm = panel->panelWidget;
  if (!label)
    label = " ";
  char buf[400];
  wxStripMenuCodes(label, buf);
  XmString text = XmStringCreateSimple (buf);

  formWidget = XtVaCreateManagedWidget (windowName,
					xmFormWidgetClass, panelForm,
					XmNmarginHeight, 0,
					XmNmarginWidth, 0,
					NULL);

  /*
   * Patch Note (important)
   * There is no major reason to put a defaultButtonThickness here.
   * Not requesting it give the ability to put wxButton with a spacing
   * as small as requested. However, if some button become a DefaultButton,
   * other buttons are no more aligned -- This is why we set
   * defaultButtonThickness of ALL buttons belonging to the same wxPanel,
   * in the ::SetDefaultButton method.
   */
  Widget buttonWidget = XtVaCreateManagedWidget ("button",
  // Gadget causes problems for default button operation.
  // Ah! But now you can create wxButton which have 'problems' with
  // the wxCOLOURED flag. And we are both happy!!
  // [It seems to me that it's better to use as MANY gadgets as possible...]
  // Now #ifdefed independently, JACS 25/4/93
#if (USE_GADGETS && USE_BUTTON_GADGET)
						 style & wxCOLOURED ?
			  xmPushButtonWidgetClass : xmPushButtonGadgetClass,
						 formWidget,
#else
					xmPushButtonWidgetClass, formWidget,
#endif
						 XmNlabelString, text,
//                  XmNdefaultButtonShadowThickness, 1, // See comment for wxButton::SetDefault
					    XmNtopAttachment, XmATTACH_FORM,
					   XmNleftAttachment, XmATTACH_FORM,
					 XmNbottomAttachment, XmATTACH_FORM,
					  XmNrightAttachment, XmATTACH_FORM,
						 NULL);

  XmStringFree (text);

  // Record in case button callback gets called after the widget has
  // been deleted.
  wxAddWindowToTable(buttonWidget, this);

  if (buttonFont)
    XtVaSetValues (buttonWidget,
		   XmNfontList, buttonFont->GetInternalFont (XtDisplay(formWidget)),
		   NULL);

  handle = (char *) buttonWidget;

  XtAddCallback (buttonWidget, XmNactivateCallback, (XtCallbackProc) wxButtonCallback,
		 (XtPointer) this);

/*
    XtInsertEventHandler((Widget)buttonWidget,
       ButtonPressMask | PointerMotionMask | KeyPressMask,
       True,
       wxPanelItemEventHandler,
       (XtPointer)this,
       XtListHead);
*/

  panel->AttachWidget (this, formWidget, x, y, width, height);
//  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_NONE, NULL);
  ChangeColour ();
#endif
#ifdef wx_xview
  Panel x_panel = (Panel) (panel->GetHandle ());
  Panel_item x_button;
  char buf[400];
  wxStripMenuCodes(label, buf);

  if (panel->new_line)
    {
      x_button = (Panel_item) xv_create (x_panel, PANEL_BUTTON, PANEL_LAYOUT, PANEL_HORIZONTAL, PANEL_NEXT_ROW, -1, NULL);
      panel->new_line = FALSE;
    }
  else
    x_button = (Panel_item) xv_create (x_panel, PANEL_BUTTON, PANEL_LAYOUT, PANEL_HORIZONTAL, NULL);

  xv_set (x_button,
	  PANEL_LABEL_STRING, buf,
	  PANEL_NOTIFY_PROC, wxItemProc,
	  PANEL_CLIENT_DATA, (char *) this,
	  NULL);
/*
   Doesn't seem to works
   if (buttonFont)
   xv_set(x_button, XV_FONT, buttonFont->GetInternalFont(XtDisplay(formWidget)), NULL) ;
 */
/*
  if (x > -1 && y > -1)
    (void) xv_set (x_button, XV_X, x, XV_Y, y, NULL);
*/
  handle = (char *) x_button;

  SetSize(x, y, width, height);

#endif
  return TRUE;
}

Bool wxButton::Create (wxPanel * panel, wxFunction Function, wxBitmap * bitmap,
	int x, int y, int width, int height, long style, char *name)
{
  isBitmap = TRUE;
  SetName(name);
  if (panel)
    panel->AddChild (this);
  buttonFont = panel->buttonFont;
  labelFont = panel->labelFont;
  backColour = panel->backColour;
  labelColour = panel->labelColour;
  buttonColour = panel->buttonColour;
  buttonBitmap = bitmap;
  window_parent = panel;
  windowStyle = style;

  Callback (Function);
  SetLabelPosition(wxHORIZONTAL);

#ifdef wx_motif
  canAddEventHandler = TRUE;
  SetName(name);

  Widget panelForm = panel->panelWidget;

  formWidget = XtVaCreateManagedWidget (windowName,
					xmFormWidgetClass, panelForm,
					NULL);

  /*
   * Patch Note (important)
   * There is no major reason to put a defaultButtonThickness here.
   * Not requesting it give the ability to put wxButton with a spacing
   * as small as requested. However, if some button become a DefaultButton,
   * other buttons are no more aligned -- This is why we set
   * defaultButtonThickness of ALL buttons belonging to the same wxPanel,
   * in the ::SetDefaultButton method.
   */
  Widget buttonWidget = XtVaCreateManagedWidget ("button",
  // Gadget causes problems for default button operation.
  // Ah! But now you can create wxButton which have 'problems' with
  // the wxCOLOURED flag. And we are both happy!!
  // [It seems to me that it's better to use as MANY gadgets as possible...]
#if (USE_GADGETS && USE_BUTTON_GADGET)
						 style & wxCOLOURED ?
			  xmPushButtonWidgetClass : xmPushButtonGadgetClass,
						 formWidget,
#else
					xmPushButtonWidgetClass, formWidget,
#endif
//                  XmNdefaultButtonShadowThickness, 1, // See comment for wxButton::SetDefault
					    XmNtopAttachment, XmATTACH_FORM,
					   XmNleftAttachment, XmATTACH_FORM,
					 XmNbottomAttachment, XmATTACH_FORM,
					  XmNrightAttachment, XmATTACH_FORM,
						 NULL);

  // Record in case button callback gets called after the widget has
  // been deleted.

  wxAddWindowToTable(buttonWidget, this);

  handle = (char *) buttonWidget;

  // Yes, this is useful, even if we are setting a wxBitmap*, because
  // one can call ::SetLabel(char*) on this item!
  if (buttonFont)
    XtVaSetValues (buttonWidget,
		   XmNfontList, buttonFont->GetInternalFont (XtDisplay(formWidget)),
		   NULL);

  if (bitmap)
  {
    Pixmap p1, p2;

    p1 = bitmap->GetLabelPixmap(buttonWidget);
    p2 = bitmap->GetInsensPixmap(buttonWidget);

    if(p1 == p2) // <- the Get...Pixmap()-functions return the same pixmap!
    p2 =
      XCreateInsensitivePixmap(DisplayOfScreen(XtScreen(buttonWidget)), p1);

    XtVaSetValues (buttonWidget,
	  	   XmNlabelPixmap, p1,
	           XmNlabelInsensitivePixmap, p2,
		   XmNarmPixmap, bitmap->GetArmPixmap (buttonWidget),
		   XmNlabelType, XmPIXMAP,
		   NULL);
  }

  XtAddCallback (buttonWidget, XmNactivateCallback, (XtCallbackProc) wxButtonCallback,
		 (XtPointer) this);

  panel->AttachWidget (this, formWidget, x, y, width, height);
//  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_NONE, NULL);
  ChangeColour ();
#endif
#ifdef wx_xview
  Panel x_panel = (Panel) (panel->GetHandle ());
  Panel_item x_button;

  if (panel->new_line)
    {
      x_button = (Panel_item) xv_create (x_panel, PANEL_BUTTON, PANEL_LAYOUT, PANEL_HORIZONTAL, PANEL_NEXT_ROW, -1, NULL);
      panel->new_line = FALSE;
    }
  else
    x_button = (Panel_item) xv_create (x_panel, PANEL_BUTTON, PANEL_LAYOUT, PANEL_HORIZONTAL, NULL);

  xv_set (x_button,
	  PANEL_NOTIFY_PROC, wxItemProc,
	  PANEL_CLIENT_DATA, (char *) this,
	  NULL);
/*
   if (buttonFont)
   xv_set(x_button, XV_FONT, buttonFont->GetInternalFont(XtDisplay(formWidget)), NULL) ;
 */
  if (x > -1 && y > -1)
    (void) xv_set (x_button, XV_X, x, XV_Y, y, NULL);

  if (width > 0)
    {
      xv_set (x_button, PANEL_LABEL_WIDTH, (int) width, NULL);
    };

  if (bitmap)
  {
    if (!bitmap->x_image)
      bitmap->CreateServerImage(TRUE);

    xv_set (x_button, PANEL_LABEL_IMAGE, bitmap->x_image, NULL);
  }

  handle = (char *) x_button;
#endif
  return TRUE;
}

wxButton::~wxButton (void)
{
#ifdef wx_motif
//  wxWidgetHashTable->Delete ((long) handle);
#endif
}

void wxButton::ChangeColour (void)
{
#ifdef wx_motif
  int change;

  wxPanel *panel = (wxPanel *) window_parent;
  if (!(panel->GetWindowStyleFlag() & wxUSER_COLOURS))
    return;
    
  change = wxComputeColors (XtDisplay(formWidget),
                            panel->GetBackgroundColour(),
			    panel->GetButtonColour());
  if (change == wxBACK_COLORS)
    XtVaSetValues (formWidget,
		   XmNbackground, itemColors[wxBACK_INDEX].pixel,
		   XmNtopShadowColor, itemColors[wxTOPS_INDEX].pixel,
		   XmNbottomShadowColor, itemColors[wxBOTS_INDEX].pixel,
		   XmNforeground, itemColors[wxFORE_INDEX].pixel,
		   NULL);
  else if (change == wxFORE_COLORS)
    XtVaSetValues (formWidget,
		   XmNforeground, itemColors[wxFORE_INDEX].pixel,
		   NULL);

  change = wxComputeColors (XtDisplay(formWidget), GetBackgroundColour(), GetButtonColour());
  if (change == wxBACK_COLORS)
    XtVaSetValues ((Widget) handle,
		   XmNbackground, itemColors[wxBACK_INDEX].pixel,
		   XmNtopShadowColor, itemColors[wxTOPS_INDEX].pixel,
		   XmNbottomShadowColor, itemColors[wxBOTS_INDEX].pixel,
		   XmNarmColor, itemColors[wxSELE_INDEX].pixel,
		   XmNforeground, itemColors[wxFORE_INDEX].pixel,
		   NULL);
  else if (change == wxFORE_COLORS)
    XtVaSetValues ((Widget) handle,
		   XmNforeground, itemColors[wxFORE_INDEX].pixel,
		   NULL);

  if (buttonBitmap)
    XtVaSetValues ((Widget) handle,
	     XmNlabelPixmap, buttonBitmap->GetLabelPixmap ((Widget) handle),
		   XmNlabelInsensitivePixmap, buttonBitmap->GetInsensPixmap ((Widget) handle),
		 XmNarmPixmap, buttonBitmap->GetArmPixmap ((Widget) handle),
		   NULL);

#endif
}

void wxButton::SetSize (int x, int y, int width, int height, int sizeFlags)
{
//  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_ANY, NULL);
  wxItem::SetSize (x, y, width, height, sizeFlags);
//  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_NONE, NULL);
#ifdef wx_motif
  if (width > -1)
    XtVaSetValues ((Widget) handle, XmNwidth, width,
		   NULL);
  if (height > -1)
    XtVaSetValues ((Widget) handle, XmNheight, height,
		   NULL);
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) handle;

  int oldLabelWidth = xv_get (item, PANEL_LABEL_WIDTH, NULL);
  Rect *rect = (Rect *) xv_get (item, XV_RECT);
  int oldWidth = rect->r_width;

  int diff = oldWidth - oldLabelWidth;

  if (width > -1)
    width = width - diff;

  if (width > -1)
  {
    xv_set (item, PANEL_LABEL_WIDTH, (int) width, NULL);
  };
#endif
  GetEventHandler()->OnSize (width, height);
}

void wxButton::SetLabel (char *label)
{
  isBitmap = FALSE;
  buttonBitmap = NULL;
#ifdef wx_motif
  Widget widget = (Widget) handle;
  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_ANY, NULL);
  if (label)
    {
      char buf[400];
      wxStripMenuCodes(label, buf);
      XmString text = XmStringCreateSimple (buf);
      XtVaSetValues (widget,
		     XmNlabelString, text,
		     XmNlabelType, XmSTRING,
		     NULL);
      XmStringFree (text);
    }
  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_NONE, NULL);
#endif
#ifdef wx_xview
  if (label)
    {
      Panel_item item = (Panel_item) handle;
      xv_set (item, PANEL_LABEL_STRING, label, NULL);
    }
#endif
}

void wxButton::SetLabel (wxBitmap * bitmap)
{
  isBitmap = TRUE;
  buttonBitmap = bitmap;
#ifdef wx_motif
  Widget widget = (Widget) handle;
  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_ANY, NULL);
  if (bitmap)
    {
      XtVaSetValues (widget,
		     XmNlabelPixmap, bitmap->GetLabelPixmap (widget),
		XmNlabelInsensitivePixmap, bitmap->GetInsensPixmap (widget),
		     XmNarmPixmap, bitmap->GetArmPixmap (widget),
		     XmNlabelType, XmPIXMAP,
		     NULL);
    }
  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_NONE, NULL);
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) handle;
  if (!bitmap->x_image)
    bitmap->CreateServerImage(TRUE);

  xv_set (item, PANEL_LABEL_IMAGE, bitmap->x_image, NULL);
#endif
}

char *wxButton::GetLabel (void)
{
#ifdef wx_motif
  Widget widget = (Widget) handle;
  XmString text;
  char *s;
  XtVaGetValues (widget,
		 XmNlabelString, &text,
		 NULL);

  if (XmStringGetLtoR (text, XmSTRING_DEFAULT_CHARSET, &s))
    {
      strcpy (wxBuffer, s);
      XtFree (s);
      return wxBuffer;
    }
  else
    {
      return NULL;
    }
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) handle;
  strcpy (wxBuffer, (char *) xv_get (item, PANEL_LABEL_STRING));
  return wxBuffer;
#endif
}

void wxButton::SetDefault (void)
{
  wxPanel *panel = (wxPanel *) GetParent ();
  if (panel)
    panel->defaultItem = this;

#ifdef wx_motif			// BUGBUG: NB CHECK FOLLOWING WORKS
  // We initially do not set XmNdefaultShadowThickness, to have small buttons.
  // Unfortunattelly, buttons are now mis-aligned. We try to correct this
  // now -- setting this ressource to 1 for each button in the same row.
  // Because it's very hard to find wxButton in the same row,
  // correction is straighforward: we set resource for all wxButton
  // in this wxPanel (but not sub panels)
  for (wxNode * node = panel->GetChildren ()->First (); node; node = node->Next ())
    {
      wxButton *item = (wxButton *) node->Data ();
      if (item->IsKindOf(CLASSINFO(wxButton)))
	{
          Bool managed = XtIsManaged(item->formWidget);
          if (managed)
            XtUnmanageChild (item->formWidget);

	  XtVaSetValues ((Widget) (item->handle),
			 XmNdefaultButtonShadowThickness, 1,
			 NULL);

          if (managed)
            XtManageChild (item->formWidget);
	}
    }				// while

//  XtVaSetValues((Widget)handle, XmNshowAsDefault, 1, NULL);
  XtVaSetValues ((Widget) panel->handle, XmNdefaultButton, (Widget) handle, NULL);
#endif
#ifdef wx_xview
  if (panel)
    {
      xv_set ((Panel) panel->handle, PANEL_DEFAULT_ITEM, (Panel_item) handle, 0);
    }
#endif
}

void wxButton::Command (wxCommandEvent & event)
{
#ifdef wx_motif
  // How do we fill in this event structure.
  XButtonEvent buttonEvent;
  XtCallActionProc ((Widget) handle, "ArmAndActivate", (XEvent *) & buttonEvent, NULL, 0);
#endif
#ifdef wx_xview
  wxbButton::Command (event);
#endif
}
