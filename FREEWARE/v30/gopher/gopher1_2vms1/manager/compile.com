$ v = 'f$verify(0)'
$! COMPILE.COM
$!	compile VMS Gopher Server manager tools
$!
$!  19931125    F.Macrides		macrides@sci.wfeb.edu
$!		eliminated prefix qualifier
$!  19931120    F.Macrides		macrides@sci.wfeb.edu
$!		formatting and qualifier fixups
$!  19931119	D.Sherman		dennis_sherman@unc.edu
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
$    write sys$output "Using VAXC:"
$    v1 = f$verify(1)
$    cc := cc/object=[-.obj]
$    v1 = 'f$verify(0)'
$ ENDIF
$!
$ v1 = f$verify(1)
$ cc forcex.c
$!
$ v1 = 'f$verify(0)'
$ CLEANUP:
$    v1 = f$verify(v)
$exit
