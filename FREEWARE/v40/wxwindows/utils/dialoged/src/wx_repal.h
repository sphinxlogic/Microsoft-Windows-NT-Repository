/*
 * File:	wx_repal.h
 * Purpose:	Resource editor palette
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

#ifndef wx_repalh
#define wx_repalh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx.h"
#include "wxstring.h"
#include "wx_lay.h"
#include "wx_plist.h"
#include "wx_res.h"
#include "wx_tbar.h"

/*
 * Object editor tool palette
 *
 */

// For some reason, wxButtonBar under Windows 95 cannot be moved to a non-0,0 position!
#define TOOLPALETTECLASS    wxToolBar

class EditorToolPalette: public TOOLPALETTECLASS
{
 public:
  int currentlySelected;
  wxResourceManager *resourceManager;

  EditorToolPalette(wxResourceManager *manager, wxFrame *frame, int x = 0, int y = 0, int w = -1, int h = -1,
            long style = 0, int direction = wxVERTICAL, int RowsOrColumns = 2);
  Bool OnLeftClick(int toolIndex, Bool toggled);
  void OnMouseEnter(int toolIndex);
  void OnPaint(void);
  void SetSize(int x, int y, int width, int height, int sizeFlags);
};

#define PALETTE_FRAME           1
#define PALETTE_DIALOG_BOX      2
#define PALETTE_PANEL           3
#define PALETTE_CANVAS          4
#define PALETTE_TEXT_WINDOW     5
#define PALETTE_MESSAGE         6
#define PALETTE_BUTTON          7
#define PALETTE_CHECKBOX        8
#define PALETTE_LISTBOX         9
#define PALETTE_RADIOBOX        10
#define PALETTE_CHOICE          11
#define PALETTE_TEXT            12
#define PALETTE_MULTITEXT       13
#define PALETTE_SLIDER          14
#define PALETTE_ARROW           15
#define PALETTE_GAUGE           16
#define PALETTE_GROUPBOX        17
#define PALETTE_BITMAP_MESSAGE  18
#define PALETTE_BITMAP_BUTTON   19
#define PALETTE_SCROLLBAR       20

#endif
