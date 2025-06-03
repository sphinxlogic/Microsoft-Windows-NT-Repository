Spatial: three-dimensional tetris.

(C) Copyright 1991-3 Andrew Plotkin. (ap1i+@andrew.cmu.edu) 
Permission is given to copy and use, as long as this copyright notice 
is retained.

To compile: change the first few lines of the Makefile to reflect
where you want the game installed.

The piece shape file *must* be
installed in the DESTLIBDIR you define in the Makefile.
Type "make install" to do this.
(The score file, which in earlier versions had to be installed,
is now generated automatically if it does not exist.)

------------------

Some comments about porting to various systems: 
(these are gathered from play-testing comments, and
have not been tested)

If your system does not have random(), uncomment the line
#DEFINES = -Drandom=lrand48 -Dsrandom=srand48

If your system does not have flock(), add -DUSELOCKF
to the DEFINES line.

Solaris: Uncomment the line
#DEFINES = -DUSELOCKF -Drandom=lrand48 -Dsrandom=srand48
in the Makefile.

AIX on RS/6000: Add the line
#include <sys/select.h> 
in game.c and intro.c.

You may have to add
#include <sys/types.h>
on score.c.

