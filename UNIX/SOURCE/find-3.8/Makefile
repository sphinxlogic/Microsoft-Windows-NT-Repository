# Generated automatically from Makefile.in by configure.
# Master Makefile for GNU find, xargs, and locate.
# Copyright (C) 1987, 1990, 1991 Free Software Foundation, Inc.

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

srcdir = .
VPATH = .

CC = gcc
AR = ar
# Set RANLIB = echo if your system doesn't have or need ranlib.
RANLIB = ranlib
# Use cp if you don't have install.
INSTALL = /opt/bin/install -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644

# Things you might add to DEFS:
# -DCACHE_IDS		If you want find -nouser and -nogroup to make
#			tables of used UIDs and GIDs at startup
#			instead of using getpwuid or getgrgid when
#			needed.  Speeds up -nouser and -nogroup unless
#			you are running NIS or Hesiod, which make
#			password and group calls very expensive. 
# -DSTDC_HEADERS	If you have ANSI C headers and libraries.
# -DHAVE_UNISTD_H	If you have unistd.h.
# -DHAVE_STRING_H	If you don't have ANSI C headers but have string.h.
# -DDIRENT		If you have dirent.h.
# -DSYSNDIR		Old Xenix systems (sys/ndir.h).
# -DSYSDIR		Old BSD systems (sys/dir.h).
# -DNDIR		Old System V systems (ndir.h).
# -DVOID_CLOSEDIR	If your closedir function returns void, not int.
# -DMAJOR_IN_MKDEV	If major, minor, makedev defined in sys/mkdev.h.
# -DMAJOR_IN_SYSMACROS	If major, minor, makedev defined in sys/sysmacros.h.
# -DHAVE_LIMITS_H	If you have limits.h; otherwise uses sys/param.h.
# -DHAVE_ST_BLOCKS	If your `struct stat' has st_blocks and st_blksize.
# -DHAVE_ST_RDEV	If your `struct stat' has st_rdev.
# -DHAVE_STRERROR	If you have strerror function.
# -DHAVE_VPRINTF	If you have vprintf function.
# -DHAVE_DOPRNT		If you have _doprnt function (but lack vprintf).
# -DHAVE_TM_ZONE	If you have tm_zone in struct tm; otherwise
#			uses tm_isdst and tzname.
# -DHAVE_TZNAME		If you have tm_zone and tzname but not tm_zone;
#			otherwise uses tm_isdst and timezone.
# Define zero or one of the following:
# If no FSTYPE_* is defined, -fstype treats every filesystem as type "unknown".
# -DFSTYPE_MNTENT	Use 4.3BSD getmntent to get filesystem type.
# -DFSTYPE_GETMNT	Use Ultrix getmnt to get filesystem type.
# -DFSTYPE_STATFS	Use 4.4BSD and OSF1 statfs to get filesystem type.
# -DFSTYPE_USG_STATFS	Use SVR3.2 statfs to get filesystem type.
# -DFSTYPE_AIX_STATFS	Use AIX3 statfs to get filesystem type.
# -DFSTYPE_STATVFS	Use SVR4 statvfs to get filesystem type.

DEFS =  -DMAJOR_IN_MKDEV=1 -DDIRENT=1 -DFSTYPE_STATVFS=1 -DSTDC_HEADERS=1 -DHAVE_UNISTD_H=1 -DHAVE_STRING_H=1 -DHAVE_LIMITS_H=1 -DHAVE_STRERROR=1 -DHAVE_VPRINTF=1 -DHAVE_ST_BLOCKS=1 -DHAVE_ST_RDEV=1 -DHAVE_TZNAME=1

CFLAGS = -g -O3
LDFLAGS = -g
LIBS = 

prefix = /opt
exec_prefix = ${prefix}

# Prefix for each installed program, normally empty or `g'.
binprefix = 
# Prefix for each installed man page, normally empty or `g'.
manprefix = 

# Where to install `find' and `locate'.
bindir = $(exec_prefix)/bin

# Where to install programs to create the `locate' database.
libdir = $(exec_prefix)/lib/locate

# Where to install the `locate' database.
datadir = $(prefix)/lib/locate

# Name of the `locate' database file.
LOCATE_DB = $(datadir)/find.codes

# Where to install manual pages.
mandir = $(prefix)/man/man1
# Extension (not including `.') for the installed manual page filenames.
manext = 1

#### End of system configuration section. ####

# Arguments to pass to subdirectory makes.
MDEFINES = bindir='$(bindir)' libdir='$(libdir)' datadir='$(datadir)' \
binprefix='$(binprefix)' manprefix='$(manprefix)' \
mandir='$(mandir)' manext='$(manext)' LOCATE_DB='$(LOCATE_DB)' \
INSTALL_PROGRAM='$(INSTALL_PROGRAM)' INSTALL_DATA='$(INSTALL_DATA)' \
AR='$(AR)' RANLIB='$(RANLIB)' DEFS='$(DEFS)' \
CFLAGS='$(CFLAGS)' LDFLAGS='$(LDFLAGS)' LIBS='$(LIBS)' CC='$(CC)'

DISTFILES = COPYING COPYING.LIB ChangeLog Makefile.in README INSTALL \
NEWS configure configure.in

# Subdirectories to run make in for the primary targets.
SUBDIRS = lib find xargs locate man

all:
	for dir in $(SUBDIRS); do echo making $@ in $$dir; cd $$dir; $(MAKE) $(MDEFINES) $@; cd ..; done

install uninstall TAGS clean mostlyclean distclean realclean::
	for dir in $(SUBDIRS); do echo making $@ in $$dir; cd $$dir; $(MAKE) $(MDEFINES) $@; cd ..; done

distclean realclean::
	rm -f Makefile config.status

dist:
	echo find-`sed -e '/version_string/!d' -e 's/[^0-9.]*\([0-9.]*\).*/\1/' -e q find/version.c` > .fname
	rm -rf `cat .fname`
	mkdir `cat .fname`
	ln $(DISTFILES) `cat .fname`
	for dir in $(SUBDIRS); do mkdir `cat .fname`/$$dir; cd $$dir; $(MAKE) $@; cd ..; done
	tar chzf `cat .fname`.tar.z `cat .fname`
	rm -rf `cat .fname` .fname

# Prevent GNU make v3 from overflowing arg limit on SysV.
.NOEXPORT:
