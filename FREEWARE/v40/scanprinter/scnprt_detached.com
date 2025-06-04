$!-----------------------------------------------------
$!
$!            SCNPRT_DETACHED Version 2.6
$!
$!          Start detached Scan Printers process
$!
$!-----------------------------------------------------
$!
$!
$ params="|"+P1+"|"+P2+"|"+P3+"|"+P4+"|"+P5+"|"+P6+"|"+P7+"|"+P8
$ params=f$edit(params,"TRIM,COLLAPSE,UPCASE")
$ if f$locate("|RES",params).nes. f$length(params)
$ then 
$  if f$trnlnm("scnprt_dir").eqs."" then define/nolog scnprt_dir sys$manager:
$  @scnprt_dir:scnprt_shut.com 'P1' 'p2' 'p3' 'P4' 'p5' 'p6' 'P7' 'p8' 
$ endif
$!
$! Setup context to search process
$
$  if f$type(scnprt_ctx).eqs."PROCESS_CONTEXT" then -
      scnprt_temp=f$context("PROCESS",scnprt_ctx,"CANCEL")
$  scnprt_temp=f$context("PROCESS",scnprt_ctx,"JOBTYPE","DETACHED","EQL")
$  scnprt_temp=f$context("PROCESS",scnprt_ctx,"PRCNAM","ScanPrt_Daemon","EQL")
$  cluster=f$getsyi("cluster_member")
$  node="node"
$  if cluster then node="cluster"
$  scnprt_temp=f$context("PROCESS",scnprt_ctx,"NODENAME","*","EQL")
$
$! Scan process on system with specified context
$
$  scnprt_pid = ""				!Start with null PID context
$!
$	scnprt_pid = f$pid(scnprt_ctx)		!Get process ID
$	if scnprt_pid .nes. "" 
$       then 
$         write sys$output -
  "%SCNPRTDET-E-ALREDACT, process ScanPrt_Daemon is already active on this ''node'"
$         goto end_proc	!Exit
$       endif
$!
$ RUN -
     /INPUT=scnprt_dir:scan_printers.com-
     /OUTPUT=sys$manager:scan_printers.log-
     /PRIORITY=4-
     /UIC=[1,4]-
     /PRIVILEGES=(NOSAME,NETMBX,TMPMBX,OPER,SYSPRV,SYSNAM)-
     /PROCESS_NAME="ScanPrt_Daemon" -
     /MAXIMUM = 1024 -
     /EXTEND = 1024 -
     /WORKING = 512 -
     SYS$SYSTEM:LOGINOUT.EXE
$!
$ end_proc:
$  if f$type(scnprt_ctx).eqs."PROCESS_CONTEXT" then -
      scnprt_temp=f$context("PROCESS",scnprt_ctx,"CANCEL")
$ exit
