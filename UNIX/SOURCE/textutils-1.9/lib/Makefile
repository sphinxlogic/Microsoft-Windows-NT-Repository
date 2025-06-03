# Generated automatically from Makefile.in by configure.
# Makefile for library files used by GNU text utilities.
# Copyright (C) 1991, 1992, 1993 Free Software Foundation, Inc.

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

CC = gcc
AR = ar
RANLIB = ranlib
DEFS = -DCONFIG_BROKETS -DHAVE_CONFIG_H
CFLAGS = -O3 -g
YACC = @YACC@

SOURCES = regex.c error.c getopt.c getopt1.c linebuffer.c xmalloc.c \
xwrite.c bcopy.c memchr.c memcmp.c memset.c stpcpy.c \
strtol.c strtoul.c alloca.c

# Always link in the fast GNU memchr; it speeds up sort, especially on HP-UX.
OBJECTS = regex.o error.o getopt.o getopt1.o linebuffer.o xmalloc.o \
xwrite.o memchr.o  bcopy.o stpcpy.o 

DISTFILES = Makefile.in getopt.h linebuffer.h regex.h $(SOURCES)

all: libtu.a

.c.o:
	$(CC) -c $(CPPFLAGS) $(DEFS) -I.. -I$(srcdir) $(CFLAGS) $<

install: all

uninstall:

TAGS: $(SOURCES)
	etags $(SOURCES)

check:

clean:
	rm -f *.a *.o

mostlyclean: clean

distclean: clean
	rm -f Makefile

realclean: distclean
	rm -f TAGS

dist:
	for file in $(DISTFILES); do \
	  ln $$file ../`cat ../.fname`/lib \
	    || cp -p $$file ../`cat ../.fname`/lib; \
	done

libtu.a: $(OBJECTS)
	rm -f $@
	$(AR) cr $@ $(OBJECTS)
	-$(RANLIB) $@

getopt1.o: getopt.h
regex.o: regex.h

$(OBJECTS): ../config.h

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
