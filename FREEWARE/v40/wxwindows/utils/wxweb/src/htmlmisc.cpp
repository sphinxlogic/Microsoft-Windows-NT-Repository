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

#include "wxhtml.h"
#include "htmlpars.h"
#include "url.h"

///////////////////////////////////////////////////////////////////////////////
// attr1=value1 attr2=value2 ...
///////////////////////////////////////////////////////////////////////////////

#if !ALLOW_SITE_MODE
int wxHtml::GetAttributeValuePair(const char*& src, string& p1, string& p2)
{
	return ::GetAttributeValuePair(src, p1, p2);
}
#endif

///////////////////////////////////////////////////////////////////////////////

void wxHtml::SaveState()
{
	State* state = new State();
	state->color = foreground;
	state->family = family;
	state->mode = mode;
	state->fsz = fsz;
	state->center = center;
	state->p_center = p_center;
	state->p_right = p_right;
	state->next = states;
	states = state;
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::RestoreState()
{
	if (!states) return;

	if ((states->mode != mode) || (states->fsz != fsz) || (states->family != family))
	{
		GetDC()->SetFont(wxTheFontList->FindOrCreateFont(
			states->fsz,
			(states->mode & FIXED ? wxMODERN : family),
			(states->mode & ITALIC ? wxITALIC : style),
			(states->mode & BOLD ? wxBOLD : weight),
			(states->mode & UNDERLINE ? TRUE : FALSE)));
	}

	State* save = states;
	foreground = states->color;
	family = states->family;
	mode = states->mode;
	fsz = states->fsz;
	center = states->center;
	p_center = states->p_center;
	p_right = states->p_right;
	states = states->next;
	delete save;
}

///////////////////////////////////////////////////////////////////////////////
// To do...

static string Roman(unsigned n, int /*upper*/)
{
	string r = Number(n).data();
	return r;
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::ParseSome(wxDC* dc, char*&, char* /*buf*/, char* tag)
{
	int status = 0;

#if 0
	if (!strncmpi(tag, "!--#", 4))
	{
		string key;
		string name;
		const char* ptr = strchr(tag+4, ' ')+1;
		string newtag = string(tag, ptr-tag);

		while (GetAttributeValuePair(ptr, key, name))
		{
			newtag += key;

			if (key != "--")
				newtag += string("=\"") + name + "\" ";
		}

		Hold(new HtmlMarkup(this, x, y, newtag.data()));
		status = 1;
	}
#endif
	if (!ignore && !strcmpi(tag, "PLAINTEXT"))
	{
		mode |= FIXED;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
					(mode & FIXED ? wxMODERN : family),
					(mode & ITALIC ? wxITALIC : style),
					(mode & BOLD ? wxBOLD : weight),
					(mode & UNDERLINE ? TRUE : FALSE)));

		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		ignore = 1;
		status = 1;
	}
	else if (ignore && !strcmpi(tag, "/PLAINTEXT"))
	{
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		mode &= ~FIXED;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
					(mode & FIXED ? wxMODERN : family),
					(mode & ITALIC ? wxITALIC : style),
					(mode & BOLD ? wxBOLD : weight),
					(mode & UNDERLINE ? TRUE : FALSE)));
		ignore = 0;
		status = 1;
	}
	else if (!ignore && !strcmpi(tag, "COMMENT"))
	{
		quiet = ignore = 1;
		status = 1;
	}
	else if (ignore && !strcmpi(tag, "/COMMENT"))
	{
		quiet = ignore = 0;
		status = 1;
	}
	else if (!strncmpi(tag, "SCRIPT", 6))
	{
		quiet = ignore = 1;
		status = 1;
	}
	else if (!strcmpi(tag, "/SCRIPT"))
	{
		quiet = ignore = 0;
		status = 1;
	}

#if USE_HTML_FRAMES
	else if (!strcmpi(tag, "NOFRAMES"))
	{
		Hold(new HtmlMarkup(this, x, y, tag));
		
		if (!compiling && frames)
			quiet = ignore = 1;
		
		status = 1;
	}
	else if (!strcmpi(tag, "/NOFRAMES"))
	{
		Hold(new HtmlMarkup(this, x, y, tag));

		if (!compiling && frames)
			quiet = ignore = 0;

		status = 1;
	}
#endif

	return status;
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::ParseSome3(wxDC* dc, char*&, char* buf, char* tag)
{
	int status = 0;

	if (!strncmpi(tag, "A ", 2))
	{
		// In case they left off the </A> or try to overlap...

		if (addressing)
		{
			RestoreState();
			dc->SetTextForeground(foreground);
			Hold(new HtmlDeanchor(this, x, y));
			addressing = 0;
		}

		addressing_max_h = 0.0F;

		string key;
		string name;
		string ref = "", target = "";
		const char* ptr = tag+2;
		Bool isname = FALSE;

		SaveState();

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "NAME"))
			{
				isname = TRUE;
				ref = name;
				fsz = h4;
				mode |= BOLD;
			}
			else if (!strcmpi(key.data(), "HREF"))
			{
				if (name[0] != '#')
					ref = AbsoluteURL(name);
				else
					ref = name;

				string localname, mime_type, last_modified, expires;
				Bool not_found;

				if (!IsCompiling() && SearchCache(AbsoluteURL(ref), last_modified, expires, localname, not_found, mime_type, TRUE))
					foreground = HtmlColour("MAROON");
				else
					foreground = HtmlColour("BLUE");

				dc->SetTextForeground(foreground);

				isname = FALSE;
				ref = name;
				fsz = basefont;
				mode |= UNDERLINE;
			}
			else if (!strcmpi(key.data(), "TARGET"))
			{
				target = name;
			}
		}

		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
				(mode & FIXED ? wxMODERN : family),
				(mode & ITALIC ? wxITALIC : style),
				(mode & BOLD ? wxBOLD : weight),
				(mode & UNDERLINE ? TRUE : FALSE)));

		//if (IsCompiling() && !IsVerifying())
		//	Hold(new HtmlMarkup(this, x, y, tag));
		//else
		Hold(new HtmlAnchor(this, x, y, ref, isname, target));
		addressing = 1;
		status = 1;
	}
	else if (!strcmpi(tag, "/A"))
	{
		RestoreState();
		dc->SetTextForeground(foreground);

		//if (IsCompiling() && !IsVerifying())
		//	Hold(new HtmlMarkup(this, x, y, tag));
		//else
		{
			Hold(new HtmlDeanchor(this, x, y));
		}

		addressing = 0;
		status = 1;
	}
	else if (!strncmpi(tag, "H", 1) && isdigit(tag[1]))
	{
		SaveState();

		Release(dc);

		string key;
		string name;
		const char* ptr = tag+2;
		string align = "";

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "ALIGN"))
			{
				if (!strcmpi(name.data(), "CENTER"))
				{
					p_center = 1;
					p_right = 0;
				}
				else if (!strcmpi(name.data(), "LEFT"))
				{
					p_center = 0;
					p_right = 0;
				}
				else if (!strcmpi(name.data(), "RIGHT"))
				{
					p_center = 0;
					p_right = 1;
				}

				align = name;
			}
		}

		extent_x = wxMax(extent_x, x);
		//overflow = 1;
		if (!left_floating && left_limit_y) { y = left_limit_y; left_limit_y = 0.0F; }
		if (x != left_margin)
		{
			y += max_h;
			max_h = 0.0F;
			y += char_height;
		}

		SaveState();

		switch (tag[1])
		{
			case '1': fsz = h1; break;
			case '2': fsz = h2; break;
			case '3': fsz = h3; break;
			case '4': fsz = h4; break;
			case '5': fsz = h5; break;
			case '6': fsz = h6; break;
		}

		mode |= BOLD;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));

		if (!left_floating) left_margin = default_left_margin;
		x = left_margin;
		if (IsCompiling())
			Hold(new HtmlMarkup(this, x, y, tag));
		else
			Hold(new HtmlHeading(this, x, y, tag[1]-'0', align));

		heading = tag[1]-'0';
		status = 1;
	}
	else if (!strncmpi(tag, "HTML", 4))
	{
		status = 1;
	}
	else if (!strcmpi(tag, "/HTML"))
	{
		status = 1;
	}
	else if (!strncmpi(tag, "HEAD", 4))
	{
		status = 1;
	}
	else if (!strcmpi(tag, "/HEAD"))
	{
		status = 1;
	}
	else if (!strncmpi(tag, "TITLE", 5))
	{
		quiet = 1;
		status = 1;
	}
	else if (!strcmpi(tag, "/TITLE"))
	{
#if !defined( wx_xview )
		// Second call to SetTitle() crashes under XView
		GetParent()->SetTitle((char*)(string("WXWEB - ")+buf).data());
#endif
		bgtitle = buf;
		quiet = 0;
		status = 1;
	}

	return status;
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::ParseSome1(wxDC* dc, char*& dst, char* buf, char* tag)
{
	int status = 0;

	if ( heading && !strncmpi(tag, "/H", 2) && isdigit(tag[2]))
	{
		RestoreState();
		Hold(new HtmlEndHeading(this, x, y, heading));
		Release(dc);
		RestoreState();

		y += 4;//0.2 * max_h;
		//max_h = 0;
		overflow = 1;
		heading = 0;
		status = 1;
	}

	else if ( heading && !IsCompiling() && (
					!strncmpi(tag, "BR", 2) || 
					!strncmpi(tag, "P ", 2) ||
					!strcmpi(tag, "P")
					)
		)
	{
		RestoreState();
		Hold(new HtmlEndHeading(this, x, y, heading));
		Release(dc);
		RestoreState();

		y += 4;//0.2 * max_h;
		overflow = 1;
		heading = 0;
		status = 1;
	}

	else if (!strncmpi(tag, "/H", 2) && isdigit(tag[2]))
	{
		status = 1;
	}

	if (!strncmpi(tag, "BR", 2))
	{
		// In case they left off the </A> or try to overlap,
		// Netscape Gold is really good at this crap!

		if (addressing)
		{
			RestoreState();
			dc->SetTextForeground(foreground);
			Hold(new HtmlDeanchor(this, x, y));
			addressing = 0;
		}

		if (IsCompiling())
			Hold(new HtmlMarkup(this, x, y, tag));
		else
			Hold(new HtmlBreak(this, x, y));
		if (IsEdit()) max_h = wxMax(max_h, char_height);
		Release(dc);

		string key;
		string name;
		const char* ptr = tag+2;
		Bool clear_left = FALSE, clear_right = FALSE;

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "CLEAR"))
			{
				if (!strcmpi(name.data(), "LEFT"))
				{
					clear_left = TRUE;
					clear_right = FALSE;
				}
				else if (!strcmpi(name.data(), "RIGHT"))
				{
					clear_left = FALSE;
					clear_right = TRUE;
				}
				else if (!strcmpi(name.data(), "ALL"))
				{
					clear_left = TRUE;
					clear_right = TRUE;
				}
			}
		}

		extent_x = wxMax(extent_x, x);

		if (clear_left)
		{
			if (left_limit_y)
				y = wxMax(y, left_limit_y);
			left_limit_y = 0.0F;
			left_floating = 0;
			left_margin = default_left_margin;
			right_margin = default_right_margin;
		}

		if (clear_right)
		{
			if (right_limit_y)
				y = wxMax(y, right_limit_y);
			right_limit_y = 0.0F;
			right_floating = 0;
			left_margin = default_left_margin;
			right_margin = default_right_margin;
		}

		overflow = 1;
		if (!left_floating && left_limit_y)
			{ y = wxMax(y+max_h, left_limit_y); left_limit_y = 0.0F; overflow = 0; }

		if (!left_floating) left_margin = default_left_margin;
		x = left_margin;
		status = 1;
	}
	else if (!strcmpi(tag, "WBR"))
	{
		Hold(new HtmlMarkup(this, x, y, tag));
		overflow = 1;
		br = 1;
		status = 1;
	}
	else if (!strcmpi(tag, "NOBR"))
	{
		Hold(new HtmlMarkup(this, x, y, tag));
		nobr = 1;
		status = 1;
	}
	else if (!strcmpi(tag, "/NOBR"))
	{
		Hold(new HtmlMarkup(this, x, y, tag));
		nobr = 0;
		status = 1;
	}
	else if (!heading && (!strncmpi(tag, "P ", 2) || !strcmpi(tag, "P") ||
				!strncmpi(tag, "DIV", 3)))
	{
		int is_div = !strncmpi(tag, "DIV", 3);
		p_center = p_right = 0;

		extent_x = wxMax(extent_x, x);

		if (!previous_object || !previous_object->IsParagraph())
		{
			Release(dc);

			if (x != left_margin)
			{
				y += max_h;
			}

			string key;
			string name;
			const char* ptr = tag+2;

			while (GetAttributeValuePair(ptr, key, name))
			{
				if (!strcmpi(key.data(), "ALIGN"))
				{
					if (!strcmpi(name.data(), "CENTER"))
					{
						p_center = 1;
						p_right = 0;
					}
					else if (!strcmpi(name.data(), "LEFT"))
					{
						p_center = 0;
						p_right = 0;
					}
					else if (!strcmpi(name.data(), "RIGHT"))
					{
						p_center = 0;
						p_right = 1;
					}
				}
			}

			if (!left_floating && left_limit_y)
				{ y = left_limit_y; left_limit_y = 0.0F; }

			if (!left_floating) left_margin = default_left_margin;
			x = left_margin;
			if (IsCompiling())
				Hold(new HtmlMarkup(this, x, y, tag));
			else
				Hold(new HtmlParagraph(this, x, y));
			if (IsEdit()) max_h = wxMax(max_h, char_height);
			Release(dc);
			br = 1;
			if (!is_div) overflow = 1;
		}

		status = 1;
	}
	else if (!strcmpi(tag, "/P") || !strcmpi(tag, "/DIV"))
	{
		Release(dc);
		int is_div = !strcmpi(tag, "/DIV");

		extent_x = wxMax(extent_x, x);

		if (x != left_margin)
		{
			y += max_h;

			if (!is_div)
				y += char_height;
		}

		if (!left_floating && left_limit_y)
			{ y = left_limit_y; left_limit_y = 0.0F; }

		if (!left_floating) left_margin = default_left_margin;
		x = left_margin;
		if (IsCompiling())
			Hold(new HtmlMarkup(this, x, y, tag));
		else
			Hold(new HtmlEndParagraph(this, x, y));
		if (IsEdit()) max_h = wxMax(max_h, char_height);
		Release(dc);
		br = 1;
		p_right = p_center = 0;
		status = 1;
	}
	else if (tabs &&
			(!strcmpi(tag, "/UL") || !strcmpi(tag, "/OL") ||
				!strcmpi(tag, "/MENU") || !strcmpi(tag, "/DIR") ||
				!strcmpi(tag, "/DL"))
			)
	{
		Release(dc);

		tabs--;
		left_margin -= TAB_SIZE * char_width;
		x = left_margin;
		left_floating--;
		y += max_h;
		max_h = 0.0F;

		if (!tabs) left_limit_y = y;

		if (IsCompiling())
			Hold(new HtmlMarkup(this, x, y, tag));
		else
		Hold(new HtmlEndList(this, x, y, list_type[tabs]));
		Release(dc);

		//if (!tabs) y += char_height;
		//if (x != left_margin) overflow = 1;

		extent_x = wxMax(extent_x, x);
		if (!tabs) overflow = 1;
		status = 1;
	}
	else if (!strncmpi(tag, "OL", 2))
	{
		Release(dc);

		string key;
		string name;
		const char* ptr = tag+2;
		order_type = '1';

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "TYPE"))
			{
				order_type = name[0];
			}
		}

		if (!tabs && (x != left_margin))
		{
			y += max_h;
			overflow = 1;
		}

		list_type[tabs] = 1;
		count[tabs] = 0;

		tabs++;
		left_margin += TAB_SIZE * char_width;
		left_floating++;
		x = left_margin;

		if (IsCompiling())
			Hold(new HtmlMarkup(this, x, y, tag));
		else
		Hold(new HtmlList(this, x, y, 1));
		status = 1;
	}
	else if (!strncmpi(tag, "UL", 2))
	{
		Release(dc);

		if (!tabs && (x != left_margin))
		{
			y += max_h;
			overflow = 1;
		}

		list_type[tabs] = 2;

		tabs++;
		left_margin += TAB_SIZE * char_width;
		left_floating++;
		x = left_margin;

		if (IsCompiling())
			Hold(new HtmlMarkup(this, x, y, tag));
		else
		Hold(new HtmlList(this, x, y, 2));
		status = 1;
	}
	else if (!strncmpi(tag, "MENU", 4))
	{
		Release(dc);

		if (!tabs && (x != left_margin))
		{
			y += max_h;
			overflow = 1;
		}

		list_type[tabs] = 3;

		tabs++;
		left_margin += TAB_SIZE * char_width;
		left_floating++;
		x = left_margin;

		if (IsCompiling())
			Hold(new HtmlMarkup(this, x, y, tag));
		else
		Hold(new HtmlList(this, x, y, 3));
		status = 1;
	}
	else if (!strncmpi(tag, "DIR", 3))
	{
		Release(dc);

		if (!tabs && (x != left_margin))
		{
			y += max_h;
			overflow = 1;
		}

		list_type[tabs] = 4;

		tabs++;
		left_margin += TAB_SIZE * char_width;
		left_floating++;
		x = left_margin;

		if (IsCompiling())
			Hold(new HtmlMarkup(this, x, y, tag));
		else
		Hold(new HtmlList(this, x, y, 4));
		status = 1;
	}
	else if (!strncmpi(tag, "LI", 2) && tabs && (list_type[tabs-1] == 4))
	{
		Release(dc);

		extent_x = wxMax(extent_x, x);
		y += max_h;
		max_h = 0.0F;
		x = left_margin;
		//x += tabs * TAB_SIZE * char_width;
		//x += char_width;

		// Dodgy bug-fix...

		if (x >= right_margin)
			x = left_margin;

		if (IsCompiling())
			Hold(new HtmlMarkup(this, x, y, tag));
		else
		Hold(new HtmlListIndex(this, x, y, char_width, char_height));
		*dst = 0;
		status = 1;
	}
	else if (!strncmpi(tag, "LI", 2))
	{
		Release(dc);

		string key;
		string name;
		const char* ptr = tag+2;

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "TYPE"))
			{
				order_type = name[0];
			}
			else if (!strcmpi(key.data(), "VALUE"))
			{
				count[tabs-1] = atoi(name.data());
			}
		}

		extent_x = wxMax(extent_x, x);
		y += max_h;
		max_h = 0.0F;
		x = left_margin;
		//x += tabs * TAB_SIZE * char_width;
		//x += char_width;

		// Dodgy bug-fix...
		//if (x >= right_margin) x = left_margin;

		// Ordered list...

		if (tabs && list_type[tabs-1] == 1)
		{
			float tmp_w, tmp_h;

			count[tabs-1]++;

			if (order_type == 'A')
				sprintf(buf, "%c.  ", 'A'+(unsigned)count[tabs-1]-1);
			else if (order_type == 'a')
				sprintf(buf, "%c.  ", 'a'+(unsigned)count[tabs-1]-1);
			else if (order_type == 'I')
				sprintf(buf, "%s.  ", Roman((unsigned)count[tabs-1], 1).data());
			else if (order_type == 'i')
				sprintf(buf, "%s.  ", Roman((unsigned)count[tabs-1], 0).data());
			else
				sprintf(buf, "%d.  ", count[tabs-1]);

			dc->GetTextExtent(buf, &tmp_w, &tmp_h);
			if (IsCompiling())
				Hold(new HtmlMarkup(this, x, y, tag));
			else
			Hold(new HtmlOrderedListIndex(this, x, y, tmp_w, tmp_h, buf));
			if (tmp_h > max_h) max_h = tmp_h;
			x += tmp_w;
			dst = buf;
		}
		else if (!tabs || list_type[tabs-1] == 2)
		{
			if (IsCompiling())
				Hold(new HtmlMarkup(this, x, y, tag));
			else
			Hold(new HtmlUnorderedListIndex(this, x, y, 15.0F, 4.0F));
			x += 15.0F;
		}

		*dst = 0;
		status = 1;
	}
	else if (!strncmpi(tag, "DL", 2))
	{
		Release(dc);

		if (!tabs && (x != left_margin))
		{
			y += max_h;
			overflow = 1;
		}

		if (IsCompiling())
			Hold(new HtmlMarkup(this, x, y, tag));
		else
		Hold(new HtmlList(this, x, y, 5));
		list_type[tabs] = 5;
		tabs++;
		status = 1;
	}
	else if (!strncmpi(tag, "DT", 2))
	{
		Release(dc);

		extent_x = wxMax(extent_x, x);
		y += max_h;
		*dst = 0;
		x = left_margin;
		//x += (tabs-1) * TAB_SIZE * char_width;
		//x += char_width;

		status = 1;
	}
	else if (!strncmpi(tag, "DD", 2))
	{
		Release(dc);

		extent_x = wxMax(extent_x, x);
		y += max_h;
		*dst = 0;
		x = left_margin;
		//x += tabs * TAB_SIZE * char_width;
		//x += char_width;

		status = 1;
	}
	else if (!strncmpi(tag, "PRE", 3))
	{
		if (x != left_margin)
			overflow = 1;

		preformatted = 1;

		mode |= FIXED;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));

		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "/PRE"))
	{
		overflow = 1;
		preformatted = 0;

		mode &= ~FIXED;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));

		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "NOTE"))
	{
		Release(dc);
		Hold(new HtmlMarkup(this, x, y, tag));

		default_left_margin += 50.0F;
		default_right_margin -= 50.0F;
		overflow = 1;
		status = 1;
	}
	else if (!strcmpi(tag, "/NOTE"))
	{
		Hold(new HtmlMarkup(this, x, y, tag));
		default_left_margin -= 50.0F;
		default_right_margin += 50.0F;
		overflow = 1;
		status = 1;
	}
	else if (!strcmpi(tag, "ABSTRACT") || !strcmpi(tag, "MARGIN") || !strcmpi(tag, "FOOTNOTE"))
	{
		Release(dc);

		SaveState();
		fsz = basefont - 1;
		mode |= ITALIC;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));

		Hold(new HtmlMarkup(this, x, y, tag));
		default_left_margin += 50.0F;
		default_right_margin -= 50.0F;
		overflow = 1;
		status = 1;
	}
	else if (!strcmpi(tag, "/ABSTRACT") || !strcmpi(tag, "/MARGIN") || !strcmpi(tag, "/FOOTNOTE"))
	{
		Release(dc);
		RestoreState();
		Hold(new HtmlMarkup(this, x, y, tag));
		default_left_margin -= 50.0F;
		default_right_margin += 50.0F;
		overflow = 1;
		status = 1;
	}
	else if (!strcmpi(tag, "BLOCKQUOTE") || !strcmpi(tag, "QUOTE") || !strcmpi(tag, "BQ"))
	{
		Release(dc);
		Hold(new HtmlMarkup(this, x, y, tag));
		default_left_margin += 50.0F;
		default_right_margin -= 50.0F;
		overflow = 1;
		status = 1;
	}
	else if (!strcmpi(tag, "/BLOCKQUOTE") || !strcmpi(tag, "/QUOTE") || !strcmpi(tag, "/BQ"))
	{
		Hold(new HtmlMarkup(this, x, y, tag));
		default_left_margin -= 50.0F;
		default_right_margin += 50.0F;
		overflow = 1;
		status = 1;
	}
	else if (!strcmpi(tag, "STRONG"))
	{
		mode |= BOLD;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));

		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "/STRONG"))
	{
		mode &= ~BOLD;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));

		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "VAR"))
	{
		fsz = basefont - 1;
		mode |= FIXED;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));

		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "/VAR"))
	{
		fsz = basefont;
		mode &= ~FIXED;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "I") ||
					!strcmpi(tag, "CITE") ||
						!strcmpi(tag, "DFN") ||
							!strcmpi(tag, "ADDRESS"))
	{
		mode |= ITALIC;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));

		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "/I") ||
					!strcmpi(tag, "/CITE") ||
						!strcmpi(tag, "/DFN") ||
							!strcmpi(tag, "/ADDRESS"))
	{
		mode &= ~ITALIC;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "B") || !strcmpi(tag, "EM"))
	{
		mode |= BOLD;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "/B") || !strcmpi(tag, "/EM"))
	{
		mode &= ~BOLD;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "U"))
	{
		mode |= UNDERLINE;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "/U"))
	{
		mode &= ~UNDERLINE;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "S") || !strcmpi(tag, "STRIKE"))
	{
		mode |= STRIKE;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "/S") || !strcmpi(tag, "/STRIKE"))
	{
		mode &= ~STRIKE;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "TT") ||
					!strcmpi(tag, "SAMP") ||
						!strcmpi(tag, "XMP") ||
							!strcmpi(tag, "XMP") ||
								!strcmpi(tag, "CODE"))
	{
		mode |= FIXED;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "/TT") ||
					!strcmpi(tag, "/SAMP") ||
						!strcmpi(tag, "/XMP") ||
							!strcmpi(tag, "/LISTING") ||
								!strcmpi(tag, "/CODE"))
	{
		mode &= ~FIXED;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "KBD"))
	{
		mode |= FIXED|BOLD;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "/KBD"))
	{
		mode &= ~(FIXED|BOLD);
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "Q"))
	{
		if (!quote_level++)
			strcat(buf, "\"");
		else
			strcat(buf, "\'");

		status = 1;
	}
	else if (!strcmpi(tag, "/Q"))
	{
		if (!--quote_level)
			strcat(buf, "\"");
		else
			strcat(buf, "\'");

		status = 1;
	}
	else if (!strcmpi(tag, "HUGE"))
	{
		SaveState();
		fsz = basefont + 6;
		mode |= BOLD;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "/HUGE"))
	{
		RestoreState();
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "BIG"))
	{
		SaveState();
		fsz = basefont + 4;
		mode |= BOLD;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "/BIG"))
	{
		RestoreState();
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "SMALL"))
	{
		SaveState();
		fsz = basefont - 2;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "/SMALL"))
	{
		RestoreState();
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "TINY"))
	{
		SaveState();
		fsz = basefont - 3;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "/TINY"))
	{
		RestoreState();
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "SUB"))
	{
		fsz = basefont - 4;
		y += max_h/2;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			FALSE));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "/SUB"))
	{
		fsz += 4;
		y -= max_h/2;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "SUP"))
	{
		fsz = basefont + 4;
		y -= max_h/4;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			FALSE));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strcmpi(tag, "/SUP"))
	{
		fsz += 4;
		y += max_h/4;
		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));
		
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}

	return status;
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::ParseSome2(wxDC* dc, char*& dst, char* buf, char* tag)
{
	int status = 0;

	if (!strncmpi(tag, "CENTER", 6))
	{
		if (!tabling) Release(dc);
		Hold(new HtmlMarkup(this, x, y, tag));
		if (!tabling) overflow = 1;
		center = 1;
		right = 0;
		status = 1;
	}
	else if (!strcmpi(tag, "/CENTER"))
	{
		if (!tabling) Release(dc);
		if (!tabling) overflow = 1;
		Hold(new HtmlMarkup(this, x, y, tag));
		center = 0;
		right = 0;
		status = 1;
	}
	else if (!strncmpi(tag, "IMG", 3))
	{
		string key, name, srcname, dynsrc, usemap, alt;
		const char* ptr = tag+3;
		float w = -1.0F, h = -1.0F;
		int defer = 0;
		int align = 1;			// Should be 3
		int ismap = 0;
		int loop = 1;
		int border = -1;
		alt = "";

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (images && !strcmpi(key.data(), "SRC"))
			{
				srcname = name;
			}
			else if (!strcmpi(key.data(), "DYNSRC"))
			{
				dynsrc = name;
			}
			else if (!strcmpi(key.data(), "LOOP"))
			{
				if (!strcmpi(name.data(), "INFINITE"))
					loop = -1;
				else
					loop = atoi(name.data());
			}
			else if (!strcmpi(key.data(), "ISMAP"))
			{
				ismap = 1;
			}
			else if (!strcmpi(key.data(), "USEMAP"))
			{
				usemap = name;
			}
			else if (!strcmpi(key.data(), "BORDER"))
			{
				border = atoi(name.data());
			}
			else if (!strcmpi(key.data(), "ALIGN"))
			{
				if (!strcmpi(name.data(), "TOP"))
					align = 1;
				else if (!strcmpi(name.data(), "MIDDLE"))
					align = 2;
				else if (!strcmpi(name.data(), "BOTTOM"))
					align = 3;
				else if (!strcmpi(name.data(), "LEFT"))
					{ align = 4; left_floating = 1; }
				else if (!strcmpi(name.data(), "RIGHT"))
					{ align = 5; right_floating = 1; }
			}
			else if (!strcmpi(key.data(), "TEXT"))
			{
			}
			else if (!strcmpi(key.data(), "WIDTH"))
			{
				w = (float)atoi(name.data());
				if (w > 0) defer++;
			}
			else if (!strcmpi(key.data(), "HEIGHT"))
			{
				h = (float)atoi(name.data());
				if (w > 0) defer++;
			}
			else if (!strcmpi(key.data(), "ALT"))
			{
				alt = name;
			}
		}

		if (srcname.length() && IsVerifying())
		{
			Anchor* item = new Anchor;
			item->name = srcname;
			item->target = "_self";
			item->isname = 0;
			item->ismap = 0;
			item->x = 0.0F;
			item->y = 0.0F;
			item->w = 0.0F;
			item->h = 0.0F;
			item->object = 0;
			anchors.Push(item);
		}

		if (!alt.length())
			alt = srcname;

		if (!images)
		{
			strcpy(dst, alt.data());
			return 1;
		}

		// Draw it now?

		if (defer != 2)
		{
			if (0)
			{
				deferred_images = TRUE;
				w = h = 64.0F;
			}
			else
			{
				string localname;
	
				if (GetURL(srcname, TRUE, localname))
				{
					int quiet = (center||p_center||tabling);
					DisplayImage(quiet, GetDC(), localname, x, y, w, h);
				}
			}
		}

		if ((x + w) >= right_margin)
			x = left_margin;

		if (right_floating)
		{
			right_margin -= w;
			right_limit_y = wxMax(right_limit_y, y + h);
			Hold(new HtmlImage(this, right_margin, y, w, h, alt, srcname, dynsrc, loop, ismap, usemap, align, border));
		}
		else if (left_floating)
		{
			Hold(new HtmlImage(this, x, y, w, h, alt, srcname, dynsrc, loop, ismap, usemap, align, border));
			left_margin = x + w + default_left_margin;
			left_limit_y = wxMax(left_limit_y, y + h);
			x = left_margin;
		}
		else
		{
			left_limit_y = wxMax(left_limit_y, y + h);
			Hold(new HtmlImage(this, x, y, w, h, alt, srcname, dynsrc, loop, ismap, usemap, align, border));
			x += w;
		}

		// Now set the position for drawing text...

		if (!(left_floating||right_floating) && (align == 3))
		{
			y += h;
			y = (y - char_height);
		}
		else if (!(left_floating||right_floating) && (align == 2))
		{
			y += h / 2;
			y -= char_height / 2;
		}
		else if (!left_floating && !right_floating)
		{
		}

		addressing_max_h = wxMax(addressing_max_h, h);
		status = 1;
	}
	else if (!strncmpi(tag, "BGSOUND", 7))
	{
		string key;
		string name;
		const char* ptr = tag+7;
		string soundname;

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "SRC"))
			{
				bgsound = soundname = name;
			}
			else if (!strcmpi(key.data(), "LOOP"))
			{
				if (!strcmpi(name.data(), "INFINITE"))
					bgloop = -1;
				else
					bgloop = atoi(name.data());
			}
		}

		if (!thru_count && soundname.length())
		{
			if (soundname.length())
			{
				if (!AllreadyDeferred(soundname, x, y))
				{
					Deferral* image = new Deferral;
					image->soundname = soundname;
					image->x = x;
					image->y = y;
					image->w = 0.0F;
					image->h = 0.0F;
					deferrals_lo.Append(image);
				}
			}
		}

		status = 1;
	}
	else if (!strncmpi(tag, "HR", 2))
	{
		Release(dc);

		if (!left_floating && left_limit_y) {y = left_limit_y; left_limit_y = 0.0F; left_floating = 0;}
		if (left_floating && (y > left_limit_y)) { left_limit_y = 0.0F; left_floating = 0; }

		if (!left_floating) left_margin = default_left_margin;
		if (x != left_margin)
		{
			y += max_h;
			max_h = 0.0F;
		}

		extent_x = wxMax(extent_x, x);
		y += char_height;
		x = left_margin;

		string key;
		string name;
		const char* ptr = tag+2;
		int width = int((right_margin - left_margin) - (2 * default_left_margin));
		int height = 2;

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "SIZE"))
			{
				if (name.length())
				{
					height = atoi(name.data());
				}
			}
			if (!strcmpi(key.data(), "WIDTH"))
			{
				if (name.length())
				{
					width = atoi(name.data());

					if (name[name.length()-1] == '%')
						width = int(((double)width * (right_margin - left_margin)) / 100);
				}
			}
		}

		float x1 = left_margin + ((right_margin - width) / 2);

		if (IsCompiling())
			Hold(new HtmlMarkup(this, x, y, tag));
		else
			Hold(new HtmlHorizontalRule(this, x1, y, (float)width, (float)height));
		Release(dc);
		max_h = (float)height;
		overflow = 1;
		status = 1;
	}
	else if (!strncmpi(tag, "BASE", 4))
	{
		Hold(new HtmlMarkup(this, x, y, tag));
		string key;
		string name;
		const char* ptr = tag+4;

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "HREF"))
			{
				base = name;
			}
			else if (!strcmpi(key.data(), "TARGET"))
			{
				base_target = name;
			}
		}

		status = 1;
	}
	else if (!strncmpi(tag, "BODY", 4))
	{
		string key;
		string name;
		const char* ptr = tag+4;

		fsz = basefont;
		mode = NONE;

		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & FIXED ? wxMODERN : family),
			(mode & ITALIC ? wxITALIC : style),
			(mode & BOLD ? wxBOLD : weight),
			(mode & UNDERLINE ? TRUE : FALSE)));

		while (GetAttributeValuePair(ptr, key, name) && (dc == GetDC()))
		{
			if (!strcmpi(key.data(), "BGCOLOR"))
			{
				bgcolor = name;
			}
			else if (!strcmpi(key.data(), "TEXT"))
			{
				fgcolor = name;
			}
			else if (images && !strcmpi(key.data(), "BACKGROUND"))
			{
				bgimage = name;
			}
			else if (!strcmpi(key.data(), "LINK"))
			{
				fglink = name;
			}
			else if (!strcmpi(key.data(), "VLINK"))
			{
				fgvlink = name;
			}
			else if (!strcmpi(key.data(), "ALINK"))
			{
				fgalink = name;
			}
		}

		if (bgimage.length() && IsVerifying())
		{
			Anchor* item = new Anchor;
			item->name = bgimage;
			item->target = "_self";
			item->isname = 0;
			item->ismap = 0;
			item->x = 0.0F;
			item->y = 0.0F;
			item->w = 0.0F;
			item->h = 0.0F;
			item->object = 0;
			anchors.Push(item);
		}

		if (!IsCompiling())
			ClearBody();

		status = 1;
	}
	else if (!strcmpi(tag, "/BODY"))
	{
		Release(dc);
		overflow = 1;
		status = 1;
	}
	else if (!strncmpi(tag, "BASEFONT", 8))
	{
		Hold(new HtmlMarkup(this, x, y, tag));
		string key;
		string name;
		const char* ptr = tag+8;

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "SIZE"))
			{
				fsz = basefont = (atoi(name.data())%7)-2;
				dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
					(mode & FIXED ? wxMODERN : family),
					(mode & ITALIC ? wxITALIC : style),
					(mode & BOLD ? wxBOLD : weight),
					(mode & UNDERLINE ? TRUE : FALSE)));
			}
		}

		Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strncmpi(tag, "FONT", 4))
	{
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		string key;
		string name;
		const char* ptr = tag+4;

		SaveState();

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "COLOR"))
			{
				if (name[0] == '#')
				{
					unsigned long rgb = strtoul(name.data()+1, 0, 16);
					rgb = FindMatchingColour(rgb);
					foreground = new wxColour(char((rgb>>16)&0xFF),char((rgb>>8)&0xFF),char(rgb&0xFF));
				}
				else
				{
					foreground = HtmlColour(name.data());
				}

				SetTextForeground(foreground);
			}
			else if (!strcmpi(key.data(), "SIZE"))
			{
				fsz = basefont + atoi(name.data())%7;
			}
			else if (!strcmpi(key.data(), "FACE"))
			{
				if (!strcmpi(name.data(), "ARIAL"))
				{
					family = wxDEFAULT;
				}
				else if (!strcmpi(name.data(), "LUCIDA SANS"))
				{
					family = wxSWISS;
				}
				else if (!strcmpi(name.data(), "TIMES ROMAN") ||
							!strcmpi(name.data(), "ROMAN"))
				{
					family = wxROMAN;
				}
			}
		}

		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
					(mode & FIXED ? wxMODERN : family),
					(mode & ITALIC ? wxITALIC : style),
					(mode & BOLD ? wxBOLD : weight),
					(mode & UNDERLINE ? TRUE : FALSE)));

		status = 1;
	}
	else if (!strcmpi(tag, "/FONT"))
	{
		RestoreState();
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		status = 1;
	}
	else if (!strncmpi(tag, "META", 4))
	{
		Hold(new HtmlMarkup(this, x, y, tag));
		string key;
		string name;
		const char* ptr = tag+4;
		Bool refresh = FALSE, pragma = FALSE;
		string content = "";

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "HTTP-EQUIV"))
			{
				if (!strcmpi(name.data(), "REFRESH"))
				{
					refresh = TRUE;
				}
				else if (!strcmpi(name.data(), "PRAGMA"))
				{
					pragma = TRUE;
				}
			}
			else if (!strcmpi(key.data(), "CONTENT"))
			{
				content = name;
			}
		}

		if (refresh && content.length())
		{
			int secs;
			char buf[1024], dummy[256];

			buf[0] = 0;

			if (sscanf(content.data(), "%d;%[^=]=%s", &secs, dummy, buf) > 0)
			{
				meta_seconds = secs;

				if (!strcmpi(dummy, "url"))
				{
					const char* src = dummy;
					while (*src && (*src == ' ')) src++;
					meta_url = buf;
				}
			}
		}
		else if (pragma && !strcmpi(content.data(), "no-cache"))
		{
			string u = GetCurrentURL();
			RemoveCache(u);
		}

		status = 1;
	}
	else if (!strncmpi(tag, "MAP", 3))
	{
		Hold(new HtmlMarkup(this, x, y, tag));
		string key;
		string name;
		const char* ptr = tag+3;

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "NAME"))
			{
				Imagemap* map = new Imagemap;
				map->name = name;
				imagemaps.Append(map);
			}
		}

		mapping = 1;
		status = 1;
	}
	else if (mapping && !strncmpi(tag, "AREA", 4))
	{
		Hold(new HtmlMarkup(this, x, y, tag));
		string key;
		string name;
		const char* ptr = tag+4;
		string shape = "rect", ref, coords;

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "SHAPE"))
			{
				shape = name;
			}
			else if (!strcmpi(key.data(), "HREF"))
			{
				ref = name;
			}
			else if (!strcmpi(key.data(), "COORDS"))
			{
				coords = name;
			}
		}

		Imagemap::Area* area = new Imagemap::Area;
		area->shape = shape;
		area->name = ref;
		area->coords = coords;

		void* a;
		imagemaps.Tail(a);
		Imagemap* map = (Imagemap*)a;
		map->areas.Append(area);
		status = 1;

		if (ref.length() && IsVerifying())
		{
			Anchor* item = new Anchor;
			item->name = ref;
			item->target = "_self";
			item->isname = 0;
			item->ismap = 0;
			item->x = 0.0F;
			item->y = 0.0F;
			item->w = 0.0F;
			item->h = 0.0F;
			item->object = 0;
			anchors.Push(item);
		}
	}
	else if (mapping && !strcmpi(tag, "/MAP"))
	{
		Hold(new HtmlMarkup(this, x, y, tag));
		Release(dc);
		mapping = 0;
		status = 1;
	}
	else if (!strncmpi(tag, "MARQUEE", 7))
	{
		Release(dc);

		string key;
		string name;
		const char* ptr = tag+7;

		marquee_text = marquee_bgcolor = marquee_fgcolor = "";

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "BGCOLOR"))
			{
				marquee_bgcolor = name;
			}
			else if (!strcmpi(key.data(), "TEXT"))
			{
				marquee_fgcolor = name;
			}
		}

		float ww, hh;

		extent_x = wxMax(extent_x, x);
		x = left_margin;
		y += max_h;
		overflow = 1;
		marquee_fsz = fsz;
		marquee_mode = mode;

		dc->GetTextExtent("X", &ww, &hh);
		marquee_char_width = ww;
		marquee_x = left_margin + (2*default_left_margin);
		marquee_y = y;
		marquee_w = right_margin - left_margin - (2*default_left_margin);
		marquee_h = hh + 10;
		marquee_idx = 0;

		if (IsCompiling())
			Hold(new HtmlMarkup(this, x, y, tag));
		else
		Hold(new HtmlMarquee(this, marquee_x, marquee_y, marquee_w, marquee_h));

		y = marquee_y + marquee_h + 5;

		quiet = 1;
		status = 1;
	}
	else if (!strcmpi(tag, "/MARQUEE"))
	{
		Release(dc);
		marquee_text = buf;
		quiet = 0;
		status = 1;
	}

	return status;
}

