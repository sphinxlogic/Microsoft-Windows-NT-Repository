$! File created by DCL_DIET at  6-NOV-2000 07:13:19.79 from
$! DBS0:[OLGA.COM]GENERIC_JANITOR.COS;
$__vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$if (f$type('__vfy') .eqs. "") then __vfy = 0
$__vfy_saved = f$verify(&__vfy)
$procedure = f$element(0,";",f$environment("PROCEDURE"))
$procedure_name = f$parse(procedure,,,"NAME")
$location = f$parse(procedure,,,"DEVICE","NO_CONCEAL") -
+ f$parse(procedure,,,"DIRECTORY","NO_CONCEAL") - "]["
$set noon
$set message/facility/severity/identification/text
$on control_y then goto bail_out
$set process/privilege=all
$vax = (f$getsyi("HW_MODEL") .lt. 1024)
$axp = (f$getsyi("HW_MODEL") .ge. 1024)
$vms_version = f$edit(f$getsyi("VERSION"),"COLLAPSE,UPCASE")
$dc$_disk = 1
$dc$_tape = 2
$unsupported = "/34/53/53/129/130/131/132/133/134/135/136/139/140/"
$len_unsupported = f$length(unsupported)
$say = "write sys$output"
$err = "call reporter"
$log = "call reporter"
$facility = "BAGJANITOR"
$weekday = f$edit(f$cvtime(,,"WEEKDAY"),"UPCASE")
$err I STARTING "starting version V2-007 (20001106.1)"
$disk_getter = "OLGA$SYS:GET_DISK_DEVICES.EXE"
$scsnode = f$edit(f$getsyi("SCSNODE"),"COLLAPSE,UPCASE")
$cluster_member = f$getsyi("CLUSTER_MEMBER")
$alloclass = f$getsyi("ALLOCLASS")
$system_device = f$getdvi(f$trnlnm("SYS$SYSDEVICE"),"FULLDEVNAM")
$if (alloclass .eq. 0)
$then
$prefix = "_"
$else
$prefix = "$''alloclass'$"
$endif
$prefix2 = "''scsnode'$"
$P1 = f$edit(P1,"COLLAPSE,UPCASE")
$P2 = f$edit(P2,"COLLAPSE,UPCASE")
$if ((P1 .eqs. "XCLEANUPX") .and. (P2 .nes. ""))
$then
$call cleanup 'P2'
$else
$if (f$search(disk_getter) .eqs. "") then goto no_disk_getter
$run 'disk_getter'
$before_janitor = "SYS$MANAGER:''scsnode'_BEFORE_JANITOR.COM"
$after_janitor = "SYS$MANAGER:''scsnode'_AFTER_JANITOR.COM"
$call spawner 'before_janitor'
$set audit/server=new_log
$wait 00:00:10.00
$if (f$search("SYS$MANAGER:SECURITY.AUDIT$JOURNAL;-1") .nes. "")
$then
$rename SYS$MANAGER:SECURITY.AUDIT$JOURNAL;-1 -
SYS$MANAGER:SECURITY_LOG.'weekday'
$endif
$counter = 0
$loop:
$counter = counter + 1
$disk_device = f$trnlnm("DISK_DEVICE_''counter'","LNM$PROCESS_TABLE")
$if (disk_device .eqs. "") then goto end_loop
$if (.not. f$getdvi(disk_device,"EXISTS")) then goto loop
$if (.not. f$getdvi(disk_device,"MNT")) then goto loop
$if (f$getdvi(disk_device,"SHDW_MEMBER")) then goto loop
$disk_type = f$getdvi(disk_device,"DEVTYPE")
$if (f$locate("/''disk_type'/",unsupported) .lt. len_unsupported) then goto loop
$fulldevnam = f$getdvi(disk_device,"FULLDEVNAM")
$if (cluster_member) then -
$if (f$locate(prefix,fulldevnam) .eq. f$length(fulldevnam)) then goto loop
$skip_disk = f$search("''fulldevnam'[000000]SKIP.JANITOR")
$if (skip_disk .nes. "")
$then
$say f$fao("(!6%D !8%T) !15*< Skipping !AS !15*>",0,0,fulldevnam)
$goto loop
$endif
$call starter 'fulldevnam'
$goto loop
$end_loop:
$call system_cleanup
$call spawner 'after_janitor'
$goto bail_out
$no_disk_getter:
$err f nodiskget "unable to find <''disk_getter'>"
$endif
$bail_out:
$exitt 1+(0*'f$verify(__vfy_saved)')
$system_cleanup: subroutine
$set noon
$if (f$trnlnm("DVSRV_LOCATION") .nes. "")
$then
$deletee/nolog DVSRV_LOCATION:*.*LOG*;*
$endif
$if (f$trnlnm("SAFSRV_LOCATION") .nes. "")
$then
$deletee/nolog SAFSRV_LOCATION:*.*LOG*;*
$endif
$if (f$search("SYS$MANAGER:SMBD_STARTUP.LOG") .nes. "")
$then
$deletee/nolog SYS$MANAGER:SMBD_STARTUP.LOG;*
$endif
$definee/nolog sys$command OPA0:
$reply/enable
$reply/log
$reply/disable
$deassign sys$command
$exitt 1
$endsubroutine
$cleanup: subroutine
$set noon
$fulldevnam = f$edit(P1,"COLLAPSE,UPCASE")
$disk_device = fulldevnam - prefix - ":" - "_" - "_"
$disk_device = disk_device - prefix2
$say f$fao("(!6%D !8%T) !15*< Starting work on !AS !15*.",0,0,disk_device)
$say "$ purge/nolog ''fulldevnam'[000000...]/exclude=([SYSLOST...])"
$createe/directory 'fulldevnam'[SYSLOST]
$purgee/nolog 'fulldevnam'[000000...]/exclude=([SYSLOST...])
$deletee/nolog 'fulldevnam'[000000...]CONTROL_%%%%_%%%%%%.COM;*
$deletee/nolog 'fulldevnam'[%%%.FILES]%%%_%%%_SERVER.LOG*;*
$deletee/nolog 'fulldevnam'[%%%.SPOOL]*.*;*/created/before=today-5-00:00
$if (fulldevnam .eqs. system_device)
$then
$deletee/nolog 'fulldevnam'[CRITICAL_SHIT]*.*; -
/created/before=today-365-00:00
$endif
$say "$ analyze/disk/repair ''fulldevnam'"
$definee/user/nolog sys$output nla0:
$definee/user/nolog sys$error nla0:
$analyze/disk/repair 'fulldevnam'
$if (f$search("''fulldevnam'[000000]SYSLOST.DIR;1") .nes. "")
$then
$lostfiles = "''fulldevnam'[SYSLOST...]*.*;*"
$directory/size=all/width=file=38 'lostfiles'
$endif
$say "$ set volume ''fulldevnam'/rebuild=force"
$set volume 'fulldevnam'/rebuild=force
$set volume 'fulldevnam'/windows=32
$set volume 'fulldevnam'/nohighwater_marking
$say f$fao("(!6%D !8%T) !15*< Finished with !AS !15*.",0,0,disk_device)
$exit_cleanup:
$exitt 1
$endsubroutine
$starter: subroutine
$set noon
$fulldevnam = f$edit(P1,"COLLAPSE,UPCASE")
$disk_device = fulldevnam - prefix - ":" - "_" - "_"
$disk_device = disk_device - prefix2
$control_file = "''fulldevnam'[000000]JANITOR_FOR_''disk_device'.COM"
$log_file = "''fulldevnam'[000000]JANITOR_FOR_''disk_device'.LOG"
$process_name = f$extract(0,15,"Cleanup ''disk_device'")
$copyy/nolog nla0: 'control_file'
$openn/append/error=exit_starter control 'control_file'
$write control "$set noon"
$write control "$@''procedure' XCLEANUPX ''fulldevnam'"
$write control "$exitt 1"
$closee/nolog control
$err I PROCESS "creating process <''process_name'>"
$run/detached sys$system:loginout -
/authorize -
/process_name="''process_name'" -
/input='control_file' -
/output='log_file' -
/error=nla0:
$exit_starter:
$exitt 1
$endsubroutine
$reporter: subroutine
$set noon
$say "%''facility'-''P1'-''P2', ''P3'"
$exitt 1
$endsubroutine
$spawner: subroutine
$set noon
$thing = f$edit(P1,"COLLAPSE,UPCASE")
$if (thing .eqs. "") then goto exit_spawner
$if (f$search(thing) .eqs. "") then goto exit_spawner
$thing = f$element(0,";",thing)
$log i start "starting <''thing'>"
$spawnn/wait @'thing'
$log i end "finished <''thing'>"
$exit_spawner:
$exitt 1
$endsubroutine
