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

#include <wx_prec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx.h>
#endif

#include <AString.h>

#include <stdio.h>
#include <strstream.h>
#include <time.h>

#include "simsock.h"
#include "strsock.h"
#include "wxcgi.h"

#define SURVEY_EXIT 1
#define CLIENT_PORT "7000"
#define CONNECTION_TIMEOUT 30

static SockServer* server;
static wxHashTable fields(wxKEY_STRING, 97);
static wxHashTable vars(wxKEY_STRING, 97);
static long timeout = CONNECTION_TIMEOUT;
static AString port = CLIENT_PORT;

static const char* STATUS_500 = "<HTML><HEAD><TITLE>Error</TITLE></HEAD><BODY><H1>Status 500 Internal Server Error</H1></BODY></HTML>\n";

#ifndef TZSET
#define TZSET tzset
#endif

///////////////////////////////////////////////////////////////////////////////

wxCGI_Frame::wxCGI_Frame(char *title, int x, int y, int w, int h):
	wxFrame(0, title, x, y, w, h)
{
}

///////////////////////////////////////////////////////////////////////////////

wxCGI_App::wxCGI_App()
{
}

///////////////////////////////////////////////////////////////////////////////

wxCGI_App::~wxCGI_App()
{
	if (server)
	{
		handler.DeRegister(server);
		delete server;
	}
}

///////////////////////////////////////////////////////////////////////////////

wxFrame *wxCGI_App::OnInit()
{
	TZSET();

	frame = new wxCGI_Frame("WX-CGI");
	frame->Show(FALSE);

#if defined ( __BORLANDC__ ) && defined ( WIN32 )
	int i = 0;
#else
	int i = 1;
#endif

	if (i != argc)
		port = argv[i++];

	if (i != argc)
		timeout = atol(argv[i++]);

	server = new SockServer(port.Chars(), SimSock::NONE, '\n');

	if (server)
	{
		handler.Register(server);
		time(&last);
		running = TRUE;
	}

	return frame;
}

///////////////////////////////////////////////////////////////////////////////

int wxCGI_App::MainLoop()
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

	while ( running && ((time(0) - last) < timeout))
	{
#ifndef wx_xview
		if (Pending())
			Dispatch();
#else
		notify_dispatch();
		XFlush(display);
#endif

		if (handler.Wait(1))
			Poll();
	}

	return running;
}

///////////////////////////////////////////////////////////////////////////////
// Copy bytes, but note: a '%' followed by 2 hex-digits must be
// converted to a single byte-value...
///////////////////////////////////////////////////////////////////////////////

const char* URL_decode(const char* src)
{
	char tmp[1024];
	char* dst = tmp;
	char ch;

	while ((ch = *src++) != 0)
	{
		if (ch == '%')
		{
			AString n = *src++;
			n += *src++;

			ch = (char)strtoul(n.Chars(), 0, 16);
		}

		*dst++ = ch;
	}

	*dst = 0;

	return strdup(tmp);
}

///////////////////////////////////////////////////////////////////////////////
// Get name1=value1[&name2=value2...] pairs...
///////////////////////////////////////////////////////////////////////////////

void wxCGI_App::ParseQuery(const char* src)
{
	char name[1024];
	char value[1024];
	char* dst = name;
	char ch;

	while ((ch = *src++) != 0)
	{
		if ((ch != '=') && (ch != '&'))
		{
			*dst++ = ch;
			continue;
		}

		*dst = 0;

		if (ch == '=')
		{
			dst = value;
			continue;
		}

		AString tmp = name;
		tmp.UpperCase();
		fields.Put(tmp.Chars(), (wxObject*)URL_decode(value));

		dst = name;
	}

	*dst = 0;

	AString tmp = name;
	tmp.UpperCase();
	fields.Put(tmp.Chars(), (wxObject*)URL_decode(value));
}

///////////////////////////////////////////////////////////////////////////////
// Get name1=value1[&name2=value2...] pairs...
///////////////////////////////////////////////////////////////////////////////

void wxCGI_App::ParseContents(istream& istr, long nbytes)
{
	char name[1024];
	char value[1024];
	char* dst = name;
	char ch = 0;

	while ((nbytes-- > 0) && ((ch = (char)istr.get()) != 0) && !istr.eof())
	{
		if ((ch != '=') && (ch != '&'))
		{
			*dst++ = ch;
			continue;
		}

		*dst = 0;

		if (ch == '=')
		{
			dst = value;
			continue;
		}

		AString tmp = name;
		tmp.UpperCase();
		fields.Put(tmp.Chars(), (wxObject*)URL_decode(value));

		dst = name;
	}

	*dst = 0;

	AString tmp = name;
	tmp.UpperCase();
	fields.Put(tmp.Chars(), (wxObject*)URL_decode(value));
}

///////////////////////////////////////////////////////////////////////////////

void wxCGI_App::Poll()
{
	SockHandler::SockStatus status;

	SimSock* sock = handler.Notify(status);

	switch (status)
	{
		case SockHandler::SOCK_NONE:
		{
			break;
		}

		case SockHandler::SOCK_CONNECT:
		{
			time(&last);
			break;
		}

		case SockHandler::SOCK_DISCONNECT:
		{
			break;
		}

		case SockHandler::SOCK_ERROR:
		{
			break;
		}

		case SockHandler::SOCK_DATA:
		{
			// Process request...

			char msg[1024];
			int len;

			// Use ReadMsg() here to make sure we get a
			// whole message as they're '\n' delimited...

			if ((len = sock->ReadMsg(msg, sizeof(msg))) < 0)
				break;

			msg[len] = 0;

			// Simple CrLf will terminate request...

			if (len <= 2)
			{
				SockStream str(sock);

				// Process the request...

				if (!OnRequest(str, str))
				{
					str << "HTTP/1.0 500 Internal Server Error\n"
							<< "Content-Type: text/html\n"
							<< "Content-Length: " << strlen(STATUS_500) << '\n'
							<< "\n"
							<< STATUS_500
							<< flush;
				}

				// Cleanup...

				handler.DeRegister(sock);
				delete sock;

				fields.BeginFind();
				wxNode* node;

				while ((node = fields.Next()) != 0)
					free((char*)node->Data());

				fields.Clear();

				vars.BeginFind();

				while ((node = vars.Next()) != 0)
					free((char*)node->Data());

				vars.Clear();
			}
			else
			{
				char var[1024];
				char value[1024];

				if (sscanf(msg, "%[^:]:%[^\r\n]", var, value) == 2)
				{
					const char* src = value;
					while (*src == ' ') src++;

					AString tmp = var;
					tmp.UpperCase();
					vars.Put(tmp.Chars(), (wxObject*)strdup(src));
				}
			}

			time(&last);
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Get fields in a query or form...
///////////////////////////////////////////////////////////////////////////////

const char* wxCGI_App::GetField(const char* name)
{
	AString tmp = name;
	tmp.UpperCase();
	const char* ptr = (const char*)fields.Get(tmp.Chars());
	return (ptr ? ptr : "");
}

///////////////////////////////////////////////////////////////////////////////
// Get environment variables...
///////////////////////////////////////////////////////////////////////////////

const char* wxCGI_App::GetVariable(const char* name)
{
	AString tmp = name;
	tmp.UpperCase();
	const char* ptr = (const char*)vars.Get(tmp.Chars());
	return (ptr ? ptr : "");
}


