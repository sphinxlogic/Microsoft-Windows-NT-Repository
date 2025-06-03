# LEMIS surrogate Makefile. This package did not contain a top-level Makefile,
# so this was added in the interests of uniformity.
CC=gcc
CFLAGS=-g -O3 -DPOSIX_MISTAKE
LFLAGS=-g
NROFF=nroff
BINDIR=/opt/bin
LIBDIR=/opt/lib
MANDIR=/opt/man
CAT1DIR=cat1
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
	cp $(PROGRAM) $(LIBnDIR)
	$(NROFF) -man regex.o  > $(MANDIR)/$(CAT3DIR)/regex.1; \
        done
	for i in $(MAN3); do \
	  $(NROFF) -man $$i  > $(MANDIR)/$(CAT3DIR)/$$i.1; \
        done

clean:
	rm -f \#* *~ core $(PROGRAM) *.o

#	@(#)Makefile.inc	5.1 (Berkeley) 8/6/92

# regex sources
.PATH: ${.CURDIR}/regex

CFLAGS+=-DPOSIX_MISTAKE

SRCS+=	

MAN3+=	regex.0
MAN7+=	re_format.0

MLINKS+=regex.3 regcomp.3 regex.3 regexec.3 regex.3 regerror.3
MLINKS+=regexec.3 regfree.3
