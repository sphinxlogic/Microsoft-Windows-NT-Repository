# Generated automatically from Makefile.in by configure.
# Makefile for GNU find, xargs, and locate documentation.
# Do not use this makefile directly, but only from `../Makefile'.
# Copyright (C) 1990 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

SHELL = /bin/sh

srcdir = .
VPATH = .

MANFILES = find.1 locate.1 xargs.1
DISTFILES = Makefile.in $(MANFILES)

all:

install: all $(MANFILES)
	-cd $(srcdir); for f in $(MANFILES); do \
	$(INSTALL_DATA) $$f $(mandir)/$(manprefix)`basename $$f 1`$(manext); \
	done

uninstall:
	for f in $(MANFILES); do rm -f \
	$(mandir)/$(manprefix)`basename $$f 1`$(manext); \
	done

TAGS:

clean:

mostlyclean: clean

distclean: clean
	rm -f Makefile

realclean: distclean
	rm -f TAGS

dist:
	ln $(DISTFILES) ../`cat ../.fname`/man

# Prevent GNU make v3 from overflowing arg limit on SysV.
.NOEXPORT:
