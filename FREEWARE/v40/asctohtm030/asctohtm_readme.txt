Notes on Release 3.0 of AscToHTM for VMS
========================================

Thank you for downloading the AscToHTM conversion software.  This software has
been developed under VMS in C++, but is primarily being produced as shareware
for Windows (cost $35).  The conversion engine software is platform 
independant, and thus will continue to be developed under VMS.

Yezerski Roper Limited and JAF (John A Fotheringham) would like to make this
software available free to VMS (God's own OS) users.  

Oh, alright then... call it "hitware".  All we ask is that you hit our
web page at

      http://www.yrl.co.uk/

and take some other free VMS goodies off us.  Of course, we're quite prepared
to write software for money, should you insist :)  The direct link to our free
software is

      http://www.yrl.co.uk/~yr/Free/

This software is free to FAQ maintainers on both platforms.


Building the software
=====================
Because this software is offered commercially elsewhere, we are unable to supply
sources.  The kit you download will contain some/all of the following

      - .EXE's that you can run.  These will require the C++ RTL installed

      - .EXE_NOSHARE.  These do not require the C++ RTL installed, but at 
        present we can only supply this for Alpha

      - Object libraries and a LINK_PROGS.COM file.  Execute the command file to
        generate the .EXE for your system

      - Documentation set.  The documentation is supplied as a number of text
        files that can be built into HTML using the software itself (see below).


Running the software
====================

Under VMS you'll probably want to define a foreign command such as

      ASCTOHTM :== $disk:[dir]ASCTOHTM

although it will run in a limited form from the command line, prompting you for
input files.  Running it interactively means you will be unable to use the 
program qualifiers.

To test it, why not convert this text file with the command

      ASCTOHTM ASCTOHTM_README.TXT/CONTENTS

and carry on reading this file in your favourite browser :)


Documentation
=============

We've bundled a documentation build kit into the download file.  This is a 
set of text and .jpg files that can be built into HTML using the software 
itself, thus serving as a good example of what can be done.  To do this 
simply do the following

      $ A2H :== $disk:[dir]ASCTOHTM  		! define a symbol
      $!
      $ A2H a2hdoco.txt ia2hdoco.pol 		! AscToHTM
      $ A2H a2tdoco.txt ia2tdoco.pol 		! AscToTab

This will generate an HTML version of the doco from the text supplied identical
to the version of the on-line at

      http://www.yrl.co.uk/~jaf/doco/A2H.HTM

(although the on-line version may eventually be more up to date as new versions
are released).  The on-line documentation has a link to a .zip file from 
which it can be downloaded.  Alternatively, details are at

      http://www.yrl.co.uk/~jaf/a2hdownload.html

A more VMS-specific page can be found at

      http://www.yrl.co.uk/~jaf/a2hvms.html


Links and stuff
===============

The AscToHTM home page is currently

      http://www.yrl.co.uk/~jaf/asctohtm.html



Registering the software
========================
Because it's free to VMS users, this is not necessary.  However, if you want
to keep track of changes in the software go to

      http://www.yrl.co.uk/~jaf/a2hvms.html

and use the NETMIND option at the bottom.  This will email you whenever that
page changes, which will only be when the VMS version is being updated.
You can Netmind the main page, but that will change more often as the Windows
version undergoes new releases.

Alternatively, register your interest by emailing jaf@yrl.co.uk, and we will
email you as and when new releases occur.  It's possible that a mailing list
will be set up in the near future to dicuss conversion issues.  Again, contact
us at jaf@yrl.co.uk to sign up for this list.

See the wishlist

      http://www.yrl.co.uk/~jaf/a2hwish.html

to see what we hope to do.  Be aware that most recent changes "fast tracked"
their way into the product, bypassing the wish list completely.


Warranty
========
Please don't make any safetly critical applications such as Nuclear Power
plants depend on it .... it's just not *that* good (yet) :-)

(above message primarily aimed at PC users)


Contacts
========
Any problems or feedback should be mailed to jaf@yrl.co.uk.  I'm particularly
open to bug reports and suggested improvements, indeed most recent developments
have been in direct response to comments.

Please note, this development is by a solitary human being who has other work 
to do, so regrettably a prompt reply to your enquiry cannot always be 
guaranteed.

Once again, thanks for looking

John A Fotheringham     01-Aug-1998
