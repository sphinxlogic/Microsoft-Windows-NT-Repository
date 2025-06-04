/*
 * File:        wx_txt.cc
 * Purpose:     Panel items implementation (X version)
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	April 1995
 * Copyright:   (c) 1995, AIAI, University of Edinburgh
 */

#ifdef __GNUG__
#pragma implementation
#pragma implementation "wx_txt.h"
#endif

#include <stdlib.h>
#include "wx_defs.h"
#include "wx_utils.h"
#include "wx_privt.h"
#include "wx_txt.h"

#include <X11/keysym.h>

#ifdef wx_motif
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/Text.h>
#include <Xm/Form.h>

void 
wxTextModifyProc (Widget w, XtPointer clientData, XmTextVerifyCallbackStruct *cbs);
#endif

static Bool checkFunctionKey(wxPanel *panelPtr, wxItem *itemPtr,
		XEvent *event)
{
	if (event->type == KeyPress) {

		KeySym keySym;
		XLookupString((XKeyEvent *) event, wxBuffer, 20,
				&keySym, NULL);

		int id = CharCodeXToWX(keySym);

		wxKeyEvent wxevent(wxEVENT_TYPE_CHAR);
      
		if (event->xkey.state & ShiftMask)
			wxevent.shiftDown = TRUE;
		if (event->xkey.state & ControlMask)
			wxevent.controlDown = TRUE;
		wxevent.eventObject = panelPtr;
		wxevent.keyCode = id;
        
		if (IsCursorKey(keySym) || IsPFKey(keySym) ||
				IsFunctionKey(keySym) || IsMiscFunctionKey(keySym) ||
				IsKeypadKey(keySym)) {
			if (panelPtr->GetEventHandler()->OnFunctionKey(wxevent))
				return TRUE;
		}
		itemPtr->GetEventHandler()->OnChar(wxevent);
		return TRUE;
	}
	return FALSE;
}

#ifdef wx_xview
Panel_setting 
wxTextProc (Panel_item item, Event * x_event)
{
  Panel_setting ret_code = panel_text_notify (item, x_event);
  if (ret_code == PANEL_NONE)
    return (PANEL_NONE);
  // So, ret_code is PANEL_INSERT,PANEL_NEXT or PANEL_PREVIOUS
  wxText *text = (wxText *) xv_get (item, PANEL_CLIENT_DATA);
  int event_type;
  if (ret_code != PANEL_INSERT)
    event_type = wxEVENT_TYPE_TEXT_ENTER_COMMAND;
  else
    event_type = wxEVENT_TYPE_TEXT_COMMAND;

  wxCommandEvent event (event_type);
  event.commandString = (char *) xv_get (item, PANEL_VALUE);
  event.eventHandle = (char *) x_event;
  event.eventObject = text;
  text->ProcessCommand (event);
  return (Panel_setting) ret_code;
}

void (*old_event_proc)(Panel_item, Event *) = NULL;

#if 0
    Text_info	   *dp = TEXT_PRIVATE(item_public);
    int		    format;
    Frame	    frame;
    Item_info      *ip = ITEM_PRIVATE(item_public);
    long	    length;
    Xv_Notice	    notice;
    int		    own_primary_seln;
    Panel_info	   *panel = ip->panel;
    struct pr_size  size;
    int		    take_down_caret;
    int		    retstatus;
#endif

/*
 * THIS DOESN'T WORK
 * because sometimes the text item where the caret is, is a wxSlider,
 * not a wxText. There is currently no way to distinguish between them,
 * so it causes seg violations when changing focus from a wxSlider
 * to a wxText.
 *
void wxTextEventProc(Panel_item item, Event *event)
{
    if (event_is_up(event)) {
    	(old_event_proc)(item, event);
	} else {
		Panel panel = (Panel) xv_get(item, XV_OWNER);
		wxPanel *panelPtr = (wxPanel *) xv_get(panel, WIN_CLIENT_DATA);

		Panel_item oldCurr = (Panel_item) xv_get(panel, PANEL_CARET_ITEM);
		wxText *oldTxt = NULL;
                if (oldCurr)
                  oldTxt = (wxText *) xv_get(oldCurr, PANEL_CLIENT_DATA);

                if (oldTxt)
                  checkFunctionKey(panelPtr, oldTxt, event->ie_xevent);

		// Try to detect keyboard focus changes between items
		(old_event_proc)(item, event);
		Panel_item newCurr = (Panel_item) xv_get(panel, PANEL_CARET_ITEM);

		if (oldCurr != newCurr) {

			// Detected a change in keyboard focus, notify the items
			// and their panel.
			wxText *newTxt = (wxText *) xv_get(newCurr, PANEL_CLIENT_DATA);
                        if (oldTxt)
                          oldTxt->GetEventHandler()->OnKillFocus();

			// current item may have changed again!
			newCurr = (Panel_item) xv_get(panel, PANEL_CARET_ITEM);
			if (oldCurr != newCurr) {
				newTxt = (wxText *) xv_get(newCurr, PANEL_CLIENT_DATA);
				newTxt->GetEventHandler()->OnSetFocus();
				panelPtr->GetEventHandler()->OnChangeFocus(oldTxt, newTxt);
			}
		}
	}
}
*/

#endif


#ifdef wx_motif
void clientMsgHandler(Widget w, XtPointer cdata, XEvent *event)
{
	if (event->type != ClientMessage)
		return;

	// toDo: use memcpy to retrieve the pointers (using the '.b' part)
	wxItem *item1 = (wxItem *) event->xclient.data.l[0];
	wxItem *item2 = (wxItem *) event->xclient.data.l[1];
	wxPanel *panel = (wxPanel *) item1->GetParent();
	panel->GetEventHandler()->OnChangeFocus(item1, item2);
}

void keyPressHandler(Widget w, XtPointer cData, XEvent *event)
{
	wxText *item = (wxText *) cData;
	wxPanel *panel = (wxPanel *) item->GetParent();
	checkFunctionKey(panel, item, event);
}

void wxTextCallback(Widget w, XtPointer clientData,
                      XmAnyCallbackStruct *ptr)
{
  wxText *item = (wxText *)clientData;
  int type_event ;
  // sorry... but it's the only way we can get to the panel!
  wxPanel *panel = (wxPanel *) item->GetParent();
  if (panel->manualChange)
	return;

  switch (ptr->reason) {
  case XmCR_LOSING_FOCUS:
    item->GetEventHandler()->OnKillFocus();
    return;
/*
	panel->previousFocus = item;
	return;
*/
  case XmCR_FOCUS:
    item->GetEventHandler()->OnSetFocus();
    return;

/* THIS CODE CAUSES A CRASH in clientMsgHandler for more than 1
   text control. So for now, we're commenting it out. JACS 17/5/95
   
  	if (panel->previousFocus != NULL && panel->previousFocus != item) {
  		wxItem *item1 = panel->previousFocus;
  		wxItem *item2 = item;

	  	XClientMessageEvent ev;
	  	ev.display = XtDisplay(Widget(panel->handle));
	  	ev.window = XtWindow(Widget(panel->handle));
	  	ev.type = ClientMessage;
	  	ev.format = 32;
#if XmVersion > 1100
	  	ev.message_type = XA_INTEGER;
#endif
	  	ev.data.l[0] = (long) item1;
	  	ev.data.l[1] = (long) item2;
                // Surely this is static for all text items and will
                // only get added to the first created??? - JACS
	  	static Bool addedClientMsgHandler = FALSE;
	  	if (!addedClientMsgHandler) {
		  	XtAddEventHandler(Widget(panel->handle), NoEventMask, TRUE,
		  			(XtEventHandler) clientMsgHandler, NULL);
		  	addedClientMsgHandler = TRUE;
		}
	  	XSendEvent(XtDisplay((Widget) panel->handle),
	  			XtWindow((Widget) panel->handle), TRUE, 0, (XEvent *) &ev);
	}
*/
  case XmCR_ACTIVATE:
    type_event = wxEVENT_TYPE_TEXT_ENTER_COMMAND ;
    break;
  default:
    type_event = wxEVENT_TYPE_TEXT_COMMAND ;
    break;
  }

  if (item->inSetValue)
    return;
    
  wxCommandEvent event(type_event);
  event.commandString = item->GetValue();
  event.eventObject = item;
  item->ProcessCommand(event);
}
#endif

// Text item
wxText::wxText (void)
{
#ifdef wx_motif
  internalTextValue = NULL;
#endif
}

wxText::wxText (wxPanel * panel, wxFunction Function, char *label, char *value,
	int x, int y, int width, int height, long style, char *name):
wxbText (panel, Function, label, value, x, y, width, height, style, name)
{
  Create (panel, Function, label, value, x, y, width, height, style, name);
}

Bool wxText::
Create (wxPanel * panel, wxFunction Function, char *label, char *value,
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

  SetAppropriateLabelPosition();
#ifdef wx_motif
  if (width < 0) width = 150;
  canAddEventHandler = TRUE;
  internalTextValue = NULL;
  Widget panelForm = panel->panelWidget;
  formWidget = XtVaCreateManagedWidget (windowName,
					xmFormWidgetClass, panelForm,
					XmNmarginHeight, 0,
					XmNmarginWidth, 0,
					NULL);

  Widget textWidget;

  if (buttonFont)
    textWidget = XtVaCreateManagedWidget ("text",
					  xmTextWidgetClass, formWidget,
				XmNfontList, buttonFont->GetInternalFont (XtDisplay(formWidget)),
					  NULL);
  else
    textWidget = XtVaCreateManagedWidget ("text",
					  xmTextWidgetClass, formWidget,
					  NULL);

  int noCols = 2;
  if (value && (strlen (value) > noCols))
    noCols = strlen (value);

  XtVaSetValues (textWidget,
		 XmNcolumns, noCols,
		 NULL);

  if (label)
    {
      actualLabel = wxStripMenuCodes(label);
      char *the_label = (style & wxFIXED_LENGTH) ? fillCopy (actualLabel) : copystring (actualLabel);
      XmString text = XmStringCreateSimple (the_label);
      labelWidget = XtVaCreateManagedWidget (actualLabel,
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

  if (GetLabelPosition() == wxHORIZONTAL)
    {
      if (labelWidget)
	XtVaSetValues (labelWidget,
		       XmNtopAttachment, XmATTACH_FORM,
		       XmNleftAttachment, XmATTACH_FORM,
		       XmNbottomAttachment, XmATTACH_FORM,
		       XmNalignment, XmALIGNMENT_BEGINNING,
		       NULL);

      XtVaSetValues (textWidget,
//		     XmNleftOffset, labelWidget ? 4 : 0,
		     XmNleftOffset, 4,
		     XmNtopAttachment, XmATTACH_FORM,
		     XmNbottomAttachment, XmATTACH_FORM,
	   XmNleftAttachment, labelWidget ? XmATTACH_WIDGET : XmATTACH_FORM,
		     XmNleftWidget, labelWidget ? labelWidget : formWidget,
		     XmNrightAttachment, XmATTACH_FORM,
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

      XtVaSetValues (textWidget,
	    XmNtopAttachment, labelWidget ? XmATTACH_WIDGET : XmATTACH_FORM,
		     XmNtopWidget, labelWidget ? labelWidget : formWidget,
		     XmNbottomAttachment, XmATTACH_FORM,
		     XmNleftAttachment, XmATTACH_FORM,
		     XmNrightAttachment, XmATTACH_FORM,
		     XmNalignment, XmALIGNMENT_BEGINNING,
		     NULL);
    }

  handle = (char *) textWidget;
  wxAddWindowToTable(textWidget, this);

  if (windowStyle & wxREADONLY)
    XmTextSetEditable(textWidget, False);

  XtAddCallback(textWidget, XmNfocusCallback, (XtCallbackProc)wxTextCallback,
                  (XtPointer)this);
  XtAddCallback(textWidget, XmNlosingFocusCallback, (XtCallbackProc)wxTextCallback,
                  (XtPointer)this);
  XtAddCallback (textWidget, XmNactivateCallback, (XtCallbackProc) wxTextCallback,
		 (XtPointer) this);
  XtAddCallback (textWidget, XmNvalueChangedCallback, (XtCallbackProc) wxTextCallback,
		 (XtPointer) this);
  XtAddCallback(textWidget, XmNmodifyVerifyCallback, (XtCallbackProc)wxTextModifyProc, (XtPointer)this);


  panel->AttachWidget (this, formWidget, x, y, width, height);
  ChangeColour ();

  if (value)
    XmTextSetString (textWidget, value);

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

      XtVaSetValues (textWidget,
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
  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_NONE, NULL);
#endif
#ifdef wx_xview
//TOMS ADDITIONS //used at the end
  int _x = x;
  int _y = y;
  int _w = width;
  int _h = height;
//END TOMS ADDITIONS

  Panel x_panel = (Panel) (panel->GetHandle ());
  Panel_item x_text;

  int label_position;
  if (GetLabelPosition() == wxVERTICAL)
    label_position = PANEL_VERTICAL;
  else
    label_position = PANEL_HORIZONTAL;

  if (panel->new_line)
    {
      x_text = (Panel_item) xv_create (x_panel, PANEL_TEXT,
				       PANEL_VALUE_DISPLAY_WIDTH, 30,
		    PANEL_LAYOUT, label_position, PANEL_NEXT_ROW, -1, NULL);
      panel->new_line = FALSE;
    }
  else
    x_text = (Panel_item) xv_create (x_panel, PANEL_TEXT,
				     PANEL_VALUE_DISPLAY_WIDTH, 30,
				     PANEL_LAYOUT, label_position, NULL);

  old_event_proc = (void (*)(Panel_item, Event *)) xv_get(x_text, PANEL_EVENT_PROC);
  xv_set(x_text, WIN_X_EVENT_MASK, FocusChangeMask, NULL);

  xv_set (x_text,
	  PANEL_NOTIFY_PROC, wxTextProc,
  // Removed for now because it causes crashes (see wxTextEventProc above)
//          PANEL_EVENT_PROC, wxTextEventProc,
	  PANEL_NOTIFY_LEVEL, PANEL_ALL,
	  PANEL_CLIENT_DATA, (char *) this,
	  NULL);

  if ((windowStyle & wxPASSWORD) == wxPASSWORD)
    xv_set(x_text, PANEL_MASK_CHAR, '*', NULL);
  
  if ((windowStyle & wxREADONLY) == wxREADONLY)
    xv_set(x_text, PANEL_READ_ONLY, TRUE, NULL);

  if (x > -1 && y > -1)
    (void) xv_set (x_text, XV_X, x, XV_Y, y, NULL);

 if (value && strlen (value) && (width == -1))
    {
      width = strlen (value);
      if (width < 4)
	width = 4;
	//TOMS MOD changed this line
 //     xv_set (x_text, PANEL_VALUE_DISPLAY_LENGTH, (int) width, NULL); 
      xv_set (x_text, PANEL_VALUE_DISPLAY_WIDTH, (int) width, NULL); 
    }
  else
    {
      if (width <= 0)
	width = 80;
      xv_set (x_text, PANEL_VALUE_DISPLAY_WIDTH, (int) width, NULL);
    }

  if (label)
    {
      actualLabel = wxStripMenuCodes(label);

      if (style & wxFIXED_LENGTH)
	{
	  char *the_label = fillCopy (actualLabel);
	  xv_set (x_text, PANEL_LABEL_STRING, the_label, NULL);

	  int label_x = (int) xv_get (x_text, PANEL_LABEL_X);
	  int item_x = (int) xv_get (x_text, PANEL_VALUE_X);
	  xv_set (x_text, PANEL_LABEL_STRING, actualLabel,
		  PANEL_VALUE_X, item_x,
		  PANEL_LABEL_X, label_x,
		  NULL);
	  delete[] the_label;
	}
      else
	xv_set (x_text, PANEL_LABEL_STRING, actualLabel, NULL);
    }

  if (value)
    {
      xv_set (x_text, PANEL_VALUE, value, NULL);
    };

  handle = (char *) x_text;
//TOMS ADDITION now we need to make sure that what's constructed here
//is the same as what SetSize return. So easiest thing is to add:
  SetSize( _x , _y , _w , _h );
#endif
  Callback (Function);

  return TRUE;
}

wxText::~wxText (void)
{
#ifdef wx_motif
  if (internalTextValue)
    delete[] internalTextValue;
#endif
}

void wxText::ChangeColour (void)
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

char *wxText::GetValue (void)
{
#ifdef wx_motif
  // If a password widget, return the REAL
  // string value, not a useless string of asterisks.
  if (internalTextValue)
    return internalTextValue;

  char *s = XmTextGetString ((Widget) handle);
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

void wxText::SetValue (char *value)
{
#ifdef wx_motif
  inSetValue = TRUE;
  if (value)
    XmTextSetString ((Widget) handle, value);
  inSetValue = FALSE;
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) handle;
  xv_set (item, PANEL_VALUE, value, NULL);
#endif
}

void wxText::SetSize (int x, int y, int width, int height, int sizeFlags)
{
#ifdef wx_motif
  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_ANY, NULL);

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

  if (is_managed)
    XtManageChild (formWidget);

  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_NONE, NULL);

  GetEventHandler()->OnSize (width, height);
#endif
#ifdef wx_xview
  Xv_opaque x_win = (Xv_opaque) handle;
  if (x > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    (void) xv_set (x_win, XV_X, x, NULL);

  if (y > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    (void) xv_set (x_win, XV_Y, y, NULL);

  // If we're not prepared to use the existing width, then...
  if (!(width == -1 && ((sizeFlags & wxSIZE_AUTO_WIDTH) != wxSIZE_AUTO_WIDTH)))
  {
    int labelWidth = 0;
    int panelLayout = (int)xv_get(x_win, PANEL_LAYOUT);
    if (panelLayout == PANEL_HORIZONTAL)
    {
      labelWidth = (int)xv_get(x_win, PANEL_LABEL_WIDTH);

      labelWidth+=8; // TOMS FUDGE so that now SetSize(w,h) returns
      				 // the same thing as GetSize(w,h) 
      				 //to say the truth the getsize function should be changed
      				 //so that sizes are calculated in the same manner as here!!
      				 //this is just a quick mend which seems to work!
    }

    if (width == -1)
    {
      int ww, hh;
      GetSize (&ww, &hh);
      width = ww;
    }

    if (width <= 0)
      width = 80;

    (void) xv_set (x_win, PANEL_VALUE_DISPLAY_WIDTH, wxMax(width-labelWidth, 10), NULL);
  }
  GetEventHandler()->OnSize (width, height);
#endif
}

void wxText::SetClientSize (int width, int height)
{
#ifdef wx_motif
  Bool managed = XtIsManaged(formWidget);

  if (managed)
    XtUnmanageChild (formWidget);
  if (width > -1)
    XtVaSetValues ((Widget) handle, XmNwidth, width,
                   NULL);
  if (height > -1)
    XtVaSetValues ((Widget) handle, XmNheight, height,
                   NULL);
  if (managed)
    XtManageChild (formWidget);
#endif
#ifdef wx_xview
  Xv_opaque x_win = (Xv_opaque) handle;
  if (width == -1)
  {
    int ww, hh;
    GetSize (&ww, &hh);
    width = ww;
  }
  if (width < 10)
  {
    width = 10;
  }
  (void) xv_set (x_win, PANEL_VALUE_DISPLAY_WIDTH, width, 0);
#endif
  int new_width, new_height;
  GetSize(&new_width, &new_height);
  OnSize(new_width, new_height);
}
 
void wxText::SetFocus (void)
{
#ifdef wx_motif
  wxPanel *panel = (wxPanel *) GetParent();
  panel->manualChange = TRUE;
  wxItem::SetFocus();
  panel->manualChange = FALSE;
#endif
#ifdef wx_xview
  wxPanel *panel = (wxPanel *) GetParent ();
  if (panel)
    {
      Panel p = (Panel) panel->handle;
      xv_set (p, PANEL_CARET_ITEM, (Panel_item) handle, 0);
    }
#endif
}

// Clipboard operations
void wxText::Copy(void)
{
#ifdef wx_motif
  XmTextCopy((Widget)handle, CurrentTime);
#endif
}

void wxText::Cut(void)
{
#ifdef wx_motif
  XmTextCut((Widget)handle, CurrentTime);
#endif
}

void wxText::Paste(void)
{
#ifdef wx_motif
  XmTextPaste((Widget)handle);
#endif
}

void wxText::SetEditable(Bool editable)
{
#ifdef wx_motif
  Widget textWidget = (Widget)handle;
  XmTextSetEditable(textWidget, editable);
#endif
#ifdef wx_xview
  xv_set((Panel_item)handle, PANEL_READ_ONLY, !editable, NULL);
#endif
}

void wxText::SetInsertionPoint(long pos)
{
#ifdef wx_motif
  Widget textWidget = (Widget)handle;
  XmTextSetInsertionPosition (textWidget, (XmTextPosition) pos);
#endif
}

void wxText::SetInsertionPointEnd(void)
{
#ifdef wx_motif
  Widget textWidget = (Widget)handle;
  XmTextPosition pos = XmTextGetLastPosition (textWidget);
  XmTextSetInsertionPosition (textWidget, (XmTextPosition) (pos + 1));
#endif
}

long wxText::GetInsertionPoint(void)
{
#ifdef wx_motif
  Widget textWidget = (Widget) handle;
  return (long) XmTextGetInsertionPosition (textWidget);
#endif
#ifdef wx_xview
  return 0;
#endif
}

long wxText::GetLastPosition(void)
{
#ifdef wx_motif
  Widget textWidget = (Widget) handle;
  return (long) XmTextGetLastPosition (textWidget);
#endif
#ifdef wx_xview
  return 0;
#endif
}

void wxText::Replace(long from, long to, char *value)
{
#ifdef wx_motif
  Widget textWidget = (Widget) handle;
  XmTextReplace (textWidget, (XmTextPosition) from, (XmTextPosition) to,
		 value);
#endif
#ifdef wx_xview
#endif
}

void wxText::Remove(long from, long to)
{
#ifdef wx_motif
  Widget textWidget = (Widget) handle;
  XmTextSetSelection (textWidget, (XmTextPosition) from, (XmTextPosition) to,
		      (Time) 0);
  XmTextRemove (textWidget);
#endif
#ifdef wx_xview
#endif
}

void wxText::SetSelection(long from, long to)
{
#ifdef wx_motif
  Widget textWidget = (Widget) handle;
  XmTextSetSelection (textWidget, (XmTextPosition) from, (XmTextPosition) to,
		      (Time) 0);
#endif
#ifdef wx_xview
#endif
}

#ifdef wx_motif
void 
wxTextModifyProc (Widget w, XtPointer clientData, XmTextVerifyCallbackStruct *cbs)
{
  wxText *tw = (wxText *) clientData;

  if ((tw->GetWindowStyleFlag() & wxPASSWORD) != wxPASSWORD)
    return;

#if 0  // _sm_ -- Broken original.
  if (cbs->text->ptr == NULL)
  {
    // Backspace
    if (tw->internalTextValue)
    {
      cbs->endPos = strlen(tw->internalTextValue);
      tw->internalTextValue[cbs->startPos] = 0;
      return;
    }
    else return;
  }

  char *newS = new char[cbs->endPos + 2]; // new char + NULL
  if (tw->internalTextValue)
  {
    strcpy(newS, tw->internalTextValue);
    delete[] tw->internalTextValue;
  }
  else newS[0] = 0;

  tw->internalTextValue = newS;
  strncat(tw->internalTextValue, cbs->text->ptr, cbs->text->length);
  tw->internalTextValue[cbs->endPos + cbs->text->length] = 0;

  int i;
  for (i = 0; i < cbs->text->length; i ++)
    cbs->text->ptr[i] = '*';
#else  // _sm_ My stab at it.
  /*
   * The preceding code has a number of serious bugs, inherited from the
   * original code in The Motif Programming Manual vol 6 (by Dan Heller).
   *
   * At least on my system (SunOS 4.1.3 + Motif 1.2), you need to think of
   * every event as a replace event.  cbs->text->ptr gives the replacement
   * text, cbs->startPos gives the index of the first char affected by the
   * replace, and cbs->endPos gives the index one more than the last char
   * affected by the replace (startPos == endPos implies an empty range).
   * Hence, a deletion is represented by replacing all input text with a
   * blank string ("", *not* NULL!).  A simple insertion that does not
   * overwrite any text has startPos == endPos.
   */

  char * & passwd = tw->internalTextValue;  // Set up a more convenient alias.

  if (!passwd)
    passwd = copystring(cbs->text->ptr);
  else
  {
    int len = passwd ? strlen(passwd) : 0; // Enough room for old text
    len += strlen(cbs->text->ptr) + 1;     // + new text (if any) + NUL
    len -= cbs->endPos - cbs->startPos;    // - text from affected region.

    char * newS = new char [len];
    char * p = passwd, * dest = newS, * insert = cbs->text->ptr;

    // Copy (old) text from passwd, up to the start posn of the change.
    int i;
    for (i = 0; i < cbs->startPos; ++i)
      *dest++ = *p++;

    // Copy the text to be inserted).
    while (*insert)
      *dest++ = *insert++;

    // Finally, copy into newS any remaining text from passwd[endPos] on.
    for (p = passwd + cbs->endPos; *p; )
      *dest++ = *p++;
    *dest = 0;

    delete [] passwd;  passwd = newS;
  }

  if (cbs->text->length>0)
  {
    int i;
    for (i = 0; i < cbs->text->length; ++i)
      cbs->text->ptr[i] = '*';
    cbs->text->ptr[i] = 0;
  }
#endif
}
#endif
