# Generated automatically from Makefile.in by configure.
# Master Makefile for the GNU Concurrent Versions System.
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

# @(#)Makefile.in 1.17 92/04/10

SHELL = /bin/sh

#### Start of system configuration section. ####

srcdir = .


# If you use gcc, you should either run the fixincludes script that
# comes with it or else use gcc with the -traditional option.  Otherwise
# ioctl calls will be compiled incorrectly on some systems.
CC = gcc
AR = ar
# Older makes don't set $(MAKE), so we set it for them.
MAKE = make
# Set RANLIB = echo if your system doesn't have or need ranlib.
RANLIB = ranlib
# Set YACC = bison or yacc, depending on which you have on your system
YACC = bison -y
# Use cp if you don't have install.
INSTALL = install -c
INSTALLDATA = install -c -m 644

# Things you might add to DEFS:
# -DSTDC_HEADERS	If you have ANSI C headers and libraries.
# -DPOSIX		If you have POSIX.1 headers and libraries.
#			Also need to define -DDIRENT.
# -DUSG			If you have System V/ANSI C string and
#			memory functions and headers, ndir.h,
#			sys/sysmacros.h, no sys/times.h, fcntl.h,
#			getcwd.
# -DSIGTYPE=int		If your signal handlers return int, not void.
# -DDIRENT		If you have dirent.h.
# -DSYSNDIR		Old Xenix systems (selects sys/ndir.h).
# -DNDIR		Uses <ndir.h> instead od <sys/ndir.h>.
# -Duid_t=int		If <sys/types.h> does not define uid_t.
# -Dgid_t=int		If <sys/types.h> does not define gid_t.
# -Dmode_t=int		If <sys/types.h> does not define mode_t.
# -Dsize_t=int		If <sys/types.h> does not define size_t.
# -D__CHAR_UNSIGNED__	If type `char' is unsigned.
# -DTM_IN_SYS_TIME	If <time.h> does not define struct tm.
# -DFCHMOD_MISSING	If you lack fchmod system call.
# -DFSYNC_MISSING	If you lack fsync system call.
# -DFTIME_MISSING	If you lack ftime function.
# -DMKFIFO_MISSING	If you lack mkfifo system call, but have FIFOs.
# -DPUTENV_MISSING	If you lack putenv function.
# -DSTRERROR_MISSING	If you lack strerror function.
# -DSETVBUF_MISSING	If you lack setvbuf function.
# -DVFORK_MISSING	If you lack vfork system call.
# -DVPRINTF_MISSING	If you lack vprintf function (but have _doprnt).
# -DDOPRNT_MISSING	If you lack _doprnt function.  Also need to define
#			-DVPRINTF_MISSING.
# -DTIMEB_H_MISSING	If you don't have a <sys/timeb.h> include file.
# -DHAVE_TIMEZONE	If you lack ftime, but do have a timezone function.
# -DBSD_SIGNALS		If you lack POSIX signals, but have BSD signals.
# -DFTRUNCATE_MISSING	If you lack ftruncate system call.
# -DUTIME_NULL_MISSING	If your utime system call does not use the
#			current time when passed a null time pointer.
# -DMVDIR=libdir/mvdir	If you lack rename system call.

DEFS =  -DUSG -DDIRENT -DSTDC_HEADERS -DPOSIX -DFTIME_MISSING
LIBS = 
LIBPROGS = 
CDEBUG = -g -O

INCLUDES = -I. -I../lib 
CFLAGS = $(CDEBUG) $(DEFS) $(INCLUDES)
LDFLAGS = $(CDEBUG)

prefix = /opt
exec_prefix = /opt

# Where to install the executables.
bindir = $(exec_prefix)/bin

# Where to put the system-wide .cvsrc file
libdir = $(prefix)/lib

# Where to put the manual pages.
mandir = $(prefix)/man

#### End of system configuration section. ####

MDEFINES = bindir='$(bindir)' libdir='$(libdir)' mandir='$(mandir)' \
INSTALL='$(INSTALL)' INSTALLDATA='$(INSTALLDATA)' \
LIBS='$(LIBS)' LIBPROGS='$(LIBPROGS)' MAKE='$(MAKE)' \
AR='$(AR)' RANLIB='$(RANLIB)' \
YACC='$(YACC)' CFLAGS='$(CFLAGS)' LDFLAGS='$(LDFLAGS)' LIBS='$(LIBS)' \
CC='$(CC)' INCLUDES='$(INCLUDES)' DEFS='$(DEFS)'

DISTFILES = COPYING COPYING.LIB ChangeLog INSTALL Makefile.in README TODO configure

# Subdirectories to run make in for the primary targets.
SUBDIRS = lib src man doc examples contrib
# Only make TAGS/tags files in these directories, in this order
TSUBDIRS= src lib

all:
	@for dir in $(SUBDIRS); do echo making $@ in $$dir; cd $$dir; $(MAKE) $(MDEFINES) $@ || exit 1; cd ..; done
.PHONY: all

info:
clean-info:
install-info:

install:
	@for dir in $(SUBDIRS); do echo making $@ in $$dir; cd $$dir; $(MAKE) $(MDEFINES) $@ || exit 1; cd ..; done
.PHONY: install

tags:
	@for dir in $(TSUBDIRS); do echo making $@ in $$dir; cd $$dir; $(MAKE) $(MDEFINES) $@ || exit 1; cd ..; done
	@echo making $@ in .
	@ctags `for i in \`$(MAKE) SUBDIRS="$(TSUBDIRS)" ls\` ; do echo $(srcdir)/$$i ; done`
.PHONY: tags

TAGS:
	@for dir in $(TSUBDIRS); do echo making $@ in $$dir; cd $$dir; $(MAKE) $(MDEFINES) $@ || exit 1; cd ..; done
	@echo making $@ in .
	@etags `for i in \`$(MAKE) SUBDIRS="$(TSUBDIRS)" ls\` ; do echo $(srcdir)/$$i ; done`

ls:
	@echo $(DISTFILES)
	@for dir in $(SUBDIRS); do \
		for i in `cd $$dir; $(MAKE) $(MDEFINES) $@` ; do \
			echo $$dir/$$i ; \
		done ; \
	done
.PHONY: ls

clean:
	@for dir in $(SUBDIRS); do echo making $@ in $$dir; cd $$dir; $(MAKE) $(MDEFINES) $@ || exit 1; cd ..; done
.PHONY: clean

distclean:
	@for dir in $(SUBDIRS); do echo making $@ in $$dir; cd $$dir; $(MAKE) $(MDEFINES) $@ || exit 1; cd ..; done
	rm -f Makefile config.status tags TAGS
.PHONY: distclean

realclean:
	@for dir in $(SUBDIRS); do echo making $@ in $$dir; cd $$dir; $(MAKE) $(MDEFINES) $@ || exit 1; cd ..; done
	rm -f Makefile config.status tags TAGS
.PHONY: realclean

saber:
	@for dir in $(SUBDIRS); do cd $$dir; $(MAKE) $(MDEFINES) $@ || exit 1; cd ..; done
.PHONY: saber

lint:
	@for dir in $(SUBDIRS); do cd $$dir; $(MAKE) $(MDEFINES) xlint || exit 1; cd ..; done
.PHONY: lint

dist:
	echo cvs-`sed -e '/version_string/!d' -e 's/[^0-9.]*\([0-9.]*\).*/\1/' -e q src/version.c` > .fname
	rm -rf `cat .fname`
	mkdir `cat .fname`
	ln $(DISTFILES) `cat .fname`
	for dir in $(SUBDIRS); do mkdir `cat .fname`/$$dir; cd $$dir; $(MAKE) $@; cd ..; done
	tar chZf `cat .fname`.tar.Z `cat .fname`
	rm -rf `cat .fname` .fname
