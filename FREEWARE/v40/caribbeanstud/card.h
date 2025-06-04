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

#ifndef Card_h
#define Card_h
#include <iostream.h>

class Card 
{
  public:

		Card() {};
      Card(int CardIndex);
      Card(char Suit, char Rank);

    //## Destructor
      virtual ~Card();

    //## Assignment Operation (generated)
	 //  const Card & operator=(const Card &right);

    //## Equality Operations (generated)

      int operator==(const Card &right) const;
      int operator!=(const Card &right) const;

      virtual void ShowFront() const;
      virtual void ShowBack () const;
      char			 Suit     () const;
      char 			 Rank     () const;

  protected:
  private:

  private:  //## implementation
		char _suit;
		char _rank;
};

// Class Card 

inline Card::operator==(const Card &right) const
{
	return ( _suit == right._suit &&
				(_rank == right._rank ||
				 _rank-13 == right._rank ||  // for ACE
				 _rank == right._rank-13
            )
		    );
}

inline Card::operator!=(const Card &right) const
{
	return !( operator == (right) );
}

inline  char	 	Card::Suit     () const
{
	return _suit;
}

inline  char 		Card::Rank     () const
{
	return _rank;
}

#endif

