# Generated automatically from Makefile.in by configure.
# Makefile for GNU text utilities.
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

prefix = /opt
exec_prefix = ${prefix}
bindir = $(exec_prefix)/bin
binprefix = 

INSTALL = /opt/bin/install -c
INSTALL_PROGRAM = $(INSTALL)
CC = gcc
DEFS = -DCONFIG_BROKETS -DHAVE_CONFIG_H
LIBS = 
CFLAGS = -O3 -g
LDFLAGS = -g

SOURCES = cat.c cksum.c comm.c csplit.c cut.c expand.c fold.c \
head.c join.c nl.c od.c paste.c pr.c sort.c split.c sum.c tac.c \
tail.c tr.c unexpand.c uniq.c wc.c version.c long-options.c

OBJECTS = cat.o cksum.o comm.o csplit.o cut.o expand.o fold.o \
head.o join.o nl.o od.o paste.o pr.o sort.o split.o sum.o tac.o \
tail.o tr.o unexpand.o uniq.o wc.o version.o long-options.o

DISTFILES = Makefile.in system.h version.h long-options.h $(SOURCES)

PROGS = cat cksum comm csplit cut expand fold head join nl od \
paste pr sort split sum tac tail tr unexpand uniq wc

all: $(PROGS)

incl = -I.. -I$(srcdir) -I$(srcdir)/../lib
.c.o:
	$(CC) -c $(CPPFLAGS) $(DEFS) $(incl) $(CFLAGS) $<

install: all
	for f in $(PROGS); do $(INSTALL_PROGRAM) $$f $(bindir)/$(binprefix)$$f; done

uninstall:
	for f in $(PROGS); do rm -f $(bindir)/$(binprefix)$$f; done

TAGS: $(SOURCES)
	etags $(SOURCES)

check:

clean:
	rm -f $(PROGS) $(LIBPROGS) *.o core

mostlyclean: clean

distclean: clean
	rm -f Makefile

realclean: distclean
	rm -f TAGS

dist:
	ln $(DISTFILES) ../`cat ../.fname`/src

# Linking rules.

$(PROGS): ../lib/libtu.a version.o

cat: cat.o
	$(CC) $(LDFLAGS) -o $@ cat.o ../lib/libtu.a version.o $(LIBS)

cksum: cksum.o
	$(CC) $(LDFLAGS) -o $@ cksum.o ../lib/libtu.a version.o $(LIBS)

comm: comm.o
	$(CC) $(LDFLAGS) -o $@ comm.o ../lib/libtu.a version.o $(LIBS)

csplit: csplit.o
	$(CC) $(LDFLAGS) -o $@ csplit.o ../lib/libtu.a version.o $(LIBS)

cut: cut.o
	$(CC) $(LDFLAGS) -o $@ cut.o ../lib/libtu.a version.o $(LIBS)

expand: expand.o
	$(CC) $(LDFLAGS) -o $@ expand.o ../lib/libtu.a version.o $(LIBS)

fold: fold.o
	$(CC) $(LDFLAGS) -o $@ fold.o ../lib/libtu.a version.o $(LIBS)

head: head.o
	$(CC) $(LDFLAGS) -o $@ head.o ../lib/libtu.a version.o $(LIBS)

join: join.o long-options.o
	$(CC) $(LDFLAGS) -o $@ join.o long-options.o ../lib/libtu.a version.o $(LIBS)

nl: nl.o
	$(CC) $(LDFLAGS) -o $@ nl.o ../lib/libtu.a version.o $(LIBS)

od: od.o
	$(CC) $(LDFLAGS) -o $@ od.o ../lib/libtu.a version.o $(LIBS)

paste: paste.o
	$(CC) $(LDFLAGS) -o $@ paste.o ../lib/libtu.a version.o $(LIBS)

pr: pr.o
	$(CC) $(LDFLAGS) -o $@ pr.o ../lib/libtu.a version.o $(LIBS)

sort: sort.o long-options.o
	$(CC) $(LDFLAGS) -o $@ sort.o long-options.o ../lib/libtu.a version.o $(LIBS)

split: split.o
	$(CC) $(LDFLAGS) -o $@ split.o ../lib/libtu.a version.o $(LIBS)

sum: sum.o
	$(CC) $(LDFLAGS) -o $@ sum.o ../lib/libtu.a version.o $(LIBS)

tac: tac.o
	$(CC) $(LDFLAGS) -o $@ tac.o ../lib/libtu.a version.o $(LIBS)

tail: tail.o
	$(CC) $(LDFLAGS) -o $@ tail.o ../lib/libtu.a version.o $(LIBS)

tr: tr.o
	$(CC) $(LDFLAGS) -o $@ tr.o ../lib/libtu.a version.o $(LIBS)

unexpand: unexpand.o
	$(CC) $(LDFLAGS) -o $@ unexpand.o ../lib/libtu.a version.o $(LIBS)

uniq: uniq.o
	$(CC) $(LDFLAGS) -o $@ uniq.o ../lib/libtu.a version.o $(LIBS)

wc: wc.o
	$(CC) $(LDFLAGS) -o $@ wc.o ../lib/libtu.a version.o $(LIBS)

csplit.o nl.o tac.o: ../lib/regex.h
comm.o nl.o uniq.o: ../lib/linebuffer.h
join.o sort.o: long-options.h
long-options.o: long-options.h

$(OBJECTS): ../config.h system.h version.h

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
