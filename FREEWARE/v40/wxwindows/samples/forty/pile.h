//+-------------------------------------------------------------+
//| File:	pile.h												|
//| Author:	Chris Breeze										|
//| e-mail:	chris.breeze@iname.com								|
//| Last modified: 21st July 1997 - ported to wxWindows			|
//+-------------------------------------------------------------+
//|		Copyright (c) 1993-1997 Chris Breeze					|
//| This software is freeware and may be copied and distributed |
//| without restriction.										| 
//+-------------------------------------------------------------+
//| Description:												|
//|	The base class for holding piles of playing cards.			|
//|	This is the basic building block for card games. A pile		|
//|	has a position on the screen and an offset for each			|
//|	card placed on it e.g. a pack has no offset, but the		|
//|	discard pile may be fanned out across the screen.			|
//|																|
//|	The pile knows how to draw itself, though this may be		|
//|	overridden if the default layout needs to be changed.		|
//|	One or more cards can be removed from the top of a pile,	|
//|	and single cards can be added to the top of a pile.     	|
//|	Functions are provided which redraw the screen when			|
//|	cards are added or removed.									|
//|																|
//|	Cards know which way up they are and how to draw			|
//|	themselves. Piles are lists of cards. Piles know which		|
//|	cards they contain and where they are to be drawn.			|
//+-------------------------------------------------------------+
#ifndef _PILE_H_
#define _PILE_H_
#include "card.h"

const int NumCards = 2 * PackSize;


//----------------------------------------------------------------//
// A class defining a pile of cards with a position on the screen //
//----------------------------------------------------------------//
class Pile {
public:
	Pile(int x, int y, int dx = 0, int dy = 0);
	virtual ~Pile();

        	// General functions
	virtual void ResetPile() { topCard = -1; }
	virtual void Redraw(wxDC* pDC);

		// Card query functions
	virtual Card* GetCard(int x, int y);	// Get pointer to card at x, y
	Card* GetTopCard();                     // Get pointer to top card
	virtual void GetCardPos(Card* card, int& x, int& y);
        				// Get position of a card
	virtual void GetTopCardPos(int& x, int& y);
        				// Get position of the top card
	int GetNumCards() { return topCard + 1; }	// Number of cards in pile
	Bool Overlap(int x, int y);	// does card at x,y overlap the pile?
	int CalcDistance(int x, int y);	// calculates the square of the distance
					// of a card at (x,y) from the top of the pile

		// Functions removing one or more cards from the top of a pile
	virtual Bool CanCardLeave(Card* card);
	Card* RemoveTopCard();
	virtual Card* RemoveTopCard(wxDC* pDC, int xOffset = 0, int yOffset = 0);

        	// Functions to add a card to the top of a pile
	virtual Bool AcceptCard(Card*) { return FALSE; }
	virtual void AddCard(Card* card);	// Add card to top of pile
	virtual void AddCard(wxDC* pDC, Card* card);	// Add card + redraw it

protected:
	int   X, Y;		// Position of the pile on the screen
	int   Dx, Dy;		// Offset when drawing the pile
	Card* cards[NumCards];	// Array of cards in this pile
        int   topCard;		// Array index of the top card
};

#endif // _PILE_H_
