$ sav_verify = 'f$verify(0)
$ set = "set"
$ set symbol/scope=noglobal
$ delete /symbol/local set
$!
$ if f$trnlnm("PLMENU$DEBUG") .nes. "" then set verify
$ set control = Y
$ on control_y then GOTO Control_y
$ set noon
$ user = F$edit("''f$getjpi("","USERNAME")'","Trim,Upcase")
$ source = f$trnlnm("PLMENU$DEFAULT_SOURCE_STREAM")
$ facility = ""
$ mnemonic = ""
$ product = f$trnlnm("PLMENU$DEFAULT_PRODUCT")
$ work_dir = f$trnlnm("PLMENU$DEFAULT_DIRECTORY")
$ batch_queue = f$trnlnm("PLMENU$DEFAULT_BATCH_QUEUE")
$ cur_dir = f$environment("default")
$ option = "OBSOLETE"
$ submit_time = ""
$ default_value = ""
$ type sys$input

Archive/Obsolete A Facility Version V1.3

Enter ? or "HELP" at any prompt to obtain help.
Prompt default values (if any) appear in brackets [].
Enter <Ctrl/Z>, "Quit" or "Exit" to exit immediately.

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
$! Obtain the facility to be obsoleted
$!+-----------------------------------------------------------------------------
$Get_Facility:
$!------------
$ if facility .nes. ""
$ then
$   default_value = facility
$ else
$   default_value = ""
$ endif
$!
$ user_prompt = "Facility To Obsolete?"
$ prompt_symbol = "facility"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Mnemonic
$ endif
$!+
$! Insure that there is a directory library for this facility. check if the
$! facility is already obsolete - issue a warning, you may continue.
$!+
$Validate_Facility:
$!-----------------
$ if f$search("''root':[000000]''facility'.DIR") .eqs. ""
$ then
$   write sys$output "%OBSFAC-E-NOTFOUND, facility ''facility' not found!" 
$   facility = ""
$   GOTO Get_Facility
$ else
$   if f$search("''root':[''facility']VMSCMS$ARCHIVED_FACILITY.FLAG") .NES. ""
$   then
$	write sys$output "%OBSFAC-W-ISOBS, facility ''facility' is already archived!" 
$   endif
$ endif   
$!
$!
$!+-----------------------------------------------------------------------------
$! See if the user wnats to archive or obsolete the facility
$!+-----------------------------------------------------------------------------
$Get_Option:
$!-----------
$ if option .eqs. ""
$ then
$   default_value = "OBSOLETE"
$ else
$   default_value = option
$ endif
$!
$ user_prompt = "Archive or Obsolete ''facility'?"
$ prompt_symbol = "option"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Facility
$ endif
$!
$ option = f$edit(option,"collapse,UPCASE")
$ if F$locate(option,"OBSOLETE") .eq. 0
$ then
$   option = "OBSOLETE"
$ else
$   if F$locate(option,"ARCHIVE") .eq. 0   
$   then
$	option = "ARCHIVE"
$   else
$	write sys$output -
	"%OBSFAC-E-INVALID, ''option' is not a valid option!"
$	option = ""
$	GOTO Get_Option
$   endif
$ endif
$!
$!
$!+-----------------------------------------------------------------------------
$! obtain the source stream where the facility becomes obsolete
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
$ user_prompt = "Stream at which to ''option' the facility?"
$ prompt_symbol = "source"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Option
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
$ user_prompt = "Queue to submit obsolete batch job"
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
$   GOTO Get_Source
$ endif
$!
$! validate the queue
$!
$ if .not. f$getqui("DISPLAY_QUEUE","QUEUE_BATCH","''batch_queue'")
$ then
$   write sys$output -
    "%OBSFAC-F-INVQUEUE, ''batch_queue' is not a valid batch queue"
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
	"%DIFSTRM-F-INVLOG, invalid logical name, logical does not exist."
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
    "%DIFSTRM-F-INVDIR, invalid work directory, directory does not exist."
$   GOTO Get_work_dir
$ else
$   work_dir = work_dir - ".;"
$ endif
$!
$!+
$! obtain the time to submit the populate job
$!+
$Get_default_time:
$!-----------------
$ if submit_time .nes. ""
$ then
$   default_value = submit_time
$ else
$   default_value = "NOW"
$ endif
$!
$Get_Submit_Time:
$ prompt_symbol = "submit_time"
$ user_prompt = "Time to begin obsolete facility job?"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_work_dir
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
$   write sys$output "%OBSFAC-F-INVTIME, ''submit_time' is not a valid time"
$   submit_time = ""
$   goto Get_default_time
$ endif
$!
$! f$cvtime right justifies the time, so put the space in if it was taken out
$!
$ if f$extract(1,1,submit_time) .eqs. "-"
$ then
$   submit_time = " " + submit_time
$ endif
$!
$ submit_ans = ""
$!+
$! have the user validate the input
$!+
$Confirm_Input:
$!-------------
$ jobname = "OBSOLETE_" + facility
$ write sys$output ""
$ write sys$output -
" If you proceed, a job to ''option' facility ''facility' beginning"
$ write sys$output -
" at stream ''source' will be submitted. "
$ write sys$output ""
$ user_prompt = "Do you wish to proceed?"
$ default_value = "Yes"
$ prompt_symbol = "submit_ans"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Default_time
$ endif
$!
$ if .not. submit_ans then GOTO Get_Mnemonic
$!
$ submit/restart/noprint/keep/after="''submit_time'" -
    /log='work_dir'  -
    /name="Obsolete_''facility'" -
    /queue='batch_queue' -
    /param=('mnemonic','facility','source', 'option') -
  vde$system:plmenu$$OBSFAC.COM
$!
$Control_y:
$!
$Main_Exit:
$ EXIT 1+0*'f$verify(sav_verify)
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
$!
$Help_Facility:
$!-------------
$ type sys$input

    Enter the facility to be obsoleted.  Enter BACK or PREV to return to
    VDE library (mnemonic) prompt.

$!
$ RETURN
$!
$Help_Mnemonic:
$!------------
$ type sys$input 

   Enter the VDE Library mnemonic containing the facility to obsolete.
   Examples of common VDE libraries are VMS and VMSTEST. 

$!
$ RETURN
$!
$Help_source:
$!-----------
$ type sys$input

     Enter the stream at which the facility becomes obsolete.  For example,
     if the facility was active in version V5.4-3 and the facility is obsolete
     in version V5.5, enter "V5.5".  Enter BACK or PREV to return to the
     option prompt.
	
$!
$ RETURN
$!
$Help_Batch_queue:
$!
$ type sys$input

     Enter the batch queue in which to submit the facility obsolete batch
     job.  You can define the logical PLMENU$DEFAULT_BATCH_QUEUE to supply
     a default value to this prompt.  Enter BACK or PREV to return to the
     stream prompt.

$!
$ RETURN
$!
$Help_Option:
$!-----------
$ type sys$input

     Indicate if the facility is to be archived or obsoleted.  Archiving a
     facility effects all open development streams, except remedial streams.
     You should not archive a facility until all mainline streams for which the
     facility is active are closed.  Obsoleting a facility removes the facility
     module generations from the facility for the stream entered at the next
     prompt.  Enter ARCHIVE or OBSOLETE to this prompt.  Enter BACK or PREV
     to return to the facility prompt.

$!
$ RETURN
$!
$Help_Work_dir:
$!-------------
$ type sys$input

     Enter a directory to be used for the log files and work files
     used by this procedure.  The work directory defaults to the
     current directory.  Please use brackets, "[,]" to enclose the
     directory name.  If the input does not include brackets, the
     input is assumed to be a logical name.  Define the logical name
     PLMENU$DEFAULT_DIRECTORY to automatically supply a default value
     to this prompt. Enter BACK or PREV to return to the queue prompt.

$!
$ RETURN
$!
$Help_Submit_Ans:
$!---------------
$ type sys$input

     Enter Yes/No or True/False to this question.  
     Enter BACK or PREV to return to the previous prompt.

$!
$ RETURN
$!
$Help_Submit_Time:
$!----------------
$ type sys$input

     Enter the time at which the facility obsolete job should begin.
     Enter BACK or PREV to return to the work directory prompt.
     
$!
$ RETURN
$!
$!------------------------------------------------------------------------------
$!
$! OBSOLETE_A_FACILITY.COM
$!
$! This command procedure marks a facility as being obsolete.
$!
$! Author:  David Sweeney  Circa March 1991
$!
$!
$! MODIFICATIONS:
$!      Steve Hoffman           9-May-1995
$!              Fixed latent parenthetical bug
$!
$!------------------------------------------------------------------------------
