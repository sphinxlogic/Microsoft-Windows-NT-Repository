# LEMIS surrogate Makefile. This package did not contain a top-level Makefile,
# so this was added in the interests of uniformity.
CC=gcc
CFLAGS=-g -O3
LFLAGS=-g
NROFF=nroff
BINDIR=/opt/bin
LIBDIR=/opt/lib
MANDIR=/opt/man
CAT1DIR=cat1
INFODIR=/opt/info
PROGRAM=
SUBDIRS=doc man src

all:
	(cd src; make all)

man:

doc:	makedoc

makedoc:
	(cd doc; make)

install:  all makedoc
	for i in ${SUBDIRS}; do \
	  (cd $$i; make install) \
	done
	
clean:
	rm -f \#* *~ core $(PROGRAM) *.o
	for i in ${SUBDIRS}; do \
	  (cd $$i; make clean) \
	done
	

