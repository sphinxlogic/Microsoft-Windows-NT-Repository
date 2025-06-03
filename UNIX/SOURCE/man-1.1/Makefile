# Makefile for man and manpath
#
# Copyright (c) 1991, John W. Eaton.
#
# You may distribute under the terms of the GNU General Public
# License as specified in the README file that comes with the man 1.0
# distribution.  
#
# John W. Eaton
# jwe@che.utexas.edu
# Department of Chemical Engineering
# The University of Texas at Austin
# Austin, Texas  78712

DISTDIR = man-1.0

TOP = /opt

BINDIR = $(TOP)/bin
LIBDIR = $(TOP)/lib/man
MANDIR = $(TOP)/man
MANEXT = 1
ALTEXT1 = 0
ALTEXT2 = n

MANBINNAME = man

# Compression options (Greg Lehey, 1 December 1993)
# Define the following if your system has them, and if man should even look at
# man pages with those extensions.
# Add -DPACK		if man should unpack packed man pages
# Add -DCOMPRESS	if man should uncompress compressed man pages
# Add -DGZIP		if man should unzip gzipped man pages
COMPRESS=-D"COMPRESS=\"/opt/bin/compress\"" -D"GZIP=\"/opt/bin/gzip\"" -D"ZCAT=\"/opt/bin/zcat\"" -D"GUNZIP=\"/opt/bin/zcat\"" -D"UNPACK=\"zcat\""

# The file that defines the mapping of bin directories to man
# directories.  This should match the definition of config_file[] in
# config.h.

MANPATH_CONFIG_FILE=$(LIBDIR)/manpath.config

# Add these to DEFS if...
#
# -DHAS_TROFF		if you have troff installed
# -DSUPPORT_CAT_DIRS	if you have directories of preformatted man pages
# -DCHARSPRINTF		if your sprintf returns char *
# -DSTD_HEADERS		if you have standard headers installed
# -DALT_SYSTEMS		if you have system specific subdirectories in
#			your manpath (i.e. /usr/man/sunos/...)
# -DSTRDUP_MISSING	if your system lacks strdup()
#
# Edit and add any of the defs below if you want to override the
# defaults in config.h.
#
# -DMAXPATHLEN=512	maximum number of characters in an absolute filename
# -DBUFSIZ=1024		input buffers and some other things have this length
# -DMAXDIRS=64		something more than the maximum number of
#			directories in the config file 
# Greg Lehey, LEMIS, 30 November 1993: 
# Add -DHAS_GROFF if you have installed groff in $(TOP)/bin, etc.
# Add -DANYEXTENT if your man pages do not always have the section name after the first .
# Add -DSAVE_FORMATTED if man pages from man should be formatted into cat on demand,
#		        so that they can be accessed more quickly the next time.
# Add -DPIPE_PAGER=	for a pager command which will contain the name of the input file.
#			This is used for displaying the name of the input file in the
#			pager prompt when formatting directly from a man* directory.
DEFS = -DSYSV -DHAS_TROFF -DSUPPORT_CAT_DIRS -DSTD_HEADERS -DHAS_GROFF -DANYEXT -DSAVE_FORMATTED -D"PIPE_PAGER=\"/opt/bin/less -sCe -P%s\"" $(COMPRESS)

CC = gcc
CFLAGS = -Wall -g -O3 $(DEFS)

all: man manpath apropos whatis makewhatis manpages

manpath: manpath.c config.h manpath.h gripes.o util.o strdup.o
	$(CC) $(CFLAGS) -DMAIN -o manpath manpath.c gripes.o util.o strdup.o

man: man.c config.h gripes.h manpath.c manpath.h gripes.o glob.o \
	util.o strdup.o
	$(CC) $(CFLAGS) -o man man.c manpath.c gripes.o glob.o util.o strdup.o

gripes.o: gripes.h
util.o: gripes.h

apropos: apropos.sh
	sed -e 's,LIBDIR,$(LIBDIR),' -e 's,BINDIR,$(BINDIR),' \
		apropos.sh > apropos

whatis: whatis.sh
	sed -e 's,LIBDIR,$(LIBDIR),' -e 's,BINDIR,$(BINDIR),' \
		whatis.sh > whatis

makewhatis: makewhatis.sh
	cp makewhatis.sh makewhatis

install: all install.man apropos whatis makewhatis
	-mkdir -p $(BINDIR) $(LIBDIR)
	install -c  -m 755 man $(BINDIR)/$(MANBINNAME)
	install -c  -m 755 manpath $(BINDIR)/manpath
	install -c  -m 755 apropos $(BINDIR)/apropos
	install -c  -m 755 whatis $(BINDIR)/whatis
	install -c  -m 754 makewhatis $(LIBDIR)/makewhatis
	install -c  -m 644 manpath.config $(LIBDIR)/manpath.config

manpages: man.$(MANEXT) manpath.$(MANEXT) apropos.$(MANEXT) whatis.$(MANEXT) 

man.$(MANEXT): man.man
	sed -e 's,MANPATH_CONFIG_FILE,$(MANPATH_CONFIG_FILE),' \
	man.man > man.$(MANEXT)

manpath.$(MANEXT): manpath.man
	sed -e 's,MANPATH_CONFIG_FILE,$(MANPATH_CONFIG_FILE),' \
	manpath.man > manpath.$(MANEXT)

apropos.$(MANEXT): apropos.man
	sed -e 's,MANPATH_CONFIG_FILE,$(MANPATH_CONFIG_FILE),' \
	apropos.man > apropos.$(MANEXT)

whatis.$(MANEXT): whatis.man
	sed -e 's,MANPATH_CONFIG_FILE,$(MANPATH_CONFIG_FILE),' \
	whatis.man > whatis.$(MANEXT)

install.man:
	-mkdir -p $(MANDIR)/man$(MANEXT)
	install -c  -m 644 man.$(MANEXT) \
		$(MANDIR)/man$(MANEXT)/man.$(MANEXT)
	install -c  -m 644 manpath.$(MANEXT) \
		$(MANDIR)/man$(MANEXT)/manpath.$(MANEXT)
	install -c  -m 644 apropos.$(MANEXT) \
		$(MANDIR)/man$(MANEXT)/apropos.$(MANEXT)
	install -c  -m 644 whatis.$(MANEXT) \
		$(MANDIR)/man$(MANEXT)/whatis.$(MANEXT)
	
clean:
	rm -f *.o *~ core

spotless: clean
	rm -f manpath man apropos whatis makewhatis
	rm -f man.$(MANEXT) manpath.$(MANEXT)
	rm -f apropos.$(MANEXT) whatis.$(MANEXT) 

dist:
	[ -d $(DISTDIR) ] || mkdir $(DISTDIR)
	cp glob.c gripes.c manpath.c strdup.c util.c $(DISTDIR)
	cp gripes.h manpath.h ndir.h version.h $(DISTDIR)
	cp apropos.man man.man manpath.man whatis.man $(DISTDIR)
	cp apropos.sh whatis.sh makewhatis.sh $(DISTDIR)
	cp README COPYING TODO $(DISTDIR)
	cp config.h.dist $(DISTDIR)/config.h
	cp manpath.config.dist $(DISTDIR)/manpath.config
	cp Makefile.dist $(DISTDIR)/Makefile
	(cd $(DISTDIR) ; gawk -f ../fixdist.awk ../man.c > man.c)

tar.dist: dist
	tar czvf $(DISTDIR).tar.Z $(DISTDIR)
