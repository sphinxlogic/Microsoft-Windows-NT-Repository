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

#include "wxhtml.h"
#include "htmlpars.h"

class TableCell
{
public:

	HtmlObject* html_object;
	HtmlObject* html_end_object;
	wxColour* bg;
	ListOf* saved_tags;
	string bgcolor;
	ListOf tags;
	float width, height;
	int col_span, row_span, col_nbr;
	Bool is_header, nowrap;
	Bool left, center, right;
	Bool top, middle, bottom;

	void GetDimensions(Table*);
	void Adjust(float x, float y);
};

class TableRow
{
public:

	ListOf cells;
	float max_height;
	int cell_count;
	Bool left, center, right;
	Bool top, middle, bottom;
};

#define HTML_MAX_COLS 256

class Table
{    
public:

	wxHtml::Tag* saved_tag;
	HtmlTable* html_object;
	TableCell* current_cell;
	TableRow* current_row;
	ListOf rows, caption;
	ListOf* saved_tags;
	wxMemoryDC mem_dc;
	float saved_x, saved_y, width, height, spec_width;
	float saved_left_margin, saved_default_left_margin;
	float border_width, cell_spacing, cell_padding;
	float min_width[HTML_MAX_COLS], max_width[HTML_MAX_COLS], col_spec_width[HTML_MAX_COLS];
	Bool col_pc[HTML_MAX_COLS], pc;
	Bool in_row, in_cell, align_top, align_bottom;
	int set_border, set_padding, set_spacing;
	int row_span[HTML_MAX_COLS], row_idx, col_idx, max_cols;
	Bool saved_nowrap;
	
	inline float Spacing() const { return cell_spacing; };
	inline float Padding() const { return cell_padding; };
	inline float Border() const { return border_width; };

	void Output(ListOf& tags, string& text);
	void DrawTags(wxDC* dc, ListOf& tags);
	void DrawCells(wxDC*, ListOf&);
	void PreProcess();
	void Process();
	void AutoLayout(wxHtml*, float drawing_width);
};

