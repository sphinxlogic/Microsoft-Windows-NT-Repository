$ v = 'f$verify(0)'
$! MAKE.COM	
$!	compile and link VMS Gopher Server Lookaside
$!	using MMS if we have it, .com files if we don't.
$!
$!  19931220	JLWilkinson		jlw@psulias.psu.edu
$!		adapted Foteos Macrides' UCX special handling for DECC
$!  19931112	D.Sherman		dennis_sherman@unc.edu
$!		original version
$!
$ ON CONTROL_Y THEN GOTO CLEANUP
$ ON ERROR THEN GOTO CLEANUP
$!
$ if f$search("SYS$SYSTEM:MMS.EXE") .eqs. ""
$ then
$ 	@compile 'option'
$ 	@link 'option'
$ else
$	compiler = "VAX_C"
$	edt_libr = "NATIVE_EDT"
$ if f$trnlnm("VAXCMSG") .eqs. "DECC$MSG" .or. -
	  f$trnlnm("DECC$CC_DEFAULT") .eqs."/DECC"
$		then
$		compiler = "DEC_C"
$		if f$getsyi("CPU") .ge. 128 then edt_libr = "VESTED_EDT"
$		endif
$	MMS/MACRO=("''compiler'=1","''edt_libr'=1")
$ endif
$!
$ CLEANUP:
$    v1 = f$verify(v)
$exit
