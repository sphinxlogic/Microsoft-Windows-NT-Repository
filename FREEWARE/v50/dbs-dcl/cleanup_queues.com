$ ! Procedure:	CLEANUP_QUEUES.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ set noon
$ on control_y then goto bail_out
$ say = "write sys$output"
$ job_count = 0
$ display_job = "DISPLAY_JOB"
$ freeze_context = "FREEZE_CONTEXT,ALL_JOBS"
$ qui$m_queue_batch = 1
$ qui$m_queue_generic = 512
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
$ delta_time = f$edit(P3,"COLLAPSE,UPCASE")
$ if (delta_time .eqs. "") then delta_time = "+1-00:00:00.00"
$ deadline = f$cvtime(delta_time)
$ if (deadline .eqs. "") then goto bail_out
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
$job_loop:
$     entry_number = f$getqui(display_job,"ENTRY_NUMBER",,"ALL_JOBS")
$     if (entry_number .eqs. "") then goto end_job_loop
$     delete_job = 0
$     delete_reason = "*Unknown*"
$     if (f$getqui(display_job,"JOB_RETAINED",,freeze_context))
$	then
$	submitted_at = f$getqui(display_job,"SUBMISSION_TIME",,freeze_context)
$	submitted_at = f$cvtime(submitted_at)
$	if (submitted_at .lts. deadline)
$	  then
$	  delete_job = 1
$         delete_reason = "Retained"
$	endif !(submitted_at .lts. deadline)
$     else
$     if ((queue_flags .and. (qui$m_queue_batch .or. qui$m_queue_generic)) -
		.eq. 0)
$	then
$       if (f$getqui(display_job,"JOB_SIZE",,freeze_context) .eq. 0)
$         then
$         delete_job = 1
$         delete_reason = "Zero length"
$       endif !(f$getqui(display_job,"JOB_SIZE",,freeze_context) .eq. 0)
$     endif !((queue_flags .and. (qui$m_queue_batch .or. qui$m_queue_generic)) -
$     endif !(f$getqui(display_job,"JOB_RETAINED",,freeze_context))
$     if (delete_job)
$       then
$       say f$fao("Deleting entry !UL on !AS!ASqueue !AS (!AS)" -
		,entry_number,qgeneric,qtype,queue_name,delete_reason)
$       delete/entry='entry_number'
$       job_count = job_count + 1
$     endif !(delete_job)
$ goto job_loop
$end_job_loop:
$ goto queue_loop
$end_queue_loop:
$ goto bail_out
$invalid_queue_type:
$ say "%You have specified an invalid queue type"
$ goto bail_out
$bail_out:
$ say f$fao(" !UL job!%S deleted", job_count)
$ !'f$verify(__vfy_saved)'
$ exitt 1
$ !+==========================================================================
$ !
$ ! Procedure:	CLEANUP_QUEUES.COM
$ !
$ ! Purpose:	
$ !
$ ! Parameters:
$ !
$ ! History:
$ !		08-Dec-1995, DBS; Version V1-001
$ !  	001 -	Original version.
$ !-==========================================================================
