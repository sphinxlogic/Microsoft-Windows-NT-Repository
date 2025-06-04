XLI116, MOTIF_TOYS, Load images into an X11 window or onto the root window

WHAT IS IT?

xli is a version of xloadimage.

This utility will view several types of images under X11, or load
images onto the X11 root window. xli can also be used on some (32 bit)
MSDOS systems.  The current version (1.16) supports:

	CMU Window Manager raster files
	Faces Project images
	Fuzzy Bitmap (.fbm) images
	GEM bit images
	GIF images (Including GIF89a compatibility)
	G3 FAX images
	JFIF style jpeg images
	McIDAS areafiles
	MacPaint images
->	Windows, OS/2 BMP Image
	Monochrome PC Paintbrush (.pcx) images
->	Photograph on CD Image
	Portable Bitmap (.pbm, .pgm, .ppm) images
	Sun monochrome rasterfiles
	Sun color RGB rasterfiles
	Targa (.tga) files
	Utah Raster Toolkit (.rle) files
	X pixmap (.xpm) files (Version 1, 2C and 3)
	X10 bitmap files
	X11 bitmap files
	X Window Dump (except TrueColor and DirectColor)

A variety of options are available to modify images prior to viewing.
These options include clipping, dithering, depth reduction, zoom
(either X or Y axis independently or both at once), brightening or
darkening, input gamma correction, and image merging.  When applicable,
these options are done automatically (eg. a color image to be displayed
on a monochrome screen will be dithered automatically). A utility (xlito)
is provided that allows these viewing options to be appended to the image
files.

