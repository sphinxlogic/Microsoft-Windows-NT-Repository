RCARD, UTILITIES, Read Cardfiler cards on VT terminal

RCARD V1.4 - Read DECwindows/Motif Cardfiler card files
PCARD V1.0 - Print DECwindows/Motify Cardfiler card files

Copyright � 1998, Hunter Goatley.  All rights reserved.

RCARD is a program that will read a DECwindows CardFiler card file and
display it on non-DECwindows terminals using the SMG$ routines.  It works
with the files produced by DECwindows v5.4 and Motif v1.x (yes, the files
are different).

RCARD can be compiled with either VAX C or DEC C.  MMS or MMK should
be used to built it.

To use it, just define a symbol and give the name of the card file:

	$ rcard :== $dev:[dir]rcard.exe
	$ rcard phone			!Defaults to SYS$DISK:[].CARD

Once the file has been read into memory, a window will appear containing a
menu of card subjects.  You can scroll through the menu using up-arrow,
down-arrow, NextScreen, and PrevScreen.  When the cursor is positioned
on the subject for the card you want to see, press RETURN.  A new window
will pop up containing the text of the card.

You can scroll through the card's contents and press either "Q" or ^Z to
exit the card and go back to the subject menu.  You can also press "N" or
"P" to move to the next or previous card, respectively without closing the
card window.

CTRL-W repaints the screen.

Please note the following limitations:

  o  The file format was based on observation only; I didn't have access to
     the DECW$CARDFILER sources.  RCARD could break in the future.
  o  Card files containing DDIF images *cannot* be viewed using RCARD.  I
     never could figure out how the images were stored so that RCARD could
     skip over them.  Instead, the program bombs with a horrible access
     violation and stack dump.  So much for error checking.
  o  Card files cannot be created or modified by RCARD (the "R" stands for
     "read").  Since I couldn't identify all of the information in the
     files, I didn't want to claim to be able to write them.
  o  Really large files could crash the program.  The entire file is read
     into memory, so it is limited by whatever process limits are in
     effect.

I wrote the program because I was using the DECwindows CardFiler while at
work and didn't have any easy way to look at the information while I was
dialed in from home.  The program isn't very robust; I never store images
in the files, so it works well for me.

---------------
PCARD reads DECwindows/Motif Cardfiler card files and generates a TeX
output file that will produce two-column output showing the contents of
each card.  The TeX source file NEWSLETTER.TEX is required.

	$ pcard :== $dev:[dir]pcard.exe
	$ pcard phone		!Creates PHONE.TEX
	$ tex "\input newsletter   \input phone"


Questions, comments, or suggestions are welcome.

Hunter Goatley, goathunter@PROCESS.COM
--------------------------------------------------------------------------------
COPYRIGHT NOTICE

This software is COPYRIGHT � 1991, 1998  Hunter Goatley. ALL RIGHTS RESERVED.
Permission is granted for not-for-profit redistribution, provided all source
and object code remain unchanged from the original distribution, and that all
copyright notices remain intact.

DISCLAIMER

This software is provided "AS IS".  The author makes no representations or
warranties with repsect to the software and specifically disclaim any
implied warranties of merchantability or fitness for any particular
purpose.
