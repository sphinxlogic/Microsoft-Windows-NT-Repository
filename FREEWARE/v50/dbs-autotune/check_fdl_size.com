$ ! Procedure:	CHECK_FDL_SIZE.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ set noon
$ on control_y then goto bail_out
$ say = "write sys$output"
$ fld_home = f$edit(P1,"collapse,upcase")
$ data_home = f$edit(P2,"collapse,upcase")
$ data_type = f$edit(P3,"collapse,upcase")
$ if (data_type .eqs. "") then data_type = ".DAT"
$ data_type = data_type - "."
$ say ""
$ total_difference = 0
$loop:
$   fdl_spec = f$search("''fdl_home'*.FDL",5243)
$   if (fdl_spec .eqs. "") then goto end_loop
$   display_spec = f$parse(fdl_spec,,,"name")
$   data_spec = data_home + display_spec + "." + data_type
$   if (f$search(data_spec) .eqs. "") then goto loop
$   alq = f$file_attributes(data_spec, "alq")
$   gosub get_fdl_allocation
$   difference = fdl_allocation - alq
$   total_difference = total_difference + difference
$   say f$fao("!24AS  !10UL, fdl=!10UL, diff= !10SL" -
		,display_spec, alq, fdl_allocation, difference)
$ goto loop
$end_loop:
$ say f$fao("!/ >>>>> Total difference is !SL", total_difference)
$bail_out:
$ close/nolog fdl
$ !'f$verify(__vfy_saved)'
$ exitt 1
$
$get_fdl_allocation:
$ set noon
$ fdl_allocation = 0
$ open/read/error=fdl_problem fdl 'fdl_spec'
$fdl_loop:
$   read/end=end_fdl_loop/error=fdl_problem fdl fdl_line
$   fdl_line = f$edit(fdl_line, "compress,trim")
$   if (f$element(0, " ", fdl_line) .eqs. "ALLOCATION") then -
$     fdl_allocation = fdl_allocation + f$element(1, " ", fdl_line)
$ goto fdl_loop
$end_fdl_loop:
$ if (fdl_allocation .eq. 0) then goto fdl_problem
$ goto exit_get_fdl_allocation
$fdl_problem:
$ fdl_allocation = -1
$exit_get_fdl_allocation:
$ close/nolog fdl
$ return
