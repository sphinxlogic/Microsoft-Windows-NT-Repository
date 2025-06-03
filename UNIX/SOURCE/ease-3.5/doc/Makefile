#! /bin/make -f
#
# $Header: /home/alydar/u0/barnett/Src/Ease/ease/doc/RCS/Makefile,v 1.2 1991/11/25 18:31:53 barnett Exp $
#
# $Log: Makefile,v $
# Revision 1.2  1991/11/25  18:31:53  barnett
# Changes by Paul A. Vixie
#
# Revision 1.1  1991/11/25  18:24:01  barnett
# Initial revision - from 1.2 version
#
#
#
#	Makefile for Ease document.
#
#	James S. Schoner
#	Purdue University Computing Center
#	Modifications by Paul A. Vixie <vixie@pa.dec.com>
#

EXT=l
MANDEST = /opt/man/man${EXT}
ROFF=nroff -t

DOCS = cover ease.paper ap1 ap2 ap3 ap4
MANS = ease.man et.man cfc.man

all:; @echo "'all' is a no-op here.  use install, printman, or printpaper"

install: ${MANS} FRC
	if [ ! -d $(MANDEST) ]; then \
	  mkdir -p $(MANDEST); \
	fi
	install -c ease.man $(MANDEST)/ease.${EXT}
	install -c et.man $(MANDEST)/et.${EXT}
	install -c cfc.man $(MANDEST)/cfc.${EXT}

printpaper: ${DOCS} FRC
	${ROFF} -ms ${DOCS} >crap
	gs -q -sOUTPUTFILE=/dev/lp0 -sDEVICE=ljetplus crap
	rm crap

printman: ${MANS} FRC
	${ROFF} -man ${MANS} >crap
	gs -q -sOUTPUTFILE=/dev/lp0 -sDEVICE=ljetplus crap
	rm crap

clean:

FRC:
