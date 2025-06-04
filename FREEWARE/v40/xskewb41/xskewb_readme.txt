	X-BASED SKEWB
	=============	Skewb X widgets, V4.1


    It has been tested on the following platforms:

	HP 3xx & 700		HP-UX 9.0x	X11R5
	Sun4, SPARC		SunOS 4.1.3	X11R4
	PC, 486 (*)		Linux 1.0	X11R5

    *	Keypad 5 key may not work, in which case, try it with the
	Num-Lock on.

    If yours is not in this list, please let me know -- thanks. The
    most updated source could be found on ftp.x.org under
    /contrib/games/puzzles.

HOW TO BUILD?

    It should be easy. Edit the Imakefile (or Makefile.std, see below)
    for appropriate settings of variable SCOREFILE, RANDOM, then do:
	xmkmf
	make
	xskewb

    If that does not work then:
	mv Imakefile Imakefile.r5
	sed 's/^XCOMM/\/\*\*\/#/' > Imakefile < Imakefile.r5
	xmkmf
	make
	xskewb

    Note: if you don't have 'xmkmf' or the "Makefile" it generates
    doesn't work well, edit Makefile.std for appropriate settings for
    XINCLUDEPATH and XLIBPATH, then try:
	make -f Makefile.std

    You should have Skewb.ad copied to $(HOME)/Skewb or 
    /usr/lib/X11/app-defaults/Skewb to run, especially if the background
    is similar to one of the faces. Edit this file for your preferences.
    You might want to move xskewb into /usr/bin/X11 (or wherever your
    X binaries are).
    You might also want to move xskewb.man to /usr/man/man6/xskewb.6

ANYTHING SPECIAL IN USING?

    Click on the left mouse button to move CCW.
    Click on the middle mouse button toggle the practice mode. (This is
      good for learning moves).  Also 'P' or 'p' does the same thing.
      One must double click on the middle mouse button if the puzzle is
      being worked on.
    Click on the right mouse button if you want to randomize the cube.
      Also 'R' or 'r' does the same thing.  One must double click on
      the right mouse button if the puzzle is being worked on.
    'O' or 'o' to toggle orient mode. (I prefer the orient mode since
      it is slightly more challenging).
    'Q', 'q', Control-C to kill program.

    Key pad is defined for 2d skewb as:
    7       9
        ^
      < 5 >
        v
    1       3

    The key pad along with the use of the mouse will allow you to move the
    cube more than just CCW ie.,
    KP_7=>TopLeft                       KP_9=>TopRight
			KP_5=>CW
    KP_1=>BottomLeft                    KP_3=>BottomRight

    If the mouse is on a diamond, the above "KP" keys will not move cube
    because the move is ambiguous. Also if the mouse is on a triangle, not
    all the "KP" keys will function because the puzzle will only rotate on
    the cuts, ie. a triangle with a TL-BR cut will rotate only TL & BR, a
    triangle with a TR-BL cut will rotate only TR & BL. Therefore, a
    triangle can only move tangential to the center of the face. No doubt
    this is confusing, but the physical skewb is the same way. In fact,
    that is part of its appeal.

    Key pad for 3d skewb, use your intuition (is this a cop out or what?).
    The key pad is defined differently depending on which side of the cube
    your mouse is pointing at. Again, the cube can not turn pointing at
    center diamonds. One thing that stays the same is KP_5=>CW.

    The new control feature allows you to move the whole cube at once
    without being entered as a move.  Hold down the control key while
    using the left mouse button or the keypad.
 
    One has to orient the faces in orient mode, besides getting all the
    faces to be the same color. To do this one has to get the lines to
    be oriented in the same direction, this only matters with center
    diamond. This does add complexity so there are 2 sets of records.

    Try resizing the cube. Notice the puzzle resizes to take advantage
    of the "room" available.

    The title is in the following format:
	xskewb{2|3}d: (<Number of moves>/\
		{<Record number of moves>|NEVER|PRACTICE}) - <Comment>
    {2|3}: current dimensional view
    {<Record...}: puzzle is either in practice mode or record mode
       In record mode, if there is no record of the current puzzle, it
       displays "NEVER".

    If you were looking for a self-solver, sorry.  If you know of one
    let me know.

    Also a tesseract (4D cube) would be nice. (Does such an analog exist
    for 4d?)

    Personally, I have solved this puzzle, but only physically, never in
    software. The biggest handicap is that the puzzle is hard to follow
    through the diagonals. But the real one is like that too.

    Recently, Disney released Mickey's Challenge, its a spherical skewb
    with a pretty good internal mechanism. It also comes with a pretty
    neat book. The whole package is only about $10.
     
    Refer to the man page for detailed command line options.

    If you want further information on puzzles, I'll be glad :-) to send
    it to you.

FOUND A BUG?

    Send bugs reports and fixes to the author. For bugs, please include
    as much information as to their location as possible, since I may not
    have the resources to duplicate the them.

	David Albert Bagley,	bagleyd@source.asset.com

HISTORY

   [Jun 15, 94]	V4.2: Mistakenly used a Boolean variable in an array.
   [Jun 02, 94]	V4.1: R6.
   [May 30, 94]	V4.0: Xt version.
		I got some good ideas from oclock.
   [Oct 14, 93]	V3.0: Motif version.
		I got some good ideas from Douglas A. Young's
		book: "The X Window System Programming and Applications
		with Xt OSF/Motif Edition", particularly his dial widget.
		I got some good ideas on presentation from Q. Zhao's
		tetris.
   [Jan 22, 92]	V2.0: XView version.
   [Mar 19, 91]	V1.0: SunView version.
