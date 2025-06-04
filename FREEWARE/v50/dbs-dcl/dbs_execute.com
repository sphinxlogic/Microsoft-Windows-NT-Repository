$ ! Procedure:	SYS_EXECUTE.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$	set noon
$	__success = %X10000001
$	__failure = %X10000002
$	__exit_status = __success
$	exec_say      = "write sys$output"
$	exec_nothing  = "%EXEC-E-NOTHING, I have nothing to execute ???"
$	exec_notfound = "%EXEC-E-NOTFOUND, file !AS not found"
$	exec_starting = "%EXEC-I-EXECUTING, executing !AS"
$	exec_p1 = f$edit(P1, "collapse, upcase")
$!$	define/nolog execution_list 'f$environment("default")' -
$	define/nolog execution_list sys$disk:[] -
		,dbscom,dbsexe,dbslibrary -
		,sys$manager,sys$startup,sys$system
$	if (exec_p1 .eqs. "")
$		then
$		exec_say exec_nothing
$		__exit_status = __failure
$	else
$	exec_name = f$parse(exec_p1,,,"NAME","SYNTAX_ONLY")
$	com_file = "EXECUTION_LIST:''exec_name'.COM"
$!$	com_file = f$parse(exec_p1,com_file)
$!$ say "''com_file'"
$	exe_file = "EXECUTION_LIST:''exec_name'.EXE"
$!$	exe_file = f$parse(exec_p1,exe_file)
$!$ say "''exe_file'"
$	com_file = f$element(0, ";", f$search(com_file))
$!$ say "''com_file'"
$	if (com_file .nes. "")
$		then
$!$		exec_say f$fao(exec_starting, com_file)
$		@'com_file' "''P2'" "''P3'" "''P4'" "''P5'" -
				"''P6'" "''P7'" "''P8'"
$	else
$	exe_file = f$element(0, ";", f$search(exe_file))
$!$ say "''exe_file'"
$	if (exe_file .nes. "")
$		then
$!$		exec_say f$fao(exec_starting, exe_file)
$		define/user/nolog sys$input sys$command
$		runforeign = "$''exe_file'"
$		runforeign 'P2' 'P3' 'P4' 'P5' 'P6' 'P7' 'P8'
$	else
$	exec_say f$fao(exec_notfound, exec_p1)
$	__exit_status = __failure
$	endif !(exe_file .nes. "")
$	endif !(com_file .nes. "")
$	endif !(exec_p1 .eqs. "")
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
$ !		12-Sep-1989, DBS; Version V1-004
$ !	004 -	Added WMC$EXE and WMC$MANEXE to the list.
$ !		19-Feb-1990, DBS; Version V1-005
$ !	005 -	Added code to look for a .exe file if no procedure was found.
$ !		22-Aug-1991, DBS; Version V1-006
$ !	006 -	More or less rewritten to be even nicer than before.
$ !		03-Nov-1992, DBS; Version V1-007
$ !	007 -	Added dbsalias and dbsspecific to the list.
$ !-==========================================================================
