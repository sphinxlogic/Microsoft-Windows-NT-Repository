/*
 * File:	wxgridm.h
 * Purpose:	wxGrid, Motif implementation
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

#ifndef wx_gridmh
#define wx_gridmh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_frame.h"
#include "wx_panel.h"
#include "wxstring.h"

#ifdef IN_CPROTO
typedef       void    *wxMotifGrid ;
#else

#define wxGRID_DEFAULT_CELL_HEIGHT 20
#define wxGRID_DEFAULT_CELL_WIDTH 80
#define wxGRID_DEFAULT_VERTICAL_LABEL_WIDTH 40
#define wxGRID_DEFAULT_HORIZONTAL_LABEL_HEIGHT 20

#ifndef wxLEFT
#define wxLEFT 0x0400
#endif

#ifndef wxRIGHT
#define wxRIGHT 0x0800
#endif

class wxMotifGrid: public wxPanel
{
  DECLARE_DYNAMIC_CLASS(wxMotifGrid)
 private:

#ifdef wx_motif
#endif
 public:
#ifdef wx_motif
  XtPointer tempCallbackStruct;
#endif

  int totalRows;
  int totalCols;
  Bool editable;
  
  wxColour cellTextColour;
  wxColour cellBackgroundColour;
  wxColour labelTextColour;
  wxColour labelBackgroundColour;
  wxFont *cellTextFont;
  wxFont *labelTextFont;
  wxPen *divisionPen;
  int rowLabelWidth;
  int colLabelHeight;

  wxMotifGrid(void);
  wxMotifGrid(wxWindow *parent, int x=-1, int y=-1, int width=-1, int height=-1,
               long style=0, char *name = "grid");
  ~wxMotifGrid(void);

  Bool Create(wxWindow *window, int x=-1, int y=-1, int width=-1, int height=-1,
               long style=0, char *name = "grid");
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  // Avoid compiler warning
  void SetSize(int w, int h) { wxWindow::SetSize(w, h); }
  void SetClientSize(int width, int height);
  void DiscardEdits(void);
  Bool Modified(void);

  Bool CreateGrid(int rows, int cols, wxString **cellValues = NULL, short *widths = NULL,
     short defaultWidth = wxGRID_DEFAULT_CELL_WIDTH, short defaultHeight = wxGRID_DEFAULT_CELL_HEIGHT);

  void ClearGrid(void);

  Bool InsertCols(int pos = 0, int n = 1, Bool updateLabels = TRUE);
  Bool InsertRows(int pos = 0, int n = 1, Bool updateLabels = TRUE);
  Bool AppendCols(int n = 1, Bool updateLabels = TRUE);
  Bool AppendRows(int n = 1, Bool updateLabels = TRUE);
  Bool DeleteCols(int pos = 0, int n = 1, Bool updateLabels = TRUE);
  Bool DeleteRows(int pos = 0, int n = 1, Bool updateLabels = TRUE);
  
  // Cell accessors
  void SetCellValue(const wxString& val, int row, int col);
  wxString& GetCellValue(int row, int col);
  void SetCellAlignment(int flag, int row, int col);
  void SetCellAlignment(int flag);
  int GetCellAlignment(int row, int col);
  int GetCellAlignment(void);
  void SetCellTextColour(const wxColour& val, int row, int col);
  void SetCellTextColour(const wxColour& col);
  wxColour& GetCellTextColour(int row, int col);
  inline wxColour& GetCellTextColour(void) { return cellTextColour; }
  void SetCellBackgroundColour(const wxColour& col);
  void SetCellBackgroundColour(const wxColour& colour, int row, int col);
  inline wxColour& GetCellBackgroundColour(void) { return cellBackgroundColour; }
  wxColour& GetCellBackgroundColour(int row, int col);
  inline wxFont *GetCellTextFont(void) { return cellTextFont; }
  wxFont *GetCellTextFont(int row, int col);
  void SetCellTextFont(wxFont *font);
  void SetCellTextFont(wxFont *font, int row, int col);
  
  // Size accessors
  void SetColumnWidth(int col, int width);
  int GetColumnWidth(int col);
  void SetRowHeight(int row, int height);
  int GetRowHeight(int row);
  
  // Label accessors
  void SetLabelSize(int orientation, int sz);
  int GetLabelSize(int orientation);
  void SetLabelAlignment(int orientation, int alignment);
  int GetLabelAlignment(int orientation);
  void SetLabelValue(int orientation, const wxString& val, int pos);
  wxString& GetLabelValue(int orientation, int pos);
  void SetLabelTextColour(const wxColour& colour);
  void SetLabelBackgroundColour(const wxColour& colour);
  inline wxColour& GetLabelTextColour(void) { return labelTextColour; }
  inline wxColour& GetLabelBackgroundColour(void) { return labelBackgroundColour; }
  inline wxFont *GetLabelTextFont(void) { return labelTextFont; }
  inline void SetLabelTextFont(wxFont *font) { labelTextFont = font; }

  // Miscellaneous accessors
  int GetCursorRow(void);
  int GetCursorColumn(void);
  void SetGridCursor(int row, int col);
  inline int GetRows(void) { return totalRows; }
  inline int GetCols(void) { return totalCols; }
  int GetScrollPosX(void);
  int GetScrollPosY(void);
  inline Bool GetEditable(void) { return editable; }
  void SetEditable(Bool edit);
//  Bool CurrentCellVisible(void);
  inline void SetDividerPen(wxPen *pen) { divisionPen = pen; }
  inline wxPen *GetDividerPen(void) { return divisionPen; }
  
  // High-level event handling
  // Override e.g. to check value of current cell; but call
  // base member for default processing.
  virtual void OnSelectCell(int row, int col);

  // Override to change labels e.g. creation of grid, inserting/deleting a row/col.
  // By default, auto-labels the grid.
  virtual void OnChangeLabels(void);

  // Whenever the user edits the cell (e.g. presses a key)
  virtual void OnCellChange(int row, int col) {};

  // Override to change the label of the edit field when selecting a cell
  // By default, sets it to e.g. A12
  virtual void OnChangeSelectionLabel(void);

  // Activation: call from wxFrame::OnActivate
  void OnActivate(Bool active);

  // Miscellaneous
  void AdjustScrollbars(void);
  void UpdateDimensions(void);

/*
  void Copy(void); // Copy selection to clipboard
  void Paste(void); // Paste clipboard into text window
  void Cut(void); // Copy selection to clipboard, then remove selection.
*/
};

class wxGrid: public wxMotifGrid
{
  public:
    wxGrid(void):wxMotifGrid() {}
    wxGrid(wxWindow *parent, int x=-1, int y=-1, int width=-1, int height=-1,
               long style=0, char *name = "gridWindow"):
     wxMotifGrid(parent, x, y, width, height, style, name)
    {
    }
};

#endif // IN_CPROTO
#endif // wx_gridmh
