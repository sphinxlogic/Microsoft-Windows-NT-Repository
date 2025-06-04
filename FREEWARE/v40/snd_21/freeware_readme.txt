SND_21, SOFTWARE, Sound Editor and Mixer 

                                Snd

                 Bill Schottstaedt (bil@ccrma.stanford.edu)

  ------------------------------------------------------------------------

Snd is a sound editor modelled loosely after Emacs and an old, sorely-missed
PDP-10 sound editor named Dpysnd. It can accomodate any number of sounds at
once, each with any number of channels. Each channel is normally displayed
in its own window, with its own cursor, edit history, and marks; each sound
has a 'control panel' to try out various changes quickly, and an expression
parser, used mainly during searches; there is an overall stack of 'regions'
that can be browsed and edited; channels and sounds can be grouped together
during editing; edits can be undone and redone without restriction
('unlimited undo'); Snd can be customized and extended using a built-in
Scheme interpreter (Gnu Guile); and it's free; the code is available via
anonymous ftp from ccrma-ftp.stanford.edu as pub/Lisp/snd.tar.gz.

  ------------------------------------------------------------------------

SND version 2.1 port under OpenVMS                   October 1998
==================================

SND is a Motif based sound file editor/mixer. Supports a lot of sound file
formats.

Binaries:
========

Object files and executables are provided. They were built with DEC C 5.0 under
OpenVMS VAX 6.1 and DEC C 5.2 OpenVMS Alpha 6.2

Rebuild:
=======

You can rebuild with COMPILE_DECC.COM prodedure and either LINK_VAX.COM or
LINK_AXP.COM procedures.


Bugs/Limitations:
================

Actually, Record or Play functions are not available into the VMS port.

Patrick moreau
pmoreau@cena.dgac.fr
moreau_p@decus.fr
http://www2.cenaath.cena.dgac.fr/~pmoreau/

