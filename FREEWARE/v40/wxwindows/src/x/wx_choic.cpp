/*
 * File:        wx_choic.cc
 * Purpose:     Choice item implementation (X version)
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	April 1995
 * Copyright:   (c) 1995, AIAI, University of Edinburgh
 */


#ifdef __GNUG__
#pragma implementation
#pragma implementation "wx_choic.h"
#endif

#include <stdlib.h>
#include "wx_defs.h"
#include "wx_utils.h"
#include "wx_privt.h"
#include "wx_choic.h"

#ifdef wx_motif
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#endif

#ifdef wx_xview
void wxChoiceProc (Panel_item item, int value, Event * event);
#endif

#ifdef wx_motif
void 
wxChoiceCallback (Widget w, XtPointer clientData,
		  XtPointer ptr)
{
  wxChoice *item = (wxChoice *) clientData;
  if (item)
    {
      if (item->inSetValue)
        return;
        
      char *s = NULL;
      XtVaGetValues (w, XmNuserData, &s, NULL);
      if (s)
	{
	  wxCommandEvent event (wxEVENT_TYPE_CHOICE_COMMAND);
	  event.commandInt = item->FindString (s);
	  event.commandString = s;
	  item->ProcessCommand (event);
	}
    }
}

#endif

wxChoice::wxChoice (void)
{
  no_strings = 0;
#ifdef wx_motif
  labelWidget = NULL;
  buttonWidget = NULL;
  menuWidget = NULL;
  widgetList = NULL;
#endif
#ifdef wx_xview
#endif
}

wxChoice::wxChoice (wxPanel * panel, wxFunction func, char *Title,
	  int x, int y, int width, int height, int N, char **Choices,
	  long style, char *name):
wxbChoice (panel, func, Title, x, y, width, height, N, Choices, style, name)
{
  Create (panel, func, Title, x, y, width, height, N, Choices, style, name);
}

Bool wxChoice::
Create (wxPanel * panel, wxFunction func, char *Title,
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
  no_strings = 0;
  buttonWidget = NULL;
  widgetList = NULL;

  if (!Title)
    Title = "";

  Widget panelForm = panel->panelWidget;
  formWidget = XtVaCreateManagedWidget (windowName,
					xmRowColumnWidgetClass, panelForm,
					XmNmarginHeight, 0,
					XmNmarginWidth, 0,
					XmNpacking, XmPACK_TIGHT,
					XmNorientation, GetLabelPosition() == wxHORIZONTAL ? XmHORIZONTAL : XmVERTICAL,
					NULL);

  if (GetLabelPosition() == wxHORIZONTAL)
    XtVaSetValues (formWidget, XmNspacing, 0, NULL);

  if (Title)
    {
      char buf[400];
      wxStripMenuCodes(Title, buf);

      char *the_label = (style & wxFIXED_LENGTH) ? fillCopy (buf) : copystring (buf);

      XmString text = XmStringCreateSimple (the_label);
      labelWidget = XtVaCreateManagedWidget ("choiceLabel",
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

  /*
   * Create the popup menu
   */
  menuWidget = XmCreatePulldownMenu (formWidget, "choiceMenu", NULL, 0);

  no_strings = 0;
  if (N > 0)
  {
    int i;
    for (i = 0; i < N; i++)
      Append (Choices[i]);
  }

  /*
   * Create button
   */
  Arg args[10];
  Cardinal argcnt = 0;

  XtSetArg (args[argcnt], XmNsubMenuId, menuWidget);
  argcnt++;
  XtSetArg (args[argcnt], XmNmarginWidth, 0);
  argcnt++;
  XtSetArg (args[argcnt], XmNmarginHeight, 0);
  argcnt++;
  XtSetArg (args[argcnt], XmNpacking, XmPACK_TIGHT);
  argcnt++;
  buttonWidget = XmCreateOptionMenu (formWidget, "choiceButton", args, argcnt);
  if (buttonFont)
    XtVaSetValues (buttonWidget,
		   XmNfontList, buttonFont->GetInternalFont (XtDisplay(formWidget)),
		   NULL);

  handle = (char *) buttonWidget;

  wxAddWindowToTable(buttonWidget, this);

  XtManageChild (buttonWidget);

  // Want to get rid of the label by unmanaging it; need
  // different behaviour for different Motif versions.

// OLD CODE
#if 0
#if XmVersion >= 1002
  /*
   ALS:
	In fact, this seems to be a 1.2 problem: with 1.1.4, I've no
	'OptionLabel' under the actual label.
	More, if I execute this code, later access to the widget via
	wxChoice::SetSelection() or wxChoice::SetSize() give me
	many errors under purify, about uninitialized memory read,
	unallocated memory write and so on...
	Because I don't want these warnings and
	since I've no real reason to unmanage the widget, I do not
	execute these 2 instructions. :-))

	BTW, I've looked RowColumn.c code for 1.2 and 1.1, and there
	is diffs in the part which deals with the XmOpttionLabelGadget :-)))
  */
  Widget optionLabel = XmOptionLabelGadget (buttonWidget);
  XtUnmanageChild (optionLabel);
#endif
#endif
  // END OLD CODE

// New code from Roland Haenel (roland_haenel@ac.cybercity.de)
// Some time ago, I reported a problem with wxChoice-items under
// Linux and Motif 2.0 (they caused sporadic GPFs). Now it seems
// that I have found the code responsible for this behauviour.
// This replaces the code above.
#if XmVersion >= 1002
#if XmVersion <  2000
  Widget optionLabel = XmOptionLabelGadget (buttonWidget);
  XtUnmanageChild (optionLabel);
#endif
#endif

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
/**
Doesn't seem to work... Bad boy, Motif!!
      if (GetLabelPosition()==wxHORIZONTAL)
      {
      Position y_label;
      Dimension h_label,h_option ;
        XtVaGetValues(labelWidget,XmNy,&y_label,XmNheight,&h_label,NULL) ;
        XtVaGetValues(buttonWidget,XmNheight,&h_option,NULL) ;
        XtVaSetValues(labelWidget,XmNy,y_label+(h_option-h_label)/2,NULL) ;
      }
**/
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
	      PANEL_NOTIFY_PROC, wxChoiceProc,
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

wxChoice::~wxChoice (void)
{
#ifdef wx_motif
  // For some reason destroying the menuWidget
  // can cause crashes on some machines. It will
  // be deleted implicitly by deleting the parent form
  // anyway.
//  XtDestroyWidget (menuWidget);
  if (widgetList)
    delete[]widgetList;
#endif
}

void wxChoice::ChangeColour (void)
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

void wxChoice::SetSize (int x, int y, int width, int height, int sizeFlags)
{
#ifdef wx_motif
  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_ANY, NULL);
  Bool managed = XtIsManaged(formWidget);

  if (managed)
    XtUnmanageChild (formWidget);

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
    {
      int i;
      for (i = 0; i < no_strings; i++)
	XtVaSetValues (widgetList[i], XmNwidth, actualWidth, NULL);
      XtVaSetValues ((Widget) handle, XmNwidth, actualWidth,
		     NULL);
    }
  if (height > -1)
    {
      int i;
      for (i = 0; i < no_strings; i++)
	XtVaSetValues (widgetList[i], XmNheight, actualHeight, NULL);
      XtVaSetValues ((Widget) handle, XmNheight, actualHeight,
		     NULL);
    }

  if (managed)
    XtManageChild (formWidget);
  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_NONE, NULL);

  wxItem::SetSize (x, y, width, height, sizeFlags);
  GetEventHandler()->OnSize (width, height);
#endif
#ifdef wx_xview
  wxItem::SetSize (x, y, width, height, sizeFlags);
  GetEventHandler()->OnSize (width, height);
#endif
}

void wxChoice::Append (char *Item)
{
#ifdef wx_motif
  wxStripMenuCodes (Item, wxBuffer);
  Widget w = XtVaCreateManagedWidget (wxBuffer,
#if USE_GADGETS
				      xmPushButtonGadgetClass, menuWidget,
#else
				      xmPushButtonWidgetClass, menuWidget,
#endif
				      NULL);
  if (buttonFont)
    XtVaSetValues (w,
		   XmNfontList, buttonFont->GetInternalFont (XtDisplay(formWidget)),
		   NULL);


  Widget *new_widgetList = new Widget[no_strings + 1];
  int i;
  for (i = 0; i < no_strings; i++)
    new_widgetList[i] = widgetList[i];
  new_widgetList[no_strings] = w;
  if (widgetList)
    delete[]widgetList;
  widgetList = new_widgetList;

  char mnem = wxFindMnemonic (Item);
  if (mnem != 0)
    XtVaSetValues (w, XmNmnemonic, mnem, NULL);

  XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) wxChoiceCallback, (XtPointer) this);

  if (no_strings == 0 && buttonWidget)
    {
      XtVaSetValues (buttonWidget, XmNmenuHistory, w, NULL);
      Widget label = XmOptionButtonGadget (buttonWidget);
      XmString text = XmStringCreateSimple (Item);
      XtVaSetValues (label,
		     XmNlabelString, text,
		     NULL);
      XmStringFree (text);
    }
  wxNode *node = stringList.Add (Item);
  XtVaSetValues (w, XmNuserData, node->Data (), NULL);
#endif
#ifdef wx_xview
  Panel_item choice_item = (Panel_item) handle;

  xv_set (choice_item, PANEL_CHOICE_STRING, no_strings, Item, NULL);
#endif
  no_strings++;
}

// Unfortunately, under XView it doesn't redisplay until user resizes
// window. Any suggestions folks?
void wxChoice::Clear (void)
{
#ifdef wx_motif
  stringList.Clear ();
  int i;
  for (i = 0; i < no_strings; i++)
    {
      XtUnmanageChild (widgetList[i]);
      XtDestroyWidget (widgetList[i]);
    }
  if (no_strings)
    delete[]widgetList;
  widgetList = NULL;
  if (buttonWidget)
    XtVaSetValues (buttonWidget, XmNmenuHistory, (Widget) NULL, NULL);
#endif
#ifdef wx_xview
  Panel_item choice_item = (Panel_item) handle;
  xv_set(choice_item, PANEL_VALUE, 0, PANEL_CHOICE_STRINGS, "", NULL, NULL);
/*
  Rect *rect = (Rect *) xv_get (choice_item, XV_RECT);

  int height = rect->r_height;
  int width = rect->r_width;
  int x = (int) xv_get (choice_item, XV_X);
  int y = (int) xv_get (choice_item, XV_Y);
  char *label = GetLabel ();

  xv_destroy_safe (choice_item);
  Panel panel = (Panel) GetParent ()->handle;
  choice_item = (Panel_item) xv_create (panel, PANEL_CHOICE_STACK,
					PANEL_LABEL_STRING, label,
					PANEL_NOTIFY_PROC, wxChoiceProc,
					PANEL_CLIENT_DATA, (char *) this,
		       XV_X, x, XV_Y, y, XV_WIDTH, width, XV_HEIGHT, height,
					XV_SHOW, TRUE,
					NULL);

  handle = (char *) choice_item;
*/
#endif
  no_strings = 0;
}


int wxChoice::GetSelection (void)
{
#ifdef wx_motif
  XmString text;
  char *s;
  Widget label = XmOptionButtonGadget (buttonWidget);
  XtVaGetValues (label,
		 XmNlabelString, &text,
		 NULL);

  if (XmStringGetLtoR (text, XmSTRING_DEFAULT_CHARSET, &s))
    {
      int i = 0;
      for (wxNode * node = stringList.First (); node; node = node->Next ())
	{
	  char *s1 = (char *) node->Data ();
	  if (s1 == s || strcmp (s1, s) == 0)
	    {
              XmStringFree(text) ;
	      XtFree (s);
	      return i;
	    }
	  else
	    i++;
	}			// for()

      XmStringFree(text) ;
      XtFree (s);
      return -1;
    }
  XmStringFree(text) ;
  return -1;
#endif
#ifdef wx_xview
  Panel_item x_choice = (Panel_item) handle;

  return xv_get (x_choice, PANEL_VALUE);
#endif
}

void wxChoice::SetSelection (int n)
{
#ifdef wx_motif
  inSetValue = TRUE;
  
  wxNode *node = stringList.Nth (n);
  if (node)
    {
      Dimension selectionWidth, selectionHeight;

      char *s = (char *) node->Data ();
      XmString text = XmStringCreateSimple (s);
      XtVaGetValues (widgetList[n], XmNwidth, &selectionWidth, XmNheight, &selectionHeight, NULL);
      Widget label = XmOptionButtonGadget (buttonWidget);
      XtVaSetValues (label,
                     XmNlabelString, text,
                     NULL);
      XmStringFree (text);
      XtVaSetValues (buttonWidget,
                     XmNwidth, selectionWidth, XmNheight, selectionHeight,
                     XmNmenuHistory, widgetList[n], NULL);
    }
  inSetValue = FALSE;

/*
  wxNode *node = stringList.Nth (n);
  if (node)
    {
      char *s = (char *) node->Data ();
      XmString text = XmStringCreateSimple (s);
      Widget label = XmOptionButtonGadget (buttonWidget);
      XtVaSetValues (label,
		     XmNlabelString, text,
		     NULL);
      XmStringFree (text);
      XtVaSetValues (buttonWidget, XmNmenuHistory, widgetList[n], NULL);
    }
*/
#endif
#ifdef wx_xview
  Panel_item x_choice = (Panel_item) handle;

  (void) xv_set (x_choice, PANEL_VALUE, n, NULL);
#endif
}

int wxChoice::FindString (char *s)
{
#ifdef wx_motif
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

char *wxChoice::GetString (int n)
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

void wxChoice::SetColumns(int n)
{
  if (n<1) n = 1 ;

#ifdef wx_motif
  short numColumns = n ;
  Arg args[3];

  XtSetArg(args[0], XmNnumColumns, numColumns);
  XtSetArg(args[1], XmNpacking, XmPACK_COLUMN);
  XtSetValues(menuWidget,args,2) ;
#endif
#ifdef wx_xview
  xv_set((Xv_opaque)handle,PANEL_CHOICE_NCOLS,n,NULL) ;
#endif
}

int  wxChoice::GetColumns(void)
{
#ifdef wx_motif
  short numColumns ;

  XtVaGetValues(menuWidget,XmNnumColumns,&numColumns,NULL) ;
  return numColumns ;
#endif
#ifdef wx_xview
  return xv_get((Xv_opaque)handle,PANEL_CHOICE_NCOLS) ;
#endif
}

void wxChoice::SetFocus(void)
{
#ifdef wx_motif
  XmProcessTraversal(XtParent((Widget)handle), XmTRAVERSE_CURRENT);
#endif
#ifdef wx_xview
  wxItem::SetFocus();
#endif
}

#ifdef wx_xview
void 
wxChoiceProc (Panel_item item, int value, Event * x_event)
{
  wxChoice *choice = (wxChoice *) xv_get (item, PANEL_CLIENT_DATA);
  wxCommandEvent event (wxEVENT_TYPE_CHOICE_COMMAND);

  event.commandString = (char *) xv_get (item, PANEL_CHOICE_STRING, value);
  event.commandInt = value;
  event.eventObject = choice;

  choice->ProcessCommand (event);
}
#endif
