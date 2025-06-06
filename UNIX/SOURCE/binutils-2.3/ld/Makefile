EMULATION_OFILES =  em_elf_i386.o
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
target_makefile_frag = ./config/i386-elf.mt
host_makefile_frag = config/i386v4.mh
# Makefile for the GNU linker ld (version 2)
# Copyright (C) 1989-1993 Free Software Foundation, Inc.

# This file is part of GNU ld..

# This program is free software; you can redistribute it and/or modify
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
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


srcdir = .
objdir = .

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
# We put the scripts in the directory $(scriptdir)/ldscripts.
# We can't put the scripts in $(datadir) because the SEARCH_DIR
# directives need to be different for native and cross linkers.
scriptdir = $(tooldir)/lib

gcclibdir = $(libdir)/gcc/$(target_alias)

SHELL = /bin/sh

INSTALL = `cd $(srcdir); pwd`/../install.sh -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL)
INSTALL_XFORM = $(INSTALL) -t='$(program_transform_name)'
INSTALL_XFORM1 = $(INSTALL_XFORM) -b=.1

AR = ar
AR_FLAGS = qv
CFLAGS = -g
MAKEINFO = makeinfo
TEXI2DVI = texi2dvi
RANLIB = ranlib
CC_FOR_BUILD=$(CC)
BISON = `if [ -f ../bison/bison ] ; then echo ../bison/bison -y -L../bison/bison ; else echo bison -y ; fi`
LEX = `if [ -f ../flex/flex ] ; then echo ../flex/flex ;  else echo flex ; fi`

#version=/`./../gcc/gcc -dumpversion`
version=

# Seach path to override the default search path for -lfoo libraries.
# If LIB_PATH is empty, the ones in the script (if any) are left alone.
# (The default is usually /lib:usr/lib:/usr/local/lib, unless building
# a cross-linker, in which case the default is empty.  See genscripts.sh.)
# Otherwise, they are replaced with the ones given in LIB_PATH,
# which may have the form: LIB_PATH=/lib:/usr/local/lib
LIB_PATH =

# Additional libraries which are used when ld is built native.  This
# is set by some host makefile fragments.
NATIVE_LIB_DIRS =

BASEDIR = $(srcdir)/..
BFDDIR = $(BASEDIR)/bfd
INCDIR	= $(BASEDIR)/include
INCLUDES = -I. -I$(srcdir) -I../bfd -I$(BFDDIR) -I$(INCDIR)

# What version of the manual to build
DOCVER = gen

# Where to find texinfo.tex to format docn with TeX
TEXIDIR = $(srcdir)/../texinfo

# Where to find other docs needed to format with TeX
TEXINPUTS = $(TEXIDIR):$(BFDDIR)/doc:$(srcdir)

# Whether to get roff to put indexing entries on stderr
TEXI2OPT =
# You neeed this to generate ld-index.ms (or .mm or .me)
# TEXI2OPT = -i

TEXI2ROFF=texi2roff

# Which roff program to use to generate index for texi2roff'd doc
ROFF = groff

#stuff for self hosting (can be overridden in config file).
HOSTING_CRT0=/lib/crt0.o
HOSTING_LIBS=`if [ -f ../gcc/libgcc.a ] ; then echo ../gcc/libgcc.a ; else gcc -print-libgcc-file-name; fi`  -lc -lfl
HOSTING_EMU=-m $(EMUL)

CXX = `if [ -f ../gcc/xgcc ] ; then \
	        echo ../gcc/xgcc -B../gcc/; \
	    else echo gcc; \
	    fi`
CXXFLAGS = -fgnu-linker

# FIX_ME: using ../gcc/xgcc breaks the cdtest. 
CXX= g++

# Setup the testing framework, if you have one
RUNTEST = runtest
RUNTEST_FLAGS =
RUNTEST_CC = `if [ -f ../gcc/xgcc ] ; then \
	       echo ../gcc/xgcc -B../gcc/; \
	    else echo gcc; \
	    fi`
RUNTEST_CFLAGS = $(CFLAGS)
RUNTEST_CXX = `if [ -f ../gcc/xgcc ] ; then \
	        echo ../gcc/xgcc -B../gcc/; \
	    else echo gcc; \
	    fi`
# FIX_ME: using ../gcc/xgcc breaks the cdtest. 
RUNTEST_CXX = $(CXX)
RUNTEST_CXXFLAGS = $(CXXFLAGS)

all:

### Host, target, and site specific Makefile fragments come in here.
####
EMUL=elf_i386
HOSTING_CRT0=/usr/ccs/lib/crt1.o /usr/ccs/lib/crti.o /usr/ccs/lib/values-Xa.o `if [ -f ../gcc/crtbegin.o ]; then echo ../gcc/crtbegin.o; fi`
HOSTING_LIBS=`if [ -f ../gcc/libgcc.a ] ; then echo ../gcc/libgcc.a ; else gcc -print-libgcc-file-name; fi`  -lc `if [ -f ../gcc/crtend.o ]; then echo ../gcc/crtend.o; fi` /usr/ccs/lib/crtn.o -lfl
NATIVE_LIB_DIRS=/usr/ccs/lib

LINTFLAGS =  $(INCLUDES) $(EXTRA_DEF) 

# The .cc suffix is used by `make check'.

.SUFFIXES: .y $(SUFFIXES) .cc

# Suppress smart makes who think they know how to automake Yacc files
.y.c:

.cc.o:
	$(CXX) -c -I$(srcdir) $(CXXFLAGS) $(CFLAGS) $<

.c.o:
	$(CC) -c $(INCLUDES) $(HDEFINES) $(TDEFINES) $(CDEFINES) $(CFLAGS) $<

# go directly to ld.new in case this ld isn't capable of
# linking native object on this host.  It can be renamed on
# install.
LD_PROG	= ld.new

# for self hosting
BFDLIB = ../bfd/libbfd.a
LIBIBERTY = ../libiberty/libiberty.a

ALL_EMULATIONS=em_lnk960.o em_sun3.o em_i386aout.o em_go32.o \
	em_m88kbcs.o em_a29k.o em_news.o em_hp300bsd.o em_hp3hpux.o \
	em_h8300.o em_h8300h.o em_ebmon29k.o em_sun4.o em_gld960.o \
	em_m68kcoff.o em_st2000.o em_sa29200.o em_i386mach.o \
	em_vanilla.o em_i386coff.o em_z8ksim.o em_mipslit.o em_i386bsd.o \
	em_mipsbig.o em_mipsbsd.o em_mipsidt.o em_vax.o em_h8500.o \
	em_hppaosf.o em_mipsidtl.o em_sh.o em_elf_i386.o em_alpha.o \
	em_i386lynx.o em_m68klynx.o em_sparclynx.o \
	$(OTHER_EMULATIONS)

# This is now set by configure.in.
#EMULATION_OFILES=${ALL_EMULATIONS}

OFILES= ldgram.o ldlex.o lexsup.o ldlang.o mri.o ldctor.o ldmain.o ldindr.o \
	ldwarn.o ldwrite.o ldexp.o  ldemul.o ldver.o ldmisc.o ldsym.o \
	ldfile.o relax.o  lderror.o ${EMULATION_OFILES}

HEADERS=config.h ldmain.h ldmain.h ldwarn.h ldmisc.h ldindr.h \
	ldsym.h ldctor.h ldlang.h ldexp.h \
	ldlex.h ldwrite.h ldver.h ldemul.h ldfile.h ldgram.h ld.h

MANSOURCES=ld.tex

LDCSOURCES=ldlang.c lexsup.c ldctor.c mri.c ldindr.c ldmain.c ldwrite.c ldwarn.c ldlnk960.c \
	em_gld.c em_sun3.c em_go32.c em_m88k.c em_ebmon29k.c em_hppaosf.c \
	ldgld960.c ldemul.c ldver.c ldmisc.c ldexp.c ldsym.c ldfile.c \
	relax.c lderror.c

GENERATED_SOURCES=ldgram.c ldlex.c em_*.c ldemul-list.h
GENERATED_HEADERS=ldgram.h ldemul-list.h

LDSOURCES=$(LDCSOURCES) ldgram.y ldlex.l ldgram.h

BFDSOURCES=../../bfd/common/*.c

SOURCES= $(LDSOURCES) $(BFDSOURCES)
LINTSOURCES=   $(LDCSOURCES) $(BFDSOURCES) $(GENERATED_SOURCES)

STAGESTUFF = *.o ldscripts/* $(GENERATED_SOURCES) $(GENERATED_HEADERS)

all: $(LD_PROG)

info: ld.info

ldgram.h ldgram.c: ldgram.y
	$(BISON) $(BISONFLAGS) -d $(srcdir)/ldgram.y
	mv -f y.tab.c ldgram.c
	mv -f y.tab.h ldgram.h

# EMUL is the name of a file in the emulparams subdir, without the .sh.
DEF_EMUL = ` if [ -z "$(EMUL)" ] ; then \
	       echo "you must set a default emulation" 1>&2 ; \
	       exit 1 ; \
	     else \
	       echo -DDEFAULT_EMULATION='"$(EMUL)"' ; \
             fi`

ldmain.o: ldmain.c ../bfd/bfd.h $(INCDIR)/ansidecl.h \
  $(INCDIR)/obstack.h ../bfd/sysdep.h $(BFDDIR)/hosts/std-host.h \
  $(INCDIR)/fopen-same.h config.h ld.h \
  ldmain.h ldmisc.h ldwrite.h ./ldgram.h \
  ldsym.h ldlang.h ldemul.h ldlex.h \
  ldfile.h ldindr.h ldwarn.h ldctor.h \
  lderror.h 
	$(CC) -c $(INCLUDES) $(HDEFINES) $(TDEFINES) $(CDEFINES) $(DEF_EMUL) -DSCRIPTDIR='"$(scriptdir)"' $(CFLAGS) $<

ldemul-list.h: Makefile
	(echo "/* This file is automatically generated.  DO NOT EDIT! */";\
	for f in `echo " " ${EMULATION_OFILES} "" \
	 | sed -e 's/em_/ld/g' -e 's/ ld/ /g' -e 's/[.]o//g'`; do \
	    echo "extern ld_emulation_xfer_type ld_$${f}_emulation;"; \
	done;\
	echo "";\
	echo "#define EMULATION_LIST \\";\
	for f in `echo " " ${EMULATION_OFILES} "" \
	 | sed -e 's/em_/ld/g' -e 's/ ld/ /g' -e 's/[.]o//g'`; do \
	    echo "  &ld_$${f}_emulation, \\"; \
	done;\
	echo "  0") >ldemul-list.h

ldlex.c: ldlex.l ldgram.h
	$(LEX) -I -Cem $(srcdir)/ldlex.l
	mv lex.yy.c ldlex.c

# These all start with em_ so 'make clean' can find them.

GENSCRIPTS=$(SHELL) $(srcdir)/genscripts.sh ${srcdir} ${libdir} ${host_alias} ${target_alias} ${EMUL} "$(NATIVE_LIB_DIRS)"
GEN_DEPENDS=$(srcdir)/genscripts.sh

em_sun4.c: $(srcdir)/emulparams/sun4.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/aout.sc ${GEN_DEPENDS}
	${GENSCRIPTS} sun4
em_sun3.c: $(srcdir)/emulparams/sun3.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/aout.sc ${GEN_DEPENDS}
	${GENSCRIPTS} sun3
em_go32.c: $(srcdir)/emulparams/go32.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/aout.sc ${GEN_DEPENDS}
	${GENSCRIPTS} go32
em_news.c: $(srcdir)/emulparams/news.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/aout.sc ${GEN_DEPENDS}
	${GENSCRIPTS} news
em_vax.c: $(srcdir)/emulparams/vax.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/aout.sc ${GEN_DEPENDS}
	${GENSCRIPTS} vax
em_hp300bsd.c: $(srcdir)/emulparams/hp300bsd.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/aout.sc ${GEN_DEPENDS}
	${GENSCRIPTS} hp300bsd
em_hp3hpux.c: $(srcdir)/emulparams/hp3hpux.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/aout.sc ${GEN_DEPENDS}
	${GENSCRIPTS} hp3hpux
em_hppaosf.c: $(srcdir)/emulparams/hppaosf.sh \
  $(srcdir)/emultempl/hppaosf.em $(srcdir)/scripttempl/hppaosf.sc ${GEN_DEPENDS}
	${GENSCRIPTS} hppaosf
em_i386aout.c: $(srcdir)/emulparams/i386aout.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/aout.sc ${GEN_DEPENDS}
	${GENSCRIPTS} i386aout
em_i386mach.c: $(srcdir)/emulparams/i386mach.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/aout.sc ${GEN_DEPENDS}
	${GENSCRIPTS} i386mach
em_ebmon29k.c: $(srcdir)/emulparams/ebmon29k.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/ebmon29k.sc ${GEN_DEPENDS}
	${GENSCRIPTS} ebmon29k
em_sa29200.c: $(srcdir)/emulparams/sa29200.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/sa29200.sc ${GEN_DEPENDS}
	${GENSCRIPTS} sa29200
em_a29k.c: $(srcdir)/emulparams/a29k.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/a29k.sc ${GEN_DEPENDS}
	${GENSCRIPTS} a29k
em_m88kbcs.c: $(srcdir)/emulparams/m88kbcs.sh \
  $(srcdir)/emultempl/m88kbcs.em $(srcdir)/scripttempl/m88kbcs.sc ${GEN_DEPENDS}
	${GENSCRIPTS} m88kbcs
em_h8300.c: $(srcdir)/emulparams/h8300.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/h8300.sc ${GEN_DEPENDS}
	${GENSCRIPTS} h8300
em_h8300h.c: $(srcdir)/emulparams/h8300h.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/h8300h.sc ${GEN_DEPENDS}
	${GENSCRIPTS} h8300h
em_h8500.c: $(srcdir)/emulparams/h8500.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/h8500.sc ${GEN_DEPENDS}
	${GENSCRIPTS} h8500
em_sh.c: $(srcdir)/emulparams/sh.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/sh.sc ${GEN_DEPENDS}
	${GENSCRIPTS} sh
em_st2000.c: $(srcdir)/emulparams/st2000.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/st2000.sc ${GEN_DEPENDS}
	${GENSCRIPTS} st2000
em_z8ksim.c: $(srcdir)/emulparams/z8ksim.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/z8ksim.sc ${GEN_DEPENDS}
	${GENSCRIPTS} z8ksim
em_vanilla.c: $(srcdir)/emulparams/vanilla.sh \
  $(srcdir)/emultempl/vanilla.em $(srcdir)/scripttempl/vanilla.sc ${GEN_DEPENDS}
	${GENSCRIPTS} vanilla
em_lnk960.c: $(srcdir)/emulparams/lnk960.sh \
  $(srcdir)/emultempl/lnk960.em $(srcdir)/scripttempl/i960.sc ${GEN_DEPENDS}
	${GENSCRIPTS} lnk960
em_gld960.c: $(srcdir)/emulparams/gld960.sh \
  $(srcdir)/emultempl/gld960.em $(srcdir)/scripttempl/i960.sc ${GEN_DEPENDS}
	${GENSCRIPTS} gld960
em_m68kcoff.c: $(srcdir)/emulparams/m68kcoff.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/m68kcoff.sc ${GEN_DEPENDS}
	${GENSCRIPTS} m68kcoff
em_m68klynx.c: $(srcdir)/emulparams/m68klynx.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/m68kcoff.sc ${GEN_DEPENDS}
	${GENSCRIPTS} m68klynx
em_i386coff.c: $(srcdir)/emulparams/i386coff.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/i386coff.sc ${GEN_DEPENDS}
	${GENSCRIPTS} i386coff
em_i386lynx.c: $(srcdir)/emulparams/i386lynx.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/i386coff.sc ${GEN_DEPENDS}
	${GENSCRIPTS} i386lynx
em_sparclynx.c: $(srcdir)/emulparams/sparclynx.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/sparccoff.sc ${GEN_DEPENDS}
	${GENSCRIPTS} sparclynx
em_mipslit.c:  $(srcdir)/emulparams/mipslit.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/mips.sc ${GEN_DEPENDS}
	${GENSCRIPTS} mipslit
em_i386bsd.c: $(srcdir)/emulparams/i386bsd.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/aout.sc ${GEN_DEPENDS}
	${GENSCRIPTS} i386bsd
em_mipsbig.c:  $(srcdir)/emulparams/mipsbig.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/mips.sc ${GEN_DEPENDS}
	${GENSCRIPTS} mipsbig
em_mipsbsd.c: $(srcdir)/emulparams/mipsbsd.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/aout.sc ${GEN_DEPENDS}
	${GENSCRIPTS} mipsbsd
em_mipsidt.c: $(srcdir)/emulparams/mipsidt.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/mips.sc ${GEN_DEPENDS}
	${GENSCRIPTS} mipsidt
em_mipsidtl.c: $(srcdir)/emulparams/mipsidtl.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/mips.sc ${GEN_DEPENDS}
	${GENSCRIPTS} mipsidtl
em_elf_i386.c: $(srcdir)/emulparams/elf_i386.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/elf.sc ${GEN_DEPENDS}
	${GENSCRIPTS} elf_i386
em_elf32mipb.c: $(srcdir)/emulparams/elf32mipb.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/elf.sc ${GEN_DEPENDS}
	${GENSCRIPTS} elf32mipb
em_alpha.c: $(srcdir)/emulparams/alpha.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/alpha.sc ${GEN_DEPENDS}
	${GENSCRIPTS} alpha

$(LD_PROG): $(OFILES) $(BFDLIB) $(LIBIBERTY)
	$(CC) $(INCLUDES) $(HDEFINES) $(TDEFINES) $(CDEFINES) $(CFLAGS) $(LDFLAGS) -o $(LD_PROG) $(OFILES) $(BFDLIB) $(LIBIBERTY) $(LOADLIBES)

# The generated emulation files mostly have the same dependencies.
$(EMULATION_OFILES): ../bfd/bfd.h ../bfd/sysdep.h ld.h ldemul.h \
  ldfile.h ldmisc.h config.h

# This list of dependencies was generated by doing a make with gcc -MM
# saving the output in a file and removing the gcc commands
# changing "../../devo/ld/../bfd" to "$(BFDDIR)"
# removing "../../devo/ld/"
# changing "../include" to "$(INCDIR)"

ldgram.o: ldgram.c ../bfd/bfd.h $(INCDIR)/ansidecl.h $(INCDIR)/obstack.h \
  ../bfd/sysdep.h $(BFDDIR)/hosts/std-host.h $(INCDIR)/fopen-same.h \
  ld.h ldexp.h ldver.h ldlang.h \
  ldemul.h ldfile.h ldmisc.h mri.h 
ldlex.o: ldlex.c ../bfd/bfd.h $(INCDIR)/obstack.h ./ldgram.h 
lexsup.o: lexsup.c ../bfd/bfd.h $(INCDIR)/ansidecl.h \
  $(INCDIR)/obstack.h ../bfd/sysdep.h $(BFDDIR)/hosts/std-host.h \
  $(INCDIR)/fopen-same.h ldlex.h ld.h \
  ldexp.h ./ldgram.h ldmisc.h 
ldlang.o: ldlang.c ../bfd/bfd.h $(INCDIR)/ansidecl.h \
  $(INCDIR)/obstack.h ../bfd/sysdep.h $(BFDDIR)/hosts/std-host.h \
  $(INCDIR)/fopen-same.h ld.h ldmain.h \
  ldsym.h ./ldgram.h ldwarn.h ldlang.h \
  ldexp.h ldemul.h ldlex.h ldmisc.h \
  ldindr.h ldctor.h 
mri.o: mri.c ../bfd/bfd.h $(INCDIR)/ansidecl.h \
  $(INCDIR)/obstack.h ../bfd/sysdep.h $(BFDDIR)/hosts/std-host.h \
  $(INCDIR)/fopen-same.h ld.h ldlang.h \
  mri.h ./ldgram.h ldexp.h 
ldctor.o: ldctor.c ../bfd/bfd.h $(INCDIR)/ansidecl.h \
  $(INCDIR)/obstack.h ../bfd/sysdep.h $(BFDDIR)/hosts/std-host.h \
  $(INCDIR)/fopen-same.h ld.h ldlang.h \
  ldsym.h ldmisc.h ldexp.h ./ldgram.h 
ldmain.o: ldmain.c ../bfd/bfd.h $(INCDIR)/ansidecl.h \
  $(INCDIR)/obstack.h ../bfd/sysdep.h $(BFDDIR)/hosts/std-host.h \
  $(INCDIR)/fopen-same.h config.h ld.h \
  ldmain.h ldmisc.h ldwrite.h ./ldgram.h \
  ldsym.h ldlang.h ldemul.h ldlex.h \
  ldfile.h ldindr.h ldwarn.h ldctor.h \
  lderror.h 
ldindr.o: ldindr.c ../bfd/bfd.h $(INCDIR)/ansidecl.h \
  $(INCDIR)/obstack.h ../bfd/sysdep.h $(BFDDIR)/hosts/std-host.h \
  $(INCDIR)/fopen-same.h ld.h ldsym.h \
  ldmisc.h 
ldwarn.o: ldwarn.c ../bfd/bfd.h $(INCDIR)/ansidecl.h \
  $(INCDIR)/obstack.h ../bfd/sysdep.h $(BFDDIR)/hosts/std-host.h \
  $(INCDIR)/fopen-same.h ldsym.h ldwarn.h \
  ldmisc.h 
ldwrite.o: ldwrite.c ../bfd/bfd.h $(INCDIR)/ansidecl.h \
  $(INCDIR)/obstack.h ../bfd/sysdep.h $(BFDDIR)/hosts/std-host.h \
  $(INCDIR)/fopen-same.h ldlang.h ld.h \
  ldwrite.h ldmisc.h ldsym.h ./ldgram.h \
  relax.h 
ldexp.o: ldexp.c ../bfd/bfd.h $(INCDIR)/ansidecl.h \
  $(INCDIR)/obstack.h ../bfd/sysdep.h $(BFDDIR)/hosts/std-host.h \
  $(INCDIR)/fopen-same.h ld.h ldmain.h \
  ldmisc.h ldexp.h ./ldgram.h ldsym.h \
  ldlang.h 
ldemul.o: ldemul.c ../bfd/bfd.h $(INCDIR)/ansidecl.h \
  $(INCDIR)/obstack.h ../bfd/sysdep.h $(BFDDIR)/hosts/std-host.h \
  $(INCDIR)/fopen-same.h config.h ld.h \
  ldemul.h ldmisc.h ./ldemul-list.h 
ldver.o: ldver.c ../bfd/bfd.h $(INCDIR)/ansidecl.h \
  $(INCDIR)/obstack.h ../bfd/sysdep.h $(BFDDIR)/hosts/std-host.h \
  $(INCDIR)/fopen-same.h ldver.h ldemul.h 
ldmisc.o: ldmisc.c ../bfd/bfd.h $(INCDIR)/ansidecl.h \
  $(INCDIR)/obstack.h ../bfd/sysdep.h $(BFDDIR)/hosts/std-host.h \
  $(INCDIR)/fopen-same.h ld.h ldmisc.h \
  ldlang.h ldlex.h 
ldsym.o: ldsym.c ../bfd/bfd.h $(INCDIR)/ansidecl.h \
  $(INCDIR)/obstack.h ../bfd/sysdep.h $(BFDDIR)/hosts/std-host.h \
  $(INCDIR)/fopen-same.h ld.h ldsym.h \
  ldmisc.h ldlang.h 
ldfile.o: ldfile.c ../bfd/bfd.h $(INCDIR)/ansidecl.h \
  $(INCDIR)/obstack.h ../bfd/sysdep.h $(BFDDIR)/hosts/std-host.h \
  $(INCDIR)/fopen-same.h ldmisc.h ldlang.h \
  ldfile.h 
relax.o: relax.c ../bfd/bfd.h $(INCDIR)/ansidecl.h \
  $(INCDIR)/obstack.h $(BFDDIR)/seclet.h $(INCDIR)/coff/internal.h \
  ../bfd/sysdep.h $(BFDDIR)/hosts/std-host.h $(INCDIR)/fopen-same.h \
  ldlang.h ld.h ldwrite.h ldmisc.h \
  ldsym.h ./ldgram.h relax.h 
lderror.o: lderror.c ../bfd/bfd.h $(INCDIR)/ansidecl.h \
  $(INCDIR)/obstack.h ../bfd/sysdep.h $(BFDDIR)/hosts/std-host.h \
  $(INCDIR)/fopen-same.h $(BFDDIR)/seclet.h ld.h \
  ldmisc.h 

# CYGNUS LOCAL targets.
# These targets are for the dejagnu testsuites. The file site.exp 
# contains global variables that all the testsuites will use.
# There is a current debate as to how and where to generate test
# outputs. Rob feels each test should be built in $(objdir) with
# a unique name. Cassidy feels that we should create a directory
# called $(objdir)/tmpdir and do the work there. This way, there
# is no potential conflict with existing objects, ie: as there
# was in the past with loop.o and flow.o, and, there is no chance
# of filling /tmp, which would cause other problems. Lastly, this 
# allow retention of the testcase name making debugging easier.
#
testdir = $(objdir)/tmpdir

site.exp: ./config.status Makefile
	@if [ -d $(testdir) ]; then true; else mkdir $(testdir); fi
	@echo "Making a new config file..."
	@rm -f ./tmp?
	@touch site.exp
	@mv site.exp site.bak
	@echo "## variables are automatically generated by make ##" > ./tmp0
	@echo "# Do not edit here. If you wish to override these" >> ./tmp0
	@echo "# values, add them to the last section" >> ./tmp0
	@echo "# HOST AND TARGET INFO" >> ./tmp0
	@echo "set host_os $(host_os)" >> ./tmp0
	@echo "set host_alias $(host_alias)" >> ./tmp0
	@echo "set host_cpu $(host_cpu)" >> ./tmp0
	@echo "set host_vendor $(host_vendor)" >> ./tmp0
	@echo "set target_os $(target_os)" >> ./tmp0
	@echo "set target_alias $(target_alias)" >> ./tmp0
	@echo "set target_cpu $(target_cpu)" >> ./tmp0
	@echo "set target_vendor $(target_vendor)" >> ./tmp0
	@echo "set host_triplet $(host_canonical)" >> ./tmp0
	@echo "set target_triplet $(target_canonical)" >> ./tmp0
	@echo "# DIRECTORY INFO" >> ./tmp0
	@echo "set objdir `pwd`" >> ./tmp0
	@echo "set tmpdir `cd $(testdir); pwd`" >> ./tmp0
	@echo "" >> ./tmp0	
	@echo "# LD DEPENDANCIES" >> ./tmp0	
	@echo "set OFILES \"$(OFILES)\"" >> ./tmp0
	@echo "set BFDLIB \"$(BFDLIB)\"" >> ./tmp0
	@echo "set LIBIBERTY \"$(LIBIBERTY)\"" >> ./tmp0
	@echo "set HOSTING_EMU \"$(HOSTING_EMU)\"" >> ./tmp0
	@echo "set HOSTING_CRT0 \"$(HOSTING_CRT0)\"" >> ./tmp0
	@echo "set HOSTING_LIBS \"$(HOSTING_LIBS)\"" >> ./tmp0
	@echo "" >> ./tmp0	
	@echo "## Variables generated by configure. Do Not Edit ##" >> ./tmp0
	@cat ./tmp0 > site.exp
	@cat site.bak | sed \
			-e '1,/^## Variables generated by.*##/ d' >> site.exp
	-@rm -f ./tmp?

#check: ld.new site.exp
#	$(RUNTEST) --tool ld \
#	--srcdir $(srcdir)/testsuite $(RUNTEST_FLAGS) \
#	CC="$(RUNTEST_CC)" CFLAGS="$(RUNTEST_CFLAGS)" \
#	CXX="$(RUNTEST_CXX)" CXXFLAGS="$(RUNTEST_CXXFLAGS)"
check:

installcheck:

# Rules for testing by relinking ld itself.

ld-partial.o: ld.new
	./ld.new $(HOSTING_EMU) -o ld-partial.o -r $(OFILES)
ld1: ld-partial.o
	./ld.new $(HOSTING_EMU) -o ld1 $(HOSTING_CRT0) ld-partial.o $(BFDLIB) $(LIBIBERTY) $(HOSTING_LIBS)

ld1-full: ld.new
	./ld.new $(HOSTING_EMU) -o ld1-full $(HOSTING_CRT0) $(OFILES) $(BFDLIB) $(LIBIBERTY) $(HOSTING_LIBS)

ld2: ld1
	./ld1 $(HOSTING_EMU) -o ld2 $(HOSTING_CRT0) $(OFILES) $(BFDLIB) $(LIBIBERTY) $(HOSTING_LIBS)

ld3: ld2
	./ld2 $(HOSTING_EMU) -o ld3 $(HOSTING_CRT0) $(OFILES) $(BFDLIB) $(LIBIBERTY) $(HOSTING_LIBS)

bootstrap: ld3
	cmp ld2 ld3

# A test program for C++ constructors and destructors.

cdtest: cdtest-main.o cdtest-func.o cdtest-foo.o ld.new
	./ld.new $(HOSTING_EMU) -o cdtest $(HOSTING_CRT0) \
	  cdtest-main.o cdtest-func.o cdtest-foo.o $(HOSTING_LIBS)

check-cdtest: cdtest $(srcdir)/cdtest.exp
	./cdtest >cdtest.out
	diff $(srcdir)/cdtest.exp cdtest.out
# END OF CHECK TARGETS

# DOCUMENTATION TARGETS
# Manual configuration file; not usually attached to normal configuration,
# because almost all configs use "gen" version of manual.
#  Set DOCVER above to change.
configdoc.texi:	${DOCVER}-doc.texi
	ln -s ${srcdir}/${DOCVER}-doc.texi ./configdoc.texi || \
	ln ${srcdir}/${DOCVER}-doc.texi ./configdoc.texi || \
	cp ${srcdir}/${DOCVER}-doc.texi ./configdoc.texi

# TeX output
dvi: ld.dvi
ld.dvi: $(srcdir)/ld.texinfo $(srcdir)/configdoc.texi $(BFDDIR)/doc/bfdsumm.texi
	TEXINPUTS=$(BFDDIR)/doc:$$TEXINPUTS $(TEXI2DVI) $(srcdir)/ld.texinfo

ldint.dvi: $(srcdir)/ldint.texinfo
	$(TEXI2DVI) $(srcdir)/ldint.texinfo

# info file for online browsing
ld.info: $(srcdir)/ld.texinfo configdoc.texi $(BFDDIR)/doc/bfdsumm.texi
	$(MAKEINFO) -I$(BFDDIR)/doc -o ld.info $(srcdir)/ld.texinfo

ldint.info: $(srcdir)/ldint.texinfo
	$(MAKEINFO) -o ldint.info $(srcdir)/ldint.texinfo

#separate targets for "ms", "me", and "mm" forms of roff doc
# Try to use a recent texi2roff.  v2 was put on prep in jan91.
# If you want an index, see texi2roff doc for postprocessing 
# and add -i to texi2roff invocations below.
# Workarounds for texi2roff-2 (probably fixed in later texi2roff's, delete
#    correspondint -e lines when later texi2roff's are current)
# + @ifinfo's deleted explicitly due to texi2roff-2 bug w nested constructs.
# + @c's deleted explicitly because texi2roff sees texinfo commands in them
# + @   (that's at-BLANK) not recognized by texi2roff, turned into blank
# + @alphaenumerate is ridiculously new, turned into @enumerate

ld.ms: $(srcdir)/ld.texinfo
	sed -e '/\\input texinfo/d' \
		-e '/@c TEXI2ROFF-KILL/,/@c END TEXI2ROFF-KILL/d' \
		-e '/^@ifinfo/,/^@end ifinfo/d' \
		-e '/^@c/d' \
		-e 's/{.*,,/{/' \
		-e 's/@ / /g' \
		-e 's/^@alphaenumerate/@enumerate/g' \
		-e 's/^@end alphaenumerate/@end enumerate/g' \
		$(srcdir)/ld.texinfo | \
	$(TEXI2ROFF) $(TEXI2OPT) -ms | \
	sed -e 's/---/\\(em/g' \
		>>ld.ms 

# index for roff output
ld-index.ms: ld.ms
	$(ROFF) -ms ld.ms 2>&1 1>/dev/null | \
		sed -e '/: warning:/d' | \
		texi2index >ld-index.ms

# roff output (-mm)
ld.mm: $(srcdir)/ld.texinfo
	sed -e '/\\input texinfo/d' \
		-e '/@c TEXI2ROFF-KILL/,/@c END TEXI2ROFF-KILL/d' \
		-e '/^@ifinfo/,/^@end ifinfo/d' \
		-e '/^@c/d' \
		-e 's/{.*,,/{/' \
		-e '/@noindent/d' \
		-e 's/@ / /g' \
		-e 's/^@alphaenumerate/@enumerate/g' \
		-e 's/^@end alphaenumerate/@end enumerate/g' \
		$(srcdir)/ld.texinfo | \
	$(TEXI2ROFF) $(TEXI2OPT) -mm | \
	sed -e 's/---/\\(em/g' \
	>ld.mm 

# index for roff output
ld-index.mm: ld.mm
	$(ROFF) -mm ld.mm 2>&1 1>/dev/null | \
		sed -e '/: warning:/d' | \
		texi2index >ld-index.mm

# roff output (-me)
ld.me: $(srcdir)/ld.texinfo
	sed -e '/\\input texinfo/d' \
		-e '/@c TEXI2ROFF-KILL/,/@c END TEXI2ROFF-KILL/d' \
		-e '/^@ifinfo/,/^@end ifinfo/d' \
		-e '/^@c/d' \
		-e 's/{.*,,/{/' \
		-e 's/@ / /g' \
		-e 's/^@alphaenumerate/@enumerate/g' \
		-e 's/^@end alphaenumerate/@end enumerate/g' \
		$(srcdir)/ld.texinfo | \
	$(TEXI2ROFF) $(TEXI2OPT) -me | \
	sed -e 's/---/\\(em/g' \
		>>ld.me 

# index for roff output
ld-index.me: ld.me
	$(ROFF) -me ld.me 2>&1 1>/dev/null | \
		sed -e '/: warning:/d' | \
		texi2index >ld-index.me

stage1:	force
	-mkdir stage1
	-mv -f $(STAGESTUFF) $(LD_PROG) stage1
	-(cd stage1 ; ln -s $(LD_PROG) ld)

stage2:	force
	-mkdir stage2
	-mv -f $(STAGESTUFF) $(LD_PROG) stage2
	-(cd stage2 ; ln -s $(LD_PROG) ld)

stage3:	force
	-mkdir stage3
	-mv -f $(STAGESTUFF) $(LD_PROG) stage3
	-(cd stage3 ; ln -s $(LD_PROG) ld)

against=stage2

comparison: force
	for i in $(STAGESTUFF) $(LD_PROG) ; do cmp $$i $(against)/$$i ; done

de-stage1: force
	-(cd stage1 ; mv -f * ..)
	-rm ld
	-rmdir stage1

de-stage2: force
	-(cd stage2 ; mv -f * ..)
	-rm ld
	-rmdir stage2

de-stage3: force
	-(cd stage3 ; mv -f * ..)
	-rm ld
	-rmdir stage3

# Stuff that should be included in a distribution:
LDDISTSTUFF=ldgram.c ldgram.h ldlex.c
diststuff: $(LDDISTSTUFF)

mostlyclean:
	-rm -f $(STAGESTUFF) ld.?? ld.??? ldlex.[qp]
	-rm -f ld ld1 ld2 ld3 *.o y.output cdtest cdtest.out
clean: mostlyclean
	-rm -f $(LD_PROG)
distclean:
	-rm -fr Makefile config.status TAGS sysdep.h ldscripts site.exp \
		$(STAGESTUFF) ld.?? ld.??s ld.toc ld.aux ld.log ldlex.[qp] \
		$(LD_PROG) ld ld1 ld2 ld3 *.o y.output cdtest cdtest.out
realclean: clean distclean
	-rm -f $(LDDISTSTUFF)

lintlog:$(SOURCES) Makefile
	$(LINT) -abhxzn  $(LINTFLAGS)  $(LINTSOURCES) \
| grep -v "pointer casts may be troublesome" \
| grep -v "possible pointer alignment problem" \
| grep -v "ignore" \
| grep -v "conversion from long may lose accuracy" \
| grep -v "warning: constant argument to NOT" \
| grep -v "enumeration type clash, operator CAST" \
| grep -v "warning: constant in conditional context"\
| grep -v "archive\.c"


TAGS:
	etags -t $(srcdir)/*.[chly] *.[chly]


.PHONY: install
install: 
	$(INSTALL_XFORM) ld.new $(bindir)/ld
	$(INSTALL_XFORM1) $(srcdir)/ld.1 $(man1dir)/ld.1
	for f in ldscripts/*; do \
	  $(INSTALL_DATA) $$f $(scriptdir)/$$f ; \
	done
	-n=`t='$(program_transform_name)'; echo ld | sed -e "" $$t`; \
	  rm -f $(tooldir)/bin/ld; \
	  ln $(bindir)/$$n $(tooldir)/bin/ld \
	   || $(INSTALL_PROGRAM) ld.new $(tooldir)/bin/ld

install-info: 
	for i in ld.info* ; do \
		$(INSTALL_DATA) $$i $(infodir)/$$i ; \
	done

clean-info:
	-rm -rf *.info*

#-----------------------------------------------------------------------------
#		'STANDARD' GNU/960 TARGETS BELOW THIS POINT
#
# 'VERSION' file must be present and contain a string of the form "x.y"
#-----------------------------------------------------------------------------

ver960.c: FORCE
	rm -f ver960.c
	echo "char ${TARG}_ver[]= \"${TARG} `cat VERSION`, `date`\";" > ver960.c


# This target should be invoked before building a new release.
# 'VERSION' file must be present and contain a string of the form "x.y"
#
roll:
	@V=`cat VERSION`		; \
	MAJ=`sed 's/\..*//' VERSION`	; \
	MIN=`sed 's/.*\.//' VERSION`	; \
	V=$$MAJ.`expr $$MIN + 1`	; \
	rm -f VERSION			; \
	echo $$V >VERSION		; \
	echo Version $$V


dep: $(LDSOURCES)
	mkdep $(CFLAGS) $?

# Dummy target to force execution of dependent targets.
#
force:

# Target to uncomment host-specific lines in this makefile.  Such lines must
# have the following string beginning in column 1: #__<hostname>__#
# Original Makefile is backed up as 'Makefile.old'.
#
# Invoke with:  make make HOST=xxx
#
make:
	-@if test $(HOST)x = x ; then \
		echo 'Specify "make make HOST=???"'; \
		exit 1; \
	fi ; \
	grep -s "^#The next line was generated by 'make make'" Makefile; \
	if test $$? = 0 ; then	\
		echo "Makefile has already been processed with 'make make'";\
		exit 1; \
	fi ; \
	mv -f Makefile Makefile.old; \
	echo "#The next line was generated by 'make make'"	 >Makefile ; \
	echo "HOST=$(HOST)"					>>Makefile ; \
	echo							>>Makefile ; \
	sed "s/^#__$(HOST)__#//" < Makefile.old			>>Makefile

#

Makefile: $(srcdir)/Makefile.in $(host_makefile_frag) $(target_makefile_frag)
	$(SHELL) ./config.status

### mode:fundamental ***
### Local Variables: ***
### page-delimiter: "^#" ***
### End: ***
### end of file


# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
