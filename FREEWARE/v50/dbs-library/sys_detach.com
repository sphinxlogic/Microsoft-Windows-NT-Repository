$ ! Procedure:	SYS_DETACH.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$	set noon
$	say = "write sys$output"
$	wc  = "write control"
$	input_spec = f$edit(P1, "collapse, upcase")
$	if (input_spec .eqs. "") then goto no_thing
$	input_spec = f$parse(input_spec, ".COM")
$	input_spec = f$search(input_spec, 191817)
$	if (input_spec .eqs. "") then goto no_thing
$	output_spec = "SYS$LOGIN:" -
		+ f$parse(input_spec,,, "name", "syntax_only") + ".LOG"
$	output_spec = f$parse(output_spec,,,, "syntax_only")
$	index = f$fao("!4XL" -
		,f$getsyi("maxprocesscnt")-f$getjpi("", "proc_index"))
$	now = f$cvtime() - "-" - "-" - " " - ":" - ":" - "."
$	now = f$extract(8, 6, now)
$	username = f$getjpi("", "username")
$	controller = "SYS$LOGIN:CONTROL_''index'_''now'.COM"
$	controller = f$parse(controller,,,, "syntax_only")
$	jobname = f$parse(controller,,, "name", "syntax_only")
$	open/write control 'controller'
$	wc "$ set noverify"
$	wc "$ set noon"
$	wc "$ @''input_spec' ""''P2'"" ""''P3'"" ""''P4'"" ""''P5'"" " -
		,"""''P6'"" ""''P7'"" ""''P8'"""
$	wc "$! reply/bell/username=''username' ""  ''jobname' is complete"""
$	wc "$ delete ''controller'"
$	wc "$exit"
$	close/nolog control
$	say "%DETACH-I-CONTROL, control via ''controller'"
$	say "-DETACH-I-INPUT, input is from ''input_spec'"
$	say "-DETACH-I-OUTPUT, output is to ''output_spec'"
$	saved_message = f$environment("message")
$	set message/facility/severity/identification/text
$	run/detach sys$system:loginout -
		/authorize -
		/input='controller' -
		/output='output_spec'
$	set message'saved_message'
$ goto bail_out
$no_thing:
$	say "%DETACH-I-NOTHING, nothing to detach"
$ goto bail_out
$bail_out:
$ !'f$verify(__vfy_saved)'
$ exit
$ !+==========================================================================
$ !
$ ! Procedure:	SYS_DETACH.COM
$ !
$ ! Purpose:	To fire up a procedure as a detached process.
$ !
$ ! Parameters:
$ !	 P1	The name of the procedure to execute - required.
$ !   [P2..P8]	Optional parameters to pass on to the procedure in P1.
$ !
$ ! History:
$ !		14-Sep-1989, DBS; Version V1-001
$ !  	001 -	Original version.
$ !		15-Sep-1989, DBS; Version V1-002
$ !  	002 -	Changed to create a controlling procedure which is the one
$ !		that is passed to loginout for input.  This will allow
$ !		parameters to be passed to the real procedure that is to be
$ !		executed.
$ !		15-May-1990, DBS; Version V1-003
$ !	003 -	Renamed to SYS_DETACH and moved into dbslibrary.
$ !-==========================================================================
