# Generated automatically from build.sh.in by configure.
#!/bin/sh

# Shell script to build GNU Make in the absence of any `make' program.

# Copyright (C) 1993 Free Software Foundation, Inc.
# This file is part of GNU Make.
# 
# GNU Make is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# GNU Make is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Make; see the file COPYING.  If not, write to
# the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

# See Makefile.in for comments describing these variables.

srcdir='.'
CC='gcc'
CFLAGS='-g -O3'
LDFLAGS='-g'
defines='-DHAVE_CONFIG_H -DLIBDIR="${libdir}" -DINCLUDEDIR="${includedir}"'
ALLOCA=''
LOADLIBES=' -lelf'
extras=' getloadavg.o'
REMOTE='stub'

# Common prefix for machine-independent installed files.
prefix=/opt
# Common prefix for machine-dependent installed files.
exec_prefix=${prefix}
# Directory to find libraries in for `-lXXX'.
libdir=${exec_prefix}/lib
# Directory to search by default for included makefiles.
includedir=${prefix}/include

# Exit as soon as any command fails.
set -e

# These are all the objects we need to link together.
objs="commands.o job.o dir.o file.o misc.o main.o read.o remake.o rule.o implicit.o default.o variable.o expand.o function.o vpath.o version.o ar.o arscan.o signame.o getopt.o getopt1.o glob/glob.o glob/fnmatch.o remote-${REMOTE}.o ${extras} ${ALLOCA}"

# Compile the source files into those objects.
for file in `echo ${objs} | sed 's/\.o/.c/g'`; do
  echo compiling ${file}...
  $CC $CFLAGS $defines -c -I. -I${srcdir} -I${srcdir}/glob ${srcdir}/$file
done

# The object files were actually all put in the current directory.
# Remove the source directory names from the list.
srcobjs="$objs"
objs=
for obj in $srcobjs; do
  objs="$objs `basename $obj`"
done

# Link all the objects together.
echo linking make...
$CC $LDFLAGS $objs $LOADLIBES -o make.new
echo done
mv -f make.new make
