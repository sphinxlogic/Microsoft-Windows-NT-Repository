XPDF, Software, VMS/X11 reader for Adobe Acrobat etc. files
xpdf
====
and pdftops

version 0.5 (beta)
96-may-23

The xpdf and pdftops software and documentation are
copyright 1996 Derek B. Noonburg.

Email: derekn@ece.cmu.edu
WWW: http://www.contrib.andrew.cmu.edu/usr/dn0o/xpdf/xpdf.html

The PDF data structures, operators, and specification
are copyright 1995 Adobe Systems Inc.


What is xpdf?
-------------

Xpdf is a viewer for Portable Document Format (PDF) files.  (These are
also sometimes also called 'Acrobat' files, from the name of Adobe's
PDF software.)  Xpdf runs under UNIX or VMS and the X Window System.

Xpdf is designed to be small and efficient.  It does not use the Motif
or Xt libraries.  It uses standard X fonts.  Xpdf is quite usable on a
486-66 PC running Linux.


Compatibility
-------------

Xpdf is known to work on the following systems:

* x86, Linux 1.2.13 a.out, gcc 2.7.2
* x86, Linux 1.2.13 ELF, gcc 2.7.2
* PowerPC, AIX 4.1, gcc 2.7.0
* SPARC, SunOS 4.1.3, gcc 2.7.2
* MIPS, Ultrix 4.4, gcc 2.6.3
* Alpha, OSF/1 3.2, gcc 2.7.0
* HP-PA, HP-UX 9.05, gcc 2.7.2

It should work on pretty much any UNIX or VMS system which runs X11.
You'll need ANSI C++ and C compilers to compile it.  If you compile it
for a system not listed on the xpdf web page, please let me know.  If
you're willing to make your binary available by ftp or on the web,
I'll add a pointer to the web page.  If you can't get it to compile on
your system, I'll try to help.


Getting xpdf
------------

The latest version is available from:

  http://www.contrib.andrew.cmu.edu/usr/dn0o/xpdf/xpdf.html

or:

  ftp://ftp.andrew.cmu.edu/pub/xpdf/

Source code and several precompiled executables are available.

Announcements of new versions are posted to several newsgroups
(comp.text.pdf, comp.os.linux.announce, and others) and emailed to a
list of people.  If you'd like to receive email notification of new
versions, just let me know.


Running xpdf
------------

To run xpdf, simply type:

  xpdf file.pdf

where 'file.pdf' is your PDF file.  The file name can be followed by a
number specifying the page which should be displayed first, e.g.:

  xpdf file.pdf 18

On-screen controls, at the bottom of the xpdf window:

  left/right arrow buttons:
    Move to the previous/next page.

  'Page' entry box:
    Move to a specific page number.  Click in the box to activate it,
    type the page number, then hit return.

  magnifying glass buttons:
    Increase and decrease the resolution, i.e., zoom in and zoom out.

  print button:
    Bring up a dialog for generating a PostScript file.  The dialog
    has options to set the pages to be printed and the PostScript file
    name.

  curved arrow buttons:
    Rotate the page clockwise and counterclockwise.  These are
    intended primarily for PDF files where the rotation isn't
    correctly specified in the file, but they're also useful if your X
    server doesn't support font rotation.

  '?' button:
    Bring up the 'about xpdf' window.

  'Quit' button:
    Quit xpdf.

Links:

  Clicking on a hyperlink will jump to the link's destination.
  Currently, only links within PDF documents or to another PDF file
  are supported.  Clicking on a URL link just prints the URL.
  Clicking on any other type of link (e.g., links to executable
  programs) prints an error message.

Key bindings:

  n            move to the next page

  p            move to the previous page

  <Space>      scroll down on the current page; if already at bottom,
  <PageDown>     move to next page
  <Next>

  <Backspace>  scroll up on the current page; if already at top, move
  <Delete>       to previous page
  <PageUp> 
  <Previous>

  <Home>       scroll to top of current page

  <End>        scroll to bottom of current page

  arrows       scroll the current page

  control-L    redraw the current page

  q            quit xpdf

Command line options and X resources:

  -err
    Send error messages to /dev/tty instead of stderr (useful if xpdf
    is started from another application, e.g., netscape which
    otherwise pops up an annoying little window for every error).

  -z <number>                [xpdf.initialZoom]
    Set the initial zoom factor (-5 .. 5).

  -g <geometry>              [xpdf.geometry]
  -geometry <geometry>
    Set the initial window geometry.

  -remote <name>
    Start/contact xpdf remote server with specified name (see the
    'Remote server mode' section below).

  -raise
    Raise xpdf remote server window (with -remote only).

  -quit
    Kill xpdf remote server (with -remote only).

  -rgb <number>
    Set the size of largest RGB cube xpdf will try to allocate.  The
    default is 5 (for a 5x5x5 cube); set to a smaller number to
    conserve color table entries.

  -cmd
    Print commands as they're executed (useful for debugging).

  -h or -help
    Print usage information.

Several other standard X options and resources will work as expected:

  -display <display>         [xpdf.display]

  -foreground <color>        [xpdf.foreground]
  -fg <color>

  -background <color>        [xpdf.background]
  -bg <color>

  -font <font>               [xpdf.font]
  -fn <font>

The color and font options only affect the user interface elements,
not the PDF display (the 'paper').


Web browsers
------------

If you want to run xpdf automatically from netscape or mosaic (and
probably other browsers) when you click on a link to a PDF file, you
need to edit (or create) the files '.mime.types' and '.mailcap' in
your home directory.  In '.mime.types' add the line:

    application/pdf pdf

In '.mailcap' add the lines:

    # Use xpdf to view PDF files.
    application/pdf; xpdf -err %s

Make sure that xpdf is on your executable search path.


Remote server mode
------------------

Xpdf can be started in remote server mode by specifying a server name
(in addition to the file name and page number).  For example:

  xpdf -remote myServer file.pdf

If there is currently no xpdf running in server mode with the name
'myServer', a new xpdf window will be opened.  If another command:

  xpdf -remote myServer another.pdf 9

is issued, a new copy of xpdf will not be started.  Instead, the first
xpdf (the server) will load 'another.pdf' and display page nine.  If
the file name is the same:

  xpdf -remote myServer another.pdf 4

the xpdf server will simply display the specified page.

The -raise option tells the server to raise its window; it can be
specified with or without a file name and page number.

The -quit option tells the server to close its window and exit.


Fonts
-----

Xpdf uses X server fonts.  It requires the following fonts:

* courier: medium-r, bold-r, medium-o, and bold-o
* helvetica: medium-r, bold-r, medium-o, and bold-o
* times: medium-r, bold-r, medium-i, and bold-i
* symbol: medium-r
* Zapf Dingbats: medium-r

Most X installations should already have all of these fonts, except
Zapf Dingbats (which isn't used very heavily anyway).

X servers, starting at R5, support font scaling.  Xpdf will
automatically take advantage of this.  There are two types of scaling.
The first type uses standard bitmap fonts: if a font doesn't exist in
the requested size, the server will scale the bitmapped characters.
This is reasonably fast, and the results are readable but not very
pretty.  X servers can also handle true scalable, e.g., Type-1, fonts.
(The free Adobe Acrobat Reader comes with a set of Type-1 fonts which
can be used by X; see the mkfontdir(1) man page for details on setting
these up.)  Scalable fonts are slower, especially since PDF documents
tend to use lots of fonts, but they look much nicer.

Some X servers also support font rotation.  Xpdf will use this feature
if available.


PostScript output
-----------------

Xpdf can generate PostScript files.  Click on the 'printer' button,
select the pages you want to print and the name for the PostScript
file, and click 'Ok' (or hit return).

There is also a separate program which converts PDF files to
PostScript.  It runs from the command line, and can be compiled
without the X includes and libraries.  To run it, type:

  pdftops file.pdf

to generate 'file.ps' or:

  pdftops file.pdf anotherfile.ps

to generate 'anotherfile.ps'.

You can specify a range of pages to convert:

  pdftops -f 3 -l 17 file.ps

will generate PostScript for pages 3 through 17.

Downloading fonts is not currently supported.  This means that if xpdf
has trouble displaying the document, the PostScript output will
probably look just as bad.

The generated PostScript is Level 2.  If you have an old printer which
supports only Level 1 PostScript, you won't be able to print the
files.  I'm not sure how many Level 1 printers are out there -- if you
want Level 1 PostScript output, send me email, and if there's enough
interest, I'll work on it.

The PostScript output has not been thoroughly tested yet.  Please send
me email if you find any bugs.


Distribution
------------

Xpdf may be distributed under the following conditions:

1. The file 'README' is included, unmodified, in the distribution.

2. The distribution includes:
   a. all of the xpdf source code, unmodified; OR
   b. one or more xpdf executables, compiled from the unmodified
      source code; OR
   c. both of the above.

If you distribute xpdf, I would appreciate receiving a pointer (ftp
site, BBS phone number, etc.) or a copy (disk, CD-ROM, etc.).

Modified versions of xpdf or applications using some or all of the
xpdf code (i.e., 'derivative works' under copyright law) may not be
distributed without explicit permission.


Bugs
----

This is an beta release of xpdf, and there are still a bunch of PDF
features which I have yet to implement.

At this point, the biggest problem is that embedded fonts are not
handled correctly.

If you find a bug in xpdf, i.e., if it prints an error message,
crashes, or incorrectly displays a document, and you don't see that
bug listed here, please send me email, with a pointer (URL, ftp site,
etc.) to the PDF file.

This is a list of things which I plan to add (in approximate order of
priority):

* forms
* text search/select/copy
* separate program to extract all text from PDF file
* text annotations
* bookmarks
* support for Adobe Acrobat Amber optimized PDF files
* article threads
* index window or mode -- outline and/or thumbnails
* distribute with a small demo PDF file
* RunLength filter (I haven't found a PDF file which uses this)
* encryption (if I can get documentation)
* adding text to clipping path
* miter limit
* text rendering mode (X display and PostScript output)
* separate pdftoppm program (args are PDF file, page number, and
  resolution (in dpi))

There are also some problems which I know about.  This is a list of
things which I plan to fix:

* handle embedded Type-1 and Type-3 fonts
  - on screen (dump Type-1 fonts to file and let X server load them?)
  - embed in PostScript files
* "cancel" button to stop page computation
* pan by dragging with button 2
* do a better job of scaling and dithering images
  - look at 'Interpolate' param;
* rotated (non-90) images
* fix trademark ('TM') constructed char to use smaller font
* greek small fonts
* deal better with file names in GoToR links
* deal better with TrueColor visuals (don't use RGB cube)
* option and resource for private color map
* "load file" button, file dialog
* mark links on screen
* support other link types
* some kind of 'configure' script to build Makefile.config
* "save bitmap" (PPM) command (how to generate 24-bit image?)
* efficiently handle very large files - don't read entire catalog,
  xref, etc. (is this necessary?)
* don't allocate full-size pixmap (optional?  only if > N bytes?)


Acknowledgments
---------------

Thanks to:

* Patrick Voigt for help with the remote server code.
* Patrick Moreau for the VMS port.
* David Boldt and Rick Rodgers for sample man pages.

