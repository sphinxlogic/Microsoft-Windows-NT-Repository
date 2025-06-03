*** XTIC version 1.12 - 18th june 1995 ***

Description:
------------
Xtic is a board game designed for the X windows environment.
The game is a two-player game, although for the moment,it
is only possible to play against the computer. The board is
composed of 4x4 squares and 16 pieces. Each piece has four 
properties: black or brown, horizontal or vertical, solid or hollow,
round or square. This makes up 16 possible combinations and there
is exactly one piece for each possibility.

Initially the board is empty. Now the two players try to place 
out pieces in a way that the opponent has to complete a row, 
column or diagonal with a common property (e.g. all black). 
This makes the opponent lose. The players do not choose what piece
to put but it is rather player 1 who gives a piece to player 2, who
then has to place this piece somewhere and choose a piece for
player 1 to put somewhere etc.

The rules are really very simple - believe me!

In any case, there is a more extensive description of the rules
in the manual file xtic1.12/src/xtic.man in the distribution directory.
To read the manual without installing it, try nroff -man xtic.man

Requirements:
-------------
To install and play the game you need:

1) The XPM library, installed at a reasonable place. If you don't
   have it you can get it at many anonymous ftp sites.

2) A Color X display and the MIT X11 Server. X11R5 and X11R6 should work.

3) xmkmf/imake correctly installed and in your path.


Where to get it:
---------------

Xtic resides in ftp.x.org in the /contrib/games directory. You'd
better get it at one of the numerous mirror sites, though.
Alternatively, you could check out my URL http://www.math.kth.se/~mjo


Which is the latest version:
----------------------------
Xtic1.12  (as of 18th June 1995)


Platforms:
----------
Xtic has been tested on a number of platforms, including

SUN SparcStation 5 with SUNOS 4.1.3
Pentium PC with Linux 1.1.59

I have tried to design the program and the installation files 
in a machine-independent way. This, however, does not necessarily 
mean that the program will compile everywhere at once :-(.


Installation:
-------------
First you have to unpack the software. The distribution comes in two files:

xtic1.12.tar.gz  -  GNU gzipped tar file. Unpack/-tar with:
                    zcat xtic1.12.tar.gz | tar xfv -
xtic1.12.README  -  This file.

After untarring you will get a directory called xtic1.12 containing all the
relevant files. Now begin the installation and start by reading the
file INSTALL. Hopefully everything works out fine :-)


Usage:
-----
If the installation works out fine you can just start the game by entering
"xtic" if you have the appropriate directory (e.g. /usr/local/X11/bin)
in your path. Otherwise, step into the directory xtic1.12/src and type "xtic".
There is the usual hassle with the X11 colormap, unless you have much more
than 256 colors. You may notice this if you are running e.g. netscape.


Thanks:
------------
I want to thank Karl-Johan Johnsson for helping me with the 
Xt/Xaw-programming and everyone responsible for excellent libraries
such as XPM.


Comments etc.:
--------------
Any suggestions, bug reports, complaints etc. should be sent to
mjo@math.kth.se. Feedback is appreciated!
Anyone wanting to compensate the author for the amount
of spare time wasted on this project are welcome to send a check of
20 USD to the address below.

Mattias Jonsson
Dept.of Mathematics
Royal Institute of Technology
S-100 44 Stockholm
Sweden

Email: mjo@math.kth.se
URL: http://www.math.kth.se/~mjo
