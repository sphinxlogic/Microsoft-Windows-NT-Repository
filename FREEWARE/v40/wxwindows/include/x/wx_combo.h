/*
 * File:	wx_combo.h
 * Purpose:	Declares wxComboBox panel item (X version)
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

#ifndef wx_comboh
#define wx_comboh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_list.h"
#include "wb_combo.h"

#ifdef IN_CPROTO
typedef       void    *wxComboBox ;
#else

class wxPanel;

// Choice item
class wxComboBox: public wxbComboBox
{
  DECLARE_DYNAMIC_CLASS(wxComboBox)
 private:
 public:
#ifdef wx_motif
/*
  Widget menuWidget;
  Widget buttonWidget;
  Widget *widgetList ;
  Widget rowWidget;
*/
  wxStringList stringList;
#endif
  wxComboBox(void);
  wxComboBox(wxPanel *panel, wxFunction func, Const char *title, Constdata char *value = "",
           int x = -1, int y = -1, int width = -1, int height = -1,
           int n = 0, char **choices = NULL,
           long style = 0, char *name = "comboBox");
  ~wxComboBox(void);

  Bool Create(wxPanel *panel, wxFunction func, Const char *title, Constdata char *value = "",
           int x = -1, int y = -1, int width = -1, int height = -1,
           int n = 0, char **choices = NULL,
           long style = 0, char *name = "comboBox");

  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void SetSize(int width, int height) { wxItem::SetSize(width, height); }
  virtual void Append(char *Item);
  virtual void Clear(void);
  virtual int GetSelection(void);
  virtual void SetSelection(int n);
  virtual int FindString(char *s);
  virtual char *GetString(int n);
  virtual void Delete(int n);
  virtual char *GetValue(void);
  virtual void SetValue(char *val);
  
  // Clipboard operations
  virtual void Copy(void);
  virtual void Cut(void);
  virtual void Paste(void);
  virtual void SetInsertionPoint(long pos);
  virtual void SetInsertionPointEnd(void);
  virtual long GetInsertionPoint(void);
  virtual long GetLastPosition(void);
  virtual void Replace(long from, long to, char *value);
  virtual void Remove(long from, long to);
  virtual void SetSelection(long from, long to);
  virtual void SetEditable(Bool editable);

  Bool Show(Bool show);
  void Enable(Bool enable);

  void SetFocus(void);

  void ChangeColour(void) ;

  void SetColumns(int n = 1 );
  int  GetColumns(void);
};

#endif // IN_CPROTO
#endif // wx_choich
