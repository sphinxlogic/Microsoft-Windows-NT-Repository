$ v = 'f$verify(0)'
$! COMPILE.COM
$!	compile VMS Gopher server
$!
$!  19940908	FMacrides		macrides@sci.wfeb.edu
$!		fixed prefixing for DECC
$!  19931209    F.Macrides		macrides@sci.wfeb.edu
$!		Fixed cc's for UCX and globalext's when DECC
$!  19931125    F.Macrides		macrides@sci.wfeb.edu
$!		eliminated prefix qualifier
$!  19931120    F.Macrides		macrides@sci.wfeb.edu
$!		formatting and qualifier fixups
$!  19931119	D.Sherman		dennis_sherman@unc.edu
$!		rewritten for Gopher1_2VMS-1 release
$!  19930909	D.Sherman		dennis_sherman@unc.edu
$!		rewritten version for Gopher1_2VMS0, based in part on
$!		the Gopher1_2VMS0 client make.com
$!
$ ON CONTROL_Y THEN GOTO CLEANUP
$ ON ERROR THEN GOTO CLEANUP
$ agent = 0
$ IF P1 .EQS. ""
$ THEN
$ write sys$output "Acceptable TCP/IP agents are"
$ write sys$output " [1] MultiNet (default)"
$ write sys$output " [2] UCX"
$ write sys$output " [3] Wollongong"
$ read sys$command/prompt="Agent [1,2,3] (RETURN = [1]) " agent
$ ENDIF
$ if agent .eq. 1 .or. agent .eqs. "" .or. p1 .eqs. "MULTINET" then -
    option = "MULTINET"
$ if agent .eq. 2 .or. p1 .eqs. "UCX" then option = "UCX"
$ if agent .eq. 3 .or. p1 .eqs. "WOLLONGONG" then option = "WOLLONGONG"
$!
$ if f$trnlnm("VAXCMSG") .eqs. "DECC$MSG" .or. -
     f$trnlnm("DECC$CC_DEFAULT") .eqs."/DECC"
$ then
$!      DECC:
$  if option .eqs. "UCX"
$  then
$    cc := cc/warning=(disable=implicitfunc)-
	     /prefix=(all)-
	     /include=([-])-
	     /object=[-.obj]-
	     /define=("''option'","SERVER")
$  else
$    cc := cc/warning=(disable=implicitfunc)-
	     /prefix=(except=(setsockopt,ntohs,htonl,htons,getsockname,getpeername,gethostbyname,gethostbyaddr,gethostname,inet_ntoa,inet_addr,connect,listen,bind,accept,socket))-
	     /include=([-])-
	     /object=[-.obj]-
	     /define=("''option'","SERVER")
$  endif
$    write sys$output "Using DECC:"
$ else
$!      VAXC:
$    cc := cc/include=([-])-
	     /object=[-.obj]-
	     /define=('option',SERVER)
$    write sys$output "Using VAXC:"
$ ENDIF
$    show sym cc
$!
$ v1 = f$verify(1)
$ cc daarray.c
$ cc debug.c
$ cc ext.c
$ cc ftp.c
$ cc gdgopherdir.c
$ cc globals.c
$ cc gopherd.c
$ cc gopherdconf.c
$ cc gsgopherobj.c
$ cc index.c
$ cc kernutils.c
$ cc site.c
$ cc sockets.c
$ cc special.c
$ cc strstring.c
$ cc util.c
$ cc waisgopher.c
$ v1 = 'f$verify(0)'
$ if f$trnlnm("VAXCMSG") .eqs. "DECC$MSG" .or. -
     f$trnlnm("DECC$CC_DEFAULT") .eqs."/DECC"
$ then
$    cc = "''cc'" + " /stand=vaxc"
$    show sym cc
$ ENDIF
$ v1 = f$verify(1)
$ cc compatible.c
$!
$ v1 = 'f$verify(0)'
$ CLEANUP:
$    v1 = f$verify(v)
$exit
