# Copyright (C) 1992, 1993 Free Software Foundation, Inc.
# This file is part of the GNU C Library.

# The GNU C Library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public License
# as published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.

# The GNU C Library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.

# You should have received a copy of the GNU Library General Public
# License along with the GNU C Library; see the file COPYING.LIB.  If
# not, write to the Free Software Foundation, Inc., 675 Mass Ave,
# Cambridge, MA 02139, USA.

ifeq	($(subdir),math)
ifndef	math-twiddled

routines:= $(filter-out acos asin cos sin hypot,$(routines)) \
	   sincos asincos
aux	:= $(aux) exp__E log__L

math-twiddled := t
endif

endif

ifeq (,$(filter-out $(sysdep_dir)/stub/ $(common-objpfx),\
	$(dir $(firstword $(wildcard $(+sysdep_dirs:%=%/bytesex.h))))))

$(common-objpfx)bytesex.h: $(common-objpfx)det_endian
	$(dir $<)$(notdir $<) > $@-tmp
	mv $@-tmp $@

$(common-objpfx)det_endian: $(sysdep_dir)/generic/det_endian.c
	$(native-compile)

before-compile := $(before-compile) $(common-objpfx)bytesex.h
generated := $(generated) $(common-objpfx)bytesex.h $(common-objpfx)det_endian

endif

ifeq ($(subdir),stdio)

ifeq	"$(filter $(objpfx)siglist.c,$(before-compile))" ""
before-compile := $(before-compile) $(objpfx)siglist.c
$(objpfx)siglist.c: $(objpfx)make_siglist
	@rm -f $@
	$(dir $<)$(notdir $<) > $@-tmp
	mv $@-tmp $@

$(objpfx)make_siglist: $(sysdep_dir)/generic/make_siglist.c
	$(native-compile)

generated := $(generated) $(objpfx)make_siglist $(objpfx)siglist.c
endif

endif
