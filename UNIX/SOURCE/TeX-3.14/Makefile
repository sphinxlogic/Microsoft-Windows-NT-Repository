# Overall Makefile for making TeX, Metafont, and friends, using WEB to C.
 

# TeX 3.14 + Metafont 2.7 + web2c b.
# 
version=5.84b

# Directory that site.h is in relative to subdirectories, i.e., `..'
# means the directory containing this Makefile.
SITEDIR=..

# Default C compiler.
CC=gcc

# Default flags to give the C compiler.  (Don't define CFLAGS directly.)
OPT=-g -O3

# Default flags to give to the loader.
LDFLAGS=-g

# Default libraries with which to link (libraries for the Metafont
# support are specified separately).
LOADLIBES=-lnsl

# Make sure we use the sh.  If your sh is broken, you might try bash,
# the GNU replacement.
SHELL=/bin/sh

# The yacc program to use on web2c/web2c.yacc.
YACC=bison -y

# The lex program to use on web2c/web2c.lex.
LEX=lex
# -ll for lex, -lfl for flex.
LEXLIB=-ll

# Need -D_POSIX_SOURCE here, perhaps, if on a POSIX system.
LEXCFLAGS=

# How to make a link.
LN=ln -s

# Support libraries for Metafont.
x10lib=-lX
x11lib=-lXt -lX11
sunlib=-lsuntool -lsunwindow -lpixrect

# This line should select the libraries that match what you said in site.h.
wlibs=$(x11lib)

# These lines define the format and base files that `make fmts' and
# `make bases' will try to make, and `make install' will try to install.
# See README.W2C for a brief description of each of these formats.
# formats=tex.fmt latex.fmt etex.fmt latex.fmt slitex.fmt \
#        amstex.fmt amslatex.fmt picplus.fmt texinfo.fmt
formats=tex.fmt latex.fmt
bases=mf.base cmmf.base

# The name of the file that defines your local devices, for use with
# Metafont.  (Only relevant during `make bases'.)  I urge you to get the
# file `modes.mf', which defines all known modes, plus useful
# definitions for all fonts.  It's available from ftp.cs.umb.edu
# [192.12.26.23] as pub/tex/modes.mf, among other places.
localmodes=modes

# You may need INSTALL=cp on system V, if you don't have GNU install. 
# Or you can install the programs by hand.
# $(fileinstall) is used for the format and base files, man files, and
# other non-executables, and $(programinstall) is used for
# the executables.
INSTALL=install -g lemis -c
fileinstall=$(INSTALL) -m 664
proginstall=$(INSTALL) -m 775 -s
# NROFF is the program to format man pages (normally nroff)
NROFF=nroff

# These pathnames are used to customize the manual pages with local
# directory names; some of them are used in `make install'.  They should
# match the system directories in the paths defined in `./site.h'.
manext=1
mandir=/opt/man/cat$(manext)

texlibdir=/opt/lib/tex
texbindir=/opt/bin
texpooldir=$(texlibdir)/formats
texinputdir=$(texlibdir)/inputs
texfontdir=$(texlibdir)/fonts
formatdir=$(texlibdir)/formats

mflibdir=/opt/lib/mf
mfbindir=/opt/bin
mfpooldir=$(mflibdir)/bases
mfinputdir=$(mflibdir)/inputs
basedir=$(mflibdir)/bases

# Where the Computer Modern sources are.
# (The man pages claim that you have installed the standard utility
# sources, like `grayf.mf' and `expr.mf', somewhere in $(mfinputdir).)
cmsources=$(mfinputdir)/cmfonts

default: all


# Stuff below here probably doesn't need to be changed.
# 
alldirs=web tex mf texware mfware fontutil bibtex dviutil dvisrc
triptrapdirs=tex texware mf mfware

makeargs=SITEDIR="$(SITEDIR)" CC="$(CC)" OPT="$(OPT)" \
SHELL="$(SHELL)" LDFLAGS="$(LDFLAGS)" LOADLIBES="$(LOADLIBES)" wlibs="$(wlibs)"

web2c=web2c
web2cprograms=$(web2c)/web2c $(web2c)/regfix $(web2c)/splitup \
              $(web2c)/fixwrites
commonsources=common/alloca.c common/extra.c common/main.c common/endian.c

triptrap: stamp-tangle stamp-common
	for name in $(triptrapdirs);					\
        do								\
          (cd $${name}; make $(makeargs) LN="$(LN)" triptrap);		\
        done  

run-triptrap: run-trip run-trap

clean-triptrap:
	cd tex; make veryclean
	cd mf; make veryclean

all:	stamp-web2c stamp-common bases formats
	for name in $(alldirs);						\
        do								\
          (cd $${name}; echo $${name}; make $(makeargs) all);		\
        done

TeX:	stamp-web2c stamp-tangle stamp-common
	cd tex; make $(makeargs) all

BibTeX: stamp-web2c stamp-tangle stamp-common
	cd bibtex; make $(makeargs) all

run-trip:
	cd tex; make run-trip

MF:	stamp-web2c stamp-tangle stamp-common
	cd mf; make $(makeargs) all

run-trap:
	cd mf; make run-trap


# The targets below must be up to date for anything to be made.
# 

stamp-web2c:
	cd $(web2c); \
          make $(makeargs) YACC="$(YACC)" LEX="$(LEX)" LEXLIB="$(LEXLIB)" all
	touch stamp-web2c

stamp-common: $(commonsources)
	cd common; make $(makeargs) all
	touch stamp-common

stamp-tangle: stamp-web2c stamp-common
	cd web; make $(makeargs) tangle
	touch stamp-tangle


manpages:
	cd man; make SITEDIR=$(SITEDIR)					 \
		texbindir=$(texbindir)		texlibdir=$(texlibdir)	 \
		texpooldir=$(texpooldir)	formatdir=$(formatdir)	 \
		texinputdir=$(texinputdir)	texfontdir=$(texfontdir) \
		mfbindir=$(mfbindir)		mflibdir=$(mflibdir)	 \
		mfpooldir=$(mfpooldir)		basedir=$(basedir)	 \
		mfinputdir=$(mfinputdir)	cmsources=$(cmsources)	 \
                all
	cd dvidoc; make all

bases: stamp-bases
stamp-bases:
	MFPOOL=`pwd`/lib/mf/bases ; \
	MFINPUTS=`pwd`/lib/mf/inputs ; \
	export MFPOOL MFINPUTS ; \
	cd mf; \
          make bases="$(bases)" localmodes="$(localmodes)" bases
	touch stamp-bases

formats: stamp-formats

stamp-formats:
	TEXPOOL=`pwd`/lib/tex/formats ; \
	TEXINPUTS=`pwd`/lib/tex/inputs ; \
	export TEXPOOL TEXINPUTS ; \
	cd tex; make formats="$(formats)" fmts
	touch stamp-formats


# Installation targets.
# 
install-manpages:
	cd man; make mandir=$(mandir) INSTALL="$(fileinstall)" \
          SITEDIR="$(SITEDIR)" NROFF="$(NROFF)" manext=$(manext) install

install-everything: install install-formats install-bases install-manpages install-dviutil install-dvialw install-dvibit install-dvican install-dvie72 install-dvieps install-dvigd install-dviimp install-dvijep install-dvijet install-dvil3p install-dvil75 install-dvim72 install-dvimac install-dvimpi install-dvio72 install-dvioki install-dviprx install-dvitos


install:
	if test -d $(texbindir); then exit 0; else mkdir -p $(texbindir); fi
	$(proginstall) texenv		$(texbindir)/texenv
	$(proginstall) tex/initex	$(texbindir)/initex
	$(proginstall) tex/virtex	$(texbindir)/virtex
	$(proginstall) web/tangle	$(texbindir)/tangle
	$(proginstall) web/weave	$(texbindir)/weave
	$(proginstall) texware/dvitype	$(texbindir)/dvitype
	$(proginstall) texware/pltotf	$(texbindir)/pltotf
	$(proginstall) texware/tftopl	$(texbindir)/tftopl
	$(proginstall) fontutil/vftovp	$(texbindir)/vftovp
	$(proginstall) fontutil/vptovf	$(texbindir)/vptovf
	$(proginstall) bibtex/bibtex	$(texbindir)/bibtex
	$(proginstall) dviutil/dvicopy	$(texbindir)/dvicopy
	if test -d $(texpooldir); then exit 0; else mkdir -p $(texpooldir); fi
	-if test -s tex/tex.pool; \
          then (cd tex; $(fileinstall) tex.pool $(texpooldir) ); fi
	if test -d $(mfbindir); then exit 0; else mkdir -p $(mfbindir); fi
	$(proginstall) mf/inimf		$(mfbindir)/inimf
	$(proginstall) mf/virmf		$(mfbindir)/virmf
	$(proginstall) mfware/gftodvi	$(mfbindir)/gftodvi
	$(proginstall) mfware/gftopk	$(mfbindir)/gftopk
	$(proginstall) mfware/gftype	$(mfbindir)/gftype
	$(proginstall) mfware/mft	$(mfbindir)/mft
	$(proginstall) fontutil/pktogf	$(mfbindir)/pktogf
	$(proginstall) fontutil/pktype	$(mfbindir)/pktype
	if test -d $(mfpooldir); then exit 0; else mkdir -p $(mfpooldir); fi
	-if test -s mf/mf.pool; \
          then (cd mf; $(fileinstall) mf.pool $(mfpooldir) ); fi
	cp texenv			$(texbindir)/texenv
	chmod 644			$(texbindir)/texenv
	chown bin			$(texbindir)/texenv
	chgrp bin			$(texbindir)/texenv

install-formats: stamp-formats
	if test -d $(formatdir); then exit 0; else -mkdir -p $(formatdir); fi
	for f in $(formats);						\
	do								\
	  rm -f $(texbindir)/`basename $$f .fmt`;			\
          ln $(texbindir)/virtex $(texbindir)/`basename $$f .fmt`;	\
          $(fileinstall) tex/$$f $(formatdir);				\
	done
	cd $(formatdir); rm -f plain.fmt; ln tex.fmt plain.fmt

install-bases: stamp-bases
	if test -d $(basedir); then exit 0; else -mkdir -p $(basedir); fi
	for f in $(bases);						\
	do								\
	  rm -f $(mfbindir)/`basename $$f .base`;			\
	  ln $(mfbindir)/virmf $(mfbindir)/`basename $$f .base`;	\
          $(fileinstall) mf/$$f $(basedir);				\
	done
	cd $(basedir); rm -f plain.base; ln mf.base plain.base


# Install dvi utility programs (Greg Lehey, LEMIS, 20 April 1993). 
# Note that the texidx supplied here is probably not as good as the
# one in the GNU  texinfo package, so we will not overwrite anything
# we find of that name
install-dviutil:
	$(proginstall) dvisrc/keytst $(texbindir)/keytst
	$(proginstall) dvisrc/lptops $(texbindir)/lptops
	if [ -f $(texbindir)/texidx ] ; then true ; \
		else \
		$(proginstall) dvisrc/texidx $(texbindir)/texidx ; \
		fi
	$(proginstall) dvisrc/tosprx $(texbindir)/tosprx

install-dvialw:
	$(proginstall) dvisrc/dvialw $(texbindir)/dvialw

install-dvibit:
	$(proginstall) dvisrc/dvibit $(texbindir)/dvibit

install-dvican:
	$(proginstall) dvisrc/dvican $(texbindir)/dvican

install-dvie72:
	$(proginstall) dvisrc/dvie72 $(texbindir)/dvie72

install-dvieps:
	$(proginstall) dvisrc/dvieps $(texbindir)/dvieps

install-dvigd:
	$(proginstall) dvisrc/dvigd $(texbindir)/dvigd

install-dviimp:
	$(proginstall) dvisrc/dviimp $(texbindir)/dviimp

install-dvijep:
	$(proginstall) dvisrc/dvijep $(texbindir)/dvijep

install-dvijet:
	$(proginstall) dvisrc/dvijet $(texbindir)/dvijet

install-dvil3p:
	$(proginstall) dvisrc/dvil3p $(texbindir)/dvil3p

install-dvil75:
	$(proginstall) dvisrc/dvil75 $(texbindir)/dvil75

install-dvim72:
	$(proginstall) dvisrc/dvim72 $(texbindir)/dvim72

install-dvimac:
	$(proginstall) dvisrc/dvimac $(texbindir)/dvimac

install-dvimpi:
	$(proginstall) dvisrc/dvimpi $(texbindir)/dvimpi

install-dvio72:
	$(proginstall) dvisrc/dvio72 $(texbindir)/dvio72

install-dvioki:
	$(proginstall) dvisrc/dvioki $(texbindir)/dvioki

install-dviprx:
	$(proginstall) dvisrc/dviprx $(texbindir)/dviprx

install-dvitos:
	$(proginstall) dvisrc/dvitos $(texbindir)/dvitos

	

# Need GNU tar for this.
# 
web2c-dist: web/tangleboot.pas
	@echo -n "check -linet;tangleboot"
	@echo "big{tex,mf.ch; trie change"
	mv ChangeLog ChangeLog.W2C
	cd ..;								\
	  find src-$(version) \! -name '*.web' -type f -print > /tmp/files; \
	  tar czfT web2c-$(version).tar.Z /tmp/files;			\
          ls -ld web*
	mv ChangeLog.W2C ChangeLog

web-dist: web/tangleboot.pas
	cd ..;								\
          find src-$(version) -name '*.web' -print > /tmp/webfest;	\
          tar czfT web-$(version).tar.Z /tmp/webfest;			\
          ls -ld web*

web/tangleboot.pas:
	cd web; make tangle.p; cp tangle.p tangleboot.pas; make veryclean

clean:
	rm -f stamp-*
	for name in web2c common $(alldirs);				\
        do								\
          (cd $${name}; make clean);					\
        done

veryclean:
	rm -f stamp-*
	for name in web2c common man $(alldirs);			\
        do								\
          (cd $${name}; make veryclean);				\
        done
	rm -f \#*\# *~ *.bak *.ckp core

.PHONY: default triptrap run-triptrap clean-triptrap all TeX run-trip \
	MF run-trap manpages bases formats install-manpages install \
        install-formats install-bases web2c-dist web-dist clean veryclean
