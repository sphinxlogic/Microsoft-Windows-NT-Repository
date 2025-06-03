$ v = 'f$verify(0)'
$! MAKE.COM
$!	compile and link glog for VMS  (Gopher Log analyzer)
$!	using MMS if we have it, .com files if we don't.
$!
$!  19940909    D.Sherman <dennis_sherman@unc.edu>
$!      fixed prefixing for DECC
$!  19940228	D.Sherman		dennis_sherman@unc.edu
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
$	if f$trnlnm("VAXCMSG") .eqs. "DECC$MSG" .or. -
	  f$trnlnm("DECC$CC_DEFAULT") .eqs."/DECC"
$	then
$	  compiler = "DEC_C"
$	endif
$	MMS/MACRO=("''compiler'=1")
$ endif
$!
$ CLEANUP:
$    v1 = f$verify(v)
$exit
