# LEMIS surrogate Makefile. This package did not contain a top-level Makefile,
# so this was added in the interests of uniformity.
CC=gcc
CFLAGS=-g -O3
LFLAGS=-g
LIBS=-lsocket -lnsl
NROFF=nroff
BINDIR=/opt/bin
LIBDIR=/opt/lib
MANDIR=/opt/man
CAT1DIR=cat1
INFODIR=/opt/info
PROG=xtset

all:	$(PROG)

$(PROG):	$(PROG).c
	$(CC) $(CFLAGS) $(PROG).c -o $(PROG) $(LIBS)

man:

doc:

install:  all
	cp $(PROG) $(BINDIR)
	compress < $(PROG).doc > $(MANDIR)/$(CAT1DIR)/$(PROG).1.Z

clean:
	rm -f \#* *~ core $(PROG) *.o

