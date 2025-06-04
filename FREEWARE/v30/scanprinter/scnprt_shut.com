$!******************************************************************************
$!                     SCNPRT_SHUT.COM  Version 2.6
$!
$!      Shutdown the ScanPrt_Daemon if not in "executing" mode
$!
$!******************************************************************************
$!
$  on error then goto end_proc		  !Set up an error trap
$  set process /privileges=(TMPMBX,WORLD,READALL) !Get needed privileges
$!
$! Setup parameters options
$ params="|"+P1+"|"+P2+"|"+P3+"|"+P4+"|"+P5+"|"+P6+"|"+P7+"|"+P8
$ params=f$edit(params,"TRIM,COLLAPSE,UPCASE")
$ scnprt_abort="FALSE"
$ if f$locate("|ABO",params).ne.f$length(params) then scnprt_abort="TRUE"
$!
$! Setup context to search process
$
$  if f$type(scnprt_ctx).eqs."PROCESS_CONTEXT" then -
      scnprt_temp=f$context("PROCESS",scnprt_ctx,"CANCEL")
$  scnprt_temp=f$context("PROCESS",scnprt_ctx,"JOBTYPE","DETACHED","EQL")
$  scnprt_temp=f$context("PROCESS",scnprt_ctx,"PRCNAM","ScanPrt_Daemon","EQL")
$  if f$getsyi("cluster_member") 
$  then  
$  	scnprt_curnode=f$getsyi("nodename")
$       scnprt_temp=-
        f$context("PROCESS",scnprt_ctx,"NODENAME","''scnprt_curnode'","EQL")
$  endif
$
$! Scan all processes on system with specified context
$
$  scnprt_pid = ""				!Start with null PID context
$!
$	scnprt_pid = f$pid(scnprt_ctx)		!Get next process ID
$	if scnprt_pid .eqs. "" 
$       then 
$         write sys$output -
    "%SCNPRTSHUT-W-NOTRUNNING, process ScanPrt_Daemon is not running on this node"
$       else
$         process_name=f$getjpi(scnprt_pid,"PRCNAM")
$         scn_table=f$trnlnm("SCNPRT_TABLE","LNM$SYSTEM_DIRECTORY",,,,"TABLE")
$       	if scn_table 
$       	then
$            		gosub stop_proc
$       	else
$             		write sys$output -
  			"%SCNPRTSHUT-W-NOTABLE, SCNPRT_TABLE does not exist"
$             		gosub scnprt_stop
$              endif
$       endif
$!
$end_proc:
$  if f$type(scnprt_ctx).eqs."PROCESS_CONTEXT" then -
      scnprt_temp=f$context("PROCESS",scnprt_ctx,"CANCEL")
$  exit
$!
$ SCNPRT_STOP:
$ 	stop/id='scnprt_pid' 	         !Stop the ScanPrt_Daemon process
$       write sys$output -
  "%SCNPRTSHUT-I-PROCSTOP, Process ''scnprt_pid' (''process_name') stopped"
$       if scn_table 
$       then 
$       	define/system/nolog/executive/table=SCNPRT_TABLE SCNPRT$STATE -
        	"STOPPED BY ''f$getjpi("","username")'"
$               define/system/nolog/executive/table=SCNPRT_TABLE -
                SCNPRT$LAST_SCAN_END "''f$time()'"
$       endif
$       wait 00:00:02	! Wait for process stop completion
$       return
$!
$STOP_PROC:
$loop:
$     scn_state = f$trnlnm("SCNPRT$STATE","SCNPRT_TABLE")
$     if f$extract(0,3,scn_state).eqs."HIB" 
$     then
$   	gosub scnprt_stop
$     else 
$	if f$extract(0,4,scn_state).eqs."STOP" 
$     	then 
$		write sys$output -
  "%SCNPRTSHUT-I-TBLSTPD, Process ''scnprt_pid' (''process_name') already stopped in SCNPRT_TABLE"
$       	gosub scnprt_stop
$     		else 
$			if scn_state.eqs."" 
$     			then 
$			write sys$output -
  "%SCNPRTSHUT-W-NOSTATE, Cannot find process ''scnprt_pid' (''process_name') state"
$       		gosub scnprt_stop
$ 	    		else 
$				if scnprt_abort 
$     				then
$       				write sys$output -
  "%SCNPRTSHUT-W-ABORT, Aborting process ''scnprt_pid' (''process_name') while working"
$       				gosub scnprt_stop
$  				else
$       				write sys$output -
  "%SCNPRTSHUT-W-PROCWORK, Process ''scnprt_pid' (''process_name') working, wait ..."
$				       	wait 00:00:05
$       				goto loop
$     				endif
$			endif
$	endif
$     endif
$ return
