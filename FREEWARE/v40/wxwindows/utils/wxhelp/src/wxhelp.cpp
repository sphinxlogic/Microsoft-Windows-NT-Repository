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
#include "hytext.h"
#include "wxhelp.h"
#include "wxhlpblk.h"

float helpVersion = 1.5;

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
wxHyperTextMapping *mapping = NULL;
wxFont *swiss_font_12 = NULL;
char *hyFilename = NULL;
Bool hyEditMode = FALSE;

int SearchWindowX = 10;
int SearchWindowY = 10;

wxList hyHistory;
HypertextItem *hySelection = NULL;
wxNode *hyHistoryPointer = NULL;

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
IMPLEMENT_CLASS(HelpWindow, wxHyperTextWindow)
IMPLEMENT_CLASS(MyFrame, wxFrame)
IMPLEMENT_CLASS(SearchBox, wxDialogBox)
IMPLEMENT_CLASS(HelpConnection, wxConnection)
IMPLEMENT_CLASS(HelpServer, wxServer)

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initialises the whole application
MyApp     myApp;

void hyHelpPrevious(wxButton& but, wxCommandEvent& ev)
{
  MainFrame->window->DisplayPreviousSection();
  MainFrame->window->DisplaySection();
  long block_id = MainFrame->window->FindBlockForSection(MainFrame->window->current_section);
  MainFrame->window->StoreHypertextItem(block_id);
}

void hyHelpNext(wxButton& but, wxCommandEvent& ev)
{
  MainFrame->window->DisplayNextSection();
  MainFrame->window->DisplaySection();
  long block_id = MainFrame->window->FindBlockForSection(MainFrame->window->current_section);
  MainFrame->window->StoreHypertextItem(block_id);
}

void hyHelpBack(wxButton& but, wxCommandEvent& ev)
{
  MainFrame->window->HistoryBack();
}

void hyHelpHistory(wxButton& but, wxCommandEvent& ev)
{
}

void hyHelpContents(wxButton& but, wxCommandEvent& ev)
{
  MainFrame->window->DisplayFileAtTop();
  long block_id = MainFrame->window->FindBlockForSection(MainFrame->window->current_section);
  MainFrame->window->StoreHypertextItem(block_id);
}

void hyHelpSearch(wxButton& but, wxCommandEvent& ev)
{
  HelpSearch();
}

// The `main program' equivalent, creating the windows and returning the
// main frame
wxFrame *MyApp::OnInit(void)
{
  int i = 1;
  
  // Read input file if given without using -f
  if (argc > 1 && (argv[1][0] != '-'))
  {
    hyFilename = copystring(argv[1]);
    i ++;
  }

  while (i < argc)
  {
    if (strcmp(argv[i], "-f") == 0)
    {
      i ++;
      if (i == argc)
        hyErrorMsg("-f must take a filename!");
      else
      {
        hyFilename = copystring(argv[i]);
        i ++;
      }
    }
    else if (strcmp(argv[i], "-edit") == 0)
    {
      i ++;
      hyEditMode = TRUE;
    }
    else if (strcmp(argv[i], "-server") == 0)
    {
      i ++;
      TheHelpServerId = atoi(argv[i]);
      i ++;
    }
    else i++;
  }

  if (hyFilename)
  {
    int len = strlen(hyFilename);
    if (!(hyFilename[len-1] == 'p' && hyFilename[len-2] == 'l' && hyFilename[len-3] == 'x' && hyFilename[len-4] == '.'))
    {
      char buf[400];
      strcpy(buf, hyFilename);
      strcat(buf, ".xlp");
      delete[] hyFilename;
      hyFilename = copystring(buf);
    }
  }
  
  HelpPathList.AddEnvList("WXHELPFILES");
  HelpPathList.AddEnvList("PATH");

  // Create the main frame window
  MainFrame = new MyFrame(NULL, "wxHelp", 0, 0, 500, 400);

  if (hyEditMode)
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
  if (hyEditMode)
    file_menu->Append(HELP_SAVE, "&Save File");

  file_menu->AppendSeparator();
  file_menu->Append(HELP_EXIT, "&Exit");

  wxMenu *edit_menu = NULL;
  wxMenu *block_menu = NULL;
  if (hyEditMode)
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

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(HELP_HELP_CONTENTS, "&Help on wxHelp");
  help_menu->AppendSeparator();
  help_menu->Append(HELP_ABOUT, "&About wxHelp");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");

  if (hyEditMode)
  {
    menu_bar->Append(edit_menu, "&Edit");
    menu_bar->Append(block_menu, "&Blocks");
  }

  menu_bar->Append(help_menu, "&Help");

  // Associate the menu bar with the frame
  MainFrame->SetMenuBar(menu_bar);

  // Make a button bar, a la Windows Help
  MainFrame->panel = new wxPanel(MainFrame, 0, 0, 400, HELP_PANEL_HEIGHT);

  (void)new wxButton(MainFrame->panel, (wxFunction)hyHelpContents, "Contents");
  (void)new wxButton(MainFrame->panel, (wxFunction)hyHelpSearch, "Search");
//  (void)new wxButton(MainFrame->panel, (wxFunction)hyHelpHistory, "History");
  (void)new wxButton(MainFrame->panel, (wxFunction)hyHelpBack, "Back");
  (void)new wxButton(MainFrame->panel, (wxFunction)hyHelpPrevious, "<<");
  (void)new wxButton(MainFrame->panel, (wxFunction)hyHelpNext, ">>");

  // Make a hypertext window
  MainFrame->window = new HelpWindow(MainFrame, 0, 0, 400, 400, 0);
  MainFrame->window->SetEditMode(hyEditMode);
  MainFrame->window->SetIndexWriting(TRUE);

  mapping = new wxHyperTextMapping;
  // This recognized by library as the default text
  mapping->AddMapping(500, 12, wxSWISS, wxNORMAL, wxNORMAL, "BLACK", "Default");

  mapping->AddMapping(hyBLOCK_NORMAL, -1, -1, -1, -1, NULL, "Normal text");
  mapping->AddMapping(hyBLOCK_RED, -1, -1, -1, -1, "RED", "Red");
  mapping->AddMapping(hyBLOCK_BLUE, -1, -1, -1, -1, "BLUE", "Blue");
  mapping->AddMapping(hyBLOCK_GREEN, -1, -1, -1, -1, "FOREST GREEN", "Green");
  mapping->AddMapping(hyBLOCK_LARGE_HEADING, 18, wxSWISS, wxNORMAL, wxBOLD, "BLACK", "Large heading");
  mapping->AddMapping(hyBLOCK_SMALL_HEADING, 14, wxSWISS, wxNORMAL, wxBOLD, "BLACK", "Small heading");
  mapping->AddMapping(hyBLOCK_ITALIC, -1, -1, wxITALIC, -1, NULL, "Italic");
  mapping->AddMapping(hyBLOCK_BOLD, -1, -1, -1, wxBOLD, NULL, "Bold");
  mapping->AddMapping(hyBLOCK_RED_ITALIC, -1, -1, wxITALIC, -1, "RED", "Red italic");
  mapping->AddMapping(hyBLOCK_INVISIBLE_SECTION, -1, -1, -1, wxITALIC, "RED", "Invisible section marker", wxHYPER_SECTION, FALSE);
  mapping->AddMapping(hyBLOCK_LARGE_VISIBLE_SECTION, 18, -1, -1, wxBOLD, "BLACK", "Large visible section", wxHYPER_SECTION, TRUE);
  mapping->AddMapping(hyBLOCK_SMALL_VISIBLE_SECTION, 14, -1, -1, wxBOLD, "BLACK", "Small visible section", wxHYPER_SECTION, TRUE);
  mapping->AddMapping(hyBLOCK_SMALL_TEXT, 10, -1, -1, -1, NULL, "Small text");
  mapping->AddMapping(hyBLOCK_TELETYPE, -1, wxMODERN, -1, -1, NULL, "Teletype");

  MainFrame->window->SetMapping(mapping);

  if (hyFilename)
  {
    char *s = HelpPathList.FindValidPath(hyFilename);
    if (!s)
    {
      MainFrame->SetTitle("wxHelp (no title)");
      char buf[500];
      sprintf(buf, "Cannot find file %s.\nTrying editing the WXHELPFILES environment variable.", hyFilename);
#ifdef wx_x
      cerr << buf << "\n";
#endif
#ifdef wx_msw
      wxMessageBox(buf);
#endif
    }
    else
    {
      MainFrame->window->LoadFile(s);
      if (MainFrame->window->GetTitle())
        MainFrame->SetTitle(MainFrame->window->GetTitle());
      else
        MainFrame->SetTitle("wxHelp (no title)");

      MainFrame->window->DisplayFileAtTop();
      MainFrame->window->StoreHypertextItem(-1);
      MainFrame->window->DisplaySection();
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
    case HELP_SET_TITLE:
    {
      char *def = "";
      if (window->GetTitle())
        def = window->GetTitle();
      char *title = wxGetTextFromUser("Enter help file title", "Text input",
                    def);
      if (title)
      {
        window->SetTitle(title);
        SetTitle(title);
        window->modified = TRUE;
      }
      break;
    }
    case HELP_OPEN:
    {
      // First save any edits in the current text window
      if (window->Modified())
      {
        int choice = wxMessageBox("Save edits (Yes), Discard edits (No)?",
                     "Message", wxYES_NO, this);
        if (choice == wxYES)
        {
          char *file = wxFileSelector("Select a help file to save to", PathOnly(hyFilename),  FileNameFromPath(hyFilename), "xlp", "*.xlp",
            wxSAVE | wxOVERWRITE_PROMPT);
          if (file)
	  {
            hyFilename = copystring(file);
            window->SaveFile(hyFilename);
	  }
        }
      }
      char *s = wxFileSelector("Load hypertext file", NULL, NULL, NULL, "*.xlp");
      if (s)
      {
        hyFilename = copystring(s);
        window->LoadFile(HelpPathList.FindValidPath(hyFilename));
        if (window->GetTitle())
          SetTitle(window->GetTitle());
        else
          SetTitle("wxHelp (no title)");
        window->DisplayFileAtTop();
        window->StoreHypertextItem(-1);
        window->DisplaySection();
      }
      break;
    }
    case HELP_SAVE:
    {
      char *s = wxFileSelector("Save hypertext file", PathOnly(hyFilename), FileNameFromPath(hyFilename), "txt", "*.xlp",
      wxSAVE | wxOVERWRITE_PROMPT);
      if (s)
        window->SaveFile(s);
      break;
    }

    case HELP_RUN_EDITOR:
    {
      // First save any edits in the current text window
      if (window->Modified())
      {
        int choice = wxMessageBox("Save edits (Yes), Discard edits (No)?",
                     "Message", wxYES_NO, this);
        if (choice == wxYES)
        {
          char *file = wxFileSelector("Select a help file to save to", PathOnly(hyFilename),  FileNameFromPath(hyFilename), "xlp", "*.xlp",
          wxSAVE | wxOVERWRITE_PROMPT);
          if (file)
	  {
            hyFilename = copystring(file);
            window->SaveFile(hyFilename);
	  }
        }
      }

      char *editor = getenv("EDITOR");
      if (!editor)
        editor = DEFAULT_EDITOR;

      char buf[300];
      if (!hyFilename)
      {
        char *s = wxFileSelector("Select a help file to edit", NULL, NULL, "xlp", "*.xlp");
        if (s)
          hyFilename = copystring(s);
      }
      if (hyFilename)
      {
        window->SaveSection();
        strcpy(buf, editor);
        strcat(buf, " ");
        strcat(buf, hyFilename);
        wxExecute(buf);

        // In Windows, it returns immediately so we can't automatically
        // redisplay the file
#ifdef wx_x
        window->LoadFile(HelpPathList.FindValidPath(hyFilename));
        if (window->GetTitle())
          SetTitle(window->GetTitle());
        else
          SetTitle("wxHelp (no title)");
        window->RestoreSection();
        window->DisplaySection();
#endif
      }
      break;
    }

    case HELP_EXIT:
    {
      OnClose();
      if (mapping)
        delete mapping ;
      if (TheHelpServer)
        delete TheHelpServer ;
      if (HelpInstance)
        delete HelpInstance ;
      wxNode *node = hyHistory.First();
      while (node)
      {
        HypertextItem *item = (HypertextItem *)node->Data();
        wxNode *next = node->Next();
        delete item;
        delete node;
        node = next;
      }
      delete this;
      break;
    }

    case HELP_MARK_RED:
    {
      long id = window->GetFirstSelection();
      while (id != -1)
      {
        window->SetBlockType(id, hyBLOCK_RED);
        window->SelectBlock(id, FALSE);
        id = window->GetNextSelection();
      }
      window->SaveSection();
      window->Compile();
      window->RestoreSection();
      window->DisplaySection();
      break;
    }
    case HELP_MARK_BLUE:
    {
      long id = window->GetFirstSelection();
      while (id != -1)
      {
        window->SetBlockType(id, hyBLOCK_BLUE);
        window->SelectBlock(id, FALSE);
        id = window->GetNextSelection();
      }
      window->SaveSection();
      window->Compile();
      window->RestoreSection();
      window->DisplaySection();
      break;
    }

    case HELP_MARK_ITALIC:
    {
      long id = window->GetFirstSelection();
      while (id != -1)
      {
        window->SetBlockType(id, hyBLOCK_ITALIC);
        window->SelectBlock(id, FALSE);
        id = window->GetNextSelection();
      }
      window->SaveSection();
      window->Compile();
      window->RestoreSection();
      window->DisplaySection();
      break;
    }

    case HELP_MARK_RED_ITALIC:
    {
      long id = window->GetFirstSelection();
      while (id != -1)
      {
        window->SetBlockType(id, hyBLOCK_RED_ITALIC);
        window->SelectBlock(id, FALSE);
        id = window->GetNextSelection();
      }
      window->SaveSection();
      window->Compile();
      window->RestoreSection();
      window->DisplaySection();
      break;
    }

    case HELP_MARK_BOLD:
    {
      long id = window->GetFirstSelection();
      while (id != -1)
      {
        window->SetBlockType(id, hyBLOCK_BOLD);
        window->SelectBlock(id, FALSE);
        id = window->GetNextSelection();
      }
      window->SaveSection();
      window->Compile();
      window->RestoreSection();
      window->DisplaySection();
      break;
    }
    case HELP_MARK_SMALL_TEXT:
    {
      long id = window->GetFirstSelection();
      while (id != -1)
      {
        window->SetBlockType(id, hyBLOCK_SMALL_TEXT);
        window->SelectBlock(id, FALSE);
        id = window->GetNextSelection();
      }
      window->SaveSection();
      window->Compile();
      window->RestoreSection();
      window->DisplaySection();
      break;
    }

    case HELP_MARK_LARGE_HEADING:
    {
      long id = window->GetFirstSelection();
      while (id != -1)
      {
        window->SetBlockType(id, hyBLOCK_LARGE_HEADING);
        window->SelectBlock(id, FALSE);
        id = window->GetNextSelection();
      }
      window->SaveSection();
      window->Compile();
      window->RestoreSection();
      window->DisplaySection();
      break;
    }
    case HELP_MARK_SMALL_HEADING:
    {
      long id = window->GetFirstSelection();
      while (id != -1)
      {
        window->SetBlockType(id, hyBLOCK_SMALL_HEADING);
        window->SelectBlock(id, FALSE);
        id = window->GetNextSelection();
      }
      window->SaveSection();
      window->Compile();
      window->RestoreSection();
      window->DisplaySection();
      break;
    }
    case HELP_MARK_INVISIBLE_SECTION:
    {
      window->SaveSection();
      long id = window->GetFirstSelection();
      while (id != -1)
      {
        window->SetBlockType(id, hyBLOCK_INVISIBLE_SECTION);
        window->SelectBlock(id, FALSE);
        id = window->GetNextSelection();
      }
      window->SaveSection();
      window->Compile();
      window->RestoreSection();
      window->DisplaySection();
      break;
    }
    case HELP_MARK_LARGE_VISIBLE_SECTION:
    {
      window->SaveSection();
      long id = window->GetFirstSelection();
      while (id != -1)
      {
        window->SetBlockType(id, hyBLOCK_LARGE_VISIBLE_SECTION);
        window->SelectBlock(id, FALSE);
        id = window->GetNextSelection();
      }
      window->SaveSection();
      window->Compile();
      window->RestoreSection();
      window->DisplaySection();
      break;
    }
    case HELP_MARK_SMALL_VISIBLE_SECTION:
    {
      window->SaveSection();
      long id = window->GetFirstSelection();
      while (id != -1)
      {
        window->SetBlockType(id, hyBLOCK_SMALL_VISIBLE_SECTION);
        window->SelectBlock(id, FALSE);
        id = window->GetNextSelection();
      }
      window->SaveSection();
      window->Compile();
      window->RestoreSection();
      window->DisplaySection();
      break;
    }
    case HELP_CLEAR_SELECTION:
    {
      long id = window->GetFirstSelection();
      while (id != -1)
      {
        window->SelectBlock(id, FALSE);
        id = window->GetNextSelection();
      }
      window->SaveSection();
      window->Compile();
      window->RestoreSection();
      window->DisplaySection();
      break;
    }

    case HELP_CLEAR_BLOCK:
    {
      window->SaveSection();
      long id = window->GetFirstSelection();
      while (id != -1)
      {
        window->SelectBlock(id, FALSE);

        window->ClearBlock(id);
        id = window->GetNextSelection();
      }
      window->SaveSection();
      window->Compile();
      window->RestoreSection();
      window->DisplaySection();
      break;
    }

/*
    case HELP_TEST:
    {
      wxDC *dc = MainFrame->window->GetDC();
      dc->SetFont(swiss_font_12);

      char **choices = new char*[wxTheColourDatabase->Number()];
      wxNode *node = wxTheColourDatabase->First();
      int i = 0;
      while (node)
      {
        choices[i] = node->key.string;
        node = node->Next();
        i ++;
      }
      char *col = wxGetSingleChoice("Choose a colour", "Choice",
          wxTheColourDatabase->Number(), choices);
      if (col)
      {
        dc->SetTextBackground(wxWHITE);
        dc->SetTextForeground(wxTheColourDatabase->FindColour(col));
        dc->DrawText("Hello, some test text", 30, 30);
      }
      break;
    }
*/
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
      strcat(buf, "by Julian Smart, AIAI (c) April 1993\n");
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
  // First save any edits in the current text window
  if (window->Modified())
  {
    int choice = wxMessageBox("Save edits (Yes), Discard edits (No)?",
                 "Message", wxYES_NO | wxCANCEL, this);
    if (choice == wxYES)
    {
      char *file = wxFileSelector("Select a help file to save to", PathOnly(hyFilename),  FileNameFromPath(hyFilename), "xlp", "*.xlp",
      wxSAVE | wxOVERWRITE_PROMPT);
      if (file)
      {
        hyFilename = copystring(file);
        window->SaveFile(hyFilename);
      } else return FALSE;
    } else if (choice == wxCANCEL) return FALSE;
  }
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
  }
}

HelpWindow::HelpWindow(wxFrame *frame, int x, int y, int w, int h, long style):
  wxHyperTextWindow(frame, x, y, w, h, style)
{
}

HelpWindow::~HelpWindow(void)
{
}

void HelpWindow::OnSelectBlock(long block_id, Bool select)
{
  if (select)
  {
    if (hySelection)
      delete hySelection;
    hySelection = new HypertextItem(hyFilename, block_id);
  }
  else
  {
    if (hySelection && (hySelection->block_id == block_id))
    {
      delete hySelection;
      hySelection = NULL;
    }
  }
}

void HelpWindow::ClearBlock(long block_id)
{
  wxHyperTextWindow::ClearBlock(block_id);
  HypertextItem *item = (HypertextItem *)GetLinkTable()->Get(block_id);
  if (item)
  {
    long other_block = item->block_id;
    if (other_block != 0)
    {
      (void)GetLinkTable()->Delete(block_id);
      delete item;
    }
  }
}

// Select/deselect block if SHIFT held down (default behaviour),
// or traverse a link otherwise.
void HelpWindow::OnLeftClick(float x, float y, int char_pos, int line, long block_id, int keys)
{
  if (keys & KEY_SHIFT)
  {
    wxHyperTextWindow::OnLeftClick(x, y, char_pos, line, block_id, keys);
    return;
  }
  else if ((keys & KEY_CTRL) && block_id > -1 && hyEditMode)
  {
    // Display some useful info
    int type = GetBlockType(block_id);

    wxNode *node = mapping->Find(type);
    wxHTMappingStructure *struc = (wxHTMappingStructure *)node->Data();

    char *linked = " (not linked)";
    if (GetLinkTable()->Get(block_id))
      linked = " (linked)";
    char buf[300];

    sprintf(buf, "%s %s", struc->name, linked);
    MainFrame->SetStatusText(buf);
  }
  else if (block_id > -1)
  {
    HypertextItem *item = (HypertextItem *)GetLinkTable()->Get(block_id);
    if (!item)
      return;

    long other_block = item->block_id;
    if (other_block > -1)
    {
      if (item->filename)
      {
        char *s = HelpPathList.FindValidPath(item->filename);
        if (s)
        {
          if (!hyFilename || (strcmp(hyFilename, s) != 0))
          {
            if (hyFilename)
              delete[] hyFilename;
            hyFilename = copystring(s);
            LoadFile(hyFilename);
            if (GetTitle())
              MainFrame->SetTitle(GetTitle());
            else
              MainFrame->SetTitle("wxHelp (no title)");
          }
        }
        else
        {
          char buf[500];
          sprintf(buf, "Could not find file %s.\nTry editing the WXHELPFILES environment variable", item->filename);
          (void)wxMessageBox(buf, "wxHelp Error");
          return;
        }
      }
      DisplayFileAt(other_block);
      StoreHypertextItem(other_block);
      DisplaySection();
    }
  }
}

// Popup a menu for deleting block, linking with other items,
// etc.
void HelpWindow::OnRightClick(float x, float y, int char_pos, int line, long block_id, int keys)
{
  if (!hyEditMode)
    return;

  int block_type = GetBlockType(block_id);
  if (block_type == BLOCK_TYPE_SELECTION)
    return;

  int x1, y1, x2, y2;
  MainFrame->GetPosition(&x2, &y2);

  ViewStart(&x1, &y1);
  int mouse_x = (int)(GetDC()->LogicalToDeviceX(x - x1) + x2);
  int mouse_y = (int)(GetDC()->LogicalToDeviceY(y - y1) + y2);

  char **choices = new char *[10];
  int no_choices = 0;

  choices[no_choices] = "Link block to selection";
  no_choices ++;
  choices[no_choices] = "Unlink block";
  no_choices ++;
  choices[no_choices] = "Clear block";
  no_choices ++;

  char *choice = wxGetSingleChoice("Pick a block action", "Menu",
                                   no_choices, choices, MainFrame, mouse_x, mouse_y);
  if (!choice)
  {
    delete choices;
    return;
  }

  if (strcmp(choice, "Link block to selection") == 0)
  {
    if (hySelection)
    {
      if (GetLinkTable()->Get(block_id))
        MainFrame->SetStatusText("This block already linked!");
      else if (hySelection->block_id > -1)
      {
        GetLinkTable()->Put(block_id,
          new HypertextItem(hySelection->filename, hySelection->block_id));
        modified = TRUE;
        SelectBlock(hySelection->block_id, FALSE);
        Compile();
        DisplayFile();
      }
    }
  }
  else if (strcmp(choice, "Unlink block") == 0)
  {
    HypertextItem *item = (HypertextItem *)GetLinkTable()->Get(block_id);
    if (!item)
      MainFrame->SetStatusText("Nothing to unlink.");
    else
    {
      (void)GetLinkTable()->Delete(block_id);
      delete item;
      modified = TRUE;
      MainFrame->SetStatusText("Unlinked block.");
    }
  }
  else if (strcmp(choice, "Clear block") == 0)
  {
    SaveSection();
    HypertextItem *item = (HypertextItem *)GetLinkTable()->Get(block_id);
    if (item)
    {
      (void)GetLinkTable()->Delete(block_id);
      delete item;
    }

    SelectBlock(block_id, FALSE);
    ClearBlock(block_id);
    modified = TRUE;
    Compile();
    RestoreSection();
  }

  delete choices;
}

void HelpWindow::DisplaySection(void)
{
  if (hyEditMode)
  {
    char buf[300];
    int sect = GetCurrentSectionNumber();
    sprintf(buf, "Section %d", sect);
    MainFrame->SetStatusText(buf);
  }
}

void HelpWindow::StoreHypertextItem(long block_id)
{
  wxNode *last = hyHistory.Last();
  // Don't store if the same as last time!
  if (last)
  {
    HypertextItem *ep = (HypertextItem *)last->Data();
    if (ep->filename && hyFilename &&
        ((strcmp(ep->filename, hyFilename) == 0) && ep->block_id == block_id))
      return;
  }
  hyHistory.Append(new HypertextItem(hyFilename, block_id));
  hyHistoryPointer = hyHistory.Last();
}

void HelpWindow::HistoryBack(void)
{
  if (!hyHistoryPointer)
    hyHistoryPointer = hyHistory.First();

  hyHistoryPointer = hyHistoryPointer->Previous();
  if (!hyHistoryPointer)
    hyHistoryPointer = hyHistory.First();

  HypertextItem *ep = (HypertextItem *)hyHistoryPointer->Data();
  char *s = ep->filename;
  if (s && strcmp(s, hyFilename) != 0)
  {
    LoadFile(HelpPathList.FindValidPath(s));
    if (GetTitle())
      MainFrame->SetTitle(GetTitle());
    else
      MainFrame->SetTitle("wxHelp (no title)");
    hyFilename = copystring(s);
  }
  DisplayFileAt(ep->block_id);
}

void HelpWindow::DisplayHistory(void)
{
}


void hyErrorMsg(char *msg)
{
#ifdef wx_x
  if (myApp.Initialized())
    (void)wxMessageBox(msg, "Error", wxOK);
  else
    cerr << "Error: " << msg << "\n";
#endif
#ifdef wx_msw
    (void)wxMessageBox(msg, "Error", wxOK);
#endif
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


void hyDoSearch(wxButton& button, wxCommandEvent& event)
{
  SearchBox *box = (SearchBox *)button.GetParent();
  box->DoSearch();
}

void hySearchQuit(wxButton& button, wxCommandEvent& event)
{
  SearchBox *box = (SearchBox *)button.GetParent();
//  box->GetPosition(&SearchWindowX, &SearchWindowY);
  TheSearchBox = NULL;
  delete box;
}

void hySearchSelection(wxListBox& listbox, wxCommandEvent& event)
{
  int sel = listbox.GetSelection();
  long block_id = (long)listbox.GetClientData(sel);
  MainFrame->window->DisplayFileAt(block_id);
  MainFrame->window->StoreHypertextItem(block_id);
  MainFrame->window->DisplaySection();
  MainFrame->Iconize(FALSE);
  MainFrame->Show(TRUE);
}

// Search window
SearchBox::SearchBox(wxFrame *parent):
  wxDialogBox(parent, "Search", FALSE, SearchWindowX, SearchWindowY, 500, 500,
    wxDEFAULT_DIALOG_STYLE)
{
  SetClientSize(500, 300);

  (void)new wxButton(this, (wxFunction)&hySearchQuit, "Quit");

  search_item = new wxText(this, (wxFunction)NULL, "Search string", "*", -1, -1, 400, -1);
  NewLine();

  (void)new wxButton(this, (wxFunction)&hyDoSearch, "Do Search");
  NewLine();

  titles_item = new wxListBox(this, (wxFunction)&hySearchSelection,
                             "Items", wxSINGLE, -1, -1, 400, 200);

  Fit();
}

wxList *FindMatchingStrings(char *search_string1, Bool subString = TRUE)
{
  wxCursor *old_cursor = MainFrame->window->SetCursor(wxHOURGLASS_CURSOR);
  ::wxSetCursor(wxHOURGLASS_CURSOR);

  char buffer[300];
  char *search_string = copystring(search_string1);

  int i;
  for (i = 0; i < (int)strlen(search_string); i ++)
    search_string[i] = toupper(search_string[i]);

  wxList *string_list = new wxList;

  wxNode *node = MainFrame->window->text_chunks.First();
  while (node)
  {
    wxTextChunk *chunk = (wxTextChunk *)node->Data();
    if ((chunk->special_attribute == wxHYPER_SECTION) && chunk->visibility)
    {
      MainFrame->window->GetBlockText(buffer, 300, node, chunk->block_id);
      char *text = copystring(buffer);

      for (i = 0; i < (int)strlen(buffer); i ++)
        buffer[i] = toupper(buffer[i]);

      if (search_string && (search_string[0] == '*' || StringMatch(search_string, buffer, subString)))
        string_list->Append(new StringData(text, (wxObject *)chunk->block_id));
    }
    node = node->Next();
  }

  SortStringDataList(string_list);
  delete search_string;

  MainFrame->window->SetCursor(old_cursor);
  ::wxSetCursor(wxSTANDARD_CURSOR);

  return string_list;
}

void DeleteMatchingStrings(wxList *string_list)
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
    string_list = FindMatchingStrings(search_string);

  wxNode *node = string_list->First();
  while (node)
  {
    StringData *sd = (StringData *)node->Data();
    titles_item->Append(sd->s, (char *)sd->data);

    node = node->Next();
  }
  DeleteMatchingStrings(string_list);

  titles_item->Show(TRUE);
  delete[] search_string;

  MainFrame->window->SetCursor(old_cursor);
  ::wxSetCursor(wxSTANDARD_CURSOR);
}

void HelpSearch(void)
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
        break;
      }
      case 'b': // Display at block
      {
        long block_no = atol(data + 2);
        MainFrame->window->DisplayFileAt(block_no);
        MainFrame->window->StoreHypertextItem(block_no);
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
          strcat(buf, ".xlp");
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

        if (!hyFilename || (strcmp(s, hyFilename) != 0))
	{
          if (hyFilename) delete[] hyFilename;
          hyFilename = copystring(s);

          Bool succ = MainFrame->window->LoadFile(hyFilename);
          MainFrame->window->StoreHypertextItem(-1);
          MainFrame->Iconize(FALSE);
          MainFrame->Show(TRUE);
          return succ;
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
        wxList *string_list = FindMatchingStrings(str, FALSE);
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
          DeleteMatchingStrings(string_list);

          MainFrame->window->DisplayFileAt(block_id);
          MainFrame->window->StoreHypertextItem(block_id);
          MainFrame->window->DisplaySection();
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
