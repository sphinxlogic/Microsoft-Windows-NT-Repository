$ ! Procedure:	REPEAT.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$	on control_y then goto bail_out
$	page_size	= f$getdvi("TT","TT_PAGE")
$	refresh		= "goto restart"
$	restart		= "goto restart"
$	csi[0,8]	= %X1B
$	clear_screen	= csi + "[J"
$	define_scroll	= csi + "[2;''page_size'r"
$	full_screen	= csi + "[1;''page_size'r"
$	home_cursor	= csi + "[1;1H"
$	top_of_scroll	= csi + "[2;1H"
$	for_repeat	= csi + "[1;59HRepeat count is"
$	for_count	= csi + "[1;75H"
$	header_format	= "''home_cursor'Repeating !23AS at !3AS second" -
				+ " intervals''for_repeat' ."
$	default_timer	= 30
$	repeat_count	= 0
$	say = "write sys$output"
$	if (P1 .eqs. "") then P1 = default_timer
$	if (P2 .eqs. "") then -
$		read/prompt="Repeat what: "/error=go_on sys$command P2
$go_on:
$      	if (P2 .eqs. "") then P2 = "show process"
$	idle_time  = P1
$	if (f$integer(idle_time) .le. 1) .or. -
			(f$integer(idle_time) .gt. 255) then -
$		idle_time = default_timer
$	the_command = "''P2' ''P3' ''P4' ''P5' ''P6' ''P7' ''P8'"
$restart:
$	say "''home_cursor'''clear_screen'"
$	say f$fao(header_format, the_command, idle_time)
$	say "''define_scroll'"
$start:
$	if (repeat_count .gt. 0) then say "''for_count'''repeat_count'"
$	say "''top_of_scroll'''clear_screen'"
$	define/user_mode/nolog sys$input sys$command
$	'P2' 'P3' 'P4' 'P5' 'P6' 'P7' 'P8'
$	repeat_count = repeat_count + 1
$	now	= f$time()
$	cputim	= f$getjpi("", "CPUTIM")
$	seconds	= cputim/100
$	hunds	= cputim - seconds*100
$	minutes	= seconds/60
$	seconds	= seconds - minutes*60
$	hours	= minutes/60
$	minutes	= minutes - hours*60
$	cpu = f$fao("!2ZL:!2ZL:!2ZL.!2ZL", hours, minutes, seconds, hunds)
$	say " Waiting . . .               ''now'        cpu=''cpu'"
$wait_for_it:
$      	read/time_out='idle_time'/error=start/end_of_file=bail_out/prompt=" " -
		sys$command dummy
$ 	if (f$length(dummy) .eq. 0) then goto start
$!	say "''full_screen'''clear_screen'"
$	define/user_mode/nolog sys$input sys$command
$	'dummy'
$!	wait 00:00:05.00
$! goto restart
$ goto wait_for_it
$bail_out:
$	say "''full_screen'''clear_screen'"
$	say "  ''f$edit(f$environment("default"), "lowercase")'"
$	say " "
$ !'f$verify(__vfy_saved)'
$	exit
$ !+==========================================================================
$ !
$ ! Procedure:	REPEAT.COM
$ !
$ ! Purpose:	To repeat any command you want at regular intervals.
$ !
$ ! Parameters:
$ !	[P1]	The interval, in seconds, defaults to 30.
$ !	[P2	P2 through P8 are executed as one command
$ !	...	and can be anything you want - as long as it is
$ !	 P3]	valid - or you will just get lots of errors...
$ !
$ ! History:
$ !		25-Aug-1988, DBS; Version V1-001
$ !  	001 -	Original version.
$ !		02-Nov-1988, DBS; Version V1-002
$ !	002 -	Added display of current default on way out.
$ !		07-Nov-1988, DBS; Version V1-003
$ !	003 -	Changed handling of commands entered during wait.  I now wait
$ !		for a null string before continuing (or a time out).
$ !-==========================================================================
