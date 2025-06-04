/*
 * File:        wx_rbox.cc
 * Purpose:     Radio box implementation (X version)
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	April 1995
 * Copyright:   (c) 1995, AIAI, University of Edinburgh
 */

#ifdef __GNUG__
#pragma implementation
#pragma implementation "wx_rbox.h"
#endif

#include <stdlib.h>
#include "wx_defs.h"
#include "wx_utils.h"
#include "wx_privt.h"
#include "wx_rbox.h"

#ifdef wx_motif
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#endif

#ifdef wx_xview
void wxRadioBoxProc (Panel_item item, int value, Event * event);
#endif

#ifdef wx_motif
void wxRadioBoxCallback (Widget w, XtPointer clientData,
		    XmToggleButtonCallbackStruct * cbs)
{
  if (!cbs->set)
    return;

  wxRadioBox *item = (wxRadioBox *) clientData;
  int sel = -1;
  int i;
  for (i = 0; i < item->no_items; i++)
    if (item->radioButtons && (item->radioButtons[i] == w))
      sel = i;
  item->selected = sel;

  wxCommandEvent event (wxEVENT_TYPE_RADIOBOX_COMMAND);
  event.commandInt = sel;
  event.eventHandle = (char *) cbs->event;
  event.eventObject = item;

  if (item->inSetValue)
    return;

  item->ProcessCommand (event);
}

#if USE_RADIOBUTTON
void wxRadioButtonCallback (Widget w, XtPointer clientData,
		    XmToggleButtonCallbackStruct * cbs)
{
  if (!cbs->set)
    return;

  wxRadioButton *item = (wxRadioButton *) clientData;
  wxCommandEvent event (wxEVENT_TYPE_RADIOBUTTON_COMMAND);
  event.commandInt = 0; // sel;
  event.eventHandle = (char *) cbs->event;
  event.eventObject = item;

  if (item->inSetValue)
    return;

  item->ProcessCommand (event);
}
#endif
#endif

#ifdef wx_xview
void 
wxRadioBoxProc (Panel_item item, int value, Event * x_event)
{
  wxRadioBox *box = (wxRadioBox *) xv_get (item, PANEL_CLIENT_DATA);

  wxCommandEvent event (wxEVENT_TYPE_RADIOBOX_COMMAND);

  event.commandString = (char *) xv_get (item, PANEL_CHOICE_STRING, value);
  event.commandInt = value;
  event.eventHandle = (char *) x_event;
  event.eventObject = box;
  box->ProcessCommand (event);
}

#if USE_RADIOBUTTON
void wxRadioButtonProc (Panel_item item, int value, Event * x_event)
{
  wxRadioButton *box = (wxRadioButton *) xv_get (item, PANEL_CLIENT_DATA);
  wxCommandEvent event (wxEVENT_TYPE_RADIOBUTTON_COMMAND);

  event.commandString = (char *) xv_get (item, PANEL_LABEL_STRING);
  event.commandInt = value;
  event.eventObject = box;
  box->ProcessCommand (event);
}
#endif

#endif


// Radiobox item
wxRadioBox::wxRadioBox (void)
{
  selected = -1;
  no_items = 0;
  buttonBitmap = NULL;
#ifdef wx_motif
  radioButtons = NULL;
  radioButtonLabels = NULL;
  labelWidget = NULL;
#endif
#ifdef wx_xview
#endif
}

wxRadioBox::wxRadioBox (wxPanel * panel, wxFunction func,
	    char *Title,
	    int x, int y, int width, int height,
	    int N, char **Choices,
	    int majorDim, long Style, char *name):
wxbRadioBox (panel, func, Title, x, y, width, height, N, Choices,
	     majorDim, Style, name)
{
  Create (panel, func, Title, x, y, width, height, N, Choices, majorDim, Style, name);
}

wxRadioBox::wxRadioBox (wxPanel * panel, wxFunction func,
	    char *Title,
	    int x, int y, int width, int height,
	    int N, wxBitmap ** Choices,
	    int majorDim, long Style, char *name):
wxbRadioBox (panel, func, Title, x, y, width, height, N, Choices,
	     majorDim, Style, name)
{
  Create (panel, func, Title, x, y, width, height, N, Choices, majorDim, Style, name);
}

Bool wxRadioBox::
Create (wxPanel * panel, wxFunction func,
	char *Title,
	int x, int y, int width, int height,
	int N, char **Choices,
	int majorDim, long style, char *name)
{
  if (panel)
    panel->AddChild (this);

  SetName(name);
  buttonFont = panel->buttonFont;
  labelFont = panel->labelFont;
  backColour = panel->backColour;
  labelColour = panel->labelColour;
  buttonColour = panel->buttonColour;
  buttonBitmap = new wxBitmap *[N];
  int i;
  for (i = 0; i < N; i++)
    buttonBitmap[i] = NULL;
  selected = -1;
  window_parent = panel;
  no_items = N;
  noRowsOrCols = majorDim;
  if (majorDim == 0)
    majorDim = N;
  windowStyle = style;
  
  SetAppropriateLabelPosition();
#ifdef wx_motif
  canAddEventHandler = TRUE;
  Widget panelForm = panel->panelWidget;
  formWidget = XtVaCreateManagedWidget (windowName,
					xmFormWidgetClass, panelForm,
					XmNmarginHeight, 0,
					XmNmarginWidth, 0,
					NULL);

  XmString text = 0;

  if (Title)
    {
      char buf[400];
      wxStripMenuCodes(Title, buf);

      char *the_label = (style & wxFIXED_LENGTH) ? fillCopy (buf) : copystring (buf);
      text = XmStringCreateSimple (the_label);
      labelWidget = XtVaCreateManagedWidget (Title,
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
      delete[]the_label;
    }

  Arg args[3];
  //XtSetArg(args[0], XmNorientation, XmVERTICAL);
  //XtSetArg(args[1], XmNnumColumns, (GetLabelPosition() == wxHORIZONTAL) ? N : 1);

  majorDim = (N + majorDim - 1) / majorDim;
/*
  XtSetArg (args[0], XmNorientation, GetLabelPosition() == wxHORIZONTAL ?
	    XmHORIZONTAL : XmVERTICAL);
*/
  XtSetArg (args[0], XmNorientation, ((windowStyle & wxHORIZONTAL) == wxHORIZONTAL ?
  	                                XmHORIZONTAL : XmVERTICAL));
  XtSetArg (args[1], XmNnumColumns, majorDim);

  Widget radioBoxWidget = XmCreateRadioBox (formWidget, "radioBoxWidget", args, 2);

  handle = (char *) radioBoxWidget;
  wxAddWindowToTable(radioBoxWidget, this);

  if (GetLabelPosition() == wxHORIZONTAL)
    {
      if (labelWidget)
	XtVaSetValues (labelWidget,
		       XmNtopAttachment, XmATTACH_FORM,
		       XmNleftAttachment, XmATTACH_FORM,
		       XmNbottomAttachment, XmATTACH_FORM,
		       XmNalignment, XmALIGNMENT_BEGINNING,
		       NULL);
      XtVaSetValues (radioBoxWidget,
		     XmNleftOffset, 4,
		     XmNtopAttachment, XmATTACH_FORM,
		     XmNbottomAttachment, XmATTACH_FORM,
	   XmNleftAttachment, labelWidget ? XmATTACH_WIDGET : XmATTACH_FORM,
		     XmNleftWidget, labelWidget ? labelWidget : formWidget,
		     NULL);
    }
  else
    {
      if (labelWidget)
	XtVaSetValues (labelWidget,
		       XmNtopAttachment, XmATTACH_FORM,
		       XmNleftAttachment, XmATTACH_FORM,
		       XmNalignment, XmALIGNMENT_BEGINNING,
		       NULL);

      XtVaSetValues (radioBoxWidget,
	    XmNtopAttachment, labelWidget ? XmATTACH_WIDGET : XmATTACH_FORM,
		     XmNtopWidget, labelWidget ? labelWidget : formWidget,
		     XmNbottomAttachment, XmATTACH_FORM,
		     XmNleftAttachment, XmATTACH_FORM,
		     NULL);
    }

  if (style & wxFLAT)
    XtVaSetValues (radioBoxWidget, XmNborderWidth, 1, NULL);

  radioButtons = new Widget[N];
  radioButtonLabels = new char *[N];
  for (i = 0; i < N; i++)
    {
      radioButtonLabels[i] = copystring (Choices[i]);
      radioButtons[i] = XtVaCreateManagedWidget (Choices[i],
#if USE_GADGETS
						 style & wxCOLOURED ?
		      xmToggleButtonWidgetClass : xmToggleButtonGadgetClass,
						 radioBoxWidget,
#else
				  xmToggleButtonWidgetClass, radioBoxWidget,
#endif
						 NULL);
      XtAddCallback (radioButtons[i], XmNvalueChangedCallback, (XtCallbackProc) wxRadioBoxCallback,
		     (XtCallbackProc) this);

      if (buttonFont)
	XtVaSetValues (radioButtons[i],
		       XmNfontList, buttonFont->GetInternalFont (XtDisplay(formWidget)),
		       NULL);
    }
  SetSelection (0);

  XtManageChild (radioBoxWidget);

  if (width == -1)
    width = 150;
  if (height == -1)
    height = 80;

  panel->AttachWidget (this, formWidget, x, y, width, height);
  ChangeColour ();

  /* After creating widgets, no more resizes. */
  if (style & wxFIXED_LENGTH)
    {
      if (labelWidget)
	XtVaSetValues (labelWidget,
		       XmNtopAttachment, XmATTACH_SELF,
		       XmNleftAttachment, XmATTACH_SELF,
                       // Added on advice from Klaus Guenther
  		       XmNbottomAttachment, XmATTACH_SELF,
		       XmNrightAttachment, XmATTACH_SELF,
		       NULL);

      XtVaSetValues (radioBoxWidget,
		     XmNtopAttachment, XmATTACH_SELF,
		     XmNbottomAttachment, XmATTACH_SELF,
		     XmNleftAttachment, XmATTACH_SELF,
		     XmNrightAttachment, XmATTACH_SELF,
		     NULL);
      if (labelWidget)
	{
	  XmString text = XmStringCreateSimple (Title);
	  XtVaSetValues (labelWidget,
			 XmNlabelString, text,
			 NULL);
	  XmStringFree (text);
	}
    }
#endif
#ifdef wx_xview
  int _x = x;
  int _y = y;
  int _w = width;
  int _h = height;

  Panel x_panel = (Panel) panel->GetHandle ();
  Panel_item x_choice;

  int label_position;
  if (GetLabelPosition() == wxVERTICAL)
    label_position = PANEL_VERTICAL;
  else
    label_position = PANEL_HORIZONTAL;

  if (panel->new_line)
    {
      x_choice = (Panel_item) xv_create (x_panel, PANEL_CHOICE, PANEL_LAYOUT, label_position, PANEL_NEXT_ROW, -1, NULL);
      panel->new_line = FALSE;
    }
  else
    x_choice = (Panel_item) xv_create (x_panel, PANEL_CHOICE, PANEL_LAYOUT, label_position, NULL);

/*
   if (buttonFont)
   xv_set(x_choice, XV_FONT, buttonFont->GetInternalFont(), NULL) ;
 */
  xv_set (x_choice,
	  PANEL_NOTIFY_PROC, wxRadioBoxProc,
	  PANEL_CLIENT_DATA, (char *) this,
	  NULL);

  // Shouldn't we set PANEL_CHOICE_NROWS if windowStyle is wxVERTICAL?
/*
  if (majorDim != N)
    {
      majorDim = (N + majorDim - 1) / majorDim;
      xv_set (x_choice,
	      PANEL_CHOICE_NCOLS, majorDim,
	      NULL);
    }
*/
  if (windowStyle & wxVERTICAL)
    {
      if (majorDim > 0)
        xv_set (x_choice,
	      PANEL_CHOICE_NROWS, majorDim,
	      NULL);
    }
    else
    {
      if (majorDim > 0)
        xv_set (x_choice,
	      PANEL_CHOICE_NCOLS, majorDim,
	      NULL);
    }

  if (x > -1 && y > -1)
    (void) xv_set (x_choice, XV_X, x, XV_Y, y, NULL);

  for (i = 0; i < N; i++)
    {
      char *label = Choices[i];
      xv_set (x_choice, PANEL_CHOICE_STRING, i, label, NULL);
    }
  handle = (char *) x_choice;
  xv_set (x_choice, PANEL_VALUE, 0, NULL);

  if (Title)
  {
    actualLabel = wxStripMenuCodes(Title);

    if (style & wxFIXED_LENGTH)
    {
      char *the_label = fillCopy (actualLabel);
      xv_set (x_choice, PANEL_LABEL_STRING, the_label, NULL);
      int label_x = (int) xv_get (x_choice, PANEL_LABEL_X);
      int item_x = (int) xv_get (x_choice, PANEL_ITEM_X);
      xv_set (x_choice, PANEL_LABEL_STRING, actualLabel,
  	      PANEL_LABEL_X, label_x,
  	      PANEL_ITEM_X, item_x,
	      NULL);
      delete[] the_label;
    }
    else
      xv_set (x_choice, PANEL_LABEL_STRING, actualLabel, NULL);
  }
//TOMS ADDITION now we need to make sure that what's constructed here
//is the same as what SetSize return. So easiest thing is to add:
  SetSize( _x , _y , _w , _h );

#endif

  Callback (func);
  return TRUE;
}

Bool wxRadioBox::
Create (wxPanel * panel, wxFunction func,
	char *Title,
	int x, int y, int width, int height,
	int N, wxBitmap ** Choices,
	int majorDim, long style, char *name)
{
  if (panel)
    panel->AddChild (this);
  SetName(name);
  buttonFont = panel->buttonFont;
  labelFont = panel->labelFont;
  backColour = panel->backColour;
  labelColour = panel->labelColour;
  buttonColour = panel->buttonColour;
  selected = -1;
  window_parent = panel;
  no_items = N;
  noRowsOrCols = majorDim;
  if (majorDim == 0)
    majorDim = N;
  buttonBitmap = new wxBitmap *[N];
  int i;
  for (i = 0; i < N; i++)
    buttonBitmap[i] = Choices[i];
  windowStyle = style;
  
  SetAppropriateLabelPosition();
#ifdef wx_motif
  canAddEventHandler = TRUE;
  Widget panelForm = panel->panelWidget;
  formWidget = XtVaCreateManagedWidget (windowName,
					xmFormWidgetClass, panelForm,
					XmNmarginHeight, 0,
					XmNmarginWidth, 0,
					NULL);

  if (Title)
    {
      char *the_label = (style & wxFIXED_LENGTH) ? fillCopy (Title) : copystring (Title);
      XmString text = XmStringCreateSimple (the_label);
      labelWidget = XtVaCreateManagedWidget (Title,
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
      delete[]the_label;
    }

  Arg args[3];
  //XtSetArg(args[0], XmNorientation, XmVERTICAL);
  //XtSetArg(args[1], XmNnumColumns, (GetLabelPosition() == wxHORIZONTAL) ? N : 1);

  majorDim = (N + majorDim - 1) / majorDim;
/*
  XtSetArg (args[0], XmNorientation, GetLabelPosition() == wxHORIZONTAL ?
	    XmHORIZONTAL : XmVERTICAL);
*/
  XtSetArg (args[0], XmNorientation, ((windowStyle & wxHORIZONTAL) == wxHORIZONTAL ?
  	                                XmHORIZONTAL : XmVERTICAL));

  XtSetArg (args[1], XmNnumColumns, majorDim);

  Widget radioBoxWidget = XmCreateRadioBox (formWidget, "radioBoxWidget", args, 2);

  handle = (char *) radioBoxWidget;
  wxAddWindowToTable(radioBoxWidget, this);

  if (GetLabelPosition() == wxHORIZONTAL)
    {
      if (labelWidget)
	XtVaSetValues (labelWidget,
		       XmNtopAttachment, XmATTACH_FORM,
		       XmNleftAttachment, XmATTACH_FORM,
		       XmNbottomAttachment, XmATTACH_FORM,
		       XmNalignment, XmALIGNMENT_BEGINNING,
		       NULL);
      XtVaSetValues (radioBoxWidget,
		     XmNleftOffset, 4,
		     XmNtopAttachment, XmATTACH_FORM,
		     XmNbottomAttachment, XmATTACH_FORM,
	   XmNleftAttachment, labelWidget ? XmATTACH_WIDGET : XmATTACH_FORM,
		     XmNleftWidget, labelWidget ? labelWidget : formWidget,
		     NULL);
    }
  else
    {
      if (labelWidget)
	XtVaSetValues (labelWidget,
		       XmNtopAttachment, XmATTACH_FORM,
		       XmNleftAttachment, XmATTACH_FORM,
		       XmNalignment, XmALIGNMENT_BEGINNING,
		       NULL);

      XtVaSetValues (radioBoxWidget,
	    XmNtopAttachment, labelWidget ? XmATTACH_WIDGET : XmATTACH_FORM,
		     XmNtopWidget, labelWidget ? labelWidget : formWidget,
		     XmNbottomAttachment, XmATTACH_FORM,
		     XmNleftAttachment, XmATTACH_FORM,
		     NULL);
    }

  if (style & wxFLAT)
    XtVaSetValues (radioBoxWidget, XmNborderWidth, 1, NULL);

  radioButtons = new Widget[N];
  radioButtonLabels = new char *[N];
  for (i = 0; i < N; i++)
    {
      char tmp[20];
      sprintf (tmp, "Toggle%d", i);
      radioButtonLabels[i] = copystring (tmp);
      radioButtons[i] = XtVaCreateManagedWidget (tmp,
#if USE_GADGETS
						 style & wxCOLOURED ?
		      xmToggleButtonWidgetClass : xmToggleButtonGadgetClass,
						 radioBoxWidget,
#else
				  xmToggleButtonWidgetClass, radioBoxWidget,
#endif
						 NULL);
      Widget buttonWidget = radioButtons[i];
      XtVaSetValues (buttonWidget,
		  XmNlabelPixmap, Choices[i]->GetLabelPixmap (buttonWidget),
		 XmNselectPixmap, Choices[i]->GetLabelPixmap (buttonWidget),
      XmNlabelInsensitivePixmap, Choices[i]->GetInsensPixmap (buttonWidget),
      XmNselectInsensitivePixmap, Choices[i]->GetInsensPixmap (buttonWidget),
		     XmNarmPixmap, Choices[i]->GetArmPixmap (buttonWidget),
		     XmNlabelType, XmPIXMAP,
		     NULL);

      XtAddCallback (buttonWidget, XmNvalueChangedCallback, (XtCallbackProc) wxRadioBoxCallback, (XtPointer) this);
      if (buttonFont)
	XtVaSetValues (buttonWidget,
		       XmNfontList, buttonFont->GetInternalFont (XtDisplay(formWidget)),
		       NULL);
    }
  SetSelection (0);

  XtManageChild (radioBoxWidget);

  if (width == -1)
    width = 150;
  if (height == -1)
    height = 80;

  panel->AttachWidget (this, formWidget, x, y, width, height);
  ChangeColour ();


  /* After creating widgets, no more resizes. */
  if (style & wxFIXED_LENGTH)
    {
      if (labelWidget)
	XtVaSetValues (labelWidget,
		       XmNtopAttachment, XmATTACH_SELF,
		       XmNleftAttachment, XmATTACH_SELF,
                       // Added on advice from Klaus Guenther
  		       XmNbottomAttachment, XmATTACH_SELF,
		       XmNrightAttachment, XmATTACH_SELF,
		       NULL);

      XtVaSetValues (radioBoxWidget,
		     XmNtopAttachment, XmATTACH_SELF,
		     XmNbottomAttachment, XmATTACH_SELF,
		     XmNleftAttachment, XmATTACH_SELF,
		     XmNrightAttachment, XmATTACH_SELF,
		     NULL);
      if (labelWidget)
	{
	  XmString text = XmStringCreateSimple (Title);
	  XtVaSetValues (labelWidget,
			 XmNlabelString, text,
			 NULL);
	  XmStringFree (text);
	}
    }
#endif
#ifdef wx_xview
  int _x = x;
  int _y = y;
  int _w = width;
  int _h = height;

  char *title = NULL;
  if (Title)
    title = Title;

  Panel x_panel = (Panel) panel->GetHandle ();
  Panel_item x_choice;

  int label_position;
  if (GetLabelPosition() == wxVERTICAL)
    label_position = PANEL_VERTICAL;
  else
    label_position = PANEL_HORIZONTAL;

  if (panel->new_line)
    {
      x_choice = (Panel_item) xv_create (x_panel, PANEL_CHOICE, PANEL_LAYOUT, label_position, PANEL_NEXT_ROW, -1, NULL);
      panel->new_line = FALSE;
    }
  else
    x_choice = (Panel_item) xv_create (x_panel, PANEL_CHOICE, PANEL_LAYOUT, label_position, NULL);

/*
   if (buttonFont)
   xv_set(x_choice, XV_FONT, buttonFont->GetInternalFont(), NULL) ;
 */
  xv_set (x_choice,
	  PANEL_LABEL_STRING, title,
	  PANEL_NOTIFY_PROC, wxRadioBoxProc,
	  PANEL_CLIENT_DATA, (char *) this,
	  NULL);
/*
  if (majorDim != N)
    {
      majorDim = (N + majorDim - 1) / majorDim;
      xv_set (x_choice,
	      PANEL_CHOICE_NCOLS, majorDim,
	      NULL);
    }
*/
  if (windowStyle & wxVERTICAL)
    {
      if (majorDim > 0)
        xv_set (x_choice,
	      PANEL_CHOICE_NROWS, majorDim,
	      NULL);
    }
    else
    {
      if (majorDim > 0)
        xv_set (x_choice,
	      PANEL_CHOICE_NCOLS, majorDim,
	      NULL);
    }

  if (x > -1 && y > -1)
    (void) xv_set (x_choice, XV_X, x, XV_Y, y, NULL);

  for (i = 0; i < N; i++)
    {
      wxBitmap *bitmap = Choices[i];
      if (!bitmap->x_image)
        bitmap->CreateServerImage(TRUE);

      xv_set (x_choice, PANEL_CHOICE_IMAGE, i, bitmap->x_image, NULL);
    }
  handle = (char *) x_choice;
  xv_set (x_choice, PANEL_VALUE, 0, NULL);
//TOMS ADDITION now we need to make sure that what's constructed here
//is the same as what SetSize return. So easiest thing is to add:
  SetSize( _x , _y , _w , _h );
#endif

  Callback (func);
  return TRUE;
}

wxRadioBox::~wxRadioBox (void)
{
#ifdef wx_motif
  if (radioButtons)
    delete[]radioButtons;
  if (radioButtonLabels)
    {
      int i;
      for (i = 0; i < no_items; i++)
	delete[]radioButtonLabels[i];
      delete[]radioButtonLabels;
    }
#endif
  if (buttonBitmap)
    delete[]buttonBitmap;
}

void wxRadioBox::ChangeColour (void)
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
    {
      XtVaSetValues ((Widget) handle,
		     XmNbackground, itemColors[wxBACK_INDEX].pixel,
		     XmNtopShadowColor, itemColors[wxTOPS_INDEX].pixel,
		     XmNbottomShadowColor, itemColors[wxBOTS_INDEX].pixel,
		     XmNarmColor, itemColors[wxSELE_INDEX].pixel,
		     XmNforeground, itemColors[wxFORE_INDEX].pixel,
		     XmNborderColor, itemColors[wxFORE_INDEX].pixel,
		     NULL);
      int i;
      for (i = 0; i < no_items; i++)
	XtVaSetValues (radioButtons[i],
		       XmNbackground, itemColors[wxBACK_INDEX].pixel,
		       XmNtopShadowColor, itemColors[wxTOPS_INDEX].pixel,
		       XmNbottomShadowColor, itemColors[wxBOTS_INDEX].pixel,
		       XmNarmColor, itemColors[wxSELE_INDEX].pixel,
		       XmNforeground, itemColors[wxFORE_INDEX].pixel,
		       NULL);
    }
  else if (change == wxFORE_COLORS)
    {
      XtVaSetValues ((Widget) handle,
		     XmNforeground, itemColors[wxFORE_INDEX].pixel,
		     XmNborderColor, itemColors[wxFORE_INDEX].pixel,
		     NULL);
      int i;
      for (i = 0; i < no_items; i++)
	XtVaSetValues (radioButtons[i],
		       XmNforeground, itemColors[wxFORE_INDEX].pixel,
		       NULL);
    }

  if (labelWidget)
    {
      change = wxComputeColors (XtDisplay(formWidget), panel->GetBackgroundColour(), GetLabelColour());
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
    }
  int i;
  if (buttonBitmap)
    for (i = 0; i < no_items; i++)
      if (buttonBitmap[i])
	XtVaSetValues (radioButtons[i],
		       XmNlabelPixmap,
		       buttonBitmap[i]->GetLabelPixmap (radioButtons[i]),
		       XmNlabelInsensitivePixmap,
		       buttonBitmap[i]->GetInsensPixmap (radioButtons[i]),
		       XmNarmPixmap,
		       buttonBitmap[i]->GetArmPixmap (radioButtons[i]),
		       NULL);

#endif
}

char *wxRadioBox::GetLabel (int item)
{
  if (item < 0 || item >= no_items)
    return NULL;
#ifdef wx_motif
  Widget widget = (Widget) radioButtons[item];
  XmString text;
  char *s;
  XtVaGetValues (widget,
		 XmNlabelString, &text,
		 NULL);

  if (XmStringGetLtoR (text, XmSTRING_DEFAULT_CHARSET, &s))
    {
      // Should we free 'text'???
      XmStringFree(text);
      strcpy(wxBuffer, s);
      XtFree (s);
      return wxBuffer;
    }
  else
    {
      XmStringFree(text);
      return NULL;
    }
#endif
#ifdef wx_xview
  Panel_item x_choice = (Panel_item) handle;
  return ((char *) xv_get (x_choice, PANEL_CHOICE_STRING, item));
#endif
}

void wxRadioBox::SetLabel (int item, char *label)
{
  if (item < 0 || item >= no_items)
    return;
#ifdef wx_motif
  Widget widget = (Widget) radioButtons[item];
  if (label)
    {
      XmString text = XmStringCreateSimple (label);
      XtVaSetValues (widget,
		     XmNlabelString, text,
		     XmNlabelType, XmSTRING,
		     NULL);
      XmStringFree (text);
    }
#endif
#ifdef wx_xview
  Panel_item x_choice = (Panel_item) handle;
  if (label)
    xv_set (x_choice, PANEL_CHOICE_STRING, item, label, NULL);
#endif
}

void wxRadioBox::SetLabel (int item, wxBitmap * bitmap)
{
  if (item < 0 || item >= no_items)
    return;
  buttonBitmap[item] = bitmap;
#ifdef wx_motif
  Widget widget = (Widget) radioButtons[item];
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
#endif
#ifdef wx_xview
  Panel_item x_choice = (Panel_item) handle;
  if (bitmap)
  {
    if (!bitmap->x_image)
      bitmap->CreateServerImage(TRUE);

    xv_set (x_choice, PANEL_CHOICE_IMAGE, item, bitmap->x_image, NULL);
  }
#endif
}

int wxRadioBox::FindString (char *s)
{
#ifdef wx_motif
  int i;
  for (i = 0; i < no_items; i++)
    if (strcmp (s, radioButtonLabels[i]) == 0)
      return i;
  return -1;
#endif
#ifdef wx_xview
  Panel_item choice = (Panel_item) handle;

  int max1 = no_items;

  int i = 0;
  int found = -1;
  while (found == -1 && i < max1)
    {
      char *label = (char *) xv_get (choice, PANEL_CHOICE_STRING, i);
      if (label && strcmp (label, s) == 0)
	found = i;
      else
	i++;
    }
  return found;
#endif
}

void wxRadioBox::SetSelection (int N)
{
  if ((N < 0) || (N >= no_items))
    return;

#ifdef wx_motif
  inSetValue = TRUE;
  selected = N;
  XmToggleButtonSetState (radioButtons[N], TRUE, FALSE);
  int i;
  for (i = 0; i < no_items; i++)
    if (i != N)
      XmToggleButtonSetState (radioButtons[i], FALSE, FALSE);
  inSetValue = FALSE;
#endif
#ifdef wx_xview
  Panel_item choice_item = (Panel_item) handle;
  xv_set (choice_item, PANEL_VALUE, N, NULL);
#endif
}

// Get selection
int wxRadioBox::GetSelection (void)
{
#ifdef wx_motif
  return selected;
#endif
#ifdef wx_xview
  return xv_get ((Panel_item) handle, PANEL_VALUE, NULL);
#endif
}

// Find string for position
char *wxRadioBox::GetString (int N)
{
  if ((N < 0) || (N >= no_items))
    return NULL;

#ifdef wx_motif

  return radioButtonLabels[N];
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) handle;
  return (char *) xv_get (item, PANEL_CHOICE_STRING, N);
#endif
}

void wxRadioBox::SetSize (int x, int y, int width, int height, int sizeFlags)
{
#ifdef wx_motif
  Widget widget = (Widget) handle;

  Bool managed = XtIsManaged(formWidget);

  if (managed)
    XtUnmanageChild (formWidget);

  if (x > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    XtVaSetValues (formWidget, XmNleftAttachment, XmATTACH_SELF,
		   XmNx, x, NULL);
  if (y > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    XtVaSetValues (formWidget, XmNtopAttachment, XmATTACH_SELF,
		   XmNy, y, NULL);

  // Must set the actual RadioBox to be desired size MINUS label size
  Dimension labelWidth = 0, labelHeight = 0, actualWidth = 0, actualHeight = 0;

  if (labelWidget)
    XtVaGetValues (labelWidget, XmNwidth, &labelWidth, XmNheight, &labelHeight, NULL);

  if (GetLabelPosition() == wxHORIZONTAL)
    {
      actualWidth = width - labelWidth;
      actualHeight = height;
    }
  else
    {
      actualWidth = width;
      actualHeight = height - labelHeight;
    }

  if (width > -1)
    {
      XtVaSetValues (widget, XmNwidth, actualWidth, NULL);
    }
  if (height > -1)
    {
      XtVaSetValues (widget, XmNheight, actualHeight, NULL);
    }
  if (managed)
    XtManageChild (formWidget);

  GetEventHandler()->OnSize (width, height);
#endif
#ifdef wx_xview
  wxItem::SetSize (x, y, width, height, sizeFlags);
#endif
}

#ifdef wx_motif
void wxRadioBox::Enable (Bool enable)
{
  int i;
  for (i = 0; i < no_items; i++)
    XtSetSensitive (radioButtons[i], enable);
}
#endif

void wxRadioBox::Enable (int item, Bool enable)
{
#ifdef wx_motif
  if (item < 0)
    wxRadioBox::Enable (enable);
  else if (item < no_items)
    XtSetSensitive (radioButtons[item], enable);
#endif
#ifdef wx_xview
  wxWindow::Enable (enable);
#endif
}

void wxRadioBox::Show (int item, Bool show)
{
  // This method isn't complete, and we try do do our best...
  // It's main purpose isn't for allowing Show/Unshow dynamically,
  // but rather to provide a way to design wxRadioBox such:
  //
  //        o Val1  o Val2   o Val3 
  //        o Val4           o Val6 
  //        o Val7  o Val8   o Val9 
  //
  // In my case, this is a 'direction' box, and the Show(5,False) is
  // coupled with an Enable(5,False)
  //
#ifdef wx_motif
  if (item < 0)
    wxItem::Show (show);
  else if (item < no_items)
    {
      XtVaSetValues (radioButtons[item],
		     XmNindicatorOn, (unsigned char) show,
		     NULL);
      // Please note that this is all we can do: removing the label
      // if switching to unshow state. However, when switching
      // to the on state, it's the prog. resp. to call SetLabel(item,...)
      // after this call!!
      if (!show)
        wxRadioBox::SetLabel (item, " ");
    }
#endif
#ifdef wx_xview
  //No way...
#endif
}

#if USE_RADIOBUTTON
/*
 * Radio button
 */

wxRadioButton::wxRadioButton(void)
{
  theButtonBitmap = NULL;
}

wxRadioButton::wxRadioButton (wxPanel * panel, wxFunction func,
	    Const char *label, Bool value,
	    int x, int y, int width, int height,
	    long Style, Constdata char *name):
wxbRadioButton (panel, func, label, value, x, y, width, height,
	     Style, name)
{
  Create (panel, func, label, value, x, y, width, height, Style, name);
}

wxRadioButton::wxRadioButton (wxPanel * panel, wxFunction func,
	    wxBitmap *label, Bool value,
	    int x, int y, int width, int height,
	    long Style, Constdata char *name):
wxbRadioButton (panel, func, label, value, x, y, width, height,
	     Style, name)
{
  Create (panel, func, label, value, x, y, width, height, Style, name);
}

Bool wxRadioButton::
Create (wxPanel * panel, wxFunction func,
	char *label, Bool value,
	int x, int y, int width, int height,
	long style, char *name)
{
  if (panel)
    panel->AddChild (this);

  SetName(name);
  buttonFont = panel->buttonFont;
  labelFont = panel->labelFont;
  backColour = panel->backColour;
  labelColour = panel->labelColour;
  buttonColour = panel->buttonColour;
  theButtonBitmap = NULL;

  window_parent = panel;
  windowStyle = style;
  
  SetAppropriateLabelPosition();
#ifdef wx_motif
  canAddEventHandler = TRUE;
  Widget panelForm = panel->panelWidget;
  formWidget = XtVaCreateManagedWidget (windowName,
					xmFormWidgetClass, panelForm,
					XmNmarginHeight, 0,
					XmNmarginWidth, 0,
					NULL);

  XmString text = 0;

  if (label)
    {
      char buf[400];
      wxStripMenuCodes(label, buf);

      char *the_label = (style & wxFIXED_LENGTH) ? fillCopy (buf) : copystring (buf);
      text = XmStringCreateSimple (the_label);
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
      delete[]the_label;
    }

  Widget radioButtonWidget = XtVaCreateManagedWidget ("toggle",
#if USE_GADGETS
						 style & wxCOLOURED ?
		      xmToggleButtonWidgetClass : xmToggleButtonGadgetClass,
						 formWidget,
#else
				  xmToggleButtonWidgetClass, formWidget,
#endif
						 NULL);
  XtAddCallback (radioButtonWidget, XmNvalueChangedCallback, (XtCallbackProc) wxRadioButtonCallback,
		     (XtCallbackProc) this);

  if (labelFont)
   XtVaSetValues (radioButtonWidget,
                  XmNfontList, labelFont->GetInternalFont (XtDisplay(formWidget)),
                  NULL);

  handle = (char *) radioButtonWidget;
  wxAddWindowToTable(radioButtonWidget, this);

//  if (GetLabelPosition() == wxHORIZONTAL)
    {
      if (labelWidget)
	XtVaSetValues (labelWidget,
		       XmNtopAttachment, XmATTACH_FORM,
		       XmNleftAttachment, XmATTACH_FORM,
		       XmNbottomAttachment, XmATTACH_FORM,
		       XmNalignment, XmALIGNMENT_BEGINNING,
		       NULL);
      XtVaSetValues (radioButtonWidget,
		     XmNleftOffset, 4,
		     XmNtopAttachment, XmATTACH_FORM,
		     XmNbottomAttachment, XmATTACH_FORM,
	   XmNleftAttachment, labelWidget ? XmATTACH_WIDGET : XmATTACH_FORM,
		     XmNleftWidget, labelWidget ? labelWidget : formWidget,
		     NULL);
    }
/*
  else
    {
      if (labelWidget)
	XtVaSetValues (labelWidget,
		       XmNtopAttachment, XmATTACH_FORM,
		       XmNleftAttachment, XmATTACH_FORM,
		       XmNalignment, XmALIGNMENT_BEGINNING,
		       NULL);

      XtVaSetValues (radioButtonWidget,
	    XmNtopAttachment, labelWidget ? XmATTACH_WIDGET : XmATTACH_FORM,
		     XmNtopWidget, labelWidget ? labelWidget : formWidget,
		     XmNbottomAttachment, XmATTACH_FORM,
		     XmNleftAttachment, XmATTACH_FORM,
		     NULL);
    }

*/

  SetValue (value);

  XtManageChild (radioButtonWidget);

  if (width == -1)
    width = 100;
  if (height == -1)
    height = 30;

  panel->AttachWidget (this, formWidget, x, y, width, height);
  ChangeColour ();

  /* After creating widgets, no more resizes. */
  if (style & wxFIXED_LENGTH)
    {
      if (labelWidget)
	XtVaSetValues (labelWidget,
		       XmNtopAttachment, XmATTACH_SELF,
		       XmNleftAttachment, XmATTACH_SELF,
                       // Added on advice from Klaus Guenther
  		       XmNbottomAttachment, XmATTACH_SELF,
		       XmNrightAttachment, XmATTACH_SELF,
		       NULL);

      XtVaSetValues (radioButtonWidget,
		     XmNtopAttachment, XmATTACH_SELF,
		     XmNbottomAttachment, XmATTACH_SELF,
		     XmNleftAttachment, XmATTACH_SELF,
		     XmNrightAttachment, XmATTACH_SELF,
		     NULL);
      if (labelWidget)
	{
	  XmString text = XmStringCreateSimple (label);
	  XtVaSetValues (labelWidget,
			 XmNlabelString, text,
			 NULL);
	  XmStringFree (text);
	}
    }
#endif
#ifdef wx_xview
  Panel x_panel = (Panel) panel->GetHandle ();
  Panel_item x_choice;

  int label_position;

//  if (GetLabelPosition() == wxVERTICAL)
//    label_position = PANEL_VERTICAL;
//  else
    label_position = PANEL_HORIZONTAL;

  if (panel->new_line)
    {
      x_choice = (Panel_item) xv_create (x_panel, PANEL_CHECK_BOX, PANEL_LAYOUT, label_position, PANEL_NEXT_ROW, -1, NULL);
      panel->new_line = FALSE;
    }
  else
    x_choice = (Panel_item) xv_create (x_panel, PANEL_CHECK_BOX, PANEL_LAYOUT, label_position, NULL);

  if (label)
    actualLabel = wxStripMenuCodes(label);

  xv_set (x_choice,
	  PANEL_NOTIFY_PROC, wxRadioButtonProc,
	  PANEL_CLIENT_DATA, (char *) this,
	  PANEL_VALUE, 0,
	  NULL);
  if (actualLabel)
    xv_set(x_choice, PANEL_CHOICE_STRING, 0, actualLabel, NULL);

  handle = (char *) x_choice;
  SetSize(x, y, -1, -1);
#endif

  Callback (func);
  return TRUE;
}

Bool wxRadioButton::
Create (wxPanel * panel, wxFunction func,
	wxBitmap *label, Bool value,
	int x, int y, int width, int height,
	long style, char *name)
{
  if (panel)
    panel->AddChild (this);
  SetName(name);
  buttonFont = panel->buttonFont;
  labelFont = panel->labelFont;
  backColour = panel->backColour;
  labelColour = panel->labelColour;
  buttonColour = panel->buttonColour;
  window_parent = panel;
  theButtonBitmap = label;
  windowStyle = style;
  
  SetAppropriateLabelPosition();
#ifdef wx_motif
  canAddEventHandler = TRUE;
  Widget panelForm = panel->panelWidget;
  formWidget = XtVaCreateManagedWidget (windowName,
					xmFormWidgetClass, panelForm,
					XmNmarginHeight, 0,
					XmNmarginWidth, 0,
					NULL);

  Widget radioButtonWidget = XtVaCreateManagedWidget ("toggle",
#if USE_GADGETS
						 style & wxCOLOURED ?
		      xmToggleButtonWidgetClass : xmToggleButtonGadgetClass,
						 formWidget,
#else
				  xmToggleButtonWidgetClass, radioButtonWidget,
#endif
					    XmNtopAttachment, XmATTACH_FORM,
					   XmNleftAttachment, XmATTACH_FORM,
					 XmNbottomAttachment, XmATTACH_FORM,
					  XmNrightAttachment, XmATTACH_FORM,
						 NULL);
  XtVaSetValues (radioButtonWidget,
         XmNlabelPixmap, label->GetLabelPixmap (radioButtonWidget),
	 XmNselectPixmap, label->GetLabelPixmap (radioButtonWidget),
         XmNlabelInsensitivePixmap, label->GetInsensPixmap (radioButtonWidget),
         XmNselectInsensitivePixmap, label->GetInsensPixmap (radioButtonWidget),
         XmNarmPixmap, label->GetArmPixmap (radioButtonWidget),
	 XmNlabelType, XmPIXMAP,
	 NULL);

  XtAddCallback (radioButtonWidget, XmNvalueChangedCallback, (XtCallbackProc) wxRadioButtonCallback, (XtPointer) this);
  if (buttonFont)
   XtVaSetValues (radioButtonWidget,
	          XmNfontList, buttonFont->GetInternalFont (XtDisplay(formWidget)),
                  NULL);

  handle = (char *) radioButtonWidget;
  wxAddWindowToTable(radioButtonWidget, this);

  SetValue (value);

  XtManageChild (radioButtonWidget);

  if (width == -1)
    width = 100;
  if (height == -1)
    height = 30;

  panel->AttachWidget (this, formWidget, x, y, width, height);
  ChangeColour ();
#endif
#ifdef wx_xview
  Panel x_panel = (Panel) panel->GetHandle ();
  Panel_item x_choice;

  int label_position;

/*
  if (GetLabelPosition() == wxVERTICAL)
    label_position = PANEL_VERTICAL;
  else
*/
  label_position = PANEL_HORIZONTAL;

  if (panel->new_line)
    {
      x_choice = (Panel_item) xv_create (x_panel, PANEL_CHECK_BOX, PANEL_LAYOUT, label_position, PANEL_NEXT_ROW, -1, NULL);
      panel->new_line = FALSE;
    }
  else
    x_choice = (Panel_item) xv_create (x_panel, PANEL_CHECK_BOX, PANEL_LAYOUT, label_position, NULL);

  if (!label->x_image)
    label->CreateServerImage(TRUE);

  xv_set (x_choice,
	  PANEL_NOTIFY_PROC, wxRadioButtonProc,
	  PANEL_CLIENT_DATA, (char *) this,
	  PANEL_VALUE, 0,
	  PANEL_CHOICE_IMAGE, 0, label->x_image,
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

wxRadioButton::~wxRadioButton (void)
{
}

void wxRadioButton::ChangeColour (void)
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

  if (theButtonBitmap)
    XtVaSetValues ((Widget) handle,
	     XmNlabelPixmap, theButtonBitmap->GetLabelPixmap ((Widget) handle),
		   XmNlabelInsensitivePixmap, theButtonBitmap->GetInsensPixmap ((Widget) handle),
		 XmNarmPixmap, theButtonBitmap->GetArmPixmap ((Widget) handle),
		   NULL);
#endif
}

char *wxRadioButton::GetLabel (void)
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

void wxRadioButton::SetLabel (char *label)
{
  theButtonBitmap = NULL;
#ifdef wx_motif
  if (label)
    {
      XmString text = XmStringCreateSimple (label);
      XtVaSetValues ((Widget)handle,
		     XmNlabelString, text,
		     XmNlabelType, XmSTRING,
		     NULL);
      XmStringFree (text);
    }
#endif
#ifdef wx_xview
  Panel_item x_choice = (Panel_item) handle;
  if (label)
    xv_set (x_choice, PANEL_VALUE, label, NULL);
#endif
}

void wxRadioButton::SetLabel (wxBitmap * bitmap)
{
  theButtonBitmap = bitmap;
#ifdef wx_motif
  Widget widget = (Widget)handle;
  if (bitmap)
    {
      XtVaSetValues ((Widget)handle,
		     XmNlabelPixmap, bitmap->GetLabelPixmap (widget),
		     XmNselectPixmap, bitmap->GetLabelPixmap (widget),
		XmNlabelInsensitivePixmap, bitmap->GetInsensPixmap (widget),
	       XmNselectInsensitivePixmap, bitmap->GetInsensPixmap (widget),
		     XmNarmPixmap, bitmap->GetArmPixmap (widget),
		     XmNlabelType, XmPIXMAP,
		     NULL);
    }
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) handle;
  if (!bitmap->x_image)
    bitmap->CreateServerImage(TRUE);

  xv_set (item, PANEL_CHOICE_IMAGE, 0, bitmap->x_image, NULL);
#endif
}

void wxRadioButton::SetValue (Bool value)
{
#ifdef wx_motif
  inSetValue = TRUE;
  XmToggleButtonSetState ((Widget)handle, value, FALSE);
  inSetValue = FALSE;
#endif
#ifdef wx_xview
  Panel_item choice_item = (Panel_item) handle;
  xv_set (choice_item, PANEL_VALUE, value, NULL);
#endif
}

Bool wxRadioButton::GetValue (void)
{
#ifdef wx_motif
  return (Bool)XmToggleButtonGetState ((Widget)handle);
#endif
#ifdef wx_xview
  Panel_item choice_item = (Panel_item) handle;
  return (Bool) xv_get (choice_item, PANEL_VALUE);
#endif
}

#endif // USE_RADIOBUTTON
