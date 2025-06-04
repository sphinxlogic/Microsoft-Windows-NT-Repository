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

#include <wx_prec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx.h>
#endif

#include <fstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "listof.h"
#include "simsock.h"
#include "strsock.h"
#include "wxhtml.h"
#include "wxhttpd.h"

#ifdef BSD
#define SPRINTF(exp) strlen(sprintf exp)
#else
#define SPRINTF(exp) sprintf exp
#endif

#define WXWEB_EXIT 1
#define WXWEB_LOG 2

static int fastcgi_timeout = 300;

const KEEPALIVE_TIMEOUT = 60;		// hold connection after request (seconds)
const KEEPALIVE_REQUESTS = 256;		// max requests per session, 0=unlimited

static const char* STATUS_400 = "<HTML><HEAD><TITLE>Error</TITLE></HEAD><BODY><H1>Status 400 Bad Request</H1></BODY></HTML>\n";
static const char* STATUS_403 = "<HTML><HEAD><TITLE>Error</TITLE></HEAD><BODY><H1>Status 403 Forbidden</H1></BODY></HTML>\n";
static const char* STATUS_404 = "<HTML><HEAD><TITLE>Error</TITLE></HEAD><BODY><H1>Status 404 Not Found</H1></BODY></HTML>\n";

#define FASTCGI_PORT "7000"

#ifndef TZSET
#define TZSET tzset
#endif

static ofstream* logfile = 0;
static Bool logging = FALSE;
static SockServer server("80", SimSock::NONE, '\n');

//////////////////////////////////////////////////////////////

static void log(const char* msg)
{
	if (!logging) return;

	time_t now = time(0);
	char *ptr = asctime(gmtime(&now));
	*strchr(ptr, '\n') = 0;
	*logfile << ptr << " GMT ";

	if ((ptr = (char*)strrchr(msg, '\r')) != 0)
		*ptr = ' ';

	*logfile << msg << flush;
}

///////////////////////////////////////////////////////////////////////////////

HttpdApp::HttpConnection::HttpConnection(SimSock* s)
{
	sock = s;
	time(&last);
	forward = 0;
	istr = 0;
	nbytes = 0;
	cgi = keep_alive = simple = doget = dopost = FALSE;
}

///////////////////////////////////////////////////////////////////////////////

HttpdApp::HttpConnection::~HttpConnection()
{
	if (sock) delete sock;
	if (istr) delete istr;
	if (forward) forward->forward = 0;
	sock = 0;
	istr = 0;
	forward = 0;
}

///////////////////////////////////////////////////////////////////////////////

HttpdFrame::HttpdFrame(char* title, int x, int y, int w, int h):
	wxFrame(0, title, x, y, w, h)
{
	console = 0;
}

///////////////////////////////////////////////////////////////////////////////

HttpdFrame::~HttpdFrame()
{
}

unsigned long HttpdApp::count = 0;


///////////////////////////////////////////////////////////////////////////////

HttpdApp::HttpdApp()
{
#ifdef wx_msw
	wxGetResource("WXHTTPD", "ApplicationTimeout", &fastcgi_timeout);
#endif

	handler.Register(&server);
}

///////////////////////////////////////////////////////////////////////////////

HttpdApp::~HttpdApp()
{
#ifdef wx_msw
	wxWriteResource("WXHTTPD", "ApplicationTimeout", fastcgi_timeout);
#endif

	ListOfIterator ilist(connections);
	void* t;

	for (int ok = ilist.First(t); ok; ok = ilist.Next(t))
	{
		HttpConnection* item = (HttpConnection*)t;
		handler.DeRegister(item->sock);
		delete item;
	}

	handler.DeRegister(&server);
}

///////////////////////////////////////////////////////////////////////////////

wxFrame* HttpdApp::OnInit()

{
	TZSET();

	frame = new HttpdFrame("wxHTTPd", 10, 10, 450, 200);

	wxMenu* file_menu = new wxMenu;
	file_menu->Append(WXWEB_LOG, "&Logging on/off");
	file_menu->AppendSeparator();
	file_menu->Append(WXWEB_EXIT, "E&xit");

	wxMenuBar* menu_bar = new wxMenuBar;
	menu_bar->Append(file_menu, "&File");

	frame->SetMenuBar(menu_bar);
	frame->CreateStatusLine(1);

#ifdef wx_msw
	frame->SetIcon(new wxIcon("wxhttpd_ico"));
#endif
#ifdef wx_x
	frame->SetIcon(new wxIcon("aiai.xbm"));
#endif

	int w, h;
	frame->GetClientSize(&w, &h);
	frame->console = new wxTextWindow(frame, -1, -1, w, h, wxREADONLY);

	frame->Show(TRUE);

	frame->console->WriteText("Started\n");

	if (!server.OK())
		frame->console->WriteText("Server error\n");

	running = TRUE;
	return frame;
}

///////////////////////////////////////////////////////////////////////////////

int HttpdApp::MainLoop()
{
#ifdef wx_xview
#if wxVersionNumber > 1603
	Display* display = wxGetDisplay();
#else
	extern  Xv_Server xview_server;
	Xv_Screen screen = xv_get(xview_server, SERVER_NTH_SCREEN, 0);
	Xv_opaque root_window = xv_get(screen, XV_ROOT);
	Display* display = (Display*)xv_get(root_window, XV_DISPLAY);
#endif
#endif

	while (running)
	{
#ifdef wx_xview
		notify_dispatch();
		XFlush(display);
#else
		if (Pending())
			Dispatch();
#endif

		if (connections.Count())
			Poll();

		while (handler.Wait(0, 100000L))
			ProcessHttpConnectionOrRequest();
	}

	return running;
}

///////////////////////////////////////////////////////////////////////////////

void HttpdApp::Poll()
{
	ListOfIterator ilist(connections);
	static char buf[1024];
	time_t now = time(0);
	void* t;

	for (int ok = ilist.First(t); ok; ok = ilist.Next(t))
	{
		HttpConnection* item = (HttpConnection*)t;

		if ((now - item->last) > KEEPALIVE_TIMEOUT)
		{
			string tmp = Number(item->count) +
							string(" Timed out: ") +
							Number(handler.Count()-1) +
							"\n";
			if (logging) frame->console->WriteText((char*)tmp.data());
			log(tmp.data());
			tmp = string("Connections: ")+Number(handler.Count()-1);
			frame->SetStatusText((char*)tmp.data());

			handler.DeRegister(item->sock);
			ilist.Delete();
			delete item;
			continue;
		}

		if (!item->istr)
			continue;

		time(&item->last);

		item->istr->read(buf, sizeof(buf));

		if (item->istr->gcount())
			item->sock->Write(buf, item->istr->gcount());

		if (item->istr->eof())
		{
			delete item->istr;
			item->istr = 0;

			if (item->ssi_filename.length())
			{
				remove(item->ssi_filename.data());
				item->ssi_filename = "";
			}

			if (!item->keep_alive)
			{
				handler.DeRegister(item->sock);
				ilist.Delete();
				delete item;
			}
		}
	}
}


//////////////////////////////////////////////////////////////

void HttpdApp::ProcessHttpConnectionOrRequest()
{
	SockHandler::SockStatus status;

	SimSock* sock = handler.Notify(status);

	switch (status)
	{
		case SockHandler::SOCK_NONE:
			break;

		case SockHandler::SOCK_CONNECT:
		{
			HttpConnection* item = new HttpConnection(sock);
			item->simple = TRUE;
			item->count = ++count;
			connections.Append(item);
			string tmp = Number(item->count)+string(" Connected")+"\n";
			if (logging) frame->console->WriteText((char*)tmp.data());
			log(tmp.data());
			tmp = string("Connections: ")+Number(handler.Count()-1);
			frame->SetStatusText((char*)tmp.data());
			break;
		}

		case SockHandler::SOCK_DISCONNECT:
		{
			ListOfIterator ilist(connections);
			void* t;

			for (int ok = ilist.First(t); ok; ok = ilist.Next(t))
			{
				HttpConnection* item = (HttpConnection*)t;

				if (item->sock == sock)
				{
					if (!item->cgi)
					{
						string tmp = Number(item->count)+string(" Disconnected")+"\n";
						if (logging) frame->console->WriteText((char*)tmp.data());
						log(tmp.data());
						tmp = string("Connections: ")+Number(handler.Count()-1);
						frame->SetStatusText((char*)tmp.data());
					}

					ilist.Delete();
					delete item;
					break;
				}
			}

			break;
		}

		case SockHandler::SOCK_ERROR:
		{
			ListOfIterator ilist(connections);
			void* t;

			for (int ok = ilist.First(t); ok; ok = ilist.Next(t))
			{
				HttpConnection* item = (HttpConnection*)t;

				if (item->sock == sock)
				{
					if (!item->cgi)
					{
						string tmp = Number(item->count)+string(" Error")+"\n";
						if (logging) frame->console->WriteText((char*)tmp.data());
						log(tmp.data());
						tmp = string("Connections: ")+Number(handler.Count()-1);
						frame->SetStatusText((char*)tmp.data());
					}

					ilist.Delete();
					delete item;
					break;
				}
			}

			break;
		}

		case SockHandler::SOCK_DATA:
		{

			ListOfIterator ilist(connections);
			HttpConnection* item = 0;
			void* t;

			for (int ok = ilist.First(t); ok; ok = ilist.Next(t))
			{
				item = (HttpConnection*)t;

				if (item->sock == sock)
					break;
			}

			if (!item)
				break;

			// Process request...

			static char msg[1024], tmp1[256], tmp2[256];
			int len;
			tmp1[0] = tmp2[0] = 0;

			// If processing a CGI request or reponse, then just
			// forward everything on...

			if (item->forward)
			{
				if ((len = item->sock->Read(msg, sizeof(msg))) > 0)
					item->forward->sock->Write(msg, len);

				break;
			}

			if ((len = item->sock->ReadMsg(msg, sizeof(msg)-1)) < 0)
				break;

			msg[len] = 0;

			// In case they send CR+LF as per spec!

			if (msg[len-1] == '\r')
				msg[--len] = 0;

			string tmp = Number(item->count)+" "+msg;
			log(tmp.data());

			// Note: end of request is an empty line...

			if (len <= 2)
			{
				if (item->doget)
					ProcessGet(item);
				else if (item->dopost)
					ProcessPost(item);
				else if (item->doput)
					ProcessPut(item);

				item->content_type = item->content_length = "";
				item->authorization = "";
				item->nbytes = 0;

				break;
			}

			if (!strncmpi(msg, "Content-type:", 13))
			{
				item->content_type = msg;
				break;
			}
			else if (!strncmpi(msg, "Content-Length:", 15))
			{
				item->content_length = msg;
				item->nbytes = atol(msg+15);
				break;
			}
			else if (sscanf(msg, "Authorization: %s %s", tmp1, tmp2) == 2)
			{
				item->authorization = msg;
			}
			else if (sscanf(msg, "Connection: %[^,],%s", tmp1, tmp2) == 2)
			{
				if (!strcmpi(tmp1, "keep-alive") || !strcmpi(tmp1, "persist"))
					item->keep_alive = TRUE;
				else if (!strcmpi(tmp2, "keep-alive") || !strcmpi(tmp2, "persist"))
					item->keep_alive = TRUE;

				if (item->keep_alive)
					item->origin_server = "";
			}
			else if (sscanf(msg, "Connection: %s", tmp1) == 1)
			{
				if (!strcmpi(tmp1, "keep-alive"))
					item->keep_alive = TRUE;
				else if (!strcmpi(tmp1, "close"))
					item->keep_alive = FALSE;

				if (item->keep_alive)
					item->origin_server = "";
			}
			else if (sscanf(msg, "Persist: %s", tmp1) == 1)
			{
				item->origin_server = tmp1;
			}
			else if (!strncmpi(msg, "GET ", 4))
			{
				if (logging) frame->console->WriteText((char*)tmp.data());
				strcpy(item->cmd, msg);
				item->doget = TRUE;
				break;
			}
			else if (!strncmpi(msg, "PUT ", 4))
			{
				if (logging) frame->console->WriteText((char*)tmp.data());
				strcpy(item->cmd, msg);
				item->doput = TRUE;
				break;
			}
			else if (!strncmpi(msg, "POST ", 5))
			{
				if (logging) frame->console->WriteText((char*)tmp.data());
				strcpy(item->cmd, msg);
				item->dopost = TRUE;
				break;
			}

			break;
		}

	}
}

///////////////////////////////////////////////////////////////////////////////
// URL format... scheme://host:port/file?query
///////////////////////////////////////////////////////////////////////////////

static int ParseURL(const char* src, string& scheme, string& hostname, string& portname, string& filename, string& query)
{
	const char* ptr = src;

	if (!*src)
		return 0;

	while (*src && (*src != ':') && (*src != '/') &&
				(*src != '#') && (*src != '?'))
		src++;

	// Get the scheme...

	if (src[0] == ':')
	{
                scheme = string(ptr, (size_t)(src-ptr));
		src++;
	}
	else
	{
		src = ptr;
		scheme = "";
	}

	// Get the host...

	if ((src[0] == '/') && (src[1] == '/'))
	{
		src++; src++;
		const char* tmp;

		if ((tmp = strchr(src, ':')) != 0)
		{
			if (tmp != src)
                                hostname = string(src, (size_t)(tmp-src));
			else
				hostname = "";

			src = tmp;
			src++;

			if ((tmp = strchr(src, '/')) != 0)
			{
                                portname = string(src, (size_t)(tmp-src));
				src = tmp;
				src++;
			}
			else
			{
				portname = src;
				filename = "";
				return 1;
			}
		}
		else if ((tmp = strchr(src, '/')) != 0)
		{
			if (tmp != src)
                                hostname = string(src, (size_t)(tmp-src));
			else
				hostname = "";

			portname = "";
			src = tmp;
			src++;
		}
		else
		{
			hostname = src;
			portname = "";
			filename = "";
			return 1;
		}
	}
	else
	{
		hostname = portname = "";
	}

	if ((ptr = strchr(src, '?')) != 0)
	{
                filename = string(src, (size_t)(ptr-src));
		src = ++ptr;
		query = src;
	}
	else
	{
		filename = src;
		query = "";
	}

	return 1;
}

//////////////////////////////////////////////////////////////

int HttpdApp::ExecuteQuery(HttpConnection* item, string& filename, const string& query)
{
        string save_filename = filename.data();

#ifdef wx_msw
	for (char* s = (char*)filename.data(); *s; s++)
		if (*s == '/')
			*s = '\\';
#endif

	if (!strchr(filename.data(), '.'))
	{
#if defined wx_msw
		filename += ".exe";
#elif defined wx_xview
		filename += "_ol";
#elif defined wx_motif
		filename += "_motif";
#endif
	}

	if (!wxFileExists(filename.data()))
	{
		if (logging) frame->console->WriteText((char*)(string("Doesn't exist: ")+filename+"\n").data());
		return 0;
	}

	SockClient* cgi = new SockClient(SimSock::NONE);

	// Can connect?

	if (cgi->Connect("localhost", FASTCGI_PORT) < 0)
	{
		string tmp = filename + " " +
						FASTCGI_PORT + " " +
						Number((int)fastcgi_timeout);

#ifdef wx_x
		tmp += " &";			// Force to background
#endif

		// Shell out a new process...

		wxExecute(tmp.data());

		//if (logging) frame->console->WriteText((tmp+"\n").data());
		//log(tmp+"\n");
	}

	// Wait for connection...

	time_t now = time(0);
	time_t end_time = now + 5;

	handler.DeRegister(item->sock);

	while (!cgi->IsConnected() && (now < end_time))
	{
		if (cgi->Connect("localhost", FASTCGI_PORT) < 0)
		{
			wxSleep(1);
			time(&now);
		}
	}

	handler.Register(item->sock);

	if (!cgi->IsConnected())
	{
		if (logging) frame->console->WriteText("Could not connect\n");
		delete cgi;
		return 0;
	}

	handler.Register(cgi);

	HttpConnection* new_item = new HttpConnection(cgi);
	new_item->cgi = TRUE;
	new_item->count = ++count;
	new_item->forward = item;
	connections.Append(new_item);

	item->forward = new_item;

	// Convert the socket to a stream...

	OSockStream ostr(cgi);

	ostr << "Script-name: " << save_filename << "\n";

	if (query.length())
	{
		ostr << "Request-method: GET\n";
		ostr << "Query-string: " << query << "\n";
	}
	else
	{
		ostr << "Request-method: POST\n";
		ostr << item->content_type;
		ostr << item->content_length;
	}

	if (item->authorization.length())
		ostr << item->authorization;

	ostr << "\n" << flush;

	// Mainloop will handle forwarding...

	time(&item->last);

	return 1;
}

//////////////////////////////////////////////////////////////

void HttpdApp::ProcessPost(HttpConnection* item)
{
	char buf[1024];

	item->dopost = FALSE;

	// Get start of filename...
	char* src = item->cmd+4;
	while (*src == ' ') src++;

	// Skip leading '/' character...
	if (*src == '/') src++;

	// Get end of filename. Could be a space if it is followed by a protocol
	// version string, else will be CrLf characters...

	char* s = src;
	while (*s && (*s != ' ') && (*s != '\r') && (*s != '\n'))
		s++;

	// If less than 1.0 then it is simple protocol...
	if ((*s == ' ') && strncmpi(s+1, "HTTP/0.", 7))
		item->simple = FALSE;

	*s = 0;

	string scheme, hostname, portname, filename, query;

	if (src && !ParseURL(src, scheme, hostname, portname, filename, query))
	{
		sprintf(buf, "HTTP/%s 400 Bad Request\n"
								"Content-Type: text/html\n"
								"Content-Length: %d\n\n%s",
								(item->simple ? "0.9" : "1.1"),
								strlen(STATUS_400), STATUS_400);

		item->sock->Write(buf, strlen(buf));
		log((Number(item->count)+" "+buf).data());
		return;
	}

	if (!ExecuteQuery(item, filename, query))
	{
		sprintf(buf, "HTTP/%s 404 Not Found\n"
								"Content-Type: text/html\n"
								"Content-Length: %d\n\n%s",
								(item->simple ? "0.9" : "1.1"),
								strlen(STATUS_404), STATUS_404);

		item->sock->Write(buf, strlen(buf));
		log((Number(item->count)+" "+buf).data());

		// Flush input...

		while (item->nbytes)
		{
			unsigned len = wxMin(item->nbytes, (int)sizeof(buf));
			item->nbytes -= len;
			item->sock->Read(buf, len);
		}
	}

	return;
}

//////////////////////////////////////////////////////////////

void HttpdApp::ProcessGet(HttpConnection* item)
{
	char buf[1024];

	item->doget = FALSE;

	// Get start of filename...

	char* src = item->cmd+3;
	while (*src == ' ') src++;

	if (*src == '/') src++;

	// Get end of filename. Could be a space if it is
	// followed by a protocol version string, else
	// will be CrLf characters...

	char* s = src;
	while (*s && (*s != ' ') && (*s != '\r') && (*s != '\n'))
		s++;

	// If less than 1.0 then it is simple protocol...

	if ((*s == ' ') && strncmpi(s+1, "HTTP/0.", 7))
		item->simple = FALSE;

	*s = 0;

	string scheme, hostname, portname, filename, query;

	if (*src && !ParseURL(src, scheme, hostname, portname, filename, query))
	{
		sprintf(buf, "HTTP/%s 400 Bad Request\n"
								"Content-Type: text/html\n"
								"Content-Length: %d\n\n%s",
								(item->simple ? "0.9" : "1.1"),
								strlen(STATUS_400), STATUS_400);

		item->sock->Write(buf, strlen(buf));
        log((Number(item->count)+" "+buf).data());
		return;
	}

	if (query.length())
	{
		ProcessPost(item);
		return;
	}

#ifdef wx_msw
	for (s = (char*)filename.data(); *s; s++)
		if ((*s == '/') || (*s == '~'))
			*s = '\\';
#endif

	// Open the requested file...

	int tried = 0;

	if (!filename.length() ||
#ifdef wx_msw
			(filename[filename.length()-1] == '\\'))
#else
			(filename[filename.length()-1] == '/'))
#endif
		filename += "index.html";

loop:

	// MSVC++ assumes ios::create by default for an input stream! Truly bizarre...

#ifdef wx_msw
	item->istr = new ifstream(filename.data(), ios::in|ios::nocreate|ios::binary);
#else
	item->istr = new ifstream(filename.data(), ios::in|ios::nocreate);
#endif

	if (!*item->istr && !*src)
	{
		if (tried == 0)
		{
			filename = "index.htm";
			tried++;
			goto loop;
		}
		else if (tried == 1)
		{
			filename = "index.shtml";
			tried++;
			goto loop;
		}
		else if (tried == 2)
		{
			filename = "index.sht";
			tried++;
			goto loop;
		}
	}

	if (!*item->istr)
	{
		sprintf(buf, "HTTP/%s 404 Not Found\n"
								"Content-Type: text/html\n"
								"Content-Length: %d\n\n%s",
								(item->simple ? "0.9" : "1.1"),
								strlen(STATUS_404), STATUS_404);

		item->sock->Write(buf, strlen(buf));
		log((Number(item->count)+" "+buf).data());
		delete item->istr;
		item->istr = 0;
		return;
	}

	item->filename = filename;

	const char* ext = strrchr(filename.data(), '.');

	if (ext != 0)
	{
		// Server-side includes?

		if (!strcmp(ext, ".shtml") || !strcmp(ext, ".sht"))
		{
			delete item->istr;

			wxHtml canvas(frame);
			canvas.OpenURL(string("file:///") + filename);

			char buf[L_tmpnam];
			item->ssi_filename = filename = tmpnam(buf);
			canvas.SaveGeneratedFile(item->ssi_filename);

#ifdef wx_msw
			item->istr = new ifstream(item->ssi_filename.data(), ios::in|ios::nocreate|ios::binary);
#else
			item->istr = new ifstream(item->ssi_filename.data(), ios::in|ios::nocreate);
#endif
		}
	}

	if (!item->simple)
	{
		struct stat st;
		stat(filename.data(), &st);

		char* dst = buf;

		dst += SPRINTF((dst, "HTTP/1.1 200 OK\n"));
#ifdef wx_msw
		dst += SPRINTF((dst, "Server: wxHttpd/2.x Windows\n"));
#else
		dst += SPRINTF((dst, "Server: wxHttpd/2.x Unix\n"));
#endif

		if (item->keep_alive)
		{
			dst += SPRINTF((dst, "Connection: keep-alive,persist\n"));
			dst += SPRINTF((dst, "Persist: %s\n", item->origin_server.data()));
		}

		dst += SPRINTF((dst, "Last-Modified: %s", asctime(gmtime(&st.st_mtime))));

		const char* ext_ptr = strrchr(item->filename.data(), '.');

		if (!ext_ptr) ext_ptr = "";

		// This should be in a file...

		if (!strcmpi(ext_ptr, ".gif"))
			dst += SPRINTF((dst, "Content-Type: image/gif\n"));
		else if (!strcmpi(ext_ptr, ".xpm"))
			dst += SPRINTF((dst, "Content-Type: image/xpm\n"));
		else if (!strcmpi(ext_ptr, ".xbm"))
			dst += SPRINTF((dst, "Content-Type: image/xbm\n"));
		else if (!strcmpi(ext_ptr, ".png"))
			dst += SPRINTF((dst, "Content-Type: image/png\n"));
		else if (!strcmpi(ext_ptr, ".bmp"))
			dst += SPRINTF((dst, "Content-Type: image/x-ms-bmp\n"));
		else if (!strcmpi(ext_ptr, ".jpg") || !strcmpi(ext_ptr, ".jpeg"))
			dst += SPRINTF((dst, "Content-Type: image/jpeg\n"));
		else if (!strcmpi(ext_ptr, ".css"))
			dst += SPRINTF((dst, "Content-Type: text/css\n"));
		else if (!strcmpi(ext_ptr, ".htm"))
			dst += SPRINTF((dst, "Content-Type: text/html\n"));
		else if (!strcmpi(ext_ptr, ".html"))
			dst += SPRINTF((dst, "Content-Type: text/html\n"));
		else if (!strcmpi(ext_ptr, ".sht"))
			dst += SPRINTF((dst, "Content-Type: text/html\n"));
		else if (!strcmpi(ext_ptr, ".shtml"))
			dst += SPRINTF((dst, "Content-Type: text/html\n"));
		else if (!strcmpi(ext_ptr, ".wsml"))
			dst += SPRINTF((dst, "Content-Type: text/wsml\n"));
		else if (!strcmpi(ext_ptr, ".mpg") || !strcmpi(ext_ptr, ".mpeg"))
			dst += SPRINTF((dst, "Content-Type: video/mpeg\n"));
		else if (!strcmpi(ext_ptr, ".qt") || !strcmpi(ext_ptr, ".mov"))
			dst += SPRINTF((dst, "Content-Type: video/quicktime\n"));
		else if (!strcmpi(ext_ptr, ".au"))
			dst += SPRINTF((dst, "Content-Type: audio/basic\n"));
		else if (!strcmpi(ext_ptr, ".wav"))
			dst += SPRINTF((dst, "Content-Type: audio/x-wav\n"));
		else if (!strcmpi(ext_ptr, ".txt") || !strcmpi(ext_ptr, ".text"))
			dst += SPRINTF((dst, "Content-Type: text/plain\n"));
		else if (!strcmpi(ext_ptr, ".gz") || !strcmpi(ext_ptr, ".tgz"))
			dst += SPRINTF((dst, "Content-Type: application/x-gzip\n"));
		else if (!strcmpi(ext_ptr, ".zip"))
			dst += SPRINTF((dst, "Content-Type: application/x-zip-compressed\n"));
		else if (!strcmpi(ext_ptr, ".avi"))
			dst += SPRINTF((dst, "Content-Type: application/x-msvideo\n"));
		else if (!strcmpi(ext_ptr, ".Z"))
			dst += SPRINTF((dst, "Content-Type: application/x-compress\n"));
		else if (!strcmpi(ext_ptr, ".tar"))
			dst += SPRINTF((dst, "Content-Type: application/x-tar\n"));
		else if (!strcmpi(ext_ptr, ".dvi"))
			dst += SPRINTF((dst, "Content-Type: application/x-dvi\n"));
		else if (!strcmpi(ext_ptr, ".tex"))
			dst += SPRINTF((dst, "Content-Type: application/x-tex\n"));
		else if (!strcmpi(ext_ptr, ".pdf"))
			dst += SPRINTF((dst, "Content-Type: application/pdf\n"));
		else if (!strcmpi(ext_ptr, ".rtf"))
			dst += SPRINTF((dst, "Content-Type: application/rtf\n"));
		else if (!strcmpi(ext_ptr, ".ps"))
			dst += SPRINTF((dst, "Content-Type: application/postscript\n"));
		else
			dst += SPRINTF((dst, "Content-Type: application/octet-stream\n"));

		dst += SPRINTF((dst, "Content-Length: %ld\n", st.st_size));

		// terminate the response header...

		dst += SPRINTF((dst, "\n"));

		item->sock->Write(buf, (size_t)(dst-buf));
	}

	// Mainloop will handle transmission...

	time(&item->last);
}

//////////////////////////////////////////////////////////////

void HttpdApp::ProcessPut(HttpConnection* item)
{
	char buf[1024];

	item->doput = FALSE;

	// Get start of filename...
	char* src = item->cmd+4;
	while (*src == ' ') src++;

	// Skip leading '/' character...
	if (*src == '/') src++;

	// Get end of filename. Could be a space if it is followed by a protocol
	// version string, else will be CrLf characters...

	char* s = src;
	while (*s && (*s != ' ') && (*s != '\r') && (*s != '\n'))
		s++;

	// If less than 1.0 then it is simple protocol...
	if ((*s == ' ') && strncmpi(s+1, "HTTP/0.", 7))
		item->simple = FALSE;

	*s = 0;

	string scheme, hostname, portname, filename, query;

	if (src && !ParseURL(src, scheme, hostname, portname, filename, query))
	{
		sprintf(buf, "HTTP/%s 400 Bad Request\n"
								"Content-Type: text/html\n"
								"Content-Length: %d\n\n%s",
								(item->simple ? "0.9" : "1.1"),
								strlen(STATUS_400), STATUS_400);

		item->sock->Write(buf, strlen(buf));
		log((Number(item->count)+" "+buf).data());
		return;
	}

#ifdef wx_msw
	for (s = (char*)filename.data(); *s; s++)
		if ((*s == '/') || (*s == '~'))
			*s = '\\';
#endif

	// Open the requested file...

	if (!filename.length() ||
#ifdef wx_msw
			(filename[filename.length()-1] == '\\'))
#else
			(filename[filename.length()-1] == '/'))
#endif
		filename += "index.html";

#ifdef wx_msw
	ofstream file(filename.data(), ios::out|ios::binary|ios::trunc);
#else
	ofstream file(filename.data(), ios::out|ios::trunc);
#endif

	if (file.bad())
	{
		sprintf(buf, "HTTP/%s 403 Forbidden\n"
								"Content-Type: text/html\n"
								"Content-Length: %d\n\n%s",
								(item->simple ? "0.9" : "1.1"),
								strlen(STATUS_403), STATUS_403);

		item->sock->Write(buf, strlen(buf));
		log((Number(item->count)+" "+buf).data());
		return;
	}

	// Save input to file...

	while (item->nbytes)
	{
		unsigned len = wxMin(item->nbytes, (int)sizeof(buf));
		item->nbytes -= len;
		item->sock->Read(buf, len);
		file.write(buf, len);
	}

	sprintf(buf, "HTTP/%s 204 No content\n\n", item->simple ? "0.9" : "1.1");

	item->sock->Write(buf, strlen(buf));
	log((Number(item->count)+" "+buf).data());

	return;
}

//////////////////////////////////////////////////////////////

HttpdApp MyHttpdApp;


///////////////////////////////////////////////////////////////////////////////

Bool HttpdFrame::OnClose()
{
	MyHttpdApp.ExitMainLoop();
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

void HttpdFrame::OnMenuCommand(int id)
{
	switch (id)
	{
		case WXWEB_LOG:

			logging = !logging;

			if (logging)
			{
				logfile = new ofstream("wxhttpd.log");
			}
			else
			{
				delete logfile;
			}

			break;

		case WXWEB_EXIT:

			if (OnClose())
				delete this;

			break;
	}
}


