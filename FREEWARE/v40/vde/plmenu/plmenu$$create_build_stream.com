$ sav_verify = 'f$verify(0)
$ set = "set"
$ set symbol/scope=noglobal
$ delete /symbol/local set
$!
$ if f$trnlnm("PLMENU$DEBUG") .nes. "" then set verify
$ set control = Y
$ on control_y then GOTO Control_y
$ set noon
$ create_option = "YES"
$ source = f$trnlnm("PLMENU$DEFAULT_SOURCE_STREAM")
$ target = f$trnlnm("PLMENU$DEFAULT_TARGET_STREAM")
$ variant = f$trnlnm("PLMENU$DEFAULT_VARIANT_GENERATION")
$ mnemonic = f$trnlnm("PLMENU$DEFAULT_PRODUCT")
$ user = F$edit("''f$getjpi("","USERNAME")'","Trim,Upcase")
$ work_dir = f$trnlnm("PLMENU$DEFAULT_DIRECTORY")
$ batch_queue = f$trnlnm("PLMENU$DEFAULT_BATCH_QUEUE")
$ cur_dir = f$environment("default")
$ owner = user
$ remark = ""
$ submit_time = ""
$ default_value = ""
$ type sys$input

Create Stream Version V1.3

Enter ? or "HELP" at any prompt to obtain assistance.
Prompt default values (if any) appear in brackets []
Enter <Ctrl/Z>, "Quit" or "Exit" to exit immediately.
Submission time value must be NOW, or some future time.

$!
$! See if the VDE$FETCH_METHOD logical is defined to VDE.
$! If the logical is defined to VDE, create and populate the
$! stream in the VDE database only.  If the logical is undefined,
$! the stream will be created in both VDE and CMS.
$!
$ stream_fetch_method = f$trnlnm("VDE$FETCH_METHOD")
$ stream_fetch_method = F$edit(stream_fetch_method,"Trim,UPCASE")
$!
$ if stream_fetch_method .EQS. "VDE"
$ then
$   write sys$output -
   "%CRESTRM-I-CREATEVDE, Stream will be created in VDE database only, NO CMS CLASSES"
$   write sys$output " "
$   GOTO Skip_CMS_Write_Check
$ endif
$!
$! CMS libraries will be processed so ...
$! Check if this account has write access to the Masterpack CMS libraries
$!
$ @vde$system:plmenu$$validate_cms_access
$ if .not. $status
$ then
$   write sys$output - 
 "%CRESTRM-F-NOCMSWRITE, This account does not have write access to CMS libraries"
$   GOTO Main_Exit
$ endif
$!
$Skip_CMS_Write_Check:
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
$! obtain the development stream for which the build class will be used
$! store the value in symbol SOURCE
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
$ user_prompt = "Parent stream for the new stream"
$ prompt_symbol = "source"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Mnemonic 
$ endif
$!
$!
$!+-----------------------------------------------------------------------------
$! obtain information whether the new stream will be child to the SOURCE
$! or become a variant stream.  Store the symbol in VARIANT.
$!+-----------------------------------------------------------------------------
$Get_variant:
$!
$! no default answer
$!
$ user_prompt = "Is the new stream a variant stream (Y/N)"
$ prompt_symbol = "variant"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_source
$ endif
$!
$ variant = f$edit(variant, "Trim,Upcase")
$ if (f$locate("Y", variant) .ne. 0) .and. (f$locate("N", variant) .ne. 0) 
$ then
$     write sys$output "You need to answer Yes or No."
$     goto Get_variant
$ endif
$!
$ if (f$locate("Y", variant) .eq. 0)
$ then
$     variant = "NOMAIN "
$ else
$     variant = "MAIN "
$ endif
$!
$!
$!+-----------------------------------------------------------------------------
$! Obtain the new stream name, store in symbol TARGET
$!+-----------------------------------------------------------------------------
$Get_target:
$!----------
$ if target .eqs. ""
$ then
$   default_value = f$trnlnm("PLMENU$DEFAULT_TARGET_STREAM")
$   if default_value .eqs. "" then default_value = source + "_BLD0"
$ else
$   default_value = target
$ endif
$!
$ user_prompt = "Enter the new stream name"
$ prompt_symbol = "target"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_source
$ endif
$!
$! see if the user wants a list of existing streams 
$!
$ target = f$edit(target,"UPCASE")
$ if F$locate(target,"SHOW") .eq. 0
$ then
$   write sys$output "Getting list of streams matching *''source*..."
$   VDE show stream *'source'*
$   target = ""
$   GOTO Get_target
$ endif
$!
$!
$!+-----------------------------------------------------------------------------
$!  get a remark to be associated with the new stream
$!+-----------------------------------------------------------------------------
$Get_Remark:
$!----------
$ if remark .eqs. "" 
$ then
$   default_value = "Decendent of ''source'"
$ else
$   default_value = "''remark'"
$ endif
$ user_prompt = "Remark string for ''target'"
$ prompt_symbol = "remark"
$ GOSUB Get_Text_Value
$!
$ if f$locate("""",remark) .lt. f$length(remark)
$ then
$   remark = ""
$   GOTO Get_Remark
$ endif
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_target
$ else
$   default_value = ""
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
$ user_prompt = "Directory for log files"
$ prompt_symbol = "work_dir"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Remark
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
	"%CRESTRM-F-INVLOG, invalid logical name, logical does not exist."
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
    "%CRESTRM-F-INVDIR, invalid work directory, directory does not exist."
$   GOTO Get_work_dir
$ else
$   work_dir = work_dir - ".;"
$ endif
$!
$!+
$! obtain the batch queue to which they which to submit the populate jobs(s)
$!+
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
$ user_prompt = "Queue to submit create job"
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
    "%CRESTRM-F-INVQUEUE, ''batch_queue' is not a valid batch queue"
$   batch_queue = ""
$   GOTO Get_Queue
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
$    default_value = submit_time
$    goto Get_submit_time
$ endif
$!
$ current_time = "''f$time()'"
$ cur_comp_time = f$cvtime(current_time,"comparison")
$ current_day = f$cvtime("today",,"weekday")
$ if current_day .eqs. "Saturday" .or. current_day .eqs. "Sunday"
$ then
$   default_value = "NOW"
$   GOTO Get_submit_time
$ endif
$!
$ hhmm = F$integer(f$extract(12,2,current_time) + f$extract(15,2,current_time))
$ if stream_fetch_method .EQS. "VDE"
$ then
$   if hhmm .gt. 600 .and. hhmm .lt. 1800 
$   then 
$	default_value = "18:00" 
$   else
$	default_value = "NOW"
$   endif
$ else
$   if hhmm .gt. 300 .and. hhmm .lt. 2359
$   then
$	default_value = "TOMORROW"
$   else
$	default_value = "NOW"
$   endif
$ endif
$!
$ if submit_time .nes. ""
$ then
$   sub_comp_time = f$cvtime(submit_time,"comparison")
$   if .not. (sub_comp_time .les. cur_comp_time) then default_value = submit_time
$ endif
$!
$Get_Submit_Time:
$ current_time = "''f$time()'"
$ prompt_symbol = "submit_time"
$ user_prompt = "Time to begin create job?"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Queue
$ endif
$!
$ if submit_time .eqs. ""
$ then
$   if default_value .eqs. ""
$   then
$	goto Get_default_time
$   else
$       submit_time = default_value
$   endif
$ endif
$!
$ if f$edit(submit_time,"UPCASE") .eqs. "NOW" then submit_time = "''f$time()'"
$!
$! validate the entered time
$ submit_time = f$cvtime(submit_time,"Absolute")
$ if .not. $status
$ then
$   write sys$output "%POP-F-INVTIME, ''submit_time' is not a valid time"
$   submit_time = ""
$   goto Get_default_time
$ endif
$!
$! f$cvtime right justifies the time, so put the space in if it was taken out
$ if f$extract(1,1,submit_time) .eqs. "-"
$ then
$   submit_time = " " + submit_time
$ endif
$!
$! make sure the user doesn't enter a time in the past
$ cur_comp_time = f$cvtime(current_time,"comparison")
$ sub_comp_time = f$cvtime(submit_time,"comparison")
$ if sub_comp_time .les. cur_comp_time
$ then
$   write sys$output "You are not allowed to enter a time in the past."
$   submit_time = ""
$   goto Get_Default_time
$ endif
$!
$ submit_ans = ""
$ submit_day = f$cvtime(submit_time,,"weekday")
$ if submit_day .eqs. "Saturday" .or. submit_day .eqs. "Sunday" then GOTO Validate_Input
$ hhmm = F$integer(f$extract(12,2,submit_time) + f$extract(15,2,submit_time))
$!
$ if (stream_fetch_method .EQS. "VDE" .AND. (hhmm .gt. 600 .and. hhmm .lt. 1800))
      .OR.
     (stream_fetch_method .NES. "VDE" .AND. (hhmm .gt. 300 .and. hhmm .lt. 2359))
$ then
$   GOSUB Override_Submit_limits
$   if return_to_previous_prompt then goto Get_Default_time
$   if submit_ans then goto Validate_Input
$   submit_time = ""
$   GOTO Get_Mnemonic
$ endif
$!
$!+-----------------------------------------------------------------------------
$! Validate the inputs which are not already validated... i.e. those which take
$! more than a few seconds to validate.  First confirm with the user the source
$! and target streams.
$!+-----------------------------------------------------------------------------
$Validate_input:
$!--------------
$ write sys$output ""
$ write sys$output " You are attempting to create a new stream, ''target', based"
$ write sys$output " on the current contents of stream ''source'."
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
$ if .not. submit_ans then GOTO Get_Mnemonic
$!
$ valid_input = 1
$!
$ GOSUB Validate_Source
$ if .not. valid_input
$ then
$   source = ""
$   target = ""
$   remark = ""
$   GOTO Get_Source
$ endif
$!
$ GOSUB Validate_target
$ if .not. valid_input
$ then
$   target = ""
$   GOTO Get_Target
$ endif
$!
$SUBMIT_JOB:
$!
$ submit := SUBMIT/NOTIFY/NOPRINTER/KEEP
$ jobname = "CREATE_" + TARGET - "." - "."
$ submit/after="''submit_time'"/queue='batch_queue'/log='work_dir'/name='jobname' -
  /param=('mnemonic','source','target',"''remark'",'batch_queue','work_dir','variant') -
  VDE$SYSTEM:PLMENU$$CRESTRM.COM
$!!!
$!!! BBB  - for testing only
$!!!
$ type sys$input
     submit := SUBMIT/NOTIFY/NOPRINTER/KEEP
$!
$ jobname = "CREATE_" + TARGET - "." - "."
$ show symb jobname
$ write sys$output "    submit/after=''submit_time'"
$ write sys$output "    /queue=''batch_queue'"
$ write sys$output "    /log=''work_dir'"
$ write sys$output "    /name=''jobname' "
$ write sys$output "    /param=(''mnemonic',''source',''target',''remark',''batch_queue',''work_dir',''variant') "
$ write sys$output "    VDE$SYSTEM:PLMENU$$CRESTRM.COM"
$!!!
$!!! BBB  - for testing only
$!!!
$!
$Control_y:
$!
$Main_Exit:
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
$ if $status .nes. "%X10000001"
$ then
$   write sys$output "%CRESTRM-E-VALERR, validation error for stream ''source' in VDE"
$   source = ""
$   valid_input = 0
$ endif
$!
$ RETURN
$!+-----------------------------------------------------------------------------
$! check if the target stream already exists
$!+-----------------------------------------------------------------------------
$Validate_Target:
$!---------------
$ write sys$output "Validating stream ''target' in VDE..."
$ define/user sys$output _nla0:
$ define/user sys$error _nla0:
$ VDE show stream 'target'
$ if $status .eqs. "%X10000001"
$ then
$   write sys$output "%CRESTRM-E-ALREXISTS, stream ''target' already exists"
$   target = ""
$   GOTO Get_target
$ endif
$!
$ RETURN
$!
$!
$!+-----------------------------------------------------------------------------
$! This routine is called when the user enters a time that does not fit within
$! the range for beginning a create stream job.  The user is forced to confirm
$! theywant to begin the create stream.
$!+-----------------------------------------------------------------------------
$Override_Submit_limits:
$!----------------------
$ submit_ans = "N"
$ type sys$input 

Due to lock conflicts and disk contention on the master pack, this job should
be submitted after 12:00AM and before 3:00AM on working days.  It is strongly
advised to avoid creating a stream during regular working hours.

$Get_submit_answer:
$ user_prompt = "Do you want to submit the job anyway?"
$ default_value = "N"
$ prompt_symbol = "submit_ans"
$ GOSUB Get_Text_Value
$ submit_ans = f$extract(0,1,f$edit(submit_ans,"collapse,upcase"))
$ if submit_ans .eqs. "" then submit_ans = "N"
$!
$ RETURN
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
$   inquire_prompt = user_prompt + ": "
$ else
$   inquire_prompt = user_prompt + " [''default_value']: "
$ endif
$!
$ read/End_of_File=Main_Exit/Error=time_expired/timeout=255 -
      /prompt=" ''inquire_prompt'" SYS$COMMAND 'prompt_symbol' 
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
$ if F$locate(temp_value,"PREV") .eq. 0 .OR. -
     F$locate(temp_value,"BACK") .eq. 0 
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

    Enter a batch queue to which the create stream job and the populate stream
    jobs should be submitted.  Define the logical PLMENU$DEFAULT_BATCH_QUEUE 
    to automatically supply a default value to this prompt.  Enter BACK or 
    PREV to return to the log file directory prompt.     

$!
$ RETURN
$!
$Help_Mnemonic:
$!
$ type sys$input 

    Enter the VDE library mnemonic for the VDE library that will
    contain the stream.  Examples of mnemonics are "EVMS" and
    "VMSTEST".   Define the logical name PLMENU$DEFAULT_PRODUCT
    to override the default value (the default is the first
    translation from the VDE$LIBRARY_DEFAULTS_MNEMONICS logical)
    for this prompt.

$!
$ RETURN
$!
$Help_Remark:
$!
$ write sys$output ""
$ write sys$output -
"    Enter a remark to be associated with stream ''target'." 
$!
$ type sys$input

    Enter BACK or PREV to return to the stream name prompt.

$!
$ RETURN
$!
$Help_source:
$!-----------
$!
$ type sys$input

    Enter the stream name for which the build class is to be used.  For
    example, to create a build class V5.4_bld_1 for development stream
    V5.4, you would enter V5.4.  The parent stream is used for the initial
    population of the stream.  Define the logical PLMENU$DEFAULT_SOURCE_STREAM
    to automatically supply a default value to this prompt.  Enter BACK or
    PREV to return to VDE library mnemonic name prompt.

$!
$ RETURN
$!
$Help_variant:
$!-----------
$!
$ type sys$input 

    Enter Yes or No.  You may also enter the letter Y or N.
    If the new stream is a varaint stream, longer generation expressions
    will be used for modules in that stream.  If the stream is not a
    variant stream, shorter generation expressions will be used.
    For example if the parent stream had a module with the last
    generation number as 3; if a new module was placed into the
    child stream and the child stream was not a variant, the new module
    would have a generation expression of 4.  If the child stream was
    a variant stream the new module would have a generation expression
    of 3A1.

$!
$ RETURN
$!
$Help_target:
$!-----------
$!
$ type sys$input 

    Enter the stream name.  Build stream names should begin with
    the development stream name followed by _BLD'suffix'.  For example, to
    create a build stream, MAGIC_BLD_A for parent stream MAGIC, you
    enter "MAGIC_BLD_A".   Release streams generally use a code name,
    such as "GHOST" or "THETA_EFT".

    Enter "SHOW" to list all streams in the VDE database which contain
    the source stream name.  

    Define the logical PLMENU$DEFAULT_TARGET_STREAM to automatically
    supply a default value to this prompt.

    Enter BACK or PREV to return to the previous prompt. 

$!
$ RETURN
$!
$Help_Work_dir:
$!
$ type sys$input

    Enter a directory for the log files of the create stream jobs. 
    The work directory defaults to the current directory.  Please use
    brackets, "[,]" to enclose the directory name.  If the input does
    not include brackets, the input is assumed to be a logical name.
    Define the logical name PLMENU$DEFAULT_DIRECTORY to automatically
    supply a default value to this prompt.  Enter BACK or PREV to return
    to the previous prompt.

$!
$ RETURN
$!
$!
$Help_Submit_Ans:
$!---------------
$!
$   type sys$input

    Answer in the affirmative should you wish to proceed along and
    create the stream. 

    Enter BACK or PREV to return to the submit time prompt.

$!
$ RETURN
$!
$Help_Submit_Time:
$!----------------
$!
$   type sys$input

    Enter the time to begin the create stream job.  Enter NOW to submit
    the job immediately.  Enter an absolute or delta time to begin the job
    at some later time.  You are creating a new stream.  It is suggested that
    you enter a submit time after 12:00AM and before 3:00AM.
    Enter BACK or PREV to return to the previous prompt.

$!
$ RETURN
$!
$!+-----------------------------------------------------------------------------
$! CREATE_BUILD_STREAM - obtains the necessary input to create a build
$! stream for a devlopment stream.  The job ....CRESTRM.COM is submitted
$! to perform the VDE and CMS commands necessary to physically create the
$! stream.
$!
$! Modification History:
$!
$!	DJS	13-JAN-1991	    Original Version
$!	SRH     15-Jun-1995	    integrated into VDE
$!	SRH     23-Jul-1997	    added ability to specify whether or not to use
$!				    variant generations for the new stream
$!
$!+-----------------------------------------------------------------------------
