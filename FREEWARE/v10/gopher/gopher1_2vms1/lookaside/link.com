$ v = 'f$verify(0)'
$! LINK.COM
$!      link VMS Gopher Server ACL-resident LOOKASIDE utility
$!
$!  19931120    F.Macrides		macrides@sci.wfeb.edu
$!		formatting fixups
$!		supply lookaside.ini in [-.exe] rather than copying from here
$!  19931112	D.Sherman		dennis_sherman@unc.edu
$!		rewritten for Gopher1_2VMS-1 release
$!  19930909    D.Sherman               dennis_sherman@unc.edu
$!              rewritten version for Gopher1_2VMS0, based in part on
$!              the Gopher1_2VMS0 client make.com
$!
$ ON CONTROL_Y THEN GOTO CLEANUP
$ ON ERROR THEN GOTO CLEANUP
$!
$ if f$trnlnm("VAXCMSG") .eqs. "DECC$MSG" .or. -
	  f$trnlnm("DECC$CC_DEFAULT") .eqs."/DECC"
$ then
$!      DECC:
$   optfile = "DECC"
$   if f$getsyi("CPU") .ge. 128
$   then ! DECC/AXP
$       v1 = f$verify(1)
$       link := link/nonative
$       v1 ='f$verify(0)'
$   endif
$ else
$!      VAXC:
$   optfile = "VAXC"
$ endif
$!
$ show sym optfile
$ v1 = f$verify(1)
$ link /executable=[-.exe]lookaside.exe -
    [-.obj]lookaside.obj,-
    [-.optfiles]ident/opt,-
    [-.optfiles]'optfile'/opt
$!
$ v1 = 'f$verify(0)'
$ CLEANUP:
$    v1 = f$verify(v)
$exit
