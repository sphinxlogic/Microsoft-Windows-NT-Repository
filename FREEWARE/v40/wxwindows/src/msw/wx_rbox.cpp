/*
 * File:	wx_rbox.cc
 * Purpose:	Radio box item implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	April 1995
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx_panel.h"
#include "wx_rbox.h"
#endif

#include "wx_itemp.h"

BOOL wxRadioBox::MSWCommand(UINT param, WORD id)
{
  if (param == BN_CLICKED)
  {
#ifdef WIN32
    int i;
    for (i = 0; i < no_items; i++)
      if (id == GetWindowLong(radioButtons[i], GWL_ID))
        selected = i;
#else
    int i;
    for (i = 0; i < no_items; i++)
      if (id == GetWindowWord(radioButtons[i], GWW_ID))
        selected = i;
#endif

    wxCommandEvent event(wxEVENT_TYPE_RADIOBOX_COMMAND);
    event.commandInt = selected;
    event.eventObject = this;
    ProcessCommand(event);
    return TRUE;
  }
  else return FALSE;
}

// Radio box item
wxRadioBox::wxRadioBox(void)
{
  wxWinType = wxTYPE_HWND;
  windows_id = 0;
  no_items = 0;
  ms_handle = 0;
  radioButtons = NULL;
  majorDim = 0 ;
  selected = -1;
  radioWidth = NULL ;
  radioHeight = NULL ;
  isFafa = RADIO_IS_FAFA ;
}

wxRadioBox::wxRadioBox(wxPanel *panel, wxFunction func,
                       Const char *Title,
                       int x, int y, int width, int height,
                       int N, char **Choices,
                       int MajorDim,long style, Constdata char *name):
  wxbRadioBox(panel, func, Title, x, y, width, height, N, Choices,
              MajorDim, style, name)
{
  Create(panel, func, Title, x, y, width, height, N, Choices, MajorDim, style, name);
}

wxRadioBox::wxRadioBox(wxPanel *panel, wxFunction func,
                       Const char *Title,
                       int x, int y, int width, int height,
                       int N, wxBitmap **Choices,
                       int MajorDim,long style, Constdata char *name):
  wxbRadioBox(panel, func, Title, x, y, width, height, N, Choices,
              MajorDim, style, name)
{
  Create(panel, func, Title, x, y, width, height, N, Choices, MajorDim, style, name);
}

Bool wxRadioBox::Create(wxPanel *panel, wxFunction func,
                       Const char *Title,
                       int x, int y, int width, int height,
                       int N, char **Choices,
                       int MajorDim, long style, Constdata char *name)
{
  SetName(name);
  if (panel) panel->AddChild(this);
  buttonFont = panel->buttonFont ;
  labelFont = panel->labelFont ;
  if (buttonFont)
    buttonFont->UseResource();
  if (labelFont)
    labelFont->UseResource();
  SetBackgroundColour(*panel->GetBackgroundColour()) ;
  labelColour = panel->labelColour ;
  buttonColour = panel->buttonColour ;
  noRowsOrCols = MajorDim;
  if (MajorDim==0)
    MajorDim = N ;
  majorDim = MajorDim ;
  windowStyle = style ;
  selected = -1;
  ms_handle = 0;
  isFafa = RADIO_IS_FAFA ;
  wxWinType = wxTYPE_HWND;

  SetAppropriateLabelPosition();
  panel->GetValidPosition(&x, &y);

  char *the_label = NULL ;

  if (Title)
  {
    the_label = new char[strlen(Title)+1] ;
    if (style&wxFIXED_LENGTH)
    {
      int i;
      for (i=0;i<(int)strlen(Title);i++)
        the_label[i]=MEANING_CHARACTER ;
    }
    else
      strcpy(the_label,Title) ;
    the_label[strlen(Title)] = '\0' ;
  }
  else the_label=copystring("") ;

  HWND the_handle ;

  long msStyle = GROUP_FLAGS;
#if WIN95
//  msStyle |= WS_EX_CLIENTEDGE;
#endif

  ms_handle = CreateWindow(GROUP_CLASS,Title,
                           msStyle,
                           0,0,0,0,
                           panel->GetHWND(),(HMENU)NewControlId(),wxhInstance,NULL) ;

  the_handle = panel->GetHWND() ;

#if CTL3D
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
    Ctl3dSubclassCtl(ms_handle);
#endif
  if (labelFont)
  {
    labelFont->RealizeResource();
    if (labelFont->GetResourceHandle())
      SendMessage(ms_handle,WM_SETFONT,
                (WPARAM)labelFont->GetResourceHandle(),0L);
  }

  // Subclass again for purposes of dialog editing mode
  SubclassControl((HWND)ms_handle);

  // Some radio boxes test consecutive id.
  (void)NewControlId() ;
  radioButtons = new HWND[N];
  radioWidth = new int[N] ;
  radioHeight = new int[N] ;
  int i;
  for (i = 0; i < N; i++)
  {
    radioWidth[i] = radioHeight[i] = -1 ;
    long groupStyle = 0;
    if (i == 0 && style==0)
      groupStyle = WS_GROUP;
    long newId = NewControlId();
    long msStyle = groupStyle | RADIO_FLAGS;
#if WIN95
//    msStyle |= WS_EX_CLIENTEDGE;
#endif
    radioButtons[i] = CreateWindowEx(0, RADIO_CLASS, Choices[i], 
                          msStyle,0,0,0,0,
                          the_handle, (HMENU)newId, wxhInstance, NULL);
#if CTL3D
    if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
      Ctl3dSubclassCtl(radioButtons[i]);
#endif
    if (buttonFont)
    {
      buttonFont->RealizeResource();
      if (buttonFont->GetResourceHandle())
        SendMessage((HWND)radioButtons[i],WM_SETFONT,
                    (WPARAM)buttonFont->GetResourceHandle(),0L);
    }
    subControls.Append((wxObject *)newId);
  }

  // Create a dummy radio control to end the group.
  (void)CreateWindowEx(0, RADIO_CLASS, "", WS_GROUP|RADIO_FLAGS, 0,0,0,0, the_handle, (HMENU)NewControlId(), wxhInstance, NULL);

  no_items = N;
  SetSelection(0);

  SetSize(x, y, width, height);
  panel->AdvanceCursor(this);
  Callback(func);

  if (Title)
  {
    if (style&wxFIXED_LENGTH)
      SetLabel((char *)Title) ;
  }
  if (the_label)
    delete [] the_label ;

  return TRUE;
}

Bool wxRadioBox::Create(wxPanel *panel, wxFunction func,
                       Const char *Title,
                       int x, int y, int width, int height,
                       int N, wxBitmap **Choices,
                       int MajorDim, long style, Constdata char *name)
{
  SetName(name);
  if (panel) panel->AddChild(this);
  buttonFont = panel->buttonFont ;
  labelFont = panel->labelFont ;
  if (buttonFont)
    buttonFont->UseResource();
  if (labelFont)
    labelFont->UseResource();
  SetBackgroundColour(*panel->GetBackgroundColour()) ;
  labelColour = panel->labelColour ;
  buttonColour = panel->buttonColour ;
  noRowsOrCols = MajorDim;
  if (MajorDim==0)
    MajorDim = N ;
  majorDim = MajorDim ;
  windowStyle = style ;
  selected = -1;
  ms_handle = 0;
  wxWinType = wxTYPE_HWND;

  panel->GetValidPosition(&x, &y);

  char *the_label = NULL ;

  if (Title)
  {
    the_label = new char[strlen(Title)+1] ;
    if (style&wxFIXED_LENGTH)
    {
      int i;
      for (i=0;i<(int)strlen(Title);i++)
        the_label[i]=MEANING_CHARACTER ;
    }
    else
      strcpy(the_label,Title) ;
    the_label[strlen(Title)] = '\0' ;
  }
  else
    the_label = copystring("") ;

  HWND the_handle ;

  long msStyle = GROUP_FLAGS;
#if WIN95
//  msStyle |= WS_EX_CLIENTEDGE;
#endif
  ms_handle = CreateWindow(GROUP_CLASS,the_label,
                           msStyle,
                           0,0,0,0,
                           panel->GetHWND(),(HMENU)NewControlId(),wxhInstance,NULL) ;

  if (labelFont)
  {
    labelFont->RealizeResource();
    if (labelFont->GetResourceHandle())
      SendMessage(ms_handle,WM_SETFONT,
                (WPARAM)labelFont->GetResourceHandle(),0L);
  }

  the_handle = panel->GetHWND();

  // Subclass again for purposes of dialog editing mode
  SubclassControl((HWND)ms_handle);

  (void)NewControlId() ;
  radioButtons = new HWND[N];
  radioWidth = new int[N] ;
  radioHeight = new int[N] ;
#if FAFA_LIB // && !CTL3D
  isFafa = TRUE ; //always !!
#else
  isFafa = RADIO_IS_FAFA ;
#endif
  int i;
  for (i = 0; i < N; i++)
  {
    long groupStyle = 0;
    if (i == 0 && style==0)
      groupStyle = WS_GROUP;
    long newId = NewControlId();
#if FAFA_LIB // && !CTL3D
    radioWidth[i]  = Choices[i]->GetWidth()  + FB_MARGIN ;
    radioHeight[i] = Choices[i]->GetHeight() + FB_MARGIN ;
#else
    radioWidth[i]  = Choices[i]->GetWidth();
    radioHeight[i] = Choices[i]->GetHeight();
#endif
    char tmp[32] ;
    sprintf(tmp,"Toggle%d",i) ;
#if FAFA_LIB // && !CTL3D
    long msStyle = groupStyle | BITRADIO_FLAGS;
#if WIN95
//    msStyle |= WS_EX_CLIENTEDGE;
#endif
    radioButtons[i] = CreateWindowEx(0, FafaChck, tmp,
                          msStyle,0,0,0,0,
                          the_handle, (HMENU)newId, wxhInstance, NULL);
    SendMessage((HWND)radioButtons[i],WM_CHANGEBITMAP,
                  (WPARAM)((Choices[i]->GetHeight()<<8)+Choices[i]->GetWidth()),
                  (LPARAM)Choices[i]->ms_bitmap);

#else
    long msStyle = groupStyle | RADIO_FLAGS;
#if WIN95
//    msStyle |= WS_EX_CLIENTEDGE;
#endif
    radioButtons[i] = CreateWindowEx(0, RADIO_CLASS, tmp,
                          msStyle,0,0,0,0,
                          the_handle, (HMENU)newId, wxhInstance, NULL);
#if CTL3D
    if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
      Ctl3dSubclassCtl(radioButtons[i]);
#endif
#endif
    if (buttonFont)
    {
      buttonFont->RealizeResource();
      if (buttonFont->GetResourceHandle())
        SendMessage((HWND)radioButtons[i],WM_SETFONT,
                    (WPARAM)buttonFont->GetResourceHandle(),0L);
    }
    subControls.Append((wxObject *)newId);
  }
  // Create a dummy radio control to end the group.
  (void)CreateWindowEx(0, RADIO_CLASS, "", WS_GROUP|RADIO_FLAGS, 0,0,0,0, the_handle, (HMENU)NewControlId(), wxhInstance, NULL);

  no_items = N;
  SetSelection(0);

  SetSize(x, y, width, height);
  panel->AdvanceCursor(this);
  Callback(func);

  if (Title)
  {
    if (style&wxFIXED_LENGTH)
      SetLabel((char *)Title) ;
  }
  if (the_label)
    delete [] the_label ;

  return TRUE;
}

wxRadioBox::~wxRadioBox(void)
{
  isBeingDeleted = TRUE;
  
  if (radioButtons)
  {
    int i;
    for (i = 0; i < no_items; i++)
      DestroyWindow(radioButtons[i]);
    delete[] radioButtons;
  }
  if (radioWidth)
    delete[] radioWidth ;
  if (radioHeight)
    delete[] radioHeight ;
  if (ms_handle)
    DestroyWindow(ms_handle) ;
  ms_handle = NULL ;

}

char *wxRadioBox::GetLabel(int item)
{
  GetWindowText((HWND)radioButtons[item], wxBuffer, 300);
  return wxBuffer;
}

void wxRadioBox::SetLabel(int item,char *label)
{
#if FAFA_LIB // && !CTL3D
    // This message will switch from FB_BITMAP style to FB_TEXT, if needed.
    SendMessage((HWND)radioButtons[item],WM_CHANGEBITMAP,
                (WPARAM)0,
                (LPARAM)NULL);
#endif
  radioWidth[item] = radioHeight[item] = -1 ;
  SetWindowText((HWND)radioButtons[item], label);
}

void wxRadioBox::SetLabel(int item,wxBitmap *bitmap)
{
#if FAFA_LIB // && !CTL3D
    SendMessage((HWND)radioButtons[item],WM_CHANGEBITMAP,
                (WPARAM)((bitmap->GetHeight()<<8)+bitmap->GetWidth()),
                (LPARAM)bitmap->ms_bitmap);
    radioWidth[item] = bitmap->GetWidth() + FB_MARGIN ;
    radioHeight[item] = bitmap->GetHeight() + FB_MARGIN ;
#endif
}

int wxRadioBox::FindString(char *s)
{
 int i;
 for (i = 0; i < no_items; i++)
 {
  GetWindowText(radioButtons[i], wxBuffer, 1000);
  if (strcmp(wxBuffer, s) == 0)
    return i;
 }
 return -1;
}

void wxRadioBox::SetSelection(int N)
{
  if ((N < 0) || (N >= no_items))
    return;

// Following necessary for Win32s, because Win32s translate BM_SETCHECK
#if FAFA_LIB
  if (selected >= 0 && selected < no_items)
    SendMessage(radioButtons[selected], isFafa?FAFA_SETCHECK:BM_SETCHECK, 0, 0L);
    
  SendMessage(radioButtons[N], isFafa?FAFA_SETCHECK:BM_SETCHECK, 1, 0L);
#else
  if (selected >= 0 && selected < no_items)
    SendMessage(radioButtons[selected], BM_SETCHECK, 0, 0L);
  
  SendMessage(radioButtons[N], BM_SETCHECK, 1, 0L);
#endif
  selected = N;
}

// Get single selection, for single choice list items
int wxRadioBox::GetSelection(void)
{
  return selected;
}

// Find string for position
char *wxRadioBox::GetString(int N)
{
  GetWindowText(radioButtons[N], wxBuffer, 1000);
  return wxBuffer;
}

void wxRadioBox::SetSize(int x, int y, int width, int height, int sizeFlags)
{
  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y = currentY;

  char buf[400];

  int y_offset = y;
  int x_offset = x;
  float current_width;

  float cyf;
  HWND wnd = (HWND)ms_handle;

  int cx1,cy1 ;
  wxGetCharSize(wnd, &cx1, &cy1,buttonFont);
  // Attempt to have a look coherent with other platforms:
  // We compute the biggest toggle dim, then we align all
  // items according this value.
  int maxWidth =  -1;
  int maxHeight = -1 ;

  int i;
  for (i = 0 ; i < no_items; i++)
  {
    int eachWidth;
    int eachHeight ;
    if (radioWidth[i]<0)
    {
      // It's a labelled toggle
      GetWindowText(radioButtons[i], buf, 300);
      GetTextExtent(buf, &current_width, &cyf,NULL,NULL, buttonFont);
      eachWidth = (int)(current_width + RADIO_SIZE);
      eachHeight = (int)((3*cyf)/2);
    }
    else
    {
      eachWidth = radioWidth[i] ;
      eachHeight = radioHeight[i] ;
    }
    if (maxWidth<eachWidth) maxWidth = eachWidth ;
    if (maxHeight<eachHeight) maxHeight = eachHeight ;
  }

  if (ms_handle)
  {
    int totWidth ;
    int totHeight;

    int nbHor,nbVer;

    if (windowStyle & wxVERTICAL)
    {
      nbVer = majorDim ;
      nbHor = (no_items+majorDim-1)/majorDim ;
    }
    else
    {
      nbHor = majorDim ;
      nbVer = (no_items+majorDim-1)/majorDim ;
    }

    // this formula works, but I don't know why.
    // Please, be sure what you do if you modify it!!
    if (radioWidth[0]<0)
      totHeight = (nbVer * maxHeight) + cy1/2 ;
    else
      totHeight = nbVer * (maxHeight+cy1/2) ;
    totWidth  = nbHor * (maxWidth+cx1) ;

#if (!CTL3D)
    // Requires a bigger group box in plain Windows
    MoveWindow(ms_handle,x_offset,y_offset,totWidth+cx1,totHeight+(3*cy1)/2,TRUE) ;
#else
    MoveWindow(ms_handle,x_offset,y_offset,totWidth+cx1,totHeight+cy1,TRUE) ;
#endif
    x_offset += cx1;
    y_offset += cy1;
  }

#if (!CTL3D)
  y_offset += (int)(cy1/2); // Fudge factor since buttons overlapped label
                            // JACS 2/12/93. CTL3D draws group label quite high.
#endif
  int startX = x_offset ;
  int startY = y_offset ;

  for ( i = 0 ; i < no_items; i++)
  {
    // Bidimensional radio adjustment
    if (i&&((i%majorDim)==0)) // Why is this omitted for i = 0?
    {
      if (windowStyle & wxVERTICAL)
      {
        y_offset = startY;
        x_offset += maxWidth + cx1 ;
      }
      else
      {
        x_offset = startX ;
        y_offset += maxHeight ;
        if (radioWidth[0]>0)
          y_offset += cy1/2 ;
      }
    }
    int eachWidth ;
    int eachHeight ;
    if (radioWidth[i]<0)
    {
      // It's a labeled item
      GetWindowText(radioButtons[i], buf, 300);
      GetTextExtent(buf, &current_width, &cyf,NULL,NULL,buttonFont);

      // How do we find out radio button bitmap size!!
      // By adjusting them carefully, manually :-)
      eachWidth = (int)(current_width + RADIO_SIZE);
      eachHeight = (int)((3*cyf)/2);
    }
    else
    {
      eachWidth = radioWidth[i] ;
      eachHeight = radioHeight[i] ;
    }

    MoveWindow(radioButtons[i],x_offset,y_offset,eachWidth,eachHeight,TRUE);
    if (windowStyle & wxVERTICAL)
    {
      y_offset += maxHeight;
      if (radioWidth[0]>0)
        y_offset += cy1/2 ;
    }
    else
      x_offset += maxWidth + cx1;
  }
  GetEventHandler()->OnSize(width, height);
}

void wxRadioBox::GetSize(int *width, int *height)
{
  RECT rect;
  rect.left = -1; rect.right = -1; rect.top = -1; rect.bottom = -1;

  if (ms_handle)
    wxFindMaxSize(ms_handle, &rect);

  int i;
  for (i = 0; i < no_items; i++)
    wxFindMaxSize(radioButtons[i], &rect);

  *width = rect.right - rect.left;
  *height = rect.bottom - rect.top;
}

void wxRadioBox::GetPosition(int *x, int *y)
{
  wxWindow *parent = GetParent();
  RECT rect;
  rect.left = -1; rect.right = -1; rect.top = -1; rect.bottom = -1;

  int i;
  for (i = 0; i < no_items; i++)
    wxFindMaxSize(radioButtons[i], &rect);

  if (ms_handle)
    wxFindMaxSize(ms_handle, &rect);

  // Since we now have the absolute screen coords,
  // if there's a parent we must subtract its top left corner
  POINT point;
  point.x = rect.left;
  point.y = rect.top;
  if (parent)
  {
    ::ScreenToClient(parent->GetHWND(), &point);
  }

  *x = point.x;
  *y = point.y;
}

char *wxRadioBox::GetLabel(void)
{
  if (ms_handle)
  {
    GetWindowText(ms_handle, wxBuffer, 300);
    return wxBuffer;
  }
  else return NULL;
}

void wxRadioBox::SetLabel(char *label)
{
  if (ms_handle && label)
    SetWindowText(ms_handle, label);
}

void wxRadioBox::SetFocus(void)
{
/*
  if (no_items > 0)
   ::SetFocus(radioButtons[0]);
*/
/* Begin Alberts Patch 26. 5. 1997*/
  if (no_items > 0)
  {
    if (selected == -1)
      ::SetFocus(radioButtons[0]);
    else
      ::SetFocus(radioButtons[selected]);
  }
/* Ende Alberts Patch*/
}

Bool wxRadioBox::Show(Bool show)
{
  int cshow;
  if (show)
    cshow = SW_SHOW;
  else
    cshow = SW_HIDE;
  if (ms_handle)
    ShowWindow(ms_handle, cshow);
  int i;
  for (i = 0; i < no_items; i++)
    ShowWindow(radioButtons[i], cshow);
  return TRUE;
}

// Enable a specific button
void wxRadioBox::Enable(int item, Bool enable)
{
  if (item<0)
    wxWindow::Enable(enable) ;
  else if (item < no_items)
    ::EnableWindow(radioButtons[item], enable);
}

// Enable all controls
void wxRadioBox::Enable(Bool enable)
{
  wxItem::Enable(enable);
  
  int i;
  for (i = 0; i < no_items; i++)
    ::EnableWindow(radioButtons[i], enable);
}

// Show a specific button
void wxRadioBox::Show(int item, Bool show)
{
  if (item<0)
    wxRadioBox::Show(show) ;
  else if (item < no_items)
  {
    int cshow;
    if (show)
      cshow = SW_SHOW;
    else
      cshow = SW_HIDE;
    ShowWindow(radioButtons[item], cshow);
  }
}

void wxRadioBox::SetLabelFont(wxFont *font)
{
  // Decrement the usage count of the old label font
  // (we may be able to free it up)
  if (labelFont)
    labelFont->ReleaseResource();

  labelFont = font;

  // Increment usage count
  if (font)
    font->UseResource();

  HWND hWnd = GetHWND();
  if (hWnd != 0)
  {
    if (font)
    {
      font->RealizeResource();
      
      if (font->GetResourceHandle())
        SendMessage(hWnd, WM_SETFONT,
                  (WPARAM)font->GetResourceHandle(),TRUE);
    }
  }
}

void wxRadioBox::SetButtonFont(wxFont *font)
{
  // Decrement the usage count of the old label font
  // (we may be able to free it up)
  if (buttonFont)
    buttonFont->ReleaseResource();
    
  buttonFont = font;
  if (font)
  {
    font->UseResource();
    font->RealizeResource();
    
    int i;
    for (i = 0; i < no_items; i++)
    {
      if (font->GetResourceHandle())
      {
        SendMessage(radioButtons[i], WM_SETFONT,
                  (WPARAM)font->GetResourceHandle(),TRUE);
      }
    }
  }
}

#if USE_RADIOBUTTON

// Radio button item
wxRadioButton::wxRadioButton(void)
{
  wxWinType = wxTYPE_HWND;
  windows_id = 0;
  ms_handle = 0;
  isFafa = RADIO_IS_FAFA ;
}

wxRadioButton::wxRadioButton(wxPanel *panel, wxFunction func,
                       Const char *Title, Bool value,
                       int x, int y, int width, int height,
                       long style, Constdata char *name):
  wxbRadioButton(panel, func, Title, value, x, y, width, height,
              style, name)
{
  Create(panel, func, Title, value, x, y, width, height, style, name);
}

wxRadioButton::wxRadioButton(wxPanel *panel, wxFunction func,
                       wxBitmap *bitmap, Bool value,
                       int x, int y, int width, int height,
                       long style, Constdata char *name):
  wxbRadioButton(panel, func, bitmap, value, x, y, width, height,
              style, name)
{
  Create(panel, func, bitmap, value, x, y, width, height, style, name);
}

Bool wxRadioButton::Create(wxPanel *panel, wxFunction func,
                       Const char *Title, Bool value,
                       int x, int y, int width, int height,
                       long style, Constdata char *name)
{
  SetName(name);
  if (panel) panel->AddChild(this);
  buttonFont = panel->buttonFont ;
  labelFont = panel->labelFont ;
  if (buttonFont)
    buttonFont->UseResource();
  if (labelFont)
    labelFont->UseResource();
  SetBackgroundColour(*panel->GetBackgroundColour()) ;
  labelColour = panel->labelColour ;
  buttonColour = panel->buttonColour ;
  windowStyle = style ;
  ms_handle = 0;
  isFafa = RADIO_IS_FAFA ;
  wxWinType = wxTYPE_HWND;

  SetAppropriateLabelPosition();
  panel->GetValidPosition(&x, &y);

  char *the_label = NULL ;

  if (Title)
  {
    the_label = new char[strlen(Title)+1] ;
    if (style&wxFIXED_LENGTH)
    {
      int i;
      for (i=0;i<(int)strlen(Title);i++)
        the_label[i]=MEANING_CHARACTER ;
    }
    else
      strcpy(the_label,Title) ;
    the_label[strlen(Title)] = '\0' ;
  }
  else the_label=copystring("") ;

  long groupStyle = 0;
  if (windowStyle & wxRB_GROUP)
    groupStyle = WS_GROUP;
  long newId = NewControlId();
  long msStyle = groupStyle | RADIO_FLAGS;
#if WIN95
//    msStyle |= WS_EX_CLIENTEDGE;
#endif
  ms_handle = CreateWindowEx(0, RADIO_CLASS, the_label, 
                          msStyle,0,0,0,0,
                          panel->GetHWND(), (HMENU)newId, wxhInstance, NULL);
#if CTL3D
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
    Ctl3dSubclassCtl(ms_handle);
#endif
  if (labelFont)
  {
    labelFont->RealizeResource();
    if (labelFont->GetResourceHandle())
      SendMessage(ms_handle,WM_SETFONT,
                  (WPARAM)labelFont->GetResourceHandle(),0L);
  }

  // Subclass again for purposes of dialog editing mode
  SubclassControl((HWND)ms_handle);

  SetValue(value);

  // start GRW fix
  if (the_label && *the_label)
  {
    float label_width, label_height;
    GetTextExtent(the_label, &label_width, &label_height, NULL, NULL, labelFont);
    if (width < 0)
      width = (int)(label_width + RADIO_SIZE);
    if (height<0)
    {
      height = (int)(label_height);
      if (height < RADIO_SIZE)
        height = RADIO_SIZE;
    }
  }
  else
  {
    if (width < 0)
      width = RADIO_SIZE;
    if (height < 0)
      height = RADIO_SIZE;
  }
  // end GRW fix

  SetSize(x, y, width, height);
  panel->AdvanceCursor(this);
//  Callback(func);

  if (Title)
  {
    if (style&wxFIXED_LENGTH)
      SetLabel((char *)Title) ;
  }
  if (the_label)
    delete [] the_label ;

  return TRUE;
}

Bool wxRadioButton::Create(wxPanel *panel, wxFunction func,
                       wxBitmap *bitmap, Bool value,
                       int x, int y, int width, int height,
                       long style, Constdata char *name)
{
  SetName(name);
  if (panel) panel->AddChild(this);
  buttonFont = panel->buttonFont ;
  labelFont = panel->labelFont ;
  if (buttonFont)
    buttonFont->UseResource();
  if (labelFont)
    labelFont->UseResource();
  SetBackgroundColour(*panel->GetBackgroundColour()) ;
  labelColour = panel->labelColour ;
  buttonColour = panel->buttonColour ;
  windowStyle = style ;
  ms_handle = 0;
  wxWinType = wxTYPE_HWND;

  panel->GetValidPosition(&x, &y);

#if FAFA_LIB // && !CTL3D
  isFafa = TRUE ; //always !!
#else
  isFafa = RADIO_IS_FAFA ;
#endif

  long groupStyle = 0;
  if (windowStyle & wxRB_GROUP)
    groupStyle = WS_GROUP;
  long newId = NewControlId();

#if FAFA_LIB // && !CTL3D
  long msStyle = groupStyle | BITRADIO_FLAGS;
#if WIN95
//  msStyle |= WS_EX_CLIENTEDGE;
#endif
  ms_handle = CreateWindowEx(0, FafaChck, "toggle",
                          msStyle,0,0,0,0,
                          panel->GetHWND(), (HMENU)newId, wxhInstance, NULL);
  SendMessage(ms_handle, WM_CHANGEBITMAP,
                  (WPARAM)((bitmap->GetHeight()<<8)+bitmap->GetWidth()),
                  (LPARAM)bitmap->ms_bitmap);

#else
    long msStyle = groupStyle | RADIO_FLAGS;
#if WIN95
//    msStyle |= WS_EX_CLIENTEDGE;
#endif
  ms_handle = CreateWindowEx(0, RADIO_CLASS, "toggle",
                          msStyle,0,0,0,0,
                          panel->GetHWND(), (HMENU)newId, wxhInstance, NULL);
#if CTL3D
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
    Ctl3dSubclassCtl(ms_handle);
#endif
#endif

  // Subclass again for purposes of dialog editing mode
  SubclassControl((HWND)ms_handle);

  SetValue(value);

  SetSize(x, y, width, height);
  panel->AdvanceCursor(this);
//  Callback(func);

  return TRUE;
}

wxRadioButton::~wxRadioButton(void)
{
  isBeingDeleted = TRUE;
  
  if (ms_handle)
    DestroyWindow(ms_handle) ;
  ms_handle = NULL ;
}

char *wxRadioButton::GetLabel(void)
{
  GetWindowText((HWND)ms_handle, wxBuffer, 300);
  return wxBuffer;
}

void wxRadioButton::SetLabel(char *label)
{
#if FAFA_LIB // && !CTL3D
  // This message will switch from FB_BITMAP style to FB_TEXT, if needed.
  SendMessage((HWND)ms_handle,WM_CHANGEBITMAP,
                (WPARAM)0,
                (LPARAM)NULL);
#endif
  SetWindowText((HWND)ms_handle, label);
}

void wxRadioButton::SetLabel(wxBitmap *bitmap)
{
#if FAFA_LIB // && !CTL3D
  SendMessage((HWND)ms_handle,WM_CHANGEBITMAP,
                (WPARAM)((bitmap->GetHeight()<<8)+bitmap->GetWidth()),
                (LPARAM)bitmap->ms_bitmap);
//radioWidth = bitmap->GetWidth() + FB_MARGIN ;
//radioHeight = bitmap->GetHeight() + FB_MARGIN ;
#endif
}

void wxRadioButton::SetValue(Bool value)
{
// Following necessary for Win32s, because Win32s translate BM_SETCHECK
#if FAFA_LIB
  SendMessage(ms_handle, isFafa?FAFA_SETCHECK:BM_SETCHECK, (WPARAM)value, 0L);
#else
  SendMessage(ms_handle, BM_SETCHECK, (WPARAM)value, 0L);
#endif
}

// Get single selection, for single choice list items
Bool wxRadioButton::GetValue(void)
{
#if FAFA_LIB
  return (Bool)SendMessage(ms_handle, isFafa?FAFA_GETCHECK:BM_GETCHECK, 0, 0L);
#else
  return (Bool)SendMessage(ms_handle, BM_SETCHECK, 0, 0L);
#endif
}

#endif // USE_RADIOBUTTON
