# LEMIS surrogate Makefile. This package did not contain a top-level Makefile,
# so this was added in the interests of uniformity.
CC=gcc
CFLAGS=-g -O3
LFLAGS=-g
NROFF=nroff
BINDIR=/opt/bin
LIBDIR=/opt/lib/ghostscript
MANDIR=/opt/man
CAT1DIR=cat1
INFODIR=/opt/info

all:

man:

doc:

install:  all
	if [ ! -d $(LIBDIR) ]; then \
	  mkdir -p $(LIBDIR); \
	fi
	cp *.gsf *.afm $(LIBDIR)
	chmod 644 $(LIBDIR)/*

clean:


