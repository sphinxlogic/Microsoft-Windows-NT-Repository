
To build:

  -  read the comments in `config.h' and edit it as appropriate
  -  xmkmf ; make
  -  make install install.man

The xscreensaver program waits until the keyboard and mouse have been idle
for a period, and then runs a graphics demo chosen at random.  It turns off
as soon as there is any mouse or keyboard activity.

The purpose of xscreensaver is to display pretty pictures on your screen 
when it is not in use, in keeping with the philosophy that unattended 
monitors should always be doing something interesting, just like they do 
in the movies.

However, xscreensaver can also be used as a screen locker, to prevent
others from using your terminal while your are away.

The benefit that this program has over the combination of the xlock and
xautolock programs is the ease with which new graphics hacks can be
installed: you don't need to recompile this program to add a new display
mode, you just change some resource settings.  Any program which can be
invoked in such a way that it draws on the root window of the screen can
now be used as a screensaver without modification.  The programs that
are being run as screensavers don't need to have any special knowledge
about what it means to be a screensaver.

The XIdle extension will be used if you have it (win win.)

Unfortunately, locking doesn't work if you don't have Motif.

Also included are several graphics hacks for use as screensavers.  There's
nothing magic about these: they're just programs that draw on the root
window, which are pointed at by the screensaver's default resource settings.

   qix		- My own implementation of this, with many more options
		  than you would have thought qix could have.
   helix	- Generates spirally "stringart" patterns.
   rorschach	- Random inkblot patterns.
   attraction	- A bouncing ball demo, or a qix-like demo, or a wild
		  color-cycling thing, with some odd rules.
   greynetic	- Random colored/stippled rectangles.
   rocks	- Flying through an asteroid field.
   blitspin	- Rotate a bitmap using bitblts.
   imsmap	- Generates random maps or cloud formations.
   hypercube	- 2d projection of a hypercube rotating on all four axes.
   slidescreen	- Divides the screen into a grid and plays a 16-puzzle on it.
   decayscreen	- A melting effect.
   halo		- Random circular patterns.
   pyro		- Fireworks.  Looks a lot like the version in xlock.
   hopalong	- Fractals.  I snarfed this code from xlock.
   flame	- Fractals.  Also from xlock.
   noseguy	- A guy with a big nose wanders around the screen saying
		  things.  I snarfed this code from xnlock.
   maze		- This is the X maze demo modified to take a -root option
		  so that it works with xscreensaver.

All of these will pop up their own window unless given that -root option.
See their man pages for more details.

Other reasonable things to use as screensavers, if you have them, are

  xdaliclock -root -builtin2	- melting digital clock
  xswarm -r 2>&-		- swimming sperm
  xwave -root			- random 3d graphs
  xbouncebits			- bounce arbitrary bitmaps around
  ico -r			- it's dull, but it's there
  xv -root file.gif -quit	- they don't all have to animate!
  xsplinefun			- bouncing splines
  kaleid -root			- qix-like kaleidescope patterns
  xfishtank -c black -d -r 1	- fish (use version 2.0 or later)

You can get all of these from the contrib directory on ftp.x.org.  If you
know of (or write) any other interesting programs that can be used as
screensavers, please let me know!

This program should be updated to take advantage of the new R6 `screensaver'
server extension instead of XIdle.  I didn't have time to do this before the
deadline for the R6 contrib tape, but by the time you read this, maybe it's
done - check a later release.

The latest version of xscreensaver is on FTP.X.ORG.

	-- Jamie Zawinski <jwz@mcom.com>


Changes since 1.23:	Fixed some private-colormap oddities in slidescreen,
			decayscreen, and xroger.  Fixed apparent conservation-
			of-mass problem in pyro; made the shrapnel round.
Changes since 1.22:	Minor tweaks for IRIX5; fixed locking race condition.
Changes since 1.21:	Minor tweaks for X11R6.
			Fixes for non-default visuals.
Changes since 1.20:	Fixed bug in color blitspin; added default image.
			Added diagnostics to noseguy.  Fixed off-by-one
			error in flame.  Added some missing casts.
Changes since 1.18:	Added `flame' hack.
			Fixed a minor Motif dialog text field bug.
			Fixed yet another XPointer-not-defined-in-R4 bug.
Changes since 1.17:	Added support for shadow password files.
			Fixed some Motif-related locking bugs.
			Added diagnostics when locking is disabled.
			Made blitspin able to use the XPM library.
			Added `decayscreen' hack.
Changes since 1.16:	Added `halo' hack.
Changes since 1.15:	Portability fixes.
Changes since 1.14:	Broke the driver up into more source files.
			Moved the hacks into their own directory.
			Made all `time' parameters accept the 00:00:00 syntax,
			so that even the parameters which are normally read as
			`minutes' can be specified in seconds.
			Added colormap cycling to `imsmap'.
			Made hyper work with K&R compilers.
Changes since 1.13:	Added `orbit' option to `attraction' hack.
			Added `lock-timeout' option.
			Cleaned up options of `maze' hack.
Changes since 1.8:	Added demo mode, and locking.
			Added `maze' hack.
			Added `norotate' option to `rocks' hack.
