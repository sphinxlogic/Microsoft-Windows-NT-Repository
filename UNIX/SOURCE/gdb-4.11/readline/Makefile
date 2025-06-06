# This file was generated automatically by configure.  Do not edit.
VPATH = .
links = sysdep.h
host_alias = i386-univel-sysv4.2
host_cpu = i386
host_vendor = univel
host_os = sysv4.2
host_canonical = i386-univel-sysv4.2
target_alias = i386-univel-sysv4.2
target_cpu = i386
target_vendor = univel
target_os = sysv4.2
target_canonical = i386-univel-sysv4.2
host_makefile_frag = config/mh-sysv4
#
# Makefile
#   Copyright (C) 1990, 1991, 1992 Free Software Foundation
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

#
# Makefile for readline and history libraries.
#

srcdir = .

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
docdir = $(datadir)/doc

SHELL = /bin/sh

INSTALL = install -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL)

AR = ar
AR_FLAGS = qv
CFLAGS = -g
BISON = bison
MAKEINFO = makeinfo
RANLIB = ranlib

# Define TYPES as -DVOID_SIGHANDLER if your operating system uses
# a return type of "void" for signal handlers.
TYPES = -DVOID_SIGHANDLER

# HP-UX compilation requires the BSD library.
#LOCAL_LIBS = -lBSD

# Xenix compilation requires -ldir -lx
#LOCAL_LIBS = -ldir -lx

# Comment out "-DVI_MODE" if you don't think that anyone will ever desire
# the vi line editing mode and features.
READLINE_DEFINES = $(TYPES) -DVI_MODE

DEBUG_FLAGS = $(MINUS_G)
LDFLAGS = $(DEBUG_FLAGS) 

# A good alternative is gcc -traditional.
#CC = gcc -traditional
RM = rm
CP = cp

LOCAL_INCLUDES = -I$(srcdir)/../

CSOURCES = readline.c history.c funmap.c keymaps.c vi_mode.c \
	   emacs_keymap.c vi_keymap.c

HSOURCES = readline.h chardefs.h history.h keymaps.h
SOURCES  = $(CSOURCES) $(HSOURCES)

DOCUMENTATION = readline.texi inc-read.texi \
		history.texi inc-hist.texi

SUPPORT = COPYING Makefile $(DOCUMENTATION) ChangeLog

THINGS_TO_TAR = $(SOURCES) $(SUPPORT)

#### Host, target, and site specific Makefile fragments come in here.
# Define USG as -DUSG if you are using a System V operating system.
# Define USGr4 also if you are on an SVR4 system.
USG = -DUSG -DUSGr4
RANLIB = true
INSTALL_PROG = /usr/ucb/install -c

# The l flag generates a warning from the SVR4 archiver, remove it.
AR_FLAGS = cq
###

.c.o:
	$(CC) -c $(CFLAGS) $(H_CFLAGS) $(DEBUG_FLAGS) $(USG) -I. $(LOCAL_INCLUDES) $(CPPFLAGS) $(READLINE_DEFINES) $<

STAGESTUFF = *.o

all: libreadline.a

check:
installcheck:
info:
dvi:

clean-info: force
	-rm -f *.info*

history.info: $(srcdir)/history.texi
	$(MAKEINFO) -o history.info $(srcdir)/history.texi

readline.info: $(srcdir)/readline.texi $(srcdir)/inc-read.texi
	$(MAKEINFO) -o readline.info $(srcdir)/readline.texi

libreadline.a:	readline.o history.o funmap.o keymaps.o tilde.o vi_mode.o
	$(RM) -f libreadline.a
	$(AR) $(AR_FLAGS) libreadline.a readline.o history.o funmap.o keymaps.o tilde.o vi_mode.o
	$(RANLIB) libreadline.a

readline.o:	readline.h chardefs.h  keymaps.h history.h readline.c vi_mode.c
history.o:	history.c history.h
funmap.o:	readline.h
keymaps.o:	emacs_keymap.c vi_keymap.c keymaps.h chardefs.h keymaps.c

tilde.o:	$(srcdir)/../glob/tilde.c
	$(CC) -c $(CFLAGS) $(H_CFLAGS) $(DEBUG_FLAGS) $(USG) -I. $(LOCAL_INCLUDES) $(CPPFLAGS) $(READLINE_DEFINES) $(srcdir)/../glob/tilde.c

libtest:	libreadline.a libtest.c
	$(CC) -o libtest $(CFLAGS) $(H_CFLAGS) $(DEBUG_FLAGS) $(USG) -I. $(LOCAL_INCLUDES) $(CPPFLAGS) $(READLINE_DEFINES) -L. libtest.c -lreadline -ltermcap

readline: readline.c history.o keymaps.o funmap.o readline.h chardefs.h vi_mode.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $(READLINE_DEFINES) \
		$(LOCAL_INCLUDES) -DTEST -o readline readline.c funmap.o \
		 keymaps.o history.o vi_mode.o -L. -ltermcap

readline.tar:	$(THINGS_TO_TAR)
		tar -cf readline.tar $(THINGS_TO_TAR)

readline.tar.Z:	readline.tar
		compress -f readline.tar

install:
		$(INSTALL_DATA) libreadline.a $(libdir)/libreadline.a
		$(RANLIB) $(libdir)/libreadline.a
		$(INSTALL_DATA) $(srcdir)/readline.h $(includedir)/readline/readline.h
		$(INSTALL_DATA) $(srcdir)/keymaps.h $(includedir)/readline/keymaps.h
		$(INSTALL_DATA) $(srcdir)/chardefs.h $(includedir)/readline/chardefs.h

install-info: info
#		for i in *.info* ; do \
#			$(INSTALL_DATA) $$i $(infodir)/$$i ; \
#		done

includes:
		$(INSTALL_FILE) $(srcdir)/readline.h $(includedir)/readline/readline.h
		$(INSTALL_FILE) $(srcdir)/keymaps.h $(includedir)/readline/keymaps.h
		$(INSTALL_FILE) $(srcdir)/chardefs.h $(includedir)/readline/chardefs.h

clean mostlyclean:
		rm -f $(STAGESTUFF) *.a *.log *.cp *.tp *.vr *.fn
		rm -f *.aux *.pg *.toc

distclean realclean: clean
	rm -f Makefile config.status sysdep.h

$(DESTDIR)/libreadline.a: libreadline.a

# Copy the object files from a particular stage into a subdirectory.
stage1: force
	-mkdir stage1
	-mv $(STAGESTUFF) stage1

stage2: force
	-mkdir stage2
	-mv $(STAGESTUFF) stage2

stage3: force
	-mkdir stage3
	-mv $(STAGESTUFF) stage3

against=stage2

comparison: force
	for i in $(STAGESTUFF) ; do cmp $$i $(against)/$$i ; done

de-stage1: force
	-(cd stage1 ; mv -f * ..)
	-rmdir stage1

de-stage2: force
	-(cd stage2 ; mv -f * ..)
	-rmdir stage2

de-stage3: force
	-(cd stage3 ; mv -f * ..)
	-rmdir stage3

force:

# with the gnu make, this is done automatically.

Makefile: $(srcdir)/Makefile.in $(host_makefile_frag) $(target_makefile_frag)
	$(SHELL) ./config.status
