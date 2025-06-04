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

#ifndef HTMLEDIT_H
#define HTMLEDIT_H

class HtmlAttributes : public wxDialogBox
{
public:

	HtmlAttributes(wxHtml* canvas, const char* title);
	virtual ~HtmlAttributes() {};

protected:

	static void AttrOK(wxButton& button, wxCommandEvent&);
	static void AttrCancel(wxButton& button, wxCommandEvent&);

	void Begin();
	void End();
	virtual void OnOK() {};
	virtual void OnCancel() {};

	wxButton* ok;
	wxButton* cancel;
	wxHtml* canvas;
	wxCheckBox* center;
};

class TextAttributes : public HtmlAttributes
{
public:

  TextAttributes(wxHtml* canvas);

private:

	void OnOK();

	wxRadioBox* font_size;
	wxCheckBox* bold;
	wxCheckBox* italic;
	wxCheckBox* underline;
	wxCheckBox* strike;
};

class BodyAttributes : public HtmlAttributes
{
public:

  BodyAttributes(wxHtml* canvas);

private:

	void OnOK();

	wxText* title;
	wxText* background;
	wxText* bgcolor;
	wxText* text;
	wxText* bgsound;
	wxText* loop;
};

class MarqueeAttributes : public HtmlAttributes
{
public:

  MarqueeAttributes(wxHtml* canvas);

private:

	void OnOK();

	wxText* fgcolor;
	wxText* bgcolor;
	wxText* text;
};

class ImageAttributes : public HtmlAttributes
{
public:

  ImageAttributes(wxHtml* canvas);

private:

	static void ImageMap(wxButton& button, wxCommandEvent&);
	static void ImageAspect(wxButton& button, wxCommandEvent&);
	void OnOK();

	wxRadioBox* alignment;
	wxText* src;
	wxText* dynsrc;
	wxText* loop;
	wxText* usemap;
	wxText* width;
	wxText* height;
	wxCheckBox* ismap;
	wxButton* imagemap;
	wxFrame* grid;
	wxButton* aspect;
	float aspect_w, aspect_h;

	friend class GridFrame;
};

class ParagraphAttributes : public HtmlAttributes
{
public:

  ParagraphAttributes(wxHtml* canvas);

private:

	void OnOK();
};

#endif


