/*
 * File:	client.cc
 * Purpose:	Client demo for wxWindows class library
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
#include "client.h"

MyFrame *frame = NULL;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initialises the whole application
MyApp myApp;

char ipc_buffer[4000];
wxListBox *the_list = NULL;

MyConnection *the_connection = NULL;
MyClient *my_client ;

// The `main program' equivalent, creating the windows and returning the
// main frame
wxFrame *MyApp::OnInit(void)
{
  // Create the main frame window
  frame = new MyFrame(NULL, "Client", 400, 0, 400, 300);

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

  file_menu->Append(CLIENT_EXECUTE, "Execute");
  file_menu->Append(CLIENT_REQUEST, "Request");
  file_menu->Append(CLIENT_POKE, "Poke");
  file_menu->Append(CLIENT_QUIT, "Quit");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "File");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  // Make a panel
  frame->panel = new wxPanel(frame, 0, 0, 400, 250);
  frame->panel->SetLabelPosition(wxVERTICAL);
  the_list = new wxListBox(frame->panel, (wxFunction)NULL, "A list",
                                  wxSINGLE, -1, -1, 150, 120);
  the_list->Append("Apple");
  the_list->Append("Pear");
  the_list->Append("Orange");
  the_list->Append("Banana");
  the_list->Append("Fruit");

  frame->panel->Fit();
  frame->Fit();

  // Initialize IPC
  wxIPCInitialize(); // (frame);

  char *server = "4242";
  char hostName[256];
  wxGetHostName(hostName, sizeof(hostName));

  char *host = hostName;

  if (argc > 1)
    server = argv[1];
  if (argc > 2)
    host = argv[2];

  // Create a new client
  my_client = new MyClient;
  the_connection = (MyConnection *)my_client->MakeConnection(host, server, "IPC TEST");

  if (!the_connection)
  {
    wxMessageBox("Failed to make connection to server", "Client Demo Error");
#ifdef wx_msw
    extern void wxPrintDDEError();
    wxPrintDDEError();
#endif
    return NULL;
  }
  the_connection->StartAdvise("Item");

  frame->Show(TRUE);

  // Essential - return the main frame window
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
    case CLIENT_EXECUTE:
    {
      if (the_connection)
        the_connection->Execute("Hello from the client!");
      break;
    }
    case CLIENT_POKE:
    {
      if (the_connection)
        the_connection->Poke("An item", "Some data to poke at the server!");
      break;
    }
    case CLIENT_REQUEST:
    {
      if (the_connection)
      {
        char *data = the_connection->Request("An item");
        if (data)
          wxMessageBox(data, "Client: Request", wxOK);
      }
      break;
    }
    case CLIENT_QUIT:
    {
      OnClose();
      if (my_client)
        delete my_client ;
      wxIPCCleanUp() ;
      delete this;
      break;
    }
  }
}

// Define the behaviour for the frame closing
Bool MyFrame::OnClose(void)
{
  if (the_connection)
  {
    the_connection->Disconnect();
  }
  return TRUE;
}

MyClient::MyClient(void)
{
}

wxConnection *MyClient::OnMakeConnection(void)
{
  return new MyConnection;
}

MyConnection::MyConnection(void):wxConnection(ipc_buffer, 3999)
{
}

MyConnection::~MyConnection(void)
{
  the_connection = NULL;
}

Bool MyConnection::OnAdvise(char *topic, char *item, char *data, int size, int format)
{
  if (the_list)
  {
    int n = the_list->FindString(data);
    if (n > -1)
      the_list->SetSelection(n);
  }
  return TRUE;
}

