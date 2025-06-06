
This is the latest version of xlock (sccs version 22.4)

This will start with patchlevel = 1.1

There were serveral minor bugfixes made after I put this on expo, but before
it was posted to comp.sources.x

   o -display hostname:0 did not work even when hostname was the localhost.
   o -mode qix failed with a count < 4.
   o qix was using rand() shifted right some amount to get some accuracy.
     I switched to random() and srandom() and the performance is still too
     fast to worry about measuring... (at least on OpenWindows on a 4/60)
   o I was coughing on EnterNotify and LeaveNotify, now I ignore them.
   o The window manager was sometimes installing the default colormap when
     the mouse crossed a window boundry *after* I had installed my colormap,
     but *before* I had my window up to block crossing events.  I moved the
     colormap installation down to the very last moment.
   o a couple of gratuitous non-portable functions were used.
	- strtok() replaced with simple strchr() and a null store.
	- srand48() replaced with srandom();
	- usleep() this is still used in life.c, but people can just rip it
	out if they don't have it, or stick in a loop polling timeofday, or
	use VTALRM, but usleep works nice and cleanly under SunOS, so I'm
	not really motivated to change it.

----------------------------------------------------------------------

    There is a minimal Makefile which you may need to massage in order
    to get it to generate the right thing from the Imakefile.  Neither
    of these has been tested since we don't use Imake.

Changes since last version (1.8):

    Ability to set the nice level of the xlock process, to keep it from
    hogging all of the cpu.

    -display is parsed to be able to lock unix:1, etc...

    new modal interface to different lockscreen screensavers.

    three sample modes.
	-mode qix is the old spinning lines demo...
	-mode life is a new implementation of Conway's Life simulation.
	-mode hop is the "hopalong fractals" from last version.

    lots of bug fixes and code cleanup.

    ______________________________________________________________________
    Patrick J. Naughton				    ARPA: naughton@sun.com
    Window Systems Group			    UUCP: ...!sun!naughton
    Sun Microsystems, Inc.			    AT&T: (415) 336 - 1080
