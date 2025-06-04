$ sav_verify = 'f$verify(0)
$!+
$! Documentation at end of command file
$!+
$ set = "set"
$ set symbol/scope=noglobal
$ delete /symbol/local set
$ if f$trnlnm("PLMENU$$VERIFY") .nes. "" then Set verify
$ if f$trnlnm("PLMENU$DEBUG") .nes. "" then set verify
$!
$!             123456789012345678901234567890123456789012345678901234567890 - 60 blanks
$ blank_key = "                                                            "
$!
$!+-----------------------------------------------------------------------------
$!	First, check to see if any parameters were passed to this command
$! procedure. If no parameters were passed, prompt user for input. We 
$! cannot do anything with out the necessary input.
$!------------------------------------------------------------------------------
$!
$ if (f$mode() .eqs. "BATCH") .AND. -
     ((p2 .eqs. "") .or. (p3 .eqs. "") .or. (p4 .eqs. "") .or. (p5 .eqs. "") )
$ then
$   Write sys$output "%POP-F-MISSPARM, Missing required parameter..."
$   goto Error_Exit
$ endif
$!
$Get_root:
$ if p2 .eqs. "" then inquire p2 -
	"Root to populate"
$ if p2 .eqs. "" then goto Get_root
$Get_fac:
$ if p3 .eqs. "" then inquire p3 -
	"Facility to populate"
$ if p3 .eqs. "" then goto Get_Fac
$start:
$ if p4 .eqs. "" then inquire p4 "Class name to populate"
$ if p4 .eqs. "" then goto start
$stopclass:
$ if p5 .eqs. "" then inquire p5 -
	"Class name to populate from"
$ if p5 .eqs. "" then goto stopclass
$workdir:
$ if p6 .eqs. "" then inquire p6 -
	"Status file"
$ if p6 .eqs. "" then goto workdir
$!
$ root = p2
$ facility = p3
$ target_class = p4
$ input_class = p5
$ stat_file_name = p6
$ pop_options = f$edit("''p7'","UPCASE,COLLAPSE")
$
$ pop_domerge = "FALSE"
$ if f$locate(",MERGE,",",''pop_options',") .ne. f$length(",''pop_options',") then pop_domerge = "TRUE"
$
$!+
$! check if populates are turned off on this node
$!+
$ if f$trnlnm("FAC$ABORT_POPLIBS") .nes. ""
$ then
$    write sys$output "%POP-E-LOGNMABRT, FAC$ABORT_POPLIBS logical found"
$    error_message = "*ABORTED FAC$ABORT_POPLIBS logical found"
$    GOTO Error_EXIT
$ endif
$!
$!+-----------------------------------------------------------------------------
$! update the status file to indicate this facility is executing
$!+-----------------------------------------------------------------------------
$ stat_key = root + ":" + facility + blank_key
$ stat_key = f$extract(0,60,stat_key)
$ on error then goto error_exit
$ open/read/write/share statfile 'stat_file_name'
$ read/key="''stat_key'" statfile statfile_rec
$ node = f$trnlnm("sys$node") - "::"
$ stat_key = root + ":" + facility + blank_key
$ stat_key = f$extract(0,60,stat_key)
$ statfile_rec =  "''stat_key' ''node' ''f$extract(0,17,f$time())' *EXECUTING"
$ write/update statfile statfile_rec
$ close statfile
$!
$!
$!+-----------------------------------------------------------------------------
$! check to see if both classes are present.  If one or both classes
$! are not present print an appropriate warning message and exit.
$!+-----------------------------------------------------------------------------
$!
$ CMS$_NOTFOUND = %X109C8672
$ SET MESSAGE sys$message:cmsmsg.exe
$! 
$! use set CMS library to capture any errors accessing the library
$!
$ set noon
$ define/nolog CMS$LIB 'root':['facility'.cms]
$!
$! Explicitly check for CMS$_NOTFOUND condition in order to identify any
$! problems accessing the library, i.e. library locked or corrupt
$! 
$ target_not_found = 0
$ define/user sys$error _nla0:
$ define/user sys$output _nla0:
$ CMS SHOW CLASS 'target_class'
$ if .not. $status
$ then
$   if $status .eq. CMS$_NOTFOUND
$   then
$	target_not_found = 1
$   else
$	error_message = "*ABORTED ''f$message($status)'"
$	write sys$output "%POP-F-SHOCLSERR, Error on CMS show class ''target_class'"
$	GOTO Error_Exit
$   endif
$ endif
$!
$ input_not_found = 0
$ define/user sys$error _nla0:
$ define/user sys$output _nla0:
$ CMS SHOW CLASS 'input_class'
$ if .not. $status
$ then
$   if $status .eq. CMS$_NOTFOUND
$   then
$	input_not_found = 1
$   else
$	error_message = "*ABORTED ''f$message($status)'"
$	write sys$output "%POP-F-SHOCLSERR, Error on show class ''input_class'"
$	GOTO Error_Exit
$   endif
$ endif
$!
$ if target_not_found .and. input_not_found
$ then
$   write sys$output "%POP-W-NEITHER, Neither class found in ''facility'"
$   error_message = "*WARNING Neither class found"
$   GOTO Error_Exit
$ endif
$!
$ if input_not_found
$ then
$   write sys$output "%POP-W-NOINPUT, class ''input_class' not found in ''facility'"
$   error_message = "*WARNING ''input_class' not found"
$   GOTO Error_Exit
$ endif
$!
$ if target_not_found
$ then
$   write sys$output "%POP-I-NOTARGET, class ''target_class' not found in ''facility'"
$   write sys$output "%POP-I-CREATE, class ''target_class' will be created in ''facility'"
$ endif
$!
$!
$!+--------------------------------------------------------------------
$! Populate the target stream from the base stream using FACPOPULATE
$!---------------------------------------------------------------------
$ set command vde$system:plmenu$$facpop.cld
$!
$ write sys$output "%POP-I-POPCMS, Populating ''target_class' from ''input_class' at ''F$time()'"
$!
$ if pop_domerge
$ then
$   FACPOPULATE/create/log=ALL/process=ALL/MERGE 'root':['facility'.cms] 'input_class' 'target_class'
$ else
$   FACPOPULATE/create/log=ALL/process=ALL/REMOVE 'root':['facility'.cms] 'input_class' 'target_class'
$ endIf
$ if .not. $status
$ then
$   error_message = "*ABORTED"
$   GOTO Error_Exit
$ endif
$!
$ if f$trnlnm("FAC$ABORT_POPLIBS") .nes. ""
$ then
$    write sys$output "%POP-E-LOGNMABRT, FAC$ABORT_POPLIBS logical found"
$    error_message = "*ABORTED FAC$ABORT_POPLIBS logical found"
$    GOTO Error_EXIT
$ endif
$!
$!+-----------------------------------------------------------------------------
$!   EXIT ROUTINES - update the status file with the 
$!+-----------------------------------------------------------------------------
$Main_exit:
$ open/read/write/share statfile 'stat_file_name'
$ stat_key = root + ":" + facility + blank_key
$ stat_key = f$extract(0,60,stat_key)
$ read/key="''stat_key'" statfile statfile_rec
$ stat_key = root + ":" + facility + blank_key
$ stat_key = f$extract(0,60,stat_key)
$ statfile_rec =  "''stat_key' ''node' ''f$extract(0,17,f$time())'  POPULATED "
$ write/update statfile statfile_rec
$ close statfile
$! write some extra lines to the log so SCANBLD won't complain
$ i = 0
$ write sys$output "$! <filler lines for reputed SCANBLD bug>"
$write_loop:
$ write sys$output "$! <************   SCANBLD  ************>"
$ i = i + 1
$ if i .lt. 10 then GOTO write_loop
$!
$ exit 1 + 0*F$VERIFY(sav_verify)
$!
$Error_exit:
$ open/read/write/share statfile 'stat_file_name'
$ stat_key = root + ":" + facility + blank_key
$ stat_key = f$extract(0,60,stat_key)
$ read/key="''stat_key'" statfile statfile_rec
$ stat_key = root + ":" + facility + blank_key
$ stat_key = f$extract(0,60,stat_key)
$ statfile_rec =  "''stat_key' ''node' ''f$extract(0,17,f$time())'  ''error_message' "
$ write/update statfile statfile_rec
$ close statfile
$ exit 1 + 0*F$VERIFY(sav_verify)
$!
$!++
$! POPCLASS.COM - Populate a CMS class (specified by P4) from another CMS 
$!	           class (specified by P5). For the facility specified in P3.
$!                 This job is intended to be executed as a batch job 
$!
$!
$! Inputs:    Description               (local symbol name)
$!            ------------------------- -------------------                                   
$!      P1    CMS library specification	               <-------| compatible with
$!	P2    Root			(root)                 |    FACBATCH
$!      P3    Facilty to be populated	(facility)     <-------|
$!	P4    Class to be populated,	(target_class)
$!	P5    Class which will be used to populate class specified by P4.
$!					(input_class)
$!	P6    Status File Name		(stat_file_name)
$!	P7    Options			(pop_options)
$! EX:  P2 = VMS$
$!      P3 = DRIVER   Populate the class in the driver facility.
$!      P4 = V5.5   The class V5.5 will be populated from the class V5.4.
$!      P5 = V5.4  
$!
$! Outputs: NONE
$!
$! Work files: NONE
$!
$!--  Modification History
$!  
$!	    DJS		X-1	    Original Version
$!
$!	    DJS          Jul-30-1992
$!		Update to check for CMS$_NOTFOUND from CMS show class call
$!		rather than simply checking .NOT. STATUS.  These
$!		changes are designed to catch errors where access to the
$!              CMS library is denied.
