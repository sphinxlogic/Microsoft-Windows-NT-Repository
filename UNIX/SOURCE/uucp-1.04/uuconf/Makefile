# Generated automatically from Makefile.in by configure.
# This is the Makefile for the Taylor UUCP uuconf library
#
# The file Makefile.in should be processed by configure to generate
# Makefile.  If you want to generate Makefile by hand, you must find
# all variables surrounded by @ and replace them with the correct
# value (e.g. gcc must be replaced by something like cc or gcc).

SHELL = /bin/sh

# These values are overriden by the top level Makefile
prefix = /usr/local
newconfigdir = $(prefix)/conf/uucp
oldconfigdir = /usr/lib/uucp

# These values are overridden by the top level Makefile
CC = gcc
CFLAGS = -g -O3
RANLIB = ranlib
LN_S = ln -s

# Source directory and, if necessary, VPATH
srcdir = .
VPATH = .

MORECFLAGS = -I$(srcdir) -I. -I$(srcdir)/.. -I.. -DNEWCONFIGLIB=\"$(newconfigdir)\" -DOLDCONFIGLIB=\"$(oldconfigdir)\"

OBJS = addblk.o addstr.o allblk.o alloc.o base.o bool.o callin.o \
	calout.o chatc.o cmdarg.o cmdfil.o cmdlin.o debfil.o deblev.o \
	diacod.o dial.o diasub.o dnams.o errno.o errstr.o filnam.o \
	freblk.o fredia.o free.o freprt.o fresys.o grdcmp.o hdial.o \
	hdnams.o hinit.o hlocnm.o hport.o hrmunk.o hsinfo.o hsnams.o \
	hsys.o hunk.o iniglb.o init.o int.o lckdir.o lineno.o llocnm.o \
	local.o locnm.o logfil.o maxuxq.o mrgblk.o paramc.o port.o \
	prtsub.o pubdir.o rdlocs.o rdperm.o reliab.o remunk.o sinfo.o \
	snams.o split.o spool.o stafil.o syssub.o tcalou.o tdial.o \
	tdialc.o tdnams.o tgcmp.o thread.o time.o tinit.o tlocnm.o \
	tport.o tportc.o tsinfo.o tsnams.o tsys.o tval.o ugtlin.o \
	unk.o val.o vinit.o vport.o vsinfo.o vsnams.o vsys.o

all: libuuconf.a

libuuconf.a: $(OBJS)
	rm -f libuuconf.a
	ar qc libuuconf.a $(OBJS)
	-$(RANLIB) libuuconf.a

.c.o:
	$(CC) -c $(CFLAGS) $(MORECFLAGS) $<

syshdr.h: syshdr.unx
	rm -f syshdr.h
	$(LN_S) $(srcdir)/syshdr.unx syshdr.h

clean:
	rm -f $(OBJS) libuuconf.a syshdr.h

distclean: clean
	rm -f Makefile

mostlyclean: clean
realclean: distclean

dist:
	mkdir ../uucp-$(VERSION)/uuconf
	ln `cat MANIFEST` ../uucp-$(VERSION)/uuconf

Makefile: Makefile.in
	(cd ..; sh config.status)

# Header file dependencies

$(OBJS): uucnfi.h syshdr.h $(srcdir)/../uucp.h ../conf.h $(srcdir)/../policy.h $(srcdir)/../uuconf.h

addblk.o: alloc.h
allblk.o: alloc.h
alloc.o: alloc.h
freblk.o: alloc.h
free.o: alloc.h
mrgblk.o: alloc.h
