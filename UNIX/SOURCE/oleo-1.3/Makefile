# Generated automatically from Makefile.in by configure.
# Copyright (C) 1992, 1993 Free Software Foundation, Inc.
# This file is part of GNU Oleo.
# 
# GNU Oleo is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# GNU Oleo is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Oleo; see the file COPYING.  If not, write to
# the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
SHELL = /bin/sh

srcdir = .
VPATH = .

# Common prefix for machine-independent installed files.
prefix = /opt
# Common prefix for machine-dependent installed files.
exec_prefix = $(prefix)
# Name under which to install GNU Oleo.
instname = oleo
# Directory to install `oleo' in.
bindir = $(exec_prefix)/bin
MANDIR=$(prefix)/man
CAT1DIR=cat1

CFLAGS = -g -O3
BISONFLAGS =
optional_libs =  -lsocket -lnsl -lX11 -lcurses -ltermcap
extra_objs = 

CC = gcc
BISON = bison -y
INSTALL = /usr/ucb/install -c
INSTALL_DATA = $(INSTALL) -m 644
INSTALL_PROGRAM = $(INSTALL)
ETAGS = etags -tw
CTAGS = ctags -tw

ALL_CFLAGS = $(CFLAGS)  -DSTDC_HEADERS=1 -DHAVE_UNISTD_H=1 -DDIRENT=1 -DHAVE_SYS_TIME_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_SYS_SELECT_H=1 -DHAVE_X11_X_H=1 -DRETSIGTYPE=void -DHAVE_RINT=1 -DHAVE_RENAME=1 -DHAVE_CBREAK=1 -DHAVE_STRDUP=1 -DHAVE_STRSTR=1 -DBITS_PER_CELLREF=16
ALL_BISONFLAGS = $(BISONFLAGS) 

.c.o:
	$(CC) -c $(ALL_CFLAGS) $(CPPFLAGS) -I$(srcdir) $<

libs =  $(optional_libs) -lm 

objs = parse.tab.o byte-compile.o eval.o ref.o \
 decompile.o sort.o regions.o utils.o obstack.o lists.o\
 io-term.o getopt.o getopt1.o io-utils.o io-x11.o window.o io-edit.o\
 hash.o panic.o line.o io-curses.o font.o display.o print.o init.o \
 sylk.o oleofile.o sc.o list.o busi.o string.o cells.o ir.o xrdb.o cmd.o \
 basic.o key.o format.o funcs.o args.o stub.o input.o info.o help.o \
 forminfo.o graph.o \

all_objs= $(objs) $(extra_obs)

srcs =  parse.y byte-compile.c eval.c ref.c \
 decompile.c sort.c regions.c utils.c obstack.c lists.c\
 io-term.c getopt.c getopt1.c  io-utils.c io-x11.c window.c io-edit.c\
 hash.c panic.c line.c  key.c format.c funcs.c \
 io-curses.c font.c display.c print.c init.c \
 sylk.c oleofile.c sc.c list.c busi.c string.c cells.c \
 random.c vfprintf.c vsprintf.c _doprnt.c alloca.c cmd.c basic.c args.c \
 stub.c input.c info.c help.c graph.c forminfo.c

headers = byte-compile.h cell.h cmd.h ir.h display.h errors.h eval.h \
 font.h funcdef.h global.h hash.h init.h io-abstract.h io-edit.h \
 io-generic.h io-term.h io-utils.h line.h list.h \
 lists.h  node.h obstack.h oleofile.h parse.h  \
 proto.h ref.h regions.h sc.h sylk.h sysdef.h \
 utils.h window.h ir.c xrdb.c io-curses.h io-x11.h \
 print.h basic.h getopt.h key.h format.h args.h stub.h decompile.h \
 input.h defun.h defuns.h stubs.h info.h help.h graph.h funcs.h \
 forminfo.h

dist_files = $(srcs) $(headers) parse.tab.c configure Makefile.in \
	configure.in ANNOUNCE COPYING USING BUGS ChangeLog NEWS INSTALL.OLEO \
	README INSTALL irs-1992.oleo KEYS

all: oleo
install: all
	$(INSTALL_PROGRAM) oleo $(bindir)/oleo
	cat USING KEYS $(MANDIR)/$(CAT1DIR)/oleo.1

uninstall:
	rm $(bindir)/oleo
Makefile: $(srcdir)/Makefile.in 
	$(SHELL) config.status
clean:
	rm -f $(all_objs) oleo
distclean: clean
	rm -f Makefile config.status 
mostlyclean: clean
realclean: distclean
	rm -f  parse.tab.c
TAGS: $(srcs)
	$(ETAGS) $(srcs)
tags: $(srcs)
	$(CTAGS) $(srcs)
dist.afs: $(dist_files)
	echo oleo-`sed -e '/version_string/!d' -e 's/[^0-9.]*\([0-9.]*\).*/\1/' -e q io-term.c` > .fname
	rm -rf `cat .fname`
	mkdir `cat .fname`
	cd `cat .fname`; \
	for file in $(dist_files); do ln -s ../"$$file" .; done; \
	cd ..
	tar chf `cat .fname`.tar `cat .fname`
	gzip `cat .fname`.tar
	rm -rf `cat .fname` .fname

dist: $(dist_files)
	echo oleo-`sed -e '/version_string/!d' -e 's/[^0-9.]*\([0-9.]*\).*/\1/' -e q io-term.c` > .fname
	rm -rf `cat .fname`
	mkdir `cat .fname`
	ln $(dist_files) `cat .fname`
	tar chZf `cat .fname`.tar.Z `cat .fname`
	rm -rf `cat .fname` .fname

distfiles:
	echo $(dist_files)

check:
	echo check check check
oleo: $(all_objs)
	$(CC) $(ALL_CFLAGS) -o oleo $(all_objs) $(libs)

parse.tab.c: parse.y
	$(BISON) parse.y
	-mv y.tab.c parse.tab.c

# Prevent GNU make v3 from overflowing arg limit on SysV.
.NOEXPORT:

# ================ automaticly generated ================
_doprnt.o : _doprnt.c 
alloca.o : alloca.c 
args.o : args.c global.h sysdef.h utils.h funcdef.h args.h cmd.h obstack.h line.h \
  key.h funcs.h info.h io-abstract.h window.h cell.h font.h io-generic.h io-edit.h \
  io-utils.h format.h 
basic.o : basic.c sysdef.h global.h utils.h funcdef.h basic.h cmd.h obstack.h \
  line.h key.h args.h funcs.h info.h regions.h window.h cell.h font.h io-term.h \
  io-generic.h io-abstract.h io-utils.h io-curses.h ref.h format.h lists.h io-edit.h \
  eval.h byte-compile.h 
busi.o : busi.c funcdef.h sysdef.h global.h utils.h cell.h font.h line.h eval.h \
  errors.h 
byte-compile.o : byte-compile.c funcdef.h obstack.h sysdef.h global.h utils.h \
  node.h eval.h cell.h font.h line.h hash.h ref.h 
cells.o : cells.c obstack.h funcdef.h sysdef.h global.h utils.h cell.h font.h \
  line.h eval.h errors.h lists.h format.h io-abstract.h window.h io-generic.h \
  io-term.h cmd.h key.h args.h funcs.h info.h 
cmd.o : cmd.c /usr/include/errno.h obstack.h global.h sysdef.h utils.h funcdef.h \
  cmd.h line.h key.h args.h funcs.h info.h io-term.h io-abstract.h window.h cell.h \
  font.h io-generic.h io-utils.h io-edit.h stub.h ref.h 
decompile.o : decompile.c global.h sysdef.h utils.h funcdef.h decompile.h eval.h \
  cell.h font.h line.h io-utils.h 
display.o : display.c display.h global.h sysdef.h utils.h funcdef.h font.h line.h \
  cell.h ir.h proto.h lists.h io-utils.h 
eval.o : eval.c funcdef.h obstack.h sysdef.h global.h utils.h cell.h font.h \
  line.h eval.h errors.h 
font.o : font.c font.h global.h sysdef.h utils.h funcdef.h line.h window.h cell.h \
  io-abstract.h cmd.h obstack.h key.h args.h funcs.h info.h io-x11.h io-term.h \
  io-utils.h 
format.o : format.c global.h sysdef.h utils.h funcdef.h format.h cell.h font.h \
  line.h cmd.h obstack.h key.h args.h funcs.h info.h io-term.h io-abstract.h window.h \
  io-generic.h 
forminfo.o : forminfo.c sysdef.h forminfo.h 
funcs.o : funcs.c global.h sysdef.h utils.h funcdef.h cmd.h obstack.h line.h \
  key.h args.h funcs.h info.h io-term.h basic.h format.h cell.h font.h print.h \
  io-x11.h io-edit.h regions.h help.h window.h graph.h lists.h defuns.h defun.h 
getopt.o : getopt.c getopt.h 
getopt1.o : getopt1.c getopt.h 
graph.o : graph.c global.h sysdef.h utils.h funcdef.h graph.h cell.h font.h \
  line.h cmd.h obstack.h key.h args.h funcs.h info.h io-term.h regions.h ref.h \
  io-utils.h 
hash.o : hash.c sysdef.h hash.h 
help.o : help.c global.h sysdef.h utils.h funcdef.h cmd.h obstack.h line.h key.h \
  args.h funcs.h info.h forminfo.h help.h io-utils.h cell.h font.h 
info.o : info.c global.h sysdef.h utils.h funcdef.h info.h hash.h cmd.h obstack.h \
  line.h key.h args.h funcs.h 
init.o : init.c proto.h init.h global.h sysdef.h utils.h funcdef.h io-term.h \
  cmd.h obstack.h line.h key.h args.h funcs.h info.h 
input.o : input.c global.h sysdef.h utils.h funcdef.h input.h cmd.h obstack.h \
  line.h key.h args.h funcs.h info.h window.h cell.h font.h io-abstract.h 
io-curses.o : io-curses.c proto.h funcdef.h sysdef.h global.h utils.h cell.h \
  font.h line.h cmd.h obstack.h key.h args.h funcs.h info.h io-generic.h io-edit.h \
  io-term.h io-abstract.h window.h io-utils.h lists.h regions.h input.h 
io-edit.o : io-edit.c funcdef.h sysdef.h global.h utils.h cell.h font.h line.h \
  io-utils.h io-edit.h cmd.h obstack.h key.h args.h funcs.h info.h io-abstract.h \
  window.h io-generic.h format.h lists.h regions.h 
io-term.o : io-term.c global.h sysdef.h utils.h funcdef.h basic.h cell.h font.h \
  line.h cmd.h obstack.h key.h args.h funcs.h info.h format.h getopt.h init.h \
  io-abstract.h window.h io-curses.h io-edit.h io-generic.h io-term.h io-utils.h \
  io-x11.h lists.h oleofile.h print.h ref.h regions.h graph.h list.h sc.h sylk.h 
io-utils.o : io-utils.c sysdef.h io-utils.h global.h utils.h funcdef.h cell.h \
  font.h line.h ref.h decompile.h io-generic.h obstack.h io-abstract.h window.h \
  lists.h io-term.h cmd.h key.h args.h funcs.h info.h 
io-x11.o : io-x11.c 
ir.o : ir.c ir.h proto.h 
key.o : key.c key.h global.h sysdef.h utils.h funcdef.h line.h cmd.h obstack.h \
  args.h funcs.h info.h io-abstract.h window.h cell.h font.h io-generic.h io-utils.h \
  io-term.h 
line.o : line.c global.h sysdef.h utils.h funcdef.h line.h 
list.o : list.c funcdef.h sysdef.h global.h utils.h cell.h font.h line.h io-generic.h \
  obstack.h io-abstract.h window.h regions.h io-utils.h cmd.h key.h args.h funcs.h \
  info.h 
lists.o : lists.c funcdef.h obstack.h sysdef.h global.h utils.h cell.h font.h \
  line.h ref.h window.h io-term.h io-abstract.h 
obstack.o : obstack.c obstack.h 
oleofile.o : oleofile.c funcdef.h sysdef.h io-generic.h global.h utils.h obstack.h \
  io-abstract.h line.h window.h cell.h font.h io-utils.h io-term.h sylk.h lists.h \
  ref.h regions.h info.h cmd.h key.h args.h funcs.h 
panic.o : panic.c funcdef.h sysdef.h global.h utils.h io-generic.h obstack.h \
  io-abstract.h line.h window.h cell.h font.h info.h cmd.h key.h args.h funcs.h 
parse.tab.o : parse.tab.c funcdef.h obstack.h sysdef.h global.h utils.h errors.h \
  node.h eval.h cell.h font.h line.h ref.h 
print.o : print.c display.h global.h sysdef.h utils.h funcdef.h font.h line.h \
  cell.h ir.h proto.h cmd.h obstack.h key.h args.h funcs.h info.h io-generic.h \
  io-abstract.h window.h 
random.o : random.c 
ref.o : ref.c funcdef.h sysdef.h global.h utils.h cell.h font.h line.h eval.h \
  io-abstract.h window.h io-generic.h obstack.h hash.h byte-compile.h parse.h \
  ref.h cmd.h key.h args.h funcs.h info.h 
regions.o : regions.c funcdef.h sysdef.h global.h utils.h cell.h font.h line.h \
  io-generic.h obstack.h io-abstract.h window.h io-utils.h lists.h ref.h regions.h \
  io-term.h cmd.h key.h args.h funcs.h info.h 
sc.o : sc.c funcdef.h sysdef.h global.h utils.h cell.h font.h line.h io-generic.h \
  obstack.h io-abstract.h window.h io-utils.h lists.h ref.h parse.h regions.h \
  cmd.h key.h args.h funcs.h info.h 
sort.o : sort.c 
string.o : string.c funcdef.h obstack.h sysdef.h global.h utils.h cell.h font.h \
  line.h eval.h errors.h 
stub.o : stub.c global.h sysdef.h utils.h funcdef.h cmd.h obstack.h line.h key.h \
  args.h funcs.h info.h stub.h stubs.h 
sylk.o : sylk.c funcdef.h sysdef.h io-generic.h global.h utils.h obstack.h io-abstract.h \
  line.h window.h cell.h font.h io-term.h lists.h io-utils.h ref.h regions.h info.h \
  cmd.h key.h args.h funcs.h 
utils.o : utils.c sysdef.h utils.h funcdef.h global.h 
vfprintf.o : vfprintf.c sysdef.h 
vsprintf.o : vsprintf.c sysdef.h 
window.o : window.c global.h sysdef.h utils.h funcdef.h window.h cell.h font.h \
  line.h io-generic.h obstack.h io-abstract.h io-utils.h io-term.h cmd.h key.h \
  args.h funcs.h info.h lists.h regions.h 
xrdb.o : xrdb.c 
