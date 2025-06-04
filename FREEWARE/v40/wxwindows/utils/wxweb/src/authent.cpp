///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//   Authentication: prompts for a user-name and password.
//
//
// Edit History
//
//	  Started February 21st 1996 by Andrew Davison.
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

#ifdef _MSC_VER
#include <strstrea.h>
#else
#include <strstream.h>
#endif

#include "wxhtml.h"
#include "base64.h"
#include "authent.h"

string Authenticate::save_userid;
string Authenticate::save_password;
Bool Authenticate::isok;

///////////////////////////////////////////////////////////////////////////////

void Authenticate::AuthOK(wxButton& button, wxCommandEvent&)
{
	Authenticate* auth = (Authenticate*)(button.GetParent());
	auth->isok = TRUE;
	save_userid = auth->userid->GetValue();
	save_password = auth->password->GetValue();
	delete auth;
}

void Authenticate::AuthCancel(wxButton& button, wxCommandEvent&)
{
	Authenticate* auth = (Authenticate*)(button.GetParent());
	auth->isok = FALSE;
	save_userid = save_password = "";
	delete auth;
}

///////////////////////////////////////////////////////////////////////////////

Authenticate::Authenticate(wxWindow* parent, const string& realm) :
	wxDialogBox(parent, (char*)realm.data(), TRUE)
{
	save_userid = save_password = "";
	isok = FALSE;
	userid = new wxText(this, 0, "Userid:", "", 20, 60, 200, -1);
	password = new wxText(this, 0, "Password:", "", 20, 100, 200, -1, wxPASSWORD);
	ok = new wxButton(this, (wxFunction)AuthOK, "OK", 40, 140);
	cancel = new wxButton(this, (wxFunction)AuthCancel, "Cancel", 100, 140);
	Fit();
	Centre();
	Show(TRUE);
}

string Authenticate::GetCookie()
{
	//wxString s = save_userid.Strip(wxString::trailing, ' ') + ":" + save_password.Strip(wxString::trailing, ' ');
	string s = save_userid + ":" + save_password;

	strstream str;
	Base64Encoder b64(str, 0);
	b64.Put((const unsigned char*)s.data(), s.length());
	b64.InputFinished();
	string tmp;
	str >> tmp;
	return tmp;
}

Bool Authenticate::CrackCookie(const string& s, string& u, string& p)
{
	strstream str;
	Base64Decoder b64(str);
	b64.Put((const unsigned char*)s.data(), s.length());
	b64.InputFinished();
	string tmp;
	str >> tmp;

	const char* ptr = strchr(tmp.data(), ':');
	if (!ptr) return FALSE;

	int offset = ptr - tmp.data();
	u = string(tmp, 0, offset);
	p = string(tmp, offset, tmp.length()-offset);
	return TRUE;
}


