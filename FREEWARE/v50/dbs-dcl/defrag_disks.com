$ ! Procedure:	DEFRAG_DISKS.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ set noon
$ on control_y then goto bail_out
$ say = "write sys$output"
$ if (f$search("SYS$SYSTEM:DFU.EXE") .eqs. "")
$   then
$   say "%DFU is not on this system, please install it . . ."
$ else
$ dc$_disk = 1
$ dfu = "$sys$system:dfu"
$ run dbslibrary:get_disk_devices
$ options = f$edit(P1,"COLLAPSE,UPCASE")
$ if (options .eqs. "")
$   then
$   options = "REPORT_ONLY"
$ endif !(options .eqs. "")
$ options_len = f$length(options)
$ do_defrag = 0
$ do_report_only = 0
$ do_search_only = 0
$ do_selective = 0
$ if (options .eqs. f$extract(0,options_len,"DEFRAGMENT"))
$   then
$   do_defrag = 1
$ else
$ if (options .eqs. f$extract(0,options_len,"REPORT_ONLY"))
$   then
$   do_report_only = 1
$ else
$ if (options .eqs. f$extract(0,options_len,"SEARCH_ONLY"))
$   then
$   do_report_only = 1
$   do_search_only = 1
$ endif !(options .eqs. f$extract(0,options_len,"SEARCH_ONLY"))
$ endif !(options .eqs. f$extract(0,options_len,"REPORT_ONLY"))
$ endif !(options .eqs. f$extract(0,options_len,"DEFRAGMENT"))
$ if (.not. (do_defrag .or. do_report_only .or. do_search_only))
$   then
$   do_report_only = 1
$   disk_selection = options + "," + f$edit(P2,"COLLAPSE,UPCASE")
$ else
$ disk_selection = f$edit(P2,"COLLAPSE,UPCASE")
$ endif !(.not. (do_defrag .or. do_report_only .or. do_search_only))
$ if (disk_selection .nes. "")
$   then
$   disk_selection = disk_selection + ",''P3',''P4',''P5',''P6',''P7',''P8'"
$   disk_selection = f$edit(disk_selection,"COLLAPSE,UPCASE")
$   do_selective = 1
$ endif !(disk_selection .nes. "")
$ say "%Turning on BYPASS privilege"
$ set process/privilege=(all,bypass)
$ say ""
$ if (do_selective)
$   then
$   gosub do_selected_disks
$ else
$ gosub do_all_disks
$ endif !(do_selective)
$ say "---- finished -----------------"
$ say "%Turning off BYPASS privilege"
$ set process/privilege=(all,nobypass)
$ endif !(f$search("SYS$SYSTEM:DFU.EXE") .eqs. "")
$bail_out:
$ !'f$verify(__vfy_saved)'
$ exitt 1
$
$do_all_disks:
$ count = 0
$a_loop:
$   count = count + 1
$   device_name = f$trnlnm("DISK_DEVICE_''count'","LNM$PROCESS_TABLE")
$   if (device_name .eqs. "") then goto end_a_loop
$   if (.not. f$getdvi(device_name, "EXISTS")) then goto a_loop
$   if (.not. f$getdvi(device_name, "MNT")) then goto a_loop
$   if (f$getdvi(device_name, "SHDW_MEMBER")) then goto a_loop
$   gosub defrag_a_disk
$ goto a_loop
$end_a_loop:
$exit_do_all_disks:
$ return
$
$do_selected_disks:
$ count = 0
$s_loop:
$   device_name = f$element(count,",",disk_selection)
$   count = count + 1
$   if (device_name .eqs. "") then goto s_loop
$   if (device_name .eqs. ",") then goto end_s_loop
$   if (.not. f$getdvi(device_name, "EXISTS")) then goto s_loop
$   if (f$getdvi(device_name,"DEVCLASS") .ne. dc$_disk) then goto s_loop
$   if (.not. f$getdvi(device_name, "MNT")) then goto s_loop
$   if (f$getdvi(device_name, "SHDW_MEMBER")) then goto s_loop
$   gosub defrag_a_disk
$ goto s_loop
$end_s_loop:
$exit_do_selected_disks:
$ return
$
$defrag_a_disk:
$ devnam = f$getdvi(device_name, "DEVNAM")
$ filename = devnam - "_" - "_" - ":"
$ gosub create_time_stamp
$ if (do_report_only)
$   then
$   pre_report_file = filename + ".REPORT-''time_stamp'"
$ else
$ pre_report_file = filename + ".BEFORE-DFU-''time_stamp'"
$ endif !(do_report_only)
$ post_report_file = filename + ".AFTER-DFU-''time_stamp'"
$ if (do_search_only)
$   then
$   search_file = filename + ".SEARCH-''time_stamp'"
$ else
$ search_file = filename + ".SEARCH-BEFORE-''time_stamp'"
$ endif !(do_search_only)
$ post_search_file = filename + ".SEARCH-AFTER-''time_stamp'"
$ say "---- ''filename' -----------------------"
$ dfu report 'devnam'/output='pre_report_file'
$ if (do_defrag .or. do_search_only)
$   then
$   dfu search 'devnam'/fragment=minimum=7/output='search_file'
$ endif !(do_defrag .or. do_search_only)
$ if (do_defrag)
$   then
$   dfu defrag/besttry @'search_file'
$   dfu search 'devnam'/fragment=minimum=7/output='post_search_file'
$   dfu report 'devnam'/output='post_report_file'
$ endif !(do_defrag)
$ purgee/nolog 'filename'.*
$exit_defrag_a_disk:
$ return
$
$create_time_stamp:
$ time_stamp = f$cvtime() - "-" - "-" - " " - ":" - ":" - "."
$ time_stamp = f$extract(0,8,time_stamp) + "-" + f$extract(8,4,time_stamp)
$ return
$ !+==========================================================================
$ !
$ ! Procedure:	DEFRAG_DISKS.COM
$ !
$ ! Purpose:	To do a defrag pass on all mounted disks.
$ !
$ ! Parameters:
$ !	[P1]	Type of run - DEFRAGMENT or REPORT_ONLY (default).
$ !   [P2..P8]	Optional list of disks to use.  Default is to use all disks.
$ !
$ ! History:
$ !		20-Oct-1995, DBS; Version V1-001
$ !  	001 -	Original version.
$ !-==========================================================================
