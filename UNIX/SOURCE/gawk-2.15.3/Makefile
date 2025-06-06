# Makefile for GNU Awk.
#
# Copyright (C) 1986, 1988-1992 the Free Software Foundation, Inc.
# 
# This file is part of GAWK, the GNU implementation of the
# AWK Progamming Language.
# 
# GAWK is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# GAWK is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GAWK; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# User tunable macros -- CHANGE THESE IN Makefile.in RATHER THAN IN 
# Makefile, OR configure WILL OVERWRITE YOUR CHANGES

prefix = /opt
exec_prefix = $(prefix)
binprefix =
manprefix =

bindir = $(exec_prefix)/bin
libdir = $(exec_prefix)/lib
mandir = $(prefix)/man/man1
manext = .1
infodir = $(prefix)/info

#	The provided "configure" is used to turn a config file (samples in
#	the "config" directory into commands to edit config.in into
#	a suitable config.h and to edit Makefile.in into Makefile.
#	To port GAWK, create an appropriate config file using the ones in
#	the config directory as examples and using the comments in config.in
#	as a guide.

CC=		 gcc
##MAKE_CC## CC = cc

PROFILE=	#-pg
DEBUG=		#-DMALLOCDEBUG #-DDEBUG #-DFUNC_TRACE #-DMPROF
LINKSTATIC=	#-Bstatic
WARN=		#-W -Wunused -Wimplicit -Wreturn-type -Wcomment	# for gcc only

# Parser to use on grammar - any one of the following will work
PARSER = bison -y
#PARSER = yacc
#PARSER = byacc

# Set LIBS to any libraries that are machine specific
LIBS =

# Cray 2 running Unicos 5.0.7
##MAKE_LIBNET## LIBS = -lnet


# Systems with alloca in /lib/libPW.a
##MAKE_ALLOCA_PW## LIBS = -lPW

# ALLOCA - only needed if you use bison
#	Set equal to alloca.o if your system is S5 and you don't have
#	alloca. Uncomment one of the rules below to make alloca.o from
#	either alloca.s or alloca.c.
#	This should have already been done automatically by configure.
#
#	Some systems have alloca in libPW.a, so LIBS=-lPW may work, too.
ALLOCA= alloca.o
##MAKE_ALLOCA_S## ALLOCA= alloca.o

VFLAGS=

# VMS POSIX, VAXC V3.2
##MAKE_VMS-Posix## VFLAGS = -UVMS -D__STDC__=0

# HP/Apollo running cc version 6.7 or earlier
##MAKE_Apollo## VFLAGS = -U__STDC__ -A run,sys5.3
##MAKE_Apollo## LIBS = -A sys,any

# SGI IRIX 4.0.5 cc flags
##MAKE_SGI## VFLAGS = -cckr

##MAKE_NeXT## VFLAGS = -DGFMT_WORKAROUND

CFLAGS = -g -O3
FLAGS = -DGAWK -DHAVE_CONFIG_H $(VFLAGS) $(DEBUG) $(PROFILE) $(WARN)
LDFLAGS = $(LINKSTATIC) $(PROFILE)

.c.o:
	$(CC) $(CFLAGS) $(FLAGS) -c $<

# object files
AWKOBJS = main.o eval.o builtin.o msg.o iop.o io.o field.o array.o \
	node.o version.o missing.o re.o getopt.o getopt1.o

ALLOBJS = $(AWKOBJS) awktab.o

# GNUOBJS
#	GNU stuff that gawk uses as library routines.
GNUOBJS= regex.o dfa.o $(ALLOCA)

# source and documentation files
SRC =	main.c eval.c builtin.c msg.c version.c \
	iop.c io.c field.c array.c node.c missing.c re.c getopt.c getopt1.c

ALLSRC= $(SRC) awktab.c

AWKSRC= awk.h awk.y $(ALLSRC) patchlevel.h protos.h config.in getopt.h

GNUSRC = alloca.c alloca.s dfa.c dfa.h regex.c regex.h

COPIES = missing/system.c missing/tzset.c \
	missing/memcmp.c missing/memcpy.c missing/memset.c \
	missing/random.c missing/strncasecmp.c missing/strchr.c \
	missing/strerror.c missing/strtod.c \
	missing/strftime.c missing/strftime.3 

SUPPORT = support/texindex.c support/texinfo.tex

DOCS= gawk.1 gawk.texi

TEXFILES= gawk.aux gawk.cp gawk.cps gawk.fn gawk.fns gawk.ky gawk.kys \
	   gawk.pg gawk.pgs gawk.toc gawk.tp gawk.tps gawk.vr gawk.vrs

MISC =	NEWS COPYING FUTURES Makefile.* PROBLEMS README* PORTS POSIX \
	mungeconf configure ACKNOWLEDGMENT LIMITATIONS

OTHERS= pc/* atari/* vms/*

ALLDOC= gawk.dvi $(TEXFILES) gawk.info*

# Release of gawk.  There can be no leading or trailing white space here!
REL=2.15

# rules to build gawk
gawk:	$(ALLOBJS) $(GNUOBJS) $(REOBJS)
	$(CC) -o gawk $(LDFLAGS) $(ALLOBJS) $(GNUOBJS) $(REOBJS) -lm $(LIBS)

$(AWKOBJS) regex.o dfa.o:	awk.h dfa.h regex.h

getopt.o:	getopt.h

getopt1.o:	getopt.h

main.o:	patchlevel.h

awktab.c:	awk.y
	$(PARSER) -v awk.y
##MAKE_VMS-Posix## 	mv ytab.c awktab.c
##MAKE_VMS-Posix## dummy.awk_tab.target:
	sed '/^extern char .malloc(), .realloc();$$/d' y.tab.c >awktab.c
	rm y.tab.c

awktab.o:	awk.h

config.h:	config.in
	@echo You must provide a config.h!
	@echo Run \"./configure\" to build it for known systems
	@echo or copy config.in to config.h and edit it.; exit 1

install:	gawk gawk.info
	cp gawk $(bindir) && chmod 755 $(bindir)/gawk
	cp gawk.1 $(mandir)/gawk$(manext) && chmod 644 $(mandir)/gawk$(manext)
	cp gawk.info* $(infodir) && chmod 644 $(infodir)/gawk.info*

uninstall:
	rm -f $(bindir)/gawk $(mandir)/gawk$(manext) $(infodir)/gawk.info*

# ALLOCA: uncomment this if your system (notably System V boxen)
# does not have alloca in /lib/libc.a or /lib/libPW.a
#
# If your machine is not supported by the assembly version of alloca.s,
# use the C version which follows instead.  It uses the default rules to
# make alloca.o.
#
# One of these rules should have already been selected by running configure.


##MAKE_ALLOCA_S## alloca.o:	alloca.s
##MAKE_ALLOCA_S## 	/lib/cpp < alloca.s | sed '/^#/d' > t.s
##MAKE_ALLOCA_S## 	as t.s -o alloca.o
##MAKE_ALLOCA_S## 	rm t.s

alloca.o:	alloca.c

# auxiliary rules for release maintenance
lint:	$(ALLSRC)
	lint -hcbax $(FLAGS) $(ALLSRC)

xref:
	cxref -c $(FLAGS) $(ALLSRC) | grep -v '	/' >xref

clean:
	rm -rf gawk *.o core
	cd test && make clean

distclean:	clean
	rm -f Makefile *.orig *.rej */*.orig */*.rej awk.output gmon.out \
		make.out y.output config.h

mostlyclean:	clean

realclean:	distclean
	rm -f awktab.c $(ALLDOC)

cleaner:	clean
	rm -f gawk awktab.c Makefile config.h

clobber:	clean
	rm -f $(ALLDOC) gawk.log config.h

gawk.dvi:	gawk.texi
	cp support/texinfo.tex .
	tex gawk.texi; texindex gawk.??
	tex gawk.texi; texindex gawk.??
	tex gawk.texi
	rm -f texinfo.tex

gawk.info:	gawk.texi
	makeinfo gawk.texi

dist:	$(AWKSRC) $(GNUSRC) $(DOCS) $(MISC) $(COPIES) $(SUPPORT) distclean
	-rm -rf gawk-$(REL)*
	dir=gawk-$(REL).`gawk '{print $$3}' patchlevel.h` && \
	mkdir $$dir && \
	cp -p $(AWKSRC) $(GNUSRC) $(DOCS) $(MISC) $$dir && \
	mkdir $$dir/missing && cp -p $(COPIES) $$dir/missing && \
	mkdir $$dir/atari && cp -p  atari/* $$dir/atari && \
	mkdir $$dir/pc && cp -p pc/* $$dir/pc && \
	mkdir $$dir/vms && cp -p vms/* $$dir/vms && \
	mkdir $$dir/config && cp -p config/* $$dir/config && \
	mkdir $$dir/support && cp -p support/* $$dir/support && \
	cp -pr test $$dir && \
	chmod -R a+r $$dir && \
	chmod -R a-w $$dir && \
	find $$dir -type d -exec chmod 755 {} ';' && \
	find $$dir -print | doschk && \
	tar -cf - $$dir | gzip > $$dir.tar.gz && \
	rm -fr $$dir

gawk-doc-$(REL).tar.gz:	gawk.info gawk.dvi gawk.1
	-rm -rf gawk-doc-$(REL) gawk-doc-$(REL).tar.gz
	-mkdir gawk-doc-$(REL)
	cp -p $(ALLDOC) gawk-doc-$(REL)
	groff -Tascii -man gawk.1 > gawk-doc-$(REL)/gawk.1.pr
	tar -cf - gawk-doc-$(REL) | gzip > gawk-doc-$(REL).tar.gz

gawk-ps-$(REL).tar.gz:	gawk.dvi gawk.1
	-rm -rf gawk-ps-$(REL) gawk-ps-$(REL).tar.gz
	-mkdir gawk-ps-$(REL)
	dvips -o gawk-ps-$(REL)/gawk.postscript gawk.dvi
	groff -man gawk.1 > gawk-ps-$(REL)/gawk.1.ps
	tar -cf - gawk-ps-$(REL) | gzip > gawk-ps-$(REL).tar.gz

release:	dist gawk-doc-$(REL).tar.gz gawk-ps-$(REL).tar.gz

test:	gawk
	cd test; make -k

check:	test

