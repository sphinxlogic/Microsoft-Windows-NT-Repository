Notice V2.0, TOOLS, A system for managing login notices
                                       
   [16-AUG-1995]
   
   NOTICE informs users of important (?) notices when they log in to a
   VMS system. When a user logs in, he/she is informed of any new notices
   added to the system since his/her last interactive login.
   
   NOTICE was written to provide some means for notifying users of
   important system changes (new software, new operating hours, etc.)
   without having to display a text file each time they log in. The heart
   of NOTICE is a normal VMS text library, which contains the text of all
   NOTICE messages. When NOTICE is run with the /NEW qualifier, a brief
   description of all notices added to the system since the user's last
   interactive login is displayed:

        $ notice

        NOTICE topics.  Type NOTICE  for more information.

        Topic      Date         Description
        ---------  -----------  -----------
        VUG        22-APR-1992  VMS Users Group meeting, April 23

        $

   Such a call to NOTICE would typically be placed in the system login
   procedure (SYS$SYLOGIN) so it is executed each time an interactive
   user logs in.
   
   Users can examine the actual text of the message by adding the topic
   name to the NOTICE command:

        $ NOTICE VUG

   NOTICE works like TYPE/PAGE on video terminals---the user is prompted
   to press RETURN before each screen is scrolled so they can read the
   notice without having it scroll off-screen.
   
   NOTICE can be configured to always display a topic in the NOTICE list.
   NOTICE without any parameters will display all of the notices
   available.
   
   The BLISS sources are distributed with the software.

     _________________________________________________________________
   
  CONTACTING THE AUTHOR
  
   MadGoat NOTICE was written by Hunter Goatley. To report bugs, please
   send mail to MadGoat-Bugs@WKUVX1.WKU.EDU. Additional MadGoat mailing
   lists are available as well.
