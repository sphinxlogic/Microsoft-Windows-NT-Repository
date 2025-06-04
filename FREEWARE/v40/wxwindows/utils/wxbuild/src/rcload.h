/*
 * File:	rcload.h
 * Purpose:	wxWindows GUI builder -- RC file loading/translation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#ifdef wx_msw

#ifndef rcloadh
#define rcloadh

#include "bframe.h"
#include "bsubwin.h"
#include "rcparser.h"
#include "rcobject.h"

class BuildFrameData;

void ShowRCLoader(void);
void CloseRCLoader(void);
Bool ConvertMenuFromRC(BuildFrameData *frameData);
BuildDialogBoxData *ConvertDialogFromRC(void);

extern rcFile *TheRCFile;

class RCLoaderDialog: public wxDialogBox
{
 public:
  wxListBox *dialogList;
  wxListBox *menuList;
  wxText *includeItem;
  
  RCLoaderDialog(wxFrame *parent, char *title, Bool modal,
   int x, int y, int w, int h);
  void UpdateLists(void);
  Bool OnClose(void);
};

void RCLoaderQuit(wxButton& but, wxCommandEvent& event);
void RCLoaderHelp(wxButton& but, wxCommandEvent& event);
void RCLoaderLoad(wxButton& but, wxCommandEvent& event);
void RCLoaderAdd(wxButton& but, wxCommandEvent& event);
void RCLoaderDialogList(wxListBox& lbox, wxCommandEvent& event);
void RCLoaderMenuList(wxListBox& lbox, wxCommandEvent& event);

#endif // rcloadh

#endif // wx_msw
