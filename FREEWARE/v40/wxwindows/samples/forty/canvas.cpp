//+-------------------------------------------------------------+
//| File:	canvas.cpp											|
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

#include "forty.h"
#include "card.h"
#include "game.h"
#include "scorefil.h"
#include "playerdg.h"
#include "canvas.h"

FortyCanvas::FortyCanvas(wxWindow* parent, int x, int y, int w, int h) :
	wxCanvas(parent, x, y, w, h),
	m_helpingHand(TRUE),
	m_rightBtnUndo(TRUE),
	m_player(0),
	m_playerDialog(0)
{
	m_font = wxTheFontList->FindOrCreateFont(12, wxROMAN, wxNORMAL, wxNORMAL);
	SetBackground(FortyApp::BackgroundBrush());
	AllowDoubleClick(150);

	m_handCursor = new wxCursor(wxCURSOR_HAND);
	m_arrowCursor = new wxCursor(wxCURSOR_ARROW);

	m_scoreFile = new ScoreFile(wxTheApp->GetAppName());
	m_game = new Game(0, 0, 0);
	m_game->Deal();
}


FortyCanvas::~FortyCanvas()
{
	UpdateScores();
	delete m_game;
	delete m_scoreFile;
	delete m_player;
}


/*
Write the current player's score back to the score file
*/
void FortyCanvas::UpdateScores()
{
	if (m_player && m_scoreFile && m_game)
	{
		m_scoreFile->WritePlayersScore(
				m_player,
				m_game->GetNumWins(),
				m_game->GetNumGames(),
				m_game->GetScore()
				);
	}
}


void FortyCanvas::OnPaint()
{
	wxCanvasDC* dc = GetDC();
	dc->BeginDrawing();
	dc->SetFont(m_font);

	m_game->Redraw(dc);

	dc->SetFont(0);
	dc->EndDrawing();

	// if player name not set (and selection dialog is not displayed)
	// then ask the player for their name
	if (!m_player && !m_playerDialog)
	{
		m_playerDialog = new PlayerSelectionDialog(this, m_scoreFile);
		m_player = m_playerDialog->GetPlayersName();
		if (m_player)
		{
			// user entered a name - lookup their score
			int wins, games, score;
			m_scoreFile->ReadPlayersScore(m_player, wins, games, score);
			m_game->NewPlayer(wins, games, score);
		}
		else
		{
			// user cancelled the dialog - exit the app
			((wxFrame*)GetParent())->Close(TRUE);
		}
		delete m_playerDialog;
		m_playerDialog = 0;
	}
}

/*
Called when the main frame is closed
*/
Bool FortyCanvas::OnClose()
{
	if (m_game->InPlay() &&
		wxMessageBox("Are you sure you want to\nabandon the current game?",
			"Warning", wxYES_NO | wxICON_QUESTION) == wxNO)
	{
		return FALSE;
	}
	return TRUE;
}

void FortyCanvas::OnEvent(wxMouseEvent& event)
{
	wxCanvasDC* dc = GetDC();
	dc->BeginDrawing();
	dc->SetFont(m_font);

	if (event.LeftDClick())
	{
		m_game->LButtonDblClk(dc, (int)event.x, (int)event.y);
	}
	else if (event.LeftDown())
	{
		CaptureMouse();
		m_game->LButtonDown(dc, (int)event.x, (int)event.y);
	}
	else if (event.LeftUp())
	{
		ReleaseMouse();
		m_game->LButtonUp(dc, (int)event.x, (int)event.y);
	}
	else if (event.RightDown() && !event.LeftIsDown())
	{
		// only allow right button undo if m_rightBtnUndo is TRUE
		if (m_rightBtnUndo)
		{
			if (event.ControlDown() || event.ShiftDown())
			{
				m_game->Redo(dc);
			}
			else
			{
				m_game->Undo(dc);
			}
		}
	}
	else if (event.Dragging())
	{
		m_game->MouseMove(dc, (int)event.x, (int)event.y);
	}

	if (!event.LeftIsDown())
	{
		SetCursorStyle((int)event.x, (int)event.y);
	}
	dc->SetFont(0);
	dc->EndDrawing();
}

void FortyCanvas::SetCursorStyle(int x, int y)
{
	if (m_game->HaveYouWon())
	{
		if (wxMessageBox("Do you wish to play again?",
			"Well Done, You have won!", wxYES_NO | wxICON_QUESTION) == wxYES)
		{
			m_game->Deal();
			m_game->Redraw(GetDC());
		}
		else
		{
			// Send a message to ourself telling us to quit
			//PostAppMessage(GetCurrentTask(), WM_QUIT, 0, 0L);
		}
	}

	// Only set cursor to a hand if 'helping hand' is enabled and
	// the card under the cursor can go somewhere 
	if (m_game->CanYouGo(x, y) && m_helpingHand)
	{
		SetCursor(m_handCursor);
	}
	else
	{
		SetCursor(m_arrowCursor);
	}

}

void FortyCanvas::NewGame()
{
	m_game->Deal();
	Refresh();
}

void FortyCanvas::Undo()
{
	wxCanvasDC* dc = GetDC();
	dc->BeginDrawing();
	dc->SetFont(m_font);
	m_game->Undo(dc);
	dc->SetFont(0);
	dc->EndDrawing();
}

void FortyCanvas::Redo()
{
	wxCanvasDC* dc = GetDC();
	dc->BeginDrawing();
	dc->SetFont(m_font);
	m_game->Redo(dc);
	dc->SetFont(0);
	dc->EndDrawing();
}
