# Generated automatically from Makefile.in by configure.
# Makefile for GNU DIFF
# Copyright (C) 1988, 1989, 1991, 1992, 1993 Free Software Foundation, Inc.
#
# This file is part of GNU DIFF.
#
# GNU DIFF is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# GNU DIFF is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU DIFF; see the file COPYING.  If not, write to
# the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

#### Start of system configuration section. ####

srcdir = .
VPATH = .

CC = gcc
INSTALL = /usr/ucb/install -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644
MAKEINFO = makeinfo

# Things you might add to DEFS:
# -DSTDC_HEADERS	If you have ANSI C headers and libraries.
# -DHAVE_UNISTD_H	If you have unistd.h.
# -DHAVE_STRING_H	If you don't have ANSI C headers but have string.h.
# -DDIRENT		If you have dirent.h.
# -DSYSNDIR		Old Xenix systems (sys/ndir.h).
# -DSYSDIR		Old BSD systems (sys/dir.h).
# -DNDIR		Old System V systems (ndir.h).
# -DHAVE_VFORK_H	If you have vfork and it needs vfork.h.
# -DHAVE_DUP2		If you have dup2 system call.
# -DHAVE_WAITPID	If you have waitpid system call.
# -DHAVE_ST_BLKSIZE	If your struct stat has an st_blksize member.
# -DHAVE_STRERROR	If you have strerror function.
# -DHAVE_ALLOCA_H	If you have a working alloca.h.

DEFS =  -DDIRENT=1 -DSTDC_HEADERS=1 -DHAVE_UNISTD_H=1 -DHAVE_DUP2=1 -DHAVE_MEMCHR=1 -DHAVE_STRERROR=1 -DHAVE_WAITPID=1 -DHAVE_FCNTL_H=1 -DHAVE_STRING_H=1 -DHAVE_SYS_WAIT_H=1 -DHAVE_TIME_H=1 -DHAVE_ST_BLKSIZE=1

CFLAGS = -g -O3
LDFLAGS = -g
LIBS = 

# Some System V machines do not come with libPW.
# If this is true for you, use the GNU alloca.o here.
ALLOCA = 

prefix = /opt
exec_prefix = $(prefix)

# Prefix for each installed program, normally empty or `g'.
binprefix = 

bindir = $(exec_prefix)/bin

infodir = $(prefix)/info

#### End of system configuration section. ####

SHELL = /bin/sh

# The source files for all of the programs.
srcs=diff.c analyze.c io.c context.c ed.c normal.c ifdef.c util.c dir.c \
	version.c diff.h regex.c regex.h side.c system.h \
	diff3.c sdiff.c cmp.c error.c xmalloc.c getopt.c getopt1.c getopt.h \
	fnmatch.c fnmatch.h alloca.c
# Object files for diff only.
objs=diff.o analyze.o io.o context.o \
     ed.o normal.o util.o dir.o fnmatch.o \
     regex.o side.o ifdef.o version.o \
     getopt.o getopt1.o $(ALLOCA)
distfiles = $(srcs) README INSTALL NEWS diagmeet.note Makefile.in configure \
	configure.in COPYING ChangeLog diff.texi diff.info* texinfo.tex

all: diff diff3 sdiff cmp diff.info

.c.o:
	$(CC) -c $(DEFS) $(CPPFLAGS) -I. -I$(srcdir) $(CFLAGS) $<

diff: $(objs)
	$(CC) -o $@ $(LDFLAGS) $(objs) $(LIBS)

diff3: diff3.o getopt.o getopt1.o version.o $(ALLOCA)
	$(CC) -o $@ $(LDFLAGS) diff3.o getopt.o getopt1.o version.o $(ALLOCA) $(LIBS)

sdiff: sdiff.o getopt.o getopt1.o version.o $(ALLOCA)
	$(CC) -o $@ $(LDFLAGS) sdiff.o getopt.o getopt1.o version.o $(ALLOCA) $(LIBS)

cmp: cmp.o getopt.o getopt1.o error.o xmalloc.o $(ALLOCA)
	$(CC) -o $@ $(LDFLAGS) cmp.o getopt.o getopt1.o error.o xmalloc.o $(ALLOCA) $(LIBS)

diff.info: diff.texi
	$(MAKEINFO) $(srcdir)/diff.texi

$(objs): diff.h system.h
cmp.o diff3.o sdiff.o: system.h
context.o diff.o regex.o: regex.h
cmp.o diff.o diff3.o sdiff.o getopt.o getopt1.o: getopt.h
diff.o fnmatch.o: fnmatch.h

diff3.o: diff3.c
	$(CC) -c $(DEFS) $(CPPFLAGS) $(CFLAGS) -DDIFF_PROGRAM=\"$(bindir)/$(binprefix)diff\" $(srcdir)/diff3.c

sdiff.o: sdiff.c
	$(CC) -c $(DEFS) $(CPPFLAGS) $(CFLAGS) -DDIFF_PROGRAM=\"$(bindir)/$(binprefix)diff\" $(srcdir)/sdiff.c

TAGS: $(srcs)
	etags $(srcs)

clean:
	rm -f *.o diff diff3 sdiff cmp core

mostlyclean: clean

distclean: clean
	rm -f Makefile config.status

realclean: distclean
	rm -f TAGS *.info*

install: all
	for p in diff diff3 sdiff cmp; \
	do $(INSTALL_PROGRAM) $$p $(bindir)/$(binprefix)$$p; done
	cd $(srcdir); for f in diff.info*; \
	do $(INSTALL_DATA) $$f $(infodir)/$$f; done

uninstall:
	for p in diff diff3 sdiff cmp; \
	do rm -f $(bindir)/$(binprefix)$$p; done
	rm -f $(infodir)/diff.info*

dist: $(distfiles)
	echo diff-`sed -e '/version_string/!d' -e 's/[^0-9]*\([0-9a-z.]*\).*/\1/' -e q version.c` > .fname
	rm -rf `cat .fname`
	mkdir `cat .fname`
	-ln $(distfiles) `cat .fname`
	for file in $(distfiles); do \
	  if [ ! -r `cat .fname`/$$file ]; then cp -p $$file `cat .fname`; fi \
	done
	tar chZf `cat .fname`.tar.Z `cat .fname`
	tar chf - `cat .fname` | gzip > `cat .fname`.tar.z
	rm -rf `cat .fname` .fname

# Prevent GNU make v3 from overflowing arg limit on SysV.
.NOEXPORT:
