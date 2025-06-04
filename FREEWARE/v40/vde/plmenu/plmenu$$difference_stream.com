$ sav_verify = 'f$verify(0)
$ set = "set"
$ set symbol/scope=noglobal
$ delete /symbol/local set
$!
$ if f$trnlnm("PLMENU$DEBUG") .nes. "" then set verify
$ set control = Y
$ on control_y then GOTO Control_y
$ set noon
$!
$ user = F$edit("''f$getjpi("","USERNAME")'","Trim,Upcase")
$ target = f$trnlnm("PLMENU$DEFAULT_TARGET_STREAM")
$ source = f$trnlnm("PLMENU$DEFAULT_SOURCE_STREAM")
$ product = f$trnlnm("PLMENU$DEFAULT_PRODUCT")
$ work_dir = f$trnlnm("PLMENU$DEFAULT_DIRECTORY")
$ batch_queue = f$trnlnm("PLMENU$DEFAULT_BATCH_QUEUE")
$ report_type = ""
$ facility_spec = ""
$ cur_dir = f$environment("default")
$ mnemonic = ""
$ remark = ""
$ submit_time = ""
$ default_value = ""
$ type sys$input

Difference Stream Version V1.5-000

Enter ? or "HELP" at any prompt to obtain help.
Prompt default values (if any) appear in brackets [].
Enter <Ctrl/Z>, "Quit" or "Exit" to exit immediately.

$!
$!
$!+
$!   USER PROMPTS
$
$!+----------------------------------------------------------------------------
$! obtain the VDE library mnemonic (eg EVMS, VMSTEST, etc.), and store it in
$! the symbol MNEMONIC.  From the mnemonic, derive the CMS root and store it
$! in ROOT. 
$
$! By default we use the specified mnemonic value, the translation of the
$! PLMENU$DEFAULT_PRODUCT logical name, or the first translation of the
$! system-wide library list.
$!+----------------------------------------------------------------------------
$Get_mnemonic:
$!----------
$ root = "???"
$ if mnemonic .nes. ""
$ then
$   default_value = mnemonic
$ else
$   default_value = f$trnlnm("PLMENU$DEFAULT_PRODUCT")
$   if default_value .eqs. "" then default_value = "''f$trnlnm(""VDE$LIBRARY_DEFAULTS_MNEMONICS"",,0)'"
$ endif
$!
$ user_prompt = "VDE library"
$ prompt_symbol = "mnemonic"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   type sys$input

   This is the first prompt.

$   GOTO Get_mnemonic
$ endif
$!
$Validate_mnemonic:
$!
$ @vde$system:plmenu$$set_vde_library 'mnemonic'
$ if .not. $status
$ then
$   root = "???"
$   mnemonic = ""
$   GOTO Get_mnemonic
$ endif
$ root = f$trnlnm("PLMENU$CMSROOT")
$ mnemonic = f$trnlnm("PLMENU$MNEMONIC")
$!
$!
$!+-----------------------------------------------------------------------------
$! obtain the first stream to difference
$!+-----------------------------------------------------------------------------
$Get_source:
$!----------
$ if source .nes. ""
$ then
$   default_value = source
$ else
$   default_value = f$trnlnm("PLMENU$DEFAULT_SOURCE_STREAM")
$ endif
$!
$ user_prompt = "First Stream"
$ prompt_symbol = "source"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Mnemonic
$ endif
$!
$!+-----------------------------------------------------------------------------
$! Obtain the second stream to difference
$!+-----------------------------------------------------------------------------
$Get_target:
$!----------
$ if target .nes. "" 
$ then
$   default_value = target
$ else
$   default_value = f$trnlnm("PLMENU$DEFAULT_TARGET_STREAM")
$ endif
$ user_prompt = "Second Stream"
$ prompt_symbol = "target"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_source
$ endif
$!
$!+-----------------------------------------------------------------------------
$! obtain the batch queue to which they which to submit the difference jobs(s)
$!+-----------------------------------------------------------------------------
$Get_Queue:
$!---------
$ if batch_queue .nes. ""
$ then
$   default_value = batch_queue
$ else
$   default_value = F$trnlnm("PLMENU$DEFAULT_BATCH_QUEUE")
$   if default_value .eqs. "" then default_value = "SYS$LOBUILD"
$ endif
$!
$ user_prompt = "Queue to submit difference job(s)"
$ prompt_symbol = "batch_queue"
$ GOSUB Get_Text_Value
$!
$ if batch_queue .eqs. "" 
$ then
$   if default_value .eqs. ""
$   then
$	goto Get_queue 
$   else
$       batch_queue = default_value
$   endif
$ endif
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Target
$ endif
$!
$! validate the queue
$!
$ if .not. f$getqui("DISPLAY_QUEUE","QUEUE_BATCH","''batch_queue'")
$ then
$   write sys$output -
    "%PLMENU-E-INVQUEUE, ''batch_queue' is not a valid batch queue"
$   batch_queue = ""
$   GOTO Get_Queue
$ endif
$!
$!+-----------------------------------------------------------------------------
$!  Obtain the work directory to be used for log and work files
$!+-----------------------------------------------------------------------------
$Get_Work_Dir:
$!-------------
$ if work_dir .nes. ""
$ then
$   default_value = work_dir
$ else
$   default_value = F$trnlmn("PLMENU$DEFAULT_DIRECTORY")
$   if default_value .eqs. "" then default_value = cur_dir
$ endif
$!
$ user_prompt = "Directory for logs and work files"
$ prompt_symbol = "work_dir"
$ GOSUB Get_Text_Value
$ if work_dir .eqs. "" 
$ then
$   if default_value .eqs. ""
$   then
$	goto Get_work_dir 
$   else
$       work_dir = default_value
$   endif
$ endif
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Queue
$ endif
$!
$! validate the work directory
$!
$ if F$locate("[",work_dir) .eq. f$length(work_dir) .and. F$locate("<",work_dir) .eq. f$length(work_dir)
$ then
$    work_dir = work_dir + "¿?"
$    work_dir = work_dir - ":¿?" - "¿?"
$    work_dir = f$trnlnm("''work_dir'")
$    if work_dir .eqs. ""
$    then
$	write sys$output -
	"%PLMENU-E-INVLOG, invalid logical name, logical does not exist."
$	GOTO Get_work_dir
$     endif
$ endif
$!
$ work_dir = F$parse("''work_dir'",,,"DEVICE") + -
             F$parse("''work_dir'",,,"DIRECTORY")
$!
$ work_dir = f$parse("''work_dir'")
$ if work_dir .eqs. ""
$ then
$   write sys$output -
    "%PLMENU-E-INVDIR, invalid work directory, directory does not exist."
$   GOTO Get_work_dir
$ else
$   work_dir = work_dir - ".;"
$ endif
$!
$!
$!+-----------------------------------------------------------------------------
$! obtain a name for the differences report file, default to printing the
$! differences in the logfile.
$!+-----------------------------------------------------------------------------
$Get_Report_type:
$!---------------
$ if report_type .nes. ""
$ then
$   default_value = report_type
$ else
$   default_value = "LOG"
$ endif
$!
$ user_prompt = "Report file name ?"
$ prompt_symbol = "report_type"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Work_Dir
$ else
$   default_value = ""
$ endif
$!
$! clobber the characters that cannot be used in an ODS2 file specification
$!
$ i = 0
$ temp_target = ""
$Get_Report_type_target_loop:
$ chr = f$edit(f$extract(i,1,target),"UPCASE,COLLAPSE")
$ if chr .nes. ""
$ then
$   if chr .ges. "A" .and. chr .les. "Z" then temp_target = temp_target + chr
$   if chr .ges. "0" .and. chr .les. "9" then temp_target = temp_target + chr
$   if chr .eqs. "$" .or. chr .eqs. "_" then temp_target = temp_target + chr
$   i = i + 1
$   goto Get_Report_type_target_loop
$ endif
$!
$! clobber the characters that cannot be used in an ODS2 file specification
$!
$ i = 0
$ temp_source = ""
$Get_Report_type_source_loop:
$ chr = f$edit(f$extract(i,1,source),"UPCASE,COLLAPSE")
$ if chr .nes. ""
$ then
$   if chr .ges. "A" .and. chr .les. "Z" then temp_source = temp_source + chr
$   if chr .ges. "0" .and. chr .les. "9" then temp_source = temp_source + chr
$   if chr .eqs. "$" .or. chr .eqs. "_" then temp_source = temp_source + chr
$   i = i + 1
$   goto Get_Report_type_source_loop
$ endif
$
$ report_type = f$edit(report_type,"UPCASE")
$ if (f$locate(report_type, "LOG") .eq. 0)
$ then
$   report_file_name = "LOG"
$ else
$   default_spec = work_dir + temp_source + "_" + temp_target + "_diff.lis"
$   report_file_name = F$parse(report_type,default_spec)
$   if report_file_name .eqs. ""
$   then
$	write sys$output "%PLMENU-E-INVREPFIL, invalid report file name"
$	report_type = ""
$	GOTO get_report_type
$   else
$	write sys$output "%PLMENU-I-REPFILNAM, report file name: ''report_file_name'"
$   endif
$ endif
$!
$!+-----------------------------------------------------------------------------
$! obtain the facilities to process
$!+-----------------------------------------------------------------------------
$Get_Facility_Spec:
$!-----------------
$ if facility_spec .nes. ""
$ then
$   default_value = facility_spec
$ else
$   default_value = "ALL"
$ endif
$!
$ user_prompt = "Facility to process"
$ prompt_symbol = "facility_spec"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Report_Type
$ else
$   default_value = ""
$ endif
$!
$ facility_spec = f$edit(facility_spec,"UPCASE")
$ if (f$locate(facility_spec, "ALL") .eq. 0)
$ then
$   full_facility_spec = root +":[*.CMS]"
$ else
$   full_facility_spec = root +":[''facility_spec'.CMS]"
$ endif
$!
$ temp_spec = f$parse(full_facility_spec)
$ if temp_spec .eqs. ""
$ then
$   write sys$output -
    "%COMSTRM-E-NOFACS, No facility found for ''facility_spec'"
$   facility_spec = ""
$   GOTO Get_Facility_Spec
$ endif
$!
$!+-----------------------------------------------------------------------------
$! Confirm with teh user that the input is satisfactory
$!+-----------------------------------------------------------------------------
$Confirm_Input:
$!-------------
$ jobname = f$parse(report_file_name,,,"NAME")
$ if jobname .eqs. "LOG"
$ then
$   jobname = temp_source + "_" + temp_target + "_DIFF"
$ endif
$ write sys$output ""
$ write sys$output   " Source stream name:   ''source'"
$ write sys$output   " Target stream name:   ''target'
$ write sys$output   " Target Facility spec: ''full_facility_spec'
$ if report_file_name .eqs. "LOG"
$ then
$   write sys$output " Differences log file: <DEFAULT LOG FILE>"
$ else
$   write sys$output " Differences log file: ''report_file_name'."
$ endif
$ write sys$output ""
$ write sys$output -
" If you proceed, you will submit a job to difference stream ''source'"
$ write sys$output -
" to stream ''target' for all facilities matching the specification "
$ write sys$output -
" ''full_facility_spec'.  Differences will be reported in "
$ if report_file_name .eqs. "LOG"
$ then
$   write sys$output -
" the log file."
$ else
$   write sys$output -
" file ''report_file_name'."
$ endif
$
$ write sys$output ""
$ user_prompt = "Do you wish to proceed?"
$ default_value = "Yes"
$ prompt_symbol = "submit_ans"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Facility_Spec
$ endif
$!
$ if .not. submit_ans then GOTO Get_Mnemonic
$!
$!+-----------------------------------------------------------------------------
$! submit VMSCMS$:DIFFSTRM.COM to perform the difference
$!+-----------------------------------------------------------------------------
$DO_Submit:
$!
$ submit/noprint/keep/notify -
       /log='work_dir' -
       /name='jobname' -
       /queue='batch_queue' -
       /param=('source','target','work_dir', -
	       'report_file_name','full_facility_spec') -
       VDE$SYSTEM:PLMENU$$DIFFSTRM.COM
$
$!
$!+-----------------------------------------------------------------------------
$! See if the user wants to process any more facilities
$!+-----------------------------------------------------------------------------
$Ask_For_More:
$!------------
$ write sys$output ""
$ user_prompt = "Do you want to process any more facilities?"
$ default_value = "Yes"
$ prompt_symbol = "submit_ans"
$ GOSUB Get_Text_Value
$! 
$ if return_to_previous_prompt
$ then
$   GOTO Get_Report_Type
$ endif
$!
$ if submit_ans
$ then
$   facility_spec = ""
$   GOTO Get_Report_Type
$ endif
$!
$Control_y:
$!
$Main_Exit:
$ EXIT 1+0*'f$verify(sav_verify)
$!
$!
$!+-----------------------------------------------------------------------------
$!
$! obtain a text value for the symbol name stored in symbol prompt_symbol
$!
$!  prompt_symbol => symbol to contain the value to the symbol name to prompt
$!  user_prompt   => prompt string for 'prompt_symbol'
$!  default_value => default value for 'prompt_symbol'(if any)
$!
$!+-----------------------------------------------------------------------------
$Get_Text_Value:
$!-------------
$ return_to_previous_prompt = 0
$!
$ if default_value .eqs. ""
$ then
$   inquire_prompt = user_prompt
$ else
$   inquire_prompt = user_prompt + " [''default_value']"
$ endif
$!
$ if (f$length(inquire_prompt) + (2*f$length(default_value))) .ge. f$getdvi("TT","DEVBUFSIZ")
$ then
$   write sys$output " ''inquire_prompt'"
$   read/End_of_File=Main_Exit/Error=time_expired/timeout=255 -
      /prompt=" : " SYS$COMMAND 'prompt_symbol'
$ else
$   read/End_of_File=Main_Exit/Error=time_expired/timeout=255 -
      /prompt=" ''inquire_prompt': " SYS$COMMAND 'prompt_symbol'
$ endif
$!
$ 'prompt_symbol' = f$edit('prompt_symbol',"TRIM")
$!
$ if 'prompt_symbol'  .eqs. ""
$ then
$   if default_value .eqs. ""
$   then
$	goto Get_Text_Value 
$   else
$       'prompt_symbol'  = default_value
$   endif
$ endif
$!
$ temp_value = f$extract(0,4,f$edit('prompt_symbol',"UPCASE"))
$!
$ if temp_value .eqs. "?" .or. (F$locate(temp_value,"HELP") .eq. 0) 
$ then
$   GOSUB Help_'prompt_symbol'
$   GOTO Get_Text_Value
$ endif
$
$ if F$locate(temp_value,"QUIT") .eq. 0 .OR. -
     F$locate(temp_value,"EXIT") .eq. 0 
$ then
$   GOTO Main_Exit
$ endif
$!
$ if (F$locate(temp_value,"PREV") .eq. 0 .OR. -
     F$locate(temp_value,"BACK") .eq. 0 ) .AND. -
     (F$length('prompt_symbol') .lt. 5)
$ then
$   'prompt_symbol' = default_value
$   return_to_previous_prompt = 1
$ endif
$!
$ RETURN
$!
$!
$Time_Expired:
$!------------
$ write sys$output "Timeout on input, exiting..."
$ GOTO Main_exit
$!
$Help_Mnemonic:
$!
$ type sys$input 

   Enter the VDE library containing the streams to difference.
   Examples of common VDE libraries are EVMS and VMSTEST.

$!
$ RETURN
$!
$Help_source:
$!-----------
$!
$ type sys$input

     Enter a stream name to difference.  Enter BACK or PREV to return
     to the mnemonic prompt.
	
$!
$ RETURN
$!
$!
$Help_target:
$!-----------
$!
$ type sys$input 

     Enter the second stream to be differenced.  Enter BACK or PREV to return
     to the first stream prompt.

$!
$!
$ RETURN
$!
$Help_Batch_queue:
$!
$ type sys$input

     Enter the batch queue in which to submit the difference stream 
     job(s).  You can define the logical PLMENU$DEFAULT_BATCH_QUEUE to
     supply a default value to this prompt.  Enter BACK or PREV to return to
     the target_stream prompt.

$!
$!
$ RETURN
$!
$Help_Work_dir:
$!
$ type sys$input

     Enter a directory to be used for the log files and work files
     used by difference stream.  The work directory defaults to the
     current directory.  Please use brackets, "[,]" to enclose the
     directory name.  If the input does not include brackets, the
     input is assumed to be a logical name.  Define the logical name
     PLMENU$DEFAULT_DIRECTORY to automatically supply a default value
     to this prompt. Enter BACK or PREV to return to the queue prompt.

$!
$ RETURN
$!
$Help_Submit_Ans:
$!
$ type sys$input

     Enter Yes/No or True/False to this question.  
     Enter BACK or PREV to return to the previous prompt.

$!
$ RETURN
$!
$Help_Submit_Time:
$!----------------
$!
$ type sys$input

     Enter the time at which the difference stream job should begin.
     Enter BACK or PREV to return to the work directory prompt.
     
$!
$ RETURN
$
$
$Help_Report_Type:
$!
$ type sys$input

    By default the procedure submitted by this job will print any differences
    between CMS streams in the log file.  If you enter a file name at this
    prompt, the differences will be printed to the named file.  If you omit the
    directory in the file name the directory will default to the work
    directory.  The default file type for the report is .LIS.

    Enter BACK or PREV to return to the previous prompt.
    
$ write sys$output ""
$!
$ RETURN
$!
$Help_Facility_Spec:
$!
$ type sys$input

    By default the procedure submitted will compare the streams for propagating
    non-archived facilities in the VDE library's CMS root.  Archived facilities
    and facilities marked "no propagation" are skipped by this procedure.

    You may enter a full facility name or a partial facility name with
    wildcards.  For example you may enter SYS* to process all facilities
    beginning with SYS.  

    Enter BACK or PREV to return to the previous prompt.

$!
$ RETURN
$!
$!+-----------------------------------------------------------------------------
$!
$! DIFFERENCE_STREAM.COM
$!
$! Description:
$!
$!  The procedure submits a job to compare the generations for two streams (CMS
$! classes).  A report of generation differences between the streams is
$! reported to a text file or to the log file.
$!
$!  The command procedure VDE$SYSTEM:PLMENU$$DIFFSTRM.COM is submitted. 
$!  The command file VDE$SYSTEM:PLMENU$$DIFFSTRM.COM uses the FACDIFF
$!  tool to compare the CMS classes.
$!
$!  AUTHOR:
$!	David Sweeney	    APR-1992
$!
$!  MODIFICATIONS:
$!	Steve Hoffman		9-May-1995
$!		Fixed latent parenthetical bug
$!	Steve Hoffman		15-May-1995
$!		Integrated into VDE environment
$!
$!+-----------------------------------------------------------------------------
