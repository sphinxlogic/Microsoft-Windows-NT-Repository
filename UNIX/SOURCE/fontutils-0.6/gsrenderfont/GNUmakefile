# Makefile for Ghostscript font-rendering program.
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

program = bbcount

c_and_h = input-pbm
c_only = main version

libraries = pbm

include ../data/defs.make
include ../data/defsprog.make

all: gsrenderfont

gsrenderfont: gsrf.in
	rm -f $@
	sed -e "s,replace-with-lib-path,$(default_lib_path)," $< > $@
	chmod a-w $@
	chmod +x $@

install::
	$(INSTALL_PROGRAM) gsrenderfont $(bindir)/gsrenderfont
	$(INSTALL_DATA) fixifibb.awk $(fu_datadir)/fixifibb.awk
	$(INSTALL_DATA) writefont.PS $(fu_datadir)/writefont.PS

dist::
	ln gsrf.in fixifibb.awk writefont.PS $(distdir)

include M.depend

check: gsrenderfont bbcount
	rm -f Style.tfm Style.300*
	gsrenderfont -verbose -version Style.ps -font=Style
	fontconvert -tfm ./Style.300

distclean::
	rm -f gsrenderfont

extraclean::
	rm -f *.xifi *.bfi *.pbm *.bbs *.pl *.ifi
