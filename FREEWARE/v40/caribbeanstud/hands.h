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

#ifndef Hands_h
#define Hands_h 1

#include "Card.h"


class Hands 
{
  public:
		enum  Hand { EX =0,
						 NP ,	
						 P1 ,	
						 P2 ,	
						 K3 ,	
						 ST ,
						 FL ,
						 FH ,
						 K4 ,
						 SF ,
						 RF 
      };

      Hands();
      virtual ~Hands() {};

      int operator==(const Hands &right) const;
      int operator!=(const Hands &right) const;
      int operator>=(const Hands &right) const;
      int operator<=(const Hands &right) const;
      int operator>(const Hands &right) const;
      int operator<(const Hands &right) const;

      virtual void  AddCard  (Card *card);
#ifndef VMS
      virtual Hand  HandValue				() const;
#else
      virtual void  HandValue	(const Hand hand);
#endif
      virtual long  Score					() const;
      virtual int   PayRate				() const;
      virtual void  NewGame				() ;
		virtual Card** Cards   		      () const;
		virtual int   NumOfCards   		() const;
      virtual const char* const HandName() const;

  protected:
		virtual void  SortCards				() const;

  private:
		void          ReArrange(char*, int nel, char*, Hand);
      const Hands & operator=(const Hands &right);

  private:
      Card*			_cards[5];
		long			_score;
		Hand			_hand;
		int         _next;
	   static      const int  Ratio[11];
	   static      const char * HandNames[11];
};

inline int Hands::operator==(const Hands &right) const
{
		return ( Score() == right.Score() );
}

inline int Hands::operator!=(const Hands &right) const
{
		return ( Score() != right.Score() );
}

inline int Hands::operator<=(const Hands &right) const
{
		return ( Score() <= right.Score() );
}

inline int Hands::operator>=(const Hands &right) const
{
		return ( Score() >= right.Score() );
}

inline int Hands::operator>(const Hands &right) const
{
		return ( Score() > right.Score() );
}

inline int Hands::operator<(const Hands &right) const
{
		return ( Score() < right.Score() );
}

#endif


