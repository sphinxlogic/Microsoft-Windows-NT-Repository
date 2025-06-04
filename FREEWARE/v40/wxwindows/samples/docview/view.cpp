/*
 * File:	view.cc
 * Purpose:	Implements view classes
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

static const char sccsid[] = "%W% %G%";

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#if !USE_DOC_VIEW_ARCHITECTURE
#error You must set USE_DOC_VIEW_ARCHITECTURE to 1 in wx_setup.h!
#endif

#include "docview.h"
#include "doc.h"
#include "view.h"

IMPLEMENT_DYNAMIC_CLASS(DrawingView, wxView)

// For drawing lines in a canvas
float xpos = -1;
float ypos = -1;

// What to do when a view is created. Creates actual
// windows for displaying the view.
Bool DrawingView::OnCreate(wxDocument *doc, long flags)
{
  if (!singleWindowMode)
  {
    frame = myApp.CreateChildFrame(doc, this, TRUE);
    frame->SetTitle("DrawingView");

    canvas = GetMainFrame()->CreateCanvas(this, frame);
#ifdef wx_x
    // X seems to require a forced resize
    int x, y;
    frame->GetSize(&x, &y);
    frame->SetSize(x, y);
#endif
    frame->Show(TRUE);
  }
  else
  {
    frame = GetMainFrame();
    canvas = GetMainFrame()->canvas;
    canvas->view = this;

    SetFrame(frame);
    Activate(TRUE);

    // Initialize the edit menu Undo and Redo items
    doc->GetCommandProcessor()->SetEditMenu(((MyFrame *)frame)->editMenu);
    doc->GetCommandProcessor()->Initialize();
  }

  return TRUE;
}

// Sneakily gets used for default print/preview
// as well as drawing on the screen.
void DrawingView::OnDraw(wxDC *dc)
{
  dc->SetFont(wxNORMAL_FONT);
  dc->SetPen(wxBLACK_PEN);

  wxNode *node = ((DrawingDocument *)GetDocument())->GetDoodleSegments().First();
  while (node)
  {
    DoodleSegment *seg = (DoodleSegment *)node->Data();
    seg->Draw(dc);
    node = node->Next();
  }
}

void DrawingView::OnUpdate(wxView *sender, wxObject *hint)
{
#ifdef wx_msw
  if (canvas)
    canvas->Refresh();
#else
  if (canvas)
    {
      canvas->Clear();
      OnDraw(canvas->GetDC());
    }
#endif
}

// Clean up windows used for displaying the view.
Bool DrawingView::OnClose(Bool deleteWindow)
{
  if (!GetDocument()->Close())
    return FALSE;

  canvas->Clear();
  canvas->view = NULL;
  canvas = NULL;

  char *s = wxTheApp->GetAppName();
  if (s && frame)
    frame->SetTitle(s);

  SetFrame(NULL);

  Activate(FALSE);
  
  if (deleteWindow && !singleWindowMode)
  {
    delete frame;
    return TRUE;
  }
  return TRUE;
}

void DrawingView::OnMenuCommand(int cmd)
{
  DrawingDocument *doc = (DrawingDocument *)GetDocument();
  switch (cmd)
  {
    case DOCVIEW_CUT:
    {
      doc->GetCommandProcessor()->Submit(new DrawingCommand("Cut Last Segment", DOODLE_CUT, doc, NULL));
      break;
    }
    // We should probably be passing unprocessed commands
    // to the document, in case it has anything clever to do
    // like cut/paste (not implemented).
    default:
    {
      break;
    }
  }
}

IMPLEMENT_DYNAMIC_CLASS(TextEditView, wxView)

Bool TextEditView::OnCreate(wxDocument *doc, long flags)
{
  frame = myApp.CreateChildFrame(doc, this, FALSE);

  int width, height;
  frame->GetClientSize(&width, &height);
  textsw = new MyTextWindow(this, frame, 0, 0, width, height, wxNATIVE_IMPL);
  frame->SetTitle("TextEditView");

#ifdef wx_x
  // X seems to require a forced resize
  int x, y;
  frame->GetSize(&x, &y);
  frame->SetSize(x, y);
#endif

  frame->Show(TRUE);
  Activate(TRUE);
  
  return TRUE;
}

// Handled by wxTextWindow
void TextEditView::OnDraw(wxDC *dc)
{
}

void TextEditView::OnUpdate(wxView *sender, wxObject *hint)
{
}

Bool TextEditView::OnClose(Bool deleteWindow)
{
  if (!GetDocument()->Close())
    return FALSE;
    
  Activate(FALSE);

  if (deleteWindow)
  {
    delete frame;
    return TRUE;
  }
  return TRUE;
}

/*
 * Window implementations
 */

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxView *v, wxFrame *frame, int x, int y, int w, int h, long style):
 wxCanvas(frame, x, y, w, h, style)
{
  view = v;
}

// Define the repainting behaviour
void MyCanvas::OnPaint(void)
{
  if (view)
    view->OnDraw(GetDC());
}

// This implements a tiny doodling program. Drag the mouse using
// the left button.
void MyCanvas::OnEvent(wxMouseEvent& event)
{
  if (!view)
    return;
    
  static DoodleSegment *currentSegment = NULL;
  
  SetPen(wxBLACK_PEN);
  float x, y;
  event.Position(&x, &y);

  if (currentSegment && event.LeftUp())
  {
    if (currentSegment->lines.Number() == 0)
    {
      delete currentSegment;
      currentSegment = NULL;
    }
    else
    {
      // We've got a valid segment on mouse left up, so store it.
      DrawingDocument *doc = (DrawingDocument *)view->GetDocument();

      doc->GetCommandProcessor()->Submit(new DrawingCommand("Add Segment", DOODLE_ADD, doc, currentSegment));

      view->GetDocument()->Modify(TRUE);
      currentSegment = NULL;
    }
  }
  
  if (xpos > -1 && ypos > -1 && event.Dragging())
  {
    if (!currentSegment)
      currentSegment = new DoodleSegment;

    DoodleLine *newLine = new DoodleLine;
    newLine->x1 = xpos; newLine->y1 = ypos;
    newLine->x2 = x; newLine->y2 = y;
    currentSegment->lines.Append(newLine);
    
    DrawLine(xpos, ypos, x, y);
  }
  xpos = x;
  ypos = y;
}

// Define a constructor for my text subwindow
MyTextWindow::MyTextWindow(wxView *v, wxFrame *frame, int x, int y, int w, int h, long style):
 wxTextWindow(frame, x, y, w, h, style)
{
  view = v;
}


