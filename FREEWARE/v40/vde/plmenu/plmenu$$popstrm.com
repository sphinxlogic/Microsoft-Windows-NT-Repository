$ sav_verify = 'f$verify(0)
$!+
$! Documentation at end of command file
$!+
$ set = "set"
$ set symbol/scope=noglobal
$ delete /symbol/local set
$!
$ set noon
$ if f$trnlnm("PLMENU$DEBUG") .nes. "" then set verify
$!+
$! check if populates are turned off on this node
$!+
$ if f$trnlnm("FAC$ABORT_POPLIBS") .nes. ""
$ then
$    write sys$output "%POP-E-LOGNMABRT, FAC$ABORT_POPLIBS logical found"
$    GOTO Error_EXIT
$ endif
$!
$!  validate the root and set to the correct VDE library
$!
$ mnemonic = P1
$ @VDE$SYSTEM:PLMENU$$set_vde_library 'mnemonic'
$ if .not. $status then GOTO Error_Exit
$ root = f$trnlnm("PLMENU$CMSROOT")
$ target_stream = P2
$ input_stream  = p3
$!
$! validate the batch queue
$!
$ queue = p4
$ define/user/nolog sys$output _nla0:
$ show queue 'queue'
$ if .not. $status then GOTO Error_Exit
$!
$ if p5 .eqs. "" then p5 = "SYS$LOGIN"
$ scratch_dir = f$parse("''P5'",,,,"NO_CONCEAL") - ".;"
$ 
$ poption = "BOTH"
$ pOption_text = "both CMS and VDE"
$ if f$locate(",CMS,",",''P6',") .ne. f$length(",''P6',") then pOption = "CMS"
$ if f$locate(",CMS,",",''P6',") .ne. f$length(",''P6',") then pOption_text = "CMS only"
$ if f$locate(",VDE,",",''P6',") .ne. f$length(",''P6',") then pOption = "VDE"
$ if f$locate(",VDE,",",''P6',") .ne. f$length(",''P6',") then pOption_text = "VDE only"
$ if f$locate(",BOTH,",",''P6',") .ne. f$length(",''P6',") then pOption = "BOTH"
$ if f$locate(",BOTH,",",''P6',") .ne. f$length(",''P6',") then pOption_text = "both CMS and VDE"
$ moption = " "
$ moption_text = " replacing"
$ if f$locate(",MERGE,",",''P6',") .ne. f$length(",''P6',") then mOption = "MERGE"
$ if f$locate(",MERGE,",",''P6',") .ne. f$length(",''P6',") then mOption_text = "merging"
$
$ set default 'scratch_dir'
$!  
$ write sys$output -
  "%POPSTRM-I-BEGIN, Begin populate of ''target_stream' from ''input_stream' at ''F$time()"
$ write sys$output -
  "-POPSTRM-I-OPTS, populating ''pOption_text', and ''mOption_text' duplicates"
$!
$ if poption .nes. "CMS" then GOSUB Populate_VDE_Stream 
$!
$! See if the vde$fetch_method logical is defined to VDE.
$! If the logical is defined to VDE, the populate is complete
$! because the stream should exist in the VDE database only.
$! If the logical is undefined, populate the stream in CMS.
$!
$ stream_fetch_method = f$trnlnm("vde$fetch_method")
$ stream_fetch_method = F$edit(stream_fetch_method,"TRIM,UPCASE")
$!
$ if stream_fetch_method .EQS. "VDE" then GOTO Skip_CMS_Pop
$!
$!+
$! check if populates are turned off on this node
$!+
$ if f$trnlnm("FAC$ABORT_POPLIBS") .nes. ""
$ then
$    write sys$output "%POP-E-LOGNMABRT, FAC$ABORT_POPLIBS logical found"
$    GOTO Error_EXIT
$ endif
$!
$  if poption .nes. "VDE" then GOSUB Submit_CMS_Populates
$!
$Skip_CMS_Pop:
$!------------
$ GOSUB Wait_for_Completion
$
$MAIN_EXIT:
$ if f$trnlnm("statfile") .nes. "" then close statfile
$ if f$search("poperrs.tmp;") .nes. "" then -
    delete/nolog poperrs.tmp;*
$ if f$search("''pid'_jobs.tmp;") .nes. "" then -
    delete/nolog 'pid'_jobs.tmp;*
$ EXIT 1+1*'f$verify(sav_verify)
$!
$ERROR_EXIT:
$ if f$trnlnm("statfile") .nes. "" then close statfile
$ if f$search("poperrs.tmp;") .nes. "" then -
    delete/nolog poperrs.tmp;*
$ if f$search("''pid'_jobs.tmp;") .nes. "" then -
    delete/nolog 'pid'_jobs.tmp;*
$ EXIT 0+0*'f$verify(sav_verify)
$!
$!+-----------------------------------------------------------------------------
$! Execute the VDE commands necessary to populate the build stream 
$!+-----------------------------------------------------------------------------
$Populate_VDE_Stream:
$!-------------------
$ write sys$output "%POP-I-VDE, VDE populate begins at ''F$time()'"
$!
$! populate the build stream
$!
$ VDE$_STRMNOTPOP = 57652120
$ VDE SET PRIV FROZEN, PERFREP;POPULATE STREAM/FROM='input_stream' 'target_stream'
$ SaveStatus = $status
$ if SaveStatus .eq. VDE$_STRMNOTPOP
$ then
$   write sys$output "%POPFACS-F-NOPOP, VDE stream ''target_stream' marked /NOPOPULATE"
$   RETURN
$ EndIf
$ if .not. SaveStatus
$ then
$   write sys$output "%POPSTRM-F-FAILED, VDE populate stream command failed!"
$   GOTO Error_Exit
$ endif
$!
$ RETURN
$!
$!+-----------------------------------------------------------------------------
$! Submit PLMENU$$POPCLASS.COM to perform the CMS insert generations from the
$! input class to the target class in all non-archived, propagating facilities.
$!
$! Check if there is a facility name that matches(exactly) the input class name
$! if such a facility exists, populate the class in this facility.
$!+-----------------------------------------------------------------------------
$Submit_CMS_Populates:
$!--------------------
$ write sys$output "%POP-I-SUBMIT, Submitting CMS populates at ''f$time()'"
$!
$! create the populate status file
$!
$ stat_file_name = target_stream - "." - "." + "_POP.DAT"
$ stat_file_name = scratch_dir + stat_file_name - "><" - "]["
!
$! clear out any old data files
$!
$ if f$search("''stat_file_name'") .nes. "" then delete/nolog 'stat_file_name';*
$ create/fdl=vde$system:plmenu$$poplibs.fdl 'stat_file_name'
$ open/read/write/share statfile 'stat_file_name'
$ if .not. $status then GOTO Error_Exit
$!
$!             123456789012345678901234567890123456789012345678901234567890 - 60 blanks
$ blank_key = "                                                            "
$ set command vde$system:plmenu$$facbatch
$! 
$Library_loop:
$!
$ library = f$search("''root':[*]cms.dir",1)
$ if library .eqs. "" then GOTO Submit_CMS_Populates_RET
$!
$ facility = f$parse(library,,,"DIRECTORY") - "[" - "]" - "<" - ">"
$!+
$! skip archived facilities
$!+
$ if f$search("''root':[''facility']VMSCMS$ARCHIVED_FACILITY.FLAG") .nes. ""
$ then
$   write sys$output "%POPSTRM-I-FACARCH, skipping ''facility'; facility archived
$   GOTO Library_loop
$ EndIf
$!+
$! skip no propagate facilities unless the facility name matches the input
$! stream name
$!+
$ if f$search("''root':[''facility']VMSCMS$NO_PROPAGATE.FLAG") .nes. ""
$ then
$   if facility .nes. input_stream then GOTO Library_loop
$ endif
$!
$! key to status file record is root + facility
$!
$ stat_key = root + ":" + facility + blank_key
$ stat_key = f$extract(0,60,stat_key)
$!
$ facbatch/process=ALL/LOG/output='scratch_dir'/postfix_jobname="$POPSTRM"/queue='queue' -
  /P4="''target_stream'"/P5="''input_stream'"/p6="''stat_file_name'" /p7="''moption'" -
  'root':['facility'.cms] VDE$SYSTEM:PLMENU$$POPCLASS.COM
$!+
$! update the status file to indicate the job is submitted
$!+
$ write statfile "''stat_key'        ''f$extract(0,17,f$time())' *SUBMITTED "
$!
$ GOTO Library_loop
$!
$Submit_CMS_Populates_RET:
$ if f$trnlnm("statfile") .nes. "" then close statfile
$ RETURN
$!
$!+-----------------------------------------------------------------------------
$! check the entries until all populate jobs are gone, when all jobs are gone
$! send notification to user with any problems that are detected
$!+-----------------------------------------------------------------------------
$Wait_for_Completion:
$!-------------------
$ if stream_fetch_method .EQS. "VDE"
$ then
$   mail_subject = "Populate of ''target_stream' completed, check log file"
$   create poperrs.tmp

$   GOTO Send_Mail
$ endif
$!
$ wait 00:02:00     
$ pid = f$getjpi("","PID")
$!
$! this assumes your running one populate at a time
$Check_for_completion_loop:
$!-------------------------
$ temp = f$getqui("cancel_operation")
$ temp = f$getqui("DISPLAY_ENTRY","JOB_NAME","*$POPSTRM","BATCH,WILDCARD")
$ if temp .nes. ""
$ then
$   wait 00:05:00
$   GOTO Check_for_completion_loop
$ endif
$!
$ write sys$output "%POP-I-CHECK, Checking populate jobs for errors at ''F$time()'"
$!
$ mail_subject = "Populate for ''target_stream' finished with no errors detected"
$ define/user sys$error _nla0:
$ define/user sys$output _nla0:
$ search/output=poperrs.tmp 'stat_file_name' "*"
$ if $status .eq. %X08D78053 then GOTO Send_mail  ! no matches
$!
$! check or errors
$!
$ mail_subject = "Populate for ''target_stream' finished with errors"
$ define/user sys$error _nla0:
$ define/user sys$output _nla0:
$ search/out=_nla0: poperrs.tmp "*ABORTED"
$ if $status .eq. %x00000001 then GOTO Send_Mail
$!
$! check for warnings
$!
$ mail_subject = "Populate for ''target_stream' finished with warnings"
$ define/user sys$error _nla0:
$ define/user sys$output _nla0:
$ search/out=_nla0: poperrs.tmp "*WARNING"
$ if $status .eq. %x00000001 then GOTO Send_Mail
$!
$! check for jobs with submitted or executing status in file
$!
$ mail_subject = "Some jobs incomplete for populate of ''target_stream'"
$ define/user sys$error _nla0:
$ define/user sys$output _nla0:
$ search/MATCH=OR/out=_nla0: poperrs.tmp "*SUBMITTED","*EXECUTING"
$ if $status .eq. %x00000001 then GOTO Send_Mail
$!
$ mail_subject = "Populate for ''target_stream' finished with no errors detected"
$!
$Send_Mail:
$ user = F$edit("''f$getjpi("","USERNAME")'","Trim,Upcase")
$!
$ mail/subject="''mail_subject'" poperrs.tmp "''user'"
$!
$ delete/nolog poperrs.tmp;*
$!
$ RETURN 
$!
$!++
$! POPSTRM.COM - Populate a VMS stream.
$!
$!  This is the main command procedure used to populate a VDE
$! stream and its associated CMS classes.  The command procedure performs the
$! populate of the VDE library stream.  If the VDE populate is successful, then
$! PLMENU$$POPCLASS.COM is submitted to perform the CMS populations for the root
$! facilities.
$!
$! Inputs:    Description               (local symbol name)
$!            ------------------------- -------------------                                   
$!  P1	      product VDE mnemonic	(mnemonic)
$!	      (used to find CMS root)	(root)
$!  P2        stream to populate	(target_stream)
$!  P3        parent stream		(input_stream)
$!  P4	      queue for CMS jobs	(queue)
$!  P5        work directory for logs   (scratch_dir)
$!  P6        populate options		(poption/moption)
$!  vde$fetch_method
$!	      a logical used to define	(stream_fetch_method)
$!	      the scope of the create
$!	      VDE or VDE and CMS
$!
$! FILES:
$!	All files are located in the directory passed in P5.
$!
$!	    CMS POPULATE STATUS FILE
$!	    format: 'target_stream'_POP.DAT
$!
$!	    CMS populate logfiles
$!          format: 'facility'$POPSTRM.LOG
$!
$!  Modified:
$!	Hoffman	    21-Jun-1995	    Added NO_CONCEAL on P5
$!				    
$!
$!	X-2	BMAN	Brian Man		30-MAR-1998
$!		Changed format: 'target_stream'_POP.DATA to
$!		'target_stream'_POP.DAT for consistency
$!
$!	X-3	BMAN	Brian Man		14-APR-1998
$!		Set the blank_key to 60 chars and all of the
$!		associated vars (ie. stat_key) to be the same.
$!
