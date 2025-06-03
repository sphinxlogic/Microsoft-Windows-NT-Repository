$ v = 'f$verify(0)'
$! MAKE.COM	
$!	compile and link VMS Gopher Server
$!	using MMS if we have it, .com files if we don't.
$!
$!  19940908	FMacrides	macrides@sci.wfeb.edu
$!		fixed prefixing for DECC
$!  19931220	JLWilkinson		jlw@psulias.psu.edu
$!		adapted Foteos Macrides' UCX special handling for DECC
$!  19931116	D.Sherman		dennis_sherman@unc.edu
$!		original version
$!
$ ON CONTROL_Y THEN GOTO CLEANUP
$ ON ERROR THEN GOTO CLEANUP
$!
$ agent = 0
$ IF P1 .EQS. ""
$ THEN
$ write sys$output "Acceptable TCP/IP agents are"
$ write sys$output " [1] MultiNet (default)"
$ write sys$output " [2] UCX"
$ write sys$output " [3] Wollongong"
$ read sys$command/prompt="Agent [1,2,3] (RETURN = [1]) " agent
$ ENDIF
$ if agent .eq. 1 .or. agent .eq. "" .or. p1 .eqs. "MULTINET" then -
    option = "MULTINET"
$ if agent .eq. 2 .or. p1 .eqs. "UCX" then option = "UCX"
$ if agent .eq. 3 .or. p1 .eqs. "WOLLONGONG" then option = "WOLLONGONG"
$!
$ if f$search("SYS$SYSTEM:MMS.EXE") .eqs. ""
$ then
$ 	@compile 'option'
$ 	@link 'option'
$ else
$	compiler = "VAX_C"
$	prefix = ""
$	if f$trnlnm("VAXCMSG") .eqs. "DECC$MSG" .or. -
	   f$trnlnm("DECC$CC_DEFAULT") .eqs."/DECC"
$		then
$		compiler = "DEC_C"
$		prefix = "/prefix=(except=(setsockopt,ntohs,htonl,htons,getsockname,getpeername,gethostbyname,gethostbyaddr,gethostname,inet_ntoa,inet_addr,connect,listen,bind,accept,socket))"
$		if option .eqs. "UCX" then prefix = "/prefix=(all)"
$		endif
$	MMS/MACRO=("TCPIP=''option'","''compiler'=1","PREFIX=''prefix'")
$ endif
$!
$ CLEANUP:
$    v1 = f$verify(v)
$exit
