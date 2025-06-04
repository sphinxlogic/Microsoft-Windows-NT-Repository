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

#ifndef Dealer_h
#define Dealer_h 1
#include <assert.h>


#include "Hands.h"

class Player;
class Deck;
class Seat;


class Dealer : public Hands
{

  public:
      Dealer( Seat * dealerSeat );
      ~Dealer() {};

      void CollectAnte	() const;
      void DealCards		() ;
      void AskBet			() const;
      void CollectBet	() const;
      void NewGame		() ;
		long JackPot      () const;
		void JackPot      (long);
		void MinJackPot   (long);
      void Add    (Player *) ;
      void Remove (Player *) ;
		Deck* GetDeck () const;
		void  SetDeck (Deck*);
		Seat* GetSeat () const;
		Player* CurrentPlayer() { return _players[_current]; }
		Player* FirstPlayer() { _current = 0; return _players[_current]; }
		Player* NextPlayer();
		int	CurrentSpot()  { return _current + 1; }
  protected:


  private:
      Dealer(const Dealer &right);
      const Dealer & operator=(const Dealer &right);
      int operator==(const Dealer &right) const;
      int operator!=(const Dealer &right) const;

  private:
      Deck *   _deck;
      Seat *   _seat;
		long     _jackPot;
		long     _minjackPot;
      Player * _players[10];
		int      _current;
};

inline long Dealer::JackPot() const
{
	return _jackPot;
}

inline void Dealer::MinJackPot(long jackpot)
{
	 _minjackPot = jackpot;
}

inline void Dealer::JackPot(long jackpot)
{
	 _jackPot = jackpot;
}

inline void Dealer::Add    (Player* newPlayer)
{
   for(int i=0; i < 10; i++)
      if( !_players[i] ){
			_players[i] = newPlayer;
         break;
      }
#ifndef __DECCXX
   if(i == 10) throw (" Can not add more player. ");
#else
   if(i == 10) cout << " Can not add more player. " << endl;
#endif
}

inline void Dealer::Remove (Player* aplayer)
{
   int pos = 20;
	for(int i=0; _players[i]; i++)
		if(aplayer == _players[i])
		{
			pos = i; break;
		}

#ifndef __DECCXX
   if(pos >= 10) throw ("Player not in dealer list. ");
#else
   if(pos >= 10) cout <<"Player not in dealer list. " << endl;
#endif
	for(i=9; !_players[i]; i--);
   _players[pos] = _players[i];
   _players[i] = (Player *) 0 ; 
}

inline Deck* Dealer::GetDeck () const
{
	return _deck;
}

inline void  Dealer::SetDeck (Deck* deck)
{
	assert(deck);
	_deck = deck;
}

inline Seat*   Dealer::GetSeat () const
{
	return _seat;
}

inline Player* Dealer::NextPlayer()
{
	if(_players[_current+1])
		return _players[++_current];

	return (Player*) 0;
}

#endif


