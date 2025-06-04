XBOING-24, GAMES, X11 Blockout type game


              .o8                  o8o                            ooooo ooooo
             "888                  `"'                            `888' `888'
 oooo    ooo  888oooo.   .ooooo.  oooo  ooo. .oo.    .oooooooo     888   888
  `88b..8P'   d88' `88b d88' `88b `888  `888P"Y88b  888' `88b      888   888
    Y888'     888   888 888   888  888   888   888  888   888      888   888
  .o8"'88b    888   888 888   888  888   888   888  `88bod8P'      888   888
 o88'   888o  `Y8bod8P' `Y8bod8P' o888o o888o o888o `8oooooo.     o888o o888o
                                                    d"     YD
                                                    "Y88888P'

                       Version 2.4 - 22nd Nov 1996


Description
-----------

XBoing is a blockout type game where you have a paddle which you use to
bounce a ball around the game area blowing up blocks with the ball. You win by
obtaining points for each block destroyed and each level completed. The person
with the highest score wins.

Justin Kibell
jck@catt.rmit.edu.au
SnailMail: PO BOX 260, Eltham, Victoria, Australia, 3095

XBOING V 2.4 port under OpenVMS    Dec 1996
===============================

Xboing 2.4 can be built with DEC C on Alpha (etsted with DEC C 5.2 & DEC C 5.2)
and DEC C or GCC on VAX  (DEC C 5.0 or DEC C 5.2, GCC 2.5.8)

You need to define XBOING24_DIR to point a directory where you'll put level
files and high score file:

$ create/dir disk:[directory]
$ set prot=(w:rwe) disk:[directory]
$ define/SYSTEM XBOING24_DIR  disk:[directory]
$ COPY HIGH.SCORE XBOING_24_DIR:* /lo/prot=(w:rwe)
$ set def [.levels]
$ copy *.* XBOING24_DIR:* /lo/prot=(w:re)

Nota: High score is now working !!

OBJ and binaries files are provided (DEC C 5.0, VMS 6.1-1H2 for Alpha; DEC C
5.0 VMS 6.1 and  GCC 2.5.8 VMS 6.1 fro VAX).

If you want to rebuild:

Compilation:
===========
COMPILE_DECC.COM          : compile with DEC C
COMPILE_DECC_AUDIO.COM    : compile with DEC C (with Amd audio support)
COMPILE_GCC.COM           : compile with GCC
COMPILE_GCC_AUDIO.COM     : compile with GCC (with Amd audio support)

Nota: Amd (8 bits 8 Khz) audio support is not really functionnal. I'll try to
implement sound with MMOV (MultiMedia services for OpenVms).

Link:
====

LINK_AUDIO_DECC.COM       : for VAX with DEC C
LINK_AUDIO_GCC.COM        : for VAX with GCC
LINK_AXP.COM              : for AXP with DEC C
LINK_DECC.COM             : for VAX with DEC C
LINK_GCC.COM;             : for VAX with GCC

Patrick Moreau - CENA/Athis_Mons - FRANCE
pmoreau@cena.dgac.fr
moreau_p@decus.fr
