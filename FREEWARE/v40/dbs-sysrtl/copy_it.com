$ ! Procedure:	COPY_IT.COM
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
$ if (f$trnlnm("DBSLIBRARY") .eqs. "")
$   then
$   say "%No translation for DBSLIBRARY - copy cannot be done"
$ else
$ rtl_exe = "DBSSYSRTL.''arch_name'_EXE"
$ rtl_olb = "DBSSYSRTL.''arch_name'_OLB"
$ olb_olb = "SYS_OBJECTS.''arch_name'_OLB"
$ say "%Copying libraries for ''arch_name'"
$ say ""
$ say "  ... ''rtl_exe'"
$ copyy/nolog 'rtl_exe' dbslibrary:*.exe
$ say "  ... ''rtl_olb'"
$ copyy/nolog 'rtl_olb' dbslibrary:*.olb
$ say "  ... ''olb_olb'"
$ copyy/nolog 'olb_olb' dbslibrary:*.*
$ endif !(f$trnlnm("DBSLIBRARY") .eqs. "")
$bail_out:
$ !'f$verify(__vfy_saved)'
$ exitt 1
