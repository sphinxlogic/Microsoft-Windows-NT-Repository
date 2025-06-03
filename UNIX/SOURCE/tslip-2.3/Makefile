# Generated automatically from Makefile.in by configure.
# Top Makefile for TSLIP.	-*- Indented-Text -*-
# @(#)Makefile.in	1.10 (13 Oct 1993)

#...........................................................................
# Copyright 1993, Jim Avera.  All Rights Reserved.
#  
# This program contains new material owned by the above copyright holder,
# and may also contain portions derived from existing works used according
# to permission granted by the owners of those works.
# 
# You may use, modify, or distribute this work (or the portions owned by the 
# above copyright holder) ONLY as described in the file "COPYRIGHT" which 
# accompanies this file or program.
# 
# BECAUSE THIS SOFTWARE IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY,
# EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, TO
# THE EXTENT PERMITTED BY LAW.  THIS SOFTWARE IS PROVIDED "AS IS".
# IN NO EVENT UNLESS REQUIRED BY LAW WILL COPYRIGHT HOLDER BE LIABLE FOR 
# DAMAGES OF ANY NATURE ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.
#..........................................................................

# NOTE: You must first build Taylor UUCP 1.04 (with the patches in
# PATCHES_TO_STOCK_TAYLOR1.04).  Several .o and .a files built in the
# Taylor source tree are linked into the slip dialing daemon (see util/slipd).

#### Start of system configuration section. ####

srcdir = .
VPATH = .

# Set "taylorsrc" to the root of the Taylor UUCP 1.04 source tree, and
# "taylorobj" to the object code tree (usually the same).
# This path may be relative to the current directory (../something is okay).
taylorsrc=/cdcopy/SOURCE/uucp-1.04
taylorobj=/cdcopy/SOURCE/uucp-1.04

CC = gcc

#DVRCC is used only to compile the driver, to avoid gcc compatibility problems
DVRCC=/usr/bin/cc

INSTALL = /opt/bin/install -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644

DEFS = -DHAVE_CONFIG_H
LIBS =  -lsocket -lnsl

CFLAGS = -g -O3
LDFLAGS = -g

prefix = /opt
exec_prefix = ${prefix}

bindir = $(exec_prefix)/bin
sbindir = $(exec_prefix)/lib/slip

#### End of system configuration section. ####


SHELL = /bin/sh

VERSION = `$(SHELL) get_version.sh patchlevel.h`
# Derive package name from the current directory (e.g.  /dir/foo-1.2 => foo)
PKGNAME = `pwd | sed -e 's@.*/@@' -e 's@-.*@@'`
DISTNAME = $(PKGNAME)-$(VERSION)

# Where to start find(1) to locate files in the distribution
DIST_FIND_START = $(srcdir)

# subdirectories which have their own Makefiles
MAKE_SUBDIRS = driver/io utils dialin man

.c.o:
	$(CC) -c -I$(srcdir) $(DEFS) $(CPPFLAGS) $(CFLAGS) $<

default: all

Makefile:: Makefile.in config.h.in config.status
	rm -f Makefile
	$(SHELL) ./config.status
	chmod -w Makefile

config.status: configure
	$(SHELL) $(srcdir)/configure \
		--srcdir=$(srcdir) --prefix=$(prefix) --no-create

configure: configure.in
	cd $(srcdir); autoconf

config.h.in: configure.in acconfig.h
	cd $(srcdir); autoheader

clean::
	rm -f *.o *.a core *out out* TMP* autoh[0-9]*
	@pkgname=$(PKGNAME); set -x; /bin/rm -rf $$pkgname-[0-9]*

mostlyclean:: clean

realclean:: distclean
	rm -f TAGS tags *.info* 

distclean:: clean
	-cp config.status /tmp/tslip.config.status  #In case make dist bombs
	rm -f Makefile config.h config.status
	rm -rf $(PKGNAME)-*

# Do subdirectories for each of these rules (this must follow local rules)
all install installman clean distclean mostlyclean realclean uninstall:: force
	for dir in $(MAKE_SUBDIRS) ; do \
		echo "----- $$dir -----: $@"; \
		(cd $$dir; $(MAKE) $(MFLAGS) $(MDEFINES) $@) || exit 1; \
	done

dist: distclean DISTFILES
	distname=$(DISTNAME); \
	set -x; \
	rm -f $$distname; \
	mkdir $$distname; \
	cpio -pdlmv $$distname <DISTFILES; \
	tar -cvf $$distname.tar $$distname; \
	rm -rf $$distname; \
	gzip -v $$distname.tar

# Gets rid of most of the unwanted files.  Verify manually (if necessary)
# that this produces a list of just the files desired in the distribution
DISTFILES: force
DISTFILES: distclean
	(cd $(srcdir); find $(DIST_FIND_START) ! -type d -print) \
	| sed  '/\/RCS\//d; \
	       /\/SCCS\//d; \
	       /\/nondist\//d; \
	       /\.tar.*/d; \
	       /\/#.*#/d; /~$$/d; /\.o$$/d; \
	       /\/config\.status$$/d; \
	       /\/Makefile$$/d;	\
	       /\/config.h$$/d;	\
	       /\/.*\.BAK$$/d; \
	       /\/core$$/d; \
	       /\/out[^/]*$$/d; \
	       /\.out$$/d; \
	       /\/mkout$$/d; \
	       /\/=/d; \
	       /\/BUG[^/]*$$/d; \
	       /\/conftest\.c$$/d; \
	       /\/DISTFILES$$/d; \
	       /\/old_[^/]*$$/d; \
	       /\.old$$/d; \
	       /\.orig$$/d; \
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


force:

# Prevent GNU make v3 from overflowing arg limit on SysV.
.NOEXPORT:
