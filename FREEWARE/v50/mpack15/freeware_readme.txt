MPACK, UTILITIES, Encode and Decode MIME Mail messages

MPACK and MUNPACK are utilities for encoding and decoding (respectively)
binary files in MIME (Multipurpose Internet Mail Extensions) format mail
messages.

This is based on Patrick Moreau's port of MPACK/MUNPACK to OpenVMS, enhanced
and polished for submission to the OpenVMS Freware CD by Martin Vorlaender.

The files [.SRC]README.VMS and [.SRC]README.UNIX carry further information.

The supplied object and image files were built under OpenVMS/VAX 5.5-2 using
VAX C 3.2 and OpenVMS/Alpha 6.2-1H3 using DEC C 5.7.

I must admit that the code to parse VMS SMTP addresses (SMTP%"user@domain"
or MX%"..." or whatever) is a bit simple - please don't stress it too much.

The really bad message is that sending by VMS mail (via the callable
interface, as implemented) will not yield a usable MIME message because
the header lines MPACK generates are not recognized as such and sent as
part of the body.  The good message is the "MPACK -o" works and MUNPACK
will correctly decode such messages.

Also, MUNPACK only stores fixed-length 512 byte record files. When MPACKing
any other type of file (e.g. object files), restoring the correct record
format must be left to some other tool, e.g. by first ZIPping the file
(which is a good idea anyway).


To install:
===========

Copy MPACK.EXE and MUNPACK.EXE from the appropriate subdirectory ([.VAX]
or [.ALPHA]) to some convenient location.  Define - e.g. in LOGIN.COM -
foreign symbols that point at them:

  $ MPACK   :== $dev:[dir]MPACK.EXE
  $ MUNPACK :== $dev:[dir]MUNPACK.EXE

Then you can call them as described in the online help file MPACK.HLP
and MUNPACK.HLP.  These must be assembled into a help library to be
useful, e.g.

  $ LIBRARY /CREATE /HELP TOOLSHELP.HLB
  $ LIBRARY /REPLACE TOOLSHELP.HLB MPACK.HLP,MUNPACK.HLP

Online help can then be called by

  $ HELP @dev:[disk]TOOLSHELP MPACK


To build from object files:
===========================

To link the executable images from the supplied object files, just enter

  $ @LINK

in the top-level directory.


To build from sources:
======================

The source files are supplied in the [.SRC] subdirectory.  A MMS/MMK
description file is supplied.  To build, just enter

  $ MMS  ! or MMK

Using macros, the build process can be directed: Define
__VAXC__=1         to build using the VAX C compiler instead of Compaq C
__DEBUG__=1        to build a debug version
USE_GETOPT=1       if your C RTL doesn't define getopt()
USE_GETHOSTNAME=1  if your C RTL doesn't define gethostname()

You can safely ignore any %MMS-W-GWKACTNOUPD messages.  Recent versions
of Compaq C are known to throw a %CC-I-INTRINSICINT message which is
harmless as well.

MMK is available from http://www.madgoat.com/ .

The build process has been successfully tested under
OpenVMS/VAX 5.5-2 (VAX C 3.2) and 6.2 (DEC C 5.7),
OpenVMS/Alpha 6.2-1H3 (DEC C 5.7), 7.1 (DEC C 5.6) and 7.2-1 (Compaq C 6.2).


Martin Vorlaender, 3-FEB-2001

mv@pdv-systeme.de
martin@radiogaga.harz.de
