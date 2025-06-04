$ sav_verify = 'f$verify(0)
$ set = "set"
$ set symbol/scope=noglobal
$ delete /symbol/local set
$!
$ if f$trnlnm("PLMENU$DEBUG") .nes. "" then set verify
$ set control = Y
$ on control_y then GOTO Control_y
$ set noon
$!+
$! check if populates are turned off on this node
$!+
$ if f$trnlnm("FAC$ABORT_POPLIBS") .nes. ""
$ then
$    write sys$output "%POP-E-LOGNMABRT, FAC$ABORT_POPLIBS logical found"
$    GOTO MAIN_EXIT
$ endif
$!
$ user = F$edit("''f$getjpi("","USERNAME")'","Trim,Upcase")
$ target = f$trnlnm("PLMENU$DEFAULT_TARGET_STREAM")
$ source = f$trnlnm("PLMENU$DEFAULT_SOURCE_STREAM")
$ fac_input = ""
$ fac_list = ""
$ facility_file = ""
$ poption = "BOTH"
$ moption = ",NOMERGE"
$ mnemonic = f$trnlnm("PLMENU$DEFAULT_PRODUCT")
$ work_dir = f$trnlnm("PLMENU$DEFAULT_DIRECTORY")
$ batch_queue = f$trnlnm("PLMENU$DEFAULT_BATCH_QUEUE")
$ cur_dir = f$environment("default")
$ remark = ""
$ submit_time = ""
$ default_value = ""
$ type sys$input

Populate Facilities Version V1.3

Enter ? or "HELP" at any prompt to obtain help.
Prompt default values (if any) appear in brackets [].
Enter <Ctrl/Z>, "Quit" or "Exit" to exit immediately.

$!
$! See if the VDE$FETCH_METHOD logical is defined to VDE.
$! If the logical is defined to VDE, it does not make sense to
$! use this procedure.  Tell the user to use the populate stream
$! option.  This procedure is useful to control CMS class
$! populations.  VDE only population should be done from the
$! POPULATE_STREAM.COM procedure.
$!
$ stream_fetch_method = f$trnlnm("VDE$FETCH_METHOD")
$ stream_fetch_method = F$edit(stream_fetch_method,"Trim,UPCASE")
$!
$ if stream_fetch_method .EQS. "VDE"
$ then
$   write sys$output -
   "%POPFACS-I-USEPOPSTREAM, Use Populate Stream Option of the Menu, Exiting..."
$   write sys$output " "
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
$! obtain the source stream for the population
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
$ user_prompt = "Source Stream/Class"
$ prompt_symbol = "source"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Mnemonic
$ endif
$!
$!+-----------------------------------------------------------------------------
$! Obtain the target stream to populate
$!+-----------------------------------------------------------------------------
$Get_target:
$!----------
$ if target .eqs. "" 
$ then
$   default_value = f$trnlnm("PLMENU$DEFAULT_TARGET_STREAM")
$ else
$   default_value = "''target'"
$ endif
$ user_prompt = "Target Stream/Class"
$ prompt_symbol = "target"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_source
$ endif
$!
$!
$!+-----------------------------------------------------------------------------
$! see if the user wants to interactively supply a list of facilities or supply 
$! the name of a file containing a list of facilities to process
$!+-----------------------------------------------------------------------------
$Get_Fac_Input:
$!------------
$ if fac_input .nes. ""
$ then
$   default_value = fac_input
$ else
$   default_value = "Y"
$ endif
$!
$ user_prompt = "Do you want to enter the facilities to process?"
$ prompt_symbol = "fac_input"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Target
$ endif
$!
$ if .not. fac_input  then GOTO Get_Facility_List
$!
$! Get the list of facilities to process
$!
$Get_Fac_list:
$!------------
$ if fac_list .nes. ""
$ then
$   default_value = fac_list
$ else
$   default_value = ""
$ endif
$ user_prompt = F$FAO("Enter a list of facilities delimited by commas(,)(256 character maximum) Hit <RET> to finish !/")
$ prompt_symbol = "fac_list"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Fac_Input
$ endif
$!
$ fac_list = f$edit(fac_list,"COLLAPSE,UPCASE")
$ facility = f$element(0,",",fac_list)
$ if facility .eqs. ","
$ then
$   write sys$output -
    "%POPFACS-E-NOFAC, No facility was entered, please try again."
$   fac_list = ""
$   GOTO Get_fac_list
$ endif
$!
$! there is at least one facility to process
$!
$ Goto Get_OverWrite
$!
$!+-----------------------------------------------------------------------------
$! Obtain the location of a file containing a list of facilities to process
$!+-----------------------------------------------------------------------------
$Get_Facility_List:
$!-----------------
$ if facility_file .nes. ""
$ then
$   default_value = facility_file
$ else
$   default_value = ""
$ endif
$!
$ user_prompt = "Facility List File"
$ prompt_symbol = "facility_file"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Fac_Input
$ endif
$!
$! Validate the facility file specification and existence
$ full_facility_spec = f$parse(facility_file)
$ if full_facility_spec .eqs. ""
$ then
$   write sys$output "%POPFACS-E-INVALID, ''facility_file' not a valid file!"
$   facility_file = ""
$   GOTO Get_Facility_List
$ endif
$!
$ if f$search("''full_facility_spec'") .eqs. ""
$ then
$   write sys$output "%POPFACS-E-NOTFOUND, ''facility_file' not found!"
$   facility_file = ""
$   GOTO Get_Facility_List
$ endif
$
$!
$!
$!+-----------------------------------------------------------------------------
$! Determine if we should overwrite the contents of the target stream
$!+-----------------------------------------------------------------------------
$Get_OverWrite:
$!----------
$ default_value = "YES"
$ user_prompt = "Overwrite contents of target stream"
$ prompt_symbol = "overwrite"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Facility_List
$ endif
$!
$ if .not. overwrite then moption = ",MERGE"
$
$!
$!+-----------------------------------------------------------------------------
$! See if the user wants to populate VDE, CMS or BOTH
$!+-----------------------------------------------------------------------------
$Get_Populate_Scope:
$ if poption .nes. ""
$ then
$   default_value = poption
$ else
$   default_value = "BOTH"
$ endif
$!
$ user_prompt = "Populate CMS only, VDE only, or BOTH"
$ prompt_symbol = "poption"
$ GOSUB Get_Text_Value
$ if return_to_previous_prompt
$ then
$   if fac_input
$   then
$	GOTO Get_Fac_list
$   else
$	GOTO Get_OverWrite
$   endif
$ endif
$ poption = f$edit(poption,"Collapse,UPCASE")
$!
$! Validate the entry
$!
$ if F$locate(poption,"BOTH") .eq. 0
$ then
$   poption = "BOTH"
$ else
$   if F$locate(poption,"CMS") .eq. 0
$   then
$	poption = "CMS"
$   else
$     if F$locate(poption,"VDE") .eq. 0
$     then
$	poption = "VDE"
$     else
$	poption = ""
$	write sys$output "%POPFACS-E-INVALID, ''poption' is not a valid entry"
$	GOTO Get_Populate_Scope
$     endif
$   endif
$ endif
$!
$ if poption .eqs. "VDE" then GOTO Get_Queue
$!
$! Check if this account has write access to the Masterpack CMS libraries
$!
$ @vde$system:plmenu$$validate_cms_access 
$ if .not. $status
$ then
$   write sys$output -
 "%POPFACS-F-NOCMSWRITE, This account does not have write access to MASD CMS libraries"
$    write sys$output -
 "%POPFACS-F-VDEONLY, choose VDE only populate or EXIT."
$   GOTO Get_Populate_Scope
$ endif
$!
$!
$!+-----------------------------------------------------------------------------
$! obtain the batch queue to which they which to submit the populate jobs(s)
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
$ user_prompt = "Queue to submit populate jobs"
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
$   GOTO Get_Populate_Scope
$ endif
$!
$! validate the queue
$!
$ if .not. f$getqui("DISPLAY_QUEUE","QUEUE_BATCH","''batch_queue'")
$ then
$   write sys$output -
    "%POPFACS-F-INVQUEUE, ''batch_queue' is not a valid batch queue"
$   batch_queue = ""
$   GOTO Get_Queue
$ endif
$!
$!+
$!  Obtain the work directory to be used for log and work files
$!+
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
	"%POPFACS-F-INVLOG, invalid logical name, logical does not exist."
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
    "%POPFACS-F-INVDIR, invalid work directory, directory does not exist."
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
$ if hhmm .gt. 500 .and. hhmm .lt. 1700
$ then
$   default_value = f$extract(0,12,current_time) + "17:00:00"
$ else
$   default_value = "NOW"
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
$ user_prompt = "Time to begin submission of populate jobs?"
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
$   write sys$output "%POPFACS-F-INVTIME, ''submit_time' is not a valid time"
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
$! make sure the user doesn't enter a time in the past
$!
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
$ if submit_day .eqs. "Saturday" .or. submit_day .eqs. "Sunday" then GOTO Validate_input
$ hhmm = F$integer(f$extract(12,2,submit_time) + f$extract(15,2,submit_time))
$!
$ if hhmm .gt. 500 .and. hhmm .lt. 1700
$ then
$   GOSUB Override_Submit_limits
$   if return_to_previous_prompt then goto Get_Default_time
$   if submit_ans then goto Validate_Input
$   submit_time = ""
$   GOTO Get_Source
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
" If you proceed, you will populate stream ''target' from stream "
$ if fac_input
$ then
$   write sys$output -
  "''source' for the facilities you listed"
$ else
$   write sys$output -
  " ''source' for the facilities listed in ''facility_file'."
$ endif
$ write sys$output ""
$ user_prompt = "Do you wish to proceed?" 
$ default_value = "Y"
$ prompt_symbol = "submit_ans"
$ GOSUB Get_Text_Value
$ if return_to_previous_prompt then goto Get_Default_Time
$ if .not. submit_ans then GOTO Get_Mnemonic
$!
$ valid_input = 1
$ return_to_label = ""
$!
$ stream = source
$ if poption .nes. "CMS" then GOSUB Validate_Stream
$ if .not. valid_input then return_to_label = "GET_SOURCE"
$!
$ stream = target
$ if poption .nes. "CMS" then GOSUB Validate_Stream
$ if .not. valid_input
$ then
$   if return_to_label .eqs. "" then return_to_label = "GET_TARGET"
$ endif
$!
$ if return_to_label .nes. "" then GOTO 'return_to_label'
$!
$DO_Submit:
$!
$ if fac_input
$ then
$   GOSUB Build_Facility_File
$   if .not. wrote_output
$   then
$	write sys$output -
    "%POPFACS-E-NOFACS, No facilities found on your list, exiting..."
$	GOTO Main_Exit
$   endif
$ endif
$!
$ jobname = target - "." - "." + "_POP"
$ submit/notify/noprint/keep/after="''submit_time'" -
       /log='work_dir' -
       /name='jobname' -
       /queue='batch_queue' -
       /param=('mnemonic','target','source', -
	       'batch_queue','work_dir','full_facility_spec',"''poption'''moption'") -
    VDE$SYSTEM:PLMENU$$POPFACS.COM
$!
$Control_y:
$!
$Main_Exit:
$ if f$trnlnm("fac_file") .nes. "" then close fac_file
$ EXIT 1+0*'f$verify(sav_verify)
$!
$!+-----------------------------------------------------------------------------
$! Build a facilty file list from the list entered by the user
$!+-----------------------------------------------------------------------------
$Build_Facility_File:
$!-------------------
$ wrote_output = "FALSE"
$ index = 0
$ time = f$cvtime("","comparison")
$ facility_file = f$extract(5,2,time) + f$extract(8,2,time) + -
                  f$extract(11,2,time) + f$extract(14,2,time) + -
		  f$extract(17,2,time) + ".POP$TMP"
$ facility_file = work_dir + facility_file
$ full_facility_spec = f$parse(facility_file)
$!
$ open/write fac_file 'full_facility_spec'
$ if .not. $status then GOTO Main_Exit
$!
$FAC_LOOP:
$   facility = f$element(index,",",fac_list)
$   if facility .eqs. "," then GOTO No_More_Facs
$   if facility .eqs. "" then GOTO No_More_Facs
$   write fac_file facility
$   index = index + 1
$   wrote_output = "TRUE"
$   GOTO FAC_LOOP
$No_More_Facs:
$   close fac_file
$   RETURN
$!      
$!
$!+-----------------------------------------------------------------------------
$! Force the user to confirm that they want to submit the job during "normal
$! working hours
$!+-----------------------------------------------------------------------------
$Override_Submit_limits:
$!----------------------
$ submit_ans = "N"
$ type sys$input 

Due to locking contention on the master pack, this job should be submitted
after 5:00PM and before 5:00AM on weekdays.  It is advised to avoid 
submitting populations during regular working hours unless you are sure
the source stream and the target stream are almost identical.
 
$Get_submit_answer:
$ user_prompt = "Do you still want to submit the job at the entered time?"
$ default_value = "N"
$ prompt_symbol = "submit_ans"
$ GOSUB Get_Text_Value
$ submit_ans = f$extract(0,1,f$edit(submit_ans,"collapse,upcase"))
$ if submit_ans .eqs. "" then submit_ans = "N"
$!
$ RETURN
$!
$!+-----------------------------------------------------------------------------
$! validate the source stream exists before submitting the job
$!+-----------------------------------------------------------------------------
$Validate_stream:
$ write sys$output "Validating stream ''stream' in VDE..."
$ define/user/nolog sys$output _nla0:
$ define/user/nolog sys$error _nla0:
$ VDE show stream 'stream'
$ if $status .ne. %x10000001
$ then
$   write sys$output "%POPFACS-E-VALERR, validation error for stream ''stream' in VDE"
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
$Help_Overwrite:
$!
$ type sys$input

   Enter YES if you wish anything already in the target stream be
   overwritten.  Under normal circumstances, the target stream is
   newly created and empty, or the contents of the target stream
   are no long required, and this is the correct choice.

   Enter NO if there are modules already in the target stream, and
   these modules should be preserved -- this option should only be
   selected if parent of the specified target stream is the specified
   source stream.

   YES is the default.

$!
$ RETURN
$!
$Help_Mnemonic:
$!
$ type sys$input 

   Enter the VDE library mnemonic for the library that contains
   the stream to rename.  Examples of common VDE libraries are
   EVMS and VMS.

$!
$ RETURN
$!
$Help_source:
$!-----------
$!
$ type sys$input

     Enter the stream name from which to populate.  For example,
     to populate a build class V5.4_build from V5.4, you would 
     enter V5.4.  You can define the logical PLMENU$DEFAULT_SOURCE_STREAM
     to automatically supply a default value to this prompt.  Enter
     BACK or PREV to return to the mnemonic prompt.
	
$!
$ RETURN
$!
$!
$Help_target:
$!-----------
$!
$ type sys$input 

     Enter the stream name to be populated.  For example,
     to populate a build class V5.4_build from V5.4, you would 
     enter V5.4_build.  You can define logical PLMENU$DEFAULT_TARGET_STREAM
     to automatically supply a default value to this prompt. 
     Enter BACK or PREV to return to the source stream prompt.

$!
$ RETURN
$!
$Help_Facility_File:
$!------------------
$ type sys$input

    Enter the location of a file containing a list of facilities to populate.
    The file should be a text file containing one facility per record.   Enter
    BACK or PREV to return to the facility input prompt.

$!
$ RETURN
$!
$Help_Fac_Input:
$!------------------
$ type sys$input

    Answer "Y"es if you want to interactively enter the facilities to process.
    If you answer no, you will be expected to supply the name of a file which 
    lists the facilities to process.  Enter BACK or PREV to return to the target
    stream prompt. 

$!
$ RETURN
$!
$Help_Fac_list:
$!------------------
$ type sys$input

    Enter a list of facility names delimited by commas "," or enter one facility
    name.  Enter BACK or PREV to return to the facility input prompt.

$!
$ RETURN
$!
$Help_Poption:
$!-------------
$ type sys$input

    If you want to populate both the VDE database and the CMS libraries, enter
    BOTH to this prompt.  If you only want to populate the CMS classes enter CMS
    to this prompt.  If you want to populate the VDE database only, enter VDE to
    this prompt.  Enter BACK or PREV to return to the facility file prompt.

$!
$ RETURN
$!
$Help_Batch_queue:
$!
$ type sys$input

     Enter the batch queue in which to submit the populate stream 
     jobs.  You can define the logical PLMENU$DEFAULT_BATCH_QUEUE to
     supply a default value to this prompt.  Enter BACK or PREV to return to
     the populate scope prompt.

$!
$ RETURN
$!
$Help_Work_dir:
$!
$ type sys$input

     Enter a directory to be used for the log files and work files
     used by populate stream.  The work directory defaults to the
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

     Enter the time at which the populate stream job should begin.
     If there are many differences between the source and target streams
     it is advised to avoid running the populate during regular working
     hours.  Enter BACK or PREV to return to the work directory prompt.
     
$!
$ RETURN
$
$!+-----------------------------------------------------------------------------
$!
$!  POPULATE_FACILITIES - Obtains the information necessary to populate a VDE
$!  stream and the associated CMS class in selected facilities.  The user is
$!  prompted for the source stream, target stream, work directory, queue etc... 
$!
$!  The procedure submits ....:POPFACS.com to perform the VDE and CMS commands.
$!
$!  Modification History
$!
$!	DJS	X-1	04/22/91    Original Version 
$!+
$! End of POPULATE_FACILITIES.COM
