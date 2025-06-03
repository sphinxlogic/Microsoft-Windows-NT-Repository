MadGoat FINGER V1.2, UTILITIES, A FINGER client/server

   [26-FEB-1995]
   
   MadGoat FINGER is a FINGER client and server for OpenVMS VAX and
   OpenVMS AXP. MadGoat FINGER works with MadGoat's NETLIB TCP/IP
   library, which means that it works with virtually every implementation
   of TCP/IP for VMS.
   
   The MadGoat FINGER server is a relatively no-frills server. When asked
   for system information, it returns the usernames, owner names, nodes,
   and terminals of all logged-in users. It does not return the image
   name, and the information returned cannot be customized without
   modifying the source code.
   
   When information for a particular user is requested, that user's login
   directory is searched for a plan file, which is returned to the
   client. If the user is currently logged in, that information is also
   returned.
   
   The BLISS sources are distributed with the software.
   
  CONTACTING THE AUTHORS
  
   MadGoat FINGER was written by Matt Madison and Hunter Goatley. To
   report bugs, please send mail to MadGoat-Bugs@WKUVX1.WKU.EDU.
   Additional MadGoat mailing lists are available as well.
     _________________________________________________________________
   
   
    MadGoat Software Repository, Hunter Goatley
