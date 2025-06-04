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

#include "Card.h"
#include "Hands.h"


const int	 Hands::Ratio[11] = {0, 1, 1, 2, 3, 4, 5, 7, 20, 50, 100 };
const char * Hands::HandNames[11] = { "          ",
                            "          ",
                            " One Pair ",
                            " Two Pairs",
                            " 3 of a Kind",
                            " Straight ",
                            " Flush ",
                            " Full House",
                            " 4 of a Kind",
                            " Straight Flush ",
                            " Royal Flush " };

 

Hands::Hands() : _score(0), _hand(EX)
{
      for(int i=0; i < 5; i++)
			_cards[i] = ( Card *)0;
		_next = 0;
}

#ifndef VMS
Hand Hands::HandValue() const
{
       return	_hand;
}
#else
void Hands::HandValue(const Hand _hand)
{
}
#endif

int Hands::NumOfCards() const
{
	return (_cards[0] == (Card*) 0) ? 0 : 5;
}

long Hands::Score() const
{
	if(_score == 0)
		SortCards();
#ifdef DEBUG
   cout << "Score: " <<  _score <<endl;
#endif
	return _score;
}

const char* const Hands::HandName() const
{
	return Hands::HandNames[_hand];
}

int Hands::PayRate() const
{
	return Ratio[ (int)_hand ];
}

void Hands::AddCard(Card *card)
{
	_cards[_next++] = card;
}

void Hands::NewGame()
{
   _score =0;
	_hand = EX;
   _next = 0;
//   for(int i=0; i < 5; i++)
//      _cards[i] = ( Card *)0;

}

Card** Hands::Cards() const
{
	return _cards;
}

void Hands::SortCards() const
{
	char ranks[6];

   if(!_next) return;

   for(int i=0; i<5; i++)
		ranks[i] = _cards[i]->Rank();
	
	for(int j=0; j < 4; j++) // sorting based on rank
		for( i=0; i < 4-j; i++)
		{
			if( ranks[i] < ranks[i+1] )
			{
				ranks[6] = ranks[i];
				ranks[i] = ranks[i+1];
				ranks[i+1] = ranks[6];
			}
		}
	ranks[6] = '\0';
			
   // first looking to 1p, 2p, 3k, fh and 4k

	char r = ranks[0];
	char board[4];
	for( i =0; i<4; i++) board[i] = '\0';

   int s=1;
   for ( i=1; i< 6; i++)
   {
		if( r == ranks[i] ) s++;
		else{
			switch(s){
			case 2:
			{
				int t = (board[0] == '\0') ? 0 : 1;
				board[t] = r;
			}
				break;
			case 3:
				board[2] = r;
				break;
			case 4:
				board[3] = r;
				break;
			}
			s = 1;
			r = ranks[i];
		}
	}

	((Hands*)this) ->_hand = NP;

   if( board[0] || board[1] || board[2] || board[3] )
	{
		if(board[3])
			((Hands*)this) ->_hand = K4;
		else if(board[2])
			((Hands*)this) ->_hand = (board[0]) ? FH : K3;
		else
			((Hands*)this) ->_hand = (board[1]) ? P2 : P1;

		((Hands*)this) ->ReArrange(ranks, 5, board, _hand);
   }
	else{
		if(ranks[0] == (char) '\14') ranks[5] = '\1';
		for( i =0; i < 4; i++)
			if(_cards[i]->Suit() != _cards[i+1]->Suit())
			{
				i = 8;
				break;
			}
		if( i != 8 ) ((Hands*)this) ->_hand = FL;
		for( i = 0; i<4; i++)
			if(ranks[i] != ranks[i+1] + 1)
			{
				i=8;
				break;
			}
		if(i==8 && ranks[i] == '\14') // for ACE as the first one
			for(i=1; i<4; i++)
				if(ranks[i] != ranks[i+1] + 1)
				{
					i=8; break;
				}
		if( i != 8 )
		{
			((Hands*)this) ->_hand = ( _hand == FL ) ? SF : ST;
			if(ranks[4] == ranks[5]+1)
				for(ranks[5] = '\14',i=0; i<5; i++)
					ranks[i] = ranks[i+1];
			if(ranks[0] == '\14'&& _hand == SF) ((Hands*)this) ->_hand = RF;
		}
	}

// calculate the score for a hand

	((Hands*)this) ->_score = ((int)_hand -1) * 7529536;
	((Hands*)this) ->_score+=((((ranks[0]*14)+ranks[1])*14+ranks[2])*14+ranks[3])*14+ranks[4];

// rearrange cards order

	Card* tmpcards[5];
   for(i = 0; i < 5; i++) tmpcards[i] = _cards[i];
	for(i = 0; i < 5; i++)
		for(j=0; j<5; j++)
			if(tmpcards[j] && ranks[i]==tmpcards[j]->Rank())
			{
				((Hands*)this) ->_cards[i] = tmpcards[j];
				tmpcards[j] = (Card *)0;
				break;
			}

#ifdef DEBUG
		for(i = 0; i < 5; i++)
		  cout << _cards[i]->Suit() << (int) _cards[i]->Rank() << " ";
      cout << endl;
#endif

}

void    Hands::ReArrange(char* ranks , int nel,char* board, Hand score)
{
	int j=0 , k = 0;
	char tmprank[10];
   for( int i = 0; i < nel; i++)  // copy all the cards to the tmprank
		tmprank[i] = *(ranks+i);
	tmprank[nel] = '\0';

	switch(score) {
	case  P1:
		for( k=2,i = 0; i < nel; i++)
			if(tmprank[i] == *board) *(ranks+j) = tmprank[i], j++;
		   else *(ranks+k) = tmprank[i], k++;
		break;
	case  P2:
		for( k=2,i = 0; i < nel; i++)
			if(tmprank[i] == *board) *(ranks+j) = tmprank[i], j++;
		   else if(tmprank[i] == *(board+1)) *(ranks+k) = tmprank[i], k++;
			else *(ranks+4) =  tmprank[i];
		break;
	case  K3:
	case  FH:
		for( k=3,i = 0; i < nel; i++)
			if(tmprank[i] == *(board+2)) *(ranks+j) = tmprank[i], j++;
		   else *(ranks+k) = tmprank[i], k++;
		break;
	case  K4:
		for(i = 0; i < nel; i++)
			if(tmprank[i] !=*(board+3) )
			{
		     *(ranks+i) = *(board+3);
			  *(ranks+4) = tmprank[i];
			  break;	
			}
   }
}

