$ v = 'f$verify(0)'
$! MAKE.COM	
$!	compile and link VMS Gopher Server Indexing
$!	using MMS if we have it, .com files if we don't.
$!
$!  19931112	D.Sherman		dennis_sherman@unc.edu
$!		original version
$!
$ ON CONTROL_Y THEN GOTO CLEANUP
$ ON ERROR THEN GOTO CLEANUP
$!
$ if f$search("SYS$SYSTEM:MMS.EXE") .eqs. ""
$ then
$ 	@compile
$ 	@link
$ else
$	compiler = "VAX_C"
$ if f$trnlnm("VAXCMSG") .eqs. "DECC$MSG" .or. -
	  f$trnlnm("DECC$CC_DEFAULT") .eqs."/DECC" then compiler = "DEC_C"
$	MMS/MACRO=("''compiler'=1")
$ endif
$!
$ CLEANUP:
$    v1 = f$verify(v)
$exit
