//+-------------------------------------------------------------+
//| File:	scoredg.cpp											|
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

#ifdef wx_msw
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#include "scorefil.h"
#include "scoredg.h"

class ScoreCanvas : public wxCanvas
{
public:
	ScoreCanvas(wxWindow* parent, ScoreFile* scoreFile);
	virtual ~ScoreCanvas();

	void OnPaint();

private:
	wxFont*	m_font;
	char*	m_text;
};


ScoreCanvas::ScoreCanvas(wxWindow* parent, ScoreFile* scoreFile) :
	wxCanvas(parent)
{
	m_font = wxTheFontList->FindOrCreateFont(12, wxROMAN, wxNORMAL, wxNORMAL);

	int numPlayers;
	char** players;
	scoreFile->GetPlayerList(&players, numPlayers);

	ostrstream os;

	os << "Player\tWins\tGames\tScore\n";
	if (numPlayers > 0)
	{
		for (int i = 0; i < numPlayers; i++)
		{
			int wins, games, score;
			scoreFile->ReadPlayersScore(players[i], wins, games, score);
			int average = 0;
			if (games > 0)
			{
				average = (2 * score + games) / (2 * games);
			}

			os << players[i] << '\t' 
			   << wins  << '\t'
			   << games << '\t'
			   << average << '\n';
		}
		delete [] players;
	}
	os << '\0';
	m_text = os.str();
}

ScoreCanvas::~ScoreCanvas()
{
	delete m_text;
}

void ScoreCanvas::OnPaint()
{
	wxCanvasDC* dc = GetDC();
	dc->BeginDrawing();
	dc->SetFont(m_font);

	char* str = m_text;
	unsigned int tab = 0;
	unsigned int tabstops[] = { 5, 100, 150, 200 };

	// get the line spacing for the current font
	int lineSpacing;
	{
		float w, h;
		dc->GetTextExtent("Testing", &w, &h);
		lineSpacing = (int)h;
	}

	int y = 0;
	while (*str)
	{
		char text[256];
		char* dest = text;
		
		while (*str && *str >= ' ') *dest++ = *str++;
		*dest = '\0';

		dc->DrawText(text, tabstops[tab], y);

		if (*str == '\t')
		{
			if (tab < sizeof(tabstops) / sizeof(tabstops[0]) - 1)
			{
				tab++;
			}
		}
		else if (*str == '\n')
		{
			tab = 0;
			y += lineSpacing;
		}
		if (*str) str++;
	}

	dc->SetFont(0);
	dc->EndDrawing();
}

// Function		: btnCB 
// Description	: Processes the event generated when a button is pressed
// Comments		: This is a static function because it is a callback
static void btnCB (wxButton& b, wxCommandEvent& WXUNUSED(event))
{
	ScoreDialog* dialog = (ScoreDialog*)b.GetParent();
	dialog->ButtonCallback(b);
}


ScoreDialog::ScoreDialog(
							wxWindow* parent,
							ScoreFile* file
							) :
	wxDialogBox(parent, "Forty Thieves Scores", TRUE, -1, -1, 310, 200),
	m_scoreFile(file)
{
	// enable constraints
	SetAutoLayout (TRUE);

	ScoreCanvas* list = new ScoreCanvas(this, m_scoreFile);
	m_OK = new wxButton(this, (wxFunction)btnCB, "OK");

	wxLayoutConstraints* layout;

	// Constrain the OK button
	layout = new wxLayoutConstraints;
	layout->left.SameAs		(this,	wxLeft,		10);
	layout->bottom.SameAs	(this,	 wxBottom,	10);
	layout->height.AsIs();
	layout->width.AsIs();
	m_OK->SetConstraints(layout);

	// Constrain the list of players
	layout = new wxLayoutConstraints;
	layout->left.SameAs		(this,	wxLeft,		10);
	layout->right.SameAs	(this,	wxRight,	10);
	layout->top.SameAs		(this,	wxTop,		10);
	layout->bottom.SameAs	(m_OK,	wxTop,		10);
	list->SetConstraints(layout);
}

ScoreDialog::~ScoreDialog()
{
}

void ScoreDialog::Display()
{
	Show(TRUE);
}

Bool ScoreDialog::OnClose()
{
	// hide the dialog
	// NB don't return TRUE otherwise delete is called
	Show(FALSE);
	return FALSE;
}


void ScoreDialog::ButtonCallback(wxButton& button)
{
	if (&button == m_OK)
	{
		Show(FALSE);
	}
}
