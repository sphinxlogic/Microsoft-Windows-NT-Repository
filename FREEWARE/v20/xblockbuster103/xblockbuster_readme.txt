DESCRIPTION:

XBlockbuster is a variation of the break-out type games, only a bit more
complex in the type of bricks it has and effects the bricks have.  It should
run on any X11 system (color or black and white).

RESTRICTIONS:
	This game may be freely copied or distributed as long as no fee
is required.  Since the code is a mixture of both the original and my
own, I make no claims on it.  However, if you do use it for something
else, credit would by nice.

TO BUILD:

    If you are on a System V machine, -DSYSV will need to be added to the
    DEFINES variable.

    If XBlockBuster is seems slow, or is using a lot of network bandwidth,
    try defining (uncommenting) the MOTION_HINTS line near the top
    of xblockbuster.h. (see CHANGES file for more information.)

    XBlockbuster can use the same playground that Blockbuster (the sunview
    version) does.  If you want to use the same playground, set 
    StageDir (Imakefile) or LIBDIR (Makefile.noIm) appropriately.

  IMAKEFILE directions:

    If you have already installed the stages file for xblockbuster, and
    don't want to copy over them, change InstallLib in Imakefile to No.

    The Imakefile, as distributed, should work fine (unless on System V, 
    see above).  You may still want to edit the Imakefile to change where the
    files will be installed.  Once you have edited the Imakefile to your
    satisfaction, all that is needed is:

	1) xmkmf
	2) make
	3) make install

  MAKEFILE directions:

    If you don't want to use the Imakefile, do the following:
	1) cp Makefile.noIm Makefile
	2) Edit the Makefile.  Everything should be fairly well documented.
	3) make

	If you have already installed the XBlockBuster stages, and don't
	want to copy over them, do:

	4) make install.bin

	If this is your first installation, or you want to copy over the
	score files, do:

	4) make install

    I have successfully compiled the source with both cc and gcc on
    a sun 3/sunos 4.1.1.

IF YOU HAVE PROBLEMS:

	If it can't find the font, change FONT in xblockbuster.h

	If your on system V/Ultrix machine, did you add -DSYSV to to
	defines?

	If you get link errors of random or srandom not found, change RAND
	and SRAND in xblockbuster.h to comparable functions for your
	machine (and please send me mail, so I can make this more automatic
	in the future.)  Note that RAND and SRAND are only used for
	determining what the order of the stages are, so they need not
	be especially good.

	Check the BUGS/TODO section to see if it is a problem I
	already know about.  If it is, still send me mail, but maybe
	by it being a known problem, you won't feel quite so bad.

	Other problems?  Send me mail. (see below)

FILES (of interest):

CHANGES: changes made from blockbuster to xblockbuster.

RATIONAL.SV, README.SV: Files from the original SunView version that may be
	of interest to people.

XBlockbuster.ad: X Defaults I use for coloring the bricks.
	NOTE: xblockbuster will not use these values if they are installed
	in the standard app-defaults directory.  They MUST be read in
	by xrdb, either by being in the user .Xdefaults file or by
	perform xrdb on XBlockbuster.ad file itself.

icons (directory):  Contains the brick images, as well as a few other.  Brick
	images with _alt in the name are the alternate images used for color.
	(See the manual page for more information on coloring the bricks and
	alternate bricks.)

BUGS/TODO:

 A nice opening screen, showing the bricks (with proper coloring, if 
applicable), along with descriptions of what each brick does.

 Fix some of the remaining bugs (most are quite minor, or happen very
infrequently)

 Use a setuid mechanism so that the save directory and score files don't
need to be publicly writable.

 Make it so the xblockbuster will read in the XBlockbuster.ad file if
it is stored in the standard place.

 Fix the BUGS as listed in the man page.

NOTES:
	I have personally verified it works on the following:

	sun 3/60, sunos 4.1.1 (both color and black & white)

	I have gotten reports it works on the following:

	SGI with IRIS 4.0.x


	I will maintain it and release patches as required. 

	If you send me mail, please include a valid mail address as
part of your signature.  I have gotten at least a few letters where
the mail address in the header was incorrect.  Also, please be
sure to state the version you are using (this should appear
in both the window header, and the file patchlevel.h)

Mark Wedel
master@cats.ucsc.edu
March 17, 1993

