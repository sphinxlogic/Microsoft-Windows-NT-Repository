SNAKE, GAMES, X11 Fruit-eating snake game

snake4 - Fruit-eating snake game.
=================================

This program is my fourth implementation of the snake game, written
for Unix and X11. Previous versions were written for MS-DOS.

Move the snake around the screen and eat food. The snake grows in
length as you eat. Avoid hitting the fence, eating lethal mushrooms
and rotten food, and biting your own tail.

Now and then a slim-pill will bounce around. If you eat this, you will
get shorter, and you will gain bonus points for every length you
loose. The bonus is also given when a round is over.

When food starts blinking, it's about to rot. Rotten food is
poisonous, and thus uneatable. As long as it blinks, it can still be
eaten.

Oh, yes - I almost forgot: You may be visited by the evil headbanger.
Take my advice and run away from him before he hits you in the head!
He won't stay long.

Author
------

The program is written by

        Sverre H. Huseby
        Kurvn. 30
        N-0495 Oslo
        Norway

        sverrehu@ifi.uio.no
        http://www.ifi.uio.no/~sverrehu/

SNAKE4 V 1.0.9 port under OpenVMS                       January 1998
=================================

A small arcade game.

Installation:
------------

You need to copy SNAKE4.SCORE file under the directory pointed by SNAKE_DIR
logical name. You must give write permission to the file:

$ define snake_dir disk:[toto.tutu]
$ copy snake4.score snake_dir:* /lo/prot=(w:rwe)

Executables provided were built with DEC C 5.0 on VAX VMS 6.1 and DEC C 5.2 on
Alpha VMS 6.2

Rebuild:
-------
Recompile with COMPILE_DECC.COM procedure, and relink with LINK_AXP.COM or
LINK_DECC.COM ; you need DECUs X11R4 XAW, XMU and XVMSUTILS libraries to
ecompile and link. You can find them at my site:

 http://www2.cenaath.cena.dgac.fr/ftp/decwindows/

Enjoy !!

Patrick Moreau
pmoreau@cena.dgac.fr
moreau_p@decus.fr
http://www2.cenaath.cena.dgac.fr/~pmoreau/
