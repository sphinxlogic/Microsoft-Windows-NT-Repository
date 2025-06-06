/*
 * File:	wb_item.h
 * Purpose:	Declares panel items base class
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wb_item.h	1.2 5/9/94" */

#ifndef wxb_itemh
#define wxb_itemh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_panel.h"
#include "wx_dcpan.h"
#include "wx_stdev.h"

#ifdef IN_CPROTO
typedef       void    *wxbItem ;
#else

class wxFont ;

// General item class
class WXDLLEXPORT wxbItem: public wxWindow
{
   Bool isSelected;
   int handleSize;   // selection handle size
   int handleMargin; // Distance between item edge and handle edge
   static int dragOffsetX;  // Distance between pointer at start of drag and 
   static int dragOffsetY;  // top-left of item
 public:
    wxbItem(void);
   ~wxbItem(void);

   wxFont *buttonFont ;
   wxFont *labelFont ;
   wxColour backColour ;
   wxColour labelColour;
   wxColour buttonColour;

   int labelPosition;

   virtual void GetSize(int *width, int *height) = 0;
   virtual void GetPosition(int *x, int *y) = 0;
   virtual void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO) = 0;
   // Avoid compiler warning
   void SetSize(int w, int h) { wxWindow::SetSize(w, h); }
   virtual void SetClientSize(int width, int height);
   virtual void SetFocus(void) = 0;
   virtual void SetLabel(char *label) = 0;
   inline void SetLabel(const char *label) { SetLabel((char *)label); }
   virtual char *GetLabel(void) = 0;

   inline virtual void Command(wxCommandEvent& WXUNUSED(event)) {};        // Simulates an event
   inline virtual void ProcessCommand(wxCommandEvent& WXUNUSED(event)) {}; // Calls the callback and 
                                                                 // appropriate event handlers
   virtual float GetCharWidth(void) = 0;
   virtual float GetCharHeight(void) = 0;

   virtual int GetLabelPosition(void);
   virtual void SetLabelPosition(int pos);
   // Helper function that sets the
   // appropriate label position depending on windowStyle flags
   // and parent label position setting
   virtual void SetAppropriateLabelPosition(void);

  // Places item in centre of panel - so can't be used BEFORE panel->Fit()
  void Centre(int direction = wxHORIZONTAL);

  inline wxFont  *GetLabelFont(void)        { return labelFont ; }
  inline wxFont  *GetButtonFont(void)       { return buttonFont ; }
  inline wxColour*GetBackgroundColour(void) { return &backColour ; }
  inline wxColour*GetLabelColour(void)      { return &labelColour ; }
  inline wxColour*GetButtonColour(void)     { return &buttonColour ; }

/*
  virtual void SetBackgroundColour(wxColour*col) { if (col) SetBackgroundColour(*col); }
  virtual void SetLabelColour(wxColour*col) { if (col) SetLabelColour(*col); }
  virtual void SetButtonColour(wxColour*col) { if (col) SetButtonColour(*col); }
*/

  virtual void SetBackgroundColour(wxColour& col) = 0 ;
  virtual void SetLabelColour(wxColour& col) = 0 ;
  virtual void SetButtonColour(wxColour& col) = 0 ;
  
  // Not all platforms may be able to set item fonts dynamically,
  // so by default do nothing.
  virtual void SetLabelFont(wxFont *WXUNUSED(font)) {}
  virtual void SetButtonFont(wxFont *WXUNUSED(font)) {}

  // Manipulation and drawing of items in Edit Mode

  // Calculate position of the 8 handles
  virtual void CalcSelectionHandles(int *hx, int *hy);
  virtual void DrawSelectionHandles(wxPanelDC *dc, Bool erase = FALSE);
  virtual void DrawBoundingBox(wxPanelDC *dc, int x, int y, int w, int h);
  virtual void SelectItem(Bool select);
  virtual inline Bool IsSelected(void) { return isSelected; }

  // Returns TRUE or FALSE
  virtual Bool HitTest(int x, int y);

  // Returns 0 (no hit), 1 - 8 for which selection handle
  // (clockwise from top middle)
  virtual int SelectionHandleHitTest(int x, int y);

  // If selectionHandle is zero, not dragging the selection handle.
  virtual void OnDragBegin(int x, int y, int keys, wxPanelDC *dc, int selectionHandle);
  virtual void OnDragContinue(Bool paintIt, int x, int y, int keys, wxPanelDC *dc, int selectionHandle);
  virtual void OnDragEnd(int x, int y, int keys, wxPanelDC *dc, int selectionHandle);

  // These functions call panel functions
  // by default.
  void OnEvent(wxMouseEvent& event);
  void OnMove(int x, int y);
  void OnSize(int w, int h);
  virtual void OnLeftClick(int x, int y, int keys);
  virtual void OnRightClick(int x, int y, int keys);
  virtual void OnSelect(Bool select);
};

#endif // IN_CPROTO
#endif // wxb_itemh
