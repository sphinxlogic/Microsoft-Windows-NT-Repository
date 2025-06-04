PUZZLES, GAMES, Collection od Puzzles under X Window

The collection includes:
SLIDING BLOCK PUZZLES
xcubes:		expanded 15 puzzle
xtriangles:	same complexity as 15 puzzle
xhexagons:	2 modes: one ridiculously easy, one harder than 15 puzzle
 
ROTATIONAL 3D PUZZLES
  hold down control key to move whole puzzle
  letters that represent colors can be changed in mono-mode

xrubik:		a nxnxn Erno Rubik's Cube(tm) (or Magic Cube)
		auto-solves 2x2x2 and 3x3x3 (non-orient mode)
xpyraminx:	a nxnxn Uwe Meffert's Pyraminx(tm) (and Senior Pyraminx),
                a tetrahedron with Period 2, Period 3, and Combined cut modes
		and it also has a sticky mode to simulate a Halpern's
		Tetrahedron or a Pyraminx Tetrahedron
xoct:		a nxnxn Uwe Meffert's Magic Octahedron (or Star Puzzler) and
		Trajber's Octahedron with Period 3, Period 4, and Combined
		cut modes and it also includes a sticky mode
xskewb:		a Meffert's Skewb (or Pyraminx Cube), a cube with diagonal
                cuts, each face is cut with a diamond shape
xdino:		a Triangle - 4 Cube - Dinosaur, (or Triangle - 4 Cube -
		6 Colors) a cube with different diagonal cuts, each face is
		cut with a "X" with Period 3 corner turning, Period 2 edge
		turning (Bosch's Cube), and Combined modes
xmball:		a variable cut Masterball(tm), variable number of latitudinal
                and longitudinal cuts on a sphere, where the longitudinal cuts
                permit only 180 degree turns

COMBINATION ROTATIONAL AND SLIDING 3D PUZZLES
  hold down shift key to move whole puzzle
  letters that represent colors can be changed in mono-mode
xmlink:		a nxm Erno Rubik's Missing Link(tm)

Future directions:
  Sorry about the lack of auto-solvers, but I would rather write the puzzle
    than the tedious solution.
  Currently the saved files are cryptic (not intentionally).
  Also xmlink and xmball need better algorithms for drawing sectors than just
    a series of arcs. If you know of any, tell me.

  The rest of the platonic solids (the dodecahedron and the icosahedron), as
    well as Square-1(tm), seem too hard for me.  (Square-1, by the way, was
    the hardest puzzle I ever solved).
  A combined xrubik 2x2x2 and xskewb (Longridge's Super Skewb) is possible as
    well but xskewb as it stands, seems hard enough (for me).
  A Billion Barrel would be nice but only with a auto-solver (the puzzle is
    too hard (I confess, I never solved it)).
  A Panex Puzzle with a Tower of Hanoi mode would be nice as well.

xrubik is currently the only one in this collection with a auto-solver.

X Window Puzzles port under OpenVMS:                 Nov 1995
===================================

Puzzle programs were already ported under OpenVMS, I have only made some
adjustments to the make procedures:

Each program has its own MAKE.COM procedure to compile, as previously, with VAX
C on VAX platform. I have added automatic DECW/Motif 1.2/1.1 support.

Each program has now a MAKE_DECC.COM procedure to compile with DEC C compiler
on VAX platform and a MAKE_AXP.COM procedure to compile under Alpha platform.

All references to DEC/TCPIP libraries (aka UCX) are removed

Each program has now an .HLP VMS help file.

Ressource files:
---------------

Each program has an PROGRAM.DAT ressource file. You can either copy it under
yout home directory (SYS$LOGIN) or under a system location for a system-wide
definition:

$ copy program.dat sys$common:[decw$defaults.user]* /lo/prot=(w:re)

Exe and obj provided
--------------------

VAX : VMS 5.5-2 , VAX C 3.2, Motif 1.1
Alpha: VMS 6.1-1H2, DEC C 5.0, Motif 1.2


Enjoy,

Patrick MOREAU - CENA/Athis-Mons - FRANCE
pmoreau@cena.dgac.fr
moreau_p@decus.decus.fr

