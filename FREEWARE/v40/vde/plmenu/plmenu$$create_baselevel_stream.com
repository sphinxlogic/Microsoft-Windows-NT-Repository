$ sav_verify = 'f$verify(0)
$!+
$! documentation at end of command file
$!+
$ set = "set"
$ set symbol/scope=noglobal
$ delete /symbol/local set
$ say = "write sys$output"
$!
$ if f$trnlnm("PLMENU$DEBUG") .nes. "" then set verify
$ set control = Y
$ on control_y then GOTO Control_y
$ set noon
$!
$ suffix = ""
$ baselevel = ""
$ field_test = ""
$ source = ""
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
$ chars36 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
$ type sys$input

Create Baselevel Stream Version V1.3

Enter ? or "Help" at any prompt to obtain help.
Prompt default values (if any) appear in brackets [].
Enter <Ctrl/Z>, "Quit" or "Exit" to exit immediately.

$!
$! Check if this account has write access to the Masterpack CMS libraries
$!
$ @vde$system:plmenu$$validate_cms_access
$ if .not. $status
$ then
$   write sys$output -
 "%CREBSLVL-F-NOCMSWRITE, This account does not have write access to MASD CMS libraries"
$   GOTO Main_Exit
$ endif
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
$
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
$! obtain the build stream to rename to the baselevel
$!+-----------------------------------------------------------------------------
$Get_source:
$!----------
$ if source .nes. ""
$ then
$   default_value = source
$ else
$   default_value = ""
$ endif
$!
$ user_prompt = "Build stream to rename"
$ prompt_symbol = "source"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Mnemonic 
$ endif
$!
$! locate the development stream name
$!
$ source = f$edit(source,"UPCASE")
$ if f$locate("_BLD",source) .lt. F$length(source)
$ then
$   dev_stream = f$extract(0,f$locate("_BLD",source),source)
$ else
$   say "%CREBSLVL-E-NODEVSTRM, unable to determine development stream name"
$   say "                       enter another name or use the Rename Stream function." 
$   GOTO Get_source
$ endif
$!
$!+-----------------------------------------------------------------------------
$! obtain the the baselevel number RESD$ number
$!+-----------------------------------------------------------------------------
$Get_Baselevel:
$!-------------
$ if baselevel .nes. ""
$ then
$   default_value = baselevel
$ else
$   default_value = ""
$ endif
$!
$ user_prompt = "Result disk baselevel number"
$ prompt_symbol = "baselevel"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Source
$ endif
$!
$ baselevel = f$edit(baselevel,"UPCASE")
$!
$! validate the baselevel syntax
$! 
$ if f$extract(0,1,baselevel) .nes. "X"
$ then
$   say "%CREBSLVL-E-Invalid, Invalid! baselevel must begin with an X"
$   baselevel = ""
$   GOTO Get_Baselevel
$ endif
$!
$ base36_length = f$length(baselevel) - 1
$ temp_value = f$extract(1,base36_length,baselevel)
$ index = base36_length
$base36_loop:
$ num = f$extract(index,1,temp_value)
$ if f$locate(num,chars36) .eq. f$length(chars36)
$ then
$   say "%CREBSLVL-E-Invalid, Invalid! ''num' is not a valid base 36 number"
$   baselevel = ""
$   GOTO Get_Baselevel
$ endif  
$ index = index - 1
$ if index .gt. 0 then GOTO base36_loop
$!
$!+-----------------------------------------------------------------------------
$! ask if this is an EFT or IFT baselevel 
$!+-----------------------------------------------------------------------------
$Get_Field_Test:
$!--------------
$ if field_test .nes. ""
$ then
$   default_value = field_test
$ else
$   default_value = "OTHER"
$ endif
$  
$ user_prompt = "Baselevel type (EFT, IFT or OTHER)"
$ prompt_symbol = "field_test"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Baselevel
$ endif
$!
$ field_test = f$edit(field_test,"UPCASE")
$ if F$locate(field_test,"EFT") .eq. 0 then field_test = "_EFT"
$ if F$locate(field_test,"IFT") .eq. 0 then field_test = "_IFT" 
$ if F$locate(field_test,"OTHER") .eq. 0 then field_test = ""
$! 
$ target = dev_stream + "_" + baselevel + field_test
$!
$!
$!+-----------------------------------------------------------------------------
$! ask for any additional string to be added to the stream name
$!+-----------------------------------------------------------------------------
$Get_Suffix:
$!----------
$ if suffix .nes. ""
$ then
$   default_value = suffix
$ else
$   default_value = "NONE"
$ endif
$  
$ user_prompt = "Suffix to append to stream ''target'"
$ prompt_symbol = "suffix"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Field_Test
$ endif
$!
$ suffix = f$edit(suffix,"UPCASE")
$ if F$locate(suffix,"NONE") .eq. 0 then suffix = ""
$ target = target + suffix
$!
$!+-----------------------------------------------------------------------------
$!  get a remark to be associated with the new stream
$!+-----------------------------------------------------------------------------
$Get_Remark:
$!----------
$ if remark .eqs. "" 
$ then
$   if field_test .eqs. "_EFT"
$   then
$     default_value = "''dev_stream' EFT baselevel ''baselevel'"
$   else
$     if field_test .eqs. "_IFT"
$     then
$       default_value = "''dev_stream' IFT baselevel ''baselevel'"
$     else
$	default_value = "''dev_stream' baselevel ''baselevel'"
$     endif
$   endif
$ else
$   default_value = "''remark'"
$ endif
$!
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
$   GOTO Get_suffix
$ endif
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
$	say -
	"%CREBSLVL-F-INVLOG, invalid logical name, logical does not exist."
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
$   say -
    "%CREBSLVL-F-INVDIR, invalid work directory, directory does not exist."
$   GOTO Get_work_dir
$ else
$   work_dir = work_dir - ".;"
$ endif
$!
$!+-----------------------------------------------------------------------------
$! obtain the batch queue to which they which to submit the rename job
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
$   say -
    "%CREBSLVL-F-INVQUEUE, ''batch_queue' is not a valid batch queue"
$   batch_queue = ""
$   GOTO Get_Queue
$ endif
$!
$!
$!+-----------------------------------------------------------------------------
$! Validate the inputs which are not already validated... i.e. those which take
$! more than a few seconds to validate.  First confirm with the user the source
$! and target streams.
$!+-----------------------------------------------------------------------------
$Validate_input:
$!--------------
$ say ""
$ say " If you proceed you will rename stream ''source' to ''target'."
$ say " Stream ''target' will be frozen and cannot be populated "
$ say " from the populate stream option of the menu." 
$ say ""
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
$   if return_to_label .eqs. "" then return_to_label = "Get_Baselevel"
$ endif
$!
$ if return_to_label .nes. "" then GOTO 'return_to_label'
$!
$Create_Baselevel_stream:
$!-----------------------
$!
$! set the stream nopopulate attribute
$!
$ VDE set priv modstrm; modify stream/FROZEN/NOPOP 'source'
$ if .not. $status
$ then
$   say "%CREBSLVL-E-NOPOP, VDE modify stream/nopop failed."
$   GOTO Main_Exit
$ endif
$!
$! rename the stream
$! 
$ jobname = "RENAME_" + "''source'" - "." - "."
$ submit/noprint/keep/log='work_dir'/queue='batch_queue' -
        /param=('source','target',"''remark'",'mnemonic') -
        /notify/name='jobname' VDE$SYSTEM:PLMENU$$RENSTRM
$!
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
$   write sys$output "%CREBSLVL-E-VALERR, validation error for ''source' stream in VDE"
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
$   write sys$output "%%CREBSLVL-E-ALREXISTS, stream ''target' already exists"
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
$! LABELS:
$!	    MAIN_EXIT and HELP_'prompt_symbol' must exists
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
$ say "Timeout on input, exiting..."
$ GOTO Main_exit
$!
$!
$Help_Baselevel:
$!
$ type sys$input 

    Enter the system baselevel number of the result disk for the stream
    you want to freeze.  Enter BACK or PREV to return to the build stream
    prompt.

$!
$ RETURN
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
$Help_Field_Test:
$!
$ type sys$input 

    If the baselevel is an internal field test version please enter IFT.
    If the baselevel is an external field test version please enter EFT.
    Please enter "OTHER" for non-IFT and non-EFT baselevels.  Enter BACK
    or PREV to return to the baselevel number prompt.

$!
$ RETURN
$!
$Help_Mnemonic:
$!
$ type sys$input 

    Enter the VDE library mnemonic for the library containing the
    stream to rename.  Examples of common library mnemonics are "EVMS"
    and "VMSTEST".  Define the logical name PLMENU$DEFAULT_PRODUCT to
    override the default value (the default is the first translation
    from the VDE$LIBRARY_DEFAULTS_MNEMONICS logical) for this prompt. 

$!
$ RETURN
$!
$Help_Remark:
$!
$ type sys$input 

    Enter a remark to be associated with the baselevel stream. 
    Enter BACK or PREV to return to the previous prompt.

$!
$ RETURN
$!
$Help_Source:
$!
$ type sys$input 

    Enter the build stream to rename and freeze to the baselevel stream.
    For example, to create baselevel MAGIC_X999 from MAGIC_BLD_A enter
    MAGIC_BLD_A.  Enter BACK or PREV to return to the mnemonic prompt.

$!
$ RETURN
$!
$Help_Submit_Ans:
$!
$ type sys$input 

    Enter affirmatively to initiate creating the baselevel stream from
    the build stream.  Enter negatively to return to the mnemonic prompt.
    Enter BACK or PREV to return to the work directory prompt.

$!
$ RETURN
$!
$Help_Suffix:
$!
$ type sys$input 

    Enter a suffix for the baselevel stream name if you want to provide 
    more information about the baselevel in the name.  For example, if
    the baselevel integrates support for a new CPU you might append
    the CPU name to the baselevel.  You may wish to enter an underscore
    "_" to separate the suffix in the name.  Enter BACK or PREV to return
    to the baselevel type prompt.

$!
$ RETURN
$!
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
$! CREATE_BASELEVEL_STREAM.COM - Renames a build stream to a baselevel stream.
$! The baselevel stream name will always begin with the string:
$!
$!	'development_stream_name'_'baselevel'
$!
$!
$!
$!+-----------------------------------------------------------------------------

