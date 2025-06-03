NEWSRDR V4.8-1, TOOLS, A VMS Mail-like USEnet NEWS reader
                                       
   [16-JUN-1994]
   
   NEWSRDR is an NNTP client program for VMS systems running CMU-Tek
   TCP/IP, TGV MultiNet, DEC's VMS/ULTRIX Connection, or any TCP/IP
   package supported by the NETLIB network interface library.
   
   NNTP is the Network News Transfer Protocol, which is used to
   communicate USENET news over TCP/IP. Since many systems cannot afford
   the disk space to store all net news locally, this program can be used
   to communicate with a cooperating news server system to read and post
   net news. News articles are fetched on an as-needed basis and are
   stored on the client system only while they are being read.
   
   NEWSRDR is not a port of UNIX-based news readers. It is written
   strictly for VMS and provides an interface that VMS users should find
   reasonably comfortable. Most of the commands in NEWSRDR should be
   familiar to VMS MAIL users.
   
   NEWSRDR is written entirely in C. NEWSRDR can be used with any TCP/IP
   package that supports a Berkeley socket library interface, or with any
   package supported by the provided NETLIB network interface library.
   NEWSRDR requires VAX/VMS V5.0 or later, or OpenVMS Alpha AXP V1.0 or
   later.
   
   Support for Japanese sites using Kanji in news articles is provided
   through the use of an installable library of character conversion
   routines which can be mapped in at run-time by NEWSRDR. C source is
   provided for the Kanji character code conversion routines. Support for
   username and mail address conversion routines is also provided, also
   through the use of shareable libraries.
   
   Refer to the NEWSRDR documentation for specific system and user
   requirements.
   
     _________________________________________________________________
   
  CONTACTING THE AUTHOR
  
   NEWSRDR was written by Matt Madison. To report bugs, please send mail
   to MadGoat-Bugs@WKUVX1.WKU.EDU. Additional MadGoat mailing lists are
   available as well.
