/*
 * File:	wx_tbar.h
 * Purpose:
 * Authors:     Julian Smart & Vitaly Prokopenko
 * Created:	1993
 * Updated:
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#ifndef wx_tbarh
#define wx_tbarh

#ifdef __GNUG__
#pragma interface
#endif

#include <common.h>

#if USE_TOOLBAR

#include "wx_gdi.h"
#include "wx_list.h"
#include "wx_canvs.h"
#include "wx_dcmem.h"

WXDLLEXPORT extern Constdata char *wxToolBarNameStr;

#define wxTOOL_STYLE_BUTTON          1
#define wxTOOL_STYLE_SEPARATOR       2

class WXDLLEXPORT wxToolBarTool: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxToolBarTool)
 public:
  int toolStyle;
  wxObject *clientData;
  int index;
  float x;
  float y;
  float width;
  float height;
  Bool toggleState;
  Bool isToggle;
  Bool deleteSecondBitmap;
  Bool enabled;
  wxBitmap * bitmap1, * bitmap2;
  Bool isMenuCommand;
  char *shortHelpString;
  char *longHelpString;

  wxToolBarTool(int theIndex = 0, wxBitmap *bitmap1 = NULL, wxBitmap *bitmap2 = NULL,
                Bool toggle = FALSE, wxCanvas *canvas = NULL, float xPos = -1, float yPos = -1,
                char *shortHelpString = NULL, char *longHelpString = NULL);
  ~wxToolBarTool ();
  inline void SetSize( float w, float h ) { width = w; height = h; }
  inline float GetWidth () { return width; }
  inline float GetHeight () { return height; }
};

// XView can't cope properly with panels that behave like canvases
// (e.g. no scrollbars in panels)
class WXDLLEXPORT wxToolBar :
#ifdef wx_xview
  public wxCanvas
#else
  public wxPanel
#endif
{
  DECLARE_DYNAMIC_CLASS(wxToolBar)
 protected:
  wxList tools;
  int tilingDirection;
  int rowsOrColumns;
  int currentRowsOrColumns;
  float lastX, lastY;
  float maxWidth, maxHeight;
  int currentTool; // Tool where mouse currently is
  int pressedTool; // Tool where mouse pressed
  int xMargin;
  int yMargin;
  int toolPacking;
  int toolSeparation;

 public:

  wxToolBar(void);
  wxToolBar(wxWindow *parent, int x = 0, int y = 0, int w = -1, int h = -1,
            long style = 0, int orientation = wxVERTICAL, int RowsOrColumns = 1, Constdata char *name = wxToolBarNameStr);
  ~wxToolBar(void);

  // Handle wxWindows events
  void OnPaint(void);
  void OnSize(int w, int h);
  void OnEvent(wxMouseEvent& event);
  void OnKillFocus(void);

  // Handle wxToolBar events

  // Only allow toggle if returns TRUE. Call when left button up.
  virtual Bool OnLeftClick(int toolIndex, Bool toggleDown);

  // Call when right button down.
  virtual void OnRightClick(int WXUNUSED(toolIndex), float WXUNUSED(x), float WXUNUSED(y)) {}

  // Eliminate compiler warnings.
  virtual void OnLeftClick(int x, int y, int keys) { wxCanvas::OnLeftClick(x, y, keys); }
  virtual void OnRightClick(int x, int y, int keys) { wxCanvas::OnRightClick(x, y, keys); }

  // Called when the mouse cursor enters a tool bitmap.
  // Argument is -1 if mouse is exiting the toolbar.
  virtual void OnMouseEnter(int toolIndex);

  // If pushedBitmap is NULL, a reversed version of bitmap is
  // created and used as the pushed/toggled image.
  // If toggle is TRUE, the button toggles between the two states.
  virtual wxToolBarTool *AddTool(int toolIndex, wxBitmap *bitmap, wxBitmap *pushedBitmap = NULL,
               Bool toggle = FALSE, float xPos = -1, float yPos = -1, wxObject *clientData = NULL,
               char *helpString1 = NULL, char *helpString2 = NULL);
  virtual void AddSeparator(void);
  virtual void ClearTools(void);

  virtual void DrawTool(wxMemoryDC& memDC, wxToolBarTool *tool);
  virtual void EnableTool(int toolIndex, Bool enable);
  virtual void ToggleTool(int toolIndex, Bool toggle); // toggle is TRUE if toggled on
  virtual void SetToggle(int toolIndex, Bool toggle); // Set this to be togglable (or not)
  virtual wxObject *GetToolClientData(int index);
  inline wxList& GetTools(void) { return tools; }

  // After the toolbar has initialized, this is the size the tools take up
  virtual void GetMaxSize ( float * width, float * height );
  virtual Bool GetToolState(int toolIndex);
  virtual Bool GetToolEnabled(int toolIndex);
  virtual wxToolBarTool *FindToolForPosition(float x, float y);
  virtual void SpringUpButton(int index);

  virtual void SetToolShortHelp(int toolIndex, char *helpString);
  virtual char *GetToolShortHelp(int toolIndex);
  virtual void SetToolLongHelp(int toolIndex, char *helpString);
  virtual char *GetToolLongHelp(int toolIndex);

  virtual void SetMargins(int x, int y);
  virtual void SetToolPacking(int packing);
  virtual void SetToolSeparation(int separation);
  
  virtual void Layout (void);
  // Add all the buttons: required for Win95.
  virtual Bool CreateTools(void) { return TRUE; }
};

#endif // USE_TOOLBAR
#endif // wx_tbarh
