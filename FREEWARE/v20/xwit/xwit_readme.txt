xwit ("x window interface tool") is a hotch-potch collection of simple
routines to call some of those X11 functions that don't already have any
utility commands built around them.  The reasoning behind this is that
loosely every X function should be accessible from a shell script.

For example, XWarpPointer() will move the X pointer, but no utility program
exists to do so on those rare occasions when you could really use it.  xwit
will also resize, iconify, pop, and move windows given by name or id,
change an icon, title or name, set the screen saver going, and change
individual key autorepeat settings.

The program is released into the public domain.  Only the considerate
will leave credit for the authors.

	Mark M Martin. mmm@cetia.fr  dec 1993.
	David DiGiacomo dd@mv.us.adobe.com
