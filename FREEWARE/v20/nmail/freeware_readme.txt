Nmail, utilities, queued DECnet mail system for use with VMS MAIL


   SO WHAT'S NMAIL ANYWAY?

   Nmail works in conjunction with the  VMS  MAIL  utility  to  implement
   queued  transmission  of  mail.   This  allows you to queue up mail to
   nodes that are currently unreachable, for example, or to transmit long
   documents without needing to wait until transmission is complete.

   Nmail requires no special software to be installed at the remote node;
   it can send to any destination that ordinary MAIL can send to.


   HOW DO I INSTALL IT?

   To install, use VMSINSTAL:

            $ @VMSINSTAL NMAIL100 SYS$UPDATE:
   
   The same kit is used for both VAX and Alpha systems.

   You will be asked a few questions about your configuration.  If  you
   don't  understand  a  question,  simply enter "?" in response to the
   prompt.  The installation procedure will  display  some  explanatory
   text and then repeat the question.

   It is fairly easy to change the configuration at a later date,  just
   by editing the Nmail startup command file.
   
   After installing Nmail, you should read the documentation, which  is
   located in the following files:

            SYS$HELP:NM$SYSMGR.DOC      System Manager's manual
            SYS$HELP:NM$UGUIDE.DOC      User Guide
   

   HOW DO I BUILD IT FROM SOURCES?

   Expand the sources  saveset into a directory and  execute BUILD.COM. 
   On the  author's system, this will  build the VAX and  Alpha images.
   To do this, it  requires the Alpha  cross-system tools, which aren't
   support  on  VAX VMS V6.1, so some trickery is  required to get them
   working.  This trickery is left as an exercise for the reader.

   Since the author has a VAX, the build procedure probably  won't work
   on an Alpha without modification -- for example, it will expect some
   non-existent cross compiler so that it can build the VAX images.
  
   Once you have actually  managed to  build the  images, use CREAK.COM 
   to create a VMSINSTAL-format kit. 



