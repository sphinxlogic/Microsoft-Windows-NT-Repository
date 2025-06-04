$!-----------------------------------------------------
$!
$!            SCNPRT_LOGICALS Version V2.6
$!
$!          Logical names definition
$!
$!-----------------------------------------------------
$!
$ params="|"+P1+"|"+P2+"|"+P3+"|"+P4+"|"+P5+"|"+P6+"|"+P7+"|"+P8
$ params=f$edit(params,"TRIM,COLLAPSE,UPCASE")
$ if f$locate("|NOL",params).nes. f$length(params)
$ then 
$   if (f$trnlnm("SCNPRT_TABLE","LNM$SYSTEM_DIRECTORY",,,,"TABLE")) then -
$   deass/system/executive/table=lnm$system_directory scnprt_table
$ else
$!
$ if (.not.f$trnlnm("SCNPRT_TABLE","LNM$SYSTEM_DIRECTORY",,,,"TABLE")) then -
	create/name_table/executive/attributes=SUPERSEDE/nolog - 
	/parent=LNM$SYSTEM_DIRECTORY  SCNPRT_TABLE 
$ define/system/executive/table=SCNPRT_TABLE SCNPRT$START            "''F$TIME()'"
$ define/system/executive/table=SCNPRT_TABLE SCNPRT$VERSION           V2.6
$!
$! ----------- Here are lines to customize -----------------------------------
$ define/system/executive/table=SCNPRT_TABLE SCNPRT$priority          2
$ define/system/executive/table=SCNPRT_TABLE SCNPRT$hibernate_delay   "00:20:00"
$ define/system/executive/table=SCNPRT_TABLE SCNPRT$alt_hibernate_delay "14:00:00"
$ define/system/executive/table=SCNPRT_TABLE SCNPRT$expected_alt_date "18:00:00"
$ define/system/executive/table=SCNPRT_TABLE SCNPRT$special_hibernate_delay "23:59:59"
$ define/system/executive/table=SCNPRT_TABLE SCNPRT$special_days "Saturday,Sunday"
$ define/system/executive/table=SCNPRT_TABLE SCNPRT$mail              SYSTEM
$ define/system/executive/table=SCNPRT_TABLE SCNPRT$mail_on_stop      SYSTEM
$ define/system/executive/table=SCNPRT_TABLE SCNPRT$ignore_cluster    YES
$ define/system/executive/table=SCNPRT_TABLE SCNPRT$work_directory    SYS$SCRATCH:
$ define/system/executive/table=SCNPRT_TABLE SCNPRT$request_class     "PRINTER"
$ define/system/executive/table=SCNPRT_TABLE SCNPRT$restart_queue     YES
$ define/system/executive/table=SCNPRT_TABLE SCNPRT$release_hold      YES
$ define/system/executive/table=SCNPRT_TABLE SCNPRT$mail_on_stalled   2
$ define/system/executive/table=SCNPRT_TABLE SCNPRT$message           "SYSTEM"
$! define/system/executive/table=SCNPRT_TABLE SCNPRT$statistics_file   scnprt_dir:scan_printers.stat
$! define/system/executive/table=SCNPRT_TABLE SCNPRT$exclusion_file   scnprt_dir:scan_exclude.dat
$!
$! ----------- End of lines to customize -------------------------------------
$ endif
$!
$ exit
