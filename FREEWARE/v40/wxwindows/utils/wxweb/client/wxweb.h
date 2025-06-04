///////////////////////////////////////////////////////////////////////////////
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

#include <wx_timer.h>

class WebFrame;

class BusyTimer : public wxTimer
{
public:

  BusyTimer(WebFrame* _frame) { frame = _frame; };

private:

	void Notify();

	WebFrame* frame;
};


#if USE_WXWWW && USE_MAP

class MapFrame;

class MapCanvas: public wxCanvas
{
public:

	 MapCanvas(MapFrame *frame, int x = 0, int y = 0, int w = -1, int h = -1, long style = wxRETAINED);
	 void OnPaint();

private:

	void OnEvent(wxMouseEvent& event);
	MapFrame* frame;
};

class MapFrame: public wxFrame
{
public:

	MapFrame(wxWWW* web, char* title, int x=-1, int y=-1, int w=-1, int h=-1);
	~MapFrame();

	void Draw(wxDC& dc, Bool draw_bitmaps = TRUE);
	Bool OnClose() { return TRUE; };
	void OnMenuCommand(int);

private:

	wxCanvas* canvas;
	wxStoredTree* tree;
	float userscale;

	void OnPaint() { if (tree) tree->Draw(); };

	friend class WebFrame;
	friend class MapCanvas;
};

#endif

class SourceFrame: public wxFrame
{
public:

	SourceFrame(char* title, int x=-1, int y=-1, int w=-1, int h=-1);
	~SourceFrame();

	Bool OnClose() { return TRUE; };
	void OnSize(int, int);
	void OnMenuCommand(int);

private:

	wxHtml* canvas;

	friend class WebFrame;
};

class WebConfigDialog : public wxDialogBox
{
public:

#if USE_WXWWW
	WebConfigDialog(wxWindow* parent, wxWWW* canvas);
#else
	WebConfigDialog(wxWindow* parent, wxHtml* canvas);
#endif

protected:

	static void AttrOK(wxButton& button, wxCommandEvent&);
	static void AttrCancel(wxButton& button, wxCommandEvent&);

	wxButton* ok;
	wxButton* cancel;
	wxText* host;
	wxText* port;
	wxText* exclude;
	wxCheckBox* proxies;
	wxCheckBox* caching;
	wxCheckBox* session;
	wxCheckBox* sansserif;
	wxCheckBox* cookies;
	wxCheckBox* log;

#if USE_WXWWW
	wxWWW* canvas;
#else
	wxHtml* canvas;
#endif
};

class WebCanvas;

class WebFrame: public wxFrame
{
public:

	WebFrame(char* title, int x=-1, int y=-1, int w=-1, int h=-1);
	~WebFrame();

	Bool OnClose();
	void OnSize(int, int);
	void OnMenuCommand(int);
	void ShowMail();
	void SaveState(Bool full = FALSE);

private:

	static WebFrame* CreateBrowser(int x, int y, int w, int h);
	static void WebFirst(wxButton& but, wxCommandEvent& ev);
	static void WebBack(wxButton& but, wxCommandEvent& ev);
	static void WebStop(wxButton& but, wxCommandEvent& ev);
	static void WebExit(wxButton& but, wxCommandEvent& ev);
	static void WebHome(wxButton& but, wxCommandEvent& ev);
	static void WebEdit(wxButton& but, wxCommandEvent& ev);
	static void WebPrint(wxButton& but, wxCommandEvent& ev);
	static void WebMail(wxButton& but, wxCommandEvent& ev);
	static void WebReload(wxButton& but, wxCommandEvent& ev);
	static void WebBookit(wxButton& but, wxCommandEvent& ev);
	static void WebBookmarks(wxButton& but, wxCommandEvent& ev);
	static void WebOpen(wxText& but, wxCommandEvent& ev);
#if ALLOW_SITE_MODE
	static void WebCompile(wxButton& but, wxCommandEvent& ev);
	wxButton* b_compile;
#endif

	static void WebVerify(wxButton& but, wxCommandEvent& ev);

	WebCanvas* canvas;

	wxPanel* toolbar;
	wxPanel* throbber;
	wxText* url;
	wxButton* b_first;
	wxButton* b_back;
	wxButton* b_edit;
	wxButton* b_stop;
	wxButton* b_home;
	wxButton* b_reload;
	wxButton* b_print;
	wxButton* b_mail;
	wxButton* b_exit;
	wxButton* b_verify;
	wxButton* b_bookit;
	wxButton* b_bookmarks;
	Bool allow_exit;
	string home;
	BusyTimer busy_timer;

	friend class WebApp;
	friend class MapCanvas;
	friend class WebCanvas;
	friend class BusyTimer;
};

#if USE_WXWWW
class WebCanvas : public wxWWW
#else
class WebCanvas : public wxHtml
#endif
{
public:

	WebCanvas(WebFrame* _frame, int x, int y, int w, int h, long style=wxBORDER) :
#if USE_WXWWW
		wxWWW(_frame, x, y, w, h, style)
#else
		wxHtml(_frame, x, y, w, h, style)
#endif
	{
		frame = _frame;
		prev_secure = FALSE;
		SetFrames(TRUE);
	}

	int OpenURL(const string& url, Bool post=FALSE, Bool head=FALSE, Bool put=FALSE);

	const string& GetCurrentTitle() const { return wxHtml::bgtitle; };

protected:

	void OnDownload(const string& url, const string& filename);
	void OnOpen(const string& url);

private:

	WebFrame* frame;
	Bool prev_secure;

   friend class WebFrame;
};

class WebApp: public wxApp
{
public:

	WebApp() {};

	wxFrame* OnInit();
};

