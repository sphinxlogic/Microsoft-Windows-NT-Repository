XSCREENSAVER, MOTIF TOYS, Screen saver-locker with many graphics demos

         XSCREENSAVER V 1.24.2 

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

