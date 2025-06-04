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

#ifndef WXHTML_H
#define WXHTML_H

#include <stdio.h>
#include <time.h>

#include <wx_timer.h>

#include "htmlconf.h"
#include "listof.h"
#include "astring.h"

#ifdef wx_msw
class wxImage;
#endif

class HtmlObject;
class Table;
class TableRow;
class TableCell;
class HtmlAnchor;
class wxFrame;
class wxHtml;

class wxPeriodicTimer : public wxTimer
{
public:

  wxPeriodicTimer(wxHtml* html_canvas) { canvas = html_canvas; count = 0; };

private:

	void Notify();

  wxHtml* canvas;
  unsigned long count;
};

class wxDeferralTimer : public wxTimer
{
public:

  wxDeferralTimer(wxHtml* html_canvas) { canvas = html_canvas; };

private:

	void Notify();

  wxHtml* canvas;
};

#if USE_HTML_FORMS
class wxHtml : public wxPanel
#else
class wxHtml : public wxCanvas
#endif
{
public:

DECLARE_ABSTRACT_CLASS(wxHtml);

	wxHtml(wxWindow* parent, int x=-1, int y=-1, int w=-1, int h=-1, long style=0);
	~wxHtml();

	virtual void OnSetStatusText(const char*, int /*number*/=0) {};
	int Back(int cnt=1);
	int SaveFile(const string& filename);
	int SaveGeneratedFile(const string& filename);
	virtual PutGenerated();
	void Parse(wxDC* dc);
	int Stop();
	virtual void Reload(Bool /*from_host*/=FALSE);
	void SetEdit(Bool state);
	Bool IsEdit() const { return (edit?TRUE:FALSE); };
	void SetDirty(Bool state = TRUE) { dirty = state; };
	Bool IsDirty() const { return (dirty?TRUE:FALSE); };
	const string& GetCurrentURL() const { return url; };
	const string& GetBaseURL() const { return base; };
	const string& GetBaseTarget() const { return base; };
	const char* GetText() const { return text; };
	string GetCurrentFilename() const;
	void SetText(const char* t, Bool copy_buffer=TRUE);
	virtual void SetCache(Bool /*state*/, Bool /*persistent*/, const string& ="") {};
	virtual void GetCache(Bool& state, Bool& persistent, string& path) { state = persistent = FALSE; path=""; };
	void GetCacheList(ListOf&);
	virtual void ClearCache();
	string AbsoluteURL(const string& url);
	string Generate(Bool base=FALSE);
	virtual int OpenURL(const string& url, Bool post=FALSE, Bool head=FALSE, Bool put=FALSE);
	virtual void MapURL(const string& /*url*/, const string& /*base*/) {};
	void SetTables(Bool state) { tables = state; };
	void SetTables() { tables = !tables; };
	void SetFrames(Bool state) { frames = state; };
	void SetFrames() { frames = !frames; };
	void SetImages(Bool state) { images = state; };
	void SetImages() { images = !images; };
	void SetPlainText(Bool state) { plain_text = (state?1:0); };
	virtual void About();
	void SaveHistory();
	void SetSansSerif(Bool state);
	Bool GetSansSerif() const;
	int DeferredLoads() { return (int)(deferrals_lo.Count() + deferrals_hi.Count()); };
#if ALLOW_SITE_MODE
	Bool CompileSiteAll();
	Bool VerifySiteAll(Bool head);
	Bool IsWSML() const { return is_wsml; };
#endif
	void ConsoleBegin();
	void ConsoleClear();
	void ConsoleWrite(const string&);
	void ConsoleWrite(const char*);
	void ConsoleDone();
	void ClearBody();
	void SetCompiling(Bool state) { compiling = state; };
	void SetVerifying(Bool state) { verifying = compiling = state; };
	static wxColour* HtmlColour(const char*);

protected:

	virtual void OnDownload(const string& /*url*/, const string& /*filename*/) {};
	virtual void OnOpen(const string& /*url*/) {};
	virtual Bool OnUserOpen(const string& /*url*/) { return TRUE; };
	virtual int GetAttributeValuePair(const char*& src, string& p1, string& p2);

	void OnDropFiles(int n, char* files[], int x, int y);
	void OnEvent(wxMouseEvent& event);
	void OnPaint();
	void OnChar(wxKeyEvent& event);
	//void OnSize(int, int);

	virtual int GetHTTP(const string& /*host*/,const string& /*portname*/,const string& /*filename*/,string& /*localname*/,Bool /*inline_object*/=FALSE,Bool /*post*/=FALSE,Bool /*head*/=FALSE,int /*secure*/=0)
					{ return 0;};
	virtual int GetFTP(const string& /*host*/,const string& /*portname*/,const string& /*filename*/,string& /*localname*/,Bool /*inline_object*/=FALSE)
					{ return 0; };
	virtual void SendMail(const string& /*to*/="", const string& /*subject*/="", const string& /*text*/="") {};

	virtual void AddCache(const string& u, const string& localname="", const string& last_modified="", const string& expires="", Bool not_found=FALSE, const string& mime_type="", Bool head=FALSE);
	virtual int SearchCache(const string& u, string& localname, string& last_modified, string& expires, Bool& not_found, string& mime_type, Bool head);
	virtual int RemoveCache(const string& u);
	virtual int ParseUser(wxDC*, char*&, char*, char*) { return 0; };
	int GetURL(const string& url, Bool inline_object, string& localname, Bool post=FALSE, Bool head=FALSE, Bool put=FALSE);
	virtual wxHtml* Clone(int x, int y, int w, int h, long style);
	int Cancel();
	void DeleteSubFrames();
	void DeferDeleteSubFrames();
	int WallpaperImage(wxDC*, const string& filename, float x1, float w, float y1, float h);

	static void FormSubmit(wxButton&, wxCommandEvent&);
	static void FormReset(wxButton&, wxCommandEvent&);
	static void FormButton(wxButton&, wxCommandEvent&);
	static void FormBrowse(wxButton&, wxCommandEvent&);
	void FormShow(Bool state);
	void FormClear();
	Bool IsCompiling() const { return compiling; };
	Bool IsVerifying() const { return verifying; };

	const char* text;
	string url, base, base_target, wallpaper, bgcolor, fgcolor;
	string fglink, fgvlink, fgalink;
	string bgsound, bgimage, bgtitle;
	string marquee_text, marquee_bgcolor, marquee_fgcolor, meta_url;
	string post_data, form_action, form_method;
	wxColour* background;
	wxColour* foreground;
	int in_progress, thru_count, addressing, draw_count, br;
	int starting_x, starting_y, drawing_w, drawing_h;
	int left_floating, right_floating;
	float extent_x, extent_y;
	float marquee_x, marquee_y, marquee_w, marquee_h, marquee_char_width;
	float x, y, max_h, left_margin, right_margin, left_limit_y, right_limit_y;
	float default_left_margin, default_right_margin, addressing_max_h;
	float char_width, char_height;
	int quiet, scrolled, overflow, center, right, finished, bgloop;
	enum { NONE=0, BOLD=1<<0, ITALIC=1<<1, UNDERLINE=1<<2, FIXED=1<<3, STRIKE=1<<4 };
	unsigned mode, marquee_mode;
	int meta_seconds, mapping, tabling, framing, forming, heading, wsmling;
	long meta_last;
	int p_center, p_right, tagged, coded, preformatted, ignore, nobr, nowrap;
	int plain_text, marquee_idx, marquee_fsz, fsz, basefont, comment;
#if USE_HTML_FORMS
	wxHashTable form_objects, form_values;
#endif
	wxPeriodicTimer periodic_timer;
	Bool forward;

	static int outstanding, cancel;
	static unsigned int instance_count;
	int n0, h1, h2, h3, h4, h5, h6;
	int family, style, weight;

	struct History
	{
		int scroll_x, scroll_y;
		wxColour* background;
		string url;
	};
	ListOf history;

	struct Imagemap
	{
		string name;

		struct Area
		{
			string shape, name, coords;
		};
		ListOf areas;
	};
	ListOf imagemaps;

	struct Anchor
	{
		int isname, ismap;
		string name, usemap, target;
		float x, y, w, h;
		HtmlAnchor* object;
	};
	ListOf anchors;

	struct State
	{
		wxColour* color;
		int fsz, family;
		unsigned mode;
		int center, p_center, p_right;
		State* next;
	};
	State* states;

public:

	struct Tag
	{
		int center, p_center, left_floating, right_floating;
		float left_margin, right_margin, y, h;
		ListOf objects;
	};

private:

	ListOf* tags;

public:

	struct Cache
	{
		string url, filename, mime_type;
		string last_modified, expires;
		Bool not_found, head;
	};

	struct Bitmap
	{
		string name;
#if defined( wx_msw )
		wxImage* bitmap;
#else
		wxBitmap* bitmap;
#endif
		int width, height;
	};

private:

	int DisplaySound(const string& filename);
	int DisplayDynamic(Bool quiet, wxDC*, const string& filename, float x, float y, float w, float h);
	int DisplayImage(Bool quiet, wxDC*, const string& filename, float x, float y, float& w, float& h);
	int LoadFile(const string& filename);
	const char* ReadFile(const string& filename);
	void Generate(string& s, Bool do_base=FALSE);
	void SetWordBreak(Bool state) { word_break = state; };
	Bool IsWordBreak() const { return word_break; };

	int ParseSome(wxDC*, char*& dst, char* buf, char* tag);
	int ParseSome1(wxDC*, char*& dst, char* buf, char* tag);
	int ParseSome2(wxDC*, char*& dst, char* buf, char* tag);
	int ParseSome3(wxDC*, char*& dst, char* buf, char* tag);
	int ParseForm(wxDC*, char*& dst, char* buf, char* tag);
	int ParseTable(wxDC*, char*& dst, char* buf, char* tag);
	int ParseFrame(wxDC*, char*& dst, char* buf, char* tag);

#if ALLOW_SITE_MODE
	int ParseSite(wxDC*, char*& dst, char* buf, char* tag);
	void SetParams(ListOf& attributes, ListOf& values);
	void ClearParams();
	Bool CompileSite(Anchor*, wxHtml*);
	Bool VerifySite(Anchor*, wxHtml*, Bool head);
#endif

	void MarqueeNotify();
	void MetaNotify();
	void SaveState();
	void RestoreState();
	void Hold(HtmlObject*);
	void DrawTags(wxDC*, ListOf&);
	void Release(wxDC*);
	Bool LocateHtmlObject(float x, float y);
	Anchor* LocateAnchor(float x, float y);
	Anchor* FindAnchor(const char*);
	wxHtml* FindSubFrame(const string& frame_name);
	wxHtml* FindTopFrame();
	wxHtml* WhichFrame(string& target);
	int AllreadyDeferred(const string& name, float x, float y);
	int JavaScript(const char* cmd);
	void TableDraw(wxDC*, Table*, float x, float y);
	void TableOutput(Table*, string& text);
	void TableDelete(Table*);
	unsigned long FindMatchingColour(unsigned long rgb);
	static void DropLine(ListOf& list);
	static Bool Area(Imagemap::Area* area, float pos_x, float pos_y);
	void SetLoad(const string& u) { setload_url = u; };
	wxHtml* GetRealParent() const { return real_parent; };

	int ABSOLUTE_X(float X) const { return (int)(X - (starting_x * VSCROLL_INCREMENT)); };
	int ABSOLUTE_Y(float Y) const { return (int)(Y - (starting_y * VSCROLL_INCREMENT)); };

#if ALLOW_EDIT_MODE
	static void PopUpFunction2(wxMenu& menu, wxCommandEvent& event);
	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();
	void MovePageUp();
	void MovePageDown();
	void MoveHome();
	void MoveEnd();
	void MergeLeft();
	void MergeRight();
	void Delete();
	void Backspace();
	void Adjust(float delta, ListOfIterator tags_ilist);
	void Select(float pos_x, float pos_y);
	void Deselect();
	Bool DeleteHtmlObject(HtmlObject*);
	void ProcessEvent(wxMouseEvent& event);
	void ProcessChar(wxKeyEvent& event);
	void Copy(Bool cut = FALSE, Bool to_clipboard = TRUE);
	void Cut() { Copy(TRUE, TRUE); };
	void Drop() { Copy(TRUE, FALSE); };
	void Paste();
	void Paste(ListOfIterator);
	void DrawCaret(Bool on);
	void MoveCaret(float pos_x, float pos_y);
	void SetInsertMode(Bool state) { insert_mode = state; };
	Bool IsInsertMode() const { return insert_mode; };
	void SetMode(unsigned mode, int size_delta);
	void Mode(unsigned mode) { SetMode(mode, 0); };
	void Size(int size) { SetMode(0, size); };
#endif

	static void PopUpFunction1(wxMenu& menu, wxCommandEvent& event);
	wxMenu* menu1;

#if ALLOW_EDIT_MODE
	wxMenu* menu2;
	ListOfIterator edit_objects, edit_tags;
	HtmlObject* edit_object;
	Tag* edit_tag;
	float selected_x, selected_y, current_x, current_y, last_x, last_y;
	float old_y1, old_y2;
	long selected_objects, caret_count;
	Bool selected, skipped, insert_mode;
#endif

	static wxHtml* last_canvas;
	static Anchor* last_anchor_item;

	unsigned count[MAX_TABS];
	char list_type[MAX_TABS], order_type;
	char tabs;
	int quote_level, edit, dirty;
	Bool have_body, is_wsml, have_frames, deferred_images, word_break;
	ListOf holding_line;
	wxDeferralTimer deferral_timer;
	string frame_name, setload_url;
	Bool images, frames, tables, compiling, verifying, bad_include;
	HtmlObject* previous_object;
	ListOf sub_frames, delete_list;
	string default_title, default_fgcolor, default_bgcolor;
	string default_bgimage, bgcomment, default_link;
	string default_vlink, default_alink;
	unsigned long line_nbr, tick, last_tick;
	int sbx, sby;
	wxHtml* real_parent;

#ifdef wx_msw
	wxColourMap cm;
#endif

	struct Deferral
	{
		string srcname, dynsrc, soundname;
		wxHtml* wxHtml;
		float x, y, w, h;
	};
	ListOf deferrals_lo;
	ListOf deferrals_hi;

	friend class wxPeriodicTimer;
	friend class wxDeferralTimer;

	friend class HtmlObject;
	friend class HtmlMarkup;
	friend class HtmlText;
	friend class HtmlAnchor;
	friend class HtmlDeanchor;
	friend class HtmlImage;
	friend class HtmlList;
	friend class HtmlEndList;
	friend class HtmlOrderedListIndex;
	friend class HtmlUnorderedListIndex;
	friend class HtmlHorizontalRule;
	friend class HtmlMarquee;
	friend class HtmlBreak;
	friend class HtmlParagraph;
	friend class HtmlEndParagraph;
	friend class HtmlFormItem;
	friend class HtmlTable;
	friend class HtmlTableRow;
	friend class HtmlTableCell;
	friend class HtmlSiteAnchor;

	friend class HtmlAttributes;
	friend class TextAttributes;
	friend class ImageAttributes;
	friend class BodyAttributes;
	friend class MarqueeAttributes;
	friend class ParagraphAttributes;

	friend class GridFrame;
	friend class Table;
	friend class wxWWW;
};

// Convert numbers to strings...

inline string Number(double n)
{
	char s[20];
	sprintf(s, "%g", n);
	return string(s);
}

#endif


