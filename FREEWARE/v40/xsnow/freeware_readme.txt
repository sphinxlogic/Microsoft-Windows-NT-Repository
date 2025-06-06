XSNOW, MOTIF_TOYS, Create a snowy desktop

This distribution contains a file 'make.com' to build Xsnow on
VMS systems, and 'xsnow.hlp'. To build type '@make' in the
xsnow directory. Another make.com is make2.com 

XSNOW

  Create a snowy and Santa-y desktop

                  xsnow [-option .,..]

  Xsnow displays lovely moving snowflakes on your desktop.


OPTIONS

    -display display_name
 Drop the snowflakes on the given display. Make sure the display is
 nearby, so you can hear them enjoy...

    -snowflakes num_snowflakes
 This is the number of snowflakes. Default is 100, max is 1000.

    -sc snowflake_color
 Use the given string as the color for the flakes instead of the default "snow".

    -bg background_color
 Use the given string as the color for the background. Note that the
 usual default desktop pattern consisting of 50% white 50% black doesn't 
 particularly look good with Xsnow. 

    -tc tree_color
 Use the given string as the color for the trees. 

    -slc sleigh_color
 Use the given string as the color for the sleigh. Note that the default
 color is black, so not much of Santa will be seen on a black
 background.

    -santa santa_size
 There are 3 sizes of Santa: 0, 1 and 2. Default is 1.

    -santaspeed santa_speed
 The speed Santa should not exceed if he doesn't want to get fined.  The
 default speed for Santa size 0 is 1, for Santa 1 it's 2 and for  Big
 Santa it's 4.

    -delay delay
 This is the number of milliseconds delay after updating everything.
 Default is 50 milliseconds, i.e. 20 updates per second max.

    -unsmooth
 If you specify this option the snowflakes will 'whirl' more
 dramatically, resulting in a somewhat jerkier movement.

    -whirl
 This sets the whirl factor, i.e. the maximum adjustment of the
 horizontal  speed. The default value is 4.

    -nowind 
 Default it gets windy now and then. If you prefer it quiet specify
 -nowind.

    -windtimer period
 With -windtimer you can specify how often it gets windy. It's sort of a
 period in seconds, default value is 30.

   -xspeed -yspeed
 These options set the maximum horizontal and vertical speed. The
 default X maximum speed is 4, the default maximum Y speed is 8.

    -wsnowdepth -ssnowdepth
 This sets the maximum thickness of the snow on top of windows and at
 the bottom of the display respectively. The default snowdepth for 
 windows is 8, at the bottom of the screen the default is 50.

    -notrees 
 Do not display the trees.

    -nosanta
 Do not display Santa Claus running all over the screen.

    -norudolf
 No Rudolf.

    -nokeepsnow
 Do not have snow sticking anywhere.

    -nokeepsnowonwindows
 Do not keep snow on top of the windows.

    -nokeepsnowonscreen
 Do not keep snow at the bottom of the screen.

    -version
 Prints the current Xsnow version.


EXAMPLES

    $ xsnow -bg LightSkyBlue3 -sc snow  
 Sets the background to a bluish color and lets it snow white.

    $ xsnow -ssnowdepth 100
 Starts with a thin layer of snow that gradually builds up at the bottom 
 of the screen.

    $ xsnow -santa 2 -santaspeed 10
 Gives you the biggest Santa at a speed that is hardly legal.

    $ xsnow -delay 100 -notrees
 For slow systems use longer delay and don't draw the trees.

    $ xsnow -snowflakes 1000 -delay 0
 Uses the maximum number of snowflakes and runs as fast as possible.

COPYRIGHT
 Copyright 1984, 1988, 1990, 1993 by Rick Jansen (rick@sara.nl)

 Some code borrowed from xroach by J.T. Anderson (jta@locus.com)

 Xsnow uses vroot.h for use with virtual window managers.  vroot.h is
 copyright 1991 by Andreas Stolcke, copyright 1990 by Solbourne Computer
 Inc. (stolcke@ICSI.Berkeley.EDU)

 The big Santa with the nice moving antlers and reins  was made by
 Holger Veit (Holger.Veit@gmd.de).

 The idea and code for wind are from Eiichi TAZOE
 (tazoe@sfc.keio.cp.jp).

AUTHORS
 Rick Jansen (rick@sara.nl)

NOTES
 Xsnow itself doen't use very much CPU time, but it can load the X
 server and/or network quite substantially. Use less snowflakes and a
 bigger delay in such a case. On a standalone workstation there won't be
 any problem. 

