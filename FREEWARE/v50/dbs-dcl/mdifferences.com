$ ! Procedure:	MDIFFERENCES.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ set noon
$ on control_y then goto bail_out
$ vax = (f$getsyi("ARCH_TYPE") .eq. 1)
$ axp = (f$getsyi("ARCH_TYPE") .eq. 2)
$ say = "write sys$output"
$ filespec = f$edit(P1,"COLLAPSE,UPCASE")
$ otherspec = f$edit(P2,"COLLAPSE,UPCASE")
$ if (filespec .eqs. "")
$   then
$   say "%MDIFF-E-NOFILE, no filespec was given"
$ else
$ if (otherspec .eqs. "")
$   then
$   say "MDIFF-E-NOOTHER, no other filespec was given"
$ else
$ lastfile = ""
$loop:
$   thisfile = f$search(filespec,615243)
$   if (thisfile .eqs. "") then goto end_loop
$   if (thisfile .eqs. lastfile) then goto end_loop
$   lastfile = thisfile
$   say f$fao("!/!10*= !AS!/",thisfile)
$   differences 'thisfile' 'otherspec'
$ goto loop
$end_loop:
$ endif !(otherspec .eqs. "")
$ endif !(filespec .eqs. "")
$bail_out:
$ say f$fao("!/!10*= Multi-Differences Complete!/")
$ !'f$verify(__vfy_saved)'
$ exitt 1
$ !+==========================================================================
$ !
$ ! Procedure:	MDIFFERENCES.COM
$ !
$ ! Purpose:	
$ !
$ ! Parameters:
$ !
$ ! History:
$ !		14-Aug-1995, DBS; Version V1-001
$ !	001 -	Original version.
$ !-==========================================================================
