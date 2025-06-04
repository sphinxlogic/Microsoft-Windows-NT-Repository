///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  An HTML canvas for wxWindows with networking.
//
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
#include <fstream.h>
#include <sys/stat.h>
#include <limits.h>

#ifdef wx_msw
#include <mmsystem.h>
#endif

#include "wxwww.h"
#include "listof.h"
#include "simsock.h"
#include "smtpmail.h"
#include "authent.h"
#include "url.h"

ListOf wxWWW::maps;
Bool wxWWW::persistent_cache = FALSE;
string wxWWW::cwd_path;

static const char* MAPFILE = "wxwebmap.db";
static const char* CACHEFILE = "wxwebdir.db";

extern char* RESOURCE_FILE;


IMPLEMENT_ABSTRACT_CLASS(wxWWW, wxHtml);

#ifndef TZSET
#define TZSET tzset
#endif

///////////////////////////////////////////////////////////////////////////////

wxWWW::wxWWW(wxWindow* parent, int x, int y, int w, int h, long style) :
		wxHtml(parent, x, y, w, h, style)
{
	current_server = "";

	// First one?

	if (instance_count == 1)
	{
		TZSET();

		const char* ptr;
#ifdef wx_msw
		char buf[128];
#endif

		if ((ptr = getenv("wxweb_cache_path")) != 0)
		{
		}
		else if ((ptr = getenv("temp")) != 0)
		{
		}
		else if ((ptr = getenv("tmp")) != 0)
		{
		}
#ifdef wx_msw
		else if (GetWindowsDirectory(buf, sizeof(buf)) != 0)
		{
			ptr = buf;
		}
#elif defined wx_x
		else if (1)
		{
			ptr = "/tmp";
		}
#endif
		else
		{
			ptr = wxGetWorkingDirectory();
		}

		wxMkdir((char*)ptr);
		cwd_path = ptr;

		cache_path = cwd_path + SEP + "cache";
		wxMkdir((char*)cache_path.data());
		LoadFiles();
	}
}

///////////////////////////////////////////////////////////////////////////////

wxWWW::~wxWWW()
{
	// This is needed here for 'instance_count' check...
	DeleteSubFrames();

	// Last one?

	if (instance_count == 1)
	{
		if (!persistent_cache)
		{
			ClearCache();
			remove((cache_path + SEP + CACHEFILE).data());
			wxRmdir((char*)cache_path.data());
			return;
		}
		
		SaveCacheFile();

		if (maps.Count())
			SaveMapFile();

		void* a;

		while (maps.Pop(a))
			delete (Map*)a;
	}
}

///////////////////////////////////////////////////////////////////////////////

wxHtml* wxWWW::Clone(int x, int y, int w, int h, long style)
{
	wxFrame* frame = (wxFrame*)GetParent();
	int tmp_x, tmp_y;

	GetPosition(&tmp_x, &tmp_y);
	x += tmp_x;
	y += tmp_y;

	wxHtml* canvas = new wxWWW(frame, x, y, w, h, style);
	canvas->real_parent = this;
	return canvas;
}

///////////////////////////////////////////////////////////////////////////////

void wxWWW::LoadFiles()
{
	ifstream cachefile((cache_path + SEP + CACHEFILE).data(), ios::in|ios::nocreate);
	char buf[256];

	while (cachefile && !cachefile.get(buf, sizeof(buf)).eof())
	{
		char localname[256], urlname[1024], mime_type[256];
		char method[256], last_modified[256], expires[256];
		int not_found;

		sscanf(buf, "%s %s %d %s %s %s %s", last_modified, expires,
			&not_found, mime_type, method, localname, urlname);

		AddCache(urlname, localname, string(last_modified)._unpack().data(),
				string(expires)._unpack().data(),
				(not_found?TRUE:FALSE),
				mime_type,
				(!strcmpi(method, "HEAD")?TRUE:FALSE));

		cachefile.get();			// skip the LF
	}

	ifstream mapfile((cwd_path + SEP + MAPFILE).data(), ios::in|ios::nocreate);

	while (mapfile && !mapfile.get(buf, sizeof(buf)).eof())
	{
		char from[256], to[256];
		sscanf(buf, "%s %s", from, to);

		MapURL(to, from);

		mapfile.get();			// skip the LF
	}
}

///////////////////////////////////////////////////////////////////////////////

void wxWWW::SaveMapFile()
{
	ofstream mapfile((cwd_path + SEP + MAPFILE).data());

	ListOfIterator ilist = maps;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		Map* item = (Map*)a;
		mapfile << item->from << " " << item->to << endl;
	}
}

///////////////////////////////////////////////////////////////////////////////

void wxWWW::SaveCacheFile()
{
	ofstream cachefile((cache_path + SEP + CACHEFILE).data());

	ListOf list;
	GetCacheList(list);

	void* a;

	while (list.Pop(a))
	{
		Cache* item = (Cache*)a;

		if (item->filename.length())
		{
			cachefile << item->last_modified._pack() << " " <<
					item->expires._pack() << " " <<
					(item->not_found ? 1 : 0) << " " <<
					item->mime_type << " " <<
					(item->head ? "HEAD" : "GET") << " " <<
					item->filename << " " <<
					item->url <<
					endl;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void wxWWW::SetCache(Bool caching, Bool persistent, const string& path)
{
	nocache = !caching;
	persistent_cache = persistent;
	if (path.length()) cache_path = path;
}

///////////////////////////////////////////////////////////////////////////////

void wxWWW::GetCache(Bool& caching, Bool& persistent, string& path)
{
	caching = !nocache;
	persistent = persistent_cache;
	path = cache_path;
}

///////////////////////////////////////////////////////////////////////////////

void wxWWW::ClearCache()
{
	void* a;

	while (HttpClient::unreachables.Pop(a))
	{
		delete (HttpClient::Unreachable*)a;
	}

	wxHtml::ClearCache();

	SaveCacheFile();
}

///////////////////////////////////////////////////////////////////////////////

void wxWWW::Reload(Bool from_host)
{
	if (from_host) RemoveCache(GetCurrentURL());
	periodic_timer.Stop();
	wxHtml::Reload(FALSE);
}

///////////////////////////////////////////////////////////////////////////////

void wxWWW::MapURL(const string& to, const string& _from)
{
	if (!strncmpi(to.data(), "file:", 5))
		return;

	string from;

	if (!_from.length())
		from = "root";
	else
		from = _from;

	ListOfIterator ilist = maps;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		Map* item = (Map*)a;

		// Already there?

		if ((item->to == to) && (item->from == from))
			return;

		// Or the reverse?

		if ((item->from == to) && (item->to == from))
			return;
	}

	Map* item = new Map;
	item->from = from;
	item->to = to;
	maps.Append(item);
}

///////////////////////////////////////////////////////////////////////////////

string wxWWW::SaveAs(const string& u)
{
	const char* ptr;

	if ((ptr = wxFileSelector("Save file as...", 0, (char*)u.data())) == 0)
		return "";

	return ptr;
}

///////////////////////////////////////////////////////////////////////////////

string wxWWW::Authenticate(const string& realm)
{
	new ::Authenticate(0, realm);

	if (!Authenticate::Ok())
		return "";

	return Authenticate::GetCookie();
}

///////////////////////////////////////////////////////////////////////////////

int wxWWW::GetHTTP(const string& hostname, const string& portname, const string& filename, string& localname, Bool inline_object, Bool post, Bool head, int secure)
{
	outstanding++;
	int stat = HttpClient::Get(hostname, portname, filename, localname, inline_object, post, head, (secure?PROTO_HTTPS:PROTO_HTTP), post_data);
	outstanding--;

	if (stat)
	{
		wxHtml::base = HttpClient::base;
		wxHtml::url = HttpClient::url;
	}

	return stat;
}

///////////////////////////////////////////////////////////////////////////////

int wxWWW::GetFTP(const string& hostname, const string& portname, const string& filename, string& localname, Bool inline_object)
{
	// Using a proxy?

	Bool using_proxies = proxies;

	if ((hostname == "localhost") || 
		(hostname == "127.0.0.1") ||
		(hostname == proxy_host) ||
		(hostname == SimSock::LocalHost()))
	{
		using_proxies = FALSE;
	}

	if (using_proxies)
	{
		outstanding++;
		int stat = HttpClient::Get(hostname, portname, filename, localname, inline_object, FALSE, FALSE, PROTO_FTP);
		outstanding--;
		return stat;
	}
	else
		return 0;
}

///////////////////////////////////////////////////////////////////////////////

int wxWWW::PutHTTP(const string& hostname, const string& portname, const string& filename)
{
	outstanding++;
	int stat = HttpClient::Put(hostname, portname, filename, PROTO_HTTP, Generate());
	outstanding--;
	return stat;
}

///////////////////////////////////////////////////////////////////////////////

class WebMailDialog : public wxDialogBox
{
public:

	WebMailDialog(wxHtml* canvas, const string& from, const string& server, const string& to, const string& subject, const string& text);
	~WebMailDialog();

private:

	static void AttrOK(wxButton& button, wxCommandEvent&);
	static void AttrCancel(wxButton& button, wxCommandEvent&);
	static void AttrBrowse(wxButton& button, wxCommandEvent&);
	static void AttrCurrentUrl(wxButton& button, wxCommandEvent&);

	wxButton* ok;
	wxButton* cancel;
	wxText* to;
	wxText* subject;
	wxMultiText* text;
	wxCheckBox* use_html;
	wxCheckBox* use_mime;
	wxCheckBox* map_8bit;
	wxButton* use_current_url;
	wxButton* browse;
	wxText* attachments;

	wxHtml* canvas;
	string from, server, attached_files_text;
	ListOf attached_files;
	string base;
};

void WebMailDialog::AttrOK(wxButton& button, wxCommandEvent&)
{
	WebMailDialog* attr = (WebMailDialog*)(button.GetParent());

	string from = attr->from;
	string server = attr->server;
	string to = attr->to->GetValue();
	string subject = attr->subject->GetValue();
	string base = attr->base;
	Bool use_html = attr->use_html->GetValue();
	Bool use_mime = attr->use_mime->GetValue();
	Bool map_8bit = attr->map_8bit->GetValue();

	string text = "";

	if (base.length())
	{
		text += "<BASE href=\"";
		text += base;
		text += "\">\n";
	}

#ifdef wx_msw
	const BUF_LEN = 32767;
	char* buf = new char[BUF_LEN];
	attr->text->GetValue(buf, BUF_LEN);
	text += buf;
	delete [] buf;
#else
	text += attr->text->GetValue();
#endif

	attr->canvas->SaveHistory();

	SmtpClient smtp(use_mime, map_8bit);

	void* t;
	while (attr->attached_files.Pop(t))
	{
		smtp.Attach((char*)t);
		free(t);
	}

	string s = "Connect to mail server \'";
	s += server + "\' ";
	int status;

	if ((status = smtp.Connect(server.data())) <= 0)
	{
		s += "Error = ";
		s += Number(status);
		s += "<BR>";
		s += smtp.GetStatus();
		attr->canvas->SetText(s.data());
		delete attr;
		return;
	}

	s += "\' for \'" + attr->from + "\'<P>";

	wxBeginBusyCursor();

	if ((status = smtp.Post(from.data(), "", to.data(), subject.data(), text.data(), text.length(), use_html, base.data())) <= 0)
	{
		wxEndBusyCursor();
		s += "Error = ";
		s += Number(status);
		s += "<BR>";
		s += smtp.GetStatus();
		attr->canvas->SetText(s.data());
		smtp.Disconnect();
		delete attr;
		return;
	}

	wxEndBusyCursor();

	smtp.Disconnect();

	// Show it...

	attr->canvas->SetText(s.data());

	delete attr;
}

void WebMailDialog::AttrCancel(wxButton& button, wxCommandEvent&)
{
	WebMailDialog* attr = (WebMailDialog*)(button.GetParent());
	delete attr;
}

void WebMailDialog::AttrBrowse(wxButton& button, wxCommandEvent&)
{
	WebMailDialog* attr = (WebMailDialog*)(button.GetParent());
	const char* filename;

	if ((filename = wxFileSelector("Attach...", 0, 0, "txt", "*.*")) != 0)
	{
		attr->attached_files.Append(strdup(filename));
		attr->attached_files_text += filename;
		attr->attached_files_text += ';';
		attr->attachments->SetValue((char*)attr->attached_files_text.data());
	}
}

void WebMailDialog::AttrCurrentUrl(wxButton& button, wxCommandEvent&)
{
	WebMailDialog* attr = (WebMailDialog*)(button.GetParent());
	wxHtml* canvas = attr->canvas;

	attr->use_html->SetValue(TRUE);
	attr->use_mime->SetValue(TRUE);
	attr->map_8bit->SetValue(TRUE);
	string s = attr->subject->GetValue();
	s += canvas->GetCurrentURL();
	attr->subject->SetValue((char*)s.data());
	attr->text->SetValue((char*)canvas->GetText());
	attr->base = canvas->GetBaseURL();
}

WebMailDialog::WebMailDialog(wxHtml* _canvas, const string& _from, const string& _server,
		const string& _to, const string& _subject, const string& _text) :
	wxDialogBox(_canvas, "Send mail", TRUE, -1, -1, -1, -1, wxRESIZE_BORDER|wxDEFAULT_DIALOG_STYLE)
{
	canvas = _canvas;
	from = _from;
	server = _server;
	base = attached_files_text = "";

	float w, h;
	GetDC()->GetTextExtent("X", &w, &h);

	to = new wxText(this, 0, "To", (char*)_to.data(), -1, -1, (int)w*60);
	NewLine();

	subject = new wxText(this, 0, "Subject", (char*)_subject.data(), -1, -1, (int)w*60);
	NewLine();

	browse = new wxButton(this, (wxFunction)AttrBrowse, "Attach...");
	attachments = new wxText(this, 0, "", "", -1, -1, (int)w*50);
	attachments->Enable(FALSE);
	NewLine();

	use_html = new wxCheckBox(this, 0, "HTML");
	use_html->SetValue(FALSE);
	use_mime = new wxCheckBox(this, 0, "Mime");
	use_mime->SetValue(TRUE);
	map_8bit = new wxCheckBox(this, 0, "8bit");
	map_8bit->SetValue(TRUE);
	use_current_url = new wxButton(this, (wxFunction)AttrCurrentUrl, "Current page?");
	NewLine();

	text = new wxMultiText(this, 0, "", (char*)_text.data(), -1, -1, (int)w*60, (int)h*10, wxTE_PROCESS_ENTER/*|wxHSCROLL*/);
	NewLine();

	ok = new wxButton(this, (wxFunction)AttrOK, "OK");
	cancel = new wxButton(this, (wxFunction)AttrCancel, "Cancel");

	Fit();
	Centre();
}

WebMailDialog::~WebMailDialog()
{
	void* t;
	while (attached_files.Pop(t))
		free(t);
}

///////////////////////////////////////////////////////////////////////////////

void wxWWW::SendMail(const string& to, const string& subject, const string& text)
{
	char* ptr = 0;

	wxGetResource("WXWEB", "SmtpServer", &ptr, RESOURCE_FILE);

	if (!ptr || !strlen(ptr))
	{
		ptr = wxGetTextFromUser("Hostname of mail server", "Send mail");
		if (!ptr) return;
	}

	string server = ptr;
	ptr = 0;

	wxGetResource("WXWEB", "FromAddress", &ptr, RESOURCE_FILE);

	if (!ptr || !strlen(ptr))
	{
		ptr = wxGetTextFromUser("From (reply-to) address", "Send mail");
		if (!ptr) return;
	}

	string from = ptr;

	wxWriteResource("WXWEB", "SmtpServer", (char*)server.data(), RESOURCE_FILE);
	wxWriteResource("WXWEB", "FromAddress", (char*)from.data(), RESOURCE_FILE);

	WebMailDialog* p = new WebMailDialog(this, from, server, to, subject, text);
	p->Show(TRUE);
}

///////////////////////////////////////////////////////////////////////////////

static ListOf verify_list;

Bool wxWWW::VerifyWeb(const string& the_url, wxHtml* canvas)
{
	ConsoleWrite(the_url+"\n");

	// Open the URL...

	if (!canvas->OpenURL(the_url))
	{
		ConsoleWrite("***ERROR*** NOT FOUND: ");
		ConsoleWrite(the_url);
		ConsoleWrite("\n");
		wxBell();
		return FALSE;
	}

	// Now get all the anchors...

	ListOfIterator ilist = canvas->anchors;
	void* a;
	int ok;

	for (ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		Anchor* anchor = (Anchor*)a;

		if (!anchor->isname && (anchor->name[0] != '#') &&
			(anchor->name.find("/cgi-bin/") == string::npos) &&
			(anchor->name.find("_vti_") == string::npos) )		// FrontPage crap
		{
			string tmp_url = canvas->AbsoluteURL(anchor->name);
			verify_list.Append(new string(tmp_url));
		}
	}

	// Now get all the sub-frames...

	ilist = canvas->sub_frames;

	for (ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxWWW* frame = (wxWWW*)a;

		string tmp_url = frame->GetBaseURL();
		verify_list.Append(new string(tmp_url));
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

void wxWWW::AddCache(const string& u, const string& localname, const string& last_modified, const string& expires, Bool not_found, const string& mime_type, Bool head)
{
	wxHtml::AddCache(u, localname, last_modified, expires, not_found, mime_type, head);
}

///////////////////////////////////////////////////////////////////////////////

int wxWWW::SearchCache(const string& u, string& localname, string& last_modified, string& expires, Bool& not_found, string& mime_type, Bool head)
{
	return wxHtml::SearchCache(u, localname, last_modified, expires, not_found, mime_type, head);
}

///////////////////////////////////////////////////////////////////////////////

int wxWWW::RemoveCache(const string& u)
{
	return wxHtml::RemoveCache(u);
}

///////////////////////////////////////////////////////////////////////////////

Bool wxWWW::VerifyWeb()
{
	Bool status = TRUE;

	ClearCache();
	SetVerifying(TRUE);
	ConsoleBegin();

	int w, h;
	GetSize(&w, &h);

	wxHtml* canvas = Clone(0, drawing_h, w, h, 0);

	canvas->SetFrames(TRUE);
	canvas->SetTables(TRUE);
	canvas->SetImages(TRUE);
	canvas->SetVerifying(TRUE);
	
	string base_path = wxPathOnly((char*)GetBaseURL().data());

	VerifyWeb(GetCurrentURL(), canvas);

	void* a;

	while (!wxHtml::cancel && verify_list.Pop(a))
	{
		string* the_url = (string*)a;

		if (!strncmp(the_url->data(), base_path.data(), base_path.length()))
		{
			string localname, mime_type, last_modified, expires;
			Bool not_found;

			if (!SearchCache(*the_url, localname, last_modified, expires, not_found, mime_type, TRUE))
				VerifyWeb(*the_url, canvas);
		}

		delete the_url;
		wxYield();
	}

	while (verify_list.Pop(a))
		delete (string*)a;

	delete canvas;

	SetVerifying(FALSE);

	return status;
}

///////////////////////////////////////////////////////////////////////////////

int wxWWW::PutGenerated()
{
	string scheme, hostname, portname, filename, section;

	if (!ParseURL(GetCurrentURL().data(), scheme, hostname, portname, filename, section))
		return 0;

	if (!scheme.length())
		scheme = "http";

	if (!strcmpi(scheme.data(), "file"))
		return wxHtml::PutGenerated();
	else if (strcmpi(scheme.data(), "http"))
		return 0;

	string localname;

	wxBeginBusyCursor();
	int status = PutHTTP(hostname, portname, filename);
	if (status) SetDirty(FALSE);
	wxEndBusyCursor();

	return status;
}

///////////////////////////////////////////////////////////////////////////////

void wxWWW::OnSetStatusText(const char* text, int number)
{
	wxFrame* frame = (wxFrame*)GetParent();
	frame->SetStatusText((char*)text, number);
}

