# Makefile for the fontutils library.
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

library = lib

# If malloc.c gives you trouble, just remove it.  The system malloc will
# probably work well enough.
malloc = malloc

# The headers are in the `include' directory, so don't use `c_and_h' for
# anything.
c_only = $(malloc) atou basename bb-list bb-outline bitmap bounding-box	\
charcode charspec cmdline concat concat3 concat4 concat5 dir-p dlsym	\
edge encoding extend-fname float-ok fmod file-input file-output file-p	\
filename find-suffix fix-num font getopt getopt1 hexify integer-ok	\
libfile line list logreport make-prefix make-suffix math now numtoa	\
pathsrch rand remove-suffx report safe-free scaled-num spline str-lcase	\
strstr str-to-bit substring varstring vector xcalloc xfopen xfseek	\
xftell xmalloc xmessage xopendir xrealloc xrename xstat xstrdup

include ../data/defs.make
include ../data/defslib.make

include M.depend
