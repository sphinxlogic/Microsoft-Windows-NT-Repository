$ sav_verify = 'f$verify(0)
$!+
$! documentation at end of command file
$!+
$ set = "set"
$ set symbol/scope=noglobal
$ delete /symbol/local set
$!
$ if f$trnlnm("PLMENU$DEBUG") .nes. "" then set verify
$ set control = Y
$ on control_y then GOTO Control_y
$ set noon
$!
$ source = f$trnlnm("PLMENU$Default_Source_Stream")
$ target = f$trnlnm("PLMENU$Default_Target_Stream")
$ user = F$edit("''f$getjpi("","USERNAME")'","Trim,Upcase")
$ mnemonic = f$trnlnm("PLMENU$DEFAULT_PRODUCT")
$ remark = ""
$ work_dir = f$trnlnm("PLMENU$DEFAULT_DIRECTORY")
$ cur_dir = f$environment("default")
$ batch_queue = f$trnlnm("PLMENU$Default_Batch_Queue")
$ if batch_queue .nes. ""
$ then
$   if .not. f$getqui("DISPLAY_QUEUE","QUEUE_BATCH","''batch_queue'")
$   then
$	batch_queue = "SYS$LOBUILD"
$   endif
$ else
$   batch_queue = "SYS$LOBUILD"
$ endif
$!
$ default_value = ""
$ mnemonic = ""
$ root = ""
$ type sys$input

Rename Stream Version V1.3

Enter ? or "Help" at any prompt to obtain help.
Prompt default values (if any) appear in brackets [].
Enter <Ctrl/Z>, "Quit" or "Exit" to exit immediately.

$!
$! See if the vde$fetch_method logical is defined to VDE.
$! If the logical is defined to VDE, rename the
$! stream in the VDE database only.  If the logical is undefined,
$! the stream will be renamed in both VDE and CMS.
$!
$ stream_fetch_method = f$trnlnm("vde$fetch_method")
$ stream_fetch_method = F$edit(stream_fetch_method,"Trim,UPCASE")
$!
$ if stream_fetch_method .EQS. "VDE"
$ then
$   write sys$output -
   "%RENSTRM-I-RENVDE, Stream will be renamed in VDE database only, NO CMS CLASSES"
$   write sys$output " "
$   GOTO Skip_CMS_Write_Check
$ endif
$!
$! Check if this account has write access to the Masterpack CMS libraries
$!
$ @vde$system:plmenu$$validate_cms_access
$ if .not. $status
$ then
$   write sys$output -
 "%RENSTRM-F-NOCMSWRITE, This account does not have write access to MASD CMS libraries"
$   GOTO Main_Exit
$ endif
$Skip_CMS_Write_Check:
$!
$!+
$!   USER PROMPTS
$!+
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
$! obtain the stream to rename
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
$ user_prompt = "Stream to rename"
$ prompt_symbol = "source"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Mnemonic
$ endif
$!
$!+-----------------------------------------------------------------------------
$! Obtain the new stream name
$!+-----------------------------------------------------------------------------
$Get_target:
$!----------
$ if target .eqs. "" 
$ then
$   default_value = f$trnlnm("PLMENU$DEFAULT_TARGET_STREAM")
$ else
$   default_value = "''target'"
$ endif
$ user_prompt = "New Stream Name"
$ prompt_symbol = "target"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_source
$ else
$   default_value = ""
$ endif
$!
$!+-----------------------------------------------------------------------------
$!  get a remark to be associated with the new stream
$!+-----------------------------------------------------------------------------
$Get_Remark:
$!----------
$ if remark .nes. ""
$ then
$   default_value = "''remark'"
$ else
$   default_value = "Use current remark"
$ endif
$!
$ user_prompt = "New remark string for stream ''target'"
$ prompt_symbol = "remark"
$ GOSUB Get_Text_Value
$ remark = f$edit(remark,"COMPRESS,TRIM")
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
$ user_prompt = "Directory for log file"
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
$   GOTO Get_Remark
$ else
$   default_value = ""
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
	"%RENSTRM-F-INVLOG, invalid logical name, logical does not exist."
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
    "%RENSTRM-F-INVDIR, invalid work directory, directory does not exist."
$   GOTO Get_work_dir
$ else
$   work_dir = work_dir - ".;"
$ endif
$!
$!+
$! obtain the batch queue to which they which to submit the rename job
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
$ user_prompt = "Queue to submit rename job"
$ prompt_symbol = "batch_queue"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Work_Dir
$ endif
$!
$! validate the queue
$!
$ if .not. f$getqui("DISPLAY_QUEUE","QUEUE_BATCH","''batch_queue'")
$ then
$   write sys$output -
    "%RENSTRM-F-INVQUEUE, ''batch_queue' is not a valid batch queue"
$   batch_queue = ""
$   GOTO Get_Queue
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
$ write sys$output -
" If you proceed you will rename stream ''source' to ''target'."
$ user_prompt = "Do you wish to proceed?"
$ default_value = "Yes"
$ prompt_symbol = "submit_ans"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Work_Dir
$ endif
$!
$ if .not. submit_ans then GOTO Get_Mnemonic
$!
$ valid_input = 1
$ return_to_label = ""
$!
$ GOSUB Validate_Source
$ if .not. valid_input
$ then
$   return_to_label = "Get_Source"
$ endif
$!
$ GOSUB Validate_target
$ if .not. valid_input
$ then
$   if return_to_label .eqs. "" then return_to_label = "Get_Target"
$ endif
$!
$ if return_to_label .nes. "" then GOTO 'return_to_label'
$!
$!
$!+-----------------------------------------------------------------------------
$! submit the job to perform the modify stream/class commands
$!+-----------------------------------------------------------------------------
$SUBMIT_RENAME_JOB:
$!------------------
$ if F$edit(remark,"UPCASE,COMPRESS") .eqs. "USE CURRENT REMARK" then remark = ""
$ jobname = "RENAME_" + "''source'" - "." - "."
$ submit/noprint/keep/log='work_dir'/queue='batch_queue' -
        /param=('source','target',"''remark'",'mnemonic') -
        /notify/name='jobname' VDE$SYSTEM:PLMENU$$RENSTRM
$!
$Control_y:
$!
$MAIN_EXIT:
$ EXIT 1+0*f$verify(sav_verify)
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
$   write sys$output "%RENSTRM-E-VALERR, validation error for ''source' stream in VDE"
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
$ if $status .eq. %x10000001
$ then
$   write sys$output "%RENSTRM-E-ALREXISTS, stream ''target' already exists"
$   target = ""
$   valid_input = 0
$ endif
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
$!+
$!  Help text for prompts
$!+
$Help_Batch_Queue:
$!
$ type sys$input 

    Enter a batch queue to which the rename job should be submitted.
    Define the logical PLMENU$DEFAULT_BATCH_QUEUE to automatically supply
    a default value to this prompt.  Enter BACK or PREV to return to the
    previous prompt.     

$!
$ RETURN
$!
$Help_Mnemonic:
$!
$ type sys$input 

    Enter the VDE library mnemonic for the library containing the stream
    to rename.  Examples of mnemonics are VMS and EVMSTEST.  Define the
    logical PLMENU$DEFAULT_PRODUCT to automatically supply a default value
    to this prompt -- if neither the library nor the PLMENU$DEFAULT_PRODUCT
    are specified, the first translation from VDE$LIBRARY_DEFAULTS_MNEMONICS
    is used.

$!
$ RETURN
$!
$Help_Remark:
$!
$ type sys$input 

    Enter a remark to be associated with the new stream name.  Leave this
    prompt blank to retain the remark currently on the stream.  Enter BACK or
    PREV to return to the previous prompt.

$!
$ RETURN
$!
$Help_source:
$!-----------
$!
$ type sys$input

    Enter the stream name to rename.  For example, to rename V5.4_build to 
    V5.4, you would enter V5.4_build.  Define the logical
    PLMENU$DEFAULT_SOURCE_STREAM to automatically supply a default value to
    this prompt.  Enter BACK or PREV to return to the previous prompt.

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
$Help_target:
$!-----------
$!
$ type sys$input 

    Enter the new stream name.  For example, to rename V5.4_Build to V5.4
    you would enter V5.4.  Define the logical PLMENU$DEFAULT_TARGET_STREAM
    to automatically supply a default value to this prompt.  Enter BACK or
    PREV to return to the previous prompt. 

$!
$ RETURN
$!
$Help_Work_dir:
$!
$ type sys$input

    Enter a directory for the log file of the rename stream job.
    The directory defaults to the current directory.  Please use
    brackets, "[,]" to enclose the directory name.  If the input does
    not include brackets, the input is assumed to be a logical name.
    Define the logical PLMENU$DEFAULT_DIRECTORY to automatically supply a
    default value to this prompt.  Enter BACK or PREV to return to the
    previous prompt.

$!
$ RETURN
$!
$!+-----------------------------------------------------------------------------
$!
$!  RENAME_STREAM.COM
$!      Prompts the user to obtain the information necessary to submit a job to
$!  rename a VSC stream and it's associated CMS class in all facilities.
$!
$!  Submits command file VDE$SYSTEM:PLMENU$$RENSTRM.COM to perform the VDE
$!  and CMS commands that rename the stream/classes.  
$!
$!  modification history:
$!
$!	DJS	X-1	Original version
$!	SRH 	15-Jun-1995	Integrated into VDE
$!
$!+-----------------------------------------------------------------------------
