README file for XSpringies 1.1                                    March 1, 1993
==============================

General info:
-------------
  XSpringies is a mass and spring simulation system.  It's intended use is more
like that of a game, than some design package.

  It's written using Xlib only.  No Motif or any other widgets sets are used.

  The animation in XSpringies is done using an off-screen Pixmap.  The next
frame is drawn on this pixmap, then is blitted onto the screen.  Since the
frame rate is about 30 frames per second, slower machines (or machines which
have poorly written bit-blitting code) will be deathly slow and blinky.

Installation info:
------------------
First, edit the Imakefile/Makefile.std and change the value of DDIR to reflect
where you want to install the xspringies files.  Don't forget the trailing /
on the end of your filename.

If you are using the Imakefile, type "xmkmf".

    If you have problems at the beginning, and you are using the
    Imakefile, it might be caused by the strange definition I have for
    "MKDIRHIER", which I had to redefine because of the silly way Imake
    is set up.  If this happens, type "which mkdirhier" to see what the
    full path for mkdirhier is, and put this in place of the backquoted
    expression in the Imakefile. (i.e. - if the full path is
    "/usr/local/bin/mkdirhier", then you change the line to
    "MKDIRHIER = BourneShell /usr/local/bin/mkdirhier").

Then, type "make install".

Then, just run xspringies and have fun.

Updates
-------
For version 1.1:
 Bugs fixed:
 - Compiles for NEWS system
 - Keyboard input now works under openlook window manager
 - Animation no longer gets 'stuck' after like 35 minutes
 - File format is more compact by removing unnecessary zeros
   (i.e. - before 1.500000, now 1.5) and has more precision to eliminate
   tiny errors perceived when saving and reloading a file
 - Strange ~-expansion behavior for filenames is gone
 - The GO! button replaces the hard to find Action checkbox
 - A Delete button was added, which is the same as the Delete key

 New features:
 - Spring thickness can be specified using the "-st <number>" command
   line switch (0 is default width - thin line)
 - Instead of the entire screen being bit-blitted, only the bounding box
   necessary to draw new objects and cover old objects is drawn.  This
   greatly speeds up the animation when drawing small objects (such as
   a "jello").  If the bounding box changes size periodically (as in the
   "pendbees" file), the "-nbb" command line switch disables this feature
   to allow for slower, but smoother animation.
 - A cursor, and filename editing features were added.  It uses emacs-like
   keys:
      ^B/^F = back/forward
      ^A/^E = goto beginning/end of line
      ^K/^Y = kill to end of line/yank from kill buffer
      ^D    = delete character under cursor
      ^U    = kill all input
      ^T    = transpose character under cursor with previous character
      ESC   = exit from filename edit mode

Misc info:
----------
  The author of xspringies is Douglas DeCarlo.
  Email address:  dmd@gradient.cis.upenn.edu
  Please send any bug-reports/comments/suggestions/demo-files to this address.
