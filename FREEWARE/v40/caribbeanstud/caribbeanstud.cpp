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
#include "Dealer.h"
#include "Deck.h"
#include "Player.h"
#include "Table.h"


int main(int argc, char ** argv)
{
#ifndef VMS
	 try{
#endif
		 Table table(argc, argv);
		 Deck  deck(52);

		 deck.Wash();
		 deck.Shuffle();
		 deck.Cut();
		 deck.Shuffle();
		 deck.Cut();
		 deck.Shuffle();
		 deck.Cut();
		 deck.Cut();

		 table.GetDealer()->SetDeck(&deck);
		 table.Start();
#ifndef VMS
	 }
    catch (char *msg)
    {
		cout << msg << endl;
    }
#endif
}
