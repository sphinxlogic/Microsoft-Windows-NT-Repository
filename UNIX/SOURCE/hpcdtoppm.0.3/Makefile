CC=gcc
CFLAGS=-g -O3
LFLAGS=-g
NROFF=nroff
BINDIR=/opt/bin
LIBDIR=/opt/lib
MANDIR=/opt/man
CAT1DIR=cat1
INFODIR=/opt/info

all: hpcdtoppm

man:

CC=gcc
COPTS=  -O3 -g

# if you get problems with compilation of "signed",
# read the README.TOO and try something like -DsBYTE=char
hpcdtoppm: hpcdtoppm.c 
	${CC} ${CFLAGS} -DOWN_WRITE -o hpcdtoppm hpcdtoppm.c

# Targets added by Greg Lehey, LEMIS, 15 May 1993

install: hpcdtoppm
	cp hpcdtoppm $(BINDIR)
	cp simple_viewer.sh $(BINDIR)/viewpcd
	$(NROFF) -man hpcdtoppm.man  > $(MANDIR)/$(CAT1DIR)/hpcdtoppm.1

clean:
	rm -f \#* *~ core hpcdtoppm *.o

