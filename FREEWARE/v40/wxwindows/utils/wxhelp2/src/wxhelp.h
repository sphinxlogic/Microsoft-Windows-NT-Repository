/*
 * File:	wxhelp.h
 * Purpose:	wxWindows help system
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#include "wx.h"
#include "wxhtml.h"

// Define a new application
class MyApp: public wxApp
{
  DECLARE_CLASS(MyApp)
  public:
    wxFrame *OnInit(void);
};

class KeywordEntry: public wxObject
{
 public:
  char *helpKeyword;
  char *helpTopic;
  char *helpFile;

  KeywordEntry(char *topic, char *file, char *keyword);
  ~KeywordEntry(void);
};


class HelpWindow: public wxHtml
{
  DECLARE_CLASS(HelpWindow)
 protected:
  wxList keywords;
 public:
  HelpWindow(wxFrame *frame, int x, int y, int w, int h, long style, char *name = "helpWindow");
  ~HelpWindow(void);
  
  Bool LoadIndex(const char *filename);
  void ClearIndex(void);
  
  Bool LoadHelpFile(const char *filename);

  wxList *FindMatchingStrings(char *search_string1, Bool subString = TRUE);
  void DeleteMatchingStrings(wxList *string_list);
  void HelpSearch(void);
  
  inline wxList& GetKeywords(void) { return keywords; }

#if 0
  // Derived members
  void OnLeftClick(float x, float y, int char_pos, int line, long block_id, int keys);
  void OnRightClick(float x, float y, int char_pos, int line, long block_id, int keys);
  void OnSelectBlock(long block_id, Bool select);
  void ClearBlock(long block_id);

  // New members
  void DisplaySection(void);
  void StoreHypertextItem(long block_id);
  void HistoryBack(void);
  void DisplayHistory(void);
#endif
};

// Define a new frame
class MyFrame: public wxFrame
{
  DECLARE_CLASS(MyFrame)
  public:
    HelpWindow *window;
    wxPanel *panel;
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    Bool OnClose(void);
    void OnSize(int x, int y);
    void OnMenuCommand(int id);
    inline void OnActivate(Bool active) { if (window) window->SetFocus(); }
};

class SearchBox: public wxDialogBox
{
  DECLARE_CLASS(SearchBox)
 public:
  wxText *search_item;
  wxListBox *titles_item;
  SearchBox(wxFrame *parent);
  void DoSearch(wxList *string_list = NULL);
};

#define HELP_OPEN        100
#define HELP_EXIT        101
#define HELP_CLEAR_BLOCK 104
#define HELP_SAVE        105
#define HELP_SET_TITLE   107
#define HELP_HELP_CONTENTS 108
#define HELP_ABOUT       109

// void hyErrorMsg(char *msg);
void HelpSearch(void);

class HelpConnection;
extern HelpConnection *TheHelpConnection;

// Communication with applications
class HelpConnection: public wxConnection
{
  DECLARE_CLASS(HelpConnection)
 public:
  HelpConnection(void)
    { TheHelpConnection = this; }
  ~HelpConnection(void)
    { TheHelpConnection = NULL; }
  Bool OnExecute(char *topic, char *data, int size, int format);
  Bool OnDisconnect(void);
//  char *OnRequest(char *topic, char *item, int *size, int format);
//  Bool OnPoke(char *topic, char *item, char *data, int size, int format);
};

class HelpServer: public wxServer
{
  DECLARE_CLASS(HelpServer)
 public:
  wxConnection *OnAcceptConnection(char *topic)
    { if (strcmp(topic, "WXHELP") == 0 && (!TheHelpConnection))
        return new HelpConnection;
      else return NULL; }
};
