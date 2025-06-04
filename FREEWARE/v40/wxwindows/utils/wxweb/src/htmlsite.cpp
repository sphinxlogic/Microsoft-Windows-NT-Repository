///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  An HTML canvas for wxWindows.
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
#include <ctype.h>
#include <errno.h>

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include "wxhtml.h"
#include "htmlpars.h"
#include "url.h"

#if ALLOW_SITE_MODE

static string project_base[MAX_TABS], project_bgcolor[MAX_TABS];
static string project_background[MAX_TABS], project_text[MAX_TABS];
static string project_link[MAX_TABS];
static string project_vlink[MAX_TABS];
static string project_alink[MAX_TABS];
static ListOf project_attribute[MAX_TABS], project_value[MAX_TABS];
static string project_target[MAX_TABS];
static ListOf attributes, values;

#define SHOW_OBJECTS 0
const LIST_TYPE = 2;

///////////////////////////////////////////////////////////////////////////////

int wxHtml::ParseSite(wxDC* dc, char*& /*dst*/, char* /*buf*/, char* tag)
{
	static HtmlSiteAnchor* sa = 0;
	int status = 0;

	if (!strncmpi(tag, "WSML", 4))
	{
		Release(dc);
		string key;
		string name;
		const char* ptr = tag+4;

		string site_target = "";

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "TARGET"))
			{
				site_target = name;
			}
			else if (!strcmpi(key.data(), "COMMENT"))
			{
				bgcomment = name;
			}
		}

		if (!tabs)
		{
			project_base[tabs] = project_bgcolor[tabs] = 
			project_text[tabs] = project_background[tabs] = 
			project_link[tabs] = project_vlink[tabs] =
			project_alink[tabs] = "";
			project_target[tabs] = site_target;
		}

		list_type[tabs] = LIST_TYPE;
		count[tabs] = 0;
		wsmling++;
		is_wsml = TRUE;
		status = 1;
	}
	else if (wsmling && !strcmpi(tag, "/WSML"))
	{
		Release(dc);
		wsmling--;
		overflow = 1;
		status = 1;
	}
	else if (wsmling && !strncmpi(tag, "PROJECT", 7))
	{
		Release(dc);

		if (tabs)
		{
			project_base[tabs] = project_base[tabs-1];
			project_bgcolor[tabs] = project_bgcolor[tabs-1];
			project_link[tabs] = project_link[tabs-1];
			project_alink[tabs] = project_alink[tabs-1];
			project_vlink[tabs] = project_vlink[tabs-1];
			project_text[tabs] = project_text[tabs-1];
			project_target[tabs] = project_target[tabs-1];
			project_background[tabs] = project_background[tabs-1];
		}

		string key;
		string name;
		const char* ptr = tag+7;
		string pagetitle = "";

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "TITLE"))
			{
				pagetitle = name;
			}
			else if (!strcmpi(key.data(), "TARGET"))
			{
				project_target[tabs] += "/" + name;
			}
			else if (!strcmpi(key.data(), "BASE"))
			{
				if (project_background[tabs].length() && (project_background[tabs][0] != '/'))
					project_background[tabs] = "../" + project_background[tabs];
	
				if (project_base[tabs].length())
					project_base[tabs] += "/";

				project_base[tabs] += name;
			}
			else if (!strcmpi(key.data(), "BGCOLOR"))
			{
				project_bgcolor[tabs] = name;
			}
			else if (!strcmpi(key.data(), "TEXT"))
			{
				project_text[tabs] = name;
			}
			else if (!strcmpi(key.data(), "BACKGROUND"))
			{
				project_background[tabs] = name;
			}
			else if (!strcmpi(key.data(), "LINK"))
			{
				project_link[tabs] = name;
			}
			else if (!strcmpi(key.data(), "VLINK"))
			{
				project_vlink[tabs] = name;
			}
			else if (!strcmpi(key.data(), "ALINK"))
			{
				project_alink[tabs] = name;
			}
		}

		y += max_h;
		max_h = 0.0F;
		x = left_margin;

		Hold(new HtmlList(this, x, y, LIST_TYPE));
		Release(dc);
		list_type[tabs] = LIST_TYPE;
		count[tabs] = 0;
		tabs++;
		status = 1;
		sa = 0;
	}
	else if (wsmling && !strcmpi(tag, "/PROJECT")) 
	{
		void* a;

		while (project_attribute[tabs-1].Pop(a))
			delete (string*)a;

		while (project_value[tabs-1].Pop(a))
			delete (string*)a;
		
		Hold(new HtmlEndList(this, x, y, LIST_TYPE));
		Release(dc);
		tabs--;

		ClearParams();
		for (int i = 0; i < tabs; i++)
			SetParams(project_attribute[i], project_value[i]);

		y += max_h;
		max_h = 0.0F;
		x = left_margin;
		status = 1;
		sa = 0;
	}
	else if (wsmling && !strncmpi(tag, "PAGE", 4))
	{
		Release(dc);
		SaveState();

		y += max_h;
		max_h = 0.0F;
		x = left_margin;
		
		project_base[tabs] = project_base[tabs-1];
		project_bgcolor[tabs] = project_bgcolor[tabs-1];
		project_text[tabs] = project_text[tabs-1];
		project_background[tabs] = project_background[tabs-1];
		project_target[tabs] = project_target[tabs-1];
		project_link[tabs] = project_link[tabs-1];
		project_vlink[tabs] = project_vlink[tabs-1];
		project_alink[tabs] = project_alink[tabs-1];

		string key;
		string name;
		const char* ptr = tag+4;

		string pagetitle = "", pageref = "", pagesrc = "";

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "TITLE"))
			{
				pagetitle = name;
			}
			else if (!strcmpi(key.data(), "HREF"))
			{
				if (name[0] == '/')
					pageref = name;
				else
				{
					pageref = project_base[tabs];
					if (pageref.length()) pageref += "/";
					pageref += name;
				}
			}
			else if (!strcmpi(key.data(), "SRC"))
			{
				pagesrc = name;
			}
			else if (!strcmpi(key.data(), "BGCOLOR"))
			{
				project_bgcolor[tabs] = name;
			}
			else if (!strcmpi(key.data(), "TEXT"))
			{
				project_text[tabs] = name;
			}
			else if (!strcmpi(key.data(), "BACKGROUND"))
			{
				project_background[tabs] = name;
			}
			else if (!strcmpi(key.data(), "LINK"))
			{
				project_link[tabs] = name;
			}
			else if (!strcmpi(key.data(), "VLINK"))
			{
				project_vlink[tabs] = name;
			}
			else if (!strcmpi(key.data(), "ALINK"))
			{
				project_alink[tabs] = name;
			}
		}

		int i;

		x += (tabs-1) * TAB_SIZE * char_width;

		Hold(new HtmlUnorderedListIndex(this, x, y, 15.0F, char_height));

		foreground = HtmlColour("BLUE");
		dc->SetTextForeground(foreground);

		fsz = basefont;
		mode = UNDERLINE;

		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
				(mode & FIXED ? wxMODERN : family),
				(mode & ITALIC ? wxITALIC : style),
				(mode & BOLD ? wxBOLD : weight),
				(mode & UNDERLINE ? TRUE : FALSE)));

		string u = wxPathOnly((char*)base.data());
		if (pagesrc.length())
			u += "/" + pagesrc;
		else
			u += "/" + pageref;

		Hold(sa = new HtmlSiteAnchor(this, x, y, u));

		// Copy all the accumulated params to date...

		for (i = 0; i < tabs; i++)
		{
			ListOfIterator ilist = project_attribute[i];
			void* a;

			for (int ok1 = ilist.First(a); ok1; ok1 = ilist.Next(a))
			{
				string* s = (string*)a;
				sa->attributes.Append(new string(*s));
			}

			ilist = project_value[i];

			for (int ok2 = ilist.First(a); ok2; ok2 = ilist.Next(a))
			{
				string* s = (string*)a;
				sa->values.Append(new string(*s));
			}
		}

		sa->bgcolor = project_bgcolor[tabs];
		sa->text = project_text[tabs];
		sa->background = project_background[tabs];
		sa->filename = project_target[tabs] + "/" + pageref;
		sa->link = project_link[tabs];
		sa->vlink = project_vlink[tabs];
		sa->alink = project_alink[tabs];
		sa->title = pagetitle;
		sa->isobject = FALSE;

		x += 15;
		float tmp_w, tmp_h;
		dc->GetTextExtent((char*)pagetitle.data(), &tmp_w, &tmp_h);
		Hold(new HtmlText(this, x, y, tmp_w, tmp_h, (char*)pagetitle.data()));
		max_h = tmp_h;
		x += tmp_w;
		
		RestoreState();
		Hold(new HtmlDeanchor(this, x, y));
		Release(dc);
		dc->SetTextForeground(foreground);
		status = 1;
	}
	else if (wsmling && !strcmpi(tag, "/PAGE"))
	{
		sa = 0;
		status = 1;
	}
	else if (wsmling && !strncmpi(tag, "PARAM", 5))
	{
		string key;
		string name;
		const char* ptr = tag+5;
		string p_name = "", p_value = "";

		while (::GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "NAME"))
			{
				p_name = name;
			}
			else if (!strcmpi(key.data(), "VALUE"))
			{
				p_value = name;
			}
		}

		// Build up the project level...

		if (!sa)
		{
			project_attribute[tabs-1].Append(new string(p_name));
			project_value[tabs-1].Append(new string(p_value));

			ClearParams();
			for (int i = 0; i < tabs; i++)
				SetParams(project_attribute[i], project_value[i]);
		}

		// Build up the page level...

		if (sa)
		{
			sa->attributes.Append(new string(p_name));
			sa->values.Append(new string(p_value));
		}

		status = 1;
	}
	else if (wsmling && !strncmpi(tag, "OBJECT", 6))
	{
		Release(dc);
		SaveState();

		y += max_h;
		max_h = 0.0F;
		x = left_margin;
		
		project_base[tabs] = project_base[tabs-1];
		project_target[tabs] = project_target[tabs-1];

		string key;
		string name;
		const char* ptr = tag+6;

		string pageref = "", pagesrc = "";

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "HREF"))
			{
				if ((name[0] == '/') || (name[0] == '.'))
					pageref = name;
				else
				{
					pageref = project_base[tabs];
					if (pageref.length()) pageref += "/";
					pageref += name;
				}
			}
			else if (!strcmpi(key.data(), "SRC"))
			{
				pagesrc = name;
			}
#if 0
			if (!strcmpi(key.data(), "SRC"))
			{
				if ((name[0] == '/') || (name[0] == '.'))
					objectref = name;
				else
					objectref = project_base[tabs] + "/" + name;
			}
#endif
		}

#if SHOW_OBJECTS
		x += (tabs-1) * TAB_SIZE * char_width;

		Hold(new HtmlUnorderedListIndex(this, x, y, 15, char_height));

		foreground = HtmlColour("BLUE");
		dc->SetTextForeground(foreground);

		fsz = basefont;
		mode = UNDERLINE;

		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
				(mode & FIXED ? wxMODERN : family),
				(mode & ITALIC ? wxITALIC : style),
				(mode & BOLD ? wxBOLD : weight),
				(mode & UNDERLINE ? TRUE : FALSE)));
#endif

		string u = wxPathOnly((char*)base.data());
		if (pagesrc.length())
			u += "/" + pagesrc;
		else
			u += "/" + pageref;

		Hold(sa = new HtmlSiteAnchor(this, x, y, u));
		sa->filename = project_target[tabs] + "/" + pageref;
		sa->isobject = TRUE;

#if SHOW_OBJECTS
		x += 15;
		float tmp_w, tmp_h;
		dc->GetTextExtent((char*)url.data(), &tmp_w, &tmp_h);
		Hold(new HtmlText(this, x, y, tmp_w, tmp_h, (char*)u.data()));
		max_h = tmp_h;
		x += tmp_w;
#endif

		RestoreState();
		Hold(new HtmlDeanchor(this, x, y));
		Release(dc);
		dc->SetTextForeground(foreground);
		status = 1;
		sa = 0;
	}
	else if (wsmling && !strncmpi(tag, "DEBUG", 5))
	{
		Release(dc);

		string key;
		string name;
		const char* ptr = tag+5;

		string s = "DEBUG ";

		while (GetAttributeValuePair(ptr, key, name))
		{
			s += key + " = "+ name + " ";
		}

		wxMessageBox((char*)s.data());
		status = 1;
	}

	return status;
}

///////////////////////////////////////////////////////////////////////////////

HtmlSiteAnchor::~HtmlSiteAnchor()
{
	void* a;

	while (attributes.Pop(a))
		delete (string*)a;

	while (values.Pop(a))
		delete (string*)a;
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::SetParams(ListOf& _attributes, ListOf& _values)
{
	ListOfIterator ilist = _attributes;
	void* aa;

	for (int ok1 = ilist.First(aa); ok1; ok1 = ilist.Next(aa))
	{
		string* a = (string*)aa;
		attributes.Append(new string(*a));
	}

	ilist = _values;

	for (int ok2 = ilist.First(aa); ok2; ok2 = ilist.Next(aa))
	{
		string* v = (string*)aa;
		values.Append(new string(*v));
	}

}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::ClearParams()
{
	void* a;

	while (attributes.Pop(a))
		delete (string*)a;

	while (values.Pop(a))
		delete (string*)a;
}

///////////////////////////////////////////////////////////////////////////////

Bool wxHtml::CompileSite(Anchor* a, wxHtml* canvas)
{
	HtmlSiteAnchor* sa = (HtmlSiteAnchor*)a->object;
	Bool status = TRUE;

	ConsoleWrite(a->name);
	ConsoleWrite(" => ");
	ConsoleWrite(sa->filename);
	ConsoleWrite("\n");

	// A Goto! I'm so lazy sometimes...

loop:

	string dir = wxPathOnly((char*)sa->filename.data());
	int i;

	// Create any intermediate directories...

	for (i = 0; i < 16; i++)
	{
		if (!wxMkdir(dir.data()) && (errno == ENOENT))
		{
			dir = wxPathOnly((char*)dir.data());
			continue;
		}

		break;
	}

	if (i != 0) goto loop;

	// Open & copy the file...

	if (sa->isobject)
	{
		string localname;

		if (!GetURL(a->name, TRUE, localname))
		{
			ConsoleWrite("***ERROR*** Could not open input: ");
			ConsoleWrite(strerror(errno));
			ConsoleWrite("\n");
			status = FALSE;
		}
		else if (!::wxCopyFile(localname.data(), sa->filename.data()))
		{
			ConsoleWrite("***ERROR*** Could not create output: \n");
			ConsoleWrite(strerror(errno));
			ConsoleWrite("\n");
			status = FALSE;
		}
	}

	// Open & save the file...

	else
	{
		canvas->SetParams(sa->attributes, sa->values);
		canvas->default_bgcolor = sa->bgcolor;
		canvas->default_fgcolor = sa->text;
		canvas->default_bgimage = sa->background;
		canvas->default_title = sa->title;
		canvas->default_link = sa->link;
		canvas->default_vlink = sa->vlink;
		canvas->default_alink = sa->alink;
		canvas->bgcomment = bgcomment;

		if (!canvas->OpenURL(a->name))
		{
			ConsoleWrite("***ERROR*** Could not open input: ");
			ConsoleWrite(strerror(errno));
			ConsoleWrite("\n");
			status = FALSE;
		}
		else if (!canvas->SaveGeneratedFile(sa->filename))
		{
			ConsoleWrite("***ERROR*** Could not create output: \n");
			ConsoleWrite(strerror(errno));
			ConsoleWrite("\n");
			status = FALSE;
		}
		else
		{
			if (canvas->IsWSML())
			{
				if (!canvas->CompileSiteAll())
					status = FALSE;
			}
		}

		canvas->ClearParams();
	}

	return status;
}

///////////////////////////////////////////////////////////////////////////////

Bool wxHtml::CompileSiteAll()
{
	Bool status = TRUE;

	int w, h;
	GetSize(&w, &h);
	SetCompiling(TRUE);

	ConsoleBegin();

	wxHtml* canvas = Clone(0, drawing_h, w, h, 0);

	canvas->SetFrames(TRUE);
	canvas->SetTables(TRUE);
	canvas->SetImages(TRUE);
	canvas->SetCompiling(TRUE);

	ListOfIterator ilist = anchors;
	void* _a;

	for (int ok = ilist.First(_a); ok; ok = ilist.Next(_a))
	{
		Anchor* a = (Anchor*)_a;

		if (a->object->IsSiteAnchor())
		{
			if (!CompileSite(a, canvas))
			{
				status = FALSE;
				break;
			}
		}

		wxYield();
	}

	delete canvas;

	SetCompiling(FALSE);

	return status;
}

///////////////////////////////////////////////////////////////////////////////

Bool wxHtml::VerifySite(Anchor* a, wxHtml* canvas, Bool head)
{
	HtmlSiteAnchor* sa = (HtmlSiteAnchor*)a->object;
	Bool status = TRUE;

	ConsoleWrite(sa->filename);
	ConsoleWrite("\n");

	// Open the file...

	if (!sa->isobject)
	{
		canvas->SetParams(sa->attributes, sa->values);
		canvas->default_bgcolor = sa->bgcolor;
		canvas->default_fgcolor = sa->text;
		canvas->default_bgimage = sa->background;
		canvas->default_title = sa->title;
		canvas->default_link = sa->link;
		canvas->default_vlink = sa->vlink;
		canvas->default_alink = sa->alink;
		canvas->bgcomment = bgcomment;

		if (!canvas->OpenURL(a->name))
		{
			ConsoleWrite("***ERROR*** Could not open input: ");
			ConsoleWrite(strerror(errno));
			ConsoleWrite("\n");
			status = FALSE;
		}
		else
		{
			if (canvas->IsWSML())
			{
				if (!canvas->VerifySiteAll(head))
					status = FALSE;
			}
			else
			{
				// Now validate all the anchors...

				ListOfIterator ilist = anchors;
				void* _a;

				for (int ok = ilist.First(_a); ok && !cancel; ok = ilist.Next(_a))
				{
					Anchor* a = (Anchor*)_a;

					if (!a->isname && !a->ismap)
					{
						string localname;

						if (!canvas->GetURL(a->name, TRUE, localname, FALSE, head))
						{
							ConsoleWrite("***ERROR*** Invalid link: ");
							ConsoleWrite(a->name);
							ConsoleWrite("\n");
						}

						wxYield();
					}
				}
			}
		}

		canvas->ClearParams();
	}

	return status;
}

///////////////////////////////////////////////////////////////////////////////

Bool wxHtml::VerifySiteAll(Bool head)
{
	Bool status = TRUE;

	Enable(FALSE);
	SetVerifying(TRUE);

	int w, h;
	GetSize(&w, &h);

	ConsoleBegin();

	wxHtml* canvas = Clone(0, drawing_h, w, h, 0);

	canvas->SetFrames(TRUE);
	canvas->SetTables(TRUE);
	canvas->SetImages(TRUE);
	canvas->SetVerifying(TRUE);
	
	ListOfIterator ilist = anchors;
	void* _a;

	for (int ok = ilist.First(_a); ok && !cancel; ok = ilist.Next(_a))
	{
		Anchor* a = (Anchor*)_a;

		if (a->object->IsSiteAnchor())
		{
			if (!VerifySite(a, canvas, head))
			{
				status = FALSE;
				break;
			}
		}

		wxYield();
	}

	delete canvas;

	SetVerifying(FALSE);

	return status;
}

///////////////////////////////////////////////////////////////////////////////

static string ReplaceIn(const string& s, const string& s1, const string& s2)
{
	if (!s.length()) return s;
	if (!s1.length()) return s;

	const char* ptr = strstr(s.data(), s1.data());
	if (!ptr) return s;

	string tmp(s.data(), ptr - s.data());
	tmp += s2;
	tmp += ptr + s1.length();
	return tmp;
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::GetAttributeValuePair(const char*& src, string& p1, string& p2)
{
	int ret = ::GetAttributeValuePair(src, p1, p2);

	if (!ret) return ret;

	ListOfIterator ilist1 = attributes;
	ListOfIterator ilist2 = values;

	void* a1;
	void* a2;

	int ok1 = ilist1.Last(a1);
	int ok2 = ilist2.Last(a2);

	while (ok1 && ok2)
	{
		string* s1 = (string*)a1;
		string* s2 = (string*)a2;

		p2 = ReplaceIn(p2, *s1, *s2);

		ok1 = ilist1.Previous(a1);
		ok2 = ilist2.Previous(a2);
	}

	return ret;
}

#endif
