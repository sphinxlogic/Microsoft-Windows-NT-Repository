# Common definitions for Makefiles in this directory hierarchy.
# 
# Copyright (C) 1992 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 
# A GNUmakefile in this hierarchy should first include this file, and
# then include either `defsprog.make' or `defslib.make', as appropriate.
# Dependencies are created by `make depend', which writes onto the file
# `M.depend'.  A Makefile should include that, also, at its end.


# These variables are used only when the top-level Makefile is not.
CC = gcc
CFLAGS = -g
LDFLAGS = $(XLDFLAGS)
LIBS = -lm
RANLIB = @true
srcdir = .

# Here is the rule to make `foo.o' from `foo.c'.  We want CFLAGS to be
# user-settable, so we can't use the default implicit rule.
CPPFLAGS = -I../$(srcdir)/include $(xincludedir)
%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

# Here is the rule to make `foo.c' from `foo.y' using Bison.
# (GNU make's default rule assumes $(YACC) outputs y.tab.c.)
# We always use Bison, not Yacc.
BISON = bison
BISONFLAGS = -d -t
%.c %.h: %.y
	$(BISON) $(BISONFLAGS) $< -o $*.c

# Libraries to support bitmap font formats.  Defined just so the main
# Makefiles can use $(bitmap_libs), in case we ever add more formats.
bitmap_libs = gf pk

# If the caller wants our widgets, they also want the other X libraries.
# On SunOS 4.1, dynamic linking with -lXaw fails: formWidgetClass and
# the other X widgets in ../widgets are multiply defined.  I don't
# understand why the entire library file is pulled in with dynamic linking.
# 
ifneq "$(findstring widgets, $(libraries))" ""
dlsym = ../$(srcdir)/lib/dlsym.o
X_sys_libraries = -lXaw -lXmu -lXext -lXt -lX11
# This was:
# X_libraries = -static $(xlibdir) $(X_sys_libraries) $(dlsym) $(wlibs)
# but that doesn't work with SVR4.2, since we don't have all the static
# libraries we need (Greg Lehey, LEMIS, 7 May 1993)
X_libraries = $(xlibdir) $(X_sys_libraries) $(dlsym) $(wlibs)
endif

# Compose the entire list of libraries from the defined `libraries'.  We
# always add `lib' at the beginning and the end, since everything uses that.
ourlibs := $(foreach lib,lib $(libraries) lib,../$(srcdir)/$(lib)/$(lib).a)

# This is what we will link with.
LOADLIBES = $(ourlibs) $(X_libraries) $(LIBS)

# Make the list of object files, headers, and sources.  The headers only
# matter for tags.
sources := $(addsuffix .c, $(c_and_h) $(c_only))
headers := $(addsuffix .h, $(c_and_h) $(h_only))
objects := $(addsuffix .o, $(basename $(y) $(sources)))


# These set things correctly for our development environment.
ifeq ($(HOSTNAME_FULL),hayley.fsf.org)
override CC = gcc -posix
override CFLAGS := $(CFLAGS) -Wall
override wlibs = -lnsl_s -linet
endif
ifeq ($(HOSTNAME_FULL),fosse.fsf.org)
override xincludedir = -I/opt/include
override RANLIB = ranlib
endif

# The real default target is in either defslib.make or defsprog.make.
default: all
.PHONY: default

# Make the dependency file.
# 
# We don't depend on `$(sources)' because most changes to the sources don't
# change the dependencies.
# 
# We assume at most one Bison source per directory here.  We also assume
# GCC 2 for generating the dependencies of the Bison-generated C file.
# Put the Bison stuff first in the dependency file, since typically lots
# of other files depend on the Bison .h files.
M.depend depend:
	rm -f M.depend
	if test -n "$(y)"; \
        then DEPENDENCIES_OUTPUT="M.depend $(y).o" \
               gcc -x c-header -E $(CPPFLAGS) $(y).y > /dev/null; \
        fi
	$(CC) -MM $(CPPFLAGS) $(sources) >> M.depend
.PHONY: depend

# When `make dist' is called from above, we expect $(dir) to be set to
# the directory in which  we're currently making.
distdir = ../$(top_distdir)/$(dir)

# We use `find' instead of just `ln *.c *.h' here because not all
# directories have .c and .h files.
dist::
	mkdir $(distdir)
	ln ChangeLog GNUmakefile M.depend README $(distdir)
	if test -n "$(y)"; then $(MAKE) $(y).c; fi
	find \( -name \*.c -o -name \*.h \) -exec ln '{}' $(distdir)/'{}' \;
.PHONY: dist

# Prevent GNU make version 3 from overflowing system V's arg limit.
.NOEXPORT:

mostlyclean::
# Remove most files.
	rm -f $(objects) *.output
.PHONY: mostlyclean

clean:: mostlyclean
# Remove all files that are made by `make all', etc.
	rm -f *.dvi *.log *.ps core
.PHONY: clean

distclean:: clean
# Delete all files that users would normally create from compilation and
# installation. 
.PHONY: distclean

extraclean:: distclean
# Delete anything likely to be found in the source directory
# that shouldn't be in the distribution.
	rm -f *.tfm *gf *pk *.bzr *~ *\#* patch* *.orig *.rej

realclean:: distclean
# Remove all files that can be remade with `make'.
	rm -f TAGS M.depend $(addsuffix .c, $(y)) $(addsuffix .h, $(y))
.PHONY: realclean
