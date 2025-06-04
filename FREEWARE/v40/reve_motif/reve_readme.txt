
/*  @(#)README 1.23 94/01/27
 *
 *  Copyright (C) 1990 - 1994 - Rich Burridge & Yves Gallot.
 *  All rights reserved.
 *
 *  Permission is granted to copy this source, for redistribution
 *  in source form only, provided the news headers in "substantially
 *  unaltered format" are retained, the introductory messages are not
 *  removed, and no monies are exchanged.
 *
 *  Permission is also granted to copy this source, without the
 *  news headers, for the purposes of making an executable copy by
 *  means of compilation, provided that such copy will not be used
 *  for the purposes of competition in any othello tournaments, without
 *  prior permission from the authors.
 *
 *  No responsibility is taken for any errors on inaccuracies inherent
 *  either to the comments or the code of this program, but if reported
 *  (see README file), then an attempt will be made to fix them.
 */

Reve  -  README  -  January 1994.

-----------------------------------------------------------------------
CONTENTS:

1. What is reve?
2. Getting started.
3. Strategy used by reve.
4. Acknowledgements.
-----------------------------------------------------------------------

1. What is reve?
----------------

This is version 2.0.0 of reve (an othello game). It requires a C compiler,
X11 and Motif.

See the manual pages for a detailed description of the rules of Othello.

Two humans can play each other (either locally or remotely), or you can
play against the computer.

Reve has various levels of difficulty.

You can decide, whether you would like to set a time limit, or a search depth 
limit on the computer move. 
 
The levels of play can be rated approximately as follows:
 
Instant: 
  Beginner level. The search depth is one ply. Even at this level,
  beating reve is not trivial! 

1 Minute/3 Minutes/5 Minutes:
  Intermediate levels. If you often beat reve at the 5 minute level, you
  have probably mastered the basics, and played a lot of games. Interested
  in playing real people in tournaments ? 

10 Minutes:
  Advanced. Beating reve at this level is no piece of cake. 

30 Minutes:  (tournament level)
  More advanced. None of us has beaten reve at this level yet. If you do,
  you should seriously consider participating in your countries national 
  championship. 

60 Minutes: 
  At this level reve looks further ahead than most humans can. If you
  win a game here, send us a transcript, because you have either found
  a serious bug in the algorithm, or your game deserves to be published. 
 
Set Search Depth:  
  Setting the depth is an alternative to using a clock. With a clock, the time
  left decides, how many plys you search for each move. This usually varies
  by one or two plys, depending on where in the game you are, and towards
  the end, it is often worth calculating all the way and then storing some
  moves.

  By setting the depth you force the computer to search n plys, no matter where
  in the game you are. There is no clock. This means, that if the depth is set
  to something other than say '0', or whatever you want to use, the computer
  uses the depth, otherwise it will use the time. Or something like that.
  The point of this feature is game analysis. Especially towards the endgame
  you want to be able to let the computer run (for example, over night). Or if
  you know that you usually look 4 plys ahead, you want to play an opponent
  that consistently examines 6 plys. Or you are simply curious what the
  computer would have played had it searched a little further (like situations
  which can only be recognized properly in 6 plys etc. Note, that for each
  additional ply, the compute time increases by about a factor of 4.

----------

There is a mailing list for people interested in reve. It is:

  reve@stard.Eng.Sun.COM

To get added to the list, send a request to:

  reve-request@stard.Eng.Sun.COM

Early patches are sent to the mailing list, plus active discussion on
ideas for enhancements to reve, plus othello strategy in general.

----------

The latest version of reve, recent patches plus an archive of the messages
sent to the reve mailing list are available via anonymous ftp from:
 
ftp.adelaide.edu.au [Australia] (129.127.40.3) in the pub/sun/richb directory.

  Thanks to Mark Prior <mrp@itd.adelaide.edu.au> for providing this service

ftp.x.org [USA] in the contrib directory.
 
Pick the site nearest to you, and please try to restrict your connections
to outside 9:00am - 5:00pm local time during weekdays.

----------

The address where people can get information about tournaments, Othello
Quarterly, and "Othello Brief and Basic' is:
 
        Othello Quarterly
        c/o C.Hewlett
        920 Northgate Ave.
        Waynesboro, VA 22980
        USA

2. Getting started.
-------------------

Initially there is no Makefile file present in the reve source directory.
You will need to copy Makefile.dist to Makefile, then adjust accordingly.

The Makefile compilation details are setup to default to compiling the Motif
version on a Sun4 running Solaris v2.3. Note that there are various
compilation definitions that might need uncommenting if you are trying to
compile and run it on other machines or operating systems.

These are:

EDGENAMEFILE - alternate location for the reve edge table.
EDGENAME     - alternate location for the reve edge table.
EXTRALIBS    - extra libraries that are needed to link reve.
HAS_GETDTAB  - uncomment if have getdtablesize().
HASREVEPROC  - uncomment to enable separate process for computer moves.
HELPNAMEFILE - alternate location for the reve online help file.
HELPNAME     - alternate location for the reve online help file.
MOTIFINCDIR  - Motif version only: location of the Motif #include files.
MOTIFLIBDIR  - Motif version only: location of the Motif libraries.
NO_NETWORK   - uncomment to disable -opponent user@host networking code.
NO_TIMEVAL   - uncomment if you don't have a timeval struct definition.
PROLOGUE     - alternate location for the trans PostScript prologue file.
REVEPROCFILE - alternate location of the reve_proc program.
REVEPROC     - alternate location of the reve_proc program.
SELTYPE      - uncomment for old select(2) calls.
SIGRET       - uncomment if your signal function expects an integer function.
SYSV         - uncomment if you are running Unix System V.
X11INCDIR    - X11 (Motif & Xlib) only: location of the X11 #include files.
X11LIBDIR    - X11 (Motif & Xlib) only: location of the X11 libraries.

See the Makefile for a detailed description of each of these definitions.

Now type "make". This will build all of the reve components.
You now need to do a "make install", to install the files in their proper
locations. You might need super-user permission to do this successfully.

----

Introduced with reve v1.3, was the ability to play over the network. A network
daemon is used for this, and in order for this to work correctly, you firstly
must have compiled reve with the HASNETWORK definition in the Makefile
uncommented, then (as super-user) you need to edit a couple of files.

Add the following line to /etc/services:

reve		518/udp

[Note if you are running NIS, then you might wish to add this to the
 /etc/services file on the NIS master server, then propogate the change].

Add the following line to /etc/inetd.conf:

reve    dgram   udp     wait    root    /usr/local/lib/in.reved     in.reved

[Note that /usr/local/games/in.reved needs to be replaced with the pathname of
 wherever you've installed the in.reved program].

The inetd daemon needs to either be restarted, or forced to re-read its'
configuration file.

The manual pages describe the rules of reve in detail.

If you need to make other changes in order to get reve to compile and run
on your system, please let us know the details (see email addresses below),
and we will try to include them into a future version.

See the TODO file for the list of known problems and ideas for future
enhancements.

Here are the settings for a variety of machines. Please send additional
machine settings to the address below, to be included in this section.

   Type of machine:               Sun 4/65 (color - 1152x900x8).
   O/S release:                   Solaris 2.3(FCS) + OpenWindows.
   Uncommented Makefile settings:

     EXTRALIBS       = -lnsl -lsocket -lgen
     HASREVEPROC     = -DHASREVEPROC
     MOTIFINCDIR     = -I$(MOTIFHOME)/include
     MOTIFLIBDIR     = -L$(MOTIFHOME)/lib
     SVR4            = -DSVR4
     X11INCDIR       = -I$(OPENWINHOME)/include
     X11LIBDIR       = -L$(OPENWINHOME)/lib

   ---------

   Type of machine:               HP Series 9000, model 3[67]0
   O/S release:                   HP-UX release 7.0
   Version of reve:               X11 (Xlib), MIT X11R4 source
   Imakefile settings:

     USRLIBDIR       = /usr/games/lib

   ---------

   Type of machine:               DEC 5000/200 (color - 1024x864x8) 
                                  DEC 3100     (mono - 1024x864x1)
   O/S release:                   Ultrix 4.1
                                  Ultrix 4.0
   Version of reve:               X11 (Xlib) (release 4)
   Uncommented Makefile settings:

     X11INCDIR       = -I$(OPENWINHOME)/include
     X11LIBDIR       = -L$(OPENWINHOME)/lib


3. Strategy used by reve.
-------------------------

reve is a "classical Othello program", using state of the art othello
strategy. It does not include Bayesian learning [Bill, 1989].

References : "A World-Championship-Level Othello Program", Paul S.Rosenbloom,
             Artificial Intelligence 19(1982), 279-320.
             "The Development of a World Class Othello Program", Kai-Fu Lee
             and Sanjoy Mahajan, Art. Int. 43(1990) 21-36.

Turning Notes on, displays an evaluation for each computer move. This value
is calculated as follows:

  An edgetable is pre-computed. A value is given to each border configuration.
  The average is 0, the mean value about 500.

  Then I fix c1 = 312 + 6 * movenumber
             c2 = | 50 + 2 * movenumber   1 <= movenumber <= 25
                  | 75 + movenumber      25 <= movenumber <= 60
             c3 = 99
  ( Iago implementation constants )

  then eval = ( c1 * ( edgesnote + Xsquaresnote ) +
                c2 * currentmobility +
                c3 * Potentialmobility ) / 1000

  mobility = 1000 * ( p - o ) / ( p + o + 2 )
  where p is the evaluation for the player and o for the opponent

  p and o are about in the set [ -10, 10 ]

In short, a positive number is good for the computer, and a negative numvber
is good for the opponent.

Here is a listing of a high level reve vs reve match.

                reve vs reve example.

      nt : note reve gives to the position.
      profmax : Depth reve looks forward.
      nbtotnodes : Accumulated number of final nodes evaluated.

 1, <C-4>, nt : 0,        profmax : 1,  nbtotnodes : 0
 2, <E-3>, nt : 5107,     profmax : 9,  nbtotnodes : 168236
 3, <F-4>, nt : 31284,    profmax : 9,  nbtotnodes : 459892
 4, <C-5>, nt : 7392,     profmax : 9,  nbtotnodes : 692337
 5, <D-6>, nt : -7392,    profmax : 8,  nbtotnodes : 843560
 6, <C-3>, nt : -29267,   profmax : 8,  nbtotnodes : 1305926
 7, <C-6>, nt : 8574,     profmax : 8,  nbtotnodes : 1461111
 8, <F-5>, nt : -38130,   profmax : 8,  nbtotnodes : 1630589
 9, <E-6>, nt : 7020,     profmax : 8,  nbtotnodes : 1882791
10, <D-3>, nt : -36894,   profmax : 8,  nbtotnodes : 2136012
11, <F-3>, nt : 4819,     profmax : 8,  nbtotnodes : 2684014
12, <F-6>, nt : -4819,    profmax : 7,  nbtotnodes : 3331165
13, <E-2>, nt : 34302,    profmax : 7,  nbtotnodes : 3539609
14, <G-4>, nt : -5858,    profmax : 7,  nbtotnodes : 4229607
15, <G-3>, nt : 5858,     profmax : 6,  nbtotnodes : 4390818
16, <E-1>, nt : -11790,   profmax : 7,  nbtotnodes : 5137455
17, <D-2>, nt : 11790,    profmax : 6,  nbtotnodes : 5418077
18, <F-2>, nt : -46674,   profmax : 6,  nbtotnodes : 5570855
19, <F-1>, nt : 15648,    profmax : 6,  nbtotnodes : 5766803
20, <C-1>, nt : -15648,   profmax : 5,  nbtotnodes : 5825491
21, <D-1>, nt : 52994,    profmax : 7,  nbtotnodes : 6365630
22, <G-1>, nt : -52994,   profmax : 6,  nbtotnodes : 6556423
23, <H-5>, nt : 5229,     profmax : 6,  nbtotnodes : 6656104
24, <H-4>, nt : -3054,    profmax : 7,  nbtotnodes : 7221420
25, <G-5>, nt : 3054,     profmax : 6,  nbtotnodes : 7313040
26, <H-6>, nt : -845,     profmax : 7,  nbtotnodes : 7615130
27, <H-3>, nt : 26592,    profmax : 7,  nbtotnodes : 8026020
28, <C-2>, nt : -26592,   profmax : 6,  nbtotnodes : 8123951
29, <H-7>, nt : 5691,     profmax : 6,  nbtotnodes : 8226483
30, <G-6>, nt : -13265,   profmax : 7,  nbtotnodes : 8301302
31, <E-7>, nt : 21196,    profmax : 7,  nbtotnodes : 8498532
32, <D-7>, nt : -19960,   profmax : 7,  nbtotnodes : 8700743
33, <C-8>, nt : 30104,    profmax : 8,  nbtotnodes : 9672532
34, <E-8>, nt : -30104,   profmax : 7,  nbtotnodes : 9953691
35, <D-8>, nt : 63346,    profmax : 8,  nbtotnodes : 10817345
36, <B-8>, nt : -63346,   profmax : 7,  nbtotnodes : 11038102
37, <F-7>, nt : 50472,    profmax : 8,  nbtotnodes : 11346042
38, <C-7>, nt : -11415,   profmax : 8,  nbtotnodes : 11495189
39, <B-6>, nt : 61958,    profmax : 8,  nbtotnodes : 11783153
40, <A-5>, nt : 3313,     profmax : 8,  nbtotnodes : 12069521
41, <B-3>, nt : -585518,  profmax : 9,  nbtotnodes : 12484782
42, <B-5>, nt : -50483,   profmax : 9,  nbtotnodes : 12680889
43, <A-6>, nt : -1469439, profmax : 9,  nbtotnodes : 12791711
44, <A-4>, nt : 2250360,  profmax : 10, nbtotnodes : 13053695
45, <B-4>, nt : 3,        profmax : 16, nbtotnodes : 13217789
46, <F-8>, nt : -3,       profmax : 15, nbtotnodes : 13408870
47, <B-2>, nt : 3,        profmax : 14, nbtotnodes : 13471036
48, <A-7>, nt : -3,       profmax : 13, nbtotnodes : 13492740
49, <G-7>, nt : 3,        profmax : 12, nbtotnodes : 13500234
50, <B-7>, nt : -3,       profmax : 11, nbtotnodes : 13503151
51, <B-1>, nt : 3,        profmax : 10, nbtotnodes : 13503785
52, <A-1>, nt : -3,       profmax : 9,  nbtotnodes : 13504016
53, <A-8>, nt : 3,        profmax : 8,  nbtotnodes : 13504094
54, <A-2>, nt : -3,       profmax : 7,  nbtotnodes : 13504146
55, <A-3>, nt : 3,        profmax : 6,  nbtotnodes : 13504159
56, <H-8>, nt : -3,       profmax : 5,  nbtotnodes : 13504167
57, <G-8>, nt : 3,        profmax : 4,  nbtotnodes : 13504171
58, <H-2>, nt : 0,        profmax : 1,  nbtotnodes : 13504171
59, <G-2>, nt : 0,        profmax : 1,  nbtotnodes : 13504171


4. Acknowledgements.
--------------------

The previous version of this program was posted to comp.sources.games
under the name othello2. That version was a rework of the SunView
graphics front-end from Ed Falk, Sun Microsystems, CA. It contained
computer strategy and remarks code from Chris Miller.

The graphics code was abstracted, and graphics drivers were added for
X11 (Xlib), the XView toolkit, and a dumb tty version.

This version has virtually the same graphical interface. The remarks code
has been removed, and the computer strategy is written by Yves Gallot.

Thanks to Valerie Haecky for the code for printing reve game boards to
PostScript printers.

Thanks to the University of California (Berkeley) for making the talk and
in.talkd programs freely distributable, and therefore providing the basis
for the networked version of reve.

Also others too numerous to mention for various bug reports, fixes and
suggestions for improvement:

------------------------------------------------------------------------

Suggestions for furthur improvement would be most welcome, plus bugs,
comments and flames.

Rich Burridge           richb@Eng.Sun.COM
Yves Gallot             (no net address at present).
