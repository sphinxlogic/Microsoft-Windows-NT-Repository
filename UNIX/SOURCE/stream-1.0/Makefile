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
PROGRAM=stream

all:	$(PROGRAM)

stream.o:	stream.c local.h

man:

doc:

install:  all
	cp $(PROGRAM) $(BINDIR)
	cp stream.1 /opt/man/cat1

clean:
	rm -f \#* *~ core $(PROGRAM) *.o

