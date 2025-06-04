/*
 * File:	wxgridg.cc
 * Purpose:	wxGenericGrid implementation, MS Windows
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	12 May 1997 - Sean Starkey
 *              Addition of support for label hits
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
	Last change:  JS   25 May 97    8:22 pm
 */

static const char sccsid[] = "%W% %G%";

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include "wxstring.h"
#include "wxgridg.h"

#include "wx_timer.h"

static wxFont *wxGridEntryFont = NULL;
static long paintTime = 0;

#define wxGRID_DRAG_NONE       0
#define wxGRID_DRAG_LEFT_RIGHT 1
#define wxGRID_DRAG_UP_DOWN    2

static void wxGridFunc(wxText& txt, wxCommandEvent& ev);
static void wxGridScrollFunc(wxScrollBar& sb, wxCommandEvent& ev);

IMPLEMENT_DYNAMIC_CLASS(wxGenericGrid, wxPanel)

wxCursor *wxGenericGrid::horizontalSashCursor = NULL;
wxCursor *wxGenericGrid::verticalSashCursor = NULL;

wxGenericGrid::wxGenericGrid(void)
{
}

wxGenericGrid::wxGenericGrid(wxWindow *parent, int x, int y, int width, int height, long style, char *name)
  :wxPanel(parent, x, y, width, height, style, name)
{
  hScrollBar = NULL;
  vScrollBar = NULL;
  
  batchCount = 0;
  cellTextColour = *wxBLACK;
  cellBackgroundColour = *wxWHITE;
  labelTextColour = *wxBLACK;
  labelBackgroundColour = *wxLIGHT_GREY;
  labelBackgroundBrush = NULL;
  labelTextFont = wxTheFontList->FindOrCreateFont(12, wxSWISS, wxNORMAL, wxBOLD);
  cellTextFont = wxTheFontList->FindOrCreateFont(12, wxSWISS, wxNORMAL, wxNORMAL);
  textItem = NULL;
  currentRectVisible = FALSE;
  editable = TRUE;
  scrollWidth = 16;
  dragStatus = wxGRID_DRAG_NONE;
  dragRowOrCol = 0;
  dragStartPosition = 0;
  dragLastPosition = 0;
  divisionPen = wxThePenList->FindOrCreatePen("LIGHT GREY", 1, wxSOLID);
  
  if (!wxGridEntryFont)
    wxGridEntryFont = new wxFont(10, wxSWISS, wxNORMAL, wxBOLD);

  if (!horizontalSashCursor)
  {
    horizontalSashCursor = new wxCursor(wxCURSOR_SIZEWE);
    verticalSashCursor = new wxCursor(wxCURSOR_SIZENS);
  }
    
  SetLabelBackgroundColour(labelBackgroundColour);

  leftOfSheet = wxGRID_DEFAULT_SHEET_LEFT;
  topOfSheet = wxGRID_DEFAULT_SHEET_TOP;
  cellHeight = wxGRID_DEFAULT_CELL_HEIGHT;
  totalGridWidth = 0;
  totalGridHeight = 0;
  colWidths = NULL;
  rowHeights = NULL;

  verticalLabelWidth = wxGRID_DEFAULT_VERTICAL_LABEL_WIDTH;
  horizontalLabelHeight = wxGRID_DEFAULT_HORIZONAL_LABEL_HEIGHT;
  verticalLabelAlignment = wxCENTRE;
  horizontalLabelAlignment = wxCENTRE;
  editControlPosition.x = wxGRID_DEFAULT_EDIT_X;
  editControlPosition.y = wxGRID_DEFAULT_EDIT_Y;
  editControlPosition.width = wxGRID_DEFAULT_EDIT_WIDTH;
  editControlPosition.height = wxGRID_DEFAULT_EDIT_HEIGHT;

  wCursorRow = 0;
  wCursorColumn = 0;
  
  scrollPosX = 0;
  scrollPosY = 0;

  /* Store the rect. coordinates for the current cell */
  SetCurrentRect(wCursorRow, wCursorColumn);

  bEditCreated = FALSE;

  totalRows = 0;
  totalCols = 0;
  gridCells = NULL;
  rowLabelCells = NULL;
  colLabelCells = NULL;

  SetButtonFont(wxGridEntryFont);
  SetLabelFont(wxGridEntryFont);
  
  textItem = new wxText(this, (wxFunction)wxGridFunc, "XXXX", "",
     editControlPosition.x, editControlPosition.y, editControlPosition.width, -1,
     0);
  textItem->Show(TRUE);
  textItem->SetLabel("");
  textItem->SetFocus();
  int controlW, controlH;
  
  textItem->GetSize(&controlW, &controlH);
  editControlPosition.height = controlH;
  
  topOfSheet = editControlPosition.x + controlH + 2;

  bEditCreated = TRUE;
  
  hScrollBar = new wxScrollBar(this, (wxFunction)wxGridScrollFunc, 0, 0, 20, 100, wxHORIZONTAL);
  vScrollBar = new wxScrollBar(this, (wxFunction)wxGridScrollFunc, 0, 0, 100, 20, wxVERTICAL);
//  hScrollBar->Show(FALSE);
//  vScrollBar->Show(FALSE);
  AllowDoubleClick(TRUE);
}

wxGenericGrid::~wxGenericGrid(void)
{
  ClearGrid();
}

void wxGenericGrid::ClearGrid(void)
{
  int i,j;
  if (gridCells)
  {
    for (i = 0; i < totalRows; i++)
    {
      for (j = 0; j < totalCols; j++)
        if (gridCells[i][j])
          delete gridCells[i][j];
      delete[] gridCells[i];
    }
    delete[] gridCells;
    gridCells = NULL;
  }
  if (colWidths)
    delete[] colWidths;
  colWidths = NULL;
  if (rowHeights)
    delete[] rowHeights;
  rowHeights = NULL;
  
  if (rowLabelCells)
  {
    for (i = 0; i < totalRows; i++)
      delete rowLabelCells[i];
    delete[] rowLabelCells;
    rowLabelCells = NULL;
  }
  if (colLabelCells)
  {
    for (i = 0; i < totalCols; i++)
      delete colLabelCells[i];
    delete[] colLabelCells;
    colLabelCells = NULL;
  }
}

Bool wxGenericGrid::CreateGrid(int nRows, int nCols, wxString **cellValues, short *widths,
     short defaultWidth, short defaultHeight)
{
  totalRows = nRows;
  totalCols = nCols;

  int i,j;
  colWidths = new short[nCols];
  rowHeights = new short[nRows];
  for (i = 0; i < nCols; i++)
    if (widths)
      colWidths[i] = widths[i];
    else
      colWidths[i] = defaultWidth;
  for (i = 0; i < nRows; i++)
    rowHeights[i] = defaultHeight;
  
  gridCells = new wxGridCell **[nRows];
  
  for (i = 0; i < nRows; i++)
    gridCells[i] = new wxGridCell *[nCols];
    
  for (i = 0; i < nRows; i++)
    for (j = 0; j < nCols; j++)
      if (cellValues)
      {
        gridCells[i][j] = OnCreateCell();
        gridCells[i][j]->SetTextValue(cellValues[i][j]);
      }
      else
        gridCells[i][j] = NULL;
        
  rowLabelCells = new wxGridCell *[nRows];
  for (i = 0; i < nRows; i++)
    rowLabelCells[i] = new wxGridCell(this);
  colLabelCells = new wxGridCell *[nCols];
  for (i = 0; i < nCols; i++)
    colLabelCells[i] = new wxGridCell(this);

  wCursorRow = wCursorColumn = 0;
  SetCurrentRect(0, 0);

  // Need to determine various dimensions
  UpdateDimensions();

  // Number of 'lines'
  int objectSizeX = totalCols;
  int pageSizeX = 1;
  int viewLengthX = totalCols;
  hScrollBar->SetViewLength(viewLengthX);
  hScrollBar->SetObjectLength(objectSizeX);
  hScrollBar->SetPageLength(pageSizeX);

  int objectSizeY = totalRows;
  int pageSizeY = 1;
  int viewLengthY = totalRows;

  vScrollBar->SetViewLength(viewLengthY);
  vScrollBar->SetObjectLength(objectSizeY);
  vScrollBar->SetPageLength(pageSizeY);

  OnChangeLabels();
  OnChangeSelectionLabel();
  
  return TRUE;
}

// Need to determine various dimensions
void wxGenericGrid::UpdateDimensions(void)
{
  int canvasWidth, canvasHeight;
  GetSize(&canvasWidth, &canvasHeight);
  
  if (editable)
  {
    int controlW, controlH;
    GetTextItem()->GetSize(&controlW, &controlH);
    topOfSheet = editControlPosition.x + controlH + 2;
  }
  else
    topOfSheet = 0;
  rightOfSheet = leftOfSheet + verticalLabelWidth;
  int i;
  for (i = scrollPosX; i < totalCols; i++)
  {
    if (rightOfSheet > canvasWidth)
      break;
    else
      rightOfSheet += colWidths[i];
  }
  bottomOfSheet = topOfSheet + horizontalLabelHeight;
  for (i = scrollPosY; i < totalRows; i++)
  {
    if (bottomOfSheet > canvasHeight)
      break;
    else
      bottomOfSheet += rowHeights[i];
  }
    
  totalGridWidth = leftOfSheet + verticalLabelWidth;
  for (i = 0; i < totalCols; i++)
  {
    totalGridWidth += colWidths[i];
  }
  totalGridHeight = topOfSheet + horizontalLabelHeight;
  for (i = 0; i < totalRows; i++)
  {
    totalGridHeight += rowHeights[i];
  }
}

wxGridCell *wxGenericGrid::GetCell(int row, int col)
{
  if (!gridCells)
    return NULL;

  if ((row >= totalRows) || (col >= totalCols))
    return NULL;
    
  wxGridCell *cell = gridCells[row][col];
  if (!cell)
  {
    gridCells[row][col] = OnCreateCell();
    return gridCells[row][col];
  }
  else
    return cell;
}

void wxGenericGrid::SetGridClippingRegion(wxDC *dc)
{
  int scrollWidthHoriz = 0;
  int scrollWidthVert = 0;
  int cw, ch;
  GetClientSize(&cw, &ch);

  if (hScrollBar && hScrollBar->IsShown())
    scrollWidthHoriz = scrollWidth;
  if (vScrollBar && vScrollBar->IsShown())
    scrollWidthVert = scrollWidth;

  // Don't paint over the scrollbars
  dc->SetClippingRegion(leftOfSheet, topOfSheet,
     cw - scrollWidthVert - leftOfSheet, ch - scrollWidthHoriz - topOfSheet);
}

void wxGenericGrid::OnPaint(void)
{
  // Within a second, don't repaint.
  long timeNow = wxGetCurrentTime();
  if (timeNow == paintTime)
    return;

  paintTime = timeNow;

  wxRectangle rect;

  wxDC *dc = GetDC();
  dc->BeginDrawing();
  dc->SetOptimization(FALSE);
  
  SetGridClippingRegion(dc);

  DrawLabelAreas(dc);
  DrawEditableArea(dc);
  DrawColumnLabels(dc);
  DrawRowLabels(dc);
  DrawCells(dc);
  DrawGridLines(dc);

  /* Hilight present cell */
  SetCurrentRect(wCursorRow, wCursorColumn);
  if (currentRectVisible)
    HighlightCell(dc);

  dc->DestroyClippingRegion();
  dc->SetOptimization(TRUE);
  dc->EndDrawing();

}

void wxGenericGrid::DrawLabelAreas(wxDC *dc)
{
  int cw, ch;
  GetClientSize(&cw, &ch);

  dc->SetPen(wxTRANSPARENT_PEN);
//  dc->SetBrush(dc->GetBackground());

  // Should blank out any area which isn't going to be painted over.
//  dc->DrawRectangle(leftOfSheet, bottomOfSheet, cw - leftOfSheet, ch - bottomOfSheet);
//  dc->DrawRectangle(rightOfSheet, topOfSheet, cw - rightOfSheet, ch - topOfSheet);

  // Paint the label areas
  dc->SetBrush(labelBackgroundBrush);
//  dc->DrawRectangle(leftOfSheet, topOfSheet, rightOfSheet - leftOfSheet + 1, horizontalLabelHeight + 1);
  dc->DrawRectangle(leftOfSheet, topOfSheet, cw-leftOfSheet, horizontalLabelHeight + 1);
//  dc->DrawRectangle(leftOfSheet, topOfSheet, verticalLabelWidth + 1, bottomOfSheet - topOfSheet + 1);
  dc->DrawRectangle(leftOfSheet, topOfSheet, verticalLabelWidth + 1, ch-topOfSheet);
}

void wxGenericGrid::DrawEditableArea(wxDC *dc)
{
  int cw, ch;
  GetClientSize(&cw, &ch);

  dc->SetPen(wxTRANSPARENT_PEN);
  dc->SetBrush(wxTheBrushList->FindOrCreateBrush(&cellBackgroundColour, wxSOLID));
//  dc->DrawRectangle(leftOfSheet+verticalLabelWidth, topOfSheet+horizontalLabelHeight,
//      rightOfSheet-(leftOfSheet+verticalLabelWidth) + 1, bottomOfSheet - (topOfSheet+horizontalLabelHeight) + 1);
  dc->DrawRectangle(leftOfSheet+verticalLabelWidth, topOfSheet+horizontalLabelHeight,
      cw-(leftOfSheet+verticalLabelWidth), ch - (topOfSheet+horizontalLabelHeight));
}

void wxGenericGrid::DrawGridLines(wxDC *dc)
{
  int cw, ch;
  GetClientSize(&cw, &ch);
  
  int i;

  if (divisionPen)
  {
    dc->SetPen(divisionPen);

    int heightCount = topOfSheet + horizontalLabelHeight;

    // Draw horizontal grey lines for cells
    for (i = scrollPosY; i < (totalRows+1); i++)
    {
      if (heightCount > ch)
        break;
      else
      {
        dc->DrawLine((float)leftOfSheet, (float)heightCount,
                   (float)cw, (float)heightCount);
        if (i < totalRows)
          heightCount += rowHeights[i];
      }
    }
  }

  if (verticalLabelWidth > 0)
  {
    dc->SetPen(wxBLACK_PEN);

    // Draw horizontal black lines for row labels
    int heightCount = topOfSheet + horizontalLabelHeight;
    for (i = scrollPosY; i < (totalRows+1); i++)
    {
      if (heightCount > ch)
        break;
      else
      {
        dc->DrawLine((float)leftOfSheet, (float)heightCount,
                     (float)verticalLabelWidth, (float)heightCount);
        if (i < totalRows)
          heightCount += rowHeights[i];
      }
    }
    // Draw a black vertical line for row number cells
    dc->DrawLine(leftOfSheet + verticalLabelWidth, topOfSheet,
      leftOfSheet + verticalLabelWidth, ch);
    // First vertical line
    dc->DrawLine(leftOfSheet, topOfSheet, leftOfSheet, ch);

    dc->SetPen(wxWHITE_PEN);

    // Draw highlights on row labels
    heightCount = topOfSheet + horizontalLabelHeight;
    for (i = scrollPosY; i < totalRows; i++)
    {
      if (heightCount > ch)
        break;
      else
      {
        dc->DrawLine((float)leftOfSheet+1, (float)heightCount+1,
                     (float)verticalLabelWidth, (float)heightCount+1);
        dc->DrawLine((float)leftOfSheet+1, (float)heightCount+1,
                     (float)leftOfSheet+1, (float)heightCount + rowHeights[i] - 1);
        heightCount += rowHeights[i];
      }
    }
    // Last one - down to the floor.
    dc->DrawLine((float)leftOfSheet+1, (float)heightCount+1,
                 (float)verticalLabelWidth, (float)heightCount+1);
    dc->DrawLine((float)leftOfSheet+1, (float)heightCount+1,
                   (float)leftOfSheet+1, (float)ch);

  }

  if (divisionPen)
  {
    dc->SetPen(divisionPen);

    // Draw vertical grey lines for cells
    int widthCount = leftOfSheet + verticalLabelWidth;
    for (i = scrollPosX; i < totalCols; i++)
    {
      if (widthCount > cw)
        break;
      else
      {
        // Skip the first one
        if (i != scrollPosX)
        {
         dc->DrawLine(widthCount, topOfSheet + horizontalLabelHeight,
                       widthCount, bottomOfSheet);
        }
        widthCount += colWidths[i];
      }
    }
    // Last one
    dc->DrawLine(widthCount, topOfSheet + horizontalLabelHeight,
                    widthCount, bottomOfSheet);
  }

  dc->SetPen(wxBLACK_PEN);

  // Draw two black horizontal lines for column number cells
  dc->DrawLine(
          leftOfSheet, topOfSheet,
          cw, topOfSheet);
  dc->DrawLine(leftOfSheet,  topOfSheet + horizontalLabelHeight,
               cw, topOfSheet + horizontalLabelHeight);

  if (horizontalLabelHeight > 0)
  {
    int widthCount = leftOfSheet + verticalLabelWidth;
  
    // Draw black vertical lines for column number cells
    for (i = scrollPosX; i < totalCols; i++)
    {
      if (widthCount > cw)
        break;
      else
      {
        dc->DrawLine(widthCount, topOfSheet,
                      widthCount, topOfSheet + horizontalLabelHeight);
        widthCount += colWidths[i];
      }
    }

    // Last one
    dc->DrawLine(widthCount, topOfSheet,
                    widthCount, topOfSheet + horizontalLabelHeight);

    // Draw highlights
    dc->SetPen(wxWHITE_PEN);
    widthCount = leftOfSheet + verticalLabelWidth;
  
    for (i = scrollPosX; i < totalCols; i++)
    {
      if (widthCount > cw)
        break;
      else
      {
        dc->DrawLine(widthCount+1, topOfSheet+1,
                     widthCount+colWidths[i], topOfSheet+1);
        dc->DrawLine(widthCount+1, topOfSheet+1,
                     widthCount+1, topOfSheet+horizontalLabelHeight);
        widthCount += colWidths[i];
      }
    }
    // Last one - to the right side of the canvas.
    dc->DrawLine(widthCount+1, topOfSheet+1,
                   cw, topOfSheet+1);
    dc->DrawLine(widthCount+1, topOfSheet+1,
                   widthCount+1, topOfSheet+horizontalLabelHeight);

  }
}

void wxGenericGrid::DrawColumnLabels(wxDC *dc)
{
  int cw, ch;
  GetClientSize(&cw, &ch);

  if (horizontalLabelHeight == 0)
    return;
    
  int i;
  wxRectangle rect;

  // Draw letters for columns
  rect.y = topOfSheet + 1;
  rect.height = horizontalLabelHeight - 1;

  dc->SetTextBackground(&labelBackgroundColour);
  dc->SetBackgroundMode(wxTRANSPARENT);
//  dc->SetTextForeground(&labelTextColour);
  
  int widthCount = leftOfSheet + verticalLabelWidth;
  for (i = scrollPosX; i < totalCols; i++)
  {
     if (widthCount > cw)
       break;
     else
     {
       rect.x = 1 + widthCount;
       rect.width = colWidths[i];
       DrawColumnLabel(dc, &rect, i);

       widthCount += colWidths[i];
     }
  }
}

void wxGenericGrid::DrawColumnLabel(wxDC *dc, wxRectangle *rect, int col)
{
  wxGridCell *cell = GetLabelCell(wxHORIZONTAL, col);
  if (cell)
  {
    wxRectangle rect2;
    rect2 = *rect;
    rect2.x += 3;
    rect2.y += 2;
    rect2.width -= 5;
    rect2.height -= 4;
    dc->SetTextForeground(&GetLabelTextColour());
    dc->SetFont(GetLabelTextFont());
    DrawTextRect(dc, cell->GetTextValue().GetData(), &rect2, GetLabelAlignment(wxHORIZONTAL));
  }
}

void wxGenericGrid::DrawRowLabels(wxDC *dc)
{
  int cw, ch;
  GetClientSize(&cw, &ch);

  if (verticalLabelWidth == 0)
    return;

  int i;
  wxRectangle rect;
  
  // Draw numbers for rows
  rect.x = leftOfSheet;
  rect.width = verticalLabelWidth;

  int heightCount = topOfSheet + horizontalLabelHeight;

  dc->SetTextBackground(&labelBackgroundColour);
  dc->SetBackgroundMode(wxTRANSPARENT);

  for (i = scrollPosY; i < totalRows; i++)
  {
     if (heightCount > ch)
       break;
     else
     {
       rect.y = 1 + heightCount;
       rect.height = rowHeights[i];
       DrawRowLabel(dc, &rect, i);
     
       heightCount += rowHeights[i];
     }
  }
}

void wxGenericGrid::DrawRowLabel(wxDC *dc, wxRectangle *rect, int row)
{
  wxGridCell *cell = GetLabelCell(wxVERTICAL, row);
  if (cell)
  {
    wxRectangle rect2;
    rect2 = *rect;
    rect2.x += 3;
    rect2.y += 2;
    rect2.width -= 5;
    rect2.height -= 4;
    dc->SetTextForeground(&GetLabelTextColour());
    dc->SetFont(GetLabelTextFont());
    DrawTextRect(dc, cell->GetTextValue().GetData(), &rect2, GetLabelAlignment(wxVERTICAL));
  }
}

void wxGenericGrid::DrawCells(wxDC *dc)
{
  int cw, ch;
  GetClientSize(&cw, &ch);

  int i,j;
  
  // Draw value corresponding to each cell
  for (i = scrollPosY; i < totalRows; i++)
  {
    for (j = scrollPosX; j < totalCols; j++)
    {
      SetCurrentRect(i, j, cw, ch);
      if (currentRectVisible)
      {
        DrawCellBackground(dc, &CurrentRect, i, j);
        DrawCellValue(dc, &CurrentRect, i, j);
      }
      if (CurrentRect.x > cw)
        break;
    }
    if (CurrentRect.y > ch)
      break;
  }
  dc->SetBackgroundMode(wxSOLID);
  dc->SetPen(wxBLACK_PEN);
}

void wxGenericGrid::DrawCellBackground(wxDC *dc, wxRectangle *rect, int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
  {
    dc->SetBrush(cell->GetBackgroundBrush());
    dc->SetPen(wxTRANSPARENT_PEN);
#ifdef wx_motif
    dc->DrawRectangle(rect->x+1, rect->y+1, rect->width-1, rect->height-1);
#else
    dc->DrawRectangle(rect->x+1, rect->y+1, rect->width, rect->height);
#endif
    dc->SetPen(wxBLACK_PEN);
  }
}

void wxGenericGrid::DrawCellValue(wxDC *dc, wxRectangle *rect, int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
  {
    wxBitmap *bitmap = cell->GetCellBitmap();
    wxRectangle rect2;
    rect2 = *rect;
    rect2.x += 3;
    rect2.y += 2;
    rect2.width -= 5;
    rect2.height -= 4;

    if (bitmap)
    {
      DrawBitmapRect(dc, bitmap, &rect2, cell->GetAlignment());
    }
    else
    {
      dc->SetBackgroundMode(wxTRANSPARENT);
      dc->SetTextForeground(&cell->GetTextColour());
      dc->SetFont(cell->GetFont());

      DrawTextRect(dc, cell->GetTextValue().GetData(), &rect2, cell->GetAlignment());
    }
  }
}

void wxGenericGrid::AdjustScrollbars(void)
{
  int cw, ch;
  GetClientSize(&cw, &ch);
  
  // We find the view size by seeing how many rows/cols fit on
  // the current view.
  // BUT... this means that the scrollbar should be adjusted every time
  // it's scrolled, as well as when sized, because with variable size rows/cols,
  // the number of rows/col visible on the view differs according to what bit
  // you're looking at. The object length is always the same, but the
  // view length differs.

  // Since this may not be known until the end of this function, we should probably call AdjustScrollbars
  // twice.
  int vertScrollBarWidth = scrollWidth;
  int horizScrollBarHeight = scrollWidth;
  if (vScrollBar && !vScrollBar->IsShown())
    vertScrollBarWidth = 0;
  if (hScrollBar && !hScrollBar->IsShown())
    horizScrollBarHeight = 0;
  
  int noHorizSteps = 0;
  int noVertSteps = 0;
  
  if (totalGridWidth <= cw)
    noHorizSteps = 0;
  else
  {
    noHorizSteps = 0;
    int widthCount = 0;

    int i;
	int nx = 0;
    for (i = scrollPosX ; i < totalCols; i++)
    {
      widthCount += colWidths[i];
	  // A partial bit doesn't count, we still have to scroll to see the
	  // rest of it
      if (widthCount + leftOfSheet + verticalLabelWidth > (cw-vertScrollBarWidth))
        break;
	  else
   	    nx ++;

    }
    
    noHorizSteps += nx;
  }
  if (totalGridHeight <= ch)
    noVertSteps = 0;
  else
  {
    noVertSteps = 0;
    int heightCount = 0;

    int i;
	int ny = 0;
    for (i = scrollPosY ; i < totalRows; i++)
    {
      heightCount += rowHeights[i];
	  // A partial bit doesn't count, we still have to scroll to see the
	  // rest of it
      if (heightCount + topOfSheet + horizontalLabelHeight > (ch-horizScrollBarHeight))
        break;
	  else
	    ny ++;
    }
    
    noVertSteps += ny;
  }
  
  if (totalGridWidth <= cw)
  {
	if ( hScrollBar )
    	hScrollBar->Show(FALSE);
    SetScrollPosX(0);
  }
  else
  {
    	if ( hScrollBar )
	      	hScrollBar->Show(TRUE);
  }

  if (totalGridHeight <= ch)
  {
	if ( vScrollBar )
	  vScrollBar->Show(FALSE);
    SetScrollPosY(0);
  }
  else
  {
    	if ( vScrollBar )
	     	vScrollBar->Show(TRUE);
  }

  UpdateDimensions(); // Necessary in case scrollPosX/Y changed

  vertScrollBarWidth = scrollWidth;
  horizScrollBarHeight = scrollWidth;
  if (vScrollBar && !vScrollBar->IsShown())
    vertScrollBarWidth = 0;
  if (hScrollBar && !hScrollBar->IsShown())
    horizScrollBarHeight = 0;

  if (hScrollBar)
  {
    int nCols = GetCols();
        hScrollBar->SetPageLength(wxMax(noHorizSteps, 1));
        hScrollBar->SetViewLength(wxMax(noHorizSteps, 1));
        hScrollBar->SetObjectLength(nCols);

	    hScrollBar->SetSize(leftOfSheet, ch - scrollWidth,
	cw - vertScrollBarWidth - leftOfSheet, scrollWidth);
  }
       
  if (vScrollBar)
  {
    int nRows = GetRows();
        vScrollBar->SetPageLength(wxMax(noVertSteps, 1));
        vScrollBar->SetViewLength(wxMax(noVertSteps, 1));
        vScrollBar->SetObjectLength(nRows);

        vScrollBar->SetSize(cw - scrollWidth, topOfSheet,
           scrollWidth, ch - topOfSheet - horizScrollBarHeight);
  }
}

// Old code (1.66F)
#if 0
void wxGenericGrid::AdjustScrollbars(void)
{
  int cw, ch;
  GetClientSize(&cw, &ch);
  
  // To calculate the number of steps for each scrollbar,
  // we need to see how much will fit onto the canvas
  // at the present size. So:
  // 1) Find the *last* row r1 such that when it's at the top of the
  //    window, all the remaining rows are visible.
  // 2) There should therefore be r1 - 1 steps in the scrollbar.
  // Similarly with columns.
  
  int noHorizSteps = 0;
  int noVertSteps = 0;
  
  if (totalGridWidth <= cw)
    noHorizSteps = 0;
  else
  {
    // Before wxWindows 1.66, there is a difference in interpretation
    // of wxScrollBar::SetObjectLength. Consistent now.
#if 0 // def wx_msw
    // Old pre-1.66 code
    int widthCount = 0;
    int i;
    for (i = (totalCols - 1); i >= 0; i--)
    {
      widthCount += colWidths[i];
      if (widthCount + leftOfSheet + verticalLabelWidth > (cw-scrollWidth))
        break;
    }
    noHorizSteps = wxMax(i+1, 0);
#else
    noHorizSteps = 0;
    int widthCount = 0;
    if (GetLabelSize(wxVERTICAL) > 0)
      noHorizSteps ++;

    int i;
    for (i = 0 ; i < totalCols; i++)
    {
      widthCount += colWidths[i];
      if (widthCount + leftOfSheet + verticalLabelWidth > (cw-scrollWidth))
        break;
    }
    
    noHorizSteps += i;
#endif
  }
  if (totalGridHeight <= ch)
    noVertSteps = 0;
  else
  {
#if 0 // def wx_msw
    // Old pre-1.66 code
    int heightCount = 0;
    int i;
    for (i = (totalRows - 1); i >= 0; i--)
    {
      heightCount += rowHeights[i];
      if (heightCount + topOfSheet + horizontalLabelHeight > (ch-scrollWidth))
        break;
    }
    
    noVertSteps = wxMax(i+1, 0);
#else
    noVertSteps = 0;
    int heightCount = 0;
    if (GetLabelSize(wxHORIZONTAL) > 0)
      noVertSteps ++;

    int i;
    for (i = 0 ; i < totalRows; i++)
    {
      heightCount += rowHeights[i];
      if (heightCount + topOfSheet + horizontalLabelHeight > (ch-scrollWidth))
        break;
    }
    
    noVertSteps += i;
#endif
  }
  
  if (totalGridWidth <= cw)
  {
    hScrollBar->Show(FALSE);
    SetScrollPosX(0);
  }
  else
  {
    hScrollBar->Show(TRUE);
  }

  if (totalGridHeight <= ch)
  {
    vScrollBar->Show(FALSE);
    SetScrollPosY(0);
  }
  else
    vScrollBar->Show(TRUE);

  UpdateDimensions(); // Necessary in case scrollPosX/Y changed

  int vertScrollBarWidth = scrollWidth;
  int horizScrollBarHeight = scrollWidth;
  if (!vScrollBar->IsShown())
  {
    vertScrollBarWidth = 0;
  }
  if (!hScrollBar->IsShown())
    horizScrollBarHeight = 0;

  if (hScrollBar)
  {
    int nCols = GetCols();

    // Plus size allowance for column label if necessary
    if (GetLabelSize(wxVERTICAL) != 0)
      nCols ++;
    hScrollBar->SetViewLength(wxMax(noHorizSteps, 1));
#if 0 // def wx_msw
    // Old pre-1.66 code
    hScrollBar->SetObjectLength(noHorizSteps);
#else
    hScrollBar->SetObjectLength(nCols);
#endif
    hScrollBar->SetPageLength(1);

    hScrollBar->SetSize(leftOfSheet, ch - scrollWidth,
      cw - vertScrollBarWidth - leftOfSheet, scrollWidth);
  }
       
  if (vScrollBar)
  {
    int nRows = GetRows();

    // Plus size allowance for column label if necessary
    if (GetLabelSize(wxHORIZONTAL) != 0)
      nRows ++;

    vScrollBar->SetViewLength(wxMax(noVertSteps, 1));
#if 0 // def wx_msw
    // Old pre-1.66 code
    vScrollBar->SetObjectLength(noVertSteps);
#else
    vScrollBar->SetObjectLength(nRows);
#endif
    vScrollBar->SetPageLength(1);

    vScrollBar->SetSize(cw - scrollWidth, topOfSheet,
       scrollWidth, ch - topOfSheet - horizScrollBarHeight);
  }
}
#endif

void wxGenericGrid::OnSize(int w, int h)
{
  static Bool inSize = FALSE;

  if (inSize)
    return;

  inSize = TRUE;
  AdjustScrollbars();
  
  int cw, ch;
  GetClientSize(&cw, &ch);

  if (GetTextItem() && GetTextItem()->IsShown())
  {
        GetTextItem()->SetSize(editControlPosition.x, editControlPosition.y,
          cw - editControlPosition.x, editControlPosition.height);
  }

  inSize = FALSE;
}

Bool wxGenericGrid::CellHitTest(int x, int y, int *row, int *col)
{
      // Find the selected cell and call OnSelectCell
      if (x >= (leftOfSheet + verticalLabelWidth) && y >= (topOfSheet + horizontalLabelHeight) &&
          x <= rightOfSheet && y <= bottomOfSheet)
      {
         // Calculate the cell number from x and y
         x -= (verticalLabelWidth + leftOfSheet);
         y -= (topOfSheet + horizontalLabelHeight);

         int i;
         
         // Now we need to do a hit test for which row we're on
         int currentHeight = 0;
         for (i = scrollPosY; i < totalRows; i++)
         {
            if (y >= currentHeight && y <= (currentHeight + rowHeights[i]))
            {
              *row = i;
              break;
            }
            currentHeight += rowHeights[i];
         }
         
         // Now we need to do a hit test for which column we're on
         int currentWidth = 0;
         for (i = scrollPosX; i < totalCols; i++)
         {
            if (x >= currentWidth && x <= (currentWidth + colWidths[i]))
            {
              *col = i;
              break;
            }
            currentWidth += colWidths[i];
         }
         return TRUE;
       }
  return FALSE;
}

Bool wxGenericGrid::LabelSashHitTest(int x, int y, int *orientation, int *rowOrCol, int *startPos)
{
  int i;
  int tolerance = 3;
  
  if (x >= (leftOfSheet + verticalLabelWidth) && y >= topOfSheet &&
      x <= rightOfSheet && y <= (topOfSheet + horizontalLabelHeight))
  {
    // We may be on a column label sash.
    int currentWidth = leftOfSheet + verticalLabelWidth;
    for (i = scrollPosX; i < totalCols; i++)
    {
      if (x >= (currentWidth + colWidths[i] - tolerance) && x <= (currentWidth + colWidths[i] + tolerance))
      {
        *orientation = wxHORIZONTAL;
        *rowOrCol = i;
        *startPos = currentWidth;
        return TRUE;
      }
      currentWidth += colWidths[i];
    }
    return FALSE;
  }
  else if (x >= leftOfSheet && y >= (topOfSheet + horizontalLabelHeight) &&
      x <= (leftOfSheet + verticalLabelWidth) && y <= bottomOfSheet)
  {
    // We may be on a row label sash.
    int currentHeight = topOfSheet + horizontalLabelHeight;
    for (i = scrollPosY; i < totalRows; i++)
    {
      if (y >= (currentHeight + rowHeights[i] - tolerance) && y <= (currentHeight + rowHeights[i] + tolerance))
      {
        *orientation = wxVERTICAL;
        *rowOrCol = i;
        *startPos = currentHeight;
        return TRUE;
      }
      currentHeight += rowHeights[i];
    }
    return FALSE;
  }
  return FALSE;
}

Bool wxGenericGrid::LabelHitTest(int x, int y, int *row, int *col)
{
      // Find the selected label
      if (x >= leftOfSheet && y >= topOfSheet &&
          x <= rightOfSheet && y <= bottomOfSheet)
      {
         // Calculate the cell number from x and y
         x -= leftOfSheet;
         y -= topOfSheet;

         int i;

         // Now we need to do a hit test for which row we're on
         int currentHeight = horizontalLabelHeight;
         for (i = scrollPosY; i < totalRows; i++)
         {
            if (y >= currentHeight && y <= (currentHeight + rowHeights[i]))
            {
              *row = i;
              break;
            }
            currentHeight += rowHeights[i];
         }
         if (y >= 0 && y <= horizontalLabelHeight)
         {
             *row = -1;
         }

         // Now we need to do a hit test for which column we're on
         int currentWidth = verticalLabelWidth;
         for (i = scrollPosX; i < totalCols; i++)
         {
            if (x >= currentWidth && x <= (currentWidth + colWidths[i]))
            {
              *col = i;
              break;
            }
            currentWidth += colWidths[i];
         }
         if (x >= 0 && x <= verticalLabelWidth)
         {
             *col = -1;
         }

         if ((*col == -1) || (*row == -1))
         {
             return TRUE;
         }
       }
  return FALSE;
}

void wxGenericGrid::OnEvent(wxMouseEvent& ev)
{
  if (ev.LeftDown())
  {
    wxDC *dc = GetDC();
    dc->BeginDrawing();
    
    int row, col;
    if (CellHitTest((int)ev.x, (int)ev.y, &row, &col))
    {
      OnSelectCellImplementation(dc, row, col);
      OnCellLeftClick(row, col, (int)ev.x, (int)ev.y, ev.ControlDown(), ev.ShiftDown());
    }
    if (LabelHitTest((int)ev.x, (int)ev.y, &row, &col))
    {
      OnLabelLeftClick(row, col, (int)ev.x, (int)ev.y, ev.ControlDown(), ev.ShiftDown());
    }
    dc->EndDrawing();
  }
  else if (ev.Dragging() && ev.LeftIsDown())
  {
    switch (dragStatus)
    {
      case wxGRID_DRAG_NONE:
      {
        int orientation;
        if (LabelSashHitTest((int)ev.x, (int)ev.y, &orientation, &dragRowOrCol, &dragStartPosition))
        {
          if (orientation == wxHORIZONTAL)
          {
            dragStatus = wxGRID_DRAG_LEFT_RIGHT;
            SetCursor(horizontalSashCursor);
            dragLastPosition = (int)ev.x;
          }
          else
          {
            dragStatus = wxGRID_DRAG_UP_DOWN;
            SetCursor(verticalSashCursor);
            dragLastPosition = (int)ev.y;
          }
          wxDC *dc = GetDC();
          dc->BeginDrawing();
          dc->SetLogicalFunction(wxINVERT);
          if (orientation == wxHORIZONTAL)
            dc->DrawLine((int)ev.x, topOfSheet, (int)ev.x, bottomOfSheet);
          else
            dc->DrawLine(leftOfSheet, (int)ev.y, rightOfSheet, (int)ev.y);
          dc->EndDrawing();
          
          CaptureMouse();
        }
        break;
      }
      case wxGRID_DRAG_LEFT_RIGHT:
      {
        wxDC *dc = GetDC();
        dc->BeginDrawing();
        dc->SetLogicalFunction(wxINVERT);
        dc->DrawLine(dragLastPosition, topOfSheet, dragLastPosition, bottomOfSheet);

        dc->DrawLine((int)ev.x, topOfSheet, (int)ev.x, bottomOfSheet);
        dc->EndDrawing();

        dragLastPosition = (int)ev.x;
        SetCursor(horizontalSashCursor);
        break;
      }
      case wxGRID_DRAG_UP_DOWN:
      {
        wxDC *dc = GetDC();
        dc->BeginDrawing();
        dc->SetLogicalFunction(wxINVERT);
        dc->DrawLine(leftOfSheet, dragLastPosition, rightOfSheet, dragLastPosition);

        dc->DrawLine(leftOfSheet, (int)ev.y, rightOfSheet, (int)ev.y);
        dc->EndDrawing();

        dragLastPosition = (int)ev.y;
        SetCursor(verticalSashCursor);
        break;
      }
    }
  }
  else if (ev.Moving())
  {
    int rowOrCol, orientation, startPos;
    if (LabelSashHitTest((int)ev.x, (int)ev.y, &orientation, &rowOrCol, &startPos))
    {
      if (orientation == wxHORIZONTAL)
        SetCursor(horizontalSashCursor);
       else
        SetCursor(verticalSashCursor);
    }
    else
      SetCursor(wxSTANDARD_CURSOR);
  }
  else if (ev.LeftUp())
  {
    switch (dragStatus)
    {
      case wxGRID_DRAG_LEFT_RIGHT:
      {
        wxDC *dc = GetDC();
        dc->BeginDrawing();
        dc->SetLogicalFunction(wxINVERT);
        dc->DrawLine(dragLastPosition, topOfSheet, dragLastPosition, bottomOfSheet);
        dc->SetLogicalFunction(wxCOPY);
        dc->EndDrawing();

        ReleaseMouse();
        if (ev.x > dragStartPosition)
        {
          colWidths[dragRowOrCol] = (short)(ev.x - dragStartPosition);
          UpdateDimensions();
          AdjustScrollbars();
	  paintTime = 0;
	  Refresh();
        }
        SetCursor(wxSTANDARD_CURSOR);
        break;
      }
      case wxGRID_DRAG_UP_DOWN:
      {
        wxDC *dc = GetDC();
        dc->BeginDrawing();
        dc->SetLogicalFunction(wxINVERT);
        dc->DrawLine(leftOfSheet, dragLastPosition, rightOfSheet, dragLastPosition);
        dc->SetLogicalFunction(wxCOPY);
        dc->EndDrawing();

        ReleaseMouse();
        if (ev.y > dragStartPosition)
        {
          rowHeights[dragRowOrCol] = (short)(ev.y - dragStartPosition);
          UpdateDimensions();
          AdjustScrollbars();
	  paintTime = 0;
          Refresh();
        }
        SetCursor(wxSTANDARD_CURSOR);
        break;
      }
    }
    dragStatus = wxGRID_DRAG_NONE;
  }
  else if (ev.RightDown())
  {
    int row, col;
    if (CellHitTest((int)ev.x, (int)ev.y, &row, &col))
    {
      OnCellRightClick(row, col, (int)ev.x, (int)ev.y, ev.ControlDown(), ev.ShiftDown());
    }
    if (LabelHitTest((int)ev.x, (int)ev.y, &row, &col))
    {
      OnLabelRightClick(row, col, (int)ev.x, (int)ev.y, ev.ControlDown(), ev.ShiftDown());
    }
  }
}

void wxGenericGrid::OnSelectCellImplementation(wxDC *dc, int row, int col)
{
  wCursorColumn = col;
  wCursorRow = row;
  
  OnChangeSelectionLabel();

  SetGridClippingRegion(dc);

  // Remove the highlight from the old cell
  if (currentRectVisible)
     HighlightCell(dc);

  // Highlight the new cell and copy its content to the edit control
  SetCurrentRect(wCursorRow, wCursorColumn);
  wxGridCell *cell = GetCell(wCursorRow, wCursorColumn);
  if (cell)
    textItem->SetValue(cell->GetTextValue().GetData());

  SetGridClippingRegion(dc);

  // Why isn't this needed for Windows??
  // Probably because of the SetValue??
#ifndef wx_msw
  HighlightCell(dc);
#endif
  dc->DestroyClippingRegion();
  
  OnSelectCell(row, col);
}

wxGridCell *wxGenericGrid::OnCreateCell(void)
{
  return new wxGridCell(this);
}

void wxGenericGrid::OnChangeLabels(void)
{
  char buf[100];
  int i;
  for (i = 0; i < totalRows; i++)
  {
    sprintf(buf, "%d", i+1);
    SetLabelValue(wxVERTICAL, buf, i);
  }
  // A...Z,AA...ZZ,AAA...ZZZ, etc.
  for (i = 0; i < totalCols; i++)
  {
    int j;
    int noTimes = (i/26 + 1);
    int ch = (i % 26) + 65;
    buf[0] = 0;
    for (j = 0; j < noTimes; j++)
    {
      char buf2[20];
      sprintf(buf2, "%c", (char)ch);
      strcat(buf, buf2);
    }
    SetLabelValue(wxHORIZONTAL, buf, i);
  }
}

void wxGenericGrid::OnChangeSelectionLabel(void)
{
  if (!GetEditable())
    return;
    
  wxString rowLabel(GetLabelValue(wxVERTICAL, GetCursorRow()));
  wxString colLabel(GetLabelValue(wxHORIZONTAL, GetCursorColumn()));
  
  wxString newLabel = colLabel + rowLabel;
  if ((newLabel.Length() > 0) && (newLabel.Length() <= 8) && GetTextItem())
  {
    GetTextItem()->SetLabel(newLabel.GetData());
  }
}

void wxGenericGrid::HighlightCell(wxDC *dc)
{
  dc->SetLogicalFunction(wxINVERT);
  // Top
  dc->DrawLine(CurrentRect.x + 1, CurrentRect.y + 1, CurrentRect.x + CurrentRect.width - 1, CurrentRect.y + 1);
  // Right
  dc->DrawLine(CurrentRect.x + CurrentRect.width - 1, CurrentRect.y + 1,
      CurrentRect.x + CurrentRect.width - 1, CurrentRect.y +CurrentRect.height - 1);
  // Bottom
  dc->DrawLine(CurrentRect.x + CurrentRect.width - 1, CurrentRect.y + CurrentRect.height - 1,
         CurrentRect.x + 1, CurrentRect.y + CurrentRect.height - 1);
  // Left
  dc->DrawLine(CurrentRect.x + 1, CurrentRect.y + CurrentRect.height - 1, CurrentRect.x + 1, CurrentRect.y + 1);

  dc->SetLogicalFunction(wxCOPY);
}

void wxGenericGrid::DrawCellText(void)
{
  if (!currentRectVisible)
    return;
  
  wxGridCell *cell = GetCell(GetCursorRow(), GetCursorColumn());
  if (!cell)
    return;
    
  static char szEdit[300];

  wxDC *dc = GetDC();
  dc->BeginDrawing();

  SetGridClippingRegion(dc);

  dc->SetBackgroundMode(wxTRANSPARENT);
  dc->SetBrush(cell->GetBackgroundBrush());

  strcpy(szEdit, textItem->GetValue());
  
  wxRectangle rect;
  rect = CurrentRect;
  rect.x += 3;
  rect.y += 2;
  rect.width -= 5;
  rect.height -= 4;

  DrawTextRect(dc, "                                    ", &rect, wxLEFT);
  DrawTextRect(dc, szEdit, &rect, cell->GetAlignment());
  
  dc->DestroyClippingRegion();
  
  dc->SetBackgroundMode(wxSOLID);

  dc->EndDrawing();
}

void wxGenericGrid::SetCurrentRect(int Row, int Column, int canvasW, int canvasH)
{
  int currentWidth = leftOfSheet + verticalLabelWidth;
  int i;
  for (i = scrollPosX; i < Column; i++)
    currentWidth += colWidths[i];

  int currentHeight = topOfSheet + horizontalLabelHeight;
  for (i = scrollPosY; i < Row; i++)
    currentHeight += rowHeights[i];

  CurrentRect.x = currentWidth;
  CurrentRect.y = currentHeight;
  CurrentRect.width = colWidths ? (colWidths[Column]) : 0;
  CurrentRect.height = rowHeights ? (rowHeights[Row]) : 0;
  
  if (Row < scrollPosY || Column < scrollPosX)
    currentRectVisible = FALSE;
  else if ((canvasW != -1 && canvasH != -1) && (CurrentRect.x > canvasW || CurrentRect.y > canvasH))
    currentRectVisible = FALSE;
  else currentRectVisible = TRUE;
}

static Bool wxRectIntersection(wxRectangle *rect1, wxRectangle *rect2, wxRectangle *rect3)
{
  int x2_1 = rect1->x + rect1->width;
  int y2_1 = rect1->y + rect1->height;

  int x2_2 = rect2->x + rect2->width;
  int y2_2 = rect2->y + rect2->height;
  
  int x2_3, y2_3;
  
  // Check for intersection
  if ((rect1->x > x2_2) || (rect2->x > x2_1) ||
      (rect1->y > y2_2) || (rect2->y > y2_1))
  {
    // No intersection
    rect3->x = rect3->y = rect3->width = rect3->height = 0;
    return FALSE;
  }

  if (rect1->x > rect2->x)
    rect3->x = rect1->x;
  else
    rect3->x = rect2->x;
  if (rect1->y > rect2->y)
    rect3->y = rect1->y;
  else
    rect3->y = rect2->y;
  
  if (x2_1 > x2_2)
    x2_3 = x2_2;
  else
    x2_3 = x2_1;
  if (y2_1 > y2_2)
    y2_3 = y2_2;
  else
    y2_3 = y2_1;
    
  rect3->width = (int)(x2_3 - rect3->x);
  rect3->height = (int)(y2_3 - rect3->y);
  return TRUE;
}

void wxGenericGrid::DrawTextRect(wxDC *dc, char *text, wxRectangle *rect, int flag)
{
  dc->BeginDrawing();
  
  // Ultimately, this functionality should be built into wxWindows,
  // and optimized for each platform. E.g. on Windows, use DrawText
  // passing a clipping rectangle, so that the wxWindows clipping region
  // does not have to be used to implement this.
  
  // If we're already clipping, we need to find the intersection
  // between current clipping area and text clipping area.
  
  wxRectangle clipRect;
  wxRectangle clipRect2;
  float clipX, clipY, clipW, clipH;
  ((wxCanvasDC *)dc)->GetClippingBox(&clipX, &clipY, &clipW, &clipH);
  clipRect.x = (int)clipX; clipRect.y = (int)clipY;
  clipRect.width = (int)clipW; clipRect.height = (int)clipH;
  
  Bool alreadyClipping = TRUE;
  
  if (clipRect.x == 0 && clipRect.y == 0 && clipRect.width == 0 && clipRect.height == 0)
  {
    alreadyClipping = FALSE;
    clipRect2.x = rect->x; clipRect2.y = rect->y;
    clipRect2.width = rect->width; clipRect2.height = rect->height;
  }
  else
  {
    // Find intersection.
    if (!wxRectIntersection(rect, &clipRect, &clipRect2))
      return;
  }

  if (alreadyClipping)
    dc->DestroyClippingRegion();
  
  dc->SetClippingRegion(clipRect2.x, clipRect2.y, clipRect2.width, clipRect2.height);
  float textWidth, textHeight;
  
  dc->GetTextExtent(text, &textWidth, &textHeight);
  
  // Do alignment
  float x,y;
  switch (flag)
  {
    case wxRIGHT:
    {
      x = (float)(rect->x + rect->width - textWidth - 1.0);
      y = (float)(rect->y + (rect->height - textHeight)/2.0);
      break;
    }
    case wxCENTRE:
    {
      x = (float)(rect->x + (rect->width - textWidth)/2.0);
      y = (float)(rect->y + (rect->height - textHeight)/2.0);
      break;
    }
    case wxLEFT:
    default:
    {
      x = (float)(rect->x + 1.0);
      y = (float)(rect->y + (rect->height - textHeight)/2.0);
      break;
    }
  }
  dc->DrawText(text, x, y);
  
  dc->DestroyClippingRegion();

  // Restore old clipping
  if (alreadyClipping)
    dc->SetClippingRegion(clipRect.x, clipRect.y, clipRect.width, clipRect.height);

  dc->EndDrawing();
}

void wxGenericGrid::DrawBitmapRect(wxDC *dc, wxBitmap *bitmap, wxRectangle *rect, int flag)
{
  dc->BeginDrawing();
  
  // Ultimately, this functionality should be built into wxWindows,
  // and optimized for each platform. E.g. on Windows, use DrawText
  // passing a clipping rectangle, so that the wxWindows clipping region
  // does not have to be used to implement this.
  
  // If we're already clipping, we need to find the intersection
  // between current clipping area and text clipping area.
  
  wxRectangle clipRect;
  wxRectangle clipRect2;
  float clipX, clipY, clipW, clipH;
  ((wxCanvasDC *)dc)->GetClippingBox(&clipX, &clipY, &clipW, &clipH);
  clipRect.x = (int)clipX; clipRect.y = (int)clipY;
  clipRect.width = (int)clipW; clipRect.height = (int)clipH;
  
  Bool alreadyClipping = TRUE;
  
  if (clipRect.x == 0 && clipRect.y == 0 && clipRect.width == 0 && clipRect.height == 0)
  {
    alreadyClipping = FALSE;
    clipRect2.x = rect->x; clipRect2.y = rect->y;
    clipRect2.width = rect->width; clipRect2.height = rect->height;
  }
  else
  {
    // Find intersection.
    if (!wxRectIntersection(rect, &clipRect, &clipRect2))
      return;
  }

  if (alreadyClipping)
    dc->DestroyClippingRegion();
  
  dc->SetClippingRegion(clipRect2.x, clipRect2.y, clipRect2.width, clipRect2.height);
  float bitmapWidth, bitmapHeight;
  
  bitmapWidth = bitmap->GetWidth();
  bitmapHeight = bitmap->GetHeight();
  
  // Do alignment
  float x,y;
  switch (flag)
  {
    case wxRIGHT:
    {
      x = (float)(rect->x + rect->width - bitmapWidth - 1.0);
      y = (float)(rect->y + (rect->height - bitmapHeight)/2.0);
      break;
    }
    case wxCENTRE:
    {
      x = (float)(rect->x + (rect->width - bitmapWidth)/2.0);
      y = (float)(rect->y + (rect->height - bitmapHeight)/2.0);
      break;
    }
    case wxLEFT:
    default:
    {
      x = (float)(rect->x + 1.0);
      y = (float)(rect->y + (rect->height - bitmapHeight)/2.0);
      break;
    }
  }
  wxMemoryDC dcTemp;
  dcTemp.SelectObject(bitmap);
  
  dc->Blit(x, y, bitmapWidth, bitmapHeight, &dcTemp, 0, 0);
  dcTemp.SelectObject(NULL);
  
  dc->DestroyClippingRegion();

  // Restore old clipping
  if (alreadyClipping)
    dc->SetClippingRegion(clipRect.x, clipRect.y, clipRect.width, clipRect.height);

  dc->EndDrawing();
}

void wxGenericGrid::OnActivate(Bool active)
{
  if (active)
  {
    // Edit control should always have the focus
    if (GetTextItem() && GetEditable())
    {
      GetTextItem()->SetFocus();
      wxGridCell *cell = GetCell(GetCursorRow(), GetCursorColumn());
      if (cell)
        GetTextItem()->SetValue(cell->GetTextValue().GetData());
    }
  }
}

void wxGenericGrid::SetCellValue(const wxString& val, int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
  {
    cell->SetTextValue(val);

    RefreshCell(row, col, TRUE);
  }
}

void wxGenericGrid::RefreshCell(int row, int col, Bool setText)
{
    // Don't refresh within a pair of batch brackets
    if (GetBatchCount() > 0)
        return;
      
    int cw, ch;
    GetClientSize(&cw, &ch);

    SetCurrentRect(row, col, cw, ch);
    if (currentRectVisible)
    {
      wxGridCell *cell = GetCell(row, col);

      Bool currentPos = FALSE;
      if (row == wCursorRow && col == wCursorColumn && GetTextItem() && GetTextItem()->IsShown() && setText)
      {
        GetTextItem()->SetValue(cell->GetTextValue().GetData());
        currentPos = TRUE;
      }
      // Gets refreshed anyway in MSW
#ifdef wx_msw
      if (!currentPos)
#endif
      {
        wxDC *dc = GetDC();
        dc->BeginDrawing();
        DrawCellBackground(dc, &CurrentRect, row, col);
        DrawCellValue(dc, &CurrentRect, row, col);
        dc->EndDrawing();
      }
    }
}

wxString& wxGenericGrid::GetCellValue(int row, int col)
{
  static wxString emptyString("");
 
  wxGridCell *cell = GetCell(row, col);
  if (cell)
    return cell->GetTextValue();
  else
    return emptyString;
}

void wxGenericGrid::SetColumnWidth(int col, int width)
{
  if (col <= totalCols)
    colWidths[col] = width;
}

int wxGenericGrid::GetColumnWidth(int col)
{
  if (col <= totalCols)
    return colWidths[col];
  else
    return 0;
}

void wxGenericGrid::SetRowHeight(int row, int height)
{
  if (row <= totalRows)
    rowHeights[row] = height;
}

int wxGenericGrid::GetRowHeight(int row)
{
  if (row <= totalRows)
    return rowHeights[row];
  else
    return 0;
}

void wxGenericGrid::SetLabelSize(int orientation, int sz)
{
  if (orientation == wxHORIZONTAL)
    horizontalLabelHeight = sz;
  else
    verticalLabelWidth = sz;
  UpdateDimensions();
  SetCurrentRect(GetCursorRow(), GetCursorColumn());
}

int wxGenericGrid::GetLabelSize(int orientation)
{
  if (orientation == wxHORIZONTAL)
    return horizontalLabelHeight;
  else
    return verticalLabelWidth;
}

wxGridCell *wxGenericGrid::GetLabelCell(int orientation, int pos)
{
  if (orientation == wxHORIZONTAL)
  {
    if (colLabelCells && pos < totalCols)
      return colLabelCells[pos];
    else
      return NULL;
  }
  else
  {
    if (rowLabelCells && pos < totalRows)
      return rowLabelCells[pos];
    else
      return NULL;
  }
}

void wxGenericGrid::SetLabelValue(int orientation, const wxString& val, int pos)
{
  wxGridCell *cell = GetLabelCell(orientation, pos);
  if (cell)
    cell->SetTextValue(val);
}

wxString& wxGenericGrid::GetLabelValue(int orientation, int pos)
{
 static wxString emptyString = "";
  wxGridCell *cell = GetLabelCell(orientation, pos);
  if (cell)
    return cell->GetTextValue();
  else
    return emptyString;
}

void wxGenericGrid::SetLabelAlignment(int orientation, int align)
{
  if (orientation == wxHORIZONTAL)
    horizontalLabelAlignment = align;
  else
    verticalLabelAlignment = align;
  UpdateDimensions();
  SetCurrentRect(GetCursorRow(), GetCursorColumn());
}

int wxGenericGrid::GetLabelAlignment(int orientation)
{
  if (orientation == wxHORIZONTAL)
    return horizontalLabelAlignment;
  else
    return verticalLabelAlignment;
}

void wxGenericGrid::SetLabelTextColour(const wxColour& colour)
{
  labelTextColour = colour;

}

void wxGenericGrid::SetLabelBackgroundColour(const wxColour& colour)
{
  labelBackgroundColour = colour;
  labelBackgroundBrush = wxTheBrushList->FindOrCreateBrush(&labelBackgroundColour, wxSOLID);
}

void wxGenericGrid::SetEditable(Bool edit)
{
  editable = edit;
  if (edit)
  {
    int controlW, controlH;
    textItem->GetSize(&controlW, &controlH);
    editControlPosition.height = controlH;
  
    topOfSheet = editControlPosition.x + controlH + 2;
    if (textItem)
    {
      textItem->Show(TRUE);
      // Bug fix
#if defined(wx_msw) && (wxMINOR_VERSION == 6 && wxRELEASE_NUMBER < 6)
      ::ShowWindow(textItem->GetLabelHWND(), TRUE);
#endif
      textItem->SetFocus();
    }
  }
  else
  {
    topOfSheet = 0;
    if (textItem)
    {
      textItem->Show(FALSE);
      // Bug fix
#if defined(wx_msw) && (wxMINOR_VERSION == 6 && wxRELEASE_NUMBER < 6)
      ::ShowWindow(textItem->GetLabelHWND(), FALSE);
#endif
    }
  }
  UpdateDimensions();
  SetCurrentRect(GetCursorRow(), GetCursorColumn());

  int cw, ch;
  GetClientSize(&cw, &ch);
  OnSize(cw, ch);
/*
  int cw, ch;
  int scrollWidth = 16;
  GetClientSize(&cw, &ch);
  
  if (vScrollBar)
    vScrollBar->SetSize(cw - scrollWidth, topOfSheet,
       scrollWidth, ch - topOfSheet - scrollWidth);
*/
}

void wxGenericGrid::SetCellAlignment(int flag, int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
    cell->SetAlignment(flag);
}

int wxGenericGrid::GetCellAlignment(int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
    return cell->GetAlignment();
  else
    return cellAlignment;
}

void wxGenericGrid::SetCellAlignment(int flag)
{
  cellAlignment = flag;
  int i,j;
  for (i = 0; i < GetRows(); i++)
    for (j = 0; j < GetCols(); j++)
      if (GetCell(i, j))
        GetCell(i, j)->SetAlignment(flag);
}

int wxGenericGrid::GetCellAlignment(void)
{
  return cellAlignment;
}

void wxGenericGrid::SetCellBackgroundColour(const wxColour& col)
{
  cellBackgroundColour = col;
  int i,j;
  for (i = 0; i < GetRows(); i++)
    for (j = 0; j < GetCols(); j++)
      if (GetCell(i, j))
        GetCell(i, j)->SetBackgroundColour(col);
}

void wxGenericGrid::SetCellBackgroundColour(const wxColour& val, int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
  {
    cell->SetBackgroundColour(val);
    RefreshCell(row, col);
  }
}

wxColour& wxGenericGrid::GetCellBackgroundColour(int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
    return cell->GetBackgroundColour();
  else
    return cellBackgroundColour;
}

void wxGenericGrid::SetCellTextColour(const wxColour& val, int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
  {
    cell->SetTextColour(val);
    RefreshCell(row, col);
  }
}

void wxGenericGrid::SetCellTextFont(wxFont *fnt, int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
  {
    cell->SetFont(fnt);
    RefreshCell(row, col);
  }
}

wxFont *wxGenericGrid::GetCellTextFont(int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
    return cell->GetFont();
  else
    return cellTextFont;
}

wxColour& wxGenericGrid::GetCellTextColour(int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
    return cell->GetTextColour();
  else
    return cellTextColour;
}

void wxGenericGrid::SetCellTextColour(const wxColour& val)
{
  cellTextColour = val;
  int i,j;
  for (i = 0; i < GetRows(); i++)
    for (j = 0; j < GetCols(); j++)
      if (GetCell(i, j))
        GetCell(i, j)->SetTextColour(val);
}

void wxGenericGrid::SetCellTextFont(wxFont *fnt)
{
  cellTextFont = fnt;
  int i,j;
  for (i = 0; i < GetRows(); i++)
    for (j = 0; j < GetCols(); j++)
      if (GetCell(i, j))
        GetCell(i, j)->SetFont(fnt);
}

void wxGenericGrid::SetCellBitmap(wxBitmap *bitmap, int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
  {
    cell->SetCellBitmap(bitmap);
    RefreshCell(row, col);
  }
}

wxBitmap *wxGenericGrid::GetCellBitmap(int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
  {
    return cell->GetCellBitmap();
  }
  else
    return NULL;
}

Bool wxGenericGrid::InsertCols(int pos, int n, Bool updateLabels)
{
  if (pos > totalCols)
    return FALSE;
    
  if (!gridCells)
    return CreateGrid(1, n);
  else
  {
    int i, j;
    // Cells
    for (i = 0; i < totalRows; i++)
    {
      wxGridCell **cols = gridCells[i];
      wxGridCell **newCols = new wxGridCell *[totalCols + n];
      for (j = 0; j < pos; j++)
        newCols[j] = cols[j];
      for (j = pos; j < pos + n; j++)
        newCols[j] = new wxGridCell(this);
      for (j = pos + n; j < totalCols + n; j++)
        newCols[j] = cols[j - n];
        
      delete[] cols;
      gridCells[i] = newCols;
    }

    // Column widths
    short *newColWidths = new short[totalCols + n];
    for (j = 0; j < pos; j++)
      newColWidths[j] = colWidths[j];
    for (j = pos; j < pos + n; j++)
      newColWidths[j] = wxGRID_DEFAULT_CELL_WIDTH;
    for (j = pos + n; j < totalCols + n; j++)
      newColWidths[j] = colWidths[j - n];
    delete[] colWidths;
    colWidths = newColWidths;

    // Column labels
    wxGridCell **newLabels = new wxGridCell *[totalCols + n];
    for (j = 0; j < pos; j++)
      newLabels[j] = colLabelCells[j];
    for (j = pos; j < pos + n; j++)
      newLabels[j] = new wxGridCell(this);
    for (j = pos + n; j < totalCols + n; j++)
      newLabels[j] = colLabelCells[j - n];
        
    delete[] colLabelCells;
    colLabelCells = newLabels;
      
    totalCols += n;

    if (updateLabels)
      OnChangeLabels();
    UpdateDimensions();
    AdjustScrollbars();
    return TRUE;
  }
}

Bool wxGenericGrid::InsertRows(int pos, int n, Bool updateLabels)
{
  if (pos > totalRows)
    return FALSE;
    
  if (!gridCells)
    return CreateGrid(n, 1);
  else
  {
    int i, j;
    
    wxGridCell ***rows = new wxGridCell **[totalRows + n];

    // Cells
    for (i = 0; i < pos; i++)
      rows[i] = gridCells[i];

    for (i = pos; i < pos + n; i++)
    {
      rows[i] = new wxGridCell *[totalCols];
      for (j = 0; j < totalCols; j++)
        rows[i][j] = new wxGridCell(this);
    }
    
    for (i = pos + n; i < totalRows + n; i++)
      rows[i] = gridCells[i - n];
      
    delete[] gridCells;
    gridCells = rows;

    // Row heights
    short *newRowHeights = new short[totalRows + n];
    for (i = 0; i < pos; i++)
      newRowHeights[i] = rowHeights[i];
    for (i = pos; i < pos + n; i++)
      newRowHeights[i] = wxGRID_DEFAULT_CELL_HEIGHT;
    for (i = pos + n; i < totalRows + n; i++)
      newRowHeights[i] = rowHeights[i - n];
    delete[] rowHeights;
    rowHeights = newRowHeights;

    // Column labels
    wxGridCell **newLabels = new wxGridCell *[totalRows + n];
    for (i = 0; i < pos; i++)
      newLabels[i] = rowLabelCells[i];
    for (i = pos; i < pos + n; i++)
      newLabels[i] = new wxGridCell(this);
    for (i = pos + n; i < totalRows + n; i++)
      newLabels[i] = rowLabelCells[i - n];
        
    delete[] rowLabelCells;
    rowLabelCells = newLabels;
      
    totalRows += n;

    if (updateLabels)
      OnChangeLabels();
    UpdateDimensions();
    AdjustScrollbars();
    return TRUE;
  }
}

Bool wxGenericGrid::AppendCols(int n, Bool updateLabels)
{
  return InsertCols(GetCols(), n, updateLabels);
}

Bool wxGenericGrid::AppendRows(int n, Bool updateLabels)
{
  return InsertRows(GetRows(), n, updateLabels);
}

Bool wxGenericGrid::DeleteRows(int pos, int n, Bool updateLabels)
{
  if (pos > totalRows)
    return FALSE;
  if (!gridCells)
    return FALSE;

  int i;
    
  wxGridCell ***rows = new wxGridCell **[totalRows - n];

  // Cells
  for (i = 0; i < pos; i++)
    rows[i] = gridCells[i];

  for (i = pos + n; i < totalRows; i++)
    rows[i-n] = gridCells[i];
      
  delete[] gridCells;
  gridCells = rows;

  // Row heights
  short *newRowHeights = new short[totalRows - n];
  for (i = 0; i < pos; i++)
    newRowHeights[i] = rowHeights[i];
  for (i = pos + n; i < totalRows; i++)
    newRowHeights[i-n] = rowHeights[i];
  delete[] rowHeights;
  rowHeights = newRowHeights;

  // Column labels
  wxGridCell **newLabels = new wxGridCell *[totalRows - n];
  for (i = 0; i < pos; i++)
    newLabels[i] = rowLabelCells[i];
  for (i = pos + n; i < totalRows; i++)
    newLabels[i-n] = rowLabelCells[i];
        
  delete[] rowLabelCells;
  rowLabelCells = newLabels;
      
  totalRows -= n;

  if (updateLabels)
    OnChangeLabels();
  UpdateDimensions();
  AdjustScrollbars();
  return TRUE;
}

Bool wxGenericGrid::DeleteCols(int pos, int n, Bool updateLabels)
{
  if (pos + n > totalCols)
    return FALSE;
  if (!gridCells)
    return FALSE;

  int i, j;

  // Cells
  for (i = 0; i < totalRows; i++)
  {
      wxGridCell **cols = gridCells[i];
      wxGridCell **newCols = new wxGridCell *[totalCols - n];
      for (j = 0; j < pos; j++)
        newCols[j] = cols[j];
      for (j = pos; j < pos + n; j++)
        delete cols[j];
      for (j = pos + n; j < totalCols; j++)
        newCols[j-n] = cols[j];
        
      delete[] cols;
      gridCells[i] = newCols;
  }

  // Column widths
  short *newColWidths = new short[totalCols - n];
  for (j = 0; j < pos; j++)
    newColWidths[j] = colWidths[j];
  for (j = pos + n; j < totalCols; j++)
    newColWidths[j-n] = colWidths[j];
  delete[] colWidths;
  colWidths = newColWidths;

  // Column labels
  wxGridCell **newLabels = new wxGridCell *[totalCols - n];
  for (j = 0; j < pos; j++)
    newLabels[j] = colLabelCells[j];
  for (j = pos + n; j < totalCols; j++)
    newLabels[j-n] = colLabelCells[j];
        
  delete[] colLabelCells;
  colLabelCells = newLabels;
      
  totalCols -= n;

  if (updateLabels)
    OnChangeLabels();
  UpdateDimensions();
  AdjustScrollbars();
  return TRUE;
}

void wxGenericGrid::SetGridCursor(int row, int col)
{
  if (row >= totalRows || col >= totalCols)
    return;
    
  if (row == GetCursorRow() && col == GetCursorColumn())
    return;
    
  wxDC *dc = GetDC();
  dc->BeginDrawing();
  
  SetGridClippingRegion(dc);

  if (currentRectVisible)
    HighlightCell(dc);
  
  wCursorRow = row;
  wCursorColumn = col;
  SetCurrentRect(row, col);
  if (currentRectVisible)
    HighlightCell(dc);

  dc->DestroyClippingRegion();
  dc->EndDrawing();
}

/*
 * Grid cell
 */
 
wxGridCell::wxGridCell(wxGenericGrid *window)
{
  cellBitmap = NULL;
  font = NULL;
  backgroundBrush = NULL;
  if (window)
    textColour = window->GetCellTextColour();
  else
    textColour.Set(0,0,0);
  if (window)
    backgroundColour = window->GetCellBackgroundColour();
  else
    backgroundColour.Set(255,255,255);
    
  if (window)
    font = window->GetCellTextFont();
  else
    font = wxTheFontList->FindOrCreateFont(12, wxSWISS, wxNORMAL, wxNORMAL);
    
  SetBackgroundColour(backgroundColour);
  
  if (window)
    alignment = window->GetCellAlignment();
  else
    alignment = wxLEFT;
}

wxGridCell::~wxGridCell(void)
{
}

void wxGridCell::SetBackgroundColour(const wxColour& colour)
{
  backgroundColour = colour;
  backgroundBrush = wxTheBrushList->FindOrCreateBrush(&backgroundColour, wxSOLID);
}

static void wxGridFunc(wxText& txt, wxCommandEvent& ev)
{
  wxGenericGrid *grid = (wxGenericGrid *)txt.GetParent();
  wxGridCell *cell = grid->GetCell(grid->GetCursorRow(), grid->GetCursorColumn());
  if (cell && grid->CurrentCellVisible())
  {
    cell->SetTextValue(grid->GetTextItem()->GetValue());
    wxDC *dc = grid->GetDC();
    dc->BeginDrawing();
    grid->SetGridClippingRegion(dc);
    grid->DrawCellBackground(dc, &grid->GetCurrentRect(), grid->GetCursorRow(), grid->GetCursorColumn());
    grid->DrawCellValue(dc, &grid->GetCurrentRect(), grid->GetCursorRow(), grid->GetCursorColumn());
    grid->HighlightCell(dc);
    dc->DestroyClippingRegion();
    dc->EndDrawing();
      
    grid->OnCellChange(grid->GetCursorRow(), grid->GetCursorColumn());
      
//    grid->DrawCellText();
  }
}

static void wxGridScrollFunc(wxScrollBar& sb, wxCommandEvent& ev)
{
  static Bool inScroll = FALSE;

  if ( inScroll )
	return;

  inScroll = TRUE;
  wxGenericGrid *win = (wxGenericGrid *)sb.GetParent();
  
  if (ev.eventObject == win->GetHorizScrollBar())
    win->SetScrollPosX(ev.commandInt);
  else win->SetScrollPosY(ev.commandInt);

  win->UpdateDimensions();
  win->SetCurrentRect(win->GetCursorRow(), win->GetCursorColumn());

  // Because rows and columns can be arbitrary sizes,
  // the scrollbars will need to be adjusted to reflect the
  // current view.
  win->AdjustScrollbars();

  // Definitely paint now
  paintTime = 0;
  win->Refresh(FALSE);

  inScroll = FALSE;
}
