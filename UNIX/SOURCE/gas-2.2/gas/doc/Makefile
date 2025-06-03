# This file was generated automatically by configure.  Do not edit.
VPATH = .
links = 
host_alias = i386-unknown-sysv4.2
host_cpu = i386
host_vendor = unknown
host_os = sysv4.2
host_canonical = i386-unknown-sysv4.2
target_alias = i386-unknown-sysv4.2
target_cpu = i386
target_vendor = unknown
target_os = sysv4.2
target_canonical = i386-unknown-sysv4.2
# Makefile for GNU Assembler documentation
#   Copyright (C) 1987-1993 Free Software Foundation, Inc.

#This file is part of GNU GAS.

#GNU GAS is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2, or (at your option)
#any later version.

#GNU GAS is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with GNU GAS; see the file COPYING.  If not, write to
#the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

# The targets for external use include:
# all, doc, proto, install, uninstall, includes, TAGS,
# clean, cleanconfig, realclean, stage1, stage2, stage3, stage4.

# Variables that exist for you to override.
# See below for how to change them for certain systems.

srcdir = .

prefix = /opt

program_transform_name = 
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
libdir = $(exec_prefix)/lib

datadir = $(prefix)/lib
mandir = $(prefix)/man
man1dir = $(mandir)/man1
man2dir = $(mandir)/man2
man3dir = $(mandir)/man3
man4dir = $(mandir)/man4
man5dir = $(mandir)/man5
man6dir = $(mandir)/man6
man7dir = $(mandir)/man7
man8dir = $(mandir)/man8
man9dir = $(mandir)/man9
infodir = $(prefix)/info
includedir = $(prefix)/include
docdir = $(datadir)/doc

SHELL = /bin/sh

INSTALL = install -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL)
INSTALL_XFORM1 = $(INSTALL) -b $(program_transform_name) .1

AR = ar
AR_FLAGS = qv
BISON = bison
MAKEINFO = makeinfo
TEXI2DVI = TEXINPUTS=$(TEXIDIR):$$TEXINPUTS texi2dvi
RANLIB = ranlib

# What version of the manual you want; "all" includes everything
CONFIG=all

# Where to find texinfo.tex to format docn with TeX, AND GPL
TEXIDIR = $(srcdir)/../../texinfo

#### host, target, and site specific Makefile frags come in here.
##

all:
install:
	$(INSTALL_XFORM1) $(srcdir)/as.1 $(man1dir)/as.1

info: as.info
dvi: as.dvi

asdoc-config.texi: $(CONFIG).texi
	rm -f asdoc-config.texi
	ln -s $(srcdir)/$(CONFIG).texi ./asdoc-config.texi || \
	ln $(srcdir)/$(CONFIG).texi ./asdoc-config.texi || \
	cp $(srcdir)/$(CONFIG).texi ./asdoc-config.texi

as.info: $(srcdir)/as.texinfo asdoc-config.texi
	$(MAKEINFO) -I$(TEXIDIR) -o as.info $(srcdir)/as.texinfo

install-info: as.info
	for i in $<* ; do \
		$(INSTALL_DATA) $$i $(infodir)/$$i ; \
	done

dvi: as.dvi

as.dvi:	$(srcdir)/as.texinfo asdoc-config.texi
	$(TEXI2DVI) $(srcdir)/as.texinfo

# ROFF doc targets as.ms, as.mm, as.me
# (we don't use a variable because we don't trust all makes to handle
# a var in the target name right).
# roff output (-ms)
# THESE ARE PROBABLY BROKEN until texi2roff extended for Texinfo conditionals
as.ms: $(srcdir)/as.texinfo asdoc-config.texi
	sed -e '/\\input texinfo/d' \
		-e '/@c TEXI2ROFF-KILL/,/@c END TEXI2ROFF-KILL/d' \
		-e 's/{.*,,/{/' \
		$(srcdir)/as.texinfo | \
	texi2roff -ms >as.ms 

# roff output (-mm)
as.mm: $(srcdir)/as.texinfo asdoc-config.texi
	sed -e '/\\input texinfo/d' \
		-e '/@c TEXI2ROFF-KILL/,/@c END TEXI2ROFF-KILL/d' \
		-e 's/{.*,,/{/' \
		-e '/@noindent/d' \
		$(srcdir)/as.texinfo | \
	texi2roff -mm | \
	sed -e 's/---/\\(em/g' \
	>as.mm 

# roff output (-me)
as.me: $(srcdir)/as.texinfo asdoc-config.texi
	sed -e '/\\input texinfo/d' \
		-e '/@c TEXI2ROFF-KILL/,/@c END TEXI2ROFF-KILL/d' \
		-e 's/{.*,,/{/' \
		$(srcdir)/as.texinfo | \
	texi2roff -me >as.me 

clean: clean-dvi clean-info

distclean:
	rm -f Makefile config.status as.?? as.??s as.aux as.log as.toc

clean-dvi:
	rm -f as.?? as.???

clean-info:
	rm -f as.info*

force:

Makefile: $(srcdir)/Makefile.in $(host_makefile_frag) $(target_makefile_frag)
	$(SHELL) ./config.status
