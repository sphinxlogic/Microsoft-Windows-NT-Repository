///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  An HTML canvas for wxWindows.
//
// Edit History
//
//	  Started November 25th 1995 by Andrew Davison.
//
// Copyright (c) 1995-6 by Andrew Davison. Permission is granted to use the
// source in unmodified form quite freely, and to modify the source and use
// as part of a WXWINDOWS program only.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef WXWWW_H
#define WXWWW_H

#ifndef HttpClient_H
#include "http.h"
#endif

#ifndef WXHTML_H
#include "wxhtml.h"
#endif

class wxWWW : public wxHtml, public HttpClient
{
public:

DECLARE_ABSTRACT_CLASS(wxWWW);

	wxWWW(wxWindow* parent, int x=-1, int y=-1, int w=-1, int h=-1, long style=0);
	~wxWWW();

	void SetPersistentCache(Bool state) { persistent_cache = state; };
	void Reload(Bool /*from_host*/ = FALSE);
	Bool VerifyWeb();
	void MapURL(const string& url, const string& base);
	const string& GetCurrentServer() const { return current_server; };
	void SendMail(const string& to="", const string& subject="", const string& text="");
	int PutGenerated();
	Bool IsSecure() const { return was_secure; };

	int SearchCache(const string& u, string& localname, string& last_modified, string& expires, Bool& not_found, string& mime_type, Bool head);
	void SetCache(Bool state, Bool persistent, const string& path="");
	void GetCache(Bool& state, Bool& persistent, string& path);
	void ClearCache();
	const string& GetCwdPath() const { return cwd_path; };

	struct Map
	{
		string from, to;
	};
	static ListOf maps;

protected:

	int GetHTTP(const string& host, const string& portname, const string& filename, string& localname, Bool inline_object=FALSE, Bool post=FALSE, Bool head=FALSE, int secure=FALSE);
	int PutHTTP(const string& hostname, const string& portname, const string& filename);
	int GetFTP(const string& host, const string& portname, const string& filename, string& localname, Bool inline_object=FALSE);

	wxHtml* Clone(int x, int y, int w, int h, long style);

	void AddCache(const string& u, const string& localname="", const string& last_modified="", const string& expires="", Bool not_found=FALSE, const string& mime_type="", Bool head=FALSE);
	int RemoveCache(const string& u);
	void SaveCacheFile();
	void OnSetStatusText(const char* text, int number);

private:

	string SaveAs(const string& u);
	string Authenticate(const string& realm);
	void SaveMapFile();
	void LoadFiles();
	Bool VerifyWeb(const string&, wxHtml*);
	void DoYield() const { wxYield(); };

	static persistent_cache;
	static string cwd_path;
};

#endif


