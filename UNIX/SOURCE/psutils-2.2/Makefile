# Makefile for PS utilities

CFLAGS=-O3 -g
CC=gcc

# epsffit fits an epsf file to a given bounding box
# psbook rearranges pages into signatures
# psselect selects page ranges
# pstops performs general page rearrangement and merging

all: psbook psselect pstops epsffit

epsffit: epsffit.c
	$(CC) $(CFLAGS) -o epsffit epsffit.c

psbook: psbook.o psutil.o
	$(CC) -o psbook psutil.o psbook.o

psselect: psselect.o psutil.o
	$(CC) -o psselect psutil.o psselect.o

pstops: pstops.o psutil.o
	$(CC) -o pstops psutil.o pstops.o

psbook.c: psutil.h

pstops.c: psutil.h

psutil.c: psutil.h

psselect.c: psutil.h

clean:
	rm -f *.o psbook psselect pstops epsffit

install:
	cp  psbook psselect pstops psnup epsffit /opt/bin
	for i in  psbook psselect pstops psnup; do \
	  nroff -man $$i.1 >/opt/man/cat1/$$i.1; \
	done