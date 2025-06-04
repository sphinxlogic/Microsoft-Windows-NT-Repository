/*
 * File:	client.h
 * Purpose:	Client demo for wxWindows class library
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

class MyConnection: public wxConnection
{
 public:
  MyConnection(void);
  ~MyConnection(void);
  Bool OnAdvise(char *topic, char *item, char *data, int size, int format);
};

class MyClient: public wxClient
{
 public:
  MyClient(void);
  wxConnection *OnMakeConnection(void);
};

#define CLIENT_QUIT     1
#define CLIENT_EXECUTE  2
#define CLIENT_REQUEST  3
#define CLIENT_POKE     4
