/* wxdb.cc ODBC Example
You must first create a data source called "Tarif" from the control
panel (ODBC setup), pointing to the database in this directory.
*/

#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#if !USE_ODBC
#error You must set USE_ODBC to 1 in wx_setup.h to compile this demo.
#endif

#include "wx_db.h"

wxDatabase *Database = NULL;
wxRecordSet* Record = NULL;

// Define a new application type
// Define a new frame type
class MainFrame: public wxFrame
{ 
    public:
    wxListBox* DataSource;
    wxListBox* Tables;
    wxListBox* Columns;
    wxButton* execbut;
    wxButton* clearbut;
    wxMultiText* input;
    wxMultiText* output;
    MainFrame(wxFrame *frame, char *title, int x, int y, int w, int h, long);
    void OnMenuCommand(int id);
    void OnSize(int w, int h);
    void RefreshSources(void);
    Bool OnClose(void);
};

class ODBCApp: public wxApp
{
  MainFrame* frame;
  
  public:
  wxFrame *OnInit(void);
  MainFrame* GetMainFrame(void);
};

// ID for the menu quit command
#define IDM_QUIT 1
#define IDM_REFRESH_SOURCES 2

// This statement initializes the whole application and calls OnInit
ODBCApp theApp;

void list_proc_datasrc(wxListBox& list, wxCommandEvent& event)
{
  if (LB_ERR != list.GetSelection()) {
    Database->Close();
    Database->Open(list.GetString(list.GetSelection()));
    
    float odbcDLLVersion = Database->GetODBCVersionFloat(TRUE);
    float odbcHeaderVersion = Database->GetODBCVersionFloat(FALSE);
    
    if (odbcHeaderVersion != 0.0)
    {
      if (odbcHeaderVersion != odbcDLLVersion)
      {
        char buf[300];
        sprintf(buf, "Warning: ODBC DLLs are version %.2f, application expects version %.2f", odbcDLLVersion, odbcHeaderVersion);
        wxMessageBox(buf, "ODBC Demo Warning", wxOK);
      }
    }
    
    if (!Record->GetTables()) {
      char buf[300];
      sprintf(buf, "%s %s\n", Database->GetErrorClass(), Database->GetErrorMessage());
      theApp.GetMainFrame()->output->SetValue(buf);
    }
    else {
      theApp.GetMainFrame()->Tables->Clear();
      theApp.GetMainFrame()->Columns->Clear();
      do {
	if (Record->GetFieldDataPtr(2, SQL_VARCHAR))
	  theApp.GetMainFrame()->Tables->Append((char*)Record->GetFieldDataPtr(2, SQL_VARCHAR));
      } while (Record->MoveNext());
    }
  }
}

void list_proc_tables(wxListBox& list, wxCommandEvent& event)
{
  if (LB_ERR != list.GetSelection()) {
    Record->SetTableName(list.GetString(list.GetSelection()));
    if (!Record->GetColumns()) {
      char buf[300];
      sprintf(buf, "%s %s\n", Database->GetErrorClass(), Database->GetErrorMessage());
      theApp.GetMainFrame()->output->SetValue(buf);
    }
    else {
      theApp.GetMainFrame()->Columns->Clear();
#if 0
      int i = Record->GetNumberCols();
      while(i)
      	theApp.GetMainFrame()->Columns->Append(Record->GetColName(--i));
#endif      
      do {
	if (Record->GetFieldDataPtr(3, SQL_VARCHAR))
  	  theApp.GetMainFrame()->Columns->Append((char*)Record->GetFieldDataPtr(3, SQL_VARCHAR));
      } while (Record->MoveNext());
    }
  }
}

void list_proc_columns(wxListBox& list, wxCommandEvent& event)
{
}

void clear_button_proc(wxButton& but, wxCommandEvent& event)
{
  theApp.GetMainFrame()->input->SetValue("");
}

void exec_button_proc(wxButton& but, wxCommandEvent& event)
{
  if (!Record->ExecuteSQL(theApp.GetMainFrame()->input->GetValue())) {
    char buf[300];
    sprintf(buf, "%s %s\n", Database->GetErrorClass(), Database->GetErrorMessage());
    theApp.GetMainFrame()->output->SetValue(buf);
    return;
  }
  
  char buf[8192] = { 0 };
  
  do {
    for (int i = 0; i<Record->GetNumberCols(); i++) {
      switch (Record->GetColType(i)) {
	case SQL_CHAR:
	if (Record->GetFieldDataPtr(i, SQL_CHAR)) {
      	  strcat(buf, (char*)Record->GetFieldDataPtr(i, SQL_CHAR));
	  strcat(buf, " ");
	}
	break;
 	case SQL_VARCHAR:
	if (Record->GetFieldDataPtr(i, SQL_VARCHAR)) {
      	  strcat(buf, (char*)Record->GetFieldDataPtr(i, SQL_VARCHAR));
	  strcat(buf, " ");
	}
	break;
	case SQL_INTEGER:
	long val;
	if (Record->GetFieldDataPtr(i, SQL_INTEGER)) {
	  char valchr[20];
	  val = *(long*)Record->GetFieldDataPtr(i, SQL_INTEGER);
	  sprintf(valchr, "%ld ", val);
	  strcat(buf, valchr);
	}
	break;
	case SQL_TIMESTAMP:
        {
	  TIMESTAMP_STRUCT val;
	  if (Record->GetFieldDataPtr(i, SQL_TIMESTAMP)) {
            char valchr[50];
	    val = *(TIMESTAMP_STRUCT *)Record->GetFieldDataPtr(i, SQL_TIMESTAMP);
	    sprintf(valchr, "%d/%d/%d %d:%d:%d", (int)val.day, (int)val.month, (int)val.year, (int)val.hour,
              (int)val.minute, (int)val.second);
	    strcat(buf, valchr);
	  }
	  break;
        }
	case SQL_TIME:
        {
	  TIME_STRUCT val;
	  if (Record->GetFieldDataPtr(i, SQL_TIME)) {
            char valchr[50];
	    val = *(TIME_STRUCT *)Record->GetFieldDataPtr(i, SQL_TIME);
	    sprintf(valchr, "%d:%d:%d", (int)val.hour, (int)val.minute, (int)val.second);
	    strcat(buf, valchr);
	  }
	  break;
        }
	case SQL_DATE:
        {
	  DATE_STRUCT val;
	  if (Record->GetFieldDataPtr(i, SQL_DATE)) {
            char valchr[50];
	    val = *(DATE_STRUCT *)Record->GetFieldDataPtr(i, SQL_DATE);
	    sprintf(valchr, "%d/%d/%d", (int)val.day, (int)val.month, (int)val.year);
	    strcat(buf, valchr);
	  }
	  break;
        }
      }
    }
    strcat(buf, "\n");
  } while (Record->MoveNext());
  
  theApp.GetMainFrame()->output->SetValue(buf);
}

MainFrame* ODBCApp::GetMainFrame(void) {
  return frame;
}

// `Main program' equivalent, creating windows and returning main app frame
wxFrame *ODBCApp::OnInit(void)
{
  // Create the main frame window
  frame = new MainFrame(NULL, "wxWindows ODBC-Browser", 50, 50, 560, 500, wxSDI|wxCAPTION|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxSYSTEM_MENU|wxTHICK_FRAME);

  // Give it an icon
  frame->SetIcon(new wxIcon("aiai_icn"));

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(IDM_REFRESH_SOURCES, "&Refresh source list");
  file_menu->Append(IDM_QUIT, "E&xit");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "File");
  frame->SetMenuBar(menu_bar);

  // Make a panel with a message
  wxPanel *panel = new wxPanel(frame, 0, 0, 650, 450);
  panel->SetLabelPosition(wxVERTICAL);

  frame->DataSource = new wxListBox(panel, (wxFunction)&list_proc_datasrc, "Datasources",
                                  wxSINGLE|wxALWAYS_SB, 10, 30, 140, 200);

  frame->Tables = new wxListBox(panel, (wxFunction)&list_proc_tables, "Tables",
                                  wxSINGLE|wxALWAYS_SB, 160, 30, 140, 200);

  frame->Columns = new wxListBox(panel, (wxFunction)&list_proc_columns, "Columns",
                                  wxSINGLE|wxALWAYS_SB, 310, 30, 140, 200);

  frame->input = new wxMultiText(panel, NULL, "SQL", "", 10, 230, 440, 100);
  frame->output = new wxMultiText(panel, NULL, "OUT", "", 10, 340, 440, 100);

  frame->execbut = new wxButton(panel, (wxFunction)&exec_button_proc, "Execute", 455, 230, 70);
  frame->clearbut = new wxButton(panel, (wxFunction)&clear_button_proc, "Clear", 455, 255, 70);

  /**** Constraints added by JACS to lay out the subwindows properly
   **** 21/5/95
   ****/
  // Set constraints for panel subwindow

  wxLayoutConstraints *c = new wxLayoutConstraints;

  c->left.SameAs       (frame, wxLeft);
  c->top.SameAs        (frame, wxTop);
  c->right.SameAs      (frame, wxRight);
  c->bottom.SameAs     (frame, wxBottom);
//  c->height.PercentOf  (frame, wxHeight, 40);

  panel->SetConstraints(c);


  // Set constraints for data source list
  c = new wxLayoutConstraints;
  
  c->left.SameAs       (panel, wxLeft, 10);
  c->top.SameAs        (panel, wxTop, 10);
  c->width.PercentOf   (panel, wxWidth, 25);
  c->height.PercentOf  (panel, wxHeight, 40);

  frame->DataSource->SetConstraints(c);

  // Set constraints for tables list
  c = new wxLayoutConstraints;
  
  c->left.SameAs       (frame->DataSource, wxRight, 10);
  c->top.SameAs        (panel, wxTop, 10);
  c->width.PercentOf   (panel, wxWidth, 25);
  c->height.PercentOf  (panel, wxHeight, 40);

  frame->Tables->SetConstraints(c);

  // Set constraints for columns list
  c = new wxLayoutConstraints;
  
  c->left.SameAs       (frame->Tables, wxRight, 10);
  c->top.SameAs        (panel, wxTop, 10);
  c->width.PercentOf   (panel, wxWidth, 25);
  c->height.PercentOf  (panel, wxHeight, 40);

  frame->Columns->SetConstraints(c);
  
  // Set constraints for text input window
  c = new wxLayoutConstraints;
  
  c->left.SameAs       (panel, wxLeft, 10);
  c->top.Below         (frame->DataSource, 10);
  c->right.SameAs      (frame->Columns, wxRight);
  c->height.PercentOf  (panel, wxHeight, 20);

  frame->input->SetConstraints(c);

  // Set constraints for text output window
  c = new wxLayoutConstraints;
  
  c->left.SameAs       (panel, wxLeft, 10);
  c->top.Below         (frame->input, 10);
  c->right.SameAs      (frame->Columns, wxRight);
  c->bottom.SameAs     (panel, wxBottom, 10);

  frame->output->SetConstraints(c);

  // Set constraints for exec button
  c = new wxLayoutConstraints;
  
  c->left.SameAs       (frame->input, wxRight, 5);
  c->top.SameAs        (frame->input, wxTop);
  c->width.Absolute    (70);

  frame->execbut->SetConstraints(c);

  // Set constraints for exec button
  c = new wxLayoutConstraints;
  
  c->left.SameAs       (frame->execbut, wxLeft);
  c->top.SameAs        (frame->execbut, wxBottom, 5);
  c->width.Absolute    (70);

  frame->clearbut->SetConstraints(c);

/*
  if (!Database->Open("Tarif")) {
    wxMessageBox(Database->GetErrorMessage(), "Error", wxOK, frame);
  } 
*/

  frame->RefreshSources();

  // Show the frame
  frame->Show(TRUE);

  // Return the main frame window
  return frame;
}

// My frame constructor
MainFrame::MainFrame(wxFrame *frame, char *title, int x, int y, int w, int h, long s)
          :wxFrame(frame, title, x, y, w, h, s)
{
}

// Intercept menu commands
void MainFrame::OnMenuCommand(int id)
{
  switch (id) {
    case IDM_QUIT:
      if (OnClose())
        delete this;
      break;
    case IDM_REFRESH_SOURCES:
      RefreshSources();
      break;
  }
}

void MainFrame::OnSize(int w, int h)
{
  Layout();
}
  

void MainFrame::RefreshSources(void)
{
  DataSource->Clear();

  if (Record)
    delete Record;

  if (Database)
  {
    Database->Close();
    delete Database;
  }

  Database = new wxDatabase;
  
  Record = new wxRecordSet(Database, wxOPEN_TYPE_SNAPSHOT);

  if (!Record->GetDataSources()) {
    char buf[300];
    sprintf(buf, "%s %s\n", Database->GetErrorClass(), Database->GetErrorMessage());
    output->SetValue(buf);
  }
  else {
    do {
      DataSource->Append((char*)Record->GetFieldDataPtr(0, SQL_CHAR));
    } while (Record->MoveNext());
  }
}

Bool MainFrame::OnClose(void)
{
  if (Record)
    delete Record;
  if (Database)
  {
    Database->Close();
    delete Database;
  }
  return TRUE;
}

