# Generated automatically from Makefile.in by configure.
# Makefile.in --- Mesa GL Widget for X11 Toolkit Programming
# Copyright (C) 1995 by
#   Jeroen van der Zijp <jvz@cyberia.cfdrc.com>
#   Thorsten Ohl <Thorsten.Ohl@Physik.TH-Darmstadt.de>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# $Id: Makefile.in,v 1.4 1996/01/15 00:44:05 ohl Exp $
#
########################################################################

SHELL = /bin/sh

srcdir = .
top_srcdir = ../..
mesa_srcdir = /usr/people/jvz/Mesa-2.2

prefix = /usr/local
exec_prefix = ${prefix}

incdir = $(prefix)/include/GL

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644

HEADERS = GLwDrawA.h GLwDrawAP.h GLwMDrawA.h GLwMDrawAP.h \
	  MesaDrawingArea.h MesaDrawingAreaP.h \
	  MesaMDrawingArea.h MesaMDrawingAreaP.h \
	  MesaWorkstation.h MesaWorkstationP.h

all:

install: $(HEADERS)
	$(INSTALL_DATA) $(HEADERS) $(incdir)

uninstall:
	cd $(incdir) && rm -f $(HEADERS)

# Standard GNU clean* targets:
maintainer-clean: clean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

mostlyclean: clean

distclean: clean
	rm -f Makefile

clean:
	rm -f .*~ *~

# Additional clean* targets:
realclean: distclean

# GNU autoconf(1) targets:
Makefile: Makefile.in $(top_srcdir)/config.status
	cd $(top_srcdir); $(SHELL) config.status
