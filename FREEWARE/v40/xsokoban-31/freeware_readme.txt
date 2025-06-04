XSOKOBAN-31, GAMES, X1 version of Sokoban game

XSokoban 3.1
------------

This is the third release of XSokoban.

Credits -

    Unknown Hacker - Original curses(1) implementation.
    Joseph L. Traub - X windows interface.
    Kevin Solie - simultaneously developed X implementation (merged in).
    Mark Linderman, Christos Zoulas - Mouse code (added to Solie's version).
    Andrew Myers - Improved mouse UI, score ranking and color support

--Andrew Myers
  (andru@lcs.mit.edu)

VMS Port of Xsokoban 3.1                                      January 1997
========================

Jérôme Lauret has sent to me some mods to build Xsokoban with DEC C on VAX and
Alpha. There is also a multi-user score file and the support of super-user for
score file creation .

Installation:
------------

You need to define some logical names:

- XSOKOBAN_DIR : directory where screens and bitmaps files are

  $ create/dir  disk:[toto.xsokoban]
  $ DEF/SYS XSOKOBAN_DIR disk:[toto.xsokoban]
  $ set def [.screens]
  $ copy *.* xsokoban_dir:* /lo/prot=(w:re)
  $ set def [-.bitmaps.defaults]
  $ copy *.* xsokoban_dir:* /lo/prot=(w:re)
  $ set def [--]

- SOKOBAN_SCORES: where is located the score file

  $ create/dir disk:[toto.xsokoban.scores]
  $ def/sys SOKOBAN_SCORES disk:[toto.xsokoban.scores]
  $ set prot=(w:rwe) XSOKOBAN_DIR:scores.dir

You need to create the score file (your name must match the superuser name
found into CONFIG.H). A blank score file is provided into [.SCORES] directory
if you cannot.

Regular creation (the password is into CONFIG.H file)

$ xsokoban -c
password:

Rebuild:
-------

$ @make builds xsokoban with DEC C on Alpha and GCC on VAX. You can build
with DEC C on VAX with:

$ @make DECC

I've removed the need of Xvmsutils library providing VMS sources for
STRDUP and UNLINK functions.

Exes:
----
Binaries were built on VAX VMS 6.1 with DEC C 5.0 Motif 1.2 and on Alpha VMS
6.2 with DEC C 5.2 Motif 1.2-4

Obj files are in the two .zip files

Enjoy !!

Patrick Moreau
<pmoreau@cena.dgac.fr>
<moreau_p@decus.fr>
http://www2.cenaath.cena.dgac.fr/~pmoreau/
