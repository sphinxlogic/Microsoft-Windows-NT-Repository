/*
 * File:	wb_scrol.cc
 * Purpose: Scrollbar items implementation
 * Author:  Sergey Krasnov (ksa@orgland.ru)
 * Created: 1995
 * Updated:
 * Copyright:
 */

/* static const char sccsid[] = "%W% %G%"; */

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_defs.h"
#endif

#if USE_SCROLLBAR

#include "wx_stdev.h"
#include "wx_scrol.h"

wxbScrollBar::wxbScrollBar(void)
{
    WXSET_TYPE(wxScrollBar, wxTYPE_SCROLL_BAR)
    
    buttonFont = NULL ;
    labelFont = NULL ;
}

wxbScrollBar::wxbScrollBar(wxPanel *panel, wxFunction func,
			int x, int y, int width, int height, long style, Constdata char *name)
{
    WXSET_TYPE(wxScrollBar, wxTYPE_SCROLL_BAR)
    
    if (!panel)
        return;
    window_parent = panel;
    labelPosition = panel->label_position;
    buttonFont = panel->buttonFont ;
    labelFont = panel->labelFont ;
}

wxbScrollBar::~wxbScrollBar(void)
{
}

void wxbScrollBar::Command(wxCommandEvent& event)
{
    SetValue(event.commandInt);
    ProcessCommand(event);
}

void wxbScrollBar::ProcessCommand(wxCommandEvent& event)
{
    if (wxNotifyEvent(event, TRUE))
        return;

    wxFunction fun = callback;
    if (fun) {
        (void)(*(fun))(*this, event);
    }
    wxNotifyEvent(event, FALSE);
}

#endif
