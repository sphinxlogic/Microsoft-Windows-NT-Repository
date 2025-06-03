$ v = 'f$verify(0)'
$! COMPILE.COM
$!	compile VMS Gopher Server Indexing utilities
$!
$!  19931125    F.Macrides		macrides@sci.wfeb.edu
$!		eliminated prefix qualifer
$!  19931120    F.Macrides		macrides@sci.wfeb.edu
$!		formatting and qualifer fixups
$!  19931112	D.Sherman		dennis_sherman@unc.edu
$!		rewrote for Gopher1_2VMS-1
$!  19930909	D.Sherman		dennis_sherman@unc.edu
$!		rewritten version for Gopher1_2VMS0, based in part on
$!		the Gopher1_2VMS0 client make.com
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
$ cc build_index.c
$ cc query.c
$!
$ set command/object=[-.obj] indexcld.cld
$!
$ v1 = 'f$verify(0)'
$ CLEANUP:
$    v1 = f$verify(v)
$exit
