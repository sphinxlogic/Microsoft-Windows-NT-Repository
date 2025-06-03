# Top level Makefile for MandelSpawn
# LEMIS, 20 April 1993
#
VERSION=0.06
MANDIR=/opt/man# Put the man pages in this directory
MANDIR1=$(MANDIR)/cat1
MANDIR8=$(MANDIR)/cat8
NROFF=nroff

all:
	for i in bms mslaved xms ; do \
		(echo cd  $$i-$(VERSION); cd $$i-$(VERSION); make all) \
	done

doc:
	for i in bms mslaved ; do \
		(echo cd  $$i-$(VERSION); cd $$i-$(VERSION); make doc) \
	done 
	nroff -man xms-$(VERSION)/xms.man > xms-$(VERSION)/xms.1

clean:
	for i in bms mslaved xms ; do \
		(echo cd  $$i-$(VERSION); cd $$i-$(VERSION); make clean) \
	done

install:
	if [ -d $(MANDIR1) ] ; then true ; else mkdir $(MANDIR1) ; fi
	if [ -d $(MANDIR8) ] ; then true ; else mkdir $(MANDIR8) ; fi
	for i in bms mslaved xms ; do \
		(echo cd  $$i-$(VERSION); \
		cd $$i-$(VERSION); \
		make MANDIR1=$(MANDIR1) MANDIR8=$(MANDIR8) install) \
	done
	cp xms-$(VERSION)/xms.1 $(MANDIR1)

install-sysv:
	if [ -d $(MANDIR1) ] ; then true ; else mkdir $(MANDIR1) ; fi
	if [ -d $(MANDIR8) ] ; then true ; else mkdir $(MANDIR8) ; fi
	for i in bms mslaved  ; do \
		(echo cd  $$i-$(VERSION); \
		cd $$i-$(VERSION); \
		make NROFF=$(NROFF) MANDIR1=$(MANDIR1) MANDIR8=$(MANDIR8) install-sysv) \
	done
	(cd xms-$(VERSION); make install)
	$(NROFF) -man xms-$(VERSION)/xms.1  > $(MANDIR1)/xms.1

