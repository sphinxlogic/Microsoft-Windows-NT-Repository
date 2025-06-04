///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  A World Wide Web server for wxWindows.
//
// Edit History
//
//	  Started December 7th 1995 by Andrew Davison.
//
// Copyright (c) 1995-6 by Andrew Davison. Permission is granted to use the
// source in unmodified form quite freely, and to modify the source and use
// as part of a WXWINDOWS program only.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef WXHTTPD_H
#define WXHTTPD_H

class HttpdFrame: public wxFrame
{
public:

	HttpdFrame(char* title, int x=-1, int y=-1, int w=-1, int h=-1);
	~HttpdFrame();

	Bool OnClose();
	void OnMenuCommand(int);

private:

	wxTextWindow* console;

	friend class HttpdApp;
};

///////////////////////////////////////////////////////////////////////////////

class HttpdApp: public wxApp
{
public:

	class HttpConnection
	{
	public:

		HttpConnection(SimSock* s);
		~HttpConnection();

	private:

		char cmd[1024];
		Bool simple, doget, dopost, doput, keep_alive, cgi;
		string filename, content_type, content_length, authorization;
		string origin_server, ssi_filename;
		long nbytes;
		HttpConnection* forward;
		SimSock* sock;
		istream* istr;
		time_t last;
		unsigned long count;

		friend class HttpdApp;
	};

	HttpdApp();
	~HttpdApp();

	void ExitMainLoop() { running = FALSE; };

private:

	wxFrame* OnInit();
	int MainLoop();
	void Poll();

        int ExecuteQuery(HttpConnection*, string& filename, const string& query);
	void ProcessGet(HttpConnection*);
	void ProcessPost(HttpConnection*);
	void ProcessPut(HttpConnection*);
	void ProcessHttpConnectionOrRequest();

	HttpdFrame* frame;
	SockHandler handler;
	ListOf connections;
	Bool running;
	static unsigned long count;
};

#endif
