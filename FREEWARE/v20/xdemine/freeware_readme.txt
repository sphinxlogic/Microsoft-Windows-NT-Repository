XDEMINE, GAMES, X11 minesweeper game

                                  xdemineur 1.1

                    Author : Marc Baudoin (baudoin@ensta.fr)

                                 |\
                               |\| \
                               |//  )
                             |\ /  /        /\/\_
                             |//  /        /. . /
                           |\ /  /     ___/    |_
                           |//  /     (____/    /_/\_
                            |__|      (_____/ __     >
                          /| ___  ________ _< \ \__  >
                          \|| __\| _|_   _/ \\ \___\/
                            | __\____ | |/ _ \\    >
                          /||___\_____|___/ \_\\  _>
                          \|____           ____ \|
                            \   \_________/   /
                             \   _    _      /
                              \_//   //_____/
                                (_   \ (_  _\
                                  |/\|   \/

            Ecole Nationale Superieure de Techniques Avancees (ENSTA)
                   32, boulevard Victor - 75015 Paris - France



                         Copyright (C) 1993 Marc Baudoin

     Permission to use, copy, modify, and distribute this software and
     its documentation for any purpose and without fee is hereby granted,
     provided that the above copyright notice appear in all copies and that
     both that copyright notice and this permission notice appear in
     supporting documentation.  The author makes no representations about
     the suitability of this software for any purpose.  It is provided "as is"
     without express or implied warranty.

********************************************************************************

Xdemineur is not just another version of the minefield game, it's the best!

DESCRIPTION

The object of the game is to find the location of the mines and to mark their
location by placing a small flag.  You have exactly as many flags as there are
mines, no more, no less.  The number of flags that remain to be placed is
displayed in the top left corner and a timer is displayed in the top right
corner.

The game is played using a three buttons mouse.  The left button is used to
uncover the cell on which you click (hoping that a mine is not hidden there)
and the right button is used to place a flag (this indicates that there is a
mine here).  If you click again with the right button on a cell that has a
flag, the flag is changed into a question mark: this if you are trying to guess
the possible location of specific mines.  One more click with the right button
and the question mark disappears.  Clicking with both buttons on an uncovered
cell which has all its flags uncovers the surrounding cells (this will save a
lot of time).

The numbers displayed in uncovered cells indicate the number of mines which
are located all around (for example, a 3 means that there are three mines
within the 8 adjacent cells).  The whole game relies on the use of these
numbers.

To start playing, click randomly to uncover some cells until you find an
"opening" (a number of cells that will be uncovered at the same time) or
specify the option -ouvre which automatically finds an opening.

Finally, to start a new game, just click on the little yellow face above the
playfield.

