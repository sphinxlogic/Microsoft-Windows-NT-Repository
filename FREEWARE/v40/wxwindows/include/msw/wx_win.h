/*
 * File:	wx_win.h
 * Purpose:	
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

/*
 * Purpose:  wxWindow class declaration. Base class for all windows and
 *           panel items.
 */


#ifndef wx_winh
#define wx_winh

#include "wb_win.h"

#include <windows.h>

/*
 * Base class for frame, panel, canvas, panel items, dialog box.
 *
 */

#ifdef IN_CPROTO
typedef       void    *wxWindow ;
#else

class WXDLLEXPORT wxWindow: public wxbWindow
{
  DECLARE_ABSTRACT_CLASS(wxWindow)
 protected:
  Bool inOnSize; // Protection against OnSize reentry
 public:
  HANDLE ms_handle;                   // For menus and hwnds: using 'handle'
                                      // causes too many compiler messages
  int wxWinType;                      // For knowing how to delete the object

#ifndef WIN32
  // Pointer to global memory, for EDIT controls that need
  // special treatment to reduce USER area consumption.
  HGLOBAL globalHandle;
#endif

  int cxChar;
  int cyChar;
  int windows_id;
  Bool mouseInWindow ;
  Bool winEnabled;
  int minSizeX;
  int minSizeY;
  int maxSizeX;
  int maxSizeY;

  RECT updateRect;             // Bounding box for screen damage area
#ifdef WIN32
  HRGN updateRgn;                  // NT allows access to the rectangle list
#endif

  // Caret data
  int caretWidth;
  int caretHeight;
  Bool caretEnabled;
  Bool caretShown;
  
  // Temporary device context stored during an OnPaint
  HDC paintHDC;
  // Device context being stored whilst drawing is done
  HDC tempHDC;
  // Counter for number of times GetHDC() is called
  int countHDC;
  // Old window proc, for subclassed controls
  FARPROC oldWndProc;

  virtual BOOL MSWCommand(UINT param, WORD id);
  virtual BOOL MSWNotify(WPARAM wParam, LPARAM lParam);
  virtual wxWindow *FindItem(int id);
  virtual wxWindow *FindItemByHWND(HWND hWnd);
  virtual wxWindow *FindItemByStatic(HWND hWnd);
  virtual void PreDelete(HDC dc);              // Allows system cleanup
  virtual HWND GetHWND(void);
  virtual HDC GetHDC(void);
  virtual void ReleaseHDC(void);

  // Constructors/Destructors
  wxWindow(void);
  virtual ~wxWindow(void);

  virtual Bool Show(Bool show);
  virtual Bool IsShown(void);
  virtual wxCursor *SetCursor(wxCursor *cursor);
  virtual void SetColourMap(wxColourMap *cmap);

  virtual float GetCharHeight(void);
  virtual float GetCharWidth(void);
  virtual void GetTextExtent(const char *string, float *x, float *y,
      float *descent = NULL, float *externalLeading = NULL,
      wxFont *theFont = NULL, Bool use16bit = FALSE);

  void GetSize(int *width, int *height);
  void GetPosition(int *x, int *y);
  void GetClientSize(int *width, int *height); // Size client can use
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void SetSize(int width, int height) { wxbWindow::SetSize(width, height); }
  void SetClientSize(int width, int size);
  void ClientToScreen(int *x, int *y);
  void ScreenToClient(int *x, int *y);
  void SetFocus(void);
  void CaptureMouse(void);
  void ReleaseMouse(void);
  void Enable(Bool enable);
  void DragAcceptFiles(Bool accept);
  inline void SetTitle(char *WXUNUSED(title)) {};
  inline void Fit(void) {};
  inline void Centre(int WXUNUSED(direction)) {};
  Bool PopupMenu(wxMenu *menu, float x, float y);

  void Refresh(Bool eraseBack = TRUE, wxRectangle *rect = NULL);

  void OnScroll(wxCommandEvent& event);
  void SetScrollPos(int orient, int pos);
  void SetScrollRange(int orient, int range);
  void SetScrollPage(int orient, int page);
  int GetScrollPos(int orient);
  int GetScrollRange(int orient);
  int GetScrollPage(int orient);

  // Caret manipulation
  void CreateCaret(int w, int h);
  void CreateCaret(wxBitmap *bitmap);
  void DestroyCaret(void);
  void ShowCaret(Bool show);
  void SetCaretPos(int x, int y);
  void GetCaretPos(int *x, int *y);

  void SetSizeHints(int minW = -1, int minH = -1, int maxW = -1, int maxH = -1, int incW = -1, int incH = -1);

  // The default implementation sets scroll ranges, if any
  void OnSize(int w, int h);

  // Internal function to update scrollbars
  void DoScroll(wxCommandEvent& event);

  // Calculate scroll increment
  int CalcScrollInc(wxCommandEvent& event);
};

// Window specific (so far)
WXDLLEXPORT wxWindow *wxGetActiveWindow(void);

// Allows iteration through damaged rectangles in OnPaint
class WXDLLEXPORT wxUpdateIterator
{
  int rects;						// How many rects in Update region
  int current;					        // Current rectangle index
  RECT *rp;						// current rectangle
#ifdef	WIN32
  RGNDATA *rlist;					// Storage for regiondata
#endif

 public:
  wxUpdateIterator(wxWindow* wnd);
  ~wxUpdateIterator(void);

#ifndef __SALFORD__
  operator int (void);
#endif

  Bool HasRects(void); // equivalent to 'int' operator but not all compilers
                       // can cope.
  wxUpdateIterator* operator ++(int);
  RECT*	GetMSWRect(void);
  void GetRect(wxRectangle *rect);
  int GetX();
  int GetY();
  int GetW();
  int GetH();
};

WXDLLEXPORT int wxCharCodeMSWToWX(int keySym);
WXDLLEXPORT int wxCharCodeWXToMSW(int id, Bool *IsVirtual);

// Allocates control ids
WXDLLEXPORT int NewControlId(void);

#endif // IN_CPROTO
#endif
