/*
 * File:	wxhelp.cc
 * Purpose:	wxWindows help system
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

#include "wx_help.h"
#include "wx_hash.h"
#include "wxhtml.h"
#include "wxhelp.h"
// #include "wxhlpblk.h"

float helpVersion = 2.0;

#define HELP_SERVER_ID -1

#ifdef wx_x
#define DEFAULT_EDITOR "emacs"
#endif
#ifdef wx_msw
#define DEFAULT_EDITOR "notepad"
#endif

#ifdef wx_x
#include "help.xbm"
#endif

MyFrame   *MainFrame = NULL;
SearchBox *TheSearchBox = NULL;
wxFont *swiss_font_12 = NULL;
char *helpFilename = NULL;
Bool helpEditMode = FALSE;

int SearchWindowX = 10;
int SearchWindowY = 10;

wxList helpHistory;
wxNode *helpHistoryPointer = NULL;

// Communication
HelpConnection *TheHelpConnection = NULL;
HelpServer *TheHelpServer = NULL;

#ifdef wx_msw // Only one instance in Windows 3 so make it a server ALWAYS
int TheHelpServerId = 4000;
#endif
#ifdef wx_x
int TheHelpServerId = -1;
#endif

// wxHelp also has help!!
wxHelpInstance *HelpInstance = NULL;
wxPathList HelpPathList;

#ifdef wx_xview
#define HELP_PANEL_HEIGHT 30
#endif
#ifdef wx_motif
#define HELP_PANEL_HEIGHT 40
#endif
#ifdef wx_msw
#define HELP_PANEL_HEIGHT 40
#endif

IMPLEMENT_CLASS(MyApp, wxApp)
IMPLEMENT_CLASS(HelpWindow, wxHtml)
IMPLEMENT_CLASS(MyFrame, wxFrame)
IMPLEMENT_CLASS(SearchBox, wxDialogBox)
IMPLEMENT_CLASS(HelpConnection, wxConnection)
IMPLEMENT_CLASS(HelpServer, wxServer)

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initialises the whole application
MyApp     myApp;

void helpHelpPrevious(wxButton& but, wxCommandEvent& ev)
{
/*
  MainFrame->window->DisplayPreviousSection();
  MainFrame->window->DisplaySection();
  long block_id = MainFrame->window->FindBlockForSection(MainFrame->window->current_section);
  MainFrame->window->StoreHypertextItem(block_id);
*/
}

void helpHelpNext(wxButton& but, wxCommandEvent& ev)
{
/*
  MainFrame->window->DisplayNextSection();
  MainFrame->window->DisplaySection();
  long block_id = MainFrame->window->FindBlockForSection(MainFrame->window->current_section);
  MainFrame->window->StoreHypertextItem(block_id);
*/
}

void helpHelpBack(wxButton& but, wxCommandEvent& ev)
{
  MainFrame->window->Back();
  MainFrame->window->SetFocus();
}

void helpHelpHistory(wxButton& but, wxCommandEvent& ev)
{
}

void helpHelpContents(wxButton& but, wxCommandEvent& ev)
{
  if (helpFilename)
  {
    string str("file:///");
    str += helpFilename;
    MainFrame->window->OpenURL(str);
  }
  MainFrame->window->SetFocus();
}

void helpHelpSearch(wxButton& but, wxCommandEvent& ev)
{
  MainFrame->window->HelpSearch();
  MainFrame->window->SetFocus();
}

// The `main program' equivalent, creating the windows and returning the
// main frame
wxFrame *MyApp::OnInit(void)
{
  int i = 1;
  
  // Read input file if given without using -f
  if (argc > 1 && (argv[1][0] != '-'))
  {
    helpFilename = copystring(argv[1]);
    i ++;
  }

  while (i < argc)
  {
    if (strcmp(argv[i], "-f") == 0)
    {
      i ++;
      if (i == argc)
        wxMessageBox("-f must take a filename!", "wxHelp Error", wxOK);
      else
      {
        helpFilename = copystring(argv[i]);
        i ++;
      }
    }
    else if (strcmp(argv[i], "-edit") == 0)
    {
      i ++;
      helpEditMode = TRUE;
    }
    else if (strcmp(argv[i], "-server") == 0)
    {
      i ++;
      TheHelpServerId = atoi(argv[i]);
      i ++;
    }
    else i++;
  }

/*
  if (helpFilename)
  {
    int len = strlen(helpFilename);
    if (!(helpFilename[len-1] == 'p' && helpFilename[len-2] == 'l' && helpFilename[len-3] == 'x' && helpFilename[len-4] == '.'))
    {
      char buf[400];
      strcpy(buf, helpFilename);
      strcat(buf, ".xlp");
      delete[] helpFilename;
      helpFilename = copystring(buf);
    }
  }
*/
  
  HelpPathList.AddEnvList("WXHELPFILES");
  HelpPathList.AddEnvList("PATH");

  // Create the main frame window
  MainFrame = new MyFrame(NULL, "wxHelp", 0, 0, 500, 400);

  if (helpEditMode)
    MainFrame->CreateStatusLine();

  // Give it an icon
#ifdef wx_msw
  wxIcon *icon = new wxIcon("hlp_icn");
#endif
#ifdef wx_x
  wxIcon *icon = new wxIcon(help_bits, help_width, help_height);
#endif

  MainFrame->SetIcon(icon);

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(HELP_OPEN, "&Open File");
  if (helpEditMode)
    file_menu->Append(HELP_SAVE, "&Save File");

  file_menu->AppendSeparator();
  file_menu->Append(HELP_EXIT, "&Exit");

  wxMenu *edit_menu = NULL;
  wxMenu *block_menu = NULL;
#if 0
  if (helpEditMode)
  {
    block_menu = new wxMenu;
    edit_menu = new wxMenu;
    edit_menu->Append(HELP_RUN_EDITOR, "Run &Editor");
    edit_menu->Append(HELP_CLEAR_SELECTION, "&Clear selection");
    edit_menu->Append(HELP_CLEAR_BLOCK, "Clear &block");
    edit_menu->AppendSeparator();
    edit_menu->Append(HELP_SET_TITLE, "&Set title");

    block_menu->Append(HELP_MARK_LARGE_HEADING, "Mark &large heading");
    block_menu->Append(HELP_MARK_SMALL_HEADING, "Mark &small heading");
    block_menu->Append(HELP_MARK_LARGE_VISIBLE_SECTION, "Mark large &visible section");
    block_menu->Append(HELP_MARK_SMALL_VISIBLE_SECTION, "Mark small visible &section");
    block_menu->Append(HELP_MARK_INVISIBLE_SECTION, "Mark &invisible section");
    block_menu->AppendSeparator();

    block_menu->Append(HELP_MARK_RED, "Mark &red");
    block_menu->Append(HELP_MARK_BLUE, "Mark &blue");
    block_menu->Append(HELP_MARK_GREEN, "Mark &green");
    block_menu->AppendSeparator();
    block_menu->Append(HELP_MARK_BOLD, "Mark &bold");
    block_menu->Append(HELP_MARK_ITALIC, "Mark &italics");
    block_menu->Append(HELP_MARK_RED_ITALIC, "Mark red i&talics");
    block_menu->Append(HELP_MARK_SMALL_TEXT, "Mark small te&xt");
  }
#endif

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(HELP_HELP_CONTENTS, "&Help on wxHelp");
  help_menu->AppendSeparator();
  help_menu->Append(HELP_ABOUT, "&About wxHelp");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");

#if 0
  if (helpEditMode)
  {
    menu_bar->Append(edit_menu, "&Edit");
    menu_bar->Append(block_menu, "&Blocks");
  }
#endif

  menu_bar->Append(help_menu, "&Help");

  // Associate the menu bar with the frame
  MainFrame->SetMenuBar(menu_bar);

  // Make a button bar, a la Windows Help
  MainFrame->panel = new wxPanel(MainFrame, 0, 0, 400, HELP_PANEL_HEIGHT);

  (void)new wxButton(MainFrame->panel, (wxFunction)helpHelpContents, "Contents");
  (void)new wxButton(MainFrame->panel, (wxFunction)helpHelpSearch, "Search");
//  (void)new wxButton(MainFrame->panel, (wxFunction)helpHelpHistory, "History");
  (void)new wxButton(MainFrame->panel, (wxFunction)helpHelpBack, "Back");
/*
  (void)new wxButton(MainFrame->panel, (wxFunction)helpHelpPrevious, "<<");
  (void)new wxButton(MainFrame->panel, (wxFunction)helpHelpNext, ">>");
*/
  // Make a helppertext window
  MainFrame->window = new HelpWindow(MainFrame, 0, 0, 400, 400, 0);
/*
  MainFrame->window->SetEditMode(helpEditMode);
  MainFrame->window->SetIndexWriting(TRUE);
*/
  if (helpFilename)
  {
    char *s = HelpPathList.FindValidPath(helpFilename);
    if (!s)
    {
      MainFrame->SetTitle("wxHelp (no title)");
      char buf[500];
      sprintf(buf, "Cannot find file %s.\nTrying editing the WXHELPFILES environment variable.", helpFilename);
#ifdef wx_x
      cerr << buf << "\n";
#endif
#ifdef wx_msw
      wxMessageBox(buf);
#endif
    }
    else
    {
      MainFrame->window->LoadHelpFile((const char *)s);
/*
      if (MainFrame->window->GetTitle())
        MainFrame->SetTitle(MainFrame->window->GetTitle());
      else
*/
        MainFrame->SetTitle("wxHelp (no title)");
    }
  }

  if (TheHelpServerId > 0)
  {
    wxIPCInitialize();

    TheHelpServer = new HelpServer;
    char buf[50];
    sprintf(buf, "%d", TheHelpServerId);
    TheHelpServer->Create(buf);
  }

  HelpInstance = new wxHelpInstance(TRUE);
  HelpInstance->Initialize("help", HELP_SERVER_ID);

  MainFrame->Centre();
  MainFrame->Show(TRUE);

  return MainFrame;
}

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{
  window = NULL;
  panel = NULL;
}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  switch (id)
  {
    case HELP_OPEN:
    {
#if defined(wx_msw) && !defined(WIN32)
      char *s = wxFileSelector("Load file", NULL, NULL, NULL, "*.htm");
#else
      char *s = wxFileSelector("Load file", NULL, NULL, NULL, "*.html");
#endif
      if (s)
      {
        char *s1 = HelpPathList.FindValidPath(s);
        if (s1)
        {
          helpFilename = copystring(s1);
          window->LoadHelpFile(helpFilename);
        }
        else
          wxMessageBox("Could not open file", "wxHelp", wxOK);
/*
        if (window->GetTitle())
          SetTitle(window->GetTitle());
        else
*/
        SetTitle("wxHelp (no title)");
      }
      break;
    }
    case HELP_EXIT:
    {
      OnClose();

      if (TheHelpServer)
        delete TheHelpServer ;
      if (HelpInstance)
        delete HelpInstance ;
      delete this;
      break;
    }

    case HELP_HELP_CONTENTS:
    {
      HelpInstance->LoadFile("help");
      HelpInstance->DisplayContents();
      break;
    }

    case HELP_ABOUT:
    {
      char buf[400];
      sprintf(buf, "wxHelp version %.2f\n", helpVersion);
      strcat(buf, "by Julian Smart, AIAI (c) January 1996\n");
      strcat(buf, "J.Smart@ed.ac.uk");
      wxMessageBox(buf, "About wxHelp");
      break;
    }
  }
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
Bool MyFrame::OnClose(void)
{
  if (TheSearchBox)
    delete TheSearchBox;

  if (TheHelpConnection)
    TheHelpConnection->Disconnect();

  HelpInstance->Quit();

  return TRUE;
}

void MyFrame::OnSize(int w, int h)
{
  if (panel && window)
  {
    int width, height;
    GetClientSize(&width, &height);
    panel->SetSize(0, 0, width, HELP_PANEL_HEIGHT);
    window->SetSize(0, HELP_PANEL_HEIGHT, width, height - HELP_PANEL_HEIGHT);
//    window->ReDraw();
    window->Reload();
  }
}

HelpWindow::HelpWindow(wxFrame *frame, int x, int y, int w, int h, long style, char *name):
  wxHtml(frame, x, y, w, h), keywords(wxKEY_STRING)
{
}

HelpWindow::~HelpWindow(void)
{
  ClearIndex();
}

// Loads .htx index file, generated by e.g. Tex2RTF,
// with each line containing:
// topic|file|keyword
Bool HelpWindow::LoadIndex(const char *filename)
{
  ifstream stream(filename);
  if (stream.bad())
    return FALSE;

  ClearIndex();
  while (!stream.eof())
  {
    stream.getline(wxBuffer, 500);
    if (strlen(wxBuffer) > 0)
    {
      char *tok1 = strtok(wxBuffer, "|,");
      char *tok2 = strtok(NULL, "|,");
      char *tok3 = strtok(NULL, "|,");
      
      if (tok1 && tok2 && tok3 &&
         strlen(tok1) > 0 && strlen(tok2) > 0 && strlen(tok3) > 0)
      {
        KeywordEntry *entry = new KeywordEntry(tok1, tok2, tok3);
        keywords.Append(entry);
      }
    }
  }
  
  return TRUE;
}

void HelpWindow::ClearIndex(void)
{
  keywords.DeleteContents(TRUE);
  keywords.Clear();
  keywords.DeleteContents(FALSE);
}

Bool HelpWindow::LoadHelpFile(const char *filename)
{
  char rootBuf[300];
  char htxBuf[300];
  char htmlBuf[300];
  strcpy(rootBuf, filename);
  wxStripExtension(rootBuf);
  
  strcpy(htxBuf, rootBuf);
  strcpy(htmlBuf, rootBuf);
  strcat(htxBuf, ".htx");
  
#if defined(wx_msw) && !defined(WIN32)
  strcat(htmlBuf, ".htm");
#else
  strcat(htmlBuf, ".html");
  if (!wxFileExists(htmlBuf))
  {
    wxStripExtension(htmlBuf);
    strcat(htmlBuf, ".htm");
  }
#endif

  if (wxFileExists(htxBuf))
    LoadIndex((const char *)htxBuf);
    
  if (wxFileExists(htmlBuf))
  {
    string str("file:///");
    str += (const char *)htmlBuf;
    OpenURL(str);
    return TRUE;
  }
  else return FALSE;
}


KeywordEntry::KeywordEntry(char *topic, char *file, char *keyword)
{
  if (topic)
    helpTopic = copystring(topic);
  else
    helpTopic = NULL;
  if (file)
    helpFile = copystring(file);
  else
    file = NULL;
  if (keyword)
    helpKeyword = copystring(keyword);
  else
    helpKeyword = NULL;
}

KeywordEntry::~KeywordEntry(void)
{
  if (helpKeyword)
    delete[] helpKeyword;
  if (helpTopic)
    delete[] helpTopic;
  if (helpFile)
    delete[] helpFile;
}

/* Searching
 */

class StringData: public wxObject
{
 public:
  char *s;
  wxObject *data;
  StringData(char *s, wxObject *data);
};

StringData::StringData(char *the_s, wxObject *the_data)
{
  s = the_s;
  data = the_data;
}

int wxCompareStringData(const void *arg1, const void *arg2)
{
  StringData **one = (StringData **)arg1;
  StringData **two = (StringData **)arg2;

  return strcmp((*one)->s, (*two)->s);
}

void SortStringDataList(wxList *list)
{
  int n = list->Number();
  StringData **array = new StringData *[list->Number()];
  int i = 0;
  wxNode *node = list->First();
  while (node)
  {
    array[i] = (StringData *)node->Data();
    i ++;
    node = node->Next();
  }
  qsort(array, n, sizeof(StringData *), wxCompareStringData);
  list->Clear();

  for (i = 0; i < n; i++)
    list->Append((StringData *)(array[i]));

  delete array;
}

void helpDoSearch(wxButton& button, wxCommandEvent& event)
{
  SearchBox *box = (SearchBox *)button.GetParent();
  box->DoSearch();
}

void helpSearchString(wxText& text, wxCommandEvent& event)
{
  SearchBox *box = (SearchBox *)text.GetParent();
  if (event.GetEventType() == wxEVENT_TYPE_TEXT_ENTER_COMMAND)
    box->DoSearch();
}

void helpSearchQuit(wxButton& button, wxCommandEvent& event)
{
  SearchBox *box = (SearchBox *)button.GetParent();
//  box->GetPosition(&SearchWindowX, &SearchWindowY);
  TheSearchBox = NULL;
  delete box;
}

void helpSearchSelection(wxListBox& listbox, wxCommandEvent& event)
{
  int sel = listbox.GetSelection();
  KeywordEntry *entry = (KeywordEntry *)listbox.GetClientData(sel);
  
  string str("file:///");
  str += entry->helpFile;
  str += "#";
  str += entry->helpTopic;

  // Show file
  MainFrame->window->OpenURL(str);
  
  MainFrame->Iconize(FALSE);
  MainFrame->Show(TRUE);
}

// Search window
SearchBox::SearchBox(wxFrame *parent):
  wxDialogBox(parent, "Search", FALSE, SearchWindowX, SearchWindowY, 500, 500,
    wxDEFAULT_DIALOG_STYLE)
{
  SetClientSize(500, 300);

  (void)new wxButton(this, (wxFunction)&helpSearchQuit, "OK");
  (void)new wxButton(this, (wxFunction)&helpDoSearch, "Do search");
  NewLine();

  search_item = new wxText(this, (wxFunction)helpSearchString, "Search string", "*", -1, -1, 400, -1, wxPROCESS_ENTER);
  NewLine();

  SetLabelPosition(wxVERTICAL);
  titles_item = new wxListBox(this, (wxFunction)&helpSearchSelection,
                             "Items", wxSINGLE, -1, -1, 410, 200);

  Fit();
}

wxList *HelpWindow::FindMatchingStrings(char *search_string1, Bool subString)
{
  wxCursor *old_cursor = MainFrame->window->SetCursor(wxHOURGLASS_CURSOR);
  ::wxSetCursor(wxHOURGLASS_CURSOR);

  char *search_string = copystring(search_string1);

  int i;
  for (i = 0; i < (int)strlen(search_string); i ++)
    search_string[i] = toupper(search_string[i]);

  wxList *string_list = new wxList;

  wxNode *node = MainFrame->window->GetKeywords().First();
  char buf[300];
  while (node)
  {
    KeywordEntry *entry = (KeywordEntry *)node->Data();
    strcpy(buf, entry->helpKeyword);

    for (i = 0; i < (int)strlen(buf); i ++)
      buf[i] = toupper(buf[i]);

    if (search_string && (search_string[0] == '*' || StringMatch(search_string, buf, subString)))
      string_list->Append(new StringData(copystring(entry->helpKeyword), entry));

    node = node->Next();
  }

  SortStringDataList(string_list);
  delete search_string;

  MainFrame->window->SetCursor(old_cursor);
  ::wxSetCursor(wxSTANDARD_CURSOR);

  return string_list;
}

void HelpWindow::DeleteMatchingStrings(wxList *string_list)
{
  wxNode *node = string_list->First();
  while (node)
  {
    StringData *sd = (StringData *)node->Data();

    delete sd;
    delete node;
    node = string_list->First();
  }
  delete string_list;
}

void SearchBox::DoSearch(wxList *string_list)
{
  char *search_string = copystring(search_item->GetValue());
  if (!search_string)
    return;

  wxCursor *old_cursor = MainFrame->window->SetCursor(wxHOURGLASS_CURSOR);
  ::wxSetCursor(wxHOURGLASS_CURSOR);

  titles_item->Show(FALSE);
  titles_item->Clear();

  if (!string_list)
    string_list = MainFrame->window->FindMatchingStrings(search_string);

  wxNode *node = string_list->First();
  while (node)
  {
    StringData *sd = (StringData *)node->Data();
    titles_item->Append(sd->s, (char *)sd->data);

    node = node->Next();
  }
  MainFrame->window->DeleteMatchingStrings(string_list);

  titles_item->Show(TRUE);
  delete[] search_string;

  MainFrame->window->SetCursor(old_cursor);
  ::wxSetCursor(wxSTANDARD_CURSOR);
}

void HelpWindow::HelpSearch(void)
{
  if (TheSearchBox)
  {
    TheSearchBox->Show(TRUE);
    return;
  }
  TheSearchBox = new SearchBox(NULL);
  TheSearchBox->DoSearch();

  TheSearchBox->Show(TRUE);
}

/*
 * Communication with applications
 *
 */

Bool HelpConnection::OnExecute(char *topic, char *data, int size, int format)
{
  if (strcmp(topic, "WXHELP") == 0)
  {
    switch (data[0])
    {
      case 's': // Display at section (-1 for top)
      {
/*
        int section_no = atoi(data + 2);
        if (section_no == -1)
        {
          MainFrame->window->DisplayFileAtTop();
          MainFrame->window->StoreHypertextItem(-1);
          MainFrame->Iconize(FALSE);
          MainFrame->Show(TRUE);
          return TRUE;
        }
        else
        {
          // Find section for block
          wxNode *sectionNode = MainFrame->window->sections.Nth(section_no - 1);
          if (sectionNode)
	  {
            wxTextChunk *chunk = (wxTextChunk *)sectionNode->Data();
            MainFrame->window->DisplayFileAt(chunk->block_id);
            MainFrame->window->StoreHypertextItem(chunk->block_id);
            MainFrame->Iconize(FALSE);
            MainFrame->Show(TRUE);
            return TRUE;
	  }
          else return TRUE;
        }
*/
        break;
      }
      case 'b': // Display at block
      {
/*
        long block_no = atol(data + 2);
        MainFrame->window->DisplayFileAt(block_no);
        MainFrame->window->StoreHypertextItem(block_no);
*/
        MainFrame->Iconize(FALSE);
        MainFrame->Show(TRUE);
        return TRUE;
        break;
      }
      case 'f': // Load file
      {
        char *name = data + 2;

        int len = strlen(name);
        char buf[400];

        strcpy(buf, name);
        if (!(name[len-1] == 'p' && name[len-2] == 'l' && name[len-3] == 'x' && name[len-4] == '.'))
	{
          strcat(buf, ".html");
	}
        char *s = HelpPathList.FindValidPath(buf);
        if (!s)
        {
          MainFrame->SetTitle("wxHelp (no title)");
          char buf1[400];
          sprintf(buf1, "Cannot find file %s.\nTry editing the WXHELPFILES environment variable.", name);
          wxMessageBox(buf1);
          return FALSE;
        }

        if (!helpFilename || (strcmp(s, helpFilename) != 0))
	{
          if (helpFilename) delete[] helpFilename;
          helpFilename = copystring(s);
/*
          Bool succ = MainFrame->window->LoadFile(helpFilename);
*/
          MainFrame->Iconize(FALSE);
          MainFrame->Show(TRUE);
          return FALSE;
	}
        else
        {
          MainFrame->Iconize(FALSE);
          MainFrame->Show(TRUE);
          return TRUE;
	}
        break;
      }
      case 'k': // Search for string
      {
        char *str = data + 2;
        MainFrame->Iconize(FALSE);
        MainFrame->Show(TRUE);

        // No substring matching programmatically
        wxList *string_list = MainFrame->window->FindMatchingStrings(str, FALSE);
        int n = string_list->Number();
        if (n == 0)
	{
          delete string_list;
          return FALSE;
	}
        else if (n == 1)
	{
          StringData *sd = (StringData *)string_list->First()->Data();
          long block_id = (long)sd->data;
          MainFrame->window->DeleteMatchingStrings(string_list);
          
          // LOAD THE FILE HERE
          return TRUE;
	}
        else
	{
          MainFrame->Iconize(FALSE);
          MainFrame->Show(TRUE);
          if (TheSearchBox)
            TheSearchBox->Show(TRUE);
          else
            TheSearchBox = new SearchBox(NULL);
          TheSearchBox->search_item->SetValue(str);
          TheSearchBox->DoSearch(string_list);

          TheSearchBox->Show(TRUE);
          return TRUE;
	}
        break;
      }
      default:
        return FALSE;
    }
  } else return FALSE;
  return FALSE;
}

Bool HelpConnection::OnDisconnect(void)
{
  TheHelpConnection = NULL;
  MainFrame->OnClose();
  delete MainFrame;
  delete this;
  return TRUE;
}
