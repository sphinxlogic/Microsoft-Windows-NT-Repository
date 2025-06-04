XMORPH-12JUL96, GRAPHICS, Digital image warping (morphing) for X window system

Written and Copyright (C) 1994, 1995, 1996 by Michael J. Gourlay

NO WARRANTEES, EXPRESS OR IMPLIED.

Xmorph is a digital image warping program, also known as a "morphing"
program.  It runs under the X Window System, using the X library, X
toolkit, X miscellaneous utilities, and the Athena widgets, all of
which are publically available from MIT, and part of the typical X
installation.  It is written in ANSI C.

Xmorph loads, saves, warps, and dissolves images, and loads, saves,
creates, and manipulates control meshes which determine the warping.
Xmorph has help pages built into it, so no external documentation is
necessary.

---

FURTHER READING:

Morphing was invented and first used by Industrial Light and Magic.
The original author of the first morphing algorithym is Douglas B.
Smythe.  If you can get ahold of the article, read Douglas B.
Smythe's article ``A Two-Pass Mesh Warping Algorithm for Object
Transformation and Image Interpolation'', ILM Technical Memo #1030,
Computer Graphics Department, Lucasfilm Ltd., 1990.

The first commercial use of morphing was in a sequence in the movie
_Willow_.  Since then, morphing has been wildely used.  Among the
more memorable morphing sequences are those found in Michael
Jackson's "Black or White" video, and in the movie _T2_.

Be sure to read George Wolberg's _Digital Image Warping_.  I've
corresponded with George Wolberg about this program.  I asked him
whether he considered xmorph to be a violation of copyright of the
algorithms in his book, since there are similarities.  Mr. Wolberg
assured me that my algorithms were different enough that there was no
problem, and he seemed very interested and enthused about the
existence of my public domain implementation.  Also, the algorithms
published in Mr. Wolberg's book had bugs in them which I and other
xmorph contributors have found, and those bugs have been reported to
George Wolberg, who verified my corrections to be proper.  I was also
told that these bugs were propagated on to Lucasfilm, although I've
heard from no one at Lucasfilm directly.

---

DISPLAY HARDWARE:

Xmorph is designed to work on static gray 1-bit, pseudo-color 8-bit,
and true-color 24-bit displays.  If you are running on a static gray
or pseudo-color display, the images that show up in the xmorph image
panels are dithered (which makes them look grainy), but the images
stored internally are full-color non-dithered, and the image files
written by xmorph are full-color non-dithered.  Don't let the
relatively poor image quality of the dithered image panels concern
you.  They are only "schematic" images to let you manipulate the
meshes.  Remember that the xmorph graphical user interface is
primarily used to manipulate meshes, not to display images.  To view
your morphs in full glory requires that you make an animation of your
image sequence.

A note about color dithering:

I've made the color dithering routines somewhat generic, in the sense
that you can choose the number of bits per channel that you want to
use when creating the so-called "dithering colormap".  Look in
diw_map.h.  You can change the number of bits used to represent each of
the channels red, green, and blue.  It's generally believed that the
human eye is moste sensative to green, and least sensative to blue, so
it makes sense to give more bits to green and fewer to blue.  However,
one could imagine that somebody could prefer to give more bits to
another color, or whatever you want.

Also, for 16-bit colormapped displays (which are becoming more common
now), you could conceivably use up 8 bits or more for your colormap
without worrying about interference with other programs.  It'd
recommend using a RGB-332 dithering colormap in that case.

Another thing:  Some monochrome displays simulate PseudoColor by
allowing the applications to have a colormap.  This makes application
programming more simple.  The X server then has the responsibility of
dithering from color to monochrome.  Some such monochrome systems
have a 4-bit PseudoColor simulation mode which could conceivably be
used with xmorph, if some changes were made in the dither_image
routine in image.c.  The colormap situation is easy to deal with:
Just use 1 bit per channel to get an RGB-111 dithering colormap.

---

TARGA IMAGE FILE FORMAT:

I've been asked by many people what image file format xmorph uses.  I
use Targa.  I intend to use others in the future, but for now Targa
works well because it's sufficiently flexible to use a number of
storage schemes (including colormapped, 24-bit true color, and 32-bit
alpha true-color, and gray scale) and it provides for some
compression using a form of run-length encoding, which is modified to
minimize the worst-case expansion.

Right now, Xmorph only reads and writes Targa image files.  There is
a PBMplus (and netpbm) utility to convert Targas to and from other
formats.  Also, Art Department Pro's Professional Conversion Pack
(which runs on Amiga systems) has a Targa loader and saver.  Targa
files are also viewable from Image Magick utilities, but they have to
have a .tga extension to them for some reason (probably because Targa
files don't have a magic number in their header identifying them as
Targas.)

---

CREATING MOTION ANIMATIONS:

I've been asked about programs which convert these images to
animations.  You can use PBMplus or netpbm or ImageMagick to convert
Targa to other image formats.  Use mpeg_encode from Berkeley to
create MPEG animations.  Also see Andy Thaler's WWW home page for
links to some relavent programs.

----====####====----

WORLD WIDE WEB SITES:

For xmorph animations see Andy Thaller's WWW home page at

http://www.informatik.tu-muenchen.de/cgi-bin/nph-gateway/hphalle2/~thaller

The old one was at this address:
http://www.informatik.tu-muenchen.de/cgi-bin/nph-gateway/hphalle8/~thaller

---

Please report to me if you have animations that are cool that you'd
like other people to see.


Xmorph 12 Jul 1996 OpenVMS port :  July 1996
===============================

Patrick Moreau

pmoreau@cena.dgac.fr
moreau_p@decus.fr
