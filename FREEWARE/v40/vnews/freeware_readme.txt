VNEWS V1.50A, SOFTWARE, A USENET News Newsreader

Executables can be found in the [.VAX] and [.ALPHA] subdirectories.
The VAX version was built with DEC Fortran V6.3 on OpenVMS V6.1
The Alpha version was built with DEC Fortran V6.2 on OpenVMS V6.2

To Build:
  $ UNZIP VNEWS_SRC
  $ SET DEF [.VNEWS]
  $ @COMPILE_VNEWS
  $ @LINK_VNEWS UCX

To run VNEWS:
  $ VNEWS :== $DRIVE:[DIRECTORY]VNEWS
  $ DEFINE NNTP_SERVER "your.news.server"
  $ VNEWS

For more information see INSTALL_AND_MANAGEMENT.DOC and VNEWS.DOC.
