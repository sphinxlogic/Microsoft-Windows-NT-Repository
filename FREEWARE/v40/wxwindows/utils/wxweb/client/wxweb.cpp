//////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  A World Wide Web browser for wxWindows.
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

#ifndef USE_WXWWW
#define USE_WXWWW 1
#endif

#include <fstream.h>

#include "wxhtml.h"

#if USE_WXWWW
#include "wxwww.h"
#include "popmail.h"
#endif

#ifndef USE_MAP
#define USE_MAP 0
#endif

#if USE_WXWWW && USE_MAP
#include <wxtree.h>
#endif

#include "wxweb.h"
#include "listof.h"

#define WXWEB_EXIT 1
#define WXWEB_OPEN_FILE 2
#define WXWEB_OPEN_URL 3
#define WXWEB_PRINT 4
#define WXWEB_POSTSCRIPT 5
#define WXWEB_BACK 6
#define WXWEB_CLEAR_CACHE 7
#define WXWEB_STOP 8
#define WXWEB_SAVE_AS 9
#define WXWEB_ABOUT 10
#define WXWEB_REFRESH 11
#define WXWEB_IMAGES 12
#define WXWEB_VIEW_SOURCE 13
#define WXWEB_NEW_BROWSER 14
#define WXWEB_GO_HOME 15
#define WXWEB_SET_HOME 16
#define WXWEB_VIEW_MAP 17
#define WXWEB_EDIT_MODE 18
#define WXWEB_RELOAD 19
#define WXWEB_FRAMES 20
#define WXWEB_CONFIG 21
#define WXWEB_SHOW_MAIL 22
#define WXWEB_TABLES 23
#define WXWEB_VIEW_GENERATED_SOURCE 24
#define WXWEB_SAVE_GENERATED_AS 25
#define WXWEB_SEND_MAIL 26
#define WXWEB_PUT_GENERATED 27

#define WXWEB_ZOOM_75 40
#define WXWEB_ZOOM_90 41
#define WXWEB_ZOOM_100 42
#define WXWEB_ZOOM_110 43
#define WXWEB_ZOOM_125 44

static int child_count = 0;
class WebFrame;
static wxFrame* main_frame = 0;
static int use_log = 0;
static int save_x, save_y, save_w, save_h;

extern ofstream log;
extern char* RESOURCE_FILE;
extern const char* version;

const TOOLBAR_HEIGHT = 80;
const THROBBER_HEIGHT = 5;

struct Bookmark
{
	string url, title;
};
static ListOf bookmarks;
static unsigned max_bookmarks = 0;

///////////////////////////////////////////////////////////////////////////////

#if USE_WXWWW && USE_MAP

MapFrame::MapFrame(wxWWW* web, char* title, int x, int y, int w, int h):
	wxFrame(main_frame, title, x, y, w, h)
{
	child_count++;

	tree = 0;
	userscale = 1.0F;

	wxMenu* file_menu = new wxMenu;
	file_menu->Append(WXWEB_ZOOM_75, "Zoom 75%");
	file_menu->Append(WXWEB_ZOOM_90, "Zoom 90%");
	file_menu->Append(WXWEB_ZOOM_100, "Zoom 100%");
	file_menu->Append(WXWEB_ZOOM_110, "Zoom 110%");
	file_menu->Append(WXWEB_ZOOM_125, "Zoom 125%");
	file_menu->AppendSeparator();
	file_menu->Append(WXWEB_EXIT, "&Close");

	wxMenuBar* menu_bar = new wxMenuBar;
	menu_bar->Append(file_menu, "&File");

	SetMenuBar(menu_bar);

#ifdef wx_msw
	SetIcon(new wxIcon("wxweb_ico"));
#endif
#ifdef wx_x
	SetIcon(new wxIcon("aiai.xbm"));
#endif

	wxGetResource("WXWEB", "MapUserscale", &userscale, RESOURCE_FILE);

	int ww, hh;
	GetClientSize(&ww, &hh);
	canvas = new MapCanvas(this, -1, -1, ww, hh);
	canvas->SetScrollbars(20, 20, 50, 50, 4, 4);
	canvas->GetDC()->SetPen(wxBLACK_PEN);
	canvas->GetDC()->SetFont(new wxFont(12, wxSWISS, wxNORMAL, wxNORMAL));
	canvas->GetDC()->SetUserScale(userscale, userscale);

	tree = new wxStoredTree(canvas->GetDC());
	tree->Initialize(200);
	tree->AddChild("root");

	ListOfIterator ilist = web->maps;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxWWW::Map* item = (wxWWW::Map*)a;

		string to = item->to;
		string from = item->from;

		if (to.mid(0, 7) == "http://")
			to = to.mid(7, to.length()-7);

		if (from.mid(0, 7) == "http://")
			from = from.mid(7, from.length()-7);

		tree->AddChild((char*)to.data(), (char*)from.data());
	}

	tree->DoLayout();
	tree->Draw();
}

MapFrame::~MapFrame()
{
	wxWriteResource("WXWEB", "MapUserscale", userscale, RESOURCE_FILE);
	if (tree) delete tree;
	if (canvas) delete canvas;
	child_count--;
}

void MapFrame::Draw(wxDC&, Bool)
{
	if (tree) tree->Draw();
}

void MapFrame::OnMenuCommand(int id)
{
	switch (id)
	{
	case WXWEB_ZOOM_75:

		userscale = 0.75F;
		canvas->GetDC()->SetUserScale(0.75F, 0.75F);
		canvas->Clear();
		OnPaint();
		break;

	case WXWEB_ZOOM_90:

		userscale = 0.90F;
		canvas->GetDC()->SetUserScale(0.90F, 0.90F);
		canvas->Clear();
		OnPaint();
		break;

	case WXWEB_ZOOM_100:

		userscale = 1.0F;
		canvas->GetDC()->SetUserScale(1.0F, 1.0F);
		canvas->Clear();
		OnPaint();
		break;

	case WXWEB_ZOOM_110:

		userscale = 1.1F;
		canvas->GetDC()->SetUserScale(1.1F, 1.1F);
		canvas->Clear();
		OnPaint();
		break;

	case WXWEB_ZOOM_125:

		userscale = 1.25F;
		canvas->GetDC()->SetUserScale(1.25F, 1.25F);
		canvas->Clear();
		OnPaint();
		break;

	case WXWEB_EXIT:

		delete this;
		break;
	}
}

MapCanvas::MapCanvas(MapFrame *_frame, int x, int y, int w, int h, long style):
 wxCanvas(_frame, x, y, w, h, style)
{
	frame = _frame;
}

void MapCanvas::OnPaint(void)
{
  frame->Draw(*(GetDC()), TRUE);
}

#endif

///////////////////////////////////////////////////////////////////////////////

SourceFrame::SourceFrame(char* title, int x, int y, int w, int h):
	wxFrame(main_frame, title, x, y, w, h)
{
	child_count++;

	canvas = 0;

	wxMenu* file_menu = new wxMenu;
	file_menu->Append(WXWEB_SAVE_AS, "&Save as...");
	file_menu->AppendSeparator();
	file_menu->Append(WXWEB_EXIT, "&Close");

	wxMenuBar* menu_bar = new wxMenuBar;
	menu_bar->Append(file_menu, "&File");

	SetMenuBar(menu_bar);

#ifdef wx_msw
	SetIcon(new wxIcon("wxweb_ico"));
#endif
#ifdef wx_x
	SetIcon(new wxIcon("aiai.xbm"));
#endif

	int ww, hh;
	GetClientSize(&ww, &hh);

	canvas = new wxHtml(this, 0, 0, ww, hh, wxBORDER);

	Show(TRUE);
}

SourceFrame::~SourceFrame()
{
	child_count--;
	delete canvas;
}

void SourceFrame::OnMenuCommand(int id)
{
	switch (id)
	{
	case WXWEB_SAVE_AS:
	{
#if defined ( wx_msw )
#ifdef WIN32
		char* filename = wxFileSelector("Save file", 0, 0, "html", "*.htm;*.html");
#else
		char* filename = wxFileSelector("Save file", 0, 0, "htm", "*.htm");
#endif
#else
		char* filename = wxFileSelector("Save file", 0, 0, "html", "*.htm*");
#endif
		if (filename)
			canvas->SaveFile(filename);

		break;
	}

	case WXWEB_EXIT:

		delete this;
		break;
	}
}

void SourceFrame::OnSize(int w, int h)
{
	wxFrame::OnSize(w, h);

	GetClientSize(&w, &h);

	if (canvas)
	{
		canvas->SetSize(0, 0, w, h);
		canvas->Reload();
	}
}

///////////////////////////////////////////////////////////////////////////////

void WebConfigDialog::AttrOK(wxButton& button, wxCommandEvent&)
{
	WebConfigDialog* attr = (WebConfigDialog*)(button.GetParent());

#if USE_WXWWW
	attr->canvas->SetProxyServer(
			attr->host->GetValue(),
			attr->port->GetValue(),
			attr->proxies->GetValue(),
			attr->exclude->GetValue());

	attr->canvas->SetCookies(attr->cookies->GetValue());
	use_log = attr->log->GetValue();
#endif

	attr->canvas->SetCache(
			attr->caching->GetValue(),
			!attr->session->GetValue());

	attr->canvas->SetSansSerif(attr->sansserif->GetValue());

	delete attr;
}

void WebConfigDialog::AttrCancel(wxButton& button, wxCommandEvent&)
{
	WebConfigDialog* attr = (WebConfigDialog*)(button.GetParent());
	delete attr;
}

#if USE_WXWWW
WebConfigDialog::WebConfigDialog(wxWindow* parent, wxWWW* _canvas) :
#else
WebConfigDialog::WebConfigDialog(wxWindow* parent, wxHtml* _canvas) :
#endif
	wxDialogBox(parent, "Proxy Server Configuration", TRUE)
{
	canvas = _canvas;

	string h, p, x, path;
	Bool use_proxies = FALSE, use_caching, use_persistent_caching;

#if USE_WXWWW
	canvas->GetProxyServer(h, p, use_proxies, x);
#endif

	canvas->GetCache(use_caching, use_persistent_caching, path);

	host = new wxText(this, 0, "Host name", (char*)h.data(), -1, -1, 10*30);
	NewLine();

	port = new wxText(this, 0, "Port number", (char*)p.data(), -1, -1, 10*6);
	NewLine();

	exclude = new wxText(this, 0, "Proxy exclusion", (char*)x.data(), -1, -1, 10*30);
	NewLine();

	proxies = new wxCheckBox(this, 0, "Enable proxies?");
	proxies->SetValue(use_proxies);
	NewLine();

	caching = new wxCheckBox(this, 0, "Enable caching?");
	caching->SetValue(use_caching);
	session = new wxCheckBox(this, 0, "This session only?");
	session->SetValue(!use_persistent_caching);
	NewLine();

	sansserif = new wxCheckBox(this, 0, "Use sans-serif font?");
	sansserif->SetValue(canvas->GetSansSerif());

#if USE_WXWWW
	cookies = new wxCheckBox(this, 0, "Accept cookies?");
	cookies->SetValue(canvas->GetCookies());

	log = new wxCheckBox(this, 0, "Logfile?");
	log->SetValue(use_log);
#endif

	NewLine();

	ok = new wxButton(this, (wxFunction)AttrOK, "OK");
	cancel = new wxButton(this, (wxFunction)AttrCancel, "Cancel");

	Fit();
	Centre();
}

///////////////////////////////////////////////////////////////////////////////

void BusyTimer::Notify()
{
	static char* colours[16] = {"black",/*"silver*/"yellow","gray","white","maroon","red",
		"purple","fuschia","green","lime","olive","yellow","navy","blue",
		"teal","aqua"};

	static int cnt = 0;

	wxColour* background = wxHtml::HtmlColour(colours[cnt++%16]);
	frame->throbber->SetBackground(wxTheBrushList->FindOrCreateBrush(background, wxSOLID));
	frame->throbber->Refresh();
}

///////////////////////////////////////////////////////////////////////////////

WebFrame::WebFrame(char* title, int x, int y, int w, int h):
	wxFrame(0, title, x, y, w, h),
		busy_timer(this)
{
	toolbar = throbber = 0;
	allow_exit = FALSE;
	canvas = 0;
	child_count++;
}

///////////////////////////////////////////////////////////////////////////////

WebFrame::~WebFrame()
{
	busy_timer.Stop();

	int x, y, w, h;
	GetPosition(&x, &y); GetSize(&w, &h);

	if ((x != save_x) || (y != save_y) ||
			(w != save_w) || (h != save_h))
	{
		wxWriteResource("WXWEB", "Left", x, RESOURCE_FILE);
		wxWriteResource("WXWEB", "Top", y, RESOURCE_FILE);
		wxWriteResource("WXWEB", "Width", w, RESOURCE_FILE);
		wxWriteResource("WXWEB", "Height", h, RESOURCE_FILE);
	}

	if (canvas) delete canvas;
	if (toolbar) delete toolbar;
	if (throbber) delete throbber;

	child_count--;
}

///////////////////////////////////////////////////////////////////////////////

void WebFrame::SaveState(Bool full)
{
	ListOfIterator ilist = bookmarks;
	void* a;
	unsigned i = 1;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		Bookmark* bk = (Bookmark*)a;
		string s = "Bookmark_url_";
		s += Number(i);
		wxWriteResource("WXWEB", s.data(), (char*)bk->url.data(), RESOURCE_FILE);
		s = "Bookmark_title_";
		s += Number(i);
		wxWriteResource("WXWEB", s.data(), (char*)bk->title.data(), RESOURCE_FILE);
		i++;
	}

	while (i < max_bookmarks)
	{
		string s = "Bookmark_url_";
		s += Number(i);
		wxWriteResource("WXWEB", s.data(), (char*)0, RESOURCE_FILE);
		s = "Bookmark_title_";
		s += Number(i);
		wxWriteResource("WXWEB", s.data(), (char*)0, RESOURCE_FILE);
		i++;
	}

	max_bookmarks = wxMax(max_bookmarks, bookmarks.Count());

	if (!full) return;

	string host, port, exclude, path;
	Bool proxies, caching, persistent_caching;

#if USE_WXWWW
	canvas->GetProxyServer(host, port, proxies, exclude);
	canvas->GetCache(caching, persistent_caching, path);
#endif

	wxWriteResource("WXWEB", "ProxyServer", (char*)host.data(), RESOURCE_FILE);
	wxWriteResource("WXWEB", "ProxyPort", (char*)port.data(), RESOURCE_FILE);
	wxWriteResource("WXWEB", "ProxyExclude", (char*)exclude.data(), RESOURCE_FILE);
	wxWriteResource("WXWEB", "Proxies", proxies, RESOURCE_FILE);
	wxWriteResource("WXWEB", "Caching", caching, RESOURCE_FILE);
	wxWriteResource("WXWEB", "Persistent", persistent_caching, RESOURCE_FILE);
	wxWriteResource("WXWEB", "Family", canvas->GetSansSerif()?"wxDEFAULT":"wxROMAN", RESOURCE_FILE);
#if USE_WXWWW
	wxWriteResource("WXWEB", "Cookies", canvas->GetCookies(), RESOURCE_FILE);
#endif
	wxWriteResource("WXWEB", "Log", use_log, RESOURCE_FILE);
}

///////////////////////////////////////////////////////////////////////////////

void WebFrame::WebFirst(wxButton& but, wxCommandEvent&)
{
	WebFrame* frame = (WebFrame*)but.GetParent()->GetParent();
	frame->canvas->Back(-1);
	frame->canvas->SetFocus();
}

void WebFrame::WebBack(wxButton& but, wxCommandEvent&)
{
	WebFrame* frame = (WebFrame*)but.GetParent()->GetParent();
	frame->canvas->Back(1);
	frame->canvas->SetFocus();
}

void WebFrame::WebStop(wxButton& but, wxCommandEvent&)
{
	WebFrame* frame = (WebFrame*)but.GetParent()->GetParent();
	frame->Command(WXWEB_STOP);
	frame->canvas->SetFocus();
}

void WebFrame::WebExit(wxButton& but, wxCommandEvent&)
{
	WebFrame* frame = (WebFrame*)but.GetParent()->GetParent();
	frame->Command(WXWEB_EXIT);
}

void WebFrame::WebHome(wxButton& but, wxCommandEvent&)
{
	WebFrame* frame = (WebFrame*)but.GetParent()->GetParent();
	frame->Command(WXWEB_GO_HOME);
	frame->canvas->SetFocus();
}

void WebFrame::WebEdit(wxButton& but, wxCommandEvent&)
{
	WebFrame* frame = (WebFrame*)but.GetParent()->GetParent();
	frame->Command(WXWEB_EDIT_MODE);
	frame->canvas->SetFocus();
}

void WebFrame::WebPrint(wxButton& but, wxCommandEvent&)
{
	WebFrame* frame = (WebFrame*)but.GetParent()->GetParent();
	frame->Command(WXWEB_PRINT);
	frame->canvas->SetFocus();
}

void WebFrame::WebMail(wxButton& but, wxCommandEvent&)
{
	WebFrame* frame = (WebFrame*)but.GetParent()->GetParent();
	frame->Command(WXWEB_SEND_MAIL);
	frame->canvas->SetFocus();
}

void WebFrame::WebReload(wxButton& but, wxCommandEvent&)
{
	WebFrame* frame = (WebFrame*)but.GetParent()->GetParent();
	frame->Command(WXWEB_RELOAD);
	frame->canvas->SetFocus();
}

void WebFrame::WebBookit(wxButton& but, wxCommandEvent&)
{
	WebFrame* frame = (WebFrame*)but.GetParent()->GetParent();
	string url = frame->canvas->GetCurrentURL();
	ListOfIterator ilist = bookmarks;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		Bookmark* bk = (Bookmark*)a;

		if (bk->url == url)
		{
			ilist.Delete();
			delete bk;
			frame->SetStatusText("Bookmark deleted!");
			frame->SaveState();
			return;
		}
	}

	Bookmark* bk = new Bookmark;
	bk->url = url;
	bk->title = frame->canvas->GetCurrentTitle();
	if (!bk->title.length()) bk->title = url;
	bookmarks.Append(bk);
	frame->SetStatusText("Bookmark added!");
	frame->SaveState();

	frame->canvas->SetFocus();
}

void WebFrame::WebBookmarks(wxButton& but, wxCommandEvent&)
{
	WebFrame* frame = (WebFrame*)but.GetParent()->GetParent();
	ListOfIterator ilist = bookmarks;
	void* a;
	string s = "<HTML><HEAD><TITLE>Bookmarks</TITLE></HEAD>\n"
				"<BODY bgcolor=white text=black>\n"
				"<H3>Bookmarks...</H3>\n";

	s += "<BR><LI><A href=\"";
#ifdef wx_msw
	s += "file:///C:\\Program Files\\Netscape\\Navigator\\bookmark.htm";
#else
	s += "file:///";
	char buf[256];
	s += wxGetHomeDir(buf);
	s += "/.netscape/bookmarks.html";
#endif
	s += "\">Netscape bookmarks</A>\n";

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		Bookmark* bk = (Bookmark*)a;

		s += "<LI><A href=\"";
		s += bk->url;
		s += "\">";
		s += bk->title;
		s += "</A>\n";
	}

	s += "<P></BODY></HTML>";

	frame->canvas->SaveHistory();
	frame->canvas->SetText(s.data());
	frame->canvas->SetFocus();
}

#if ALLOW_SITE_MODE
void WebFrame::WebCompile(wxButton& but, wxCommandEvent&)
{
	WebFrame* frame = (WebFrame*)but.GetParent()->GetParent();

	if (strcmpi(but.GetLabel(), "  Done  "))
	{
		frame->b_home->Enable(FALSE);
		frame->b_back->Enable(FALSE);
		frame->b_print->Enable(FALSE);
		frame->b_reload->Enable(FALSE);
		frame->b_compile->Enable(FALSE);
		frame->b_verify->Enable(FALSE);
		frame->url->Enable(FALSE);
		frame->b_stop->Enable(TRUE);

		frame->canvas->CompileSiteAll();

		frame->b_stop->Enable(FALSE);
		frame->b_compile->Enable(TRUE);
		but.SetLabel("  Done  ");
	}
	else
	{
		frame->canvas->ConsoleClear();
		frame->canvas->ConsoleDone();

		frame->b_home->Enable(TRUE);
		frame->b_back->Enable(TRUE);
		frame->b_print->Enable(TRUE);
		frame->b_reload->Enable(TRUE);
		frame->b_compile->Enable(TRUE);
		frame->b_verify->Enable(TRUE);
		frame->url->Enable(TRUE);
		but.SetLabel(" Compile ");
	}

	frame->canvas->SetFocus();
}
#endif

void WebFrame::WebVerify(wxButton& but, wxCommandEvent&)
{
	WebFrame* frame = (WebFrame*)but.GetParent()->GetParent();

	if (strcmpi(but.GetLabel(), "  Done  "))
	{
		frame->b_home->Enable(FALSE);
		frame->b_back->Enable(FALSE);
		frame->b_print->Enable(FALSE);
		frame->b_reload->Enable(FALSE);
#if ALLOW_SITE_MODE
		frame->b_compile->Enable(FALSE);
#endif
		frame->b_verify->Enable(FALSE);
		frame->url->Enable(FALSE);
		frame->b_stop->Enable(TRUE);

#if ALLOW_SITE_MODE
		if (frame->canvas->IsWSML())
			frame->canvas->VerifySiteAll(TRUE);
		else
#endif
		{
#if USE_WXWWW
			frame->canvas->VerifyWeb();
#endif
		}
		
		frame->b_verify->Enable(TRUE);
		frame->b_stop->Enable(FALSE);
		
		but.SetLabel("  Done  ");
	}
	else
	{
		frame->canvas->ConsoleClear();
		frame->canvas->ConsoleDone();

		frame->b_home->Enable(TRUE);
		frame->b_back->Enable(TRUE);
		frame->b_print->Enable(TRUE);
		frame->b_reload->Enable(TRUE);
#if ALLOW_SITE_MODE
		frame->b_compile->Enable(TRUE);
#endif
		frame->b_verify->Enable(TRUE);
		frame->url->Enable(TRUE);
		
		but.SetLabel(" Verify ");
	}

	frame->canvas->SetFocus();
}

void WebFrame::WebOpen(wxText& but, wxCommandEvent& event)
{
	WebFrame* frame = (WebFrame*)but.GetParent()->GetParent();

	if (event.eventType == wxEVENT_TYPE_TEXT_ENTER_COMMAND)
	{
		string u = but.GetValue();

		if (!strstr(u.data(), "://"))
			u = string("http://") + u;

		frame->canvas->OpenURL(u);
	}

	//frame->canvas->SetFocus();
}

///////////////////////////////////////////////////////////////////////////////

WebFrame* WebFrame::CreateBrowser(int x, int y, int w, int h)
{
	WebFrame* frame = new WebFrame((char*)(string("wxWeb/")+version).data(), x, y, w, h);

	if (!main_frame) main_frame = frame;

	char* ptr = 0;
	wxGetResource("WXWEB", "Home", &ptr, RESOURCE_FILE);
	frame->home = ptr;

	wxMenu* file_menu = new wxMenu;
	file_menu->Append(WXWEB_NEW_BROWSER, "&New browser");
	file_menu->AppendSeparator();
#if USE_WXWWW
	file_menu->Append(WXWEB_OPEN_URL, "Open &URL...");
#endif
	file_menu->Append(WXWEB_OPEN_FILE, "&Open file...");
	file_menu->Append(WXWEB_SAVE_AS, "Save file &as...");
#if ALLOW_SITE_MODE
	file_menu->Append(WXWEB_SAVE_GENERATED_AS, "Save generated file &as...");
#endif
	file_menu->AppendSeparator();
#ifdef wx_msw
	file_menu->Append(WXWEB_PRINT, "&Print");
#endif
#if USE_POSTSCRIPT
	file_menu->Append(WXWEB_POSTSCRIPT, "Po&script (EPS) file...");
#endif
	file_menu->AppendSeparator();
	file_menu->Append(WXWEB_CLEAR_CACHE, "&Clear cache");
	file_menu->AppendSeparator();
	file_menu->Append(WXWEB_EXIT, "E&xit");

	wxMenu* view_menu = new wxMenu;
	view_menu->Append(WXWEB_RELOAD, "&Reload (from host)");
	view_menu->Append(WXWEB_BACK, "&Back");
	view_menu->Append(WXWEB_STOP, "&Stop");
	view_menu->AppendSeparator();
	view_menu->Append(WXWEB_GO_HOME, "Go to &home page");
	view_menu->AppendSeparator();
	view_menu->Append(WXWEB_IMAGES, "Toggle &images");
	view_menu->Append(WXWEB_FRAMES, "Toggle &frames");
	view_menu->Append(WXWEB_TABLES, "Toggle &tables");
	view_menu->AppendSeparator();
	view_menu->Append(WXWEB_REFRESH, "&Refresh screen");
	view_menu->AppendSeparator();
#if ALLOW_EDIT_MODE
	view_menu->Append(WXWEB_EDIT_MODE, "&Edit");
#endif
	view_menu->Append(WXWEB_VIEW_SOURCE, "View document s&ource...");
#if ALLOW_SITE_MODE
	view_menu->Append(WXWEB_VIEW_GENERATED_SOURCE, "View &generated source...");
#endif
#if USE_WXWWW && USE_MAP
	view_menu->Append(WXWEB_VIEW_MAP, "Map of known space...");
#endif
#if USE_WXWWW
	view_menu->Append(WXWEB_SHOW_MAIL, "Show &mail...");
	view_menu->Append(WXWEB_SEND_MAIL, "Sen&d mail...");
#endif

	wxMenu* optn_menu = new wxMenu;
	optn_menu->Append(WXWEB_SET_HOME, "Set document &as home");
	optn_menu->Append(WXWEB_CONFIG, "&Configure...");

	wxMenu* help_menu = new wxMenu;
	help_menu->Append(WXWEB_ABOUT, "&About...");

	wxMenuBar* menu_bar = new wxMenuBar;
	menu_bar->Append(file_menu, "&File");
	menu_bar->Append(view_menu, "&View");
	menu_bar->Append(optn_menu, "&Options");
	menu_bar->Append(help_menu, "&Help");
	frame->SetMenuBar(menu_bar);

#if USE_WXWWW
	frame->CreateStatusLine(2);
#ifdef wx_msw
	int widths[2] = {w*2/3, -1};
	frame->SetStatusWidths(2, widths);
#endif
#else
	frame->CreateStatusLine(1);
#endif

	frame->toolbar = new wxPanel(frame, 0, 0, w, TOOLBAR_HEIGHT, wxBORDER|wxUSER_COLOURS);
	frame->throbber = new wxPanel(frame, 0, TOOLBAR_HEIGHT, w, THROBBER_HEIGHT, wxBORDER|wxUSER_COLOURS);

	wxColour* background = wxTheColourDatabase->FindColour("LIGHT GREY");
	frame->toolbar->SetBackground(wxTheBrushList->FindOrCreateBrush(background, wxSOLID));
	frame->toolbar->SetButtonFont(wxTheFontList->FindOrCreateFont(10, wxDEFAULT, wxNORMAL, wxNORMAL));

	frame->b_first = new wxButton(frame->toolbar, (wxFunction)WebFirst, "|<-");
	frame->b_back = new wxButton(frame->toolbar, (wxFunction)WebBack, "<-");
	frame->b_home = new wxButton(frame->toolbar, (wxFunction)WebHome, "Home");
	frame->b_reload = new wxButton(frame->toolbar, (wxFunction)WebReload, "Reload");
#if ALLOW_EDIT_MODE
	frame->b_edit = new wxButton(frame->toolbar, (wxFunction)WebEdit, "Edit");
#endif
	frame->b_print = new wxButton(frame->toolbar, (wxFunction)WebPrint, "Print");
	frame->b_stop = new wxButton(frame->toolbar, (wxFunction)WebStop, "Stop");
	frame->b_stop->Enable(FALSE);
	frame->b_verify = new wxButton(frame->toolbar, (wxFunction)WebVerify, "Verify");
#if ALLOW_SITE_MODE
	frame->b_compile = new wxButton(frame->toolbar, (wxFunction)WebCompile, "Compile");
	frame->b_compile->Enable(FALSE);
#endif
#if USE_WXWWW
	frame->b_mail = new wxButton(frame->toolbar, (wxFunction)WebMail, "Mail");
#endif
	frame->b_exit = new wxButton(frame->toolbar, (wxFunction)WebExit, "Exit");

	frame->toolbar->NewLine();

	frame->GetClientSize(&w, &h);
#ifdef wx_msw
	frame->url = new wxText(frame->toolbar, (wxFunction)WebOpen, " Document:", (char*)frame->home.data(), -1, -1, w-90, -1, wxTE_PROCESS_ENTER);
#else
	frame->url = new wxText(frame->toolbar, (wxFunction)WebOpen, " Document:", (char*)frame->home.data(), -1, -1, w-110, -1, wxTE_PROCESS_ENTER);
#endif
	frame->b_bookit = new wxButton(frame->toolbar, (wxFunction)WebBookit, "+/-");
	frame->b_bookmarks = new wxButton(frame->toolbar, (wxFunction)WebBookmarks, "?");

	background = wxTheColourDatabase->FindColour("LIGHT GREY");
	frame->throbber->SetBackground(wxTheBrushList->FindOrCreateBrush(background, wxSOLID));
	frame->throbber->Refresh();

#ifdef wx_msw
	frame->SetIcon(new wxIcon("wxweb_ico"));
#endif
#ifdef wx_x
	frame->SetIcon(new wxIcon("aiai.xbm"));
#endif

	frame->canvas = new WebCanvas(frame, 0, TOOLBAR_HEIGHT+THROBBER_HEIGHT, w, h - (TOOLBAR_HEIGHT+THROBBER_HEIGHT));
	frame->Show(TRUE);

	return frame;
}

///////////////////////////////////////////////////////////////////////////////

Bool WebFrame::OnClose()
{
	//if (!allow_exit) return FALSE;

	allow_exit = FALSE;

	if ((this == main_frame) && (child_count != 1))
		return FALSE;

#if USE_WXWWW
	canvas->HttpClient::Cancel();
#endif

	canvas->Stop();
	//if (canvas->Stop())
	//	return FALSE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

void WebFrame::OnSize(int w, int h)
{
	wxFrame::OnSize(w, h);

	if (canvas)
	{
		GetClientSize(&w, &h);
		toolbar->SetSize(0, 0, w, TOOLBAR_HEIGHT);
		toolbar->Refresh();
		throbber->SetSize(0, TOOLBAR_HEIGHT, w, THROBBER_HEIGHT);
		throbber->Refresh();
		canvas->SetSize(0, TOOLBAR_HEIGHT+THROBBER_HEIGHT, w, h - (TOOLBAR_HEIGHT+THROBBER_HEIGHT));
		canvas->Reload();
		canvas->Refresh();
	}
}

///////////////////////////////////////////////////////////////////////////////

void WebFrame::OnMenuCommand(int id)
{
	switch (id)
	{
	case WXWEB_SAVE_AS:
	{
		string default_filename = canvas->GetCurrentFilename();

		char* filename = wxFileSelector("Save file", 0, (char*)default_filename.data(), "html", "*.*");
		if (filename)
			canvas->SaveFile(filename);

		break;
	}

#if ALLOW_SITE_MODE
	case WXWEB_SAVE_GENERATED_AS:
	{
		string default_filename = canvas->GetCurrentFilename();

		char* filename = wxFileSelector("Save file", 0, (char*)default_filename.data(), "html", "*.*");
		if (filename)
			canvas->SaveGeneratedFile(filename);

		break;
	}
#endif

#if ALLOW_EDIT_MODE
	case WXWEB_PUT_GENERATED:
	{
		canvas->PutGenerated();
		break;
	}
#endif

	case WXWEB_OPEN_FILE:
	{
		char* filename = wxFileSelector("Open file", 0, 0, "html", "*.*");
		if (filename)
			if (strlen(filename))
			{
				string u = "file:///";
				u += filename;
#if USE_WXWWW && USE_MAP
				canvas->MapURL(u, "");
#endif
				canvas->OpenURL(u);
			}

		break;
	}

	case WXWEB_OPEN_URL:
	{
		char* u = wxGetTextFromUser("Enter text", "Open URL");

		if (u)
			if (strlen(u))
			{
#if USE_WXWWW && USE_MAP
				canvas->MapURL(canvas->AbsoluteURL(u), "");
#endif
				canvas->OpenURL(u);
			}

		break;
	}

#ifdef wx_msw
	case WXWEB_PRINT:
	{
		wxPrinterDC dc(0, 0, 0, TRUE);
		if (dc.Ok())
		{
			dc.StartDoc(GetTitle());
			dc.StartPage();
			canvas->Parse(&dc);
			dc.EndPage();
			dc.EndDoc();
		}
		break;
	}
#endif

#if USE_POSTSCRIPT
	case WXWEB_POSTSCRIPT:
	{
		wxPostScriptDC dc(0, TRUE);
		if (dc.Ok())
		{
			dc.StartDoc(GetTitle());
			dc.StartPage();
			canvas->Parse(&dc);
			dc.EndPage();
			dc.EndDoc();
		}
		break;
	}
#endif

	case WXWEB_CLEAR_CACHE:
	{
		canvas->ClearCache();
		canvas->Reload();
		break;
	}

	case WXWEB_BACK:
	{
		canvas->Back(1);
		break;
	}

	case WXWEB_STOP:
	{
#if USE_WXWWW
		canvas->HttpClient::Cancel();
#endif
		canvas->Stop();
		b_stop->Enable(FALSE);
		busy_timer.Stop();
		break;
	}

	case WXWEB_ABOUT:

		canvas->About();
		break;

	case WXWEB_CONFIG:
	{
		WebConfigDialog* p = new WebConfigDialog(this, canvas);
		p->Show(TRUE);
		SaveState(TRUE);
		break;
	}

	case WXWEB_SHOW_MAIL:

		ShowMail();
		break;

	case WXWEB_SEND_MAIL:

#if USE_WXWWW
		canvas->SendMail();
#endif
		break;

	case WXWEB_IMAGES:

		canvas->SetImages();
		canvas->Reload();
		break;

	case WXWEB_FRAMES:

		canvas->SetFrames();
		canvas->Reload();
		break;

	case WXWEB_TABLES:

		canvas->SetTables();
		canvas->Reload();
		break;

	case WXWEB_RELOAD:

#if USE_WXWWW
		canvas->HttpClient::Cancel();
#endif

		canvas->Reload(TRUE);
		break;

	case WXWEB_REFRESH:

		canvas->Reload();
		break;

	case WXWEB_GO_HOME:
	{
		canvas->OpenURL(home);
		break;
	}

	case WXWEB_SET_HOME:
	{
		const char* ptr = canvas->GetCurrentURL().data();
		wxWriteResource("WXWEB", "Home", (char*)ptr, RESOURCE_FILE);
		home = ptr;
		break;
	}

	case WXWEB_VIEW_MAP:
	{
#if USE_WXWWW && USE_MAP
		int x, y, w, h;
		GetPosition(&x, &y); GetSize(&w, &h);
		MapFrame* f = new MapFrame((wxWWW*)canvas, "Known space", x, y, w, h);
		f->Show(TRUE);
		break;
#endif
	}

	case WXWEB_EDIT_MODE:
	{
#if ALLOW_EDIT_MODE
		if (canvas->IsEdit())
		{
			if (canvas->IsDirty())
				OnMenuCommand(WXWEB_PUT_GENERATED);

			b_edit->SetLabel("  Edit  ");
		}
		else
		{
			b_edit->SetLabel("  Save  ");
		}

		canvas->SetEdit(!canvas->IsEdit());
#endif
		break;
	}

#if ALLOW_SITE_MODE
	case WXWEB_VIEW_GENERATED_SOURCE:
	{
		string s = canvas->Generate();

		const char* url = canvas->GetCurrentURL().data();
		
		int x, y, w, h;
		GetPosition(&x, &y); GetSize(&w, &h);
		SourceFrame* f = new SourceFrame((char*)url, x, y, w, h);
		f->canvas->SetPlainText(TRUE);
		f->canvas->SetText(s.data());
		f->canvas->SetPlainText(FALSE);
		break;
	}
#endif

	case WXWEB_VIEW_SOURCE:
	{
		const char* text = canvas->GetText();
		
		if (!text || !strlen(text))
			break;

		const char* url = canvas->GetCurrentURL().data();

		int x, y, w, h;
		GetPosition(&x, &y); GetSize(&w, &h);
		SourceFrame* f = new SourceFrame((char*)url, x, y, w, h);
		f->canvas->SetPlainText(TRUE);
		f->canvas->SetText(text);
		f->canvas->SetPlainText(FALSE);
		break;
	}

	case WXWEB_NEW_BROWSER:
	{
		int x, y, w, h;
		GetPosition(&x, &y); GetSize(&w, &h);
		WebFrame* frame = WebFrame::CreateBrowser(x+20, y+20, w, h);

		const char* u = canvas->GetCurrentURL().data();

		if (u && strlen(u))
			frame->canvas->OpenURL(u);

		break;
	}

	case WXWEB_EXIT:

		allow_exit = TRUE;

		if (!OnClose())
		{
			wxBell();
			break;
		}

		delete this;
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////

static WebApp MyWebApp;

///////////////////////////////////////////////////////////////////////////////

wxFrame* WebApp::OnInit()
{
	int x = 100, y = 100, w = 600, h = 500;
	int proxies = 0, caching = 1, persistent_caching = 0, cookies = 1;
	char* host = 0;
	char* port = 0;
	char* exclude = 0;

	wxGetResource("WXWEB", "Left", &x, RESOURCE_FILE);
	wxGetResource("WXWEB", "Top", &y, RESOURCE_FILE);
	wxGetResource("WXWEB", "Width", &w, RESOURCE_FILE);
	wxGetResource("WXWEB", "Height", &h, RESOURCE_FILE);
	wxGetResource("WXWEB", "ProxyServer", &host, RESOURCE_FILE);
	wxGetResource("WXWEB", "ProxyPort", &port, RESOURCE_FILE);
	wxGetResource("WXWEB", "Proxies", &proxies, RESOURCE_FILE);
	wxGetResource("WXWEB", "Caching", &caching, RESOURCE_FILE);
	wxGetResource("WXWEB", "Persistent", &persistent_caching, RESOURCE_FILE);
	wxGetResource("WXWEB", "ProxyExclude", &exclude, RESOURCE_FILE);
	wxGetResource("WXWEB", "Cookies", &cookies, RESOURCE_FILE);
	wxGetResource("WXWEB", "Log", &use_log, RESOURCE_FILE);

	for (int i = 1; 1; i++)
	{
		string s = "Bookmark_url_";
		s += Number(i);
		char* url = 0;
		if (!wxGetResource("WXWEB", s.data(), &url, RESOURCE_FILE))
			break;
		s = "Bookmark_title_";
		s += Number(i);
		char* title = 0;
		if (!wxGetResource("WXWEB", s.data(), &title, RESOURCE_FILE))
			break;

		Bookmark* bk = new Bookmark;
		bk->url = url;
		bk->title = title;
		bookmarks.Append(bk);

		max_bookmarks = i;
	}

	if (!exclude)
		exclude = "localhost, 127.0.0.1";

	WebFrame* frame = WebFrame::CreateBrowser(x, y, w, h);

#if USE_WXWWW
	if (host && strlen(host) && port && strlen(port))
		frame->canvas->SetProxyServer(host, port, proxies, exclude);
	else
		frame->canvas->SetProxyServer("", "", FALSE, "");

	frame->canvas->SetCookies(cookies);

	if (use_log)
		log.open((frame->canvas->GetCwdPath() + SEP + "wxweb.log").data());

#endif

	frame->canvas->SetCache(caching, persistent_caching);

	wxYield();

	if (argc == 2)
	{
#if USE_WXWWW && USE_MAP
		frame->canvas->MapURL(argv[1], "");
#endif
		frame->canvas->OpenURL(argv[1]);
	}
	else if (frame->home.length())
	{
#if USE_WXWWW && USE_MAP
		frame->canvas->MapURL(frame->home, "");
#endif
		frame->canvas->OpenURL(frame->home);
	}

	frame->GetPosition(&save_x, &save_y);
	frame->GetSize(&save_w, &save_h);

	return frame;
}

///////////////////////////////////////////////////////////////////////////////

#if USE_WXWWW && USE_MAP

void MapCanvas::OnEvent(wxMouseEvent& event)
{
	WebFrame *web = (WebFrame*)main_frame;
	char* name;

	if ((name = frame->tree->Selected(event)) != 0)
	{
		wxSetCursor(new wxCursor(wxCURSOR_HAND));

		web->SetStatusText(name);

		if (!event.LeftUp()) return;

		wxSetCursor(new wxCursor(wxCURSOR_ARROW));
		web->SetStatusText("Loading...");
		wxBeginBusyCursor();
		web->canvas->OpenURL((string("http://")+name));
		wxEndBusyCursor();
		web->SetStatusText("Done");

		//delete frame;
	}
	else
	{
		wxSetCursor(new wxCursor(wxCURSOR_ARROW));
		web->SetStatusText("");
	}
}

#endif

///////////////////////////////////////////////////////////////////////////////
// Derived classes can override to handle this themselves...
///////////////////////////////////////////////////////////////////////////////

void WebCanvas::OnDownload(const string&, const string&)
{
}

///////////////////////////////////////////////////////////////////////////////
// Derived classes can override to handle this themselves...
///////////////////////////////////////////////////////////////////////////////

void WebCanvas::OnOpen(const string& u)
{
	if (u.length())
		((WebFrame*)GetParent())->url->SetValue((char*)u.data());

	if (!DeferredLoads())
	{
		frame->b_stop->Enable(FALSE);
		frame->busy_timer.Stop();
		wxColour* background = wxTheColourDatabase->FindColour("LIGHT GREY");
		frame->throbber->SetBackground(wxTheBrushList->FindOrCreateBrush(background, wxSOLID));
		frame->throbber->Refresh();

#if USE_WXWWW
		SaveCacheFile();
#endif
	}

#if USE_WXWWW
	static string last_server = "";

	if (GetCurrentServer() != last_server)
	{
		last_server = GetCurrentServer();
		frame->SetStatusText((char*)last_server.data(), 1);
	}

	if (IsSecure() && !prev_secure)
	{
		wxColour* background = wxTheColourDatabase->FindColour("BLUE");
		frame->toolbar->SetBackground(wxTheBrushList->FindOrCreateBrush(background, wxSOLID));
		frame->toolbar->Refresh();
	}
	else if (!IsSecure() && prev_secure)
	{
		wxColour* background = wxTheColourDatabase->FindColour("LIGHT GREY");
		frame->toolbar->SetBackground(wxTheBrushList->FindOrCreateBrush(background, wxSOLID));
		frame->toolbar->Refresh();
	}

	prev_secure = IsSecure();
#endif
}

///////////////////////////////////////////////////////////////////////////////
// Derived classes can override to handle this themselves...
///////////////////////////////////////////////////////////////////////////////

int WebCanvas::OpenURL(const string& url, Bool post, Bool head, Bool put)
{
	frame->busy_timer.Start(500);
	frame->b_stop->Enable(TRUE);

	int ret = wxHtml::OpenURL(url, post, head, put);

	if (!ret)
	{
		frame->b_stop->Enable(FALSE);
		frame->busy_timer.Stop();
	}
	else
	{
#if ALLOW_SITE_MODE
		frame->b_compile->Enable(IsWSML());
#endif
	}

	return ret;
}

///////////////////////////////////////////////////////////////////////////////

void WebFrame::ShowMail()
{
#if USE_WXWWW

	string text = "";
	PopClient pop;
	int status;

	char* ptr = 0;

	wxGetResource("WXWEB", "PopServer", &ptr, RESOURCE_FILE);

	if (!ptr || !strlen(ptr))
	{
		ptr = wxGetTextFromUser("Hostname of mail server (mail will not be deleted)", "Show mail");
		if (!ptr) return;
	}

	string popserver = ptr;
	ptr = 0;

	wxGetResource("WXWEB", "PopUserid", &ptr, RESOURCE_FILE);

	if (!ptr || !strlen(ptr))
	{
		ptr = wxGetTextFromUser("Userid for mail account", "Show mail");
		if (!ptr) return;
	}

	string userid = ptr;

	ptr = wxGetTextFromUser("Password (will not be recorded)", "Show mail");
	if (!ptr) return;
	string passwd = ptr;

	wxWriteResource("WXWEB", "PopServer", (char*)popserver.data(), RESOURCE_FILE);
	wxWriteResource("WXWEB", "PopUserid", (char*)userid.data(), RESOURCE_FILE);

	canvas->SaveHistory();

	text += "Connect to mail server \'";
	text += popserver + "\'";

	if ((status = pop.Connect(popserver.data())) <= 0)
	{
		text += "Error = ";
		text += Number(status);
		text += "<BR>";
		text += pop.GetStatus();
		canvas->SetText(text.data());
		return;
	}

	text += "\' for \'" + userid + "\'<P>";

	if ((status = pop.Login(userid.data(), passwd.data())) <= 0)
	{
		text += "Error = ";
		text += Number(status);
		text += "<BR>";
		text += pop.GetStatus();
		canvas->SetText(text.data());
		pop.Disconnect();
		return;
	}

	int n = pop.GetCount();
	char** msgs = (char**)new char[sizeof(char*)*n];

	text += "Messages = ";
	text += Number(n);
	text += "<OL>";

	wxBeginBusyCursor();

	int i;

	for (i = 1; i <= n; i++)
	{
		text += "<LI>";

		if (pop.Retrieve(i) <= 0)
		{
			text += "Error = ";
			text += pop.GetStatus();
			break;
		}

		msgs[i-1] = (char*)strdup(pop.GetRaw());

		text += "<A href=\"#msg_";
		text += Number(i);
		text += "\">";
		text += pop.GetSubject();
		text += "</A>";

		//pop.Delete(i);
	}

	wxEndBusyCursor();
	pop.Disconnect();

	text += "</OL><P>";

	for (i = 1; i <= n; i++)
	{
		pop.SetRaw(msgs[i-1]);

		text += "<HR><A name=\"msg_";
		text += Number(i);
		text += "\">";
		text += pop.GetSubject();
		text += "</A>";

		text += " <A href=\"mailto:";
		text += pop.GetReplyTo();
		text += "\">(";
		text += pop.GetReplyTo();
		text += ")</A>";

		text += "<P>";
		text += "<PRE>";
		text += pop.GetBody();
		text += "</PRE>";
		text += "<P>";
	}

	// Show it...

	canvas->SetText(text.data());

#endif
}

