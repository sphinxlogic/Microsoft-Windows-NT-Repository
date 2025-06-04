XBOING23_NEW, GAMES, An X Window System based blockout clone. V2.3 (New)

              .o8                  o8o                            ooooo ooooo 
             "888                  `"'                            `888' `888' 
 oooo    ooo  888oooo.   .ooooo.  oooo  ooo. .oo.    .oooooooo     888   888  
  `88b..8P'   d88' `88b d88' `88b `888  `888P"Y88b  888' `88b      888   888  
    Y888'     888   888 888   888  888   888   888  888   888      888   888  
  .o8"'88b    888   888 888   888  888   888   888  `88bod8P'      888   888  
 o88'   888o  `Y8bod8P' `Y8bod8P' o888o o888o o888o `8oooooo.     o888o o888o 
                                                    d"     YD                 
                                                    "Y88888P'                 

                       Version 2.3 - 13th Feb 1995


Description
-----------

XBoing is a blockout type game where you have a paddle which you use to
bounce a ball around the game area blowing up blocks with the ball. You win by
obtaining points for each block destroyed and each level completed. The person
with the highest score wins.

XBoing was started like many other projects to learn Xlib better. I had the 
XPM library and was already using it in a Motif application. I thought that it
would be cool to have nice colour pictures/animations in an Xlib game. So I 
did. Without the XPM library I would be still playing with the colours I think.

Please read the manual page as the manual has some important game play 
information. 

WARNING: This game requires a lot of grunt to run well. If someone is doing
         a compile or the load is large then expect the game to be a bit
         bumpy and hard to play. Sorry about that. ;-|

Please see version.doc for information on this version.


Usage
-----

The game does have command line options so check them out. 'xboing -help'

The colourmap is taken over as I need all the colours. Sorry but thats the
problem with only 256 colourmap entries. I cannot see a problem with this.

The colourmap is released afterwards and should return you to your lovely
colour scheme. If it doesn't please let me know.

Special Thanks
--------------

Really special thanks to Arnaud Le Hors (lehors@sophia.inria.fr) for the
wonderful XPM library and for making it free :-).

Thanks for a some cool backgrounds found in Anthony's Icons - some copyright.
Anthony Thyssen - anthony@cit.gu.edu.au

Please read the COPYRIGHT message in the backgrounds directory as some images
are copyright and must NOT be used in a Microsoft Windows environment.

The comp.windows.x newsgroup netters for help over the past few years. See
the changes.doc doco inline lisitings of the some of the many helpers.

Thankyou to those who have emailed me over the past year reporting bugs :-(
and making me aware of your preferences.

Justin Kibell
jck@citri.edu.au
SnailMail: 42 Beard Street, Eltham, Victoria, Australia, 3095


XBOING V 2.3 port under OpenVMS    Feb 1995/March 1996
===============================

New mods - DEC 1995
========

Automatic link procedures to support various versions of DECW/Motif (1.1/1.2
especially).

Support of DEC C 5.0


New mods - March 1996
========

Support of DEC C 5.2 on VAX & Alpha

EXE and Objs provided:
=====================

VAX : VMS 5.5-2, GCC 2.5.8, MOTIF 1.1
Alpha: VMS 6.1-1H2, DEC C 5.0, MOTIF 1.2

Patrick Moreau - CENA/Athis_mons - FRANCE

pmoreau@cena.dgac.fr
moreau_p@decus.fr
