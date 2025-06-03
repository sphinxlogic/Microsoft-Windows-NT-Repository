#  @(#)Makefile 1.3 91/05/06
#
#  Copyright (c) Steve Kinzler - April 1991.
#
#  Permission is given to distribute these sources, as long as the
#  copyright messages are not removed, and no monies are exchanged.
#
#  No responsibility is taken for any errors on inaccuracies inherent
#  either to the comments or the code of this program, but if reported
#  to me, then an attempt will be made to fix them.
#
#  BINDIR is expected to be overridden by the calling Makefile

LBINDIR = /usr/local/bin
BINDIR	= /usr/local/faces/bin

install:
	install -c -m 755 addxface $(BINDIR)
	install -c -m 755 mailq.faces $(BINDIR)
	install -c -m 755 from.faces $(BINDIR)
	install -c -m 755 lpqall.faces $(BINDIR)
	install -c -m 755 rotary.faces $(BINDIR)
	install -c -m 755 facesaddr $(BINDIR)
	install -c -m 755 facesall $(BINDIR)
	install -c -m 755 mkfacesindex $(BINDIR)
	install -c -m 755 newscheck.faces $(BINDIR)
	install -c -m 755 newsfrom.faces $(BINDIR)

install2:
	install -c -m 755 faces $(LBINDIR)
