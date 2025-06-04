/*
 * File:        wx_group.cc
 * Purpose:     Group item implementation (X version)
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	April 1995
 * Copyright:   (c) 1995, AIAI, University of Edinburgh
 */

#ifdef __GNUG__
#pragma implementation
#pragma implementation "wx_group.h"
#endif

#include <stdlib.h>
#include "wx_defs.h"
#include "wx_utils.h"
#include "wx_privt.h"
#include "wx_group.h"

#ifdef wx_motif
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/Form.h>
#endif

/*
 * Group box
 */
 
wxGroupBox::wxGroupBox (void)
{
}

wxGroupBox::wxGroupBox (wxPanel * panel, char *label,
	  int x, int y, int width, int height, long style, char *name):
 wxbGroupBox(panel, label, x, y, width, height, style, name)
{
  Create (panel, label, x, y, width, height, style, name);
}

Bool wxGroupBox::Create (wxPanel * panel, char *label,
	int x, int y, int width, int height, long style, char *name)
{
  SetName(name);
  if (panel)
    panel->AddChild (this);
  buttonFont = panel->buttonFont;
  labelFont = panel->labelFont;
  backColour = panel->backColour;
  labelColour = panel->labelColour;
  buttonColour = panel->buttonColour;
  window_parent = panel;
  windowStyle = style;

  SetLabelPosition(wxVERTICAL);
#ifdef wx_motif
  canAddEventHandler = TRUE;

  Widget panelForm = panel->panelWidget;

  if (!label)
    label = " ";
  char buf[400];
  wxStripMenuCodes(label, buf);

  formWidget = XtVaCreateManagedWidget (windowName,
					xmFormWidgetClass, panelForm,
					XmNmarginHeight, 0,
					XmNmarginWidth, 0,
					NULL);

  if (label)
  {
      XmString text = XmStringCreateSimple (buf);
      labelWidget = XtVaCreateManagedWidget (label,
#if USE_GADGETS
					     style & wxCOLOURED ?
				    xmLabelWidgetClass : xmLabelGadgetClass,
					     formWidget,
#else
					     xmLabelWidgetClass, formWidget,
#endif
					     XmNlabelString, text,
					     NULL);
      if (labelFont)
	XtVaSetValues (labelWidget,
		       XmNfontList, labelFont->GetInternalFont (XtDisplay(formWidget)),
		       NULL);

      XmStringFree (text);
   }

  Widget frameWidget = XtVaCreateManagedWidget ("frame",
					xmFrameWidgetClass, formWidget,
                                        XmNshadowType, XmSHADOW_IN,
//					XmNmarginHeight, 0,
//					XmNmarginWidth, 0,
					NULL);

    if (labelWidget)
      XtVaSetValues (labelWidget,
		       XmNtopAttachment, XmATTACH_FORM,
		       XmNleftAttachment, XmATTACH_FORM,
		       XmNrightAttachment, XmATTACH_FORM,
		       XmNalignment, XmALIGNMENT_BEGINNING,
		       NULL);

      XtVaSetValues (frameWidget,
	    XmNtopAttachment, labelWidget ? XmATTACH_WIDGET : XmATTACH_FORM,
		     XmNtopWidget, labelWidget ? labelWidget : formWidget,
		     XmNbottomAttachment, XmATTACH_FORM,
		     XmNleftAttachment, XmATTACH_FORM,
		     XmNrightAttachment, XmATTACH_FORM,
		     NULL);

  handle = (char *) frameWidget;
  wxAddWindowToTable(frameWidget, this);

  panel->AttachWidget (this, formWidget, x, y, width, height);
  ChangeColour ();
#endif
#ifdef wx_xview
  Panel x_panel = (Panel) (panel->GetHandle ());
  Panel_item x_message;

  if (panel->new_line)
    {
      x_message = (Panel_item) xv_create (x_panel, PANEL_MESSAGE, PANEL_LAYOUT, PANEL_HORIZONTAL, PANEL_NEXT_ROW, -1, NULL);
      panel->new_line = FALSE;
    }
  else
    x_message = (Panel_item) xv_create (x_panel, PANEL_MESSAGE, PANEL_LAYOUT, PANEL_HORIZONTAL, NULL);

  xv_set (x_message,
             PANEL_CLIENT_DATA, (char *) this,
             NULL);

  if (label)
  {
    actualLabel = wxStripMenuCodes(label);
    xv_set (x_message,
             PANEL_LABEL_STRING, actualLabel);
  }


  if (x > -1 && y > -1)
    (void) xv_set (x_message, XV_X, x, XV_Y, y, NULL);

  handle = (char *) x_message;
#endif
  return TRUE;
}


wxGroupBox::~wxGroupBox (void)
{
#ifdef wx_motif
//  wxWidgetHashTable->Delete ((long) handle);
#endif
}

void wxGroupBox::SetSize (int x, int y, int width, int height, int sizeFlags)
{

#ifdef wx_motif
  wxItem::SetSize (x, y, width, height);

  Dimension xx, yy;
  XtVaGetValues (labelWidget, XmNwidth, &xx, XmNheight, &yy, NULL);

  if (width > -1)
    XtVaSetValues ((Widget) handle, XmNwidth, width,
		   NULL);
  if (height > -1)
    XtVaSetValues ((Widget) handle, XmNheight, height - yy,
		   NULL);
  GetEventHandler()->OnSize (width, height);
#endif
#ifdef wx_xview
#endif
}

void wxGroupBox::SetLabel (char *label)
{
#ifdef wx_motif
  if (!labelWidget)
    return;

  if (label)
    {
      char buf[400];
      wxStripMenuCodes(label, buf);
      XmString text = XmStringCreateSimple (buf);
      XtVaSetValues (labelWidget,
		     XmNlabelString, text,
		     XmNlabelType, XmSTRING,
		     NULL);
      XmStringFree (text);
    }
#endif
#ifdef wx_xview
  if (label)
    { 
     char buf[400];
     wxStripMenuCodes(label, buf);
     Panel_item item = (Panel_item) handle;
      xv_set (item, PANEL_LABEL_STRING, buf, NULL);
    }
#endif
}

char *wxGroupBox::GetLabel (void)
{
#ifdef wx_motif
  if (!labelWidget)
    return NULL;

  XmString text = 0;
  char *s;
  XtVaGetValues (labelWidget,
		 XmNlabelString, &text,
		 NULL);
  if (!text)
    return NULL;

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

void wxGroupBox::ChangeColour (void)
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

  if (labelWidget)
    {
      change = wxComputeColors (XtDisplay(formWidget), panel->GetBackgroundColour(), GetLabelColour());

      if (change == wxBACK_COLORS)
	XtVaSetValues (labelWidget,
		       XmNbackground, itemColors[wxBACK_INDEX].pixel,
		       XmNtopShadowColor, itemColors[wxTOPS_INDEX].pixel,
		       XmNbottomShadowColor, itemColors[wxBOTS_INDEX].pixel,
		       XmNforeground, itemColors[wxFORE_INDEX].pixel,
		       NULL);
      else if (change == wxFORE_COLORS)
	XtVaSetValues (labelWidget,
		       XmNforeground, itemColors[wxFORE_INDEX].pixel,
		       NULL);
    }
#endif
}

