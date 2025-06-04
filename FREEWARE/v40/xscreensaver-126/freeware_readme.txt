XSCREENSAVER, MOTIF_TOYS, X Window Screen Saver and Locker -126


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

The XIDLE or MIT-SCREEN-SAVER server extensions will be used if you have them.

Unfortunately, locking doesn't work if you don't have Motif.

Also included are several graphics hacks for use as screensavers.  There's
nothing magic about these: they're just programs that draw on the root
window, which are pointed at by the screensaver's default resource settings.

   qix		- My own implementation of this, with many more options
		  than you would have thought qix could have.
   helix	- Generates spirally "stringart" patterns.
   pedal	- Draws a different kind of spirally pattern.
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
   lmorph	- morphing line drawings.

The latest version of xscreensaver is always ftpable from ftp.x.org.  You can
also get it from my web page at http://www.netscape.com/people/jwz/.

	-- Jamie Zawinski <jwz@netscape.com>

OpenVMS port of Xscreensavser version 1.26     December 1995
==========================================

Xscreensaver distribution can be found in 3 subdirectories:

[.DRIVER]   The Xscreensaver and Xscreensaver-command programs.
[.HACKS]    Graphic demos ,can be run either through the xscreensaver program 
            or standalone.
[.UTILS]    A small libraries of various utilities.

All objets, libraries and exes files are provided for VAX VMS 5.5-2 (compiled
with VAX C 3.2 / Motif 1.1) and AXP VMS 6.1 (compiled with DEC C 5.0 / Motif
1.2). 

If you wan to rebuild, you need to rebuild [.UTILS] directory first and
re-create the object library (look at the end of COMPILE*.COM procedure).

You can now rebuild the [.HACKS] directory and the [.DRIVER] directory.

WARNING : before rebuilding [.HACKS], you need to correct some of DECwindows 
bitmap files. Some files are bogus !! (they have a long line of null chars 
at the end). These files are under SYS$COMMON:[DECW$INCLUDE.BITMAPS] directory:

STIPPLE.XBM
HLINES2.XBM
LIGHT_GRAY.XBM
ROOT_WEAVES.XBM
VLINES2.XBM

These files are all used by Greynetic demo.

Nota: link procedure automagically select appropriate X and Motif Libraries
(X11R4/Motif 1.1 - X11R4/Motif 1.2). 

The SETUP.COM procedure gives you a definition of all DCL foreign command
symbols needed to run Xscreensaver and all the graphic hacks. You need to
modify this procedure if you install these programs in another directory tree.

VMS .HLP files are provied for all programms of this distribution.

You can easily add new graphic demos without recompiling Xscreensaver. You just
need to add them in resource file XSCREENSAVER.DAT. This file (originally
present in [.DRIVER] directory ) can be installed under your SYS$LOGIN 
directory for a per-user customization basis. You can also install it under 
the system-wide user resource directory SYS$COMMON:[DECW$DEFAULT.USER] 
(with (W:RE) protections). The new graphics hack must be run in root-window 
mode to be accepted by Xscreensaver.

The graphic demos are spawn into subprocess created by the system() call (in
the Unix version the execve() call is used).

The VMS passord checking programs were picked up in the Xlock distribution.

Many thanks to Martin P.J. Zinser for many portability improvements to this
port (look at XSCREEN_MARTIN.TXT for a list of changes).

Thanks to Grégoire Sandré (sandre@clamart.wireline.slb.com) for a patch
concerning FileVue X Messages.

Aug 96 : Mods to compile with DEC C 5.2

Enjoy,

Patrick MOREAU - CENA/Athis-Mons - FRANCE  (pmoreau@cena.dgac.fr)
                                           (moreau_p@decus.decus.fr)
