$ ! Procedure:	CHECK_BATCH_JOBS.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ procedure = f$element(0,";",f$environment("PROCEDURE"))
$ procedure_name = f$parse(procedure,,,"NAME")
$ location = f$parse(procedure,,,"DEVICE","NO_CONCEAL") -
                + f$parse(procedure,,,"DIRECTORY","NO_CONCEAL") - "]["
$ set noon
$ on control_y then goto bail_out
$ vax = (f$getsyi("HW_MODEL") .lt. 1024)
$ axp = (f$getsyi("HW_MODEL") .ge. 1024)
$ say = "write sys$output"
$ scsnode = f$edit(f$getsyi("scsnode"),"COLLAPSE,UPCASE")
$ check_batch_job_mail_recipient_lnm = "CHECK_BATCH_JOB_MAIL_RECIPIENT"
$ check_batch_job_interval_lnm = "CHECK_BATCH_JOB_INTERVAL"
$ check_batch_job_mail_recipient = f$trnlnm(check_batch_job_mail_recipient_lnm)
$ default_transport = f$trnlnm("RSTUFF_DEFAULT_TRANSPORT")
$ default_transport = f$edit(default_transport,"COLLAPSE,UPCASE")
$ if (default_transport .eqs. "") then default_transport = "DECNET"
$ if (default_transport .eqs. "IP")
$   then
$   default_mail_recipient = "hotline@onl3.on-line"
$ else
$ default_mail_recipient = "ONL3::HOTLINE"
$ endif
$ if (check_batch_job_mail_recipient .eqs. "")
$   then
$   mail_recipient = default_mail_recipient
$ else
$ mail_recipient = "''default_mail_recipient',''check_batch_job_mail_recipient'"
$ endif !(check_batch_job_mail_recipient .eqs. "")
$ check_batch_job_interval = f$trnlnm(check_batch_job_interval_lnm)
$ default_interval = "+0-01:00:00.00"
$ if (check_batch_job_interval .nes. "")
$   then
$   interval = "''check_batch_job_interval'"
$ else
$ interval = "''default_interval'"
$ endif !(check_batch_job_interval .nes. "")
$ submitt/queue=OLGA$BATCH/after="''interval'"/user=on_line -
	'procedure'/noprint/nokeep
$
$ call process_filespec sys$manager:common_batch_jobs.data
$ call process_filespec sys$manager:'scsnode'_batch_jobs.data
$ call process_filespec sys$manager:group_%%%_batch_jobs.data
$
$bail_out:
$ !'f$verify(__vfy_saved)'
$ exitt 1
$
$process_filespec: subroutine
$ set noon
$ filespec = f$edit(P1,"COLLAPSE,UPCASE")
$ if (filespec .eqs. "")
$   then
$   say "%No filename passed to PROCESS_FILESPEC"
$ else
$ if (f$locate("%",filespec) .ne. f$length(filespec))
$   then
$   call process_wildcard 'filespec'
$ else
$ if (f$locate("*",filespec) .ne. f$length(filespec))
$   then
$   call process_wildcard 'filespec'
$ else
$ call process_file 'filespec'
$ endif !(f$locate("*",filespec) .ne. f$length(filespec))
$ endif !(f$locate("%",filespec) .ne. f$length(filespec))
$ endif !(filespec .eqs. "")
$exit_process_filespec:
$ exitt 1
$ endsubroutine
$
$process_wildcard: subroutine
$ set noon
$ wildspec = f$edit(P1,"COLLAPSE,UPCASE")
$ if (wildspec .eqs. "")
$   then
$   say "%No filename passed to PROCESS_WILDCARD"
$ else
$loop:
$   filespec = f$search(wildspec,192837)
$   if (filespec .eqs. "") then goto end_loop
$   call process_file 'filespec'
$  goto loop
$end_loop:
$ endif !(wildspec .eqs. "")
$exit_process_wildcard:
$ exitt 1
$ endsubroutine
$
$process_file: subroutine
$ set noon
$ filespec = f$edit(P1,"COLLAPSE,UPCASE")
$ if (filespec .eqs. "") then goto no_filename
$ open/read/error=exit_process_file infile 'filespec'
$loop:
$   read/error=end_loop/end_of_file=end_loop infile info
$   info = f$edit(info,"COMPRESS,UPCASE,TRIM,UNCOMMENT")
$   if (info .eqs. "") then goto loop
$   call locate_job 'info'
$  goto loop
$end_loop:
$ goto exit_process_file
$no_filename:
$ say "%No filename passed to PROCESS_FILE"
$ goto exit_process_file
$exit_process_file:
$ close/nolog infile
$ exitt 1
$ endsubroutine
$
$locate_job: subroutine
$ set noon
$ display_job = "DISPLAY_JOB"
$ freeze_context = "FREEZE_CONTEXT,ALL_JOBS"
$ flags = "WILDCARD"
$ job_name = f$edit(P1,"COLLAPSE,UPCASE")
$ search_name = f$edit(P2,"COLLAPSE,UPCASE")
$ if (job_name .eqs. "") then goto no_job_name
$ if (search_name .eqs. "") then goto no_queue_name
$ cancel = f$getqui("CANCEL_OPERATION")
$ queue_name = f$getqui("DISPLAY_QUEUE","QUEUE_NAME",search_name,flags)
$ job_found = 0
$job_loop:
$   entry_name = f$getqui(display_job,"JOB_NAME",,"ALL_JOBS")
$   if (entry_name .eqs. "") then goto end_job_loop
$   if (entry_name .eqs. job_name) then job_found = 1
$  if (.not. job_found) then goto job_loop
$end_job_loop:
$ if (.not. job_found)
$   then
$   mail/subject="%Failed to locate job ''job_name' on ''scsnode'" -
	nla0: "''mail_recipient'"
$ endif !(job_found)
$ goto exit_locate_job
$no_job_name:
$ say "%No job name supplied in LOCATE_JOB"
$ goto exit_locate_job
$no_queue_name:
$ say "%No queue name supplied in LOCATE_JOB"
$ goto exit_locate_job
$exit_locate_job:
$ cancel = f$getqui("CANCEL_OPERATION")
$ exitt 1
$ endsubroutine
$ !+==========================================================================
$ !
$ ! Procedure:	CHECK_BATCH_JOBS.COM
$ !
$ ! Purpose:	
$ !
$ ! Parameters:
$ !
$ ! History:
$ !		01-Jul-1997, DBS; Version V1-001
$ !  	001 -	Original version.
$ !		07-Aug-1997, DBS; Version V1-002
$ !	002 -	Now driven by data files in SYS$MANAGER:.
$ !		06-Mar-1998, DBS; Version V1-003
$ !	003 -	Added an endsubroutine to locate_job - 'twas missing...
$ !		08-Oct-1999, DBS; Version V1-004
$ !	004 -	Added code to check default transport.
$ !		19-Oct-1999, DBS; Version V1-005
$ !	005 -	Changed hotline ip address.
$ !-==========================================================================
