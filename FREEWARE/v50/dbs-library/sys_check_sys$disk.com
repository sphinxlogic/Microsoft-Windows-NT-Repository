$ ! Procedure:	SYS_CHECK_SYS$DISK.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ set noon
$ say = "write sys$output"
$ sys_disk = "SYS$DISK"
$ free_threshold = 500
$ disk_read_only = f$getdvi(sys_disk, "swl")
$ disk_mounted = f$getdvi(sys_disk, "mnt")
$ disk_available = f$getdvi(sys_disk, "avl")
$ free_blocks = f$getdvi(sys_disk, "freeblocks")
$ if ((.not. disk_mounted) .or. (.not. disk_available) .or. (disk_read_only) -
		.or. (free_blocks .lt. free_threshold))
$	then
$	say "%CHKSYS$DISK-W-PROBLEM, current default disk is unavailable or full"
$	say "-CHKSYS$DISK-I-SHIFTING, setting default to SYS$MANAGER"
$	set default sys$manager
$ endif !((.not. disk_mounted) .or. (.not. disk_available) -
$bail_out:
$ !'f$verify(__vfy_saved)'
$ exit
$ !+==========================================================================
$ !
$ ! Procedure:	SYS_CHECK_SYS$DISK.COM
$ !
$ ! Purpose:	This procedure will check that SYS$DISK is available and has
$ !		some free space on it, if not it will set default to
$ !		sys$manager.  This is called by those "system" procedures that
$ !		need to create temporary files.
$ !
$ ! Parameters:
$ !
$ ! History:
$ !		28-Jul-1990, DBS; Version V1-001
$ !  	001 -	Original version.
$ !-==========================================================================
