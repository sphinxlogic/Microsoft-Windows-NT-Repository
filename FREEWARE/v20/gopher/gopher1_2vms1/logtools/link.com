$ v = 'f$verify(0)'
$! LINK.COM
$!      link VMS Gopher Server Log Analysis tools
$!
$!  19940228    D.Sherman               dennis_sherman@unc.edu
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
$ show sym optfile
$!
$ v1 = f$verify(1)
$ link /executable=[-.exe]glog.exe -
    [-.obj]glog.obj,-
    [-.optfiles]ident/opt,-
    [-.optfiles]'optfile'/opt
$!
$ v1 = 'f$verify(0)'
$ CLEANUP:
$    v1 = f$verify(v)
$exit
