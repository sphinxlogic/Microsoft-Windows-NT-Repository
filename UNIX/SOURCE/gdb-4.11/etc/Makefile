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
# 
# Makefile.in for etc
#

prefix = /opt
exec_prefix = $(prefix)

srcdir = .
bindir  = $(exec_prefix)/bin
libdir  = $(exec_prefix)/lib
tooldir = $(exec_prefix)/i386-univel-sysv4.2
datadir = $(prefix)/lib

mandir  = $(prefix)/man
man1dir = $(mandir)/man1
man2dir = $(mandir)/man2
man3dir = $(mandir)/man3
man4dir = $(mandir)/man4
man5dir = $(mandir)/man5
man6dir = $(mandir)/man6
man7dir = $(mandir)/man7
man8dir = $(mandir)/man8
man9dir = $(mandir)/man9
infodir = $(prefix)/info

SHELL = /bin/sh

INSTALL 	= install -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA    = $(INSTALL)

RM	= rm -f

MAKEINFO = makeinfo

#### Host, target, and site specific Makefile fragments come in here.
###

INFOFILES = configure.info standards.info cfg-paper.info
DVIFILES = configure.dvi standards.dvi cfg-paper.dvi

all:

install:  $(srcdir)/configure.man
	$(INSTALL_DATA) $(srcdir)/configure.man $(man1dir)/configure.1

info: $(INFOFILES)

install-info:
	for i in *.info* ; do \
	  $(INSTALL_DATA) $$i $(infodir) ; \
	done

dvi : $(DVIFILES)

configure.info: $(srcdir)/configure.texi
	$(MAKEINFO) -o configure.info $(srcdir)/configure.texi

configure.dvi : $(srcdir)/configure.texi
	$(TEXI2DVI) $(srcdir)/configure.texi

standards.info: $(srcdir)/standards.texi
	$(MAKEINFO) -I$(srcdir) -o standards.info $(srcdir)/standards.texi

standards.dvi : $(srcdir)/standards.texi
	$(TEXI2DVI) $(srcdir)/standards.texi

cfg-paper.info : $(srcdir)/cfg-paper.texi
	$(MAKEINFO) -o cfg-paper.info $(srcdir)/cfg-paper.texi

cfg-paper.dvi : $(srcdir)/cfg-paper.texi
	$(TEXI2DVI) $(srcdir)/cfg-paper.texi


clean:
	$(RM) *.info *.info-*

mostlyclean: clean
distclean:   clean
	rm -f Makefile config.status

realclean:   distclean

Makefile: $(srcdir)/Makefile.in $(host_makefile_frag) $(target_makefile_frag)
	$(SHELL) ./config.status

## these last targets are for standards.texi conformance
dist:
check:
installcheck:

TAGS:
