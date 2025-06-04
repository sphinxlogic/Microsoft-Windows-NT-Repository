$ !sav_verify = 'f$verify(0)
$!+
$! Documentation at end of command file
$!+
$ set = "set"
$ set symbol/scope=noglobal
$ delete /symbol/local set
$!
$ if f$trnlnm("PLMENU$DEBUG") .nes. "" then set verify
$ set noon
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
$ sav_verify = 'f$verify(1)
$ target_stream = P2
$ input_stream  = p3
$ root = f$trnlnm("PLMENU$CMSROOT")
$!
$! validate the batch queue
$!
$ queue = p4
$ define/user/nolog sys$output _nla0:
$ show queue 'queue'
$ if .not. $status then GOTO Error_Exit
$!
$ if p5 .eqs. "" then p5 = "SYS$LOGIN"
$ scratch_dir = f$parse("''P5'") - ".;"
$! 
$ facility_file = P6
$!
$ poption = "BOTH"
$ pOption_text = "both CMS and VDE"
$ if f$locate(",CMS,",",''p7',") .ne. f$length(",''p7',") then pOption = "CMS"
$ if f$locate(",CMS,",",''p7',") .ne. f$length(",''p7',") then pOption_text = "CMS only"
$ if f$locate(",VDE,",",''p7',") .ne. f$length(",''p7',") then poption = "VDE"
$ if f$locate(",VDE,",",''p7',") .ne. f$length(",''p7',") then pOption_text = "VDE only"
$ if f$locate(",BOTH,",",''p7',") .ne. f$length(",''p7',") then pOption = "BOTH"
$ if f$locate(",BOTH,",",''p7',") .ne. f$length(",''p7',") then pOption_text = "both CMS and VDE"
$ moption = " "
$ moption_text = " replacing"
$ if f$locate(",MERGE,",",''p7',") .ne. f$length(",''p7',") then mOption = "MERGE"
$ if f$locate(",MERGE,",",''p7',") .ne. f$length(",''p7',") then mOption_text = "merging"
$!
$ set default 'scratch_dir'
$!  
$ write sys$output -
  "%POPFACS-I-BEGIN, Begin populate of ''target_stream' from ''input_stream' at ''F$time()"
$ write sys$output -
  "-POPFACS-I-OPTS, populating ''pOption_text', and ''mOption_text' duplicates"
$!
$! create the status file
$!
$ stat_file_name = target_stream - "." - "." + "_POP.DAT"
$ stat_file_name = scratch_dir + stat_file_name - "><" - "]["
$
$ create/fdl=VDE$SYSTEM:PLMENU$$poplibs.fdl 'stat_file_name'
$!
$ if poption .nes. "CMS" then GOSUB Populate_VDE_Stream
$!
$ if poption .nes. "VDE" then GOSUB Submit_CMS_Populates
$!
$ GOSUB Wait_for_Completion
$
$MAIN_EXIT:
$ if f$trnlnm("statfile") .nes. "" then close statfile
$ if f$trnlnm("facfile") .nes. "" then close facfile
$ if f$search("poperrs.tmp;") .nes. "" then -
    delete/nolog poperrs.tmp;*
$ if f$search("''pid'_jobs.tmp;") .nes. "" then -
    delete/nolog 'pid'_jobs.tmp;*
$ if f$search("''facility_file'") .nes. ""
$ then
$   if F$PARSE(facility_file,,,"TYPE") .eqs. ".POP$TMP" 
$   then
$	delete/nolog 'facility_file'
$   endif
$ endif
$ EXIT 1+1*'f$verify(sav_verify)
$!
$ERROR_EXIT:
$ if f$trnlnm("statfile") .nes. "" then close statfile
$ if f$trnlnm("facfile") .nes. "" then close facfile
$ if f$search("poperrs.tmp;") .nes. "" then -
    delete/nolog poperrs.tmp;*
$ if f$search("''pid'_jobs.tmp;") .nes. "" then -
    delete/nolog 'pid'_jobs.tmp;*
$ if f$search("''facility_file'") .nes. ""
$ then
$   if F$PARSE(facility_file,,,"TYPE") .eqs. ".POP$TMP" 
$   then
$	delete/nolog 'facility_file'
$   endif
$ endif
$ EXIT 0+'f$verify(sav_verify)
$!
$!+-----------------------------------------------------------------------------
$! Execute the VDE commands necessary to populate the build stream 
$!+-----------------------------------------------------------------------------
$Populate_VDE_Stream:
$!-------------------
$ write sys$output "%POPFACS-I-VDE, VDE populate begins at ''F$time()'"
$!
$! populate the build stream
$!
$ VDE$_STRMNOTPOP = 57652120
$ VDE SET PRIV FROZEN;POPULATE STREAM/FROM='input_stream' 'target_stream'
$ SaveStatus = $status
$ if SaveStatus .eq. VDE$_STRMNOTPOP
$ then
$   write sys$output "%POPFACS-F-NOPOP, VDE stream ''target_stream' marked /NOPOPULATE"
$   RETURN
$ EndIf
$ if .not. SaveStatus
$ then
$   open/read/write/share statfile 'stat_file_name'
$   if .not. $status then GOTO Error_Exit
$   write statfile -
"*ABORT VDE POPULATE STREAM COMMAND FAILED at ''f$extract(0,17,f$time())'"
$   close statfile
$   write sys$output "%POPFACS-F-FAILED, VDE populate stream command failed!"
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
$ write sys$output "%POPFACS-I-SUBMIT, Submitting CMS populates at ''f$time()'"
$!
$! create the populate status file
$!
$ open/read/write/share statfile 'stat_file_name'
$ if .not. $status then GOTO Error_Exit
$!
$ open/read/share facfile 'facility_file'
$ if .not. $status then GOTO Error_Exit
$!
$!             123456789012345678901234567890123456789012345678901234567890 - 60 blanks
$ blank_key = "                                                            "

$ set command VDE$SYSTEM:PLMENU$$facbatch
$! 
$Library_loop:
$!
$ read/end_of_file=Submit_CMS_Populates_Ret facfile facrec
$ facility = f$edit(facrec,"TRIM,UPCASE")
$ if facility .eqs. "" then GOTO Library_Loop
$!
$!+
$! skip archived facilities
$!+
$ if f$search("''root':[''facility']VMSCMS$ARCHIVED_FACILITY.FLAG") .nes. ""
$ then
$   write sys$output "%POPFACS-I-FACARCH, skipping ''facility'; facility archived"
$   GOTO Library_loop
$ EndIf
$!
$! key to status file record is root + facility
$!
$ stat_key = root + ":" + facility + blank_key
$ stat_key = f$extract(0,60,stat_key)
$!+
$! insure this is a valid facility
$!+
$ if f$search("''root':[''facility'.CMS]00CMS.CMS") .eqs. ""
$ then
$   write sys$output -
    "%POPFACS-E-NOCMS, No CMS database for ''facility', skipping it"
$   write statfile "''stat_key'        ''f$extract(0,17,f$time())' *WARNING INVALID FACILITY NAME"
$   GOTO Library_Loop
$ endif
$!
$ facbatch/process=ALL/LOG/output='scratch_dir'/postfix_jobname="$POPFACS"/queue='queue' -
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
$ if f$trnlnm("facfile") .nes. "" then close facfile
$ RETURN
$!
$!+-----------------------------------------------------------------------------
$! check the entries until all populate jobs are gone, when all jobs are gone
$! send notification to user with any problems that are detected
$!+-----------------------------------------------------------------------------
$Wait_for_Completion:
$!-------------------
$ wait 00:02:00     
$ pid = f$getjpi("","PID")
$!
$Check_for_completion_loop:
$!-------------------------
$ temp = f$getqui("cancel_operation")
$ temp = f$getqui("DISPLAY_ENTRY","JOB_NAME","*$POPFACS","BATCH,WILDCARD")
$ if temp .nes. ""
$ then
$   wait 00:05:00
$   GOTO Check_for_completion_loop
$ endif
$!
$ write sys$output "%POPFACS-I-CHECK, Checking populate jobs for errors at ''F$time()'"
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
$! POPFACS.COM - Populate a VMS stream.
$!
$!  This is the main command procedure used to populate a VDE
$! stream and its associated CMS classes.  The command procedure performs the
$! populate of the VDE library stream.  If the VDE populate is successful, then
$! PLMENU$$POPCLASS.COM is submitted to perform the CMS populations for the root
$! facilities listed in the file passed in P6.
$!
$! Inputs:    Description               (local symbol name)
$!            ------------------------- -------------------                                   
$!  P1	      product			(root)
$!  P2        stream to populate	(target_stream)
$!  P3        parent stream		(input_stream)
$!  P4	      queue for CMS jobs	(queue)
$!  P5        work directory for logs   (scratch_dir)
$!  P6        Facility list file        (facility_file)
$!  P7        Populate & marge options  (poption/moption)
$!		CMS, VDE or BOTH
$!
$! FILES:
$!	All work files are located in the directory passed in P5.
$!
$!	    CMS POPULATE STATUS FILE
$!	    format: 'target_stream'_POP.DAT
$!
$!	    CMS populate logfiles
$!          format: 'facility'$POPFACS.LOG
$!
$!	X-2	BMAN	Brian Man		30-MAR-1998
$!		Changed format: 'target_stream'_POP.DATA to 
$!		'target_stream'_POP.DAT for consistency
$!
$!	X-3	BMAN	Brian Man		14-APR-1998
$!		Set the blank_key to 60 chars and all of the
$!		associated vars (ie. stat_key) to be the same.
$!
