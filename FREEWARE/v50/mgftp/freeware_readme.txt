MGFTP, NETWORKING, An FTP client/server for VMS

    This is MGFTP V2.6-5

   [23-FEB-2001]

   MGFTP is an FTP client and server for OpenVMS VAX and OpenVMS
   AXP. MGFTP works with MadGoat's NETLIB TCP/IP library, which
   means that it works with virtually every implementation of TCP/IP for
   VMS.

   MGFTP started life as the CMU-Tek FTP client and server. Many
   thanks go to John Clement and Henry Miller, who made the CMU FTP much
   better than it used to be. The MadGoat version was modified to work
   with NETLIB and includes numerous enhancements and bug fixes. The
   MadGoat version was done by Darrell Burkhead and Hunter Goatley.

   UCX sites, in particular, should find MGFTP of interest. Some of
   the features include:

     * The client features a VMS CLI$ interface, with support for normal
       UNIX-style commands. For example, SET DEFAULT and CD both change
       the remote working directory. Unlike MultiNet's client, which
       offers a TOPS-20-style interface, MGFTP provides a VMS-style
       interface with the case preservation, etc., of a UNIX-style
       interface.

     * The MGFTP client and server support STRU O VMS. When both a
       server and a client support STRU VMS, VMS file attributes are
       retained when files are transferred.

       UCX does not support STRU VMS, making MGFTP ideal for UCX
       sites connecting to anonymous FTP sites running MultiNet.

     * The MGFTP server fully supports ANONYMOUS FTP accounts.
       Anonymous accounts can be restricted to certain directories, etc.,
       as you expect from an anonymous FTP server. The MGFTP server
       is running on ftp.wku.edu, if you'd like to see its output.

     * The MGFTP server implements the REIN command, which lets you
       log out of an account and log in to another without disconnecting
       from the remote server.

     * FTP/ANONYMOUS on the DCL command line automatically logs you in as
       an anonymous user.

     * The client automatically senses the file type, so you don't have
       to remember to say "Image" or "Ascii" before transferring a file.

     * May be started without the server, supplying a client-only option.

     * Works in batch mode.

   MGFTP installs using VMSINSTAL. It is distributed with full
   documentation in the form of on-line help, PostScript, ASCII text,
   HTML, and Bookreader files. The BLISS sources are also distributed
   with the software.

     _________________________________________________________________

  CONTACTING THE AUTHORS

   MGFTP is maintained and enhanced by Hunter Goatley. To report bugs,
   please send mail to MadGoat-Bugs@MadGoat.com.
