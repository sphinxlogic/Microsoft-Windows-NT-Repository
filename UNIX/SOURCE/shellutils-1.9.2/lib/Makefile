# Generated automatically from Makefile.in by configure.
# Makefile for library files used by GNU shell utilities.
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
CFLAGS = -g -O3
YACC = bison -y

SOURCES = regex.c getdate.y basename.c error.c stripslash.c xmalloc.c \
xstrdup.c xwrite.c putenv.c getopt.c getopt1.c getugroups.c \
posixtm.y strftime.c gethostname.c getusershell.c memcmp.c mktime.c stime.c \
strcspn.c strstr.c strtol.c strtoul.c strtod.c alloca.c

OBJECTS = regex.o getdate.o basename.o error.o stripslash.o xmalloc.o \
xstrdup.o xwrite.o putenv.o getopt.o getopt1.o getugroups.o \
posixtm.o strftime.o  gethostname.o getusershell.o 

DISTFILES = Makefile.in getopt.h regex.h $(SOURCES)

all: libshu.a

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
	rm -f Makefile *.tab.c getdate.c *posixtm.c

realclean: distclean
	rm -f TAGS

dist:
	for file in $(DISTFILES); do \
	  ln $$file ../`cat ../.fname`/lib \
	    || cp -p $$file ../`cat ../.fname`/lib; \
	done

libshu.a: $(OBJECTS)
	rm -f $@
	$(AR) cr $@ $(OBJECTS)
	-$(RANLIB) $@

# Since this directory contains two parsers, we have to be careful to avoid
# running two $(YACC)s during parallel makes.  See below.
getdate.c: getdate.y
	@echo expect 9 shift/reduce conflicts
	$(YACC) $(srcdir)/getdate.y
	mv y.tab.c getdate.c

# Make the rename atomic, in case sed is interrupted and later rerun.
# The artificial dependency on getdate.c keeps the two parsers from being
# built in parallel.  Enforcing this little bit of sequentiality lets
# everyone (even those without bison) still run mostly parallel builds.
posixtm.c: posixtm.y getdate.c
	$(YACC) $(srcdir)/posixtm.y
	mv y.tab.c posixtm.tab.c
	sed -e 's/yy/zz/g' posixtm.tab.c > tposixtm.c
	mv tposixtm.c posixtm.c
	rm -f posixtm.tab.c

getopt1.o: getopt.h
regex.o: regex.h

$(OBJECTS): ../config.h

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
