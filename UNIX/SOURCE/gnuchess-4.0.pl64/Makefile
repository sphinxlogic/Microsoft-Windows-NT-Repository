# LEMIS surrogate Makefile. This package did not contain a top-level Makefile,
# so this was added in the interests of uniformity.
CC=gcc
CFLAGS=-g -O3
LFLAGS=-g
NROFF=nroff
BINDIR=/opt/bin
LIBDIR=/opt/lib
MANDIR=/opt/man
MAN1DIR=man1
INFODIR=/opt/info
PROGRAM=
SUBDIRS=doc misc src test

all:
	(cd src; make all)

man:

doc:

install:  all
	(cd src; make install)
	(cd doc; cp -p *.1 $(MANDIR)/$(MAN1DIR))

clean:
	rm -f \#* *~ core $(PROGRAM) *.o
	(cd src; make clean)

