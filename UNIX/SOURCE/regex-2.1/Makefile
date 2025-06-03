# LEMIS surrogate Makefile. This package did not contain a top-level Makefile,
# so this was added in the interests of uniformity.
CC=gcc
CFLAGS=-g -O3 -DPOSIX_MISTAKE -I.
LFLAGS=-g
NROFF=nroff
BINDIR=/opt/bin
LIBDIR=/opt/lib
INCLDIR=/opt/include
MANDIR=/opt/man
CAT3DIR=cat3
CAT7DIR=cat7
INFODIR=/opt/info
SRCS=regcomp.c regerror.c regexec.c regfree.c
OBJS=regcomp.o regerror.o regexec.o regfree.o
PROGRAM=libregex.a

all:	$(PROGRAM)
	
$(PROGRAM):	$(OBJS)
	ar -r $(PROGRAM) $(OBJS)

man:

doc:

install:  all
	if [ ! -d $(INCLDIR)/sys ]; then \
	  mkdir -p $(INCLDIR)/sys; \
	fi
	if [ ! -d $(LIBDIR) ]; then \
	  mkdir -p $(LIBDIR); \
	fi
	if [ ! -d $(MANDIR)/$(CAT3DIR) ]; then \
	  mkdir -p $(MANDIR)/$(CAT3DIR); \
	fi
	if [ ! -d $(MANDIR)/$(CAT7DIR) ]; then \
	  mkdir -p $(MANDIR)/$(CAT7DIR); \
	fi
	cp $(PROGRAM) $(LIBDIR)
	cp regex.h $(INCLDIR)
	cp cdefs.h $(INCLDIR)/sys
	$(NROFF) -man regex.3  > $(MANDIR)/$(CAT3DIR)/regex.3
	$(NROFF) -man re_format.7  > $(MANDIR)/$(CAT7DIR)/re_format.7

clean:
	rm -f \#* *~ core $(PROGRAM) *.o
