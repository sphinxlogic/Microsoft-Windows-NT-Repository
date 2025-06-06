Spider is a challenging double decked solitaire game.  Unlike most
soilitaire games, skill is just as important as luck.

The X11 version of spider is based on the NeWS version by Don Woods,
and uses the same save file format and card images.

The game has three interfaces:  raw Xlib, Athena widget and XView.  The
Athena interface was built with the R4 toolkit, and may not work with
earlier versions.

Installation:

1. Decide which interfaces you wish to build, and set up the Imakefile
	appropriately.

2. Set the HELPDIR directory for 'make install' to
	work correctly, and the help files to be found.

3a. If you plan to run on a small display (less than ~900 pixels wide),
	define SMALL_CARDS.  These aren't as pretty, but its a lot
	better than having to move the window while playing.  (if your
	display is less than ~500 pixels, you're SOL unless you want
	to make some smaller cards.)

3b. If you have a wider display, define ROUND_CARDS if you want it to look 
	prettier, at the cost of a considerable amount of performance
	(although its still quite playable on a reasonable server).

4. Type 'xmkmf' to build the Makefile, and type 'make'.

If you are compiling multiple versions, be sure to do a "make clean"
before doing a "make" of the next version.  This will avoid mixing
modules which have been compiled with different flags.

Spider has been tested on the following:

Hosts:
	Sun 3 & 4 running 4.0.x, 4.1.x
	Solbourne 5 running 4.0x
	Decstation 3100 Ultrix 3.1
	Decstation 3100 running OSF/1
	Textronix 4319 running UTek 4.1
	Silicon Graphics running Irix 5.2

    partially:
	DG AViiON 5100 running DG/UX 4.20
	(kitless only, and no usleep() since it doesn't have setitimer(2))

Displays:
	NCD  X terminals
	Labtam  X terminals
	Sun 3 & 4 running MIT X11R4
	Sun 3 & 4 running X11/NeWS
	Decstation 3100, running UWS 2.2
	Decstation 3100, running MIT X11R4
	Textronix 4319, running MIT X11R4

Please send any bugs or comments to:

Dave Lemke
lemke@ncd.com

I'd especially like to see new interfaces to it, particularly
non-Xt based toolkits like InterViews, OI or Andrew.

Note on XView version:

The supplied XView code will compile with either XView 1.0.1 (which is
included on the X11R4 distribution) or XView 2.0.

Before running the XView version of the tool, set the environment variable
HELPDIR to the location of the spider.info file so spot help will work.
For XView 2.0, HELPDIR becomes HELPPATH which works similarly to MANPATH.

The XView version also has two extra command line args:  -name and
-resource_file.  These allow you to have instances of the Spider tool
which resources apply to and allow one to use a different resource
file than the default.  The default resource file for the XView version
is $HOME/.Xdefaults.  The spider.info file has more information on 
resources when you prompt it for spot help.

Please send any bugs or comments about the XView front end to
xviewbugs@Eng.Sun.COM
