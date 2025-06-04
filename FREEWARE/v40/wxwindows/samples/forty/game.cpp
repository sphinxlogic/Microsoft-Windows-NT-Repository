//+-------------------------------------------------------------+
//| File:	game.cpp											|
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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "forty.h"
#include "game.h"

Game::Game(int wins, int games, int score) :
	inPlay_(FALSE),
	moveIndex_(0),
	redoIndex_(0),
	bmap_(0),
	bmapCard_(0)
{
    int i;

    pack_ = new Pack(2, 2 + 4 * (CardHeight + 2));
    //randomize();
    srand(time(0));
    for (i = 0; i < 5; i++) pack_->Shuffle();

    discard = new Discard(2, 2 + 5 * (CardHeight + 2));

    for (i = 0; i < 8; i++)
    {
		foundations[i] = new Foundation(2 + (i / 4) * (CardWidth + 2),
					2 + (i % 4) * (CardHeight + 2));
    }

    for (i = 0; i < 10; i++)
    {
    	bases[i] = new Base(8 + (i + 2) * (CardWidth + 2), 2);
    }
    Deal();
    srcPile = 0;
    liftedCard = 0;

	// copy the input parameters for future reference
    numWins = wins;
    numGames = games;
    totalScore = score;
    currentScore = 0;
}


// Make sure we delete all objects created by the game object
Game::~Game()
{
    int i;

    delete pack_;
    delete discard;
    for (i = 0; i < 8; i++)
    {
		delete foundations[i];
    }
    for (i = 0; i < 10; i++)
    {
		delete bases[i];
    }
	delete bmap_;
	delete bmapCard_;
}

/*
Set the score for a new player.
NB: call Deal() first if the new player is to start
a new game
*/
void Game::NewPlayer(int wins, int games, int score)
{
    numWins = wins;
    numGames = games;
    totalScore = score;
    currentScore = 0;
}

// Undo the last move
void Game::Undo(wxDC* dc)
{
    if (moveIndex_ > 0)
    {
		moveIndex_--;
		Card* card = moves[moveIndex_].dest->RemoveTopCard(dc);
		moves[moveIndex_].src->AddCard(dc, card);
		DisplayScore(dc);
    }
}

// Redo the last move
void Game::Redo(wxDC* dc)
{
    if (moveIndex_ < redoIndex_)
    {
		Card* card = moves[moveIndex_].src->RemoveTopCard(dc);
		if (moves[moveIndex_].src == pack_)
		{
			pack_->Redraw(dc);
			card->TurnCard(faceup);
		}
		moves[moveIndex_].dest->AddCard(dc, card);
		DisplayScore(dc);
		moveIndex_++;
    }
}

void Game::DoMove(wxDC* dc, Pile* src, Pile* dest)
{
    if (moveIndex_ < MaxMoves)
    {
		if (src == dest)
		{
			wxMessageBox("Game::DoMove() src == dest", "Debug message",
				   wxOK | wxICON_EXCLAMATION);
		}
		moves[moveIndex_].src = src;
		moves[moveIndex_].dest = dest;
		moveIndex_++;

		// when we do a move any moves in redo buffer are discarded
		redoIndex_ = moveIndex_;
    }
    else
    {
		wxMessageBox("Game::DoMove() Undo buffer full", "Debug message",
			   wxOK | wxICON_EXCLAMATION);
    }

    if (!inPlay_)
	{
		inPlay_ = TRUE;
		numGames++;
	}
    DisplayScore(dc);
}


void Game::DisplayScore(wxDC* dc)
{
    wxColour* bgColour = FortyApp::BackgroundColour();
	wxPen* pen = wxThePenList->FindOrCreatePen(bgColour, 1, wxSOLID);
    dc->SetTextBackground(bgColour);
    dc->SetTextForeground(FortyApp::TextColour());
	dc->SetBrush(FortyApp::BackgroundBrush());
	dc->SetPen(pen);

	// count the number of cards in foundations
    currentScore = 0;
    for (int i = 0; i < 8; i++)
    {
    	currentScore += foundations[i]->GetNumCards();
    }

    int x, y;
    pack_->GetTopCardPos(x, y);
    x += 12 * CardWidth - 105;

	int w, h;
	{
		float width, height;
		dc->GetTextExtent("Games played:X", &width, &height);
		w = (int)width;
		h = (int)height;
	}
	dc->DrawRectangle(x + w, y, 20, 4 * h);

    char str[80];
    sprintf(str, "%d", currentScore);
    dc->DrawText("Score:", x, y);
    dc->DrawText(str, x + w, y);
    y += h;

    sprintf(str, "%d", numGames);
    dc->DrawText("Games played:", x, y);
    dc->DrawText(str, x + w, y);
    y += h;

    sprintf(str, "%d", numWins);
    dc->DrawText("Games won:", x, y);
    dc->DrawText(str, x + w, y);
    y += h;

    int average = 0;
	if (numGames > 0)
	{
		average = (2 * (currentScore + totalScore) + numGames ) / (2 * numGames);
	}
    sprintf(str, "%d", average);
    dc->DrawText("Average score:", x, y);
    dc->DrawText(str, x + w, y);
}


// Shuffle the pack_ and deal the cards
void Game::Deal()
{
    int i, j;
    Card* card;

	// Reset all the piles, the undo buffer and shuffle the pack_
    moveIndex_ = 0;
    pack_->ResetPile();
    for (i = 0; i < 5; i++)
	{
		pack_->Shuffle();
	}
    discard->ResetPile();
    for (i = 0; i < 10; i++)
	{
		bases[i]->ResetPile();
	}
    for (i = 0; i <  8; i++)
	{
		foundations[i]->ResetPile();
	}

    	// Deal the initial 40 cards onto the bases
    for (i = 0; i < 10; i++)
    {
		for (j = 1; j <= 4; j++)
		{
			card = pack_->RemoveTopCard();
			card->TurnCard(faceup);
			bases[i]->AddCard(card);
		}
    }

    if (inPlay_)
	{
		// player has started the game and then redealt
		// and so we must add the score for this game to the total score
		totalScore += currentScore;
	}
    currentScore = 0;
    inPlay_ = FALSE;
}


// Redraw the pack_, discard pile, the bases and the foundations
void Game::Redraw(wxDC* dc)
{
	int i;
	pack_->Redraw(dc);
	discard->Redraw(dc);
	for (i = 0; i < 8; i++)
	{
		foundations[i]->Redraw(dc);
	}
	for (i = 0; i < 10; i++)
	{
		bases[i]->Redraw(dc);
	}
	DisplayScore(dc);

	if (bmap_ == 0)
	{
		bmap_ = new wxBitmap(CardWidth, CardHeight);
		bmapCard_ = new wxBitmap(CardWidth, CardHeight);

		// Initialise the card bitmap to the background colour
		wxMemoryDC memoryDC;
		memoryDC.SelectObject(bmapCard_);
		memoryDC.SetBrush(FortyApp::BackgroundBrush());
		memoryDC.DrawRectangle(0, 0, CardWidth, CardHeight);
		memoryDC.SelectObject(bmap_);
		memoryDC.DrawRectangle(0, 0, CardWidth, CardHeight);
		memoryDC.SelectObject(0);
	}
}


// Test to see if the point (x, y) is over the top card of one of the piles
// Returns pointer to the pile, or NULL if (x, y) is not over a pile
// or the pile is empty
Pile* Game::WhichPile(int x, int y)
{
	if (pack_->GetCard(x, y) != NULL &&
		pack_->GetCard(x, y) == pack_->GetTopCard())
	{
		return pack_;
	}

	if (discard->GetCard(x, y) != NULL &&
		discard->GetCard(x, y) == discard->GetTopCard())
	{
		return discard;
	}

	int i;
	for (i = 0; i < 8; i++)
	{
		if (foundations[i]->GetCard(x, y) != NULL &&
			foundations[i]->GetCard(x, y) == foundations[i]->GetTopCard())
		{
			return foundations[i];
		}
	}

	for (i = 0; i < 10; i++)
	{
		if (bases[i]->GetCard(x, y) != NULL &&
			bases[i]->GetCard(x, y) == bases[i]->GetTopCard())
		{
			return bases[i];
		}
	}
	return (Pile*)NULL;
}


// Left button is pressed - if cursor is over the pack_ then deal a card
// otherwise if it is over a card pick it up ready to be dragged - see MouseMove()
Bool Game::LButtonDown(wxDC* dc, int x, int y)
{
    srcPile = WhichPile(x, y);
    if (srcPile == pack_)
    {
		Card* card = pack_->RemoveTopCard();
		if (card)
		{
			pack_->Redraw(dc);
			card->TurnCard(faceup);
			discard->AddCard(dc, card);
			DoMove(dc, pack_, discard);
		}
        srcPile = (Pile*)NULL;
    }
    else if (srcPile != NULL)
    {
		srcPile->GetTopCardPos(xPos, yPos);
		xOffset = xPos - x;
		yOffset = yPos - y;

			// Copy the area under the card
			// Initialise the card bitmap to the background colour
		wxMemoryDC memoryDC;
		memoryDC.SelectObject(bmap_);
		liftedCard = srcPile->RemoveTopCard(&memoryDC, xPos, yPos);

			// Draw the card in card bitmap ready for blitting onto
			// the screen
		memoryDC.SelectObject(bmapCard_);
		liftedCard->Draw(&memoryDC, 0, 0);
		memoryDC.SelectObject(0);
    }
    return (srcPile != NULL);
}


// Called when the left button is double clicked
// If a card is under the pointer and it can move elsewhere then move it.
// Move onto a foundation as first choice, a populated base as second and
// an empty base as third choice.
// NB Cards in the pack_ cannot be moved in this way - they aren't in play
// yet
void Game::LButtonDblClk(wxDC* dc, int x, int y)
{
    Pile* pile = WhichPile(x, y);
    if (pile == NULL) return;

    	// Double click on pack_ is the same as left button down
    if (pile == pack_)
    {
		LButtonDown(dc, x, y);
    }
    else
    {
		Card* card = pile->GetTopCard();

		if (card != NULL)
		{
			int i;

			// if the card is an ace then try to place it next
			// to an ace of the same suit
			if (card->GetPipValue() == 1)
			{
				for(i = 0; i < 4; i++)
				{
					Card* topCard;
					if ((topCard = foundations[i]->GetTopCard()) != NULL)
                    {
						if (topCard->GetSuit() == card->GetSuit() &&
                            foundations[i + 4] != pile &&
							foundations[i + 4]->GetTopCard() == NULL)
						{
							pile->RemoveTopCard(dc);
							foundations[i + 4]->AddCard(dc, card);
							DoMove(dc, pile, foundations[i + 4]);
							return;
						}
                    }
				}
			}

			// try to place the card on a foundation
			for(i = 0; i < 8; i++)
			{
				if (foundations[i]->AcceptCard(card) && foundations[i] != pile)
				{
					pile->RemoveTopCard(dc);
					foundations[i]->AddCard(dc, card);
					DoMove(dc, pile, foundations[i]);
					return;
				}
            }
			// try to place the card on a populated base
			for(i = 0; i < 10; i++)
			{
				if (bases[i]->AcceptCard(card) &&
					bases[i] != pile &&
					bases[i]->GetTopCard() != NULL)
				{
					pile->RemoveTopCard(dc);
					bases[i]->AddCard(dc, card);
					DoMove(dc, pile, bases[i]);
					return;
				}
            }
			// try to place the card on any base
			for(i = 0; i < 10; i++)
			{
				if (bases[i]->AcceptCard(card) && bases[i] != pile)
				{
					pile->RemoveTopCard(dc);
					bases[i]->AddCard(dc, card);
					DoMove(dc, pile, bases[i]);
					return;
				}
            }
		}
    }
}


// Test to see whether the game has been won:
// i.e. pack_, discard and bases are empty
Bool Game::HaveYouWon()
{
    if (pack_->GetTopCard() != NULL) return FALSE;
    if (discard->GetTopCard() != NULL) return FALSE;
    for(int i = 0; i < 10; i++)
    {
    	if (bases[i]->GetTopCard() != NULL) return FALSE;
    }
    numWins++;
    totalScore += currentScore;
    currentScore = 0;
    return TRUE;
}


// See whether the card under the cursor can be moved somewhere else
// Returns TRUE if it can be moved, FALSE otherwise
Bool Game::CanYouGo(int x, int y)
{
    Pile* pile = WhichPile(x, y);
    if (pile != NULL && pile != pack_)
    {
	Card* card = pile->GetTopCard();

	if (card != NULL)
	{
	    int i;
	    for(i = 0; i < 8; i++)
	    {
	    	if (foundations[i]->AcceptCard(card) && foundations[i] != pile)
		{
                    return TRUE;
		}
            }
	    for(i = 0; i < 10; i++)
	    {
		if (bases[i]->GetTopCard() != NULL &&
		    bases[i]->AcceptCard(card) &&
		    bases[i] != pile)
		{
		    return TRUE;
		}
            }
	}
    }
    return FALSE;
}


// Called when the left button is released after dragging a card
// Scan the piles to see if this card overlaps a pile and can be added
// to the pile. If the card overlaps more than one pile on which it can be placed
// then put it on the nearest pile.
void Game::LButtonUp(wxDC* dc, int x, int y)
{
    if (srcPile != NULL)
    {
        	// work out the position of the dragged card
		x += xOffset;
        y += yOffset;

		Pile* nearestPile = (Pile*)NULL;
		int distance = (CardHeight + CardWidth) * (CardHeight + CardWidth);

        	// find the nearest pile which will accept the card
		int i;
		for (i = 0; i < 8; i++)
		{
			if (DropCard(x, y, foundations[i], liftedCard))
			{
				if (foundations[i]->CalcDistance(x, y) < distance)
				{
					nearestPile = foundations[i];
                    distance = nearestPile->CalcDistance(x, y);
                }
            }
		}
		for (i = 0; i < 10; i++)
		{
			if (DropCard(x, y, bases[i], liftedCard))
			{
				if (bases[i]->CalcDistance(x, y) < distance)
                {
					nearestPile = bases[i];
                    distance = nearestPile->CalcDistance(x, y);
                }
            }
		}

		// Restore the area under the card
		wxMemoryDC memoryDC;
		memoryDC.SelectObject(bmap_);
		dc->Blit(xPos, yPos, CardWidth, CardHeight,
			   &memoryDC, 0, 0, wxCOPY);
		memoryDC.SelectObject(0);

        	// Draw the card in its new position
		if (nearestPile != (Pile*)NULL)
		{
			// Add to new pile
			nearestPile->AddCard(dc, liftedCard);
			if (nearestPile != srcPile)
			{
				DoMove(dc, srcPile, nearestPile);
			}
		}
        else
        {
			// Return card to src pile
			srcPile->AddCard(dc, liftedCard);
		}
		srcPile = (Pile*)NULL;
		liftedCard = (Card*)NULL;
    }
}




Bool Game::DropCard(int x, int y, Pile* pile, Card* card)
{
    Bool retval = FALSE;
    if (pile->Overlap(x, y))
    {
	if (pile->AcceptCard(card))
	{
	    retval = TRUE;
        }
    }
    return retval;
}


void Game::MouseMove(wxCanvasDC* dc, int mx, int my)
{
    if (liftedCard != NULL)
    {
		wxMemoryDC memoryDC;
		memoryDC.SelectObject(bmap_);

		int dx = mx + xOffset - xPos;
		int dy = my + yOffset - yPos;

		if (abs(dx) >= CardWidth || abs(dy) >= CardHeight)
        {
			// Restore the area under the card
			dc->Blit(xPos, yPos, CardWidth, CardHeight,
			   &memoryDC, 0, 0, wxCOPY);

			// Copy the area under the card in the new position
			memoryDC.Blit(0, 0, CardWidth, CardHeight,
			   dc, xPos + dx, yPos + dy, wxCOPY);
		}
		else if (dx >= 0)
		{
			// dx >= 0
			dc->Blit(xPos, yPos, dx, CardHeight, &memoryDC, 0, 0, wxCOPY);
			if (dy >= 0)
			{
					// dy >= 0
				dc->Blit(xPos + dx, yPos, CardWidth - dx, dy, &memoryDC, dx, 0, wxCOPY);
				memoryDC.Blit(0, 0, CardWidth - dx, CardHeight - dy,
					   &memoryDC, dx, dy, wxCOPY);
				memoryDC.Blit(0, CardHeight - dy, CardWidth - dx, dy,
					   dc, xPos + dx, yPos + CardHeight, wxCOPY);
			}
			else
			{
                		// dy < 0
				dc->Blit(xPos + dx, yPos + dy + CardHeight, CardWidth - dx, -dy,
					   &memoryDC, dx, CardHeight + dy, wxCOPY);
				memoryDC.Blit(0, -dy, CardWidth - dx, CardHeight + dy,
					   &memoryDC, dx, 0, wxCOPY);
				memoryDC.Blit(0, 0, CardWidth - dx, -dy,
					   dc, xPos + dx, yPos + dy, wxCOPY);
			}
			memoryDC.Blit(CardWidth - dx, 0, dx, CardHeight,
	    		   dc, xPos + CardWidth, yPos + dy, wxCOPY);
		}
		else
		{
			// dx < 0
			dc->Blit(xPos + CardWidth + dx, yPos, -dx, CardHeight,
	    		   &memoryDC, CardWidth + dx, 0, wxCOPY);
			if (dy >= 0)
			{
				dc->Blit(xPos, yPos, CardWidth + dx, dy, &memoryDC, 0, 0, wxCOPY);
				memoryDC.Blit(-dx, 0, CardWidth + dx, CardHeight - dy,
					   &memoryDC, 0, dy, wxCOPY);
				memoryDC.Blit(-dx, CardHeight - dy, CardWidth + dx, dy,
					   dc, xPos, yPos + CardHeight, wxCOPY);
			}
			else
			{
                			// dy < 0
				dc->Blit(xPos, yPos + CardHeight + dy, CardWidth + dx, -dy,
					   &memoryDC, 0, CardHeight + dy, wxCOPY);
				memoryDC.Blit(-dx, -dy, CardWidth + dx, CardHeight + dy,
					   &memoryDC, 0, 0, wxCOPY);
				memoryDC.Blit(-dx, 0, CardWidth + dx, -dy,
					   dc, xPos, yPos + dy, wxCOPY);
			}
			memoryDC.Blit(0, 0, -dx, CardHeight,
	    		   dc, xPos + dx, yPos + dy, wxCOPY);
		}
		xPos += dx;
		yPos += dy;

			// draw the card in its new position
		memoryDC.SelectObject(bmapCard_);
		dc->Blit(xPos, yPos, CardWidth, CardHeight,
			   &memoryDC, 0, 0, wxCOPY);

		memoryDC.SelectObject(0);
    }
}



//----------------------------------------------//
// The Pack class: holds the two decks of cards //
//----------------------------------------------//
Pack::Pack(int x, int y) : Pile(x, y, 0, 0)
{
    for (topCard = 0; topCard < NumCards; topCard++)
    {
    	cards[topCard] = new Card(1 + topCard / 2, facedown);
    }
    topCard = NumCards - 1;
}


void Pack::Shuffle()
{
    Card* temp[NumCards];
    int i;

	// Don't try to shuffle an empty pack_!
    if (topCard < 0) return;

	// Copy the cards into a temporary array. Start by clearing
	// the array and then copy the card into a random position.
        // If the position is occupied then find the next lower position.
    for (i = 0; i <= topCard; i++)
    {
	temp[i] = (Card*)NULL;
    }
    for (i = 0; i <= topCard; i++)
    {
	int pos = rand() % (topCard + 1);
	while (temp[pos] != NULL)
	{
	    pos--;
	    if (pos < 0) pos = topCard;
	}
	cards[i]->TurnCard(facedown);
	temp[pos] = cards[i];
        cards[i] = (Card*)NULL;
    }

	// Copy each card back into the pack_ in a random
	// position. If position is occupied then find nearest
        // unoccupied position after the random position.
    for (i = 0; i <= topCard; i++)
    {
	int pos = rand() % (topCard + 1);
	while (cards[pos] != NULL)
	{
	    pos++;
            if (pos > topCard) pos = 0;
	}
        cards[pos] = temp[i];
    }
}

void Pack::Redraw(wxDC* dc)
{
    Pile::Redraw(dc);

    char str[10];
    sprintf(str, "%d  ", topCard + 1);

	dc->SetTextBackground(FortyApp::BackgroundColour());
	dc->SetTextForeground(FortyApp::TextColour());
    dc->DrawText(str, X + CardWidth + 5, Y + CardHeight / 2);

}

void Pack::AddCard(Card* card)
{
    if (card == cards[topCard + 1])
    {
		topCard++;
    }
    else
    {
		wxMessageBox("Pack::AddCard() Undo error", "Forty Thieves: Warning",
		   wxOK | wxICON_EXCLAMATION);
    }
    card->TurnCard(facedown);
}


Pack::~Pack()
{
    for (topCard = 0; topCard < NumCards; topCard++)
    {
	delete cards[topCard];
    }
};


//------------------------------------------------------//
// The Base class: holds the initial pile of four cards //
//------------------------------------------------------//
Base::Base(int x, int y) : Pile(x, y, 0, 12)
{
    topCard = -1;
}


Bool Base::AcceptCard(Card* card)
{
    Bool retval = FALSE;

    if (topCard >= 0)
    {
	if (cards[topCard]->GetSuit() == card->GetSuit() &&
	    cards[topCard]->GetPipValue() - 1 == card->GetPipValue())
	{
            retval = TRUE;
        }
    }
    else
    {
		// pile is empty - ACCEPT
        retval = TRUE;
    }
    return retval;
}

Base::~Base()
{
// nothing special at the moment
};


//----------------------------------------------------------------//
// The Foundation class: holds the cards built up from the ace... //
//----------------------------------------------------------------//
Foundation::Foundation(int x, int y) : Pile(x, y, 0, 0)
{
    topCard = -1;
}

Bool Foundation::AcceptCard(Card* card)
{
    Bool retval = FALSE;

    if (topCard >= 0)
    {
	if (cards[topCard]->GetSuit() == card->GetSuit() &&
	    cards[topCard]->GetPipValue() + 1 == card->GetPipValue())
	{
            retval = TRUE;
        }
    }
    else if (card->GetPipValue() == 1)
    {
	// It's an ace and the pile is empty - ACCEPT
        retval = TRUE;
    }
    return retval;
}

Foundation::~Foundation()
{
// nothing special at the moment
};


//----------------------------------------------------//
// The Discard class: holds cards dealt from the pack_ //
//----------------------------------------------------//
Discard::Discard(int x, int y) : Pile(x, y, 19, 0)
{
    topCard = -1;
}

void Discard::Redraw(wxDC* dc)
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
		if (i == 31)
		{
		    x = X;
                    y = Y + CardHeight / 3;
                }
            }
        }
    }
    else
    {
		Card::DrawNullCard(dc, X, Y);
    }
}


void Discard::GetTopCardPos(int& x, int& y)
{
    if (topCard < 0)
    {
	x = X;
	y = Y;
    }
    else if (topCard > 31)
    {
	x = X + Dx * (topCard - 32);
	y = Y + CardHeight / 3;
    }
    else
    {
	x = X + Dx * topCard;
	y = Y;
    }
}


Card* Discard::RemoveTopCard(wxDC* dc, int xOffset, int yOffset)
{
    Card* card;

    if (topCard <= 31)
    {
		card = Pile::RemoveTopCard(dc, xOffset, yOffset);
    }
    else
    {
    	int topX, topY, x, y;
    	GetTopCardPos(topX, topY);
		card = Pile::RemoveTopCard();
		card->Erase(dc, topX - xOffset, topY - yOffset);
		GetTopCardPos(x, y);
		dc->SetClippingRegion(topX - xOffset, topY - yOffset,
					 CardWidth, CardHeight);

		for (int i = topCard - 31; i <= topCard - 31 + CardWidth / Dx; i++)
		{
			cards[i]->Draw(dc, X - xOffset + i * Dx, Y - yOffset); 
		}
		if (topCard > 31)
		{
			cards[topCard]->Draw(dc, topX - xOffset - Dx, topY - yOffset);
		}
		dc->DestroyClippingRegion();
    }

    return card;
}


Discard::~Discard()
{
// nothing special at the moment
};
