$ ! Procedure:	SYS_SWAP_LOGICALS.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy_saved = f$verify(0)
$ if (f$type('__vfy') .nes. "") then __vfy_saved = f$verify(&__vfy)
$	set noon
$	say = "write sys$output"
$	P1 = f$edit(P1, "upcase, collapse")
$	P2 = f$edit(P2, "upcase, collapse")
$	P3 = f$edit(P3, "upcase, collapse")
$	if (P1 .eqs. "") then goto no_params
$	if (P2 .eqs. "") then P2 = "SAVED_LOGICAL"
$	if (P3 .eqs. "") then P3 = "LNM$SYSTEM_TABLE"
$	if (f$locate("LNM$", P3) .eq. f$length(P3)) then P3 = "LNM$" + P3
$	if (f$locate("_TABLE", P3) .eq. f$length(P3)) then P3 = P3 + "_TABLE"
$	table_name	= "''P3'"
$	qualifier	= "/" + table_name - "LNM$" - "_TABLE"
$	current_logical	= "''P1'"
$	new_logical	= "''P2'"
$	equivalence = f$trnlnm(current_logical, table_name)
$	if (equivalence .eqs. "") then goto start_loop
$	say "Moving ''current_logical' to ''new_logical' (''equivalence')"
$	define'qualifier' 'new_logical' 'equivalence'
$	deassign'qualifier' 'current_logical'
$start_loop:
$	count = 1
$loop:
$	current_logical = "''P1'_''count'"
$	new_logical = "''P2'_''count'"
$	equivalence = f$trnlnm(current_logical, table_name)
$	if (equivalence .eqs. "") then goto end_loop
$	say "Moving ''current_logical' to ''new_logical' (''equivalence')"
$	define'qualifier' 'new_logical' 'equivalence'
$	deassign'qualifier' 'current_logical'
$	count = count + 1
$ goto loop
$end_loop:
$ goto bail_out
$no_params:
$	say "%SWAPLOG-E-NOPARAM, required parameter is missing"
$ goto bail_out
$bail_out:
$ !'f$verify(__vfy_saved)'
$ exit
$ !+==========================================================================
$ !
$ ! Procedure:	SYS_SWAP_LOGICALS.COM
$ !
$ ! Purpose:	To save a set of logicals of the form <lognam>..<lognam>_n,
$ !		e.g. HLP$LIBRARY, HLP$LIBRARY_1 etc.  This was written mainly
$ !		for use during installations, where we don't want people
$ !		accessing the help libraries as we are updating them.
$ !
$ ! Parameters:
$ !	 P1	The current logical name.
$ !	[P2]	The new logical name, defaults to SAVED_LOGICAL.
$ !	[P3]	The logical name table to use, default to LNM$SYSTEM_TABLE.
$ !
$ ! History:
$ !		23-Feb-1989, DBS; Version V1-001
$ !  	001 -	Original version.
$ !-==========================================================================
