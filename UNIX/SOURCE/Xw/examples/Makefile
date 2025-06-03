# LEMIS surrogate Makefile. This package did not contain a top-level Makefile,
# so this was added in the interests of uniformity.
RESULT=demo3
CC=gcc
CFLAGS=-g -O3
LFLAGS=-g
NROFF=nroff
X=/usr/X11
BINDIR=/opt/bin
LIBDIR=/opt/lib
MANDIR=/opt/man
CAT1DIR=cat1
INFODIR=/opt/info
INCLUDES=-I../Xw -I$(X)/include
LIBS=-L$(X)/lib -lXw -lXt -lX11

all:	$(RESULT)

man:

doc:

install:  all
	cp ?? $(BINDIR)
	$(NROFF) -man ?? |compress > $(MANDIR)/$(CAT1DIR)/??.1.Z

clean:
	rm -f \#* *~ core $(RESULT) *.o

$(RESULT):	$(RESULT).c
	$(CC) $(CFLAGS) $(INCLUDES) $(RESULT).c -o $(RESULT) $(LIBS)
	