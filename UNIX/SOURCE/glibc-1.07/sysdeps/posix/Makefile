# Copyright (C) 1991, 1992, 1993 Free Software Foundation, Inc.
# This file is part of the GNU C Library.

# The GNU C Library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public License as
# published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.

# The GNU C Library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.

# You should have received a copy of the GNU Library General Public
# License along with the GNU C Library; see the file COPYING.LIB.  If
# not, write to the Free Software Foundation, Inc., 675 Mass Ave,
# Cambridge, MA 02139, USA.

$(common-objpfx)stdio_lim.h: $(common-objpfx)mk-stdiolim
	$(dir $<)$(notdir $<) > $@-t
	mv $@-t $@

ifdef subdir
objdir-CPPFLAGS = $(CPPFLAGS) # Already has appropriate `..'s.
else
objdir-CPPFLAGS = $(patsubst -I/..//%,-I/%,$(CPPFLAGS:-I%=-I../%))
endif
# Turn into a version that works when cd'd into $(objdir).
cded-objdir-CPPFLAGS = $(patsubst -I$$cwd//%,-I/%,\
				  $(patsubst -I%,-I$$cwd/%,$(CPPFLAGS)))
# The point of this is to not use -W options to gcc.
$(common-objpfx)mk-stdiolim: $(sysdep_dir)/posix/mk-stdiolim.c \
			     posix1_lim.h local_lim.h
	cwd=`pwd`; cd $(common-objdir); $(CC) $(cded-objdir-CPPFLAGS) \
	  $$cwd/$< -o $(patsubst $(common-objpfx)%,%,$@)


generated := $(generated) $(common-objpfx)stdio_lim.h \
			  $(common-objpfx)mk-stdiolim
before-compile := $(before-compile) $(common-objpfx)stdio_lim.h
