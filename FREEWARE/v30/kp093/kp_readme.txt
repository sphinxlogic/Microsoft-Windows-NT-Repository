This is the source distribution kit of Keyboard Practicer (KP).  You
need Motif (ver 1.1 or higher) to compile this package.

Kp shows a keyboard and practice text.  If you hit the right keys, the
cursor advances.  The keys you hit appear to be pushed in for
amusement.  (This is the primary reason why I chose Motif.)

It also shows the next key you have to hit in reverse-video so that
you don't have to watch your fingers (I know from experience that it
is hard not to watch your fingers).  After you get rid of the habit of
watching your fingers, you can turn off the next-key showing to
perfect your typing.

Kp has a "filter" feature which will let you select the keys you want
to practice.  When this is enabled, only words entirely consisting of
those characters are displayed.  This will make the sentences
meaningless, but it's just practice and you don't have to comprehend
what the practice text is saying.

Kp is compiled and tested (sort of) on HP9000/720 running HP-UX 8.05
(gcc 2.3.2 and native cc), DECstation 5000 running Ultrix 4.2 (gcc
2.5.8), 486 PC running FreeBSD 1.1 (gcc 2.4.5, XF86_S3, Orchid F1280+
VLB), SPARCstation2 running SunOS 4.1.1 (native cc) and Sun3 running
SunOS 4.1.1 (gcc 2.3.1), although Sun3 had much less CPU power than
necessary to run this program in meaningful speed.

To compile, "xmkmf", "make depend" and "make".  If the compiler
complains that it can't find include files or the library for Xm,
either you don't have Motif on your system, or include files/libXm are
not in the standard places and the imake configuration files on your
site is not set up properly.

If you do have Motif, try using the line with -L<dir> in Imakefile
where <dir> is the directory where libXm is installed.  You also might
have to add STD_INCLUDES = -I<dir> where <dir> is the parent directory
of where the Motif header files are installed (i.e., the directory
which has Xm as a subdirectory).  If it still doesn't work, please let
me know.

This version comes with a bunch of xmodmap files if you want to try
the Dvorak keyboard on several machines.  Take a look at the xmodmaps/
subdirectory.  Note that the qwerty versions are not very well tested,
and may leave some keys un-remapped!

Please report any bugs, suggestions, appraisals (if any), complaints,
improvements, etc., to the author:

	Satoshi Asami

	571 Evans Hall
	Computer Science Division
	Department of EECS
	University of California
	Berkeley, CA 94720
	United States of America

	asami@cs.berkeley.edu
	asami@rabbit.is.s.u-tokyo.ac.jp


This program is copyrighted (1992, 1993, 1994) by Satoshi Asami.  This
is an alpha release, and you can use, modify and redistribute it
without any fee, subject to these conditions:

(1) you like the program,

(2) you are not USL/AT&T or work for them, and

(3) in case of redistribution, it is accompanied by this README file
    as well as the source


I would like to thank May Cheng, Peter Klier, Ritsuko Nakamura,
Tetsuya Ikeda, Kaoru Maeda, Ken Wakita, David Brookler and Tim
Callahan for their kind and useful suggestions.  Special thanks go to
Larry Rowe, who taught an excellent course of user interface which
stimulated the author's interest in this program.

#	See version.h for version and release date.
