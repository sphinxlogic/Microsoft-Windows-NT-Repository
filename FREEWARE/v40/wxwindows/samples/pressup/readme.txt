
		README.TXT for pressup game.


This game is a wxWindows port of the game "pressup",
found in the C-Users Grouo Archive. It recyles the game playing code
taken from there. The following extract from the original listing should
explain the game play.

/* Press-up game...
       pressup <options>
  where <options> may include
       -f      Machine goes first
       -d n    Search depth is n moves (default 3)
		(greater search depths take longer...but
		 play better!!)
       -b	Print machine's evaluation of its moves.

	THIS PROGRAM WILL ONLY WORK ON TERMINALS HAVING
	LOWER CASE CHARACTERS!!!!!!!!!!!!!!!!!!!!!!!!!!

	This excellant program was written by:
		Prof. Steve Ward
		Director, Real-Time Systems Group
		MIT Lab for Computer Science
		Cambridge, Massachussetts, 02139

		(slightly modified by Leor Zolman)

	The game of Press-Ups is played as follows:
	The board is a n by n array of pegs, each of
	which is standing up at the start of the game. Pegs
	come in 3 colors: Red (yours), Blue (the machine's),
	and white (periods, actually; they're neutral.)

	The first player to move must"push down" a neutral
	peg. Thereafter, players take turns pushing down pegs,
	where each peg pushed must be adjacent to the last one
	pushed.

	Pegs are named by giving a letter and a number, for the
	row and column of the desired peg.

	As soon as a player gets all of his pegs down, he wins.
	When there are no more legal moves to play,
	the player with the most of his own colored pegs down
	is the winner.

	Watch out...at search depths of 6 or more, this program
	plays a mean game!!!
*/

I have slightly adapted the original code for dumb terminals so that
it compiles for modern ANSI compilers (at least for my BORLAND 3.1 compiler).
The files are  pressup.h and pressup.c.


The wxWindows code is in the files board.h, board.cc, gui.h, gui.cc, with the extra files
pressup.rc and pressup.def for MS_WINDOWS.
It does not require any special special wx library utilities. I have compiled it
using Borland 3.1 using wxWindows versions 1.60 and 1.63.  I put the code together
on a couple of free afternoons for recreation and to play around with the canvas class.
It will therefor certainly show inefficiencies and deficiencies, and probably could be
tidied up further.   All play selection is done via the left mouse button.


Things which could be added or made better.

1) an undo option to take back moves.
2) the possibility to swap sides with the computer.
3) make the window resize to the board size when playing on a larger board.


Have fun.

Robert Cowell
School of Mathematics, Actuarial Science and Statistics
City University
Northampton Square
London EC1V 0HB

Oct 1995.

----------

Modified slightly by Julian Smart: renamed a few files,
created more makefiles, added a marble backdrop, centred
board on window.

October 29th 1995