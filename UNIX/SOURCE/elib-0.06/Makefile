# $Id: dist-makefile,v 0.8 1993/01/24 19:29:02 ceder Exp $
# Makefile for the GNU Emacs lisp library, Elib
# Copyright (C) 1991, 1992 Free Software Foundation
#
# This file is part of the GNU Emacs lisp library, Elib.
#
# GNU Elib is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 1, or (at your option)
# any later version.
#
# GNU Elib is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Emacs; see the file COPYING.  If not, write to
# the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
#

# ================================================================
# Change the following to reflect the situation at your site:

PREFIX=/opt
LISPDIR = $(PREFIX)/lib/emacs/elisp

# ================================================================
# You shouldn't change anything below this line.
#


ELFILES = stack-f.el stack-m.el queue-f.el queue-m.el \
elib-node.el bintree.el avltree.el string.el read.el cookie.el dll.el \
dll-debug.el
ELCFILES = stack-f.elc stack-m.elc queue-f.elc queue-m.elc\
elib-node.elc bintree.elc avltree.elc string.elc read.elc cookie.elc dll.elc \
dll-debug.elc

ELIBDIR = $(LISPDIR)/elib


elcfiles:
	cd library;emacs -batch -l ../elib-compile-all.el -f compile-elib

install: install_elc elib.dvi elib.info
	cd library; cp $(ELFILES) $(ELIBDIR)
	cp elib.info $(PREFIX)/info

install_elc: elcfiles install_dir
	cd library; cp $(ELCFILES) $(ELIBDIR)
	sed s,ELIB_PATH,\"$(ELIBDIR)\", < startup-template.el > elib-startup.el

install_dir:
	-mkdir $(ELIBDIR)

# Documentation targets added by Greg Lehey, LEMIS, 5 February 1994
elib.dvi:	elib.texinfo
	tex	elib.texinfo
	tex	elib.texinfo

elib.info:	elib.texinfo
	makeinfo elib.texinfo
clean:
	rm -f *~ elib-startup.el core $(ELCFILES)
