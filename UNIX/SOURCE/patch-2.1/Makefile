# Generated automatically from Makefile.in by configure.
# Makefile for GNU patch.

#### Start of system configuration section. ####

srcdir = .
VPATH = .

CC = gcc

INSTALL = /opt/bin/install -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644

LIBS = 

CFLAGS = -g -O3
LDFLAGS = -g

prefix = /opt
exec_prefix = ${prefix}

bindir = $(exec_prefix)/bin

# Where to put the manual pages.
mandir = $(prefix)/man/man1
# Extension (not including `.') for the manual page filenames.
manext = 1

#### End of system configuration section. ####

SHELL = /bin/sh

SRCS = backupfile.c getopt.c getopt1.c inp.c patch.c pch.c util.c \
	version.c rename.c alloca.c
OBJS = backupfile.o getopt.o getopt1.o inp.o patch.o pch.o util.o \
	version.o  
HDRS = EXTERN.h INTERN.h backupfile.h common.h getopt.h \
	inp.h patchlevel.h pch.h util.h version.h
MISC = COPYING ChangeLog INSTALL Makefile.in README NEWS \
	configure configure.in config.h.in patch.man
DISTFILES = $(MISC) $(SRCS) $(HDRS)

all: patch

.c.o:
	$(CC) -c -DHAVE_CONFIG_H -I. $(CPPFLAGS) $(CFLAGS) $<

patch: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

install: all
	$(INSTALL_PROGRAM) patch $(bindir)/patch
	-$(INSTALL_DATA) $(srcdir)/patch.man $(mandir)/patch.$(manext)

uninstall:
	rm -f $(bindir)/patch $(mandir)/patch.$(manext)

TAGS: $(SRCS)
	etags $(SRCS)

clean:
	rm -f patch *.o core

mostlyclean: clean

distclean: clean
	rm -f Makefile config.status config.h

realclean: distclean
	rm -f TAGS

dist: $(DISTFILES)
	echo patch-`sed -e '/PATCH_VERSION/!d' -e 's/[^0-9]*\([0-9a-z.]*\).*/\1/' -e q patchlevel.h` > .fname
	rm -rf `cat .fname`
	mkdir `cat .fname`
	ln $(DISTFILES) `cat .fname`
	tar -chf - `cat .fname` | gzip >`cat .fname`.tar.gz
	rm -rf `cat .fname` .fname

backupfile.o: config.h backupfile.h
getopt.o getopt1.o: config.h getopt.h
inp.o: config.h common.h inp.h util.h EXTERN.h INTERN.h pch.h
patch.o: config.h common.h inp.h pch.h util.h version.h backupfile.h \
	INTERN.h EXTERN.h getopt.h
pch.o: config.h common.h pch.h util.h EXTERN.h INTERN.h
util.o: config.h common.h util.h backupfile.h EXTERN.h INTERN.h
version.o: config.h common.h version.h patchlevel.h util.h \
	EXTERN.h INTERN.h
