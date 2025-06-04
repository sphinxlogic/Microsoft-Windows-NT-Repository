
			     XGalaga v1.3

		    Copyright (c) 1995 Joe Rumsey
			  <ogre@netcom.com>

  This is the first game I've written for X from "scratch" (the
graphics interface is borrowed, I wrote everything else), but is by no
means my first game.  I have written several popular shareware games
for the Amiga, and plan to start porting them to X.  This project is
to help me get used to X and to fine tune the graphics routines to
make the next project easier.

  I am releasing this as SHAREWARE.  I ask that if you enjoy it and
want to encourage me to write more games for X (and I have a whole
slew of Amiga games much better than this waiting to be ported)  you
send me the paltry sum of FIVE DOLLARS.  Whether or not you send me
the dough, you are free to distribute this archive provided all files
are left intact, and no fees are charged other than a reasonable
amount for the cost of distributing it (floppy disk, CD-rom, etc.)
Note that x11window.c and mkimgsrc are covered by their own copyrights.

  The address to send your bribe for future games to is:

	Joe Rumsey
	1122 N. Towner St.
	Santa Ana, CA 92703

That's $5 in United States money.  US Checks and Money Orders are
fine.  The Post Office recommends that you not send cash, but I don't
care, cash is fine too.

  My email address is ogre@netcom.com.

See the file INSTALL for installation instructions.

  Note that XGalaga is a major X server hog.  I doubt it will run
well over a network.  Run it on the same machine as your display if at
all possible.  On my 486DX/66 w/ accelerated graphics, the X server
uses every available cycle to keep up, but the game still runs at
nearly 30 frames/sec in buffered mode (and in non-buffered mode would
run much faster than that if it weren't being purposely held to 30 fps)

Command line options:
  -display <display>   Set your display
  -mouse               Use mouse control (same as 'm' at the title screen)
  -keyboard	       Use keyboard control (same as 'k')
  -nosound             Turn sound off on platforms with sound support
  -b                   turn buffered mode off, use this if it runs
                       too slowly.  Will cause flicker, maybe lots,
                       maybe only a little.
  <anything else>      print usage and version.

Basic instructions:
  It's Galaga, you know how to play Galaga, stop bothering me.
  (Ship follows the mouse, button fires.  Auto-fire by holding it
   down, so no-one accuses me of breaking their mouse!)

Keyboard commands:

  p - pauses
  q - end this game
  b - Toggle buffering (flicker vs. speed.)
  o - Toggle OR mode.  Don't use this, it sucks.
  s - toggle sound if your system supports it
  m - Turn mouse control on     (also works to start the game)
  k - Turn keyboard controls on (also works to start the game)
      Controls: left_arrow  - move left
		right_arrow - move right
		space_bar   - fire

Credits:
   Galaga code: 
            Joe Rumsey (ogre@netcom.com)
   X interface:
            Bill Dyess(dyessww@eng.auburn.edu), 
            Rob Forsman(thoth@cis.ufl.edu), 
            and about a million others.

   Sound server/interface:
	    Sujal Patel (smpatel@wam.umd.edu)

   Pixmaps: 
            Joe Rumsey (ogre@netcom.com)
            Mike McGrath (mcgrath@epx.cis.umn.edu)

  I (Joe) am the only one who's done anything specifically for
Galaga, everything else is stolen shamelessly from the Paradise full
color netrek client.
