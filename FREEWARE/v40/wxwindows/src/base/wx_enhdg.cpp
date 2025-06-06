/*
 * File:	wx_enhdg.cc
 * Purpose:	wxEnhancedDialogBox
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_enhdg.cc,v 1.3 1994/08/14 21:34:01 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

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
#endif

#if USE_ENHANCED_DIALOG

#ifndef WX_PRECOMP
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
#include "wx_rbox.h"
#endif

#include "wx_mgstr.h"
#include "wx_enhdg.h"

#include <stdio.h>

#ifdef wx_xview
#include <xview/frame.h>
  // Under XView, font setting doesn't works. 2 possibles reasons:
  // 1) I make a mistake
  // 2) XView doesn't allow font setting
  // Until clear, no font changes for XView!!
#undef ENHANCED_FONTS
#define ENHANCED_FONTS 0
#endif

// Override ENHANCED_FONTS if requested
#ifdef WINDOWS_STANDARD
#undef ENHANCED_FONTS
#define ENHANCED_FONTS 0
#endif

#ifdef WINDOWS_BUTTON_ONLY
#undef ENHANCED_FONTS
#define ENHANCED_FONTS 0
#endif

//
//	Enhanced Dialog Box
//
#if !defined(wx_xview)
static        wxFont  *pin_font ;
#endif
static        wxFont  *label_font ;
static        wxFont  *button_font ;
static        wxFont  *cmd_font ;

#define	DO_UNSHOW	0
#define	DO_NOT_UNSHOW	1

void UnpinEnhDialog(wxCheckBox &pin,wxCommandEvent WXUNUSED(evt))
{
  if (pin.GetValue())
    return ;
  long clientData = (long)pin.GetClientData() ;
  if (clientData==DO_NOT_UNSHOW)
    return ;
  wxPanel *pinpanel = (wxPanel*)pin.GetParent();
  wxEnhDialogBox *dialog = (wxEnhDialogBox*)pinpanel->GetParent();
  dialog->Show(FALSE) ;
}

void CancelEnhDialog(wxItem &button,wxCommandEvent WXUNUSED(evt))
{
  wxPanel *cmdpanel = (wxPanel*)button.GetParent();
  wxEnhDialogBox *dialog = (wxEnhDialogBox*)cmdpanel->GetParent();
  dialog->pinned = FALSE ;
  dialog->Show(FALSE) ;
}

void wxEnhDialogCreateFonts()
{
  // Put your own defaults fonts here!

  // "Normal" font setting. Works for most applications.
#if ENHANCED_FONTS
  if (label_font==NULL)
    label_font = new wxFont(12,wxSWISS,wxNORMAL,wxBOLD) ;
  if (button_font==NULL)
    button_font = new wxFont(12,wxSWISS,wxNORMAL,wxNORMAL) ;
#if !defined(wx_xview)
  if (pin_font==NULL)
    pin_font = new wxFont(14,wxROMAN,wxITALIC,wxBOLD) ;
#endif
#ifdef wx_x
  if (cmd_font==NULL)
    cmd_font = new wxFont(17,wxROMAN,wxNORMAL,wxNORMAL) ;
#else
  if (cmd_font==NULL)
    cmd_font = new wxFont(14,wxROMAN,wxNORMAL,wxNORMAL) ;
#endif
#endif

  // Strict Windows standard: all with the same bold font...
#ifdef WINDOWS_STANDARD
  if (label_font==NULL)
    label_font = new wxFont(6,wxDEFAULT,wxNORMAL,wxBOLD) ;
  if (button_font==NULL)
    button_font = new wxFont(6,wxDEFAULT,wxNORMAL,wxBOLD) ;
#if !defined(wx_xview)
  if (pin_font==NULL)
    pin_font = new wxFont(6,wxDEFAULT,wxNORMAL,wxBOLD) ;
#endif
  if (cmd_font==NULL)
    cmd_font = new wxFont(6,wxDEFAULT,wxNORMAL,wxBOLD) ;
#endif

  // A compromise for Windows: only command buttons are standard.
#ifdef WINDOWS_BUTTON_ONLY
  if (label_font==NULL)
    label_font = new wxFont(12,wxSWISS,wxNORMAL,wxBOLD) ;
  if (button_font==NULL)
    button_font = new wxFont(12,wxSWISS,wxNORMAL,wxNORMAL) ;
#if !defined(wx_xview)
  if (pin_font==NULL)
    pin_font = new wxFont(14,wxROMAN,wxITALIC,wxBOLD) ;
#endif
  if (cmd_font==NULL)
    cmd_font = new wxFont(12,wxDEFAULT,wxNORMAL,wxBOLD) ;
#endif

}

wxEnhDialogBox::wxEnhDialogBox(void)
{
  WXSET_TYPE(wxEnhDialogBox, wxTYPE_ENHANCED_DIALOG)
  
  panel = NULL;
  windowStyle = 0 ;
  maxWidth = 0 ;
  maxHeight = 0 ;

#if !defined(wx_xview)
  pinPanel = 0 ;
#endif
  pinCheck = 0 ;
  unshow = NULL ;
  secondaryPanel = 0 ;
  cmdPanel = 0 ;
  statusPanel = 0 ;
  statusText = 0 ;
  userSpacing = 0 ;
  enhStyle = 0;
}

wxEnhDialogBox::wxEnhDialogBox(wxFrame *Parent, Const char *title, Bool Modal, 
                         wxFunction fun,int space,
                         int x, int y, int width, int height,
			 long wStyle, long Style, Constdata char *name)
{
   Create(Parent, title, Modal, fun, space, x, y, width, height, wStyle, Style, name);
}

Bool wxEnhDialogBox::Create(wxFrame *Parent, Const char *title, Bool Modal, 
                         wxFunction fun,int space,
                         int x, int y, int width, int height,
			 long wStyle, long Style, Constdata char *name)
{
  wxDialogBox::Create(Parent,title,Modal,x,y,width,height,wStyle,name) ;
  
  WXSET_TYPE(wxEnhDialogBox, wxTYPE_ENHANCED_DIALOG)
  
  wxEnhDialogCreateFonts() ;
  panel = this ;
  panel->SetVerticalSpacing(0) ;
  panel->SetHorizontalSpacing(0) ;
  panel->SetLabelFont(label_font) ;
  panel->SetButtonFont(button_font) ;
  windowStyle = wStyle ;
  enhStyle = Style;
  maxWidth = 0 ;
  maxHeight = 0 ;

#if !defined(wx_xview)
  long cancel = enhStyle&wxMASK_CANCEL ;
  if (cancel==wxNO_CANCEL_BUTTON)
  {
    pinPanel = new wxPanel(panel) ;
    pinPanel->SetVerticalSpacing(0) ;
    pinPanel->SetButtonFont(pin_font) ;
#ifdef wx_motif
    // Motif provide enough spacing...
    pinPanel->SetHorizontalSpacing(0) ;
#endif
    pinCheck = new wxCheckBox(pinPanel,(wxFunction)UnpinEnhDialog,"Pinned",
                              -1,-1,100,20,0,"pin") ;
    pinCheck->SetClientData((char*)DO_UNSHOW) ;
    pinPanel->Fit() ;
    panel->NewLine() ;
  }
  else
  {
    pinPanel = 0 ;
    pinCheck = 0 ;
  }
#endif

  unshow = fun ;
  userPanel = new wxPanel(panel,-1,-1,-1,-1,0,"user") ;
  secondaryPanel = 0 ;
  cmdPanel = 0 ;
  statusPanel = 0 ;
  statusText = 0 ;
  SetPin(FALSE) ;
  userSpacing = space ;

  return TRUE;
}

wxEnhDialogBox::~wxEnhDialogBox(void)
{
}

wxButton* wxEnhDialogBox::AddCmd(char *label,wxFunction fun,int tag)
{
Bool first_button ;
wxButton *btn ;
#ifndef wx_xview
wxButton *cancel ;
#endif
int	ww,hh;

  if (cmdPanel==0)
  {
    first_button = TRUE ;
    userPanel->Fit() ;
    if ((enhStyle&wxMASK_COMMANDS)==wxBOTTOM_COMMANDS)
      panel->NewLine() ;
    cmdPanel = new wxPanel(panel) ;
    cmdPanel->SetButtonFont(cmd_font) ;
    if ((enhStyle&wxMASK_COMMANDS)==wxBOTTOM_COMMANDS)
    {
      cmdPanel->SetVerticalSpacing(0) ;
      if (userSpacing>-1)
        cmdPanel->SetHorizontalSpacing(userSpacing) ;
    }
    else
    {
      //cmdPanel->SetHorizontalSpacing(0) ;
      if (userSpacing>-1)
        cmdPanel->SetVerticalSpacing(userSpacing) ;
    }
#ifndef wx_xview
    if ((enhStyle&wxMASK_CANCEL)==wxCANCEL_BUTTON_FIRST)
    {
      cancel = new wxButton(cmdPanel,(wxFunction)CancelEnhDialog, wxSTR_BUTTON_CANCEL) ;
      if (cancel)
      {
        cancel->GetSize(&ww,&hh) ;
        if (ww>maxWidth) maxWidth = ww ;
        if (hh>maxHeight) maxHeight = hh ;
      }
      if ((enhStyle&wxMASK_COMMANDS)==wxRIGHT_COMMANDS)
        cmdPanel->NewLine() ;
    }
#endif
  }
  else
    first_button = FALSE ;

  btn = new wxButton(cmdPanel,fun,label) ;
  if (btn)
  {
    btn->SetClientData((char*)tag) ;
    btn->GetSize(&ww,&hh) ;
    if (ww>maxWidth) maxWidth = ww ;
    if (hh>maxHeight) maxHeight = hh ;
  }
  if ((enhStyle&wxMASK_COMMANDS)==wxRIGHT_COMMANDS)
    cmdPanel->NewLine() ;
#ifndef wx_xview
    if (first_button)
      if ((enhStyle&wxMASK_CANCEL)==wxCANCEL_BUTTON_SECOND)
      {
        cancel = new wxButton(cmdPanel,(wxFunction)CancelEnhDialog, wxSTR_BUTTON_CANCEL) ;
        if (cancel)
        {
          cancel->GetSize(&ww,&hh) ;
          if (ww>maxWidth) maxWidth = ww ;
          if (hh>maxHeight) maxHeight = hh ;
        }
        if ((enhStyle&wxMASK_COMMANDS)==wxRIGHT_COMMANDS)
          cmdPanel->NewLine() ;
      }
#endif
  return(btn) ;
}

wxButton* wxEnhDialogBox::AddCmd(wxBitmap *bitmap,wxFunction fun,int tag)
{
Bool first_button ;
wxButton *btn;
#ifndef wx_xview
wxButton *cancel ;
#endif
int ww,hh ;

  if (cmdPanel==0)
  {
    first_button = TRUE ;
    userPanel->Fit() ;
    if ((enhStyle&wxMASK_COMMANDS)==wxBOTTOM_COMMANDS)
      panel->NewLine() ;
    cmdPanel = new wxPanel(panel) ;
    cmdPanel->SetButtonFont(cmd_font) ;
    if ((enhStyle&wxMASK_COMMANDS)==wxBOTTOM_COMMANDS)
    {
      cmdPanel->SetVerticalSpacing(0) ;
      if (userSpacing>-1)
        cmdPanel->SetHorizontalSpacing(userSpacing) ;
    }
    else
    {
      //cmdPanel->SetHorizontalSpacing(0) ;
      if (userSpacing>-1)
        cmdPanel->SetVerticalSpacing(userSpacing) ;
    }
#ifndef wx_xview
    if ((enhStyle&wxMASK_CANCEL)==wxCANCEL_BUTTON_FIRST)
    {
      cancel = new wxButton(cmdPanel,(wxFunction)CancelEnhDialog, wxSTR_BUTTON_CANCEL) ;
      if (cancel)
      {
        cancel->GetSize(&ww,&hh) ;
        if (ww>maxWidth) maxWidth = ww ;
        if (hh>maxHeight) maxHeight = hh ;
      }
      if ((enhStyle&wxMASK_COMMANDS)==wxRIGHT_COMMANDS)
        cmdPanel->NewLine() ;
    }
#endif
  }
  else
    first_button = FALSE ;

  btn = new wxButton(cmdPanel,fun,bitmap) ;
  if (btn)
  {
    btn->SetClientData((char*)tag) ;
    btn->GetSize(&ww,&hh) ;
    if (ww>maxWidth) maxWidth = ww ;
    if (hh>maxHeight) maxHeight = hh ;
  }
  if ((enhStyle&wxMASK_COMMANDS)==wxRIGHT_COMMANDS)
    cmdPanel->NewLine() ;
#ifndef wx_xview
    if (first_button)
      if ((enhStyle&wxMASK_CANCEL)==wxCANCEL_BUTTON_SECOND)
      {
        cancel = new wxButton(cmdPanel,(wxFunction)CancelEnhDialog, wxSTR_BUTTON_CANCEL) ;
        if (cancel)
        {
          cancel->GetSize(&ww,&hh) ;
          if (ww>maxWidth) maxWidth = ww ;
          if (hh>maxHeight) maxHeight = hh ;
        }
        if ((enhStyle&wxMASK_COMMANDS)==wxRIGHT_COMMANDS)
          cmdPanel->NewLine() ;
      }
#endif
  return(btn) ;
}

wxButton* wxEnhDialogBox::GetCmd(int number)
{
  if (cmdPanel)
    return ((wxButton*)(cmdPanel->GetChild(number)));
  else
    return(NULL) ;
}

Bool wxEnhDialogBox::Show(Bool show,Bool flag)
{
  if (!show)
  {
#ifndef wx_xview
    if (pinCheck)
    {
      if (pinCheck->GetValue())
        return TRUE;
    }
    else
    {
      if (pinned)
        return TRUE;
    }
#else
    wxFrame *frame = (wxFrame*)GetParent() ;
    Frame x_frame = (Frame)(frame->GetHandle());
    if (xv_get(x_frame,FRAME_CMD_PUSHPIN_IN))
      return TRUE;
#endif
    if (unshow)
    {
    wxCommandEvent evt = 0;
      (void)(*(unshow))(*this,evt) ;
    }
  }
  if (show)
  {
    SetPin(flag) ;
    SetStatus("") ; // reset previous status text
  }
  return wxDialogBox::Show(show) ;
}

void wxEnhDialogBox::SetPin(Bool flag)
{
#ifdef wx_xview
  wxFrame *frame = (wxFrame*)GetParent() ;
  Frame x_frame = (Frame)(frame->GetHandle());
  (void)xv_set(x_frame,FRAME_CMD_PUSHPIN_IN,flag) ;
#else
  if (pinCheck)
  {
    pinCheck->SetClientData((char*)DO_NOT_UNSHOW) ;
    if (flag)
    {
      if (pinCheck->GetValue())
      {
        pinCheck->SetClientData((char*)DO_UNSHOW) ;
        return ;
      }
    }
    else
    {
      if (!pinCheck->GetValue())
      {
        pinCheck->SetClientData((char*)DO_UNSHOW) ;
        return ;
      }
    }
    // Change state only if needed
    pinCheck->SetValue(flag) ;
  pinCheck->SetClientData((char*)DO_UNSHOW) ;
  }
  else
    pinned = flag ;
#endif
}

void wxEnhDialogBox::SetStatus(char *label)
{
  if (statusText==NULL)
    return ;
  if (label==0 || *label=='\0')
    label = "   " ;
  statusText->SetLabel(label) ;
}

void wxEnhDialogBox::PrimaryFit(void)
{
long cancel = enhStyle&wxMASK_CANCEL ;
wxButton *btn ;
int ww,hh ;

  if (cancel!=wxNO_CANCEL_BUTTON)
  {
    if (cmdPanel==0||cancel==wxCANCEL_BUTTON_LAST)
    {
      if (cmdPanel==0)
      {
        userPanel->Fit() ;
        if ((enhStyle&wxMASK_COMMANDS)==wxBOTTOM_COMMANDS)
          panel->NewLine() ;
        cmdPanel = new wxPanel(panel) ;
        cmdPanel->SetButtonFont(cmd_font) ;
        if ((enhStyle&wxMASK_COMMANDS)==wxBOTTOM_COMMANDS)
        {
          cmdPanel->SetVerticalSpacing(0) ;
          if (userSpacing>-1)
            cmdPanel->SetHorizontalSpacing(userSpacing) ;
        }
        else
        {
          //cmdPanel->SetHorizontalSpacing(0) ;
          if (userSpacing>-1)
            cmdPanel->SetVerticalSpacing(userSpacing) ;
        }
      }
      btn = new wxButton(cmdPanel,(wxFunction)CancelEnhDialog, wxSTR_BUTTON_CANCEL) ;
      if (btn)
      {
        btn->GetSize(&ww,&hh) ;
        if (ww>maxWidth) maxWidth = ww ;
        if (hh>maxHeight) maxHeight = hh ;
      }
      if ((enhStyle&wxMASK_COMMANDS)==wxRIGHT_COMMANDS)
        cmdPanel->NewLine() ;
    }
  }

  
  if ((enhStyle&wxMASK_COMMANDS)==wxRIGHT_COMMANDS)
  {
    int number = 0 ;
    while ((btn=GetCmd(number++)))
      btn->SetSize(-1,-1,maxWidth,maxHeight) ;
  }

  if (cmdPanel)
    cmdPanel->Fit() ;
}

void wxEnhDialogBox::PreFit(void)
{
  PrimaryFit() ;
  panel->NewLine() ;
  secondaryPanel = new wxPanel(panel,-1,-1,-1,-1,0,"secondUser") ;
}

void wxEnhDialogBox::Fit(void)
{

  if (secondaryPanel==NULL)
  {
    PrimaryFit() ;
  }
  else
  {
    secondaryPanel->Fit() ;
  }

  panel->NewLine() ;
  if ((enhStyle&wxMASK_STATUS)==wxSTATUS_FOOTER)
  {
    statusPanel = new wxPanel(panel) ;
    statusPanel->SetVerticalSpacing(0) ;
#ifdef wx_motif
    // Motif provide enough spacing...
    statusPanel->SetHorizontalSpacing(0) ;
#endif
    statusText = new wxMessage(statusPanel,"   ") ;
    statusPanel->Fit() ;
    panel->NewLine() ;
  }
  wxDialogBox::Fit() ;
  if (cmdPanel && (enhStyle&wxMASK_COMMANDS)==wxBOTTOM_COMMANDS)
    cmdPanel->Centre(wxHORIZONTAL) ;
  if (statusPanel)
  {
    int fw,fh ;
    GetClientSize(&fw,&fh) ;
    statusPanel->SetSize(-1,-1,fw,-1) ;
    statusText->SetSize(-1,-1,fw,-1) ;
  }
}
#endif
