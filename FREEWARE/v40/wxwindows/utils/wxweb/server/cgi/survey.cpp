///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  This is SURVEY, an example WX-CGI application.
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream.h>

#include "simsock.h"
#include "wxcgi.h"
#include "authent.h"

#ifndef wx_msw
#define strcmpi(s1, s2) strcasecmp(s1, s2)
#define strncmpi(s1, s2, n) strncasecmp(s1, s2, n)
#if BSD
extern "C" int strcasecmp(const char*s1, const char* s2);
extern "C" int strncasecmp(const char*s1, const char* s2, int n);
#endif
#endif

static const char* STATUS_401 = "<HTML><HEAD><TITLE>No authorization</TITLE></HEAD><BODY><H1>Status 401 Unauthorized</H1></BODY></HTML>\n";
static const char* STATUS_403 = "<HTML><HEAD><TITLE>Access forbidden</TITLE></HEAD><BODY><H1>Status 403 Forbidden (use password 'please')</H1></BODY></HTML>\n";

///////////////////////////////////////////////////////////////////////////////
// Create the application...
///////////////////////////////////////////////////////////////////////////////

class SurveyApp: public wxCGI_App
{
	public:

		SurveyApp() : wxCGI_App() {};

	private:

		Bool OnRequest(istream&, ostream&);
};

SurveyApp MyApp;


///////////////////////////////////////////////////////////////////////////////
// Execute the request...
///////////////////////////////////////////////////////////////////////////////

Bool SurveyApp::OnRequest(istream& istr, ostream& ostr)
{
	const char* var = GetVariable("Request-method");

	// A GET request takes query parameters from an
	// environment variable...

	if (!strcmpi(var, "GET"))
	{
		var = GetVariable("Query-string");
		ParseQuery(var);
	}

	// A POST request takes query paramaters from the data
	// contents following the request, or else the
	// data is some other object (which we won't handle
	// here)...

	else if (!strcmpi(var, "POST"))
	{
		var = GetVariable("Content-length");

		long nbytes = atol(var);

		var = GetVariable("Content-type");

		if (strcmpi(var, "application/x-www-form-urlencoded"))
			return FALSE;

		ParseContents(istr, nbytes);
	}
	else
		return FALSE;

	const char* auth = GetVariable("Authorization");
	char scheme[256], cookie[256];
	if (sscanf(auth, "%s %s", scheme, cookie) != 2)
	{
		ostr << "HTTP/1.0 401 Unauthorized\n";
		ostr << "WWW-Authenticate: Basic realm=\"UserSurvey\"\n";
		ostr << "Content-Type: text/html\n";
		ostr << "Content-Length: " << strlen(STATUS_401) << '\n';
		ostr << "\n";
		ostr << STATUS_401;
		ostr << flush;
		return TRUE;
	}

	if (strcmpi(scheme, "Basic"))
		return FALSE;

	// For demo purposes...

	wxString userid, password;
	Authenticate::CrackCookie(cookie, userid, password);

	if (strcmpi(password.Chars(), "please"))
	{
		ostr << "HTTP/1.0 403 Forbidden\n";
		ostr << "WWW-Authenticate: Basic realm=\"UserSurvey\"\n";
		ostr << "Content-Type: text/html\n";
		ostr << "Content-Length: " << strlen(STATUS_403) << '\n';
		ostr << "\n";
		ostr << STATUS_403;
		ostr << flush;
		return TRUE;
	}

	char text[1024];
	sprintf(text,  "Your address is: <B>%.256s</B>\n"
						"You are running a: <B>%.256s</B>\n"
						"With operating system(s): <B>%s %s %s</B>\n\n"
						"You munch: <B>%s</B>\n\n"
						"You said...\n\n"
						"<B><I>%.256s</I></B>\n",
						GetField("ADDRESS"),
						GetField("HARDWARE"),
						GetField("FRUIT"),
						(*GetField("MOTIF") ? "Motif" : ""),
						(*GetField("XVIEW") ? "XView" : ""),
						(*GetField("WINDOWS") ? "Windows" : ""),
						GetField("NUMBER"),
						GetField("COMMENTS")
			);

	char msg[1024];
	sprintf(msg, "%s%s%s",
		"<HTML>""<HEAD><TITLE>Survey Results</TITLE></HEAD>"
		"<BODY><CENTER><H1>Thanks!</H1></CENTER><P><BIG><PRE>",
		text,
		"</PRE></UR></BODY></HTML>");

	ostr << "HTTP/1.0 200 OK\n";
	ostr << "Content-Type: text/html\n";
	ostr << "Content-Length: " << strlen(msg) << "\n";
	ostr << "\n";
	ostr << msg;
	ostr << flush;

	return TRUE;
}


