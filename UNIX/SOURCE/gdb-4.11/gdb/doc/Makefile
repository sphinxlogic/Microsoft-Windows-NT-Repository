# This file was generated automatically by configure.  Do not edit.
VPATH = .
links = 
host_alias = i386-univel-sysv4.2
host_cpu = i386
host_vendor = univel
host_os = sysv4.2
host_canonical = i386-univel-sysv4.2
target_alias = i386-univel-sysv4.2
target_cpu = i386
target_vendor = univel
target_os = sysv4.2
target_canonical = i386-univel-sysv4.2
##Copyright (C) 1991, 1992 Free Software Foundation, Inc.

# Makefile for GDB documentation.
# This file is part of GDB.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

srcdir = .

prefix = /opt

infodir = $(prefix)/info

SHELL = /bin/sh

INSTALL = install -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL)

# main GDB source directory
gdbdir = $(srcdir)/..

# where to find texinfo; GDB dist should include a recent one
TEXIDIR=${gdbdir}/../texinfo

# where to find makeinfo, preferably one designed for texinfo-2
MAKEINFO=makeinfo

# where to find texi2roff, ditto
TEXI2ROFF=texi2roff

# Where is the source dir for the READLINE library doc?  
# Traditionally readline is in .. or .
READLINE_DIR = ${gdbdir}/../readline/doc

SET_TEXINPUTS = TEXINPUTS=${TEXIDIR}:.:$(srcdir):$(READLINE_DIR):$$TEXINPUTS

# There may be alternate predefined collections of switches to configure
# the GDB manual.  Normally this is not done in synch with the software
# config system, since this choice tends to be independent; most people
# want a doc config of `all' for a generic manual, regardless of sw config.
DOC_CONFIG = all

# This list of sed edits will edit the GDB reference card
# for what fonts and what papersize to use.
# By default (NO edits applied), the refcard uses:
#    - Computer Modern (CM) fonts
#    - US letter paper (8.5x11in)
# List some of the following files for alternative fonts and paper:
#  a4rc.sed     use A4 paper (297 x 210 mm)
#  psrc.sed     use PostScript fonts (Karl Berry short TeX names)
# lpsrc.sed     use PostScript fonts (full PostScript names in TeX)
# e.g. for A4, Postscript:  REFEDITS = a4rc.sed psrc.sed
#      for A4, CM fonts:    REFEDITS = a4rc.sed
#      for US, PS fonts:    REFEDITS = psrc.sed
#      for default:
REFEDITS =

# Don Knuth's TeX formatter
TEX = tex

# auxiliary program for sorting Texinfo indices
TEXINDEX = texindex

# Main GDB manual's source files
SFILES_INCLUDED = gdb-cfg.texi $(srcdir)/remote.texi

SFILES_LOCAL = $(srcdir)/gdb.texinfo GDBvn.texi $(SFILES_INCLUDED)

SFILES_DOC = $(SFILES_LOCAL) \
                $(READLINE_DIR)/rluser.texinfo $(READLINE_DIR)/inc-hist.texi

#### Host, target, and site specific Makefile fragments come in here.
###

all install:

info: gdb.info gdbint.info stabs.info
dvi: gdb.dvi refcard.dvi gdbint.dvi
all-doc: gdb.info gdb.dvi refcard.dvi gdb-internals gdbint.dvi

install-info: info
	for i in *.info* ; do \
		$(INSTALL_DATA) $$i $(infodir)/$$i ; \
	done

STAGESTUFF = *.info* gdb-all.texi GDBvn.texi

# Copy the object files from a particular stage into a subdirectory.
stage1: force
	-mkdir stage1
	-mv $(STAGESTUFF) stage1

stage2: force
	-mkdir stage2
	-mv $(STAGESTUFF) stage2

stage3: force
	-mkdir stage3
	-mv $(STAGESTUFF) stage3

against=stage2

comparison: force
	for i in $(STAGESTUFF) ; do cmp $$i $(against)/$$i ; done

de-stage1: force
	-(cd stage1 ; mv -f * ..)
	-rmdir stage1

de-stage2: force
	-(cd stage2 ; mv -f * ..)
	-rmdir stage2

de-stage3: force
	-(cd stage3 ; mv -f * ..)
	-rmdir stage3

clean-info:
	rm -f gdb.info* gdbint.info* stabs.info*

clean-dvi:
	rm -f gdb.dvi refcard.dvi gdbint.dvi stabs.dvi sedref.dvi
	
mostlyclean: clean-info clean-dvi
	rm -f gdb.?? gdb.??? gdb.mm gdb.ms gdb.me
	rm -f links2roff
	rm -f refcard.ps lrefcard.ps refcard.log sedref.* *~
	rm -f gdbint.?? gdbint.??? stabs.?? stabs.???

clean: mostlyclean
	rm -f GDBvn.texi rluser.texinfo inc-hist.texi

distclean: clean
	rm -f Makefile config.status

realclean: distclean clean-dvi clean-info

# GDB QUICK REFERENCE (dvi output)
refcard.dvi : refcard.tex $(REFEDITS)
	if [ -z "$(REFEDITS)" ]; then \
		cp $(srcdir)/refcard.tex sedref.tex ; \
	else \
		echo > tmp.sed ; \
		for f in "$(REFEDITS)" ; do \
			cat $(srcdir)/$$f >>tmp.sed ; done ; \
		sed -f tmp.sed $(srcdir)/refcard.tex >sedref.tex ; \
	fi
	$(SET_TEXINPUTS) $(TEX) sedref.tex
	mv sedref.dvi refcard.dvi
	rm -f sedref.log sedref.tex tmp.sed

refcard.ps : refcard.dvi
	dvips -t landscape refcard.dvi -o

# File to record current GDB version number (copied from main dir Makefile.in)
GDBvn.texi : ${gdbdir}/Makefile.in
	echo "@set GDBVN `sed <$(srcdir)/../Makefile.in -n 's/VERSION = //p'`" > ./GDBvn.new
	mv GDBvn.new GDBvn.texi

# Updated atomically
.PRECIOUS: GDBvn.texi

# Choose configuration for GDB manual (normally `all'; normally not tied into
# `configure' script because most users prefer generic version of manual,
# not one for their binary config---which may not be specifically
# defined anyways).
gdb-cfg.texi: ${srcdir}/${DOC_CONFIG}-cfg.texi
	ln -s ${srcdir}/${DOC_CONFIG}-cfg.texi gdb-cfg.texi || \
	ln ${srcdir}/${DOC_CONFIG}-cfg.texi gdb-cfg.texi || \
	cp ${srcdir}/${DOC_CONFIG}-cfg.texi gdb-cfg.texi

# GDB MANUAL: texinfo source, using @set/@clear/@value/@ifset/@ifclear
# If your texinfo or makeinfo don't support these, get a new texinfo release
#
# The nonsense with GDBvn.texi gets this to run with both Sun and GNU make.
#   Note that we can *generate* GDBvn.texi, but since we distribute one in the
#   source directory for the benefit of people who *don't* use this makefile,
#   VPATH will often tell make not to bother building it, because the one
#   in the srcdir is up to date.  (if not, then make should build one here).

# GDB MANUAL: TeX dvi file
gdb.dvi: ${SFILES_DOC}
	if [ ! -f ./GDBvn.texi ]; then \
		ln -s $(srcdir)/GDBvn.texi . || \
		ln $(srcdir)/GDBvn.texi . || \
		cp $(srcdir)/GDBvn.texi . ; else true; fi
	$(SET_TEXINPUTS) $(TEX) gdb.texinfo
	$(SET_TEXINPUTS) $(TEX) gdb.texinfo
	$(TEXINDEX) gdb.??
	$(SET_TEXINPUTS) $(TEX) gdb.texinfo
	rm -f gdb.?? gdb.log gdb.aux gdb.toc gdb.??s

# GDB MANUAL: info file
# We're using texinfo2, and older makeinfo's may not be able to
# cope with all the markup.  
gdb.info: ${SFILES_DOC}
	$(MAKEINFO) -I ${READLINE_DIR} -I $(srcdir) -o ./gdb.info gdb.texinfo

# GDB MANUAL: roff translations
# Try to use a recent texi2roff.  v2 was put on prep in jan91.
# If you want an index, see texi2roff doc for postprocessing 
# and add -i to texi2roff invocations below.
# Workarounds for texi2roff-2 (probably fixed in later texi2roff's, delete
#    corresponding -e lines when later texi2roff's are current)
# + @ifinfo's deleted explicitly due to texi2roff-2 bug w nested constructs.
# + @c's deleted explicitly because texi2roff sees texinfo commands in them
# + @   (that's at-BLANK) not recognized by texi2roff, turned into blank
# + @alphaenumerate is ridiculously new, turned into @enumerate

# texi2roff doesn't have a notion of include dirs, so we have to fake 
# it out for gdb manual's include files---but only if not configured
# in main sourcedir.
links2roff: $(SFILES_INCLUDED)
	if [ ! -f gdb.texinfo ]; then \
		ln -s $(SFILES_INCLUDED) . || \
		ln $(SFILES_INCLUDED)    . || \
		cp $(SFILES_INCLUDED)    . ; \
	fi
	touch links2roff

#  "Readline" appendices.  Get them also due to lack of includes, 
# regardless of whether or not configuring in main sourcedir.
# @ftable removed due to bug in texi2roff-2; if your texi2roff
# is newer, try just ln or cp
rluser.texinfo: ${READLINE_DIR}/rluser.texinfo
		sed -e 's/^@ftable/@table/g' \
		-e 's/^@end ftable/@end table/g' \
		${READLINE_DIR}/rluser.texinfo > ./rluser.texinfo

inc-hist.texi: ${READLINE_DIR}/inc-hist.texi
	ln -s ${READLINE_DIR}/inc-hist.texi . || \
	ln ${READLINE_DIR}/inc-hist.texi . || \
	cp ${READLINE_DIR}/inc-hist.texi .

# gdb manual suitable for [gtn]roff -me
gdb.me: $(SFILES_LOCAL) links2roff rluser.texinfo inc-hist.texi
	sed -e '/\\input texinfo/d' \
		-e '/@c TEXI2ROFF-KILL/,/@c END TEXI2ROFF-KILL/d' \
		-e '/^@ifinfo/,/^@end ifinfo/d' \
		-e '/^@c /d' \
		-e 's/{.*,,/{/' \
		-e 's/@ / /g' \
		-e 's/^@alphaenumerate/@enumerate/g' \
		-e 's/^@end alphaenumerate/@end enumerate/g' \
		$(srcdir)/gdb.texinfo | \
	$(TEXI2ROFF) -me | \
	sed -e 's/---/\\(em/g' \
	>gdb.me 

# gdb manual suitable for [gtn]roff -ms
gdb.ms: $(SFILES_LOCAL) links2roff rluser.texinfo inc-hist.texi
	sed -e '/\\input texinfo/d' \
		-e '/@c TEXI2ROFF-KILL/,/@c END TEXI2ROFF-KILL/d' \
		-e '/^@ifinfo/,/^@end ifinfo/d' \
		-e '/^@c /d' \
		-e 's/{.*,,/{/' \
		-e 's/@ / /g' \
		-e 's/^@alphaenumerate/@enumerate/g' \
		-e 's/^@end alphaenumerate/@end enumerate/g' \
		$(srcdir)/gdb.texinfo | \
	$(TEXI2ROFF) -ms | \
	sed -e 's/---/\\(em/g' \
	>gdb.ms 

# gdb manual suitable for [tn]roff -mm
# '@noindent's removed due to texi2roff-2 mm bug; if yours is newer, 
#   try leaving them in
gdb.mm: $(SFILES_LOCAL) links2roff rluser.texinfo inc-hist.texi
	sed -e '/\\input texinfo/d' \
		-e '/@c TEXI2ROFF-KILL/,/@c END TEXI2ROFF-KILL/d' \
		-e '/^@ifinfo/,/^@end ifinfo/d' \
		-e '/^@c /d' \
		-e 's/{.*,,/{/' \
		-e '/@noindent/d' \
		-e 's/@ / /g' \
		-e 's/^@alphaenumerate/@enumerate/g' \
		-e 's/^@end alphaenumerate/@end enumerate/g' \
		$(srcdir)/gdb.texinfo | \
	$(TEXI2ROFF) -mm | \
	sed -e 's/---/\\(em/g' \
	>gdb.mm 

# GDB INTERNALS MANUAL: TeX dvi file
gdbint.dvi : gdbint.texinfo
	$(SET_TEXINPUTS) $(TEX) gdbint.texinfo
	$(TEXINDEX) gdbint.??
	$(SET_TEXINPUTS) $(TEX) gdbint.texinfo
	rm -f gdbint.?? gdbint.aux gdbint.cps gdbint.fns gdbint.kys \
		gdbint.log gdbint.pgs gdbint.toc gdbint.tps gdbint.vrs

# GDB INTERNALS MANUAL: info file
gdb-internals: gdbint.info

gdbint.info: gdbint.texinfo
	$(MAKEINFO) -o gdbint.info $(srcdir)/gdbint.texinfo

stabs.info: stabs.texinfo
	$(MAKEINFO) -o stabs.info $(srcdir)/stabs.texinfo

# STABS DOCUMENTATION: TeX dvi file
stabs.dvi : stabs.texinfo
	$(SET_TEXINPUTS) $(TEX) stabs.texinfo
	$(TEXINDEX) stabs.??
	$(SET_TEXINPUTS) $(TEX) stabs.texinfo
	rm -f stabs.?? stabs.aux stabs.cps stabs.fns stabs.kys \
		stabs.log stabs.pgs stabs.toc stabs.tps stabs.vrs

stabs.ps: stabs.dvi
	dvips -o stabs.ps stabs

force:

Makefile: $(srcdir)/Makefile.in $(host_makefile_frag) $(target_makefile_frag)
	$(SHELL) ./config.status
