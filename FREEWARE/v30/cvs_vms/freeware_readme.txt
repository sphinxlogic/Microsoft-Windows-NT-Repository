CVS, Software, VMS version of Concurrent Versioning System (build ctl tool)

You heard that right, folks.  We are porting CVS *to* VMS (not often
one hears that, about *any* program).

CVS is a version control system, which allows you to keep old versions
of files (usually source code), keep a log of who, when, and why
changes occurred, etc., like RCS or SCCS.  It handles multiple
developers, multiple directories, triggers to enable/log/control
various operations, and can work over a wide area network.  The
following tasks are not included; they can be done in conjunction with
CVS but will tend to require some script-writing and software other
than CVS: bug-tracking, build management (that is, make and make-like
tools), and automated testing.

Download CVS 1.8.85 (including binaries for AXP/VMS) from
    ftp://ftp.cyclic.com/pub/cvs/cvs-1.8.85

The next release of CVS, CVS 1.9, is likely to be very similar to
1.8.85 (particularly with respect to the VMS-specific parts), although
of course if anyone finds any serious bugs we'd love to know about
them and fix them before the release.  Please send any correspondence
to bug-cvs@prep.ai.mit.edu (I'll try to monitor this newsgroup, but
in general I only read it occasionally).

Here are some excerpts from the README.VMS file, which should clarify
the state of the VMS port:

                             CVS port to VMS

DISCLAIMER: This port must be considered experimental.  Although
previous versions have been in use at one large site since about
October, 1995, and the port is believed to be quite usable, various
VMS-specific quirks are known and the port cannot be considered as
mature as the ports to, say, Windows NT or unix.  As always, future
progress of this port will depend on volunteer and customer interest.

This port is of the CVS client only.  Or in other words, the port
implements the full set of CVS commands, but cannot access
repositories located on the local machine.  The repository must live
on another machine (a Unix box) which runs a complete port of CVS.

Most (all?) work to date has been done on OpenVMS/AXP 6.2.  Other VMS
variants might work too.

. . .

Please send bug reports to bug-cvs@prep.ai.mit.edu.

. . .

Wildcard expansion is not yet implemented (i.e. CVS COMMIT *.c won't
work.)  I have found GPL'd routine which does shell globbing, but I
have not tried to put it in yet.

Log messages must be entered on the command line using -m.  I wanted to start
up TPU for editing log messages, but apparently SYS$SYSTEM:TPU.EXE requires
some command table parsing through DCL, and cannot be directly invoked using C
within CVS.  [I did get LSEDIT to launch, but it wasn't interested in argv at
all.]

You can use -e or define the logical EDITOR to cause CVS to try other editors
if you want to test what's available on your system.  I haven't tested this,
but if you install vi or emacs, chances are it will probably work.  Just make
sure the .EXE files are in a directory listed in VAXC$PATH.

. . .



