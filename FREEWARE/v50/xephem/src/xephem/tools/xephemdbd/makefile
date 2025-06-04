# Makefile for xephemdbd
# Only tested on Linux

CC = gcc

CLDFLAGS =
CFLAGS = $(CLDFLAGS) -Wall -O2 -ffast-math \
	-I../../../../libastro
LDFLAGS = $(CLDFLAGS) \
	-L../../../../libastro
LIBS = -lastro -lm

XED = ../..

OBJS =	xephemdbd.o 		\
	db.o			\
	fsfetch.o		\
	request.o		\
	stubs.o			\
	$(XED)/gsc.o		\
	$(XED)/ppm.o		\
	$(XED)/constel.o

xephemdbd: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

clobber:
	touch x.o
	rm -f *.o xephemdbd


# For RCS Only -- Do Not Edit
# @(#) $RCSfile: Makefile,v $ $Date: 1998/03/12 03:16:45 $ $Revision: 1.1 $ $Name:  $
