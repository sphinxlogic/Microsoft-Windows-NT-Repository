# Generated automatically from Makefile.in by configure.
# Master Makefile for the GNU shell programming utilities.
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

# This directory's subdirectories are mostly independent; you can cd
# into them and run `make' without going through this Makefile.
# To change the values of `make' variables: instead of editing Makefiles,
# (1) if the variable is set in `config.status', edit `config.status'
#     (which will cause the Makefiles to be regenerated when you run `make');
# (2) otherwise, pass the desired values on the `make' command line.

srcdir = .
VPATH = .

prefix = /opt
exec_prefix = ${prefix}
bindir = $(exec_prefix)/bin
libdir = $(exec_prefix)/lib
datadir = $(prefix)/lib
mandir = $(prefix)/man/man1

SHELL = /bin/sh

DISTFILES = COPYING COPYING.LIB ChangeLog Makefile.in README INSTALL \
NEWS configure configure.in config.h.in acconfig.h mkinstalldirs

# Redundant stuff for making only selected programs.
PROGS = basename date dirname echo env expr false groups id logname \
pathchk printenv printf sleep tee test true tty who whoami yes  uname nice nohup stty

# Subdirectories to run make in for the primary targets.
SUBDIRS = lib src man

all:
	for dir in $(SUBDIRS); do \
	  echo making $@ in $$dir ; \
	  (cd $$dir; $(MAKE) $@) ; \
	done

info:
install-info:
dvi:

check:
installcheck:

$(PROGS) su:
	cd lib; $(MAKE) all
	cd src; $(MAKE) $@

config.h: stamp-config ;
stamp-config: config.status $(srcdir)/config.h.in
	$(SHELL) config.status
	touch stamp-config

Makefile: config.status $(srcdir)/Makefile.in $(srcdir)/lib/Makefile.in \
	    $(srcdir)/man/Makefile.in $(srcdir)/src/Makefile.in
	$(SHELL) config.status

config.status: configure
	$(SHELL) config.status --recheck
configure: configure.in
	cd $(srcdir); autoconf
config.h.in: configure.in
	cd $(srcdir); autoheader

# Make sure all installation directories, e.g. $(bindir) actually exist by
# making them if necessary.
installdirs:
	$(SHELL) $(srcdir)/mkinstalldirs \
	  $(bindir) $(datadir) $(libdir) $(infodir) $(mandir)

install:: installdirs

install uninstall TAGS clean mostlyclean distclean realclean::
	for dir in $(SUBDIRS); do \
	  echo making $@ in $$dir ; \
	  (cd $$dir; $(MAKE) $@) ; \
	done

distclean realclean::
	rm -f Makefile config.status config.h stamp-config

dist: $(DISTFILES)
	echo shellutils-`sed -e '/version_string/!d' \
	    -e 's/[^0-9.]*\([0-9.a-z]*\).*/\1/' -e q src/version.c` > .fname
	rm -rf `cat .fname`
	mkdir `cat .fname`
	ln $(DISTFILES) `cat .fname`
	for dir in $(SUBDIRS); do \
	  mkdir `cat .fname`/$$dir; \
	  (cd $$dir; $(MAKE) $@); \
	done
	tar --gzip -chvf `cat .fname`.tar.gz `cat .fname`
	rm -rf `cat .fname` .fname

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
