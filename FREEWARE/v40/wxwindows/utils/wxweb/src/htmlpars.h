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

#ifndef HTMLPARS_H
#define HTMLPARS_H

class HtmlObject
{
public:

	HtmlObject(wxHtml* canvas, float _x, float _y, float _w=0.0, float _h=0.0);
	virtual ~HtmlObject() {};

	void SetFont(wxDC* dc);
	virtual void Draw(wxDC* dc);
	void Format(string& text) const;
	virtual void Output(string& text) const;
	Bool IsBounded(float pos_x, float pos_y, float tag_height) const;
	virtual HtmlObject* Clone() const = 0;

#if ALLOW_EDIT_MODE
	virtual Bool Edit(wxKeyEvent& event);
	virtual void Properties() {};
	virtual void Caret(float /*x*/, float /*y*/) {};
	virtual void SetCursor(float pos_x, float pos_y);
	void SetSelected(Bool state) { selected = state; };
	Bool IsSelected() const { return selected; };
#endif

	float GetLeft() const { return x; };
	float GetTop() const { return y; };
	float GetWidth() const { return w; };
	float GetHeight() const { return h; };
	float GetRight() const { return x + w - 1 ; };
	float GetBottom() const { return y + h - 1 ; };
	int GetFontSize() const { return fsz - canvas->basefont; };
	unsigned GetMode() const { return mode; };
	wxColour* GetColour() const { return colour; };
	wxHtml* GetCanvas() const { return canvas; };

	virtual Bool IsMarkup() const { return FALSE; };
	virtual Bool IsFloat() const { return FALSE; };
	virtual Bool IsBreak() const { return FALSE; };
	virtual Bool IsParagraph() const { return FALSE; };
	virtual Bool IsText() const { return FALSE; };
	virtual Bool IsListIndex() const { return FALSE; };
	virtual Bool IsTable() const { return FALSE; };
	virtual Bool IsImage() const { return FALSE; };
	virtual Bool IsAnchor() const { return FALSE; };
	virtual Bool IsHidden() const { return FALSE; };
	virtual Bool NewLine() const { return FALSE; };
	virtual Bool EndLine() const { return FALSE; };

	void SetLeft(float _x) { x = _x; };
	void SetTop(float _y) { y = _y; };
	void SetWidth(float _w) { w = _w; };
	void SetHeight(float _h) { h = _h; };
	void SetFontSize(int _fsz) { fsz = _fsz + canvas->basefont; };
	void SetMode(unsigned _mode) { mode = _mode; };
	void SetColour(wxColour* _colour) { colour = _colour; };

protected:

	float x, y, w, h;
	float last_x, last_y;
	wxHtml* canvas;
	int fsz, ignore, preformatted, family;
	unsigned mode;
	wxColour* colour;
#if ALLOW_EDIT_MODE
	Bool selected;
#endif
};

class HtmlMarkup : public HtmlObject
{
public:

	HtmlMarkup(wxHtml* canvas, float x, float y, const char* _src) :
		HtmlObject(canvas, x, y), src(_src) {};

	void Output(string& text) const;
	Bool IsMarkup() const { return TRUE; };
	HtmlObject* Clone() const { return new HtmlMarkup(*this); };

private:

	string src;
};

class HtmlText : public HtmlObject
{
public:

	HtmlText(wxHtml* canvas, float x, float y, float w, float h,
			const char* _src) :
		HtmlObject(canvas, x, y, w, h), src(_src) {};

	void Draw(wxDC* dc);
	void Output(string& text) const;
	const string& GetText() const { return src; };
	void SetText(const string& s, float _w) { src = s; w = _w; };
	Bool IsText() const { return TRUE; };
	HtmlObject* Clone() const { return new HtmlText(*this); };

#if ALLOW_EDIT_MODE
	Bool Edit(wxKeyEvent& event);
	void Properties();
	void Select(float pos_x, float pos_y, Bool start);
	void Caret(float pos_x, float pos_y);
	void SetCursor(float pos_x, float pos_y);
	size_t Index(float& pos_x, float& pos_y);
	void SelectAll();
	void Snip(ListOfIterator&);
#endif

protected:

	string src;
#if ALLOW_EDIT_MODE
	size_t pos, snip_from, snip_to;
#endif
};

class HtmlHeading : public HtmlObject
{
public:

	HtmlHeading(wxHtml* canvas, float x, float y, int _level,
			const string& _align) :
		HtmlObject(canvas, x, y), level(_level), align(_align) {};

	void Output(string& text) const;
	HtmlObject* Clone() const { return new HtmlHeading(*this); };
	Bool NewLine() const { return TRUE; };

protected:

	int level;
	string align;
};

class HtmlEndHeading : public HtmlObject
{
public:

	HtmlEndHeading(wxHtml* canvas, float x, float y, int _level) :
		HtmlObject(canvas, x, y), level(_level) {};

	void Output(string& text) const;
	HtmlObject* Clone() const { return new HtmlEndHeading(*this); };
	Bool EndLine() const { return TRUE; };

protected:

	int level;
};

class HtmlAnchor : public HtmlObject
{
public:

	HtmlAnchor(wxHtml* canvas, float x, float y,
			const string& _src, Bool _isname, const string& _target) :
		HtmlObject(canvas, x, y), src(_src), target(_target), isname(_isname)
		{
			canvas->addressing++;
		};

	~HtmlAnchor() { canvas->addressing--; };

	void Draw(wxDC* dc);
	void Output(string& text) const;
	Bool IsAnchor() const { return TRUE; };
	virtual Bool IsSiteAnchor() const { return FALSE; };
	HtmlObject* Clone() const { return new HtmlAnchor(*this); };

private:

	wxHtml::Anchor* anchor;
	string src, target;
	Bool isname;

	friend class Table;
};

#if ALLOW_SITE_MODE
class HtmlSiteAnchor : public HtmlAnchor
{
public:

	HtmlSiteAnchor(wxHtml* canvas, float x, float y, const string& _src) :
		HtmlAnchor(canvas, x, y, _src, FALSE, "")
		{};
	~HtmlSiteAnchor();

	Bool IsSiteAnchor() const { return TRUE; };
	HtmlObject* Clone() const { return new HtmlAnchor(*this); };

private:

	ListOf attributes, values;
	string title, bgcolor, text, background, filename;
	string link, vlink, alink;
	Bool isobject;

	friend class wxHtml;
};
#endif

class HtmlDeanchor : public HtmlObject
{
public:

	HtmlDeanchor(wxHtml* canvas, float x, float y) :
		HtmlObject(canvas, x, y) { max_h = canvas->addressing_max_h; };

	void Draw(wxDC* dc);
	void Output(string& text) const;
	HtmlObject* Clone() const { return new HtmlDeanchor(*this); };

	float max_h;
};

class HtmlMarquee : public HtmlObject
{
public:

	HtmlMarquee(wxHtml* canvas, float x, float y, float w, float h) :
		HtmlObject(canvas, x, y, w, h) {};

	void Draw(wxDC* /*dc*/) {};
	void Output(string& text) const;
	HtmlObject* Clone() const { return new HtmlMarquee(*this); };

#if ALLOW_EDIT_MODE
	void Properties();
#endif

};

class HtmlImage : public HtmlObject
{
public:

	HtmlImage(wxHtml* canvas, float x, float y, float w, float h,
			const string& _alt, const string& _src,
				const string& _dynsrc, int _loop, Bool _ismap,
					const string& _usemap, int _align,
						int _border) :
		HtmlObject(canvas, x, y, w, h), alt(_alt), src(_src),
				dynsrc(_dynsrc), loop(_loop), ismap(_ismap),
					usemap(_usemap), align(_align),
						border(_border) {};

	void Draw(wxDC* dc);
	void Output(string& text) const;
	Bool IsImage() const { return TRUE; };
	Bool IsLeftAligned() const { return align == 4; };
	Bool IsRightAligned() const { return align == 5; };
	HtmlObject* Clone() const { return new HtmlImage(*this); };

#if ALLOW_EDIT_MODE
	void Properties();
	void Adjust();
	void SetCursor(float pos_x, float pos_y);
#endif

	Bool IsFloat() const { return align >= 4; };

protected:

	string alt, src, dynsrc;
	int loop;
	Bool ismap;
	string usemap;
	int align;
	int border;

	friend class ImageAttributes;
};

class HtmlList : public HtmlObject
{
public:

	HtmlList(wxHtml* canvas, float x, float y, int _type) :
		HtmlObject(canvas, x, y), type(_type) {};

	void Output(string& text) const;
	HtmlObject* Clone() const { return new HtmlList(*this); };
	Bool NewLine() const { return TRUE; };

private:

	int type;
};

class HtmlEndList : public HtmlObject
{
public:

	HtmlEndList(wxHtml* canvas, float x, float y, int _type) :
		HtmlObject(canvas, x, y), type(_type) {};

	void Output(string& text) const;
	HtmlObject* Clone() const { return new HtmlEndList(*this); };
	Bool EndLine() const { return TRUE; };

private:

	int type;
};

class HtmlListIndex : public HtmlObject
{
public:

	HtmlListIndex(wxHtml* canvas, float x, float y, float w, float h) :
		HtmlObject(canvas, x, y, w, h) {};

	void Output(string& text) const;
	Bool IsListIndex() const { return TRUE; };
	HtmlObject* Clone() const { return new HtmlListIndex(*this); };
	Bool NewLine() const { return TRUE; };
};

class HtmlOrderedListIndex : public HtmlListIndex
{
public:

	HtmlOrderedListIndex(wxHtml* canvas, float x, float y, float w, float h,
			const string& _src) :
		HtmlListIndex(canvas, x, y, w, h),
			src(_src) {};

	void Draw(wxDC* dc);
	void Output(string& text) const;
	HtmlObject* Clone() const { return new HtmlOrderedListIndex(*this); };

private:

	string src;
};

class HtmlUnorderedListIndex : public HtmlListIndex
{
public:

	HtmlUnorderedListIndex(wxHtml* canvas, float x, float y, float w, float h) :
		HtmlListIndex(canvas, x, y, w, h)
			{};

	void Draw(wxDC* dc);
	void Output(string& text) const;
	HtmlObject* Clone() const { return new HtmlUnorderedListIndex(*this); };
};

class HtmlHorizontalRule : public HtmlObject
{
public:

	HtmlHorizontalRule(wxHtml* canvas, float x, float y, float w, float h) :
		HtmlObject(canvas, x, y, w, h) {};

	void Draw(wxDC* dc);
	void Output(string& text) const;
	HtmlObject* Clone() const { return new HtmlHorizontalRule(*this); };
};

class HtmlBreak : public HtmlObject
{
public:

	HtmlBreak(wxHtml* canvas, float x, float y) :
		HtmlObject(canvas, x, y, 0.0F, canvas->char_height) {};

	void Draw(wxDC* dc);
	void Output(string& text) const;
	Bool IsBreak() const { return TRUE; };
	HtmlObject* Clone() const { return new HtmlBreak(*this); };
	Bool EndLine() const { return TRUE; };
};

class HtmlParagraph : public HtmlObject
{
public:

	HtmlParagraph(wxHtml* canvas, float x, float y) :
		HtmlObject(canvas, x, y, 0.0F, canvas->char_height)
	{
		center = canvas->p_center;
		right = canvas->p_right;
	};

	void Draw(wxDC* dc);
	void Output(string& text) const;
	Bool IsParagraph() const { return TRUE; };
	HtmlObject* Clone() const { return new HtmlParagraph(*this); };
	Bool NewLine() const { return TRUE; };

#if ALLOW_EDIT_MODE
	void Properties();
#endif

private:

	int center, right;

	friend class ParagraphAttributes;
};

class HtmlEndParagraph : public HtmlObject
{
public:

	HtmlEndParagraph(wxHtml* canvas, float x, float y) :
		HtmlObject(canvas, x, y) {};

	void Draw(wxDC* dc);
	void Output(string& text) const;
	HtmlObject* Clone() const { return new HtmlEndParagraph(*this); };
	Bool NewLine() const { return TRUE; };
};

class HtmlFormItem : public HtmlObject
{
public:

	HtmlFormItem(wxHtml* canvas, float x, float y, float w, float h, wxItem* item) :
		HtmlObject(canvas, x, y, w, h), field(item) {};

	void Draw(wxDC* dc);
	void Output(string& text) const;
	HtmlObject* Clone() const { return new HtmlFormItem(*this); };

private:

	wxItem* field;
};

class HtmlHiddenFormItem : public HtmlFormItem
{
public:

	HtmlHiddenFormItem(wxHtml* canvas, float x, float y, float w, float h, wxItem* item) :
		HtmlFormItem(canvas, x, y, w, h, item) {};

	Bool IsHidden() const { return TRUE; };
	HtmlObject* Clone() const { return new HtmlHiddenFormItem(*this); };
};

class HtmlTable : public HtmlObject
{
public:

	HtmlTable(wxHtml* canvas, float x, float y, Table* t) :
		HtmlObject(canvas, x, y), table(t) {};

	~HtmlTable() { canvas->TableDelete(table); };

	void Draw(wxDC* dc);
	void Output(string& text) const;
	Bool IsTable() const { return TRUE; };
	Table* GetTable() const { return table; };
	HtmlObject* Clone() const { return new HtmlTable(*this); };

private:

	Table* table;
};

class HtmlEndTable : public HtmlObject
{
public:

	HtmlEndTable(wxHtml* canvas, float x, float y) :
		HtmlObject(canvas, x, y) {};

	void Output(string& text) const;
	HtmlObject* Clone() const { return new HtmlEndTable(*this); };
};

class HtmlTableRow : public HtmlObject
{
public:

	HtmlTableRow(wxHtml* canvas, float x, float y, TableRow* r) :
		HtmlObject(canvas, x, y), row(r) {};

	void Output(string& text) const;
	HtmlObject* Clone() const { return new HtmlTableRow(*this); };
	Bool NewLine() const { return TRUE; };

private:

	TableRow* row;
};

class HtmlEndTableRow : public HtmlObject
{
public:

	HtmlEndTableRow(wxHtml* canvas, float x, float y) :
		HtmlObject(canvas, x, y) {};

	void Output(string& text) const;
	HtmlObject* Clone() const { return new HtmlEndTableRow(*this); };
	Bool EndLine() const { return TRUE; };
};

class HtmlTableCell : public HtmlObject
{
public:

	HtmlTableCell(wxHtml* canvas, float x, float y, TableCell* c) :
		HtmlObject(canvas, x, y), cell(c) {};

	void Output(string& text) const;
	HtmlObject* Clone() const { return new HtmlTableCell(*this); };

private:

	TableCell* cell;
};

class HtmlEndTableCell : public HtmlObject
{
public:

	HtmlEndTableCell(wxHtml* canvas, float x, float y) :
		HtmlObject(canvas, x, y) {};

	void Output(string& text) const;
	HtmlObject* Clone() const { return new HtmlEndTableCell(*this); };
};

#define _ABSOLUTE_X(x) (int)(x - (canvas->starting_x * VSCROLL_INCREMENT))
#define _ABSOLUTE_Y(y) (int)(y - (canvas->starting_y * VSCROLL_INCREMENT))

#endif


