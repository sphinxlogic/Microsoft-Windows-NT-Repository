/*
 * File:	doc.cc
 * Purpose:	Defines document functionality
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

#include "doc.h"
#include "view.h"

IMPLEMENT_DYNAMIC_CLASS(DrawingDocument, wxDocument)

DrawingDocument::DrawingDocument(void)
{
}

DrawingDocument::~DrawingDocument(void)
{
  doodleSegments.DeleteContents(TRUE);
}

ostream& DrawingDocument::SaveObject(ostream& stream)
{
  wxDocument::SaveObject(stream);

  stream << doodleSegments.Number() << '\n';
  wxNode *node = doodleSegments.First();
  while (node)
  {
    DoodleSegment *segment = (DoodleSegment *)node->Data();
    segment->SaveObject(stream);
    stream << '\n';
    
    node = node->Next();
  }
  return stream;
}

istream& DrawingDocument::LoadObject(istream& stream)
{
  wxDocument::LoadObject(stream);

  int n = 0;
  stream >> n;

  for (int i = 0; i < n; i++)
  {
    DoodleSegment *segment = new DoodleSegment;
    segment->LoadObject(stream);
    doodleSegments.Append(segment);
  }

  return stream;
}

DoodleSegment::DoodleSegment(void)
{
}

DoodleSegment::DoodleSegment(DoodleSegment& seg)
{
  wxNode *node = seg.lines.First();
  while (node)
  {
    DoodleLine *line = (DoodleLine *)node->Data();
    DoodleLine *newLine = new DoodleLine;
    newLine->x1 = line->x1;
    newLine->y1 = line->y1;
    newLine->x2 = line->x2;
    newLine->y2 = line->y2;

    lines.Append(newLine);

    node = node->Next();
  }
}

DoodleSegment::~DoodleSegment(void)
{
  lines.DeleteContents(TRUE);
}

ostream& DoodleSegment::SaveObject(ostream& stream)
{
  stream << lines.Number() << '\n';
  wxNode *node = lines.First();
  while (node)
  {
    DoodleLine *line = (DoodleLine *)node->Data();
    stream << line->x1 << " " << line->y1 << " " << line->x2 << " " << line->y2 << "\n";
    node = node->Next();
  }
  return stream;
}

istream& DoodleSegment::LoadObject(istream& stream)
{
  int n = 0;
  stream >> n;

  for (int i = 0; i < n; i++)
  {
    DoodleLine *line = new DoodleLine;
    stream >> line->x1 >> line->y1 >> line->x2 >> line->y2;
    lines.Append(line);
  }
  return stream;
}

void DoodleSegment::Draw(wxDC *dc)
{
  wxNode *node = lines.First();
  while (node)
  {
    DoodleLine *line = (DoodleLine *)node->Data();
    dc->DrawLine(line->x1, line->y1, line->x2, line->y2);
    node = node->Next();
  }
}

/*
 * Implementation of drawing command
 */

DrawingCommand::DrawingCommand(char *name, int command, DrawingDocument *ddoc, DoodleSegment *seg):
  wxCommand(TRUE, name)
{
  doc = ddoc;
  segment = seg;
  cmd = command;
}

DrawingCommand::~DrawingCommand(void)
{
  if (segment)
    delete segment;
}

Bool DrawingCommand::Do(void)
{
  switch (cmd)
  {
    case DOODLE_CUT:
    {
      // Cut the last segment
      if (doc->GetDoodleSegments().Number() > 0)
      {
        wxNode *node = doc->GetDoodleSegments().Last();
        if (segment)
          delete segment;
          
        segment = (DoodleSegment *)node->Data();
        delete node;

        doc->Modify(TRUE);
        doc->UpdateAllViews();
      }
      break;
    }
    case DOODLE_ADD:
    {
      doc->GetDoodleSegments().Append(new DoodleSegment(*segment));
      doc->Modify(TRUE);
      doc->UpdateAllViews();
      break;
    }
  }
  return TRUE;
}

Bool DrawingCommand::Undo(void)
{
  switch (cmd)
  {
    case DOODLE_CUT:
    {
      // Paste the segment
      if (segment)
      {
        doc->GetDoodleSegments().Append(segment);
        doc->Modify(TRUE);
        doc->UpdateAllViews();
        segment = NULL;
      }
      doc->Modify(TRUE);
      doc->UpdateAllViews();
      break;
    }
    case DOODLE_ADD:
    {
      // Cut the last segment
      if (doc->GetDoodleSegments().Number() > 0)
      {
        wxNode *node = doc->GetDoodleSegments().Last();
        DoodleSegment *seg = (DoodleSegment *)node->Data();
        delete seg;
        delete node;

        doc->Modify(TRUE);
        doc->UpdateAllViews();
      }
    }
  }
  return TRUE;
}

IMPLEMENT_DYNAMIC_CLASS(TextEditDocument, wxDocument)

ostream& TextEditDocument::SaveObject(ostream& stream)
{
  TextEditView *view = (TextEditView *)GetFirstView();

  char buf[400];
  (void) wxGetTempFileName("dview", buf);

  view->textsw->SaveFile(buf);
  wxTransferFileToStream(buf, stream);

  wxRemoveFile(buf);

  return stream;
}

istream& TextEditDocument::LoadObject(istream& stream)
{
  TextEditView *view = (TextEditView *)GetFirstView();

  char buf[400];
  (void) wxGetTempFileName("dview", buf);

  wxTransferStreamToFile(stream, buf);

  view->textsw->LoadFile(buf);
  wxRemoveFile(buf);

  return stream;
}

Bool TextEditDocument::IsModified(void)
{
  TextEditView *view = (TextEditView *)GetFirstView();
  if (view)
  {
    return (wxDocument::IsModified() || view->textsw->Modified());
  }
  else
    return wxDocument::IsModified();
}

void TextEditDocument::Modify(Bool mod)
{
  TextEditView *view = (TextEditView *)GetFirstView();

  wxDocument::Modify(mod);

  if (!mod && view && view->textsw)
    view->textsw->DiscardEdits();
}
