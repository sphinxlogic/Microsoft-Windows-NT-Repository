/* #ident	"@(#)x11:contrib/clients/xloadimage/README 6.21 94/07/29 Labtam" */
XLI - X11 Image Loading Utility

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

CHANGES FROM XLI 1.15

    Added BMP (MS Windows, OS/2) file format loader.

    Added PCD (PhotoCD) file format loader.

    Bowed to the pressure of the number of JPEGs that are gamma
    corrected. JPEG images now are assumed to have a gamma of 2.2.

    Fixed bug in gif loader (files with local colormaps came out black)

    Improved the quality of color quantization.

    Added an -expand option that forces the image to be expanded
    to 24 bit.

    Added a -title option.

    Added interactive image rotation using the 'l' and 'r' keys.

    Numerous bug fixes.

   (See the patches file for more details and credits)

HOW IS XLI RELATED TO XLOADIMAGE ?

    xli version 1.00 was based on xloadimage version 3.01.
    xli version 1.16 has many improvements over xli 1.00. 
    
    xloadimage is maintained by Jim Frost - jimf@saber.com

    xli is maintained by Graeme Gill - graeme@labtam.oz.au

    Please read the README.orig file, which is the original xloadimage README
    file for xloadimage info and credits.

WHERE CAN I GET IT ?

	Two files:
	    xli.README
	    xli.1.16.tar.gz
	on
	    ftp.x.org
	in
	    /contrib/applications, and mirrors of this archive.

	Note that ftp.x.org will recompress as .Z on the fly.

COMPILING

    There are a variety of ways to compile xli, depending on what
    environment you have.

    The easiest way of compiling it is to use the Imakefile. Simply
    run 'xmkmf' (which should be in your systems /usr/bin/X11 or
    somewhere equivalent), which should make a Makefile suitable
    for your system. Type 'make' to then build xli. It can be
    installed in the standard place (along with its aliases xview
    and xsetbg) by typing 'make install'. The manual entry can
    be installed using 'make install.man'.

    If using the Imakefile is not possible, then you can try using
    Makefile.std. Choose your target from 

	std			BSD environment
	sysv			SYSV environment
	gcc			BSD using gcc
	sysv-gcc		SYSV using older gcc

    and use something like:

	make -f Makefile.std target

    Makefile.std can also be used to re-make the Imakefile, Makefile.dos,
    the tar archive, or a DOS ready .zip archive.

    There are a few compilation flags that you may need/want to change.
    This can be done in the Makefile.std file. The defines are:

	-DHAVE_GUNZIP if you want to use gunzip rather than uncompress on .Z files
	-DNO_UNCOMPRESS if you system doesn't have uncompress or gunzip.
    The JPEG code needs a compilation flag that tells it whether right
    shift (>>) is signed or unsigned on your machine.  If it's unsigned, add
	-DRIGHT_SHIFT_IS_UNSIGNED 

    The defines should be added to the MISC_DEFINES= line in the Makefile.std
    file.  After doing this you may need to re-bulild the Imakefile or
    Makefile.dos if you are using them.  You can do this using:

	make -f Makefile.std Imakefile
    or
	make -f Makefile.std Makefile.dos

COMPILING FOR DOS USING DJGPP 1.11

    xli can be used on 32 bit 80x86 systems running MSDOS if
    compiled using djgpp version 1.11. Since it has to run without
    the aid of a windowing system or window manager, the user interface
    leaves a lot to be desired, but it does work. It is important to
    check that your video card is supported by the djgpp go32 environment,
    and that it is configured appropriately.

    Assuming that you have unpacked the tar archive on a UNIX system,
    and have the zip utility in your path, the first step is to
    create xli.zip:

    make -f Makefile.std zip

	This can then be transfered to the MSDOS system and unpacked
    in a suitable directory using unzip or pkunzip. Assuming the djgpp
    compiling environment is set up suitably, xli can be compiled using
    ndmake:

    ndmake -f Makefile.dos

    xlito now seems to work with djgpp 1.11 (it didn't
	under version 1.09)

    xli can then be run as usual.

    For details on setting up djgpp, please read the djgpp docs.

    There is an optional compile flag -DDO_ARG_EXP that can be added
    to the MISC_DEFINES in Makefile.std, or to DEFINES in Makefile.dos
    that turns on argument expansion processing within xli. This
    is somewhat more powerful than that provided by go32, but to get
    it to work fully you will have to turn off the go32 argument expansion
    by setting the appropriate go32 options.

    Please note that this DOS release probably has several undiscovered
    bugs, as it has not been as extensively tested as the X11 version.

OTHER NOTES

    The file build-jpeg is intended to aid those who want to port
    the latest Independent JPEG Group's JPEG software to xli.
    It semi-automatically creates the jpeglib.h and jpeglib.c files
    used by xli. Note that future versions of the jpeg library
    may be require updating of the xli specific jpeg.h and jpeg.c
    files. Note that one change was made to the file jmemnobs.c,
    "methods" was renamed to "smethods" so as to avoid clashing
    with the variable of the same name in jmemmgr.c

    Please also note that the JPEG library README file is included at
    the top of jpeglib.c, and gives distribution and copyright
    information for this library.

PRAISE, SUGGESTIONS AND BUG REPORTS

    xli has been developed in a fairly limited hardware environment, and 
    not all combinations of displays and/or image formats and options
    have been tested. xli is useful to me, and maybe it will be
    useful to you. 

    Praise, suggestions, and bug reports should go to:

	Graeme Gill
	graeme@labtam.oz.au

   [I may not be able to respond promptly to mail during the period
    94/7/30 to 94/8/15]

