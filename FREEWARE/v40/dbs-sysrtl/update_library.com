$ ! Procedure:	UPDATE_LIBRARY.COM
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
$ say "%Updating library for ''arch_name'"
$ say ""
$loop:
$   next_one = f$search("''procedure_name'_*.COM",615243)
$   if (next_one .eqs. "") then goto end_loop
$   next_one = f$parse(next_one,,,"NAME")
$   say "%Invoking ''next_one'"
$   say ""
$   @'next_one'
$ goto loop
$end_loop:
$bail_out:
$ !'f$verify(__vfy_saved)'
$ exitt 1
