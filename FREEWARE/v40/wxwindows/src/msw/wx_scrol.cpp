/*
 * File:        wx_scrol.cc
 * Purpose:     Scrolbar items implementation (MSW version)
 * Author:    Sergey Krasnov (ksa@orgland.ru)
 * Created:   1995
 * Updated:
 * Copyright:
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_defs.h"
#include "wx_utils.h"
#endif

#if USE_SCROLLBAR

#include "wx_scrol.h"
#include "wx_privt.h"
#include "wx_itemp.h"

#if CTL3D
#include <ctl3d.h>
#endif

extern wxList wxScrollBarList;
extern void wxFindMaxSize(HWND hwnd, RECT *rect);

// Scrollbar
wxScrollBar::wxScrollBar(void)
{
    wxWinType = wxTYPE_HWND;
    windows_id = 0;
    ms_handle = 0;
    pageSize = 0;
    viewSize = 0;
}

wxScrollBar::wxScrollBar(wxPanel *panel, wxFunction func,
                      int x, int y, int width, int height, long style, Constdata char *name):
    wxbScrollBar(panel, func, x, y, width, height, style, name)
{
    Create(panel, func, x, y, width, height, style, name);
}

Bool wxScrollBar::Create(wxPanel *panel, wxFunction func,
                int x, int y, int width, int height,
                long style, Constdata char *name)
{
    if (!panel)
        return FALSE;
    panel->AddChild(this);
    SetName(name);
    
    buttonFont = NULL;
    labelFont = NULL;
    SetBackgroundColour(*panel->GetBackgroundColour()) ;
    labelColour = panel->labelColour ;
    buttonColour = panel->buttonColour ;
    wxWinType = wxTYPE_HWND;
    windowStyle = style;
    window_parent = panel;
    labelPosition = panel->label_position;
    panel->GetValidPosition(&x, &y);
    
    if (width == -1)
    {
      if (style & wxHORIZONTAL)
        width = 140;
      else
        width = 14;
    }
    if (height == -1)
    {
      if (style & wxVERTICAL)
        height = 140;
      else
        height = 14;
    }

    // Now create scrollbar
    windows_id = (int)NewControlId();
     DWORD _direction = (style & wxHORIZONTAL) ?
                        SBS_HORZ: SBS_VERT;
    HWND scroll_bar = CreateWindowEx(0, "SCROLLBAR", "scrollbar",
                         _direction | WS_CHILD | WS_VISIBLE,
                         0, 0, 0, 0, panel->GetHWND(), (HMENU)windows_id,
                         wxhInstance, NULL);
#if CTL3D
    if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
      Ctl3dSubclassCtl(scroll_bar);
#endif

    wxScrollBarList.Append((long)scroll_bar, this);
    pageSize = 1;
    viewSize = 1;

    ::SetScrollRange(scroll_bar, SB_CTL, 0, 1, FALSE);
    ::SetScrollPos(scroll_bar, SB_CTL, 0, FALSE);
    ShowWindow(scroll_bar, SW_SHOW);

    ms_handle = (HANDLE)scroll_bar;

    // Subclass again for purposes of dialog editing mode
    SubclassControl(scroll_bar);

    SetSize(x, y, width, height);

    panel->AdvanceCursor(this);
    Callback(func);

    return TRUE;
}

wxScrollBar::~wxScrollBar(void)
{
    wxScrollBarList.DeleteObject(this);
}

// Called from wx_win.cc: wxWnd::OnHScroll, wxWnd::OnVScroll
void wxScrollBarEvent(HWND hbar, WORD wParam, WORD pos)
{
    wxNode *node = (wxNode *)wxScrollBarList.Find((long)hbar);
    if (!node)
      return;

    wxScrollBar *scrollBar = (wxScrollBar *)node->Data();
    int position = GetScrollPos(hbar, SB_CTL);

    int nScrollInc;
    switch ( wParam )
    {
            case SB_LINEUP:
                    nScrollInc = -1;
                    break;

            case SB_LINEDOWN:
                    nScrollInc = 1;
                    break;

            case SB_PAGEUP:
                    nScrollInc = -scrollBar->pageSize;
                    break;

            case SB_PAGEDOWN:
                    nScrollInc = scrollBar->pageSize;;
                    break;

            case SB_THUMBTRACK:
                    nScrollInc = pos - position;
                    break;

            default:
                    nScrollInc = 0;
    }

    if (nScrollInc != 0)
    {
        int new_pos = position + nScrollInc;
#ifdef __WIN386__
        short minPos, maxPos;
#else
        int minPos, maxPos;
#endif
        GetScrollRange(hbar, SB_CTL, &minPos, &maxPos);
#if WIN95
		// A page size greater than one has the effect of reducing the
		// effective range, therefore the range has already been
		// boosted artificially - so reduce it again.
		if ( scrollBar->pageSize > 1 )
			maxPos -= (scrollBar->pageSize - 1);
#endif
        if (new_pos < 0)
            new_pos = 0;
        if (new_pos > maxPos)
            new_pos = maxPos;

        scrollBar->SetValue(new_pos);
        wxCommandEvent event(wxEVENT_TYPE_SCROLLBAR_COMMAND);
        event.commandInt = new_pos;
        event.eventObject = scrollBar;
        scrollBar->ProcessCommand(event);
    }
}

void wxScrollBar::SetValue(int viewStart)
{
#if WIN95
  SCROLLINFO info;
  info.cbSize = sizeof(SCROLLINFO);
  info.nPage = 0;
  info.nMin = 0;
  info.nPos = viewStart;
  info.fMask = SIF_POS ;

  ::SetScrollInfo((HWND) GetHWND(), SB_CTL, &info, TRUE);
#else
  ::SetScrollPos((HWND) GetHWND(), SB_CTL, viewStart, TRUE);
#endif
}

int wxScrollBar::GetValue(void)
{
    return ::GetScrollPos((HWND)ms_handle, SB_CTL);
}

/* From the WIN32 documentation:
In version 4.0 or later, the maximum value that a scroll bar can report
(that is, the maximum scrolling position) depends on the page size.
If the scroll bar has a page size greater than one, the maximum scrolling position
is less than the maximum range value. You can use the following formula to calculate
the maximum scrolling position:
  
MaxScrollPos = MaxRangeValue - (PageSize - 1) 
*/
void wxScrollBar::SetPageLength(int pageLength)
{
  pageSize = pageLength;

#if WIN95
  SCROLLINFO info;
  info.cbSize = sizeof(SCROLLINFO);
  info.nPage = pageLength;
  info.fMask = SIF_PAGE ;

  ::SetScrollInfo((HWND) GetHWND(), SB_CTL, &info, TRUE);
#endif
}

void wxScrollBar::SetObjectLength(int objectLength)
{
  // The range (number of scroll steps) is the
  // object length minus the view size.
  int range = wxMax((objectLength - viewSize), 0) ;

#if WIN95
  // Try to adjust the range to cope with page size > 1
  // (see comment for SetPageLength)
  if ( pageSize > 1 )
  {
	range += (pageSize - 1);
  }

  SCROLLINFO info;
  info.cbSize = sizeof(SCROLLINFO);
  info.nPage = 0;
  info.nMin = 0;
  info.nMax = range;
  info.nPos = 0;
  info.fMask = SIF_RANGE ;

  ::SetScrollInfo((HWND) GetHWND(), SB_CTL, &info, TRUE);
#else
  ::SetScrollRange((HWND) GetHWND(), SB_CTL, 0, range, TRUE);
#endif
}

void wxScrollBar::SetViewLength(int viewLength)
{
    viewSize = viewLength;
}


void wxScrollBar::GetValues(int *viewStart, int *viewLength, int *objectLength,
           int *pageLength)
{
    *viewStart = ::GetScrollPos((HWND)ms_handle, SB_CTL);
    *viewLength = viewSize;
#ifdef __WIN386__
    short minPos;
    short objectLen;
    ::GetScrollRange((HWND)ms_handle, SB_CTL, &minPos, &objectLen);
    *objectLength = objectLen;
#else
    int minPos;
    ::GetScrollRange((HWND)ms_handle, SB_CTL, &minPos, objectLength);
#endif
    *pageLength = pageSize;
}

char *wxScrollBar::GetLabel(void)
{
    return NULL;
}

void wxScrollBar::SetLabel(char *WXUNUSED(label))
{
}

#endif
