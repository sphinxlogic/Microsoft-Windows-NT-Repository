 XDim was developed 1994/95 by W. Benzing,

	benzing@iegi01.etec.uni-karlsruhe.de

 Special thanks to :

	Herwig Van Marck <Herwig.VanMarck@elis.rug.ac.be>
	Richard Allen <ra@os.is>

 who really helped me hunting bugs and to all who volunteered.


 XDim is a Motif application, which allows you to visualize
 2D-Datafields.
 XDim is free of copyright which means, that you can use XDim or any
 part of it to any purpose you can think of,

 BUT IN THE GIF AND JPEG SUBDIRECTORIES ARE FILES INCLUDED THAT ARE
 OF COPYRIGHT SO PLEASE READ THEM BEFORE USE
 
 In the julia subdirectory there is an example application how
 to write a program with a data interface suitable to xdim.

 STANDARD DISCLAIMER:

 XDIM IS NOT GUARANTEED TO BE OF ANY USE AT ALL. QUITE IN CONTRARY
 THE USE OF THIS PROGRAM AND/OR ANY INCLUDED FILE MAY LEAD TO
 CORRUPTED DATA AND/OR DISKS.
 BY USING THIS PROGRAM YOU ACCEPT THE RISKS AND AGREE NOT MAKE THE
 AUTHOR LIABLE FOR ANY DAMAGE DONE TO YOUR HARDWARE, SOFTWARE,
 DATA ETC.(INCLUDING LIFE, UNIVERSE AND EVERYTHING).
 IN COUNTRIES WHERE THIS EXCLUSION OF LIABILITY IS NOT ALLOWED
 THE USE OF XDIM IS FORBIDDEN BY THE AUTHOR.

 See also const.h to change some system dependent values

 W. Benzing 1994
 
 How to Install:

 System requirements:

 1. Standard UNIX system with ANSI C-compiler (gcc recommended)
 2. X11 from R4
 3. Motif from 1.? (tested with 1.3 and 1.2)

 Steps to Installation

 1. Unpack the file with compress -d
 2. Untar the files with tar -xf
 3. A subdirectory will be created named xdim2.x
 4. cd into this directory
 5. Only an Imakefile for XDim is delivered:
    check the Imakefile for system dependent options
    xmkmf => Makefile is created
 6. Make dependencies (only necessary if you want to change
    the source code):
    make depend
 7. Create XDim:
    make
 8. Install the application default file XDim somewhere it is found
    from Xt, p.ex. /usr/lib/X11/appl-defaults. If you don't have the
    priviliges, you can use the environment variable XAPPLRESDIR:
    setenv XAPPLRESDIR .
    will set the current directory into the search path
 9. start xdim (./xdim), example files are available in the
    ./examples subdirectory, example GIF's, JPEG's are available
    in the JPEG subdirectory. tutorial.txt gives hints what to
    do if using xdim for the first time. Also take a look at
    the help provided, either online or by reading help.h .
 10. Install xdim in your execution search path, p.ex. /usr/local/bin/X11
 11. READ tutorial.txt (it's short, do it !) for some hints on how to use
    XDim. Although there is online help available, some things are not as
    obvious as I wanted them to be.
 12. XDim now supports 24 bit graphic displays, BUT there is no atomic
    operation for 24 bits, so expect this to be very slow.

Some hints for the usage, testers reported XDim to be very slow in redraw.
Just to defend myself the following:

-On my machine (the smallest alpha AXP) the hidden line algorithm can easily
 produce about 25 000 (4 point) polygons per second, this is doubled on our
 larger alpha. Meaning that small data sets like bfield.xd can be rotated
 etc. in real time.
-The image procssing (meaning hidden line etc.) is done on your main processor
 WITHOUT any assistance of X11 (This is due to some severe limitations in X11).
 When this image is built it is sent to your display via X11, but as the
 hardware of this display is not known (it may even be on an other 
 computer over the net) X11 will process the image data in a way that it
 can be displayed. This process may take quite a time.

There are two possible reasons for this:


1. To get maximum perfomance the internal pixel size should be the same as the
   pixel size of the display you are on, use -DD16 or -DD32 option to set 16
   or 32 bit. In this case the image is sent directly via X11 without color
   translation. This color translation is necessary to display 8bit images
   on 32 bit displays and vice versa.
2. Your X11 is configured badly, some machines will use the net sockets even
   if displayed on the same computer.(See in comp.windows.X11 FAQ about this).
   Don't use XDim over the net, although possible it will sent net traffic sky 
   high. If you want use it over the net anyway, please note that XDim
   allows to set up TWO different redraw types, a so called fast redraw and
   a normal redraw, please set up the fast redraw to a redraw mode that uses
   X11 functions like Lines or Grid.



    HAVE FUN,
    
    Walter Benzing

    p.s. If you like the program you might want to put it on your favourite
         ftp site.
