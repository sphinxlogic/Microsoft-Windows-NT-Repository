/*
 * File:	wx_lbox.h
 * Purpose:	List box panel item
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#ifndef wx_lboxh
#define wx_lboxh

#include "wb_lbox.h"

#ifdef IN_CPROTO
typedef       void    *wxListBox ;
#else

WXDLLEXPORT extern Constdata char *wxListBoxNameStr;

// List box item
class WXDLLEXPORT wxListBox: public wxbListBox
{
  DECLARE_DYNAMIC_CLASS(wxListBox)
 private:
 public:
  wxListBox(void);
  wxListBox(wxPanel *panel, wxFunction func, Const char *title,
             Bool Multiple = wxSINGLE|wxNEEDED_SB,
             int x = -1, int y = -1, int width = -1, int height = -1,
             int n = 0, char **choices = NULL,
             long style = 0, Constdata char *name = wxListBoxNameStr);
  ~wxListBox(void);

  Bool Create(wxPanel *panel, wxFunction func, Const char *title, Bool Multiple = FALSE,
             int x = -1, int y = -1, int width = -1, int height = -1,
             int n = 0, char **choices = NULL,
             long style = 0, Constdata char *name = wxListBoxNameStr);
  BOOL MSWCommand(UINT param, WORD id);

  void Append(char *item);
  void Append(char *item, char *clientData);
  void Set(int n, char *choices[], char *clientData[] = NULL);
  int FindString(char *s);
  void Clear(void);
  void SetSelection(int n, Bool select = TRUE);

  void Deselect(int n);

  // For single choice list item only
  int GetSelection(void);
  void Delete(int n);
  char *GetClientData(int n);
  void SetClientData(int n, char *clientData);
  void SetString(int n, char *s);

  // For single or multiple choice list item
  int GetSelections(int **listSelections);
  Bool Selected(int n);
  char *GetString(int n);
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void GetSize(int *x, int *y);
  void GetPosition(int *x, int *y);
  char *GetLabel(void);
  void SetLabel(char *label);

  // Set the specified item at the first visible item
  // or scroll to max range.
  void SetFirstItem(int n) ;
  void SetFirstItem(char *s) ;

  void InsertItems(int nItems, char **Items, int pos);

  // Windows-specific code to set the horizontal extent of
  // the listbox, if necessary. If s is non-NULL, it's
  // used to calculate the horizontal extent.
  // Otherwise, all strings are used.
  void SetHorizontalExtent(char *s = NULL);
};

#endif // IN_CPROTO
#endif // wx_lboxh
