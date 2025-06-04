BATTALION-14, GAMES, 3D OpenGL arcade game

battalion - Monsters, explosions, destruction.
            You've seen the movies, you know what to do

            Version 1.3 August 1996

battalion was written by Andrew Johnson
                         Electronic Visualization Laboratory (M.C 154)
                         University of Illinois at Chicago
                         851 South Morgan St. Room 1120 SEO
                         Chicago, IL 60607

                         ajohnson@eecs.uic.edu
                         http://evlweb.eecs.uic.edu/aej
                         http://www.eecs.uic.edu/~ajohnson

battalion is Copyright (c) 1994-1996 by Andrew Johnson - All Rights Reserved

BATTALION V 1.4 port under OpenVMS                  January 1998
==================================

Battalion is a 3D combat game using OpenGL or MESA GL.

Installation:
------------

SETUP_VMS.COM procedure defines all necessary logical names and symbols to run
battalion on this distribution.

Symbols:

$ battalion :== $path:battalion            ! where path is the location of
$!                                           battalion executable
$ battaliondatadir :== "BATTALION_DIR:"

Logical names:

$ define/trans=conc BATTALION_DIR disk:[toto.battalion.]

Installation of data files:

$ cre/dir  disk:[toto.battalion]
$ backup/log [.BATTALION_DATA...]*.*;* battalion_dir:[000000...]*
$ set prot=(w:re)  battalion_dir:[000000...]*.*

Binaries provied where built under OpenVMS VAX 6.1 with DECW/Motif 1.2, Mesa
2.2 and DEC C 5.0 , and under OpenVMS Alpha 6.2 with DECW/Motif 1.2-4, Mesa 2.2
and DEC C 5.2

Rebuild:
------
On VAX, you need MESA 2.2 library to rebuild with COMPILE_VAX_MESA.COM and
and DEC C 5.2

Rebuild:
------
On VAX, you need MESA 2.2 library to rebuild with COMPILE_VAX_MESA.COM and
LINK_VAX_MESA.COM procedures. You need to copy LIBMESAGL.OLB_VAX and
LIBMESAGLU.OLB_VAX object libraries under [.SOURCE] directory or alter
option files.

On Alpha, you can rebuild with Open3D (however my version of Open3D crashes
when running battalion) with COMPILE_AXP_OPEN3D.COM and LINK_AXP_OPEN3D.COM
procedures, or use MESA 2.2 library with COMPILE_AXP_MESA.COM and
LINK_AXP_MESA.COM procedures. You need to copy LIBMESAGL.OLB_AXP and
LIBMESAGLU.OLB_AXP object libraries under [.SOURCE] directory or alter
option files.

When using MESA, you need to alter compile procedures to define GL logical name
pointing Mesa include files (however, on Alpha you can point to SYS$LIBRARY if
Open3D is installed).

Mesa:
----

You can find Mesa 2.2 compiled for VAX VMS and Alpha VMS at my site:

  http://www2.cenaath.cena.dgac.fr/ftp/decwindows/lib/mesa-22.zip

Enjoy !!

Patrick Moreau
pmoreau@cena.dgac.fr
moreau_p@decus.fr
http://www2.cenaath.cena.dgac.fr/~pmoreau/
