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

#ifndef Player_h
#define Player_h 1

#include "Hands.h"
#include "Dealer.h"

class Seat;

class Player : public Hands

{
  public:
      Player( Dealer*, Seat* seat);
      virtual ~Player() { _dealer->Remove(this); } 

		virtual void NewGame();
		virtual void Progressive(int);
		virtual int  Progressive() const;
      virtual void Ante  (int);
		virtual int  Ante     () const;
      virtual void Check	 ();
      virtual void Bet   (int);
		virtual int  Bet      () const;
      virtual void Fold		 ();
      virtual void Call  (int);
      virtual void Raise (int);
      virtual void Pay	 (int);
      virtual void BankRoll(int);
		virtual int  BankRoll( ) const;
              int  WinLost( ) const;
		Seat*			 GetSeat() const { return _seat; }
  protected:
		virtual void  SortCards  () const;  //defauld sord plus redraw;

  private:
      Player(const Player &right);
      const Player & operator=(const Player &right);
      int operator==(const Player &right) const;
      int operator!=(const Player &right) const;

  private:  //## implementation

      Dealer *_dealer;
      Seat *  _seat;
		int	  _bankroll;
		int	  _ante;
		int     _bet;
		int     _progressive;
      int     _lastWinLost;
};

inline Player::WinLost( ) const
{
	 return _lastWinLost;
}

// Class Player 

#endif


