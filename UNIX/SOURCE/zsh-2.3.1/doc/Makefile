# LEMIS surrogate Makefile. This package did not contain a top-level Makefile,
# so this was added in the interests of uniformity.
CC=gcc
CFLAGS=-g -O3
LFLAGS=-g
NROFF=nroff
TROFF=groff
BINDIR=/opt/bin
LIBDIR=/opt/lib
MANDIR=/opt/man
CAT1DIR=cat1
INFODIR=/opt/info
PROGRAM=

all:	intro.ps zsh.dvi

intro.ps:
	${TROFF} -ms intro.troff >intro.ps

zsh.dvi:
	tex zsh.texi
	texindex zsh.??
	tex zsh.texi

install:  

clean:
	rm -f \#* *~ core $(PROGRAM) *.o *.ps zsh.?? zsh.???

