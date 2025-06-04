XATAXX, GAMES, X11 strategy game

XAtaxx is a strategy game based on Ataxx, a popular arcade video
game.  It is a game of position and movement.  The object is to have
the most pieces of your color on the board at the end of the game.

AUTHORS
-------
Code by:
Steven Gifford <sg10+@andrew.cmu.edu>
Jim Zelenka <jz1j+@andrew.cmu.edu>
Andrew Plotkin <ap1i+@andrew.cmu.edu>

Bitmaps by:

Benjamin Peal <bp14+@andrew.cmu.edu>
Elliott C. Evans <ee0r+@andrew.cmu.edu>


XATAXX port under OpenVMS                           December 1977
=========================

Xataxx game for X window is now available for OpenVms VAX & Alpha

Sources, object files and executables are provided (for VAX & Alpha).

Installation:
------------

Xataxx need to read his bitmap files at run time. They are pointed by
XATAXX_DIR logical name. SETUP_VMS.COM procedure gives you an exemple of
definition (you can invoke the procedure to play with this distribution).

For a regular installation, you need to create a directory and point it via
XATAXX_DIR logical name, and copy bitmap files:

$ define XATAXX_DIR disk:[toto.xataxx]
$ copy [.lib]*.* xataxx_dir:* /lo/prot=(w:re)

Rebuild:
-------

You need DEC C 5.2 on VAX or on Alpha. You can recompile with COMPILE_DECC.COM
on VAX and COMPILE_AXP.COM on Alpha. You relink with LINK_DECC.COM for VAX and
LINK_AXP.COM for Alpha

Enjoy !!

Patrick Moreau
pmoreau@cena.dgac.fr
moreau_p@decus.fr
http://www2.cenaath.cena.dgac.fr/~pmoreau/
