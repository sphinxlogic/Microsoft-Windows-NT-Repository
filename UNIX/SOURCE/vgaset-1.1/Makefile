# Makefile for vgaset. 
# Greg Lehey, LEMIS, 15 May 1993

# Set HAS_USL_VTS if you are using SRV4 or (reportedly) Linux
HAS_USL_VTS=-DHAS_USL_VTS

# set COMMON to the pathname of the x386/common/compile.h header file
# in this Makefile, we supply a copy. Note that this should only be used
# if you don't have the X11 source.
COMMON=.

CC=gcc
CFLAGS=-g -O3 $(HAS_USL_VTS)
LFLAGS=-g
NROFF=nroff
BINDIR=/opt/bin
LIBDIR=/opt/lib
MANDIR=/opt/man
CAT1DIR=cat1
INFODIR=/opt/info


all:	vgaset

vgaset: vgaset.c svga.h
	$(CC) $(CFLAGS) -I$(COMMON) vgaset.c -o vgaset

man:

doc:
	echo Only documentation is man page

install:  all
	cp vgaset $(BINDIR)
	cat vgaset.doc > $(MANDIR)/$(CAT1DIR)/vgaset.1

clean:
	rm -f \#* *~ core vgaset *.o

