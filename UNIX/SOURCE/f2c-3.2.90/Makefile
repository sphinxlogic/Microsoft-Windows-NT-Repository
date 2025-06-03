# Top-level Makefile for f2c added by LEMIS.

INSTDIR=/opt
BINDIR=$(INSTDIR)/bin
MANDIR=$(INSTDIR)/man/cat1

all:
	for i in src libF77 libI77 ; do \
	  echo Making in $$i ... ; \
	  (cd $$i; make) ; \
        done

clean:
	for i in src libF77 libI77 ; do \
	  echo Making in $$i ... ; \
	  (cd $$i; make clean) ; \
        done

install:
	nroff -man src/f2c.1t >$(MANDIR)/f2c.1
	cp src/xsum src/f2c $(BINDIR)
	cp libF77/libF77.a /opt/lib
	cp libI77/libI77.a /opt/lib