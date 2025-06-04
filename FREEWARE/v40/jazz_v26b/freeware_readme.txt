JAZZ, MISCELLANEOUS, MIDI Sequencer (Midi file editor).

jazz is a full size MIDI sequencer providing functions like record / play and
many edit functions as quantize, copy, transpose, graphical pitch editing, ...
multiple undo. It has two main windows oprating on whole tracks and single
events. There are special functions like GS sound editing and random rhythm
generation.

Authors:

Andreas Voss <andreas@avix.rhein-neckar.de>

     Per Sigmond <Per.Sigmond@hia.no>
 
JAZZ V2.6b MIDI Sequencer port under OpenVMS                 18 August 1997
============================================

Jazz is a Midi software sequencer using wxWindows graphic library. WxWindows is
happily available for OpenVMS VAX & Alpha. WXwindows 1.67 for VMS is available
on this CD and at urls:

 http://www2.cenaath.cena.dgac.fr/ftp/decwindows/lib/
 ftp://ftp2.cnam.fr/decwindows/lib/


One limitation : the wxhelp system is not actually available for OpenVMS.
Jazz is using wxhelp for implementation of contextual help, and therefore Jazz
help doesn't work on VMS. However, a formatted Jazz documentation is available
into [.DOC]JAZZ.DOC

For playing Midi files created or edited by Jazz, you can use Timidity, the VMS
version is available on this CD and at urls

 http://www2.cenaath.cena.dgac.fr/ftp/decwindows/
 ftp://ftp2.cnam.fr/decwindows/

Jazz is written in C++ and was built under OpenVMS using DEC C++ 5.0 or 5.2
compilers. I don't know if it can be built with gcc on VAX.

The kit contains all sources and documentation plus VAX & Alpha executables
and object files (the object files are stored into ZIP archives AXPOBJS.ZIP and
VAXOBJS.ZIP). If you want relink, you need WX object library file and if
you want to recompile you need WX source kit.

To rebuild on vax:

$ SET DEF [.SRC]
$ @COMPILE_VAX
$ @LINK_VAX

To rebuild on Alpha

$ SET DEF [.SRC]
$ @COMPILE_AXP
$ @LINK_AXP

Before running Jazz, you need to copy [.SRC].JAZZ to SYS$LOGIN:JAZZ.CFG
file.

Enjoy !!

Patrick Moreau
pmoreau@cena.dgac.fr
moreau_p@decus.fr
