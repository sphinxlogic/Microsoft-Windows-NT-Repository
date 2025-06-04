$ ! Procedure:	DISK_USE.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$	set noon
$	on control_y then goto bail_out
$	say = "write sys$output"
$	selection = f$edit(P1, "collapse, upcase")
$	min_usage = f$integer(P2)
$	max_usage = f$integer(P3)
$	if (selection .eqs. "*") then selection = ""
$	if ((min_usage .lt. 0) .or. (min_usage .gt. 100)) then min_usage = 0
$	if ((max_usage .gt. 100) .or. (max_usage .lt. min_usage) -
		.or. (max_usage .eq. 0)) then max_usage = 100
$	run dbslibrary:get_disk_devices
$	bar[0,8]	= %xA8
$	norm[0,32]	= %x6D305B1B
$	bold[0,32]	= %x6D315B1B
$	blink[0,32]	= %x6D355B1B
$	inverse[0,32]	= %x6D375B1B
$	nodename = f$getsyi("nodename")
$	say " "
$	say "           Disk Utilization on node ''nodename' ''f$time()'"
$	say f$fao("!24<Disk Type Volume_label!>     +!50*-")
$	count = 0
$loop:
$	count = count + 1
$	device_name = f$trnlnm("DISK_DEVICE_''count'","LNM$PROCESS_TABLE")
$	if (device_name .eqs. "") then goto bail_out
$	if (.not. f$getdvi(device_name, "exists")) then goto loop
$	if (.not. f$getdvi(device_name, "mnt")) then goto loop
$	if (f$getdvi(device_name, "shdw_member")) then goto loop
$	volnam = f$getdvi(device_name, "volnam")
$	if (f$locate(selection, volnam) .eqs. f$length(volnam)) then goto loop
$	unit_number = f$getdvi(device_name, "unit")
$	media_name = f$getdvi(device_name, "media_name")
$	freeblocks = f$getdvi(device_name, "freeblocks")
$	maxblock   = f$getdvi(device_name, "maxblock")
$	percent    = (maxblock - freeblocks)*100/maxblock
$	if ((percent .lt. min_usage) .or. (percent .gt. max_usage)) then -
$		goto loop
$	if (percent .ge. 0) then percent_bar = f$fao("!#*''bar'", percent/2)
$	if (percent .lt. 0) then percent_bar = bold + "Device error"
$	if (percent .ge. 80) then percent_bar = bold + percent_bar
$!***	if (percent .ge. 50) then percent_bar = bold + percent_bar
$	if (percent .ge. 90) then percent_bar = blink + percent_bar
$!***	if (percent .ge. 70) then percent_bar = blink + percent_bar
$	percent_bar = percent_bar + norm
$	say f$fao("!24<!4UL !AS !AS!> !3UL%|!AS" -
		,unit_number, media_name, volnam, percent, percent_bar)
$ goto loop
$bail_out:
$	say f$fao("!24< !>     +!50*-")
$ !'f$verify(__vfy_saved)'
$ exit
$ !+==========================================================================
$ !
$ ! Procedure:	DISK_USE.COM
$ !
$ ! Purpose:	To give a graphic display of disk usage, highlighting those
$ !		disks that are more than 70% full.
$ !
$ ! Parameters:
$ !	[P1]	Option string to match in the disk label.
$ !	[P2]	Optional minimum percentage for selection (default to 0).
$ !	[P3]	Optional maximum percentage for selection (default to 100).
$ !
$ ! History:
$ !		16-May-1989, DBS; Version V1-001
$ !  	001 -	Original version. (Based on a procedure from a DEC Prof.)
$ !		15-Sep-1989, DBS; Version V1-002
$ !	002 -	(Actually greater than 002...) Added a show time command at
$ !		the start of the display.
$ !		27-Jul-1990, DBS; Version V1-003
$ !	003 -	Added P2 and P3 to allow selection by percentage ranges.
$ !		30-Jul-1990, DBS; Version V1-004
$ !	004 -	Added call to the disk check procedure.
$ !		19-Oct-1990, DBS; Version V1-005
$ !	005 -	Modified to call the GET_DISK_DEVICES program.
$ !		10-Sep-1991, DBS; Version V1-006
$ !	006 -	Added code to ignore shadow set members.
$ !-==========================================================================
