# Makefile for BZR conversion program.
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

program = bzrto

c_and_h = char input-ccc main metafont pstype1 pstype3 psutil text
c_only = version
y = ccc

libraries = $(bitmap_libs) bzr tfm

include ../data/defs.make
include ../data/defsprog.make

dist::
	ln bzrsetup.mf bzredit.el bzrbuildch.PS $(distdir)

install::
	$(INSTALL_DATA) bzredit.el $(emacslispdir)/bzredit.el
	$(INSTALL_DATA) bzrsetup.mf $(mfinputdir)/bzrsetup.mf

include M.depend
