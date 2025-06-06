/*
 * File:        wx_check.cc
 * Purpose:     Checkbox implementation (X version)
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	April 1995
 * Copyright:   (c) 1995, AIAI, University of Edinburgh
 */

#ifdef __GNUG__
#pragma implementation
#pragma implementation "wx_check.h"
#endif

#include <stdlib.h>
#include "wx_defs.h"
#include "wx_utils.h"
#include "wx_privt.h"
#include "wx_check.h"

#ifdef wx_motif
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/Form.h>
#endif

#ifdef wx_xview
void wxCheckBoxProc (Panel_item item, int value, Event * x_event);
#endif

#ifdef wx_motif
void 
wxCheckBoxCallback (Widget w, XtPointer clientData,
		    XtPointer ptr)
{
  wxCheckBox *item = (wxCheckBox *) clientData;
  if (item->inSetValue)
    return;
    
  wxCommandEvent event (wxEVENT_TYPE_CHECKBOX_COMMAND);
  event.commandInt = item->GetValue ();
  event.eventObject = item;
  item->ProcessCommand (event);
}
#endif


// Single check box item

wxCheckBox::wxCheckBox (void)
{
  buttonBitmap = NULL;
}

wxCheckBox::wxCheckBox (wxPanel * panel, wxFunction func, char *Title,
	    int x, int y, int width, int height, long style, char *name):
wxbCheckBox (panel, func, Title, x, y, width, height, style, name)
{
  Create (panel, func, Title, x, y, width, height, style, name);
}

wxCheckBox::wxCheckBox (wxPanel * panel, wxFunction func, wxBitmap * bitmap,
	    int x, int y, int width, int height, long style, char *name):
wxbCheckBox (panel, func, bitmap, x, y, width, height, style, name)
{
  Create (panel, func, bitmap, x, y, width, height, style, name);
}

Bool wxCheckBox::
Create (wxPanel * panel, wxFunction func, char *Title,
	int x, int y, int width, int height, long style, char *name)
{
  if (panel)
    panel->AddChild (this);

  SetName(name);
  
  buttonFont = panel->buttonFont;
  labelFont = panel->labelFont;
  backColour = panel->backColour;
  labelColour = panel->labelColour;
  buttonColour = panel->buttonColour;
  buttonBitmap = NULL;
  window_parent = panel;
  windowStyle = style;

  SetAppropriateLabelPosition();
#ifdef wx_motif
#if !USE_GADGETS
   canAddEventHandler = TRUE;
#else
   canAddEventHandler = FALSE;
#endif

  Widget panelForm = panel->panelWidget;
  if (!Title)
    Title = "";
  char buf[400];
  wxStripMenuCodes(Title, buf);
  XmString text = XmStringCreateSimple (buf);

  formWidget = XtVaCreateManagedWidget (windowName,
					xmFormWidgetClass, panelForm,
					XmNmarginHeight, 0,
					XmNmarginWidth, 0,
					NULL);

  Widget buttonWidget = XtVaCreateManagedWidget ("toggle",
#if USE_GADGETS
						 style & wxCOLOURED ?
		      xmToggleButtonWidgetClass : xmToggleButtonGadgetClass,
						 formWidget,
#else
				      xmToggleButtonWidgetClass, formWidget,
#endif
						 XmNlabelString, text,
					    XmNtopAttachment, XmATTACH_FORM,
					   XmNleftAttachment, XmATTACH_FORM,
					 XmNbottomAttachment, XmATTACH_FORM,
					  XmNrightAttachment, XmATTACH_FORM,
						 NULL);

  handle = (char *) buttonWidget;
  
  wxAddWindowToTable(buttonWidget, this);

  if (labelFont)
    XtVaSetValues (buttonWidget,
		   XmNfontList, labelFont->GetInternalFont (XtDisplay(formWidget)),
		   NULL);

  XmStringFree (text);

  XtAddCallback (buttonWidget, XmNvalueChangedCallback, (XtCallbackProc) wxCheckBoxCallback,
		 (XtPointer) this);

  panel->AttachWidget (this, formWidget, x, y, width, height);
  ChangeColour ();

  XmToggleButtonSetState (buttonWidget, FALSE, TRUE);
  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_NONE, NULL);
#endif
#ifdef wx_xview
  Panel x_panel = (Panel) panel->GetHandle ();
  Panel_item x_choice;

  int label_position;

  if (GetLabelPosition() == wxVERTICAL)
    label_position = PANEL_VERTICAL;
  else
    label_position = PANEL_HORIZONTAL;

  if (panel->new_line)
    {
      x_choice = (Panel_item) xv_create (x_panel, PANEL_CHECK_BOX, PANEL_LAYOUT, label_position, PANEL_NEXT_ROW, -1, NULL);
      panel->new_line = FALSE;
    }
  else
    x_choice = (Panel_item) xv_create (x_panel, PANEL_CHECK_BOX, PANEL_LAYOUT, label_position, NULL);

  if (Title)
    actualLabel = wxStripMenuCodes(Title);

  xv_set (x_choice,
	  PANEL_NOTIFY_PROC, wxCheckBoxProc,
	  PANEL_CLIENT_DATA, (char *) this,
	  PANEL_VALUE, 0,
//	  PANEL_CHOICE_STRING, 0, Title,
	  NULL);
  if (actualLabel)
    xv_set(x_choice, PANEL_CHOICE_STRING, 0, actualLabel, NULL);

/*
   if (labelFont)
   xv_set(x_choice, XV_FONT, labelFont->GetInternalFont(), NULL) ;
 */
  handle = (char *) x_choice;
/*
  if (x > -1 && y > -1)
    (void) xv_set (x_choice, XV_X, x, XV_Y, y, NULL);
*/
  SetSize(x, y, -1, -1);
#endif

  Callback (func);

  return TRUE;
}

Bool wxCheckBox::
Create (wxPanel * panel, wxFunction func, wxBitmap * bitmap,
	int x, int y, int width, int height, long style, char *name)
{
  if (panel)
    panel->AddChild (this);
  SetName(name);
  
  buttonFont = panel->buttonFont;
  labelFont = panel->labelFont;
  backColour = panel->backColour;
  labelColour = panel->labelColour;
  buttonColour = panel->buttonColour;
  buttonBitmap = bitmap;
  window_parent = panel;
  windowStyle = style;
  
  SetAppropriateLabelPosition();
#ifdef wx_motif
#if !USE_GADGETS
   canAddEventHandler = TRUE;
#else
   canAddEventHandler = FALSE;
#endif

  Widget panelForm = panel->panelWidget;

  formWidget = XtVaCreateManagedWidget (windowName,
					xmFormWidgetClass, panelForm,
					XmNmarginHeight, 0,
					XmNmarginWidth, 0,
					NULL);

  Widget buttonWidget = XtVaCreateManagedWidget ("toggle",
#if USE_GADGETS
						 style & wxCOLOURED ?
		      xmToggleButtonWidgetClass : xmToggleButtonGadgetClass,
						 formWidget,
#else
				      xmToggleButtonWidgetClass, formWidget,
#endif
					    XmNtopAttachment, XmATTACH_FORM,
					   XmNleftAttachment, XmATTACH_FORM,
					 XmNbottomAttachment, XmATTACH_FORM,
					  XmNrightAttachment, XmATTACH_FORM,
						 NULL);

  handle = (char *) buttonWidget;

  if (labelFont)
    XtVaSetValues (buttonWidget,
		   XmNfontList,labelFont->GetInternalFont (XtDisplay(formWidget)),
		   NULL);

  XtVaSetValues (buttonWidget,
		 XmNlabelPixmap, bitmap->GetLabelPixmap (buttonWidget),
		 XmNselectPixmap, bitmap->GetLabelPixmap (buttonWidget),
	  XmNlabelInsensitivePixmap, bitmap->GetInsensPixmap (buttonWidget),
	 XmNselectInsensitivePixmap, bitmap->GetInsensPixmap (buttonWidget),
		 XmNarmPixmap, bitmap->GetArmPixmap (buttonWidget),
		 XmNlabelType, XmPIXMAP,
		 NULL);

  XtAddCallback (buttonWidget, XmNvalueChangedCallback, wxCheckBoxCallback,
		 this);

  panel->AttachWidget (this, formWidget, x, y, width, height);
  ChangeColour ();

  XmToggleButtonSetState (buttonWidget, FALSE, TRUE);
  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_NONE, NULL);
#endif
#ifdef wx_xview
  Panel x_panel = (Panel) panel->GetHandle ();
  Panel_item x_choice;

  int label_position;

  if (GetLabelPosition() == wxVERTICAL)
    label_position = PANEL_VERTICAL;
  else
    label_position = PANEL_HORIZONTAL;

  if (panel->new_line)
    {
      x_choice = (Panel_item) xv_create (x_panel, PANEL_CHECK_BOX, PANEL_LAYOUT, label_position, PANEL_NEXT_ROW, -1, NULL);
      panel->new_line = FALSE;
    }
  else
    x_choice = (Panel_item) xv_create (x_panel, PANEL_CHECK_BOX, PANEL_LAYOUT, label_position, NULL);

  if (!bitmap->x_image)
    bitmap->CreateServerImage(TRUE);

  xv_set (x_choice,
	  PANEL_NOTIFY_PROC, wxCheckBoxProc,
	  PANEL_CLIENT_DATA, (char *) this,
	  PANEL_VALUE, 0,
	  PANEL_CHOICE_IMAGE, 0, bitmap->x_image,
	  NULL);
/*
   if (labelFont)
   xv_set(x_choice, XV_FONT, labelFont->GetInternalFont(), NULL) ;
 */

  if (x > -1 && y > -1)
    (void) xv_set (x_choice, XV_X, x, XV_Y, y, NULL);

  handle = (char *) x_choice;
#endif

  Callback (func);

  return TRUE;
}

wxCheckBox::~wxCheckBox (void)
{
}

void wxCheckBox::ChangeColour (void)
{
#ifdef wx_motif
  int change;

  wxPanel *panel = (wxPanel *) window_parent;
  if (!(panel->GetWindowStyleFlag() & wxUSER_COLOURS))
    return;

  change = wxComputeColors (XtDisplay(formWidget), panel->GetBackgroundColour(),
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

char *wxCheckBox::GetLabel ()
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
//    XmStringFree(text);
      char *val = copystring (s);
      XtFree (s);
      return val;
    }
  else
    {
//    XmStringFree(text);
      return NULL;
    }
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) handle;
  return ((char *) xv_get (item, PANEL_CHOICE_STRING, 0));
#endif
}

void wxCheckBox::SetLabel (char *label)
{
#ifdef wx_motif
  Widget widget = (Widget) handle;
  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_ANY, NULL);
  if (label)
    {
      XmString text = XmStringCreateSimple (label);
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
      xv_set (item, PANEL_CHOICE_STRING, 0, label, NULL);
    }
#endif
}

void wxCheckBox::SetLabel (wxBitmap * bitmap)
{
  buttonBitmap = bitmap;
#ifdef wx_motif
  Widget widget = (Widget) handle;
  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_ANY, NULL);
  if (bitmap)
    {
      XtVaSetValues (widget,
		     XmNlabelPixmap, bitmap->GetLabelPixmap (widget),
		     XmNselectPixmap, bitmap->GetLabelPixmap (widget),
		XmNlabelInsensitivePixmap, bitmap->GetInsensPixmap (widget),
	       XmNselectInsensitivePixmap, bitmap->GetInsensPixmap (widget),
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

  xv_set (item, PANEL_CHOICE_IMAGE, 0, bitmap->x_image, NULL);
#endif
}

void wxCheckBox::SetValue (Bool val)
{
#ifdef wx_motif
  inSetValue = TRUE;
  Widget buttonWidget = (Widget) handle;
  XmToggleButtonSetState (buttonWidget, val, TRUE);
  inSetValue = FALSE;
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) handle;
  int the_val = 0;
  if (val)
    the_val = 1;

  xv_set (item, PANEL_VALUE, the_val, NULL);
#endif
}

Bool wxCheckBox::GetValue (void)
{
#ifdef wx_motif
  Widget buttonWidget = (Widget) handle;
  return XmToggleButtonGetState (buttonWidget);
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) handle;
  return (Bool) xv_get (item, PANEL_VALUE);
#endif
}

#ifdef wx_xview
void 
wxCheckBoxProc (Panel_item item, int value, Event * x_event)
{
  wxCheckBox *box = (wxCheckBox *) xv_get (item, PANEL_CLIENT_DATA);
  wxCommandEvent event (wxEVENT_TYPE_CHECKBOX_COMMAND);

  event.commandString = (char *) xv_get (item, PANEL_LABEL_STRING);
  event.commandInt = value;
  event.eventObject = box;
  box->ProcessCommand (event);
}
#endif
