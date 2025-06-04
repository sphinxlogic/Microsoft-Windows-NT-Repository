/*
 * File:        wx_combo.cc
 * Purpose:     wxComboBox item implementation (X version)
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	April 1995
 * Copyright:   (c) 1995, AIAI, University of Edinburgh
 */

#ifdef __GNUG__
#pragma implementation
#pragma implementation "wx_combo.h"
#endif

#include <stdlib.h>
#include "wx_defs.h"

#if USE_COMBOBOX

#include "../../contrib/combobox/combobox.h"
#include "wx_utils.h"
#include "wx_privt.h"
#include "wx_combo.h"


#ifdef wx_motif
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#endif

#ifdef wx_xview
void wxComboBoxProc (Panel_item item, int value, Event * event);
#endif

#ifdef wx_motif
void 
wxComboBoxCallback (Widget w, XtPointer clientData,
		   XmComboBoxSelectionCallbackStruct * cbs)
{
  wxComboBox *item = (wxComboBox *) clientData;

/*
  if (item->inSetValue)
    return;
*/

  wxCommandEvent event (wxEVENT_TYPE_COMBOBOX_COMMAND);
  switch (cbs->reason)
  {
    case XmCR_SINGLE_SELECT:
    {
//	event.clientData = item->GetClientData (cbs->item_position - 1);
	//event.commandString = item->GetStringSelection();
	event.commandInt = cbs->index - 1;
	event.commandString = item->GetString (event.commandInt);
	event.extraLong = TRUE;
	event.eventObject = item;
	item->ProcessCommand (event);
	break;
    }
    default:
      break;
   }
}

#endif

wxComboBox::wxComboBox (void)
{
  no_strings = 0;
#ifdef wx_motif
/*
  labelWidget = NULL;
  buttonWidget = NULL;
  menuWidget = NULL;
  widgetList = NULL;
*/
#endif
#ifdef wx_xview
#endif
}

wxComboBox::wxComboBox (wxPanel * panel, wxFunction func, char *Title, Constdata char *value,
	  int x, int y, int width, int height, int N, char **Choices,
	  long style, char *name):
wxbComboBox (panel, func, Title, value, x, y, width, height, N, Choices, style, name)
{
  Create (panel, func, Title, value, x, y, width, height, N, Choices, style, name);
}

Bool wxComboBox::
Create (wxPanel * panel, wxFunction func, char *Title, Constdata char *value,
	int x, int y, int width, int height, int N, char **Choices,
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
  windowStyle = style;

  // Helper function in wb_item.cc that sets the
  // appropriate label position depending on windowStyle flags
  // and parent label position setting
  SetAppropriateLabelPosition();

#ifdef wx_motif
  canAddEventHandler = TRUE;
  Widget buttonWidget = NULL;

  if (!Title)
    Title = "";

  Widget panelForm = panel->panelWidget;
  formWidget = XtVaCreateManagedWidget (windowName,
					xmRowColumnWidgetClass, panelForm,
					XmNmarginHeight, 0,
					XmNmarginWidth, 0,
					XmNpacking, XmPACK_TIGHT,
					XmNorientation, panel->label_position == wxHORIZONTAL ? XmHORIZONTAL : XmVERTICAL,
					NULL);

  if (GetLabelPosition() == wxHORIZONTAL)
    XtVaSetValues (formWidget, XmNspacing, 0, NULL);

  if (Title)
    {
      char buf[400];
      wxStripMenuCodes(Title, buf);

      char *the_label = (style & wxFIXED_LENGTH) ? fillCopy (buf) : copystring (buf);

      XmString text = XmStringCreateSimple (the_label);
      labelWidget = XtVaCreateManagedWidget ("comboBoxLabel",
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

  buttonWidget = XtVaCreateManagedWidget(windowName,
					 xmComboBoxWidgetClass, panelForm,
 					 XmNmarginHeight, 0,
					 XmNmarginWidth, 0,
					 XmNshowLabel, False,
       XmNeditable, ((windowStyle & wxCB_READONLY) != wxCB_READONLY),
       XmNsorted, ((windowStyle & wxCB_SORT) == wxCB_SORT),
       XmNstaticList, ((windowStyle & wxCB_SIMPLE) == wxCB_SIMPLE),
					 NULL);

  XtAddCallback (buttonWidget, XmNselectionCallback, (XtCallbackProc) wxComboBoxCallback,
		 (XtPointer) this);

  int i;
  for (i = 0; i < N; i++)
  {
    XmString str = XmStringCreateLtoR(Choices[i], XmSTRING_DEFAULT_CHARSET);
    XmComboBoxAddItem(buttonWidget, str, 0);
    XmStringFree(str);
    stringList.Add(Choices[i]);
  }
  no_strings = N;

  handle = (char *) buttonWidget;

  if (value)
    SetValue(value);

  wxAddWindowToTable(buttonWidget, this);

  XtManageChild (buttonWidget);

  panel->AttachWidget (this, formWidget, x, y, width, height);
  ChangeColour ();

  /* After creating widgets, no more resizes. */
  if (style & wxFIXED_LENGTH)
    {
      XtVaSetValues (formWidget,
		     XmNpacking, XmPACK_NONE,
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

    XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_NONE, NULL);
#endif
#ifdef wx_xview
  no_strings = N;
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
      x_choice = (Panel_item) xv_create (x_panel, PANEL_CHOICE_STACK, PANEL_LAYOUT, label_position, PANEL_NEXT_ROW, -1, NULL);
      panel->new_line = FALSE;
    }
  else
    x_choice = (Panel_item) xv_create (x_panel, PANEL_CHOICE_STACK, PANEL_LAYOUT, label_position, NULL);

  xv_set (x_choice,
	      PANEL_NOTIFY_PROC, wxComboBoxProc,
	      PANEL_CLIENT_DATA, (char *) this,
	      NULL);

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
      {
        xv_set (x_choice, PANEL_LABEL_STRING, actualLabel, NULL);
      }
    }

/*
   if (buttonFont)
   xv_set(x_choice, XV_FONT, buttonFont->GetInternalFont(), NULL) ;
 */

  if (x > -1 && y > -1)
    (void) xv_set (x_choice, XV_X, x, XV_Y, y, NULL);

  int i;
  for (i = 0; i < N; i++)
    {
      char *label = Choices[i];
      xv_set (x_choice, PANEL_CHOICE_STRING, i, label, NULL);
    }
  handle = (char *) x_choice;

#endif

  Callback (func);
  return TRUE;
}

wxComboBox::~wxComboBox (void)
{
#ifdef wx_motif
#endif
}

void wxComboBox::ChangeColour (void)
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

#if 0
  if (change == wxBACK_COLORS)
    {
      XtVaSetValues (menuWidget,
		     XmNbackground, itemColors[wxBACK_INDEX].pixel,
		     XmNtopShadowColor, itemColors[wxTOPS_INDEX].pixel,
		     XmNbottomShadowColor, itemColors[wxBOTS_INDEX].pixel,
		     XmNarmColor, itemColors[wxSELE_INDEX].pixel,
		     XmNforeground, itemColors[wxFORE_INDEX].pixel,
		     NULL);
      int i;
      for (i = 0; i < no_strings; i++)
	XtVaSetValues (widgetList[i],
		       XmNbackground, itemColors[wxBACK_INDEX].pixel,
		       XmNtopShadowColor, itemColors[wxTOPS_INDEX].pixel,
		       XmNbottomShadowColor, itemColors[wxBOTS_INDEX].pixel,
		       XmNarmColor, itemColors[wxSELE_INDEX].pixel,
		       XmNforeground, itemColors[wxFORE_INDEX].pixel,
		       NULL);
      XtVaSetValues (buttonWidget,
		     XmNbackground, itemColors[wxBACK_INDEX].pixel,
		     XmNtopShadowColor, itemColors[wxTOPS_INDEX].pixel,
		     XmNbottomShadowColor, itemColors[wxBOTS_INDEX].pixel,
		     XmNarmColor, itemColors[wxSELE_INDEX].pixel,
		     XmNforeground, itemColors[wxFORE_INDEX].pixel,
		     NULL);
    }
  else if (change == wxFORE_COLORS)
    {
      int i;
      for (i = 0; i < no_strings; i++)
	XtVaSetValues (widgetList[i],
		       XmNforeground, itemColors[wxFORE_INDEX].pixel,
		       NULL);
      XtVaSetValues (menuWidget,
		     XmNforeground, itemColors[wxFORE_INDEX].pixel,
		     NULL);
      XtVaSetValues (buttonWidget,
		     XmNforeground, itemColors[wxFORE_INDEX].pixel,
		     NULL);
    }
#endif

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

#endif
}

void wxComboBox::SetSize (int x, int y, int width, int height, int sizeFlags)
{
#ifdef wx_motif
  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_ANY, NULL);
  Bool managed = XtIsManaged(formWidget);

  if (managed)
    XtUnmanageChild (formWidget);

  if (x > -1)
    XtVaSetValues (formWidget, XmNleftAttachment, XmATTACH_SELF,
		   XmNx, x,
		   NULL);
  if (y > -1)
    XtVaSetValues (formWidget, XmNtopAttachment, XmATTACH_SELF,
		   XmNy, y,
		   NULL);

  Dimension labelWidth = 0, labelHeight = 0;
  int actualWidth = width, actualHeight = height;

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
    XtVaSetValues ((Widget) handle, XmNwidth, wxMax(actualWidth, 10),
		   NULL);
  if (height > -1)
    XtVaSetValues ((Widget) handle, XmNheight, wxMax(actualHeight, 10),
		   NULL);

  if (managed)
    XtManageChild (formWidget);
  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_NONE, NULL);

  GetEventHandler()->OnSize (width, height);
#endif
#ifdef wx_xview
  wxItem::SetSize (x, y, width, height, sizeFlags);
  GetEventHandler()->OnSize (width, height);
#endif
}

void wxComboBox::Enable (Bool enable)
{
#ifdef wx_motif
  if (handle)
  {
    XtSetSensitive((Widget)handle, enable);
    XmUpdateDisplay((Widget)handle);
  }
  if (labelWidget)
  {
    XtSetSensitive(labelWidget, enable);
    XmUpdateDisplay(labelWidget);
  }
#endif
#ifdef wx_xview
  Xv_opaque window = (Xv_opaque) handle;
  if (window)
    xv_set (window, PANEL_INACTIVE, !enable, NULL);
#endif
}

Bool wxComboBox::Show (Bool show)
{
  isShown = show;
#ifdef wx_motif
  // For some reason only the label is shown/hidden???
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

void wxComboBox::Append (char *Item)
{
#ifdef wx_motif
  XmString str = XmStringCreateLtoR(Item, XmSTRING_DEFAULT_CHARSET);
  XmComboBoxAddItem((Widget)handle, str, 0);
  stringList.Add(Item);
  XmStringFree(str);
#endif
#ifdef wx_xview
  Panel_item choice_item = (Panel_item) handle;

  xv_set (choice_item, PANEL_CHOICE_STRING, no_strings, Item, NULL);
#endif
  no_strings++;
}

void wxComboBox::Delete (int pos)
{
#ifdef wx_motif
  XmComboBoxDeletePos((Widget)handle, pos-1);
  wxNode *node = stringList.Nth(pos);
  if (node)
    {
      delete[] (char *)node->Data();
      delete node;
    }
  no_strings--;
#endif
#ifdef wx_xview
  // Can't delete in XView!
#endif
}

// Unfortunately, under XView it doesn't redisplay until user resizes
// window. Any suggestions folks?
void wxComboBox::Clear (void)
{
#ifdef wx_motif
  XmComboBoxDeleteAllItems((Widget)handle);
  stringList.Clear();
#endif
#ifdef wx_xview
  Panel_item choice_item = (Panel_item) handle;
  xv_set(choice_item, PANEL_VALUE, 0, PANEL_CHOICE_STRINGS, "", NULL, NULL);
#endif
  no_strings = 0;
}


int wxComboBox::GetSelection (void)
{
#ifdef wx_motif
  int sel = XmComboBoxGetSelectedPos((Widget)handle);
  if (sel == 0)
    return -1;
  else
    return sel - 1;
#endif
#ifdef wx_xview
  Panel_item x_choice = (Panel_item) handle;

  return xv_get (x_choice, PANEL_VALUE);
#endif
}

void wxComboBox::SetSelection (int n)
{
#ifdef wx_motif
  XmComboBoxSelectPos((Widget)handle, n+1, False);
#endif
#ifdef wx_xview
  Panel_item x_choice = (Panel_item) handle;

  (void) xv_set (x_choice, PANEL_VALUE, n, NULL);
#endif
}

int wxComboBox::FindString (char *s)
{
#ifdef wx_motif
  int *pos_list = NULL;
  int count = 0;
  XmString text = XmStringCreateSimple (s);
  Bool found = XmComboBoxGetMatchPos((Widget)handle,
   text, &pos_list, &count);

  XmStringFree(text);

  if (found && count > 0)
  {
    int pos = pos_list[0] - 1;
    free(pos_list);
    return pos;
  }

  return -1;

/*
  int i = 0;
  for (wxNode * node = stringList.First (); node; node = node->Next ())
    {
      char *s1 = (char *) node->Data ();
      if (s1 == s || strcmp (s1, s) == 0)
	{
	  return i;
	}
      else
	i++;
    }
  return -1;
*/
#endif
#ifdef wx_xview
  Panel_item choice = (Panel_item) handle;

  int max1 = no_strings;

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

char *wxComboBox::GetString (int n)
{
#ifdef wx_motif
  wxNode *node = stringList.Nth (n);
  if (node)
    return (char *) node->Data ();
  else
    return NULL;
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) handle;
  return (char *) xv_get (item, PANEL_CHOICE_STRING, n);
#endif
}

void wxComboBox::SetColumns(int n)
{
  if (n<1) n = 1 ;

#ifdef wx_motif
/*
  short numColumns = n ;
  Arg args[3];

  XtSetArg(args[0], XmNnumColumns, numColumns);
  XtSetArg(args[1], XmNpacking, XmPACK_COLUMN);
  XtSetValues(menuWidget,args,2) ;
*/
#endif
#ifdef wx_xview
  xv_set((Xv_opaque)handle,PANEL_CHOICE_NCOLS,n,NULL) ;
#endif
}

int  wxComboBox::GetColumns(void)
{
#ifdef wx_motif
/*
  short numColumns ;

  XtVaGetValues(menuWidget,XmNnumColumns,&numColumns,NULL) ;
  return numColumns ;
*/
  return 1;
#endif
#ifdef wx_xview
  return xv_get((Xv_opaque)handle,PANEL_CHOICE_NCOLS) ;
#endif
}

void wxComboBox::SetFocus(void)
{
#ifdef wx_motif
  XmProcessTraversal(XtParent((Widget)handle), XmTRAVERSE_CURRENT);
/*
  wxPanel *panel = (wxPanel *) GetParent();
  panel->manualChange = TRUE;
  wxItem::SetFocus();
  panel->manualChange = FALSE;
*/
#endif
#ifdef wx_xview
  wxItem::SetFocus();
#endif
}

char *wxComboBox::GetValue (void)
{
#ifdef wx_motif
  char *s = XmComboBoxGetString ((Widget) handle);
  if (s)
    {
      strcpy (wxBuffer, s);
      XtFree (s);
      return wxBuffer;
    }
  else
    return NULL;
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) handle;
  return (char *) xv_get (item, PANEL_VALUE);
#endif
}

void wxComboBox::SetValue (char *value)
{
#ifdef wx_motif
  inSetValue = TRUE;
  if (value)
    XmComboBoxSetString ((Widget) handle, value);
  inSetValue = FALSE;
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) handle;
  xv_set (item, PANEL_VALUE, value, NULL);
#endif
}

// Clipboard operations
void wxComboBox::Copy(void)
{
#ifdef wx_motif
  XmComboBoxCopy((Widget)handle, CurrentTime);
#endif
}

void wxComboBox::Cut(void)
{
#ifdef wx_motif
  XmComboBoxCut((Widget)handle, CurrentTime);
#endif
}

void wxComboBox::Paste(void)
{
#ifdef wx_motif
  XmComboBoxPaste((Widget)handle);
#endif
}

void wxComboBox::SetEditable(Bool editable)
{
/*
#ifdef wx_motif
  Widget textWidget = (Widget)handle;
  XmTextSetEditable(textWidget, editable);
#endif
#ifdef wx_xview
  // What should it be for XView?
#endif
*/
}

void wxComboBox::SetInsertionPoint(long pos)
{
#ifdef wx_motif
  Widget textWidget = (Widget)handle;
  XmComboBoxSetInsertionPosition (textWidget, (XmTextPosition) pos);
#endif
}

void wxComboBox::SetInsertionPointEnd(void)
{
#ifdef wx_motif
  XmTextPosition pos = XmComboBoxGetLastPosition ((Widget)handle);
  XmComboBoxSetInsertionPosition ((Widget)handle, (XmTextPosition) (pos + 1));
#endif
}

long wxComboBox::GetInsertionPoint(void)
{
#ifdef wx_motif
  return (long) XmComboBoxGetInsertionPosition ((Widget)handle);
#endif
#ifdef wx_xview
  return 0;
#endif
}

long wxComboBox::GetLastPosition(void)
{
#ifdef wx_motif
  return (long) XmComboBoxGetLastPosition ((Widget)handle);
#endif
#ifdef wx_xview
  return 0;
#endif
}

void wxComboBox::Replace(long from, long to, char *value)
{
#ifdef wx_motif
  XmComboBoxReplace ((Widget)handle, (XmTextPosition) from, (XmTextPosition) to,
		 value);
#endif
#ifdef wx_xview
#endif
}

void wxComboBox::Remove(long from, long to)
{
#ifdef wx_motif
  XmComboBoxSetSelection ((Widget)handle, (XmTextPosition) from, (XmTextPosition) to,
		      (Time) 0);
  XmComboBoxRemove ((Widget)handle);
#endif
#ifdef wx_xview
#endif
}

void wxComboBox::SetSelection(long from, long to)
{
#ifdef wx_motif
  XmComboBoxSetSelection ((Widget)handle, (XmTextPosition) from, (XmTextPosition) to,
		      (Time) 0);
#endif
#ifdef wx_xview
#endif
}

#ifdef wx_xview
void 
wxComboBoxProc (Panel_item item, int value, Event * x_event)
{
  wxComboBox *choice = (wxComboBox *) xv_get (item, PANEL_CLIENT_DATA);
  wxCommandEvent event (wxEVENT_TYPE_COMBOBOX_COMMAND);

  event.commandString = (char *) xv_get (item, PANEL_CHOICE_STRING, value);
  event.commandInt = value;
  event.eventObject = choice;

  choice->ProcessCommand (event);
}
#endif

#endif
 // USE_COMBO_BOX
