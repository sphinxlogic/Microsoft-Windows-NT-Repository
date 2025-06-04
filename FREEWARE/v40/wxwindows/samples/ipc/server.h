/*
 * File:	server.h
 * Purpose:	Server demo for wxWindows class library
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

// Define a new application
class MyApp: public wxApp
{
  public:
    wxFrame *OnInit(void);
};

// Define a new frame
class MyFrame: public wxFrame
{
  public:
    wxPanel *panel;

    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    Bool OnClose(void);
    void OnMenuCommand(int id);
};

// IPC stuff

class IPCDialogBox;
class MyConnection: public wxConnection
{
 public:
  IPCDialogBox *dialog;
  MyConnection(char *buf, int size);
  ~MyConnection(void);
  Bool OnExecute(char *topic, char *data, int size, int format);
  char *OnRequest(char *topic, char *item, int *size, int format);
  Bool OnPoke(char *topic, char *item, char *data, int size, int format);
};

class MyServer: public wxServer
{
 public:
  wxConnection *OnAcceptConnection(char *topic);
};

class IPCDialogBox: public wxDialogBox
{
 public:
 MyConnection *connection;
 IPCDialogBox(wxFrame *Parent, char *Title, Bool Modal, 
                         int x, int y, int width, int height, MyConnection *the_connection);
};

// Callbacks
void list_proc(wxListBox& listbox, wxCommandEvent& event);

#define SERVER_QUIT  1
