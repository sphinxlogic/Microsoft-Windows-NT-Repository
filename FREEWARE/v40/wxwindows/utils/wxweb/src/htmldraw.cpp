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
#include <time.h>

#include "wxhtml.h"
#include "htmlpars.h"

#ifndef wx_msw
#define RGB(r,g,b) ((long(r)<<16) | (long(g)<<8) | (long(b)))
#endif

///////////////////////////////////////////////////////////////////////////////
// Note: timer interval is a tenth of a second...
///////////////////////////////////////////////////////////////////////////////

void wxPeriodicTimer::Notify()
{
	if (canvas->setload_url.length())
	{
		string tmp_url = canvas->setload_url;
		canvas->setload_url = "";
		canvas->OpenURL(tmp_url);
		canvas->OnSetStatusText("Done");
	}

	// Time to call the marquee timer?

	if (!(++count & 1) && !canvas->outstanding)
		canvas->MarqueeNotify();

	// Time to call meta timer?

	if (!canvas->IsEdit() && ((canvas->meta_seconds != -1) || !(count % 10)))
	{
		canvas->MetaNotify();
	}

	// Draw edit window cursor?

#if ALLOW_EDIT_MODE
	if (canvas->IsEdit())
	{
		if (canvas->selected && canvas->skipped)
		{
			canvas->Select(canvas->last_x, canvas->last_y);
			canvas->skipped = FALSE;
		}

		if (!canvas->selected && !(count % 5))
			canvas->DrawCaret(canvas->caret_count++ & 1);
	}
#endif

	canvas->tick++;
}

///////////////////////////////////////////////////////////////////////////////
// Periodic timer... client-side pull of a URL on a periodic basis
///////////////////////////////////////////////////////////////////////////////

void wxHtml::MetaNotify()
{
	if (meta_seconds == -1)
		return;

	time_t now = time(0);

	if ((now - meta_last) < meta_seconds)
		return;

	meta_seconds = -1;

	string tmp_url;

	if (meta_url.length())
		tmp_url = meta_url;
	else
	{
		RemoveCache(url);
		tmp_url = url;
	}

	OpenURL(tmp_url);
}

///////////////////////////////////////////////////////////////////////////////
// Periodic timer... draw marquee if present
///////////////////////////////////////////////////////////////////////////////

void wxHtml::MarqueeNotify()
{
	if (!marquee_text.length())
		return;

	wxColour* bg;

	if (marquee_bgcolor.length() && marquee_bgcolor[0] == '#')
	{
		unsigned long c = strtoul((marquee_bgcolor.data())+1, 0, 16);
		bg = new wxColour(char((c>>16)&0xFF), char((c>>8)&0xFF), char(c&0xFF));
		SetBackground(wxTheBrushList->FindOrCreateBrush(bg, wxSOLID));
		GetDC()->SetBackgroundMode(wxSOLID);
		SetBrush(wxTheBrushList->FindOrCreateBrush(bg, wxSOLID));
		SetPen(wxThePenList->FindOrCreatePen(bg, 1, wxTRANSPARENT));
		SetTextBackground(bg);
	}
	else if (marquee_bgcolor.length())
	{
		bg = HtmlColour(marquee_bgcolor.data());
		SetBackground(wxTheBrushList->FindOrCreateBrush(bg, wxSOLID));
		GetDC()->SetBackgroundMode(wxSOLID);
		SetBrush(wxTheBrushList->FindOrCreateBrush(bg, wxSOLID));
		SetPen(wxThePenList->FindOrCreatePen(bg, 1, wxTRANSPARENT));
		SetTextBackground(bg);
	}
	else
	{
		SetBackground(wxTheBrushList->FindOrCreateBrush(background, wxSOLID));
		GetDC()->SetBackgroundMode(wxSOLID);
		SetBrush(wxTheBrushList->FindOrCreateBrush(background, wxSOLID));
		SetPen(wxThePenList->FindOrCreatePen(background, 1, wxTRANSPARENT));
		SetTextBackground(background);
	}

	DrawRectangle(marquee_x, marquee_y, marquee_w, marquee_h);

	wxColour* fg;

	if (marquee_fgcolor.length() && marquee_fgcolor[0] == '#')
	{
		unsigned long c = strtoul((marquee_fgcolor.data())+1, 0, 16);
		fg = new wxColour(char((c>>16)&0xFF), char((c>>8)&0xFF), char(c&0xFF));
		SetTextForeground(fg);
	}
	else if (marquee_fgcolor.length())
	{
		fg = HtmlColour(marquee_bgcolor.data());
		SetTextForeground(fg);
	}
	else
	{
		fg = wxTheColourDatabase->FindColour("BLACK");
		SetTextForeground(fg);
	}

	SetFont(wxTheFontList->FindOrCreateFont(marquee_fsz,
			(marquee_mode & wxHtml::FIXED ? wxMODERN : family),
			(marquee_mode & wxHtml::ITALIC ? wxITALIC : style),
			(marquee_mode & wxHtml::BOLD ? wxBOLD : weight),
			(marquee_mode & wxHtml::UNDERLINE ? TRUE : FALSE)));

	string tmp_string = string("      ")+marquee_text;
	tmp_string = tmp_string.data() + marquee_idx++;

	if (!tmp_string.length())
	{
		tmp_string = string("      ")+marquee_text;
		marquee_idx = 1;
	}

	const float INDENT = default_left_margin;
	unsigned chars = (unsigned)((marquee_w-(2*INDENT))/marquee_char_width);

	string msg(tmp_string, 0, chars);

	if (msg.length() != chars)
		msg += string(" ") + string((string("      ")+marquee_text).data(), chars - 1 - msg.length());

	DrawText((char*)msg.data(), marquee_x+INDENT, marquee_y+5);

	SetFont(wxTheFontList->FindOrCreateFont(fsz,
			(mode & wxHtml::FIXED ? wxMODERN : family),
			(mode & wxHtml::ITALIC ? wxITALIC : style),
			(mode & wxHtml::BOLD ? wxBOLD : weight),
			(mode & wxHtml::UNDERLINE ? TRUE : FALSE)));

	SetBackground(wxTheBrushList->FindOrCreateBrush(background, wxSOLID));
	GetDC()->SetBackgroundMode(wxTRANSPARENT);
	SetBrush(wxTheBrushList->FindOrCreateBrush(background, wxTRANSPARENT));
	SetTextForeground(foreground);
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::ClearBody()
{
	have_body = TRUE;

	// The text colour...

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

	SetPen(wxThePenList->FindOrCreatePen(foreground, 1, wxSOLID));
	SetTextForeground(foreground);

	// The background...

	if (bgcolor[0] == '#')
	{
		unsigned long rgb = strtoul((bgcolor.data())+1, 0, 16);
		rgb = FindMatchingColour(rgb);
		background = new wxColour(char((rgb>>16)&0xFF),char((rgb>>8)&0xFF),char((rgb)&0xFF));
	}
	else
	{
		background = HtmlColour(bgcolor.data());
	}

	SetTextBackground(background);
	SetBrush(wxTheBrushList->FindOrCreateBrush(background, wxTRANSPARENT));

	// Wallpaper?

	if (!IsEdit() && bgimage.length())
	{
		string localname = "";

		if (GetURL(bgimage, TRUE, localname))
		{
			if (WallpaperImage(GetDC(), localname, 0.0F, (float)drawing_w,
					(float)starting_y*VSCROLL_INCREMENT, (float)drawing_h))
			{
				SetBackground(wxTheBrushList->FindOrCreateBrush(background, wxTRANSPARENT));
				GetDC()->SetBackgroundMode(wxTRANSPARENT);
				wallpaper = localname;
				return;
			}
		}
	}

	// No, so clear it...

	SetBackground(wxTheBrushList->FindOrCreateBrush(background, wxSOLID));
	GetDC()->SetBackgroundMode(wxTRANSPARENT);

	Clear();

#ifdef wx_x
	// Gets around wxDC::Clear() not working properly (for me)...
	SetBrush(wxTheBrushList->FindOrCreateBrush(background, wxSOLID));
	SetPen(wxThePenList->FindOrCreatePen(background, 1, wxTRANSPARENT));
	DrawRectangle(0.0F, 0.0F, (float)drawing_w, (float)drawing_h);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// Defer item untill utlimate disposition can be determined...
///////////////////////////////////////////////////////////////////////////////

void wxHtml::Hold(HtmlObject* object)
{
	holding_line.Append(object);
	previous_object = object;
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::Release(wxDC* dc)
{
	// First we draw 'em...

	DrawTags(dc, holding_line);

	// Then we store 'em...

	if (!thru_count && holding_line.Count())
	{
		Tag* tag = new Tag;
		tag->center = center;
		tag->p_center = p_center;
		tag->y = y;
		tag->h = 0.0F;
		tag->left_margin = left_margin;
		tag->right_margin = right_margin;
		tag->left_floating = (int)left_floating;
		tag->right_floating = (int)right_floating;

		// Move from holding line to tag line...

		void* a;

		while (holding_line.Pop(a))
		{
			HtmlObject* obj = (HtmlObject*)a;
			tag->objects.Append(obj);
			tag->h = wxMax(tag->h, obj->GetHeight());
		}

		// Add line to tag list...

		tags->Append(tag);
	}
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::DropLine(ListOf& list)
{
	void* a;

	while (list.Pop(a))
		delete (HtmlObject*)a;
}

///////////////////////////////////////////////////////////////////////////////
// Draw tag items...
///////////////////////////////////////////////////////////////////////////////

void wxHtml::DrawTags(wxDC* dc, ListOf& objects)
{
	if (!objects.Count()) return;

	wxColour* save_colour = foreground;
	int save_fsz = fsz, save_family = family;
	unsigned save_mode = mode;
	float line_width = 0.0F, line_height = 0.0F, x;
	
	ListOfIterator ilist = objects;
	void* a;
	int ok;

	// If centering, first calculate the width of the items to be
	// output so we know how far to indent...

	if (center || p_center || p_right || !thru_count)
	{
		for (ok = ilist.First(a); ok; ok = ilist.Next(a))
		{
			HtmlObject* obj = (HtmlObject*)a;
			line_width += obj->GetWidth();

			if (!obj->IsImage())
				line_height = wxMax(line_height, obj->GetHeight());
		}

		if (center || p_center)
		{
			x = (right_margin - left_margin - line_width) / 2;
			x += left_margin;
		}
		else if (p_right)
			x = right_margin - line_width - default_left_margin;
		else
		{
			ilist.First(a);
			HtmlObject* obj = (HtmlObject*)a;

			if (obj->NewLine())
				x = obj->GetLeft();
			else
			{
				if (obj->IsImage())
				{
					HtmlImage* img = (HtmlImage*)obj;
					if (img->IsLeftAligned())
						x = obj->GetLeft();
					else
						x = left_margin;
				}
				else
					x = left_margin;
			}
		}
	}
	else
	{
		ilist.First(a);
		HtmlObject* obj = (HtmlObject*)a;

		if (obj->NewLine())
			x = obj->GetLeft();
		else
		{
			if (obj->IsImage())
			{
				HtmlImage* img = (HtmlImage*)obj;
				if (img->IsLeftAligned())
					x = obj->GetLeft();
				else
					x = left_margin;
			}
			else
				x = left_margin;
		}
	}

	if (x < 0) x = left_margin;

	// Now go thru the objects and draw 'em...

	for (ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		HtmlObject* obj = (HtmlObject*)a;

		obj->SetLeft(x);

		if (!thru_count && !obj->IsImage())
			obj->SetTop(obj->GetTop() + (line_height - obj->GetHeight())/2);

		if (!tabling) obj->Draw(dc);
		x += obj->GetWidth();
	}

	foreground = save_colour;
	fsz = save_fsz;
	mode = save_mode;
	family = save_family;

	if (!thru_count)
	{
		dc->SetTextForeground(foreground);

		dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
				(mode & FIXED ? wxMODERN : family),
				(mode & ITALIC ? wxITALIC : style),
				(mode & BOLD ? wxBOLD : weight),
				(mode & UNDERLINE ? TRUE : FALSE)));
	}
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::OnPaint()
{
	if (!text || in_progress || outstanding || IsCompiling())
		return;

	// Where are we?

	int prev_x = starting_x, prev_y = starting_y;
	ViewStart(&starting_x, &starting_y);
	scrolled = (prev_x != starting_x) || (prev_y != starting_y);

	// Make any FORM items invisible for the moment...

	if (scrolled) FormShow(FALSE);

	// If changing positions then find the first line that will be visible, as
	// this avoids having to draw the entire file again...

	have_body = FALSE;

	ListOfIterator ilist = *tags;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		Tag* tag = (Tag *)a;

		center = (int)tag->center;
		p_center = (int)tag->p_center;
		left_margin = tag->left_margin;
		right_margin = tag->right_margin;

		if (tag->y >= (((float)starting_y*VSCROLL_INCREMENT) + drawing_h))
			break;
		else if (tag->y >= (((float)starting_y*VSCROLL_INCREMENT) - drawing_h))
		{
			fsz = 0;
			mode = 0;
			foreground = 0;

			// Go thru this line...

			DrawTags(GetDC(), tag->objects);
		}
	}

	if (deferrals_lo.Count() || deferrals_hi.Count())
		deferral_timer.Start(10);

	draw_count++;
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::Generate(string& s, Bool do_base)
{
	s = "<HTML>\n";

	if (bgcomment.length())
	{
		s += "<!-- ";
		s += bgcomment;
		s += " -->\n";
	}

	s += "<HEAD>\n<TITLE>";
	s += bgtitle;
	s += "</TITLE>\n</HEAD>\n";

	if (do_base)
	{
		s += "<BASE href=\"";
		s += GetBaseURL();
		s += "\">\n";
	}

	if (!have_frames)
	{
		s += "<BODY";
	
		if (bgimage.length())
		{
			s += " background=";
			s += '\"';
			s += bgimage;
			s += '\"';
		}

		if (bgcolor.length())
		{
			s += " bgcolor=";
			s += '\"';
			s += bgcolor;
			s += '\"';
		}
	
		if (fgcolor.length())
		{
			s += " text=";
			s += '\"';
			s += fgcolor;
			s += '\"';
		}
	
		if (fglink.length())
		{
			s += " link=";
			s += '\"';
			s += fglink;
			s += '\"';
		}
		
		if (fgvlink.length())
		{
			s += " vlink=";
			s += '\"';
			s += fgvlink;
			s += '\"';
		}
	
		if (fgalink.length())
		{
			s += " alink=";
			s += '\"';
			s += fgalink;
			s += '\"';
		}
	
		s += ">\n";
	}

	if (bgsound.length())
	{
		s += "<BGSOUND src=";
		s += '\"';
		s += bgsound;
		s += '\"';
		s += " loop=";
		s += Number(bgloop);
		s += ">\n";
	}

	float last_y;
	Bool first_time = TRUE;

	ListOfIterator ilist = *tags;
	void* a;
	Bool was_text = FALSE;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		Tag* item = (Tag *)a;
		Bool new_line = TRUE;

		if (first_time)
		{
			first_time = FALSE;
			last_y = item->y;
		}

		ListOfIterator ilist2 = item->objects;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			HtmlObject* obj = (HtmlObject*)a2;

			if (!IsCompiling() && was_text && new_line && obj->IsText())
				s += ' ';

			obj->Output(s);
			was_text = obj->IsText();
			new_line = FALSE;
		}

		last_y = item->y;
	}

	if (!have_frames)
		s += "\n</BODY>";

	s += "\n</HTML>\n";
}

string wxHtml::Generate(Bool do_base)
{
	string s = "";
	Generate(s, do_base);
	return s;
}

///////////////////////////////////////////////////////////////////////////////

HtmlObject::HtmlObject(wxHtml* _canvas, float _x, float _y, float _w, float _h)
{
	canvas = _canvas;
	x = _x;
	y = _y;
	w = _w;
	h = _h;

#if ALLOW_EDIT_MODE
	if (canvas->IsEdit())
	{
		if (w == 0) w = canvas->char_width;
		if (h == 0) h = canvas->char_height;
	}
#endif

	fsz = (char)canvas->fsz;
	mode = (unsigned char)canvas->mode;
	family = canvas->family;
	colour = canvas->foreground;
	preformatted = (char)canvas->preformatted;
	ignore = (char)canvas->ignore;

#if ALLOW_EDIT_MODE
	selected = FALSE;
#endif
}

void HtmlObject::SetFont(wxDC* dc)
{
	dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
		(mode & wxHtml::FIXED ? wxMODERN : family),
		(mode & wxHtml::ITALIC ? wxITALIC : canvas->style),
		(mode & wxHtml::BOLD ? wxBOLD : canvas->weight),
		(mode & wxHtml::UNDERLINE ? TRUE : FALSE)));
}

///////////////////////////////////////////////////////////////////////////////

Bool HtmlObject::IsBounded(float pos_x, float pos_y, float tag_height) const
{
	if (	(pos_x >= GetLeft()) &&
			(pos_x <= GetRight()) &&
			(pos_y >= GetTop()) &&
			(pos_y <= GetTop()+tag_height)
		)
		return TRUE;
	else
		return FALSE;
}

///////////////////////////////////////////////////////////////////////////////

void HtmlObject::Draw(wxDC* dc)
{
	if (!canvas->have_body && !canvas->IsCompiling())
		canvas->ClearBody();

	if (colour != canvas->foreground)
	{
		dc->SetTextForeground(colour);
	}

	if ((fsz != canvas->fsz) ||
			(mode != canvas->mode) ||
				(family != canvas->family))
	{
		SetFont(dc);
	}

	canvas->foreground = colour;
	canvas->fsz = (int)fsz;
	canvas->mode = (unsigned)mode;
	canvas->family = family;
}

void HtmlText::Draw(wxDC* dc)
{
	if (!canvas->finished && !canvas->IsCompiling())
		HtmlObject::Draw(dc);

	if (!canvas->finished)
	{
#if ALLOW_EDIT_MODE
		if (IsSelected())
		{
			string s1(src, 0, snip_from);
			string s2(src, snip_from, snip_to - snip_from);
			string s3(src, snip_to, (src.length()-1) - snip_to);
			float _x = x, w, h;

			if (s1.length())
			{
				dc->DrawText(s1.data(), _x, y);
				dc->GetTextExtent(s1.data(), &w, &h);
				_x += w;
			}

			if (s2.length())
			{
				unsigned char r, g, b;
				canvas->background->Get(&r, &g, &b);
				wxColour* bg = new wxColour(~r, ~g, ~b);
				canvas->foreground->Get(&r, &g, &b);
				wxColour* fg = new wxColour(~r, ~g, ~b);

				dc->SetBackgroundMode(wxSOLID);
				dc->SetTextBackground(bg);
				dc->SetTextForeground(fg);

				dc->DrawText(s2.data(), _x, y);
				dc->GetTextExtent(s2.data(), &w, &h);
				_x += w;

				dc->SetTextBackground(canvas->background);
				dc->SetTextForeground(canvas->foreground);
			}

			if (s3.length())
			{
				dc->DrawText(s3.data(), _x, y);
			}
		}
		else
#endif
			dc->DrawText(src.data(), x, y);

		if (mode & wxHtml::STRIKE)
		{
			dc->SetPen(wxThePenList->FindOrCreatePen(colour, 1, wxSOLID));
			dc->DrawLine(x, y+(h*2/3), x+w, y+(h*2/3));
		}
	}
}

void HtmlAnchor::Draw(wxDC* dc)
{
	if (!canvas->finished) HtmlObject::Draw(dc);

	if (!canvas->thru_count)
	{
		wxHtml::Anchor* item = new wxHtml::Anchor;
		item->name = src;
		item->target = target;
		item->isname = isname;
		item->ismap = 0;
		item->x = x;
		item->y = y;
		item->w = 0.0F;
		item->h = 0.0F;
		item->object = this;
		canvas->anchors.Append(item);
		anchor = item;
	}
}

void HtmlDeanchor::Draw(wxDC* dc)
{
	if (!canvas->finished) HtmlObject::Draw(dc);

	if (!canvas->thru_count && canvas->anchors.Count())
	{
		void* a;
		canvas->anchors.Tail(a);
		wxHtml::Anchor* item = (wxHtml::Anchor*)a;

		float tmp_w, tmp_h;
		dc->GetTextExtent("X", &tmp_w, &tmp_h);

		item->w = GetLeft() - item->x;
		item->h = wxMax(tmp_h, max_h);
	}
}

void HtmlImage::Draw(wxDC* dc)
{
	if (!canvas->finished) HtmlObject::Draw(dc);

	if (!canvas->thru_count && canvas->anchors.Count() && canvas->addressing)
	{
		void* a;
		canvas->anchors.Tail(a);
		wxHtml::Anchor* item = (wxHtml::Anchor*)a;
		item->ismap = ismap;
		item->usemap = usemap;
		item->h = h;
	}
	else if (!canvas->thru_count && usemap.length())
	{
		wxHtml::Anchor* item = new wxHtml::Anchor;
		item->isname = 0;
		item->name = usemap;
		item->usemap = usemap;
		item->ismap = 0;
		item->x = x;
		item->y = y;
		item->w = w;
		item->h = h;
		item->object = 0;
		canvas->anchors.Append((void*)item);
	}

	if (src.length() && !canvas->IsCompiling())
	{
		if (!canvas->AllreadyDeferred(src, x, y))
		{
			wxHtml::Deferral* image = new wxHtml::Deferral;
			image->srcname = src;
			image->x = x;
			image->y = y;
			image->w = w;
			image->h = h;
			canvas->deferrals_hi.Append(image);
		}
	}

	if (!canvas->thru_count && dynsrc.length() && !canvas->IsCompiling())
	{
		if (!canvas->AllreadyDeferred(dynsrc, x, y))
		{
			wxHtml::Deferral* image = new wxHtml::Deferral;
			image->dynsrc = dynsrc;
			image->x = x;
			image->y = y;
			image->w = w;
			image->h = h;
			canvas->deferrals_lo.Append(image);
		}
	}
}

void HtmlOrderedListIndex::Draw(wxDC* dc)
{
	if (!canvas->finished) HtmlObject::Draw(dc);

	if (!canvas->finished)
	{
		dc->DrawText(src.data(), x, y);
	}
}

void HtmlUnorderedListIndex::Draw(wxDC* dc)
{
	if (!canvas->finished) HtmlObject::Draw(dc);

	if (!canvas->finished)
	{
		dc->SetPen(wxThePenList->FindOrCreatePen("RED", 1, wxSOLID));
		dc->DrawEllipse(x, y, 4.0F, 4.0F);
		dc->SetPen(wxThePenList->FindOrCreatePen(colour, 1, wxSOLID));
	}
}

void HtmlHorizontalRule::Draw(wxDC* dc)
{
	if (!canvas->finished) HtmlObject::Draw(dc);

	if (!canvas->finished)
	{
		dc->SetPen(wxThePenList->FindOrCreatePen("DARK GREY", 1, wxSOLID));
		dc->DrawLine(x, y, x+w, y);
		dc->DrawLine(x, y, x, y+3);
		dc->SetPen(wxThePenList->FindOrCreatePen(new wxColour(0xF0, 0xF0, 0xF0), (int)h-1, wxSOLID));
		dc->DrawLine(x+1, y+2, x+w, y+2);
		dc->DrawLine(x+w, y+2, x+w, y-1);
		dc->SetPen(wxThePenList->FindOrCreatePen(colour, 1, wxSOLID));
	}
}

void HtmlBreak::Draw(wxDC* dc)
{
	if (!canvas->finished) HtmlObject::Draw(dc);

#if ALLOW_EDIT_MODE
	if (!canvas->finished && canvas->IsEdit())
	{
		if (IsSelected())
		{
			unsigned char r, g, b;
			canvas->background->Get(&r, &g, &b);
			wxColour* bg = new wxColour(~r, ~g, ~b);
			canvas->foreground->Get(&r, &g, &b);
			wxColour* fg = new wxColour(~r, ~g, ~b);

			dc->SetBackgroundMode(wxSOLID);
			dc->SetTextBackground(bg);
			dc->SetTextForeground(fg);
		}
		else
			dc->SetTextForeground(wxTheColourDatabase->FindColour("BROWN"));

		string c;
		c = (char)172;
		dc->DrawText(c.data(), x, y);

		if (IsSelected())
			dc->SetTextBackground(canvas->background);

		dc->SetTextForeground(colour);
	}
#endif
}

void HtmlParagraph::Draw(wxDC* dc)
{
	if (!canvas->finished) HtmlObject::Draw(dc);

#if ALLOW_EDIT_MODE
	if (!canvas->finished && canvas->IsEdit())
	{
		if (IsSelected())
		{
			unsigned char r, g, b;
			canvas->background->Get(&r, &g, &b);
			wxColour* bg = new wxColour(~r, ~g, ~b);
			canvas->foreground->Get(&r, &g, &b);
			wxColour* fg = new wxColour(~r, ~g, ~b);

			dc->SetBackgroundMode(wxSOLID);
			dc->SetTextBackground(bg);
			dc->SetTextForeground(fg);
		}
		else
			dc->SetTextForeground(wxTheColourDatabase->FindColour("BROWN"));

		string c;
		c = (char)182;
		dc->DrawText(c.data(), x, y);

		if (IsSelected())
			dc->SetTextBackground(canvas->background);

		dc->SetTextForeground(colour);
	}
#endif
}

void HtmlEndParagraph::Draw(wxDC* dc)
{
	if (!canvas->finished) HtmlObject::Draw(dc);

	if (!canvas->finished && canvas->IsEdit())
	{
		dc->SetTextForeground(wxTheColourDatabase->FindColour("BROWN"));
		string c;
		c = (unsigned char)182;
		dc->DrawText(c.data(), x, y);
		dc->SetTextForeground(colour);
	}
}

void HtmlFormItem::Draw(wxDC* dc)
{
	if (!canvas->finished) HtmlObject::Draw(dc);

	if (!canvas->finished && canvas->scrolled && !IsHidden())
	{
		field->SetSize(_ABSOLUTE_X(x), _ABSOLUTE_Y(y), (int)w, (int)h);
		field->Show(TRUE);
	}
}

void HtmlTable::Draw(wxDC* dc)
{
	if (!canvas->finished) HtmlObject::Draw(dc);

	if (!canvas->finished)
	{
		canvas->TableDraw(dc, table, x, y);
	}
}

///////////////////////////////////////////////////////////////////////////////

void HtmlObject::Format(string& text) const
{
	if (colour != canvas->foreground)
	{
		text += "<FONT color=";
		unsigned char r, g, b;
		colour->Get(&r, &g, &b);

		if ((r | g | b) == 0)
			text += "\"BLACK\"";
		else if ((r | g | b) == 255)
			text += "\"WHITE\"";
		else if ((r == 255) && ((g | b) == 0))
			text += "\"RED\"";
		else if ((g == 255) && ((r | b) == 0))
			text += "\"GREEN\"";
		else if ((b == 255) && ((r | g) == 0))
			text += "\"BLUE\"";
		else
		{
			char buf[20];
			sprintf(buf, "#%lX", RGB(r,g,b));
			text += buf;
		}

		text += ">";
	}

	if (family != canvas->family)
	{
		if (family == wxSWISS)
			text += "<FONT face=\"Lucida Sans\">";
		else if (family == wxROMAN)
			text += "<FONT face=\"Times Roman\">";
		else if (family == wxDEFAULT)
			text += "<FONT face=\"Arial\">";
	}

	if (fsz != canvas->fsz)
	{
		int diff = fsz - canvas->fsz;
		text += "<FONT size=";
		text += Number(diff);
		text += ">";
	}

	if (mode != canvas->mode)
	{
		if ((canvas->mode & wxHtml::FIXED) && !(mode & wxHtml::FIXED))
			text += "</TT>";
		if ((canvas->mode & wxHtml::STRIKE) && !(mode & wxHtml::STRIKE))
			text += "</S>";
		if ((canvas->mode & wxHtml::UNDERLINE) && !(mode & wxHtml::UNDERLINE))
			text += "</U>";
		if ((canvas->mode & wxHtml::ITALIC) && !(mode & wxHtml::ITALIC))
			text += "</I>";
		if ((canvas->mode & wxHtml::BOLD) && !(mode & wxHtml::BOLD))
			text += "</B>";

		if (!(canvas->mode & wxHtml::BOLD) && (mode & wxHtml::BOLD))
			text += "<B>";
		if (!(canvas->mode & wxHtml::ITALIC) && (mode & wxHtml::ITALIC))
			text += "<I>";
		if (!(canvas->mode & wxHtml::UNDERLINE) && (mode & wxHtml::UNDERLINE))
			text += "<U>";
		if (!(canvas->mode & wxHtml::STRIKE) && (mode & wxHtml::STRIKE))
			text += "<S>";
		if (!(canvas->mode & wxHtml::FIXED) && (mode & wxHtml::FIXED))
			text += "<TT>";
	}

	if (ignore != canvas->ignore)
	{
		if (ignore)
			text += "<PLAINTEXT>";
		else
			text += "</PLAINTEXT>";
	}

	if (preformatted != canvas->preformatted)
	{
		if (preformatted)
			text += "<PRE>";
		else
			text += "</PRE>";
	}

	HtmlObject::Output(text);
}

///////////////////////////////////////////////////////////////////////////////

void HtmlObject::Output(string&) const
{
	canvas->foreground = colour;
	canvas->fsz = fsz;
	canvas->family = family;
	canvas->mode = mode;
	canvas->ignore = ignore;
	canvas->preformatted = preformatted;
}

void HtmlMarkup::Output(string& text) const
{
	HtmlObject::Output(text);
	text += "<";
	text += src;
	text += ">";
}

void HtmlText::Output(string& text) const
{
	if (!canvas->IsCompiling()) Format(text);

	if (!src.length()) return;

	char* buf = new char[src.length()*6+1];
	char* d = buf;
	const char* s = src.data();

	while (*s)
	{
		if (*s == '<')
		{
			*d++ = '&';
			*d++ = 'l';
			*d++ = 't';
			*d++ = ';';
		}
		else if (*s == '>')
		{
			*d++ = '&';
			*d++ = 'g';
			*d++ = 't';
			*d++ = ';';
		}
		else if (*s == '&')
		{
			*d++ = '&';
			*d++ = 'a';
			*d++ = 'm';
			*d++ = 'p';
			*d++ = ';';
		}
		else if (*s == '"')
		{
			*d++ = '&';
			*d++ = 'q';
			*d++ = 'u';
			*d++ = 'o';
			*d++ = 't';
			*d++ = ';';
		}
		else if (*s == (unsigned char)160)
		{
			*d++ = '&';
			*d++ = 'n';
			*d++ = 'b';
			*d++ = 's';
			*d++ = 'p';
			*d++ = ';';
		}
		else
			*d++ = *s;

		s++;
	}

	*d = 0;
	text += buf;
	delete [] buf;
}

void HtmlMarquee::Output(string& text) const
{
	HtmlObject::Output(text);
	text += "\n<MARQUEE";

	if (canvas->marquee_bgcolor.length())
	{
		text += " bgcolor=";
		text += '\"';
		text += canvas->marquee_bgcolor;
		text += '\"';
	}

	if (canvas->marquee_fgcolor.length())
	{
		text += " fgcolor=";
		text += '\"';
		text += canvas->marquee_fgcolor;
		text += '\"';
	}

	text += ">";
	text += canvas->marquee_text;
	text += "</MARQUEE>\n";
}

void HtmlAnchor::Output(string& text) const
{
	HtmlObject::Output(text);
	text += "<A ";
	if (target.length()) text += "target=" + target + " ";
	text += (isname ? "name=" : "href=");
	text += '\"';
	text += src;
	text += '\"';
	text += ">";
}

void HtmlDeanchor::Output(string& text) const
{
	text += "</A>";
	HtmlObject::Output(text);
}

void HtmlImage::Output(string& text) const
{
	HtmlObject::Output(text);
	text += "<IMG src=";
	text += '\"';
	text += src;
	text += '\"';

	if ((GetWidth() > 0) && (GetHeight() > 0))
	{
		text += " width=";
		text += Number(GetWidth());
		text += " height=";
		text += Number(GetHeight());
	}

	if (border >= 0)
	{
		text += " border=";
		text += Number(border);
	}

	if (usemap.length())
	{
		text += " usemap=";
		text += '\"';
		text += usemap;
		text += '\"';
	}

	if (ismap)
	{
		text += " ismap";
	}

	if (dynsrc.length())
	{
		text += " dynsrc=";
		text += '\"';
		text += dynsrc;
		text += '\"';
		text += " loop=";
		text += Number(loop);
	}

	text += " align=";
	text += (align == 1 ? "top" : align == 2 ? "middle" :
				align == 3 ? "bottom" : align == 4 ? "left" :
				align == 5 ? "right" : "none");

	text += " alt=";
	text += '\"';
	text += alt;
	text += '\"';
	text += ">";
}

void HtmlList::Output(string& text) const
{
	HtmlObject::Output(text);

	text += "\n";

	switch (type)
	{
	case 1:
		text += "<OL>";
		break;
	case 2:
		text += "<UL>";
		break;
	case 3:
		text += "<MENU>";
		break;
	case 4:
		text += "<DIR>";
		break;
	case 5:
		text += "<DL>";
		break;
	}
}

void HtmlEndList::Output(string& text) const
{
	text += "\n";

	switch (type)
	{
	case 1:
		text += "</OL>";
		break;
	case 2:
		text += "</UL>";
		break;
	case 3:
		text += "</MENU>";
		break;
	case 4:
		text += "</DIR>";
		break;
	case 5:
		text += "</DL>";
		break;
	}

	text += "\n";
	HtmlObject::Output(text);
}

void HtmlListIndex::Output(string& text) const
{
	HtmlObject::Output(text);
	text += "\n<LI>";
}

void HtmlOrderedListIndex::Output(string& text) const
{
	HtmlObject::Output(text);
	text += "\n<LI>";
}

void HtmlUnorderedListIndex::Output(string& text) const
{
	HtmlObject::Output(text);
	text += "\n<LI>";
}

void HtmlHorizontalRule::Output(string& text) const
{
	HtmlObject::Output(text);
	text += "\n<HR size=";
	text += Number(h);
	text += ">\n";
}

void HtmlBreak::Output(string& text) const
{
	HtmlObject::Output(text);
	text += "<BR>\n";
}

void HtmlParagraph::Output(string& text) const
{
	HtmlObject::Output(text);
	text += "\n<P";
	if (center) text += " align=center";
	if (right) text += " align=right";
	text += ">\n";
}

void HtmlEndParagraph::Output(string& text) const
{
	HtmlObject::Output(text);
	text += "\n</P>\n";
}

void HtmlHeading::Output(string& text) const
{
	HtmlObject::Output(text);
	text += "\n<H";
	text += Number(level);

	if (align.length())
		text += " align=" + align;

	text += ">";
}

void HtmlEndHeading::Output(string& text) const
{
	HtmlObject::Output(text);
	text += "</H";
	text += Number(level);
	text += ">\n";
}

void HtmlFormItem::Output(string& text) const
{
	HtmlObject::Output(text);
}

void HtmlTable::Output(string& text) const
{
	HtmlObject::Output(text);
	canvas->TableOutput(table, text);
}

void HtmlEndTable::Output(string& text) const
{
	HtmlObject::Output(text);
}

void HtmlTableRow::Output(string& text) const
{
	HtmlObject::Output(text);
}

void HtmlEndTableRow::Output(string& text) const
{
	HtmlObject::Output(text);
}

void HtmlTableCell::Output(string& text) const
{
	HtmlObject::Output(text);
}

void HtmlEndTableCell::Output(string& text) const
{
	HtmlObject::Output(text);
}









