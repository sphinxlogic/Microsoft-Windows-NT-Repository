# Generated automatically from Makefile.in by configure.
# Makefile for GNU fileutils documentation.
# Copyright (C) 1990, 1991, 1992, 1993 Free Software Foundation, Inc.

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

prefix = /opt
exec_prefix = ${prefix}
mandir = $(prefix)/man/man1
manprefix =
manext = 1
INSTALL = @INSTALL@
INSTALL_DATA = @INSTALL_DATA@

MANFILES = chgrp.1 chown.1 chmod.1 cp.1 dd.1 df.1 du.1 install.1 \
ln.1 ls.1 mkdir.1 mkfifo.1 mknod.1 mv.1 rm.1 rmdir.1 touch.1 

DISTFILES = Makefile.in $(MANFILES)

all:

install: all $(MANFILES)
	for f in $(MANFILES); do \
	  ../src/ginstall -m 644 $(srcdir)/$$f \
	  $(mandir)/$(manprefix)`basename $$f 1`$(manext); \
	done

uninstall:
	for f in $(MANFILES); do rm -f \
	  $(mandir)/$(manprefix)`basename $$f 1`$(manext); \
	done

TAGS:

check:

clean:

mostlyclean: clean

distclean: clean
	rm -f Makefile

realclean: distclean
	rm -f TAGS

dist:
	ln $(DISTFILES) ../`cat ../.fname`/man

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
