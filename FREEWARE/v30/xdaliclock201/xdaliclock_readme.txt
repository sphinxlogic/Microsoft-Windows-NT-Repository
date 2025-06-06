The xdaliclock program displays a digital clock; when a digit changes, it
"melts" into its new shape.

It can display in 12 or 24 hour modes, and displays the date when a mouse
button is held down.  It has two large fonts built into it, but it can animate
most other fonts that contain all of the digits.  It can also do some funky
psychedelic colormap cycling, and can use the "shape" extension so that the
window is shaped like the digits.

In the default configuration, this program is very network-intensive.  If it
seems sluggish, try the "-memory high" option (see the man page).  It will
take a little longer to start up, but will then go easy on the bandwidth.

This program was inspired by the Alto/Macintosh programs of the same name,
written by Steve Capps some time in the early eighties.  This implementation
is Copyright (c) 1991, 1992, 1993, 1994 by Jamie Zawinski <jwz@lucid.com>.
Please let me know if you find any bugs or make any improvements.

You can get the latest version by anonymous FTP to lucid.com or ftp.x.org.

To build:

  -  read the comments in `Imakefile' and edit DEFINES as appropriate
  -  xmkmf ; make
  -  make install install.man

(If you don't have xmkmf, which are a standard part of X, then your vendor is
doing you a disservice.  Report it as a bug.  However, you may be able to limp
along using Makefile.simple.)
