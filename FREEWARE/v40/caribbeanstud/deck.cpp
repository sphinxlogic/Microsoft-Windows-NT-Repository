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

#include <stdlib.h>
#include <iostream.h>
#include "Card.h"
#include "Deck.h"

// Class Deck 

Card  Deck::_globalCards[54];
int   Deck::_globalInit = 0;

Deck::Deck(int numOfCards) : _numOfCards(numOfCards), _position(0)
{
	if(_globalInit == 0){
		for(int j =0; j < 54; j++)
			_globalCards[j] = Card(j);
		_globalInit = 1;
	}
   for(int i =0 ; i < _numOfCards; i++)
		 _cards[i] = &	_globalCards[i];
   for(i=_numOfCards; i < 54; i++)
		 _cards[i] = (Card *) 0;
}

Deck::~Deck(){}

void Deck::Shuffle()  // shaffle 4 times
{
   Card * tmpcards[54];
   Card **from = _cards;
	Card **to = tmpcards;
   for(int j = 0; j <=4; j++)
	{
		for (int i = 0; i < _numOfCards/2; i++)
        to[i*2] = from[_numOfCards/2+i];
		for ( i = 0; i < _numOfCards/2; i++)
        to[i*2+1] = from[i];
		Card **tmp = from;
      from = to;
		to = tmp;
	}
   ((Deck*)this)->_position = 0;
#ifdef DEBUG
	cout << " Shaffling : " << endl;
   for ( int i = 0; i < _numOfCards; i++ ){
		if( i%10 == 0 ) cout << endl;
		cout <<_cards[i]->Suit() << (int)_cards[i]->Rank() << " ";
   }
	cout << endl;
#endif
}

void Deck::Cut()
{
   Card *tmp[54];

   srand(time(0));
   int pos = rand()%10+2;
   int cut = _numOfCards/pos;

	for(int i=_numOfCards-cut; i< _numOfCards; i++)
		tmp[i] = _cards[i];
   for( i = _numOfCards-1; i >= cut; i--)
		_cards[i] = _cards[i-cut];
	for( i = 0; i < cut; i++)
		_cards[ i ] = tmp[i + _numOfCards-cut];
#ifdef DEBUG
	cout << "Cutting at " << _numOfCards-cut <<endl;
   for ( i = 0; i < _numOfCards; i++ ){
		if( i%10 == 0 ) cout << endl;
		cout << _cards[i]->Suit() << (int)_cards[i]->Rank() << " ";
   }
	cout << endl;
#endif
}

void Deck::Wash()
{
   Card *buff;
   int pos;

   srand(time(0));
   int times = rand()%30;

   for(int j=0; j < times; j++ )
	  for( int i = 0; i < _numOfCards; i++)
     {
		   pos = rand()%_numOfCards;
         buff = _cards[pos];
         _cards[ pos ] = _cards[_numOfCards-1];
         _cards[_numOfCards-1] = buff;
     }
#ifdef DEBUG
	cout << "Washing " <<endl;
   for ( int i = 0; i < _numOfCards; i++ ){
		if( i%10 == 0 ) cout << endl;
		cout << _cards[i]->Suit() << (int)_cards[i]->Rank() << " ";
   }
	cout << endl;
#endif
}

