FRACTALS - July 1989.
--------- -----------

Author:		Paul Sharpe @ DEC, Reading, England.
Inspiration:	Title:		"The Science Of Fractal Images."
		Editors:	Heinz-Otto Peitgen, Dietmar Saupe
		Published:	Springer-Verlag

>>> Copyright (c) Digital Equipment Corporation 1990  All rights reserved.
>>> Copyright is claimed in the computer program and user interface thereof.

>>> Digital Equipment Corporation cannot accept any responsibility for
>>> use, misuse, or abuse of this software.

>>> The code is provided as-is. No guarantee is made to provide support or
>>> maintenance.
>>> This said, the code may be used in any way, on the provisos:
>>> 1 - The author and the authors company can not accept any responsibilty
>>>     for the use/misuse/abuse of the software.
>>> 2 - The credits/copyrights at the top of the files remain.
>>> Any suggestions/improvements/bug-reports will be at least read.

This code was written under Ultrix(c) on a Vaxstation II GPX, with 8-plane
colour, using DECwindows. It should work on 'any' UNIX(c), with X11
derivatives and with 1 or more colour planes...

It has been tested on a DECuVax II GPX (8-plane), a DS3100 (8-plane)
and a Sun 3/50 (1-plane)
It requires the X11 library, though probably not necessarily release 3.

To compile, edit the Makefile and configure the compiler options: in
particular, change the paths to find the bitmaps for the icon and the
background.
Then just type 'make'.



Fractal mountains in 3D, by brownian motion.
--------------------------------------------

It was mainly written with a colour capacity in mind, but allows the colours
on the command line to be set for monochrome dispays:

e.g. for monochrome: xmntn -ba black -mn black -se black)

Use 'xmntn -help' for a full list of options.

There is a simple interaction to allow new generations of mountain data without
having to quit and restart: simply click any mouse button in the pointer window
at the point along the scale for the dimension. The scale is graduated at the
integer values 1.00 and 2.00, and the scale allows for values entered up to
3.00. Clicking on the 'New Mountains' window will cause new data to be
generated and the mountains drawn. On a lowly GPX, this takes 30 seconds or
so for each phase. On a MIPS box, its INSTANT!

There is an alternative perspective to the mountains shown by the executable
'xlmntn'. This attempts to bring the viewer down to the level of the
mountains. It takes many of the same options, but also allows for the distance
of the observer to be changed.

There are lots of arguments, but 'sensible' defaults are provided that allow
you to just run the program. Most options are self-explanatory: others are
not really to be changed. The general idea is to just play with them!

xmntn|xlmntn
	-display display-string	(default: "")
	-foreground colour	(default: white)
	-background colour	(default: red)
	-textfg colour		(default: white)
	-textbg colour 		(default: blue)
	-seacolour  colour	(default: blue)
	-mntncolour colour	(default: forestgreen)
	-numcolours integer	(default: 50)
	-colour r,g,b		(default: g)		<i.e. any of r,g,b>
	-fontfile fontfile	(default: )
	-H double		(default: 0.9)
	-scale double		(default: 2.5)
	-level integer		(default: 7)

	-mntndistance integer	(default: 100)		<xlmntn only>
	-scrndistance integer	(default: 500)		<xlmntn only>
	-height integer		(default: 120)		<xlmntn only>

	-xscale integer		(default: 4)		<xmntn only>
	-yscale integer		(default: 2)		<xmntn only>

Restrictions/Things To Do:
--------------------------
o Each program, when using full colour, installs a local colourmap for its
main window: this has the 'disturbing' effect of changing the colours of
existing applications. This is only for the lifetime of these fractal
programs, and only while they are currently in focus... This should
negociate/cooperate with the window-manager...

o Small level numbers (<4) cause there to be not enough width for the
'buttons' and 'labels', which then overlap. They should try harder at
fitting, or should be re-sized, or re-prioritised... Maybe the font
should be made smaller...

o There should be a visible indication of when any of the 'buttons' are
clicked upon...

o Better behaviour on having areas exposed: redrawing the entire region
is not a good idea on slow systems.

Any problems to sharpe@wessex.co.uk
