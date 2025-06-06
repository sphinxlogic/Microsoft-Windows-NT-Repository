
Welcome to Forty Thieves
------------------------

by Chris Breeze, July 1997
chris.breeze@iname.com

This implementation is a demo for the wxWindows portable GUI toolkit:

        http://web.ukonline.co.uk/julian.smart/wxwin

Rules of the game
-----------------

Forty Thieves is a patience game played with two full packs
of cards. At the start of the game forty cards are dealt on
the eight 'bases' along the top of the window. The object of
the game is to place all the cards onto the eight 'foundations'.
The foundations are built starting with the ace and adding cards
of the same suit up to the king.

Cards are dealt from the pack and placed on the discard pile.
Cards may be moved from the discard pile or one of the bases
to a base or a foundation. Only one card can be moved at a time.
Cards can only be placed on a base if the top card of the base
is of the same suit and is one higher in pip value or the base
is empty e.g. the eight of spades can only be placed on top of
the nine of spades.

When the mouse cursor is over a card which can be moved it
changes to the 'hand' cursor. The card can then be moved by
double clicking the left button.

The mouse cursor also changes to a hand when a card is
dragged by placing the cursor over the card and holding
down the left button. This feature can be enabled and disabled
by selecting the 'Helping hand' option from the Edit menu.

The 'foundations' are the eight piles of cards down the left
side of the window. When the game starts these piles are
empty. The object of the game is to place all the cards on
the foundations. An ace can be placed on any empty foundation.
Other cards can only be placed on a foundation if the top card
is of the same suit and is one lower in pip value
e.g. the three of clubs can be placed on the four of clubs.

The 'bases' are the ten piles of cards along the top of the
window. At the start of the game four cards are dealt face
up on each of the bases.
A card can be added to a base if the base is empty or if the
top card is of the same suit and is one higher in pip value
e.g. the queen of hearts can be placed on the king of hearts
The top card of a base can be moved onto another base or a
foundation.

Cards can be only moved one at a time. The top card of the
pack can be dealt onto discard pile by placing the mouse
cursor over the pack and pressing the left button. The
number of cards remaining is displayed to the right of the
pack.

Cards can be moved from the discard pile or the bases either
by 'double-clicking' or by dragging. If the left button is
double-clicked when the mouse cursor is over a card and it
can move to another pile, it will do so. This is a quick way
of moving cards when their destination is unambiguous.

A card can be dragged by placing the mouse cursor over the
card and holding down the left button. The card will follow
the mouse cursor until the left button is released. If the
card is over a pile on which it can be placed it will be
added to that pile, otherwise it will be returned to the
pile from which it was dragged.

One point is scored for every card that is placed on a
foundation. Since there are two packs of 52 cards the
maximum score is 104. A record is kept of the number of
games played, the number of games won, the current score
and the average score. This information is displayed at
bottom right of the window and is stored on disk between
games. A game is deemed to have started if the cards have
been dealt and any card has been moved. If the game is
abandoned before it is finished (i.e. by starting a new
game or closing window) it counts as a lost game.
New players can be added by selecting the 'Player' option
from the Game menu. A summary of players' scores can be
displayed by selecting the 'Scores...' option from the Game
menu.

All moves are recorded and can be undone. To undo a move
select the undo menu item from the Edit menu. A quicker
way of undoing is to press the right mouse button (it
doesn't matter where the mouse cursor is).
Right button undo can be enabled and disabled by selecting
the 'Right button undo' option from the Edit menu.
Pressing the right mouse button with the control key pressed
re-does a previously undone move.

An empty base or two is very useful as it gives the opportunity
to unscramble other bases. Try not to build onto kings which
obscure valuable cards as it will be difficult to get to them
later. The undo facility is very useful for going back and using
'hindsight'.

Don't be put off if you can't win every game. I reckon winning
1 in 10 is pretty good (winning 1 in 3 is excellent).


