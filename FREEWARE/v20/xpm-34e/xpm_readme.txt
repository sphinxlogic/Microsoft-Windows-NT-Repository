/*
 * Copyright (C) 1989-95 GROUPE BULL
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * GROUPE BULL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of GROUPE BULL shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from GROUPE BULL.
 */


				XPM Version 3

WHAT IS XPM?
============

XPM (X PixMap) is a format for storing/retrieving X pixmaps to/from files.

Here is provided a library containing a set of four functions, similar to the
X bitmap functions as defined in the Xlib: XpmCreatePixmapFromData,
XpmCreateDataFromPixmap, XpmReadFileToPixmap and XpmWriteFileFromPixmap for
respectively including, storing, reading and writing this format, plus four
other: XpmCreateImageFromData, XpmCreateDataFromImage, XpmReadFileToImage and
XpmWriteFileFromImage for working with images instead of pixmaps.

This new version provides a C includable format, defaults for different types
of display: monochrome/color/grayscale, hotspot coordinates and symbol names
for colors for overriding default colors when creating the pixmap. It provides
a mechanism for storing information while reading a file which is re-used
while writing. This way comments, default colors and symbol names aren't lost.
It also handles "transparent pixels" by returning a shape mask in addition to
the created pixmap.

See the XPM Manual for more details.

HOW TO GET XPM?
===============

New xpm updates are announced on the comp.windows.x newsgroup, and on the
"xpm-talk" list. All new "official" xpm releases can be found by ftp on:

    ftp.x.org          (18.112.44.100) contrib	 (Boston, USA)
    koala.inria.fr     (138.96.24.30) pub/xpm	 (Sophia Antipolis, France)


DOCUMENTATION:
=============

Old users might read the CHANGES file for a history of changes interesting
the user.

Read the doc. The documentation is in PostScript format (file doc/xpm.ps) and
has been produced with FrameMaker. The source files are available on request.


INSTALLATION:
============

To obtain the XPM library, first uncompress and untar the compressed tar file
in an approriate directory.

Then you can either compile xpm via "imake" or in a stand-alone way.

WITH IMAKE:

	Imakefiles are provided to build both shared and unshared libraries.
	First have a look at the beginning of the lib/Imakefile and see if you
	need to do some modification to fit with your system.
	Then execute the following commands:

		xmkmf -a
		make

	which will build the XPM library and the sxpm application. 
	Then do:

	     	make install
		make install.man

	which will install the library and the sxpm pregram and man page.

	If it fails, you may edit the Imakefiles to add compilation flags to
	suit your machine.

WITHOUT IMAKE:

	To compile xpm, in the xpm directory you just created, do:

	        make -f Makefile.noX

	Then to install it, do:

		make -f Makefile.noX install

SXPM:
====

In addition to the library the sxpm tool is provided to show XPM file and
convert them from XPM1 or XPM2 to XPM version 3. If you have previously done
'make' or 'make all' you should have it yet, otherwise just do:

		      cd sxpm; make

This application shows you most of the features of XPM and its source can be
used to quickly see how to use the provided functions.

By executing 'sxpm -help' you will get the usage.

Executing 'sxpm -plaid' will show a demo of the XpmCreatePixmapFromData
function. The pixmap is created from the static variable plaid defined in the
sxpm.c file. sxpm will end when you press the key 'q' in the created window.

Executing 'sxpm -plaid -sc lines_in_mix blue' will show the feature of
overriding color symbols giving a colorname, executing 'sxpm -plaid -sp
lines_in_mix 1' will show overriding giving a pixel value, and executing 'sxpm
-plaid -cp red 0' will show overriding giving a color value.

Then you should try 'sxpm -plaid -o output' to get an output file using the
XpmWriteFileFromPixmap function.

You can now try 'sxpm -plaid -o - -nod -rgb /usr/lib/X11/rgb.txt' to directly
get the pixmap printed out on the standard output with colornames instead of
rgb values.

Then you should try 'sxpm plaid.xpm' to use the XpmReadFileToPixmap function,
and 'cat plaid_mask.xpm|sxpm' to see how "transparent pixels" are handled.

The XpmCreatePixmapFromData function is on purpose called without any XpmInfos
flag to show the utility of this one. Indeed, compare the color section of the
two files foo and bar obtained from 'sxpm -nod -plaid -o foo' and 'sxpm -nod
plaid.xpm -o bar'. All the default colors and also the comments have been
restored.

To end look at plaid_ext.xpm and try "sxpm -nod plaid_ext.xpm -v" to see how
extensions are handled.

Of course, other combinations are allowed and should be tried. Thus, 'sxpm
plaid.xpm -o output -nod' will show you how to convert a file from XPM1 or XPM2
to a XPM version 3 using sxpm.

See the manual page for more detail.

OTHER TOOLS:
===========

Several converters dealing with XPM and a pixmap editor can be found in the
xpm-contrib distribution. Also I recommend the use of netpbm to do any kind of
general image operations such as scaling, resizing, dithering, and to convert
from and to any other image format.

DISCUSSION:
==========

There is a mailing list to discuss about XPM which is xpm-talk@sophia.inria.fr.
Any request to subscribe should be sent to xpm-talk-request@sophia.inria.fr.

COPYRIGHT:
==========

  Copyright 1989-95 GROUPE BULL --
  See license conditions in the COPYRIGHT file of the XPM distribution

Please mail any bug reports or modifications done, comments, suggestions,
requests for updates or patches to port on another machine to:

lehors@sophia.inria.fr		(INTERNET)

33 (FRANCE) 93.65.77.71		(VOICE PHONE)

Arnaud Le Hors			(SURFACE MAIL)
Bull c/o Inria BP. 109
2004, Route des lucioles
Sophia Antipolis
06561 Valbonne Cedex
FRANCE
