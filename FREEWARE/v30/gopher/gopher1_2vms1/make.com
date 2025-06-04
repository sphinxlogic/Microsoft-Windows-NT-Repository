$ v = 'f$verify(0)'
$! MAKE.COM	
$!	compile and link VMS Gopher Server and ancillary tools and utilities
$!	using MMS if we have it, .com files if we don't.
$!
$!  19940204	D.Sherman		dennis_sherman@unc.edu
$!		Added call to [.LOGTOOLS] directory
$!  19931120    F.Macrides		macrides@sci.wfeb.edu
$!		formatting fixups
$!  19931119	D.Sherman		dennis_sherman@unc.edu
$!		original version based on client code
$!
$ ON CONTROL_Y THEN GOTO CLEANUP
$ ON ERROR THEN GOTO CLEANUP
$ agent = 0
$ IF P1 .EQS. ""
$ THEN
$ 	write sys$output "Acceptable TCP/IP agents are"
$ 	write sys$output " [1] MultiNet (default)"
$ 	write sys$output " [2] UCX"
$ 	write sys$output " [3] Wollongong"
$ 	read sys$command/prompt="Agent [1,2,3] (RETURN = [1]) " agent
$ ENDIF
$ if agent .eq. 1 .or. agent .eq. "" .or. p1 .eqs. "MULTINET" then -
    option = "MULTINET"
$ if agent .eq. 2 .or. p1 .eqs. "UCX" then option = "UCX"
$ if agent .eq. 3 .or. p1 .eqs. "WOLLONGONG" then option = "WOLLONGONG"
$!
$ v1 = f$verify(1)
$!
$! BUILD SERVER
$ set def [.gopherd]
$ v1 = 'f$verify(0)'
$ @make 'option'
$ v1 = f$verify(1)
$!
$! BUILD LOOKASIDE
$ set def [-.lookaside]
$ v1 = 'f$verify(0)'
$ @make 'option'
$ v1 = f$verify(1)
$!
$! BUILD INDEXING
$ set def [-.indexing]
$ v1 = 'f$verify(0)'
$ @make 'option'
$ v1 = f$verify(1)
$!
$! BUILD EGREP
$ set def [-.egrep]
$ v1 = 'f$verify(0)'
$ @vmsmake
$ v1 = f$verify(1)
$!
$! BUILD Manager Tools
$ set def [-.manager]
$ v1 = 'f$verify(0)'
$ @make
$ v1 = f$verify(1)
$!
$! BUILD Logtools
$ set def [-.logtools]
$ v1 = 'f$verify(0)'
$ @make
$ v1 = f$verify(1)
$!
$ set def [-]
$!
$ v1 = 'f$verify(0)'
$ CLEANUP:
$    v1 = f$verify(v)
$exit
