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
package_makefile_frag = Make.pack
target_makefile_frag = ../../target-mkfrag
# Copyright (C) 1989, 1992, 1993 Free Software Foundation, Inc.
# written by Douglas C. Schmidt (schmidt@ics.uci.edu)
# 
# This file is part of GNU TRIE-GEN.
# 
# GNU TRIE-GEN is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 1, or (at your option)
# any later version.
# 
# GNU trie-gen is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU trie-gen; see the file COPYING.  If not, write to
# the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 

srcdir = .

OBJS  = trie.o compact.o main.o version.o options.o
DEPEND_SOURCES = $(srcdir)/*.cc

#### package, host, target, and site dependent Makefile fragments come in here.
# Start of package fragment generated by ./../../config.shared.
# TOPDIR=$${rootme}../../../ invsubdir= subdir=
srcdir = .
SUBDIRS = 
prefix = /opt
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
gxx_includedir = $(libdir)/g++-include
docdir = $(datadir)/doc

SHELL = /bin/sh

INSTALL = ${rootme}$(srcdir)/../../../install.sh -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL)

AR = `if [ -f $${rootme}../../../binutils/ar ] ; \
	then echo $${rootme}../../../binutils/ar ; \
	else echo ar ; fi`
AR_FLAGS = rc
RANLIB = `if [ -f $${rootme}../../../binutils/ranlib ] ; \
	then echo $${rootme}../../../binutils/ranlib ; \
	else echo ranlib ; fi`
NM = `if [ -f $${rootme}../../../binutils/nm ] ; \
	then echo $${rootme}../../../binutils/nm ; \
	else echo nm ; fi`

MAKEINFO = `if [ -f $${rootme}../../../texinfo/C/makeinfo ] ; \
	then echo $${rootme}../../../texinfo/C/makeinfo ; \
	else echo makeinfo ; fi`
TEXIDIR=${srcdir}/../../..//texinfo
TEXI2DVI = TEXINPUTS=${TEXIDIR}:$$TEXINPUTS texi2dvi

CC = `if [ -f $${rootme}../../../gcc/xgcc ] ; \
	then echo $${rootme}../../../gcc/xgcc -B$${rootme}../../../gcc/ ; \
	else echo cc ; fi`
CXX = `if [ -f $${rootme}../../../gcc/xgcc ] ; \
	then echo $${rootme}../../../gcc/xgcc -B$${rootme}../../../gcc/ ; \
	else echo gcc ; fi`

XTRAFLAGS = `if [ -f $${rootme}../../../gcc/xgcc ] ;\
	then echo -I$${rootme}../../../gcc/include;\
	else echo ; fi`
WRAP_C_INCLUDES =
CFLAGS = -g
CXXFLAGS = -g -O
PROTODIR = $(srcdir)/../../src/gen
LIBS = ../../libg++.a
IO_DIR = ../libio
# IO_DIR = no-stream
.PHONY: all
all: $(NOTHING)


CXXINCLUDES =  -I. -I../../../libio -I$(srcdir) -I$(srcdir)/../../$(IO_DIR) -I$(srcdir)/../../src $(WRAP_C_INCLUDES) $(XTRAFLAGS)
CINCLUDES = $(XTRAFLAGS)
.SUFFIXES: .o .C .cc
.c.o:
	$(CC) -c $(CFLAGS) $(CINCLUDES) $<
.C.o:
	$(CXX) -c $(CXXFLAGS) $(CXXINCLUDES) $<
.cc.o:
	$(CXX) -c $(CXXFLAGS) $(CXXINCLUDES) $<


.PHONY: install
install:
.PHONY: check
.PHONY: info dvi install-info clean-info
info:
dvi:
install-info:
.PHONY: do-clean-info clean-info do-clean-dvi clean-dvi
do-clean-info:
do-clean-dvi:
clean-info: do-clean-info
clean-dvi: do-clean-dvi

.PHONY: boltcc
boltcc:
	rootme=`pwd`/ ; export rootme ; $(MAKE) $(FLAGS_TO_PASS)


# clean rules
.PHONY: mostlyclean clean distclean realclean
mostlyclean: clean-dvi
	rm -rf *.o \#* core trie-gen out.cc a.out 
clean: clean-dvi
	rm -rf *.o \#* core trie-gen out.cc a.out   
distclean: clean
	rm -rf config.status Makefile *~ Make.pack
realclean: clean clean-info
	rm -rf config.status Makefile *~ Make.pack depend *.info*

.PHONY: force
force:

# with the gnu make, this is done automatically.

Makefile: $(srcdir)/Makefile.in $(host_makefile_frag) $(target_makefile_frag)
	$(SHELL) ./config.status

.NOEXPORT:
MAKEOVERRIDES=


# DO NOT DELETE THIS LINE -- g++dep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

compact.o : compact.cc \
  compact.h
main.o : main.cc \
  $(srcdir)/../../$(IO_DIR)/stream.h \
  $(srcdir)/../../$(IO_DIR)/iostream.h \
  $(srcdir)/../../$(IO_DIR)/streambuf.h \
  options.h \
  trie.h \
  compact.h
options.o : options.cc \
  options.h
test.o : test.cc 
trie.o : trie.cc \
  options.h \
  trie.h \
  compact.h
version.o : version.cc \
  version.cc

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
# End of package fragment generated by ./../../config.shared.
# Warning this fragment is automatically generated
RANLIB = true
# Use -nostdinc++ flag if your version of gcc understands it.
NOSTDINC = -nostdinc++

# If the C include files are C++-ready (with extern "C"), uncomment next line:
# WRAP_C_INCLUDES =

# Flags to pass to gen-params when building _G_config.h.
# For example: G_CONFIG_ARGS = size_t="unsigned long"
G_CONFIG_ARGS =
##

trie-gen: $(OBJS) 
	$(CXX) -o $@ $(OBJS) $(LIBS)

LS_1 = /bin/ls # -1 default on most systems, not supported on all.

out.cc: trie-gen
	@echo "Generating a compacted minimal-prefix trie for files in /bin"
	$(LS_1) /bin | ./trie-gen -c > out.cc

check: out.o test.o
	$(CXX) out.o test.o $(LIBS)
	$(LS_1) /bin | ./a.out -v

run_tests: check

distrib: 
	(cd ..; rm -f trie-gen.tar.Z; tar cvf trie-gen.tar trie-gen; \
                compress trie-gen.tar)

# DO NOT DELETE THIS LINE -- g++dep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

compact.o : $(srcdir)/compact.cc $(srcdir)/compact.h 
main.o : $(srcdir)/main.cc $(srcdir)/options.h $(srcdir)/trie.h $(srcdir)/compact.h 
options.o : $(srcdir)/options.cc $(srcdir)/options.h 
trie.o : $(srcdir)/trie.cc $(srcdir)/trie.h $(srcdir)/compact.h 
version.o : $(srcdir)/version.cc 

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
