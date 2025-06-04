///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  An HTML canvas for wxWindows with hypertext.
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

#ifdef wx_x
#undef USE_CLIPBOARD
#define USE_CLIPBOARD 0
#else
#include <wx_clipb.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <limits.h>

#include "wxhtml.h"
#include "htmlpars.h"
#include "hashof.h"
#include "url.h"

#if ALLOW_EDIT_MODE
#include "htmledit.h"
#endif

#ifdef wx_msw
#include "wxima.h"
#endif

#ifdef wx_msw
#include <mmsystem.h>
#endif

char* RESOURCE_FILE = ".WXWEB";

static HashOf cache(97);
static ListOf bitmaps;
int wxHtml::outstanding = 0, wxHtml::cancel = 0;
unsigned int wxHtml::instance_count = 0;
wxHtml* wxHtml::last_canvas = 0;
wxHtml::Anchor* wxHtml::last_anchor_item = 0;

static int default_n0, default_h1, default_h2, default_h3, default_h4, default_h5, default_h6;
static int default_family, default_style, default_weight;
static wxTextWindow* console = 0;
extern const char* version;

const MAX_REQUESTS = 4;
static int max_reqs = MAX_REQUESTS;

///////////////////////////////////////////////////////////////////////////////

#if 0
Bool RunAssociatedProgram(const char* filename)
{
#ifdef wx_msw
	HINSTANCE inst = ShellExecute(GetFocus(), 0, filename, 0, 0, SW_SHOW);
	return (unsigned(inst) <= 32 ? FALSE : TRUE);
#else
	string cmd = string("netscape ") + filename + "&";
	return wxExecute((char*)cmd.data());
#endif
}
#endif

///////////////////////////////////////////////////////////////////////////////

#if USE_HTML_FORMS
IMPLEMENT_ABSTRACT_CLASS(wxHtml, wxPanel);
#else
IMPLEMENT_ABSTRACT_CLASS(wxHtml, wxCanvas);
#endif

wxHtml::wxHtml(wxWindow* parent, int x, int y, int w, int h, long style) :
#if USE_HTML_FORMS
	wxPanel(parent, x, y, w, h, style|wxVSCROLL|wxUSER_COLOURS),
		form_objects(wxKEY_STRING, 13), 
			form_values(wxKEY_STRING, 13),
#else
	wxCanvas(parent, x, y, w, h, style),
#endif
		periodic_timer(this), deferral_timer(this)
{
	plain_text = 0;
	text = 0;
	in_progress = 0;
	dirty = edit = 0;
	draw_count = 0;
	sbx = sby = 0;
	line_nbr = 0;
	tick = 0;
	real_parent = 0;

	bgimage = bgcolor = fgcolor = frame_name = base =
		base_target = url = bgcomment = 
		fglink = fgvlink = fgalink =
		default_bgcolor = default_fgcolor = 
		default_link = default_vlink = default_alink =
		default_title = default_bgimage = "";
	
	forward = TRUE;
	images = TRUE;
	frames = FALSE;
	tables = TRUE;
	word_break = FALSE;

	tags = new ListOf;
	have_frames = is_wsml = compiling = verifying = FALSE;

	GetClientSize(&drawing_w, &drawing_h);
	SetCursor(new wxCursor(wxCURSOR_ARROW));

#ifdef wx_msw
	DragAcceptFiles(TRUE);
#endif

	menu1 = new wxMenu(0, (wxFunction)PopUpFunction1);
	menu1->Append(HTML_BACK, "Back in frame");
	menu1->Append(HTML_COPY, "Copy link to clipboard");
	menu1->Append(HTML_EDIT, "Edit...");

#if ALLOW_EDIT_MODE
	menu2 = new wxMenu(0, (wxFunction)PopUpFunction2);
	menu2->Append(HTML_CUT, "Cut\tCtrl+X");
	menu2->Append(HTML_COPY, "Copy\tCtrl+C");
	menu2->Append(HTML_PASTE, "Paste\tCrl+V");
	menu2->AppendSeparator();
	menu2->Append(HTML_BOLD, "Bold\tCtrl+B");
	menu2->Append(HTML_ITALIC, "Italic\tCtrl+I");
	menu2->Append(HTML_UNDERLINE, "Underline\tCtrl+U");
	menu2->Append(HTML_STRIKE, "Strike\tCtrl+S");
	menu2->Append(HTML_FIXED, "Fixed\tCtrl+F");
	menu2->Append(HTML_PLUS, "Bigger");
	menu2->Append(HTML_MINUS, "Smaller");
	menu2->AppendSeparator();
	menu2->Append(HTML_PROPERTIES, "Properties...");
#endif

	extent_x = (float)drawing_w;
	extent_y = (float)drawing_h;

	SetScrollbars
		(
			VSCROLL_INCREMENT, VSCROLL_INCREMENT,
			0, 0,
			(int)(drawing_w/VSCROLL_INCREMENT), (int)(drawing_h/VSCROLL_INCREMENT)
		);

#ifdef wx_msw
	// Check for 256 colour display...

	if (wxDisplayDepth() == 8)
	{
		unsigned char red[256], green[256], blue[256];
		unsigned i = 0;

		for (int j = 0; j < 256; j++)
		{
			red[j] = green[j] = blue[j] = (unsigned char)j;
		}

		// The Netscape colour cube...

		for (int r = 0x00; r <= 0xFF; r += 0x33)
			for (int g = 0x00; g <= 0xFF; g += 0x33)
				for (int b = 0x00; b <= 0xFF; b += 0x33)
				{
					red[i] = (char)r;
					green[i] = (char)g;
					blue[i] = (char)b;
					i++;
				}

		// These are the Windows VGA colours...

		red[i] = 0x00; green[i] = 0x00; blue[i] = 0x00; i++;
		red[i] = 0xC0; green[i] = 0xC0; blue[i] = 0xC0; i++;
		red[i] = 0x80; green[i] = 0x80; blue[i] = 0x80; i++;
		red[i] = 0xFF; green[i] = 0xFF; blue[i] = 0xFF; i++;
		red[i] = 0x80; green[i] = 0x00; blue[i] = 0x00; i++;
		red[i] = 0xFF; green[i] = 0x00; blue[i] = 0x00; i++;
		red[i] = 0x00; green[i] = 0x80; blue[i] = 0x00; i++;
		red[i] = 0x00; green[i] = 0xFF; blue[i] = 0x00; i++;
		red[i] = 0x80; green[i] = 0x80; blue[i] = 0x00; i++;
		red[i] = 0xFF; green[i] = 0xFF; blue[i] = 0x00; i++;
		red[i] = 0x00; green[i] = 0x00; blue[i] = 0x80; i++;
		red[i] = 0x00; green[i] = 0x00; blue[i] = 0xFF; i++;
		red[i] = 0x80; green[i] = 0x00; blue[i] = 0x80; i++;
		red[i] = 0xFF; green[i] = 0x00; blue[i] = 0xFF; i++;
		red[i] = 0x00; green[i] = 0x80; blue[i] = 0x80; i++;
		red[i] = 0x00; green[i] = 0xFF; blue[i] = 0xFF; i++;
		
		cm.Create(256, red, green, blue);
		//SetColourMap(&cm);
	}
#endif

	if (!instance_count++)
	{
		// Note: don't use >19 under Motif

#ifndef wx_motif
		default_h1 = 20; default_h2 = 19;
#else
		default_h1 = 19; default_h2 = 18;
#endif

		default_h3 = 17; default_h4 = 15; default_h5 = 13; default_h6 = 12; default_n0 = 12;

		if (!wxGetResource("WXWEB", "N0", &default_n0, RESOURCE_FILE))
			wxWriteResource("WXWEB", "N0", default_n0, RESOURCE_FILE);

		if (!wxGetResource("WXWEB", "H1", &default_h1, RESOURCE_FILE))
			wxWriteResource("WXWEB", "H1", default_h1, RESOURCE_FILE);

		if (!wxGetResource("WXWEB", "H2", &default_h2, RESOURCE_FILE))
			wxWriteResource("WXWEB", "H2", default_h2, RESOURCE_FILE);

		if (!wxGetResource("WXWEB", "H3", &default_h3, RESOURCE_FILE))
			wxWriteResource("WXWEB", "H3", default_h3, RESOURCE_FILE);

		if (!wxGetResource("WXWEB", "H4", &default_h4, RESOURCE_FILE))
			wxWriteResource("WXWEB", "H4", default_h4, RESOURCE_FILE);

		if (!wxGetResource("WXWEB", "H5", &default_h5, RESOURCE_FILE))
			wxWriteResource("WXWEB", "H5", default_h5, RESOURCE_FILE);

		if (!wxGetResource("WXWEB", "H6", &default_h6, RESOURCE_FILE))
			wxWriteResource("WXWEB", "H6", default_h6, RESOURCE_FILE);

		if (!wxGetResource("WXWEB", "MaxRequests", &max_reqs, RESOURCE_FILE))
			wxWriteResource("WXWEB", "MaxRequests", MAX_REQUESTS, RESOURCE_FILE);

		char* tmp = 0;

		if (wxGetResource("WXWEB", "Family", &tmp, RESOURCE_FILE))
		{
			if (!strcmpi(tmp, "wxDEFAULT"))
				default_family = wxDEFAULT;
			else if (!strcmpi(tmp, "wxSWISS"))
				default_family = wxSWISS;
			else if (!strcmpi(tmp, "wxMODERN"))
				default_family = wxMODERN;
			else if (!strcmpi(tmp, "wxROMAN"))
				default_family = wxROMAN;
			else if (!strcmpi(tmp, "wxSCRIPT"))
				default_family = wxSCRIPT;
			else if (!strcmpi(tmp, "wxDECORATIVE"))
				default_family = wxDECORATIVE;
			else
				default_family = wxDEFAULT;
		}
		else
		{
			default_family = wxROMAN;
			wxWriteResource("WXWEB", "Family", "wxDEFAULT", RESOURCE_FILE);
		}

		tmp = 0;

		if (wxGetResource("WXWEB", "Style", &tmp, RESOURCE_FILE))
		{
			if (!strcmpi(tmp, "wxNORMAL"))
				default_style = wxNORMAL;
			else if (!strcmpi(tmp, "wxSLANT"))
				default_style = wxSLANT;
			else if (!strcmpi(tmp, "wxITALIC"))
				default_style = wxITALIC;
			else
				default_style = wxNORMAL;
		}
		else
		{
			default_style = wxNORMAL;
			wxWriteResource("WXWEB", "Style", "wxNORMAL", RESOURCE_FILE);
		}

		tmp = 0;

		if (wxGetResource("WXWEB", "Weight", &tmp, RESOURCE_FILE))
		{
			if (!strcmpi(tmp, "wxNORMAL"))
				default_weight = wxNORMAL;
			else if (!strcmpi(tmp, "wxLIGHT"))
				default_weight = wxLIGHT;
			else if (!strcmpi(tmp, "wxBOLD"))
				default_weight = wxBOLD;
			else
				default_weight = wxNORMAL;
		}
		else
		{
			default_weight = wxNORMAL;
			wxWriteResource("WXWEB", "Weight", "wxNORMAL", RESOURCE_FILE);
		}
	}

	family = default_family;
	style = default_style;
	weight = default_weight;
	n0 = default_n0;
	h1 = default_h1;
	h2 = default_h2;
	h3 = default_h3;
	h4 = default_h4;
	h5 = default_h5;
	h6 = default_h6;
	fsz = n0;

#if USE_HTML_FORMS
	SetLabelFont(wxTheFontList->FindOrCreateFont(fsz, family, style, weight));
	SetButtonFont(wxTheFontList->FindOrCreateFont(fsz, family, style, weight));
#endif

	// Default font setting...

	GetDC()->SetFont(wxTheFontList->FindOrCreateFont(fsz, family, style, weight));
}

///////////////////////////////////////////////////////////////////////////////

wxHtml::~wxHtml()
{
	Stop();
	DeleteSubFrames();

	wxBeginBusyCursor();

	ListOfIterator ilist = anchors;
	void* a;
	while (ilist.First(a))
	{
		delete (Anchor*)a;
		ilist.Delete();
	}

	ilist = history;
	while (ilist.First(a))
	{
		delete (History*)a;
		ilist.Delete();
	}

	ilist = imagemaps;
	while (ilist.First(a))
	{
		Imagemap* imagemap = (Imagemap*)a;

		ListOfIterator ilist2 = imagemap->areas;

		void* aa;
		while (ilist2.First(aa))
		{
			delete (Imagemap::Area*)aa;
			ilist2.Delete();
		}

		delete imagemap;
		ilist.Delete();
	}

	ilist = *tags;
	while (ilist.First(a))
	{
		Tag* tag = (Tag *)a;

		DropLine(tag->objects);

		delete tag;
		ilist.Delete();
	}
	delete tags;

	wxEndBusyCursor();

	if (text) free((char*)text);

	delete menu1;

#if ALLOW_EDIT_MODE
	if (menu2) delete menu2;
#endif

	if (!--instance_count)
	{
		ClearCache();
		if (console) delete console;
	}
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::SetSansSerif(Bool state) { family = default_family = (state ? wxDEFAULT : wxROMAN); };
Bool wxHtml::GetSansSerif() const { return default_family == wxDEFAULT; };

///////////////////////////////////////////////////////////////////////////////

wxHtml* wxHtml::Clone(int x, int y, int w, int h, long style)
{
	wxFrame* frame = (wxFrame*)GetParent();
	int tmp_x, tmp_y;

	GetPosition(&tmp_x, &tmp_y);
	x += tmp_x;
	y += tmp_y;

	wxHtml* canvas = new wxHtml(frame, x, y, w, h, style);
	canvas->real_parent = this;
	return canvas;
}

///////////////////////////////////////////////////////////////////////////////

unsigned long wxHtml::FindMatchingColour(unsigned long rgb)
{
#ifdef wx_msw
	if (wxDisplayDepth() == 8)
	{
		unsigned char r, g, b;
		r = char((rgb>>16)&0xFF);
		g = char((rgb>>8)&0xFF);
		b = char(rgb&0xFF);

		cm.GetRGB(cm.GetPixel(r, g, b), &r, &g, &b);
		return RGB(r, g, b);
	}
	else
		return rgb;
#else
	return rgb;
#endif
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::About()
{
	SaveHistory();

	SetText((
			string(
			"<HTML><HEAD><TITLE>About WXWEB</TITLE></HEAD>"
			"<BODY><H3>WXWEB version ") +
			version +
			" is a portable WWW browser using wxWindows.</H3><P>"
			"<FONT size=+2><B>Copyright (c) 1995-6 by Andrew Davison.</B></FONT><BR>"
			"Permission is granted to use this program quite freely."
#if USE_SSL
			"<P>This version supports unrestricted security with "
			"the SSL (Secure Socket Layer) protocol versions 2 and 3. "
			"The following protocols "
			"are negotiated:- CBC-DES-MD5:RC4-MD5:CBC3-DES-MD5:EXP-RC4-MD5:CFB-DES-M1:CFB-DES-NULL "
			"<P>Restrictions apply on the use of RSA patents in the USA, "
			"and this version must not be re-exported <I>from</I> the USA."
			"SSL software provided by version 0.65 of SSLeay (see "
			"copyright notice)."
#endif
			"<P>For further information see... "
			"<a href=\"http://www.ozemail.com.au/~adavison/wxweb.html\">"
			"the WXWEB page </a>"
			"</BODY></HTML>"
			).data()
			);
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::ClearCache()
{
	void* a;

	while (bitmaps.Pop(a))
	{
		Bitmap* item = (Bitmap*)a;
		delete item->bitmap;
		delete item;
	}

#ifdef wx_msw
	// Must close before deleting cached file...
	mciSendString("close Animation", 0, 0, 0);
#endif

	ListOf list;
	cache.Copy(list);
	cache.Clear();

	while (list.Pop(a))
	{
		Cache* item = (Cache*)a;

		if (item->filename.length())
			remove(item->filename.data());

		delete (Cache*)a;
	}
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::GetCacheList(ListOf& list)
{
	cache.Copy(list);
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::AddCache(const string& u, const string& localname, const string& last_modified, const string& expires, Bool not_found, const string& mime_type, Bool head)
{
	Cache* item = new Cache;
	item->url = u;
	item->filename = localname;
	item->last_modified = last_modified;
	item->expires = expires;
	item->not_found = not_found;
	item->mime_type = mime_type;
	item->head = head;
	cache.Set(u, item);
}
		
///////////////////////////////////////////////////////////////////////////////

int wxHtml::RemoveCache(const string& u)
{
	void* a;

	if (!cache.Drop(u, a))
		return 0;

	Cache* item = (Cache*)a;

	if (item->filename.length())
		remove(item->filename.data());

	delete item;

	return 1;
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::SearchCache(const string& u, string& localname, string& last_modified, string& expires, Bool& not_found, string& mime_type, Bool /*head*/)
{
	void* a;

	if (!cache.Get(u, a))
		return 0;

	Cache* item = (Cache*)a;

	localname = item->filename;
	not_found = item->not_found;
	mime_type = item->mime_type;
	last_modified = item->last_modified;
	expires = item->expires;

	return 1;
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::OnDropFiles(int n, char* file[], int, int)
{
	if (n == 1)
	{
		OpenURL(string("file:///")+file[0]);
		return;
	}

	string src = "";
	int i = 0;

	while (i < n)
	{
		src += string("<IMG src=\"file:///") + file[i] + "\"> ";
		src += file[i];
		src += "<P>";
		i++;
	}

	SaveHistory();
	SetText(src.data());
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::Reload(Bool /*from_host*/)
{
	GetClientSize(&drawing_w, &drawing_h);
	if (!GetCurrentURL().length()) return;
	if (Stop()) return;
	forward = FALSE;
	OpenURL(GetCurrentURL());
	forward = TRUE;
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::Back(int _cnt)
{
	unsigned cnt = (unsigned)_cnt;

	if (Stop()) return 0;

	if (!history.Count() || !cnt || edit)
		return 0;

	if (_cnt == -1) cnt = history.Count();

	if (cnt > history.Count())
		return 0;

	History* last = 0;

	while (cnt--)
	{
		void* a;
		history.Remove(a);
		last = (History*)a;
		if (cnt) delete last;
	}

	// If a different document then re-open the URL, else
	// just scroll to the saved point...

	if (last->url != AbsoluteURL(url))
	{
		background = last->background;
		forward = FALSE;
		OpenURL(last->url);
		forward = TRUE;
	}
	else
		Clear();

	Scroll(last->scroll_x, last->scroll_y);
	delete last;

	return 1;
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::Stop()
{
	int ret = Cancel();

#ifdef wx_msw
	// Must close before deleting cached file...
	mciSendString("close Animation", 0, 0, 0);
	Refresh();
#endif

	return ret;
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::Cancel()
{
	cancel = 1;

	periodic_timer.Stop();
	deferral_timer.Stop();

	ListOfIterator ilist = sub_frames;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxHtml* canvas = (wxHtml*)a;
		canvas->Cancel();
	}

	ilist = deferrals_lo;
	while (ilist.First(a))
	{
		delete (Deferral*)a;
		ilist.Delete();
	}

	ilist = deferrals_hi;
	while (ilist.First(a))
	{
		delete (Deferral*)a;
		ilist.Delete();
	}

	return outstanding;
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::SaveFile(const string& filename)
{
	ofstream ostr(filename.data());

	if (!ostr)
	{
		string msg = "Could not save: " + filename;
		wxMessageBox((char*)msg.data());
		return 0;
	}

	ostr << text;

	return 1;
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::SaveGeneratedFile(const string& filename)
{
	ofstream ostr(filename.data());

	if (!ostr)
	{
		string msg = "Could not save: " + filename;
		wxMessageBox((char*)msg.data());
		return 0;
	}

	ostr << Generate();

	return 1;
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::PutGenerated()
{
	string default_filename = GetCurrentFilename();

	char* filename = wxFileSelector("Save file", 0, (char*)default_filename.data(), "html", "*.*");

	if (filename)
	{
		if (SaveGeneratedFile(filename))
		{
			SetDirty(FALSE);
			return 1;
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::SetText(const char* t, Bool copy_buffer)
{
	if (!t) return;

	// Out with the old...

	wxBeginBusyCursor();

	ListOfIterator ilist = anchors;
	void* a;

	while (ilist.First(a))
	{
		delete (Anchor*)a;
		ilist.Delete();
	}

	ilist = imagemaps;
	while (ilist.First(a))
	{
		Imagemap* imagemap = (Imagemap*)a;

		ListOfIterator ilist2 = imagemap->areas;
		void* aa;
		while (ilist2.First(aa))
		{
			delete (Imagemap::Area*)aa;
			ilist2.Delete();
		}

		delete imagemap;
		ilist.Delete();
	}

	ilist = *tags;
	while (ilist.First(a))
	{
		Tag* tag = (Tag*)a;

		DropLine(tag->objects);

		delete tag;
		ilist.Delete();
	}

	wxEndBusyCursor();

	// Stop any activity...

	periodic_timer.Stop();
	deferral_timer.Stop();

	DeferDeleteSubFrames();

	FormClear();

	if (text) free((char*)text);

	thru_count = 0;
	meta_last = time(0);
	meta_seconds = -1;
	meta_url = marquee_text = "";

#ifdef wx_msw
	mciSendString("close Animation", 0, 0, 0);
#endif

	bgcolor = default_bgcolor;
	fgcolor = default_fgcolor;
	bgimage = default_bgimage;
	bgtitle = default_title;
	fglink = default_link;
	fgvlink = default_vlink;
	fgalink = default_alink;

	default_bgcolor = default_fgcolor =
			default_link = default_vlink = default_alink = 
			default_title = default_bgimage = "";

	// In with the new...

	OnSetStatusText("Rendering...");

	if (copy_buffer)
		text = strdup(t);
	else
		text = t;

	// Set the background colour...

	if (!bgcolor.length())
		bgcolor = "SILVER";

	// Set the foreground colour...

	if (!fgcolor.length())
		fgcolor = "BLACK";

	if (fgcolor[0] == '#')
	{
		unsigned long rgb = strtoul((fgcolor.data())+1, 0, 16);
		rgb = FindMatchingColour(rgb);
		foreground = new wxColour(char((rgb>>16)&0xFF),char((rgb>>8)&0xFF),char((rgb)&0xFF));
	}
	else
	{
		foreground = HtmlColour(fgcolor.data());
	}

	// Leave it transparent for background images...

	GetDC()->SetBackgroundMode(wxTRANSPARENT);

	//int prev_x = starting_x, prev_y = starting_y;

	if (forward)
	{
		int now_x, now_y;
		ViewStart(&now_x, &now_y);
		if ((now_x != 0) || (now_y != 0))
			Scroll(0, 0);
	}

	// Where are we?

	ViewStart(&starting_x, &starting_y);
	//scrolled = (prev_x != starting_x) || (prev_y != starting_y);
	scrolled = 1;

	Parse(GetDC());

	int was_sbx = sbx;
	int was_sby = sby;

	sbx = extent_x > drawing_w;
	sby = extent_y > drawing_h;

#if 0
	if (sbx || was_sbx)
		SetScrollRange(wxHORIZONTAL, (sbx?(int)(extent_x/VSCROLL_INCREMENT)+1:0));

	if (sby || was_sby)
		SetScrollRange(wxVERTICAL, (sby?(int)(extent_y/VSCROLL_INCREMENT)+1:0));
#else

	// SetScrollBar() causes a panel refresh under Windows so is to be avoided
	// when at all possible...

	if ((sbx != was_sbx) || (sby != was_sby))
	{
		SetScrollbars
		(
			VSCROLL_INCREMENT, VSCROLL_INCREMENT,
			(sbx?(int)(extent_x/VSCROLL_INCREMENT+8):0),
			(sby?(int)(extent_y/VSCROLL_INCREMENT+8):0),
			(int)(drawing_h/VSCROLL_INCREMENT),
			(int)(drawing_w/VSCROLL_INCREMENT)
		);
	}
	else
	{
		//if (sbx)
		SetScrollRange(wxHORIZONTAL, sbx?(int)(extent_x/VSCROLL_INCREMENT+8):0);

		//if (sby)
		SetScrollRange(wxVERTICAL, sby?(int)(extent_y/VSCROLL_INCREMENT+8):0);
	}
#endif

	if (deferred_images)
		Reload();

	if (deferrals_lo.Count() ||
			deferrals_hi.Count() ||
				delete_list.Count())
	{
		deferral_timer.Start(10);
	}

#ifdef wx_x
	OnPaint();
#endif

	// Every tenth of a second...
	periodic_timer.Start(100);

	OnSetStatusText("Done.");
}

///////////////////////////////////////////////////////////////////////////////

const char* wxHtml::ReadFile(const string& _filename)
{
	string filename = _filename;

#ifdef wx_msw
	filename = filename.change('/', '\\');
	filename = filename.replace("|", "");
#endif

	ifstream istr(filename.data(), ios::in|ios::nocreate);

	if (!istr)
	{
		wxBell();
		//char tmp[256];
		//sprintf(tmp, "Could not open: %s", filename.data());
		//wxMessageBox(tmp);
		return 0;
	}

	struct stat st;
	stat(filename.data(), &st);

	// If building on 16-bit systems beware...

	char* buf = new char[(size_t)(st.st_size+1)];

	const char* ext_ptr;
	plain_text = 1;

	if ((ext_ptr = strrchr(filename.data(), '.')) != 0)
	{
		if (!strcmpi(ext_ptr, ".htm") ||
			!strcmpi(ext_ptr, ".html") ||
			!strcmpi(ext_ptr, ".sht") ||
			!strcmpi(ext_ptr, ".shtml") ||
			!strcmpi(ext_ptr, ".wsml"))
		{
			plain_text = 0;
		}
	}

#ifdef __WATCOM__
	long n = 0;
	char ch;

	// This is not going to be very efficient...

	while (!cancel && !istr.eof())
	{
		istr.get(ch);
		buf[n++] = ch;
	}

	buf[n] = 0;
#else
	char* dst = buf;

	while(!cancel && !istr.read(dst, 1024).eof())
		dst += 1024;

	dst += istr.gcount();
	*dst = 0;
#endif

	istr.close();

	if (cancel)
	{
		delete [] buf;
		buf = 0;
	}

	return buf;
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::LoadFile(const string& _filename)
{
	const char* buf = ReadFile(_filename);

	if (!buf) return 0;

	SetText(buf, FALSE);

	return 1;
}

///////////////////////////////////////////////////////////////////////////////

string wxHtml::AbsoluteURL(const string& u)
{
	return ::AbsoluteURL(base, u);
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::GetURL(const string& uu, Bool inline_object, string& localname, Bool post, Bool head, Bool /*put*/)
{
	string scheme, hostname, portname, filename, section;
	Bool is_image = FALSE;
#ifdef wx_msw
	Bool is_wave = FALSE, is_avi = FALSE;
#endif
	string save_url = url;
	string tmpname;

	string new_url = AbsoluteURL(uu);

	if (!OnUserOpen(new_url))
		return 0;

	localname = "";

	if (!ParseURL(new_url.data(), scheme, hostname, portname, filename, section))
		return 0;

	if (!scheme.length())
	{
		scheme = "http";
	}

	Bool found = TRUE;

	if (!strcmpi(scheme.data(), "file"))	// "file:///"
	{
		const char* ptr = new_url.data()+8;

#ifdef wx_msw
		// This is to get around a bug the ParseURL() routine
		// that strips out the drive-separator...

		tmpname = *ptr++;	// drive-letter
		if (*ptr != ':')
			tmpname += ":";
#endif

		const char* end;
		if ((end = strchr(ptr, '#')) != 0)
			tmpname += string(ptr, end-ptr);
		else
			tmpname += ptr;

		// Cache the link...

		if (!IsCompiling())
			AddCache(new_url);
	}
	else if (!strcmpi(scheme.data(), "https"))
	{
		if (!(found = GetHTTP(hostname, portname, filename, tmpname, inline_object, post, head, TRUE)))
			if (inline_object) return 0;
	}
	else if (!strcmpi(scheme.data(), "http"))
	{
		if (!(found = GetHTTP(hostname, portname, filename, tmpname, inline_object, post, head, FALSE)))
			if (inline_object) return 0;
	}
	else if (!strcmpi(scheme.data(), "ftp"))
	{
		if (!(found = GetFTP(hostname, portname, filename, tmpname, inline_object)))
			if (inline_object) return 0;
	}
	else if (!strcmpi(scheme.data(), "mailto"))
	{
		SendMail(filename);
		return 1;
	}
	else if (!strcmpi(scheme.data(), "about"))
	{
		About();
		return 1;
	}
	else if (!strcmpi(scheme.data(), "javascript"))
	{
		const char* cmd = strchr(new_url.data(), ':');
		return JavaScript(cmd?cmd+1:"");
	}
	else
	{
		OnSetStatusText((string("Unimplemented resource-type: ")+scheme).data());
		return 1;
	}

	// Inline image display?

	if (inline_object)
	{
		localname = tmpname;
		return 1;
	}

	base = url = new_url;

	// A known or unknown type?

	const char* ext_ptr = "";

	if ((ext_ptr = strrchr(tmpname.data(), '.')) != 0)
	{
		if (!strcmpi(ext_ptr, ".htm") || !strcmpi(ext_ptr, ".html"))
			;
		else if (!strcmpi(ext_ptr, ".sht") || !strcmpi(ext_ptr, ".shtml"))
			;
		else if (!strcmpi(ext_ptr, ".wsml"))
			;
		else if (!strcmpi(ext_ptr, ".txt"))
			;
		else if (!strcmpi(ext_ptr, ".gif"))
			is_image = TRUE;
		else if (!strcmpi(ext_ptr, ".bmp"))
			is_image = TRUE;
#ifdef wx_msw
		else if (!strcmpi(ext_ptr, ".xpm"))
			is_image = TRUE;
		else if (!strcmpi(ext_ptr, ".jpg"))
			is_image = TRUE;
		else if (!strcmpi(ext_ptr, ".png"))
			is_image = TRUE;
		else if (!strcmpi(ext_ptr, ".wav"))
			is_wave = TRUE;
		else if (!strcmpi(ext_ptr, ".avi"))
			is_avi = TRUE;
#else
		else if (!strcmpi(ext_ptr, ".pbm"))
			is_image = TRUE;
		else if (!strcmpi(ext_ptr, ".pcx"))
			is_image = TRUE;
		else if (!strcmpi(ext_ptr, ".xbm"))
			is_image = TRUE;
#endif
		else
		{
			OnSetStatusText((string("Unimplemented type: ")+tmpname).data());
			localname = tmpname;
			OnDownload(url, tmpname);
			return 1;
		}
	}

	// Is it a standalone image? If so construct a document
	// page to contain it...

	if (is_image)
	{
		if (IsVerifying())
			SetText((string("<HTML><TITLE>")+url+"</TITLE></HTML>").data());
		else
			SetText((string("<HEAD><TITLE>")+url+"</TITLE></HEAD>"+
							"<BODY><IMG src=\""+base+"\"></BODY>").data()
							);

		return found;
	}
#ifdef wx_msw
	else if (is_wave)
	{
		if (!IsVerifying())
			SetText((string("<HEAD><TITLE>")+url+"</TITLE></HEAD>"+
							"<BODY><BGSOUND src=\""+base+"\"></BODY>").data()
							);

		return found;
	}
	else if (is_avi)
	{
		if (!IsVerifying())
			SetText((string("<HEAD><TITLE>")+url+"</TITLE></HEAD>"+
							"<BODY><IMG dynsrc=\""+base+"\"></BODY>").data()
							);

		return found;
	}
#endif

	// Otherwise just load the document...

	if (!LoadFile(tmpname))
	{
		//wxMessageBox((char*)(string("Load error: ")+ext_ptr).data());
		return 0;
	}

	OnOpen(url);

	if (!section.length())
		return found;

	ListOfIterator ilist = anchors;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		Anchor* match = (Anchor*)a;

		if (!strcmpi(match->name.data(), section.data()))
		{
			if (((int)(match->y / VSCROLL_INCREMENT)) > 0)
			{
				Clear();
				Scroll(0, (int)(match->y / VSCROLL_INCREMENT));
			}

			return found;
		}
	}

	Clear();
	Scroll(0, 0);

	return 1;
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::SaveHistory()
{
	if (forward && url.length())
	{
		History* item = new History;

		ViewStart(&item->scroll_x, &item->scroll_y);
		item->url = AbsoluteURL(url);
		item->background = background;
		history.Append(item);

		while (history.Count() > 99)
		{
			void* a;
			history.Pop(a);
			delete (History*)a;
		}

		url = "";
	}
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::OpenURL(const string& u, Bool post, Bool head, Bool put)
{
	cancel = 0;

	if (edit) return 0;

	// Save what was last shown in the history list...

	if (!IsCompiling())
		SaveHistory();

	// Go and get it...

	string localname = "";

	return GetURL(u, FALSE, localname, post, head, put);
}

///////////////////////////////////////////////////////////////////////////////

void wxDeferralTimer::Notify()
{
	if ((canvas->outstanding == max_reqs) || canvas->in_progress)
		return;

	// Delete any deferred sub-frames...

	void* a;

	while (canvas->delete_list.Pop(a))
		delete (wxHtml*)a;

	// Get any high-priority deferred objects...

	//Stop();

//LOOP:

	wxHtml::Deferral* image;
	string localname;

	if (!canvas->deferrals_hi.Pop(a)) goto LOW_PRIORITY;

	image = (wxHtml::Deferral *)a;

	if (!canvas->cancel && image->srcname.length())
	{
		if (canvas->GetURL(image->srcname, TRUE, localname))
		{
			int ret = canvas->DisplayImage(FALSE, canvas->GetDC(), localname, image->x, image->y, image->w, image->h);

			if (!ret || canvas->edit)
			{
				canvas->SetPen(wxThePenList->FindOrCreatePen(canvas->foreground, 1, wxSOLID));
				canvas->SetBrush(wxTheBrushList->FindOrCreateBrush(canvas->background, wxTRANSPARENT));
				canvas->DrawRectangle(image->x, image->y, image->w, image->h);
			}
		}
	}

	delete image;
	goto DONE;

LOW_PRIORITY:

	// Get any low-priority deferred objects...

	if (!canvas->deferrals_lo.Pop(a)) goto DONE;

	image = (wxHtml::Deferral *)a;

	if (!canvas->cancel && image->dynsrc.length())
	{
		if (canvas->GetURL(image->dynsrc, TRUE, localname))
		{
			canvas->DisplayDynamic(FALSE, canvas->GetDC(), localname, image->x, image->y, image->w, image->h);
		}
	}

	else if (!canvas->cancel && image->soundname.length())
	{
		if (canvas->GetURL(image->soundname, TRUE, localname))
		{
			canvas->DisplaySound(localname);
		}
	}

	delete image;

DONE:

	if (!canvas->DeferredLoads() && !canvas->outstanding)
	{
		Stop();
		canvas->OnOpen("");
	}

	//goto LOOP;
}

///////////////////////////////////////////////////////////////////////////////

wxHtml::Anchor* wxHtml::LocateAnchor(float tmp_x, float tmp_y)
{
	ListOfIterator ilist = anchors;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		Anchor* item = (Anchor*)a;

		if ( !item->isname &&
				(tmp_x >= item->x) && (tmp_x < (item->x + item->w)) &&
					(tmp_y >= item->y) && (tmp_y < (item->y + item->h)) )
		{
			return item;
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////

static int PointInRegion(ListOf& point_list, float pos_x, float pos_y)
{
	int status;
	int n = point_list.Count();

	ListOfIterator ilist = point_list;
	void* a;

#ifdef wx_msw

	POINT* points = new POINT[n];
	int i = 0;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxPoint* p = (wxPoint*)a;
		points[i].x = (long)p->x;
		points[i].y = (long)p->y;
		i++;
	}

	HRGN hrgn = CreatePolygonRgn(points, n, WINDING);
	status = PtInRegion(hrgn, (int)pos_x, (int)pos_y);
	DeleteObject(hrgn);
	delete [] points;

#else
	
	XPoint* points = new XPoint[n];
	int i = 0;

	for(int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxPoint* p = (wxPoint*)a;
		points[i].x = (short)p->x;
		points[i].y = (short)p->y;
		i++;
	}

	Region region = XPolygonRegion(points, n, 0);
	status = XPointInRegion(region, (int)pos_x, (int)pos_y);
	XDestroyRegion(region);
	delete [] points;

#endif

	return status;
}

///////////////////////////////////////////////////////////////////////////////

Bool wxHtml::Area(Imagemap::Area* area, float pos_x, float pos_y)
{
	int status = FALSE;

	if (!strcmpi(area->shape.data(), "RECT"))
	{
		float x1, y1, x2, y2;

		if (sscanf(area->coords.data(), "%f,%f,%f,%f", &x1, &y1, &x2, &y2) == 4)
		{
			if ( (pos_x >= x1) && (pos_x <= x2) &&
					(pos_y >= y1) && (pos_y <= y2) )
				return TRUE;
		}
	}
	else if (!strcmpi(area->shape.data(), "POLY"))
	{
		const char* src = area->coords.data();
		ListOf point_list;

		while (src)
		{
			float x1, y1;
			sscanf(src, "%f", &x1);
			src = strchr(src, ',');
			if (!src) return FALSE;
			sscanf(++src, "%f", &y1);
			point_list.Append(new wxPoint(x1, y1));
			src = strchr(src, ',');
			if (!src) break;
			src++;
		}

		status = PointInRegion(point_list, pos_x, pos_y);

		void* a;
		while (point_list.Pop(a))
		{
			delete (wxPoint*)a;
		}
	}

	return status;
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::OnEvent(wxMouseEvent& event)
{
#if 1
	ListOfIterator ilist = sub_frames;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxHtml* canvas = (wxHtml*)a;
		canvas->OnEvent(event);
	}

	if (sub_frames.Count()) return;
#endif

	float pos_x, pos_y;
	event.Position(&pos_x, &pos_y);

	SetFocus();

#if ALLOW_EDIT_MODE
	if (edit)
	{
		ProcessEvent(event);
		return;
	}
#endif

	last_anchor_item = LocateAnchor(pos_x, pos_y);

	if (event.RightUp())
	{
		last_canvas = this;

		if (IsEdit())
			LocateHtmlObject(pos_x, pos_y);


		if (GetRealParent() && history.Count() && !IsEdit())
			menu1->Enable(HTML_BACK, TRUE);
		else
			menu1->Enable(HTML_BACK, FALSE);

		if (last_anchor_item)
		{
			menu1->Enable(HTML_COPY, TRUE);
			menu1->Enable(HTML_EDIT, TRUE);
		}
		else
		{
			menu1->Enable(HTML_COPY, FALSE);
			menu1->Enable(HTML_EDIT, FALSE);
		}

		PopupMenu(menu1, (float)ABSOLUTE_X(pos_x), (float)ABSOLUTE_Y(pos_y));
	}

	if (!last_anchor_item)
	{
		SetCursor(new wxCursor(wxCURSOR_ARROW));
		OnSetStatusText("");
		return;
	}

	SetCursor(new wxCursor(wxCURSOR_HAND));
	string tmp_url = "";

	if (last_anchor_item->usemap.length())
	{
		// Search thru list of imagemaps...

		ListOfIterator ilist = imagemaps;
		void* a;

		for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
		{
			Imagemap* map = (Imagemap*)a;

			// Found a match?

			if (!strcmpi(map->name.data(), last_anchor_item->usemap.data()+1))
			{
				// Search thru list of areas for map...

				ListOfIterator ilist2 = map->areas;
				void* aa;

				for (int ok2 = ilist2.First(aa); ok2; ok2 = ilist2.Next(aa))
				{
					Imagemap::Area* area = (Imagemap::Area*)aa;
					float _x = pos_x - last_anchor_item->x;
					float _y = pos_y - last_anchor_item->y;

					// Found an area of overlap?

					if (Area(area, _x, _y))
					{
						tmp_url = area->name;
						OnSetStatusText(tmp_url.data());
						break;
					}
				}
			}
		}
	}

	if (!tmp_url.length() && last_anchor_item->ismap)
	{
		tmp_url = last_anchor_item->name;
		tmp_url += "?";
		tmp_url += Number(int(pos_x - last_anchor_item->x)) + ",";
		tmp_url += Number(int(pos_y - last_anchor_item->y));
		OnSetStatusText(tmp_url.data());
	}
	else if (!tmp_url.length() && !last_anchor_item->usemap.length())
	{
		tmp_url = last_anchor_item->name;
		OnSetStatusText(tmp_url.data());
	}

	if (event.LeftUp())
	{
		// Get section...

		if (tmp_url[0] == '#')
		{
			Anchor* match;

			if ((match = FindAnchor(tmp_url.data()+1)) != 0)
			{
				if (url.length())
				{
					History* item = new History;
					ViewStart(&item->scroll_x, &item->scroll_y);
					item->url = base;
					item->background = background;
					history.Append(item);
				}

				// Just scroll to the new position in the document...

				SetCursor(new wxCursor(wxCURSOR_ARROW));
				Clear();
				Scroll(0, (int)(match->y / VSCROLL_INCREMENT));
				OnSetStatusText("");
			}
			else
			{
				wxBell();
			}

			return;
		}

		// Get URL...

		if (Stop()) return;

		//MapURL(AbsoluteURL(tmp_url), base);
		OnSetStatusText("Loading...");

		// Go to the appropriate frame (which is not
		// necessarily this one)...

		wxHtml* frame = WhichFrame(last_anchor_item->target);

#if ALLOW_SITE_MODE
		if (last_anchor_item->object && last_anchor_item->object->IsSiteAnchor())
		{
			HtmlSiteAnchor* sa = (HtmlSiteAnchor*)last_anchor_item->object;
			SetParams(sa->attributes, sa->values);
			default_bgcolor = sa->bgcolor;
			default_fgcolor = sa->text;
			default_link = sa->link;
			default_vlink = sa->vlink;
			default_alink = sa->alink;
			default_bgimage = sa->background;
			default_title = sa->title;
		}
#endif

		//frame->SetLoad(tmp_url);
		frame->OpenURL(tmp_url);
		
#if ALLOW_SITE_MODE
		ClearParams();
#endif
		
		OnSetStatusText("Done");
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////

Bool wxHtml::LocateHtmlObject(float pos_x, float pos_y)
{
	// Search thru all the tags...

	ListOfIterator ilist = *tags;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		Tag* tag = (Tag *)a;

		if (pos_y > tag->y)
		{
			// search thru all the objects on a line...

			ListOfIterator ilist2 = tag->objects;
			void* a2;
	
			for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
			{
				HtmlObject* object = (HtmlObject*)a2;

				// Co-ords are bounded by object?

				if (object->IsBounded(pos_x, pos_y, tag->h))
				{
					// Save this information...

#if ALLOW_EDIT_MODE
					if (edit)
					{
						edit_objects = ilist2;
						edit_object = object;
						edit_tags = ilist;
						edit_tag = tag;
					}
#endif

					// Set current attributes...

					center = tag->center;
					p_center = tag->p_center;
					left_margin = tag->left_margin;
					right_margin = tag->right_margin;

					return TRUE;
				}
			}
		}
	}

#if ALLOW_EDIT_MODE
	edit_object = 0;
#endif

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////

wxHtml::Anchor* wxHtml::FindAnchor(const char* name)
{
	ListOfIterator ilist = anchors;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		Anchor* match = (Anchor*)a;

		if (!match->isname) continue;

		if (!strcmpi(match->name.data(), name))
			return match;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::OnChar(wxKeyEvent& event)
{
#if 1
	ListOfIterator ilist = sub_frames;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxHtml* canvas = (wxHtml*)a;
		canvas->OnChar(event);
	}

	if (sub_frames.Count()) return;
#endif

#if ALLOW_EDIT_MODE

	if (edit)
	{
		ProcessChar(event);
	}
	else

#endif

#if USE_HTML_FORMS
	wxPanel::OnChar(event);
#else
	wxCanvas::OnChar(event);
#endif
}

///////////////////////////////////////////////////////////////////////////////

string wxHtml::GetCurrentFilename() const
{
	const char* src = url.data() + (url.length() - 1);
	int n = url.length();
	int dot = 0;

	while (n-- > 0)
	{
		if (*src == '.')
			dot = 1;
		else if (*src == '/')
		{
			src++;
			break;
		}

		src--;
	}

	if (dot)
		return src;
	else
		return "";
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::AllreadyDeferred(const string& name, float x, float y)
{
	ListOfIterator ilist = deferrals_lo;
	void* a;
	int ok;

	for (ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		Deferral* deferral = (Deferral*)a;

		if ( (deferral->x == x) && (deferral->y == y) &&
				( (deferral->soundname == name) ||
					(deferral->srcname == name) ||
					(deferral->dynsrc == name) ) )
		{
			return 1;
		}
	}

	ilist = deferrals_hi;

	for (ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		Deferral* deferral = (Deferral*)a;

		if ( (deferral->x == x) && (deferral->y == y) &&
				( (deferral->soundname == name) ||
					(deferral->srcname == name) ||
					(deferral->dynsrc == name) ) )
		{
			return 1;
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////

#ifdef wx_msw
int wxHtml::DisplaySound(const string& filename)
{
	string tmp = "play ";
	tmp += filename;
	mciSendString(tmp.data(), 0, 0, 0);
	return 1;
#else
int wxHtml::DisplaySound(const string&)
{
	return 0;
#endif
}

///////////////////////////////////////////////////////////////////////////////

#ifdef wx_msw
int wxHtml::DisplayDynamic(Bool, wxDC*, const string& filename, float x, float y, float w, float h)
{
	string

	cmd = "open ";
	cmd += filename;
	cmd += " type AVIVideo";
	cmd += " alias Animation";
	cmd += " parent ";
	cmd += Number((unsigned)GetHWND());
	cmd += " style ";
	cmd += Number(0x40000000L);
	mciSendString(cmd.data(), 0, 0, 0);

	cmd = "put Animation window at ";
	cmd += Number((int)x) + " ";
	cmd += Number((int)y) + " ";
	cmd += Number((int)w) + " ";
	cmd += Number((int)h) + " ";
	mciSendString(cmd.data(), 0, 0, 0);

	cmd = "play Animation";
	mciSendString(cmd.data(), 0, 0, 0);

	return 1;
#else
int wxHtml::DisplayDynamic(Bool, wxDC*, const string&, float, float, float, float)
{
	return 0;
#endif
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::DisplayImage(Bool quiet, wxDC* dc, const string& filename, float _x, float _y, float& w, float& h)
{
	Bitmap* item = 0;
	ListOfIterator ilist = bitmaps;
	void* a;

#if defined( wx_msw )
	const char* ext_ptr = strrchr(filename.data(), '.');
	int type;

	if (ext_ptr && !strcmpi(ext_ptr+1, "bmp"))
		type = IMAGE_FORMAT_BMP;
	else if (ext_ptr && !strcmpi(ext_ptr+1, "gif"))
		type = IMAGE_FORMAT_GIF;
	else if (ext_ptr && !strcmpi(ext_ptr+1, "xpm"))
		type = IMAGE_FORMAT_XPM;
	else if (ext_ptr && !strcmpi(ext_ptr+1, "jpg"))
		type = IMAGE_FORMAT_JPEG;
	else if (ext_ptr && !strcmpi(ext_ptr+1, "png"))
		type = IMAGE_FORMAT_PNG;
	else
		return 0;

	wxImage* bitmap = 0;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		item = (Bitmap*)a;

		if (item->name == filename)
		{
			if (quiet)
			{
				w = (float)item->width;
				h = (float)item->height;
				return 1;
			}

			ilist.Delete();
			bitmap = item->bitmap;
			delete item;
			break;
		}
	}

	if (!bitmap)
	{
		bitmap = new wxImage((char*)filename.data(), type);
		if (bitmap->GetWidth()<=0 || bitmap->GetHeight()<=0)
		{
			delete bitmap;
			return 0;
		}
	}

	if (w == -1)
		w = (float)bitmap->GetWidth();

	if (h == -1)
		h = (float)bitmap->GetHeight();

	if (!quiet)
	{
		if (bitmap->GetColourMap())
			dc->SetColourMap(bitmap->GetColourMap());

		bitmap->Stretch(dc, (int)_x, (int)_y, (int)w, (int)h, 0, 0, bitmap->GetWidth(), bitmap->GetHeight());
	}

#else

	wxBitmap* bitmap = 0;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		item = (Bitmap*)a;

		if (item->name == filename)
		{
			if (quiet)
			{
				w = item->width;
				h = item->height;
				return 1;
			}

			ilist.Delete();
			bitmap = item->bitmap;
			delete item;
			break;
		}
	}

	if (!bitmap)
	{
		bitmap = new wxBitmap((char*)filename.data(), wxBITMAP_TYPE_ANY);
		if (!bitmap->Ok())
		{
			delete bitmap;
			return 0;
		}
	}

	if (w == -1)
		w = bitmap->GetWidth();

	if (h == -1)
		h = bitmap->GetHeight();

	if (!quiet)
	{
		if (bitmap->GetColourMap())
			dc->SetColourMap(bitmap->GetColourMap());

		wxMemoryDC memdc;
		memdc.SelectObject(bitmap);

		dc->Blit(_x, _y, w, h, &memdc, 0, 0);
	}

#endif

	// Cache the bitmap...

	item = new Bitmap;
	item->name = filename;
	item->bitmap = bitmap;
	item->width = bitmap->GetWidth();
	item->height = bitmap->GetHeight();
	bitmaps.Append(item);

	// Only keep a small number of most recent around...

	while (bitmaps.Count() > 20)
	{
		bitmaps.Pop(a);
		item = (Bitmap*)a;
		delete item->bitmap;
		delete item;
	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::WallpaperImage(wxDC* dc, const string& filename, float _x, float w, float _y, float h)
{
	float save_x = _x;

#if defined( wx_msw )
	const char* ext_ptr = strrchr(filename.data(), '.');
	int type;

	if (ext_ptr && !strcmpi(ext_ptr+1, "bmp"))
		type = IMAGE_FORMAT_BMP;
	else if (ext_ptr && !strcmpi(ext_ptr+1, "gif"))
		type = IMAGE_FORMAT_GIF;
	else if (ext_ptr && !strcmpi(ext_ptr+1, "xpm"))
		type = IMAGE_FORMAT_XPM;
	else if (ext_ptr && !strcmpi(ext_ptr+1, "jpg"))
		type = IMAGE_FORMAT_JPEG;
	else if (ext_ptr && !strcmpi(ext_ptr+1, "png"))
		type = IMAGE_FORMAT_PNG;
	else
		return 0;

	wxImage bitmap((char*)filename.data(), type);

	int iw = bitmap.GetWidth();
	int ih = bitmap.GetHeight();

	if (!iw || !ih)
		return 0;

	if (bitmap.GetColourMap())
		dc->SetColourMap(bitmap.GetColourMap());

	float x2 = _x + w, y2 = _y + h;

	while (_y < y2)
	{
		while (_x < x2)
		{
			bitmap.Draw(dc, (int)_x, (int)_y, iw, ih);
			_x += iw;
		}

		_y += ih;
		_x = save_x;
	}
#else
	wxBitmap bitmap((char*)filename.data(), wxBITMAP_TYPE_ANY);

	if (!bitmap.Ok())
		return 0;

	int iw = bitmap.GetWidth();
	int ih = bitmap.GetHeight();

	if (!iw || !ih)
		return 0;

	if (bitmap.GetColourMap())
		dc->SetColourMap(bitmap.GetColourMap());

	wxMemoryDC temp_dc;
	temp_dc.SelectObject(&bitmap);

	float x2 = _x + w, y2 = _y + h;

	while (_y < y2)
	{
		while (_x < x2)
		{
			dc->Blit(_x, _y, iw, ih, &temp_dc, 0, 0);
			_x += iw;
		}

		_y += ih;
		_x = save_x;
	}

#endif

	return 1;
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::PopUpFunction1(wxMenu& /*menu*/, wxCommandEvent& event)
{
	int id = event.commandInt;

	switch (id)
	{
		case HTML_COPY:

#if USE_CLIPBOARD
			wxOpenClipboard();
			wxSetClipboardData(wxCF_TEXT, (wxObject*)last_canvas->last_anchor_item->name.data(), 0, 0);
			wxCloseClipboard();
#endif

			break;

		case HTML_BACK:

			last_canvas->Back();
			break;

#if 0
		case HTML_EDIT:
		{
			string url = last_canvas->last_anchor_item->name;
			string localname = "";

			if (last_canvas->GetURL(url, TRUE, localname))
			{
				RunAssociatedProgram(localname.data());
			}

			break;
		}
#endif
	}
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::JavaScript(const char* /*cmd*/)
{
	return 1;
}

///////////////////////////////////////////////////////////////////////////////

static struct Colours
{
	const char* name;
	wxColour* colour;
}
colours[16];

wxColour* wxHtml::HtmlColour(const char* name)
{
	static int no_colours = 1;

	if (no_colours)
	{
		no_colours = 0;

		colours[0].name = "aqua";
		colours[0].colour = new wxColour(0x00, 0xFF, 0xFF);

		colours[1].name = "black";
		colours[1].colour = new wxColour(0x00, 0x00, 0x00);

		colours[2].name = "blue";
		colours[2].colour = new wxColour(0x00, 0x00, 0xFF);

		colours[3].name = "gray";
		colours[3].colour = new wxColour(0x80, 0x80, 0x80);

		colours[4].name = "fuchsia";
		colours[4].colour = new wxColour(0xFF, 0x00, 0xFF);

		colours[5].name = "green";
		colours[5].colour = new wxColour(0x00, 0x80, 0x00);

		colours[6].name = "lime";
		colours[6].colour = new wxColour(0x00, 0xFF, 0x00);

		colours[7].name = "maroon";
		colours[7].colour = new wxColour(0x80, 0x00, 0x00);

		colours[8].name = "navy";
		colours[8].colour = new wxColour(0x00, 0x00, 0x80);

		colours[9].name = "olive";
		colours[9].colour = new wxColour(0x80, 0x80, 0x00);

		colours[10].name = "purple";
		colours[10].colour = new wxColour(0x80, 0x00, 0x80);

		colours[11].name = "red";
		colours[11].colour = new wxColour(0xFF, 0x00, 0x00);

		colours[12].name = "silver";
		colours[12].colour = new wxColour(0xC0, 0xC0, 0xC0);

		colours[13].name = "teal";
		colours[13].colour = new wxColour(0x00, 0x80, 0x80);

		colours[14].name = "white";
		colours[14].colour = new wxColour(0xFF, 0xFF, 0xFF);

		colours[15].name = "yellow";
		colours[15].colour = new wxColour(0xFF, 0xFF, 0x00);
	}

	for (int i = 0; i < 16; i++)
	{
		if (!strcmpi(colours[i].name, name))
			return colours[i].colour;
	}

	return HtmlColour("silver");
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::ConsoleBegin()
{
	Enable(FALSE);

	if (!console)
	{
		int x, y, w, h;
		GetPosition(&x, &y);
		GetSize(&w, &h);

		console = new wxTextWindow(GetParent(), x, y, w, h, wxBORDER);
		console->SetFont(wxTheFontList->FindOrCreateFont(11, wxSWISS, wxNORMAL, wxNORMAL, FALSE));
	}
	else
	{
		console->Enable(TRUE);
		console->Show(TRUE);
	}

	wxYield();
}

void wxHtml::ConsoleDone()
{
	if (console)
	{
		console->Enable(FALSE);
		console->Show(FALSE);
	}

	Enable(TRUE);
}

void wxHtml::ConsoleClear()
{
	if (console) console->Clear();
}

void wxHtml::ConsoleWrite(const char* s)
{
	if (console) console->WriteText((char*)s);
}

void wxHtml::ConsoleWrite(const string& s)
{
	if (console) ConsoleWrite(s.data());		
}




