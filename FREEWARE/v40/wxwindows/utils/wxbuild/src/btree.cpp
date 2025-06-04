/*
 * File:	btree.cc
 * Purpose:	wxWindows GUI builder: window hierarchy tree
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
#include "wx.h"
#endif

#include <ctype.h>
#include <stdlib.h>

#include "btree.h"
#include "bwin.h"
#include "bframe.h"

#ifdef wx_x
#include "bitmaps/treeicn.xbm"
#endif

wxList BuildWindowList;
BuilderTree *TheBuilderTree = NULL;
float TopNodeX = 0.0;
float TopNodeY = 0.0;

void BuilderTree::MakeBuildWindowList(BuildWindowData *data)
{
  BuildWindowList.Clear();
  wxList children;
  children.Append(data);
  MakeBuildWindowList1(children);
}

void BuilderTree::MakeBuildWindowList1(wxList children)
{
  wxNode *node = children.First();
  while (node)
  {
    BuildWindowData *win = (BuildWindowData *)node->Data();
    BuildWindowList.Append(win);
    wxList children2;
    GetChildren((long)win, children2);
    MakeBuildWindowList1(children2);

    node = node->Next();
  }
}

void BuilderTree::GetChildren(long id, wxList& children)
{
    BuildWindowData *win = (BuildWindowData *)id;
  
    wxNode *node = win->children.First();
    while (node)
    {
      BuildWindowData *child = (BuildWindowData *)node->Data();
      children.Append(child);
      node = node->Next();
    }
    if (win->windowType == wxTYPE_FRAME)
    {
      BuildFrameData *frame = (BuildFrameData *)win;
//      children.Append((wxObject *)frame->buildMenuBar);
      if (frame->toolbar)
        children.Append((wxObject *)frame->toolbar);
    }
}

char *BuilderTree::GetNodeName(long id)
{
  BuildWindowData *win = (BuildWindowData *)id;
  if (win->windowType == wxTYPE_MENU_BAR)
    return "Menu bar";
  else
  {
    if (win->name)
      return win->name;
  }
  return "<unknown name>";
}

long BuilderTree::GetNextNode(long id)
{
  wxNode *node = BuildWindowList.Member((wxObject *)id);
  if (node)
  {
    wxNode *next = node->Next();
    if (next)
      return (long)next->Data();
    else return -1;
  }
  else return -1;
}

long BuilderTree::GetNodeParent(long id)
{
  if (id == 1)
    return -1;

  BuildWindowData *win = (BuildWindowData *)id;
  if (win->buildParent)
    return (long)win->buildParent;
  else
    return -1;
}

float BuilderTree::GetNodeX(long id)
{
  if (id == -1)
    return 0.0;
//  if (id == 1)
//    return TopNodeX;

  BuildWindowData *win = (BuildWindowData *)id;
  return win->treeX;
}

float BuilderTree::GetNodeY(long id)
{
  if (id == -1)
    return 0.0;
//  if (id == 1)
//    return TopNodeY;

  BuildWindowData *win = (BuildWindowData *)id;
  return win->treeY ;
}

void BuilderTree::GetNodeSize(long id, float *x, float *y)
{
  if (id == -1)
    return;
  if (!TheBuilderTree)
  {
    wxTreeLayout::GetNodeSize(id, x, y);
    return;
  }
  char *name = GetNodeName(id);
  MainFrame->canvas->GetDC()->GetTextExtent(name, x, y);
}

void BuilderTree::SetNodeX(long id, float x)
{
  if (id == -1)
    return;
  if (id == 1)
  {
    TopNodeX = x;
    return;
  }

  BuildWindowData *win = (BuildWindowData *)id;
  win->treeX = x;
}

void BuilderTree::SetNodeY(long id, float y)
{
  if (id == -1)
    return;
  if (id == 1)
  {
    TopNodeY = y;
    return;
  }

  BuildWindowData *win = (BuildWindowData *)id;
  win->treeY = y;
}

long BuilderTree::NodeHitTest(float x, float y)
{
  wxNode *node = BuildWindowList.First();
  while (node)
  {
    long id = (long)node->Data();
    float xpos = GetNodeX(id);
    float ypos = GetNodeY(id);
    float w, h;
    GetNodeSize(id, &w, &h);
    if ((x >= xpos) && (y >= ypos) && (x <= (xpos + w)) && (y <= ypos + h))
      return id;

    node = node->Next();
  }
  return -1;
}

void ClearTree(void)
{
  if (MainFrame)
  {
    MainFrame->canvas->GetDC()->Clear();
    if (TheBuilderTree)
      TheBuilderTree->SetTopNode(-1);
    BuildWindowList.Clear();
  }
}

void DisplayTree(Bool show, BuildWindowData *data)
{
  if (!show)
  {
    ClearTree();
    return;
  }

  if (!TheBuilderTree)
  {
    TheBuilderTree = new BuilderTree(MainFrame->canvas->GetDC());
    TheBuilderTree->SetSpacing(20, 20);
    TheBuilderTree->SetMargins(20, 20);
  }
  MainFrame->canvas->GetDC()->Clear();
  TheBuilderTree->SetDC(MainFrame->canvas->GetDC());
  BuildWindowList.Clear();
  TheBuilderTree->SetTopNode((long)data);
  TheBuilderTree->MakeBuildWindowList(data);
  TheBuilderTree->DoLayout((long)data);
  TheBuilderTree->Draw();
}

void CloseTreeFrame(void)
{
}
