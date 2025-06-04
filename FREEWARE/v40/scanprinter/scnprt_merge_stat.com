$ set noverify
$!**********************************************************************
$!                    SCNPRT_MERGE_STAT.COM        V2.6
$!**********************************************************************
$!
$! On settings
$ on error then goto error
$ on warning then goto error
$ on control_c then goto ctrl_c
$!
$! scan_table=f$trnlnm("SCNPRT_TABLE","LNM$SYSTEM_DIRECTORY",,,,"TABLE")
$ say         := write sys$output
$ time 	       = f$time()
$ curnode = f$getsyi("nodename")        ! 3 lines for getting node name !!!
$ if curnode .eqs. "" then curnode = f$getsyi("scsnode")
$ if curnode .eqs. "" then curnode = f$trnlnm("sys$node") - "::" - "-"
$ scan_version ="V2.6"
$!
$ say " SCNPRT_MERGE_STAT - ''scan_version' - Starting at ''time' on node ''curnode'"
$!
$! Check to see difference in version between SCNPRT on this node and this procedure
$!
$ temp = f$trnlnm("SCNPRT$VERSION","SCNPRT_TABLE")
$ temp = f$edit(temp,"TRIM,COLLAPSE,UPCASE")
$ if temp.nes.scan_version
$ then
$  if temp.nes."" 
$  then
$    req_message= -
     "%SCANPRT-W-VERMIS, SCAN PRINTERS version ''temp' and SCNPRT_MERGE_STAT version ''scan_version' mismatch"
$    say "''req_message'"
$  endif
$ endif
$!
$ say ""
$!
$! Defining F$FAO strings parameters
$!
$ fao_pbstat= "%SCNPRTMRG-E-!AS, Error !AS file !AS"
$ fao_source= "%SCNPRTMRG-E-NOTVAL, !AS in not a valid source file name"
$!
$! Testing P1 parameter
$!
$ if p1.EQS."" then P1="scnprt_dir:scan_printers.stat"
$ p1=f$edit(P1,"trim,collapse,upcase") 
$ source_statistics=f$parse(P1,"scnprt_dir:",-
                     "SYS$SCRATCH:SCAN_PRINTERS.STAT",,)
$ if source_statistics.eqs."" .or. f$search("''source_statistics'").eqs.""
$ then 
$   write sys$output f$fao(fao_source,P1)
$   goto end
$ endif
$!
$! Initializing work directory and Statistics file name from logicals
$!
$ scan_statistics = f$trnlnm("SCNPRT$STATISTICS_FILE","SCNPRT_TABLE")
$ scan_wrkdir = f$trnlnm("SCNPRT$WORK_DIRECTORY","SCNPRT_TABLE")
$!
$    scan_statistics=f$parse(scan_statistics,scan_wrkdir,-
                     "SYS$SCRATCH:SCAN_PRINTERS.STAT",,"SYNTAX_ONLY")
$!
$! Append source file to Stat. file of this node
$!
$      scan_statistics_tmp=f$parse(scan_statistics,"SCAN_PRINTERS",-
                           ,"NAME","SYNTAX_ONLY")
$      scan_statistics_tmp=f$parse(scan_statistics_tmp,scan_wrkdir,-
                     "SYS$SCRATCH:SCAN_PRINTERS.TMP",,"SYNTAX_ONLY")
$      rename/log 'source_statistics' 'scan_statistics_tmp'
$      error=0
$ append:
$      on error then goto append_err
$      on warning then goto append_err
$      if error.gt.10 then goto end
$      append/new 'scan_statistics_tmp' 'scan_statistics'
$      on error then goto error
$      on warning then goto error
$      
$!
$ end:
$ on error then continue
$ del 'scan_statistics_tmp'*
$ exit
$!
$ append_err:
$      req_message= f$fao(fao_pbstat,"APPENDERR","merging",source_statistics)
$      say "''req_message'"
$      error=error+1
$      wait 00:01:00
$      goto error
$!
$ error:
$   say -
    "%SCNPRTSTAT-F-STOPERR Procedure stopped on error"
$   goto end
$!
$ ctrl_c:
$   say -
    "%SCNPRTSTAT-F-INTERRUPT Procedure interrupted by Ctrl C"
$   goto end
$!
$!**********************************************************************
$! Creation      : H. MERCUSOT  - 10/91         V2.4  - Cap Sesa Exploitation
$!
$!		Cette procédure permet de cumuler 2 fichiers de stat.
$!              à partir d'un fichier de stat. de SCAN_PRINTERS V2.4
$!              donné par P1 vers le fichier de stat. local.
$!
$!	Syntaxe : @SCNPRT_MERGE_STAT source_file_name_stat
$!
$! Modifications : H. MERCUSOT  - 02/92         V2.5  - Cap Sesa Exploitation
$!              Changement de version de SCAN_PRINTERS.
$!              Modification dans la recherche du nom de n÷ud courant.
$!
$! Modifications : H. MERCUSOT  - 09/92         V2.6  - Cap Sesa Exploitation
$!              Changement de version de SCAN_PRINTERS.
$!**********************************************************************
