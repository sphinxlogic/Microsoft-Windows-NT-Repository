//+-------------------------------------------------------------+
//| File:	card.h												|
//| Author:	Chris Breeze										|
//| e-mail:	chris.breeze@iname.com								|
//| Last modified: 21st July 1997 - ported to wxWindows			|
//+-------------------------------------------------------------+
//|		Copyright (c) 1993-1997 Chris Breeze					|
//| This software is freeware and may be copied and distributed |
//| without restriction.										| 
//+-------------------------------------------------------------+
//| Description:												|
//|	A class for drawing playing cards.							|
//|	InitCards() must be called before using the Card class,		|
//|	otherwise the card bitmaps will not be loaded.				|
//|	CloseCards() must be called before terminating the			|
//|	program so that the bitmaps are deleted and the memory		|
//|	given back to Windows.										|
//+-------------------------------------------------------------+
#ifndef _CARD_H_
#define _CARD_H_

	// Constants
const int PackSize = 52;
const int CardWidth = 50;
const int CardHeight = 70;

	// Data types
enum Suit { clubs = 0, diamonds = 1, hearts = 2, spades = 3 };
enum SuitColour { red = 0, black = 1 };
enum WayUp { faceup, facedown };


//--------------------------------//
// A class defining a single card //
//--------------------------------//
class Card {
public:
	Card(int value, WayUp way_up = facedown);
	virtual ~Card();

	void		Draw(wxDC* pDC, int x, int y);
	static void DrawNullCard(wxDC* pDC, int x, int y);	// Draw card place-holder
	void		Erase(wxDC* pDC, int x, int y);

	void		TurnCard(WayUp way_up = faceup) { wayUp_ = way_up; }
	WayUp		GetWayUp() const { return wayUp_; }
	int			GetPipValue() const { return pipValue_; }
	Suit		GetSuit() const { return suit_; }
	SuitColour	GetColour() const { return colour_; }
private:
	Suit		suit_;
	int			pipValue_;	// in the range 1 (Ace) to 13 (King)
	SuitColour	colour_;	// red or black
	Bool		status_;
	WayUp		wayUp_;

	static wxBitmap*	symbolBmap_;
	static wxBitmap*	pictureBmap_;
};

#endif // _CARD_H_
