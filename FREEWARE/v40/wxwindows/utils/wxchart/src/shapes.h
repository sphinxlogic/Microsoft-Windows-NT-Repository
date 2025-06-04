/*
 * File:	shapes.h
 * Purpose:	
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */
#include <wx.h>
#ifndef hy_shapesh
#define hy_shapesh

// Key identifiers
#define KEY_SHIFT 1
#define KEY_CTRL  2

/*
 * Declare types
 *
 */

#define SHAPE_BASIC           wxTYPE_USER + 1
#define SHAPE_RECTANGLE       wxTYPE_USER + 2
#define SHAPE_HANDLE          wxTYPE_USER + 3

class ShapeCanvas;
class CornerHandle;
class Shape: public wxObject
{
 private:
  wxObject *ClientData;
 public:
  Bool formatted;
  float xpos, ypos;
  wxPen *pen;
  wxBrush *brush;
  wxFont *font;
  wxColour *text_colour;
  ShapeCanvas *canvas;
  wxDC *dc;
  char *textString;
  wxList handles;
  Bool visible;
  Bool selected;
  Bool draggable;                      
  Bool sizeable;
  float textMarginX;    // Gap between text and border
  float textMarginY;
  Shape(ShapeCanvas *can = NULL);
  virtual ~Shape(void);
  virtual void GetBoundingBoxMax(float *width, float *height);
  virtual void GetBoundingBoxMin(float *width, float *height) = 0;
  inline ShapeCanvas *GetCanvas(void) { return canvas; }
  void SetCanvas(ShapeCanvas *the_canvas);
  virtual void AddToCanvas(ShapeCanvas *the_canvas);
  inline float GetX(void) { return xpos; }
  inline float GetY(void) { return ypos; }
  virtual void OnDraw(void);
  virtual void OnDrawContents(void);
  virtual void OnErase(void);
  virtual void OnEraseContents(void);
  virtual void OnHighlight(void);
  virtual void OnLeftClick(float x, float y, int keys = 0);
  virtual void OnRightClick(float x, float y, int keys = 0);
  virtual void OnSize(float x, float y);
  virtual void OnMove(float x, float y, float old_x, float old_y, Bool display = TRUE);
  virtual void OnDragLeft(Bool draw, float x, float y, int keys=0);
  virtual void OnBeginDragLeft(float x, float y, int keys=0);
  virtual void OnEndDragLeft(float x, float y, int keys=0);
  virtual void OnDrawOutline(void);
  virtual void OnDrawCornerHandles(void);
  virtual void OnEraseCornerHandles(void);
  virtual void OnBeginSize(float w, float h);
  virtual void OnEndSize(float w, float h){ };
  virtual void MakeCornerHandles(void);
  virtual void DeleteCornerHandles(void);
  virtual void ResetCornerHandles(void);
  virtual void Select(Bool select = TRUE);
  inline virtual Bool Selected(void) { return selected; }
  void SetDraggable(Bool drag);
  void SetSizeable(Bool size);
  virtual Bool HitTest(float x, float y, float *distance);
  void SetPen(wxPen *pen);
  void SetBrush(wxBrush *brush);
  void SetClientData(wxObject *client_data);
  wxObject *GetClientData(void);
  inline void Show(Bool show) { visible = show; }
  virtual void Move(float x1, float y1, Bool display = TRUE);
  virtual void Erase(void);
  virtual void EraseContents(void);
  virtual void Draw(void);
  virtual void DrawContents(void);  // E.g. for drawing text label
  virtual void SetSize(float x, float y, Bool recursive = TRUE);
  void Attach(ShapeCanvas *can);
  void Detach(void);
  virtual void SetDC(wxDC *the_dc);
  inline wxPen *GetPen(void) { return pen; }
  inline wxBrush *GetBrush(void) { return brush; }
  virtual void FormatText(char *s);
  virtual void SetFont(wxFont *font);
  virtual inline wxFont *GetFont(void) { return font; }
  virtual Bool Draggable(void) { return draggable; } //{ return TRUE; }
  virtual Bool Sizeable(void) { return sizeable; }
};

class RectangleShape: public Shape
{
 protected:
 public:
  float width;
  float height;
  RectangleShape(float w, float h);                            
  virtual void OnRightClick(float x, float y, int keys = 0);
  void GetBoundingBoxMin(float *w, float *h);
  void OnDraw(void);
  void SetSize(float x, float y, Bool recursive = TRUE);
};

class CornerHandle: public RectangleShape
{
 public:
  int type;
  float xoffset;
  float yoffset;
  Shape *canvas_object;
  CornerHandle(ShapeCanvas *the_canvas, Shape *object, float size, float the_xoffset, float the_yoffset, int the_type);
  void OnDraw(void);
  void OnDrawContents(void);
  void OnDragLeft(Bool draw, float x, float y, int keys=0);
  void OnBeginDragLeft(float x, float y, int keys=0);
  void OnEndDragLeft(float x, float y, int keys=0);
};

class ShapeCanvas: public wxCanvas
{
 public:
  Bool snap_to_grid;
  Bool quick_edit_mode;
  Bool showGrid;
  float grid_spacing;
  int mouseTolerance;
  Bool checkTolerance;
  wxList *object_list;
  int DragState;
  float old_drag_x, old_drag_y;     // Previous drag coordinates
  float first_drag_x, first_drag_y; // INITIAL drag coordinates
  float OutlineStartX;
  float OutlineStartY;
  Shape *DraggedObject;
  ShapeCanvas(wxFrame *frame, int x = -1, int y = -1, int width = -1, int height = -1,
               long style = wxBORDER | wxRETAINED);
  ~ShapeCanvas(void);
  void DrawOutline(float x1, float y1, float x2, float y2);
  virtual void OnPaint(void);
  virtual void OnEvent(wxMouseEvent& event);
  virtual void OnLeftClick(float x, float y, int keys = 0);
  virtual void OnRightClick(float x, float y, int keys = 0);
  virtual void OnDragLeft(Bool draw, float x, float y, int keys=0); // Erase if draw false
  virtual void OnBeginDragLeft(float x, float y, int keys=0);
  virtual void OnEndDragLeft(float x, float y, int keys=0);
  virtual void Redraw(void); // Called automatically by default OnPaint handler
  virtual void Clear(void);
  // Add object to end of object list
  virtual void AddObject(Shape *object);
  void SetSnapToGrid(Bool snap);
  inline void ShowGrid(Bool show) { showGrid = show; }
  void SetGridSpacing(float spacing);
  inline float GetGridSpacing(void) { return grid_spacing; }
  inline Bool GetSnapToGrid(void) { return snap_to_grid; }
  void Snap(float *x, float *y);
  virtual void RemoveObject(Shape *object);
  virtual void ShowAll(Bool show);
  virtual Shape *FindObject(float x, float y);  // Find object for mouse click
  inline void SetMouseTolerance(int tol) { mouseTolerance = tol; }
  inline wxList *GetObjectList(void) { return object_list; }
};

void InitializeShapes(void);
#endif // hy_shapesh
