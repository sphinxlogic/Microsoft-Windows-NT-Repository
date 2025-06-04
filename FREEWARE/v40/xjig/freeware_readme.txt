XJIG-24, GAMES, X11 Jigsaw Puzzle Maker

                                  _ _
                          _  __  (_|_)___ _
                         | |/_/ / / / __ `/
                        _>  <  / / / /_/ /
                       /_/|_|_/ /_/\__, /    V2.4
                           /___/  /____/
                       the jigsaw puzzle


 available at ftp.x.org and mirrors as:
                    /contrib/games/xjig-2.4.tgz


 Description:
      XJig is a puzzle, that tries to replicate a jigsaw puzzle on the
      screen as close as possible. Gif-images can be loaded and sliced
      into pieces and as in every jigsaw puzzle, the goal is to
      set the parts together again.

 Special effects:
      - Tiles are freely rotatable with intuitive pointer movement.
      - Tiles can even be flipped to their back (to mirror them).
      - Tiles can be shown as shaped windows on the desktop.

 comments to:
       __  __     __                __     __  __                 _
      / / / /__  / /___ ___  __  __/ /_   / / / /___  ___  ____  (_)___ _
     / /_/ / _ \/ / __ `__ \/ / / / __/  / /_/ / __ \/ _ \/ __ \/ / __ `/
    / __  /  __/ / / / / / / /_/ / /_   / __  / /_/ /  __/ / / / / /_/ /
 _ /_/ /_/\___/_/_/ /_/ /_/\__,_/\__/  /_/ /_/\____/\___/_/ /_/_/\__, /
/__________________________________e-mail: Helmut.Hoenig@hub.de______/

XJIG v 2.4 port under OpenVMS:                         31 December 1997
=============================

XJIG is a fabulous puzzle maker from GIF images. After making it, you can
solve the puzzle interactively !!

XJIG works either on VAX & Alpha. If you have a display with MIT SHAPE
extension, it is better (an Alpha or an X terminal), but Xjig also works on
displays without shapes.

Rebuild:
=======

You need at least DEC C++ 5.0 and DEC C 5.0 to rebuild from sources (not tested
with version 4 compilers). Use COMPILE_DECCXX.COM procedure to recompile and
either LINK_VAX.COM or LINK_AXP.COM to link.

Binaries and object files are provided (objects are in AXPOBJS.ZIP and
VAXOBJS.ZIP files). They are produced under VAX VMS 6.1 with DEC C++ 5.0 and
DEC C 5.0 and under Alpha VMS 6.2 with DEC C++ 5.2 and DEC C 5.2

Notes:
=====

Sometimes, you need to reduce the colour number of the GIF image (try with
mconvert from ImageMagick):

$ mconvert -colors 128 toto.gif toto.gif

Provided GIFs:
===============

TINA.GIF is provided by the author of Xjig. I've put another image,
VALLOIRE.GIF

Enjoy !!

Patrick Moreau
pmoreau@cena.dgac.fr
moreau_p@decus.fr
http://www2.cenaath.cena.dgac.fr
