$ ! Procedure:	BUILD_DBSSYSRTL.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ procedure = f$element(0,";",f$environment("PROCEDURE"))
$ procedure_name = f$parse(procedure,,,"NAME")
$ location = f$parse(procedure,,,"DEVICE","NO_CONCEAL") -
		+ f$parse(procedure,,,"DIRECTORY","NO_CONCEAL")
$ set noon
$ on control_y then goto bail_out
$ vax = (f$getsyi("HW_MODEL") .lt. 1024)
$ axp = (f$getsyi("HW_MODEL") .ge. 1024)
$ say = "write sys$output"
$ if (vax)
$   then
$   arch_name = "VAX"
$ else
$ arch_name = "ALPHA"
$ endif !(vax)
$ say ""
$ say "%Building DBSSYSRTL for ''arch_name'"
$ say ""
$ specific_name = procedure_name + "_''arch_name'"
$ specific_dcl = specific_name + ".COM"
$ if (f$search(specific_dcl) .eqs. "")
$   then
$   say "%Can't find ''specific_dcl'"
$ else
$ say "%Invoking ''specific_dcl'"
$ say ""
$ @'specific_dcl'
$ endif !(f$search(specific_dcl) .eqs. "")
$bail_out:
$ !'f$verify(__vfy_saved)'
$ exitt 1
