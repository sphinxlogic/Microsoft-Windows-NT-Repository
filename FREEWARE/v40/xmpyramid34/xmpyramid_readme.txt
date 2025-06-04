	MOTIF PYRAMID
	==============	Yet Another Widget on X-Motif, V3.4


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
    Click on the right mouse button if you want to randomize the pyramid.
      Also 'R' or 'r' does the same thing.
    'Q', 'q', Control-C to kill program.

    NOTE If you have the program up but it does not seem to work:
    Try copying the file Pyramid to $(HOME)/Pyramid (where
    Pyramid is the file name) and running xmpyramid again.
    "keyboardFocusPolicy" must be set to "pointer". Also if the
    background is white the yellow is hard to see.

    In the graphics window there are 2 boxes, each box is a view of
    the pyramid (actually its a tetrahedron). There are 2 faces of the
    pyramid in each box.

    Clicking on the mouse icons is ignored. They are just there to
    tell you how to use the pyramid.

    One has to orient the faces in orient mode, besides getting all the
    faces to be the same color. I do not think that adds any complexity
    on the pyramid.

    Try resizing the pyramid. Notice the puzzle resizes to take advantage
    of the "room" available.

    Key pad is defined as:
        8   9
        ^
    4 < 5 > 6
        v
    1   2

    The key pad along with the use of the mouse will allow you to move the
    pyramid more than just CCW ie.,
    		KP_8=>TOP	KP_9=>TR
     KP_4=>LEFT	KP_5=>CW	KP_6=>RIGHT
     KP_1=>BL	KP_2=>BOTTOM

    The shift keys will allow one to access "Period 3" turns from "Both"
    mode, otherwise it assumes "Period 2" turning.
    For "Period 3" motion, the "tetras" along the diagonals are mapped
    differently from the keyboard due to the fact that the perspective of
    direction changes. Here a move towards TOP, the opposite is RIGHT and a
    move towards BOTTOM, the opposite is LEFT.
    "Restricted" and "Period 2" turning allows only the edges to turn,
    and the 2 center rows turn together. It is as if the middle cut of the
    three cuts did not exist.
    "Restricted" and "Period 3" turning allows only the faces to turn,
    it is as if the middle cut of the three cuts did not exist.
    Beware, the "Restricted" mode is a hack and much could be done to
    improve its look.

    If you were looking for a self-solver, sorry. If you know of one let
    me know. One of the problems in self-solvers the generally don't work
    for different number of cuts.

    Personally I have solved 0-2 in the "Period 2" mode,  0-3 in the
    "Period 3" mode, 0-1 in the "Both" mode, and "Restricted" I only
    solved it in "Period 3".

    Keep in mind that the Pyramid is a widget that can be simply stuck in
    other Motif programs. (If you find a use for this, let me know).

    Refer to the man page for detailed command line options.

FOUND A BUG?

    Send bugs (or their reports, or fixes) to the author (please give
    version, file, and line number):

	David Albert Bagley,	bagleyd@source.asset.com

HISTORY

   [Sep 16, 93]	V3.4: Counter reset to "0" on a mode change
   [Sep 09, 93]	V3.3: Fixed up the restricted mode (although, its still
		cryptic). Records now have a more concise format. 
   [Sep 03, 93]	V3.2: R.K.Lloyd@csc.liv.ac.uk found a bug in the destroy
		function. I inherited that bug from Young's dial widget.
   [Aug 31, 93]	V3.1: Finally polished the pyramid. Added Imakefile,
		README, xmpyramid.man. I got some good ideas on 
                presentation from Qiang Zhao's tetris.
   [Apr 01, 93]	V3.0: I got some good ideas from Douglas A. Young's
		book: "The X Window System Programming and Applications
		with Xt OSF/Motif Edition", particularly his dial
		widget.
   [Jan 29, 92]	V2.0: XView version. Sun drops another quality product.
		So sad, so sad.
   [Mar 19, 91]	V1.0: SunView version.
