			 XPaint 2.1 Release


Welcome to the release of XPaint 2.1!  So, what does XPaint do for
you?  Why many wonderful things of course.  XPaint is a reasonably
versatile bitmap/pixmap editing tool.  XPaint came out of a problem
editing some very large bitmaps, and wanting more functionality then
was provided by bitmap. After XPaint 1.0 sat around for a LONG time, I
finally ended up with a strong need to edit color pixmaps.  Presto,
this is what has appeared from that problem.

The short features list:
	Usual paint operations:
		Brushes, Spray paint, Pencil
		Lines, Arcs, Pattern Fill, Text,
		Boxes, Circles, Polygons.
	Works on multiple images simultaneously.
	Cut/Copy/Paste between all active images.
	Fatbits/Zoom on the image windows.

XPaint is slightly biased towards a MacPaint-like operation set, since
this is the only paint program that I have ever used for more than 30
seconds, but not to let this discourage you, it does quite a bit more.
If you find some functionality missing and feel like adding it, there
is a rough description of how the operations interface works, and I'd
be more that willing to help you.

The following file formats are supported:
	X11 Bitmaps	reading	writing
	PPM Format	reading writing
	GIF Format	reading writing
	PostScript		writing
	XPM Format	reading writing
	TIFF Format	reading writing (condinally compiled in)
	SGI RGB Format	reading writing (only on SGI machines)
If you have an image that is not in one of the supported formats I'd
reccomend you take a look at the PBMPLUS toolkit (which is availbe
for ftp from export.lcs.mit.edu, and other fine sites).

XPaint is written using ANSI C (for the most part), and thus you need
to have a compiler that understands function prototypes.  On most MIPS
based machines this is the standard compiler, on a Sun SparcStation you
will either need to use 'gcc' or 'acc' (from Sun).  If you have some
other brand of workstation, try using 'gcc' if your compiler doesn't
work.

Hope you find this useful, 
David Koblas 
koblas@netcom.com
