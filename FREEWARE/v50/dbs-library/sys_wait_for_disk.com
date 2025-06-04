$ ! Procedure:	SYS_WAIT_FOR_DISK.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy_saved = f$verify(0)
$ if (f$type('__vfy') .nes. "") then __vfy_saved = f$verify(&__vfy)
$	set noon
$	say = "write sys$output"
$	dw_notarget = "%DISKWAIT-E-NOTARGET, no target disk was specified"
$	dw_insane   = "%DISKWAIT-I-INSANE, sanity count has been adjusted"
$	dw_notavail = "%DISKWAIT-E-NOTAVAIL, disk !AS is not available"
$	dw_problems = "-DISKWAIT-I-PROBLEMS, continuing . . . with problems"
$	dw_waiting  = "%DISKWAIT-I-WAITING, waiting for disk !AS"
$	dw_counting = "%DISKWAIT-I-COUNTING, still waiting for disk !AS" -
			+ " (count is !UL)"
$	diskwait__success = %X10000001
$	diskwait__failure = %X10000002
$	exit_status = diskwait__success
$	wait_time = "00:00:20.00"	! three "waits" per minute
$	default_sanity_count = 15	! five minutes
$	minimum_sanity_count = 0	! don't wait - don't call me...
$	maximum_sanity_count = 90	! thirty minutes
$	target = f$edit(P1, "collapse, upcase")
$	sanity_count = f$integer(f$edit(P2, "collapse, upcase"))
$	if (target .eqs. "") then goto no_target
$	if (sanity_count .eqs. "") then sanity_count = default_sanity_count
$	if ((sanity_count .le. minimum_sanity_count) -
			.or. (sanity_count .gt maximum_sanity_count))
$		then
$		say f$fao(dw_insane)
$		sanity_count = default_sanity_count
$	endif
$	on control_y then goto final_check
$	wait_count = 0
$loop:
$	if (.not. f$getdvi(target, "exists"))
$		then
$		wait_count = wait_count + 1
$		if (wait_count .gt. 1)
$			then
$			say f$fao(dw_counting, target, wait_count)
$		else
$		say f$fao(dw_waiting, target)
$		endif !(wait_count .gt. 1)
$		wait 'wait_time'
$	else
$	wait_count = sanity_count
$	endif !(.not. f$getdvi(target, "exists"))
$ if (wait_count .lt. sanity_count) then goto loop
$final_check:
$	if (.not. f$getdvi(target, "exists"))
$		then
$		say f$fao(dw_notavail, target)
$		say f$fao(dw_problems)
$		exit_status = diskwait__failure
$	endif !(.not. f$getdvi(target, "exists"))
$ goto bail_out
$no_target:
$	say f$fao(dw_notarget)
$	exit_status = diskwait__failure
$ goto bail_out
$bail_out:
$ !'f$verify(__vfy_saved)'
$ exit exit_status
$ !+==========================================================================
$ !
$ ! Procedure:	SYS_WAIT_FOR_DISK.COM
$ !
$ ! Purpose:	To wait around (within reason) for one of the disks on the
$ !		system.  This is due to a problem with the 6000 series
$ !		machines.
$ !
$ ! Parameters:
$ !	 P1	The name of the disk to wait for.
$ !	[P2]	The sanity count - how many attempts we want to make before
$ !		giving it a miss.  The default is 15 (5 minutes at 20 second
$ !		intervals).
$ !
$ ! History:
$ !		22-Aug-1989, DBS; Version V1-001
$ !  	001 -	Original version.
$ !		24-Aug-1989, DBS; Version V1-002
$ !	002 -	A bit of a cleanup and now set status on exit.
$ !-==========================================================================
