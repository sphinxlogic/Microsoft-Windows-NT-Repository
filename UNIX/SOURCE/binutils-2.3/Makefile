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
host_makefile_frag = config/mh-sysv4
#
# Makefile for directory with subdirs to build.
#   Copyright (C) 1990, 1991, 1992, 1993 Free Software Foundation
#
# This file is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */
#

srcdir = .

prefix = /opt

exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
libdir = $(exec_prefix)/lib
tooldir = $(exec_prefix)/i386-unknown-sysv4.2

program_transform_name = 

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

INSTALL = $${srcroot}/install.sh -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL)
INSTALL_XFORM = $(INSTALL) -t='$(program_transform_name)'

AS = as
AR = ar
AR_FLAGS = rc
CC = gcc

# We don't specify -g -O because many compilers don't support -g -O,
# and/or -O is broken in and of itself.
# We're using gcc-2.5.6, and we believe it works (Greg Lehey, LEMIS, 25 January 1994)
CFLAGS = -g -O3

CXX = gcc

# Use -O to stress test the compiler.
CXXFLAGS = -g -O3

RANLIB = ranlib
NM = nm
MUNCH_NM = $(NM)
GZIP = gzip

BISON = bison -y
LEX = `if [ -f $${rootme}/flex/flex ] ; \
	then echo $${rootme}/flex/flex ; \
	else echo flex ; fi`

M4 = `if [ -f $${rootme}/m4/m4 ] ; \
	then echo $${rootme}/m4/m4 ; \
	else echo m4 ; fi`

MAKEINFO = `if [ -f $${rootme}/texinfo/makeinfo/makeinfo ] ; \
	then echo $${rootme}/texinfo/makeinfo/makeinfo ; \
	else echo makeinfo ; fi`
# This just becomes part of the MAKEINFO definition passed down to
# sub-makes.  It lets flags be given on the command line while still
# using the makeinfo from the object tree.
MAKEINFOFLAGS =

EXPECT = `if [ -f $${rootme}/expect/expect ] ; \
	then echo $${rootme}/expect/expect ; \
	else echo expect ; fi`

RUNTEST = `if [ -f $${srcroot}/dejagnu/runtest ] ; \
	then echo $${srcroot}/dejagnu/runtest ; \
	else echo runtest ; fi`


# libraries that may need to be augmented on a system-by-system basis
X11_LIB = -lX11

# compilers to use to create programs which must be run in the build
# environment.
CC_FOR_BUILD = $(CC)
CXX_FOR_BUILD = $(CXX)

SUBDIRS = mmalloc libiberty opcodes bfd readline gash tcl tk tclX byacc flex bison binutils ld gas gcc gdb make patch prms send-pr gprof gdbtest tgas etc expect dejagnu sim m4 autoconf ispell grep diff rcs cvs fileutils shellutils textutils wdiff find  uudecode hello tar gzip indent recode release sed utils libm libio libg++ groff
OTHERS = 

ALL = all.normal
INSTALL_TARGET = install-dirs \
	$(INSTALL_MODULES) \
	$(INSTALL_TARGET_MODULES) \
	$(INSTALL_X11_MODULES) \
	install-gcc

CC_FOR_TARGET = ` \
  if [ -f $${rootme}/gcc/Makefile ] ; then \
    echo $${rootme}/gcc/xgcc -B$${rootme}/gcc/; \
  else \
    if [ "$(host_canonical)" = "$(target_canonical)" ] ; then \
      echo $(CC); \
    else \
      t='$(program_transform_name)'; echo gcc | sed -e '' $$t; \
    fi; \
  fi`


CXX_FOR_TARGET = ` \
  if [ -f $${rootme}/gcc/Makefile ] ; then \
    echo $${rootme}/gcc/xgcc -B$${rootme}/gcc/; \
  else \
    if [ "$(host_canonical)" = "$(target_canonical)" ] ; then \
      echo $(CXX); \
    else \
      t='$(program_transform_name)'; echo gcc | sed -e '' $$t; \
    fi; \
  fi`

AS_FOR_TARGET = ` \
  if [ -f $${rootme}/gas/Makefile ] ; then \
    echo $${rootme}/gas/as.new ; \
  else \
    if [ "$(host_canonical)" = "$(target_canonical)" ] ; then \
      echo $(AS); \
    else \
       t='$(program_transform_name)'; echo as | sed -e '' $$t ; \
    fi; \
  fi`

AR_FOR_TARGET = ` \
  if [ -f $${rootme}/binutils/ar ] ; then \
    echo $${rootme}/binutils/ar ; \
  else \
    if [ "$(host_canonical)" = "$(target_canonical)" ] ; then \
      echo $(AR); \
    else \
       t='$(program_transform_name)'; echo ar | sed -e '' $$t ; \
    fi; \
  fi`

RANLIB_FOR_TARGET = ` \
  if [ -f $${rootme}/binutils/ranlib ] ; then \
    echo $${rootme}/binutils/ranlib ; \
  else \
    if [ "$(host_canonical)" = "$(target_canonical)" ] ; then \
      echo $(RANLIB); \
    else \
       t='$(program_transform_name)'; echo ranlib | sed -e '' $$t ; \
    fi; \
  fi`

NM_FOR_TARGET = ` \
  if [ -f $${rootme}/binutils/Makefile ] ; then \
    echo $${rootme}/binutils/nm ; \
  else \
    if [ "$(host_canonical)" = "$(target_canonical)" ] ; then \
      echo $(NM); \
    else \
       t='$(program_transform_name)'; echo nm | sed -e '' $$t ; \
    fi; \
  fi`

# FIXME: This is badly named.
XTRAFLAGS = ` \
  if [ -f $${rootme}/gcc/Makefile ] ; then \
    if [ -f $${rootme}/newlib/Makefile ] ; then \
      echo -I$${rootme}/newlib/targ-include -I$${srcroot}/newlib/libc/include -I$${rootme}/gcc/include -nostdinc ; \
    else \
      echo -I$${rootme}/gcc/include ; \
    fi ; \
  else \
     echo ; \
  fi`

#### host and target specific makefile fragments come in here.
# Define SYSV as -DSYSV if you are using a System V operating system.
SYSV = -DSYSV -DSVR4
RANLIB = true

# C++ debugging is not yet supported under SVR4 (DWARF)
CXXFLAGS=-O

# The l flag generates a warning from the SVR4 archiver, remove it.
AR_FLAGS = cq


X11_INCLUDE_FLAGS = -I/usr/X/include
X11_LIB_FLAGS = -L/usr/X/lib
X11_LIB = -lX11 -lnsl
###

# Flags to pass down to all sub-makes.
# Please keep these in alphabetical order.
BASE_FLAGS_TO_PASS = \
	"AR_FLAGS=$(AR_FLAGS)" \
	"AR_FOR_TARGET=$(AR_FOR_TARGET)" \
	"AS_FOR_TARGET=$(AS_FOR_TARGET)" \
	"BISON=$(BISON)" \
	"CC_FOR_BUILD=$(CC_FOR_BUILD)" \
	"CC_FOR_TARGET=$(CC_FOR_TARGET)" \
	"CFLAGS=$(CFLAGS)" \
	"CXX_FOR_BUILD=$(CXX_FOR_BUILD)" \
	"CXXFLAGS=$(CXXFLAGS)" \
	"CXX_FOR_TARGET=$(CXX_FOR_TARGET)" \
	"GCC_FOR_TARGET=$(CC_FOR_TARGET)" \
	"INSTALL=$(INSTALL)" \
	"INSTALL_DATA=$(INSTALL_DATA)" \
	"INSTALL_PROGRAM=$(INSTALL_PROGRAM)" \
	"INSTALL_XFORM=$(INSTALL_XFORM)" \
	"LDFLAGS=$(LDFLAGS)" \
	"LEX=$(LEX)" \
	"M4=$(M4)" \
	"MAKEINFO=$(MAKEINFO) $(MAKEINFOFLAGS)" \
	"MUNCH_NM=$(MUNCH_NM)" \
	"NM_FOR_TARGET=$(NM_FOR_TARGET)" \
	"RANLIB_FOR_TARGET=$(RANLIB_FOR_TARGET)" \
	"EXPECT=$(EXPECT)" \
	"RUNTEST=$(RUNTEST)" \
	"RUNTESTFLAGS=$(RUNTESTFLAGS)" \
	"XTRAFLAGS_FOR_TARGET=$(XTRAFLAGS)" \
	"exec_prefix=$(exec_prefix)" \
	"prefix=$(prefix)" \
	"tooldir=$(tooldir)" 

# Flags to pass down to most sub-makes, in which we're building with
# the host environment.
# If any variables are added here, they must be added to do-*, below.
EXTRA_HOST_FLAGS = \
	'AR=$(AR)' \
	'AS=$(AS)' \
	'CC=$(CC)' \
	'CXX=$(CXX)' \
	'NM=$(NM)' \
	'RANLIB=$(RANLIB)' \
	'XTRAFLAGS='

FLAGS_TO_PASS = $(BASE_FLAGS_TO_PASS) $(EXTRA_HOST_FLAGS)

# Flags that are concerned with the location of the X11 include files
# and library files
X11_FLAGS_TO_PASS = \
	"X11_INCLUDE_FLAGS=$(X11_INCLUDE_FLAGS)" \
	"X11_LIB_FLAGS=$(X11_LIB_FLAGS)" \
	"X11_LIB=$(X11_LIB)"

# Flags to pass down to makes which are built with the target environment.
# The double $ decreases the length of the command line; the variables
# are set in BASE_FLAGS_TO_PASS, and the sub-make will expand them.
# If any variables are added here, they must be added to do-*, below.
EXTRA_TARGET_FLAGS = \
	'AR=$$(AR_FOR_TARGET)' \
	'AS=$$(AS_FOR_TARGET)' \
	'CC=$$(CC_FOR_TARGET)' \
	'CXX=$$(CXX_FOR_TARGET)' \
	'NM=$$(NM_FOR_TARGET)' \
	'RANLIB=$$(RANLIB_FOR_TARGET)' \
	'XTRAFLAGS=$$(XTRAFLAGS_FOR_TARGET)'

TARGET_FLAGS_TO_PASS = $(BASE_FLAGS_TO_PASS) $(EXTRA_TARGET_FLAGS)

# Flags to pass down to gcc.  gcc builds a library, libgcc.a, so it
# unfortunately needs the native compiler and the target ar and
# ranlib.
# If any variables are added here, they must be added to do-*, below.
# The HOST_* variables are a special case, which are used for the gcc
# cross-building scheme.
HOST_CC = $(CC_FOR_BUILD)
HOST_PREFIX = 
HOST_PREFIX_1 = loser-
EXTRA_GCC_FLAGS = \
	'AR=$$(AR_FOR_TARGET)' \
	'AS=$(AS)' \
	'CC=$(CC)' \
	'CXX=$(CXX)' \
	'HOST_CC=$(CC_FOR_BUILD)' \
	'HOST_PREFIX=$(HOST_PREFIX)' \
	'HOST_PREFIX_1=$(HOST_PREFIX_1)' \
	'NM=$(NM)' \
	'RANLIB=$$(RANLIB_FOR_TARGET)' \
	'XTRAFLAGS='

GCC_FLAGS_TO_PASS = $(BASE_FLAGS_TO_PASS) $(EXTRA_GCC_FLAGS)

# This is a list of the targets for all of the modules which are compiled
# using $(FLAGS_TO_PASS).
ALL_MODULES = \
	all-autoconf \
	all-bfd \
	all-binutils \
	all-byacc \
	all-cvs \
	all-dejagnu \
	all-diff \
	all-dosutils \
	all-etc \
	all-fileutils \
	all-find \
	all-flex \
	all-gas \
	all-gawk \
	all-gdb \
	all-gprof \
	all-grep \
	all-gzip \
	all-hello \
	all-indent \
	all-ispell \
	all-ld \
	all-libiberty \
	all-m4 \
	all-make \
	all-mmalloc \
	all-opcodes \
	all-pagas \
	all-patch \
	all-prms \
	all-rcs \
	all-readline \
	all-release \
	all-recode \
	all-sed \
	all-send-pr \
	all-shellutils \
	all-sim \
	all-tar \
	all-tcl \
	all-texinfo \
	all-textutils \
	all-tgas \
	all-time \
	all-uudecode \
	all-wdiff

# This is a list of the check targets for all of the modules which are
# compiled using $(FLAGS_TO_PASS).
CHECK_MODULES = \
	check-autoconf \
	check-bfd \
	check-binutils \
	check-byacc \
	check-cvs \
	check-dejagnu \
	check-diff \
	check-etc \
	check-fileutils \
	check-find \
	check-flex \
	check-gas \
	check-gawk \
	check-gdb \
	check-gprof \
	check-grep \
	check-gzip \
	check-hello \
	check-indent \
	check-ispell \
	check-ld \
	check-libiberty \
	check-m4 \
	check-make \
	check-mmcheckoc \
	check-opcodes \
	check-pagas \
	check-patch \
	check-prms \
	check-rcs \
	check-readline \
	check-recode \
	check-sed \
	check-send-pr \
	check-shellutils \
	check-sim \
	check-tar \
	check-tcl \
	check-texinfo \
	check-textutils \
	check-tgas \
	check-time \
	check-uudecode \
	check-wdiff

# This is a list of the install targets for all of the modules which are
# compiled using $(FLAGS_TO_PASS).
INSTALL_MODULES = \
	install-autoconf \
	install-bfd \
	install-binutils \
	install-byacc \
	install-cvs \
	install-dejagnu \
	install-diff \
	install-etc \
	install-fileutils \
	install-find \
	install-flex \
	install-gas \
	install-gawk \
	install-gdb \
	install-glob \
	install-gprof \
	install-grep \
	install-gzip \
	install-hello \
	install-indent \
	install-ispell \
	install-ld \
	install-libiberty \
	install-m4 \
	install-make \
	install-mmalloc \
	install-opcodes \
	install-pagas \
	install-patch \
	install-prms \
	install-rcs \
	install-readline \
	install-recode \
	install-sed \
	install-send-pr \
	install-shellutils \
	install-sim \
	install-tar \
	install-tcl \
	install-textutils \
	install-tgas \
	install-time \
	install-uudecode \
	install-wdiff

# This is a list of the targets for all of the modules which are compiled
# using $(X11_FLAGS_TO_PASS).
ALL_X11_MODULES = \
	all-emacs \
	all-expect \
	all-gash \
	all-tclX \
	all-tk

# This is a list of the check targets for all of the modules which are
# compiled using $(X11_FLAGS_TO_PASS).
CHECK_X11_MODULES = \
	check-emacs \
	check-expect \
	check-gash \
	check-tclX \
	check-tk

# This is a list of the install targets for all the modules which are
# compiled using $(X11_FLAGS_TO_PASS).
INSTALL_X11_MODULES = \
	install-emacs \
	install-expect \
	install-gash \
	install-tclX \
	install-tk

# This is a list of the targets for all of the modules which are compiled
# using $(TARGET_FLAGS_TO_PASS).
ALL_TARGET_MODULES = \
	all-libio \
	all-libg++ \
	all-newlib \
	all-xiberty

# This is a list of the check targets for all of the modules which are
# compiled using $(TARGET_FLAGS_TO_PASS).
CHECK_TARGET_MODULES = \
	check-libio \
	check-libg++ \
	check-newlib \
	check-xiberty

# This is a list of the install targets for all of the modules which are
# compiled using $(TARGET_FLAGS_TO_PASS).
INSTALL_TARGET_MODULES = \
	install-libio \
	install-libg++ \
	install-newlib \
	install-xiberty

# This is a shell case of all modules which are compiled using
# $(TARGET_FLAGS_TO_PASS), used in the do-X rule.
TARGET_LIBS = libio | libg++ | newlib | xiberty

# The first rule in the file had better be this one.  Don't put any above it.
all: all.normal
.PHONY: all

# The target built for a native build.
.PHONY: all.normal
all.normal: \
	$(ALL_MODULES) \
	$(ALL_TARGET_MODULES) \
	$(ALL_X11_MODULES) \
	all-gcc

# Do a target for all the subdirectories.  A ``make do-X'' will do a
# ``make X'' in all subdirectories (because, in general, there is a
# dependency (below) of X upon do-X, a ``make X'' will also do this,
# but it may do additional work as well).
# This target ensures that $(BASE_FLAGS_TO_PASS) appears only once,
# because it is so large that it can easily overflow the command line
# length limit on some systems.
DO_X = \
	do-clean \
	do-distclean \
	do-dvi \
	do-info \
	do-install-info \
	do-installcheck \
	do-mostlyclean \
	do-realclean
.PHONY: $(DO_X)
$(DO_X):
	@target=`echo $@ | sed -e 's/^do-//'`; \
	rootme=`pwd`; export rootme; \
	srcroot=`cd $(srcdir); pwd`; export srcroot; \
	for i in $(SUBDIRS); do \
	  if [ -f ./$$i/Makefile ]; then \
	    case $$i in \
	    $(TARGET_LIBS) ) \
	      for flag in $(EXTRA_TARGET_FLAGS); do \
		eval `echo "$$flag" | sed -e "s|^\(.*\)=\(.*\)|\1='\2'|"`; \
	      done; \
	      ;; \
	    gcc) \
	      for flag in $(EXTRA_GCC_FLAGS); do \
		eval `echo "$$flag" | sed -e "s|^\(.*\)=\(.*\)|\1='\2'|"`; \
	      done; \
	      ;; \
	    *) \
	      for flag in $(EXTRA_HOST_FLAGS); do \
		eval `echo "$$flag" | sed -e "s|^\(.*\)=\(.*\)|\1='\2'|"`; \
	      done; \
	      ;; \
	    esac ; \
	    export AR AS CC CXX NM RANLIB XTRAFLAGS; \
	    if (cd ./$$i; \
	        $(MAKE) $(BASE_FLAGS_TO_PASS) "AR=$${AR}" "AS=$${AS}" \
			"CC=$${CC}" "CXX=$${CXX}" "NM=$${NM}" \
			"RANLIB=$${RANLIB}" "XTRAFLAGS=$${XTRAFLAGS}" \
			$${target}); \
	    then true; else exit 1; fi; \
	  else true; fi; \
	done

# Here are the targets which correspond to the do-X targets.

.PHONY: info installcheck dvi install-info
.PHONY: clean distclean mostlyclean realclean local-clean local-distclean
info: do-info
installcheck: do-installcheck
dvi: do-dvi

install-info: do-install-info dir.info
	srcroot=`cd $(srcdir); pwd`; export srcroot; \
	if [ -f dir.info ] ; then \
	  $(INSTALL_DATA) dir.info $(infodir)/dir.info ; \
	else true ; fi

local-clean:
	-rm -f *.a TEMP errs core *.o *~ \#* TAGS *.E

local-distclean:
	-rm -f Makefile config.status

clean: do-clean local-clean
mostlyclean: do-mostlyclean local-clean
distclean: do-distclean local-clean local-distclean
realclean: do-realclean local-clean local-distclean

# Check target.

.PHONY: check
check: $(CHECK_MODULES) \
	$(CHECK_TARGET_MODULES) \
	$(CHECK_X11_MODULES) \
	check-gcc

# Installation targets.

.PHONY: install uninstall vault-install
install: $(INSTALL_TARGET) 

uninstall:
	@echo "the uninstall target is not supported in this tree"

vault-install:
	@if [ -f ./release/vault-install ] ; then \
	  ./release/vault-install $(host_alias) $(target_alias) ; \
	else \
	  true ; \
	fi

.PHONY: install.all
install.all: install-no-fixedincludes
	@if [ -f ./gcc/Makefile ] ; then \
		rootme=`pwd` ; export rootme ; \
		(cd ./gcc; \
		$(MAKE) $(FLAGS_TO_PASS) install-headers) ; \
	else \
		true ; \
	fi

# install-no-fixedincludes is used because Cygnus can not distribute
# the fixed header files.
.PHONY: install-no-fixedincludes
install-no-fixedincludes: \
	install-dirs \
	$(INSTALL_MODULES) \
	$(INSTALL_TARGET_MODULES) \
	$(INSTALL_X11_MODULES) \
	gcc-no-fixedincludes 

# Install the gcc headers files, but not the fixed include files,
# which Cygnus is not allowed to distribute.  This rule is very
# dependent on the workings of the gcc Makefile.in.
.PHONY: gcc-no-fixedincludes
gcc-no-fixedincludes:
	@if [ -f ./gcc/Makefile ]; then \
	  rm -rf gcc/tmp-include; \
	  mv gcc/include gcc/tmp-include 2>/dev/null; \
	  mkdir gcc/include; \
	  cp $(srcdir)/gcc/gsyslimits.h gcc/include/syslimits.h; \
	  touch gcc/stmp-fixinc gcc/stmp-fixproto; \
	  rm -f gcc/stmp-headers gcc/stmp-int-hdrs; \
	  rootme=`pwd`; export rootme; \
	  srcroot=`cd $(srcdir); pwd` ; export srcroot; \
	  (cd ./gcc; \
	   $(MAKE) $(GCC_FLAGS_TO_PASS) install); \
	  rm -rf gcc/include; \
	  mv gcc/tmp-include gcc/include 2>/dev/null; \
	else true; fi

# This rule is used to build the modules which use FLAGS_TO_PASS.  To
# build a target all-X means to cd to X and make all.
# all-glob is handled specially because it doesn't actually build.
.PHONY: $(ALL_MODULES) all-glob
$(ALL_MODULES) all-glob:
	@dir=`echo $@ | sed -e 's/all-//'`; \
	if [ -f ./$${dir}/Makefile ] ; then \
	  rootme=`pwd`; export rootme; \
	  srcroot=`cd $(srcdir); pwd`; export srcroot; \
	  (cd $${dir}; $(MAKE) $(FLAGS_TO_PASS) all); \
	else \
	  true; \
	fi

# This rule is used to check the modules which use FLAGS_TO_PASS.  To
# build a target check-X means to cd to X and make all.
.PHONY: $(CHECK_MODULES) 
$(CHECK_MODULES):
	@dir=`echo $@ | sed -e 's/check-//'`; \
	if [ -f ./$${dir}/Makefile ] ; then \
	  rootme=`pwd`; export rootme; \
	  srcroot=`cd $(srcdir); pwd`; export srcroot; \
	  (cd $${dir}; $(MAKE) $(FLAGS_TO_PASS) check); \
	else \
	  true; \
	fi

# This rule is used to install the modules which use FLAGS_TO_PASS.
# To build a target install-X means to cd to X and make install.
.PHONY: $(INSTALL_MODULES)
$(INSTALL_MODULES): install-dirs
	@dir=`echo $@ | sed -e 's/install-//'`; \
	if [ -f ./$${dir}/Makefile ] ; then \
	  rootme=`pwd`; export rootme; \
	  srcroot=`cd $(srcdir); pwd`; export srcroot; \
	  (cd $${dir}; $(MAKE) $(FLAGS_TO_PASS) install); \
	else \
	  true; \
	fi

# This rule is used to build the modules which use TARGET_FLAGS_TO_PASS.
# To build a target all-X means to cd to X and make all.
.PHONY: $(ALL_TARGET_MODULES)
$(ALL_TARGET_MODULES):
	@dir=`echo $@ | sed -e 's/all-//'`; \
	if [ -f ./$${dir}/Makefile ] ; then \
	  rootme=`pwd`; export rootme; \
	  srcroot=`cd $(srcdir); pwd`; export srcroot; \
	  (cd $${dir}; $(MAKE) $(TARGET_FLAGS_TO_PASS) all); \
	else \
	  true; \
	fi

# This rule is used to check the modules which use TARGET_FLAGS_TO_PASS.
# To build a target install-X means to cd to X and make install.
.PHONY: $(CHECK_TARGET_MODULES)
$(CHECK_TARGET_MODULES):
	@dir=`echo $@ | sed -e 's/check-//'`; \
	if [ -f ./$${dir}/Makefile ] ; then \
	  rootme=`pwd`; export rootme; \
	  srcroot=`cd $(srcdir); pwd`; export srcroot; \
	  (cd $${dir}; $(MAKE) $(TARGET_FLAGS_TO_PASS) check); \
	else \
	  true; \
	fi

# This rule is used to install the modules which use
# TARGET_FLAGS_TO_PASS.  To build a target install-X means to cd to X
# and make install.
.PHONY: $(INSTALL_TARGET_MODULES)
$(INSTALL_TARGET_MODULES): install-dirs
	@dir=`echo $@ | sed -e 's/install-//'`; \
	if [ -f ./$${dir}/Makefile ] ; then \
	  rootme=`pwd`; export rootme; \
	  srcroot=`cd $(srcdir); pwd`; export srcroot; \
	  (cd $${dir}; $(MAKE) $(TARGET_FLAGS_TO_PASS) install); \
	else \
	  true; \
	fi

# This rule is used to build the modules which use X11_FLAGS_TO_PASS.
# To build a target all-X means to cd to X and make all.
.PHONY: $(ALL_X11_MODULES)
$(ALL_X11_MODULES):
	@dir=`echo $@ | sed -e 's/all-//'`; \
	if [ -f ./$${dir}/Makefile ] ; then \
	  rootme=`pwd`; export rootme; \
	  srcroot=`cd $(srcdir); pwd`; export srcroot; \
	  (cd $${dir}; \
	   $(MAKE) $(FLAGS_TO_PASS) $(X11_FLAGS_TO_PASS) all); \
	else \
	  true; \
	fi

# This rule is used to check the modules which use X11_FLAGS_TO_PASS.
# To build a target check-X means to cd to X and make all.
.PHONY: $(CHECK_X11_MODULES)
$(CHECK_X11_MODULES):
	@dir=`echo $@ | sed -e 's/check-//'`; \
	if [ -f ./$${dir}/Makefile ] ; then \
	  rootme=`pwd`; export rootme; \
	  srcroot=`cd $(srcdir); pwd`; export srcroot; \
	  (cd $${dir}; \
	   $(MAKE) $(FLAGS_TO_PASS) $(X11_FLAGS_TO_PASS) check); \
	else \
	  true; \
	fi

# This rule is used to install the modules which use X11_FLAGS_TO_PASS.
# To build a target install-X means to cd to X and make install.
.PHONY: $(INSTALL_X11_MODULES)
$(INSTALL_X11_MODULES):
	@dir=`echo $@ | sed -e 's/install-//'`; \
	if [ -f ./$${dir}/Makefile ] ; then \
	  rootme=`pwd`; export rootme; \
	  srcroot=`cd $(srcdir); pwd`; export srcroot; \
	  (cd $${dir}; \
	   $(MAKE) $(FLAGS_TO_PASS) $(X11_FLAGS_TO_PASS) install); \
	else \
	  true; \
	fi

# gcc is the only module which uses GCC_FLAGS_TO_PASS.
.PHONY: all-gcc
all-gcc:
	@if [ -f ./gcc/Makefile ] ; then \
	  rootme=`pwd`; export rootme; \
	  srcroot=`cd $(srcdir); pwd`; export srcroot; \
	  (cd gcc; $(MAKE) $(GCC_FLAGS_TO_PASS) all); \
	else \
	  true; \
	fi

.PHONY: check-gcc
check-gcc:
	@if [ -f ./gcc/Makefile ] ; then \
	  rootme=`pwd`; export rootme; \
	  srcroot=`cd $(srcdir); pwd`; export srcroot; \
	  (cd gcc; $(MAKE) $(GCC_FLAGS_TO_PASS) check); \
	else \
	  true; \
	fi

.PHONY: install-gcc
install-gcc:
	@if [ -f ./gcc/Makefile ] ; then \
	  rootme=`pwd`; export rootme; \
	  srcroot=`cd $(srcdir); pwd`; export srcroot; \
	  (cd gcc; $(MAKE) $(GCC_FLAGS_TO_PASS) install); \
	else \
	  true; \
	fi

# This is a list of inter-dependencies among modules.
all-autoconf: all-m4
all-bfd:
all-binutils: all-libiberty all-opcodes all-bfd all-flex
all-byacc:
all-cvs:
all-dejagnu:
all-diff: all-libiberty
all-emacs:
all-etc:
all-expect: all-tcl all-tk
all-fileutils: all-libiberty
all-find:
all-flex: all-libiberty all-byacc
all-gas: all-libiberty all-opcodes all-bfd
all-gash: all-tcl
all-gawk:
all-gcc: all-libiberty all-byacc all-binutils all-gas all-pagas
all-gdb: all-libiberty all-opcodes all-bfd all-mmalloc all-readline all-glob all-byacc all-sim
all-glob:
all-gprof: all-libiberty all-bfd
all-grep: all-libiberty
all-gzip: all-libiberty
all-hello: all-libiberty
all-indent:
all-ispell: all-emacs
all-ld: all-libiberty all-bfd all-byacc all-flex
all-libg++: all-gas all-pagas all-ld all-gcc all-xiberty all-newlib all-libio
all-libio: all-gas all-pagas all-ld all-gcc all-xiberty all-newlib 
all-libiberty:
all-m4: all-libiberty
all-make: all-libiberty
all-mmalloc:
all-newlib: all-binutils all-gas all-pagas all-gcc
all-opcodes: all-bfd
all-patch:
all-prms: all-libiberty
all-rcs:
all-readline:
all-recode: all-libiberty
all-sed: all-libiberty
all-send-pr: all-prms
all-shellutils:
all-sim: all-libiberty all-bfd
all-tar: all-libiberty
all-tcl:
all-tclX: all-tcl all-tk
all-tk: all-tcl
all-texinfo: all-libiberty
all-textutils:
all-tgas: all-libiberty all-bfd
all-time:
all-wdiff:
all-uudecode: all-libiberty
all-xiberty: all-gcc all-newlib

### other supporting targets

MAKEDIRS= \
	$(prefix) \
	$(exec_prefix) \
	$(tooldir)

.PHONY: install-dirs
install-dirs:
	@for i in $(MAKEDIRS) ; do \
		echo Making $$i... ; \
		parent=`echo $$i | sed -e 's@/[^/]*$$@@' | sed -e 's@^$$@/@'`; \
		if [ -d $$parent ] ; then true ; else mkdir $$parent ; fi ; \
		if [ ! -d $$i ] ; then \
			if mkdir $$i ; then \
				true ; \
			else \
				exit 1 ; \
			fi ; \
		else \
			true ; \
		fi ; \
	done


dir.info: do-install-info
	if [ -f $(srcdir)/texinfo/gen-info-dir ] ; then \
	  $(srcdir)/texinfo/gen-info-dir $(infodir) $(srcdir)/texinfo/dir.info-template > dir.info.new ; \
	  mv -f dir.info.new dir.info ; \
	else true ; \
	fi

dist:
	@echo "Building a full distribution of this tree isn't done"
	@echo "via 'make dist'.  Check out the etc/ subdirectory" 

etags tags: TAGS

TAGS:
	etags `$(MAKE) ls`

ls:
	@echo Makefile
	@for i in $(SUBDIRS); \
	do \
		(cd $$i; \
			pwd=`pwd`; \
			wd=`basename $$pwd`; \
			for j in `$(MAKE) ls`; \
			do \
				echo $$wd/$$j; \
			done) \
	done

# with the gnu make, this is done automatically.

Makefile: Makefile.in configure.in $(host_makefile_frag) $(target_makefile_frag)
	$(SHELL) ./config.status

#
# Support for building net releases

# Files in devo used in any net release.
# ChangeLog omitted because it may refer to files which are not in this
# distribution (perhaps it would be better to include it anyway).
DEVO_SUPPORT= README Makefile.in configure configure.in \
	config.guess config.sub config move-if-change \
	COPYING COPYING.LIB install.sh

# Files in devo/etc used in any net release.
# ChangeLog omitted because it may refer to files which are not in this
# distribution (perhaps it would be better to include it anyway).
ETC_SUPPORT= Makefile.in cfg-paper.texi configure.in configure.man \
	configure.texi standards.texi make-stds.texi

GDB_SUPPORT_DIRS= bfd include libiberty mmalloc opcodes readline glob sim
GDB_SUPPORT_FILES= $(GDB_SUPPORT_DIRS)

.PHONY: setup-dirs-gdb gdb.tar.gz make-gdb.tar.gz
setup-dirs-gdb:
	./configure sun4
	$(MAKE) clean
	./configure -rm sun4
	chmod og=u `find etc $(DEVO_SUPPORT) $(GDB_SUPPORT_FILES) -print`

gdb.tar.gz: setup-dirs-gdb
	(cd gdb; $(MAKE) -f Makefile.in make-proto-gdb.dir BISON="byacc")
	(cd gdb; $(MAKE) -f Makefile.in make-proto-testsuite.dir)
	$(MAKE) $(MFLAGS) -f Makefile.in make-gdb.tar.gz

make-gdb.tar.gz: $(DEVO_SUPPORT) $(GDB_SUPPORT_DIRS) gdb texinfo/texinfo.tex
	rm -rf proto-toplev; mkdir proto-toplev
	ln -s ../gdb/proto-gdb.dir proto-toplev/gdb
	(cd proto-toplev; for i in $(DEVO_SUPPORT) $(GDB_SUPPORT_DIRS); do \
		ln -s ../$$i . ; \
	done)
	mkdir proto-toplev/etc
	(cd proto-toplev/etc; for i in $(ETC_SUPPORT); do \
		ln -s ../../etc/$$i . ; \
	done)
	# Put only one copy (four hard links) of COPYING in the tar file.
	rm                          proto-toplev/bfd/COPYING
	ln proto-toplev/gdb/COPYING proto-toplev/bfd/COPYING
	rm                          proto-toplev/include/COPYING
	ln proto-toplev/gdb/COPYING proto-toplev/include/COPYING
	rm                          proto-toplev/readline/COPYING
	ln proto-toplev/gdb/COPYING proto-toplev/readline/COPYING

	# Change the bug reporting address in configure to bug-gdb
	rm proto-toplev/configure
	sed -e 's/configure@cygnus.com/bug-gdb@prep.ai.mit.edu/' \
	    <configure >proto-toplev/configure
	chmod a+x proto-toplev/configure

	# Take out texinfo and glob from configurable dirs
	rm proto-toplev/configure.in
	sed -e '/^host_tools=/s/texinfo //' \
	    -e '/^host_libs=/s/glob //' \
	    <configure.in >proto-toplev/configure.in

	# Take out texinfo from a few places; make simple BISON=bison line.
	rm proto-toplev/Makefile.in
	sed -e '/^all\.normal: /s/\all-texinfo //' \
	    -e '/^	install-texinfo /d' \
	    -e '/^BISON = /,/^$$/d' \
	    -e '/^# BISON:/s/.*/BISON = bison -y/' \
	<Makefile.in >proto-toplev/Makefile.in

	mkdir proto-toplev/texinfo
	ln -s ../../texinfo/texinfo.tex proto-toplev/texinfo/
	ln -s ../../texinfo/gpl.texinfo proto-toplev/texinfo/
	ln -s ../../texinfo/tex3patch   proto-toplev/texinfo/
	chmod og=u `find proto-toplev -print`
	(VER=`sed <gdb/Makefile.in -n 's/VERSION = //p'`; \
		echo "==> Making gdb-$$VER.tar.gz"; \
		rm -f gdb-$$VER; ln -s proto-toplev gdb-$$VER; \
		tar cfh - gdb-$$VER \
		| $(GZIP) -v -9 >gdb-$$VER.tar.gz)

	# Make the testsuite archive separately.
	ln -s ../../gdb/proto-testsuite.dir/testsuite proto-toplev/gdb/testsuite
	# Blow away the Chill test that requires a Chill compiled executable,
	# since GNU Chill is not yet publically available.
	rm -rf proto-toplev/gdb/testsuite/gdb.t31

	# Put a copy of COPYING in the tar file.
	ln proto-toplev/gdb/COPYING proto-toplev/gdb/testsuite/COPYING
	chmod og=u `find proto-toplev/gdb/testsuite -print`
	(VER=`sed <gdb/Makefile.in -n 's/VERSION = //p'`; \
		echo "==> Making gdb-$$VER-testsuite.tar.gz"; \
		tar cfh - gdb-$$VER/configure gdb-$$VER/config.guess \
			gdb-$$VER/config.sub gdb-$$VER/move-if-change \
			gdb-$$VER/gdb/testsuite \
			| $(GZIP) -v -9 >gdb-$$VER-testsuite.tar.gz)

# When you use `make setup-dirs' or `make taz' you should always redefine
# this macro.
SUPPORT_FILES = list-of-support-files-for-tool-in-question
# Directories that might want `make diststuff' run.
DISTSTUFFDIRS= ld gprof gdb libg++ binutils
# Files where "byacc" (Cygnus version) should be changed to "bison -y" (FSF).
DISTBISONFILES= binutils/Makefile.in gas/Makefile.in gdb/Makefile.in Makefile.in
# Directories where "info" should be built.
DISTDOCDIRS= ld gprof binutils gdb libg++ gas bfd

.PHONY: taz

taz: $(DEVO_SUPPORT) $(SUPPORT_FILES) \
  texinfo/texinfo.tex texinfo/gpl.texinfo texinfo/lgpl.texinfo
	# Make sure "diststuff" files get built properly.
	for f in $(DISTBISONFILES) ; do \
	  if [ -r $$f ]; then \
	    sed '/^BISON *=.*$$/s/.*/BISON = bison -y/' <$$f >tmp ; \
	    mv -f tmp $$f ; \
	  else true; fi ; \
	done
	# Take out texinfo from a few places; make simple BISON=bison line.
	sed -e '/^all\.normal: /s/\all-texinfo //' \
	    -e '/^	install-texinfo /d' \
	<Makefile.in >tmp
	mv -f tmp Makefile.in
	#
	./configure sun4
	# Doc files don't change; include them in distribution.
	for f in $(DISTDOCDIRS) ; do \
	  if [ -r $$f/Makefile ]; then \
	    (cd $$f ; $(MAKE) info) || exit 1 ; \
	  else true ; fi ; \
	done
	# Make links, and run "make diststuff" when needed.
	# The `echo' for setting `p' is to convert all whitespace to spaces.
	# Then the `case' further below should tell whether $$d is in
	# DISTSTUFFDIRS.
	rm -rf proto-toplev ; mkdir proto-toplev
	set -e ; dirs="$(TOOL) $(DEVO_SUPPORT) $(SUPPORT_FILES)" ; \
	p=" `echo $(DISTSTUFFDIRS)` " ; \
	for d in $$dirs ; do \
	  if [ -d $$d ]; then \
	    case " $$p " in \
	    *" $$d "*)	\
		echo making diststuff in $$d ; \
		(cd $$d ; pwd ; $(MAKE) diststuff ) || exit 1  ;; \
	    esac ; \
	    if [ -d $$d/proto-$$d.dir ]; then \
	      ln -s ../$$d/proto-$$d.dir proto-toplev/$$d ; \
	    else \
	      ln -s ../$$d proto-toplev/$$d ; \
	    fi ; \
	  else ln -s ../$$d proto-toplev/$$d ; fi ; \
	done
	$(MAKE) distclean
	#
	mkdir proto-toplev/etc
	(cd proto-toplev/etc; for i in $(ETC_SUPPORT); do \
		ln -s ../../etc/$$i . ; \
	done)
	#
	# Take out texinfo and glob from configurable dirs
	rm proto-toplev/configure.in
	sed -e '/^host_tools=/s/texinfo //' \
	    -e '/^host_libs=/s/glob //' \
	    <configure.in >proto-toplev/configure.in
	#
	mkdir proto-toplev/texinfo
	ln -s ../../texinfo/texinfo.tex proto-toplev/texinfo/
	ln -s ../../texinfo/gpl.texinfo proto-toplev/texinfo/
	ln -s ../../texinfo/lgpl.texinfo proto-toplev/texinfo/
	ln -s ../../texinfo/tex3patch   proto-toplev/texinfo/
	chmod og=u `find . -print`
	(VER=`sed <$(TOOL)/Makefile.in -n 's/^VERSION *= *//p'`; \
		echo "==> Making $(TOOL)-$$VER.tar.gz"; \
		rm -f $(TOOL)-$$VER; ln -s proto-toplev $(TOOL)-$$VER; \
		tar cfh - $(TOOL)-$$VER \
		| $(GZIP) -v -9 >$(TOOL)-$$VER.tar.gz)

TEXINFO_SUPPORT= texinfo/texinfo.tex texinfo/gpl.texinfo texinfo/lgpl.texinfo
DIST_SUPPORT= $(DEVO_SUPPORT) $(TEXINFO_SUPPORT)

.PHONY: gas.tar.gz
GAS_SUPPORT_DIRS= bfd include libiberty opcodes
gas.tar.gz: $(DIST_SUPPORT) $(GAS_SUPPORT_DIRS) gas
	$(MAKE) -f Makefile.in taz TOOL=gas \
		SUPPORT_FILES="$(GAS_SUPPORT_DIRS)"

# The FSF "binutils" release includes gprof and ld.
.PHONY: binutils.tar.gz
BINUTILS_SUPPORT_DIRS= bfd include libiberty opcodes ld gprof
binutils.tar.gz: $(DIST_SUPPORT) $(BINUTILS_SUPPORT_DIRS) binutils
	$(MAKE) -f Makefile.in taz TOOL=binutils \
		SUPPORT_FILES="$(BINUTILS_SUPPORT_DIRS)"

.PHONY: gas+binutils.tar.gz
GASB_SUPPORT_DIRS= $(GAS_SUPPORT_DIRS) binutils ld gprof
gas+binutils.tar.gz: $(DIST_SUPPORT) $(GASB_SUPPORT_DIRS) gas
	$(MAKE) -f Makefile.in taz TOOL=gas \
		SUPPORT_FILES="$(GASB_SUPPORT_DIRS)"

.PHONY: libg++.tar.gz
LIBGXX_SUPPORT_DIRS=include libio libiberty xiberty
libg++.tar.gz: $(DIST_SUPPORT) libg++
	$(MAKE) -f Makefile.in taz TOOL=libg++ \
		SUPPORT_FILES="$(LIBGXX_SUPPORT_DIRS)"

.NOEXPORT:
MAKEOVERRIDES=


# end of Makefile.in
