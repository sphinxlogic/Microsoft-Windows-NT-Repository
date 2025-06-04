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

#include "wxhtml.h"
#include "htmlpars.h"

struct FrameItem
{
	int x, y, w, h, max_value, wild_card_value;
	Bool cols_specified;
	double adjuster;
	const char* list;
	string save_list;
};
static ListOf frame_list;

static int cur_x, cur_y, cur_w, cur_h;

///////////////////////////////////////////////////////////////////////////////

static int ParseList(const char*& src, string& item)
{
	item = "";

	while (*src && (*src == ' '))
		src++;

	if (!*src) return 0;

	while (*src && (*src != ',') && (*src != ' '))
		item += *src++;

	while (*src && (*src == ' '))
		src++;

	if (*src == ',')
		src++;

	return 1;
}

///////////////////////////////////////////////////////////////////////////////

static void GetNextValueFromList()
{
	if (!frame_list.Count()) return;

	void* a;
	frame_list.Tail(a);
	FrameItem* item = (FrameItem*)a;

	cur_x = item->x;
	cur_y = item->y;
	cur_w = item->w;
	cur_h = item->h;

	if (item->cols_specified)
	{
		string tmp;
		ParseList(item->list, tmp);
		int value;

		if (tmp.length() && tmp[tmp.length()-1] == '%')
			value = (int)(item->adjuster * atol(tmp.data()) * item->max_value) / 100;
		else if (tmp.length() && tmp[tmp.length()-1] != '*')
			value = atoi(tmp.data());
		else if (tmp.length() && tmp[tmp.length()-1] == '*')
			value = (atoi(tmp.data())?atoi(tmp.data()):1) * item->wild_card_value;
		else
			value = 0;

		item->x = cur_x + value;
		item->y = cur_y;
		item->w = cur_w - value;
		item->h = cur_h;

		cur_w = value;
	}
	else
	{
		string tmp;
		ParseList(item->list, tmp);
		int value;

		if (tmp.length() && tmp[tmp.length()-1] == '%')
			value = (int)(item->adjuster * atol(tmp.data()) * item->max_value) / 100;
		else if (tmp.length() && tmp[tmp.length()-1] != '*')
			value = atoi(tmp.data());
		else if (tmp.length() && tmp[tmp.length()-1] == '*')
			value = (atoi(tmp.data())?atoi(tmp.data()):1) * item->wild_card_value;
		else
			value = 0;

		item->x = cur_x;
		item->y = cur_y + value;
		item->w = cur_w;
		item->h = cur_h - value;

		cur_h = value;
	}
}

///////////////////////////////////////////////////////////////////////////////

static int GetWildCardValue(int max_value, const char* src, double& adjuster)
{
	string tmp;
	int measured_value = 0;
	int left_overs = 0;

	while (ParseList(src, tmp))
	{
		if (tmp[tmp.length()-1] == '%')
			measured_value += (int)(atol(tmp.data()) * max_value) / 100;
		else if (tmp[tmp.length()-1] != '*')
			measured_value += atoi(tmp.data());
		else if (tmp[tmp.length()-1] == '*')
			left_overs += (atoi(tmp.data()) ? atoi(tmp.data()) : 1);
	}

	adjuster = (measured_value > max_value? (double)max_value / measured_value : 1.);

	int n = (max_value > measured_value ? max_value - measured_value : 0);

	return (left_overs ? n / left_overs : n);
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::ParseFrame(wxDC* /*dc*/, char*&, char*, char* tag)
{
	if (!frames)
		return 0;

#if USE_HTML_FRAMES
	if (!strncmpi(tag, "FRAMESET", 8))
	{
		Hold(new HtmlMarkup(this, x, y, tag));
		string key;
		string name;
		const char* ptr = tag+8;

		have_body = TRUE;

		if (!framing++)
		{
			cur_x = 0;
			cur_y = 0;
			cur_w = drawing_w;
			cur_h = drawing_h;
		}

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "ROWS"))
			{
				double adjuster;

				int wild_card_value = GetWildCardValue(cur_h, name.data(), adjuster);

				FrameItem* fitem = new FrameItem;
				fitem->cols_specified = FALSE;
				fitem->save_list = name;
				fitem->list = fitem->save_list.data();

				string tmp;
				ParseList(fitem->list, tmp);
				int value;

				if (tmp[tmp.length()-1] == '%')
					value = (int)atol(tmp.data()) * cur_h / 100;
				else if (tmp[tmp.length()-1] != '*')
					value = atoi(tmp.data());
				else if (tmp[tmp.length()-1] == '*')
					value = (atoi(tmp.data())?atoi(tmp.data()):1) * wild_card_value;
				else
					value = 0;

				value = int(adjuster * value);

				fitem->x = cur_x;
				fitem->y = cur_y + value;
				fitem->w = cur_w;
				fitem->h = cur_h - value;
				fitem->max_value = cur_h;
				fitem->wild_card_value = wild_card_value;
				fitem->adjuster = adjuster;
				frame_list.Append(fitem);

				cur_h = value;
			}
			else if (!strcmpi(key.data(), "COLS"))
			{
				double adjuster;

				int wild_card_value = GetWildCardValue(cur_w, name.data(), adjuster);

				FrameItem* fitem = new FrameItem;
				fitem->cols_specified = TRUE;
				fitem->save_list = name;
				fitem->list = fitem->save_list.data();

				string tmp;
				ParseList(fitem->list, tmp);
				int value;

				if (tmp[tmp.length()-1] == '%')
					value = (int)atol(tmp.data()) * cur_w / 100;
				else if (tmp[tmp.length()-1] != '*')
					value = atoi(tmp.data());
				else if (tmp[tmp.length()-1] == '*')
					value = (atoi(tmp.data())?atoi(tmp.data()):1) * wild_card_value;
				else
					value = 0;

				value = int(adjuster * value); 

				fitem->x = cur_x + value;
				fitem->y = cur_y;
				fitem->w = cur_w - value;
				fitem->h = cur_h;
				fitem->max_value = cur_w;
				fitem->wild_card_value = wild_card_value;
				fitem->adjuster = adjuster;
				frame_list.Append(fitem);

				cur_w = value;
			}
		}

		have_frames = TRUE;
	}
	else if (framing && !strncmpi(tag, "FRAME", 5))
	{
		Hold(new HtmlMarkup(this, x, y, tag));
		string key;
		string name;
		const char* ptr = tag+5;
		string frame_src = "", frame_name = "";
		int frame_margin_width = 0, frame_margin_height = 0;
		Bool noresize = FALSE;

		enum { SCROLLING_YES, SCROLLING_NO, SCROLLING_AUTO }
					scrolling = SCROLLING_AUTO;

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "SRC"))
			{
				frame_src = name;
			}
			else if (!strcmpi(key.data(), "NAME"))
			{
				frame_name = name;
			}
			else if (!strcmpi(key.data(), "MARGINWIDTH"))
			{
				frame_margin_width = atoi(name.data());
			}
			else if (!strcmpi(key.data(), "MARGINHEIGHT"))
			{
				frame_margin_height = atoi(name.data());
			}
			else if (!strcmpi(key.data(), "SCROLLING"))
			{
				if (!strcmpi(name.data(), "yes"))
					scrolling = SCROLLING_YES;
				else if (!strcmpi(name.data(), "no"))
					scrolling = SCROLLING_NO;
				else if (!strcmpi(name.data(), "auto"))
					scrolling = SCROLLING_AUTO;
			}
			else if (!strcmpi(key.data(), "NORESIZE"))
			{
				noresize = TRUE;
			}
		}

		long style = (noresize ? wxBORDER : wxTHICK_FRAME);
		wxHtml* canvas = Clone(cur_x, cur_y, cur_w, cur_h, style);
		canvas->frame_name = frame_name;
		canvas->base = base;
		canvas->url = frame_src;
		canvas->SetFrames(TRUE);
		sub_frames.Append(canvas);

		if (frame_src.length() && !IsCompiling())
		{
			canvas->DrawText((char*)frame_src.data(), 10.0F, 10.0F);
			//canvas->MapURL(AbsoluteURL(frame_src), base);
		}

		GetNextValueFromList();
	}
	else if (framing && !strcmpi(tag, "/FRAMESET"))
	{
		Hold(new HtmlMarkup(this, x, y, tag));

		void* a;

		if (frame_list.Remove(a))
			delete (FrameItem*)a;

		GetNextValueFromList();

		// Load frame contents...

		if (!--framing)
		{
			Show(FALSE);

			ListOfIterator ilist = sub_frames;
			void* a;

			for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
			{
				wxHtml* canvas = (wxHtml*)a;
				canvas->Reload(FALSE);
			}
		}
	}
	else
		return 0;

	return 1;
#else
	return 0;
#endif
}

///////////////////////////////////////////////////////////////////////////////

wxHtml* wxHtml::FindSubFrame(const string& frame_name)
{
	wxHtml* parent;

	if (GetRealParent())
		parent = GetRealParent();
	else
		parent = this;

	ListOfIterator ilist = parent->sub_frames;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxHtml* canvas = (wxHtml*)a;

		if (canvas->frame_name == frame_name)
			return canvas;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////

wxHtml* wxHtml::FindTopFrame()
{
	wxHtml* tmp = this;
	wxHtml* top = this;

	while (tmp)
	{
		tmp = tmp->GetRealParent();

		if (!tmp) break;

		top = tmp;
	}

	return top;
}

///////////////////////////////////////////////////////////////////////////////

wxHtml* wxHtml::WhichFrame(string& target)
{
	if (!target.length())
		target = base_target;

	if (!target.length())
		return this;

	// Is it me?

	if (target == "_self")
		return this;

	wxHtml* frame;

	// Maybe it's a sub-frame?

	if ((frame = FindSubFrame(target)) != 0)
	{
		frame->base = base;
		return frame;
	}

	// Ok, find my parent...

	wxHtml* parent;

	if (GetRealParent())
		parent = GetRealParent();
	else
		parent = this;              // No parent!

	// Find the very top frame of 'em all...

	wxHtml* top = FindTopFrame();

	if (target == "_parent")
		frame = parent;
	else if (target == "_top")
		frame = top;
	else if (target == "_blank")
		frame = top;					// This'll do for now
	else
	{
		//OnSetStatusText((char*)(string("No target: ")+target).data());
		frame = top;
	}

	frame->base = base;
	return frame;
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::DeleteSubFrames()
{
	void* a;

	while (sub_frames.Pop(a))
		delete (wxHtml*)a;
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::DeferDeleteSubFrames()
{
	if (sub_frames.Count())
		Show(TRUE);

	wxHtml* top = FindTopFrame();

	void* a;

	while (sub_frames.Pop(a))
	{
		top->delete_list.Append(a);
		((wxWindow*)a)->Show(FALSE);
	}
}


