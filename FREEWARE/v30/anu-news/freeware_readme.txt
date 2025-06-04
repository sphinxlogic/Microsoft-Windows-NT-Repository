ANU-NEWS V6.0, SOFTWARE, USENET News Client and Server

Executables are in the [.VAX] and [.ALPHA] subdirectories.
The VAX version was built with VAXC V3.2 on OpenVMS V6.1.  
The Alpha version was built with DEC C V5.00-004 on OpenVMS V6.2.

To Build:
  UNZIP ANU-NEWS_SRC
  SET DEF [.ANU-NEWS.NEWS_SRC]
  
Client:
  @NEWSBUILD U CLIENT NODEBUG "" ""

Server:
  @NEWSBUILD U SERVER NODEBUG "" ""


To run the news client:

 $ NEWS :== $DRIVE:[DIRECTORY]NEWS.EXE
 $ NEWS

When prompted for the protocol, enter UCX.

For more information see NEWS.TXT
