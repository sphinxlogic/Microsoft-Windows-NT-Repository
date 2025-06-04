MXRN, UTILITIES, A Motif Usenet News Newsreader

This is V6.18-32A

Pre-built executable images can be found in the [.VAX] and [.ALPHA] 
subdirectories.

The OpenVMS Alpha images were built with:
  Compaq C V6.4-004 on OpenVMS Alpha V7.2-1

The OpenVMS VAX images were built with:
  Compaq C V6.4-005 on OpenVMS VAX V7.2

To Build:
  $ @MAKE MXRN    ! builds MXRN; this is the default
  or 
  $ @MAKE DXRN    ! DXRN, only on old OpenVMS VAX

To run MXRN:
  $ MXRN :== $DRIVE:[DIRECTORY]MXRN.EXE
  $ MXRN

Useful entries in MXRN.DAT:

mxrn.nntpServer:        news.server.hostname
mxrn.replyTo:           return_smtp_email_address
mxrn.replyPath:         return_smtp_email_address

For (far) more information, please see XRN.DOC.
