#
#  Makefile for the various face format filters.
#
#  @(#)Makefile 1.12 91/10/24
#
#  Copyright (c) Rich Burridge - Sun Microsystems, Australia.
#
#  Permission is given to distribute these sources, as long as the
#  copyright messages are not removed, and no monies are exchanged. 
#
#  No responsibility is taken for any errors inherent either
#  to the comments or the code of this program, but if reported
#  to me then an attempt will be made to fix them.

# BINDIR, NOINDEX and SYSV are expected to be overridden by the calling
# Makefile

#--------------------------------------------------------------------------
BINDIR		= /opt/bin
#--------------------------------------------------------------------------
#  Not all machines have the index() string library function. If you
#  don't have this function then you should uncomment the NOINDEX
#  definition below.
#
#NOINDEX	= -DNOINDEX
#--------------------------------------------------------------------------
#  If you are running on a Unix System V machine, then you should uncomment
#  the next definition.
#
#SYSV           = -DSYSV32
#--------------------------------------------------------------------------

BINARIES	= icon2ikon ikon2icon rs2icon 48icon2ikon

CC		= cc
CDEFS		= $(NOINDEX) $(SYSV)
CCOMP		= -g
CFLAGS		= $(CCOMP) $(CDEFS)

all:		$(BINARIES)

icon2ikon:	icon2ikon.o
		$(CC) -o icon2ikon $(CFLAGS) icon2ikon.o

ikon2icon:	ikon2icon.o
		$(CC) -o ikon2icon $(CFLAGS) ikon2icon.o

rs2icon:	rs2icon.o
		$(CC) -o rs2icon $(CFLAGS) rs2icon.o

vpxtoicon:	vpxtoicon.o
		$(CC) -o vpxtoicon $(CFLAGS) vpxtoicon.o

clean:
		rm -f *.o $(BINARIES)

install:	$(BINARIES)
		install -s -m 751 -f $(BINDIR) icon2ikon 
		install -s -m 751 -f $(BINDIR) ikon2icon 
		install -s -m 751 -f $(BINDIR) rs2icon 
		install -s -m 755 -f $(BINDIR) fs2ikon 
		install -s -m 755 -f $(BINDIR) fs2xbm 
		install -s -m 755 -f $(BINDIR) ikon2xbm 
		install -s -m 755 -f $(BINDIR) xbm2ikon 
		install -s -m 755 -f $(BINDIR) xbmcut48 
		install -s -m 755 -f $(BINDIR) xbmsize48 

install-sysv:	$(BINARIES)
		if [ -d $(BINDIR) ] ; then \
			true ; \
		else \
			mkdir $(BINDIR) ; \
		fi
		install -s -m 751 icon2ikon $(BINDIR)
		install -s -m 751 ikon2icon $(BINDIR)
		install -s -m 751 rs2icon $(BINDIR)
		install -s -m 755 fs2ikon $(BINDIR)
		install -s -m 755 fs2xbm $(BINDIR)
		install -s -m 755 ikon2xbm $(BINDIR)
		install -s -m 755 xbm2ikon $(BINDIR)
		install -s -m 755 xbmcut48 $(BINDIR)
		install -s -m 755 xbmsize48 $(BINDIR)
