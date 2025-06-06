/*
 * File:	wb_lbox.h
 * Purpose:	List box
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wb_lbox.h	1.2 5/9/94" */

#ifndef wxb_lboxh
#define wxb_lboxh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_panel.h"
#include "wx_item.h"

#ifdef IN_CPROTO
typedef       void    *wxbListBox ;
#else

WXDLLEXPORT extern Constdata char *wxListBoxNameStr;

// List box item
class WXDLLEXPORT wxbListBox: public wxItem
{
 public:

  int no_items;
  int selected;
  int *selections;
  int multiple;

  wxbListBox(void);
  wxbListBox(wxPanel *panel, wxFunction func, Const char *title, Bool Multiple = FALSE,
             int x = -1, int y = -1, int width = -1, int height = -1,
             int N = 0, char **choices = NULL,
             long style = 0, Constdata char *name = wxListBoxNameStr);
  ~wxbListBox(void);

  virtual void Append(char *Item) = 0;
  virtual void Append(char *Item, char *Client_data)= 0;
  virtual void Set(int N, char *Choices[], char *clientData[]) = 0;
  virtual int FindString(char *s) = 0;
  virtual void Clear(void) = 0;
  virtual void SetSelection(int N, Bool select = TRUE) = 0;
  virtual char *GetClientData(int N) = 0;
  virtual void SetClientData(int N, char *Client_data) = 0;
  virtual void Deselect(int N) = 0;
  virtual int GetSelection(void) = 0;  // For single choice list item only
  virtual char *GetStringSelection(void);
  virtual Bool SetStringSelection(char *s);
  virtual void SetString(int N, char *s) = 0;
  virtual Bool Selected(int N) = 0;

  virtual int Number(void);
  virtual void Delete(int N) = 0;

  // For single or multiple choice list item
  virtual int GetSelections(int **list_selections) = 0;
  virtual char *GetString(int N) = 0;

  // Set the specified item at the first visible item
  // or scroll to max range.
  virtual void SetFirstItem(int N)=0 ;
  virtual void SetFirstItem(char *s) = 0;

  virtual void InsertItems(int nItems, char **Items, int pos) = 0;
  
  virtual int GetSelectionMode(void) { return multiple; }
  virtual void SetSelectionMode(int sel) { multiple = sel; }

  void Command(wxCommandEvent& event);
  void ProcessCommand(wxCommandEvent& event);
};

#endif // IN_CPROTO
#endif // wxb_lboxh
