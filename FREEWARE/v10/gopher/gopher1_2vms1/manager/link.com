$ v = 'f$verify(0)'
$! LINK.COM
$!      link VMS Gopher Server manager tools
$!
$!  19931120    F.Macrides		macrides@sci.wfeb.edu
$!		formatting fixups
$!  19931119	D.Sherman		dennis_sherman@unc.edu
$!		original version
$!
$ ON CONTROL_Y THEN GOTO CLEANUP
$ ON ERROR THEN GOTO CLEANUP
$!
$ if f$trnlnm("VAXCMSG") .eqs. "DECC$MSG" .or. -
	  f$trnlnm("DECC$CC_DEFAULT") .eqs."/DECC"
$ then
$!      DECC/Alpha:
$   optfile = "DECC"
$ else
$!      VAXC:
$   optfile = "VAXC"
$ endif
$!
$ v1 = f$verify(1)
$ link /executable=[-.exe]forcex.exe -
    [-.obj]forcex.obj,-
    [-.optfiles]ident/opt,-
    [-.optfiles]'optfile'/opt
$!
$ v1 = 'f$verify(0)'
$ CLEANUP:
$    v1 = f$verify(v)
$exit
