	X-BASED ABACUS
	==============	Abacus X widget, V4.3


    It has been tested on the following platforms:

	HP 3xx & 700		HP-UX 9.0x	X11R5
	Sun4, SPARC		SunOS 4.1.3	X11R4
	Sun4, SPARC		Solaris 2.3	X11R5
	PC, 486 		Linux 1.0.9	X11R5

    If yours is not in this list, please let me know -- thanks. The
    most updated source could be found on ftp.x.org under
    /contrib/applications.

HOW TO BUILD?

    It should be easy.

	xmkmf
	make
	xabacus

   If that does not work then:
	mv Imakefile Imakefile.r5
	sed 's/^XCOMM/\/\*\*\/#/' > Imakefile < Imakefile.r5
	xmkmf
	make
	xabacus

    Note: if you don't have 'xmkmf' or the "Makefile" it generates
    doesn't work well, edit Makefile.std for appropriate settings for
    XINCLUDEPATH and XLIBPATH, then try:
	make -f Makefile.std

    You should have Abacus.ad (or Korean.ad) copied to $(HOME)/Abacus or 
    /usr/lib/X11/app-defaults/Abacus to run, especially if the background
    is similar to the beads. Edit this file for your preferences.
    You might want to move xabacus into /usr/bin/X11 (or wherever your
    X binaries are).
    You might also want to move xabacus.man to /usr/man/man1/xabacus.1

ANYTHING SPECIAL IN USING?

    Click on the left mouse button on a bead you want to move.
    Click on the right mouse button or press 'C' or 'c' if you want
    to clear the abacus.
    'Q', 'q', Control-C to kill program.

    There are books on how to use an abacus, but basically all it does
    is add and subtract, the rest you have to do in you head.  Essentially,
    this is a proof by induction that a computer is more powerful than an
    abacus, since a computer program can contain an abacus. (But then again,
    you can simulate a computer within a computer, so what does that show).
    Actually, with a real abacus, one can move more than one row at a time
    with 10 fingers.  But on the other hand, a real abacus does not have
    the current sum displayed as an integer.

    Try resizing the abacus. Notice the shape and size change of the beads.

    Try copying the file Abacus_korean to $(HOME)/Abacus (where Abacus is
    the file name) and running xabacus again.  You now have the faster
    (and more cryptic) Korean Abacus.

    Keep in mind that the Abacus is a widget that can be simply stuck in
    other X programs. (If you find a use for this, let me know).

    Refer to the man page for detailed command line options.

FOUND A BUG?

    Send bugs reports and fixes to the author. For bugs, please include
    as much information as to their location as possible, since I may not
    be able to duplicate the problems with my limited resources.

	David Albert Bagley,	bagleyd@source.asset.com

HISTORY

  [Mar 13, 95]  V4.3: Removed lint warnings and added a VMS make.com .
  [Nov 08, 94]  V4.2: Removed gcc -Wall warnings.
  [Jun 03, 94]  V4.1: R6.
  [May 07, 94]  V4.0: Xt version.
  [Feb 03, 93]  V3.0: Motif version.
		I got some good ideas from Douglas A. Young's book:
		"The X Window System Programming and Applications with
		Xt OSF/Motif Edition", particularly his dial widget.
		I got some good ideas on presentation from Q. Zhao's
		tetris.
  [Dec 17, 91]  V2.0: XView version.
  [Feb 14, 91]  V1.0: SunView version.
