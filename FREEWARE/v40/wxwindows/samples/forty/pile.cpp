//+-------------------------------------------------------------+
//| File:	pile.cpp											|
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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "card.h"
#include "pile.h"


//+-------------------------------------------------------------+
//| Pile::Pile()												|
//+-------------------------------------------------------------+
//| Description:												|
//|	Initialise the pile to be empty of cards.					|
//+-------------------------------------------------------------+
Pile::Pile(int x, int y, int dx, int dy)
{
    X = x;
    Y = y;
    Dx = dx;
    Dy = dy;
    for (topCard = 0; topCard < NumCards; topCard++)
    {
    	cards[topCard] = (Card*)NULL;
    }
    topCard = -1;	// i.e. empty
}


//+-------------------------------------------------------------+
//| Pile::Redraw()												|
//+-------------------------------------------------------------+
//| Description:												|
//|	Redraw the pile on the screen. If the pile is empty			|
//|	just draw a NULL card as a place holder for the pile.		|
//|	Otherwise draw the pile from the bottom up, starting		|
//|	at the origin of the pile, shifting each subsequent			|
//|	card by the pile's x and y offsets.							|
//+-------------------------------------------------------------+
void Pile::Redraw(wxDC* dc)
{
	if (topCard >= 0)
	{
		if (Dx == 0 && Dy == 0)
		{
			cards[topCard]->Draw(dc, X, Y);
		}
		else
		{
			int x = X;
			int y = Y;
			for (int i = 0; i <= topCard; i++)
			{
				cards[i]->Draw(dc, x, y);
				x += Dx;
				y += Dy;
			}
		}
	}
	else
	{
		Card::DrawNullCard(dc, X, Y);
	}
}


//+-------------------------------------------------------------+
//| Pile::GetTopCard()	                                        |
//+-------------------------------------------------------------+
//| Description:												|
//|	Return a pointer to the top card in the pile or NULL		|
//|	if the pile is empty.										|
//| NB:	Gets a copy of the card without removing it from the	|
//|	pile.														|
//+-------------------------------------------------------------+
Card* Pile::GetTopCard()
{
    Card* card;

    if (topCard < 0)
    {
	card = (Card*)NULL;
    }
    else
    {
    	card = cards[topCard];
    }
    return card;
}


//+-------------------------------------------------------------+
//| Pile::RemoveTopCard()                                       |
//+-------------------------------------------------------------+
//| Description:												|
//|	If the pile is not empty, remove the top card from the		|
//|	pile and return the pointer to the removed card.			|
//|	If the pile is empty return a NULL pointer.					|
//+-------------------------------------------------------------+
Card* Pile::RemoveTopCard()
{
    Card* card;

    if (topCard < 0)
    {
		card = (Card*)NULL;
    }
    else
    {
		card = cards[topCard--];
    }
    return card;
}


//+-------------------------------------------------------------+
//| Pile::RemoveTopCard()                                       |
//+-------------------------------------------------------------+
//| Description:												|
//|	As RemoveTopCard() but also redraw the top of the pile		|
//|	after the card has been removed.							|
//| NB:	the offset allows for the redrawn area to be in a		|
//|	bitmap ready for 'dragging' cards acrosss the screen.		|
//+-------------------------------------------------------------+
Card* Pile::RemoveTopCard(wxDC* dc, int xOffset, int yOffset)
{
	int topX, topY, x, y;

	GetTopCardPos(topX, topY);
	Card* card = RemoveTopCard();

	if (card != NULL)
	{
		card->Erase(dc, topX - xOffset, topY - yOffset);
		GetTopCardPos(x, y);
		if (topCard < 0)
		{
			Card::DrawNullCard(dc, x - xOffset, y - yOffset);
		}
		else
		{
			cards[topCard]->Draw(dc, x - xOffset, y - yOffset);
		}
	}

	return card;
}


void Pile::GetTopCardPos(int& x, int& y)
{
    if (topCard < 0)
    {
	x = X;
	y = Y;
    }
    else
    {
	x = X + Dx * topCard;
	y = Y + Dy * topCard;
    }
}

void Pile::AddCard(Card* card)
{
    if (topCard < -1) topCard = -1;

    cards[++topCard] = card;
}

void Pile::AddCard(wxDC* dc, Card* card)
{
    AddCard(card);
    int x, y;
    GetTopCardPos(x, y);
    card->Draw(dc, x, y);
}

// Can the card leave this pile.
// If it is a member of the pile then the answer is yes.
// Derived classes may override this behaviour to incorporate
// the rules of the game
Bool Pile::CanCardLeave(Card* card)
{
    for (int i = 0; i <= topCard; i++)
    {
    	if (card == cards[i]) return TRUE;
    }
    return FALSE;
}

// Calculate how far x, y is from top card in the pile
// Returns the square of the distance
int Pile::CalcDistance(int x, int y)
{
    int cx, cy;
    GetTopCardPos(cx, cy);
    return ((cx - x) * (cx - x) + (cy - y) * (cy - y));
}


// Return the card at x, y. Check the top card first, then
// work down the pile. If a card is found then return a pointer
// to the card, otherwise return NULL
Card* Pile::GetCard(int x, int y)
{
    int cardX;
    int cardY;
    GetTopCardPos(cardX, cardY);

    for (int i = topCard; i >= 0; i--)
    {
	if (x >= cardX && x <= cardX + CardWidth &&
	    y >= cardY && y <= cardY + CardHeight)
	{
            return cards[i];
	}
	cardX -= Dx;
        cardY -= Dy;
    }
    return (Card*)NULL;
}


// Return the position of the given card. If it is not a member of this pile
// return the origin of the pile.
void Pile::GetCardPos(Card* card, int& x, int& y)
{
    x = X;
    y = Y;

    for (int i = 0; i <= topCard; i++)
    {
	if (card == cards[i])
	{
	    return;
	}
	x += Dx;
        y += Dy;
    }

    	// card not found in pile, return origin of pile
    x = X;
    y = Y;
}


Bool Pile::Overlap(int x, int y)
{
    int cardX;
    int cardY;
    GetTopCardPos(cardX, cardY);

    if (x >= cardX - CardWidth  && x <= cardX + CardWidth &&
	y >= cardY - CardHeight && y <= cardY + CardHeight)
    {
	    return TRUE;
    }
    return FALSE;
}


Pile::~Pile()
{
// nothing special at the moment
}
