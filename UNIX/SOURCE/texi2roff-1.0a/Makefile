# Makefile modified by Greg Lehey, LEMIS, 17 May 1993
CC=gcc
CFLAGS=-g -O3
LFLAGS=-g
NROFF=nroff
BINDIR=/opt/bin
LIBDIR=/opt/lib
MANDIR=/opt/man
CAT1DIR=cat1
INFODIR=/opt/info

# Makefile for Texinfo to nroff/troff translator (texi2roff)

CC = gcc

# Flags:
# -DBSD controls use of string(s).h and strchr/index only (not needed for Sun)
# use -Dvoid=int for System III and V7

CFLAGS = -O3 -g

OBJECTS = texi2roff.o table.o translate.o

all:	texi2roff

texi2roff: $(OBJECTS)
	$(CC) -o $@ $(CFLAGS) $(OBJECTS)

texi2roff.o: texi2roff.h

translate.o: texi2roff.h

table.o: texi2roff.h tablems.h tablemm.h tableme.h

bundle:
	bundle texi2roff.1 Readme copyright Makefile  *.h *.c >bundled
	ls -l bundled

man:

doc:

install:  all
	cp texi2roff $(BINDIR)
	$(NROFF) -man texi2roff.1  > $(MANDIR)/$(CAT1DIR)/texi2roff.1

clean:
	rm -f \#* *~ core errs *.o


