$ v = 'f$verify(0)'
$! COMPILE.COM
$!	compile VMS Gopher Log Analysis tools
$!
$!  19940228	D.Sherman		dennis_sherman@unc.edu
$!		original version
$!
$ ON CONTROL_Y THEN GOTO CLEANUP
$ ON ERROR THEN GOTO CLEANUP
$!
$ if f$trnlnm("VAXCMSG") .eqs. "DECC$MSG" .or. -
	  f$trnlnm("DECC$CC_DEFAULT") .eqs."/DECC"
$ then
$! 	DECC/Alpha:
$    write sys$output "Using DECC:"
$    v1 = f$verify(1)
$    cc := cc/warning=(disable=implicitfunc)/object=[-.obj]
$    v1 = 'f$verify(0)'
$ else
$! 	VAXC:
$    cc := cc/object=[-.obj]
$    write sys$output "Using VAXC:"
$    show sym cc
$ ENDIF
$!
$ v1 = f$verify(1)
$ cc glog.c
$!
$ v1 = 'f$verify(0)'
$ CLEANUP:
$    v1 = f$verify(v)
$exit
