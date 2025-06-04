//+-------------------------------------------------------------+
//| File:	game.h												|
//| Author:	Chris Breeze										|
//| e-mail:	chris.breeze@iname.com								|
//| Last modified: 21st July 1997 - ported to wxWindows			|
//+-------------------------------------------------------------+
//|		Copyright (c) 1993-1997 Chris Breeze					|
//| This software is freeware and may be copied and distributed |
//| without restriction.										| 
//+-------------------------------------------------------------+
#ifndef _GAME_H_
#define _GAME_H_
#include "card.h"
#include "pile.h"

const int MaxMoves = 800;


//---------------------------------------//
// A class which holds the pack of cards //
//---------------------------------------//
class Pack : public Pile {
public:
	Pack(int x, int y);
	~Pack();
	void Redraw(wxDC* dc);
	void ResetPile() { topCard = NumCards - 1; }
	void Shuffle();
	void AddCard(Card* card);		// Add card
	void AddCard(wxDC* dc, Card* card) { AddCard(card); Redraw(dc); }
};


//----------------------------------------------------------//
// A class which holds a base i.e. the initial 10 x 4 cards //
//----------------------------------------------------------//
class Base : public Pile {
public:
	Base(int x, int y);
	~Base();
	Bool AcceptCard(Card* card);
};


//----------------------------------------------------//
// A class which holds a foundation i.e. Ace, 2, 3... //
//----------------------------------------------------//
class Foundation : public Pile {
public:
	Foundation(int x, int y);
	~Foundation();
	Bool AcceptCard(Card* card);
};


//--------------------------------------//
// A class which holds the discard pile //
//--------------------------------------//
class Discard : public Pile {
public:
	Discard(int x, int y);
	~Discard();
	void Redraw(wxDC* dc);
	void GetTopCardPos(int& x, int& y);
	Card* RemoveTopCard(wxDC* dc, int xOffset, int yOffset);
};


class Game {
public:
	Game(int wins, int games, int score);
	virtual ~Game();

	void NewPlayer(int wins, int games, int score);
	void Deal();		// Shuffle and deal a new game
	Bool CanYouGo(int x, int y);	// can card under (x,y) go somewhere?
	Bool HaveYouWon();		// have you won the game?

	void Undo(wxDC* dc);		// Undo the last go
	void Redo(wxDC* dc);		// Redo the last go

	void Redraw(wxDC* dc);
	Bool LButtonDown(wxDC* dc, int mx, int my);	//
	void LButtonUp(wxDC* dc, int mx, int my);
	void LButtonDblClk(wxDC* dc, int mx, int my);
	void MouseMove(wxCanvasDC* dc, int mx, int my);

	int GetNumWins() const	{ return numWins; }
	int GetNumGames() const	{ return numGames; }
	int GetScore() const	{ return currentScore + totalScore; }

	Bool InPlay() const		{ return inPlay_; }

private:
	Bool DropCard(int x, int y, Pile* pile, Card* card);
			//  can the card at (x, y) be dropped on the pile?
	Pile* WhichPile(int x, int y);	// which pile is (x, y) over?
	void DoMove(wxDC* dc, Pile* src, Pile* dest);

	Bool inPlay_;		// flag indicating that the game has started

	struct {			// undo buffer
		Pile* src;
		Pile* dest;
	} moves[MaxMoves];
	int moveIndex_;		// current position in undo/redo buffer
	int redoIndex_;		// max move index available for redo

	Pack* pack_;		// the various piles of cards
	Discard* discard;
	Base* bases[10];
	Foundation* foundations[8];

	Pile* srcPile;		// variables to do with dragging cards
	Card* liftedCard;
	int xPos, yPos;		// current coords of card being dragged
	int xOffset, yOffset;	// card/mouse offset when dragging a card
	wxBitmap* bmap_;
	wxBitmap* bmapCard_;

	int numGames;		// variables to do with scoring
	int numWins;
	int totalScore;
	int currentScore;
	void DisplayScore(wxDC* dc);
};

#endif // _GAME_H_
