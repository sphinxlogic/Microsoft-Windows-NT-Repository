/*
 * File:        wx_messg.cc
 * Purpose:     Message item implementation (X version)
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	April 1995
 * Copyright:   (c) 1995, AIAI, University of Edinburgh
 */

#ifdef __GNUG__
#pragma implementation
#pragma implementation "wx_messg.h"
#endif

#include <stdlib.h>
#include "wx_defs.h"
#include "wx_utils.h"
#include "wx_privt.h"
#include "wx_messg.h"

#ifdef wx_motif
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/Form.h>
#endif

// Message
wxMessage::wxMessage (void)
{
  messageBitmap = NULL;
}

wxMessage::wxMessage (wxPanel * panel, char *label, int x, int y, long style, char *name):
wxbMessage (panel, label, x, y, style, name)
{
  Create (panel, label, x, y, -1, -1, style, name);
}

wxMessage::wxMessage (wxPanel * panel, char *label, int x, int y, int width, int height, long style, char *name):
wxbMessage (panel, label, x, y, style, name)
{
  Create (panel, label, x, y, width, height, style, name);
}

#if USE_BITMAP_MESSAGE
wxMessage::wxMessage(wxPanel *panel, wxBitmap *image, int x, int y, long style, char *name):
  wxbMessage(panel, image, x, y, style, name)
{
  Create(panel, image, x, y, -1, -1, style, name);
}

wxMessage::wxMessage(wxPanel *panel, wxBitmap *image, int x, int y, int width, int height, long style, char *name):
  wxbMessage(panel, image, x, y, width, height, style, name)
{
  Create(panel, image, x, y, width, height, style, name);
}
#endif

Bool wxMessage::
Create (wxPanel * panel, char *label, int x, int y, int width, int height, long style, char *name)
{
  messageBitmap = NULL;
  SetName(name);
  isBitmap = FALSE;
  if (panel)
    panel->AddChild (this);
  buttonFont = panel->buttonFont;
  labelFont = panel->labelFont;
  backColour = panel->backColour;
  labelColour = panel->labelColour;
  buttonColour = panel->buttonColour;
  window_parent = panel;
  labelPosition = panel->label_position;
  windowStyle = style;
#ifdef wx_motif
#if !USE_GADGETS
  canAddEventHandler= TRUE;
#else
  canAddEventHandler= FALSE;
#endif

  if (!label)
    label = "";
  Widget panelForm = panel->panelWidget;
  XmString text = XmStringCreateSimple (label);

  formWidget = XtVaCreateManagedWidget (windowName,
					xmFormWidgetClass, panelForm,
					XmNmarginHeight, 0,
					XmNmarginWidth, 0,
					NULL);

  labelWidget = XtVaCreateManagedWidget ("messageLabel",
#if USE_GADGETS
					 style & wxCOLOURED ?
				    xmLabelWidgetClass : xmLabelGadgetClass,
					 formWidget,
#else
					 xmLabelWidgetClass, formWidget,
#endif
					 XmNlabelString, text,
                                         XmNalignment,
                     ((style & wxALIGN_RIGHT) ? XmALIGNMENT_END :
                     ((style & wxALIGN_CENTRE) ? XmALIGNMENT_CENTER :
                     XmALIGNMENT_BEGINNING)),
					 XmNtopAttachment, XmATTACH_FORM,
					 XmNleftAttachment, XmATTACH_FORM,
					 XmNbottomAttachment, XmATTACH_FORM,
					 XmNrightAttachment, XmATTACH_FORM,
					 NULL);

  XmStringFree (text);

  wxAddWindowToTable(labelWidget, this);

  handle = (char *) labelWidget;

  if (labelFont)
    XtVaSetValues (labelWidget,
		   XmNfontList, labelFont->GetInternalFont (XtDisplay(formWidget)),
		   NULL);

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
	  PANEL_LABEL_STRING, label,
	  PANEL_CLIENT_DATA, (char *) this,
	  NULL);
/*
   if (labelFont)
   xv_set(x_message, XV_FONT, labelFont->GetInternalFont(), NULL) ;
 */

  if (x > -1 && y > -1)
    (void) xv_set (x_message, XV_X, x, XV_Y, y, NULL);

  handle = (char *) x_message;
#endif
  return TRUE;
}

#if USE_BITMAP_MESSAGE
Bool wxMessage::
Create (wxPanel * panel, wxBitmap *image, int x, int y, int width, int height, long style, char *name)
{
  messageBitmap = image;
  isBitmap = TRUE;
  SetName(name);
  if (panel)
    panel->AddChild (this);
  buttonFont = panel->buttonFont;
  labelFont = panel->labelFont;
  backColour = panel->backColour;
  labelColour = panel->labelColour;
  buttonColour = panel->buttonColour;
  window_parent = panel;
  labelPosition = panel->label_position;
  windowStyle = style;
#ifdef wx_motif
#if !USE_GADGETS
  canAddEventHandler= TRUE;
#else
  canAddEventHandler= FALSE;
#endif
  windowName = copystring (name);

  Widget panelForm = panel->panelWidget;


  formWidget = XtVaCreateManagedWidget (windowName,
					xmFormWidgetClass, panelForm,
					XmNmarginHeight, 0,
					XmNmarginWidth, 0,
					NULL);

  labelWidget = XtVaCreateManagedWidget ("messageLabel",
#if USE_GADGETS
					 style & wxCOLOURED ?
				    xmLabelWidgetClass : xmLabelGadgetClass,
					 formWidget,
#else
					 xmLabelWidgetClass, formWidget,
#endif
					 XmNalignment, XmALIGNMENT_BEGINNING,
					 XmNtopAttachment, XmATTACH_FORM,
					 XmNleftAttachment, XmATTACH_FORM,
					 XmNbottomAttachment, XmATTACH_FORM,
					 XmNrightAttachment, XmATTACH_FORM,
					 NULL);

  XtVaSetValues (labelWidget,
		 XmNlabelPixmap, image->GetLabelPixmap (labelWidget),
                 XmNlabelType, XmPIXMAP,
                 NULL);

  wxAddWindowToTable(labelWidget, this);

  handle = (char *) labelWidget;
/*
  if (labelFont)
    XtVaSetValues (labelWidget,
		   XmNfontList, labelFont->GetInternalFont (),
		   NULL);
*/
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

  if (image != NULL)
  {
    if (!image->x_image)
      image->CreateServerImage(TRUE);


    xv_set (x_message,
  	  PANEL_LABEL_IMAGE, image->x_image,
          PANEL_CLIENT_DATA, (char *) this,
	  NULL);
  }

  if (x > -1 && y > -1)
    (void) xv_set (x_message, XV_X, x, XV_Y, y, NULL);

  handle = (char *) x_message;
#endif
  return TRUE;
}
#endif

wxMessage::~wxMessage (void)
{
}

void wxMessage::ChangeColour (void)
{
#ifdef wx_motif
  int change;

  wxPanel *panel = (wxPanel *) window_parent;
  if (!(panel->GetWindowStyleFlag() & wxUSER_COLOURS))
    return;

  change = wxComputeColors (XtDisplay((Widget)formWidget), panel->GetBackgroundColour(),
			    panel->GetLabelColour());
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

  change = wxComputeColors (XtDisplay((Widget)formWidget), GetBackgroundColour(), GetLabelColour());
  if (change == wxBACK_COLORS)
    XtVaSetValues (labelWidget,
		   XmNbackground, itemColors[wxBACK_INDEX].pixel,
		   XmNtopShadowColor, itemColors[wxTOPS_INDEX].pixel,
		   XmNbottomShadowColor, itemColors[wxBOTS_INDEX].pixel,
		   XmNarmColor, itemColors[wxSELE_INDEX].pixel,
		   XmNforeground, itemColors[wxFORE_INDEX].pixel,
		   NULL);
  else if (change == wxFORE_COLORS)
    XtVaSetValues (labelWidget,
		   XmNforeground, itemColors[wxFORE_INDEX].pixel,
		   NULL);

#endif
}

#if USE_BITMAP_MESSAGE
void wxMessage::SetLabel(wxBitmap *image)
{
  messageBitmap = image;
  isBitmap = TRUE;
#ifdef wx_motif
  Widget widget = (Widget) handle;
  int x, y, w1, h1, w2, h2;

  GetPosition(&x, &y);

  w2 = image->GetWidth();
  h2 = image->GetHeight();
  if (image)
    {
      XtVaSetValues (widget,
		     XmNlabelPixmap, image->GetLabelPixmap (widget),
		     XmNlabelType, XmPIXMAP,
		     NULL);
    }
  GetSize(&w1, &h1);

  if (! (w1 == w2) && (h1 == h2))
    SetSize(x, y, w2, h2);

#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) handle;
  if (!image->x_image)
    image->CreateServerImage(TRUE);
  xv_set (item, PANEL_LABEL_IMAGE, image->x_image, NULL);
#endif
}
#endif
