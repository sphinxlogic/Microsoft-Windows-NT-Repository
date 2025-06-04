XLOADIMAGE - X11 Image Loading Utility

WHAT IS IT?

This utility will view several types of images under X11, load
images onto the root window, or dump processed images into one of
several image file formats.  The current version can read many
different image file types; for a complete list type "xloadimage
-supported".

A variety of options are available to modify images prior to viewing.
These options include clipping, dithering, depth reduction, zoom
(either X or Y axis independently or both at once), brightening or
darkening, and image merging.  When applicable, these options are done
automatically (eg a color image to be displayed on a monochrome screen
will be dithered automatically).

TIFF AND JPEG SUPPORT

This version of xloadimage supports both JPEG and TIFF files through
the IJG V4 JPEG library and the Sam Leffler's V3 TIFF library.  The
libraries are included almost in their entirety.

The libconfig.c program is used to create appropriate Makefile
configurations so that no special intervention is required to compile
these libraries on most systems.  If there are problems compiling
these libraries on your system you can leave out support for them
during the configuration process.  In my tests on various machines
there were never any problems building the supplied libraries.

Unfortunately I cannot answer most questions regarding these
libraries.  You can direct such questions to:

	jpeg-info@uunet.uu.net
	tiff@ucbvax.berkeley.edu

BUILDING THE RELEASE

To build xloadimage simply type "make".  If you prefer to use xmkmf or
imake, please read the note below before doing so.

Xloadimage will build a configuration program first and run it -- the
configuration program will look at your system to try to determine
what variety of system it is, where the X libraries and include files
are, and whether or not some nonstandard libraries are likely to be
needed.  The configuration program is specifically designed to make
the configuration process very painless on most systems available
today, but it cannot predict all possible variations.

If the configuration program cannot find something that it's looking
for it will ask you where it is and it will attempt to verify the
correctness of your response.  You will also be prompted for the name
of the C compiler you want to use, and where the TIFF and JPEG
libraries and header files are located.  The questions should be
simple and straightforward; if you don't know the answer to a question
you can usually hit "return" and the default will work.

If you make a mistake during configuration you can interrupt the
configuration program (using ^C or whatever the interrupt character is
on your system); type "make" to restart.

If you are building xloadimage on multiple systems you should type
"make clean" to clean up the build on the previous system; the
configuration will be re-run when you type "make".

Once configured the xloadimage distribution will continue to use the
same configuration parameters until a "make clean" or "make configure"
is done.

All configuration options are written into Make.conf; if you need to
make changes and don't like the configuration utility you can always
edit the file yourself.  Keep another copy of any such changes because
xloadimage will overwrite Make.conf whenever you do a "make clean" or
if it needs to rebuild the configuration program.

NOTE: The Makefile assumes that "cc" exists and can be used to build
the configuration program.  If your C compiler isn't called "cc" or is
not found along your path you will need to edit the CC= line in
Make.conf to provide the appropriate compiler name.

NOTE: Many sites use the xmkmf to generate an appropriate Makefile
from an Imakefile when building X applications.  While xloadimage
includes an Imakefile for this purpose IT IS STRONGLY RECOMMENDED THAT
YOU DO NOT USE XMKMF OR IMAKE.  There is not enough information
included in the MIT releases to determine the appropriate
configuration for xloadimage.  For this reason some capabilities
cannot be configured using xmkmf/imake.  If you decide to use xmkmf
and/or imake anyway and you have problems building the distribution
you should copy Makefile.std into Makefile and follow the normal
installation instructions.

NOTE: If you are on a VMS system the automatic configuration code
won't work at all.  There is a VMS_BUILD.COM script included which
should be close to what you need to build xloadimage.  Unfortunately I
cannot test this script so it may not be perfect -- you're on your own
if you're using VMS (which is probably not surprising if you've done
this kind of thing before).

INSTALLATION

After compiling and installing xloadimage, I recommend linking or
symlinking to the executable with the names "xview" and "xsetbg".  The
default behavior is slightly different when invoked with these
commands (they're also easier to type).  If you have a public image
area you should consider setting the SYSPATHFILE option in the
makefile and setting up a system-wide configuration file.  See the man
page for information on the format of this file.

IMPLEMENTATION

Most functions are not particularly fast, and some functions use
simple-minded algorithms deliberately over more advanced ones.  I
stressed portability over all and simplicity over performance,
although many algorithms have become fairly complex over time.  I
believe the result is a usable, portable tool which should serve the
needs of most users.

The source code is basically in two parts: image manipulation routines
and everything else.  The image manipulation routines should be
completely independent of X, thus allowing people to use them under
other graphical systems.  No guarantees here, but I tried.

Performance-oriented people will notice that some operations are
redundant.  Xloadimage is designed to work fairly quickly for most
operations but in some cases I opted for a cleaner internal design
rather than pure performance.

OWNERSHIP AND COMMERCIAL USE

I used a modified version of the MIT X Consortium copyright with all
of these functions, thereby allowing full freedom with the code so
long as the copyright notices remain intact.  All contributions have
similar notices.

Commercial sites are welcome to use the code even without special
permission, licensing or royalty fees.  I'm very happy to hear of this
stuff going toward real products (it's in several that I am aware of)
so I appreciate hearing of any use of some or all of the code in other
applications.  If you tell me you're using it I'm much more likely to
keep you informed about new versions as well, so it works out well all
around.  Xloadimage does not require any form of licensing or royalty
payment to use in a commercial product, but the MIT copyright does
require mention of the copyright notice in accompanying documentation;
xloadimage has several copyrights that may need inclusion.  It's
unlikely that I would ever prosecute someone for neglecting to follow
the terms of the copyright.

If you're using xloadimage in a commercial product beware that it adds
handlers for common exceptions that print out my email address.  While
I'm happy to support any xloadimage user it is difficult to support
them if I don't know about changes you have made.  At the very least
modifications should include changes to the patchlevel information to
indicate your company and/or product name even if you do not change
the email address.

PRAISE, SUGGESTIONS AND BUG REPORTS

Praise, suggestions, and bug reports should go to:

	Jim Frost
	CenterLine Software
	10 Fawcett Street
	Cambridge, MA  02138
	(617) 498-3000
	jimf@centerline.com

Please include the version number and sample image data if you are
reporting a bug.

Functions implementing new image types are welcomed; mail them to the
same address and I'll do my best to distribute them.  Please include a
small sample image.  Try to send them as public domain so I can keep
the number of differing copyright messages to a minimum -- I'll use my
standard message and leave the implementor's name and information in
the file for credit.  I wouldn't copyright this stuff at all except
that it's a requirement for X11 distribution.

If you particularly like xloadimage, feel free to drop me a line.  It
makes me feel good and I get a feel for who does what with it, which
sometimes influences what parts are worked on.

THANKS

Special thanks to the crew at the Boston University Graphics Lab for
their assistance and sample images, and to bzs@std.com for his simple
dithering algorithm (or what's left of it).  Real special thanks to
Kirk L. Johnson (tuna@athena.mit.edu) for a very nice GIF loader and
dithering routine, to Mark Snitily (zok!mark@apple.com) for 386/ix
compatibility work, to Andreas Stolcke (stolcke@icsib12.berkeley.edu)
for miscellaneous bug fixes, to Anthony A. Datri (datri@convex.com)
for a number of things, to Mark Moraes (moraes@cs.toronto.edu) for
the slideshow colormap fix, to Gregg Townsend (gmt@cs.arizona.edu) for
a suggested dithering routine and other fixes, to Brian Frost
(B1F5814@RIGEL.TAMU.EDU) for changes for VMS, to Chip Horstman for G3
FAX support, to Deron Dann Johnson (dj@eng.sun.com) for fixing the
RetainTemporary bug, to Tom Tatlow (tatlow@dash.enet.dec.com) for
image rotation code, to Mark A. Horstman (mhorstm@sarek.sbc.com) for
tilde expansion in .xloadimagerc files and virtual-root support in
root.c, to Tim Roper (timr@labtam.labtam.oz.au), Graeme Gill
(graeme@labtam.oz.au) for gamma correction and Utah RLE image support,
Mark Majhor (uunet!sequent!markm) for FBM and MacPaint support, Ian
MacPhedran (macphed@dvinci.usask.ca) for PGM and PPM support, Per
Fogelstrom (pf@diab.se) for a fix to send.c, Hans J. Albertsson
(hans@Sweden.Sun.COM) for cleaning up GIF aborting, Graham Hudspith
(gwh@inmos.com) for a geometry patch, Glenn P. Davis
(davis@unidata.ucar.edu) for McIDAS areafile support, Keith S. Pickens
(maxwell.nde.swri.edu!ksp) for fixing the RLE loader to work with the
updated zio package, Mike Douglas (douglas@wilbur.coyote.trw.com) for
normalization, Rod Johnson (johnson@wrl.epi.com) for speedup
suggestions, Hal Peterson (hrp@cray.com) for his Imakefile fix, Matt
Caprile (Matthew.Caprile@ec.bull.fr) for slideshow delay code, Bob
Deroy (rwd@bucrsb.bu.edu) for mondo 24-bit Sun Rasterfile images that
broke everything, Christos S. Zoulas (christo@ee.cornell.edu) for a
first-cut 24-bit implementation, Gerald James Barnes
(gjb@oasis.icl.stc.co.uk) for a first-cut forced-visual
implementation, Michael Campanella (campanella@cvg.enet.dec.com) for
more VMS changes, Kee Hinckley (nazgul@alfalfa.com) for robustness
changes to the g3 and MacPaint loaders and the ZIO package, Tim
Northrup (tim@brspyr1.brs.com) for PC Paintbrush and GEM image
formats, Richard Weidner (richard@elroy.jpl.nasa.gov) for lots of
24-bit testing,  Eckhard Rueggeberg (erueg@cfgauss.uni-math.gwdg.de)
for a better PCX loader, and any others whose names I've missed.

HISTORY

Patch 01 contained a new Makefile.std, Makefile.gcc, and Imakefile.
It contained a bug-fix to sendImageToX() which allowed bitmaps to be
sent from little-endian machines (eg VAX, 80386) correctly, and a fix
to xbitmapLoad() to allow correct loading of X10 bitmap images.  An
enhancement to imageInWindow() which allowed exiting from image
windows by typing 'q' was submitted by Chris Tengi
(tengi@idunno.princeton.edu) and was included.  The previously missing
file 'patchlevel' was included.

Patch 02 contained modifications to the Makefiles, support for the X
Pixmap image type, a different dithering algorithm that didn't blow
the image up (with the old one moved to halftone.c), and a bug fix to
zoom.c to correct problems when zooming bitmaps.

Patch 03 contained a new loader for GIF files.  The dither bits array
in dither.c was changed so it worked properly, and both dither.c and
halftone.c had minor bugs fixed.  Merge.c was modified to correct bugs
when merging RGB images.  Pbm.c was modified to handle raw format
images.  Root.c was modified to deny image loads which would change
the root window's colormap.  Send.c was modified to use shared colors
whenever possible and to handle color displays which have depths which
are not a multiple of 8.  Window.c was modified to avoid deleting the
default colormap, allowing proper operation on some servers prior to
X11R3 patchlevel 08.  There were many miscellaneous bug fixes.

Patchlevel 04 contained an enhancement to root.c to use
RetainTemporary and KillClient(disp, AllTemporary) so that it could
clean up after itself when reloading.  The -quiet and -zoom options no
longer cause garbage to be displayed for the image title.  A small bug
in new.c that caused incorrect allocation of bitmap images was fixed.
Several calls to XCreateColormap were missing the "visual" parameter
in send.c; this was fixed.  A bug relating to -border and monochrome
displays was fixed.  There were several changes to the Imakefile and
Makefiles.

Patchlevel 05 contained enhancements to allow slideshows and
fullscreen viewing, some bug fixes related to scrolling around within
images, Saber-C makefile enhancements, a bug fix to the halftoning
title, and the addition of greyscale Sun Rasterfile support.

Patchlevel 06 contained support for G3 FAX images, bug fixes to
merge.c to fix some signed/unsigned errors and clipping problems,
changes to root.c to make previously allocated resources be freed
properly, and a completely new dithering routine.

Version 2, patchlevel 00 contained support for MacPaint, FBM, PGM,
PPM, CMU, Utah RLE and XWD image formats, gamma equalization, image
smoothing, and image rotation.  G3 FAX support was modified to cut
down on false positive identifications.  The zio subsystem was
modified to cache reads for performance improvements and to allow
stdin to be used as an input source.  Several loader functions which
did not properly close their files were fixed.  Color slideshows now
work.  Icon titles use an abbreviated titlebar title to enhance
readability.  The resource class name was changed from XLoadImage to
xloadimage to be more predictable.  Several options now propagate to
all images following them if the -slideshow option is specified.

Version 2.01 contained several fixes to window.c to work around a bug
in twm/tvtwm which could crash servers.  The resource class name was
changed (again) to Xloadimage to conform with standard class naming
practices.  Several problems with System-V compilation were corrected.
The gcc-1-37 make target was fixed to prevent a double-define.  The
GIF loader was patched to respond better to short GIF files.  The Utah
RLE image loader was patched to work with the updated ZIO package.

Version 2.02 added the options -default, -gray, -normalize, and
-private.  The memToVal routines were macro-ized for substantial speed
increases.  The atom used for deleting previously allocated colors for
the -onroot option was changed to correspond to that used by xsetroot.
Dithering and halftoning were changed to use a lookup table for
intensities to speed them up somewhat.  Rle.h was changed to use
memToValLSB() instead of its own byte-swapping algorithm for
portability.  Window.c was modified to eliminate the initial paint,
moving it instead to within ConfigureNotify.  A fix was made to xwd.c
to fix its colormap loader for out-of-order colormaps.  The G3 FAX
identification function was fixed and the G3 loader moved to prior to
the MacPaint loader so that both can be used.  A bug in send.c where
color images using the default colormap would not appear correctly if
the display depth was not a multiple of 8 was corrected.  The default
gamma value used in rle.c was changed from 2 to 1.  There were
miscellaneous portability changes.

Version 2.03 added -delay.  Many minor programming errors were cleaned
up.  Hash.c was changed to use image.h instead of local typedefs
(fixing a problem with PPM image loading).  Window.c was changed to
fix a problem with override redirect windows in fullscreen mode,
and refresh problems with some servers in slideshow mode were fixed.
Path.c was modified to ignore directories when searching for files.
Send.c was modified to allow TrueColor and DirectColor visuals.

Version 2.03-JPL contained interim code to handle 24-bit TrueColor and
DirectColor displays pending the release of Version 3.0.

Version 3.0 added full support for all server types.  Internal support
for 24-bit images was implemented (including the improvement of Sun
Rasterfile and PPM support to use the 24-bit format internally).  Many
modifications to image processing routines to support or make use of
24-bit images.  A much-improved colormap reduction algorithm replaced
the primitive one in previous versions.  The window display code was
modified to work with the best possible visual.  The window display
code was modified to use backing store when available (and -pixmap was
added to force the old technique).  The -fit option was added to force
an image to be fit into the default colormap if it didn't fit as
supplied.  The -fork option was added to allow automatic
backgrounding.  The -onroot -fullscreen options were modified to
preserve aspect ratio.  The -windowid option was added to allow the
setting of the background pixmap of any window.  The -onroot code was
modified to work with DECWINDOWS servers.  The -normalize option was
improved for color images.  The -slideshow option was removed (and
made the default behavior) and -merge was added to allow more flexible
image merging.  The WM_DELETE protocol is now recognized.  PC
Paintbrush (PCX) and GEM bitmap image formats are now supported.  The
GIF 89a signature is now recognized (although no support for its
additional capabilities has been implemented).

Version 3.01 corrected color problems when merging RGB files and fixed
a few small problems.

Version 3.02 fixed a -zoom bug for 24-bit images, a bug in the
imageOnRoot() function for DEC and NCD servers, and a couple of bugs
in merging and smoothing.

Version 3.03 added JPEG and color PCX image support and small fixes to
smooth() and normalize().

Version 4.0 added color PCX support, VICAR image support, NIFF image
support, -dump, -unhalftone, JPEG V4 and TIFF V3 integration, generic
filtering, and fixed a lot of bugs and minor build problems on some
systems.  Options are now handled in the order they are given rather
than a fixed order.  G3 FAX loader was removed because it was totally
unreliable.

Version 4.1 fixed a number of ANSI-C compilation problems, enhanced
the automated configuration to target HP and SGI machines
automatically, corrected a few minor bugs, and included the IJG JPEG
V4A code.
