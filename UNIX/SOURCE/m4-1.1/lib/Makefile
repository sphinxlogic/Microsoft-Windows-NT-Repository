# Generated automatically from Makefile.in by configure.
# Makefile for GNU m4 library.
# Copyright (C) 1992, 1993 Free Software Foundation, Inc.

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

DEFS =  -DSTDC_HEADERS=1 -DHAVE_STRING_H=1 -DHAVE_TMPFILE=1 -DHAVE_VPRINTF=1 -DHAVE_EFGCVT=2
CFLAGS = -g
LIBOBJS = 

CPPFLAGS = -I$(srcdir) $(DEFS)

.c.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $<

HEADERS = getopt.h obstack.h regex.h
SOURCES = getopt.c getopt1.c error.c obstack.c regex.c xmalloc.c xstrdup.c \
_doprnt.c vfprintf.c alloca.c
OBJECTS = getopt.o getopt1.o error.o obstack.o regex.o xmalloc.o xstrdup.o \
$(LIBOBJS) $(ALLOCA)

DISTFILES = COPYING.LIB Makefile.in $(HEADERS) $(SOURCES)

all: libm4.a

libm4.a: $(OBJECTS)
	rm -f libm4.a
	$(AR) cru libm4.a $(OBJECTS)
	$(RANLIB) libm4.a

#$(OBJECTS): ../config.h

install:

uninstall:

check:

tags: TAGS

TAGS: $(SOURCES) $(HEADERS)
	etags $(SOURCES) $(HEADERS)

clean:
	rm -f libm4.a *.o

distclean: clean
	rm -f tags TAGS Makefile

realclean: distclean

dist: $(DISTFILES)
	mkdir ../`cat ../.fname`/lib
	chmod 777 ../`cat ../.fname`/lib
	ln $(DISTFILES) ../`cat ../.fname`/lib

Makefile: Makefile.in
	cd ..; ./config.status

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
