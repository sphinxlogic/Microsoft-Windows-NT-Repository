# Generated automatically from Makefile.in by configure.
# Makefile for GNU ispell.	-*- Indented-Text -*-
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

#### Start of system configuration section. ####

srcdir = .
VPATH = .

CC = gcc

INSTALL = /opt.real/bin/install -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644

DEFS =  -DUSG=1 -DRETSIGTYPE=void -DHAVE_MALLOC_H=1 -DHAVE_TERMIOS_H=1 -DHAVE_TERMIO_H=1 -DHAVE_STRING_H=1
LIBS =  -ltermlib -lc -lelf -L/usr/ucblib -lucb

CHARSET = c-ibmpc.c
ALLOCA = 

CFLAGS = -g -O3
LDFLAGS = -g

prefix = /opt
exec_prefix = ${prefix}

bindir = $(exec_prefix)/bin
datadir = $(prefix)/lib/ispell
libdir = $(prefix)/lib/emacs/site-lisp
infodir = $(prefix)/info

# Where to install the manual pages.
mandir = $(prefix)/man/man1
# Prefix for each installed man page, normally empty or `g'.
manprefix = 
# Extension (not including `.') for the installed manual page filenames.
manext = 1

#### End of system configuration section. ####

SHELL = /bin/sh

LOADLIBES = $(LIBS)

DISTNAME = ispell-$(VERSION)
VERSION = `sed -e '/version/!d;s/[^0-9.]*\([0-9.]*\).*/\1/;q' version.c`

# All subdirectories that go into a distribution
# There are none at the moment. 
ALL_SUBDIRS = 

# charset.c is automatically generated from c-generic.c or c-ibmpc.c
# during configuration.
SRCS = access.c c-generic.c c-ibmpc.c charset.h ctbl.c downcase.c exp.c \
          expand.c freq.c getopt.c getopt1.c getopt.h hash.c hash.h lexfsm.c \
          look.c mapcase.c nextprime.c suf.c version.c  util.c

OBJS = access.o charset.o ctbl.o downcase.o exp.o getopt.o getopt1.o hash.o \
          lexfsm.o nextprime.o suf.o version.o util.o $(ALLOCA)

SRCS_BUILD = build.c build.h mklextbl.c
OBJS_BUILD = build.o mklextbl.o

SRCS_LOOK = look.c getopt.c getopt1.c util.c
OBJS_LOOK = look.o getopt.o getopt1.o util.o $(ALLOCA)

SRCS_ISPELL = case.c gen.c good.c good.h ispell.c ispell.h priv.c screen.c term.c tex.c
OBJS_ISPELL = ispell.o case.o gen.o good.o priv.o screen.o term.o tex.o

SRCS_ALL = $(SRCS) $(SRCS_BUILD) $(SRCS_ISPELL)
OBJS_ALL = $(OBJS) $(OBJS_BUILD) $(OBJS_ISPELL)

DICT_LIB = $(datadir)

.c.o:
	$(CC) -c -I$(srcdir) $(DEFS) -DDICT_LIB=\"$(DICT_LIB)\" $(CPPFLAGS) $(CFLAGS) $<

all: build look ispell ispell.dict ispell.words ispell.info

### targets required by GNU Coding standards ###

Makefile: Makefile.in config.status
	sh ./config.status

config.status: configure
	$(srcdir)/configure --srcdir=$(srcdir) --no-create

configure: configure.in
	cd $(srcdir); autoconf

TAGS: $(SRCS) $(SRCS_BUILD) $(SRCS_ISPELL)
	cd $(srcdir); etags $(SRCS) $(SRCS_BUILD) $(SRCS_ISPELL)

clean:
	rm -f *.o *.elc ispell build freq core ispell.a freqtbl expand look

mostlyclean: clean
	rm -f ispell.dict ispell.words

distclean: clean
	rm -f Makefile config.status charset.c

realclean: distclean mostlyclean
	rm -f TAGS ispell.info configure

install: all ispell.info installdirs
	$(INSTALL_PROGRAM) ispell $(bindir)/ispell
	$(INSTALL_PROGRAM) look $(bindir)/look
	$(INSTALL_DATA) ispell.dict $(datadir)/ispell.dict
	$(INSTALL_DATA) ispell.words $(datadir)/ispell.words
	-$(INSTALL_DATA) $(srcdir)/ispell.1 $(mandir)/ispell.$(manext)
	-d=$(srcdir); test -f ./ispell.info && d=.; \
	cd $$d; for f in ispell.info* ; do $(INSTALL_DATA) $$f $(infodir)/$$f; done
	-$(INSTALL_DATA) $(srcdir)/ispell.el $(libdir)/ispell.el
	-$(INSTALL_DATA) ispell.elc $(libdir)/ispell.elc

installdirs:
	-sh $(srcdir)/mkinstalldirs $(bindir) $(datadir) $(libdir) $(infodir) $(mandir)

uninstall: force
	-cd $(bindir); rm -f ispell look
	-cd $(datadir); rm -f ispell.dict ispell.words 
	-cd $(infodir); rm -f ispell.info
	-cd $(mandir); rm -f ispell.$(manext)
	-cd $(libdir); rm -f ispell.el ispell.elc

dist: DISTFILES
	rm -rf $(DISTNAME)
	mkdir $(DISTNAME)
	for f in `cat DISTFILES`; do \
	   ln $(srcdir)/$$f $(DISTNAME)/$$f || { echo copying $$f; cp -p $(srcdir)/$$f $(DISTNAME)/$$f ; } \
	done
	tar --gzip -chvf $(DISTNAME).tar.z $(DISTNAME)
	rm -rf $(DISTNAME)

# Gets rid of most of the unwanted files.  Verify manually (if necessary)
# that this produces a list of all the files desired in the distribution. 
DISTFILES: force
	(cd $(srcdir); find . $(ALL_SUBDIRS) ! -type d -print) \
	| sed  '/\/RCS\//d; \
	       /\.tar.*/d; \
	       /\/#.*#/d; /~$$/d; /\.o$$/d; \
	       /\/config\.status$$/d; \
	       /\/Makefile$$/d;	\
	       /\/.*\.BAK$$/d; \
	       /\/core$$/d; \
	       /\/a.out$$/d; \
	       /\/=/d; \
	       /\/conftest\.c$$/d; \
	       /\/DISTFILES$$/d; \
	       /\/charset\.c/d; \
	       /\.toc$$/d; \
	       /\.aux$$/d; /\.log$$/d; \
	       /\.cps$$/d; /\.cp$$/d; \
	       /\.fns$$/d; /\.fn$$/d; \
	       /\.tps$$/d; /\.tp$$/d; \
	       /\.vrs$$/d; /\.vr$$/d; \
	       /\.pgs$$/d; /\.pg$$/d; \
	       /\.kys$$/d; /\.ky$$/d; \
	       s/^.\///; /^\.$$/d;' \
	| sort | uniq > DISTFILES

### ispell-specific building targets ###

$(OBJS_ALL): ispell.h

build: $(OBJS_BUILD) $(OBJS)
	$(CC) $(LDFLAGS) -o build $(OBJS_BUILD) $(OBJS) $(LOADLIBES)

expand: expand.o $(OBJS)
	$(CC) $(LDFLAGS) -o expand expand.o $(OBJS) $(LOADLIBES)

freq: freq.o $(OBJS)
	$(CC) $(LDFLAGS) -o freq freq.o $(OBJS) $(LOADLIBES)

freqtbl: freq dict
	./freq < $(srcdir)/dict > freqtbl

look: $(OBJS_LOOK)
	$(CC) $(LDFLAGS) -o look $(OBJS_LOOK) $(LOADLIBES)

ispell: $(OBJS_ISPELL) $(OBJS)
	$(CC) $(LDFLAGS) -o ispell $(OBJS_ISPELL) $(OBJS) $(LOADLIBES)

ispell.info: ispell.texi
	makeinfo --no-split -I$(srcdir) ispell.texi

ispell.dvi: ispell.texi
	texi2dvi $(srcdir)/ispell.texi

ispell.dict: build dict freqtbl
	./build -b -f freqtbl -o ispell.dict $(srcdir)/dict

ispell.words: dict expand
	./expand $(srcdir)/dict | sort | uniq > ispell.words

charset.c: $(CHARSET)
	cp $(srcdir)/$(CHARSET) charset.c

ispell.elc: ispell.el
	emacs -batch -l $(srcdir)/ispell.el -f batch-make-ispell


force:

# Prevent GNU make v3 from overflowing arg limit on SysV.
.NOEXPORT:
