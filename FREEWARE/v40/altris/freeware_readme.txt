ALTRIS, GAMES, 4 Tetris games under X Window

TETRIS GAMES
  I guess I could not control myself.  I made 4 versions of the game after
Q. Alex Zhao's tetris.3.1.2.tar.Z found at ftp.x.org in /contrib/games .
Every effort will be made to keep these current with his latest version.

altetris:	Just like Zhao's version except its altered. One has access to
		more polyominoes including diagonal polyominoes.
alwelltris:	Welltris version of above. Slide the polyominoes down the
		3-D well.
altertris:	Triangular version of altetris. (altritris did not look right)
		Polyiamonds bounce off walls.
alhextris:	You guessed it ... a hexagon version of altetris. Polyhexes
		bounce off walls. (Built independent of xhextris).


Problems (I am open to ideas and fixes):
  All above games require X and UNIX.
  I guess the last 3 need some improvement in the graphics.
  I tried to port them to VMS, but gave up.  (*)
  I looked into a alboxtris, maybe I will try again if I have the time.
  my mail address is: bagleyd@perry.njit.edu
  original site: ftp.x.org//contrib/games/altris

(*) Now it is :-)


ALTRIS V 1.2.0 port under OpenVMS                 January 1995
=================================

You need to define ALTETRIS_DIR logical name pointing on the directory xhere
are POLY*.dat files (either this directory or another where you copy the 3
files).

Exe and obj provided:
--------------------

VAX: VMS 5.5-2,VAX C 3.2, Motif 1.1
Alpha: VMS 6.1, DEC C 5.0, Motif 1.2 (exe)

Patrick Moreau - CENA/Athis-Mons - FRANCE

pmoreau@cena.dgac.fr
moreau_p@decus.decus.fr

