3DPONG, GAMES, X11 3D Game based on "Pong"

3D Pong v.0.2

by Bill Kendrick
kendrick@zippy.sonoma.edu

New Breed Software
http://zippy.sonoma.edu/kendrick/nbs/

December, 1997


WHAT IS 3D PONG?

  3D Pong is an X-Window game for 1 or 2 players.  It's a 3D game based
  on the first arcade game, "Pong."  Like "Pong," it's a game played with
  a ball and paddles.  The object is to hit the ball and get it in the
  other player's goal.  Use your paddle to deflect the ball and keep it
  from getting in your goal.

  3D Pong has three game modes:

    1. Two player - requires two X servers
    2. One player - against a computer player
    3. Handball - one player against gravity

3DPONG V 0.2 port under OpenVMS:                    December 1997
===============================

A small Pong arcade game under X Window.

Installation:
------------

You just need to define a DCL foreing symbol to run 3Dpong with one of supplied
executables:

$ 3dpong :==$disk:[directory]3dpong    ! Alpha version
$ 3dpong :==$disk:[directory]3dpong.exe_vax    ! VAX version

These exes where built with DEC C 5.0 on VAX VMS 6.1 and Alpha VMS 6.2

OBjs files are provided into ZIP archives AXPOBJS.ZIP & VAXOBJS.ZIP

Using:
-----

You need to specify at least one display name (generally your's, DECW$DISPLAY):

$ 3dpong decw$display


Enjoy !!

Patrick Moreau
pmoreau@cena.dgac.fr
moreau_p@decus.fr
http://www2.cenaath.cena.dgac.fr/~pmoreau/

