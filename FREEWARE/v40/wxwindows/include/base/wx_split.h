/*
 * File:	wxplit.h
 * Purpose:	wxSplitterWindow implementation
 * Author:	Julian Smart
 * Created:	1997
 * Copyright:
 */

#ifndef __SPLITTERWND__
#define __SPLITTERWND__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_frame.h"
#include "wx_panel.h"
#include "wxstring.h"

#define WXSPLITTER_VERSION      1.0

#define wxSPLIT_HORIZONTAL      1
#define wxSPLIT_VERTICAL        2

#define wxSPLIT_DRAG_NONE       0
#define wxSPLIT_DRAG_DRAGGING   1
#define wxSPLIT_DRAG_LEFT_DOWN  2

// Window styles
#define wxSP_NOBORDER           0x0000
#define wxSP_3D                 0x0004
#define wxSP_BORDER             0x0008

/*
 * wxSplitterWindow maintains one or two panes, with
 * an optional vertical or horizontal split which
 * can be used with the mouse or programmatically.
 */

// TODO:
// 1) Perhaps make the borders sensitive to dragging in order to create a split.
//    The MFC splitter window manages scrollbars as well so is able to
//    put sash buttons on the scrollbars, but we probably don't want to go down
//    this path.
// 2) for wxWindows 2.0, we must find a way to set the WS_CLIPCHILDREN style
//    to prevent flickering. (WS_CLIPCHILDREN doesn't work in all cases so can't be
//    standard).

class WXDLLEXPORT wxSplitterWindow: public wxCanvas
{
  DECLARE_DYNAMIC_CLASS(wxSplitterWindow)

 public:

////////////////////////////////////////////////////////////////////////////
// Public API

    // Default constructor
    wxSplitterWindow(void);

    // Normal constructor
    wxSplitterWindow(wxWindow *parent, int x, int y, int width, int height, long style = wxSP_3D, char *name = "splitter");
    ~wxSplitterWindow(void);

    // Gets the only or left/top pane
    inline wxWindow *GetWindow1(void) { return m_windowOne; }

    // Gets the right/bottom pane
    inline wxWindow *GetWindow2(void) { return m_windowTwo; }

    // Sets the split mode
    inline void SetSplitMode(int mode) { m_splitMode = mode; }

    // Gets the split mode
    inline int GetSplitMode(void) { return m_splitMode; };

    // Initialize with one window
    void Initialize(wxWindow *window);

    // Associates the given window with window 2, drawing the appropriate sash
    // and changing the split mode.
    // Does nothing and returns FALSE if the window is already split.
    // A sashPosition of -1 means choose a default sash position.
    Bool SplitVertically(wxWindow *window1, wxWindow *window2, int sashPosition = -1);
    Bool SplitHorizontally(wxWindow *window1, wxWindow *window2, int sashPosition = -1);

    // Removes the specified (or second) window from the view
    // Doesn't actually delete the window.
    Bool Unsplit(wxWindow *toRemove = NULL);

    // Is the window split?
    inline Bool IsSplit(void) { return (m_windowTwo != NULL); }

    // Sets the sash size
    inline void SetSashSize(int width) { m_sashSize = width; }

    // Sets the border size
    inline void SetBorderSize(int width) { m_borderSize = width; }

    // Gets the sash size
    inline int GetSashSize(void) { return m_sashSize; }

    // Gets the border size
    inline int GetBorderSize(void) { return m_borderSize; }

    // Set the sash position
    void SetSashPosition(int position, Bool redaw = TRUE);

    // Gets the sash position
    inline int GetSashPosition(void) { return m_sashPosition; }

    // If this is zero, we can remove panes by dragging the sash.
    inline void SetMinimumPaneSize(int min) { m_minimumPaneSize = min; }
    inline int GetMinimumPaneSize(void) { return m_minimumPaneSize; }

    // If the sash is moved to an extreme position, a subwindow
    // is removed from the splitter window, and the app is
    // notified. The app should delete or hide the window.
    virtual void OnUnsplit(wxWindow *removed) { removed->Show(FALSE); }

    // Called when the sash is double-clicked.
    // The default behaviour is to remove the sash if the
    // minimum pane size is zero.
    virtual void OnDoubleClickSash(int x, int y);

////////////////////////////////////////////////////////////////////////////
// Implementation

    // Paints the border and sash
    void OnPaint(void);

    // Handles mouse events
    void OnEvent(wxMouseEvent& ev);

    // Adjusts the panes
    void OnSize(int w, int h);

    // Draws borders
    void DrawBorders(wxDC& dc);

    // Draws the sash
    void DrawSash(wxDC& dc);

    // Draws the sash tracker (for whilst moving the sash)
    void DrawSashTracker(int x, int y);

    // Tests for x, y over sash
    Bool SashHitTest(int x, int y, int tolerance = 2);

    // Resizes subwindows
    void SizeWindows(void);

    // Initialize colours
    void InitColours(void);

 protected:
    int         m_splitMode;
    wxWindow*   m_windowOne;
    wxWindow*   m_windowTwo;
    int         m_dragMode;
    int         m_oldX;
    int         m_oldY;
    int         m_borderSize;
    int         m_sashSize;     // Sash width or height
    int         m_sashPosition; // Number of pixels from left or top
    int         m_requestedSashPosition;
    int         m_firstX;
    int         m_firstY;
    int         m_minimumPaneSize;
    wxCursor*   m_sashCursorWE;
    wxCursor*   m_sashCursorNS;
    wxPen*      m_sashTrackerPen;
    wxPen*      m_lightShadowPen;
    wxPen*      m_mediumShadowPen;
    wxPen*      m_darkShadowPen;
    wxPen*      m_hilightPen;
    wxBrush*    m_faceBrush;
    wxPen*      m_facePen;
};

#endif
