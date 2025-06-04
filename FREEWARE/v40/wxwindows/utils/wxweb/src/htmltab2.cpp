///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  An HTML canvas for wxWindows.
//
//
// Edit History
//
//	  Started November 1995 by Andrew Davison.
//
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
#include "htmltabl.h"

extern ListOf table_list;

extern const float DEFAULT_BORDER_WIDTH;
extern const float DEFAULT_CELL_SPACING;
extern const float DEFAULT_CELL_PADDING;
extern const float EXTRA_X_SPACING;
extern const float EXTRA_Y_SPACING;

///////////////////////////////////////////////////////////////////////////////

void Table::Process()
{
	float x = Border() + Spacing();
	float y = Border() + Spacing();

	width = 2*Border() + Spacing();
	height = 2*Border() + Spacing();

	ListOfIterator ilist = rows;
	void* a;
	int ok;

	for (ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		TableRow* row = (TableRow*)a;
		int col_idx = 0;

		// Now adjust the position of all the objects that make up
		// a cell based upon calculated cell co-ordinates...

		ListOfIterator ilist2 = row->cells;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			TableCell* cell = (TableCell*)a2;

			// Skip any spanned cols...

			while (col_idx < cell->col_nbr)
				x += max_width[col_idx++] + 2*Padding() + Spacing();

			// Check a spanning cell doesn't overflow it's spanees. If it does
			// then spread the difference over the spanned columns...

			if (cell->col_span > 1)
			{
				float spanned_width = 0.0F;

				for (int i = 0; i < cell->col_span; i++)
					spanned_width += max_width[cell->col_nbr + i];

				if (cell->width > spanned_width)
				{                                                 
					float delta = (cell->width - spanned_width) / cell->col_span;

					for (int i = 0; i < cell->col_span; i++)
						max_width[cell->col_nbr + i] += delta;
				}
			}

			// Allow for spanned rows...

			float cell_width = 0.0F, cell_height = 0.0F;
			ListOfIterator ilist3 = ilist;
			void* a3 = a;

			for (int i = 0; i < cell->col_span; i++)
				cell_width += max_width[cell->col_nbr + i];

			for (int j = 0; j < cell->row_span; j++)
			{
				TableRow* tmp_row = (TableRow*)a3;
				cell_height += tmp_row->max_height + 2*Padding() + Spacing();
				ilist3.Next(a3);
			}

			float _x = x;
			float _y = y;

			if (cell->center)
				_x += (cell_width - cell->width - Padding()) / 2;
			else if (cell->right)
				_x += (cell_width - cell->width - Padding() + EXTRA_X_SPACING);
			else
				_x += Padding() + EXTRA_X_SPACING;

			if (cell->bottom)
				_y += (cell_height - cell->height - Padding() - EXTRA_Y_SPACING);
			else if (cell->top)
				_y += Padding() + EXTRA_Y_SPACING;
			else
				_y += (cell_height - cell->height - Padding()) / 2;

			// Adjust the location of cells as needed...

			cell->Adjust(_x, _y);

			x += max_width[col_idx++] + 2*Padding() + Spacing();
		}

		height += row->max_height + 2*Padding() + Spacing();
		y += row->max_height + 2*Padding() + Spacing();
		x = Border() + Spacing();
	}

	for (int i = 0; i < max_cols; i++)
		width += max_width[i] + 2*Padding() + Spacing();

	y += Border();

	// Now the caption...

	ilist = caption;

	for (ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxHtml::Tag* item = (wxHtml::Tag *)a;
		float max_height = 0.0F;

		ListOfIterator ilist2 = item->objects;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			HtmlObject* obj = (HtmlObject*)a2;
			obj->SetLeft(x + obj->GetLeft());
			obj->SetTop(y + obj->GetTop());
			max_height = wxMax(max_height, obj->GetHeight());
		}

		height += max_height;
		y += max_height;
	}

	html_object->SetWidth(width);
	html_object->SetHeight(height);
}

///////////////////////////////////////////////////////////////////////////////

void Table::PreProcess()
{
	float min_table_width = 2.0F*Border() + Spacing();

	width = 2*Border() + Spacing();
	height = 2*Border() + Spacing();

	ListOfIterator ilist = rows;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		TableRow* row = (TableRow*)a;
		height += row->max_height + 2*Padding() + Spacing();
	}

	for (int i = 0; i < max_cols; i++)
	{
		min_table_width += min_width[i] * 2*Padding() * Spacing();
		width += max_width[i] + 2*Padding() + Spacing();
	}

	if (caption.Count())
	{
		height += 30;
	}

	html_object->SetWidth(min_table_width);
	html_object->SetHeight(height);
}

///////////////////////////////////////////////////////////////////////////////

void Table::DrawTags(wxDC* dc, ListOf& tags)
{
	// Go thru each line...

	ListOfIterator ilist = tags;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxHtml::Tag* item = (wxHtml::Tag *)a;

		// Go thru each object...

		ListOfIterator ilist2 = item->objects;
		void* a2;

		if (item->center || item->p_center)
		{
			float w = 0.0F;

			for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
			{
				HtmlObject* obj = (HtmlObject*)a2;
				w += obj->GetWidth();
			}
		}

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			HtmlObject* obj = (HtmlObject*)a2;
			float save_left = obj->GetLeft();
			float save_top = obj->GetTop();

			obj->SetLeft(html_object->GetLeft()+obj->GetLeft());
			obj->SetTop(html_object->GetTop()+obj->GetTop());
			obj->Draw(dc);

			obj->SetLeft(save_left);
			obj->SetTop(save_top);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void Table::DrawCells(wxDC* dc, ListOf& list)
{
	// Go thru each row...

	ListOfIterator ilist = list;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		TableRow* row = (TableRow*)a;

		// Go thru each cell...

		ListOfIterator ilist2 = row->cells;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			TableCell* cell = (TableCell*)a2;
			DrawTags(dc, cell->tags);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

static void DrawBorder(wxDC* dc, float x, float y, float w, float h, float bw, Bool raised)
{
	dc->SetPen(wxThePenList->FindOrCreatePen(raised?"WHITE":"GREY", (int)bw, wxSOLID));
	dc->DrawLine(x, y, x, y+h);
	dc->DrawLine(x, y, x+w, y);

	dc->SetPen(wxThePenList->FindOrCreatePen(raised?"GREY":"WHITE", (int)bw, wxSOLID));
	dc->DrawLine(x, y+h, x+w, y+h);
	dc->DrawLine(x+w, y, x+w, y+h);
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::TableDraw(wxDC* dc, Table* t, float at_x, float at_y)
{
	if (!t->border_width)
	{
		t->DrawCells(dc, t->rows);
		t->DrawTags(dc, t->caption);
		return;
	}

	// Draw table border...

	DrawBorder(GetDC(), at_x, at_y, t->width, t->height, t->border_width, TRUE);

	float xx = at_x + t->Border() + t->Spacing();
	float yy = at_y + t->Border() + t->Spacing();

	// Go thru each row...

	ListOfIterator ilist = t->rows;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		TableRow* row = (TableRow*)a;
		int col_idx = 0;

		// Go thru each cell...

		ListOfIterator ilist2 = row->cells;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			TableCell* cell = (TableCell*)a2;

			// Skip any spanned cols...

			while (col_idx < cell->col_nbr)
				xx += t->max_width[col_idx++] + 2*t->Padding() + t->Spacing();

			// Calculate column span...

			float width = -t->Spacing();

			for (int i = 0; i < cell->col_span; i++)
				width += t->max_width[col_idx++] + 2*t->Padding() + t->Spacing();

			// Calculate row span...

			float height = -t->Spacing();
			ListOfIterator ilist3 = ilist;
			void* a3 = a;

			for (int j = 0; j < cell->row_span; j++)
			{
				TableRow* tmp_row = (TableRow*)a3;
				height += tmp_row->max_height + 2*t->Padding() + t->Spacing();
				ilist3.Next(a3);
			}

			// Clear the cell background...

			if (cell->bg != background)
			{
				dc->SetBrush(wxTheBrushList->FindOrCreateBrush(cell->bg, wxSOLID));
				dc->SetPen(wxThePenList->FindOrCreatePen(background, 1, wxTRANSPARENT));
				dc->DrawRectangle(xx, yy, width, height);
			}

			// Draw the cell border...

			DrawBorder(GetDC(), xx, yy, width, height, 1.0F, FALSE);

			if (cell->bg != background)
			{
				dc->SetBrush(wxTheBrushList->FindOrCreateBrush(background, wxTRANSPARENT));
			}

			xx += width + t->Spacing();
		}

		xx = at_x + t->Border() + t->Spacing();
		yy += row->max_height + 2*t->Padding() + t->Spacing();
	}

	// Now draw the cell contents...

	t->DrawCells(dc, t->rows);
	t->DrawTags(dc, t->caption);
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::TableDelete(Table* t)
{
	// Go thru the caption...

	ListOfIterator ilist = t->caption;
	void* a;
	int ok;

	for (ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxHtml::Tag* item = (wxHtml::Tag *)a;
		DropLine(item->objects);
	}

	// Go thru each row...

	ilist = t->rows;

	for (ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		TableRow* row = (TableRow*)a;

		// Go thru each cell...

		ListOfIterator ilist2 = row->cells;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			TableCell* cell = (TableCell*)a2;

			// Go thru each line...

			ListOfIterator ilist3 = cell->tags;
			void* a3;

			for (int ok3 = ilist3.First(a3); ok3; ok3 = ilist3.Next(a3))
			{
				wxHtml::Tag* item = (wxHtml::Tag *)a3;
				DropLine(item->objects);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void Table::Output(ListOf& tags, string& text)
{
	// Go thru each line...

	ListOfIterator ilist = tags;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxHtml::Tag* item = (wxHtml::Tag *)a;

		// Go thru each object...

		ListOfIterator ilist2 = item->objects;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			HtmlObject* obj = (HtmlObject*)a2;
			obj->Output(text);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::TableOutput(Table* t, string& text)
{
	text += "\n<TABLE";
	
	if (t->spec_width)
	{
		text += " width=";
		text += Number(t->spec_width);
		text += (t->pc ? "%" : "");
	}

	if (t->set_border)
	{
		text += " border=";
		text += Number(t->Border());
	}

	if (t->set_padding)
	{
		text += " cellpadding=";
		text += Number(t->Padding());
	}

	if (t->set_spacing)
	{
		text += " cellspacing=";
		text += Number(t->Spacing());
	}

#if 0
	if (!IsCompiling())
	{
		text += " cols=";
		text+= Number(t->col_idx + 1);
	}
#endif

	text += ">";

	if (t->caption.Count())
	{
		text += "\n<CAPTION";

		if (t->align_top)
			text += " align=top";
		else if (t->align_bottom)
			text += " align=bottom";

		text += ">";

		t->Output(t->caption, text);

		text += "</CAPTION>";
	}

	// Go thru each row...

	ListOfIterator ilist = t->rows;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		TableRow* row = (TableRow*)a;

		text += "\n<TR";
		
		if (row->left || row->center || row->right)
		{
			text += " align=";

			if (row->left)
				text += "left";
			else if (row->center)
				text += "center";
			else if (row->right)
				text += "right";
		}

		if (row->top || row->middle || row->bottom)
		{
			text += " valign=";

			if (row->top)
				text += "top";
			else if (row->middle)
				text += "middle";
			else if (row->bottom)
				text += "bottom";
		}

		text += ">";

		// Go thru each cell...

		ListOfIterator ilist2 = row->cells;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			TableCell* cell = (TableCell*)a2;

			if (cell->is_header)
			{
				text += "\n<TH";
				cell->html_object->Output(text);
			}
			else
				text += "\n<TD";

			if (cell->left || cell->center || cell->right)
			{
				text += " align=";

				if (cell->left)
					text += "left";
				else if (cell->center)
					text += "center";
				else if (cell->right)
					text += "right";
			}

			if (cell->top || cell->middle || cell->bottom)
			{
				text += " valign=";
	
				if (cell->top)
					text += "top";
				else if (cell->middle)
					text += "middle";
				else if (cell->bottom)
					text += "bottom";
			}

			if (cell->col_span > 1)
				text += " colspan=" + Number(cell->col_span);

			if (cell->row_span > 1)
				text += " rowspan=" + Number(cell->row_span);

			if (cell->bgcolor.length())
				text += " bgcolor=\"" + cell->bgcolor + "\"";

			if (t->col_spec_width[cell->col_nbr])
			{
				text += " width=";
				text += Number(t->col_spec_width[cell->col_nbr]);

				if (t->col_pc[cell->col_nbr])
					text += '%';
			}

			if (cell->nowrap)
				text += " nowrap";

			text += ">";

			t->Output(cell->tags, text);

			if (cell->is_header)
			{
				cell->html_end_object->Output(text);
				text += "</TH>";
			}
			else
				text += "</TD>";
		}

		text += "\n</TR>";
	}

	text += "\n</TABLE>\n";
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::ParseTable(wxDC* dc, char*&, char* /*buf*/, char* tag)
{
	static Table* current_table = 0;

	if (!tables)
		return 0;

	int status = 0;

	if (!IsCompiling() && !strncmpi(tag, "TABLE", 5))
	{
		Release(dc);
		tabling++;

		if (current_table)
			table_list.Append(current_table);

		if (!left_floating && left_limit_y) { y = left_limit_y; left_limit_y = 0.0F; }
		if (x != left_margin) y += max_h;
		if (!left_floating) left_margin = default_left_margin;
		x = left_margin;
		y += default_left_margin;
		max_h = 0.0F;

		current_table = new Table;
		current_table->current_row = 0;
		current_table->current_cell= 0;
		current_table->row_idx = -1;
		current_table->col_idx = -1;
		current_table->border_width = 0.0F;
		current_table->cell_padding = DEFAULT_CELL_PADDING;
		current_table->cell_spacing = DEFAULT_CELL_SPACING;
		current_table->max_cols = 0;
		current_table->in_row = 0;
		current_table->in_cell = 0;
		current_table->spec_width = 0.0F;
		current_table->pc = FALSE;
		current_table->set_padding = FALSE;
		current_table->set_spacing = FALSE;
		current_table->saved_x = x;
		current_table->saved_y = y;
		current_table->saved_nowrap = nowrap;
		current_table->saved_left_margin = left_margin;
		current_table->saved_default_left_margin = default_left_margin;
		current_table->align_top = FALSE;
		current_table->align_bottom = FALSE;

		nowrap = 1;

		string key;
		string name;
		const char* ptr = tag+5;

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "BORDER"))
			{
				current_table->set_border = TRUE;

				if (name.length())
					current_table->border_width = (float)atoi(name.data());
				else
					current_table->border_width = DEFAULT_BORDER_WIDTH;
			}
			else if (!strcmpi(key.data(), "CELLPADDING"))
			{
				current_table->set_padding = TRUE;

				if (name.length())
					current_table->cell_padding = (float)atoi(name.data());
				else
					current_table->cell_padding = DEFAULT_CELL_PADDING;
			}
			else if (!strcmpi(key.data(), "CELLSPACING"))
			{
				current_table->set_spacing = TRUE;

				if (name.length())
					current_table->cell_spacing = (float)atoi(name.data());
				else
					current_table->cell_spacing = DEFAULT_CELL_SPACING;
			}
			else if (!strcmpi(key.data(), "WIDTH"))
			{
				current_table->spec_width = (float)atoi(name.data());

				if (name[name.length()-1] == '%')
					current_table->pc = TRUE;
			}
		}

		for (int i = 0; i < HTML_MAX_COLS; i++)
		{
			current_table->row_span[i] = 0;
			current_table->min_width[i] = 0.0F;
			current_table->max_width[i] = 0.0F;
			current_table->col_spec_width[i] = 0.0F;
			current_table->col_pc[i] = FALSE;
		}

		Hold(current_table->html_object = new HtmlTable(this, x, y, current_table));
		Release(dc);
		void* a;
		tags->Tail(a);
		current_table->saved_tag = (Tag*)a;

		SaveState();
		center = p_center = p_right = 0;
		status = 1;
	}

	if (tabling && !strncmpi(tag, "CAPTION", 7))
	{
		string key;
		string name;
		const char* ptr = tag+7;
		Bool align_top = FALSE;
		Bool align_bottom = FALSE;

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "ALIGN"))
			{
				if (!strcmpi(name.data(), "TOP"))
					align_top = TRUE;
				else if (!strcmpi(name.data(), "BOTTOM"))
					align_bottom = TRUE;
			}
		}

		current_table->align_top = align_top;
		current_table->align_bottom = align_bottom;
		current_table->saved_tags = tags;
		tags = &current_table->caption;
		x = y = 0.0F;
		status = 1;
	}

	if (tabling && !strcmpi(tag, "/CAPTION"))
	{
		Release(dc);
		tags = current_table->saved_tags;
		status = 1;
	}

	if (tabling && current_table->in_cell &&
				(!strcmpi(tag, "/TH") ||
					!strcmpi(tag, "/TD") ||
					!strcmpi(tag, "/TR") ||
					!strcmpi(tag, "/TABLE") ||
					!strncmpi(tag, "TR", 2) ||
					!strncmpi(tag, "TD", 2) ||
					!strncmpi(tag, "TH", 2)
				))
	{
		Release(dc);

		current_table->in_cell = FALSE;
		RestoreState();

		TableCell* cell = current_table->current_cell;
		tags = cell->saved_tags;
		cell->GetDimensions(current_table);
			
		Hold(cell->html_end_object = new HtmlEndTableCell(this, x, y));
		Release(dc);
		current_table->current_cell = 0;
		max_h = 0.0F;
		status = 1;
	}

	if (tabling && current_table->in_row &&
				(!strcmpi(tag, "/TR") ||
					!strncmpi(tag, "TR", 2) ||
					!strcmpi(tag, "/TABLE")
				))
	{
		current_table->in_row = FALSE;
		current_table->current_row = 0;
		DropLine(holding_line);
		Hold(new HtmlEndTableRow(this, x, y));
		Release(dc);
		RestoreState();
		status = 1;
	}

	if (tabling && !strncmpi(tag, "TR", 2))
	{
		DropLine(holding_line);

		current_table->col_idx = -1;
		current_table->row_idx++;
		current_table->in_row = TRUE;

		TableRow* row = current_table->current_row = new TableRow;
		row->left = row->center = row->right = FALSE;
		row->top = row->middle = row->bottom = FALSE;
		row->cell_count = 0;
        row->max_height = 0.0F;
        
		string key;
		string name;
		const char* ptr = tag+2;

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "ALIGN") ||
						!strcmpi(key.data(), "HALIGN"))
			{
				if (!strcmpi(name.data(), "LEFT"))
				{
					row->left = TRUE;
					row->center = row->right = FALSE;
				}
				else if (!strcmpi(name.data(), "CENTER"))
				{
					row->center = TRUE;
					row->left = row->right = FALSE;
				}
				else if (!strcmpi(name.data(), "RIGHT"))
				{
					row->right = TRUE;
					row->center = row->left = FALSE;
				}
			}
			else if (!strcmpi(key.data(), "VALIGN"))
			{
				if (!strcmpi(name.data(), "TOP"))
				{
					row->top = TRUE;
					row->middle = row->bottom = FALSE;
				}
				else if (!strcmpi(name.data(), "MIDDLE"))
				{
					row->middle = TRUE;
					row->top = row->bottom = FALSE;
				}
				else if (!strcmpi(name.data(), "BOTTOM"))
				{
					row->bottom = TRUE;
					row->top = row->middle = FALSE;
				}
			}
		}

		current_table->rows.Append(row);
		Hold(new HtmlTableRow(this, x, y, row));
		Release(dc);
		SaveState();
		status = 1;
	}

	if (tabling && current_table->current_row &&
		(!strncmpi(tag, "TH", 2) || !strncmpi(tag, "TD", 2)))
	{
		DropLine(holding_line);

		default_left_margin = 0.0F;
		left_limit_y = right_limit_y = left_margin = 0.0F;
		left_floating = right_floating = 0;
		max_h = 0.0F;

		// Skip over spanned rows...

		while (--current_table->row_span[++current_table->col_idx] > 0)
		{
			current_table->current_row->cell_count++;
		}

		string key;
		string name;
		const char* ptr = tag+2;

		TableCell* cell = current_table->current_cell = new TableCell;
		cell->col_span = cell->row_span = 1;
		cell->width = cell->height = 0.0F;
		cell->left = current_table->current_row->left;
		cell->center = current_table->current_row->center;
		cell->right = current_table->current_row->right;
		cell->top = current_table->current_row->top;
		cell->middle = current_table->current_row->middle;
		cell->bottom = current_table->current_row->bottom;
		cell->bgcolor = "";
		cell->nowrap = FALSE;

		current_table->in_cell = TRUE;

		if (!strncmpi(tag, "TH", 2))
		{
			cell->center = TRUE;
			cell->is_header = TRUE;
		}
		else
			cell->is_header = FALSE;

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "COLSPAN"))
			{
				cell->col_span = atoi(name.data());
			}
			else if (!strcmpi(key.data(), "ROWSPAN"))
			{
				cell->row_span = atoi(name.data());
			}
			else if (!strcmpi(key.data(), "BGCOLOR"))
			{
				cell->bgcolor = name;
			}
			else if (!strcmpi(key.data(), "NOWRAP"))
			{
				cell->nowrap = TRUE;
			}
			else if (!strcmpi(key.data(), "ALIGN") ||
						!strcmpi(key.data(), "HALIGN"))
			{
				if (!strcmpi(name.data(), "LEFT"))
				{
					cell->left = TRUE;
					cell->center = cell->right = FALSE;
				}
				else if (!strcmpi(name.data(), "CENTER"))
				{
					cell->center = TRUE;
					cell->left = cell->right = FALSE;
				}
				else if (!strcmpi(name.data(), "RIGHT"))
				{
					cell->right = TRUE;
					cell->center = cell->left = FALSE;
				}
			}
			else if (!strcmpi(key.data(), "VALIGN"))
			{
				if (!strcmpi(name.data(), "TOP"))
				{
					cell->top = TRUE;
					cell->middle = cell->bottom = FALSE;
				}
				else if (!strcmpi(name.data(), "MIDDLE"))
				{
					cell->middle = TRUE;
					cell->top = cell->bottom = FALSE;
				}
				else if (!strcmpi(name.data(), "BOTTOM"))
				{
					cell->bottom = TRUE;
					cell->top = cell->middle = FALSE;
				}
			}
			else if (!strcmpi(key.data(), "WIDTH"))
			{
				current_table->col_spec_width[current_table->col_idx] = (float)atoi(name.data());

				if (name[name.length()-1] == '%')
					current_table->col_pc[current_table->col_idx] = TRUE;
			}
		}

		if (cell->bgcolor.length())
		{
			string localname;

			if (cell->bgcolor[0] == '#')
			{
				unsigned long rgb = strtoul((cell->bgcolor.data())+1, 0, 16);
				rgb = FindMatchingColour(rgb);
				cell->bg = new wxColour(char((rgb>>16)&0xFF),char((rgb>>8)&0xFF),char(rgb&0xFF));
			}
			else
			{
				cell->bg = HtmlColour(cell->bgcolor.data());
			}

			SetBackground(wxTheBrushList->FindOrCreateBrush(cell->bg, wxSOLID));
		}
		else
			cell->bg = background;

		SaveState();

		if (!strncmpi(tag, "TH", 2))
		{
			mode |= BOLD;
			dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
				(mode & FIXED ? wxMODERN : family),
				(mode & ITALIC ? wxITALIC : style),
				(mode & BOLD ? wxBOLD : weight),
				(mode & UNDERLINE ? TRUE : FALSE)));
		}

		current_table->current_row->cells.Append(cell);
		current_table->row_span[current_table->col_idx] = cell->row_span;

		Hold(cell->html_object = new HtmlTableCell(this, x, y, cell));
		Release(dc);
		cell->col_nbr = current_table->col_idx;
		cell->saved_tags = tags;
		tags = &cell->tags;
		current_table->col_idx += cell->col_span - 1;
		current_table->max_cols = wxMax(current_table->max_cols, current_table->col_idx+1);
		x = y = 0.0F;
		status = 1;
	}

	if (tabling && !strcmpi(tag, "/TABLE"))
	{
		RestoreState();
		tabling--;

		if (tabling) current_table->PreProcess();
		float width = current_table->width;

		// Wait till any nested tables are done, then recursively
		// do layout...

		if (!tabling)
		{
			current_table->AutoLayout(this, (right_margin - left_margin) - 2*DEFAULT_LEFT_MARGIN);
			current_table->Process();
			current_table->html_object->SetLeft(current_table->saved_x);
			current_table->html_object->SetTop(current_table->saved_y);
			x = current_table->saved_x + current_table->width;
			y = current_table->saved_y + current_table->height;

			nowrap = current_table->saved_nowrap;
			left_margin = current_table->saved_left_margin;
			default_left_margin = current_table->saved_default_left_margin;
		}

		Hold(new HtmlEndTable(this, x, y));
		Release(dc);

		// Now draw it...

		if (!tabling) DrawTags(dc, current_table->saved_tag->objects);

		void* a;
		if (table_list.Remove(a))
		{
			current_table = (Table*)a;

			current_table->max_width[current_table->col_idx] =
				wxMax(current_table->max_width[current_table->col_idx], width);
		}
		else
			current_table = 0;

		SetBackground(wxTheBrushList->FindOrCreateBrush(background, wxSOLID));
		SetBrush(wxTheBrushList->FindOrCreateBrush(background, wxTRANSPARENT));
		overflow = 1;
		status = 1;
	}

	return status;
}


