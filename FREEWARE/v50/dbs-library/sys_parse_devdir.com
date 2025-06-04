$ ! Procedure:	SYS_PARSE_DEVDIR.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy_saved = f$verify(0)
$ if (f$type('__vfy') .nes. "") then __vfy_saved = f$verify(&__vfy)
$	if (P1 .eqs. "") then goto no_symbol
$	if (P2 .eqs. "") then goto param_req
$	prs_param = P2
$	prs_dev   = f$parse(prs_param,,, "device", "no_conceal")
$	prs_dir   = f$parse(prs_param,,, "directory", "no_conceal")
$	'P1' == prs_dev + prs_dir - "]["
$ goto bail_out
$no_symbol:
$	write sys$output "%PARSE-E-NOSYMBOL, no symbol name"
$ goto bail_out
$param_req:
$	write sys$output "%PARSE-E-PARAMREQ, device or directory required"
$	'P1' == ""
$ goto bail_out
$bail_out:
$ !'f$verify(__vfy_saved)'
$ exit
$ !+==========================================================================
$ !
$ ! Procedure:	SYS_PARSE_DEVDIR.COM
$ !
$ ! Purpose:	To parse a device/directory specification and return a string
$ !		containing the physical device and real directory path,
$ !		i.e. to remove rooted logicals.
$ !
$ ! Parameters:
$ !	 P1 	Somewhere to store the result.
$ !	 P2	The string to parse, eg. USER:[BLOGGS.DEMO].
$ !
$ ! History:
$ !		17-Oct-1988, DBS; Version V1-001
$ !  	001 -	Original version.
$ !		24-Jan-1989, DBS; Version V1-002
$ !	002 -	Added verify stuff.
$ !		25-May-1989, DBS; Version V1-003
$ !	003 -	Modified to just use parse and introduced P2 to be a symbol
$ !		for the result (defaults to prs_result).
$ !		13-Jun-1989, DBS; Version V1-004
$ !	004 -	Changed the order of the parameters to conform to DEC's
$ !		recommended standard.
$ !-==========================================================================
