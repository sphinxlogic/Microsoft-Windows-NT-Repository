# Generated automatically from Makefile.in by configure.
# Makefile for GNU find.
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

SOURCES = find.c fstype.c parser.c pred.c tree.c util.c version.c
OBJECTS = find.o fstype.o parser.o pred.o tree.o util.o version.o
DISTFILES = Makefile.in defs.h $(SOURCES)

all:	find

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(DEFS) -I$(srcdir)/../lib $<

install: all
	$(INSTALL_PROGRAM) find $(bindir)/$(binprefix)find

uninstall:
	rm -f $(bindir)/$(binprefix)find

TAGS: $(SOURCES)
	etags $(SOURCES)

clean:
	rm -f find *.o core

mostlyclean: clean

distclean: clean
	rm -f Makefile

realclean: distclean
	rm -f TAGS

dist:
	ln $(DISTFILES) ../`cat ../.fname`/find

find:	$(OBJECTS) ../lib/libfind.a
	$(CC) -o $@ $(LDFLAGS) $(OBJECTS) ../lib/libfind.a $(LIBS)

parser.o: ../lib/modechange.h
find.o fstype.o parser.o pred.o: ../lib/modetype.h
find.o parser.o pred.o tree.o util.o: defs.h 
pred.o: ../lib/wait.h

# Prevent GNU make v3 from overflowing arg limit on SysV.
.NOEXPORT:
