$ ! Procedure:	SYS_DELAY.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy_saved = f$verify(0)
$ if (f$type('__vfy') .nes. "") then __vfy_saved = f$verify(&__vfy)
$	set noon
$	on control_y then goto bail_out
$	delay_logical = f$edit(P1, "upcase, collapse")
$	if (delay_logical .eqs. "") then delay_logical = "DELAY"
$	text = "''P2'"
$	say = "write sys$output"
$	time_chunk = "00:00:20.00"	! use 20 second chunks
$	multiplier = 3			! that's how many chunks per minute
$	first_time = -1
$	delay_counter = 0
$	trnlnm = "''f$trnlnm(delay_logical)'"
$	if (f$extract(0, 1, trnlnm) .eqs. "@")
$		then
$		gosub use_procedure
$	else
$	gosub use_delay
$	endif !(f$extract(0, 1, trnlnm) .eqs. "@")
$bail_out:
$	if ((text .nes. "") .and. (.not. first_time)) then -
$		say " . . . Continuing"
$ !'f$verify(__vfy_saved)'
$ exit
$
$use_delay:
$loop:
$	delay_value = f$trnlnm(delay_logical)
$	if (delay_value .eq. 0) then goto end_loop
$	if (first_time .and. (text .nes. "")) then say "''text'"
$	if (first_time) then first_time = (.not. first_time)
$	if (delay_value .lt. 0) then delay_limit = (.not. delay_value)
$	if (delay_value .gt. 0) then delay_limit = delay_value*multiplier
$	wait 'time_chunk'
$	delay_counter = delay_counter + 1
$ if (delay_counter .lt. delay_limit) then goto loop
$end_loop:
$ return
$
$use_procedure:
$	procedure = trnlnm - "@"
$	procedure = f$element(0, " ", procedure)
$	procedure = f$parse(procedure, ".COM",,,"syntax_only")
$	procedure = f$element(0, ";", procedure)
$	if (f$search(procedure) .eqs. "")
$		then
$		say "%SYSDELAY-E-NOTFOUND, procedure ''procedure' not found"
$	else
$	if (text .nes. "")
$		then
$		first_time = (.not. first_time)
$		say "''text'"
$	endif !(text .nes. "")
$	'trnlnm'
$	endif !(f$search(procedure) .eqs. "")
$ return
$ !+==========================================================================
$ !
$ ! Procedure:	SYS_DELAY.COM
$ !
$ ! Purpose:	To wait around until a predefined number of time chunks have
$ !		elapsed, which can be changed dynamically by redefining the
$ !		logical.
$ !		If the value in the logical is zero or positive, then there
$ !		will be at least a one minute delay (unless the value is
$ !		changed).  If the value is less than zero, the number of
$ !		chunks (as defined above) rather than minutes are counted.
$ !
$ ! Parameters:
$ !	[P1]	The logical which contains the delay count value.
$ !	[P2]	Optional text to display before we do the waiting.
$ !
$ ! History:
$ !		27-Jun-1989, DBS; Version V1-001
$ !  	001 -	Original version.
$ !		29-Jun-1989, DBS; Version V1-002
$ !	002 -	Moved some things around and now check for 0 value and skip
$ !		initial wait.
$ !		08-Aug-1989, DBS; Version V1-003
$ !	003 -	Moved things around again to suppress the messages if the
$ !		waiting is never done.
$ !		22-Aug-1989, DBS; Version V1-004
$ !	004 -	Added the ability to use another procedure to handle the delay
$ !		primarily for MEL and ODP disk problems.  The logical should
$ !		be in the format "@<file_spec>".
$ !		NOTE: these procedures should have some kind of sanity count
$ !		to prevent infinite loops.
$ !-==========================================================================
