BFDDEF=-DBFD_ASSEMBLER
BFDLIB=../bfd/libbfd.a

# This file was generated automatically by configure.  Do not edit.
VPATH = .
links = host.h targ-cpu.c targ-cpu.h targ-env.h obj-format.h obj-format.c atof-targ.c
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
# Makefile for GNU Assembler
#   Copyright (C) 1987-1992 Free Software Foundation, Inc.

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
srcroot = $(srcdir)/..

prefix = /opt

program_transform_name = 
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
libdir = $(exec_prefix)/lib
tooldir = $(exec_prefix)/i386-unknown-sysv4.2

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

VERSION=2.2

SHELL = /bin/sh

INSTALL = $${srcroot}/install.sh -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL)
INSTALL_XFORM = $(INSTALL) -t='$(program_transform_name)'
INSTALL_XFORM1= $(INSTALL_XFORM) -b=.1

AR = ar
AR_FLAGS = qv
BISON = bison -y
MAKEINFO = makeinfo
TEXI2DVI = texi2dvi
RANLIB = ranlib
CFLAGS = -g

MAKEOVERRIDES=

AS_FOR_TARGET = $${here}/as.new

CC_FOR_TARGET = ` \
  if [ -f $${here}/../gcc/Makefile ] ; then \
    echo $${here}/../gcc/xgcc -B$${here}/../gcc/; \
  else \
    if [ "$(host_canonical)" = "$(target_canonical)" ] ; then \
      echo $(CC); \
    else \
      t='$(program_transform_name)'; echo gcc | sed -e '' $$t; \
    fi; \
  fi`

NM_FOR_TARGET = ` \
  if [ -f $${here}/../binutils/Makefile ] ; then \
    echo $${here}/../binutils/nm ; \
  else \
    if [ "$(host_canonical)" = "$(target_canonical)" ] ; then \
      echo $(NM); \
    else \
       t='$(program_transform_name)'; echo nm | sed -e '' $$t ; \
    fi; \
  fi`

OBJDUMP=objdump
OBJDUMP_FOR_TARGET = ` \
  if [ -f $${here}/../binutils/Makefile ] ; then \
    echo $${here}/../binutils/objdump ; \
  else \
    if [ "$(host_canonical)" = "$(target_canonical)" ] ; then \
      echo $(OBJDUMP); \
    else \
       t='$(program_transform_name)'; echo objdump | sed -e '' $$t ; \
    fi; \
  fi`

FLAGS_TO_PASS = \
	"prefix=$(prefix)" \
	"exec_prefix=$(exec_prefix)" \
	"tooldir=$(tooldir)" \
	"AR=$(AR)" \
	"AR_FLAGS=$(AR_FLAGS)" \
	"CC=$(CC)" \
	"CFLAGS=$(CFLAGS)" \
	"RANLIB=$(RANLIB)" \
	"LOADLIBES=$(LOADLIBES)" \
	"LDFLAGS=$(LDFLAGS)" \
	"BISON=$(BISON)" \
	"LEX=$(LEX)" \
	"MAKEINFO=$(MAKEINFO)" \
	"INSTALL=$(INSTALL)" \
	"INSTALL_DATA=$(INSTALL_DATA)" \
	"INSTALL_PROGRAM=$(INSTALL_PROGRAM)"

CHECKFLAGS= \
	"AS_FOR_TARGET=$(AS_FOR_TARGET)" \
	"CC_FOR_TARGET=$(CC_FOR_TARGET)" \
	"NM_FOR_TARGET=$(NM_FOR_TARGET)" \
	"OBJDUMP_FOR_TARGET=$(OBJDUMP_FOR_TARGET)" \
	"RUNTESTFLAGS=$(RUNTESTFLAGS)"

# Lists of files for various purposes.

REAL_SOURCES = \
	$(srcdir)/app.c \
	$(srcdir)/as.c \
	$(srcdir)/atof-generic.c \
	$(srcdir)/bignum-copy.c \
	$(srcdir)/cond.c \
	$(srcdir)/expr.c \
	$(srcdir)/flonum-konst.c \
	$(srcdir)/flonum-copy.c \
	$(srcdir)/flonum-mult.c \
	$(srcdir)/frags.c \
	$(srcdir)/hash.c \
	$(srcdir)/hex-value.c \
	$(srcdir)/input-file.c \
	$(srcdir)/input-scrub.c \
	$(srcdir)/messages.c \
	$(srcdir)/output-file.c \
	$(srcdir)/read.c \
	$(srcdir)/subsegs.c \
	$(srcdir)/symbols.c \
	$(srcdir)/write.c \
	$(srcdir)/listing.c \
	$(srcdir)/xmalloc.c

# in an expedient order
LINKED_SOURCES = \
	targ-cpu.c \
	obj-format.c \
	atof-targ.c

SOURCES = $(LINKED_SOURCES) $(REAL_SOURCES)

REAL_HEADERS = \
	$(srcdir)/as.h \
	$(srcdir)/bignum.h \
	$(srcdir)/expr.h \
	$(srcdir)/flonum.h \
	$(srcdir)/frags.h \
	$(srcdir)/hash.h \
	$(srcdir)/input-file.h \
	$(srcdir)/listing.h \
	$(srcdir)/tc.h \
	$(srcdir)/obj.h \
	$(srcdir)/read.h \
	$(srcdir)/struc-symbol.h \
	$(srcdir)/subsegs.h \
	$(srcdir)/symbols.h \
	$(srcdir)/write.h

LINKED_HEADERS = \
	a.out.gnu.h \
	a.out.h \
	host.h \
	targ-env.h \
	targ-cpu.h \
	obj-format.h \
	atof-targ.h

HEADERS = $(LINKED_HEADERS) $(REAL_HEADERS)

OBJS = \
	targ-cpu.o \
	obj-format.o \
	atof-targ.o \
	app.o \
	as.o \
	atof-generic.o \
	bignum-copy.o \
	cond.o \
	expr.o \
	flonum-konst.o \
	flonum-copy.o \
	flonum-mult.o \
	frags.o \
	hash.o \
	hex-value.o \
	input-file.o \
	input-scrub.o \
	messages.o \
	output-file.o \
	read.o \
	subsegs.o \
	symbols.o \
	write.o \
	listing.o \
	xmalloc.o

#### host, target, and site specific Makefile frags come in here.

all: as.new
	@srcroot=`cd $(srcroot); pwd`; export srcroot; \
	(cd doc ; $(MAKE) $(FLAGS_TO_PASS) all)

dvi info install-info clean-info:
	@srcroot=`cd $(srcroot); pwd`; export srcroot; \
	(cd doc ; $(MAKE) $(FLAGS_TO_PASS) $@)

# Now figure out from those variables how to compile and link.

# This is the variable actually used when we compile.
ALL_CFLAGS = $(INTERNAL_CFLAGS) $(CROSS) $(CFLAGS) $(HDEFINES) $(TDEFINES) \
	$(BFDDEF)

# How to link with both our special library facilities
# and the system's installed libraries.

LIBS = ../opcodes/libopcodes.a $(BFDLIB) $(LOCAL_LOADLIBES) \
	../libiberty/libiberty.a

# Specify the directories to be searched for header files.
# Both . and srcdir are used, in that order,
# so that tm.h and config.h will be found in the compilation
# subdirectory rather than in the source directory.
INCLUDES = -I. -I$(srcdir) -I../bfd -I$(srcdir)/config -I$(srcdir)/../include
SUBDIR_INCLUDES = -I.. -I$(srcdir) -I$(srcdir)/config

# Always use -I$(srcdir)/config when compiling.
.c.o:
	$(CC) -c $(ALL_CFLAGS) $(CPPFLAGS) $(INCLUDES) $<

# This tells GNU make version 3 not to export all the variables
# defined in this file into the environment.
.NOEXPORT:

# Files to be copied away after each stage in building.
STAGESTUFF = *.o as.new

as.new: $(OBJS) $(LIBS)
	$(CC) $(ALL_CFLAGS) $(LDFLAGS) -o as.new $(OBJS) $(LIBS) $(LOADLIBES)

installcheck:
	@echo No installcheck target is available yet for the GNU assembler.

check: as.new
	@(here=`pwd` ; export here ; \
	  cd testsuite ; $(MAKE) $(FLAGS_TO_PASS) $(CHECKFLAGS) check)

config.status:
	@echo You must configure gas.  Look at the INSTALL file for details.
	@false

config.h: config-stamp ; @true
config-stamp: Makefile
	-rm -f config.new config-stamp
	echo '#define TARGET_CPU       "$(target_cpu)"'        > config.new
	echo '#define TARGET_ALIAS     "$(target_alias)"'     >> config.new
	echo '#define TARGET_CANONICAL "$(target_canonical)"' >> config.new
	echo '#define GAS_VERSION      "$(VERSION)"'          >> config.new
	$(srcdir)/../move-if-change config.new config.h
	touch config-stamp

# Compiling object files from source files.

app.o : app.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h 
as.o : as.c as.h host.h targ-env.h obj-format.h output-file.h \
  targ-cpu.h struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h subsegs.h \
  tc.h obj.h config.h
atof-generic.o : atof-generic.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h 
bignum-copy.o : bignum-copy.c as.h host.h \
  targ-env.h obj-format.h \
  targ-cpu.h struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h 
cond.o : cond.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h

debug.o : debug.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h \
  subsegs.h 
expr.o : expr.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h

flonum-konst.o : flonum-konst.c flonum.h bignum.h 
flonum-copy.o : flonum-copy.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h 
flonum-mult.o : flonum-mult.c flonum.h bignum.h 
frags.o : frags.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h \
  subsegs.h  
hash.o : hash.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h 
hex-value.o : hex-value.c 
input-file.o : input-file.c as.h host.h \
   targ-env.h obj-format.h targ-cpu.h \
   struc-symbol.h write.h flonum.h bignum.h expr.h \
  frags.h hash.h read.h symbols.h tc.h obj.h input-file.h 
input-scrub.o : input-scrub.c /usr/include/errno.h /usr/include/sys/errno.h \
  as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h \
  input-file.h 
listing.o : listing.c as.h host.h targ-env.h flonum.h bignum.h \
  listing.h obj-format.h targ-cpu.h struc-symbol.h write.h expr.h \
  frags.h hash.h read.h symbols.h tc.h obj.h input-file.h
messages.o : messages.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h
output-file.o : output-file.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h \
  output-file.h 
read.o : read.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h
subsegs.o : subsegs.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h \
  subsegs.h  
symbols.o : symbols.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h \
   subsegs.h 
write.o : write.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h  struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h symbols.h tc.h obj.h \
  subsegs.h  output-file.h 
xmalloc.o : xmalloc.c
atof-targ.o : atof-targ.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h \
  symbols.h tc.h obj.h 
obj-format.o : obj-format.c as.h host.h targ-env.h obj-format.h \
  targ-cpu.h struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h \
  subsegs.h symbols.h tc.h obj.h  
targ-cpu.o : targ-cpu.c config.h targ-env.h obj-format.h \
  targ-cpu.h struc-symbol.h \
  write.h flonum.h bignum.h expr.h frags.h hash.h read.h \
  symbols.h tc.h obj.h $(TARG_CPU_DEPENDENTS)

# Remake the info files.

doc: $(srcdir)/as.info

$(srcdir)/as.info: $(srcdir)/doc/as.texinfo
	@(cd doc; $(MAKE) $(FLAGS_TO_PASS) as.info; mv as.info $srcdir)	

clean-here:
	-rm -f $(STAGESTUFF) core

clean:
	@cd doc ; $(MAKE) $(FLAGS_TO_PASS) clean
	@if [ -d testsuite ] ; then \
	  cd testsuite ; $(MAKE) $(FLAGS_TO_PASS) clean ; \
	else true; fi

# Like clean but also delete the links made to configure gas.
distclean: clean-here
	@cd doc ; $(MAKE) $(FLAGS_TO_PASS) distclean
	@if [ -d testsuite ] ; then \
	  cd testsuite ; $(MAKE) $(FLAGS_TO_PASS) distclean ; \
	else true; fi
	-rm -f config.status Makefile host.h targ-env.h targ-cpu.h \
		targ-cpu.c obj-format.h obj-format.c atof-targ.c TAGS

# Entry points `install', `includes' and `uninstall'.

# Copy the files into directories where they will be run.
install:
	srcroot=`cd $(srcroot); pwd`; export srcroot; \
	$(INSTALL_XFORM) as.new $(bindir)/as; \
	$(INSTALL_XFORM1) $(srcdir)/doc/as.1 $(man1dir)/as.1; \
	n=`t='$(program_transform_name)'; echo as | sed -e "" $$t`; \
	if [ -d $(tooldir) ]; then \
	  if [ -d $(tooldir)/bin ] ; then true ; else mkdir $(tooldir)/bin ; fi; \
	  rm -f $(tooldir)/bin/as; \
	  ln $(bindir)/$$n $(tooldir)/bin/as \
	   || $(INSTALL_PROGRAM) as.new $(tooldir)/bin/as; \
	else true; fi

# Cancel installation by deleting the installed files.
uninstall:
	-n=`t='$(program_transform_name)'; echo as | sed -e "" $$t`; \
	rm -f $(bindir)/$$n; \
	rm -f $(mandir)/$$n.1

# These exist for maintenance purposes.

tags TAGS: force
	etags $(REAL_HEADERS) $(REAL_SOURCES) $(srcdir)/config/*.[hc] $(srcdir)/README $(srcdir)/Makefile.in
	
bootstrap: as.new force
	$(MAKE) stage1
	$(MAKE) CC="$(CC)" CFLAGS="-O -Bstage1/ $(CFLAGS)" libdir=$(libdir) ALLOCA= as.new
	$(MAKE) stage2
	$(MAKE) CC="$(CC)" CFLAGS="-O -Bstage2/ $(CFLAGS)" libdir=$(libdir) ALLOCA= as.new
	$(MAKE) comparison against=stage2

bootstrap2: force
	$(MAKE) CC="$(CC)" CFLAGS="-O -Bstage1/ $(CFLAGS)" libdir=$(libdir) ALLOCA= as.new
	$(MAKE) stage2
	$(MAKE) CC="$(CC)" CFLAGS="-O -Bstage2/ $(CFLAGS)" libdir=$(libdir) ALLOCA= as.new
	$(MAKE) comparison against=stage2

bootstrap3: force
	$(MAKE) CC="$(CC)" CFLAGS="-O -Bstage2/ $(CFLAGS)" libdir=$(libdir) ALLOCA= as.new
	$(MAKE) comparison against=stage2

# Copy the object files from a particular stage into a subdirectory.
stage1: force
	-mkdir stage1
	-mv $(STAGESTUFF) stage1
	if [ -f stage1/as.new -a ! -f stage1/as ] ; then (cd stage1 ; ln -s as.new as) ; fi

stage2: force
	-mkdir stage2
	-mv $(STAGESTUFF) stage2
	if [ -f stage2/as.new -a ! -f stage2/as ] ; then (cd stage2 ; ln -s as.new as) ; fi

stage3: force
	-mkdir stage3
	-mv $(STAGESTUFF) stage3
	if [ -f stage3/as.new -a ! -f stage3/as ] ; then (cd stage3 ; ln -s as.new as) ; fi

against=stage2

comparison: force
	for i in $(STAGESTUFF) ; do cmp $$i $(against)/$$i ; done

de-stage1: force
	- (cd stage1 ; rm as ; mv -f * ..)
	- rmdir stage1

de-stage2: force
	- (cd stage2 ; rm as ; mv -f * ..)
	- rmdir stage2

de-stage3: force
	- (cd stage3 ; rm as ; mv -f * ..)
	- rmdir stage3

#In GNU Make, ignore whether `stage*' exists.
.PHONY: stage1 stage2 stage3 stage4 clean realclean TAGS bootstrap

force:

Makefile: $(srcdir)/Makefile.in $(host_makefile_frag) $(target_makefile_frag) \
		$(srcdir)/configure.in
	$(SHELL) ./config.status
