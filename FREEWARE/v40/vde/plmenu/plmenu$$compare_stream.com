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
$ source = f$trnlnm("PLMENU$DEFAULT_SOURCE_STREAM")
$ root = f$trnlnm("PLMENU$DEFAULT_PRODUCT")
$ work_dir = f$trnlnm("PLMENU$DEFAULT_DIRECTORY")
$ batch_queue = f$trnlnm("PLMENU$DEFAULT_BATCH_QUEUE")
$ cur_dir = f$environment("default")
$ report_type = "LOG"
$ file_name = ""
$ facility_spec = "ALL"
$ job_name = ""
$ mnemonic = ""
$ submit_time = ""
$ submit_ans = ""
$ old_vdelib = f$trnlnm("VDE$LIBRARY")
$!
$ type sys$input

Compare Stream Version V1.5-000

Enter ? or "HELP" at any prompt to obtain help.
Prompt default values (if any) appear in brackets [].
Enter <Ctrl/Z>, "Quit" or "Exit" to exit immediately

$!
$!
$!
$!+
$!   USER PROMPTS
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
$! obtain the stream for which CMS and VDE will be compared.
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
$ user_prompt = "Stream to compare"
$ prompt_symbol = "source"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_mnemonic
$ endif
$!
$ source = f$edit(source,"UPCASE")
$!
$!
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
$ user_prompt = "Directory for log files"
$ prompt_symbol = "work_dir"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Source
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
$!+-----------------------------------------------------------------------------
$! obtain the batch queue to submit the compare job
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
$ user_prompt = "Queue to submit compare job"
$ prompt_symbol = "batch_queue"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Work_Dir
$ else
$   default_value = ""
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
$ facility_cnt = 0
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
$   GOTO Get_Queue
$ else
$   default_value = ""
$ endif
$!
$ temp_source = source - "." - "."
$ report_type = f$edit(report_type,"UPCASE")
$ if (f$locate(report_type, "LOG") .eq. 0)
$ then
$   report_file_name = "LOG"
$ else
$   default_spec = work_dir + temp_source + "_compare.lis"
$   report_file_name = F$parse(report_type,default_spec)
$   if report_file_name .eqs. ""
$   then
$	write sys$output "%PLMENU-E-INVFIL, invalid report file name"
$	report_type = ""
$	GOTO get_report_type
$   else
$       write sys$output "%PLMENU-I-REPFILNAM, report file name: ''report_file
$   endif
$ endif
$!
$ jobname = "''temp_source'_COMPARE"
$!
$!
$!+-----------------------------------------------------------------------------
$! obtain the facility files to process
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
    "%PLMENU-E-NOFACS, No facility found for ''facility_spec'"
$   facility_spec = ""
$   GOTO Get_Facility_Spec
$ endif
$! 
$!
$!+-----------------------------------------------------------------------------
$! obtain the time to submit the populate job
$!+-----------------------------------------------------------------------------
$Get_default_time:
$!-----------------
$ if submit_time .nes. ""
$ then
$   default_value = submit_time
$   goto Get_submit_time
$ else
$   default_value = "NOW"
$ endif
$!
$Get_Submit_Time:
$ current_time = "''f$time()'"
$ prompt_symbol = "submit_time"
$ user_prompt = "Time to submit compare job?"
$ GOSUB Get_Text_Value
$! 
$ if return_to_previous_prompt
$ then
$   GOTO Get_Facility_spec
$ endif
$!
$ if submit_time .eqs. ""
$ then
$   submit_time = "NOW"
$ endif
$!
$ if f$edit(submit_time,"UPCASE") .eqs. "NOW" then submit_time = "''f$time()'"
$!
$! validate the entered time
$ submit_time = f$cvtime(submit_time,"Absolute")
$ if .not. $status
$ then
$   write sys$output "%PLMENU-E-INVTIME, ''submit_time' is not a valid time"
$   submit_time = ""
$   goto Get_default_time
$ endif
$! f$cvtime right justifies the time, so put the space in if it was taken out
$ if f$extract(1,1,submit_time) .eqs. "-"
$ then
$   submit_time = " " + submit_time
$ endif
$!
$!+-----------------------------------------------------------------------------
$!
$! Validate the inputs which are not already validated... i.e. those which take
$! more than a few seconds to validate.  First confirm with the user the inputs
$! they have entered.
$!
$!+-----------------------------------------------------------------------------
$Validate_input:
$!--------------
$
$ write sys$output ""
$ write sys$output   " Compare stream name:  ''source'"
$ write sys$output   " Target Facility spec: ''full_facility_spec'
$ if report_file_name .eqs. "LOG"
$ then
$   write sys$output " Differences log file: <DEFAULT LOG FILE>"
$ else
$   write sys$output " Differences log file: ''report_file_name'."
$ endif
$ write sys$output ""
$ write sys$output -
" If you proceed you will submit a job to compare stream ''source' for all "
$ write sys$output -
" facilities matching the file specification ''full_facility_spec'."
$ if report_file_name .eqs. "LOG"
$ then
$   write sys$output -
" Differences in the stream will be reported to the log file."
$ else
$   write sys$output -
" Differences in the stream will be reported to file "
$   write sys$output " ''report_file_name'"
$ endif
$ write sys$output ""
$ user_prompt = "Do you wish to proceed?"
$ default_value = "Yes"
$ prompt_symbol = "submit_ans"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Default_Time
$ endif
$!
$ if .not. submit_ans then GOTO Get_mnemonic
$!
$ valid_input = 1
$!
$ GOSUB Validate_Source
$ if .not. valid_input
$ then
$   source = ""
$   GOTO Get_Source
$ endif
$!
$Submit_Compare_Job:
$ submit/NOPRINTER/KEEP/NOTIFY -
  /after="''submit_time'" -
  /queue='batch_queue' -
  /log='work_dir' -
  /name='jobname' -
  /param=('product','source','full_facility_spec','report_file_name','work_dir') -
  VDE$SYSTEM:PLMENU$$COMPSTRM.COM
$!
$Control_y:
$!
$Main_Exit:
$!
$! restore the old value of VDE$LIBRARY
$!
$ if old_vdelib .eqs. ""
$ then
$   deassign VDE$LIBRARY
$ else
$   if old_vdelib .nes. f$trnlnm("VDE$LIBRARY")
$   then
$	define/nolog VDE$LIBRARY 'old_vdelib'
$   endif
$ endif
$!
$ EXIT 1+0*'f$verify(sav_verify)
$!
$!+-----------------------------------------------------------------------------
$! validate the source stream exists before submitting the job
$!+-----------------------------------------------------------------------------
$Validate_source:
$ write sys$output "Validating stream ''source' in VDE..."
$ define/user/nolog sys$output _nla0:
$ define/user/nolog sys$error _nla0:
$ VDE show stream 'source'
$ if $status .ne. %x10000001
$ then
$   write sys$output "%PLMENU-E-VALERR, validation error for stream ''source' in VDE"
$   source = ""
$   valid_input = 0
$ endif
$!
$ RETURN
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
$ if (f$length(inquire_prompt) + (2 * f$length(default_value))) .ge. f$getdvi("TT","DEVBUFSIZ")
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
$Time_Expired:
$!------------
$ write sys$output "Timeout on input, exiting..."
$ GOTO Main_exit
$!
$Help_Batch_Queue:
$!
$ type sys$input 

    Enter a batch queue to which the compare stream job should be submitted.
    Define the logical PLMENU$DEFAULT_BATCH_QUEUE to automatically supply a
    default value to this prompt.  Enter BACK or PREV to return to the 

$!
$ RETURN
$!
$Help_mnemonic:
$!
$ type sys$input 

    Enter the product library containing the stream to rename.  Examples
    of products are "EVMS" and "VMSTEST".  Define the logical name
    PLMENU$DEFAULT_PRODUCT to override the default value (the first
    translation from VDE$LIBRARY_DEFAULTS_MNEMONICS) for this prompt.

$!
$ RETURN
$!
$!
$Help_source:
$!-----------
$!
$ type sys$input

    Enter the stream name for which to compare VDE and CMS.   Define the
    logical PLMENU$DEFAULT_SOURCE_STREAM  to automatically supply a default
    value to this prompt.  Enter BACK or  PREV to return to product name
    prompt.

$!
$ RETURN
$!
$Help_Work_dir:
$!
$ type sys$input

    Enter a directory for the log file and report file(optional) created
    by the compare stream procedure.   The work directory defaults to the
    current directory.  Please use  brackets, "[,]" to enclose the directory
    name.  If the input does  not include brackets, the input is assumed to
    be a logical name.  Define the logical name PLMENU$DEFAULT_DIRECTORY to
    automatically  supply a default value to this prompt.  Enter BACK or PREV
    to return to the previous prompt.

$!
$ RETURN
$
$Help_Report_Type:
$!
$ type sys$input

    By default the procedure submitted by this job will print any differences
    between CMS and VDE in the log file.  If you enter a file name at this
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

    By default the procedure submitted will compare VDE to CMS for all 
    non-archived facilities in the VDE library's CMS root.  Archived
    facilities are skipped by the procedure.  Facilities marked "no
    propagation" are compared by this procedure.

    You may enter a full facility name or a partial facility name with
    wildcards.  For example you may enter SYS* to process all facilities
    beginning with SYS.  

    Enter BACK or PREV to return to the previous prompt.

$!
$ RETURN
$!
$Help_Submit_Ans:
$!---------------
$!
$   type sys$input

    Answer affirmatively if you wish to proceed and compare the stream.
    Enter BACK or PREV to return to the previous prompt.

$!
$ RETURN
$!
$Help_Submit_Time:
$!----------------
$!
$   type sys$input

    Enter the time to begin the compare stream job.  Enter NOW to submit
    the job immediately.  Enter an absolute or delta time to begin the job
    at some later time.  Enter BACK or PREV to return to the previous prompt.

$!
$ RETURN
$!
$!------------------------------------------------------------------------------
$!
$! COMPARE_STREAM.COM
$!
$! This command procedures submits a job to produce a report of differences
$! between the CMS and the VDE represntation of a stream(cms class).
$! The procedure submits command file VDE$SYSTEM:PLMENU$$COMPSTRM to
$! perform the actual comparison.
$!
$! Author:  David Sweeney  20-MAR-1991
$!
$!    
$! MODIFICATIONS:
$!      Steve Hoffman           9-May-1995
$!              Fixed latent parenthetical bug
$!      Steve Hoffman           15-May-1995
$!		Integrated into VDE
$! 
$!------------------------------------------------------------------------------
