# Generated automatically from Makefile.in by configure.
# Makefile for GNU grep
# Copyright (C) 1992 Free Software Foundation, Inc.

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

#### Start of system configuration section. ####

srcdir=.
VPATH=.

AWK=gawk
INSTALL=/usr/ucb/install -c
INSTALL_PROGRAM=$(INSTALL)
INSTALL_DATA=$(INSTALL) -m 644

CC=gcc
LINT=lint

# Things you might add to DEFS:
# -DSTDC_HEADERS	If you have ANSI C headers and libraries.
# -DHAVE_UNISTD_H	If you have unistd.h.
# -DUSG			If you have System V/ANSI C string
#			and memory functions and headers.
# -D__CHAR_UNSIGNED__	If type `char' is unsigned.
#			gcc defines this automatically.
DEFS=-DGREP  -DSTDC_HEADERS=1 -DHAVE_STRING_H=1 -DHAVE_SYS_PARAM_H=1 -DHAVE_UNISTD_H=1 -DHAVE_MEMCHR=1 -DHAVE_STRERROR=1 -DHAVE_VALLOC=1 -DHAVE_WORKING_MMAP=1

# Extra libraries.
LIBS=
ALLOCA=

CFLAGS=-O3 -g
LDFLAGS=$(CFLAGS)

prefix=/opt
exec_prefix=${prefix}

# Prefix for installed program, normally empty or `g'.
binprefix=
# Prefix for installed man page, normally empty or `g'.
manprefix=

# Where to install executables.
bindir=$(exec_prefix)/bin

# Where to install man pages.
mandir=$(prefix)/man/man1

# Extension for man pages.
manext=1

# How to make a hard link.
LN=ln

#### End of system configuration section. ####

SRCS=grep.c getopt.c regex.c dfa.c kwset.c obstack.c search.c
OBJS=grep.o getopt.o regex.o dfa.o kwset.o obstack.o search.o

.c.o:
	$(CC) $(CFLAGS) $(DEFS) -I$(srcdir) -c $<

all: grep check.done

# For Saber C.
grep.load: $(SRCS)
	#load $(CFLAGS) $(DEFS) -I$(srcdir) (SRCS)

# For Lint.
grep.lint: $(SRCS)
	$(LINT) $(CFLAGS) $(DEFS) -I$(srcdir) $(SRCS)

install: all
	$(INSTALL_PROGRAM) grep $(bindir)/$(binprefix)grep
	rm -f $(bindir)/$(binprefix)egrep
	$(LN) $(bindir)/$(binprefix)grep $(bindir)/$(binprefix)egrep
	rm -f $(bindir)/$(binprefix)fgrep
	$(LN) $(bindir)/$(binprefix)grep $(bindir)/$(binprefix)fgrep	
	$(INSTALL_DATA) $(srcdir)/grep.man $(mandir)/grep.$(manext)

check:
	AWK=$(AWK) sh $(srcdir)/tests/check.sh $(srcdir)/tests
	touch check.done

check.done: grep
	AWK=$(AWK) sh $(srcdir)/tests/check.sh $(srcdir)/tests
	touch check.done

grep: $(OBJS) $(LIBOBJS) $(ALLOCA)
	$(CC) $(LDFLAGS) -o grep $(OBJS) $(LIBOBJS) $(LIBS) $(ALLOCA)

clean:
	rm -f core grep *.o check.done tmp.script khadafy.out

mostlyclean: clean

distclean: clean
	rm -f Makefile config.status

realclean: distclean
	rm -f TAGS

Makefile: $(srcdir)/Makefile.in config.status
	$(SHELL) config.status

dist:
	V=`sed -n '/version\\[/s/.*\\([0-9][0-9]*\\.[0-9]*\\).*/\\1/p' \
	grep.c`; \
	mkdir grep-$$V; mkdir grep-$$V/tests; \
	for f in `awk '{print $$1}' MANIFEST`; do ln $$f grep-$$V/$$f; done; \
	tar cvhf - grep-$$V | gzip > grep-$$V.tar.z; \
	rm -fr grep-$$V

# Some header file dependencies that really ought to be automatically deduced.
dfa.o search.o: dfa.h
grep.o search.o: grep.h
kwset.o search.o: kwset.h
kwset.o obstack.o: obstack.h
regex.o search.o: regex.h
