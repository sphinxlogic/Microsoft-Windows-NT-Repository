# Generated automatically from Makefile.in by configure.
# utils subdir Makefile for TSLIP.	-*- Indented-Text -*-
# @(#)Makefile.in	1.2 (13 Oct 1993)

#...........................................................................
# Copyright 1993, Jim Avera.  All Rights Reserved.
#  
# This program contains new material owned by the above copyright holder,
# and may also contain portions derived from existing works used according
# to permission granted by the owners of those works.
# 
# You may use, modify, or distribute this work (or the portions owned by the 
# above copyright holder) ONLY as described in the file "COPYRIGHT" which 
# accompanies this file or program.
# 
# BECAUSE THIS SOFTWARE IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY,
# EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, TO
# THE EXTENT PERMITTED BY LAW.  THIS SOFTWARE IS PROVIDED "AS IS".
# IN NO EVENT UNLESS REQUIRED BY LAW WILL COPYRIGHT HOLDER BE LIABLE FOR 
# DAMAGES OF ANY NATURE ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.
#..........................................................................

#### Start of system configuration section. ####

srcdir = .
VPATH = .

taylorsrc = /cdcopy/SOURCE/uucp-1.04
taylorobj = /cdcopy/SOURCE/uucp-1.04

CC = gcc

INSTALL = /opt/bin/install -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644

LIBS =  -lsocket -lnsl

CFLAGS = -g -O3
LDFLAGS = -g

prefix = /opt
exec_prefix = ${prefix}

bindir = $(exec_prefix)/bin
sbindir = $(prefix)/lib/slip

#### End of system configuration section. ####

SHELL = /bin/sh

DRIVER_NAME = tslip
SLIP_DEV_NAME_STR = "/dev/$(DRIVER_NAME)"
SLIP_HEADER = <sys/$(DRIVER_NAME).h>


MORECFLAGS = -I$(srcdir)/../driver  -I../driver \
	     -I$(taylorsrc) \
	     -DBINDIR='"$(bindir)"' -DSBINDIR=\"$(sbindir)\" \
	     -DOWNER=\"uucp\" \
	     -DDRIVER_NAME=$(DRIVER_NAME) \
	     '-DSLIP_DEV_NAME_STR=$(SLIP_DEV_NAME_STR)' \
	     '-DSLIP_HEADER=$(SLIP_HEADER)'

# If a .o file in $(taylorobj) needs to be made, just tell the user
.c.o:
	@set +e; case "$*" in \
	 $(taylorobj)/*) \
	   echo "You must make (compile) Taylor UUCP first ($(taylorobj))." >&2;\
	   exit 1;; \
	 *) exit 0;; esac
	$(CC) -c $(MORECFLAGS) $(CPPFLAGS) $(CFLAGS) $<

# Warning: make clean removes $(PROGRAMS)
PROGRAMS = slattach slipd slipcmd slipdlogsum

ATOBJS = slattach.o

DOBJS = slipd.o slipd_tuucp.o

DCUOBJS = $(taylorobj)/prot.o $(taylorobj)/log.o $(taylorobj)/chat.o $(taylorobj)/conn.o \
	  $(taylorobj)/copy.o $(taylorobj)/tcp.o $(taylorobj)/tli.o

UULIBS = $(taylorobj)/unix/libunix.a $(taylorobj)/uuconf/libuuconf.a \
	 $(taylorobj)/lib/libuucp.a

all:: $(PROGRAMS)

clean:
	rm -f $(ATOBJS) $(DOBJS) $(PROGRAMS) *.o *.a out core a.out mkout

slattach: $(ATOBJS)
	$(CC) $(LDFLAGS) -o $@ $(ATOBJS) $(LIBS)

slipd: $(DOBJS) $(DCUOBJS) $(UULIBS)
	$(CC) $(LDFLAGS) -o $@ $(DOBJS) $(DCUOBJS) $(UULIBS) $(LIBS)

# Substitute variables in perl scripts and create executable version
.SUFFIXES: .pl
.pl:
	rm -f $*
	sed -e 's#@SBINDIR@#$(sbindir)#g' \
	    -e 's#@SLIPCMD_SPOOLDIR@#/var/spool/slipcmdq#g' \
	    -e 's#@SLIP_DEV_NAME_STR@#$(SLIP_DEV_NAME_STR)#g' \
	    $*.pl > $@
	chmod -w,+x $@

lint:
	lint -x -m $(MORECFLAGS) $(DOBJS:.o=.c)
	lint -x -m $(MORECFLAGS) $(ATOBJS:.o=.c)

$(sbindir):
	$(SHELL) ${srcdir}/../mkinstalldirs $(sbindir)

install:: $(PROGRAMS) $(sbindir)
	for p in $(PROGRAMS) ; do $(INSTALL_PROGRAM) $$p $(sbindir)/$$p; done
	chown root $(sbindir)/slipcmd
	chmod u+s $(sbindir)/slipcmd

installman:

Makefile: Makefile.in
	(cd ..; $(SHELL) config.status)

distclean: clean
	rm -f Makefile

mostlyclean:: clean

realclean:: mostlyclean distclean
	rm -f TAGS

uninstall: force
	cd $(sbindir); rm -f $(PROGRAMS)

force:

# Prevent GNU make v3 from overflowing arg limit on SysV.
.NOEXPORT:
