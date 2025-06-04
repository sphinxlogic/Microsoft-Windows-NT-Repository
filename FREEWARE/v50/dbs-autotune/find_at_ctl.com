$ ! Procedure:	FIND_AT_CTL.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ set noon
$ on control_y then goto bail_out
$ say = "write sys$output"
$ dt$_rrd40 = 53
$ dt$_rrd42 = 72
$ dev$m_mntverip = %X4000
$ dir_spec = "[AUTO_TUNE]"
$ display_files = 0
$ display_options = 0
$ count = 0
$p_loop:
$   count = count + 1
$   if (count .gt. 8) then goto end_p_loop
$   param = f$edit(P'count', "collapse,upcase")
$   if (param .eqs. "") then goto end_p_loop
$   param_len = f$length(param)
$   if (param .eqs. f$extract(0, param_len, "ALL"))
$	then
$	display_files = 1
$	display_options = 1
$   endif !(param .eqs. f$extract(0, param_len, "ALL"))
$   if (param .eqs. f$extract(0, param_len, "NONE"))
$	then
$	display_files = 0
$	display_options = 0
$   endif !(param .eqs. f$extract(0, param_len, "NONE"))
$   if (param .eqs. f$extract(0, param_len, "FULL"))
$	then
$	display_files = 1
$	display_options = 1
$   endif !(param .eqs. f$extract(0, param_len, "FULL"))
$   if (param .eqs. f$extract(0, param_len, "NOFULL"))
$	then
$	display_files = 0
$	display_options = 0
$   endif !(param .eqs. f$extract(0, param_len, "NOFULL"))
$   if (param .eqs. f$extract(0, param_len, "BRIEF"))
$	then
$	display_files = 0
$	display_options = 0
$   endif !(param .eqs. f$extract(0, param_len, "BRIEF"))
$   if (param .eqs. f$extract(0, param_len, "FILES")) then -
$	display_files = 1
$   if (param .eqs. f$extract(0, param_len, "NOFILES")) then -
$	display_files = 0
$   if (param .eqs. f$extract(0, param_len, "OPTIONS")) then -
$	display_options = 1
$   if (param .eqs. f$extract(0, param_len, "NOOPTIONS")) then -
$	display_options = 0
$ goto p_loop
$end_p_loop:
$ run olga$sys:get_disk_devices
$ disk_count = 0
$read_loop:
$   disk_count = disk_count + 1
$   logical = f$trnlnm("DISK_DEVICE_''disk_count'","LNM$PROCESS_TABLE")
$   if (logical .eqs. "") then goto end_read_loop
$   if ((f$getdvi(logical,"sts") .and. dev$m_mntverip) .ne. 0) then -
$	goto read_loop
$   if (.not. f$getdvi(logical, "mnt")) then goto read_loop
$   if (f$getdvi(logical, "shdw_member")) then goto read_loop
$   devtype = f$getdvi(logical, "devtype")
$   if ((devtype .eq. dt$_rrd40) .or. (devtype .eq. dt$_rrd42)) then -
$	goto read_loop
$   label = f$getdvi(logical, "volnam")
$   control_spec = "''logical'''dir_spec'AUTO_TUNE.CONTROL_FILE*"
$   if (f$search(control_spec,66152) .nes. "") then -
$	call process_disk 'control_spec'
$ goto read_loop
$end_read_loop:
$bail_out:
$ say f$fao("!78*-")
$ !'f$verify(__vfy_saved)'
$ exitt 1
$
$process_disk: subroutine
$ set noon
$ on control_y then goto exit_process_disk
$ control_spec = f$edit(P1, "collapse,upcase")
$ if (control_spec .eqs. "") then goto exit_process_disk
$ disk_file_count = 0
$ disk_block_count = 0
$ say f$fao("!/!78*-!/Control file(s) found on !AS!AS", logical, label)
$search_loop:
$   ctl_file = f$search(control_spec,41325)
$   if (ctl_file .eqs. "") then goto end_search_loop
$   where = f$parse(ctl_file,,,"directory")
$   __where = where - "[" - "]"
$   cdt = f$file_attributes(ctl_file, "cdt")
$   rdt = f$file_attributes(ctl_file, "rdt")
$   say f$fao("!16*-!/  !AS!/  Created !17AS (Modified !17AS)" -
			, where, cdt, rdt)
$   dir_file_count = 0
$   dir_block_count = 0
$   if (f$trnlnm("cf") .nes. "") then close/nolog cf
$   open/read/error=search_loop cf 'ctl_file'
$cf_loop:
$     read/end=end_cf_loop/error=end_cf_loop cf at_file
$     at_file = f$edit(at_file, "collapse,upcase,uncomment")
$     if (at_file .eqs. "") then goto cf_loop
$     if (f$extract(0,2,at_file) .eqs. "$E") then goto end_cf_loop
$     slash = f$locate("/", at_file)
$     options = ""
$     if (slash .lt. f$length(at_file)) then -
$       options = f$extract(slash,9999,at_file)
$     at_file = logical + at_file - options - "/"
$     if (f$search(at_file) .eqs. "") then goto cf_loop
$     alq = f$file_attributes(at_file, "alq")
$     dir_file_count   = dir_file_count + 1
$     disk_file_count  = disk_file_count + 1
$     dir_block_count  = dir_block_count + alq
$     disk_block_count = disk_block_count + alq
$     file_name = at_file - logical - __where
$     if (display_files) then -
$       say f$fao("!4* !40AS  !10UL Block!%S", file_name,alq)
$     if (display_options) then -
$       if (options .nes. "") then say f$fao("!6* Options !AS", options)
$   goto cf_loop
$end_cf_loop:
$   close/nolog cf
$   say f$fao("!/!10*> !UL File!%S, !UL Block!%S for !AS" -
			,dir_file_count, dir_block_count, where)
$ goto search_loop
$end_search_loop:
$   say f$fao("!/!10*> !UL File!%S, !UL Block!%S on !AS!AS" -
			,dir_file_count, dir_block_count, logical, label)
$exit_process_disk:
$ if (f$trnlnm("cf") .nes. "") then close/nolog cf
$ exitt 1
$ endsubroutine
