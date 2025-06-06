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

# The unix-specific configure fragment writes `unix-generated' in config.make.
ifdef objdir
unix-generated-pfx = $(objpfx)
else
unix-generated-pfx = $(sysdep_dir)/unix/
endif
generated := $(generated) $(addprefix $(unix-generated-pfx),$(unix-generated))

ifeq (,$(filter-out $(sysdep_dir)/stub/ $(common-objpfx),\
	$(dir $(firstword $(wildcard $(+sysdep_dirs:%=%/local_lim.h))))))

ifneq (,$(wildcard $(sysincludedir)/sys/param.h))
local_lim-CFLAGS := $(local_lim-CFLAGS) -DHAVE_SYS_PARAM_H
endif
ifneq (,$(wildcard $(sysincludedir)/sys/limits.h))
local_lim-CFLAGS := $(local_lim-CFLAGS) -DHAVE_SYS_LIMITS_H
endif
ifneq (,$(wildcard $(sysincludedir)/limits.h))
local_lim-CFLAGS := $(local_lim-CFLAGS) -DHAVE_LIMITS_H
endif

$(common-objpfx)local_lim.h: $(common-objpfx)mk-local_lim
	if $(dir $<)$(notdir $<) > $@-t; then mv $@-t $@; else XXX; fi
$(common-objpfx)mk-local_lim: $(sysdep_dir)/unix/mk-local_lim.c
	cwd=`pwd`; cd $(@D); \
	$(CC) $(native-CFLAGS) $(local_lim-CFLAGS) $$cwd/$< -o $(@F)

before-compile := $(before-compile) $(common-objpfx)local_lim.h
generated := $(generated) \
	     $(common-objpfx)local_lim.h $(common-objpfx)mk-local_lim

endif

ifeq (,$(filter-out $(sysdep_dir)/stub/ $(common-objpfx),\
	$(dir $(firstword $(wildcard $(+sysdep_dirs:%=%/sys/param.h))))))

before-compile := $(before-compile) $(common-objpfx)sys/param.h

$(common-objpfx)sys/param.h: $(sysincludedir)/sys/param.h
	-mkdir $(@D:%/=%)
	(echo '#ifndef _GNU_SYS_PARAM_H';	\
	 echo '#define _GNU_SYS_PARAM_H 1';	\
	 echo '#include <endian.h>';		\
	 awk < $<			\
	   '/^#define[ 	]*NULL/ { print "#ifndef NULL";	\
				  print $$0;		\
				  print "#endif";	\
				  was_null = 1 }	\
	    { if (!was_null) print $$0;			\
	      was_null = 0 }';		\
	 echo '#ifndef MAXHOSTNAMELEN'; \
	 echo '#define MAXHOSTNAMELEN 64 /* XXX */'; \
	 echo '#endif /* No MAXHOSTNAMELEN.  */'; \
	 echo '#endif	/* sys/param.h */') > $@-tmp
	mv $@-tmp $@

$(common-objpfx)param.h.c: $(sysincludedir)/sys/param.h
	rm -f $@
	cp $< $@
$(common-objpfx)param.h.dep: $(common-objpfx)param.h.c \
			     $(sysdep_dir)/unix/Makefile
	$(+mkdep) -I$(sysincludedir) $<		\
	| sed > $@				\
	      -e 's@^.*:@@'			\
	      -e 's@$<@@g'			\
	      -e 's@$(sysincludedir)/*@@g'	\
	      -e 's@\\$$@@'			\
	      -e 's@^@sys/param.h-includes := $$(sys/param.h-includes) @'

# Get the generated definition of sys/param.h-includes.
include $(common-objpfx)param.h.dep

# Don't preempt our own headers. 
sys/param.h-includes := \
  $(filter-out $(patsubst $(..)%,%,\
			  $(wildcard $(addprefix $(..),\
				     $(sys/param.h-includes)))),\
	       $(sys/param.h-includes))


.PHONY: sys/param.h-includes
sys/param.h-includes: $(addprefix $(common-objpfx),$(sys/param.h-includes))

ifdef sys/param.h-includes
# Copy the system files to $(common-objdir).
$(addprefix $(common-objpfx),$(sys/param.h-includes)): $(common-objpfx)%: \
						       $(sysincludedir)/%
	-mkdir $(@D:%/=%)
# Some of these files sometimes contain conflicting declarations for htons,
# ntohs, etc.  They also often contain definitions of *_ENDIAN and
# BYTE_ORDER, which we define ourselves in <endian.h>.
	sed -e '/[ 	]*[hn]to[nh][sl][	 (]*/d' \
	    -e '/^#define[ 	]*[A-Z]*_ENDIAN/d' \
	    -e '/^#define[ 	]*BYTE_ORDER/d' \
	    $< > $@-t
	mv $@-t $@
endif

generated := $(generated) $(addprefix $(common-objpfx),\
	     sys/param.h $(sys/param.h-includes) param.h.c param.h.dep)


ifeq ($(subdir),misc)

# Install system headers the system sys/param.h uses.

sysdep_headers := $(sysdep_headers) sys/param.h $(sys/param.h-includes)

endif # misc

endif # No sysdep sys/param.h.

ifeq (,$(filter-out $(sysdep_dir)/stub/ $(common-objpfx),\
	$(dir $(firstword $(wildcard $(+sysdep_dirs:%=%/errnos.h))))))

# These need to exist before any compiling is done,
# so cpp doesn't instead find the stub versions.
before-compile := $(before-compile) $(common-objpfx)errnos.h 

$(common-objpfx)errnos.h: $(common-objpfx)make-errnos
	$(dir $<)$(notdir $<) > $@-tmp
	mv $@-tmp $@

$(common-objpfx)make-errnos: $(common-objpfx)make-errnos.c
	$(common-objdir-compile)

$(common-objpfx)make-errnos.c: $(sysdep_dir)/unix/errnos-tmpl.c \
			$(sysdep_dir)/unix/errnos.awk $(common-objpfx)errnos
	awk -f $(word 2,$^) errnos="`tr '\012' ' ' < $(word 3,$^)`" $< > $@T
	mv $@T $@

$(common-objpfx)errnos: $(wildcard $(sysincludedir)/errno.h	\
				   $(sysincludedir)/sys/errno.h)
	cat $^ | sed -n 's/^#define[ 	]*\(E[A-Z0-9][A-Z0-9]*\)[ 	].*$$/\1/p' > $@-tmp
	mv $@-tmp $@

generated := $(generated) $(addprefix $(common-objpfx),\
	     errnos.h errnos make-errnos make-errnos.c)
endif

ifeq (,$(filter-out $(sysdep_dir)/stub/ $(common-objpfx),\
	$(dir $(firstword $(wildcard $(+sysdep_dirs:%=%/ioctls.h))))))

before-compile := $(before-compile) $(common-objpfx)ioctls.h

$(common-objpfx)ioctls.h: $(common-objpfx)make-ioctls
	$(dir $<)$(notdir $<) > $@-tmp
	mv $@-tmp $@

sys/termios.h := $(wildcard $(sysincludedir)/sys/termios.h)
ifneq (,$(strip $(sys/termios.h)))
make-ioctls-CFLAGS := -DHAVE_SYS_TERMIOS_H
endif
$(common-objpfx)make-ioctls: $(common-objpfx)make-ioctls.c
	cd $(@D); $(CC) $(native-CFLAGS) $(make-ioctls-CFLAGS) \
			$(<:$(common-objpfx)%=%) -o $(@F)

$(common-objpfx)make-ioctls.c: $(sysdep_dir)/unix/ioctls-tmpl.c \
			       $(sysdep_dir)/unix/ioctls.awk \
			       $(common-objpfx)ioctls
	awk -f $(word 2,$^) requests="`cat $(word 3,$^)`" $< > $@T
	mv $@T $@


termbits.h := $(firstword $(wildcard $(+sysdep_dirs:%=%/termbits.h)))
ifeq ($(termbits.h),$(sysdep_dir)/generic/termbits.h)
termbits.h := # Ignore the generic version.
endif

# If there is a system-specific <termbits.h> file, we want to omit all the
# symbols it defines from ioctls.  Otherwise, both ioctls.h and termbits.h
# would define them.  The system-specific <termbits.h> file presumably
# defines them with the same values as we find from the system's headers.
# We also want to omit from ioctls the symbols defined in our own
# <sys/ttydefaults.h>, to avoid multiple definition conflicts.  We use
# snarf-ioctls on these files to find what symbols we want to omit.  fgrep
# -xv gives all lines which do not match in their entirety; without -x,
# CSTOP's presence elided TIOCSTOP.

$(common-objpfx)ioctls: $(sysdep_dir)/unix/snarf-ioctls \
			$(sysincludedir)/sys/ioctl.h $(sys/termios.h)
	$(dir $<)$(notdir $<) $(filter-out $<,$^) \
	| fgrep -xv "`$(dir $<)$(notdir $<) $(termbits.h) \
					    $(..)termios/sys/ttydefaults.h \
		     | sort | uniq`" \
	| sort | uniq | tr '\012' ' ' > $@-tmp
	mv $@-tmp $@

generated := $(generated) $(addprefix $(common-objpfx),\
	     ioctls.h ioctls make-ioctls make-ioctls.c)
endif

ifeq ($(subdir),stdio)
ifeq (,$(filter-out $(sysdep_dir)/stub/ $(common-objpfx),\
	$(dir $(firstword $(wildcard $(+sysdep_dirs:%=%/errlist.c))))))

before-compile: $(objpfx)errlist.c
$(objpfx)errlist.c: $(objpfx)make_errlist
	@rm -f $@
	$(dir $<)$(notdir $<) > $@-tmp
	mv $@-tmp $@

$(objpfx)make_errlist: $(sysdep_dir)/unix/make_errlist.c
	$(native-compile)

generated := $(generated) $(addprefix $(objpfx),make_errlist errlist.c)

endif
endif # stdio

ifeq (,$(filter-out $(sysdep_dir)/stub/ $(common-objpfx),\
	$(dir $(firstword $(wildcard $(+sysdep_dirs:%=%/syscall.h))))))

# The syscall code assumes a file <syscall.h> that defines macros
# `SYS_call' for syscall `call'.  Variations on this I have seen include:
# it's in <sys/syscall.h>;
# it's in <sys.s>;
# it defines `CALL' instead of `SYS_call'.

# Find a file that might have these.
syscall.h := $(firstword $(wildcard $(addprefix $(sysincludedir)/, \
						syscall.h sys/syscall.h \
						sys.s sys/sys.s \
						sys.S sys/sys.S)))
ifdef syscall.h

# Transmogrify any of several formats of the file into the one we want.
$(common-objpfx)syscall.h: $(syscall.h)
	tr '[A-Z]' '[a-z]' < $< | \
	sed -e 's/[ 	]sys_/ /' \
	    -e 's/^#define[ 	]*\([a-z0-9_]*\)[ 	]*/#define SYS_\1 /' \
	    -e 's/[ 	]sys_/ SYS_/' \
	    -e 's/SYS_syscall_basenum/syscall_basenum/g' \
	    -e '/^#/!d' \
	    -e '/^#ident/d' \
	    -e 's-\(/\*[^*]*\)$$-\1\*/-' \
	> $@-tmp
	mv $@-tmp $@

before-compile := $(before-compile) $(common-objpfx)syscall.h
generated := $(generated) $(common-objpfx)syscall.h

endif

endif
