/*
 * File:	wx_scrol.cc
 * Purpose:	Scrolbar items implementation (X version)
 * Author:  Sergey Krasnov (ksa@orgland.ru)
 * Created:	1994
 * Updated:
 * Copyright:	(c) 1994
 */

static const char sccsid[] = "%W% %G%";

#ifdef __GNUG__
#pragma implementation
#endif

#include "wx_defs.h"

#if USE_SCROLLBAR

#include "wx_stdev.h"
#include "wx_item.h"
#include "wx_scrol.h"
#include "wx_dialg.h"
#include "wx_cmdlg.h"
#include "wx_utils.h"
#include "wx_privt.h"

#ifdef wx_motif
#include <X11/IntrinsicP.h>
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrollBar.h>
extern int wxComputeColors(wxColour *backColour,wxColour *buttonColour)  ;
#endif

#ifdef wx_xview
#include <xview/panel.h>
#include <xview/scrollbar.h>
#include <xview/notify.h>
#endif

// Scroll Bar

#ifdef wx_xview
void scrollbar_compute_scroll_proc(Scrollbar scrollBar, int pos,
					int length, Scroll_motion motion,
				     unsigned long *offset,
				     unsigned long *object_length)
{
	int viewStart = xv_get(scrollBar, SCROLLBAR_VIEW_START);
	int viewLength = xv_get(scrollBar, SCROLLBAR_VIEW_LENGTH);
	int objectLength = xv_get(scrollBar, SCROLLBAR_OBJECT_LENGTH);
	int pixelsPerUnit = xv_get(scrollBar, SCROLLBAR_PIXELS_PER_UNIT);
 	int pageLength = xv_get(scrollBar, SCROLLBAR_PAGE_LENGTH);

    pos =  pos * (objectLength - viewLength) / length;

/*	
    unfortunatly scrollbar_default_compute_scroll_proc can't be used
	scrollbar_default_compute_scroll_proc(scrollBar, pos, length, motion,
                                      offset, object_length);
*/
    int minus_movement;
    switch (motion) {
        case SCROLLBAR_ABSOLUTE:
            viewStart = pos;
            break;

        case SCROLLBAR_POINT_TO_MIN:
            viewStart += pos;
            break;

        case SCROLLBAR_MIN_TO_POINT:
            if (pos > viewStart)
                viewStart = 0;
            else
                viewStart -= pos;
            break;

        case SCROLLBAR_PAGE_FORWARD:
            viewStart += pageLength;
            break;

        case SCROLLBAR_PAGE_BACKWARD:
            minus_movement = pageLength;
            if (minus_movement > viewStart)
                viewStart= 0;
            else
                viewStart -= minus_movement;
            break;

        case SCROLLBAR_LINE_FORWARD:
            viewStart ++;
            break;

        case SCROLLBAR_LINE_BACKWARD:
            if (viewStart > 0)
                viewStart -= pixelsPerUnit;
            break;

        case SCROLLBAR_TO_END:
            viewStart = objectLength - viewLength;
            break;

        case SCROLLBAR_TO_START:
            viewStart = 0;
            break;

        default:
            break;
    }

	*offset = viewStart;
    *object_length = objectLength;
}

void wxScrollBarProc(Panel_item item, int value, Event *x_event)
{
    wxScrollBar *scrollBar = (wxScrollBar *)xv_get(item, WIN_CLIENT_DATA);
    wxCommandEvent event(wxEVENT_TYPE_SCROLLBAR_COMMAND);
    event.commandInt = value;
    event.eventHandle = (char *)x_event;
    event.eventObject = scrollBar;
    if (scrollBar)
      scrollBar->ProcessCommand(event);
}

Notify_value scrollBar_handle_event(Panel_item panel_item, Event *event,
                            Scrollbar scrollBar, Notify_event_type type)
{
   	if (event_id(event) == SCROLLBAR_REQUEST) {
    	int start_view = (int)xv_get(scrollBar, SCROLLBAR_VIEW_START);
		wxScrollBarProc(scrollBar, start_view, event);
	}
	return NOTIFY_DONE;
}
#endif

#ifdef wx_motif
void wxScrollBarCallback(Widget widget, XtPointer clientData,
                        XmScaleCallbackStruct *cbs)
{
    wxScrollBar *scrollBar = (wxScrollBar *)clientData;
    wxCommandEvent event(wxEVENT_TYPE_SCROLLBAR_COMMAND);
    XtVaGetValues(widget, XmNvalue, &event.commandInt, NULL);
    event.eventHandle = (char *)cbs->event;
    event.eventObject = scrollBar;
    if (!scrollBar->inSetValue)
      scrollBar->ProcessCommand(event);
}
#endif

wxScrollBar::wxScrollBar(void)
{
}

wxScrollBar::wxScrollBar(wxPanel *panel, wxFunction func,
                      int x, int y, int width, int height, long style, char *name):
	wxbScrollBar(panel, func, x, y, width, height, style, name)
{
	Create(panel, func, x, y, width, height, style, name);
}

Bool wxScrollBar::Create(wxPanel *panel, wxFunction func,
                    int x, int y, int width, int height,
                    long style, char *name)
{
    if (!panel)
        return FALSE;
    SetName(name);
    panel->AddChild(this);
/*
    buttonFont = panel->buttonFont ;
    labelFont = panel->labelFont ;
*/
    buttonFont = NULL;
    labelFont = NULL;

    backColour = panel->backColour ;
    labelColour = panel->labelColour ;
    buttonColour = panel->buttonColour ;
    window_parent = panel;
    labelPosition = panel->label_position;

    if (width == -1)
    {
      if (style & wxHORIZONTAL)
        width = 140;
      else
        width = 12;
    }
    if (height == -1)
    {
      if (style & wxVERTICAL)
        height = 140;
      else
        height = 12;
    }

#ifdef wx_motif
    Widget panelForm = panel->panelWidget;

    formWidget = XtVaCreateManagedWidget(windowName,
                  xmRowColumnWidgetClass, panelForm,
                  XmNorientation, XmHORIZONTAL,
                  XmNmarginHeight, 0,
                  XmNmarginWidth, 0,
                  NULL);

    int _direction = (style & wxHORIZONTAL) ? XmHORIZONTAL: XmVERTICAL;

    Widget scrollBarWidget = XtVaCreateManagedWidget("scrollBarWidget",
                  xmScrollBarWidgetClass,  formWidget,
                  XmNorientation,      _direction,
                  NULL);
/*
    if (buttonFont)
        XtVaSetValues(scrollBarWidget,
                  XmNfontList, buttonFont->GetInternalFont(XtDisplay(formWidget)),
                  NULL) ;
*/
    handle = (char *)scrollBarWidget;
    wxAddWindowToTable(scrollBarWidget, this);

    XtAddCallback(scrollBarWidget, XmNvalueChangedCallback, (XtCallbackProc)wxScrollBarCallback, (XtPointer)this);
    XtAddCallback(scrollBarWidget, XmNdragCallback, (XtCallbackProc)wxScrollBarCallback, (XtPointer)this);

    panel->AttachWidget(this, formWidget, x, y, width, height);
    ChangeColour();

    /* After creating widgets, no more resizes. */
    if (style&wxFIXED_LENGTH)
        XtVaSetValues(formWidget,
                  XmNpacking,XmPACK_NONE,
                  NULL);
#endif
#ifdef wx_xview
    Panel x_panel = (Panel)(panel->GetHandle());
    Scrollbar x_scrollBar = 0;
    int _direction = (style & wxHORIZONTAL) ? SCROLLBAR_HORIZONTAL: SCROLLBAR_VERTICAL;

/*
    int label_position;
    if (panel->label_position == wxVERTICAL)
        label_position = PANEL_VERTICAL;
    else
        label_position = PANEL_HORIZONTAL;
*/

    if (panel->new_line)
        wxMessageBox("New line isn't implemented for Scroll Bar in XView", "Warning");

    if (x > -1 && y > -1)
        x_scrollBar = (Scrollbar) xv_create(x_panel, SCROLLBAR,
                            SCROLLBAR_DIRECTION, _direction,
                            XV_X, x,
                            XV_Y, y,
                            NULL);
    else
        x_scrollBar = (Scrollbar) xv_create(x_panel, SCROLLBAR,
                            SCROLLBAR_DIRECTION, _direction,
              				NULL);

    xv_set(x_scrollBar,     XV_SHOW, TRUE,
  							WIN_CLIENT_DATA, (char *)this,
							SCROLLBAR_NOTIFY_CLIENT, (Notify_client) &x_scrollBar,
							SCROLLBAR_COMPUTE_SCROLL_PROC, scrollbar_compute_scroll_proc,
                            NULL);

    if ((style & wxHORIZONTAL) && width > 0)
        xv_set(x_scrollBar, XV_WIDTH, (int) width, NULL);
    if ((style & wxVERTICAL) && height > 0)
        xv_set(x_scrollBar, XV_HEIGHT, (int) height, NULL);

    handle = (char *)x_scrollBar;
    notify_set_event_func( (Notify_client) &x_scrollBar,
			(Notify_func) scrollBar_handle_event, NOTIFY_SAFE);
#endif

    Callback(func);
    return TRUE;
}


wxScrollBar::~wxScrollBar(void)
{
}

void wxScrollBar::ChangeColour(void)
{
#ifdef wx_motif

    wxPanel *panel = (wxPanel*)window_parent ;
    if (!(panel->GetWindowStyleFlag() & wxUSER_COLOURS))
        return;                                         
    int change = wxComputeColors(XtDisplay(formWidget), panel->GetBackgroundColour(), panel->GetButtonColour());
    if (change==wxBACK_COLORS)
        XtVaSetValues(formWidget,
                  XmNbackground,        itemColors[wxBACK_INDEX].pixel,
                  XmNtopShadowColor,    itemColors[wxTOPS_INDEX].pixel,
                  XmNbottomShadowColor, itemColors[wxBOTS_INDEX].pixel,
                  XmNforeground,        itemColors[wxFORE_INDEX].pixel,
                  NULL) ;
    else if (change==wxFORE_COLORS)
        XtVaSetValues(formWidget,
                  XmNforeground,        itemColors[wxFORE_INDEX].pixel,
                  NULL) ;

    change = wxComputeColors(XtDisplay(formWidget), GetBackgroundColour(),GetButtonColour())  ;
    if (change==wxBACK_COLORS)
        XtVaSetValues((Widget)handle,
                  XmNbackground,        itemColors[wxBACK_INDEX].pixel,
                  XmNtopShadowColor,    itemColors[wxTOPS_INDEX].pixel,
                  XmNbottomShadowColor, itemColors[wxBOTS_INDEX].pixel,
                  XmNarmColor,          itemColors[wxSELE_INDEX].pixel,
                  XmNforeground,        itemColors[wxFORE_INDEX].pixel,
                  NULL) ;
    else if (change==wxFORE_COLORS)
        XtVaSetValues((Widget)handle,
                  XmNforeground,        itemColors[wxFORE_INDEX].pixel,
                  NULL) ;
#endif
}


void wxScrollBar::GetSize(int *width, int *height)
{
#ifdef wx_motif
	wxItem::GetSize(width, height);
#endif
#ifdef wx_xview
	Scrollbar x_scrollBar = (Scrollbar) handle;
	*width = xv_get(x_scrollBar, XV_WIDTH);
	*height = xv_get(x_scrollBar, XV_HEIGHT);
#endif
}

void wxScrollBar::SetSize(int x, int y, int width, int height, int sizeFlags)
{
#ifdef wx_motif
    Widget scrollBarWidget = (Widget)handle;
 	if (x > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
		XtVaSetValues(scrollBarWidget, XmNx, x, NULL);
  	if (y> -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
   		 XtVaSetValues(scrollBarWidget, XmNy, y, NULL);
  	if (width > -1)
    	XtVaSetValues(scrollBarWidget, XmNwidth, width, NULL);
  	if (height > -1)
    	XtVaSetValues(scrollBarWidget, XmNheight, height, NULL);
#endif
    wxItem::SetSize(x, y, width, height, sizeFlags);
}

void wxScrollBar::SetValue(int viewStart)
{
#ifdef wx_motif
  inSetValue = TRUE;
  XtVaSetValues((Widget)handle, XmNvalue, viewStart, NULL);
  inSetValue = FALSE;
#endif
#ifdef wx_xview
  xv_set((Scrollbar) handle, SCROLLBAR_VIEW_START, viewStart, NULL);
#endif
}

int wxScrollBar::GetValue(void)
{
#ifdef wx_motif
  int viewStart;
  XtVaGetValues((Widget)handle, XmNvalue, &viewStart, NULL);
  return viewStart;
#endif
#ifdef wx_xview
	return xv_get((Scrollbar) handle, SCROLLBAR_VIEW_START);
#endif
}

void wxScrollBar::SetPageLength(int pageLength)
{
#ifdef wx_motif
  XtVaSetValues((Widget)handle, XmNpageIncrement, pageLength, NULL);
#endif
#ifdef wx_xview
  xv_set((Scrollbar) handle, SCROLLBAR_PAGE_LENGTH, pageLength, NULL);
#endif
}

void wxScrollBar::SetObjectLength(int objectLength)
{
#ifdef wx_motif
  XtVaSetValues((Widget)handle, XmNmaximum, objectLength, NULL);
#endif
#ifdef wx_xview
  xv_set((Scrollbar) handle, SCROLLBAR_OBJECT_LENGTH, objectLength, NULL);
#endif
}

void wxScrollBar::SetViewLength(int viewLength)
{
#ifdef wx_motif
  XtVaSetValues((Widget)handle, XmNsliderSize, viewLength, NULL);
#endif
#ifdef wx_xview
  xv_set((Scrollbar) handle, SCROLLBAR_VIEW_LENGTH, viewLength, NULL);
#endif
}

void wxScrollBar::GetValues(int *viewStart, int *viewLength, int *objectLength,
//              	int *pixelsPerUnit,
				int *pageLength)
{
#ifdef wx_motif
    XtVaGetValues((Widget)handle,
			XmNvalue, viewStart,
			XmNsliderSize, viewLength,
 			XmNmaximum, objectLength,
//			XmNincrement, pixelsPerUnit,
			XmNpageIncrement, pageLength,
			NULL);
#endif
#ifdef wx_xview
	*viewStart = xv_get((Scrollbar) handle, SCROLLBAR_VIEW_START);
	*viewLength = xv_get((Scrollbar) handle, SCROLLBAR_VIEW_LENGTH);
	*objectLength = xv_get((Scrollbar) handle, SCROLLBAR_OBJECT_LENGTH);
// 	*pixelsPerUnit = xv_get((Scrollbar) handle, SCROLLBAR_PIXELS_PER_UNIT);
 	*pageLength = xv_get((Scrollbar) handle, SCROLLBAR_PAGE_LENGTH);
#endif
}

/*
void wxScrollBar::SetPixelsPerUnit(int pixelsPerUnit)
{
#ifdef wx_motif
    XtVaSetValues((Widget)handle, XmNincrement, pixelsPerUnit, NULL);
#endif
#ifdef wx_xview
	xv_set((Scrollbar) handle, SCROLLBAR_PIXELS_PER_UNIT, pixelsPerUnit, NULL);
#endif
}
*/

void wxScrollBar::Enable(Bool enable)
{
#ifdef wx_motif
    wxbScrollBar::Enable(enable);
#endif
#ifdef wx_xview
    Scrollbar window = (Scrollbar)handle;
    if (window)
        xv_set(window, SCROLLBAR_INACTIVE, !enable, NULL);
#endif
}

Bool wxScrollBar::Show (Bool show)
{
  return wxItem::Show(show);
}

char * wxScrollBar::GetLabel(void)
{
    return NULL;
}

void wxScrollBar::SetLabel(char *label)
{
}

#endif
