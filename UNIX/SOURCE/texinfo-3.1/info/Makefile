# Generated automatically from Makefile.in by configure.
# Makefile for texinfo/info.	-*- Indented-Text -*-
# Copyright (C) 1993 Free Software Foundation, Inc.

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

#### Start of system configuration section. ####

srcdir = .
VPATH  = $(srcdir):$(common)

common = $(srcdir)/../libtxi

CC = gcc

INSTALL = /opt/bin/install -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644

LN	= ln
RM	= rm -f
TAR	= tar
MKDIR	= mkdir
MAKEINFO= ../makeinfo/makeinfo
COMPRESS= compress

DEFS =  -DSTDC_HEADERS=1 -DHAVE_UNISTD_H=1 -DHAVE_TERMIO_H=1 -DHAVE_STRING_H=1 -DHAVE_VARARGS_H=1 -DHAVE_SYS_TIME_H=1 -DHAVE_SYS_FCNTL_H=1 -DHAVE_SYS_TTOLD_H=1 -DHAVE_SYS_PTEM_H=1 -DHAVE_SETVBUF=1 -DHAVE_GETCWD=1 -DHAVE_VFPRINTF=1 -DHAVE_VSPRINTF=1

LDEFS = -DNAMED_FUNCTIONS=1 -DDEFAULT_INFOPATH='"$(DEFAULT_INFOPATH)"'

TERMLIBS =  -ltermcap
LIBS = $(TERMLIBS) -L../libtxi -ltxi 
LOADLIBES = $(LIBS)

SHELL = /bin/sh

CFLAGS = -g
LDFLAGS = -g

prefix = /opt
exec_prefix = ${prefix}
bindir = $(exec_prefix)/bin
# Prefix for each installed program, normally empty or `g'.
binprefix = 
libdir = $(prefix)/lib
# Prefix for each installed man page, normally empty or `g'.
manprefix = 
mandir = $(prefix)/man/man1
manext = 1
infodir = $(prefix)/info

#### End of system configuration section. ####

SRCS =	dir.c		display.c	echo_area.c	filesys.c \
	info-utils.c	info.c		infodoc.c	infomap.c \
	m-x.c		nodes.c		search.c	session.c \
	signals.c	terminal.c	tilde.c		window.c \
	xmalloc.c	indices.c	makedoc.c	nodemenu.c \
	footnotes.c	dribble.c	variables.c	gc.c

HDRS =	display.h	doc.h		echo_area.h	filesys.h \
	general.h	getopt.h	info-utils.h	info.h \
	infomap.h	nodes.h		search.h 	session.h \
	signals.h	termdep.h	terminal.h	tilde.h \
	indices.h	window.h	footnotes.h	dribble.h \
	variables.h	gc.h

OBJS =	dir.o display.o doc.o echo_area.o filesys.o info-utils.o info.o \
	infodoc.o infomap.o m-x.o nodes.o search.o session.o signals.o \
	terminal.o tilde.o window.o indices.o xmalloc.o nodemenu.o \
	footnotes.o dribble.o variables.o gc.o 

# The names of files which declare info commands.
CMDFILES = $(srcdir)/session.c $(srcdir)/echo_area.c $(srcdir)/infodoc.c \
	   $(srcdir)/m-x.c $(srcdir)/indices.c $(srcdir)/nodemenu.c \
	   $(srcdir)/footnotes.c $(srcdir)/variables.c

# The name of the program which builds documentation structure from CMDFILES.
MAKEDOC_OBJECTS = makedoc.o xmalloc.o
MAKEDOC_SOURCE = makedoc.c xmalloc.c

.c.o:
	$(CC) -c $(CPPFLAGS) $(LDEFS) $(DEFS) -I. -I$(srcdir) -I$(common) $(CFLAGS) $<

all: info info.info info-stnd.info
sub-all: all

install: all
	$(INSTALL_PROGRAM) info $(bindir)/info
	-d=$(srcdir); test -f ./info.info && d=.; $(INSTALL_DATA) $$d/info.info $(infodir)/info.info
	-d=$(srcdir); test -f ./info-stnd.info && d=.; $(INSTALL_DATA) $$d/info-stnd.info $(infodir)/info-stnd.info
	-$(INSTALL_DATA) $(srcdir)/info.1 $(mandir)/info.$(manext)

uninstall:
	rm -f $(bindir)/info
	rm -f $(infodir)/info.info
	rm -f $(infodir)/info-stnd.info
	rm -f $(mandir)/info.$(manext)

info: $(OBJS) ../libtxi/libtxi.a
	$(CC) $(LDFLAGS) -o info $(OBJS) $(LOADLIBES)

info.info: info.texi
	$(MAKEINFO) --no-split -I$(srcdir) info.texi

info-stnd.info: info-stnd.texi
	$(MAKEINFO) --no-split -I$(srcdir) info-stnd.texi

makedoc: $(MAKEDOC_OBJECTS) ../libtxi/libtxi.a
	$(CC) $(LDFLAGS) -o makedoc $(MAKEDOC_OBJECTS) $(LOADLIBES)

Makefile: $(srcdir)/Makefile.in ../config.status
	cd ..; sh config.status

clean:
	rm -f info funs.h doc.c makedoc $(OBJS) $(MAKEDOC_OBJECTS)

distclean: clean texclean
	rm -f Makefile config.status *~ core core.* *.BAK TAGS

mostlyclean: clean

realclean: distclean
	rm -f funs.h doc.c
	rm -f info.info info-stnd.info

TAGS: $(SRCS) makedoc-TAGS
	etags $(SRCS)
	cat makedoc-TAGS >>TAGS && rm -f makedoc-TAGS

makedoc-TAGS: $(CMDFILES)
	./makedoc -tags $(CMDFILES) >makedoc-TAGS

texclean:
	rm -f *.toc *.aux *.log *.cp *.fn *.tp *.vr *.pg *.ky *.cps \
	*.tps *.fns *.kys *.pgs *.vrs

check: info

# The files `doc.c' and `funs.h' are created by ./makedoc run over the source
# files which contain DECLARE_INFO_COMMAND.  `funs.h' is a header file
# listing the functions found.  `doc.c' is a structure containing pointers
# to those functions along with completable names and documentation strings.
funs.h: makedoc $(CMDFILES)
	-@if test -f funs.h; then mv -f funs.h old-funs.h; fi; :
	-@if test -f doc.c; then mv -f doc.c old-doc.c; fi; :
	./makedoc $(CMDFILES)
	-@if cmp -s old-funs.h funs.h; then mv old-funs.h funs.h; \
	  else rm -f old-funs.h; fi; :
	-@if cmp -s old-doc.c doc.c; then mv old-doc.c doc.c; \
	  else rm -f old-doc.c; fi; :

doc.c: 		funs.h
dribble.o:	dribble.c dribble.h
display.o:	display.c
echo_area.o:	echo_area.c
filesys.o:	filesys.c
info-utils.o:	info-utils.c
info.o:		info.c
infodoc.o:	infodoc.c
infomap.o:	infomap.c
m-x.o:		m-x.c
nodes.o:	nodes.c
search.o:	search.c
session.o:	session.c
signals.o:	signals.c
terminal.o:	terminal.c
tilde.o:	tilde.c
window.o:	window.c
xmalloc.o:	xmalloc.c
indices.o:	indices.c
makedoc.o:	makedoc.c

dir.o: 		dir.c
display.o: 	nodes.h info-utils.h search.h
display.o: 	terminal.h window.h display.h
echo_area.o: 	info.h
filesys.o: 	general.h tilde.h filesys.h
footnotes.o: 	footnotes.h
info-utils.o: 	info-utils.h nodes.h search.h
info.o: 	info.h $(common)/getopt.h
infodoc.o: 	info.h doc.h
infomap.o: 	infomap.h funs.h
gc.o:		info.h
m-x.o: 		info.h
nodes.o: 	search.h filesys.h
nodes.o: 	nodes.h info-utils.h
search.o: 	general.h search.h nodes.h
session.o: 	info.h
signals.o: 	info.h signals.h
terminal.o: 	terminal.h termdep.h
tilde.o: 	tilde.h
variables.c: 	variables.h
window.o: 	nodes.h window.h display.h
window.o: 	info-utils.h search.h infomap.h

# Prevent GNU make v3 from overflowing arg limit on SysV.
.NOEXPORT:

# eof
