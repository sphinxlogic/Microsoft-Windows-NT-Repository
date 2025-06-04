/*
 * File:        wxgridm.cc
 * Purpose:     wxGrid implementation, Motif
 * Author:      Julian Smart
 * Created:     1995
 * Updated:	August 1995
 * Copyright:   (c) 1993, AIAI, University of Edinburgh
 */

static const char sccsid[] = "@(#)wxgridm.cc	1.2 5/9/94";

#ifdef __GNUG__
#pragma implementation
#endif

#if !defined(USE_IOSTREAMH) || USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif
#include <fstream.h>
#include <stdio.h>
#include <ctype.h>

#include "wx_defs.h"
#include "wx_main.h"
#include "wx_panel.h"

#include "wxgridm.h"
#include "matrix.h"

#include "wx_utils.h"
#include "wx_frame.h"
#include "wx_privt.h"

#include <sys/types.h>
#include <sys/stat.h>
/*
static void 
wxTextWindowChangedProc (Widget w, XtPointer clientData, XtPointer ptr);
static void 
wxTextWindowModifyProc (Widget w, XtPointer clientData, XmTextVerifyCallbackStruct *cbs);
static void 
wxTextWindowGainFocusProc (Widget w, XtPointer clientData, XmAnyCallbackStruct *cbs);
static void 
wxTextWindowLoseFocusProc (Widget w, XtPointer clientData, XmAnyCallbackStruct *cbs);
*/

IMPLEMENT_DYNAMIC_CLASS(wxMotifGrid, wxPanel)

wxMotifGrid::wxMotifGrid (void)
{
#ifdef wx_motif
  totalRows = 0;
  totalCols = 0;
#endif
}

wxMotifGrid::wxMotifGrid (wxWindow * parent, int x, int y, int width, int height,
	      long style, char *name)
{
  Create (parent, x, y, width, height, style, name);
}

Bool wxMotifGrid::Create (wxWindow * parent, int x, int y, int width, int height,
	long style, char *name)
{
  windowStyle = style;
  SetName(name);

  totalRows = 0;
  totalCols = 0;

  rowLabelWidth = wxGRID_DEFAULT_VERTICAL_LABEL_WIDTH;
  colLabelHeight = wxGRID_DEFAULT_HORIZONTAL_LABEL_HEIGHT;
  
  cellTextColour = *wxBLACK;
  cellBackgroundColour = *wxWHITE;
  labelTextColour = *wxBLACK;
  labelBackgroundColour = *wxLIGHT_GREY;
  labelTextFont = wxTheFontList->FindOrCreateFont(12, wxSWISS, wxNORMAL, wxBOLD);
  cellTextFont = wxTheFontList->FindOrCreateFont(12, wxSWISS, wxNORMAL, wxNORMAL);
  divisionPen = wxThePenList->FindOrCreatePen("LIGHT GREY", 1, wxSOLID);


//  tempCallbackStruct = NULL;

  Widget parentWidget = 0;
  if (parent->IsKindOf(CLASSINFO(wxFrame)))
    parentWidget = ((wxFrame *)parent)->clientArea;
  else if (parent->IsKindOf(CLASSINFO(wxPanel)))
    parentWidget = (Widget)parent->handle;
  else
  {
    wxError("Table subwindow must be a child of either a frame or panel!");
    return FALSE;
  }

  // This will be the genuine call.
//  Widget tableWidget = XtVaCreateManagedWidget (windowName, xbaeMatrixWidgetClass, parentWidget,
//     NULL);

  short widths[1];
  widths[0] = 40;

  String **cells = new String *[1];
  cells[0] = new String[1];
  cells[0][0] = "";

  Widget tableWidget = XtVaCreateManagedWidget (windowName, xbaeMatrixWidgetClass, parentWidget,
                                 XmNrows,               1,
                                 XmNcolumns,            1,
                                 XmNcolumnWidths,       widths,
                                 XmNcells,              cells,
				 XmNcellShadowThickness, 1,
				 XmNcellMarginWidth,     1,
				 XmNcellMarginHeight,    1,
     NULL);
  
  delete[] cells[0];
  delete[] cells;

  wxAddWindowToTable(tableWidget, this);

  XtTranslations ptr;
  XtOverrideTranslations (tableWidget,
		   ptr = XtParseTranslationTable ("<Configure>: resize()"));
  XtFree ((char *) ptr);

/*
  XtAddCallback(textWidget, XmNvalueChangedCallback, (XtCallbackProc)wxMotifGridChangedProc, (XtPointer)this);

  XtAddCallback(textWidget, XmNmodifyVerifyCallback, (XtCallbackProc)wxMotifGridModifyProc, (XtPointer)this);

//  XtAddCallback(textWidget, XmNactivateCallback, (XtCallbackProc)wxMotifGridModifyProc, (XtPointer)this);

  XtAddCallback(textWidget, XmNfocusCallback, (XtCallbackProc)wxMotifGridGainFocusProc, (XtPointer)this);

  XtAddCallback(textWidget, XmNlosingFocusCallback, (XtCallbackProc)wxMotifGridLoseFocusProc, (XtPointer)this);
*/

  XtManageChild (tableWidget);
  handle = (char *) tableWidget;

  if (parent->IsKindOf(CLASSINFO(wxPanel)))
    ((wxPanel *)parent)->AttachWidget(this, 0, x, y, width, height);
  else
    SetSize (x, y, width, height);

  if (parent)
    parent->AddChild (this);
  window_parent = parent;

  SetCellTextColour(cellTextColour);
  SetCellBackgroundColour(cellBackgroundColour);

  return TRUE;
}

wxMotifGrid::~wxMotifGrid (void)
{
}

void wxMotifGrid::SetSize (int x, int y, int w, int h, int sizeFlags)
{
  Widget tableWidget = (Widget) handle;
  XtUnmanageChild (tableWidget);

  if (x > -1)
    XtVaSetValues (tableWidget,
//                  XmNleftAttachment, XmATTACH_SELF,
		   XmNx, x,
		   NULL);

  if (y > -1)
    XtVaSetValues (tableWidget,
//                  XmNtopAttachment, XmATTACH_SELF,
		   XmNy, y,
		   NULL);

  if (w > -1)
    XtVaSetValues (tableWidget, XmNwidth, w, NULL);
  if (h > -1)
    XtVaSetValues (tableWidget, XmNheight, h, NULL);

  XtManageChild (tableWidget);

  GetEventHandler()->OnSize (w, h);
}

void wxMotifGrid::SetClientSize (int w, int h)
{
  SetSize (-1, -1, w, h);
}


void wxMotifGrid::ClearGrid (void)
{
}

Bool wxMotifGrid::CreateGrid(int rows, int cols, wxString **cellValues, short *widthValues, short defaultWidth, short defaultHeight)
{
  if (rows == 0 || cols == 0)
  {
    wxFatalError("wxMotifGrid rows and columns must be greater than zero!");
    return FALSE;
  }

  totalRows = rows;
  totalCols = cols;

  short *widths = new short[cols];
  String **Cells = new String *[rows];

  int i,j;
  for (i = 0; i < rows; i++)
  {
    Cells[i] = new String[cols];
    for (j = 0; j < cols; j++)
    {
      if (cellValues)
        Cells[i][j] = cellValues[i][j].GetData();
      else
       Cells[i][j] = "";
    }
  }

  for (i = 0; i < cols; i++)
  {
    if (widthValues)
      widths[i] = (short)(widthValues[i]/5); // CONVERT PIXELS TO CHARS: TEMP FIX
    else
      widths[i] = (short)(defaultWidth/5); // DITTO
  }

  Widget tableWidget = (Widget)handle;
  XtVaSetValues(tableWidget,
                                 XmNrows,               rows,
                                 XmNcolumns,            cols,
                                 XmNcolumnWidths,       widths,
                                 XmNcells,              Cells,
                                 NULL);
  
  for (i = 0; i < rows; i++)
  {
    delete[] Cells[i];
  }
  delete[] widths;

  SetLabelSize(wxHORIZONTAL, colLabelHeight);
  SetLabelSize(wxVERTICAL, rowLabelWidth);

  OnChangeLabels();

  return TRUE;
}

// Not clear whether Clear is required as well as DiscardEdits
void wxMotifGrid::DiscardEdits (void)
{
}

void wxMotifGrid::SetEditable (Bool editable)
{
}

Bool wxMotifGrid::InsertCols(int pos, int n, Bool updateLabels)
{
  return FALSE;
}

Bool wxMotifGrid::InsertRows(int pos, int n, Bool updateLabels)
{
  return FALSE;
}

Bool wxMotifGrid::AppendCols(int n, Bool updateLabels)
{
  return FALSE;
}

Bool wxMotifGrid::AppendRows(int n, Bool updateLabels)
{
  return FALSE;
}

Bool wxMotifGrid::DeleteCols(int pos, int n, Bool updateLabels)
{
  return FALSE;
}

Bool wxMotifGrid::DeleteRows(int pos, int n, Bool updateLabels)
{
  return FALSE;
}
  
void wxMotifGrid::SetCellValue(const wxString& val, int row, int col)
{
  XbaeMatrixSetCell((Widget)handle, row, col, ((wxString&)val).GetData());
}

wxString& wxMotifGrid::GetCellValue(int row, int col)
{
  static wxString value = "";
  char *s = XbaeMatrixGetCell((Widget)handle, row, col);
  if (s)
    value = s;
  return value;
}

void wxMotifGrid::SetCellAlignment(int flag, int row, int col)
{
}

void wxMotifGrid::SetCellAlignment(int flag)
{
}

int wxMotifGrid::GetCellAlignment(int row, int col)
{
  return 0;
}

int wxMotifGrid::GetCellAlignment(void)
{
  return 0;
}

void wxMotifGrid::SetCellTextColour(const wxColour& val, int row, int col)
{
  Pixel pixel;
  XColor exact_def;
  exact_def.red = (unsigned short) (((long) ((wxColour&)val).Red ()) << 8);
  exact_def.green = (unsigned short) (((long) ((wxColour&)val).Green ()) << 8);
  exact_def.blue = (unsigned short) (((long) ((wxColour&)val).Blue ()) << 8);
  exact_def.flags = DoRed | DoGreen | DoBlue;

  Colormap cmap = wxGetMainColormap(wxGetDisplay());
  if (!XAllocColor(wxGetDisplay(), cmap, &exact_def))
    pixel = BlackPixel(wxGetDisplay, DefaultScreen(wxGetDisplay()));
  else
    pixel = exact_def.pixel;

  XbaeMatrixSetCellColor((Widget)handle, row, col, pixel);
}

void wxMotifGrid::SetCellTextColour(const wxColour& col)
{
  Pixel pixel;
  XColor exact_def;
  exact_def.red = (unsigned short) (((long) ((wxColour&)col).Red ()) << 8);
  exact_def.green = (unsigned short) (((long) ((wxColour&)col).Green ()) << 8);
  exact_def.blue = (unsigned short) (((long) ((wxColour&)col).Blue ()) << 8);
  exact_def.flags = DoRed | DoGreen | DoBlue;

  Colormap cmap = wxGetMainColormap(wxGetDisplay());
  if (!XAllocColor(wxGetDisplay(), cmap, &exact_def))
    pixel = BlackPixel(wxGetDisplay, DefaultScreen(wxGetDisplay()));
  else
    pixel = exact_def.pixel;

  XtVaSetValues ((Widget)handle,
		   XmNforeground, pixel,
		   NULL);

  cellTextColour = col;
}

wxColour& wxMotifGrid::GetCellTextColour(int row, int col)
{
  return cellTextColour;
}

void wxMotifGrid::SetCellBackgroundColour(const wxColour& col)
{
  Pixel pixel;
  XColor exact_def;
  exact_def.red = (unsigned short) (((long) ((wxColour&)col).Red ()) << 8);
  exact_def.green = (unsigned short) (((long) ((wxColour&)col).Green ()) << 8);
  exact_def.blue = (unsigned short) (((long) ((wxColour&)col).Blue ()) << 8);
  exact_def.flags = DoRed | DoGreen | DoBlue;

  Colormap cmap = wxGetMainColormap(wxGetDisplay());
  if (!XAllocColor(wxGetDisplay(), cmap, &exact_def))
    pixel = BlackPixel(wxGetDisplay, DefaultScreen(wxGetDisplay()));
  else
    pixel = exact_def.pixel;

  XtVaSetValues ((Widget)handle,
		   XmNbackground, pixel,
		   NULL);
}

void wxMotifGrid::SetCellBackgroundColour(const wxColour& colour, int row, int col)
{
  // Can we set a background colour for an individual cell?
}

wxColour& wxMotifGrid::GetCellBackgroundColour(int row, int col)
{
  return cellBackgroundColour;
}

wxFont *wxMotifGrid::GetCellTextFont(int row, int col)
{
  return cellTextFont;
}

void wxMotifGrid::SetCellTextFont(wxFont *font)
{
}

void wxMotifGrid::SetCellTextFont(wxFont *font, int row, int col)
{
}
  
// Size accessors
void wxMotifGrid::SetColumnWidth(int col, int width)
{
  short *widths = new short[totalCols];
/*
  XtVaSetValues ((Widget)handle,
		   XmNbackground, pixel,
		   NULL);
*/

}

int wxMotifGrid::GetColumnWidth(int col)
{
  return 0;
}

void wxMotifGrid::SetRowHeight(int row, int height)
{
}

int wxMotifGrid::GetRowHeight(int row)
{
  return 0;
}
  
// Label accessors
void wxMotifGrid::SetLabelSize(int orientation, int sz)
{
  if (orientation == wxHORIZONTAL)
  {
    colLabelHeight = sz;
    String *strings = NULL;
    if (sz > 0)
    {
      strings = new String[totalCols];
      int i;
      for (i = 0; i < totalCols; i++)
        strings[i] = "";
    }
    XtVaSetValues ((Widget)handle,
//		   XmNcolumnLabelWidth, (int)sz/5,
                   XmNcolumnLabels, strings,
		   NULL);

    if (strings)
      delete[] strings;
  }
  else if (orientation == wxVERTICAL)
  {
    rowLabelWidth = sz;
    String *strings = NULL;
    if (sz > 0)
    {
      strings = new String[totalRows];
      int i;
      for (i = 0; i < totalRows; i++)
        strings[i] = "";
    }
    XtVaSetValues ((Widget)handle,
		   XmNrowLabelWidth, (int)sz/5,
                   XmNrowLabels, strings,
		   NULL);

    if (strings)
      delete[] strings;
  }
}

int wxMotifGrid::GetLabelSize(int orientation)
{
  return 0;
}

void wxMotifGrid::SetLabelAlignment(int orientation, int alignment)
{
}

int wxMotifGrid::GetLabelAlignment(int orientation)
{
  return 0;
}

void wxMotifGrid::SetLabelValue(int orientation, const wxString& val, int pos)
{
  // Must get/set whole of XmNcolumnLabels/XmNrowLabels
}

wxString& wxMotifGrid::GetLabelValue(int orientation, int pos)
{
  static wxString nullValue = "";
  return nullValue;
}

void wxMotifGrid::SetLabelTextColour(const wxColour& colour)
{
}

void wxMotifGrid::SetLabelBackgroundColour(const wxColour& colour)
{
}

// Miscellaneous accessors
int wxMotifGrid::GetCursorRow(void)
{
  return 0;
}

int wxMotifGrid::GetCursorColumn(void)
{
  return 0;
}

void wxMotifGrid::SetGridCursor(int row, int col)
{
}

int wxMotifGrid::GetScrollPosX(void)
{
  return 0;
}

int wxMotifGrid::GetScrollPosY(void)
{
  return 0;
}

// High-level event handling
// Override e.g. to check value of current cell; but call
// base member for default processing.
void wxMotifGrid::OnSelectCell(int row, int col)
{
}

// Override to change labels e.g. creation of grid, inserting/deleting a row/col.
// By default, auto-labels the grid.
void wxMotifGrid::OnChangeLabels(void)
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

// Override to change the label of the edit field when selecting a cell
// By default, sets it to e.g. A12
void wxMotifGrid::OnChangeSelectionLabel(void)
{
}

// Activation: call from wxFrame::OnActivate
void wxMotifGrid::OnActivate(Bool active)
{
}

// Miscellaneous
void wxMotifGrid::AdjustScrollbars(void)
{
}

void wxMotifGrid::UpdateDimensions(void)
{
}



#if 0

static void 
wxMotifGridChangedProc (Widget w, XtPointer clientData, XtPointer ptr)
{
  if (!wxWidgetHashTable->Get ((long) w))
    // Widget has been deleted!
    return;

  wxMotifGrid *tw = (wxMotifGrid *) clientData;
  tw->SetModified(TRUE);
}

static void 
wxMotifGridModifyProc (Widget w, XtPointer clientData, XmTextVerifyCallbackStruct *cbs)
{
//  if (!wxWidgetHashTable->Get ((long) w))
//    return;

  wxMotifGrid *tw = (wxMotifGrid *) clientData;

  // If we're already within an OnChar, return: probably
  // a programmatic insertion.
  if (tw->tempCallbackStruct)
    return;

  // Check for a backspace
  if (cbs->startPos == (cbs->currInsert - 1))
  {
    tw->tempCallbackStruct = (XtPointer)cbs;

    wxKeyEvent event (wxEVENT_TYPE_CHAR);
    event.keyCode = WXK_DELETE;
    event.eventObject = tw;

    // Only if wxMotifGrid::OnChar is called
    // will this be set to True (and the character
    // passed through)
    cbs->doit = False;

    tw->GetEventHandler()->OnChar(event);

    tw->tempCallbackStruct = NULL;

    return;
  }

  // Pasting operation: let it through without
  // calling OnChar
  if (cbs->text->length > 1)
    return;

  // Something other than text
  if (cbs->text->ptr == NULL)
    return;

  tw->tempCallbackStruct = (XtPointer)cbs;

  wxKeyEvent event (wxEVENT_TYPE_CHAR);

  event.keyCode = (cbs->text->ptr[0] == 10 ? 13 : cbs->text->ptr[0]);
  event.eventObject = tw;

  // Only if wxMotifGrid::OnChar is called
  // will this be set to True (and the character
  // passed through)
  cbs->doit = False;

  tw->GetEventHandler()->OnChar(event);

  tw->tempCallbackStruct = NULL;
}

static void 
wxMotifGridGainFocusProc (Widget w, XtPointer clientData, XmAnyCallbackStruct *cbs)
{
  if (!wxWidgetHashTable->Get ((long) w))
    return;

  wxMotifGrid *tw = (wxMotifGrid *) clientData;
  tw->GetEventHandler()->OnSetFocus();
}

static void 
wxMotifGridLoseFocusProc (Widget w, XtPointer clientData, XmAnyCallbackStruct *cbs)
{
  if (!wxWidgetHashTable->Get ((long) w))
    return;

  wxMotifGrid *tw = (wxMotifGrid *) clientData;
  tw->GetEventHandler()->OnKillFocus();
}

void wxMotifGrid::OnChar(wxKeyEvent& event)
{
  if (tempCallbackStruct)
  {
    XmTextVerifyCallbackStruct *textStruct =
        (XmTextVerifyCallbackStruct *)tempCallbackStruct;
    textStruct->doit = True;
    if (isascii(event.keyCode) && (textStruct->text->length == 1))
    {
      textStruct->text->ptr[0] = ((event.keyCode == WXK_RETURN) ? 10 : event.keyCode);
    }
  }
}

#endif
