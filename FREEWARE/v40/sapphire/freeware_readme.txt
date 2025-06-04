SAPPHIRE, SOFTWARE, An Acoustic Compiler

Sapphire version 1 port under OpenVMS                           November 1998
=====================================

Sapphire is an acoustic compiler ...

Binaries provided:
=================

VAX VMS 6.1, built with DEC C 5.0
Alpha VMS 6.2, built with DEC C 5.2

Rebuild:
=======

$ set def [.SRC]
$ @COMPILE_DECC
$ @LINK_DECC

Enjoy !!

Patrick Moreau
pmoreau@cena.dgac.fr
moreau_p@decus.fr
http://www2.cenaath.cena.dgac.fr/~pmoreau/

README file:

Sapphire - an acoustic compiler for Unix and DOS

Sapphire is an acoustic compiler: a program which takes a
representation of sound as a text file and generates the sound from
it, rather like a ray-tracing package takes a text representation of a
scene and generates a picture from it.

Sapphire has a large range of objects from which your sound can be
built, including envelope shapers, filters, oscillators, sample
playback modules, and arithmetic operators. These can be 'wired'
together into sounds of arbitrary complexity. Complex sounds just take
a bit longer to build.

All sounds created with sapphire can be used commercially without
royalty, provided you put a notice saying something like:

	"Sapphire acoustic compiler by J. Finnis"

Sapphire itself is distributed under the terms of the GNU General
Public License - see COPYING for details.

This Linux archive should contain:

guide.*         :   user's guide in nroff -me, postscript and ASCII
src             :   source code
insts           :   some sample instruments
                    In the beta, these are pretty dire!
scales          :   some sample scales
samples         :   some sample samples/waves - just sine and saw
sapphire        :   Linux executable
README          :   this file
COPYING         :   the GNU general public license
DOSNOTES        :   notes on the DOS version
DISTRIB         :   distribution history
sapphire-mode.el:   a simple Lucid Emacs (Xemacs) 19.10 mode for sapphire,
					mainly for font-lock settings
mid2sapp       :   a tclmidi script for converting MIDI files to sapphire scores

