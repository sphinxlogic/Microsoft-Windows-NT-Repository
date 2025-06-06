# Generated automatically from Makefile.in by configure.
# Makefile for library files used by GNU fileutils.
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

CC = gcc
AR = ar
RANLIB = ranlib
DEFS = -DCONFIG_BROKETS -DHAVE_CONFIG_H -DSYSV
CFLAGS = -g -O3
YACC = bison -y

prefix = /opt
exec_prefix = ${prefix}
libdir = $(exec_prefix)/lib

SOURCES = getdate.y posixtm.y \
argmatch.c backupfile.c basename.c dirname.c eaccess.c \
error.c filemode.c fsusage.c getopt.c getopt1.c \
getversion.c idcache.c isdir.c makepath.c \
modechange.c mountlist.c savedir.c \
stripslash.c xgetcwd.c xmalloc.c xstrdup.c userspec.c yesno.c \
fileblocks.c fnmatch.c ftruncate.c mkdir.c mktime.c rename.c stpcpy.c \
strdup.c strstr.c alloca.c

OBJECTS = getdate.o posixtm.o \
argmatch.o backupfile.o basename.o dirname.o eaccess.o \
error.o filemode.o getopt.o getopt1.o \
getversion.o idcache.o isdir.o makepath.o \
modechange.o savedir.o \
stripslash.o xgetcwd.o xmalloc.o xstrdup.o userspec.o yesno.o \
 fsusage.o mountlist.o fnmatch.o mkdir.o mktime.o stpcpy.o strdup.o strstr.o rename.o fileblocks.o alloca.o

DISTFILES = Makefile.in backupfile.h getopt.h modechange.h \
fnmatch.h fsusage.h mountlist.h pathmax.h system.h $(SOURCES)

all: libfu.a

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

dist: $(DISTFILES)
	for file in $(DISTFILES); do \
	  ln $$file ../`cat ../.fname`/lib \
	    || cp -p $$file ../`cat ../.fname`/lib; \
	done

libfu.a: $(OBJECTS)
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

rename.o: rename.c
	$(CC) -c $(CPPFLAGS) -DMVDIR="\"$(libdir)/mvdir\"" $(DEFS) \
	    -I.. -I$(srcdir) $(CFLAGS) $(srcdir)/rename.c

$(OBJECTS): ../config.h

backupfile.o getversion.o: backupfile.h
fnmatch.o: fnmatch.h
fsusage.o: fsusage.h
getopt1.o: getopt.h
modechange.o: modechange.h
mountlist.o: mountlist.h
xgetcwd.o: pathmax.h

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
