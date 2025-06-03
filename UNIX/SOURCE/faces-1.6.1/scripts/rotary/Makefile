#  @(#)Makefile 1.2 91/04/15
#
#  Copyright (c) Steve Kinzler - April 1991.
#
#  Permission is given to distribute these sources, as long as the
#  copyright messages are not removed, and no monies are exchanged.
#
#  No responsibility is taken for any errors on inaccuracies inherent
#  either to the comments or the code of this program, but if reported
#  to me, then an attempt will be made to fix them.

install:
	install -c -m 755 -o root rotary   /usr/local/bin
	install -c -m 755 -o root rotary   /nfs/supertest/usr/local/bin
	install -c -m 644 -o root rotary.1 /usr/local/man/man1

deinstall:
	rm -f /usr/local/bin/rotary /nfs/supertest/usr/local/bin/rotary \
	      /usr/local/man/???1/rotary.1 /usr/local/lib/syn/rotary.1
