

/*
 * Copyright 1990 Jeff S. Young
 */

This is version 2 of the game xmahjongg.  It has been almost totally rewrit-
ten.  I have run this program on SUN systems (3 and 4) and Michael Campanella
has provided VMS help for xmahjongg.

Xmahjongg has 2 new features which will make life enjoyable:
	1) we now have COLOR (well its about time)
	2) we now have different board layouts
See the man page for information on these features

To get the game up quickly, you only need to do the following:
	1) in xmahjongg.mk change the variable BIN to a directory where
	   the executable will reside.
	2) in xmahjongg.mk change the variable TMP to a directory where
	   the layout files (default, bridge, and wedges) will reside.
	3) cat x.bdf.1 x.bdf.2 x.bdf.3 > xmahjongg.bdf
	4) create a font out of the xmahjongg.bdf file (see below)
	5) make sure that the directory called TMP exists
	6) run make -f xmahjongg.mk install
	7) play a game

To create a font you need to convert the bdf file to an snf file (on SUNs
at least) or a pcf file (on DECstations).  To do this you need to run bdftosnf
or bdftopcf.  If you don't have these programs, look for an archive site that
does.  Next run a mkfontdir after moving the snf or pcf file into the correct
font directory for your site.  This will give you a usable font.  Remember that
you need to reboot your server or do an "xset +fp font_path" and a
"xset fp rehash".  For those of you without in depth knowledge, the new snf
file must be in a directory with other snf files only or in a directory by
itself.  This will allow the mkfontdir program to correctly make the font
database for the given directory.

The layout files are a generalization of the board layout.  The default is
the standard "pyramid".  You can create you own layouts.  See the man page
on how to do this.

I would appreciate hearing about new layouts and any bugs which are found.
I will post a patch when enough data comes in.

Remember that Mark A. Holm has the original copyright on the look of the tiles.
See his README file which is included below.

Enjoy!

Jeff S. Young
jsy@cray.com

--------------------------------------------------------------------------------

MAHJONGG Version 1.0

Copyright 1988 Mark A. Holm

Permission is given to copy and distribute for non-profit purposes.

This game will operate on any Sun hardware that I know of with either
a monochrome or a color bit mapped tube running Sun 3.2 UNIX or later.
Most of the development was done on a Sun 100U running Sun 3.5.

Please Note!! The automagic screen type determination will not recognize
all screen types unless compiled with <sys/fbio.h> from a 3.5 OS. If
compiled with 3.2 not all monochrome types are recognized.

INSTALLATION INSTRUCTIONS:

1.	Unpack this kit in an empty directory and edit the Makefile to
	set MANLOC and DESTDIR to the appropriate locations.

2.	run 'make install'.  

I have distributed the tiles as a compressed .o file to save on file
sizes and to preserve the artistic license of the game. If you would
like the source to the icons and the patches to iconedit so you can
modify them send a request (and possibly a small donation ;^) to the 
address below.

If you have any questions or problems/bug reports, send them to me:

Mark A. Holm
tektronix!tessi!exc!markh
