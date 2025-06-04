$! report number of uses per day of Gopher
$ reports_dir := gopher_root:[reports]	! CHANGE ME to path of report .COM & .GAWK files
$ if P1 .eqs. "" then P1 = "gopher_datadir:_gopher.log"
$ if P2 .eqs. "" then P2 = "sys$output"
$ if f$edit( "''P2'", "UPCASE") .eqs. "SYS$OUTPUT"
$ then 
$	MRF = "N"			! screen format
$ else					! else output to file
$	MRF = "Y"			! machine readable format
$ endif
$ if f$search("''P1'") .nes. "" then goto procfile
$ write sys$output "File ''P1' not found - no report generated."
$ exit
$!
$ procfile:
$  DTM = f$cvtime( "", "ABSOLUTE", "DATETIME")
$  gawk "-F " -f'reports_dir'rpt_uses_per_day.gawk -v mfmt='MRF' -v dtm="''DTM'" -- 'P1' >'P2'
$!
$ endprog:
