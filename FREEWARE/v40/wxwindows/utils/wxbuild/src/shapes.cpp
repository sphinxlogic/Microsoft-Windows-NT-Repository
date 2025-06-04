/*
 * File:	shapes.cc
 * Purpose:	
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

static const char sccsid[] = "%W% %G%";
 
// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx.h>
#endif

#if !defined(USE_IOSTREAMH) || USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include "shapes.h"

#define HANDLE_SIZE       6
#define HANDLE_VERTICAL   1
#define HANDLE_HORIZONTAL 2
#define HANDLE_DIAGONAL   3
#define DEFAULT_MOUSE_TOLERANCE 3
wxFont *normal_font;
wxPen *black_pen;
wxBrush *white_brush;
wxBrush *black_brush;
wxPen *white_background_pen;
wxBrush *transparent_brush;
wxBrush *white_background_brush;
wxPen *black_foreground_pen;
wxPen *black_dashed_pen;
#define NoDragging             0
#define StartDraggingLeft      1
#define ContinueDraggingLeft   2
#define StartDraggingRight     3

Shape::Shape(ShapeCanvas *can) {
  textString = NULL; formatted = FALSE; dc = NULL;
  canvas = can;  xpos = 0.0; ypos = 0.0; pen = black_pen;
  brush = white_brush; font = normal_font; text_colour = wxBLACK;
  visible = FALSE; ClientData = NULL; selected = FALSE; draggable = TRUE;
  textMarginX = 2.0; textMarginY = 2.0;
}

Shape::~Shape(void) {
  if (textString) delete[] textString;
  if (canvas) canvas->RemoveObject(this);
}

void Shape::SetClientData(wxObject *client_data){ ClientData = client_data; }
wxObject *Shape::GetClientData(void) { return ClientData; }
void Shape::SetDC(wxDC *the_dc) { dc = the_dc; }
void Shape::SetDraggable(Bool drag) { draggable = drag; }
void Shape::SetCanvas(ShapeCanvas *the_canvas) { canvas = the_canvas; }
void Shape::AddToCanvas(ShapeCanvas *the_canvas) { the_canvas->AddObject(this); }

Bool Shape::HitTest(float x, float y, float *distance)
{
  float width = 0.0, height = 0.0; GetBoundingBoxMin(&width, &height);
  if (fabs(width) < 4.0) width = 4.0;
  if (fabs(height) < 4.0) height = 4.0;
  width += (float)4.0; height += (float)4.0; // Allowance for inaccurate mousing
  float left = (float)(xpos - (width/2.0)); float top = (float)(ypos - (height/2.0));
  float right = (float)(xpos + (width/2.0)); float bottom = (float)(ypos + (height/2.0));
  if (x >= left && x <= right && y >= top && y <= bottom) {
    float nearest = 999999.0;
    float l = (float)sqrt(((GetX() - x) * (GetX() - x)) +
                 ((GetY() - y) * (GetY() - y)));
    *distance = l; return TRUE;
  }
  else return FALSE;
}

void Shape::FormatText(char *s) {
  if (textString) delete[] textString;  textString = copystring(s);
  formatted = TRUE;
}
void Shape::SetPen(wxPen *the_pen) { pen = the_pen; }
void Shape::SetBrush(wxBrush *the_brush) { brush = the_brush; }
void Shape::SetFont(wxFont *the_font) { font = the_font; }
void Shape::OnDraw(void) {}
void Shape::OnDrawContents(void)
{
  float width, height;
  GetBoundingBoxMin(&width, &height);
  if (dc) {
    if (font) dc->SetFont(font);
    if (pen) dc->SetPen(pen);
    if (text_colour) dc->SetTextForeground(text_colour);
    dc->SetBackgroundMode(wxTRANSPARENT);
    if (formatted && textString)
    {
      dc->SetClippingRegion((float)(xpos - width/2.0), (float)(ypos - height/2.0),
                    (float)width, (float)height);
      dc->DrawText(textString, (float)(textMarginX + xpos - (width/2.0)),
                               (float)(textMarginY + ypos - (height/2.0)));
      dc->DestroyClippingRegion();
    }
  }
}

void Shape::DrawContents(void){ OnDrawContents(); }
void Shape::OnSize(float x, float y) {}
void Shape::OnMove(float x, float y, float old_x, float old_y, Bool display){}
void Shape::OnErase(void){
  if (!visible) return;
  OnEraseContents();
}
void Shape::OnEraseContents(void) {
  if (!visible) return;
  float maxX, maxY, minX, minY;
  float xp = GetX(); float yp = GetY();
  GetBoundingBoxMin(&minX, &minY); GetBoundingBoxMax(&maxX, &maxY);
  float topLeftX = (float)(xp - (maxX / 2.0)); float topLeftY = (float)(yp - (maxY / 2.0));
  if (dc) {
    int penWidth = 0;
    if (pen)
      penWidth = pen->GetWidth();
    dc->SetPen(white_background_pen); dc->SetBrush(white_background_brush);
    dc->DrawRectangle((float)(topLeftX - penWidth), (float)(topLeftY - penWidth), 
                      (float)(maxX + penWidth*2.0), (float)(maxY + penWidth*2.0));
  }
}

void Shape::OnHighlight(void) {}
void Shape::OnLeftClick(float x, float y, int keys) {}
void Shape::OnRightClick(float x, float y, int keys) {}
float DragOffsetX = 0.0;
float DragOffsetY = 0.0;
void Shape::OnDragLeft(Bool draw, float x, float y, int keys)
{
  float xx, yy; xx = x + DragOffsetX; yy = y + DragOffsetY;
  canvas->Snap(&xx, &yy); xpos = xx; ypos = yy; OnDrawOutline();
}

void Shape::OnBeginDragLeft(float x, float y, int keys)
{
  DragOffsetX = xpos - x; DragOffsetY = ypos - y;
  Erase();
  float xx = x + DragOffsetX; float yy = y + DragOffsetY;
  canvas->Snap(&xx, &yy); xpos = xx; ypos = yy;
  dc->SetLogicalFunction(wxXOR);
  dc->SetPen(black_dashed_pen); dc->SetBrush(transparent_brush);
  OnDrawOutline();
}

void Shape::OnEndDragLeft(float x, float y, int keys){
  dc->SetLogicalFunction(wxCOPY); canvas->Snap(&xpos, &ypos); Move(xpos, ypos);
  if (canvas && !canvas->quick_edit_mode) canvas->Redraw();
}
void Shape::OnDrawOutline(void)
{
  float maxX, maxY, minX, minY; GetBoundingBoxMax(&maxX, &maxY); GetBoundingBoxMin(&minX, &minY);
  float top_left_x = (float)(xpos - minX/2.0);
  float top_left_y = (float)(ypos - minY/2.0);
  float top_right_x = (float)(top_left_x + maxX);
  float top_right_y = (float)top_left_y;
  float bottom_left_x = (float)top_left_x;
  float bottom_left_y = (float)(top_left_y + maxY);
  float bottom_right_x = (float)top_right_x;
  float bottom_right_y = (float)bottom_left_y;
  wxPoint points[5];
  points[0].x = top_left_x; points[0].y = top_left_y;
  points[1].x = top_right_x; points[1].y = top_right_y;
  points[2].x = bottom_right_x; points[2].y = bottom_right_y;
  points[3].x = bottom_left_x; points[3].y = bottom_left_y;
  points[4].x = top_left_x; points[4].y = top_left_y;
  dc->DrawLines(5, points);
}

void Shape::Attach(ShapeCanvas *can){ canvas = can; }
void Shape::Detach(void){ canvas = NULL; }
void Shape::Move(float x, float y, Bool display){
  float old_x = xpos; float old_y = ypos; xpos = x; ypos = y;
  OnMove(x, y, old_x, old_y, display); ResetCornerHandles();
  if (display) Draw();
  if (display) OnDrawCornerHandles();
}
void Shape::Draw(void){
  if (visible)
  { OnDraw(); OnDrawContents(); }
}

void Shape::Erase(void){
  OnErase(); OnEraseCornerHandles();
}

void Shape::EraseContents(void){
  OnEraseContents();
}

void Shape::SetSize(float x, float y, Bool recursive) {}
void Shape::MakeCornerHandles(void)
{
  float maxX, maxY, minX, minY;
  GetBoundingBoxMax(&maxX, &maxY);
  GetBoundingBoxMin(&minX, &minY);
  float widthMin = (float)(minX + HANDLE_SIZE + 2);
  float heightMin = (float)(minY + HANDLE_SIZE + 2);
  float top = (float)(- (heightMin / 2.0));
  float bottom = (float)(heightMin / 2.0 + (maxY - minY));
  float left = (float)(- (widthMin / 2.0));
  float right = (float)(widthMin / 2.0 + (maxX - minX));
  CornerHandle *control = new CornerHandle(canvas, this, HANDLE_SIZE, left, top, 
                                           HANDLE_DIAGONAL);
  canvas->AddObject(control); handles.Append(control);
  control = new CornerHandle(canvas, this, HANDLE_SIZE, 0, top, 
                                           HANDLE_VERTICAL);
  canvas->AddObject(control); handles.Append(control);
  control = new CornerHandle(canvas, this, HANDLE_SIZE, right, top, 
                                           HANDLE_DIAGONAL);
  canvas->AddObject(control); handles.Append(control);
  control = new CornerHandle(canvas, this, HANDLE_SIZE, right, 0, 
                                           HANDLE_HORIZONTAL);
  canvas->AddObject(control); handles.Append(control);
  control = new CornerHandle(canvas, this, HANDLE_SIZE, right, bottom, 
                                           HANDLE_DIAGONAL);
  canvas->AddObject(control); handles.Append(control);
  control = new CornerHandle(canvas, this, HANDLE_SIZE, 0, bottom, 
                                           HANDLE_VERTICAL);
  canvas->AddObject(control); handles.Append(control);
  control = new CornerHandle(canvas, this, HANDLE_SIZE, left, bottom, 
                                           HANDLE_DIAGONAL);
  canvas->AddObject(control); handles.Append(control);
  control = new CornerHandle(canvas, this, HANDLE_SIZE, left, 0, 
                                           HANDLE_HORIZONTAL);
  canvas->AddObject(control); handles.Append(control);
}

void Shape::ResetCornerHandles(void)
{
  if (handles.Number() < 1) return;
  float maxX, maxY, minX, minY;
  GetBoundingBoxMax(&maxX, &maxY);  GetBoundingBoxMin(&minX, &minY);
  float widthMin = (float)(minX + HANDLE_SIZE + 2);
  float heightMin = (float)(minY + HANDLE_SIZE + 2);
  float top = (float)(- (heightMin / 2.0));
  float bottom = (float)(heightMin / 2.0 + (maxY - minY));
  float left = (float)(- (widthMin / 2.0));
  float right = (float)(widthMin / 2.0 + (maxX - minX));
  wxNode *node = handles.First();
  CornerHandle *control = (CornerHandle *)node->Data();
  control->xoffset = left; control->yoffset = top;
  node = node->Next(); control = (CornerHandle *)node->Data();
  control->xoffset = 0; control->yoffset = top;
  node = node->Next(); control = (CornerHandle *)node->Data();
  control->xoffset = right; control->yoffset = top;
  node = node->Next(); control = (CornerHandle *)node->Data();
  control->xoffset = right; control->yoffset = 0;
  node = node->Next(); control = (CornerHandle *)node->Data();
  control->xoffset = right; control->yoffset = bottom;
  node = node->Next(); control = (CornerHandle *)node->Data();
  control->xoffset = 0; control->yoffset = bottom;
  node = node->Next(); control = (CornerHandle *)node->Data();
  control->xoffset = left; control->yoffset = bottom;
  node = node->Next(); control = (CornerHandle *)node->Data();
  control->xoffset = left; control->yoffset = 0;
}

void Shape::DeleteCornerHandles(void){
  wxNode *node = handles.First();
  while (node)
  {
    CornerHandle *control = (CornerHandle *)node->Data();
    control->OnErase(); canvas->RemoveObject(control);
    delete control; delete node;
    node = handles.First();
  }
}
void Shape::OnDrawCornerHandles(void){
  wxNode *node = handles.First();
  while (node)
  {
    CornerHandle *control = (CornerHandle *)node->Data();
    control->Draw(); node = node->Next();
  }
}
void Shape::OnEraseCornerHandles(void){
  wxNode *node = handles.First();
  while (node)
  {
    CornerHandle *control = (CornerHandle *)node->Data();
    control->Erase();  node = node->Next();
  }
}
void Shape::Select(Bool select){
  selected = select;
  if (select && (handles.Number() == 0))
  {
    MakeCornerHandles(); OnDrawCornerHandles();
  }
  if (!select && (handles.Number() > 0))
    DeleteCornerHandles();
}

void Shape::GetBoundingBoxMax(float *w, float *h){
  float ww, hh; GetBoundingBoxMin(&ww, &hh); *w = ww; *h = hh;
}

RectangleShape::RectangleShape(float w, float h){
  width = w; height = h;
}

void RectangleShape::OnDraw(void){
  if (dc)
  {
    float x1 = (float)(xpos - width/2.0); float y1 = (float)(ypos - height/2.0);
    if (pen) dc->SetPen(pen);
    if (brush)dc->SetBrush(brush);
    dc->DrawRectangle(x1, y1, width, height);
  }
}

void RectangleShape::GetBoundingBoxMin(float *the_width, float *the_height){
  *the_width = width; *the_height = height;
}

void RectangleShape::SetSize(float x, float y, Bool recursive){
  width = x; height = y;
}

// Control points
CornerHandle::CornerHandle(ShapeCanvas *the_canvas, Shape *object, float size, float the_xoffset, float the_yoffset, int the_type):RectangleShape(size, size)
{
  canvas = the_canvas;
  canvas_object = object; xoffset = the_xoffset; yoffset = the_yoffset;
  type = the_type; SetPen(black_foreground_pen); SetBrush(black_brush); visible = TRUE;
}
void CornerHandle::OnDrawContents(void) {}
void CornerHandle::OnDraw(void){
  xpos = canvas_object->GetX() + xoffset; ypos = canvas_object->GetY() + yoffset;
  RectangleShape::OnDraw();
}

void CornerHandle::OnDragLeft(Bool draw, float x, float y, int keys){
  canvas->Snap(&x, &y);
  float bound_x; float bound_y;
  canvas_object->GetBoundingBoxMin(&bound_x, &bound_y);
  float topLeftX = (float)(canvas_object->xpos - (bound_x/2.0));
  float topLeftY = (float)(canvas_object->ypos - (bound_y/2.0));
  float bottomRightX = (float)(canvas_object->xpos + (bound_x/2.0));
  float bottomRightY = (float)(canvas_object->ypos + (bound_y/2.0));

  float newWidth = bound_x;
  float newHeight = bound_y;
  float newX = canvas_object->xpos;
  float newY = canvas_object->ypos;

/*
  sprintf(wxBuffer, "Mouse click = (%.2f, %.2f)\n", x, y);
  wxDebugMsg(wxBuffer);
  sprintf(wxBuffer, "Top left = (%.2f, %.2f) Bottom right = (%.2f, %.2f)\n", topLeftX, topLeftY, bottomRightX, bottomRightY);
  wxDebugMsg(wxBuffer);
  sprintf(wxBuffer, "Centre = (%.2f, %.2f) Size = %.2f x %.2f\n", newX, newY, newWidth, newHeight);
  wxDebugMsg(wxBuffer);
*/
  
  switch (type)
  {
    case HANDLE_HORIZONTAL:
    {
      if (x < canvas_object->xpos)
      {
        // Right-hand side stays the same
        newWidth = (float)(bottomRightX - x);
        newX = (float)(x + (newWidth/2.0));
      }
      else
      {
        // Left-hand side stays the same
        newWidth = (float)(x - topLeftX);
        newX = (float)(x - (newWidth/2.0));
      }
      break;
    }
    case HANDLE_VERTICAL:
    {
      if (y < canvas_object->ypos)
      {
        // Bottom side stays the same
        newHeight = (float)(bottomRightY - y);
        newY = (float)(y + (newHeight/2.0));
      }
      else
      {
        // Top side stays the same
        newHeight = (float)(y - topLeftY);
        newY = (float)(y - (newHeight/2.0));
      }
      break;
    }
    case HANDLE_DIAGONAL:
    {
      if (x < canvas_object->xpos)
      {
        // Right-hand side stays the same
        newWidth = bottomRightX - x;
        newX = (float)(x + (newWidth/2.0));
      }
      else
      {
        // Left-hand side stays the same
        newWidth = x - topLeftX;
        newX = (float)(x - (newWidth/2.0));
      }
      if (y < canvas_object->ypos)
      {
        // Bottom side stays the same
        newHeight = bottomRightY - y;
        newY = (float)(y + (newHeight/2.0));
      }
      else
      {
        // Top side stays the same
        newHeight = (float)(y - topLeftY);
        newY = (float)(y - (newHeight/2.0));
      }
      break;
    }
  }
  canvas_object->xpos = newX;
  canvas_object->ypos = newY;
  
  canvas_object->OnBeginSize(newWidth, newHeight);
  canvas_object->SetSize(newWidth, newHeight);
  canvas_object->OnDrawOutline();
/*
  sprintf(wxBuffer, "New centre = (%.2f, %.2f) New size = %.2f x %.2f\n", newX, newY, newWidth, newHeight);
  wxDebugMsg(wxBuffer);
*/
/*
  float bound_x; float bound_y;
  canvas_object->GetBoundingBoxMin(&bound_x, &bound_y);
  float new_width = (float)(2.0*fabs(x - canvas_object->xpos));
  float new_height = (float)(2.0*fabs(y - canvas_object->ypos));
  if (type == HANDLE_HORIZONTAL) new_height = bound_y;
  if (type == HANDLE_VERTICAL) new_width = bound_x;
  if (type == HANDLE_DIAGONAL && (keys & KEY_SHIFT))
    new_height = bound_y*(new_width/bound_x);
  canvas_object->OnBeginSize(new_width, new_height);
  canvas_object->SetSize(new_width, new_height);
  canvas_object->OnDrawOutline();
*/
}

void CornerHandle::OnBeginDragLeft(float x, float y, int keys){
  canvas_object->Erase(); dc->SetLogicalFunction(wxXOR);
  dc->SetPen(black_dashed_pen); dc->SetBrush(transparent_brush);

  OnDragLeft(TRUE, x, y, keys);
/*
  float bound_x; float bound_y;
  canvas_object->GetBoundingBoxMin(&bound_x, &bound_y);
  float new_width = (float)(2.0*fabs(x - canvas_object->xpos));
  float new_height = (float)(2.0*fabs(y - canvas_object->ypos));
  if (type == HANDLE_HORIZONTAL) new_height = bound_y;
  if (type == HANDLE_VERTICAL) new_width = bound_x;
  if (type == HANDLE_DIAGONAL && (keys & KEY_SHIFT)) new_height = bound_y*(new_width/bound_x);

  canvas_object->SetSize(new_width, new_height, FALSE);
  dc->SetPen(black_dashed_pen); dc->SetBrush(transparent_brush);
  canvas_object->OnDrawOutline();
*/
}

void CornerHandle::OnEndDragLeft(float x, float y, int keys){
  dc->SetLogicalFunction(wxCOPY);
  canvas_object->ResetCornerHandles();
  canvas_object->Move(canvas_object->GetX(), canvas_object->GetY());
  if (!canvas->quick_edit_mode) canvas->Redraw();
  float w, h;
  canvas_object->GetBoundingBoxMax(&w, &h);
  canvas_object->OnEndSize(w, h);
/*
  dc->SetLogicalFunction(wxCOPY);
  canvas_object->ResetCornerHandles();
  canvas_object->Move(canvas_object->GetX(), canvas_object->GetY());
  if (!canvas->quick_edit_mode) canvas->Redraw();
  float width, height;
  canvas_object->GetBoundingBoxMax(&width, &height);
  canvas_object->OnEndSize(width, height);
*/
}

void InitializeShapes(void)
{
  normal_font = new wxFont(12, wxMODERN, wxNORMAL, wxNORMAL);
  black_pen = new wxPen("BLACK", 1, wxSOLID);
  white_brush = new wxBrush("WHITE", wxSOLID);
  black_brush = new wxBrush("BLACK", wxSOLID);
  white_background_pen = new wxPen("WHITE", 1, wxSOLID);
  transparent_brush = new wxBrush("BLACK", wxTRANSPARENT);
  white_background_brush = new wxBrush("WHITE", wxSOLID);
  black_foreground_pen = new wxPen("BLACK", 1, wxSOLID);
  black_dashed_pen = new wxPen("BLACK", 1, wxSHORT_DASH);
}

// Object canvas
ShapeCanvas::ShapeCanvas(wxFrame *frame, int x, int y, int w, int h, long style):
  wxCanvas(frame, x, y, w, h, style)
{
  snap_to_grid = TRUE; grid_spacing = 5.0; object_list = new wxList;
  DragState = NoDragging; DraggedObject = NULL; old_drag_x = 0;
  old_drag_y = 0; first_drag_x = 0; first_drag_y = 0;
  quick_edit_mode = FALSE;
  showGrid = FALSE;
  mouseTolerance = DEFAULT_MOUSE_TOLERANCE;
  checkTolerance = TRUE;
}

ShapeCanvas::~ShapeCanvas(void) { if (object_list) delete object_list; }
void ShapeCanvas::SetSnapToGrid(Bool snap) { snap_to_grid = snap; }
void ShapeCanvas::SetGridSpacing(float spacing) { grid_spacing = spacing; }
void ShapeCanvas::Snap(float *x, float *y) {
  if (snap_to_grid) {
    *x = grid_spacing * ((int)(*x/grid_spacing + 0.5));
    *y = grid_spacing * ((int)(*y/grid_spacing + 0.5));}}

void ShapeCanvas::OnPaint(void) { GetDC()->Clear(); Redraw(); }
void ShapeCanvas::Redraw(void)
{
  GetDC()->BeginDrawing();

  // Draw grid if necessary
/*
  if (showGrid)
  {
    int width, height;
    int viewX, viewY;
    
    ViewStart(&viewX, &viewY);
    float actualX = (float)(viewX * 20.0);
    float actualY = (float)(viewY * 20.0);
    GetClientSize(&width, &height);
    // Unscale coordinates
    width = GetDC()->DeviceToLogicalXRel(width);
    height = GetDC()->DeviceToLogicalXRel(height);
    actualX = GetDC()->DeviceToLogicalX(actualX);
    actualY = GetDC()->DeviceToLogicalY(actualY);
    
    float i;
    GetDC()->SetPen(black_dashed_pen);
    float actualGridSpacing = (float)(10*grid_spacing);
    for (i = actualY; i < (actualY + height); i += actualGridSpacing)
    {
      GetDC()->DrawLine(actualX, i, (float)(actualX + width), i);
    }    
    for (i = actualX; i < (actualX + width); i += actualGridSpacing)
    {
      GetDC()->DrawLine(i, actualY, i, (float)(actualY + height));
    }    
  }
*/  
  if (object_list) {
    wxCursor *old_cursor = SetCursor(wxHOURGLASS_CURSOR);
    wxNode *current = object_list->First();
    while (current) {
      Shape *object = (Shape *)current->Data();
      object->Draw();
      current = current->Next();
    }
    SetCursor(old_cursor);
  }
  GetDC()->EndDrawing();
}
void ShapeCanvas::Clear(void) { wxCanvas::Clear(); }
void ShapeCanvas::AddObject(Shape *object){
  object_list->Append(object); object->SetCanvas(this); object->SetDC(GetDC());
}
void ShapeCanvas::RemoveObject(Shape *object){ object_list->DeleteObject(object); }
void ShapeCanvas::ShowAll(Bool show){
  wxNode *current = object_list->First();
  while (current)
  {
    Shape *object = (Shape *)current->Data();
    object->Show(show); current = current->Next();
  }
}
void ShapeCanvas::OnEvent(wxMouseEvent& event){
  float x, y; event.Position(&x, &y);
  int keys = 0;
  if (event.ShiftDown()) keys = keys | KEY_SHIFT;
  if (event.ControlDown()) keys = keys | KEY_CTRL;
  Bool dragging = event.Dragging();
  if (dragging)
  {
    wxDC *dc = GetDC();
    int dx = abs(dc->LogicalToDeviceX(x - first_drag_x));
    int dy = abs(dc->LogicalToDeviceY(y - first_drag_y));
    if (checkTolerance && (dx <= mouseTolerance) && (dy <= mouseTolerance))
    {
      return;
    }
    else
      // If we've ignored the tolerance once, then ALWAYS ignore
      // tolerance in this drag, even if we come back within
      // the tolerance range.
      checkTolerance = FALSE;
  }
  if (dragging && DraggedObject && DragState == StartDraggingLeft)
  {
    DragState = ContinueDraggingLeft;
    // If the object isn't draggable, transfer message to canvas
    if (DraggedObject->Draggable())
      DraggedObject->OnBeginDragLeft((float)x, (float)y, keys);
    else
    {
      DraggedObject = NULL;
      OnBeginDragLeft((float)x, (float)y, keys);
    }
    old_drag_x = x; old_drag_y = y;
  }
  else if (dragging && DraggedObject && DragState == ContinueDraggingLeft)
  { 
    DraggedObject->OnDragLeft(FALSE, old_drag_x, old_drag_y, keys);
    DraggedObject->OnDragLeft(TRUE, (float)x, (float)y, keys);
    old_drag_x = x; old_drag_y = y;
  }
  else if (event.LeftUp() && DraggedObject && DragState == ContinueDraggingLeft)
  {
    DragState = NoDragging;
    checkTolerance = TRUE;
    DraggedObject->OnDragLeft(FALSE, old_drag_x, old_drag_y, keys);
    DraggedObject->OnEndDragLeft((float)x, (float)y, keys);
    DraggedObject = NULL;
  }
  else if (dragging && !DraggedObject && DragState == StartDraggingLeft)
  {
    DragState = ContinueDraggingLeft;
    OnBeginDragLeft((float)x, (float)y, keys);
    old_drag_x = x; old_drag_y = y;
  }
  else if (dragging && !DraggedObject && DragState == ContinueDraggingLeft)
  { 
    OnDragLeft(FALSE, old_drag_x, old_drag_y, keys); OnDragLeft(TRUE, (float)x, (float)y, keys);
    old_drag_x = x; old_drag_y = y;
  }
  else if (event.LeftUp() && !DraggedObject && DragState == ContinueDraggingLeft)
  {
    DragState = NoDragging;
    OnDragLeft(FALSE, old_drag_x, old_drag_y, keys); OnEndDragLeft((float)x, (float)y, keys);
    DraggedObject = NULL;
  }
  else if (event.IsButton())
  {
    checkTolerance = TRUE;

    Shape *nearest_object = FindObject(x, y);
    if (nearest_object)
    {
      if (event.LeftDown())
      {
        DraggedObject = nearest_object; DragState = StartDraggingLeft;
        first_drag_x = x; first_drag_y = y;
      }
      else if (event.LeftUp())
      {
        if (nearest_object == DraggedObject)
          nearest_object->OnLeftClick((float)x, (float)y, keys);
        DraggedObject = NULL; DragState = NoDragging;
      }
      else if (event.RightDown())
      {
        DraggedObject = nearest_object; DragState = StartDraggingRight;
        first_drag_x = x; first_drag_y = y;
      }
      else if (event.RightUp())
      {
        if (nearest_object == DraggedObject)
          nearest_object->OnRightClick((float)x, (float)y, keys);
        DraggedObject = NULL; DragState = NoDragging;
      }
    }
    else
    {
      if (event.LeftDown())
      {
        DraggedObject = NULL; DragState = StartDraggingLeft;
      }
      else if (event.LeftUp())
      {
        OnLeftClick((float)x, (float)y, keys); DraggedObject = NULL;
        DragState = NoDragging;
      }
      else if (event.RightDown())
      {
        DraggedObject = NULL; DragState = StartDraggingRight;
      }
      else if (event.RightUp())
      {
        OnRightClick((float)x, (float)y, keys); DraggedObject = NULL;
        DragState = NoDragging;
      }}}}

Shape *ShapeCanvas::FindObject(float x, float y)
{
  float nearest = 100000.0; Shape *nearest_object = NULL;
  wxNode *current = object_list->Last();
  while (current)
  {
    Shape *object = (Shape *)current->Data();
    float dist;
    if (object->HitTest(x, y, &dist))
    {
      nearest = dist; nearest_object = object; current = NULL;
    }
    if (current)
      current = current->Previous();
  }
  return nearest_object;
}

void ShapeCanvas::DrawOutline(float x1, float y1, float x2, float y2)
{
  wxDC *dc = GetDC(); dc->SetPen(black_dashed_pen); dc->SetBrush(transparent_brush);
  wxPoint points[5]; points[0].x = x1; points[0].y = y1; points[1].x = x2; points[1].y = y1;
  points[2].x = x2; points[2].y = y2; points[3].x = x1; points[3].y = y2;
  points[4].x = x1; points[4].y = y1; dc->DrawLines(5, points);
}

void ShapeCanvas::OnLeftClick(float x, float y, int keys) {}
void ShapeCanvas::OnRightClick(float x, float y, int keys) {}
void ShapeCanvas::OnDragLeft(Bool draw, float x, float y, int keys) {}
void ShapeCanvas::OnBeginDragLeft(float x, float y, int keys) {}
void ShapeCanvas::OnEndDragLeft(float x, float y, int keys) {}
