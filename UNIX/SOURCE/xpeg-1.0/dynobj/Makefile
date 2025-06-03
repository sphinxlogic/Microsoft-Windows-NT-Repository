# This file is part of libdyn.a, the C Dynamic Object library.  It
# contains the Makefile.
#
# There are no restrictions on this code; however, if you make any
# changes, I request that you document them so that I do not get
# credit or blame for your modifications.
#
# Written by Barr3y Jaspan, Student Information Processing Board (SIPB)
# and MIT-Project Athena, 1989.

DESTDIR = /afs/.athena.mit.edu/contrib/sipb/$(MACHINE)lib
CC	= cc
CFLAGS	= -g
DEST	= libdyn.a

SRCS	= dyn_create.c dyn_put.c dyn_debug.c dyn_delete.c dyn_size.c \
	dyn_append.c dyn_realloc.c dyn_paranoid.c dyn_insert.c
OBJS	= dyn_create.o dyn_put.o dyn_debug.o dyn_delete.o dyn_size.o \
	dyn_append.o dyn_realloc.o dyn_paranoid.o dyn_insert.o
HDRS	= dyn.h dynP.h

$(DEST): $(OBJS)
	rm -f $(DEST)
	ar rc $(DEST) $(OBJS)
	ranlib $(DEST)

install:
	make $(INSTFLAGS) DEST="libdyn.a" CFLAGS="-g" clean libdyn.a
	cp libdyn.a $(DESTDIR)
	ranlib $(DESTDIR)/libdyn.a
	make $(INSTFLAGS) DEST="libdyn_p.a" CFLAGS="-g -pg" clean libdyn_p.a
	cp libdyn_p.a $(DESTDIR)	
	ranlib $(DESTDIR)/libdyn_p.a
	
all: $(DEST) test

test: $(DEST) test.o
	$(CC) -o test test.o $(DEST)

saber:
	#load $(SRCS)

clean:
	rm -f $(OBJS) $(DEST) test *~

depend:
	makedepend -- $(CLFAGS) -- $(HDRS) $(SRCS)

# DO NOT DELETE THIS LINE -- make depend depends on it.
