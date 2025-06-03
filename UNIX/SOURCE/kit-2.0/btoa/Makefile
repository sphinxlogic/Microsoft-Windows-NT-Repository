#
# Makefile for kit (sub-directory btoa)
#

# $Id: Makefile.SH,v 2.0.1.2 92/04/20 17:32:09 ram Exp $
#
# $Log:	Makefile.SH,v $
# Revision 2.0.1.2  92/04/20  17:32:09  ram
# patch15: now creates directories before installing
# 
# Revision 2.0.1.1  91/05/27  14:14:41  ram
# patch5: some dumb make failed on previous makefile
# 
# Revision 2.0  91/02/19  15:49:16  ram
# Baseline for first official release.
# 

SHELL = /bin/sh

CC = gcc
CFLAGS = -fpcc-struct-return -O3 -g
RM = rm
bin = /opt/bin

BIN = btoa atob hexencode hexdecode

all: $(BIN)

btoa: btoa.c
	$(CC) $(CFLAGS) -o $@ $@.c

atob: atob.c
	$(CC) $(CFLAGS) -o $@ $@.c

hexencode: hexencode.c
	$(CC) $(CFLAGS) -o $@ $@.c

hexdecode: hexdecode.c
	$(CC) $(CFLAGS) -o $@ $@.c

install install-manman install-catman: $(BIN)
	../install -d $(bin)
	-for file in $(BIN); do \
		../install -s -m 755 $$file $(bin); \
	done

clean:
	$(RM) -f *.o $(BIN)

realclean clobber: clean
	$(RM) -f core Makefile config.sh
