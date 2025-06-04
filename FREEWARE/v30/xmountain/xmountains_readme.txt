Xmountains is a X11 based fractal landscape generator. It generates
random fractal surfaces and displays them in a window. While the
program is running the fractal is continuously extended on the right
and the image is scrolled from left to right to expose the newly
generated area.



./xmountains: version 1.1
usage: ./xmountains -[bxmslrftISTRapcegd]
 -b       [false] use root window 
 -x       [false] flat start 
 -m       [false] print map 
 -s       [true] toggle smoothing 
 -l int   [9] # levels of recursion 
 -t int   [1] # non fractal iterations 
 -r int   [20] # columns before scrolling 
 -R int   [0] rng seed, read clock if 0 
 -f float [0.650000] fractal dimension 
 -I float [0.4500000] angle of light 
 -S float [0.600000] vertical stretch 
 -T float [0.500000] vertical shift 
 -a float [2.500000] altitude of viewpoint 
 -p float [4.000000] distance of viewpoint 
 -c float [1.000000] contrast
 -e float [0.500000] ambient light level
 -g string     window geometry
 -d string     display

This program works best on a colour display. It will work on monochrome
displays but the default parameter values are not optimal for this.
Try using [-e 0.1] it may help.


The program breaks down into three sections
1) Fractal generation (calcalt.c random.c crinkle.h) This code should be
   fairly solid, It also contains some algorithmic tricks I had to
   invent (though it is not inconceivable that somebody else has also
   thought of them)
2) Image rendering & main program (artist.c global.c scroll.c global.h
   paint.h) This code is a bit more sloppy but seems to work OK.
3) The X interface (X_graphics.c) The X interface was retro-fitted to an
   earlier version of the program that was designed to use memory mapped
   video hardware. As a result this code is a little crude. A more 
   experienced X hacker could do better but ...

PROBLEMS
 All parameters are set on the command line.
 There is now way to control the speed of the program, it runs as fast
as the CPU and the X server let it.
 The choice of colours looks strange on some hardware.


If you have any comments/fixes etc for this program 
Email me at
 S.Booth@ed.ac.uk    or 
 spb@castle.ed.ac.uk

Copyright 1994 Stephen Booth, see copyright.h




