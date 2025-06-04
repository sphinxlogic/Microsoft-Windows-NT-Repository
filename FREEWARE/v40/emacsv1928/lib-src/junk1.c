# Makefile for lib-src subdirectory in GNU Emacs.
# Copyright (C) 1985, 1987, 1988, 1993, 1994 Free Software Foundation, Inc.

# This file is part of GNU Emacs.

# GNU Emacs is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# GNU Emacs is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with GNU Emacs; see the file COPYING.  If not, write to
# the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

# Avoid trouble on systems where the `SHELL' variable might be
# inherited from the environment.
SHELL = /bin/sh

# ==================== Things `configure' will edit ====================

CC=@CC@
CFLAGS=@CFLAGS@
ALLOCA=@ALLOCA@
YACC=@YACC@
version=@version@
configuration=@configuration@

# ==================== Where To Install Things ====================

# The default location for installation.  Everything is placed in
# subdirectories of this directory.  The default values for many of
# the variables below are expressed in terms of this one, so you may
# not need to change them.  This is set with the --prefix option to
# `../configure'.
prefix=@prefix@

# Like `prefix', but used for architecture-specific files.  This is
# set with the --exec-prefix option to `../configure'.
exec_prefix=@exec_prefix@

# Where to install Emacs and other binaries that people will want to
# run directly (like etags).  This is set with the --bindir option
# to `../configure'.
bindir=@bindir@

# Where to install and expect executable files to be run by Emacs
# rather than directly by users, and other architecture-dependent
# data.  ${archlibdir} is usually below this.  This is set with the
# --libdir option to `../configure'.
libdir=@libdir@

# Where to find the source code.  This is set by the configure
# script's `--srcdir' option.  However, the value of ${srcdir} in
# this makefile is not identical to what was specified with --srcdir,
# since the variable here has `/lib-src' added at the end.
srcdir=@srcdir@
VPATH=@srcdir@

# ==================== Emacs-specific directories ====================

# These variables hold the values Emacs will actually use.  They are
# based on the values of the standard Make variables above.

# Where to put executables to be run by Emacs rather than the user.
# This path usually includes the Emacs version and configuration name,
# so that multiple configurations for multiple versions of Emacs may
# be installed at once.  This can be set with the --archlibdir option
# to `../configure'.
archlibdir=@archlibdir@

# ==================== Utility Programs for the Build =================

# ../configure figures out the correct values for these.
INSTALL = @INSTALL@
INSTALL_PROGRAM = @INSTALL_PROGRAM@
INSTALL_DATA = @INSTALL_DATA@

# ========================== Lists of Files ===========================

# Things that a user might actually run,
# which should be installed in bindir.
INSTALLABLES = etags ctags emacsclient b2m
INSTALLABLE_SCRIPTS = rcs-checkin

# Things that Emacs runs internally, or during the build process,
#  which should not be installed in bindir.
UTILITIES= test-distrib make-path wakeup profile make-docfile digest-doc \
	sorted-doc movemail cvtmail fakemail yow emacsserver hexl timer

# Like UTILITIES, but they're not system-dependent, and should not be
#  deleted by the distclean target.
SCRIPTS= rcs2log vcdiff

EXECUTABLES= ${UTILITIES} ${INSTALLABLES} ${SCRIPTS} ${INSTALLABLE_SCRIPTS}

SOURCES = COPYING ChangeLog Makefile.in README aixcc.lex emacs.csh \
	makedoc.com *.[chy] rcs2log vcdiff

