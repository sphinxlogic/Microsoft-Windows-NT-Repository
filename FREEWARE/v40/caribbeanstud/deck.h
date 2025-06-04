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

#ifndef Deck_h
#define Deck_h 1

#include <sys/types.h>
#include <time.h>
#include "Card.h"

class Deck 
{
  public:
      Deck(int numOfCards);
      ~Deck();

      Card * 	GetCard	() ;
      void		Shuffle	() ;
      void 		Cut		() ;
      void 		Wash		() ;
      int 		CardsLeft() const;

  protected:

  private:
      Deck(const Deck &right);
      const Deck & operator=(const Deck &right);
      int operator==(const Deck &right) const;
      int operator!=(const Deck &right) const;

  private:
		int   _numOfCards;
		int   _position;  // point to the next card to be dealed
		Card * _cards[54];
		static Card _globalCards[54];
		static int  _globalInit;
};


inline Card * Deck::GetCard() 
{
	return _cards[ _position++ ];
}

inline int Deck::CardsLeft() const
{
	return _numOfCards-_position;
}

#endif
