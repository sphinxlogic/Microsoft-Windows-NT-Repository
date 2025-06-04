MXRN V6.18-32, SOFTWARE, A Motif USENET News Newsreader

Executables can be found in the [.VAX] and [.ALPHA] subdirectories.
The VAX version was built with VAXC V3.2-044 on OpenVMS V6.1.
The Alpha version was built with DEC C V5.00-004 on OpenVMS V6.2.

To Build:
  $ @UNZIP MXRN_SRC
  $ SET DEF [.MXRN]
  $ @MAKE-MXRN

To run MXRN:
  $ MXRN :== $DRIVE:[DIRECTORY]MXRN.EXE
  $ EDIT SYS$LOGIN:NNTP.SERVER  ! put the name of your news server in this file
  $ MXRN

For more information see XRN.DOC
