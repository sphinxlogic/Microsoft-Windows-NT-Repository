$ verify = 'f$verify(0)'
$!------------------------------------------------------------------------------
$!  Login on a DECterm as the given user.
$!
$!  Requires JUMP, available from http://rubens.its.unimelb.edu.au/ftp/
$!
$!  Author:  Jonathan Ridler,
$!	     Information Technology Services,
$!	     The University of Melbourne,
$!	     Victoria, Australia.
$!
$!  Email: jonathan@unimelb.edu.au
$!
$!  History:
$!	03-Jan-1992	JER	Original version.
$!------------------------------------------------------------------------------
$
$ set noon
$ set nocontrol
$ say := write SYS$OUTPUT
$ ask := read SYS$COMMAND /end=EXIT_DOOR /error=EXIT_DOOR /prompt=
$ su  := $SYS_SYSTEM:JUMP /SETUSER /NOLOG
$ xw  := @SYS_SYSTEM:XW
$ me   = f$getjpi ("","USERNAME")
$
$ display = f$getdvi ("DECW$DISPLAY","DEVNAM")
$ if .not. $status  then  goto  EXIT_DOOR
$
$GET_USER:
$ if p1 .eqs. ""  then  ask "Username? " p1
$ p1 = f$edit (p1,"COLLAPSE,UPCASE,UNCOMMENT")
$ if p1 .eqs. ""  then  goto  GET_USER
$
$ set uic 'p1'
$ if .not. $status  then  goto  EXIT_DOOR
$ su 'p1'
$ xw dectermn 'p2' 'p3'
$ set uic 'me'
$ su 'me'
$
$EXIT_DOOR:
$ set control
$ EXIT 1 + 0 * f$verify (verify)
