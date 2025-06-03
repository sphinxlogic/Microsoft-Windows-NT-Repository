# Generated automatically from Makefile.in by configure.
# Main Makefile for GNU m4.
# Copyright (C) 1992, 1993 Free Software Foundation, Inc.

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

SHELL = /bin/sh

# This directory's subdirectories are mostly independent; you can cd
# into them and run `make' without going through this Makefile.
# To change the values of `make' variables: instead of editing Makefiles,
# (1) if the variable is set in `config.status', edit `config.status'
#     (which will cause the Makefiles to be regenerated when you run `make');
# (2) otherwise, pass the desired values on the `make' command line.

srcdir = .
VPATH = .
U = /usr/lib/uucp

CC = gcc
INSTALL = /opt/bin/install -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644
MAKEINFO = makeinfo
TEXI2DVI = texi2dvi

DEFS =  -DSTDC_HEADERS=1 -DHAVE_STRING_H=1 -DHAVE_TMPFILE=1 -DHAVE_VPRINTF=1 -DHAVE_EFGCVT=2
CFLAGS = -g -O3
LDFLAGS = -g
LIBS = 

prefix = /opt
binprefix =
exec_prefix = ${prefix}
bindir = $(exec_prefix)/bin
infodir = $(prefix)/info

CPPFLAGS = -I$(srcdir) -I$(srcdir)/lib $(DEFS)

.c.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $<

HEADERS = m4.h
SOURCES = m4.c input.c output.c symtab.c builtin.c macro.c \
debug.c eval.c path.c format.c
# OBJECTS = $Um4.o $Uinput.o $Uoutput.o $Usymtab.o $Ubuiltin.o $Umacro.o \
# $Udebug.o $Ueval.o $Upath.o $Uformat.o
OBJECTS = m4.o input.o output.o symtab.o builtin.o macro.o \
debug.o eval.o path.o format.o

DISTFILES = README NEWS TODO THANKS COPYING INSTALL mkinstalldirs \
ChangeLog Makefile.in configure.in aclocal.m4 ansi2knr.c c-boxes.el \
$(HEADERS) $(SOURCES) m4.texinfo texinfo.tex \
configure m4.info m4.info-1 m4.info-2

all: m4

m4: $(OBJECTS) lib/libm4.a
	$(CC) $(LDFLAGS) -o m4 $(OBJECTS) lib/libm4.a $(LIBS)

$(OBJECTS): m4.h lib/obstack.h
builtin.o: lib/regex.h

lib/libm4.a:
	cd lib; $(MAKE) $(MFLAGS) libm4.a

ansi2knr: ansi2knr.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o ansi2knr $(srcdir)/ansi2knr.c
_builtin.c: builtin.c ansi2knr
	./ansi2knr $(srcdir)/builtin.c > _builtin.c
_debug.c: debug.c ansi2knr
	./ansi2knr $(srcdir)/debug.c | sed 's/ va_alist;$$//' > _debug.c
_eval.c: eval.c ansi2knr
	./ansi2knr $(srcdir)/eval.c > _eval.c
_format.c: format.c ansi2knr
	./ansi2knr $(srcdir)/format.c > _format.c
_input.c: input.c ansi2knr
	./ansi2knr $(srcdir)/input.c > _input.c
_m4.c: m4.c ansi2knr
	./ansi2knr $(srcdir)/m4.c | sed 's/ va_alist;$$//' > _m4.c
_macro.c: macro.c ansi2knr
	./ansi2knr $(srcdir)/macro.c > _macro.c
_output.c: output.c ansi2knr
	./ansi2knr $(srcdir)/output.c > _output.c
_path.c: path.c ansi2knr
	./ansi2knr $(srcdir)/path.c > _path.c
_symtab.c: symtab.c ansi2knr
	./ansi2knr $(srcdir)/symtab.c > _symtab.c

info: m4.info

m4.info: m4.texinfo
	$(MAKEINFO) -I$(srcdir) m4.texinfo

dvi: m4.dvi

m4.dvi: m4.texi
	$(TEXI2DVI) $(srcdir)/m4.texi

check: m4
	PATH=`pwd`:$$PATH; export PATH; cd checks; $(MAKE) $(MFLAGS) check

# Make sure all installation directories, e.g. $(bindir) actually exist by
# making them if necessary.
installdirs:
	$(SHELL) $(srcdir)/mkinstalldirs $(bindir) $(infodir)

install: installdirs m4 m4.info
	$(INSTALL_PROGRAM) m4 $(bindir)/$(binprefix)m4
	cd $(srcdir); for file in m4.info*; do \
	  $(INSTALL_DATA) $$file $(infodir)/$$file; \
	done

uninstall:
	rm -f $(bindir)/$(binprefix)m4 $(infodir)/m4.info*

tags: TAGS

TAGS: $(HEADERS) $(SOURCES)
	etags -t $(HEADERS) $(SOURCES)

texclean:
	rm -f *.aux *.cp *.dvi *.fn *.ky *.log *.pg *.toc *.tp *.vr

mostlyclean: mostlyclean-recursive mostlyclean-local

mostlyclean-recursive:
	cd lib; $(MAKE) $(MFLAGS) mostlyclean
	cd checks; $(MAKE) $(MFLAGS) mostlyclean

mostlyclean-local: texclean
	rm -f *.o core *~

clean: clean-recursive clean-local

clean-recursive:
	cd lib; $(MAKE) $(MFLAGS) clean
	cd checks; $(MAKE) $(MFLAGS) clean

clean-local: mostlyclean-local
	rm -f m4 _*

distclean: distclean-recursive distclean-local

distclean-recursive:
	cd lib; $(MAKE) $(MFLAGS) distclean
	cd checks; $(MAKE) $(MFLAGS) distclean

distclean-local: clean-local
	rm -f Makefile config.status ansi2knr TAGS

realclean: realclean-recursive realclean-local

realclean-recursive:
	cd lib; $(MAKE) $(MFLAGS) realclean
	cd checks; $(MAKE) $(MFLAGS) realclean

realclean-local: distclean-local
	rm -f m4.info* checks/[0-9][0-9].*

dist: $(DISTFILES)
	pwd | sed 's|.*/||' > .fname
	rm -rf `cat .fname`
	mkdir `cat .fname`
	chmod 777 `cat .fname`
	ln $(DISTFILES) `cat .fname`
	cd lib; $(MAKE) $(MFLAGS) dist
	cd checks; $(MAKE) $(MFLAGS) dist
	cp -rl examples `cat .fname`
	chmod -R a+r `cat .fname`
	chmod -R a+w `cat .fname`/examples
	tar chozf `cat .fname`.tar.gz `cat .fname`
	rm -rf `cat .fname` .fname

Makefile: Makefile.in config.status
	$(SHELL) config.status
config.status: configure
	$(SHELL) config.status --recheck
configure: configure.in aclocal.m4
	cd $(srcdir); autoconf

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
