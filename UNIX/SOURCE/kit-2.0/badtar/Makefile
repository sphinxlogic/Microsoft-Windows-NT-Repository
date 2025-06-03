#
# Makefile for kit (sub-directory badtar)
#

# $Id: Makefile.SH,v 2.0.1.2 92/04/20 17:31:53 ram Exp $
#
# $Log:	Makefile.SH,v $
# Revision 2.0.1.2  92/04/20  17:31:53  ram
# patch15: now creates directories before installing
# 
# Revision 2.0.1.1  92/01/11  19:11:57  ram
# patch11: created
# 

SHELL = /bin/sh

CC = gcc
CFLAGS = -fpcc-struct-return -O3 -g
RM = rm
bin = /opt/bin
man = /opt/man/cat1
manpage = badtar.0

all: badtar $(manpage)

badtar: badtar.o
	$(CC) $(CFLAGS) -o badtar badtar.o

$(manpage): badtar.SH
	/bin/sh badtar.SH

install install-manman: badtar $(manpage)
	../install -d $(bin)
	../install -s -m 755 badtar $(bin)
	-if test "$(man)"; then \
		../install -d $(man); \
		../install -m 444 $(manpage) $(man); \
	fi

install-catman: badtar $(manpage)
	../install -d $(bin)
	../install -s -m 755 badtar $(bin)
	-if test "$(man)"; then \
		../install -d $(man); \
		nroff -man $(manpage)  > $(man)/`basename $(manpage) 0`1; \
	fi

clean:
	rm -f *.o badtar $(manpage)

realclean clobber: clean
	$(RM) -f core Makefile config.sh

