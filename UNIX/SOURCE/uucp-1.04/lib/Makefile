# Generated automatically from Makefile.in by configure.
# This is the Makefile for the lib subdirectory of Taylor UUCP
#
# The file Makefile.in should be processed by configure to generate
# Makefile.  If you want to generate Makefile by hand, you must find
# all variables surrounded by @ and replace them with the correct
# value (e.g. gcc must be replaced by something like cc or gcc).

SHELL=/bin/sh

# These are overridden by the call from the top level Makefile
CC = gcc
CFLAGS = -g -O3
RANLIB = ranlib

srcdir = .
VPATH = .

MORECFLAGS = -I$(srcdir)/.. -I..

OBJS = buffer.o crc.o debug.o escape.o getopt.o getop1.o parse.o spool.o \
	status.o xfree.o xmall.o xreall.o \
	 getlin.o strcas.o strncs.o

all: libuucp.a

clean:
	rm -f $(OBJS) libuucp.a

distclean: clean
	rm -f Makefile

mostlyclean: clean
realclean: distclean

libuucp.a: $(OBJS)
	rm -f libuucp.a
	ar qc libuucp.a $(OBJS)
	-$(RANLIB) libuucp.a

.c.o:
	$(CC) -c $(CFLAGS) $(MORECFLAGS) $<

dist:
	mkdir ../uucp-$(VERSION)/lib
	ln `cat MANIFEST` ../uucp-$(VERSION)/lib

Makefile: Makefile.in
	(cd ..; sh config.status)

# Header file dependencies.

$(OBJS): $(srcdir)/../uucp.h ../conf.h $(srcdir)/../policy.h

buffer.o: $(srcdir)/../uudefs.h
crc.o: $(srcdir)/../prot.h
debug.o: $(srcdir)/../uudefs.h
escape.o: $(srcdir)/../uudefs.h
getopt.o: $(srcdir)/../getopt.h $(srcdir)/../uudefs.h
getop1.o: $(srcdir)/../getopt.h
parse.o: $(srcdir)/../uudefs.h
spool.o: $(srcdir)/../uudefs.h
status.o: $(srcdir)/../uudefs.h
xfree.o: $(srcdir)/../uudefs.h
xmall.o: $(srcdir)/../uudefs.h
xreall.o: $(srcdir)/../uudefs.h
