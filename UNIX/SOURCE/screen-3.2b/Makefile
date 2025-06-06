# Generated automatically from Makefile.in by configure.
#
# Makefile template for screen
#
# See config.h.in for configuration options and an explanation of why
# it is best to install screen setuid to root.
#

#### Start of system configuration section. ####

srcdir = .
VPATH = .

# Where to install screen.

prefix = /opt
exec_prefix = $(prefix)

bindir = $(exec_prefix)/bin
datadir = $(prefix)/lib
mandir = $(prefix)/man/cat1

# The global init file for screen.
ETCSCREENRC = $(datadir)/screenrc

CC = gcc -O3
CFLAGS = -g -DSVR4
DEFS = -I.
LDFLAGS = -g
LIBS =  -ltermcap -lelf -lcrypt -lsocket -lnsl

INSTALL = /usr/ucb/install -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644

#### End of system configuration section. ####

SHELL = /bin/sh

CFILES=	screen.c ansi.c help.c fileio.c mark.c window.c socket.c \
putenv.c getloadavg.c
OFILES=	screen.o ansi.o help.o fileio.o mark.o window.o socket.o  getloadavg.o
HFILES= ansi.h extern.h patchlevel.h screen.h config.h.in
MISCFILES = COPYING ChangeLog INSTALL Makefile.in NEWS README etcscreenrc \
	configure.in configure screen.1
DISTFILES = $(CFILES) $(HFILES) $(MISCFILES)

all: screen

install: all
	$(INSTALL_PROGRAM) screen $(bindir)/screen
	-chown root $(bindir)/screen && chmod 4755 $(bindir)/screen
	$(INSTALL_DATA) $(srcdir)/etcscreenrc $(ETCSCREENRC)
	-sed 's,/local/etc/screenrc,$(ETCSCREENRC),g' $(srcdir)/screen.1 > $(mandir)/screen.1
	-chmod 644 $(mandir)/screen.1

install-sysv: all
	$(INSTALL_PROGRAM) screen $(bindir)/screen
	-chown root $(bindir)/screen && chmod 4755 $(bindir)/screen
	$(INSTALL_DATA) $(srcdir)/etcscreenrc $(ETCSCREENRC)
	-sed 's,/local/etc/screenrc,$(ETCSCREENRC),g' $(srcdir)/screen.1 |nroff -man | > $(mandir)/screen.1.Z
	-chmod 644 $(mandir)/screen.1.Z

uninstall:
	rm -f $(bindir)/screen $(ETCSCREENRC) $(mandir)/screen.1

screen: $(OFILES)
	$(CC) $(LDFLAGS) -o $@ $(OFILES) $(LIBS)

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(DEFS) $<

screen.o: screen.c screen.h config.h patchlevel.h extern.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(DEFS) -DETCSCREENRC=\"$(ETCSCREENRC)\" $(srcdir)/screen.c

socket.o: socket.c config.h screen.h extern.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(DEFS) -DETCSCREENRC=\"$(ETCSCREENRC)\" $(srcdir)/socket.c

help.o: help.c screen.h config.h ansi.h extern.h patchlevel.h
fileio.o: fileio.c screen.h config.h extern.h
ansi.o: ansi.c screen.h config.h ansi.h extern.h
mark.o: mark.c screen.h config.h ansi.h extern.h
window.o: window.c config.h screen.h extern.h
putenv.o: putenv.c config.h

clean:
	rm -f $(OFILES) screen

mostlyclean: clean

distclean: clean
	rm -f Makefile config.h config.status

realclean: distclean
	rm -f TAGS

TAGS: $(CFILES)
	etags $(CFILES)

lint:
	lint -I. $(CFILES)

saber:
	#load $(CFLAGS) screen.c ansi.c $(LIBS)

dist: $(DISTFILES)
	echo screen-3.2b > .fname
	rm -rf `cat .fname`
	mkdir `cat .fname`
	-ln $(DISTFILES) `cat .fname`
	for f in $(DISTFILES); do \
	test -f `cat .fname`/$$f || cp -p $$f `cat .fname`; done
	cp -pR terminfo `cat .fname`
	tar chZf `cat .fname`.tar.Z `cat .fname`
	rm -rf `cat .fname` .fname
