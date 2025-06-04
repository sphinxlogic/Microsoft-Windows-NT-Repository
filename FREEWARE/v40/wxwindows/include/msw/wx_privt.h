/*
 * File:	wx_privt.h
 * Purpose:	Private class declarations.
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
	Last change:  JS   26 May 97    3:26 pm
 */

/* sccsid[] = "@(#)wx_privt.h	1.2 5/9/94" */

#ifndef wx_privth
#define wx_privth

#include "wx_defs.h"
#include "wx_obj.h"

/*
 * This is a generic Windows 3 window.
 * I derive from this to create windows for panels, canvases,
 * status bar, text window, frame etc.
 */

#ifdef IN_CPROTO
typedef       void    *wxWnd ;
typedef       void    *wxSubWnd ;
typedef       void    *wxCanvasWnd ;
typedef       void    *wxFrameWnd ;
typedef       void    *wxStatusWnd ;
typedef       void    *wxMDIFrame ;
typedef       void    *wxMDIChild ;
#else

class wxWindow;

class WXDLLEXPORT wxWnd : public wxObject
{
public:
    UINT last_msg;
    WPARAM last_wparam;
    LPARAM last_lparam;
    Bool x_scrolling_enabled;
    Bool y_scrolling_enabled;
    Bool calcScrolledOffset; // If TRUE, wxCanvasDC uses scrolled offsets

    int xscroll_pixels_per_line;
    int yscroll_pixels_per_line;
    int xscroll_lines;
    int yscroll_lines;
    int xscroll_lines_per_page;
    int yscroll_lines_per_page;
    int xscroll_position;
    int yscroll_position;
    float last_x_pos;
    float last_y_pos;
    int last_event;
    HWND handle;
    HACCEL accelerator_table;
    HMENU hMenu; // Menu, if any

    Bool is_canvas;
    Bool is_dialog;
    Bool userColours; // Usually FALSE, wxUSER_COLOURS overrides CTL3D etc. settings

    HBRUSH background_brush;
    COLORREF background_colour;
    Bool background_transparent;
    Bool canDeleteBackgroundBrush;

    wxWindow *wx_window;

    wxWnd(void);
    ~wxWnd(void);

    void Create(wxWnd *parent, char *wclass, wxWindow *wx_win, char *title,
               int x, int y, int width, int height,
               DWORD style, char *dialog_template = NULL,
               DWORD exendedStyle = 0);

    // Calculates the position of a point on the window
    // taking into account the position of scrollbars.
    // Windows doesn't automatically reflect the position of the
    // scrollbars - (0, 0) is always the top left of the visible window,
    // whereas in XView, (0, 0) moves according to scrollbar positions.
    virtual void CalcScrolledPosition(int x, int y, int *xx, int *yy);

    // Actually defined in wx_canvs.cc since requires wxCanvas declaration
    void DeviceToLogical(float *x, float *y);

    // Calculate logical (scroll-bar/scaling aware) position from
    // device (pixel) position
    virtual void CalcUnscrolledPosition(int x, int y, float *xx, float *yy);

    // Handlers
    virtual void OnCreate(LPCREATESTRUCT cs);
    virtual BOOL OnPaint(void);
    virtual HICON OnQueryDragIcon(void) { return 0; }
    virtual void OnSize(int x, int y, UINT flag);
    virtual void OnHScroll(WORD nSBCode, WORD pos, HWND control);
    virtual void OnVScroll(WORD nSBCode, WORD pos, HWND control);
    virtual BOOL OnCommand(WORD id, WORD cmd, HWND control);
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam);
    virtual HBRUSH OnCtlColor(HDC dc, HWND pWnd, UINT nCtlColor,
                              UINT message, WPARAM wParam, LPARAM lParam);
    virtual BOOL OnColorChange(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual BOOL OnEraseBkgnd(HDC pDC);
    virtual void OnMenuSelect(WORD item, WORD flags, HMENU sysmenu);
    virtual void OnInitMenuPopup(HMENU menu, int pos, Bool isSystem);
    virtual BOOL OnClose(void);
    virtual BOOL OnDestroy(void);
    virtual BOOL OnSetFocus(HWND wnd);
    virtual BOOL OnKillFocus(HWND wnd);
    virtual void OnDropFiles(WPARAM wParam);

    // Canvas-type events
    virtual void OnLButtonDown(int x, int y, UINT flags);
    virtual void OnLButtonUp(int x, int y, UINT flags);
    virtual void OnLButtonDClick(int x, int y, UINT flags);

    virtual void OnMButtonDown(int x, int y, UINT flags);
    virtual void OnMButtonUp(int x, int y, UINT flags);
    virtual void OnMButtonDClick(int x, int y, UINT flags);

    virtual void OnRButtonDown(int x, int y, UINT flags);
    virtual void OnRButtonUp(int x, int y, UINT flags);
    virtual void OnRButtonDClick(int x, int y, UINT flags);

    virtual void OnMouseMove(int x, int y, UINT flags);
    virtual void OnMouseEnter(int x, int y, UINT flags);
    virtual void OnMouseLeave(int x, int y, UINT flags);

    virtual void OnChar(WORD wParam, LPARAM lParam, Bool isASCII = FALSE);

    virtual BOOL OnActivate(BOOL flag, BOOL minimized, HWND activate);
    virtual BOOL OnMDIActivate(BOOL flag, HWND activate, HWND deactivate);

    virtual BOOL OnDrawItem(int id, DRAWITEMSTRUCT *item);
    virtual BOOL OnMeasureItem(int id, MEASUREITEMSTRUCT *item);

    // Window procedure
    virtual LONG WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

    // Calls an appropriate default window procedure
    virtual LONG DefWindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
    virtual BOOL ProcessMessage(MSG* pMsg);
    virtual void DestroyWindow(void);

    // Set background brush, deleting old one if necessary and
    // noting whether we can delete the new one in future.
    void SetBackgroundBrush(HBRUSH br, Bool canDelete);

    // Detach "Window" menu from menu bar so it doesn't get deleted
    void DetachWindowMenu(void);
};


/*
 * This is a Windows 3 subwindow - panel or canvas
 */

class WXDLLEXPORT wxSubWnd : public wxWnd
{
public:
    Bool mouse_in_window ;

    wxSubWnd(void);
    wxSubWnd(wxWnd *parent, char *wclass, wxWindow *wx_win,
                int x, int y, int width, int height,
                DWORD style, char *dialog_template = NULL);
    ~wxSubWnd(void);

    // Handlers
    BOOL OnPaint(void);
    void OnSize(int x, int y, UINT flag);
    BOOL OnCommand(WORD id, WORD cmd, HWND control);

    // Canvas-type events
    void OnLButtonDown(int x, int y, UINT flags);
    void OnLButtonUp(int x, int y, UINT flags);
    void OnLButtonDClick(int x, int y, UINT flags);

    void OnMButtonDown(int x, int y, UINT flags);
    void OnMButtonUp(int x, int y, UINT flags);
    void OnMButtonDClick(int x, int y, UINT flags);

    void OnRButtonDown(int x, int y, UINT flags);
    void OnRButtonUp(int x, int y, UINT flags);
    void OnRButtonDClick(int x, int y, UINT flags);

    void OnMouseMove(int x, int y, UINT flags);
    void OnMouseEnter(int x, int y, UINT flags);
    void OnMouseLeave(int x, int y, UINT flags);

    void OnChar(WORD wParam, LPARAM lParam, Bool isASCII = FALSE);

    void OnHScroll(WORD nSBCode, WORD pos, HWND control);
    void OnVScroll(WORD nSBCode, WORD pos, HWND control);
};

class WXDLLEXPORT wxCanvasWnd : public wxSubWnd
{
public:
  wxCanvasWnd(wxWnd *parent, char *winClass, wxWindow *wx_win,
              int x, int y, int width, int height,
              DWORD style);
  wxCanvasWnd(void);

  // Handlers
  BOOL OnEraseBkgnd(HDC pDC);
};

class WXDLLEXPORT wxFrameWnd : public wxWnd
{
public:
    Bool iconized;
    HICON icon;
    HICON defaultIcon;

    wxFrameWnd(void);
    wxFrameWnd(wxWnd *parent, char *wclass, wxWindow *wx_win, char *title,
                   int x, int y, int width, int height,
                   long style);
    ~wxFrameWnd(void);

    // Handlers
    BOOL OnPaint(void);
    HICON OnQueryDragIcon(void);
    void OnSize(int x, int y, UINT flag);
    BOOL OnCommand(WORD id, WORD cmd, HWND control);
    BOOL OnClose(void);
    void OnMenuSelect(WORD item, WORD flags, HMENU sysmenu);
    BOOL ProcessMessage(MSG *msg);
//    BOOL OnEraseBkgnd(HDC pDC);
};

class WXDLLEXPORT wxStatusWnd : public wxWnd
{
public:
    char *status_text;
    int height;
    HBRUSH light_grey_brush;

    wxStatusWnd(wxFrameWnd *parent, int the_height);
    ~wxStatusWnd(void);

    BOOL OnPaint(void);
};

class WXDLLEXPORT wxMDIFrame : public wxFrameWnd
{
public:
    HWND client_hwnd;
    wxWnd *current_child;
    HMENU window_menu;
    Bool parent_frame_active; // TRUE if MDI Frame is intercepting
                              // commands, not child

    wxMDIFrame(wxWnd *parent, wxWindow *wx_win, char *title=NULL,
                int x=-1, int y=-1, int width=-1, int height=-1, long style = 0);
    ~wxMDIFrame(void);

    void OnCreate(LPCREATESTRUCT cs);
//    BOOL OnPaint(void);
//    BOOL OnClose(void);
    void OnSize(int x, int y, UINT);
    BOOL OnCommand(WORD id, WORD cmd, HWND control);
    void OnMenuSelect(WORD, WORD, HMENU);
    long DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    BOOL ProcessMessage(MSG *msg);
    BOOL OnEraseBkgnd(HDC pDC);
    BOOL OnDestroy(void);
};

class WXDLLEXPORT wxMDIChild : public wxFrameWnd
{
public:
    Bool active;
    
    wxMDIChild(wxMDIFrame *parent, wxWindow *wx_win, char *title=NULL,
                int x=-1, int y=-1, int width=-1, int height=-1, long style = 0);
    ~wxMDIChild(void);

    BOOL OnMDIActivate(BOOL bActivate, HWND, HWND);
//    BOOL OnPaint(void);
    BOOL OnClose(void);
    void OnSize(int x, int y, UINT);
    BOOL OnCommand(WORD id, WORD cmd, HWND control);
//    void OnMenuSelect(WORD, WORD, HMENU);
    long DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    BOOL ProcessMessage(MSG *msg);
    void DestroyWindow(void);
};

class WXDLLEXPORT wxDialogWnd : public wxSubWnd
{
public:
  wxDialogWnd(wxWnd *parent, wxWindow *wx_win,
              int x, int y, int width, int height,
              char *dialog_template);

  // Handlers
  LONG WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
  LONG DefWindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
  BOOL ProcessMessage(MSG* pMsg);
  BOOL OnEraseBkgnd(HDC pDC);
  BOOL OnClose(void);
};

#define         wxTYPE_XWND              1
#define         wxTYPE_HWND              2
#define         wxTYPE_HMENU             3
#define         wxTYPE_MDICHILD          4
#define VIEWPORT_EXTENT 1000

class wxFont ;

WXDLLEXPORT void wxGetCharSize(HWND wnd, int *x, int *y,wxFont *the_font);
WXDLLEXPORT void wxSliderEvent(HWND control, WORD wParam, WORD pos);
WXDLLEXPORT wxWnd *wxFindWinFromHandle(HWND hWnd);
WXDLLEXPORT void wxScrollBarEvent(HWND hbar, WORD wParam, WORD pos);

extern HICON wxSTD_FRAME_ICON;
extern HICON wxSTD_MDIPARENTFRAME_ICON;
extern HICON wxSTD_MDICHILDFRAME_ICON;
extern HICON wxDEFAULT_FRAME_ICON;
extern HICON wxDEFAULT_MDIPARENTFRAME_ICON;
extern HICON wxDEFAULT_MDICHILDFRAME_ICON;
extern HFONT wxSTATUS_LINE_FONT;

#endif // IN_CPROTO
#endif // wx_privth

