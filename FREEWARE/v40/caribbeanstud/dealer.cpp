/*
 * (c) Copyright 1997, Qun Zhang.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Qun Zhang not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Qun Zhang make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * THE ABOVE-NAMED DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE ABOVE-NAMED BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include "stdlib.h"
#include <iostream.h>
#include "Card.h"
#include "Deck.h"
#include "Dealer.h"
#include "Player.h"

Dealer::Dealer( Seat * dealerSeat ) : _seat(dealerSeat), _current(0)
{
   _minjackPot = 2000000;

   for(int i=0; i<10; i++)
		_players[i] = (Player*) 0;
}
void Dealer::CollectAnte() const
{
   int ante;

	for(int i=0; _players[i]; i++)
   {
		cout << endl << "Progressive ? 0/1" << endl;
		cin >> ante;
		_players[i]->Progressive(ante);
		cout << "Bet Ante: " << endl; // need to be replaced by GUI
		cin >> ante;
#ifndef VMS
		try{
#endif
		   _players[i]->Ante(ante);
#ifndef VMS
		}
		catch (const char *mess)
		{
			cout << mess << endl;
		}
#endif
	}
}

void Dealer::DealCards()
{
	Card *card;

   for(int j=0; j< 5; j++)
	{
		for(int i=0; _players[i]; i++)
			_players[i]->AddCard(_deck->GetCard());
		card =_deck->GetCard();
#ifdef DEBUG
		if( j == 4 ) card->ShowFront();
		else card->ShowBack();
#endif
		AddCard(card);
  	}
}

void Dealer::AskBet() const
{
	Card ** hand;
	char    v;

	for(int i=0; _players[i]; i++)
	{
		hand = _players[i]->Cards();
		for(int j=0; j < 5; j++)
			hand[j]->ShowFront();
		cout << "Bet ? : Y/N ";
		cin >> v;
		if(v == 'Y' || v == 'y') _players[i]->Bet(10);
      else _players[i]->Fold();
	}
}

void Dealer::CollectBet() const
{
	Card ** hand = Cards();

	for(int j=0; j < 5; j++)
		hand[j]->ShowFront();
	cout << endl;

	if(Score() < 573496) // Does not have a King/Ace or higher
   {
#ifdef DEBUG
		cout << "Dealer does not have a ACE/KING or higher." << endl;
#endif
		for(int i=0; _players[i]; i++)
		{
			(_players[i]->Score()) ?  // see if the player has folded.
				_players[i]->Pay(_players[i]->Ante()):
				_players[i]->Pay(- _players[i]->Ante());
			_players[i]->NewGame();
		}
	}
	else{ // Pay based on Hands
		int amount;
		for(int i=0; _players[i]; i++)
		{
			if(_players[i]->Score() < Score())
			{
				amount = _players[i]->Ante() + _players[i]->Bet();
				amount = -amount;
			}
			else if (_players[i]->Score() == Score())
			{
				amount = 0;
			}
			else{
				amount =  _players[i]->Ante() +
					       _players[i]->Bet()*_players[i]->PayRate();
#ifdef DEBUG
				cout << "Ante: " << _players[i]->Ante() <<endl;
				cout << "PayRate: " << _players[i]->PayRate() <<endl;
				cout << "TotalPay: " << amount <<endl;
#endif
			}
			_players[i]->Pay(amount);
			_players[i]->NewGame();
		}
	}
}

void Dealer::NewGame()
{
	for(int i=0; _players[i]; i++)
		_players[i]->NewGame();

	Hands::NewGame();

   srand(time(0));
   long pot = rand()%1000000;

   _jackPot += (rand()%1) ? pot : -pot;
   _jackPot = ( _jackPot > _minjackPot) ?  _jackPot : _minjackPot;

	if(_deck->CardsLeft() < i*5+10)
	{
		_deck->Shuffle();
		_deck->Cut();
		_deck->Cut();
	}

   _current = 0;
}

