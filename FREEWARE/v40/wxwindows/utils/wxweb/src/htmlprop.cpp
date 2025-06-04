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

#include "wxhtml.h"
#include "htmlpars.h"
#include "htmledit.h"

#if ALLOW_EDIT_MODE

///////////////////////////////////////////////////////////////////////////////

class GridPanel;

class GridFrame : public wxFrame
{
public:

	GridFrame(ImageAttributes* attr, int x, int y, int w, int h, const string&);

private:

	static void GridOK(wxButton& button, wxCommandEvent&);
	static void GridCancel(wxButton& button, wxCommandEvent&);
	static void GridRedraw(wxButton& button, wxCommandEvent&);
	void Display();

	wxButton* ok;
	wxButton* cancel;
	wxText* grids;
	wxButton* redraw;
	string localname;
	ImageAttributes* attr;
	GridPanel* panel;
	int w, h;

   friend class GridPanel;
};

class GridPanel : public wxPanel
{
public:

	GridPanel(GridFrame* _parent, int x, int y, int w, int h) :
			wxPanel(_parent, x, y, w, h, wxUSER_COLOURS), parent(_parent) {};

private:

	void OnEvent(wxMouseEvent& event);
	void OnPaint();

	GridFrame* parent;
};

void GridPanel::OnPaint()
{
	parent->Display();
}

void GridFrame::GridOK(wxButton& button, wxCommandEvent&)
{
	GridFrame* grid = (GridFrame*)(button.GetParent()->GetParent());
	delete grid->panel;
	delete grid;
}

void GridFrame::GridCancel(wxButton& button, wxCommandEvent&)
{
	GridFrame* grid = (GridFrame*)(button.GetParent()->GetParent());
	delete grid->panel;
	delete grid;
}

void GridFrame::GridRedraw(wxButton& button, wxCommandEvent&)
{
	GridFrame* grid = (GridFrame*)(button.GetParent()->GetParent());
	grid->Refresh();
}

GridFrame::GridFrame(ImageAttributes* _attr, int _x, int _y, int _w, int _h, const string& _localname) :
	wxFrame(0, "Imagemap editor...", _x, _y, _w+20, _h+100)
{
	localname = _localname;
	w = _w, h = _h;
	attr = _attr;

	// Make room for the image...

	panel = new GridPanel(this, _x, _y, _w+20, _h+100);
	wxColour* background = wxTheColourDatabase->FindColour("LIGHT GREY");
	panel->SetBrush(wxTheBrushList->FindOrCreateBrush(background, wxTRANSPARENT));
	panel->Clear();

	for (int i = 0; i < h/panel->GetVerticalSpacing(); i++)
		panel->NewLine();

	panel->NewLine();

	ok = new wxButton(panel, (wxFunction)GridOK, "OK");
	cancel = new wxButton(panel, (wxFunction)GridCancel, "Cancel");
	grids = new wxText(panel, 0, "Grids", "20 ");
	redraw = new wxButton(panel, (wxFunction)GridRedraw, "Redraw");
	panel->NewLine();

	Centre();
	Show(TRUE);
}

void GridFrame::Display()
{
	float x = 5.0F, y = 5.0F;
	int n = atoi(grids->GetValue());
	if (n < 3) n = 3;
	else if (n > (w/2)) n = (int)w/2;
	int cell_width = w / (n-1);
	int cell_height = h / (n-1);
	grids->SetValue((char*)Number(n).data());
	float _w = (float)w, _h = (float)h;

	attr->canvas->DisplayImage(FALSE, panel->GetDC(), localname, x, y, _w, _h);

	for (int i = 0; i < n; i++)
	{
		float x1 = x + (i * cell_width);

		panel->DrawLine(x1, y, x1, y+h);
	}

	for (int j = 0; j < n; j++)
	{
		float y1 = y + (j * cell_height);

		panel->DrawLine(x, y1, x+w, y1);
	}
}

void GridPanel::OnEvent(wxMouseEvent& /*event*/)
{
}

///////////////////////////////////////////////////////////////////////////////

void HtmlText::Properties()
{
	new TextAttributes(canvas);
}

void HtmlMarquee::Properties()
{
	new MarqueeAttributes(canvas);
}

void HtmlImage::Properties()
{
	new ImageAttributes(canvas);
}

void HtmlParagraph::Properties()
{
	new ParagraphAttributes(canvas);
}

///////////////////////////////////////////////////////////////////////////////

void HtmlAttributes::AttrOK(wxButton& button, wxCommandEvent&)
{
	HtmlAttributes* attr = (HtmlAttributes*)(button.GetParent());
	attr->canvas->edit_tag->center = attr->center->GetValue();
	attr->OnOK();
	attr->canvas->Refresh();
	attr->canvas->SetDirty();
	delete attr;
}

void HtmlAttributes::AttrCancel(wxButton& button, wxCommandEvent&)
{
	HtmlAttributes* attr = (HtmlAttributes*)(button.GetParent());
	attr->OnCancel();
	delete attr;
}

HtmlAttributes::HtmlAttributes(wxHtml* _canvas, const char* title) :
	wxDialogBox(_canvas, (char*)title, TRUE),
		canvas(_canvas)
{
}

void HtmlAttributes::Begin()
{
}

void HtmlAttributes::End()
{
	center = new wxCheckBox(this, 0, "Center");
	if (canvas->edit_tag) center->SetValue(canvas->edit_tag->center);
	NewLine();

	ok = new wxButton(this, (wxFunction)AttrOK, "OK");
	cancel = new wxButton(this, (wxFunction)AttrCancel, "Cancel");
	Fit();
	Centre();
	Show(TRUE);
}

///////////////////////////////////////////////////////////////////////////////

TextAttributes::TextAttributes(wxHtml* _canvas) :
	HtmlAttributes(_canvas, "Text attributes...")
{
	Begin();

	char* _font_size[] =
	{
		"-2", "-1", "0", "+1", "+2", "+3", "+4", "+5", "+6", "+7", "+8", "+9"
	};

	font_size = new wxRadioBox(this, 0, "Font size", -1, -1, -1, -1, 12, _font_size);
	font_size->SetSelection(canvas->edit_object->GetFontSize()+2);
	NewLine();

	bold = new wxCheckBox(this, 0, "Bold");
	bold->SetValue(canvas->edit_object->GetMode()&wxHtml::BOLD);
	italic = new wxCheckBox(this, 0, "Italic");
	italic->SetValue(canvas->edit_object->GetMode()&wxHtml::ITALIC);
	underline = new wxCheckBox(this, 0, "Underline");
	underline->SetValue(canvas->edit_object->GetMode()&wxHtml::UNDERLINE);
	strike = new wxCheckBox(this, 0, "Strike");
	strike->SetValue(canvas->edit_object->GetMode()&wxHtml::STRIKE);

	End();
}

void TextAttributes::OnOK()
{
	HtmlText* obj = (HtmlText*)canvas->edit_object;
	int fsz = font_size->GetSelection()-2;
	obj->SetFontSize(fsz);

	unsigned mode = 0;
	mode |= (bold->GetValue() ? wxHtml::BOLD : 0);
	mode |= (italic->GetValue() ? wxHtml::ITALIC : 0);
	mode |= (underline->GetValue() ? wxHtml::UNDERLINE : 0);
	mode |= (strike->GetValue() ? wxHtml::STRIKE : 0);
	obj->SetMode(mode);
	obj->HtmlObject::Draw(canvas->GetDC());
	float w, h;
	canvas->GetDC()->GetTextExtent(obj->GetText().data(), &w, &h);
	obj->SetWidth(w);
	obj->SetHeight(h);
}

///////////////////////////////////////////////////////////////////////////////

BodyAttributes::BodyAttributes(wxHtml* _canvas) :
	HtmlAttributes(_canvas, "Body attributes...")
{
	Begin();

	title = new wxText(this, 0, "Title", (char*)canvas->bgtitle.data(), -1, -1, (int)canvas->char_width*60);
	NewLine();

	background = new wxText(this, 0, "Background image", (char*)canvas->bgimage.data(), -1, -1, (int)canvas->char_width*60);
	NewLine();

	bgcolor = new wxText(this, 0, "Background color", (char*)canvas->bgcolor.data(), -1, -1, (int)canvas->char_width*40);
	NewLine();

	text = new wxText(this, 0, "Text color", (char*)canvas->fgcolor.data(), -1, -1, (int)canvas->char_width*40);
	NewLine();

	bgsound = new wxText(this, 0, "Sound", (char*)canvas->bgsound.data(), -1, -1, (int)canvas->char_width*60);
	NewLine();

	string l = Number(canvas->bgloop);
	l += "  ";//l.pad(3-l.length());
	loop = new wxText(this, 0, "Loop (-1 = infinite)", (char*)l.data());
	NewLine();

	End();
}

void BodyAttributes::OnOK()
{
	canvas->bgtitle = title->GetValue();
	canvas->bgimage = background->GetValue();
	canvas->bgcolor = bgcolor->GetValue();
	canvas->fgcolor = text->GetValue();
	canvas->bgsound = bgsound->GetValue();
	canvas->bgloop = atoi(loop->GetValue());
}

///////////////////////////////////////////////////////////////////////////////

MarqueeAttributes::MarqueeAttributes(wxHtml* _canvas) :
	HtmlAttributes(_canvas, "Marquee attributes...")
{
	Begin();

	bgcolor = new wxText(this, 0, "Background color", (char*)canvas->marquee_bgcolor.data(), -1, -1, (int)canvas->char_width*40);

	fgcolor = new wxText(this, 0, "Text color", (char*)canvas->marquee_fgcolor.data(), -1, -1, (int)canvas->char_width*40);
	NewLine();

	text = new wxMultiText(this, 0, "", (char*)canvas->marquee_text.data(), -1, -1, (int)canvas->char_width*80, (int)canvas->char_height*6);
	NewLine();

	End();
}

void MarqueeAttributes::OnOK()
{
	canvas->marquee_bgcolor = bgcolor->GetValue();
	canvas->marquee_fgcolor = fgcolor->GetValue();
	canvas->marquee_text = text->GetValue();
}

///////////////////////////////////////////////////////////////////////////////

ImageAttributes::ImageAttributes(wxHtml* _canvas) :
	HtmlAttributes(_canvas, "Image attributes...")
{
	Begin();

	string w = Number(canvas->edit_object->GetWidth());
	w += "  ";//w.pad(4-w.length());
	width = new wxText(this, 0, "Width", (char*)w.data());
	string h = Number(canvas->edit_object->GetHeight());
	h += "  ";//h.pad(4-h.length());
	height = new wxText(this, 0, "Height", (char*)h.data());
	aspect = new wxButton(this, (wxFunction)ImageAspect, "Default aspect");
	NewLine();

	char* _alignment[] = {"top", "middle", "bottom", "left", "right"};
	alignment = new wxRadioBox(this, 0, "Alignment", -1, -1, -1, -1, 5, _alignment);
	alignment->SetSelection(((HtmlImage*)canvas->edit_object)->align-1);
	NewLine();

	dynsrc = new wxText(this, 0, "Dynamic", (char*)((HtmlImage*)canvas->edit_object)->dynsrc.data(), -1, -1, (int)canvas->char_width*60);
	NewLine();

	string l = Number(((HtmlImage*)canvas->edit_object)->loop);
	l += "  ";//l.pad(3-l.length());
	loop = new wxText(this, 0, "Loop (-1 = infinite)", (char*)(l.data()));
	NewLine();

	src = new wxText(this, 0, "Source", (char*)((HtmlImage*)canvas->edit_object)->src.data(), -1, -1, (int)canvas->char_width*60);
	NewLine();

	usemap = new wxText(this, 0, "Client map", (char*)((HtmlImage*)canvas->edit_object)->usemap.data(), -1, -1, (int)canvas->char_width*60);
	NewLine();

	ismap = new wxCheckBox(this, 0, "Server map");
	ismap->SetValue(((HtmlImage*)canvas->edit_object)->ismap);

	imagemap = new wxButton(this, (wxFunction)ImageMap, "Map...");

	End();
}

void ImageAttributes::OnOK()
{
	HtmlImage* image = (HtmlImage*)canvas->edit_object;

	image->src = src->GetValue();
	image->dynsrc = dynsrc->GetValue();
	image->usemap = usemap->GetValue();
	image->ismap = ismap->GetValue();
	image->align = alignment->GetSelection()+1;
	image->SetWidth((float)atof(width->GetValue()));
	image->SetHeight((float)atof(height->GetValue()));

	canvas->edit_tag->left_floating = image->align == 4;
	canvas->edit_tag->right_floating = image->align == 5;

	image->Adjust();
}

void ImageAttributes::ImageMap(wxButton& button, wxCommandEvent&)
{
	ImageAttributes* attr = (ImageAttributes*)(button.GetParent());
	string localname;

	if (attr->canvas->GetURL(attr->src->GetValue(), TRUE, localname))
	{
		int x = -1, y = -1, w = -1, h = -1;
		float _w = (float)w, _h = (float)h;
		attr->canvas->DisplayImage(TRUE, attr->GetDC(), localname, (float)x, (float)y, _w, _h);
		attr->grid = new GridFrame(attr, x, y, w, h, localname);
	}
}

void ImageAttributes::ImageAspect(wxButton& button, wxCommandEvent&)
{
	ImageAttributes* attr = (ImageAttributes*)(button.GetParent());
	string localname;

	if (attr->canvas->GetURL(attr->src->GetValue(), TRUE, localname))
	{
		int x = -1, y = -1, w = -1, h = -1;
		float _w = (float)w, _h = (float)h;
		attr->canvas->DisplayImage(TRUE, attr->GetDC(), localname, (float)x, (float)y, _w, _h);
		attr->aspect_w = (float)w;
		attr->aspect_h = (float)h;
	}

	HtmlImage* image = (HtmlImage*)attr->canvas->edit_object;

	image->SetWidth(attr->aspect_w);
	image->SetHeight(attr->aspect_h);

	attr->width->SetValue((char*)Number(attr->aspect_w).data());
	attr->height->SetValue((char*)Number(attr->aspect_h).data());
}

///////////////////////////////////////////////////////////////////////////////

ParagraphAttributes::ParagraphAttributes(wxHtml* _canvas) :
	HtmlAttributes(_canvas, "Paragraph attributes...")
{
	Begin();
	End();
}

void ParagraphAttributes::OnOK()
{
	HtmlParagraph* para = (HtmlParagraph*)canvas->edit_object;
	para->center = center->GetValue();
}

#endif

