//+-------------------------------------------------------------+
//| File:	canvas.h											|
//| Author:	Chris Breeze										|
//| e-mail:	chris.breeze@iname.com								|
//| Last modified: 21st July 1997 - ported to wxWindows			|
//+-------------------------------------------------------------+
//|		Copyright (c) 1993-1997 Chris Breeze					|
//| This software is freeware and may be copied and distributed |
//| without restriction.										| 
//+-------------------------------------------------------------+
#ifndef _CANVAS_H_
#define _CANVAS_H_

class Card;
class Game;
class ScoreFile;
class PlayerSelectionDialog;

class FortyCanvas: public wxCanvas
{
public:
	FortyCanvas(wxWindow* parent, int x, int y, int w, int h);
	virtual ~FortyCanvas();

	void OnPaint();
	Bool OnClose();
	void OnEvent(wxMouseEvent& event);
	void SetCursorStyle(int x, int y);

	void NewGame();
	void Undo();
	void Redo();

	ScoreFile* GetScoreFile() const			{ return m_scoreFile; }
	void UpdateScores();
	void EnableHelpingHand(Bool enable)		{ m_helpingHand = enable; }
	void EnableRightButtonUndo(Bool enable)	{ m_rightBtnUndo = enable; }

private:
	wxFont*		m_font;
	Game*		m_game;
	ScoreFile*	m_scoreFile;
	wxCursor*	m_arrowCursor;
	wxCursor*	m_handCursor;
	Bool		m_helpingHand;
	Bool		m_rightBtnUndo;
	char*		m_player;
	PlayerSelectionDialog* m_playerDialog;
};

#endif
