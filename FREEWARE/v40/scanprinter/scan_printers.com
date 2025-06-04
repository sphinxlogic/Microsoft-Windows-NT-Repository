$! 'f$verify(0)'
$!**********************************************************************
$!                    SCAN_PRINTERS.COM        V2.6
$!    Procédure permettant de surveiller les queues d'impression
$! 		(Historique en fin de procédure)
$!**********************************************************************
$!
$ on error then goto stop_process
$ on warning then goto stop_process
$!
$ scan_table=f$trnlnm("SCNPRT_TABLE","LNM$SYSTEM_DIRECTORY",,,,"TABLE")
$ say      :== write/error=err_logfile sys$output
$ deftable :==  if scan_table then -
                define/system/nolog/executive/table=SCNPRT_TABLE 
$ deftable SCNPRT$STATE "STARTING"
$! 
$ scan_reqclass = f$trnlnm("SCNPRT$REQUEST_CLASS","SCNPRT_TABLE")
$ if scan_reqclass.eqs."" then -
              scan_reqclass = "PRINTER"
$!
$ set output_rate = 00:00:30
$ time = f$time()			! Getting time stamp
$ curnode = f$getsyi("nodename")        ! 3 lines for getting node name !!!
$ if curnode .eqs. "" then curnode = f$edit(f$getsyi("scsnode"),"collapse")
$ if curnode .eqs. "" then curnode = f$trnlnm("sys$node") - "::" - "-"
$ clusternode = f$trnlnm("sys$cluster_node") -"::"  ! Getting cluster name
$ if clusternode .eqs. "" then clusternode=curnode
$ scan_version="V2.6"			! Current version
$ request/to=('scan_reqclass') "%SCNPRT-I-STARTING,  Starting at ''time' on node ''curnode'"
$ say " SCAN_PRINTERS - ''scan_version' - Starting at ''time' on node ''curnode'"
$ if f$getsyi("cluster_member").and. curnode.nes.clusternode then -
  say "                                  Member of VAXCluster ''clusternode'"
$!
$! Check to see if process has enough privs.
$ if scan_table 
$ then			! We need SYSNAM for modifying SCNPRT_TABLE
$     scan_privs = "SYSNAM,SYSPRV,OPER,TMPMBX,NETMBX"
$ else
$     scan_privs = "SYSPRV,OPER,TMPMBX,NETMBX" ! No system log. name specifcation
$ endif
$ if .not. f$privilege(scan_privs)
$ then                   ! Sorry, not enough privs
$  req_message="%SCNPRT-E-NOPRIVS, Needing following privs to run : ''scan_privs'"
$  say "''req_message'"
$  request/to=('scan_reqclass') "''req_message'"
$  goto stop_process
$ endif
$!
$! Check to see difference in version between startup and this procedure
$ temp = f$edit(f$trnlnm("SCNPRT$VERSION","SCNPRT_TABLE"),"TRIM,COLLAPSE,UPCASE")
$ if temp.nes.scan_version
$ then
$  if temp.nes."" 	! Is SCNPRT$VERSION define ?
$  then
$    req_message= -
     "%SCNPRT-W-VERMIS, Startup version ''temp' and Process version ''scan_version' mismatch"
$    say "''req_message'"
$    request/to=('scan_reqclass') "''req_message'"
$  endif
$  deftable SCNPRT$VERSION "''scan_version'"
$ endif
$!
$ say ""
$!
$! Defining logical tests for testing opened files
$ stat_open :== -
   f$trnlnm("""temp_stat""","""LNM$PROCESS""",,,,"""TABLE_NAME""").eqs."""LNM$PROCESS_TABLE"""
$ mail_open :== -
   f$trnlnm("""temp_mail""","""LNM$PROCESS""",,,,"""TABLE_NAME""").eqs."""LNM$PROCESS_TABLE"""
$!
$! Defining f$fao strings parameters
$ fao_start_ok  = "%SCNPRT-I-STARQUE, Queue !AS (!AS) has been restarted"
$ fao_start_bad = "%SCNPRT-E-STARQUE, Queue !AS (!AS) cannot be restarted"
$ fao_hold_ok  = "%SCNPRT-I-RELEASE, Entry !SL in queue !AS has been released"
$ fao_hold_bad = "%SCNPRT-E-RELEASE, Entry !SL in queue !AS cannot be released"
$ fao_log       = "!/%SCNPRT-W-!AS, Queue !AS (!AS) !AS at !AS"
$ fao_req       = "%SCNPRT-W-!AS, Queue !AS (!AS) found !AS"
$ fao_subj      = -
 "ScnPrt !AS, Mail Report for !SL problem!0%C!1%C!%Es!%F on !SL found, pass !SL"
$ fao_job       = -
"%SCNPRT-I-STAT, !AS has !SL executing job!0%C!1%C!%Es!%F, !SL pending, !SL retained and !SL holding"
$ fao_pbstat    = "!/%SCNPRT-E-!AS, error !AS statistics file !AS"
$ fao_crestat   = "!/%SCNPRT-I-CRESTAT, Creating statistics file !AS"
$ fao_cpsmsg    = "CPS message was :!/!AS"
$ fao_dqsmsg    = "DQS message was :!/!AS"
$!
$ scan_nb_pass=0        ! Initializing number of scanning passes
$ BEGIN:		! Beginning of scanning pass
$!
$ deftable SCNPRT$LAST_SCAN_BEGIN "''f$time()'"	! Positionning beginning pass
$ deftable SCNPRT$STATE "SCANNING"              ! logical names
$ scan_nb_pass=scan_nb_pass + 1
$ deftable SCNPRT$SCANNING_PASS "''scan_nb_pass'" 
$!
$! Defining parameters from logicals or default setting
$ scan_exclude = f$trnlnm("SCNPRT$EXCLUSION_FILE","SCNPRT_TABLE")
$ scan_statistics = f$trnlnm("SCNPRT$STATISTICS_FILE","SCNPRT_TABLE")
$ scan_wrkdir = f$trnlnm("SCNPRT$WORK_DIRECTORY","SCNPRT_TABLE")
$ scan_restart = f$trnlnm("SCNPRT$RESTART_QUEUE","SCNPRT_TABLE")
$ scan_restart = f$edit(f$extract(0,1,scan_restart),"UPCASE,TRIM,COLLAPSE")
$ if scan_restart.eqs."" then scan_restart = "N"
$ scan_release = f$trnlnm("SCNPRT$RELEASE_HOLD","SCNPRT_TABLE")
$ scan_release = f$edit(f$extract(0,1,scan_release),"UPCASE,TRIM,COLLAPSE")
$ if scan_release.eqs."" then scan_release = "N"
$ scan_nocluster = f$trnlnm("SCNPRT$IGNORE_CLUSTER","SCNPRT_TABLE")
$ scan_nocluster = f$edit(f$extract(0,1,scan_nocluster),"UPCASE,TRIM,COLLAPSE")
$ if scan_nocluster.eqs."" then scan_nocluster = "Y"
$ scan_mailuser :== 'f$trnlnm("SCNPRT$MAIL","SCNPRT_TABLE")
$ scan_queuetype :== 'f$trnlnm("SCNPRT$QUEUE_TYPE","SCNPRT_TABLE")
$ scan_mailstop = f$trnlnm("SCNPRT$MAIL_ON_STOP","SCNPRT_TABLE")
$ if scan_queuetype.eqs."" then scan_queuetype="SYMBIONT"
$ scan_queuetype = f$edit(scan_queuetype,"UPCASE,TRIM,COLLAPSE")
$ scan_mailstalled = -
  f$integer(f$trnlnm("SCNPRT$MAIL_ON_STALLED","SCNPRT_TABLE"))
$ scan_priority = f$integer(f$trnlnm("SCNPRT$PRIORITY","SCNPRT_TABLE"))
$ base_priority = f$integer(f$getjpi("","PRIB"))
$ if scan_priority.eq.0 .or. scan_priority.gt.base_priority then -
              scan_priority = base_priority
$ scan_reqclass = f$trnlnm("SCNPRT$REQUEST_CLASS","SCNPRT_TABLE")
$ if scan_reqclass.eqs."" then -
              scan_reqclass = "PRINTER"
$ scan_hib_delay = f$trnlnm("SCNPRT$HIBERNATE_DELAY","SCNPRT_TABLE")
$ if scan_hib_delay.eqs."" 	! Hibernate delay processing
$ then
$   scan_hib_delay = "00:15:00"
$   deftable SCNPRT$HIBERNATE_DELAY "00:15:00"
$ else
$   define/user sys$error nl:
$   define/user sys$output nl:
$   temp = f$cvtime("''scan_hib_delay'","delta","time")
$   status = $status
$   deassign/user sys$output
$   deassign/user sys$error
$   if .not. status 
$   then 
$      scan_hib_delay = "00:15:00"
$      deftable SCNPRT$HIBERNATE_DELAY "00:15:00"
$   endif
$ endif
$ scan_exp_alt_date = f$trnlnm("SCNPRT$EXPECTED_ALT_DATE","SCNPRT_TABLE")
$ if scan_exp_alt_date .nes. ""	! expected alternate date processing  
$ then
$   define/user sys$error nl:
$   define/user sys$output nl:
$   temp = f$cvtime("''scan_exp_alt_date'","COMPARISON")
$   status = $status
$   deassign/user sys$output
$   deassign/user sys$error
$   if .not. status 
$   then 
$      scan_exp_alt_date =  f$cvtime("18:00:00","comparison")
$      deftable SCNPRT$EXPECTED_ALT_DATE "18:00:00"
$   else
$      scan_exp_alt_date = temp
$   endif
$   scan_alt_hib_delay = f$trnlnm("SCNPRT$ALT_HIBERNATE_DELAY","SCNPRT_TABLE")
$   if scan_alt_hib_delay.eqs."" 	! Alternate hibernate delay processing
$   then
$     scan_alt_hib_delay = scan_hib_delay
$     deftable SCNPRT$ALT_HIBERNATE_DELAY "''scan_hib_delay'"
$   else
$     define/user sys$error nl:
$     define/user sys$output nl:
$     temp = f$cvtime("''scan_alt_hib_delay'","delta","time")
$     status = $status
$     deassign/user sys$output
$     deassign/user sys$error
$     if .not. status 
$     then 
$        scan_alt_hib_delay =  scan_hib_delay
$        deftable SCNPRT$ALT_HIBERNATE_DELAY "''scan_hib_delay'"
$     endif
$   endif
$ endif
$ scan_spec_days = f$trnlnm("SCNPRT$SPECIAL_DAYS","SCNPRT_TABLE")
$ if scan_spec_days.nes."" 	! Special days hib. delay processing
$ then
$   scan_spec_hib_delay = f$trnlnm("SCNPRT$SPECIAL_HIBERNATE_DELAY","SCNPRT_TABLE")
$   if scan_spec_hib_delay.eqs."" 	! Special days hibernate delay processing
$   then
$     scan_spec_hib_delay = "23:59:59"
$     deftable SCNPRT$SPECIAL_HIBERNATE_DELAY "''scan_spec_hib_delay'"
$   else
$     define/user sys$error nl:
$     define/user sys$output nl:
$     temp = f$cvtime("''scan_spec_hib_delay'","delta","time")
$     status = $status
$     deassign/user sys$output
$     deassign/user sys$error
$     if .not. status 
$     then 
$        scan_spec_hib_delay =  "23:59:59"
$        deftable SCNPRT$SPECIAL_HIBERNATE_DELAY "''scan_spec_hib_delay'"
$     endif
$   endif
$ endif
$!
$ set proc/priority = 'f$integer(scan_priority)'  ! Modifying process priority
$ if scan_nocluster.eqs. "Y" 			  ! Cluster name processing ?
$ then
$ 	cluster = curnode
$ else
$ 	cluster = clusternode
$ endif
$ temp = f$getqui("CANCEL_OPERATION")           ! Clear Context
$ scan_pb = 0
$ scan_mail_pb = 0
$!
$ QUEUE_LOOP:			! Beginning of queues scanning loop
$!
$ Queuename = f$getqui("DISPLAY_QUEUE","QUEUE_NAME","*","''scan_queuetype'")
$ IF Queuename .EQS. "" then goto END  ! there are no more queues to find.
$!
$! Get informations on the current queue.
$ Stalled = -
       f$getqui("DISPLAY_QUEUE","QUEUE_STALLED","*","''scan_queuetype',FREEZE_CONTEXT")
$ Stopped = -
       f$getqui("DISPLAY_QUEUE","QUEUE_STOPPED","*","''scan_queuetype',FREEZE_CONTEXT")
$ Paused = -
       f$getqui("DISPLAY_QUEUE","QUEUE_PAUSED","*","''scan_queuetype',FREEZE_CONTEXT")
$ Hold_job = -
       f$getqui("DISPLAY_QUEUE","HOLDING_JOB_COUNT","*","''scan_queuetype',FREEZE_CONTEXT")
$ Queue_status = ""
$ IF Hold_job.ne.0 .and. scan_release.eqs."Y" THEN Queue_status = "holding"
$ IF Stalled THEN Queue_status = "stalled"
$ IF Stopped THEN Queue_status = "stopped"
$ IF Paused  THEN Queue_status = "paused"
$!
$! Continue in queues scanning if status of current queue is "normal"
$ If Queue_Status .eqs. "" 
$ then 
$   if f$type('queuename'_stalled_count).nes."" then delete/symbol/nolog -
                                               'queuename'_stalled_count
$   goto queue_loop
$ endif
$!
$ EXCLUSION_SEARCH:	! Search current queue in exclusion file if defined
$ if scan_exclude.nes.""
$ then
$    define/user sys$output nl:
$    define/user sys$error  nl:
$    search/noexact/nooutput/nolog 'scan_exclude' "#''queuename'#"
$    if $status.eqs."%X08D78053" then goto found_pb   ! Not found in exclusion file
$    if $status.nes."%X00000001" then goto excl_err   ! Exclusion file search error
$    goto queue_loop
$ endif
$! 
$ FOUND_PB:				! Beginning of problem processing
$   if queue_status.nes."holding" then scan_pb = scan_pb+1
$   If Queue_Status .eqs. "stalled"	! Increment stalled counter for
$   then 				! current queue 
$     if f$type('queuename'_stalled_count).nes."" 
$     then 
$       'queuename'_stalled_count = 'queuename'_stalled_count + 1
$     else
$       'queuename'_stalled_count = 1
$     endif
$   else
$     if f$type('queuename'_stalled_count).nes."" then delete/symbol/nolog -
                                                 'queuename'_stalled_count
$   endif                
$   if (.not. 'mail_open') .and. scan_mailuser.nes."" 
$   then
$     if queue_status.nes."stalled" 
$     then
$        gosub open_mail
$     else
$       if (scan_mailstalled.nes.0 .and.-
            'queuename'_stalled_count .ge. scan_mailstalled) then -
            gosub open_mail
$     endif
$   endif
$   scan_repuser = f$trnlnm("SCNPRT$MESSAGE","SCNPRT_TABLE")
$   Node = -
       f$getqui("DISPLAY_QUEUE","SCSNODE_NAME","*","''scan_queuetype',FREEZE_CONTEXT")
$   Device = -
       f$getqui("DISPLAY_QUEUE","DEVICE_NAME","*","''scan_queuetype',FREEZE_CONTEXT")
$   Pend_job = -
     f$getqui("DISPLAY_QUEUE","PENDING_JOB_COUNT","*","''scan_queuetype',FREEZE_CONTEXT")
$   Exec_job = -
     f$getqui("DISPLAY_QUEUE","EXECUTING_JOB_COUNT","*","''scan_queuetype',FREEZE_CONTEXT")
$   Reta_job = -
     f$getqui("DISPLAY_QUEUE","RETAINED_JOB_COUNT","*","''scan_queuetype',FREEZE_CONTEXT")
$   Node_string = ""
$   IF node .NES. "" THEN Node_string = f$fao("on !AS::!AS", node, device)
$   if f$locate("@",node_string) .lt. f$length(node_string) then gosub trans_aro
$   say f$fao(fao_log,f$edit(Queue_status,"UPCASE"),-
                             Queuename,Node_string,-
                             Queue_status,f$extract(0,16,f$cvtime()))
$   if exec_job.ne.0 .or. pend_job.ne.0 .or. reta_job.ne.0 .or. hold_job.ne.0 then -
        say f$fao(fao_job,queuename,exec_job,pend_job,reta_job,hold_job)
$   req_message = f$fao(fao_req,f$edit(Queue_status,"UPCASE"),-
                                   Queuename,Node_string,-
                                   Queue_status)
$   request/to=('scan_reqclass') "''req_message'"
$ if scan_repuser.nes."" then -
         reply/bell/nonotif/user=('scan_repuser') "''req_message'"
$ if scan_mailuser.nes."" .and. 'mail_open'
$ then 
$     if queue_status.nes."stalled" 
$     then
$        gosub write_mail
$     else
$       if (scan_mailstalled.nes.0 .and.-
            'queuename'_stalled_count .ge. scan_mailstalled) then - 
            gosub write_mail
$     endif
$ endif
$!
$! Restart queue if stopped and needed
$ if (queue_status.eqs."paused" .or. queue_status.eqs."stopped").and. -
      scan_restart.eqs."Y" 
$ then
$     on error then continue
$     on warning then continue
$     define/user sys$error  nl:
$     define/user sys$output nl:
$     start/queue 'queuename'
$     status = $status
$     on error then goto stop_process
$     on warning then goto stop_process
$     if status 
$     then
$       fao_start = fao_start_ok
$     else
$       fao_start = fao_start_bad
$     endif
$     req_message = f$fao(fao_start,Queuename,Node_string)
$     say "''req_message'"
$     request/to=('scan_reqclass') "''req_message'"
$     if scan_repuser.nes."" then -
      reply/bell/nonotif/user=('scan_repuser') "''req_message'"
$     if scan_mailuser.nes."" .and. 'mail_open' then -
      write temp_mail f$fao(fao_start,Queuename,Node_string)
$ endif
$!
$! Release holding jobs if activated
$ if hold_job.ne.0 .and. scan_release.eqs."Y" then gosub release_hold
$!
$ if scan_statistics.nes.""	! Go ahead in statistics collecting if activated
$ then
$  deftable SCNPRT$STATE "STATISTICS"
$  gosub stat_file
$  deftable SCNPRT$STATE "SCANNING"
$ endif
$!
$ GOTO QUEUE_LOOP 		! Continue in queues scanning loop
$!
$! All queues have been scanned, close statistics file, sending mail before
$! waiting
$ END:				! End of scanning pass
$   if 'stat_open' then close temp_stat
$   if 'mail_open' then gosub send_mail 
$!
$   deftable SCNPRT$LAST_SCAN_END "''f$time()'"
$   if scan_exp_alt_date.eqs."" .or. -   
       scan_exp_alt_date .ges. f$cvtime("","comparison") 
$   then
$       deftable SCNPRT$STATE "HIBERNATE"
$       wait 'scan_hib_delay'
$   else			! Wait alternate delay if it's time
$       deftable SCNPRT$STATE "HIB_ALT"
$       wait 'scan_alt_hib_delay'
$       if scan_spec_days.nes."" then gosub special_days ! Special days processing
$   endif
$   goto begin
$!
$ WRITE_MAIL:			! Writing record pb in mail file subroutine
$   if queue_status.nes."holding" then scan_mail_pb = scan_mail_pb+1
$   write temp_mail f$fao(fao_log,f$edit(Queue_status,"UPCASE"),-
                          Queuename,Node_string,-
                          Queue_status,f$extract(0,16,f$cvtime()))
$   scan_cps_message=f$trnlnm("CPS$''queuename'_''queue_status'",-
                               "LNM$SYSTEM_TABLE")
$   if scan_cps_message.nes."" 
$   then 
$   	scan_cps_message= -
        f$extract(1,f$length(scan_cps_message)-2,scan_cps_message)
        write temp_mail f$fao(fao_cpsmsg,scan_cps_message)
$   endif          
$   scan_dqs_message=f$trnlnm("DQS$STATUS_''queuename'",-
                               "LNM$SYSTEM_TABLE")
$   if scan_dqs_message.nes."" 
$   then 
$   	scan_dqs_message= -
        f$extract(1,f$length(scan_dqs_message)-2,scan_dqs_message)
        write temp_mail f$fao(fao_dqsmsg,scan_dqs_message)
$   endif          
$   if exec_job.ne.0 .or. pend_job.ne.0 .or. reta_job.ne.0 .or. -
    hold_job.ne.0 then -
    write temp_mail f$fao(fao_job,queuename,exec_job,pend_job,reta_job,hold_job)
$   if f$type('queuename'_stalled_count).nes."" then -
           delete/symbol/nolog 'queuename'_stalled_count
$   return
$!
$ RELEASE_HOLD:			! Release holding jobs subroutine
$   scan_enumb = f$getqui("display_job","entry_number",,"holding_jobs,all_jobs")
$   if scan_enumb.eqs."" then return
$   scan_pb = scan_pb + 1
$   on error then continue
$   on warning then continue
$   define/user sys$error nl:
$   define/user sys$output nl:
$   set entry/release 'scan_enumb'
$   status = $status
$   on error then goto stop_process
$   on warning then goto stop_process
$   if status
$   then
$      fao_hold = fao_hold_ok
$   else
$      fao_hold = fao_hold_bad
$   endif
$   req_message=f$fao(fao_hold,scan_enumb,queuename)
$   say "''req_message'"
$   request/to=('scan_reqclass') "''req_message'"
$   if scan_repuser.nes."" then -
    reply/bell/nonotif/user=('scan_repuser') "''req_message'"
$   if scan_mailuser.nes."" .and. 'mail_open' 
$   then 
$      write temp_mail f$fao(fao_hold,scan_enumb,queuename)
$      scan_mail_pb = scan_mail_pb + 1
$   endif
$   goto release_hold
$ return
$!
$ TRANS_ARO:
$ TRANS_ARO_LOOP:
$ aro=f$locate("@",node_string)
$ if aro.lt.f$length(node_string)
$ then 
$ 	node_string[aro*8,8]=%XE0			! à character
$ 	goto trans_aro_loop
$ endif
$ return
$!
$ OPEN_MAIL: 			! Opening mail file subroutine
$   if scan_mailuser.eqs."" then return
$   scan_pid = f$getjpi("","pid")
$   scan_mail_file:=="''f$parse("SCNPRT_MAIL_"+scan_pid+".DAT",-
                     scan_wrkdir,"SYS$SCRATCH")'"
$   if scan_mail_file.eqs."" then goto mail_open_err
$   open/write/err=mail_open_err temp_mail 'scan_mail_file'
$   return
$!
$ SEND_MAIL: 			! Sending mail file subroutine
$   if 'mail_open' then close temp_mail
$   if scan_mailuser.nes."" 
$   then
$     on error then goto send_mail_err
$     on warning then goto send_mail_err
$     deftable SCNPRT$STATE "MAILING"
$     mail-
        /subject=-
        "''f$fao(fao_subj,scan_version,scan_mail_pb,scan_pb,scan_nb_pass)'" -
        /Personal=-
        "Scan Printers Process ''scan_version' on ''curnode'" -
        'scan_mail_file' -
        'scan_mailuser'
$   endif
$   on error then goto stop_process
$   on warning then goto stop_process
$   if f$search(scan_mail_file) .nes."" then - 
             delete/nolog/noconfirm 'scan_mail_file'
$   return
$!
$ SPECIAL_DAYS:				! Special days processing routine
$ SPECIAL_DAYS_BEGIN:
$ scan_weekday=f$edit(f$extract(0,3,f$cvtime("TODAY",,"WEEKDAY")),"UPCASE")
$ if f$locate(scan_weekday,f$edit(scan_spec_days,"UPCASE")) .ne. -
		f$length(scan_spec_days)
$ then
$       deftable SCNPRT$STATE "HIB_SPECIAL"
$ 	wait 'scan_spec_hib_delay'
$       goto special_days_begin
$ endif
$ return
$!
$ WRITE_STAT_FILE:			! Writing record in stat. file sub.
$     on error then goto err_write_stat_file
$     on warning then goto err_write_stat_file
$  if 'stat_open'
$  then
$    fao_stat_str="!AS/!AS/!AS/!AS/!AS/!AS/!SL/!SL/!SL/!SL/!AS/!AS/!AS"
$    job_name=""
$    file_spec=""
$    user_name=""
$    processor=-
    f$getqui("display_queue","processor","*","''scan_queuetype',freeze_context")
$    form_name=-
    f$getqui("display_queue","form_name","*","''scan_queuetype',freeze_context")
$    lib_spec=-
    f$getqui("display_queue","library_specification","*","''scan_queuetype',freeze_context")
$    write/error=err_write_stat_file temp_stat f$fao(fao_stat_str,-
     f$extract(0,16,f$cvtime()),cluster,queuename,node,device,-
     f$edit(queue_status,"UPCASE"),exec_job,pend_job,reta_job,hold_job,-
     processor,form_name,lib_spec)
$    if exec_job.ne.0
$    then
$      job_name=-
    f$getqui("display_job","job_name",,"all_jobs,executing_jobs")
$      user_name=-
    f$getqui("display_job","username",,"freeze_context,all_jobs,executing_jobs")
$      scan_enumb=-
    f$getqui("display_job","entry_number",,"freeze_context,all_jobs,executing_jobs")
$      file_spec=-
    f$getqui("display_file","file_specification",,"freeze_context,all_jobs,executing_jobs")
$      fao_stat_str="!AS/!AS/!AS/!AS/!AS/!AS/!AS/!AS/!SL/!AS"
$      write/error=err_write_stat_file temp_stat f$fao(fao_stat_str,-
       f$extract(0,16,f$cvtime()),cluster,queuename,node,device,-
       "STAT",job_name,user_name,scan_enumb,file_spec)
$    endif
$  endif
$  goto end_write_stat_file
$ ERR_WRITE_STAT_FILE:			! Error writing in stat. file
$  req_message = f$fao(fao_pbstat,"WRITERR","writing in",scan_statistics)
$  say "''req_message'"
$  request/to=('scan_reqclass') "''req_message'"
$  if scan_mailuser.nes."" .and. 'mail_open' then -
   write temp_mail "''req_message'"
$  scan_statistics=""
$  close temp_stat
$ end_write_stat_file:
$ return
$!
$ STAT_FILE:				! Stat. processing subroutine
$  on error then continue
$  on warning then continue
$  if 'stat_open'
$  then 
$   gosub write_stat_file
$  else 
$    scan_statistics=f$parse(scan_statistics,scan_wrkdir,-
                     "SYS$SCRATCH:SCAN_PRINTERS.STAT",,"SYNTAX_ONLY")
$    if f$search("''scan_statistics'").eqs."" 
$    then
$     define/user sys$output nl:
$     define/user sys$error  nl:
$     create/nolog 'scan_statistics'
$     status = $status
$     if .not. status 
$     then
$       req_message = f$fao(fao_pbstat,"CREATERR","creating",scan_statistics)
$       say "''req_message'"
$       request/to=('scan_reqclass') "''req_message'"
$       if scan_mailuser.nes."" .and. 'mail_open' then -
        write temp_mail "''req_message'"
$       scan_statistics=""
$     else
$       req_message = f$fao(fao_crestat,scan_statistics)
$       say "''req_message'"
$       request/to=('scan_reqclass') "''req_message'"
$       if scan_mailuser.nes."" .and. 'mail_open' then -
        write temp_mail "''req_message'"
$       gosub open_stat_file
$     endif
$    else
$     gosub open_stat_file
$    endif
$  endif
$  on error then goto stop_process
$  on warning then goto stop_process
$ return
$!
$ OPEN_STAT_FILE:			! Opening stat. file subroutine
$  on error then goto stat_open_err
$  on warning then goto stat_open_err
$  open/append/share=write/error=stat_open_err temp_stat 'scan_statistics'
$  gosub write_stat_file
$  goto end_open_stat
$ STAT_OPEN_ERR:			! Error opening stat. file
$  req_message= f$fao(fao_pbstat,"OPENERR","opening",scan_statistics)
$  say "''req_message'"
$  request/to=('scan_reqclass') "''req_message'"
$  if scan_mailuser.nes."" .and. 'mail_open' then -
   write temp_mail "''req_message'"
$  scan_statistics=""
$ end_open_stat:
$ return
$!
$ ERR_LOGFILE:				! Error writing in log file
$  deftable SCNPRT$STATE "STOPPING"
$  request/to=('scan_reqclass') -
   "%SCNPRT-F-OPENERR Error writing in Log file"
$  goto stop_process
$!
$ EXCL_ERR:				! Error accessing exclusion file
$  deftable SCNPRT$STATE "STOPPING"
$  say "%SCNPRT-F-OPENERR Error opening exclusion file ''Scan_exclude'"
$  request/to=('scan_reqclass') -
   "%SCNPRT-F-OPENERR Error opening exclusion file ''Scan_exclude'"
$  goto stop_process
$!
$ MAIL_OPEN_ERR:			! Error opening mail file
$  deftable SCNPRT$STATE "STOPPING"
$  say "%SCNPRT-F-OPENERR Error opening mail file on ''Scan_wrkdir'"
$  request/to=('scan_reqclass') -
   "%SCNPRT-F-OPENERR Error opening mail file on ''Scan_wrkdir'"
$  goto stop_process
$!
$ SEND_MAIL_ERR:			! Error sending mail file
$  deftable SCNPRT$STATE "STOPPING"
$  say "%SCNPRT-F-SENDERR Error sending mail file to ''Scan_mailuser'"
$  request/to=('scan_reqclass') -
   "%SCNPRT-F-SENDERR Error sending mail to ''Scan_mailuser'"
$  goto stop_process
$!
$ STOP_PROCESS:				! Exiting procedure
$ on error then continue
$ on warning then continue
$   say -
    "%SCNPRT-F-STOPERR Process stopped on error, please consult Log file"
$   request/to=('scan_reqclass') -
    "%SCNPRT-F-STOPERR Process stopped on error , please consult Log file"
$   deftable SCNPRT$STATE "STOPPED"
$   if scan_mailstop.nes."" 
$   then 
$      say -
       "%SCNPRT-I-SENDMAIL Sending error mail message to ''scan_mailstop'"
$      request/to=('scan_reqclass') -
       "%SCNPRT-I-SENDMAIL Sending error mail message to ''scan_mailstop'"
$      mail-
       /subject=-
       "SCAN PRINTERS STOPPED ON ERROR ON ''clusternode'"-
       /Personal=-
       "Scan Printers Process ''scan_version' on ''curnode'" -
       nl: -
       'scan_mailstop'
$   endif
$   deftable SCNPRT$LAST_SCAN_END "''f$time()'"
$ exit
$!
$!**********************************************************************
$! Creation      : A. FONTANEL                  V1.0  - Matra Espace
$!
$! Modifications : H. MERCUSOT  - 06/91         V2.0  - Cap Sesa Exploitation
$!                 * Renommage de SURV_IMPRIMANTES en SCAN_PRINTERS.
$!                 * Ré-écriture complete de la procédure pour lancement
$!                   en process détaché.
$!                 * Utilisation de noms logiques.
$!                 * Envoi de Request et REPLY.
$!                   
$! Modifications : H. MERCUSOT  - 06/91         V2.1  - Cap Sesa Exploitation
$!                 * Adjonction du module statistique et du nom logique
$!                   SCNPRT$STATISTICS_FILE.
$!                 * Test des privilèges donnés au process.
$!                 * Test de la disparité entre la version de SCNPRT_STUP.COM
$!                   et de cette procédure.
$!
$!               : H. MERCUSOT  - 07/91         V2.2  - Cap Sesa Exploitation
$!                 * Possibilité d'avoir un delai d'hibernation secondaire 
$!                   SCNPRT$ALT_HIBERNATE_DELAY à partir d'une date donnée
$!                   par SCNPRT$EXPECTED_ALT_DATE.
$!                 * Déplacement de certains modules pour optimiser les temps
$!                   de l'interpréteur DCL.
$!                 * Modifications de Control_Strings de f$fao pour des
$!                   problèmes de pluriels.
$!                 * Déplacement de la translation des noms logiques
$!                   SCNPRT$MAIL et SCNPRT$MAIL_ON_STALLED en début de passe.
$!                 * Modification de la signification de SCNPRT$MAIL_ON_STALLED
$!                   si indéfini ou = 0 -> Pas de mail pour queue stalled,
$!                   sinon mail si la queue est stalled après le nombre de
$!                   passes dont la valeur est égale à SCNPRT$MAIL_ON_STALLED.
$!                 * Creation d'un label write_mail: pour l'écriture dans
$!                   le fichier temporaire de mail.
$!                 * Mise à jour du nom logique SCNPRT$SCANNING_PASS qui est
$!                   égal au nombre de passes effectuées par le process.
$!
$!               : H. MERCUSOT  - 08/91         V2.3  - Cap Sesa Exploitation
$!                 * Possiblité de faire un SET ENTRY/RELEASE pour les jobs
$!                   trouvés en holding. Choix par SCNPRT$RELEASE_HOLD (Y/N).
$!                   Un pseudo-etat HOLDING est créé pour une queue trouvée avec
$!                   des jobs holding.
$!                 * Modification du test des privilèges nécessaires.
$!                 * Décomposition de l'enregistrement des stat. en 2 
$!                   enregistrements. Le premier concerne la queue, le 2eme
$!                   n'existe que si le nombre de jobs executing est different
$!                   de 0.
$!                 * Adjonction de SCNPRT$QUEUE_TYPE qui permet de définir sur
$!                   quel type de queue on veut travailler. Attention aucun
$!                   test de validité. (Défaut "SYMBIONT")
$!                 * Adjonction d'un <LF> en début de la Control_string de
$!                   F$FAO pour creation et erreur pour le fichier de stat.
$!                 * Suppression du symbole scan_logfile.
$!                 * Modification de l'incrémentation des compteurs scan_pb et
$!                   scan_mail_pb pour considérer qu'un job hold est une erreur.
$!
$!               : H. MERCUSOT  - 10/91         V2.4  - Cap Sesa Exploitation
$!		   * Possibilité d'attendre SCNPRT$SPECIAL_HIBERNATE_DELAY 
$!                   après attente de SCNPRT$ALT_HIBERNATE_DELAY si on est un
$!                   jour defini dans SCNPRT$SPECIAL_DAYS. 
$!                 * Modification des symboles de test des fichiers ouverts
$!                   pour limiter les tables recherchées au niveau process.
$!                 * Modification du test des privilèges nécessaires en fonction
$!                   de l'existence de SCNPRT_TABLE.
$!                 * Adjonction dans le message du mail du message d'erreur de
$!                   CPS s'il existe.
$!
$!               : H. MERCUSOT  - 02/92         V2.5  - Cap Sesa Exploitation
$!                 * Modification message de demarrage envoyé en request a cause
$!                   faute de frappe.
$!                 * Modification du module STOP_PROCESS pour envoi d'un mail
$!                   à SCNPRT$MAIL_ON_STOP et mise à jour de
$!                   SCNPRT$LAST_SCAN_END
$!                 * Adjonction de la valeur MAILING pour SCNPRT$STATE lors
$!                   de l'envoi d'un mail en fin de passe.
$!                 * Changement du 2eme champ ds fichiers stat., celui-ci 
$!                   contient le nom du cluster au lieu du nom de n÷ud si l'on
$!                   est en cluster avec un alias au niveau DECNet et que 
$!                   SCNPRT$IGNORE_CLUSTER est NO.
$!
$!               : H. MERCUSOT  - 07/92         V2.6  - Cap Sesa Exploitation
$!                 * Remplacement de @ par à dans la variable Node_string pour
$!		     probleme REPLY et REQUEST pour queues MSA.
$!                 * Adjonction dans le message du mail du message d'erreur de
$!		     DQS s'il existe.
$!**********************************************************************
