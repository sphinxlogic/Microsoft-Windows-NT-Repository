$ sav_verify = 'f$verify(0)
$ set = "set"
$ set symbol/scope=noglobal
$ delete /symbol/local set
$!
$ if f$trnlnm("PLMENU$DEBUG") .nes. "" then set verify
$ set control = Y
$ on control_y then GOTO Control_y
$ say = "write sys$output"
$ cur_dir = f$environment("default")
$ set noon
$!+
$! logical list - keep in order of entries on the menu list
$!+
$ logical_list = -
    "BATCH_QUEUE,DIRECTORY,MNEMONIC,SOURCE,TARGET"
$!
$ batch_queue = f$trnlnm("PLMENU$DEFAULT_BATCH_QUEUE")
$ work_dir = f$trnlnm("PLMENU$DEFAULT_DIRECTORY")
$ mnemonic = f$trnlnm("PLMENU$DEFAULT_PRODUCT")
$ source = f$trnlnm("PLMENU$DEFAULT_SOURCE_STREAM")
$ target = f$trnlnm("PLMENU$DEFAULT_TARGET_STREAM")
$!
$!
$ action = ""
$!
$ type sys$input

OpenVMS Project Leader Menu Set/Show Logicals Version V1.3-0

Enter ? or "HELP" at any prompt to obtain help.
Enter <Ctrl/Z>, "Quit" or "Exit" to exit immediately.

$!
$Display_Menu:
$!------------
$!
$ type sys$input

  Enter a number from the list below corresponding to the default value
  you wish to set :

$ say "  1. Define Batch Queue (PLMENU$DEFAULT_BATCH_QUEUE):       ''batch_queue'"
$ say "  2. Define Directory (PLMENU$DEFAULT_DIRECTORY):           ''work_dir'"
$ say "  3. Define VDE Library Mnemonic (PLMENU$DEFAULT_PRODUCT):  ''mnemonic'"
$ say "  4. Define Source Stream (PLMENU$DEFAULT_SOURCE_STREAM):   ''source'"
$ say "  5. Define Target Stream (PLMENU$DEFAULT_TARGET_STREAM):   ''target'"
$ say ""
$!
$ read/End_of_File=Main_Exit/Error=time_expired/timeout=255 -
  /prompt="What logical do you want to define? (1-5,EXIT,HELP): " -
  SYS$COMMAND action
$!
$ action = f$edit(action,"COMPRESS,UPCASE")
$ if action .eqs. "" then GOTO Display_Menu
$ if (F$locate(action,"EXIT") .eq. 0 ) .or. (F$locate(action,"QUIT") .eq. 0) then GOTO Main_Exit
$ if action .eqs. "?" .or. (F$locate(action,"HELP") .eq. 0) then GOTO Display_Menu
$!
$ if .not. (action .ges. "1" .and. action .les. "5") then GOTO Display_Menu
$! 
$ index = F$integer(action) - 1
$ symbol_name = F$element(index,",",logical_list)
$!
$ GOSUB Define_'symbol_name'
$!
$ goto display_menu
$!
$Time_expired:
$!------------
$ write sys$output "Timeout on input, exiting..."
$!
$Control_y:
$!---------
$!
$Main_Exit:
$!---------
$ EXIT 1+0*'f$verify(sav_verify)
$!
$!+-----------------------------------------------------------------------------
$! Get values to assign to logical name 
$!+-----------------------------------------------------------------------------
$!
$Define_Batch_QUEUE:
$!---------------
$ if batch_queue .nes. ""
$ then
$   default_value = batch_queue
$ else
$   default_value = F$trnlnm("PLMENU$DEFAULT_BATCH_QUEUE")
$   if default_value .eqs. "" then default_value = "SYS$LOBUILD"
$ endif
$!
$ prompt_symbol = "batch_queue"
$ user_prompt = "Default queue for batch jobs"
$ GOSUB Get_Text_Value
$ batch_queue = f$edit(batch_queue,"UPCASE")
$!
$! validate the queue
$!
$ if .not. f$getqui("DISPLAY_QUEUE","QUEUE_BATCH","''batch_queue'")
$ then
$   write sys$output -
    "%PLMENU-F-INVQUEUE, ''batch_queue' is not a valid batch queue"
$   batch_queue = ""
$   GOTO DEFINE_Batch_Queue
$ endif
$!
$ if F$trnlnm("PLMENU$DEFAULT_BATCH_QUEUE") .nes. batch_queue
$ then
$   define/process/nolog PLMENU$DEFAULT_BATCH_QUEUE 'batch_queue'
$ endif
$!
$ RETURN
$!
$!+-----------------------------------------------------------------------------
$Define_Directory:
$!----------------
$ if work_dir .nes. ""
$ then
$   default_value = work_dir
$ else
$   default_value = F$trnlmn("PLMENU$DEFAULT_DIRECTORY")
$   if default_value .eqs. "" then default_value = cur_dir
$ endif
$!
$ prompt_symbol = "work_dir"
$ user_prompt = "Directory for log and work files"
$ GOSUB Get_Text_Value
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
	"%PLMENU-F-INVLOG, invalid logical name, logical does not exist."
$	GOTO Define_Directory
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
    "%PLMENU-F-INVDIR, invalid work directory, directory does not exist."
$   GOTO Define_directory
$ else
$   work_dir = work_dir - ".;"
$ endif
$!
$ if F$trnlnm("PLMENU$DEFAULT_DIRECTORY") .nes. work_dir
$ then
$  define/process/nolog PLMENU$DEFAULT_DIRECTORY 'work_dir'
$ endif
$!
$ RETURN
$!
$!+----------------------------------------------------------------------------
$! obtain the VDE library mnemonic (eg EVMS, VMSTEST, etc.), and store it in
$! the symbol MNEMONIC.  From the mnemonic, derive the CMS root and store it
$! in ROOT. 
$
$! By default we use the specified mnemonic value, the translation of the
$! PLMENU$DEFAULT_PRODUCT logical name, or the first translation of the
$! system-wide library list.
$!+----------------------------------------------------------------------------
$Define_mnemonic:
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

$   GOTO Define_Mnemonic
$ endif
$!
$Validate_mnemonic:
$!
$ @vde$system:plmenu$$set_vde_library 'mnemonic'
$ if .not. $status
$ then
$   root = "???"
$   mnemonic = ""
$   GOTO Define_Mnemonic
$ endif
$ root = f$trnlnm("PLMENU$CMSROOT")
$ mnemonic = f$trnlnm("PLMENU$MNEMONIC")
$!
$!
$ if F$trnlnm("PLMENU$DEFAULT_PRODUCT") .nes. mnemonic
$ then
$  define/process/nolog PLMENU$DEFAULT_PRODUCT 'mnemonic'
$ endif
$!
$ RETURN
$!
$!+-----------------------------------------------------------------------------
$! obtain the development stream for which the build class will be used
$! store the value in symbol SOURCE
$!+-----------------------------------------------------------------------------
$Define_source:
$!-------------
$ if source .nes. ""
$ then
$   default_value = source
$ else
$   default_value = f$trnlnm("PLMENU$DEFAULT_SOURCE_STREAM")
$ endif
$!
$ user_prompt = "Default source/development stream"
$ prompt_symbol = "source"
$ GOSUB Get_Text_Value
$ source = f$edit(source,"UPCASE")
$!
$! valid_input= 1
$! GOSUB Validate_Source
$! if .not. valid_input then GOTO Define_Source
$!
$ if F$trnlnm("PLMENU$DEFAULT_SOURCE_STREAM") .nes. source
$ then
$  define/process/nolog PLMENU$DEFAULT_SOURCE_STREAM 'source'
$ endif
$!
$ RETURN
$!
$!+-----------------------------------------------------------------------------
$Define_Target:
$!-------------
$ if target .eqs. "" 
$ then
$   default_value = f$trnlnm("PLMENU$DEFAULT_TARGET_STREAM")
$ else
$   default_value = "''target'"
$ endif
$ user_prompt = "Target Stream"
$ prompt_symbol = "target"
$ GOSUB Get_Text_Value
$ target = f$edit(target,"UPCASE")
$!
$ if F$trnlnm("PLMENU$DEFAULT_TARGET_STREAM") .nes. target
$ then
$  define/process/nolog PLMENU$DEFAULT_TARGET_STREAM 'target'
$ endif
$!
$ RETURN
$! 
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
$ if temp_value .eqs "PREV" .OR. temp_value .eqs. "BACK" 
$ then
$!   'prompt_symbol' = ""
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

    Most of the functions of the project leader menu are performed by batch
    jobs.  The functions prompt you for the batch queue to which batch job(s)
    should be submitted.  Enter a valid batch queue to use as the default
    value for all batch queue prompts.
        
$!
$ RETURN
$+
$Help_Work_Dir:
$!
$ type sys$input 

    Enter a directory specification to use as the default directory for log 
    files and work files created by the project leader menu functions.  This
    directory will be used as the default value to for all work directory
    prompts.

$!
$ RETURN
$+
$Help_Mnemonic:
$!
$ type sys$input 

    Enter the VDE library mnemonic for the VDE database (and the
    associated CMS libraries) for which you want to work.  For
    example, to create and populate streams for the OpenVMS Alpha
    or OpenVMS Alpha products, specify EVMS or VMS respectively.

$!
$ RETURN
$+
$Help_Source:
$!
$ type sys$input 

    Enter the stream to be used as the default development stream.  This
    stream is used as the default stream from which to create a new stream
    or from which to populate another stream.

$!
$ RETURN
$+
$Help_Target:
$!
$ type sys$input 

    Enter the stream to be used as the default target stream.  This
    stream is used as the default stream to create or to populate from
    the source stream.

$!
$ RETURN
