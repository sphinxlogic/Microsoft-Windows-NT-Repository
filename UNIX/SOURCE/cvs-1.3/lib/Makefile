# Generated automatically from Makefile.in by configure.
# Makefile for library files used by GNU CVS.
# Do not use this makefile directly, but only from `../Makefile'.
# Copyright (C) 1986, 1988-1992 Free Software Foundation, Inc.

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

# @(#)Makefile.in 1.12 92/03/31

SHELL = /bin/sh

srcdir = .


SOURCES = argmatch.c \
error.c getopt.c getopt1.c \
sighandle.c \
strippath.c stripslash.c yesno.c \
getdate.y \
hostname.c fnmatch.c ftruncate.c mkdir.c rename.c regex.c \
strdup.c getwd.c alloca.c

OBJECTS = argmatch.o \
error.o getopt.o getopt1.o \
sighandle.o \
strippath.o stripslash.o yesno.o \
getdate.o \
 fnmatch.o getwd.o hostname.o regex.o

DISTFILES = Makefile.in getopt.h \
fnmatch.h regex.h system.h wait.h $(SOURCES)

xxx:
	@cd ..; $(MAKE) all SUBDIRS=lib

all: libcvs.a
.PHONY: all

install: all
.PHONY: install

tags: $(DISTFILES)
	ctags $(DISTFILES)

TAGS: $(DISTFILES)
	etags $(DISTFILES)

ls:
	@echo $(DISTFILES)
.PHONY: ls

clean:
	rm -f *.a *.o *.tab.c getdate.c
.PHONY: clean

distclean: clean
	rm -f tags TAGS Makefile
.PHONY: distclean

realclean: distclean
.PHONY: realclean

dist:
	ln $(DISTFILES) ../`cat ../.fname`/lib
.PHONY: dist

libcvs.a: $(OBJECTS)
	$(AR) cr $@ $(OBJECTS)
	-$(RANLIB) $@

getdate.c: getdate.y
	@echo expect 8 shift/reduce conflicts
	$(YACC) $(srcdir)/getdate.y
	-if test -f y.tab.c ; then mv y.tab.c getdate.c ; fi
	-if test -f getdate.tab.c ; then mv getdate.tab.c getdate.c ; fi

fnmatch.o: fnmatch.h
getopt1.o: getopt.h
regex.o: regex.h
getwd.o: system.h
