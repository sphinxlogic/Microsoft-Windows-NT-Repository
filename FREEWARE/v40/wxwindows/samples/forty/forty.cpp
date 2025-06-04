//+-------------------------------------------------------------+
//| File:	forty.cpp											|
//| Author:	Chris Breeze										|
//| e-mail:	chris.breeze@iname.com								|
//| Last modified: 21st July 1997 - ported to wxWindows			|
//+-------------------------------------------------------------+
//|		Copyright (c) 1993-1997 Chris Breeze					|
//| This software is freeware and may be copied and distributed |
//| without restriction.										| 
//+-------------------------------------------------------------+

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include "canvas.h"
#include "forty.h"
#include "scoredg.h"
#ifdef wx_x
#include "cards.xbm"
#endif

class FortyFrame: public wxFrame
{
public:
	FortyFrame(wxFrame* frame, char* title, int x, int y, int w, int h);
	virtual ~FortyFrame();

	void OnMenuCommand(int id);
	Bool OnClose();

private:
	enum MenuCommands { NEW_GAME = 10, SCORES, EXIT,
						UNDO, REDO,
						RIGHT_BUTTON_UNDO, HELPING_HAND,
						ABOUT };

	wxMenu*			gameMenu_;
	wxMenu*			editMenu_;
	wxMenu*			optionsMenu_;
	wxMenu*			helpMenu_;
	wxMenuBar*		m_menuBar;
	FortyCanvas*	m_canvas;
};


// This statement initializes the whole application and calls OnInit
FortyApp FortyApp;

wxColour* FortyApp::m_backgroundColour = 0;
wxColour* FortyApp::m_textColour = 0;
wxBrush*  FortyApp::m_backgroundBrush = 0;

wxFrame *FortyApp::OnInit(void)
{
	FortyFrame* frame = new FortyFrame(
			0,
			"Forty Thieves",
			-1, -1, 668, 510
			);

	// Show the frame
	frame->Show(TRUE);
  
	// Return the main frame window
	return frame;
}

wxColour* FortyApp::BackgroundColour()
{
	if (!m_backgroundColour)
	{
		m_backgroundColour = new wxColour(0, 128, 0);
	}
	return m_backgroundColour;
}

wxBrush* FortyApp::BackgroundBrush()
{
	if (!m_backgroundBrush)
	{
		m_backgroundBrush = new wxBrush(*BackgroundColour(), wxSOLID);
	}
	return m_backgroundBrush;
}

wxColour* FortyApp::TextColour()
{
	if (!m_textColour)
	{
		m_textColour = new wxColour("BLACK");
	}
	return m_textColour;
}

// My frame constructor
FortyFrame::FortyFrame(wxFrame* frame, char* title, int x, int y, int w, int h):
	wxFrame(frame, title, x, y, w, h)
{
	// set the icon
#ifdef wx_msw
	SetIcon(new wxIcon("CardsIcon"));
#endif
#ifdef wx_x
	SetIcon(new wxIcon(Cards_bits, Cards_width, Cards_height));
#endif

	// Make a m_menuBar
	gameMenu_ = new wxMenu;
	gameMenu_->Append(NEW_GAME, "&New", "Start a new game");
	gameMenu_->Append(SCORES, "&Scores...", "Displays scores");
	gameMenu_->Append(EXIT, "E&xit", "Exits Forty Thieves");

	editMenu_ = new wxMenu;
	editMenu_->Append(UNDO, "&Undo", "Undo the last move");
	editMenu_->Append(REDO, "&Redo", "Redo a move that has been undone");

	optionsMenu_ = new wxMenu;
	optionsMenu_->Append(RIGHT_BUTTON_UNDO,
			"&Right button undo",
			"Enables/disables right mouse button undo and redo",
			TRUE
			);
	optionsMenu_->Append(HELPING_HAND,
			"&Helping hand",
			"Enables/disables hand cursor when a card can be moved",
			TRUE
			);
	optionsMenu_->Check(HELPING_HAND, TRUE);
	optionsMenu_->Check(RIGHT_BUTTON_UNDO, TRUE);

	helpMenu_ = new wxMenu;
	helpMenu_->Append(ABOUT, "&About", "Displays program version information");

	m_menuBar = new wxMenuBar;
	m_menuBar->Append(gameMenu_,    "&Game");
	m_menuBar->Append(editMenu_,    "&Edit");
	m_menuBar->Append(optionsMenu_, "&Options");
	m_menuBar->Append(helpMenu_,    "&Help");

	SetMenuBar(m_menuBar);

	m_canvas = new FortyCanvas(this, 0, 0, 400, 400);
	wxLayoutConstraints* constr = new wxLayoutConstraints;
	constr->left.SameAs(this, wxLeft);
	constr->top.SameAs(this, wxTop);
	constr->right.SameAs(this, wxRight);
	constr->height.SameAs(this, wxHeight);
	m_canvas->SetConstraints(constr);

	CreateStatusLine();
}

FortyFrame::~FortyFrame()
{
}

Bool FortyFrame::OnClose()
{
	return m_canvas->OnClose();
}

// Intercept menu commands
void FortyFrame::OnMenuCommand(int id)
{
	switch (id)
	{
	case EXIT:
		Close(FALSE);
		break;
	case NEW_GAME:
		m_canvas->NewGame();
		break;
	case SCORES:
		{
			m_canvas->UpdateScores();
			ScoreDialog scores(this, m_canvas->GetScoreFile());
			scores.Display();
		}
		break;
	case UNDO:
		m_canvas->Undo();
		break;
	case REDO:
		m_canvas->Redo();
		break;
	case HELPING_HAND:
		m_canvas->EnableHelpingHand(optionsMenu_->Checked(id));
		break;
	case RIGHT_BUTTON_UNDO:
		m_canvas->EnableRightButtonUndo(optionsMenu_->Checked(id));
		break;
	case ABOUT:
		wxMessageBox(
			"Forty Thieves\n\n"
			"A freeware program using the wxWindows\n"
			"portable C++ GUI toolkit.\n"
			"http://web.ukonline.co.uk/julian.smart/wxwin\n\n"
			"Author: Chris Breeze (c) 1992-1997\n"
			"email: chris.breeze@iname.com",
			"About Forty Thieves",
			wxOK, this
			);
	}
}
