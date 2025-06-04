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
#ifdef wx_x
#include <wx_stat.h>
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>

#include "wxhtml.h"
#include "htmlpars.h"
#include "htmledit.h"

#if ALLOW_EDIT_MODE

#define CLOSEST(x, lo, hi) ( ((x)-(lo)) <= ((hi)-(x)) ? (lo) : (hi) )
#define VERTICAL_ALIGNMENT(canvas) (canvas->char_height/2)

static ListOf clipboard;

///////////////////////////////////////////////////////////////////////////////

void wxHtml::PopUpFunction2(wxMenu& /*menu*/, wxCommandEvent& event)
{
	wxHtml* canvas = last_canvas;
	int id = event.commandInt;

	switch (id)
	{
	case HTML_PROPERTIES:

		if (canvas->edit_object)
			canvas->edit_object->Properties();
		else
			new BodyAttributes(canvas);

		break;

	case HTML_CUT:

		if (canvas->selected_objects)
			canvas->Cut();

		break;

	case HTML_COPY:

		if (canvas->selected_objects)
			canvas->Copy();

		break;

	case HTML_PASTE:

		if (canvas->edit_object && clipboard.Count())
			canvas->Paste();

		break;

	case HTML_BOLD:

		if (canvas->selected_objects)
			canvas->Mode(wxHtml::BOLD);

		break;

	case HTML_ITALIC:

		if (canvas->selected_objects)
			canvas->Mode(wxHtml::ITALIC);

		break;

	case HTML_UNDERLINE:

		if (canvas->selected_objects)
			canvas->Mode(wxHtml::UNDERLINE);

		break;

	case HTML_STRIKE:

		if (canvas->selected_objects)
			canvas->Mode(wxHtml::STRIKE);

		break;

	case HTML_FIXED:

		if (canvas->selected_objects)
			canvas->Mode(wxHtml::FIXED);

		break;

	case HTML_PLUS:

		if (canvas->selected_objects)
			canvas->Size(+1);

		break;

	case HTML_MINUS:

		if (canvas->selected_objects)
			canvas->Size(-1);

		break;

	}
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::SetEdit(Bool state)
{
	if (state)
	{
		SetWordBreak(TRUE);
		Reload();
		edit_object = 0;
		selected = FALSE;
		selected_x = 0.0F;
		selected_y = 0.0F;
		current_x = 0.0F;
		current_y = 0.0F;
		last_tick = 0;
		caret_count = 0;
		SetDirty(FALSE);
	}
	else
	{
		SetCursor(new wxCursor(wxCURSOR_ARROW));
		Deselect();
		SetWordBreak(FALSE);

		if (IsDirty())
		{
			if (wxMessageBox("Discard changes?", "Edit mode", wxYES_NO|wxICON_QUESTION) == wxYES)
				Reload();
		}

		SetDirty(FALSE);
	}

	SetUserEditMode(state);
	edit = state;
	Refresh();
}

///////////////////////////////////////////////////////////////////////////////

Bool wxHtml::DeleteHtmlObject(HtmlObject* object)
{
	if (!edit_tag) return FALSE;

	ListOfIterator ilist = edit_tag->objects;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		HtmlObject* line_object = (HtmlObject*)a;

		if (line_object == object)
		{
			delete object;
			ilist.Delete();
			return TRUE;
		}
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::MoveRight()
{
	float pos_x = current_x, pos_y = current_y;
	void* a;

	if (edit_objects.Next(a))
	{
		HtmlObject* obj = (HtmlObject*)a;
		pos_x = obj->GetLeft() + 1;
		edit_object = obj;
	}
	else if (edit_tags.Next(a))
	{
		Tag* tag = (Tag*)a;
		void* aa;
		if (tag->objects.Head(aa))
		{
			HtmlObject* obj = (HtmlObject*)aa;
			pos_x = obj->GetLeft() + 1;
			pos_y = obj->GetTop() + VERTICAL_ALIGNMENT(this);
			edit_object = obj;
		}
	}

	MoveCaret(pos_x, pos_y);
}

void wxHtml::MoveLeft()
{
	float pos_x = current_x, pos_y = current_y;
	void* a;

	if (edit_objects.Previous(a))
	{
		HtmlObject* obj = (HtmlObject*)a;
		pos_x = obj->GetLeft() + obj->GetWidth() - 1;
		edit_object = obj;
	}
	else if (edit_tags.Previous(a))
	{
		Tag* tag = (Tag*)a;
		void* aa;
		if (tag->objects.Tail(aa))
		{
			HtmlObject* obj = (HtmlObject*)aa;
			pos_x = obj->GetLeft() + obj->GetWidth() - 1;
			pos_y = tag->y + VERTICAL_ALIGNMENT(this);
			edit_object = obj;
		}
	}

	MoveCaret(pos_x, pos_y);
}

void wxHtml::MoveUp()
{
	float pos_x = current_x, pos_y = current_y;
	void* a;

loop:

	if (!edit_tags.Previous(a))
		return;

	Tag* tag = (Tag*)a;
	if (!tag->objects.Count()) goto loop;
	edit_objects = tag->objects;

	void* aa;
	edit_objects.First(aa);
	Bool found = FALSE;

	do
	{
		HtmlObject* obj = (HtmlObject*)aa;

		if (obj->IsBounded(pos_x, obj->GetTop(), tag->h))
		{
			found = TRUE;
			break;
		}
	}
	  while (edit_objects.Next(aa));

	if (!found) edit_objects.Last(aa);

	HtmlObject* obj = (HtmlObject*)aa;
	pos_y = obj->GetTop() + VERTICAL_ALIGNMENT(this);
	if (!found) pos_x = obj->GetLeft() + obj->GetWidth() - 1;

	MoveCaret(pos_x, pos_y);

	if (starting_y && (current_y < (starting_y*VSCROLL_INCREMENT)))
		Scroll(-1, starting_y - 1);

	edit_object = obj;
}

void wxHtml::MoveDown()
{
	float pos_x = current_x, pos_y = current_y;
	void* a;

loop:

	if (!edit_tags.Next(a))
		return;

	Tag* tag = (Tag*)a;
	if (!tag->objects.Count()) goto loop;
	edit_objects = tag->objects;

	void* aa;
	edit_objects.First(aa);
	Bool found = FALSE;

	do
	{
		HtmlObject* obj = (HtmlObject*)aa;

		if (obj->IsBounded(pos_x, obj->GetTop(), tag->h))
		{
			found = TRUE;
			break;
		}
	}
	  while (edit_objects.Next(aa));

	if (!found) edit_objects.Last(aa);

	HtmlObject* obj = (HtmlObject*)aa;
	pos_y = obj->GetTop() + VERTICAL_ALIGNMENT(this);
	if (!found) pos_x = obj->GetLeft() + obj->GetWidth() - 1;

	MoveCaret(pos_x, pos_y);

	if (current_y > (starting_y*VSCROLL_INCREMENT + drawing_h))
		Scroll(-1, starting_y + 1);

	edit_object = obj;
}

void wxHtml::MovePageUp()
{
	for (int i = 0; i < 10; i++)
		MoveUp();

	if (starting_y && (current_y < (starting_y*VSCROLL_INCREMENT)))
		Scroll(-1, starting_y - 1);
}

void wxHtml::MovePageDown()
{
	for (int i = 0; i < 10; i++)
		MoveDown();

	if (current_y > (starting_y*VSCROLL_INCREMENT + drawing_h))
		Scroll(-1, starting_y + 1);
}

void wxHtml::MoveHome()
{
	float pos_x = current_x, pos_y = current_y;
	void* a;

	if (!edit_tags.First(a))
		return;

	Tag* tag = (Tag*)a;
	edit_objects = tag->objects;

	void* aa;

	if (!edit_objects.First(aa))
		return;

	HtmlObject* obj = (HtmlObject*)aa;
	pos_y = obj->GetTop() + VERTICAL_ALIGNMENT(this);
	pos_x = obj->GetLeft() + 1;
	MoveCaret(pos_x, pos_y);

	if (starting_y && (current_y < (starting_y*VSCROLL_INCREMENT)))
		Scroll(0, 0);

	edit_object = obj;
}

void wxHtml::MoveEnd()
{
	float pos_x = current_x, pos_y = current_y;
	void* a;

	if (!edit_tags.Last(a))
		return;

	Tag* tag = (Tag*)a;
	edit_objects = tag->objects;

	void* aa;

	if (!edit_objects.Last(aa))
		return;

	HtmlObject* obj = (HtmlObject*)aa;
	pos_y = obj->GetTop() + VERTICAL_ALIGNMENT(this);
	pos_x = obj->GetLeft() + obj->GetWidth() - 1;
	MoveCaret(pos_x, pos_y);

	if (current_y > (starting_y*VSCROLL_INCREMENT + drawing_h))
		Scroll(-1, int((extent_y-drawing_h)/VSCROLL_INCREMENT+1));

	edit_object = obj;
}

void wxHtml::Delete()
{
	if (!edit_object) return;

	if (selected_objects)
	{
		Cut();
	}
	else if (edit_objects.PeekNext())
	{
		SetClippingRegion(edit_tag->left_margin, edit_object->GetTop(), edit_tag->right_margin - edit_tag->left_margin, edit_object->GetHeight());
		Clear();
		edit_objects.Delete();
		delete edit_object;
		DrawTags(GetDC(), edit_objects);
		DestroyClippingRegion();
		edit_object = 0;
		edit_tag = 0;
		SetDirty();
	}
	else
		MergeRight();
}

void wxHtml::Backspace()
{
	if (edit_objects.PeekPrevious())
	{
		MoveLeft();
		Delete();
	}
	else
		MergeLeft();
}

void wxHtml::MergeRight()
{
	void* a;

	if (!edit_tags.Next(a))
		return;

	Tag* tag = (Tag*)a;

	float delta = edit_tag->y - tag->y;
	Adjust(delta, edit_tags);

	(ListOf&)edit_objects += tag->objects;
	edit_tags.Delete();
	delete tag;
	Refresh();
	SetDirty();
}

void wxHtml::MergeLeft()
{
	void* a;

	if (!edit_tags.PeekPrevious(a))
		return;

	Tag* tag = (Tag*)a;

	float delta = tag->y - edit_tag->y;
	Adjust(delta, edit_tags);

	(ListOf&)tag->objects += edit_objects;
	edit_tags.Delete();
	delete edit_tag;
	SetClippingRegion(tag->left_margin, tag->y, tag->right_margin - tag->left_margin, (float)drawing_h);
	Clear();
	OnPaint();
	DestroyClippingRegion();
	MoveCaret(edit_object->GetLeft() + 1, tag->y+VERTICAL_ALIGNMENT(this));
	SetDirty();
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::Adjust(float delta, ListOfIterator ilist)
{
	void* a1;
	if (!ilist.Current(a1)) return;

	// Adjust following lines up or down as appropriate...

	do
	{
		Tag* tag = (Tag*)a1;

		tag->y += delta;

		ListOfIterator ilist2 = tag->objects;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			HtmlObject* obj = (HtmlObject*)a2;
			obj->SetTop(obj->GetTop() + delta);
		}
	}
	  while (ilist.Next(a1));
}

///////////////////////////////////////////////////////////////////////////////

size_t HtmlText::Index(float& pos_x, float& /*y*/)
{
	// Select the font in...

	HtmlObject::Draw(canvas->GetDC());

	// Find the first sub-string that will position the cursor
	// at the beginning or end of a letter...

	float last_w = 0.0F;
	size_t i;

	for (i = 0; i < src.length(); i++)
	{
		float pos_w, pos_h;

		canvas->GetDC()->GetTextExtent(string(src, 0, i).data(), &pos_w, &pos_h);
		const FUDGE_FACTOR = 1;

		if ((x + pos_w + FUDGE_FACTOR) >= pos_x)
		{
			float x1 = x + last_w;
			float x2 = x + pos_w;

			if ((x2 - pos_x) < (pos_x - x1))
			{
				pos_x = x + pos_w + 1;
			}
			else
			{
				pos_x = x + last_w + 1;
				if (i) i--;
			}

			break;
		}

		last_w = pos_w;
	}

	return i;
}

///////////////////////////////////////////////////////////////////////////////

Bool HtmlObject::Edit(wxKeyEvent& event)
{
	Bool delete_me = FALSE;

	if (event.keyCode == WXK_BACK)
	{
		canvas->Backspace();
	}
	else if (event.keyCode == WXK_DELETE)
	{
		canvas->Delete();
	}
	else if (event.keyCode == WXK_LEFT)
	{
		canvas->MoveLeft();
	}
	else if (event.keyCode == WXK_RIGHT)
	{
		canvas->MoveRight();
	}
	else if (event.keyCode == WXK_UP)
	{
		canvas->MoveUp();
	}
	else if (event.keyCode == WXK_DOWN)
	{
		canvas->MoveDown();
	}
	else if (event.keyCode == WXK_PAGEUP)
	{
		canvas->MovePageUp();
	}
	else if (event.keyCode == WXK_PAGEDOWN)
	{
		canvas->MovePageDown();
	}
	else if (event.keyCode == WXK_HOME)
	{
		canvas->MoveHome();
	}
	else if (event.keyCode == WXK_END)
	{
		canvas->MoveEnd();
	}
	else if (event.keyCode == WXK_RETURN)
	{
		canvas->SetDirty();
		float x2 = canvas->edit_tag->left_margin;
		float delta = canvas->edit_tag->h;
		float save_y = y;

		// Create a new line...

		wxHtml::Tag* tag = new wxHtml::Tag;
		tag->left_margin = canvas->edit_tag->left_margin;
		tag->right_margin = canvas->edit_tag->right_margin;
		tag->center = canvas->edit_tag->center;
		tag->p_center = canvas->edit_tag->p_center;
		tag->left_floating = canvas->edit_tag->left_floating;
		tag->right_floating = canvas->edit_tag->right_floating;
		tag->y = y + delta;
		tag->h = canvas->edit_tag->h;;
		canvas->edit_tags.InsertAfter(tag);

		// Move objects to new line...

		void* t;

		while (canvas->edit_objects.Current(t))
		{
			HtmlObject* obj = (HtmlObject*)t;
			obj->SetTop(obj->GetTop() + delta);
			obj->SetLeft(x2);
			x2 += obj->GetWidth();

			canvas->edit_objects.Delete();
			tag->objects.Append(obj);
			canvas->edit_objects.Next();
		}

		// Add a break to old line...

		canvas->edit_tag->objects.Append(new HtmlBreak(canvas, x + w, save_y));

		// Move everything down...

		canvas->edit_tags.Next();
		canvas->Adjust(delta, canvas->edit_tags);
		canvas->Refresh();
		canvas->MoveCaret(tag->left_margin + 1, tag->y+VERTICAL_ALIGNMENT(canvas));
	}
	else if (isascii(event.keyCode) && !event.ControlDown())
	{
		canvas->SetDirty();
		char src[2] = {(char)event.keyCode, 0};
		float pos_w, pos_h;
		canvas->GetDC()->GetTextExtent(src, &pos_w, &pos_h);
		HtmlText* text = new HtmlText(canvas, x, y, pos_w, pos_h, src);
		canvas->edit_objects.InsertBefore(text);
		canvas->MoveCaret(x+pos_w-1, canvas->current_y);
		x += pos_w;

		canvas->SetClippingRegion(canvas->edit_tag->left_margin, y, canvas->edit_tag->right_margin - canvas->edit_tag->left_margin, h);
		canvas->Clear();
		canvas->DrawTags(canvas->GetDC(), canvas->edit_objects);
		canvas->DestroyClippingRegion();
	}

	return delete_me;
}

///////////////////////////////////////////////////////////////////////////////

Bool HtmlText::Edit(wxKeyEvent& event)
{
	float pos_x = canvas->current_x, pos_y = canvas->current_y;
	Bool delete_me = FALSE;

	if (event.ShiftDown() && !canvas->selected)
	{
		canvas->selected_x = pos_x;
		canvas->selected_y = pos_y;
		canvas->selected = TRUE;
		canvas->skipped = FALSE;
		canvas->Select(pos_x, pos_y);
	}

	size_t i = Index(pos_x, pos_y);

	if (event.keyCode == WXK_BACK)
	{
		if (i > 0)
		{
			canvas->SetDirty();
			src = string(src, 0, i-1) + string(src, i, src.length()-i);
			canvas->GetDC()->GetTextExtent(src.data(), &w, &h);
			canvas->SetClippingRegion(canvas->edit_tag->left_margin, y, canvas->edit_tag->right_margin - canvas->edit_tag->left_margin, h);
			canvas->Clear();
			canvas->DrawTags(canvas->GetDC(), canvas->edit_objects);
			canvas->DestroyClippingRegion();
			float pos_w, pos_h;
			canvas->GetDC()->GetTextExtent(string(src, 0, i-1).data(), &pos_w, &pos_h);
			canvas->MoveCaret(x + pos_w, pos_y);
		}
		else
		{
			if (canvas->edit_objects.PeekPrevious())
			{
				canvas->MoveLeft();
				canvas->edit_object->Edit(event);
			}
			else
				canvas->MergeLeft();
		}
	}
	else if (event.keyCode == WXK_DELETE)
	{
		if (i < src.length())
		{
			canvas->SetDirty();
			src = string(src, 0, i) + string(src, i+1, src.length()-(i+1));
			canvas->GetDC()->GetTextExtent(src.data(), &w, &h);
			canvas->SetClippingRegion(canvas->edit_tag->left_margin, y, canvas->edit_tag->right_margin - canvas->edit_tag->left_margin, h);
			canvas->Clear();
			canvas->DrawTags(canvas->GetDC(), canvas->edit_objects);
			canvas->DestroyClippingRegion();

			if (!src.length())
			{
				canvas->edit_objects.Delete();
				canvas->MoveRight();
				delete_me = TRUE;
			}
		}
		else
		{
			if (canvas->edit_objects.PeekNext())
			{
				canvas->MoveRight();
				canvas->edit_object->Edit(event);
			}
			else
				canvas->MergeRight();
		}
	}
	else if (event.keyCode == WXK_START)    // ????????????????????????????
	{
		float pos_w, pos_h;
		canvas->GetDC()->GetTextExtent(string(src, 0, i).data(), &pos_w, &pos_h);
		float pos_hh, pos_ww;
		canvas->GetDC()->GetTextExtent(string(src, 0, i-1).data(), &pos_ww, &pos_hh);
		canvas->MoveCaret(x + CLOSEST(event.x - x, pos_ww, pos_w), pos_y);
		if (CLOSEST(event.x-x, pos_ww, pos_w) == pos_ww)
			pos = i - 1;
		else
			pos = i;
	}
	else if (event.keyCode == WXK_LEFT)
	{
		if (i > 0)
		{
			i--;

			if (event.ControlDown())
			{
				const char* ptr = src.data() + i;

				while ((i > 0) && !isspace(*ptr))
				{
					ptr--;
					i--;
				}
			}

			float pos_w, pos_h;
			canvas->GetDC()->GetTextExtent(string(src, 0, i).data(), &pos_w, &pos_h);
			canvas->MoveCaret(x + pos_w + 1, pos_y);
		}
		else
		{
			canvas->MoveLeft();
		}
	}
	else if (event.keyCode == WXK_RIGHT)
	{
		if (i < src.length())
		{
			i++;

			if (event.ControlDown())
			{
				const char* ptr = src.data() + i;

				while ((i < src.length()) && !isspace(*ptr))
				{
					ptr++;
					i++;
				}
			}

			float pos_w, pos_h;
			canvas->GetDC()->GetTextExtent(string(src, 0, i).data(), &pos_w, &pos_h);
			canvas->MoveCaret(x + pos_w - 1, pos_y);
		}
		else
		{
			canvas->MoveRight();
		}
	}
	else if (event.keyCode == WXK_UP)
	{
		canvas->MoveUp();
	}
	else if (event.keyCode == WXK_DOWN)
	{
		canvas->MoveDown();
	}
	else if (event.keyCode == WXK_PAGEUP)
	{
		canvas->MovePageUp();
	}
	else if (event.keyCode == WXK_PAGEDOWN)
	{
		canvas->MovePageDown();
	}
	else if (event.keyCode == WXK_HOME)
	{
		canvas->MoveHome();
	}
	else if (event.keyCode == WXK_END)
	{
		canvas->MoveEnd();
	}
	else if (event.keyCode == WXK_RETURN)
	{
		canvas->SetDirty();
		float x2 = canvas->edit_tag->left_margin;
		float delta = canvas->edit_tag->h;
		float pos_w, pos_h;

		// Create a new line...

		wxHtml::Tag* tag = new wxHtml::Tag;
		tag->left_margin = canvas->edit_tag->left_margin;
		tag->right_margin = canvas->edit_tag->right_margin;
		tag->center = canvas->edit_tag->center;
		tag->p_center = canvas->edit_tag->p_center;
		tag->left_floating = canvas->edit_tag->left_floating;
		tag->right_floating = canvas->edit_tag->right_floating;
		tag->y = y + delta;
		tag->h = canvas->edit_tag->h;;
		canvas->edit_tags.InsertAfter(tag);

		if ((i > 0) && (i < src.length()))
		{
			// Truncate the text...

			string s(src, i, src.length()-i);
			//src = string(src, 0, i).trim();

			canvas->GetDC()->GetTextExtent(src.data(), &pos_w, &pos_h);
			SetWidth(pos_w);

			// If we broke in front of a space then consume it...

			const char* new_src = s.data();
			while (isspace(*new_src))
				new_src++;

			// Create a new text object...

			canvas->GetDC()->GetTextExtent(new_src, &pos_w, &pos_h);

			tag->objects.Append(new HtmlText(canvas, x2, y+delta, pos_w, pos_h, new_src));
			x2 += pos_w;

			canvas->edit_objects.Next();
		}
		else if (i == src.length())
		{
			//src = src.trim();
			canvas->GetDC()->GetTextExtent(src.data(), &pos_w, &pos_h);
			SetWidth(pos_w);

			canvas->edit_objects.Next();
		}
		else
		{
			// If we broke in front of a space then consume it...

			const char* new_src = src.data();
			while (isspace(*new_src))
				new_src++;

			src = new_src;
			canvas->GetDC()->GetTextExtent(src.data(), &pos_w, &pos_h);
			SetWidth(pos_w);
		}

		// Move objects to new line...

		void* t;

		while (canvas->edit_objects.Current(t))
		{
			HtmlObject* obj = (HtmlObject*)t;
			obj->SetTop(obj->GetTop() + delta);
			obj->SetLeft(x2);
			x2 += obj->GetWidth();

			canvas->edit_objects.Delete();
			tag->objects.Append(obj);
			canvas->edit_objects.Next();
		}

		// Add a break to old line...

		canvas->edit_tag->objects.Append(new HtmlBreak(canvas, x + pos_w, y));

		// Move everything down...

		canvas->edit_tags.Next();
		canvas->Adjust(delta, canvas->edit_tags);
		canvas->Refresh();
		canvas->MoveCaret(tag->left_margin + 1, tag->y+VERTICAL_ALIGNMENT(canvas));
	}
	else if (isascii(event.keyCode) && !event.ControlDown())
	{
		canvas->SetDirty();
		src = string(src, 0, i) + (char)event.keyCode + string(src, i, src.length()-i);
		canvas->GetDC()->GetTextExtent(src.data(), &w, &h);
		canvas->SetClippingRegion(canvas->edit_tag->left_margin, y, canvas->edit_tag->right_margin - canvas->edit_tag->left_margin, h);
		canvas->Clear();
		canvas->DrawTags(canvas->GetDC(), canvas->edit_objects);
		canvas->DestroyClippingRegion();
		float pos_w, pos_h;
		canvas->GetDC()->GetTextExtent(string(src, 0, i+1).data(), &pos_w, &pos_h);
		canvas->MoveCaret(x + pos_w-1, pos_y);
	}
	else
	{
		return FALSE;
	}


	if (event.ShiftDown() && canvas->selected)
	{
		canvas->Select(canvas->current_x, canvas->current_y);
	}
	else if (!event.ShiftDown())
	{
		canvas->selected_x = canvas->current_x;
		canvas->selected_y = canvas->current_y;
	}

	return delete_me;
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::ProcessEvent(wxMouseEvent& event)
{
	float pos_x, pos_y;
	event.Position(&pos_x, &pos_y);

	if (event.RightDown())
	{
	}
	else if (event.RightUp())
	{
		last_canvas = this;

		if (LocateHtmlObject(pos_x, pos_y))
		{
			menu2->Enable(HTML_COPY, selected_objects);
			menu2->Enable(HTML_CUT, selected_objects);
			menu2->Enable(HTML_PASTE, clipboard.Count());
			menu2->Enable(HTML_BOLD, selected_objects);
			menu2->Enable(HTML_ITALIC, selected_objects);
			menu2->Enable(HTML_UNDERLINE, selected_objects);
			menu2->Enable(HTML_STRIKE, selected_objects);
			menu2->Enable(HTML_FIXED, selected_objects);
			menu2->Enable(HTML_PLUS, selected_objects);
			menu2->Enable(HTML_MINUS, selected_objects);
			menu2->Enable(HTML_PROPERTIES, (edit_object?TRUE:FALSE));

			PopupMenu(menu2, (float)ABSOLUTE_X(pos_x), (float)ABSOLUTE_Y(pos_y));
		}
	}
	else if (event.LeftDown())
	{
		DrawCaret(FALSE);

		if (LocateHtmlObject(pos_x, pos_y))
		{
			if (edit_object->IsText())
			{
				HtmlText* text = (HtmlText*)edit_object;
				text->Index(pos_x, pos_y);
			}

			selected_x = pos_x;
			selected_y = pos_y;
			selected = TRUE;
			skipped = FALSE;
			Select(pos_x, pos_y);
			MoveCaret(pos_x, edit_object->GetTop()+VERTICAL_ALIGNMENT(this));
		}
	}
	else if (event.LeftUp())
	{
		if (selected)
		{
			if (LocateHtmlObject(pos_x, pos_y))
			{
				if (edit_object->IsText())
				{
					HtmlText* text = (HtmlText*)edit_object;
					text->Index(pos_x, pos_y);
				}

				MoveCaret(pos_x, edit_object->GetTop()+VERTICAL_ALIGNMENT(this));
			}

			if ((pos_x == selected_x) && (pos_y == selected_y))
				selected_objects = 0;

			selected = FALSE;
		}

		last_x = pos_x;
		last_y = pos_y;
		last_tick = tick;
	}
	else if (selected && ((last_x != pos_x) || (last_y != pos_y)))
	{
		skipped = last_tick == tick;

		if (!skipped)
			Select(pos_x, pos_y);

		if (starting_y && (pos_y <= (starting_y*VSCROLL_INCREMENT + 10)))
			Scroll(-1, starting_y - 1);
		else if (pos_y >= (starting_y*VSCROLL_INCREMENT + drawing_h - 10))
			Scroll(-1, starting_y + 1);

		last_x = pos_x;
		last_y = pos_y;
		last_tick = tick;
	}
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::ProcessChar(wxKeyEvent& event)
{
	float pos_x = current_x, pos_y = current_y;

	if (clipboard.Count() && edit_object && 
			event.ControlDown() && (toupper(event.keyCode) == 'V'))
	{
		if (selected_objects) Drop();
		Paste();
	}

	if (selected_objects && ((pos_x != selected_x) || (pos_y != selected_y)))
	{
		if (event.keyCode == WXK_DELETE)
			Cut();
		else if (event.ControlDown() && (toupper(event.keyCode) == 'X'))
			Cut();
		else if (event.ControlDown() && (toupper(event.keyCode) == 'C'))
			Copy();
		else if (event.ControlDown() && (toupper(event.keyCode) == 'B'))
			Mode(BOLD);
		else if (event.ControlDown() && (toupper(event.keyCode) == 'I'))
			Mode(ITALIC);
		else if (event.ControlDown() && (toupper(event.keyCode) == 'S'))
			Mode(STRIKE);
		else if (event.ControlDown() && (toupper(event.keyCode) == 'U'))
			Mode(UNDERLINE);
		else if (event.ControlDown() && (toupper(event.keyCode) == 'F'))
			Mode(FIXED);
		else if (isascii(event.keyCode))
			Drop();
		else if (!event.ShiftDown())
			Deselect();
	}

	if (LocateHtmlObject(pos_x, pos_y))
	{
		HtmlObject* obj = edit_object;
		if (obj->Edit(event))
			delete obj;
	}

	last_x = current_x;
	last_y = current_y;
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::Select(float pos_x, float pos_y)
{
	float x1, y1, x2, y2;

	selected_objects = 0;

	if ((pos_y < selected_y) ||
		((pos_y == selected_y) && (pos_x < selected_x)))
	{
		x1 = pos_x;
		y1 = pos_y;
		x2 = selected_x;
		y2 = selected_y;
	}
	else
	{
		x1 = selected_x;
		y1 = selected_y;
		x2 = pos_x;
		y2 = pos_y;
	}

	ListOfIterator ilist1 = *tags;
	void* t1;

	for (int ok1 = ilist1.First(t1); ok1; ok1 = ilist1.Next(t1))
	{
		Tag* tag = (Tag*)t1;

		// Undo old selections as well...

		if (tag->y < (wxMin(y1, old_y1)-10))
			continue;

		if (tag->y > (wxMax(y2, old_y2)+20))
			break;

		ListOfIterator ilist2 = tag->objects;
		void* t2;

		for (int ok2 = ilist2.First(t2); ok2; ok2 = ilist2.Next(t2))
		{
			HtmlObject* obj = (HtmlObject*)t2;

			obj->SetSelected(FALSE);

			if ((obj->GetTop()+obj->GetHeight() < y1) || (obj->GetTop() > y2))
				;
			else if ((obj->GetTop() <= y1) && (obj->GetTop()+obj->GetHeight() > y1) &&
					((obj->GetLeft()+obj->GetWidth()) < x1))
				;
			else if ((obj->GetTop() <= y2) && (obj->GetTop()+obj->GetHeight() > y2) &&
					(obj->GetLeft() > x2))
				;
			else
			{
				obj->SetSelected(TRUE);
				selected_objects++;
			}

			if (obj->IsSelected() && obj->IsText())
			{
				HtmlText* text = (HtmlText*)obj;
				text->SelectAll();

				if (obj->IsBounded(x1, y1, tag->h))
					text->Select(x1, y1, TRUE);

				if (obj->IsBounded(x2, y2, tag->h))
					text->Select(x2, y2, FALSE);
			}

			obj->Draw(GetDC());
		}
	}

	current_x = pos_x;
	current_y = pos_y;
	old_y1 = y1;
	old_y2 = y2;
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::Deselect()
{
	selected_x = selected_y = -1.0F;
	Select(selected_x, selected_y);
	selected_objects = 0;
	selected = FALSE;
}

///////////////////////////////////////////////////////////////////////////////

void HtmlText::Select(float pos_x, float pos_y, Bool start)
{
	size_t i = Index(pos_x, pos_y);
	(start ? snip_from : snip_to) = i;
}

///////////////////////////////////////////////////////////////////////////////

void HtmlText::SelectAll()
{
	snip_from = 0;
	snip_to = src.length();
}

///////////////////////////////////////////////////////////////////////////////

void HtmlImage::Adjust()
{
	if (align == 4)
	{
		float save_left_margin = canvas->edit_tag->left_margin;

		// Go thru each line adjusting the left margin...

		ListOfIterator ilist = canvas->edit_tags;
		void* a;
		ilist.Current(a);

		do
		{
			wxHtml::Tag* item = (wxHtml::Tag*)a;

			// Go thru each object adjusting it's position...

			ListOfIterator ilist2 = item->objects;
			void* a2;

			for (int ok2 = ilist.First(a2); ok2; ok2 = ilist2.Next(a2))
			{
				HtmlObject* obj = (HtmlObject*)a2;

				if (!obj->IsFloat())
				{
					float delta = (x + w) - save_left_margin;
					float left = obj->GetLeft() + delta;
					obj->SetLeft(left);
				}
			}

			// At the bottom of the image?

			if (item->left_margin != save_left_margin)
				break;

			item->left_margin = x + w;
		}
		  while (ilist.Next(a));
	}

	// Locate first line past this image...

	ListOfIterator ilist = canvas->edit_tags;
	void* a1;
	float delta = 0.0F;

	while (ilist.Next(a1))
	{
		wxHtml::Tag* item = (wxHtml::Tag*)a1;
		void* a2;
		item->objects.Head(a2);
		HtmlObject* obj = (HtmlObject*)a2;

		if (obj->GetLeft() < (x + w))
		{
			delta = (y + h) - item->y;
			break;
		}
	}

	canvas->Adjust(delta, ilist);
	canvas->Refresh();
}

///////////////////////////////////////////////////////////////////////////////

void HtmlText::Caret(float pos_x, float pos_y)
{
	wxKeyEvent event;
	event.x = pos_x;
	event.y = pos_y;
	event.keyCode = WXK_START;
	Edit(event);
}

///////////////////////////////////////////////////////////////////////////////

void HtmlObject::SetCursor(float /*pos_x*/, float /*pos_y*/)
{
	canvas->SetCursor(new wxCursor(wxCURSOR_ARROW));
}

void HtmlText::SetCursor(float /*pos_x*/, float /*pos_y*/)
{
	canvas->SetCursor(new wxCursor(wxCURSOR_IBEAM));
}

void HtmlImage::SetCursor(float pos_x, float pos_y)
{
	if (  (((pos_x - x) < 20) || (((x + w) - pos_x) < 20)) ||
			(((pos_y - y) < 20) || (((y + h) - pos_y) < 20)) )
	{
		canvas->SetCursor(new wxCursor(wxCURSOR_SIZING));
	}
	else
	{
		canvas->SetCursor(new wxCursor(wxCURSOR_HAND));
	}
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::SetMode(unsigned new_mode, int fsz_delta)
{
	Bool started = FALSE, ended = FALSE;
	ListOfIterator ilist1 = *tags;
	void* t1;

	for (int ok1 = ilist1.First(t1); ok1 && !ended; ok1 = ilist1.Next(t1))
	{
		Tag* tag = (Tag*)t1;

		ListOfIterator ilist2 = tag->objects;
		void* t2;

		for (int ok2 = ilist2.First(t2); ok2; ok2 = ilist2.Next(t2))
		{
			HtmlObject* obj = (HtmlObject*)t2;

			if (!obj->IsSelected())
			{
				if (started)
				{
					ended = TRUE;
					break;
				}

				continue;
			}

			if (!started)
				started = TRUE;

			if (obj->IsText())
			{
				HtmlText* text = (HtmlText*)obj;
				text->Snip(ilist2);
				if (!text->IsSelected()) continue;

				unsigned tmp_mode = text->GetMode();
				if (tmp_mode & new_mode)
					tmp_mode &= ~new_mode;
				else
					tmp_mode |= new_mode;
				text->SetMode(tmp_mode);

				int tmp_fsz = text->GetFontSize() + fsz_delta;
				text->SetFontSize(tmp_fsz);

				text->HtmlObject::Draw(GetDC());
				float w, h;
				GetDC()->GetTextExtent(text->GetText().data(), &w, &h);
				text->SetWidth(w);
				text->SetHeight(h);
			}
		}
	}

	Refresh();
	SetDirty();
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::Copy(Bool cut, Bool to_clipboard)
{
	wxBeginBusyCursor();

	Bool started = FALSE, ended = FALSE;
	float start_y = -1.0F, end_y = -1.0F, start_x, start_h;
	float start_left_margin, start_right_margin, delta = 0.0F;
	Tag* start_tag = 0;
	Tag* end_tag = 0;
	void* t;

	// Clear the clipboard first...

	while (to_clipboard && clipboard.Pop(t))
	{
		HtmlObject* obj = (HtmlObject*)t;
		delete obj;
	}

	// Now copy/cut the selection...

	ListOfIterator ilist1 = *tags;
	void* t1;

	for (int ok1 = ilist1.First(t1); ok1 && !ended; ok1 = ilist1.Next(t1))
	{
		Tag* tag = (Tag*)t1;

		ListOfIterator ilist2 = tag->objects;
		void* t2;

		for (int ok2 = ilist2.First(t2); ok2; ok2 = ilist2.Next(t2))
		{
			HtmlObject* obj = (HtmlObject*)t2;

			if (!obj->IsSelected())
			{
				if (started)
				{
					ended = TRUE;
					break;
				}

				continue;
			}

			if (obj->IsText())
			{
				HtmlText* text = (HtmlText*)obj;
				text->Snip(ilist2);
				if (!text->IsSelected()) continue;
			}

			if (!started)
			{
				start_y = tag->y;
				start_h = tag->h;
				start_left_margin = tag->left_margin;
				start_right_margin = tag->right_margin;
				start_x = obj->GetLeft();
				started = TRUE;
				start_tag = tag;
			}

			end_tag = tag;
			end_y = tag->y;

			if (cut)
				ilist2.Delete();
			else
				obj = obj->Clone();

			obj->SetSelected(FALSE);

			if (to_clipboard)
				clipboard.Append(obj);
			else if (cut)
				delete obj;
		}

		if (!tag->objects.Count())
		{
			delta -= tag->h;
			if (tag == start_tag) start_tag = 0;
			ilist1.Delete();
			delete tag;
		}
	}

	if (cut && (start_y >= 0.0F) && (clipboard.Count() || !to_clipboard))
	{
		if (start_tag && end_tag && (end_tag != start_tag))
		{
			// Move left-over objects on the end line to the
			// start line..

			void* t;

			while (end_tag->objects.Pop(t))
			{
				HtmlObject* obj = (HtmlObject*)t;
				obj->SetTop(start_tag->y);
				start_tag->objects.Append(obj);
			}

			delta -= end_tag->h;
		}

		// Move everything after the end-line up...

		if (ended && delta)
		{
			ilist1.Previous();
			Adjust(delta, ilist1);
		}

		float h = (delta == 0 ? start_h : (starting_y + drawing_h) - start_y);
		SetClippingRegion(start_left_margin, start_y, start_right_margin - start_left_margin, h);
		Clear();
		Deselect();
		OnPaint();
		DestroyClippingRegion();
		MoveCaret(start_x, start_y + VERTICAL_ALIGNMENT(this));
		LocateHtmlObject(current_x, current_y);

		if (current_y < (starting_y*VSCROLL_INCREMENT))
		{
			float pos_y = current_y - (drawing_h/2);
			if (pos_y < 0.0F) pos_y = 0.0F;
			Scroll(-1, (int)(pos_y/VSCROLL_INCREMENT));
		}

		SetDirty();
	}

	wxEndBusyCursor();
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::Paste(ListOfIterator ilist)
{
	Bool was_break = FALSE;
	float pos_x = current_x;
	void* t;

	for (int ok = ilist.First(t); ok; ok = ilist.Next(t))
	{
		HtmlObject* obj = (HtmlObject*)t;
		obj = obj->Clone();

		if (obj->NewLine() || was_break ||
			((pos_x + obj->GetWidth()) > edit_tag->right_margin))
		{
			Tag* tag = new Tag;
			tag->y = edit_tag->y + edit_tag->h;
			tag->h = 0.0F;
			tag->center = edit_tag->center;
			tag->p_center = edit_tag->p_center;
			tag->left_margin = edit_tag->left_margin;
			tag->right_margin = edit_tag->right_margin;
			tag->left_floating = edit_tag->left_floating;
			tag->right_floating = edit_tag->right_floating;
			edit_tags.InsertAfter(tag);
			edit_tags.Next();

			float delta = edit_tag->h;
			Adjust(delta, edit_tags);
			edit_tag = tag;
			edit_objects = tag->objects;
			edit_object = obj;
			pos_x = edit_tag->left_margin;
		}

		edit_tag->h = wxMax(edit_tag->h, obj->GetHeight());
		edit_objects.InsertAfter(obj);
		obj->SetTop(edit_tag->y);
		pos_x += obj->GetWidth();
		was_break = obj->NewLine() || obj->EndLine();
	}
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::Paste()
{
	wxBeginBusyCursor();

	// If in the middle of a text object then break at the current
	// cursor (caret) position...

	if (edit_object->IsText())
	{
		HtmlText* text = (HtmlText*)edit_object;
		text->Snip(edit_objects);
	}

	// Transfer the rest of the line following the insertion
	// point into a holding buffer...

	ListOf remainders;
	void* t;

	while (edit_objects.Next(t))
	{
		HtmlObject* obj = (HtmlObject*)t;
		remainders.Append(obj);
		edit_objects.Delete();
	}

	edit_objects.Last();

	// Copy the clipboard objects in...

	Paste(clipboard);

	// Now copy the 'remainders' back in...

	Paste(remainders);

	while (remainders.Pop(t))
	{
		HtmlObject* obj = (HtmlObject*)t;
		delete obj;
	}

	// Done...

	wxEndBusyCursor();
	Refresh();
	SetDirty();
}

///////////////////////////////////////////////////////////////////////////////

void HtmlText::Snip(ListOfIterator& ilist)
{
	string s1(src, 0, snip_from);
	string s2(src, snip_from, snip_to - snip_from);
	string s3(src, snip_to, (src.length()-0) - snip_to);
	float _w, _h;

	HtmlObject::Draw(canvas->GetDC());

	if (s1.length())
	{
		canvas->GetDC()->GetTextExtent(s1.data(), &_w, &_h);
		SetText(s1, _w);
		SetSelected(FALSE);
	}

	if (s2.length())
	{
		HtmlText* text;

		if (s1.length())
		{
			text = (HtmlText*)Clone();
			ilist.InsertAfter(text);
		}
		else
			text = this;
	
		canvas->GetDC()->GetTextExtent(s2.data(), &_w, &_h);
		text->SetText(s2, _w);
		text->SelectAll();
		text->SetSelected(TRUE);
	}

	if (s3.length())
	{
		HtmlText* text = (HtmlText*)Clone();
		canvas->GetDC()->GetTextExtent(s3.data(), &_w, &_h);
		text->SetText(s3, _w);
		text->SetSelected(FALSE);
		ilist.InsertAfter(text);
	}

	if (s2.length() && s1.length())
		ilist.Previous();

	if (s3.length())
		ilist.Previous();
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::MoveCaret(float pos_x, float pos_y)
{
	DrawCaret(FALSE);
	current_x = pos_x;
	current_y = pos_y;
	DrawCaret(TRUE);
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::DrawCaret(Bool on)
{
	if (!edit_object) return;

	if (!on)
		GetDC()->SetPen(wxThePenList->FindOrCreatePen(background, 2, wxSOLID));
	else
		GetDC()->SetPen(wxThePenList->FindOrCreatePen(foreground, 2, wxSOLID));

	float h = edit_object->GetHeight();

	GetDC()->DrawLine(current_x, current_y - h/2, current_x, current_y + h/2);

	if (!on) edit_object->Draw(GetDC());
}

#endif

