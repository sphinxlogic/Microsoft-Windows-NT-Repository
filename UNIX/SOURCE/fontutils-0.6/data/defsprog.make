# Additional definitions for programs, to be included after defs.make
# and the definitions of $(sources) and $(program).
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

all: $(program)

install:: $(program)
	$(INSTALL_PROGRAM) $(program) $(bindir)/$(program)

# Make sure all the libraries this program needs are built.
$(program): $(objects) $(ourlibs)
	$(CC) -o $(program) $(LDFLAGS) $(objects) $(LOADLIBES)

# Make a TAGS file for Emacs.
librarytags := $(patsubst %,../%/*.[hc], $(libraries) lib)
TAGS:
	etags -t $(headers) $(sources) $(addsuffix .y, $(y))
	etags -ta ../include/*.h
	etags -ta $(librarytags)
#
# Even though etags does create a file named `TAGS', call it .PHONY so
# that the rules will always be run.
.PHONY: TAGS

# Program directories have additional files to distribute.
dist::
	ln .gdbinit TAGS $(distdir)
	find \( -name \*.y \) -exec ln '{}' $(distdir)/'{}' \;

mostlyclean::
	rm -f $(program)
