CARIBBEANSTUD, GAMES, X11/MOTIF based gambling game

Caribbean Stud is a X11/MOTIF based gambling game. This application has been
developed and tested only under Solaris 2.5.

Qun  Zhang
qzhang@oucsace.cs.ohiou.edu

CARIBBEAN STUD Poker port under OpenVMS                     December 1997
=======================================

A card game under X11

Installation:
------------

You need to  install ressource file CARIBBEANSTUD.DAT either under SYS$LOGIN
(personal install) or under SYS$COMMON:[DECW$DEFAULTS.USER] (system-wide
installation). You can alter some ressources before installation:

   Default bankroll              200
   Table minimum                 10
   Number of Players             1       (1-6)
   Current Jackpot               34000000
   Minimum Jackpot               3000000

VAX & Alpha executables are provided into the kit, as well as object files.

Rebuild:
-------

You need DEC C++ 5.2 or higher to rebuild. Compile with COMPILE_DECCXX.COM
procedure. You link with LINK_AXP.COM or LINK_VAX.COM procedures.

Porting notes:
-------------

DEC C++ 5.2 doesn't use C++ exceptions. It was necessary to modify the source
to run without exceptions. On VAX, a link problem (XmuFillRoundedRectangle
symbol was undefined !!) was resolved by using an alternate XMU library (an old
DECUS X11R4 Xmu library).

Enjoy !!

Patrick Moreau
pmoreau@cena.dgac.fr (or moreau_patrick@cena.dgac.fr)
moreau_p@decus.fr
http://www2.cenaath.cena.dgac.fr/~pmoreau/
