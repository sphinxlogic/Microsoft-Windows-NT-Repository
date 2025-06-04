SOCKETSHR, NETWORKING, TCP/IP socket library

SOCKETSHR is a TCP/IP package independent socket library for OpenVMS (VAX and
Alpha).  All TCP/IP packages come with their own socket library, why do we need
another one?

The advantage of SOCKETSHR is that it is implemented as a shared image which
can be used with all well known TCP/IP packages.  Programs linked to it may run
on all of them *without relinking*.

This makes it possible to use only one executable if different TCP/IP packages
are used in a network.  It makes it much easier to distribute a program in
binary form to the public.  Many programs like ARCHIE, GOPHER, MOSAIC, FINGER,
TALK, etc.  may benefit from being linked against SOCKETSHR and distributed as
binaries.

SOCKETSHR also supports the standard i/o routines like fputs() etc.

Originally by Eckart Meyer, with changes by Andy Harper and John Malmberg

Send bugs to "dickey@herndon4.his.com".
