///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//   Authentication: prompts for a user-name and password.
//
//
// Edit History
//
//	  Started February 21st, 1996 by Andrew Davison.
//
// Copyright (c) 1995-6 by Andrew Davison. Permission is granted to use the
// source in unmodified form quite freely, and to modify the source and use
// as part of a WXWINDOWS program only.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef AUTHENT_H
#define AUTHENT_H

class Authenticate : public wxDialogBox
{
public:

  Authenticate(wxWindow* parent, const string& realm);
  ~Authenticate() {};

  string GetUserID() const { return save_userid; };
  string GetPassword() const { return save_password; };

  static Bool CrackCookie(const string& cookie, string& u, string& p);
  static string GetCookie();
  static Bool Ok(){ return isok; };

private:

	static void AuthOK(wxButton& button, wxCommandEvent&);
	static void AuthCancel(wxButton& button, wxCommandEvent&);
	static string save_userid, save_password;
	static Bool isok;

	wxText* userid;
	wxText* password;
	wxButton* ok;
	wxButton* cancel;
};

#endif


