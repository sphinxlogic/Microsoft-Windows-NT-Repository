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

ifeq ($(subdir),posix)
ifeq ($(sysdep_dir)/unix/common/,\
      $(dir $(firstword $(wildcard $(+sysdep_dirs:%=%/__getgrps.[cSs])))))
sysdep_routines := $(sysdep_routines) bsd_getgrp
endif
endif

ifeq ($(subdir),ctype)

sysdep_routines := $(sysdep_routines) ctype-glue

generated := $(generated) $(objpfx)glue-ctype $(objpfx)ctype-glue.c

$(objpfx)ctype-glue.c: $(objpfx)glue-ctype
	@rm -f $@
	$(dir $<)$(notdir $<) > $@-t
	mv $@-t $@

$(objpfx)glue-ctype: $(sysdep_dir)/unix/common/glue-ctype.c
	$(native-compile)

endif
