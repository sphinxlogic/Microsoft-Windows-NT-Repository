# LEMIS surrogate Makefile. This package did not contain a top-level Makefile,
# so this was added in the interests of uniformity.
CC=gcc
CFLAGS=-g -O3
LFLAGS=-g
NROFF=nroff
BINDIR=/opt/bin
LIBDIR=/opt/lib
MANDIR=/opt/man
CAT6DIR=cat6
INFODIR=/opt/info

all:
	(cd src; make all)

man:

doc:

install:  all
	set -x
	-echo ===\> Installing binaries from `pwd`/src
	(cd src; make install)
	-echo ===\> Installing man pages from `pwd`
	$(NROFF) -man man6/xpilots.6  > $(MANDIR)/$(CAT6DIR)/xpilots.1
	$(NROFF) -man man6/xpilot.6  > $(MANDIR)/$(CAT6DIR)/xpilot.1
	if [ -d /opt/lib/xpilot/maps ]; then true; \
	else mkdir -p /opt/lib/xpilot/maps; fi
	cp maps/*.map /opt/lib/xpilot/maps

clean:
	rm -f \#* *~ core *.o
	for dir in  src src/lib; do \
 	  (cd $$dir; make clean); \
	done
	rm -f src/xpilot src/xpilots


