# Generated automatically from Makefile.in by configure.
# Makefile for library files used by GNU find, xargs, and locate.
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

SOURCES = dirname.c error.c filemode.c getopt.c getopt1.c idcache.c \
listfile.c modechange.c nextelem.c regex.c savedir.c xmalloc.c \
xstrdup.c fnmatch.c fileblocks.c memset.c stpcpy.c strdup.c strftime.c \
strspn.c strstr.c strtol.c alloca.c

OBJECTS = dirname.o error.o filemode.o getopt.o getopt1.o idcache.o \
listfile.o modechange.o nextelem.o regex.o savedir.o xmalloc.o \
xstrdup.o fnmatch.o  stpcpy.o 

DISTFILES = Makefile.in \
fnmatch.h getopt.h modechange.h modetype.h pathmax.h regex.h wait.h $(SOURCES)

all: libfind.a

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(DEFS) -I$(srcdir) $<

install: all

uninstall:

TAGS: $(SOURCES)
	etags $(SOURCES)

clean:
	rm -f *.a *.o

mostlyclean: clean

distclean: clean
	rm -f Makefile

realclean: distclean
	rm -f TAGS

dist:
	-ln $(DISTFILES) ../`cat ../.fname`/lib
	test -f ../`cat ../.fname`/lib/fnmatch.c || cp -p fnmatch.[ch] ../`cat ../.fname`/lib

libfind.a: $(OBJECTS)
	rm -f $@
	$(AR) cr $@ $(OBJECTS)
	-$(RANLIB) $@

fnmatch.o: fnmatch.h
getopt1.o: getopt.h
modechange.o: modechange.h
regex.o: regex.h

# Prevent GNU make v3 from overflowing arg limit on SysV.
.NOEXPORT:
