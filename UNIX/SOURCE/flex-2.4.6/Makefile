# Generated automatically from Makefile.in by configure.
# @(#) $Header: Makefile.in,v 1.2 94/01/04 14:33:19 vern Exp $ (LBL)

# If your version of "make" does not define $(MAKE), comment in the
# definition of "MAKE" below.  (You only need to do this if you intend
# to do "make bigcheck" or "make dist".)
# MAKE = make

# Possible values for DEFS:
#
# For flex to always generate 8-bit scanners, add "-DDEFAULT_CSIZE=256"
# to DEFS.
#
# For Vax/VMS, add "-DVMS" to DEFS.
#
# For MS-DOS, add "-DMS_DOS" to DEFS.  See the directory MISC/MSDOS for
# additional info.

CFLAGS = -O3 -g
DEFS =  -DSTDC_HEADERS=1 -DHAVE_STRING_H=1
LDFLAGS = 
LIBS = 

# Installation targeting.  Files will be installed under the tree
# rooted at prefix.  flex will be installed in bindir, libfl.a in
# libdir, FlexLexer.h will be installed in includedir, and the manual
# pages will be installed in mandir with extension manext.
# 
# Raw, unformatted troff source will be installed if INSTALLMAN=man,
# nroff preformatted versions will be installed if INSTALLMAN=cat.

prefix = /opt
exec_prefix = ${prefix}
bindir = $(exec_prefix)/bin
libdir = $(exec_prefix)/lib
includedir = $(prefix)/include
manext = 1
mandir = $(prefix)/man/man$(manext)

INSTALLMAN = man

SHELL = /bin/sh
srcdir = .
VPATH = .

LN_S = ln -s
YACC = bison -y
CC = gcc
AR = ar
RANLIB = ranlib
INSTALL = /opt/bin/install -c
INSTALL_DATA = $(INSTALL) -m 644
INSTALL_PROGRAM = $(INSTALL)

# You normally do not need to modify anything below this point.
# ------------------------------------------------------------

CPPFLAGS = -I. -I$(srcdir) $(DEFS)

.c.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $<

HEADERS = flexdef.h version.h

SOURCES = ccl.c dfa.c ecs.c gen.c main.c misc.c nfa.c parse.y \
	scan.l skel.c sym.c tblcmp.c yylex.c
OBJECTS = ccl.o dfa.o ecs.o gen.o main.o misc.o nfa.o parse.o \
	scan.o skel.o sym.o tblcmp.o yylex.o 

LIBSRCS = libmain.c libyywrap.c
LIBOBJS = libmain.o libyywrap.o

LINTSRCS = ccl.c dfa.c ecs.c gen.c main.c misc.c nfa.c parse.c \
	scan.c skel.c sym.c tblcmp.c yylex.c

DISTFILES = README NEWS COPYING INSTALL FlexLexer.h \
	configure.in Makefile.in mkskel.sh flex.skl \
	$(HEADERS) $(SOURCES) $(LIBSRCS) MISC \
	flex.1 flexdoc.1 \
	scan.c install.sh mkinstalldirs configure

DIST_NAME = flex

# which "flex" to use to generate scan.c from scan.l
FLEX = ./flex
FLEX_FLAGS = -ist $(PERF_REPORT)
COMPRESSION =
PERF_REPORT = -p

FLEXLIB = libfl.a


all: flex

flex: .bootstrap $(OBJECTS) $(FLEXLIB)
	$(CC) $(CFLAGS) -o flex $(LDFLAGS) $(OBJECTS) $(FLEXLIB) $(LIBS)

.bootstrap: initscan.c
	@rm -f scan.c
	cp $(srcdir)/initscan.c scan.c
	touch .bootstrap

parse.c: parse.y
	$(YACC) -d $(srcdir)/parse.y
	@sed '/extern char.*malloc/d' <y.tab.c >parse.c
	@rm -f y.tab.c
	@mv y.tab.h parse.h

parse.h: parse.c

scan.c: scan.l
	$(FLEX) $(FLEX_FLAGS) $(COMPRESSION) $(srcdir)/scan.l >scan.tmp
	sed s,\"$(srcdir)/scan.l\",\"scan.l\", <scan.tmp >scan.c
	@rm scan.tmp

scan.o: scan.c parse.h flexdef.h
yylex.o: yylex.c parse.h flexdef.h

skel.c: flex.skl mkskel.sh
	$(SHELL) $(srcdir)/mkskel.sh $(srcdir)/flex.skl >skel.c

main.o: main.c flexdef.h version.h
ccl.o: ccl.c flexdef.h
dfa.o: dfa.c flexdef.h
ecs.o: ecs.c flexdef.h
gen.o: gen.c flexdef.h
misc.o: misc.c flexdef.h
nfa.o: nfa.c flexdef.h
parse.o: parse.c flexdef.h
skel.o: skel.c flexdef.h
sym.o: sym.c flexdef.h
tblcmp.o: tblcmp.c flexdef.h

alloca.o: alloca.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -Dxmalloc=yy_flex_xmalloc alloca.c

alloca.c: $(srcdir)/MISC/alloca.c
	@rm -f alloca.c
	cp $(srcdir)/MISC/alloca.c .

test: check
check: flex
	./flex $(FLEX_FLAGS) $(COMPRESSION) $(srcdir)/scan.l \
	| sed s,\"$(srcdir)/scan.l\",\"scan.l\", \
	| diff scan.c -
	@echo "Check successful, using COMPRESSION=\"$(COMPRESSION)\""

bigcheck:
	rm -f scan.c ; $(MAKE) COMPRESSION="-C" check
	rm -f scan.c ; $(MAKE) COMPRESSION="-Ce" check
	rm -f scan.c ; $(MAKE) COMPRESSION="-Cm" check
	rm -f scan.c ; $(MAKE) COMPRESSION="-Cfea" check
	rm -f scan.c ; $(MAKE) COMPRESSION="-CFer" check
	rm -f scan.c ; $(MAKE) COMPRESSION="-l" PERF_REPORT="" check
	rm -f scan.c ; $(MAKE)
	@echo "All checks successful"

$(FLEXLIB): $(LIBOBJS)
	$(AR) cru $(FLEXLIB) $(LIBOBJS)
	-$(RANLIB) $(FLEXLIB)

flex.man: flex.1
	cd $(srcdir); nroff -man flex.1 >flex.man

flexdoc.man: flexdoc.1
	cd $(srcdir); nroff -man flexdoc.1 >flexdoc.man

install: flex $(FLEXLIB) installdirs install.$(INSTALLMAN)
	$(INSTALL_PROGRAM) flex $(bindir)/flex
	@rm -f $(bindir)/flex++
	cd $(bindir); $(LN_S) flex flex++
	$(INSTALL_DATA) $(FLEXLIB) $(libdir)/libfl.a
	-cd $(libdir); $(RANLIB) libfl.a
	$(INSTALL_DATA) $(srcdir)/FlexLexer.h $(includedir)/FlexLexer.h

install.man: flex.1 flexdoc.1
	$(INSTALL_DATA) $(srcdir)/flex.1 $(mandir)/flex.$(manext)
	$(INSTALL_DATA) $(srcdir)/flexdoc.1 $(mandir)/flexdoc.$(manext)

install.cat: flex.man flexdoc.man
	$(INSTALL_DATA) $(srcdir)/flex.man $(mandir)/flex.$(manext)
	$(INSTALL_DATA) $(srcdir)/flexdoc.man $(mandir)/flexdoc.$(manext)

installdirs:
	$(SHELL) $(srcdir)/mkinstalldirs \
	  $(bindir) $(libdir) $(includedir) $(mandir)

uninstall:
	rm -f $(bindir)/flex $(bindir)/flex++
	rm -f $(libdir)/libfl.a
	rm -f $(includedir)/FlexLexer.h
	rm -f $(mandir)/flex.$(manext) $(mandir)/flexdoc.$(manext)

tags: $(SOURCES)
	ctags $(SOURCES)

TAGS: $(SOURCES)
	etags $(SOURCES)

lint: $(LINTSRCS)
	lint -Dconst= $(LINTSRCS) > flex.lint

gcc-lint: $(LINTSRCS)
	gcc -Dlint -Wall $(LINTSRCS) >flex.gcc-lint 2>&1

mostlyclean:
	rm -f *~ a.out *.bak core errs scan.tmp

clean: mostlyclean
	rm -f parse.c parse.h *.o alloca.c *.lint lex.yy.c lex.yy.cc $(FLEXLIB)

distclean: clean
	rm -f .bootstrap flex scan.c tags TAGS Makefile config.status

realclean: distclean
	rm -f flex.man flexdoc.man flex*.tar.gz flex*.tar.Z

dist: flex $(DISTFILES)
	$(MAKE) DIST_NAME=flex-`sed <version.h 's/[^"]*"//' | sed 's/"//'` dist2

dist2:
	@rm -rf $(DIST_NAME)
	@rm -f $(DIST_NAME).tar $(DIST_NAME).tar.Z $(DIST_NAME).tar.gz
	@mkdir $(DIST_NAME)
	tar cf - $(DISTFILES) | (cd $(DIST_NAME); tar xfB -)
	@mv $(DIST_NAME)/scan.c $(DIST_NAME)/initscan.c
	@chmod 444 $(DIST_NAME)/initscan.c
	@chmod +w $(DIST_NAME)/Makefile.in
	tar chf $(DIST_NAME).tar $(DIST_NAME)
	compress <$(DIST_NAME).tar >$(DIST_NAME).tar.Z
	gzip <$(DIST_NAME).tar >$(DIST_NAME).tar.gz
	@rm $(DIST_NAME).tar

Makefile: Makefile.in config.status
	$(SHELL) config.status
config.status: configure
	$(SHELL) config.status --recheck
configure: configure.in
	cd $(srcdir); autoconf

# Prevent GNU make v3 from overflowing arg limit on SysV.
.NOEXPORT:
