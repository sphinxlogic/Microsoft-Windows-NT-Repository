	GENERIC TETRIS
	==============	Yet Another Tetris Game on X, V3.1.2


WHY ANOTHER?

    Because "different is good"...

    Because this one is simple, easy to build, and portable --
    actually the blocks are round-cornered and bigger, more
    comfortable for your eyes.  It only uses Xlib -- no "toolkit" kind
    of thing.  The most updated source could be found on ftp.x.org
    under /contrib.


HOW TO BUILD?

    Edit the Imakefile (or Makefile.std, see below) for appropriate
    settings of variable SCOREFILE and RANDOM, then do:
	xmkmf
	make

    Note: if you don't have 'xmkmf' or the "Makefile" it generates
    doesn't work well, try:
	make -f Makefile.std

    "Tetris" is the game.  It only lists the top 15 players at the
    end, each player may has at most three scores.  With the option
    "-s", "tetris" will list all the scores.


ANYTHING SPECIAL IN PLAYING?

    Use 'j' to move left; 'l' to move right; 'k' to rotate. 
    Left-handed people may want to use 's', 'd', 'f' respectively. 
    Use the space bar to drop a block quickly.  Do you want to try the
    arrow keys?

    Use CTRL-L key combination to redraw -- in case the program messes
    somthing up.

    As usual, '+' and '-' will speed up or lower down the speed.  Note
    that you don't need to press the SHIFT key -- '=' and '_' also
    work.  The 'b' key can be used to toggle "beep" and 'n' toggles
    "show next".

    If you press 'p', the game will be paused, and the window will be
    iconified -- just in case your boss suddenly appears at your door. 
    ;o) When the tetris window loses its focus, it also goes to
    "paused" mode except it doesn't iconify itself.

    Refer to the man page for detailed command line options.


FOUND A BUG?

    Send bug reports (or fixes) to the author:

	Q. Alex Zhao,	azhao@cc.gatech.edu
	College of Computing
	Georgia Institute of Technology
	Atlanta, GA 30332-0280

