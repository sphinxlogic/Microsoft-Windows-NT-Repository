#
# Last changed: 4/14/92, v1.3.2
#
# Makefile for the minimal build for an archie Prospero client.

# Your C compiler:
CC=cc

# For most systems, these OPTIONS will suffice.  Exceptions:
#
#	* If you're on a Stardent, add  -43
#	* If you're running EP/IX, you may need to add  -systype bsd43
#	  but try it without it first.
OPTIONS= -O -I. -I/usr/netinclude -I/usr/ucbinclude

# For this, DEFINES is usually ok as-is.  Try it without any of these
# first; if some stuff fails or shows up undefined, then come back and
# add 'em.  Also please drop me a line if you had to add anything...ideally
# things will reach a point where this whole section will be unnecessary.
#
#     * if you want to include the debugging code (so you
#       can help with problem-solving if any crop up), add    -DDEBUG
#     * if you're running Interactive Unix, add		      -DISC
#     * if you're running System V, add			      -DSYSV
#     * if you're running a USG (System V.2) system, add      -DUSG
#     * if you're running UTS, add			      -DUTS
#     * if your system doesn't have the functions index(),
#       rindex(), bcopy(), or bzero(), add                    -DFUNCS
#     * if your system is missing the getenv(3) routine, add  -DGETENV
#     * if your system doesn't have the re_comp/regcmp or re_exec/regex
#       routines (no regex(3)/regcmp(3X) library), then add   -DNOREGEX
#     * if your system is lacking strspn(), add               -DSTRSPN
DEFINES= -DDEBUG

# The default Archie server; choose one of:
#    archie.ans.net		(USA [NY])
#    archie.rutgers.edu		(USA [NJ])
#    archie.sura.net		(USA [MD])
#    archie.unl.edu		(USA [NE])
#    archie.mcgill.ca		(Canada)
#    archie.funet.fi		(Finland/Mainland Europe)
#    archie.au			(Australia)
#    archie.doc.ic.ac.uk	(Great Britain/Ireland)
#
ARCHIE= archie.sura.net

#	Usually LDFLAGS is empty; if, after you build this, archie
#	 complains that it can't resolve ARCHIE.ANS.NET (or whatever
#	 you defined ARCHIE_HOST as), you need to add `-lresolv'.
#	* If you need the PW library (e.g. A/UX), add -lPW
#	* If you're using ISC, add -linet
#	* If you're using Wollongong TCP/IP on an AT&T box, use the
#	  arguments -lnet -lnsl_s .
#	* If you're using UTS, add -lsocket
#	* If you're using System V Release 4, add -L/usr/ucblib etc.
LDFLAGS=
#LDFLAGS= -lresolv
#LDFLAGS= -lnet -lnsl_s
#LDFLAGS= -lsocket
#LDFLAGS= -L/usr/ucblib -lucb -lsockdns -lnsl

# Change this if necessary.
RM=/bin/rm

# =========================
# Yer' done....make archie.
# =========================
#
CFLAGS=$(OPTIONS) $(DEFINES) -DARCHIE_HOST=\"$(ARCHIE)\"

VERSION=1.3.2
VMS_VERSION=`echo $(VERSION) | sed -e 's,\\.,_,g'`

OBJS=	aquery.o archie.o atalloc.o dirsend.o get_pauth.o get_vdir.o \
	perrmesg.o procquery.o ptalloc.o regex.o stcopy.o support.o \
	vlalloc.o vl_comp.o

all: archie

archie: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): archie.h pmachine.h pcompat.h Makefile

procquery.o: copyright.h

clean:
	$(RM) -f *.o archie archie.doc

gclean: clean
	$(RM) -f *~ archie.0* archie$(VMS_VERSION).com archie.tar* gmon.out archie.doc

unx-deadly:
	unifdef -UXARCHIE xprocquery.c > procquery.c

udp: udp.o
	$(CC) -o $@ udp.o

udptest: udp
	@echo "This should print the date, if UDP's enabled, or hang if not:"
	@./udp

FILES=	INSTALL Makefile Prospero README README.dos archie.c archie.h \
	archie.lnk archie.doc archie.man aquery.c atalloc.c dirsend.c \
	get_pauth.c get_vdir.c make.com makefile.cut makefile.dos makefile.os2 \
	msdos/cutcp.h msdos/hostform.h msdos/netevent.h \
	perrmesg.c patchlevel.h pauthent.h pcompat.h perrno.h pfs.h pmachine.h \
	pprot.h procquery.c ptalloc.c rdgram.h regex.c regex.h stcopy.c \
	support.c copyright.h udp.c vl_comp.c vlalloc.c vms.h vms_support.c \
	vms/fd.h vms/in.h vms/pseudos.h vms/signal.h vms/socket.h vms/time.h \
	vms/types.h vms/network.h vms/multi.opt vms/ucx.opt vms/woll.opt

BFILES= $(FILES) xprocquery.c

dist: tar shar dcl

shar: archie.doc
	makekit -narchie-$(VERSION). archie-$(VERSION) `echo $(FILES) | \
		tr ' ' '\012' | sed -e "s/^/archie-$(VERSION)\//g"` \
		vms msdos > Manifest

archie.doc: archie.man
	nroff -man archie.man | tr '\010' _ | sed -e s/__//g > archie.doc

dcl: archie.doc
	if [ ! -d archie$(VMS_VERSION)/ ]; then ln -s . archie$(VMS_VERSION); fi
	echo '$$ write sys$$output "Unpacking..."' > archie$(VMS_VERSION).com
	echo '$$ if f$$search("archie.dir") .eqs. "" then create/dir [.ARCHIE]' >> archie$(VMS_VERSION).com
	echo '$$ if f$$search("[.ARCHIE]vms.dir") .eqs. "" then create/dir [.ARCHIE.VMS]' >> archie$(VMS_VERSION).com
	echo '$$ if f$$search("[.ARCHIE]msdos.dir") .eqs. "" then create/dir [.ARCHIE.MSDOS]' >> archie$(VMS_VERSION).com
	echo '$$ set noverify' >> archie$(VMS_VERSION).com
	dclshar `echo "$(FILES)" | tr ' ' '\012' | \
		sed -e "s/^/archie\//g"` >> archie$(VMS_VERSION).com
	echo '$$ write sys$$output "Ok, now enter the ARCHIE directory, look at MAKE.COM, then type @MAKE ."' >> archie$(VMS_VERSION).com

tar: archie.doc
	if [ ! -d archie-$(VERSION)/ ]; then ln -s . archie-$(VERSION); fi
	tar cvf archie-$(VERSION).tar `echo $(FILES) |\
	 tr ' ' '\012' | sed -e "s/^/archie-$(VERSION)\//g"`
	compress -f archie-$(VERSION).tar
