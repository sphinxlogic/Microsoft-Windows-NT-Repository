///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  This is the base WX-CGI system.
//
// Edit History
//
//	  Started January 1996 by Andrew Davison.
//
// Copyright (c) 1995-6 by Andrew Davison. Permission is granted to use the
// source in unmodified form quite freely, and to modify the source and use
// as part of a WXWINDOWS program only.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef WXCGI_H
#define WXCGI_H

class wxCGI_Frame: public wxFrame
{
public:

	wxCGI_Frame(char *title, int x=-1, int y=-1, int w=-1, int h=-1);

protected:

	Bool OnClose() { return FALSE; };
	void OnMenuCommand(int) {};
};

class wxCGI_App: public wxApp
{
public:

	wxCGI_App();
	virtual ~wxCGI_App();

	void ExitMainLoop() { running = FALSE; };

protected:

	const char* GetField(const char* name);
	const char* GetVariable(const char* name);
	void ParseContents(istream& istr, long nbytes);
	void ParseQuery(const char* src);

private:

	void Poll();
	wxFrame* OnInit();
	int MainLoop();
	virtual Bool OnRequest(istream&, ostream&) { return FALSE; };

	SockHandler handler;
	Bool running;
	wxFrame* frame;
	time_t last;
	};

#endif
