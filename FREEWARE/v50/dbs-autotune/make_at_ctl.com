$ ! Procedure:	MAKE_AT_CTL.COM
$ set noon
$ say = "write sys$output"
$ search_spec = f$edit(P1, "collapse,upcase")
$ if (search_spec .eqs. "") then goto missing_param
$ search_spec = f$parse(search_spec,"*.*",,,"no_conceal") - "]["
$ if (search_spec .eqs. "") then goto bad_param
$! say "Searching for ''search_spec'"
$ say f$fao("!! Control file created !11%D", 0)
$ device = f$parse(search_spec,,,"device")
$ cluster = f$getdvi(device, "cluster")
$
$loop:
$   filespec = f$search(search_spec, 625387)
$   if (filespec .eqs. "") then goto end_loop
$   org = f$file_attributes(filespec, "org")
$   if (org .nes. "IDX") then goto loop
$   alq = f$file_attributes(filespec, "alq")
$   eof = f$file_attributes(filespec, "eof")
$   comment = ""
$   if ((eof .eq. 0) .or. (alq .eq. cluster)) then comment = "!"
$   text = f$parse(filespec,,,"directory") -
		+ f$parse(filespec,,,"name") -
		+ f$parse(filespec,,,"type")
$   say "''comment'''text'	/optimize	! ''eof'/''alq'"
$ goto loop
$end_loop:
$ goto bail_out
$
$missing_param:
$ say "%Filespec parameter is missing"
$ goto bail_out
$bad_param:
$ say "%Filespec parameter is bad"
$ goto bail_out
$bail_out:
$ exitt 1
