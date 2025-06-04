XPM-34E, GRAPHICS, XPM V 3.4E library (X PixMap Library)


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

