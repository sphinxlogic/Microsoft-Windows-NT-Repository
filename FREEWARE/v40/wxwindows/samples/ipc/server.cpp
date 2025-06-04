/*
 * File:	server.cc
 * Purpose:	Server demo for wxWindows class library
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include "wx_ipc.h"
#include "server.h"

MyFrame *frame = NULL;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initialises the whole application
MyApp myApp;

char ipc_buffer[4000];
MyConnection *the_connection = NULL;
MyServer *my_server ;

// The `main program' equivalent, creating the windows and returning the
// main frame
wxFrame *MyApp::OnInit(void)
{
  // Create the main frame window
  frame = new MyFrame(NULL, "Server", 0, 0, 400, 500);

  frame->CreateStatusLine();

  // Give it an icon
#ifdef wx_msw
  wxIcon *icon = new wxIcon("conn_icn");
#endif
#ifdef wx_x
  wxIcon *icon = new wxIcon("aiai.xbm");
#endif
  frame->SetIcon(icon);

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(SERVER_QUIT, "Quit");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "File");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  // Make a panel
  frame->panel = new wxPanel(frame, 0, 0, 400, 250);
  frame->panel->SetLabelPosition(wxVERTICAL);
  wxListBox *list = new wxListBox(frame->panel, (wxFunction)&list_proc, "A list",
                                  wxSINGLE, -1, -1, 150, 120);
  list->Append("Apple");
  list->Append("Pear");
  list->Append("Orange");
  list->Append("Banana");
  list->Append("Fruit");

  frame->panel->Fit();
  frame->Fit();

  // Initialize IPC
  wxIPCInitialize();

  char *server_name = "4242";
  if (argc > 1)
    server_name = argv[1];

  // Create a new server
  my_server = new MyServer;
  my_server->Create(server_name);

  frame->Show(TRUE);

  // Return the main frame window
  return frame;
}

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{
  panel = NULL;
}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  switch (id)
  {
    case SERVER_QUIT:
    {
      OnClose();
      if (my_server)
        delete my_server ;
      wxIPCCleanUp() ;
      delete this;
      break;
    }
  }
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
Bool MyFrame::OnClose(void)
{
  return TRUE;
}

/*
 * IPC stuff
 *
 */

void IPCButtonQuit(wxButton& button, wxCommandEvent& event)
{
  IPCDialogBox *dialog = (IPCDialogBox *)button.GetParent();
  dialog->connection->Disconnect();
  delete dialog->connection;
}

IPCDialogBox::IPCDialogBox(wxFrame *Parent, char *Title, Bool Modal, 
                         int x, int y, int width, int height, MyConnection *the_connection):wxDialogBox(Parent, Title,
    Modal, x, y, width, height)
{
  connection = the_connection;
  (void)new wxButton(this, (wxFunction)IPCButtonQuit, "Quit this connection");
  Fit();
  Show(TRUE);
}

wxConnection *MyServer::OnAcceptConnection(char *topic)
{
  if (strcmp(topic, "STDIO") != 0 && strcmp(topic, "IPC TEST") == 0)
    return new MyConnection(ipc_buffer, 4000);
  else
    return NULL;
}

MyConnection::MyConnection(char *buf, int size):wxConnection(buf, size)
{
  dialog = new IPCDialogBox(frame, "Connection", FALSE, 100, 100, 500, 500, this);
  the_connection = this;
}

MyConnection::~MyConnection(void)
{
  delete dialog;
  the_connection = NULL;
}

Bool MyConnection::OnExecute(char *topic, char *data, int size, int format)
{
  char buf[300];
  sprintf(buf, "Execute command: %s", data);
  frame->SetStatusText(buf);
  return TRUE;
}

Bool MyConnection::OnPoke(char *topic, char *item, char *data, int size, int format)
{
  char buf[300];
  sprintf(buf, "Poke command: %s", data);
  frame->SetStatusText(buf);
  return TRUE;
}

char *MyConnection::OnRequest(char *topic, char *item, int *size, int format)
{
  return "Here, have your data, client!";
}

// Set the client process's listbox to this item
void list_proc(wxListBox& list, wxCommandEvent& event)
{
  char *value = list.GetStringSelection();
  if (the_connection)
  {
    the_connection->Advise("Item", value);
  }
}
