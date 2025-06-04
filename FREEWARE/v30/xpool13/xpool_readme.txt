

This program is distributed in the hope that it will be useful(I mean 'fun'),
but without any warranty.  No author or distributor accepts responsibility to 
anyone for the consequences of using it or for whether it serves any particular
purpose or works at all. 
Everyone is granted permission to copy, and redistribute this file under the 
following conditions: 
  
   -Permission is granted to anyone to make or distribute copies of the program
    in any medium, provided that all copyright notices, permission and nonwarranty  
    notices are preserved, and that the distributor grants the recipient permission 
    for further redistribution as permitted by this document. 
   -No alteration on any resource that could change the copyright notice. 

  Ismail ARIT 1993 Kusadasi,AYDIN,TURKIYE.  09400
                              

Xpool v1.3
======

Xpool is a  X window game which simulates a billiard game by using an 
approach called Discrete Element Method(DEM). 

First of all, I would like to thank to those of you who were really kind to 
send very nice comments about the game. Looks like many people really enjoyed 
the game.  

I finally got some spare time and fixed some stupid bugs in the program, now 
it runs better and handles a lot of stuff more properly. This version still 
uses floating values, so you still need a fast CPU , but I am planning 
to switch to integer values, and do some other optimizations so that the game 
will run on slower CPUs too.  I am working on(almost done)  the network version 
of Xpool that lets two people play against each other. It will mainly use Xt/athena
, so this will be  the last version that uses pure Xlib (right? ;-)) 



In order to compile;

if you have GNU C, just type

$ make -f Makefile.std

----------------------------
if you have 'xmkmf & imake'

$ xmkmf

and 

$ make

----------------------------




rules:
    Check man pages for rules.
      (nroff -man xpool.1 | page) 



If you have any question/comments, you can  send your email to 
iarit@tara.mines.colorado.edu
but, I will be done with the school this summer(93) and start worring about 
some other problems, like how to find a job, and pay $40,000 education 
loan :-(, so I will be real busy. If I can't reply to your email, I 
apologize for that. 
I will post my new email address(probably, from Turkiye) with the next release 
of Xpool.



Happy playing, 
IstakayI kIrma Haaa..!!!!( Don't break the cue stick OKKKKKKKK.!!)

Ismail ARIT  
iarit@tara.mines.colorado.edu


Not: If you are interested in the theory, get a printout of the file
 'theory.ps'. It shows you the basic idea behind this game.


