/*
 * File:	namegen.cc
 * Purpose:	wxWindows GUI builder -- name space
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

#include "namegen.h"

NameSpace nameSpace;

NameSpaceBlock::~NameSpaceBlock(void)
{
  wxNode *node = First();
  while (node)
  {
    NameSpaceVar *var = (NameSpaceVar *)node->Data();
    delete var;
    delete node;
    node = First();
  }
}

NameSpace::NameSpace(void)
{
  indentLevel = 0;
  globalId = 1;
}

NameSpace::~NameSpace(void)
{
  ClearNameSpace();
}

void NameSpace::BeginBlock(void)
{
  indentLevel ++;
  Append(new NameSpaceBlock);
}

void NameSpace::EndBlock(void)
{
  if (Number() > 0)
  {
    wxNode *node = Last();
    NameSpaceBlock *block = (NameSpaceBlock *)node->Data();
    delete block;
    delete node;
  }
}

void NameSpace::ClearNameSpace(void)
{
  wxNode *node = First();
  while (node)
  {
    NameSpaceBlock *block = (NameSpaceBlock *)node->Data();
    delete block;
    delete node;
    node = First();
  }
}

char *NameSpace::MakeVariable(char *root)
{
  long nameId = 1;
  static char buffer[100];
  
  if (Number() > 0)
  {
    wxNode *node = Last();
    NameSpaceBlock *block = (NameSpaceBlock *)node->Data();
    NameSpaceVar *var = NULL;
    wxNode *varNode = block->Find(root);
    if (!varNode)
    {
      var = new NameSpaceVar(root);
      block->Append(root, var);
    }
    else var = (NameSpaceVar *)varNode->Data();
    nameId = var->id;
    var->id ++;
  }
  else
  {
    nameId = globalId;
    globalId ++;
  }
  sprintf(buffer, "%s%ld", root, nameId);
  return buffer;
}

