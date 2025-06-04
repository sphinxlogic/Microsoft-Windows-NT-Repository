/*
 * File:	wxgridg.h
 * Purpose:	wxGrid, generic implementation
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	12 May 1997 - Sean Starkey
 *              Addition of support for label hits
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

#ifndef wx_gridgh
#define wx_gridgh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_frame.h"
#include "wx_panel.h"
#include "wxstring.h"
#include "wx_scrol.h"

#ifdef IN_CPROTO
typedef       void    *wxGrid ;
typedef       void    *wxGenericGrid ;
typedef       void    *wxGridCell ;
#else

#define wxGRID_DEFAULT_EDIT_WIDTH 300
#define wxGRID_DEFAULT_EDIT_HEIGHT 27
#define wxGRID_DEFAULT_EDIT_X 5
#define wxGRID_DEFAULT_EDIT_Y 1
#define wxGRID_DEFAULT_SHEET_TOP 31
#define wxGRID_DEFAULT_SHEET_LEFT 0
#define wxGRID_DEFAULT_CELL_HEIGHT 20
#define wxGRID_DEFAULT_CELL_WIDTH 80
#define wxGRID_DEFAULT_VERTICAL_LABEL_WIDTH 40
#define wxGRID_DEFAULT_HORIZONAL_LABEL_HEIGHT 20

#ifndef wxLEFT
#define wxLEFT 0x0400
#endif

#ifndef wxRIGHT
#define wxRIGHT 0x0800
#endif

#define WXGENERIC_GRID_VERSION  0.4

class wxGridCell;
class wxGenericGrid: public wxPanel
{
  DECLARE_DYNAMIC_CLASS(wxGenericGrid)
 protected:
  wxText *textItem;
  wxScrollBar *hScrollBar;
  wxScrollBar *vScrollBar;
  int wCursorRow;
  int wCursorColumn;
  wxRectangle CurrentRect;
  Bool currentRectVisible;
  wxGridCell ***gridCells;
  wxGridCell **rowLabelCells;
  wxGridCell **colLabelCells;
  Bool bEditCreated;
  Bool editable;

  int totalRows;
  int totalCols;
  
  // Row and column we're currently looking at
  int scrollPosX;
  int scrollPosY;

  // Dimensions
  int leftOfSheet;
  int topOfSheet;
  int rightOfSheet;    // Calculated from colWidths
  int bottomOfSheet;   // Calculated from rowHeights
  int totalGridWidth; // Total 'virtual' size
  int totalGridHeight;
  int cellHeight;      // For now, a default
  int verticalLabelWidth;
  int horizontalLabelHeight;
  int verticalLabelAlignment;
  int horizontalLabelAlignment;
  int cellAlignment;
  short *colWidths;   // Dynamically allocated
  short *rowHeights;  // Dynamically allocated
  int scrollWidth;    // Vert. scroll width, horiz. scroll height
  
  // Colours
  wxColour cellTextColour;
  wxColour cellBackgroundColour;
  wxFont *cellTextFont;
  wxColour labelTextColour;
  wxColour labelBackgroundColour;
  wxBrush *labelBackgroundBrush;
  wxFont *labelTextFont;
  wxPen *divisionPen;

  // Position of Edit control
  wxRectangle editControlPosition;
  
  // Drag status
  int dragStatus;
  int dragRowOrCol;
  int dragStartPosition;
  int dragLastPosition;
  static wxCursor *horizontalSashCursor;
  static wxCursor *verticalSashCursor;
  
  // Don't refresh whilst this is > 0
  int batchCount;
  
 public:
  wxGenericGrid(void);

  wxGenericGrid(wxWindow *parent, int x, int y, int width, int height, long style = 0, char *name = "grid");
  ~wxGenericGrid(void);

  void OnPaint(void);
  void OnEvent(wxMouseEvent& ev);
  void OnSize(int w, int h);

  Bool CreateGrid(int nRows, int nCols, wxString **cellValues = NULL, short *widths = NULL,
     short defaultWidth = wxGRID_DEFAULT_CELL_WIDTH, short defaultHeight = wxGRID_DEFAULT_CELL_HEIGHT);
  void ClearGrid(void);
  virtual wxGridCell *GetCell(int row, int col);
  inline wxGridCell ***GetCells(void) { return gridCells; }
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
  void SetCellTextFont(wxFont *fnt);
  void SetCellTextFont(wxFont *fnt, int row, int col);
  wxBitmap *GetCellBitmap(int row, int col);
  void SetCellBitmap(wxBitmap *bitmap, int row, int col);
  
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
  wxGridCell *GetLabelCell(int orientation, int pos);
  void SetLabelValue(int orientation, const wxString& val, int pos);
  wxString& GetLabelValue(int orientation, int pos);
  void SetLabelTextColour(const wxColour& colour);
  void SetLabelBackgroundColour(const wxColour& colour);
  inline wxColour& GetLabelTextColour(void) { return labelTextColour; }
  inline wxColour& GetLabelBackgroundColour(void) { return labelBackgroundColour; }
  inline wxFont *GetLabelTextFont(void) { return labelTextFont; }
  inline void SetLabelTextFont(wxFont *fnt) { labelTextFont = fnt; }

  // Miscellaneous accessors
  inline int GetCursorRow(void) { return wCursorRow; }
  inline int GetCursorColumn(void) { return wCursorColumn; }
  void SetGridCursor(int row, int col);
  inline int GetRows(void) { return totalRows; }
  inline int GetCols(void) { return totalCols; }
  inline int GetScrollPosX(void) { return scrollPosX; }
  inline int GetScrollPosY(void) { return scrollPosY; }
  inline void SetScrollPosX(int pos) { scrollPosX = pos; }
  inline void SetScrollPosY(int pos) { scrollPosY = pos; }
  inline wxText *GetTextItem(void) { return textItem; }
  inline wxScrollBar *GetHorizScrollBar(void) { return hScrollBar; }
  inline wxScrollBar *GetVertScrollBar(void) { return vScrollBar; }
  inline Bool GetEditable(void) { return editable; }
  void SetEditable(Bool edit);
  inline wxRectangle& GetCurrentRect(void) { return CurrentRect; }
  inline Bool CurrentCellVisible(void) { return currentRectVisible; }
  inline void SetDividerPen(wxPen *pen) { divisionPen = pen; }
  inline wxPen *GetDividerPen(void) { return divisionPen; }
  
  // High-level event handling
  // Override e.g. to check value of current cell; but call
  // base member for default processing.
  virtual void OnSelectCellImplementation(wxDC *dc, int row, int col);

  virtual void OnSelectCell(int row, int col) {};

  // Override to create your own class of grid cell
  virtual wxGridCell *OnCreateCell(void);
  
  // Override to change labels e.g. creation of grid, inserting/deleting a row/col.
  // By default, auto-labels the grid.
  virtual void OnChangeLabels(void);

  // Override to change the label of the edit field when selecting a cell
  // By default, sets it to e.g. A12
  virtual void OnChangeSelectionLabel(void);
  
  // Override for event processing
  virtual void OnCellChange(int WXUNUSED(row), int WXUNUSED(col)) {};
  virtual void OnCellLeftClick(int WXUNUSED(row), int WXUNUSED(col), int WXUNUSED(x), int WXUNUSED(y), Bool WXUNUSED(control), Bool WXUNUSED(shift)) {};
  virtual void OnCellRightClick(int WXUNUSED(row), int WXUNUSED(col), int WXUNUSED(x), int WXUNUSED(y), Bool WXUNUSED(control), Bool WXUNUSED(shift)) {};
  virtual void OnLabelLeftClick(int WXUNUSED(row), int WXUNUSED(col), int WXUNUSED(x), int WXUNUSED(y), Bool WXUNUSED(control), Bool WXUNUSED(shift)) {};
  virtual void OnLabelRightClick(int WXUNUSED(row), int WXUNUSED(col), int WXUNUSED(x), int WXUNUSED(y), Bool WXUNUSED(control), Bool WXUNUSED(shift)) {};

  // Activation: call from wxFrame::OnActivate
  void OnActivate(Bool active);

  // Miscellaneous
  void AdjustScrollbars(void);
  void UpdateDimensions(void);

  /* INTERNAL
   */
  void SetCurrentRect (int Row, int Column, int canvasW = -1, int canvasH = -1);
  void HighlightCell (wxDC *dc);
  void DrawCellText(void);
  void SetGridClippingRegion(wxDC *dc);
  virtual Bool CellHitTest(int x, int y, int *row, int *col);
  virtual Bool LabelSashHitTest(int x, int y, int *orientation, int *rowOrCol, int *startPos);
  virtual Bool LabelHitTest(int x, int y, int *row, int *col);
  // Painting
  virtual void DrawLabelAreas(wxDC *dc);
  virtual void DrawEditableArea(wxDC *dc);
  virtual void DrawGridLines(wxDC *dc);
  virtual void DrawColumnLabels(wxDC *dc);
  virtual void DrawColumnLabel(wxDC *dc, wxRectangle *rect, int col);
  virtual void DrawRowLabels(wxDC *dc);
  virtual void DrawRowLabel(wxDC *dc, wxRectangle *rect, int row);
  virtual void DrawCells(wxDC *dc);
  virtual void DrawCellValue(wxDC *dc, wxRectangle *rect, int row, int col);
  virtual void DrawCellBackground(wxDC *dc, wxRectangle *rect, int row, int col);
  virtual void DrawTextRect(wxDC *dc, char *text, wxRectangle *rect, int flag);
  virtual void DrawBitmapRect(wxDC *dc, wxBitmap *bitmap, wxRectangle *rect, int flag);

  // Refresh cell and optionally set the text field
  void RefreshCell(int row, int col, Bool setText = FALSE);

  // Don't refresh within the outer pair of these.
  inline void BeginBatch(void) { batchCount ++; }
  inline void EndBatch(void) { batchCount --; }
  inline int GetBatchCount(void) { return batchCount; }
};

class wxGridCell: public wxObject
{
 public:
  wxString textValue;
  wxFont *font;
  wxColour textColour;
  wxColour backgroundColour;
  wxBrush *backgroundBrush;
  wxBitmap *cellBitmap;
  int alignment;

  wxGridCell(wxGenericGrid *window = NULL);
  ~wxGridCell(void);
  
  virtual wxString& GetTextValue(void) { return textValue; }
  virtual void SetTextValue(const wxString& str) { textValue = str; }
  inline wxFont *GetFont(void) { return font; }
  inline void SetFont(wxFont *f) { font = f; }
  inline wxColour& GetTextColour(void) { return textColour; }
  inline void SetTextColour(const wxColour& colour) { textColour = colour; }
  inline wxColour& GetBackgroundColour(void) { return backgroundColour; }
  void SetBackgroundColour(const wxColour& colour);
  inline wxBrush *GetBackgroundBrush(void) { return backgroundBrush; }
  inline int GetAlignment(void) { return alignment; }
  inline void SetAlignment(int align) { alignment = align; }
  inline wxBitmap *GetCellBitmap(void) { return cellBitmap; }
  inline void SetCellBitmap(wxBitmap *bitmap) { cellBitmap = bitmap; }
};

class wxGrid: public wxGenericGrid
{
  public:
    wxGrid(void):wxGenericGrid() {}
    wxGrid(wxWindow *parent, int x=-1, int y=-1, int width=-1, int height=-1,
               long style=0, char *name = "gridWindow"):
     wxGenericGrid(parent, x, y, width, height, style, name)
    {
    }
};

#endif
#endif
