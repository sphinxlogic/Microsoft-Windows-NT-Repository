------------
Contents
------------
Copyright
Authors
History
Synopsis
Files
Making IUFINGER
Configuring IUFINGER
Comments


------------
Copyright
------------

   (C) Copyright 1992-1993 The Trustees of Indiana University

   Permission to use, copy, modify, and distribute this program for
   non-commercial use and without fee is hereby granted, provided that
   this copyright and permission notice appear on all copies and
   supporting documentation, the name of Indiana University not be used
   in advertising or publicity pertaining to distribution of the program
   without specific prior permission, and notice be given in supporting
   documentation that copying and distribution is by permission of
   Indiana University.

   Indiana University makes no representations about the suitability of
   this software for any purpose. It is provided "as is" without express
   or implied warranty.


------------
Authors
------------

Indiana University
University Computing Services
Network Applications Group

Larry Hughes  - Software Engineer (Version 1.1 and later)
Colin Fraizer - Application/Systems Programmer (Version 1.0)


------------
History
------------

Current Version : 1.2
Release Date    : July 19, 1993
Comments        : Support for Multinet and TCPware.

Version         : 1.1
Release Date    : November, 1992
Comments        : Added "#ifdef NOCLUSTER" code, to optionally disallow
                  intracluster fingers.

Version         : 1.0
Release Date    : March, 1991
Comments        : Initial version.


------------
Synopsis
------------

IUFINGER is a finger client for VMS. It will compile and run on VMS systems 
with Wollongong's Pathway (formerly WIN/TCP), Digital's TCP/IP Services for 
OpenVMS (aka UCX), TGV's Multinet, or Process Software's TCPware.

IUFINGER was initially written to address the lack of a finger client
in early versions of UCX, and to be a companion to our IUFINGERD server.

IUFINGER relies fully on finger servers to provide the finger information.
This is true even when fingering the local system.  Thus, it need not
be installed as a privileged image.


------------
Files
------------

README.TXT          - this file

MAKE.COM            - Build IUFINGER distribution
MULTINET.OPT        - MULTINET MMS options file
UCX.OPT             - UCX MMS options file
WINS.OPT            - WINS MMS options file
TCPWARE.OPT         - TCPWARE MMS options file

IUFINGER.C          - IUFINGER server C source code file


---------------
Making IUFINGER
---------------

To build IUFINGER, simply type this command in the IUFINGER distribution 
directory:

   $ @MAKE

The MAKE.COM accepts one optional parameter:

   $ @MAKE  [UCX | WINS | MULTINET | TCPWARE] 

Although MAKE.COM should be able to automatically detect your TCP/IP
implementation, you may override the automatic detection by supplying 
the parameter.

This will produce the binary image IUFINGER.EXE.  To use it, define
a symbol like this, using an appropriate path:

   $ finger :== $sys$nowhere:[nobody]iufinger.exe

Usage:

   $ finger [user[@host]] [user[@host]] [...]


--------------------
Configuring IUFINGER
--------------------

IUFINGER has one option which is probably not applicable for most sites, 
but it is for ours.  

At IU, we have thousands of active user accounts on our cluster.  To 
discourage use of finger service when SHOW USERS would instead suffice, 
we do not permit IUFINGER to perform a list query for a machine in
the cluster.  (Individual user queries are permitted, however).

This behavior is not a default behavior for IUFINGER.  If you wish
to enable it, before building IUFINGER.EXE as described above, define 
the "NOCLUSTER" macro in the source code.  You can achieve this most
easily by adding this line of code to the top of IUFINGER.C:

#define NOCLUSTER 1

Then, define the finger symbol this way, instead of as desribed earlier:

  $ finger :== @sys$nowhere:[nobody]iufinger.com

Now create an "iufinger.com" containing lines like these:

  $ define/user_mode cluster$addresses "129.79.1.1 129.79.1.2 129.79.1.3"
  $ run_finger := $sys$nowhere:[nobody]iufinger.exe
  $ run_finger 'p1' 'p2' 'p3' 'p4' 'p5' 'p6' 'p7' 'p8'

using your own list of addresses for interfaces on your cluster.


------------
Comments
------------

IUFINGER is an unsupported program.  If you decide to use it, do so
only in strict adherence to the copyright/disclaimer shown at the
top of this file, in COPYRIGHT.TXT, and throughout the source code.

You may send comments, questions, and bug reports via Internet mail 
to iufingerd@indiana.edu, or BITNET mail to IUFINGERD@INDIANA.  We 
will respond on a time-available basis.  We do appreciate hearing
from you!

Also, if you wish to be notified of future updates and/or bug
reports, please send email to one of the addresses listed above,
and ask to be placed on the list.

Enjoy!
