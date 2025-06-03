# Makefile for Unix Astrolog version 2.25 (cruiser1@milton.u.washington.edu)
# First created 11/21/1991
#
# This Makefile is included only for convenience. One could easily compile
# Astrolog on a Unix system by hand with the command:
# % cc -c *.c; cc -o astrolog *.o -lm -lX11
#
# Generally, all that needs to be done to compile once astrolog.h has been
# edited, is compile each source file, and link them together with the math
# library, and if applicable, the main X library.
#
CC=gcc
CFLAGS=-g -O3
LFLAGS=-g
NROFF=nroff
BINDIR=/opt/bin
LIBDIR=/opt/lib
MANDIR=/opt/man
CAT6DIR=cat6
INFODIR=/opt/info

NAME = astrolog
OBJ = data.o formulas.o charts.o options.o xcharts.o graphics.o driver.o
# If you don't have X windows, delete the "-lX11" part from the line below:
LIBS = -lm -lX11 -lnsl
CC = gcc
CFLAGS = -O3 -g

all:	astrolog

astrolog:: $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(LIBS)
	strip $(NAME)
#

clean:
	rm -f \#* *~ core astrolog *.o

install:	all
	cp astrolog $(BINDIR)
	cat Helpfile > $(MANDIR)/$(CAT6DIR)/astrolog.1
