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

///////////////////////////////////////////////////////////////////////////////
// Queries are of the form...
//
//    URL?name1=value1&name2=value2...
//
///////////////////////////////////////////////////////////////////////////////

void wxHtml::FormSubmit(wxButton& button, wxCommandEvent&)
{
#if USE_HTML_FORMS

	wxHtml* html = (wxHtml*)(button.GetParent());

	// Go thru the fields getting name/value pairs...

	html->form_objects.BeginFind();
	wxNode* node;
	int first = 1;
	string value;
	html->post_data = "";

	while ((node = html->form_objects.Next()) != 0)
	{
		wxItem* field = (wxItem*)node->Data();

		if (field->IsKindOf(CLASSINFO(wxChoice)))
		{
			string key = ((wxChoice*)field)->GetStringSelection();
			string* tmp = (string*)html->form_values.Get((char*)key.data());
			if (tmp)
				value = *tmp;
			else
				value = "???";
		}
		else if (field->IsKindOf(CLASSINFO(wxCheckBox)))
		{
			if (((wxCheckBox*)field)->GetValue())
			{
				string* tmp = (string*)html->form_values.Get(node->key.string);
				if (tmp)
					value = *tmp;
				else
					value = "???";
			}
			else
			{
				html->form_values.Delete(node->key.string);
				continue;
			}

			html->form_values.Delete(node->key.string);
		}
		else if (field->IsKindOf(CLASSINFO(wxRadioButton)))
		{
			if (((wxRadioButton*)field)->GetValue())
			{
				string* tmp = (string*)html->form_values.Get(node->key.string);
				if (tmp)
					value = *tmp;
				else
					value = "???";
			}
			else
			{
				html->form_values.Delete(node->key.string);
				continue;
			}

			html->form_values.Delete(node->key.string);
		}
		else if (field->IsKindOf(CLASSINFO(wxButton)))
		{
			if (field == &button)
			{
				string* tmp = (string*)html->form_values.Get(node->key.string);
				if (tmp)
					value = *tmp;
				else
					value = "???";
			}
			else
				continue;
		}
		else if (field->IsKindOf(CLASSINFO(wxText)))
		{
			value = ((wxText*)field)->GetValue();
		}
		else
			continue;

		if (!first)
			html->post_data += "&";

		first = 0;

		html->post_data += node->key.string;
		html->post_data += "=";

		for (const char* src = value.data(); *src; src++)
		{
			if (!isalnum(*src))
			{
				char tmp[10];
				sprintf(tmp, "%%%.02X", (unsigned)*src);
				html->post_data += tmp;
			}
			else
				html->post_data += *src;
		}
	}

	html->OnSetStatusText("Loading...");
	html->in_progress++;
	wxBeginBusyCursor();

	if (html->form_method == "GET")
		html->OpenURL(html->form_action + "?" + html->post_data, FALSE);
	else if (html->form_method == "POST")
		html->OpenURL(html->form_action, TRUE);

	wxEndBusyCursor();
	html->in_progress--;
	html->OnSetStatusText("Done");
	html->post_data = "";

#endif
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::FormReset(wxButton& button, wxCommandEvent&)
{
#if USE_HTML_FORMS

	wxHtml* html = (wxHtml*)(button.GetParent());

	html->form_objects.BeginFind();
	wxNode* node;

	while ((node = html->form_objects.Next()) != 0)
	{
		wxItem* field = (wxItem*)node->Data();
		const char* data = field->GetClientData();

		if (field->IsKindOf(CLASSINFO(wxText)))
			((wxText*)field)->SetValue("");
		else if (field->IsKindOf(CLASSINFO(wxChoice)))
			((wxChoice*)field)->SetSelection((int)(long)data);
		else if (field->IsKindOf(CLASSINFO(wxCheckBox)))
			((wxCheckBox*)field)->SetValue((data?TRUE:FALSE));
	}

#endif
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::FormButton(wxButton& /*button*/, wxCommandEvent&)
{
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::FormShow(Bool state)
{
#if USE_HTML_FORMS

	form_objects.BeginFind();
	wxNode* node;

	while ((node = form_objects.Next()) != 0)
	{
		wxItem* field = (wxItem*)node->Data();
		field->Show(state);
	}

#endif
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::FormClear()
{
#if USE_HTML_FORMS

	// Any form items?

	form_objects.BeginFind();

	if (form_objects.Next())
		DestroyChildren();

	form_objects.Clear();

	form_values.BeginFind();
	wxNode* node;

	while ((node = form_values.Next()) != 0)
		delete (string*)node->Data();

	form_values.Clear();

#endif
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::FormBrowse(wxButton& button, wxCommandEvent&)
{
#if USE_HTML_FORMS

	wxText* field = (wxText*)button.GetClientData();

	char* filename = wxFileSelector("Select file", 0, 0, "", "*.*");

	if (field && filename)
		field->SetValue(filename);

#endif
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::ParseForm(wxDC* dc, char*&, char* buf, char* tag)
{
#if USE_HTML_FORMS

	static int new_group = 1;

	static string textarea_name, last_value;
	static int option_count = 0, choice_selected = 0, max_width = 0;

	float tmp_width, tmp_height;
	dc->GetTextExtent("X", &tmp_width, &tmp_height);

	if (!strncmpi(tag, "FORM", 4))
	{
		new_group = 1;

		Release(dc);
		Hold(new HtmlMarkup(this, x, y, tag));
		string key;
		string name;
		const char* ptr = tag+4;

		form_method = "GET";
		form_objects.Clear();

		while (GetAttributeValuePair(ptr, key, name) && (dc == GetDC()))
		{
			if (!strcmpi(key.data(), "ACTION"))
			{
				form_action = name;
			}
			else if (!strcmpi(key.data(), "SCRIPT"))
			{
				// need to look into this???
			}
			else if (!strcmpi(key.data(), "METHOD"))
			{
				form_method = name;
			}
		}

		forming++;
	}
	else if (forming && !strncmpi(tag, "INPUT", 5))
	{
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		string key;
		string name;
		const char* ptr = tag+5;
		string field_name, field_value, field_src, field_type = "TEXT";
		Bool disabled = FALSE, checked = FALSE;
		int size = 0, max_length = 0;
		double min_value = 0, max_value = 0;

		// This allows for borders...
		if (x == left_margin) y += 5;

		while (GetAttributeValuePair(ptr, key, name) && (dc == GetDC()))
		{
			if (!strcmpi(key.data(), "TYPE"))
			{
				field_type = name;
			}
			else if (!strcmpi(key.data(), "NAME"))
			{
				field_name = name;
			}
			else if (!strcmpi(key.data(), "VALUE"))
			{
				field_value = name;
			}
			else if (!strcmpi(key.data(), "DISABLED"))
			{
				disabled = TRUE;
			}
			else if (!strcmpi(key.data(), "CHECKED"))
			{
				checked = TRUE;
			}
			else if (!strcmpi(key.data(), "SIZE"))
			{
				size = atoi(name.data());  
			}
			else if (!strcmpi(key.data(), "MAXLENGTH"))
			{
				max_length = atoi(name.data());
			}
			else if (!strcmpi(key.data(), "MAX"))
			{
				 max_value = strtod(name.data(), 0);
			}
			else if (!strcmpi(key.data(), "MIN"))
			{
				 min_value = strtod(name.data(), 0);
			}
			else if (!strcmpi(key.data(), "SRC"))
			{
				field_src = name;
			}
			else if (!strcmpi(key.data(), "CLEAR"))
			{
				if (left_limit_y) y = left_limit_y; left_limit_y = 0.0F; left_floating = right_floating = 0;
				x = left_margin = DEFAULT_LEFT_MARGIN;
			}
		}

		if (!strcmpi(field_type.data(), "PASSWORD"))
		{
			new_group = 1;
			float ww = (float)((size?(size)+1:(field_value.length()?field_value.length()+1:30)) * tmp_width + 10);
			float hh = (float)(tmp_height + 10);
			y -= (hh - tmp_height) / 2;

			wxText* field = new wxText(this, 0, "", (char*)field_value.data(), (int)x, (int)y, (int)ww, (int)hh, wxPASSWORD|(disabled?wxREADONLY:0));
			//field->GetSize(&ww, &hh);
			form_objects.Put((char*)field_name.data(), field);
			if (!IsCompiling()) Hold(new HtmlFormItem(this, x, y, ww, hh, field));

			y += (hh - tmp_height) / 2;
			x += ww;
			hh += 5; if (hh > max_h) max_h = hh;
		}
		else if (!strcmpi(field_type.data(), "HIDDEN"))
		{
			new_group = 1;
			float ww = (float)((size?(size)+1:(field_value.length()?field_value.length()+1:30)) * tmp_width + 10);
			float hh = (float)(tmp_height + 10);
			//y -= (hh - tmp_height) / 2;

			wxText* field = new wxText(this, 0, "", (char*)field_value.data(), (int)drawing_w, (int)drawing_h, (int)ww, (int)hh);
			//field->GetSize(&ww, &hh);
			field->Enable(FALSE);
			field->Show(FALSE);
			form_objects.Put((char*)field_name.data(), field);
			if (!IsCompiling()) Hold(new HtmlHiddenFormItem(this, x, y, 0.0F, 0.0F, field));

			//y += (hh - tmp_height) / 2;
			//x += ww;
			//hh += 5; if (hh > max_h) max_h = hh;
		}
		else if (!strcmpi(field_type.data(), "CHECKBOX"))
		{
			new_group = 1;
			if (!field_value.length())
				field_value = " ";

			float ww = 20.0F;//(float)((field_value.length() * tmp_width) + 20);
			float hh = 20.0F;//(float)(tmp_height + 10);
			y -= (hh - tmp_height) / 2;

			wxCheckBox* field = new wxCheckBox(this, 0, "", (int)x, (int)y, (int)ww, (int)hh);
			//field->GetSize(&ww, &hh);
			field->SetClientData(checked?(char*)1:0);
#ifndef wx_motif
			field->Enable(!disabled);
#endif
			field->SetValue(checked);
			form_objects.Put((char*)field_name.data(), field);
			form_values.Put((char*)field_name.data(), (wxObject*)new string(field_value));
			if (!IsCompiling()) Hold(new HtmlFormItem(this, x, y, ww, hh, field));

			y += (hh - tmp_height) / 2;
			x += ww;
			hh += 5; if (hh > max_h) max_h = hh;
		}
		else if (!strcmpi(field_type.data(), "RADIO"))
		{
			if (!field_value.length())
				field_value = " ";

			float ww = 20.0F;//(float)((field_value.length() * tmp_width) + 20);
			float hh = 20.0F;//(float)(tmp_height + 10);
			y -= (hh - tmp_height) / 2;

			wxRadioButton* field = new wxRadioButton(this, 0, "", (int)x, (int)y, (int)ww, (int)hh, (new_group?wxRB_GROUP:0));
			//field->GetSize(&ww, &hh);
			field->SetClientData(checked?(char*)1:0);
#ifndef wx_motif
			field->Enable(!disabled);
#endif
			field->SetValue(checked);
			form_objects.Put((char*)field_name.data(), field);
			form_values.Put((char*)field_name.data(), (wxObject*)new string(field_value));
			if (!IsCompiling()) Hold(new HtmlFormItem(this, x, y, ww, hh, field));

			y += (hh - tmp_height) / 2;
			x += ww;
			hh += 5; if (hh > max_h) max_h = hh;
			new_group = 0;
		}
		else if (!strcmpi(field_type.data(), "SUBMIT") || !strcmpi(field_type.data(), "IMAGE"))
		{
			new_group = 1;
			if (!field_value.length())
				field_value = "Submit Form";

			float ww = (float)(field_value.length() * tmp_width + 20);
			float hh = (float)(tmp_height + 10);
			y -= (hh - tmp_height) / 2;

			wxButton* field = new wxButton(this, (wxFunction)FormSubmit, (char*)field_value.data(), (int)x, (int)y, (int)ww, (int)hh);
			//field->GetSize(&ww, &hh);
#ifndef wx_motif
			field->Enable(!disabled);
#endif
			form_objects.Put((char*)field_name.data(), field);
			form_values.Put((char*)field_name.data(), (wxObject*)new string(field_value));
			if (!IsCompiling()) Hold(new HtmlFormItem(this, x, y, ww, hh, field));

			y += (hh - tmp_height) / 2;
			x += ww;
			if (hh > max_h) max_h = hh;
		}
		else if (!strcmpi(field_type.data(), "RESET"))
		{
			new_group = 1;
			if (!field_value.length())
				field_value = "Reset Form";

			float ww = (float)(field_value.length() * tmp_width + 20);
			float hh = (float)(tmp_height + 10);
			y -= (hh - tmp_height) / 2;

			wxButton* field = new wxButton(this, (wxFunction)FormReset, (char*)field_value.data(), (int)x, (int)y, (int)ww, (int)hh);
			//field->GetSize(&ww, &hh);
#ifndef wx_motif
			field->Enable(!disabled);
#endif
			form_objects.Put((char*)field_name.data(), field);
			if (!IsCompiling()) Hold(new HtmlFormItem(this, x, y, ww, hh, field));

			y += (hh - tmp_height) / 2;
			x += ww;
			if (hh > max_h) max_h = hh;
		}
		else if (!strcmpi(field_type.data(), "BUTTON"))
		{
			new_group = 1;
			if (!field_value.length())
				field_value = "Button";

			float ww = (float)(field_value.length() * tmp_width + 20);
			float hh = (float)(tmp_height + 10);
			y -= (hh - tmp_height) / 2;

			wxButton* field = new wxButton(this, (wxFunction)FormButton, (char*)field_value.data(), (int)x, (int)y, (int)ww, (int)hh);
			//field->GetSize(&ww, &hh);
#ifndef wx_motif
			field->Enable(!disabled);
#endif
			form_objects.Put((char*)field_name.data(), field);
			if (!IsCompiling()) Hold(new HtmlFormItem(this, x, y, ww, hh, field));

			y += (hh - tmp_height) / 2;
			x += ww;
			if (hh > max_h) max_h = hh;
		}
		else if (!strcmpi(field_type.data(), "FILE"))
		{
			new_group = 1;
			float ww = (float)((size?(size)+1:(field_value.length()?field_value.length()+1:20)) * tmp_width + 20);
			float hh = (float)(tmp_height + 10);
			y -= (hh - tmp_height) / 2;

			wxItem* field = new wxText(this, 0, "", (char*)field_value.data(), (int)x, (int)y, (int)ww, (int)hh, (disabled?wxREADONLY:0));
			//field->GetSize(&ww, &hh);
			form_objects.Put((char*)field_name.data(), field);
			if (!IsCompiling()) Hold(new HtmlFormItem(this, x, y, ww, hh, field));

			y += (hh - tmp_height) / 2;
			x += ww;
			hh += 5; if (hh > max_h) max_h = hh;

			if (!field_value.length())
				field_value = "Reset Form";

			ww = (float)(field_value.length() * tmp_width + 20);
			hh = (float)(tmp_height + 10);
			y -= (hh - tmp_height) / 2;
			x += 10;
			string tmp_name = field_name + "_Browse";

			field = new wxButton(this, (wxFunction)FormBrowse, "Browse...", (int)x, (int)y, (int)ww, (int)hh);
			//field->GetSize(&ww, &hh);
			wxItem* tmp_field = (wxItem*)form_objects.Get((char*)field_name.data());
			field->SetClientData((char*)tmp_field);
#ifndef wx_motif
			field->Enable(!disabled);
#endif
			form_objects.Put((char*)tmp_name.data(), field);
			if (!IsCompiling()) Hold(new HtmlFormItem(this, x, y, ww, hh, field));

			y += (hh - tmp_height) / 2;
			x += ww;
			if (hh > max_h) max_h = hh;
		}
		else
		{
			float ww = (float)((size?(size)+1:(field_value.length()?field_value.length()+1:30)) * tmp_width + 10);
			float hh = (float)(tmp_height + 10);
			y -= (hh - tmp_height) / 2;

			wxText* field = new wxText(this, 0, "", (char*)field_value.data(), (int)x, (int)y, (int)ww, (int)hh, (disabled?wxREADONLY:0));
			//field->GetSize(&ww, &hh);
			form_objects.Put((char*)field_name.data(), field);
			if (!IsCompiling()) Hold(new HtmlFormItem(this, x, y, ww, hh, field));

			y += (hh - tmp_height) / 2;
			x += ww;
			hh += 5; if (hh > max_h) max_h = hh;
		}
	}
	else if (forming && !strncmpi(tag, "TEXTAREA", 8))
	{
		new_group = 1;
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		string key;
		string name;
		const char* ptr = tag+8;
		Bool disabled = FALSE, wrap = FALSE;
		int cols = 30, rows = 1;
		quiet = 1;

		while (GetAttributeValuePair(ptr, key, name) && (dc == GetDC()))
		{
			if (!strcmpi(key.data(), "NAME"))
			{
				textarea_name = name;
			}
			else if (!strcmpi(key.data(), "DISABLED"))
			{
				disabled = TRUE;
			}
			else if (!strcmpi(key.data(), "ROWS"))
			{
				rows = atoi(name.data());
			}
			else if (!strcmpi(key.data(), "COLS"))
			{
				cols = atoi(name.data());
			}
			else if (!strcmpi(key.data(), "WRAP"))
			{
				wrap = TRUE;
			}
			else if (!strcmpi(key.data(), "CLEAR"))
			{
				if (left_limit_y) y = left_limit_y;
				left_limit_y = 0.0F; left_floating = right_floating = 0;
				x = left_margin = DEFAULT_LEFT_MARGIN;
			}
		}

		float ww = (float)((cols * tmp_width) + 5);
		float hh = (float)(rows * (tmp_height + 5));

		wxText* field = new wxMultiText(this, 0, "", "", (int)x, (int)y, (int)ww, (int)hh, wxTE_PROCESS_ENTER|(disabled?wxREADONLY:(wrap?0:wxHSCROLL)));
		//field->GetSize(&ww, &hh);
		field->SetFont(wxTheFontList->FindOrCreateFont(basefont, wxMODERN, style, weight));
		form_objects.Put((char*)textarea_name.data(), field);
		if (!IsCompiling()) Hold(new HtmlFormItem(this, x, y, ww, hh, field));

		x += ww;
		hh += 5; if (hh > max_h) max_h = hh;
	}
	else if (forming && !strcmpi(tag, "/TEXTAREA"))
	{
		Hold(new HtmlMarkup(this, x, y, tag));
		wxText* field = (wxText*)form_objects.Get((char*)textarea_name.data());
		if (field) field->SetValue((char*)buf);
		quiet = 0;
	}
	else if (forming && !strncmpi(tag, "SELECT", 6))
	{
		new_group = 1;
		if (IsCompiling()) Hold(new HtmlMarkup(this, x, y, tag));
		string key;
		string name;
		const char* ptr = tag+6;
		Bool disabled = FALSE;

		option_count = 0;
		choice_selected = 0;
		max_width = 0;
		last_value = "";

		while (GetAttributeValuePair(ptr, key, name) && (dc == GetDC()))
		{
			if (!strcmpi(key.data(), "NAME"))
			{
				textarea_name = name;
			}
			else if (!strcmpi(key.data(), "DISABLED"))
			{
				disabled = TRUE;
			}
			else if (!strcmpi(key.data(), "CLEAR"))
			{
				if (left_limit_y) y = left_limit_y;
				left_limit_y = 0.0F; left_floating = right_floating = 0;
				x = left_margin = DEFAULT_LEFT_MARGIN;
			}
		}

		float ww = (float)((20 * tmp_width) + 5);
		float hh = (float)(tmp_height + 10);

		wxChoice* field = new wxChoice(this, 0, "", (int)x, (int)y, (int)ww, -1);
		//field->GetSize(&ww, &hh);
		form_objects.Put((char*)textarea_name.data(), field);
		if (!IsCompiling()) Hold(new HtmlFormItem(this, x, y, ww, hh, field));

		quiet = 1;
	}
	else if (forming && !strncmpi(tag, "OPTION", 6))
	{
		Hold(new HtmlMarkup(this, x, y, tag));
		string key, name;
		const char* ptr = tag+6;

		wxChoice* field = (wxChoice*)form_objects.Get((char*)textarea_name.data());
		if (option_count && field)
		{
			max_width = wxMax(max_width, (int)strlen(buf));
			field->Append(buf);
			form_values.Put(buf, (wxObject*)new string(last_value));
		}

		last_value = "";

		while (GetAttributeValuePair(ptr, key, name) && (dc == GetDC()))
		{
			if (!strcmpi(key.data(), "SELECTED"))
			{
				choice_selected = option_count;
			}
			else if (!strcmpi(key.data(), "VALUE"))
			{
				last_value = name;
			}
		}

		option_count++;
	}
	else if (forming && !strcmpi(tag, "/SELECT"))
	{
		Hold(new HtmlMarkup(this, x, y, tag));
		wxChoice* field = (wxChoice*)form_objects.Get((char*)textarea_name.data());

		if (option_count && field)
		{
			max_width = wxMax(max_width, (int)strlen(buf));

			field->SetSelection(choice_selected);
			field->SetClientData((char*)choice_selected);
			field->Append(buf);
			form_values.Put(buf, (wxObject*)new string(last_value));
		}

		float ww = (float)((20 * tmp_width) + 5);
		float hh = (float)(tmp_height + 10);
		ww = (float)(((max_width+4) * tmp_width) + 5);
		if (field) field->SetSize(ABSOLUTE_X(x), ABSOLUTE_Y(y), (int)ww, (int)hh);
		x += ww;
		hh += 5; if (hh > max_h) max_h = hh;
		quiet = 0;

	}
	else if (forming && !strcmpi(tag, "/FORM"))
	{
		Hold(new HtmlMarkup(this, x, y, tag));
		Release(dc);
		forming--;
		quiet = 0;
	}
	else
		return 0;

	return 1;
#else
	return 0;
#endif
}


