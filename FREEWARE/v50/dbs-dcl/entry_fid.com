$ ! Procedure:	ENTRY_FID.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ set noon
$ on control_y then goto bail_out
$ say = "write sys$output"
$ display_job = "DISPLAY_JOB"
$ display_file = "DISPLAY_FILE"
$ freeze_context = "FREEZE_CONTEXT,ALL_JOBS"
$ search_name = f$edit(P1,"COLLAPSE,UPCASE")
$ if (search_name .eqs. "") then search_name = "*"
$ queue_type = f$edit(P2,"COLLAPSE,UPCASE")
$ if (queue_type .eqs. "") then queue_type = "SYMBIONT"
$ valid_que_types = "\BATCH\GENERIC\PRINTER\SERVER\SYMBIONT\TERMINAL\"
$ flags = "WILDCARD"
$ count  = 0
$type_loop:
$   que_type = f$element(count, ",", queue_type)
$   if (que_type .eqs. ",") then goto end_type_loop
$   if (f$locate("\''que_type'\", valid_que_types) .eq. -
		f$length(valid_que_types)) then -
$ 	goto invalid_queue_type
$   flags = flags + ",''que_type'"
$   count = count + 1
$ goto type_loop
$end_type_loop:	
$ freeze = flags + ",FREEZE_CONTEXT"
$ do = f$getqui("CANCEL_OPERATION")
$queue_loop:
$   queue_name = f$getqui("DISPLAY_QUEUE","QUEUE_NAME",search_name,flags)
$   if (queue_name .eqs. "") then goto end_queue_loop
$   if (f$extract(0,3,queue_name) .eqs. "FAX") then goto queue_loop
$   queue_flags = f$getqui("DISPLAY_QUEUE","QUEUE_FLAGS" -
				,search_name,freeze_context)
$   if (f$getqui("DISPLAY_QUEUE","QUEUE_BATCH",search_name,freeze_context))
$     then
$     qtype = "batch "
$   else
$   if (f$getqui("DISPLAY_QUEUE","QUEUE_PRINTER",search_name,freeze_context))
$     then
$     qtype = "printer "
$   else
$   if (f$getqui("DISPLAY_QUEUE","QUEUE_TERMINAL",search_name,freeze_context))
$     then
$     qtype = "terminal "
$   else
$   if (f$getqui("DISPLAY_QUEUE","QUEUE_SERVER",search_name,freeze_context))
$     then
$     qtype = "server "
$   else
$   qtype = ""
$   endif
$   endif
$   endif
$   endif
$   if (f$getqui("DISPLAY_QUEUE","QUEUE_GENERIC",search_name,freeze_context))
$     then
$     qgeneric = "generic "
$   else
$   qgeneric = ""
$   endif
$   say "''queue_name'  ''qgeneric'''qtype' queue"
$   if (qtype .eqs. "") then goto end_job_loop
$job_loop:
$     entry_number = f$getqui(display_job,"ENTRY_NUMBER",,"ALL_JOBS")
$     if (entry_number .eqs. "") then goto end_job_loop
$     entry_filespec = f$getqui(display_file,"FILE_SPECIFICATION" -
					,,freeze_context)
$     entry_fid = f$getqui(display_file,"FILE_IDENTIFICATION" -
					,,freeze_context)
$     say "    ''entry_number'  ''entry_filespec'  ''entry_fid'"
$ goto job_loop
$end_job_loop:
$ goto queue_loop
$end_queue_loop:
$ goto bail_out
$invalid_queue_type:
$ say "%You have specified an invalid queue type"
$ goto bail_out
$bail_out:
$ !'f$verify(__vfy_saved)'
$ exitt 1
$ !+==========================================================================
$ !
$ ! Procedure:	ENTRY_FID.COM
$ !
$ ! Purpose:	
$ !
$ ! Parameters:
$ !
$ ! History:
$ !		09-Jul-1997, DBS; Version V1-001
$ !  	001 -	Original version.
$ !-==========================================================================
