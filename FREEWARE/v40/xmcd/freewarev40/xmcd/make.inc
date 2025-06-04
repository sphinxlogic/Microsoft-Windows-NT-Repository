#
#   @(#)make.inc	6.8 98/05/04
#
#   make.inc for xmcd, cda and libdi
#
#	xmcd  - Motif(tm) CD Audio Player
#	cda   - Command-line CD Audio Player
#	libdi - CD Audio Player Device Interface Library
#	cddb  - CD Database Management Library
#
#   NOTE: You should use this only if you don't have imake!
#
#   Copyright (C) 1993-1998  Ti Kan
#   E-mail: ti@amb.org
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

#
# Modify these as you see fit.  You will most certainly need to
# make some changes here in order to successfully build xmcd/cda
# on your platform.
#

#
# For the TIMER= line, use one of the following:
#	-DUSE_SELECT
#	-DUSE_POLL
#	-DUSE_USLEEP
#	-DUSE_NAP
#
# For the TERMIO= line, use one of the following:
#	-DUSE_TERMIOS
#	-DUSE_TERMIO
#	-DUSE_SGTTY
#
SHELL=		/bin/sh
MAKE=		make
CC=		cc
RANLIB=		/bin/true
AR=		ar cq
RM=		rm -f
CFLAGS=		-O $(DEFINES) $(INCDIRS) $(ARCH) $(TIMER) $(TERMIO)
ARCH=		-DSVR4 -Di386
TIMER=		-DUSE_SELECT
TERMIO=		-DUSE_TERMIOS
INCDIRS=	-I.. -I/usr/include -I/usr/X/include
XLIBS=		-L/usr/X/lib -lXm -lXt -lX11 -lgen
CURSESLIB=	-lcurses
SOCKETLIB=	-lsocket -lnsl
BINDIR=		/usr/bin/X11
LIBDIR=		/usr/lib/X11
MANDIR=		/usr/man/man1
MANSUF=		.1
DEPEND=		makedepend
DEPENDFLAGS=
TAGS=		ctags

