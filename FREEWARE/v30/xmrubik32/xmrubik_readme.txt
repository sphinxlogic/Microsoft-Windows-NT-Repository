	MOTIF RUBIK'S CUBE
	==============	Yet Another Widget on X-Motif, V3.2


    It has been tested on the following platforms:

	HP 300			HP-UX 8.0x	X11R4 Motif1.1
	Sun4, SPARC		SunOS 4.1.x	X11R4 Motif1.2
	 For Solaris you may have to add to link:
	 -L/usr/ucblib -lucb -lelf -R/usr/ucblib

    If yours is not in this list, please let me know -- thanks. The
    most updated source could be found on ftp.x.org under /contrib.

HOW TO BUILD?

    It should be easy.

	xmkmf
	make

    Note: if you don't have 'xmkmf' or the "Makefile" it generates
    doesn't work well, try:
        make -f Makefile.std

ANYTHING SPECIAL IN USING?

    Click on the left mouse button to move CCW.
    Click on the middle mouse button toggle the practice mode. (This is
      good for learning moves). Also 'P' or 'p' does the same thing.
    Click on the right mouse button if you want to randomize the cube.
      Also 'R' or 'r' does the same thing.
    'Q', 'q', Control-C to kill program.

    NOTE If you have the program up but it does not seem to work:
    Try copying the file Rubik to $(HOME)/Rubik (where
    Rubik is the file name) and running xmrubik again.
    "keyboardFocusPolicy" must be set to "pointer". Also if the
    background is white, the white and yellow are hard to see.

    Clicking on the mouse icons is ignored. They are just there to
    tell you how to use the cube.

    One has to orient the faces in orient mode, besides getting all the
    faces to be the same color. To do this one has to get the lines to
    be oriented in the same direction, this only matters with center
    cubes (ie. those cubes not on a corner or edge). This does add
    complexity so there are 2 sets of records.

    Try resizing the cube. Notice the puzzle resizes to take advantage
    of the "room" available.

    Key pad is defined for 2d rubik as:
        8
        ^
    4 < 5 > 6
        v
        2

    The key pad along with the use of the mouse will allow you to move the
    cube more than just CCW ie.,
		KP_8=>TOP
    KP=>4	KP_5=>CW	KP_6=>RIGHT
   		KP_2=>BOTTOM

    Key pad for 3d rubik, use your intuition (is this a cop out or what?).
    The key pad is defined differently depending on which side of the cube
    your mouse is pointing at. One thing that stays the same is KP_5=>CW 

    If you were looking for a self-solver, sorry. If you know of one let
    me know. One of the problems in self-solvers the generally don't work
    for different number of cuts.

    Also a tesseract (4D cube) would be nice. (This has been built in
    software and solved.  Actually, it was done for N-D and solved.)

    Personally I have solved 0-2 using this program, 1-4 using the
    physical cubes, and 5-6 to my knowledge have never been done (be the
    first on your block).

    Refer to the man page for detailed command line options.

FOUND A BUG?

    Send bugs (or their reports, or fixes) to the author (please give
    version, file, and line number):

	David Albert Bagley,	bagleyd@source.asset.com

HISTORY

   [Sep 16, 93]	V3.2: Bug fixes, too many with 3.1 (oops)
		1. when cube was not in orient mode one still had to
		orient the middle cubes,
		2. when one put the cube in practice mode and switched
		to the other representation it would not let you
		continue,
		3. counter is not consistent if using both
		representations
   [Sep 09, 93]	V3.1: Finally polished the cube. Added Imakefile,
		README, xmrubik.man. I got some good ideas on
		presentation from Qiang Zhao's tetris.
   [May 20, 93]	V3.0: I got some good ideas from Douglas A. Young's
		book: "The X Window System Programming and Applications
		with Xt OSF/Motif Edition", particularly his dial
		widget.
   [Jan 16, 92]	V2.0: XView version, Sun drops another quality product.
		So sad, so sad.
   [Jan 16, 91]	V1.0: SunView version.
