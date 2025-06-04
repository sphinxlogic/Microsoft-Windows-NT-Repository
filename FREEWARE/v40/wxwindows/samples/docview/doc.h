/*
 * File:	doc.h
 * Purpose:	Application's document classes
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#ifdef __GNUG__
#pragma interface
#endif

#ifndef doch
#define doch

#include "wx_doc.h"

// Plots a line from one point to the other
class DoodleLine: public wxObject
{
 public:
  float x1;
  float y1;
  float x2;
  float y2;
};

// Contains a list of lines: represents a mouse-down doodle
class DoodleSegment: public wxObject
{
 public:
  wxList lines;

  DoodleSegment(void);
  DoodleSegment(DoodleSegment& seg);
  ~DoodleSegment(void);

  void Draw(wxDC *dc);
  ostream& SaveObject(ostream& stream);
  istream& LoadObject(istream& stream);
};

class DrawingDocument: public wxDocument
{
  DECLARE_DYNAMIC_CLASS(DrawingDocument)
 private:
 public:
  wxList doodleSegments;
  
  DrawingDocument(void);
  ~DrawingDocument(void);

  ostream& SaveObject(ostream& stream);
  istream& LoadObject(istream& stream);

  inline wxList& GetDoodleSegments(void) { return doodleSegments; };
};

#define DOODLE_CUT          1
#define DOODLE_ADD          2

class DrawingCommand: public wxCommand
{
 protected:
  DoodleSegment *segment;
  DrawingDocument *doc;
  int cmd;
 public:
  DrawingCommand(char *name, int cmd, DrawingDocument *ddoc, DoodleSegment *seg);
  ~DrawingCommand(void);

  Bool Do(void);
  Bool Undo(void);
};

class TextEditDocument: public wxDocument
{
  DECLARE_DYNAMIC_CLASS(TextEditDocument)
 private:
 public:
  ostream& SaveObject(ostream& stream);
  istream& LoadObject(istream& stream);

  Bool IsModified(void);
  void Modify(Bool mod);

  TextEditDocument(void) {}
  ~TextEditDocument(void) {}
};


#endif
