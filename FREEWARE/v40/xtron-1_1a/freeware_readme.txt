XTRON-11, GAMES, X11 version of Tron game

  xtron v1.1

  Copyright (C) 1995 Rhett D. Jacobs <rhett@hotel.canberra.edu.au>

Xtron is a simple one or two player version of the old classic TRON. The game
is simple: avoid running into walls, your own tail, and that of your opponent.

The game starts in demo mode (two computer players).  To change to a human
player, simply click the human icon for either player 1/2 or both.  The
standard controls are as follows:

                     Player 1:          Player 2:
                       w - up             8 - up
                       s - down           2 - down
                       a - left           4 - left
                       d - right          6 - right

Tron 1.1a port under OpenVMS                          January 1998
============================

A one or two player version of the classical Tron arcade game.

Installation:
------------
The ressource file XTRONRC. may be placed under SYS$LOGIN.COM, under
SYS$COMMON:[DECW$DEFAULTS.USER] or directly in the directory where is the
executable. You need to alter this file to specify the Pixmap directory
(ressource xtron.PixmapsDIR). You can also alter the keys mapping.
By default, Xtronre. file is suited for running in the distribution directory.

Executables provided were built with DEC C 5.0 under VAX VMS 6.1 and DEC C 5.2
under VMS Alpha 6.2

Rebuild:
-------
You can recompile with COMPILE_DECC.COM procedure and link with LINK_AXP?COM or
LINK_VAX.COM procedures.

Enjoy !!

Patrick
pmoreau@cena.dgac.fr
moreau_p@decus.fr
http://www2.cenaath.cena.dgac.fr/~pmoreau/
