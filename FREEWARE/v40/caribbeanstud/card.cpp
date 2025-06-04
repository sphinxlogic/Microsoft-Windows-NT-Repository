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

// Card
#include <iostream.h>
#include "Card.h"

//## Constructors

Card::Card(int CardIndex)
{
      if(CardIndex >=54 || CardIndex < 0)
      {
         cout << " Card Index " << CardIndex << "Out of Range" << endl;
#ifndef __DECCXX
         throw ( " Card index out of range. " );
#endif
      }
      if(CardIndex >=52 ){
         _suit = 'W';
         _rank = CardIndex - 52;
      }
      else
      {
		 switch(CardIndex/13){
         case 0:
			 _suit = 'C';
          break;
         case 1:
			 _suit = 'D';
          break;
         case 2:
			 _suit = 'H';
          break;
         case 3:
			 _suit = 'S';
       }
       _rank = CardIndex % 13 +2;
     }
}

Card::Card(char Suit, char Rank)
{
	if(Suit == 'W'){
      _suit = Suit; _rank = 0;
   }
   else if ( Rank >=1 && Rank <= 14 &&
				 ( Suit == 'C' ||
					Suit == 'D' ||
					Suit == 'H' ||
					Suit == 'S' ) )
   {
		_suit = Suit;
		_rank = Rank == 1 ? 14 : Rank;
	}
   else
	{
		cout << "Suit " << Suit << " Rank " << (int) Rank << " not Valid " << endl;
#ifndef __DECCXX
		throw ( "Suit, Rank combination does not exist" );
#endif
   }
}

Card::~Card(){}

void Card::ShowFront() const
{
#ifdef DEBUG
   cout << _suit << (int) _rank << ' ';
#endif
}

void Card::ShowBack() const
{
#ifdef DEBUG
   cout << "XX ";
#endif
}

