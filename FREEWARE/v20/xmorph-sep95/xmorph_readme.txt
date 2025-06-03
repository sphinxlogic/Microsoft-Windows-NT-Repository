xmorph: Digital Image Warp ("morph") graphical user interface

Written and Copyright (C) 1994 by Michael J. Gourlay
NO WARRANTEES, EXPRESS OR IMPLIED.

Xmorph is a digital image warping program, also known as a "morphing"
program.  It runs under the X Window System, using the X library, X
toolkit, X miscellaneous utilities, and the Athena widgets, all of
which are publically available from MIT, and part of the typical X
installation.  It is written in ANSI C, but I can make available
versions of the code that will compile with a K&R1 C compiler.

Be sure to read George Wolberg's _Digital Image Warping_.

Xmorph loads, saves, warps, and dissolves images, and loads, saves,
creates, and manipulates control meshes which determine the warping.
Xmorph has help pages built into it, so no external documentation is
necessary.

Right now, Xmorph only reads and writes Targa image files.  There is
a PBMplus utility to convert Targas to and from other formats.  Also,
Art Department Pro's Professional Conversion Pack (which runs on
Amiga systems) has a Targa loader and saver.

See the Makefile for info on how to compile the program.  Xmorph is
self-documented, so run the program to find out how to use it.
The internal documentation is in "help_menu.c".

This program has been tested on various models of Sun SPARCs running
both SunOS and Solaris, SGI's running IRIX, IBMPC running Linux, DEC
Alpha running whatever they run nowadays, and an HP 9000 running
HPUX.  If you get Xmorph running on other architectures, please send
email to me to report your success, and whether you had any problems
compiling, and what the solutions were to those problem.

Note that this program has not been coded to run on monochrome
displays.  I have no monochrome machines to test this on.  Some
monochrome X servers have a sort of PseudoColor interface which
converts to grayscale on the fly so you don't need to change your X
code to use monochrome.  If you have code to let xmorph run on
monochrome displays, please send it to me.

---

Send bug reports to michael.gourlay@colorado.edu

---

Look in ftp.x.org (was export.lcs.mit.edu) in the contrib directory
heirarchy for xmorph.

----

For xmorph animations see Andy Thaler's WWW home page at
http://www.informatik.tu-muenchen.de/cgi-bin/nph-gateway/hphalle8/~thaller/

---

Please report to me if you have animations that are cool that you'd
like other people to see.

---

I've been asked by many people what image file format xmorph uses.
This is documented aboce.  I use Targa.  I intend to use others in the
future, but for now Targa works well because it's sufficiently
flexible to use a number of storage schemes (including colormapped,
24-bit true color, and 32-bit alpha true-color, and gray scale) and it
provides for some compression using a form of run-length encoding,
which is modified to minimize the worst-case expansion.

I've also been asked about programs which convert these images to
animations.  You can use PBMplus from Jef Poskanzer or ImageMagick to
convert Targa to other image formats.  Use mpeg_encode from Berkeley
to create MPEG animations.  Also see Andy Thaler's WWW home page for
links to some relavent programs.

----
TO DO:

I think it would be really cool and very appropriate if I got "before"
and "after" pictures of Michael Jackson and morphed one into the other
as a test.  If anybody has such images, please send them to me.

----

I would like to port xmorph to other window systems.  I need to
abstract out the functionality from the window system even further.
That's sort of tough because most of this is higly interactive at a
level that's more intricate than pushing buttons.  But still, it can
be done.

It would be slick if this were ported to Tk, in which case xmorph
would run on other window systems sort of automatically.

----

I want to change to change the user interface to a mode-based sort of
thing where only one mouse button is used to do everything.
There are 5 mesh manipulation functions:

  Pick up a mesh point and drag it to another place
  Add a horizontal mesh line
  Add a vertical mesh line
  Delete a horizontal mesh line
  Delete a vertical mesh line

What I would probably do is create a panel of mode buttons that
selected one of the above modes.  Then the cursor would reflect which
mode you were in.  That would elliminate the need to press and hold
the Shift key while using the mouse.  That's akward.  Also, as it is
now, you have to remember which mouse buttons do what.  With a
mode-driven interface, you'd be reminded by the mode buttons and the
cursor shape, as in many paint programs.

The current interface has one cool feature that would be lacking in
the mode-driven method:  All of the mesh manipulations are a bunch of
Actions, which can be re-mapped by the user to be associated with
different events.  In a mode-driven interface, re-mapping would not
be possible to the same extent.  But on the other hand, why would
someone want to remap mesh manipulation events?

I also intend to support a wider range of image file types, including
JPEG, PPM, and TIFF.  That ought to be easy enough to do.

If anybody is interested in helping out on these things, please let me
know.
