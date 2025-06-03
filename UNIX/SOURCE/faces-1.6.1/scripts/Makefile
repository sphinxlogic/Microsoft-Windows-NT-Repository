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

LBINDIR = /opt/bin
BINDIR	= /opt/lib/faces/bin

install:
	install -s -m 755 -f $(BINDIR) addxface 
#	install -s -m 755 -f $(BINDIR) mailq.faces
#	install -s -m 755 -f $(BINDIR) from.faces
#	install -s -m 755 -f $(BINDIR) lpqall.faces
#	install -s -m 755 -f $(BINDIR) rotary.faces
	install -s -m 755 -f $(BINDIR) facesaddr
	install -s -m 755 -f $(BINDIR) facesall
	install -s -m 755 -f $(BINDIR) mkfacesindex
	install -s -m 755 -f $(BINDIR) newscheck.faces
	install -s -m 755 -f $(BINDIR) newsfrom.faces

install-sysv:
	install -s -m 755 addxface $(BINDIR)
#	install -s -m 755 mailq.faces $(BINDIR)
#	install -s -m 755 from.faces $(BINDIR)
#	install -s -m 755 lpqall.faces $(BINDIR)
#	install -s -m 755 rotary.faces $(BINDIR)
	install -s -m 755 facesaddr $(BINDIR)
	install -s -m 755 facesall $(BINDIR)
	install -s -m 755 mkfacesindex $(BINDIR)
	install -s -m 755 newscheck.faces $(BINDIR)
	install -s -m 755 newsfrom.faces $(BINDIR)

install2:
	install -s -m 755 -f $(LBINDIR) faces
