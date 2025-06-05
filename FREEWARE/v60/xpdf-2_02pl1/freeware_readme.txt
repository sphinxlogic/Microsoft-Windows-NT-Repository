XPDF202PL1, UTILITIES, X-Windows based viewer for PDF files

    This is XPDF V2.02pl1.  This packages contains the source code
    necessary to build the following programs:

    XPDF	- displays Portable Document Files (PDF) (sometimes called 
    		  Adobe Acrobat files) on an X-Windows display.  Can also
    		  print PDF files by converting them to postscript and
    		  sending them to a postscript printer.
    PDFFONTS	- displays information about the fonts referenced in a
    		  PDF file.
    PDFIMAGES	- saves images from a PDF file as Portable Pixmap (PPM), 
    		  Portable Bitmap (PBM), or JPEG files.
    PDFINFO	- prints the contents of the 'Info' dictionary (plus some 
    		  other useful information) from a PDF file.
    PDFTOPBM	- converts PDF files to black-and-white image files in 
    		  Portable Bitmap (PBM) format.
    PDFTOPS	- converts PDF files to PostScript so they can be printed.
    PDFTOTXT	- converts PDF files to plain text.


    You will need the following tools in order to unpack and build this
    package:

    UNZIP		(you must use the -V switch when unzipping)
    DEC C compiler
    DEC C++ compiler
    Either MMS or MMK.

    If you do not have all of these tools you will need to get and
    install them before you can build XPDF.

    This package includes all the other packages necessary to build XPDF. 
    You must build the other packages first and then build XPDF. 
    Instructions are as follows (note that all directory references are
    relative to the directory where you unpack the kit):

    First, unpack the distribution kit:

    $ unzip XPDF-2_02PL1.ZIP

    Then, build as follows:

    FREETYPE

    $ Set Default to [.FREETYPE-2_1_3]
    $ MMS		(or MMK if you have that, instead).

    T1LIB

    $ Set Default to [.T1LIB-1_3]
    $ @CONFIGURE
    $ @MAKE
    $!
    $! N.B. T1LIB includes a standalone executable, called XGLYPH, that
    $! is used to view the glyphs from a font file.  This program
    $! requires XAW3D, which I have not included here.  You will get a
    $! note indicating that XGLYPH will not be built.  You may ignore
    $! this.  XGLYPH is not used by XPDF.

    LIBXPM

    $ Set Default to [.XPM-3_4K.LIB]
    $ @MAKE		(will invoke either MMK or MMS)

    XPDF

    $ Set Default to [.XPDF-2_02PL1]
    $ @VMS_MAKE OPI_SUPPORT	(see VMS_MAKE.COM for other options)
    $!
    $! After this build procedure completes, all of the executables will
    $! be found in [.XPDF-2_02PL1.XPDF].


    I have included all of the object files, object libraries, and
    executables that resulted when I built this on my system.  These
    executables were linked on an Alpha running OpenVMS V7.3-1 and
    DECWindows V1.2-6.  If your version of VMS is recent enough, these
    may work for you without having to rebuild the entire kit.  Also,
    every file I had to edit to make this package work and build
    correctly can be found as filename;2 (filename;1 is the original
    version).

    Mark Berryman

