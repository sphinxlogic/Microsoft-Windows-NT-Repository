# Generated automatically from Makefile.in by configure.
# Makefile for GNU fileutils programs.
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
bindir = $(exec_prefix)/bin
binprefix = 
libdir = $(exec_prefix)/lib

INSTALL = @INSTALL@
INSTALL_PROGRAM = @INSTALL_PROGRAM@
CC = gcc
DEFS = -DCONFIG_BROKETS -DHAVE_CONFIG_H
LIBS = 
CFLAGS = -g -O3
LDFLAGS = -g

SOURCES = chmod.c chgrp.c chown.c cp-aux.c cp-hash.c cp.c \
dd.c df.c du.c install.c ln.c ls.c mkdir.c mkfifo.c mknod.c mv.c mvdir.c \
rm.c rmdir.c touch.c version.c ls-ls.c ls-dir.c ls-vdir.c

OBJECTS = chmod.o chgrp.o chown.o cp-aux.o cp-hash.o cp.o \
dd.o df.o du.o install.o ln.o ls.o mkdir.o mkfifo.o mknod.o mv.o mvdir.o \
rm.o rmdir.o touch.o version.o ls-ls.o ls-dir.o ls-vdir.o

DISTFILES = Makefile.in cp.h ls.h version.h $(SOURCES)

PROGS = chgrp chown chmod cp dd du ginstall ln dir vdir ls mkdir \
mkfifo mknod mv rm rmdir touch  df

LIBPROGS =  mvdir

all: $(PROGS) $(LIBPROGS)

incl = -I.. -I$(srcdir) -I$(srcdir)/../lib
.c.o:
	$(CC) -c $(CPPFLAGS) $(DEFS) $(incl) $(CFLAGS) $<

install: all
	if [ "x$(LIBPROGS)" = x ]; then : ; else \
	  ./ginstall -o root -m 4755 $(LIBPROGS) $(libdir); fi
	for f in $(PROGS); do ./ginstall $$f $(bindir)/$(binprefix)$$f; done
	cd $(bindir); \
	  ./$(binprefix)ln -f $(binprefix)dir d; \
	  ./$(binprefix)ln -f $(binprefix)vdir v; \
	  ./$(binprefix)mv -f $(binprefix)ginstall $(binprefix)install

uninstall:
	/bin/rm -f $(libdir)/$(LIBPROGS)
	cd $(bindir); /bin/rm -f d v $(binprefix)install
	for f in $(PROGS); do /bin/rm -f $(bindir)/$(binprefix)$$f; done

TAGS: $(SOURCES)
	etags $(SOURCES)

check:

# Use /bin/rm instead of ./rm in case ./rm is compiled for a different
# architecture.
clean:
	/bin/rm -f $(PROGS) $(LIBPROGS) *.o core

mostlyclean: clean

distclean: clean
	rm -f Makefile

realclean: distclean
	rm -f TAGS

dist: $(DISTFILES)
	ln $(DISTFILES) ../`cat ../.fname`/src

# Linking rules.

$(PROGS) $(LIBPROGS): ../lib/libfu.a version.o

chmod: chmod.o
	$(CC) $(LDFLAGS) -o $@ chmod.o ../lib/libfu.a version.o $(LIBS)

chgrp: chgrp.o
	$(CC) $(LDFLAGS) -o $@ chgrp.o ../lib/libfu.a version.o $(LIBS)

chown: chown.o
	$(CC) $(LDFLAGS) -o $@ chown.o ../lib/libfu.a version.o $(LIBS)

cp: cp.o cp-hash.o cp-aux.o
	$(CC) $(LDFLAGS) -o $@ cp.o cp-hash.o cp-aux.o ../lib/libfu.a version.o $(LIBS)

dd: dd.o
	$(CC) $(LDFLAGS) -o $@ dd.o ../lib/libfu.a version.o $(LIBS)

df: df.o
	$(CC) $(LDFLAGS) -o $@ df.o ../lib/libfu.a version.o $(LIBS)

dir: ls.o ls-dir.o
	$(CC) $(LDFLAGS) -o $@ ls.o ls-dir.o ../lib/libfu.a version.o $(LIBS)

du: du.o
	$(CC) $(LDFLAGS) -o $@ du.o ../lib/libfu.a version.o $(LIBS)

ginstall: install.o
	$(CC) $(LDFLAGS) -o $@ install.o ../lib/libfu.a version.o $(LIBS)

ln: ln.o
	$(CC) $(LDFLAGS) -o $@ ln.o ../lib/libfu.a version.o $(LIBS)

ls: ls.o ls-ls.o
	$(CC) $(LDFLAGS) -o $@ ls.o ls-ls.o ../lib/libfu.a version.o $(LIBS)

mkdir: mkdir.o
	$(CC) $(LDFLAGS) -o $@ mkdir.o ../lib/libfu.a version.o $(LIBS)

mkfifo: mkfifo.o
	$(CC) $(LDFLAGS) -o $@ mkfifo.o ../lib/libfu.a version.o $(LIBS)

mknod: mknod.o
	$(CC) $(LDFLAGS) -o $@ mknod.o ../lib/libfu.a version.o $(LIBS)

mv: mv.o
	$(CC) $(LDFLAGS) -o $@ mv.o ../lib/libfu.a version.o $(LIBS)

mvdir: mvdir.o
	$(CC) $(LDFLAGS) -o $@ mvdir.o ../lib/libfu.a version.o $(LIBS)

rm: rm.o
	$(CC) $(LDFLAGS) -o $@ rm.o ../lib/libfu.a version.o $(LIBS)

rmdir: rmdir.o
	$(CC) $(LDFLAGS) -o $@ rmdir.o ../lib/libfu.a version.o $(LIBS)

touch: touch.o
	$(CC) $(LDFLAGS) -o $@ touch.o ../lib/libfu.a version.o $(LIBS)

vdir: ls.o ls-vdir.o
	$(CC) $(LDFLAGS) -o $@ ls.o ls-vdir.o ../lib/libfu.a version.o $(LIBS)

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:

$(OBJECTS): ../config.h

chmod.o : chmod.c ../lib/modechange.h ../lib/system.h \
  ../lib/pathmax.h version.h 
chgrp.o : chgrp.c ../lib/system.h ../lib/pathmax.h version.h 
chown.o : chown.c ../lib/system.h ../lib/pathmax.h version.h 
cp-aux.o : cp-aux.c cp.h ../lib/system.h ../lib/pathmax.h 
cp-hash.o : cp-hash.c cp.h ../lib/system.h ../lib/pathmax.h 
cp.o : cp.c cp.h ../lib/system.h ../lib/pathmax.h \
  ../lib/backupfile.h version.h 
dd.o : dd.c ../lib/system.h ../lib/pathmax.h version.h 
df.o : df.c ../lib/mountlist.h ../lib/fsusage.h ../lib/system.h \
  ../lib/pathmax.h version.h 
du.o : du.c ../lib/system.h ../lib/pathmax.h version.h 
install.o : install.c ../lib/system.h ../lib/pathmax.h \
  version.h ../lib/modechange.h 
ln.o : ln.c ../lib/system.h ../lib/pathmax.h ../lib/backupfile.h version.h 
ls.o : ls.c ../lib/system.h ../lib/pathmax.h version.h ls.h
mkdir.o : mkdir.c ../lib/system.h ../lib/pathmax.h \
  ../lib/modechange.h version.h 
mkfifo.o : mkfifo.c ../lib/system.h ../lib/pathmax.h \
  ../lib/modechange.h version.h 
mknod.o : mknod.c ../lib/system.h ../lib/pathmax.h \
  ../lib/modechange.h version.h 
mv.o : mv.c ../lib/system.h ../lib/pathmax.h ../lib/backupfile.h version.h 
mvdir.o : mvdir.c ../lib/system.h ../lib/pathmax.h version.h 
rm.o : rm.c ../lib/system.h ../lib/pathmax.h version.h 
rmdir.o : rmdir.c ../lib/system.h ../lib/pathmax.h version.h 
touch.o : touch.c ../lib/system.h ../lib/pathmax.h version.h 
version.o : version.c version.h 
