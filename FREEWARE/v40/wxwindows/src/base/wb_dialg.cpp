/*
 * File:	wb_dialg.cc
 * Purpose:	wxDialogBox and miscellaneous dialog functions
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wb_dialg.cc,v 1.3 1994/08/14 21:34:01 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "@(#)wb_dialg.cc	1.2 5/9/94"; */

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_defs.h"
#include "wx_setup.h"
#include "wx_dialg.h"
#include "wx_utils.h"
#include "wx_frame.h"
#include "wx_panel.h"
#include "wx_item.h"
#include "wx_buttn.h"
#include "wx_messg.h"
#include "wx_choic.h"
#include "wx_check.h"
#include "wx_menu.h"
#include "wx_txt.h"
#include "wx_mtxt.h"
#include "wx_slidr.h"
#include "wx_lbox.h"
#include "wx_mgstr.h"
#endif

#include <stdio.h>

// Dialog box - like panel but doesn't need a frame, and is modal or
// non-modal

wxbDialogBox::wxbDialogBox(void)
{
  WXSET_TYPE(wxDialogBox, wxTYPE_DIALOG_BOX)
  
  modal = FALSE;
}

wxbDialogBox::wxbDialogBox(wxWindow *WXUNUSED(Parent), Const char *WXUNUSED(Title), Bool Modal, 
                         int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height),
                         long style, Constdata char *WXUNUSED(name)):
  wxPanel()
{
  WXSET_TYPE(wxDialogBox, wxTYPE_DIALOG_BOX)
  
  windowStyle = style;
  modal = Modal;
}

Bool wxbDialogBox::Create(wxWindow *Parent, Const char *WXUNUSED(Title), Bool Modal, 
                         int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height),
                         long style, Constdata char *WXUNUSED(name))
{
  windowStyle = style;
  modal = Modal;
#if !WXGARBAGE_COLLECTION_ON
  if (!Parent)
    wxTopLevelWindows.Append(this);
#endif
  return TRUE;
}

wxbDialogBox::~wxbDialogBox()
{
  wxTopLevelWindows.DeleteObject(this);
}

void wxbDialogBox::Centre(int direction)
{
  int x_offset,y_offset ;
  int display_width, display_height;
  int  width, height, x, y;
  wxFrame *frame ;
  if (direction & wxCENTER_FRAME)
  {
    frame = (wxFrame*)GetParent() ;
    if (frame)
    {
      frame->GetPosition(&x_offset,&y_offset) ;
      frame->GetSize(&display_width,&display_height) ;
    }
  }
  else
    frame = NULL ;

  if (frame==NULL)
  {
    wxDisplaySize(&display_width, &display_height);
    x_offset = 0 ;
    y_offset = 0 ;
  }


  GetSize(&width, &height);
  GetPosition(&x, &y);

  if (direction & wxHORIZONTAL)
    x = (int)((display_width - width)/2);
  if (direction & wxVERTICAL)
    y = (int)((display_height - height)/2);

  SetSize(x+x_offset, y+y_offset, width, height);
}


