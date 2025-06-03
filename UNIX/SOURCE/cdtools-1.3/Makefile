# Makefile for tools
CC=gcc
CFLAGS=-g

SCRIPTS= expanddir mkliblist finddef mklinktree selectdirs \
findf mkmkpk breaklink getl mkpackage stripdir buildliblist \
mkpk chkequiv link-package unlink-package linkdoc xmkmf zipsrc \
cmpdir make-package reportloss dolink makelinks rmsymlinks \
printman unpackclean wh

all:	chkequiv liblist

chkequiv: chkequiv.o
	$(CC) $(CFLAGS) -o $@ $<

liblist: libs
	buildliblist

libs:
	@echo Please build the file libs manually. It should contain a list of the names
	@echo of the libraries which you want to be searched for unresolved references.
	exit 1

clean:
	rm -f *~ *.o chkequiv fileinfo newinfo liblist 

install:
	for i in $(SCRIPTS); do \
	  cp -p $$i /opt/bin; \
	done
	if [ ! -d /opt/ETC ]; then \
	  mkdir -p /opt/ETC ; \
	fi
	for i in libs liblist; do \
	  cp -p $$i /opt/ETC; \
	done
