/*
 * File:        wx_mtxt.cc
 * Purpose:     Multi-line text item implementation (X version)
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	April 1995August 1994
 * Copyright:   (c) 1995, AIAI, University of Edinburgh
 */

static const char sccsid[] = "@(#)wx_item.cc	1.2 5/9/94";

#ifdef __GNUG__
#pragma implementation
#pragma implementation "wx_mtxt.h"
#endif

#include <stdlib.h>
#include "wx_defs.h"
#include "wx_utils.h"
#include "wx_privt.h"
#include "wx_mtxt.h"

#ifdef wx_motif
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/Form.h>

extern void wxTextCallback(Widget w, XtPointer clientData,
                      XmAnyCallbackStruct *ptr);
extern void clientMsgHandler(Widget w, XtPointer cdata, XEvent *event);
extern void keyPressHandler(Widget w, XtPointer cData, XEvent *event);

extern void wxTextModifyProc (Widget w, XtPointer clientData, XmTextVerifyCallbackStruct *cbs);
#endif

#ifdef wx_xview
extern Panel_setting wxTextProc (Panel_item item, Event * x_event);
#endif

// Multi-line Text item
wxMultiText::wxMultiText (void)
{
}

wxMultiText::wxMultiText (wxPanel * panel, wxFunction Function, char *label, char *value,
	     int x, int y, int width, int height, long style, char *name):
wxbMultiText (panel, Function, label, value, x, y, width, height, style, name)
{
  Create (panel, Function, label, value, x, y, width, height, style, name);
}

Bool wxMultiText::
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
  if (height == -1)
    height = 100;

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

  if (label)
    {
      char buf[400];
      (void)wxStripMenuCodes(label, buf);

      char *the_label = (style & wxFIXED_LENGTH) ? fillCopy (buf) : copystring (buf);

      XmString text = XmStringCreateSimple (the_label);
      labelWidget = XtVaCreateManagedWidget (buf,
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

  Bool wantHorizScrolling = ((windowStyle & wxHSCROLL) != 0);
  // If we don't have horizontal scrollbars, we want word wrap.
  Bool wantWordWrap = !wantHorizScrolling;

  Arg args[2];
  XtSetArg (args[0], XmNscrollHorizontal, wantHorizScrolling ? True : False);
  XtSetArg (args[1], XmNwordWrap, wantWordWrap ? True : False);

  Widget textWidget = XmCreateScrolledText (formWidget, "multiTextWidget",
     args, 2);

  if (buttonFont)
    XtVaSetValues (textWidget,
		   XmNfontList, buttonFont->GetInternalFont (XtDisplay(formWidget)),
		   NULL);

  handle = (char *) textWidget;
  wxAddWindowToTable(textWidget, this);

  if (windowStyle & wxREADONLY)
    XmTextSetEditable(textWidget, False);

  XtVaSetValues (textWidget,
		 XmNeditMode, XmMULTI_LINE_EDIT,
		 NULL);
  XtManageChild (textWidget);

  if (GetLabelPosition() == wxHORIZONTAL)
    {
      if (labelWidget)
	XtVaSetValues (labelWidget,
		       XmNtopAttachment, XmATTACH_FORM,
		       XmNleftAttachment, XmATTACH_FORM,
		       XmNalignment, XmALIGNMENT_BEGINNING,
		       NULL);

      XtVaSetValues (XtParent (textWidget),
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

      XtVaSetValues (XtParent (textWidget),
	    XmNtopAttachment, labelWidget ? XmATTACH_WIDGET : XmATTACH_FORM,
		     XmNtopWidget, labelWidget ? labelWidget : formWidget,
		     XmNbottomAttachment, XmATTACH_FORM,
		     XmNleftAttachment, XmATTACH_FORM,
		     XmNrightAttachment, XmATTACH_FORM,
		     XmNalignment, XmALIGNMENT_BEGINNING,
		     NULL);
    }

  XtAddCallback (textWidget, XmNactivateCallback, (XtCallbackProc) wxTextCallback,
		 (XtPointer) this);

  XtAddCallback (textWidget, XmNvalueChangedCallback, (XtCallbackProc) wxTextCallback,
		 (XtPointer) this);

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
  Panel x_panel = (Panel) (panel->GetHandle ());
  Panel_item x_text;

  int label_position;
  if (GetLabelPosition() == wxVERTICAL)
    label_position = PANEL_VERTICAL;
  else
    label_position = PANEL_HORIZONTAL;

  if (panel->new_line)
    {
      x_text = (Panel_item) xv_create (x_panel, PANEL_MULTILINE_TEXT,
				       PANEL_VALUE_DISPLAY_WIDTH, 120,
		    PANEL_LAYOUT, label_position, PANEL_NEXT_ROW, -1, NULL);
      panel->new_line = FALSE;
    }
  else
    x_text = (Panel_item) xv_create (x_panel, PANEL_MULTILINE_TEXT,
				     PANEL_VALUE_DISPLAY_WIDTH, 120,
				     PANEL_LAYOUT, label_position, NULL);

  xv_set (x_text,
	  PANEL_NOTIFY_PROC, wxTextProc,
	  PANEL_NOTIFY_LEVEL, PANEL_ALL,
	  PANEL_CLIENT_DATA, (char *) this,
	  NULL);

/*
   if (buttonFont)
   xv_set(x_text, XV_FONT, buttonFont->GetInternalFont(), NULL) ;
 */

  if (label)
    {
      actualLabel = wxStripMenuCodes(label);
      if (style & wxFIXED_LENGTH)
	{
	  char *the_label = fillCopy (actualLabel);

	  xv_set (x_text, PANEL_LABEL_STRING, the_label, NULL);

	  int label_x = (int) xv_get (x_text, PANEL_LABEL_X);
	  int item_x = (int) xv_get (x_text, PANEL_ITEM_X);
	  xv_set (x_text, PANEL_LABEL_STRING, actualLabel,
		  PANEL_LABEL_X, label_x,
		  PANEL_ITEM_X, item_x,
		  NULL);
          delete[] the_label;
	}
      else
	xv_set (x_text, PANEL_LABEL_STRING, actualLabel, NULL);
    }

  handle = (char *) x_text;

  SetSize(x, y, width, height);

  if (value)
    {
      xv_set (x_text, PANEL_VALUE, value, NULL);
    };

#endif
  Callback (Function);
  return TRUE;
}

void wxMultiText::ChangeColour (void)
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
		     NULL);
      Widget parent = XtParent ((Widget) handle);
      XtVaSetValues (parent,
		     XmNbackground, itemColors[wxBACK_INDEX].pixel,
		     XmNtopShadowColor, itemColors[wxTOPS_INDEX].pixel,
		     XmNbottomShadowColor, itemColors[wxBOTS_INDEX].pixel,
		     XmNarmColor, itemColors[wxSELE_INDEX].pixel,
		     XmNforeground, itemColors[wxFORE_INDEX].pixel,
		     NULL);
      Widget hsb, vsb;
      XtVaGetValues (parent,
		     XmNhorizontalScrollBar, &hsb,
		     XmNverticalScrollBar, &vsb,
		     NULL);
      if (hsb)
	XtVaSetValues (hsb,
		       XmNbackground, itemColors[wxBACK_INDEX].pixel,
		       XmNtopShadowColor, itemColors[wxTOPS_INDEX].pixel,
		       XmNbottomShadowColor, itemColors[wxBOTS_INDEX].pixel,
		       XmNarmColor, itemColors[wxSELE_INDEX].pixel,
		       XmNforeground, itemColors[wxFORE_INDEX].pixel,
		       NULL);
      if (vsb)
	XtVaSetValues (vsb,
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
		     NULL);
      Widget parent = XtParent ((Widget) handle);
      XtVaSetValues (parent,
		     XmNforeground, itemColors[wxFORE_INDEX].pixel,
		     NULL);
      Widget hsb, vsb;
      XtVaGetValues (parent,
		     XmNhorizontalScrollBar, &hsb,
		     XmNverticalScrollBar, &vsb,
		     NULL);
      if (hsb)
	XtVaSetValues (hsb,
		       XmNforeground, itemColors[wxFORE_INDEX].pixel,
		       NULL);
      if (vsb)
	XtVaSetValues (vsb,
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
		       XmNforeground, itemColors[wxFORE_INDEX].pixel,
		       NULL);
      else if (change == wxFORE_COLORS)
	XtVaSetValues (labelWidget,
		       XmNforeground, itemColors[wxFORE_INDEX].pixel,
		       NULL);
    }
#endif
}

char *wxMultiText::GetValue (void)
{
#ifdef wx_motif
  return wxText::GetValue ();
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) handle;
  return (char *) xv_get (item, PANEL_VALUE);
#endif
}

void wxMultiText::GetValue (char *buffer, int maxSize)
{
  buffer[0] = 0;
#ifdef wx_motif
  char *s = XmTextGetString ((Widget) handle);
  if (s)
    {
      if (strlen (s) > (maxSize - 1))
	{
	  strncpy (buffer, s, maxSize - 1);
	  buffer[maxSize - 1] = 0;
	}
      else
	strcpy (buffer, s);
      XtFree (s);
    }
  else
    buffer[0] = 0;
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) handle;
  char *s = (char *) xv_get (item, PANEL_VALUE);
  int len = strlen(s);
  int actualLen = wxMin(len, (maxSize-1));
  strncpy (buffer, s, actualLen);
  buffer[len] = 0;
#endif
}

void wxMultiText::SetSize (int x, int y, int width, int height, int sizeFlags)
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

/**** Do we need these lines? JACS 19/3/95. If not after a month or so,
 * can delete. But they're not used in this function. Should they be?
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
*/

  if (width > -1)
    {
      // width - 10 is a fudge factor for taking the scroll bar into account.
      // A better way anybody?
      XtVaSetValues (formWidget, XmNwidth, wxMax(10, width - 10), XmNrightAttachment, XmATTACH_SELF, NULL);
    }
  if (height > -1)
    {
      XtVaSetValues (formWidget, XmNheight, wxMax(10, height), XmNbottomAttachment, XmATTACH_SELF, NULL);
    }

  if (is_managed)
    XtManageChild (formWidget);

  XtVaSetValues(formWidget, XmNresizePolicy, XmRESIZE_NONE, NULL);

  GetEventHandler()->OnSize (width, height);
#endif
#ifdef wx_xview
  Xv_opaque x_win = (Xv_opaque) handle;
  int panelLayout = (int)xv_get(x_win, PANEL_LAYOUT);
  if (x > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    (void) xv_set (x_win, XV_X, x, NULL);

  if (y > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    (void) xv_set (x_win, XV_Y, y, NULL);

  // If we're not prepared to use the existing width, then...
  if (!(width == -1 && ((sizeFlags & wxSIZE_AUTO_WIDTH) != wxSIZE_AUTO_WIDTH)))
  {
    int labelWidth = 0;
    if (panelLayout == PANEL_HORIZONTAL)
      labelWidth = (int)xv_get(x_win, PANEL_LABEL_WIDTH);

    if (width == -1)
      width = 120;
    (void) xv_set (x_win, PANEL_VALUE_DISPLAY_WIDTH, wxMax((width-labelWidth), 10), NULL);
  }

  // If we're not prepared to use the existing height, then...
  if (!(height == -1 && ((sizeFlags & wxSIZE_AUTO_HEIGHT) != wxSIZE_AUTO_HEIGHT)))
  {
    Panel panel = (Panel) (GetParent ()->handle);
    Xv_Font theFont = (Xv_Font) xv_get (panel, XV_FONT);
    Xv_Font theLabelFont = (Xv_Font) xv_get (x_win, PANEL_LABEL_FONT);
    int pixel_height = (int) xv_get (theFont, FONT_DEFAULT_CHAR_HEIGHT);
    int labelHeight =  (int) xv_get (theLabelFont, FONT_DEFAULT_CHAR_HEIGHT);

    if (height == -1)
      height = 100;

    // Subtract the label height if we're in vertical label mode.
    int actual_height = height;
    if (panelLayout == PANEL_VERTICAL)
      actual_height -= labelHeight;

    if (actual_height > -1)
      (void) xv_set (x_win, PANEL_DISPLAY_ROWS, (int) (actual_height / pixel_height), NULL);
  }
  GetEventHandler()->OnSize (width, height);
#endif
}

long wxMultiText::XYToPosition(long x, long y)
{
#ifdef wx_motif
/* It seems, that there is a bug in some versions of the Motif library,
   so the original wxWin-Code doesn't work. */
/*
  Widget textWidget = (Widget) handle;
  return (long) XmTextXYToPos ((Widget)handle, (Position) x, (Position) y);
*/
/* Now a little workaround: */
  long r=0;
  for (int i=0; i<y; i++) r+=(GetLineLength(i)+1);
  return r+x; 

#endif
#ifdef wx_xview
  return 0;
#endif
}

void wxMultiText::PositionToXY(long pos, long *x, long *y)
{
#ifdef wx_motif
  Position xx;
  Position yy;
  Widget textWidget = (Widget)handle;
  XmTextPosToXY(textWidget, pos, &xx, &yy);
  *x = xx;
  *y = yy;
#endif
#ifdef wx_xview
  *x = 0; *y = 0;
#endif
}

void wxMultiText::ShowPosition(long pos)
{
#ifdef wx_motif
  Widget textWidget = (Widget) handle;
  XmTextShowPosition (textWidget, (XmTextPosition) pos);
#endif
#ifdef wx_xview
#endif
}

int wxMultiText::GetLineLength(long lineNo)
{
#ifdef wx_motif
  GetLineText (lineNo, wxBuffer);
  return strlen (wxBuffer);
#endif
#ifdef wx_xview
  return 0;
#endif
}

int wxMultiText::GetLineText(long lineNo, char *buf)
{
#ifdef wx_motif
  Widget textWidget = (Widget) handle;
  // HIDEOUSLY inefficient, but we have no choice.
  char *s = XmTextGetString (textWidget);
  if (s)
    {
      long i = 0;
      int currentLine = 0;
      Bool finished = FALSE;
      while (!finished)
	{
	  int ch = s[i];
	  if (ch == '\n')
	    {
	      currentLine++;
	      i++;
	    }
	  else if (ch == 0)
	    {
	      finished = TRUE;
	    }
	  else
	    i++;
	}

      XtFree (s);
      return currentLine;
    }
  return 0;
#endif
#ifdef wx_xview
  buf[0] = 0;
  return 0;
#endif
}

int wxMultiText::GetNumberOfLines(void)
{
#ifdef wx_motif
  Widget textWidget = (Widget) handle;
  // HIDEOUSLY inefficient, but we have no choice.
  char *s = XmTextGetString (textWidget);
  if (s)
    {
      long i = 0;
      int currentLine = 0;
      Bool finished = FALSE;
      while (!finished)
	{
	  int ch = s[i];
	  if (ch == '\n')
	    {
	      currentLine++;
	      i++;
	    }
	  else if (ch == 0)
	    {
	      finished = TRUE;
	    }
	  else
	    i++;
	}

      XtFree (s);
      return currentLine;
    }
  return 0;
#endif
#ifdef wx_xview
  return 0;
#endif
}

