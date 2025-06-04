$ ! Procedure:	SYS_EXECUTE.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy_saved = f$verify(0)
$ if (f$type('__vfy') .nes. "") then __vfy_saved = f$verify(&__vfy)
$	set noon
$	__success = %X10000001
$	__failure = %X10000002
$	__exit_status = __success
$	exec_say      = "write sys$output"
$	exec_nothing  = "%EXEC-E-NOTHING, I have nothing to execute ???"
$	exec_notfound = "%EXEC-E-NOTFOUND, file !AS not found"
$	exec_starting = "%EXEC-I-EXECUTING, executing !AS"
$	exec_p1 = f$edit(P1, "collapse, upcase")
$	if (exec_p1 .eqs. "") then goto nothing
$	exec_file = f$parse(exec_p1,"dbsLIBRARY:.COM",,,"SYNTAX_ONLY") - ";"
$	if (f$search(exec_file) .nes. "") then goto execute_it
$	exec_file = f$parse(exec_p1,"dbsSTARTUP:.COM",,,"SYNTAX_ONLY") - ";"
$	if (f$search(exec_file) .nes. "") then goto execute_it
$	exec_file = f$parse(exec_p1,"SYS$MANAGER:.COM",,,"SYNTAX_ONLY") - ";"
$	if (f$search(exec_file) .nes. "") then goto execute_it
$	exec_file = f$parse(exec_p1,"SYS$STARTUP:.COM",,,"SYNTAX_ONLY") - ";"
$	if (f$search(exec_file) .nes. "") then goto execute_it
$	exec_file = f$parse(exec_p1,"SYS$SYSTEM:.COM",,,"SYNTAX_ONLY") - ";"
$	if (f$search(exec_file) .nes. "") then goto execute_it
$	goto not_found
$execute_it:
$ !	exec_say f$fao(exec_starting, exec_p1)
$	@'exec_file' "''P2'" "''P3'" "''P4'" "''P5'" "''P6'" "''P7'" "''P8'"
$ goto bail_out
$nothing:
$	exec_say exec_nothing
$ goto error_bail_out
$not_found:
$	exec_say f$fao(exec_notfound, exec_p1)
$ goto error_bail_out
$error_bail_out:
$	__exit_status = __failure
$bail_out:
$ !'f$verify(__vfy_saved)'
$ exitt __exit_status
$ !+==========================================================================
$ !
$ ! Procedure:	SYS_EXECUTE.COM
$ !
$ ! Purpose:	To execute a procedure and send some parameters to it.  This
$ !		does nice things like default to certain directories and look
$ !		to see if the file exists before trying to use it.
$ !
$ ! Parameters:
$ !	P1	The name of the procedure to execute.
$ !  [P2..P8]	Optional parameters to pass on to the procedure.
$ !
$ ! History:
$ !		14-Jun-1989, DBS; Version V1-001
$ !  	001 -	Original version.  Based on SYS_LAUNCH.
$ !		06-Jul-1989, DBS; Version V1-002
$ !	002 -	Made to look in dbslibrary as well as other places.
$ !		01-Sep-1989, DBS; Version V1-003
$ !	003 -	Modified to set $STATUS on exit.
$ !		09-Oct-1989, DBS; Version V1-004
$ !	004 -	Added SYS$STARTUP into the list of places we look.
$ !		28-Feb-1990, DBS; Version V1-005
$ !	005 -	Removed the sys$manager reference, it is included in
$ !		sys$startup.
$ !		25-Mar-1990, DBS; Version V1-006
$ !	006 -	Replaced sys$manager reference...?
$ !-==========================================================================
