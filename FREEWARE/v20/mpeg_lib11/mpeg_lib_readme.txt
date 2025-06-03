			   The MPEG Library

		   Version 1.1 - 14 November, 1994

MPEG decoding engine (c) 1992 The Regents of the University of California
Front end (c) 1994 Gregory P. Ward (greg@pet.mni.mcgill.ca)

The MPEG Library is a collection of C routines to decode MPEG movies
and dither them in a variety of colour schemes.  Most of the code in
the library comes directly from the Berkely MPEG player, an
X11-specific implementation that works fine, but suffers from minimal
documentation and a lack of modularity.  A front end to the Berkeley
decoding engine was developed by Greg Ward at the Montreal
Neurological Institute in May/June 1994 to facilitate the development
of an MPEG player specifically for Silicon Graphics workstations; the
decoding engine together with the MNI front end constitute the MPEG
Library.


AVAILABILITY
============

The original Berkeley decoder is available as mpeg_play-2.0.tar.Z via
anonymous ftp from toe.cs.berkeley.edu, in the directory
/pub/multimedia/mpeg.  There is also a version with a Motif interface;
look for mpeg_play-src.tar.Z.  

Both the above-mentioned SGI-specific MPEG player (glmpeg_play) and
the MPEG Library itself are available from yorick.mni.mcgill.ca, in
/pub/mpeg.  (Note that this address is due to change to
ftp.mni.mcgill.ca Real Soon Now, so be prepared to try this address as
well.)  Several other MPEG-related goodies are gathered here as well,
in particular the MPEG encoder/decoder from the Portable Video
Research Group at Stanford University.


CODE STRUCTURE
==============

A programmer wishing to write a basic MPEG-1 player should 1) include
the file mpeg.h to get some typedef's and function prototypes; 2) look
in the source file wrapper.c for documentation on the five functions
therein (OpenMPEG(), CloseMPEG(), RewindMPEG(), SetMPEGOption(), and
GetMPEGFrame ()); and 3) call only those five functions.  Enterprising
programmers may wish to dive into the other twenty-odd source files
comprising the Berkely decoding engine.  (Note: the files globals.c
and globals.h are really part of the MNI front end, as they were not
part of the original Berkeley code.  However, most of the code and
variables in globals.c was simply moved from various other source
files, and should not be of interest to the outside world.)

For a very simple MPEG player that uses the SGI Graphics Library to
display frames, take a look at easympeg.c included with this
distribution.  This should be easily portable, since there are only a
few calls to GL.  I have also written a full-fledged MPEG player for
SGI platfoms (with frame-buffering, interactive controls, dynamic
zooming, etc.)  called glmpeg_play.  It is available via anonymous ftp
as explained above.


BUILDING THE LIBRARY
====================

As of version 1.1, I am explicitly *not* supporting non-ANSI compilers
to build the library itself.  If your hardware vendor (eg. Sun) is so
incredibly out-of-date as to not supply an ANSI compiler with their
systems, use gcc.

However, it is possible to build programs that *use* the library with
a non-ANSI compiler, as the header file mpeg.h has the requisite
kludges that allow it be compiled with either ANSI or non-ANSI
compilers.  

Note that some compilers that are ANSI (in particular the MIPS
compiler in its default, or "extended ANSI", mode) do NOT define
__STDC__ even though they are ANSI compliant.  For such compilers, you
will have to add -D__STDC__ to the appropriate CFLAGS variable in the
Makefile.

Anyone who has problems compiling the MPEG Library and/or linking it
with other software should contact me (greg@pet.mni.mcgill.ca).  


COMPLETE LACK OF WARRANTY
=========================

This software is supplied without even the faintest shred of assurance
that it works in its entirety.  In particular, none of the dithering
options except full colour have been tested at all (except insofar as
they worked in the original Berkeley player); I've been too busy
getting a nice 24-bit movie player working to worry about dithering to
lesser video systems.  ;-)  If you want to use the other dithering
options, go right ahead, but be warned: it's anybody's guess what
format the data will be returned in (you'll probably have to look at
the code that does the actual dithering).  Also, the code has only
been compiled on SGI platforms running IRIX 4.0.[45] and 5.2.

Please drop me a line if you use the MPEG Library, either successfully
or not.  And if you use it unsuccessfully and find a nice, easy fix,
do please let me know about it!  My email address is
greg@pet.mni.mcgill.ca.

