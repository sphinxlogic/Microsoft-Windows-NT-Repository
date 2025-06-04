FLIST, UTILITIES, File and Directory Manager

FLIST V2.4  [25-FEB-1998]
Copyright © 1990, 1998, Hunter Goatley.  All rights reserved.

----------------------------------------
This code may be freely distributed and modified for non-commercial
purposes as long as this copyright notice is retained.  Modified
copies may not be redistributed.
----------------------------------------

FLIST is a file and directory manager written in VAX TPU and VAX C (DEC
C).  The program was originally written in 1987 by Hunter Goatley. 
FLIST v2.0 includes many new features, added by Hunter Goatley and Peter
Galbraith.  (Generic EVE and DCL code by Peter Galbraith is provided to
support a "kept" EVE subprocess.  This code is compatible with the code
used by EVEplus.)

Please forgive me for the bad code---I wrote it quickly and have never
had the time to go back and really do much with it.  It isn't as
pretty as I'd like, but it works.

There also isn't much documentation, but pressing "H" while in FLIST
will show you help on the commands available.

To build, simply execute LINK.COM.  FLIST runs on both OpenVMS
VAX and OpenVMS Alpha.

To use, you must define a logical and a symbol:

	$ define FLIST_TPU_SECTION device:[dir]FLIST.TPU$SECTION
	$ FLIST :== $device:[dir]FLIST.EXE

You can then type

	$ FLIST [file-spec]

The optional file-spec can contain wildcards.  Once inside of FLIST, you
can type H for a one-line description of some of the commands.  Pressing
PF2 will show you a list of all key definitions.

All of the FLIST commands consist of either single-key presses or
GOLD-key combinations; be careful which characters you type once inside
FLIST.  (If you use a DECwindows mouse for cut and paste between
DECterms, be careful that you don't paste into a DECterm running FLIST,
because each pasted character will be treated as an FLIST command.)

Known limitations:

  o  Long  file  names  get  truncated and can't be viewed; it's one of
     those little bugs I just haven't gotten to yet.

See AAAREADME.TOO for additional information on FLIST and KEPT EVE.


--------------------------------------------------------------------------------
CONTACTING THE AUTHOR

FLIST was written by Hunter Goatley.  Comments, suggestions, and questions
about this software can be directed to this e-mail address:

	HG-Bugs@LISTS.WKU.EDU

--------------------------------------------------------------------------------
COPYRIGHT NOTICE

This software is COPYRIGHT © 1990, 1998 HUNTER GOATLEY. ALL RIGHTS
RESERVED. Permission is granted for not-for-profit redistribution, provided
all source and object code remain unchanged from the original distribution,
and that all copyright notices remain intact.

DISCLAIMER

This software is provided "AS IS".   The author makes no representations or
warranties with repsect to the software and specifically disclaim any implied
warranties of merchantability or fitness for any particular purpose.
