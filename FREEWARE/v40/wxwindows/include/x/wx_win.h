/*
 * File:	wx_win.h
 * Purpose:	wxWindow class declaration (X version).
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_win.h	1.2 5/9/94" */

#ifndef wx_winh
#define wx_winh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_list.h"
#include "wb_win.h"

#ifdef wx_motif
#include <Xm/Xm.h>
#endif

#ifdef wx_xview
#include "xview/xview.h"
#endif

/*
 * Base class for frame, panel, canvas, panel items, dialog box.
 *
 */

#ifdef IN_CPROTO
typedef       void    *wxWindow ;
#else
class wxCursor;
class wxBitmap;
class wxWindow: public wxbWindow
{
  DECLARE_DYNAMIC_CLASS(wxWindow)

 public:
  // When doing global cursor changes, the current cursor
  // may need to be saved for each window @@@@
  Cursor currentWindowCursor;
#ifdef wx_xview
  Xv_opaque dropSite;
#endif
#ifdef wx_motif
  // Need to store state of buttons (whether they're still down)
  Bool button1Pressed;
  Bool button2Pressed;
  Bool button3Pressed;
#endif

  // Constructors/Destructors
  wxWindow(void);
  ~wxWindow(void);

  void GetSize(int *width, int *height);
  void GetPosition(int *x, int *y);
  void GetClientSize(int *width, int *height); // Size client can use
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  // Avoid compiler warning
  void SetSize(int w, int h) { wxbWindow::SetSize(w, h); }
  void SetClientSize(int width, int size);
  void ClientToScreen(int *x, int *y);
  void ScreenToClient(int *x, int *y);
  void Refresh(Bool eraseBack = TRUE, wxRectangle *rect = NULL);
  void SetFocus(void);
  void CaptureMouse(void);
  void ReleaseMouse(void);
  void Enable(Bool enable);
  void DragAcceptFiles(Bool accept);
  void Fit(void) {};
  void Centre(int WXUNUSED(direction)) {};
  void SetTitle(char *WXUNUSED(title)) {};
  void SetFont(wxFont *f);
  Bool PopupMenu(wxMenu *menu, float x, float y);

  // Sometimes in Motif there are problems popping up a menu
  // (for unknown reasons); use this instead when this happens.
#ifdef wx_motif
  Bool FakePopupMenu(wxMenu *menu, float x, float y);
#endif

  Bool Show(Bool show);
  Bool IsShown(void);
  wxCursor *SetCursor(wxCursor *cursor);
  void SetColourMap(wxColourMap *cmap);

  float GetCharHeight(void);
  float GetCharWidth(void);
  void GetTextExtent(const char *string, float *x, float *y,
     float *descent = NULL, float *externalLeading = NULL,
     wxFont *theFont = NULL, Bool use16 = FALSE);

#ifdef wx_motif 
  virtual Bool PreResize(void);
  virtual void PostDestroyChildren(void);
  int wxType;
#endif
  // Get the underlying X window
  virtual Window GetXWindow(void);
  virtual Display *GetXDisplay(void);

  inline void OnScroll(wxCommandEvent& WXUNUSED(event)) {}
  inline void SetScrollPosX(int WXUNUSED(pos)) {}
  inline void SetScrollPosY(int WXUNUSED(pos)) {}
  inline int GetScrollPos(int WXUNUSED(orient)) { return 0; };
  inline int GetScrollRange(int WXUNUSED(orient)) { return 0; };

  void SetSizeHints(int minW = -1, int minH = -1, int maxW = -1, int maxH = -1, int incW = -1, int incH = -1);
};

#endif // IN_CPROTO
#endif
