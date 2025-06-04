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

#include <iostream.h>
#include "Player.h"

//## begin module.additionalDeclarations preserve=yes
//## end module.additionalDeclarations


// Class Player 



Player::Player(Dealer* dealer, Seat* seat) :  _dealer(dealer),
											 				 _seat(seat),
		                            				 _bankroll(0),
		                            				 _ante(0),
	                               				 _bet(0),
                                  				 _progressive(0),
															 _lastWinLost(0)
{
}


//## Other Operations (implementation)

void Player::NewGame()
{
	Hands::NewGame();
	_bet = _ante = _progressive = 0;
#ifdef DEBUG
	cout << "Money Left: " << BankRoll() << endl;
#endif

}

void Player::Progressive(int p)
{
	_progressive = p;
	if(p) _bankroll -=1;
}

int  Player::Progressive() const
{
	return _progressive;
}

void Player::Ante(int ante)
{
	if(ante <=0 )
#ifndef __DECCXX
		throw ("Ante amount can not be less than $0."); 
#else
		cout << "Ante amount can not be less than $0." << endl; 
#endif
	if(ante > _bankroll/3)
   {
#ifdef DEBUG
     cout << "Ante: " << ante << endl;
     cout << "Bankroll: " << _bankroll << endl;
#endif
#ifndef __DECCXX
		throw ("Don't have enough money left for ADDITIONAL BET.");
#else
		cout << "Don't have enough money left for ADDITIONAL BET." << endl;
#endif
   }
   _ante = ante;
	_bankroll -= _ante;
}

int Player::Ante() const
{
   return _ante;
}

void Player::Check()
{
  //## begin Player::Check%849130052.body preserve=yes
  //## end Player::Check%849130052.body
}

int Player::Bet() const
{
	return _bet;
}

void Player::Bet(int amount) 
{
	_bet = 2*_ante;
	_bankroll -= _bet;
}

void Player::Fold()
{
    Hands::NewGame();
}

void Player::Call(int)
{
  //## begin Player::Call%849130055.body preserve=yes
  //## end Player::Call%849130055.body
}

void Player::Raise(int)
{
  //## begin Player::Raise%849130056.body preserve=yes
  //## end Player::Raise%849130056.body
}

void Player::Pay(int amount)
{
	_bankroll += amount;
	_bankroll += _bet;
	_bankroll += _ante;
   _lastWinLost = amount;
	if(_progressive){
#ifndef VMS
		switch(HandValue()){
#else
    Hand The_hand;
    HandValue(The_hand);
               switch(The_hand){ 
#endif
		case  FL:
			_bankroll += 50;
   		_lastWinLost += 50;
			break;
		case  FH:
			_bankroll += 75;
   		_lastWinLost += 75;
			break;
		case  K4:
			_bankroll += 100;
   		_lastWinLost += 100;
			break;
		case  SF:
			_bankroll += _dealer->JackPot()/10;
   		_lastWinLost +=  _dealer->JackPot()/10;
			break;
		case  RF:
			_bankroll += _dealer->JackPot();
   		_lastWinLost +=  _dealer->JackPot();
			break;
		default:
			break;
		}
	}
	NewGame();
}

void Player::BankRoll(int amount)
{
	_bankroll = amount;
}

int Player::BankRoll() const
{
	return _bankroll;
}

void  Player::SortCards  () const  //defauld sord plus redraw;
{
	Hands::SortCards();
	// redraw
}


