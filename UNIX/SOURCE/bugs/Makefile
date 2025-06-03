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

all:	$(PROGRAM)

man:

doc:

install:  all
	cp $(PROGRAM) $(BINDIR)
	$(NROFF) -man $(MANPAGE)  > $(MANDIR)/$(CAT1DIR)/$(MANPAGE).0

clean:
	rm -f \#* *~ core $(PROGRAM) *.o

