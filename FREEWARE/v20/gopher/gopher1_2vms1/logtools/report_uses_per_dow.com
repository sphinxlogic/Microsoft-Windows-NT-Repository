$! report number of uses per day of week of gopher
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
$  T1 = F$GETJPI("","PID") + F$CVTIME() - "-" - "-" - " " - ":" - ":" - "."
$  T1 = T1 + ".TMP"
$  gawk -f'reports_dir'day_first.gawk -- 'P1' > 'T1'
$  S1 = "sort_"+"''T1'"
$  sort 'T1' 'S1'
$  DTM = f$cvtime( "", "ABSOLUTE", "DATETIME")
$  gawk -f'reports_dir'rpt_uses_per_dow.gawk -v mfmt='MRF' -v dtm="''DTM'" -- 'S1' >'P2'
$!
$ endprog:
$  delete 'T1';*
$  delete 'S1';*
