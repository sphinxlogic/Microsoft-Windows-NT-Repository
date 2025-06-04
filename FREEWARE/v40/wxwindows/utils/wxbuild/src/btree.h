/*
 * File:	btree.h
 * Purpose:	wxWindows GUI builder: tree of window hierarchy
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */


#ifndef btreeh
#define btreeh

#include "wxbuild.h"
#include "bapp.h"
#include "wxtree.h"

/*
 * We use the abstract wxTreeLayout class, defining enough members
 * for it to work using CanvasObjects.
 *
 */

class BuilderTree: public wxTreeLayout
{
 public:
  BuilderTree(wxDC *dc):wxTreeLayout(dc)
  {}

  void GetChildren(long id, wxList& list);
  long GetNextNode(long id);
  long GetNodeParent(long id);
  float GetNodeX(long id);
  float GetNodeY(long id);
  void SetNodeX(long id, float x);
  void SetNodeY(long id, float y);
  void GetNodeSize(long id, float *x, float *y);
  void ActivateNode(long id, Bool active) {};
  inline Bool NodeActive(long id) { return TRUE; };
  char *GetNodeName(long id);
//  void Draw(void) {};

  void MakeBuildWindowList(BuildWindowData *data);
  void MakeBuildWindowList1(wxList children);

  long NodeHitTest(float x, float y);
};

extern BuilderTree *TheBuilderTree;

void DisplayTree(Bool show, BuildWindowData *data);
void CloseTreeFrame(void);
void ClearTree(void);

#endif // btreeh
