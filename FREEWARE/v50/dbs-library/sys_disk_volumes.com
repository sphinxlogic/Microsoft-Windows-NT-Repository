$ ! Procedure:	SYS_DISK_VOLUMES.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy_saved = f$verify(0)
$ if (f$type('__vfy') .nes. "") then __vfy_saved = f$verify(&__vfy)
$ disk_checker__ = "dbslibrary:sys_check_sys$disk.com"
$ if (f$search(disk_checker__) .nes. "") then @'disk_checker__'
$	set noon
$	on control_y then goto bail_out
$	@dbslibrary:sys_delete_globals disk_root_*
$	mounted_disks = "''f$getjpi("", "pid")'_MOUNTED_DISKS.LIST_FILE"
$	show device/mounted d/output='mounted_disks'
$	open/read/error=bail_out disks 'mounted_disks'
$	read/error=end_find_loop/end=end_find_loop disks text
$	read/error=end_find_loop/end=end_find_loop disks text
$	read/error=end_find_loop/end=end_find_loop disks text
$	counter = 0
$find_loop:
$	read/error=end_find_loop/end=end_find_loop disks text
$	text = f$edit(text, "upcase, collapse")
$	text_len = f$length(text)
$	if (f$locate(":", text) .eq. text_len) then goto find_loop
$	device_name = f$element(0, ":", text)
$	if ((device_name) .eqs. "") .or. (device_name .eqs. ":") then -
$		goto find_loop
$	device_name = device_name + ":"
$	if (f$getdvi(device_name, "exists"))
$		then
$		device_name = f$getdvi(device_name,"fulldevnam")
$		device_root = f$getdvi(device_name,"rootdevnam")
$		disk_root_'counter' == ""
$		if (device_root .nes. "")
$			then
$			if (f$getdvi(device_root,"fulldevnam") .eqs. device_name)
$				then
$				disk_root_'counter' == device_name
$			endif !f$getdvi(device_root,"fulldevnam") .eqs.
$		endif !(device_root .nes. "")
$	endif !(f$getdvi(device_name, "exists"))
$	counter = counter + 1
$ goto find_loop
$end_find_loop:
$bail_out:
$	if (f$trnlnm("disks") .nes. "") then close/nolog disks
$	if (f$search(mounted_disks) .nes. "") then -
$		deletee/nolog 'mounted_disks';*
$ !'f$verify(__vfy_saved)'
$ exit
$ !+==========================================================================
$ !
$ ! Procedure:	SYS_DISK_VOLUMES.COM
$ !
$ ! Purpose:	Returns the device names of the currently mounted disks in a
$ !		series of symbols of the form disk_root_N where N starts at
$ !		zero and increases until there are no more disks.
$ !
$ ! Parameters:
$ !
$ ! History:
$ !		08-Feb-1990, DBS; Version V1-001
$ !  	001 -	Original version.  (Based on BCK_DISK_VOLUMES)
$ !		27-Mar-1990, DBS; Version V1-002
$ !	002 -	Modified to handle volume sets.
$ !		30-Jul-1990, DBS; Version V1-003
$ !	003 -	Added call to the disk check procedure.
$ !		02-Oct-1990, SJR; Version V1-004
$ !	004 -	Added check for device_root - remote mounts stuffed things up
$ !		15-Oct-1990, SJR; Version V1-005
$ !	005 -	Defined null disk_root_'counter' symbol before root check
$ !		24-Sep-1991, KEL; Version V1-006
$ !	006 -	Allow for DS devices (i.e. host-based shadowing)
$ !-==========================================================================
