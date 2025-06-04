$	was_verify = 'f$verify(0)'
$!
$! Conditionally compile a Decus C file on VMS V3.1.
$!
$! The SOURCE file is processed to create the OUTPUT file.  This command
$! checks creation dates and does not compile if both files are present
$! and the OUTPUT file is newer (younger) than the SOURCE file.
$! This sequence defines the SOURCE and OUTPUT files:
$	SOURCE = P1
$	if SOURCE .eqs. "" then inquire SOURCE "Decus C Source File"
$	SOURCE = f$parse(SOURCE, ".C")
$	fname = f$parse(SOURCE,,, "NAME")
$	OUTPUT = "''fname'.OBJ"
$!
$! Continue at must_process if either file is missing or the source is younger
$! A missing SOURCE is, of course, an error -- but one that should be
$! caught by the "normal" command.
$!
$	on error then goto must_process
$	if f$search(SOURCE) .eqs. "" then goto must_process
$	if f$search(OUTPUT) .eqs. "" then goto must_process
$	src_time = f$file_attributes(SOURCE, "CDT")	! Get creation time
$	out_time = f$file_attributes(OUTPUT, "CDT")	! for both files.
$	if f$cvtime(src_time) .ges. f$cvtime(out_time) then -
	    goto must_process
$	write sys$output OUTPUT, " is up to date."
$	goto finish
$!
$! Come here to build OUTPUT from SOURCE
$!
$ must_process:
$	on error then goto finish
$!
$! Insert commands to create OUTPUT from SOURCE here, for example:
$!
$	write sys$output OUTPUT, " <= ", SOURCE
$	xcc 'SOURCE' 'P2' 'P3' 'P4' 'P5' 'P6' 'P7' 'P8' 'P9'
$	xas 'f$parse(SOURCE,,, "NAME")''.S -d
$!
$ finish:
$	if was_verify then set verify
