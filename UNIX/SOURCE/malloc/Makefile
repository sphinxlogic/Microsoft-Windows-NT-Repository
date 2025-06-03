BINDIR=/opt/bin
LIBDIR=/opt/lib
MANDIR=/opt/man
CAT1DIR=cat1
DEFS=-D__getpagesize=getpagesize -Dmemmove=memcpy
CFLAGS=-O -I. $(DEFS)
OBJS=free.o malloc.o mcheck.o mstats.o realloc.o unix.o valloc.o
ONESRC=ansidecl.h limits.h stddef.h stdlib.h string.h malloc.h \
 free.c malloc.c realloc.c unix.c valloc.c

all: $(OBJS)

gmalloc.c: $(ONESRC) gmalloc.sed gmalloc.skel
	sed -f gmalloc.sed gmalloc.skel > gmalloc.c

clean:
	rm -f $(OBJS) gmalloc.[co]

install:
	ar r $(LIBDIR)/libmalloc.a $(OBJS)
	chown bin $(LIBDIR)/libmalloc.a
	chgrp bin $(LIBDIR)/libmalloc.a
	chmod 644 $(LIBDIR)/libmalloc.a
	