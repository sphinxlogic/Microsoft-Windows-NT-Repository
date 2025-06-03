#
#	Make the psf v3 postscript filter system
#	----------------------------------------

#	display all "make" options with:
#
#		make
#	or:
#		make help

SHELL = /bin/sh

# - - - - Begin configuration - - - - -

# peculiar systems setup.
# set OS=-DISC22    to get around include file problems with pid_t
#                   with Interactive Unix 2.2:  not needed if gcc is used.
# OS=-DISC22

# set compiler, tar and nroff names

# CC=cc
CC = gcc
# CC=lpicc -sys -xid

TAR = tar
# TAR = gtar

NROFF = /usr/ucb/nroff
# NROFF = cawf

# man directory  (if you have man or catman directories)
# MANDIR=/usr/share/man/cat1
MANDIR=/opt/man/cat1
# MANDIR=/usr/catman/u_man/manl
# MANDIR=/usr/catman/u_man/man1
# MANDIR=/usr/man/man.1

# suffix for man pages
# MANSUF=l
# MANSUF=1
MANSUF=1

# If catman/formatted pages should be filtered with col
COL=
# COL=| col -b 

# user print interface
# UPRINT =lpr
UPRINT =lp

# Location of user binaries and scripts  (for both lp/lpsched and lpr/lpd)
BINDIR=/opt/bin
# BINDIR=/local/bin

# lpd filter directory  (ignored for lp/lpsched)
LPDDIR=/opt/lib/psf
# LPDDIR=/local/lib/psf

# Location of printer definition file   (for lpr/lpd or lp/lpsched)
DEFFILE=/opt/lib/psf/psfprint.def
# DEFFILE=/local/lib/psf/psfprint.def
# DEFFILE=/usr/spool/lp/model/psfprint.def

# A CTL/D is sent to the printer at the end of every print job if psf
# is used as a spooler filter.  The "psf" binary does not generate
# a ctl/d if it is used in shell scripts or at the shell prompt.
# Enable CTLDALWAYS if the CTL/D must be generated in all cases.
CTLD=
# CTLD=-DCTLDALWAYS

# Set the default paper type  (letter, legal, a4, b5, etc.)
PSPTYPE = a4

# By default, psf uses standard character set encoding and
# allows ISO Latin1 encoding with the -I option.  If ISO Latin1
# encoding is desired as the default, set ISOLATIN=-DISO
# ISOLATIN=
ISOLATIN=-DISO

# Psf assumes that the default paper tray is enabled by default
# in the printer.  If the default paper tray cannot be set as defined
# in PSTYPE above, or to always force tray selection, activate the following:
FORCETYPE=-DFORCETYPE

# if 2 paper trays, specifiy -DHOPPER  for double sided printing.
#		Second tray should be reserved for double sided printing only!
TRAYS=
# TRAYS=-DHOPPER

# compile and library options
LDFLAGS = -lm
CFLAGS = -O3 -g $(NAMES) $(PAPTYPE) $(TRAYS) $(FORCETYPE) -DBINDIR=\"$(BINDIR)\" -DPDEF=\"$(DEFFILE)\"  $(CTLD) $(ISOLATIN)

# extra things for SysV or Xenix with lp/lpsched
MODEL=/usr/lib/lp/model

# if getopt isn't a standard part of your system
# GETOPT = getopt.o

# -- usually, nothing below needs changing --

# Names of executable binaries  (See "Multiple printer support" in README)
PSF = psf
PSFDOUB = psfdoub
PSFBANNER = psfbanner
PSFFILTER = psffilter
PSFLPD = psflpd

RCSVER = 3.5
RCSCOM="psf3, patch 5"

# - - - - - end configuration - - - - - - - -
#           ^^^^^^^^^^^^^^^^^

# psf uses NAMES to find the current name of psfdoub

NAMES = -DPSFDOUB=\"$(PSFDOUB)\"

SOURCES  = psf.c psfbs.c psfmail.c psfdoub.c psfbanner.c psdetect.c \
		selectdef.c table.c psf.h psfproto.h ctp.h
OBJECTS  = psf.o psfbs.o psfmail.o psfdoub.o psfbanner.o psdetect.o
BINARIES = $(PSF) psfbs psfmail $(PSFDOUB)
LPBIN    = $(PSFBANNER) $(PSFFILTER)
LPDBIN   = $(PSFLPD)
SCRIPTS  = psfnroff psfman psfmbox
SUPPORT  = selectdef table
OTHERS	 = Makefile README  NOTICE Install.unix Install.lpd Install.def \
		Testfile psfunix.LP getsizes.PS psfxenix.LP \
		nlsdumb psfmbox.SH psfman.SH psfnroff.SH getopt.c Install.dos \
		Changes Makefile.dos psfnroff.bat printcap.ETC \
		psfdouble.SH psdouble.bat getprint.bat language.PSF psflpd.DEF \
		printcap.NET
PATCHLEV = patchlevel.h
DOSSRC   = psf.c psfbs.c psfdoub.c selectdef.c psf.h psfnroff.bat \
		patchlevel.h getsizes.PS getopt.c getprint.bat psfproto.h ctp.h
DOSMAN   = man/psf.1 man/psfbs.1 man/psfdoub.1 man/psfnroff.1
DOSDOC   = man/psf.doc man/psfbs.doc man/psfdoub.doc man/psfnroff.doc
DOSOTHER = Makefile.dos NOTICE README Install.dos Install.def Testfile \
		example psdouble.bat language.PSF


PTYPE = \"$(PSPTYPE)\"
PAPTYPE = -DPAPERTYPE=$(PTYPE)



# make help
# ^^^^^^^^^
help:
	@echo " "
	@echo "First:  make the following in order"
	@echo "  'make all'          compile sources"
	@echo "  'make printer'      select a printer"
	@echo "  'make scripts'      tailor scripts"
	@echo " "
	@echo "Then:  select one of the following installations"
	@echo "  'make install'      install all user and spooler files"
	@echo "  'make installuser'  install only user binaries and scripts"
	@echo "  'make update'       re-install all binaries after a source patch"
	@echo "  'make updateuser'   re-install only user binaries after a source patch"
	@echo "  'make installxenix' may work - no longer spported"
	@echo " "
	@echo "Others:"
	@echo "  'make installman'   install unformatted $(MANDIR) pages"
	@echo "  'make installcat'   format/install $(MANDIR) pages (needs nroff)"
	@echo "  'make installdoc'   install formatted $(MANDIR) pages (no nroff)"
	@echo "  'make doc'          generate formatted .doc from .1 pages (needs nroff)"
	@echo "  'make tar'          tar the original sources  (/tmp/psf.tar)"
	@echo "  'make dostar'       tar things useful for dos (/tmp/psfdos.tar)"
	@echo "  'make shar'         make shar'ed kit for email"
	@echo "  'make clean'        remove anything that can be rebuilt"



# Compile all sources
# ^^^^^^^
all:	$(BINARIES) $(LPBIN) $(LPDBIN) $(SUPPORT) getsizes.ps

$(PSF):	psf.c psf.h ctp.h $(GETOPT) psfproto.h
	$(CC) $(CFLAGS) psf.c $(GETOPT) -o $(PSF) $(LDFLAGS)

$(PSFDOUB):	psfdoub.c psf.h $(GETOPT)
	$(CC) $(CFLAGS) psfdoub.c $(GETOPT) -o $(PSFDOUB) $(LDFLAGS)

$(PSFFILTER):	psf.c psf.h $(GETOPT) ctp.h psfproto.h
	$(CC) $(CFLAGS) -DPSFFILTER psf.c $(GETOPT) -o $(PSFFILTER) $(LDFLAGS)

$(PSFLPD):	psf.c $(GETOPT) ctp.h psf.h psfproto.h
	$(CC) $(CFLAGS) -DFORLPD -DPSFFILTER $(GETOPT) psf.c -o $(PSFLPD) $(LDFLAGS)

$(PSFBANNER):	psfbanner.c psf.h ctp.h $(GETOPT)
	$(CC) $(CFLAGS) psfbanner.c $(GETOPT) -o $(PSFBANNER) $(LDFLAGS)

psfbs:		psfbs.c psf.h $(GETOPT)
	$(CC) $(CFLAGS) psfbs.c $(GETOPT) -o psfbs $(LDFLAGS)

psfmail:	psfmail.c psf.h $(GETOPT)
	$(CC) $(CFLAGS) psfmail.c $(GETOPT) -o psfmail $(LDFLAGS)

selectdef:	selectdef.c psf.h ctp.h $(GETOPT)
	$(CC) $(CFLAGS) selectdef.c $(GETOPT) -o selectdef $(LDFLAGS)

table:		table.c psf.h $(GETOPT)
	$(CC) $(CFLAGS) table.c $(GETOPT) -o table $(LDFLAGS)

getsizes.ps:
	sed "s@%letter@$(PSPTYPE)@g" getsizes.PS > getsizes.ps

# make all scripts for both lpr and lp.  install will use as needed

scripts:
	sed "s@PRINTER@$(UPRINT)@g" psfmbox.SH  > psfmbox
	sed "s@PRINTER@$(UPRINT)@g" psfman.SH   > psfman
	sed "s@PRINTER@$(UPRINT)@g" psfnroff.SH > psfnroff
	sed "s@PRINTER@$(UPRINT)@g" psfdouble.SH > psfdouble
	sed "s@LPDFILTERS@$(LPDDIR)@g" printcap.ETC > printcap
	sed "s@BINARY@$(BINDIR)@g" psfunix.LP \
		| sed "s@PAGETYPE@$(PSPTYPE)@g"   \
		| sed "s@DEFFILE@$(DEFFILE)@g" > psf.model
	chmod +x $(SCRIPTS) psf.model


# Create a printer selection
# ^^^^^^^^^^^^^^^^^^^^^^^^^^
printer:
	./selectdef defs/*.def
	@test -r psfprint.def || ( echo "Error: cannot find psfprint.def" ;  exit 1)
	@chmod 644 psfprint.def
	@if ( test  $(UPRINT) = lpr )  then rm -f zz91 ; \
		 mv psfprint.def zz91 ; \
		 cat psflpd.DEF zz91 > psfprint.def ; \
		 rm zz91 ; fi

# Install system
# ^^^^^^^^^^^^^^
install:
	@if ( test $(UPRINT) = lpr ) then make installbsd ; \
	    else make installsysv ; fi

installbsd:
	@echo "Full installation for lpr/lpd spooler systems."
	@test -r psfprint.def || ( make help ; exit 1)
	@test -r psfmbox || ( make help ; exit 1)
	@test -w $(LPDDIR) || ( echo Directory $(LPDDIR) not available ; exit 1)
	@test -w $(BINDIR) || ( echo Directory $(BINDIR) not available ; exit 1)
	strip $(LPDBIN) $(LPBIN) $(BINARIES) $(SUPPORT)
	cp $(LPDBIN) $(LPDDIR)
	cp $(BINARIES) $(SCRIPTS) $(BINDIR)
	cp psfprint.def $(DEFFILE)
	ln -f $(LPDDIR)/$(PSFLPD)  $(LPDDIR)/post
	ln -f $(LPDDIR)/$(PSFLPD)  $(LPDDIR)/p2up
	ln -f $(LPDDIR)/$(PSFLPD)  $(LPDDIR)/p4up
	ln -f $(LPDDIR)/$(PSFLPD)  $(LPDDIR)/p132x60
	ln -f $(LPDDIR)/$(PSFLPD)  $(LPDDIR)/paccount
	ln -f $(LPDDIR)/$(PSFLPD)  $(LPDDIR)/psource
	ln -f $(LPDDIR)/$(PSFLPD)  $(LPDDIR)/plegal
	ln -f $(LPDDIR)/$(PSFLPD)  $(LPDDIR)/pspread

installsysv:
	@echo "Full installation for lp/lpsched spooler systems."
	@test -r psfprint.def || ( make help ; exit 1)
	@test -r psfmbox || ( make help ; exit 1)
	@test -w $(BINDIR) || ( echo Directory $(BINDIR) not available ; exit 1)
	cp psfprint.def $(DEFFILE)
	cp psf.model $(MODEL)/psf
	chown lp $(MODEL)/psf $(DEFFILE)
	chgrp bin $(MODEL)/psf $(DEFFILE)
	strip $(BINARIES) $(LPBIN) $(SUPPORT)
	cp $(BINARIES) $(LPBIN) $(SCRIPTS) $(BINDIR)

installuser:
	@echo "User binary and script installation. (Spooler support not installed.)"
	@test -r psfprint.def || ( make help ; exit 1)
	@test -r psfmbox || ( make help ; exit 1)
	@test -w $(BINDIR) || ( echo Directory $(BINDIR) not available ; exit 1)
	cp psfprint.def $(DEFFILE)
	strip $(BINARIES) $(SUPPORT)
	cp $(BINARIES) $(SCRIPTS) $(BINDIR)

installxenix:			# SysV except for printer model details.
	@echo "Full Xenix installation for lp/lpsched spooler systems."
	@test -r psfprint.def || ( make help ; exit 1)
	@test -r psfmbox || ( make help ; exit 1)
	@test -w $(BINDIR) || ( echo Directory $(BINDIR) not available ; exit 1)
	sed "s@BINARY@$(BINDIR)@g" psfxenix.LP \
		| sed "s@PAGETYPE@$(PTYPE)@g"   \
		| sed "s@DEFFILE@$(DEFFILE)@g" > psf.model
	chmod +x psf.model
	cp psfprint.def $(DEFFILE)
	cp psf.model $(MODEL)/psf
	chown lp $(MODEL)/psf $(DEFFILE)
	chgrp bin $(MODEL)/psf $(DEFFILE)
	strip $(BINARIES) $(SUPPORT)
	cp $(BINARIES) $(SCRIPTS) $(BINDIR)

update:
	@if ( test $(UPRINT) = lpr ) then make updatebsd ; \
	    else make updatesysv ; fi

updatebsd:
	@echo "Binary only update for lpr/lpd spooler systems."
	@test -w $(LPDDIR) || ( echo Directory $(LPDDIR) not available ; exit 1)
	@test -w $(BINDIR) || ( echo Directory $(BINDIR) not available ; exit 1)
	strip $(LPDBIN) $(LPBIN) $(BINARIES) $(SUPPORT)
	cp $(LPDBIN) $(LPDDIR)
	cp $(BINARIES) $(BINDIR)

updatesysv:
	@echo "Binary only update for lp/lpsched spooler systems."
	@test -w $(BINDIR) || ( echo Directory $(BINDIR) not available ; exit 1)
	strip $(BINARIES) $(LPBIN) $(SUPPORT)
	cp $(BINARIES) $(LPBIN) $(BINDIR)

updateuser:
	@echo "User binary re-install"
	@test -w $(BINDIR) || ( echo Directory $(BINDIR) not available ; exit 1)
	strip $(BINARIES)
	cp $(BINARIES) $(BINDIR)


# Man page generation
# ^^^^^^^^^^^^^^^^^^^

installman:
	cd man ; make installman MANSUF=$(MANSUF) MANDIR=$(MANDIR) NROFF=$(NROFF) COL="$(COL)"

installcat:
	cd man ; make installcat MANSUF=$(MANSUF) MANDIR=$(MANDIR) NROFF=$(NROFF) COL="$(COL)"

installdoc:
	cd man ; make installdoc MANSUF=$(MANSUF) MANDIR=$(MANDIR) NROFF=$(NROFF) COL="$(COL)"

doc:
	cd man ; make doc MANSUF=$(MANSUF) MANDIR=$(MANDIR) NROFF=$(NROFF) COL="$(COL)"

# Copy distribution files
# ^^^^^^^^^^^^^^^^^^^^^^^
tar:
	$(TAR) cvf /tmp/psf.tar $(SOURCES) man $(OTHERS) defs Makefile.dis Makefile.ajf patchlevel.h

shar:
	makekit -m -s80k defs man man/*.1 man/*.doc man/Makefile defs/* $(SOURCES) $(OTHERS) patchlevel.h

tardis:
	$(TAR) cvf /tmp/psf.tar man/*.1 man/*.doc man/Makefile defs/*.def $(SOURCES) $(OTHERS) patchlevel.h

dostar:
	$(TAR) cvf /tmp/psfdos.tar $(DOSSRC) $(DOSMAN) $(DOSDOC) $(DOSOTHER) defs


# Create .def files from .ppd
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^

def:
	table $$HOME/adobe/ppd/*.ppd defs

# RCS checkin and out
# ^^^^^^^^^^^^^^^^^^^

ci:
	ci -r$(RCSVER) patchlevel.h
	ci -r$(RCSVER) -m$(RCSCOM) $(SOURCES) $(OTHERS) lines.c Makefile.dis Makefile.ajf

ciman:
	cd man ; make ci RCSVER=$(RCSVER) RCSCOM=$(RCSCOM)
	

co:
	co -l $(SOURCES) $(OTHERS) Makefile.dis Makefile.ajf Makefile patchlevel.h lines.c

coman:
	cd man ; make co RCSVER=$(RCSVER)

# Remove anything that can be rebuilt
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
clean:
	rm -f $(BINARIES) $(SCRIPTS) $(SUPPORT) $(LPBIN) $(LPDBIN) psfdouble
	rm -f *.o 
	rm -f *.B
	rm -f *.doc
	rm -f *~
	rm -f psf.model getsizes.ps psfprint.def psizes.ps printcap
	cd man ; make clean
