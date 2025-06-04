$!
$! Procedure to try and gather all possible information out of an existing
$! queue file, to prepare for an attempt at recovery.
$!
$! As output, creates a command procedure called FIXQUE_RELOAD.COM which can
$! be run to restore the previously existing queues, jobs, characteristics,
$! and form definitions after creating a new, empty queue file.
$!
$! Parameters to control operation, mostly for testing:
$! V4 Parse assuming VMS 4.x SHOW QUEUE listing format
$! V5 Parse assuming VMS 5.x SHOW QUEUE listing format (x=0,1,2,3,4)
$! V5.5 Parse assuming VMS 5.5 or later SHOW QUEUE listing format
$!    (default is to look at the running system to determine version)
$! RERUN Run using old .LIST files from a previous run
$!
$!                                               Keith B. Parris 5/89, 10/91
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$!   http://encompasserve.org/~parris/ or http://www.geocities.com/keithparris/ 
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$! First, clean up in case we didn't finish cleanly before
$ close/nolog chars
$ close/nolog forms
$ close/nolog queue
$ close/nolog jobs
$ close/nolog generic
$ close/nolog start
$ close/nolog com
$ parameters = -
   f$edit(" ''p1' ''p2' ''p3' ''p4' ''p5' ''p6' ''p7' ''p8' ","UPCASE,COMPRESS")
$ if parameters .eqs. " " then parameters = "(none)"
$ param_len = f$length(parameters)
$!
$ rerun   = f$locate(" RER",parameters) .ne. param_len
$!
$! Determine the VMS version, so we know which listing formats to assume.
$! The default is the version we find we're running.
$! 4.x   Version 4
$! 5.0 through 5.4-3 Version 5
$! 5.5 or later  Version 5.5
$! Note that the A5.x variants for rolling upgrade support produce the
$! newer 5.x listing format, even though they use the old queue file format...
$ vms_version = f$edit(f$getsyi("VERSION"),"TRIM")
$ version = 55 !Assume V5.5 or later unless we determine otherwise
$ if f$extract(1,2,vms_version) .eqs. "4." then version = 4
$ vms_version_number = f$element(0,"-",f$extract(1,9,vms_version))  !x.x
$ if vms_version_number .eqs. "5.0" -
      .or. vms_version_number .eqs. "5.1" -
      .or. vms_version_number .eqs. "5.2" -
      .or. vms_version_number .eqs. "5.3" -
      .or. vms_version_number .eqs. "5.4" then version = 5
$! User-entered parameters override the running version
$ if f$locate(" V4 ",parameters) .ne. param_len then version = 4
$ if f$locate(" V5 ",parameters) .ne. param_len then version = 5
$ if f$locate(" V5.5 ",parameters) .ne. param_len then version = 55
$! Set up some symbols to simplify the code and speed execution
$ if_v4 = "!"
$ if_v5 = "!"
$ if_v55 = "!"
$ if_v'version' = ""
$ if_not_v4 = ""
$ if_not_v5 = ""
$ if_not_v55 = ""
$ if_not_v'version' = "!"
$! Don't need OPER privilege for a re-run on old files -->
$ old_priv = ""
$ if rerun then goto GOT_PRIVS
$! User needs OPER privilege (e.g. to see jobs of all users)
$ old_priv = f$setprv("OPER")
$ if f$privilege("OPER") then goto GOT_PRIVS
$ write sys$output "OPER privilege is required to run this procedure."
$ exit
$ GOT_PRIVS:
$ got_error = "FALSE" !Remember whether we encountered an error
$ not_in_dir = "FALSE" !Remember whether we hit a file which was not
$    !entered in a directory and can't be requeued
$!
$ open/write com FIXQUE_RELOAD.COM !Output command procedure
$ write com "$! FIXQUE_RELOAD.COM, created by FIXQUE.COM version T1.8-2"
$ write com "$! under VMS version ",vms_version,-
  ".  Parameters: ",f$edit(parameters,"TRIM")
$ write com "$X=F$SETPRV(""CMKRNL,OPER,SYSPRV"")"
$ write com "$IF F$PRIVILEGE(""CMKRNL,OPER,SYSPRV"") THEN GOTO PRIV_OK"
$ write com "$WRITE SYS$OUTPUT ",-
  """You need CMKRNL, OPER, and SYSPRV privileges."""
$ write com "$EXIT"
$ write com "$PRIV_OK:"
$ write com "$SET NOON" !Don't stop on errors
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$!
$! Handle characteristic information
$!
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$ on error then goto CHAR_ERR !Trap errors
$ if .not. rerun then -
    write sys$output "Doing $SHOW QUEUE/CHARACTERISTICS"
$ if .not. rerun then -
    show queue/CHARACTERISTICS/output=fixque_CHARS.list !Characteristics
$ on error then exit !Restore original error handling
$ goto GOT_CHAR_INFO
$ CHAR_ERR:
$ err_code = $status
$ on error then exit !Restore original error handling
$ err_msg = f$message(err_code)
$ if err_msg .eqs. "%JBC-E-NOSUCHCHAR, no such characteristic" then -
  goto C_NONE
$ write sys$output "FIXQUE: Encountered error ",err_msg
$ write sys$output "doing $SHOW QUEUE/CHARACTERISTICS/OUTPUT=FIXQUE_CHARS.LIST"
$ exit
$ C_NONE:
$ close/nolog chars !In case we came here on an empty file
$ write sys$output "No characteristics found"
$ write com "$WRITE SYS$OUTPUT ""No characteristics defined."""
$ goto C_DONE !No characteristics defined --> skip this
$ GOT_CHAR_INFO:
$ list_file = "FIXQUE_CHARS.LIST"
$ open/read chars 'list_file'
$ read/end=C_NONE chars record
$ list_record = 1
$! Parse the characteristics listing and create DEFINE/CHARACTERISTIC commands
$ write sys$output "Parsing characteristics..."
$ write com "$ASK_CHARS:"
$ write com "$INQUIRE ANS ""Do you wish to recreate characteristics? [Y]"""
$ write com "$ANS=F$EXTRACT(0,1,F$EDIT(ANS,""TRIM,UPCASE""))"
$ write com "$IF ANS .EQS. ""N"" THEN GOTO DONE_CHARS"
$ write com "$IF ANS .NES. """" .AND. ANS .NES. ""Y"" THEN GOTO ASK_CHARS"
$ write com "$WRITE SYS$OUTPUT ""Defining characteristics"""
$! Skip the header records
$!Characteristic name                  Number
$!-------------------                  ------
$ if f$extract(0,19,record) .nes. "Characteristic name" then goto SHOW_ERROR
$ read/end=SHOW_ERROR chars record
$ list_record = 2
$ if f$extract(0,19,record) .nes. "-------------------" then goto SHOW_ERROR
$! Characteristics descriptions look like the following:
$!Characteristic name                  Number
$!-------------------                  ------
$!TEST                                      1
$!         10        20        30        40        50        60        70
$!0123456789.123456789.123456789.123456789.123456789.123456789.123456789.1234567
$ C_LOOP_1:
$ read/end=C_EOF chars record
$ list_record = list_record + 1
$ if record .eqs. "" then goto SHOW_ERROR !Unexpected blank line -->
$ if f$extract(0,1,record) .eqs. " " then goto SHOW_ERROR
$ char_name = f$edit(f$extract(0,32,record),"TRIM")
$ char_number = f$edit(f$extract(33,10,record),"TRIM")
$ if f$type(char_number) .nes. "INTEGER" then goto SHOW_ERROR
$ write sys$output "  Characteristic ",char_name
$ write com "$DEFINE/CHARACTERISTIC ",char_name," ",char_number
$ goto C_LOOP_1
$ C_EOF:
$ write com "$DONE_CHARS:"
$ close chars
$ C_DONE:
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$!
$! Handle form definition information
$!
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$ on error then goto FORM_ERR !Trap errors
$ if .not. rerun then -
    write sys$output "Doing $SHOW QUEUE/FORM/FULL"
$ if .not. rerun then -
    show queue/FORM/full/output=fixque_FORMS.list !Form definitions
$ on error then exit !Restore original error handling
$ goto GOT_FORM_INFO
$ FORM_ERR:
$ err_code = $status
$ on error then exit !Restore original error handling
$ err_msg = f$message(err_code)
$ if err_msg .eqs. "%JBC-E-NOSUCHFORM, no such form" then goto F_NONE
$ write sys$output "FIXQUE: Encountered error ",err_msg
$ write sys$output "doing $SHOW QUEUE/FORM/FULL/OUTPUT=FIXQUE_FORMS.LIST"
$ exit
$ F_NONE:
$ close/nolog forms !In case we came here on an empty file
$ write sys$output "No forms found"
$ write com "$WRITE SYS$OUTPUT ""No forms defined."""
$ goto F_DONE !No forms defined --> skip this
$ GOT_FORM_INFO:
$ list_file = "FIXQUE_FORMS.LIST"
$ open/read forms 'list_file'
$ read/end=F_NONE forms record
$ list_record = 1
$! Parse the form definition listing and create DEFINE/FORM commands
$ write sys$output "Parsing form definitions..."
$ write com "$ASK_FORMS:"
$ write com "$INQUIRE ANS ""Do you wish to recreate form definitions? [Y]"""
$ write com "$ANS=F$EXTRACT(0,1,F$EDIT(ANS,""TRIM,UPCASE""))"
$ write com "$IF ANS .EQS. ""N"" THEN GOTO DONE_FORMS"
$ write com "$IF ANS .NES. """" .AND. ANS .NES. ""Y"" THEN GOTO ASK_FORMS"
$ write com "$WRITE SYS$OUTPUT ""Defining forms"""
$! Skip the header records
$!Form name                            Number   Description
$!---------                            ------   -----------
$ if f$extract(0,9,record) .nes. "Form name" then goto SHOW_ERROR
$ read/end=SHOW_ERROR forms record
$ list_record = 2
$ if f$extract(0,9,record) .nes. "---------" then goto SHOW_ERROR
$! Forms descriptions look like the following:
$!BILL_LANDSCAPE                           29   BILL_LANDSCAPE
$!    /LENGTH=80 /SETUP=(BILL) /STOCK=BILL_LANDSCAPE /TRUNCATE /WIDTH=255
$!         10        20        30        40        50        60        70
$!0123456789.123456789.123456789.123456789.123456789.123456789.123456789.1234567
$ F_LOOP_1:
$ read/end=F_EOF forms record
$ list_record = list_record + 1
$ F_CONT_1:
$ if record .eqs. "" then goto SHOW_ERROR !Unexpected blank line -->
$ if f$extract(0,1,record) .eqs. " " then goto SHOW_ERROR
$ form_name = f$edit(f$extract(0,32,record),"TRIM")
$! The SHOW command displays nothing if the form is set to /NOTRUNCATE, and
$! if there are no margins, it doesn't display a /MARGIN qualifier.  However,
$! the DCL command defaults to /TRUNCATE /MARGIN=(BOTTOM=6), so we must
$! check the qualifiers shown, so that the DCL defaults don't slip in.
$! If we don't see /TRUNCATE or /WRAP, we'll add /NOTRUNCATE at the end.
$! If we don't see /MARGIN at all, we'll add /MARGIN=(BOTTOM=0) at the end;
$! if there is a /MARGIN qualifier, but it doesn't include BOTTOM=nnn, we'll
$! add ,BOTTOM=0 to the /MARGIN qualifier.
$ f_notruncate = "TRUE" !Remember if we need to add /NOTRUNCATE
$ f_bottom = "TRUE" !Remember if we need to add /MARGIN=(BOTTOM=0)
$ if f$extract(43,3,record) .nes. "   " then goto F_2_2LINE  !Grab contin. line -->
$ F_2_2LINE_1:
$ fn_len = f$length(form_name)
$ if f$locate("/",form_name) .ne. fn_len then -
  goto SHOW_ERROR  !Found a slash -- must be out of sync
$! Remove "(stock=xxxx)" if found
$!JUNK (stock=DEFAULT)                     30   JUNK
$!    /LENGTH=66 /MARGIN=(LEFT=2) /SETUP=(PORTRAIT) /STOCK=DEFAULT /TRUNCATE /WIDTH=80
$ t = f$locate(" (",form_name)
$ if t .ne. fn_len then form_name = f$extract(0,t,form_name)
$ form_number = f$edit(f$extract(33,10,record),"TRIM")
$ if f$type(form_number) .nes. "INTEGER" then goto SHOW_ERROR
$ form_desc = f$extract(46,999,record)
$ form_line = "$DEFINE/FORM/DESCRIPTION=""" + form_desc + """"
$ write sys$output "  Form ",form_name
$ F_LOOP_2:
$ read/end=F_END_2 forms record
$ list_record = list_record + 1
$ if record .eqs. "" then goto F_END_2 !Blank line at end of form -->
$ if f$extract(0,5,record) .nes. "    /" then goto SHOW_ERROR
$ i = 1
$! Loop to grab each of the DEFINE/FORM qualifiers
$ F_LOOP_2A:
$ qualifier = f$edit(f$element(i,"/",record),"TRIM")
$ if qualifier .eqs. "/" then goto F_LOOP_2 !Grab next record -->
$ i = i + 1
$! If we see /WRAP or /TRUNCATE explicitly specified, we won't add /NOTRUNCATE
$! at the end.
$ if qualifier .eqs. "TRUNCATE" .or. -
    qualifier .eqs. "WRAP"  then f_notruncate = "FALSE"
$ if f$extract(0,6,qualifier) .eqs. "MARGIN" then goto F_2_MARGIN
$ F_2_MARGIN_DONE:
$ if (form_line .eqs. "   " ) .or. -
    (f$length(form_line) + f$length(qualifier) .le. 77) then -
  goto F_2_ADD_QUAL
$ write com form_line,"-"
$ form_line = "   " !Indent continuation lines
$ F_2_ADD_QUAL:
$ form_line = form_line + "/" + qualifier
$ goto F_LOOP_2A
$!
$ F_2_2LINE:
$ form_name = record !Take the whole first record as form name string
$ read/end=SHOW_ERROR forms record
$ list_record = list_record + 1
$ if f$extract(0,33,record) .nes. "                                 " -
  then goto SHOW_ERROR !Not a continuation line --> out of sync
$ goto F_2_2LINE_1
$!
$ F_2_MARGIN:
$ f_bottom = "FALSE" !Won't need to add /MARGIN=(BOTTOM=0) at end
$! If we find BOTTOM=nnn already in there, no action is needed; otherwise we
$! need to add ,BOTTOM=0 to the end of the existing /MARGIN qualifier list
$ if f$locate("BOTTOM",qualifier) .eq. f$length(qualifier) then -
  qualifier = qualifier - ")" + ",BOTTOM=0)"
$ goto F_2_MARGIN_DONE
$!
$ F_END_2:
$ if .not. f_notruncate then goto F_E2_NOTR2
$ if (form_line .eqs. "   " ) .or. -
    (f$length(form_line) + 10 .le. 77) then -
  goto F_E2_NOTR1
$ write com form_line,"-"
$ form_line = "   " !Indent continuation lines
$ F_E2_NOTR1:
$ form_line = form_line + "/NOTRUNCATE"
$ F_E2_NOTR2:
$!
$ if .not. f_bottom then goto F_E2_BOTT2
$ if (form_line .eqs. "   " ) .or. -
    (f$length(form_line) + 17 .le. 77) then -
  goto F_E2_BOTT1
$ write com form_line,"-"
$ form_line = "   " !Indent continuation lines
$ F_E2_BOTT1:
$ form_line = form_line + "/MARGIN=(BOTTOM=0)"
$ F_E2_BOTT2:
$!
$ if (f$length(form_line) + f$length(form_name) + -
  f$length(form_number)) .le. 77 then goto F_E2A
$ write com form_line,"-"
$ form_line = "     "
$ F_E2A:
$ write com form_line," ",form_name," ",form_number
$ goto F_LOOP_1
$ F_EOF:
$ write com "$DONE_FORMS:"
$ close forms
$ F_DONE:
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$!
$! Create commands to initialize queues and resubmit jobs
$!
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$! List existing queues and all the jobs in them
$ on error then goto QUEUE_ERR !Trap errors
$ if .not. rerun then -
    write sys$output "Doing $SHOW QUEUE/ALL/FULL"
$ if .not. rerun then -
    show queue/all/full/output=fixque_QUEUE.list !Queues and jobs
$ on error then exit !Restore original error handling
$ goto GOT_QUEUE_INFO
$ QUEUE_ERR:
$ err_code = $status
$ on error then exit !Restore original error handling
$ err_msg = f$message(err_code)
$ if err_msg .eqs. "%JBC-E-NOSUCHQUE, no such queue" then goto Q_NONE
$ write sys$output "FIXQUE: Encountered error ",err_msg
$ write sys$output "doing $SHOW QUEUE/ALL/FULL/OUTPUT=FIXQUE_QUEUE.LIST"
$ exit
$ Q_NONE:
$ close/nolog queue !In case we came here on an empty file
$ write sys$output "No queues found"
$ write com "$WRITE SYS$OUTPUT ""No queues defined."""
$ goto Q_DONE !No queues defined --> skip this
$ GOT_QUEUE_INFO:
$ list_file = "FIXQUE_QUEUE.LIST"
$ open/read queue 'list_file'
$ read/end=Q_NONE queue record     !Read 1st queue description line
$ list_record = 1
$! Parse the queue/job listing and create INIT/QUEUE and SUBMIT or PRINT commands
$ write sys$output "Parsing queues and jobs..."
$! Open separate temporary files to contain commands to create generic queues,
$! jobs, and START/QUEUE commands, so we can put the commands in the correct
$! order in the output command procedure
$ open/write generic FIXQUE_QUEUE_GENERIC.TEMP
$ open/write jobs FIXQUE_QUEUE_JOBS.TEMP
$ open/write start FIXQUE_QUEUE_START.TEMP
$ write com "$ASK_QUEUE:"
$ write com "$INQUIRE ANS ""Do you wish to recreate queues? [Y]"""
$ write com "$ANS=F$EXTRACT(0,1,F$EDIT(ANS,""TRIM,UPCASE""))"
$ write com "$IF ANS .EQS. """" THEN ANS = ""Y"""
$ write com "$IF ANS .NES. ""N"" .AND. ANS .NES. ""Y"" THEN GOTO ASK_QUEUE"
$ write com "$IF .NOT. ANS THEN GOTO DONE_QUEUES"
$ write com "$WRITE SYS$OUTPUT ""Recreating queues"""
$ write jobs "$ASK_JOB:"
$ write jobs "$INQUIRE ANS ""Do you wish to recreate print and batch jobs? [Y]"""
$ write jobs "$ANS=F$EXTRACT(0,1,F$EDIT(ANS,""TRIM,UPCASE""))"
$ write jobs "$IF ANS .EQS. """" THEN ANS = ""Y"""
$ write jobs "$IF ANS .NES. ""N"" .AND. ANS .NES. ""Y"" THEN GOTO ASK_JOB"
$ write jobs "$IF .NOT. ANS THEN GOTO DONE_JOBS"
$ write jobs "$WRITE SYS$OUTPUT ""Recreating jobs"""
$ queue_type_phrases = -
  ";Batch queue " + -
  ";Generic batch queue " + -
  ";Printer queue " + -
  ";Generic printer queue " + -
  ";Terminal queue " + -
  ";Remote terminal queue " + -
  ";Generic terminal queue " + -
  ";Server queue " + -
  ";Generic server queue " + -
  ";Output queue " + -
  ";Logical queue "
$! "Output queue" appears when the /ON= qualifier points to a nonexistent device
$ queue_type_phrase_len = -
  ";12" + - !"Batch queue "
  ";20" + - !"Generic batch queue "
  ";14" + - !"Printer queue "
  ";22" + - !"Generic printer queue "
  ";15" + - !"Terminal queue "
  ";22" + - !"Remote terminal queue "
  ";23" + - !"Generic terminal queue "
  ";13" + - !"Server queue "
  ";21" + - !"Generic server queue "
  ";13" + - !"Output queue "
  ";14"  !"Logical queue "
$ queue_type_qualifiers = -
  ";/BATCH" + -   !Batch queue
  ";/BATCH/GENERIC" + -  !Generic batch queue
  ";/DEVICE=PRINTER" + -  !Printer queue
  ";/DEVICE=PRINTER/GENERIC" + - !Generic printer queue
  ";/DEVICE=TERMINAL" + -  !Terminal queue
  ";/DEVICE=TERMINAL" + -  !Remote terminal queue
  ";/DEVICE=TERMINAL/GENERIC" + - !Generic terminal queue
  ";/DEVICE=SERVER" + -  !Server queue
  ";/DEVICE=SERVER/GENERIC" + - !Generic server queue
  ";/DEVICE" + -   !Output queue
  ";"    !Logical queue
$! For VMS 4.x, the /DEVICE={PRINTER|TERMINAL|SERVER} qualifier isn't valid
$ if version .eq. 4 then queue_type_qualifiers = -
  ";/BATCH" + -    !Batch queue
  ";/BATCH/GENERIC" + -   !Generic batch queue
  ";" + -   !/PRINTER !Printer queue
  ";/GENERIC" + -  !/PRINTER !Generic printer queue
  ";/TERMINAL" + -   !Terminal queue
  ";/TERMINAL" + -   !Remote terminal queue
  ";/TERMINAL/GENERIC" + -  !Generic terminal queue
  ";" + -   !/DEVICE=SERVER !Server queue
  ";/GENERIC" + -  !/DEVICE=SERVER !Generic server queue
  ";" + -   !/DEVICE !Output queue
  ";"     !Logical queue
$! Indicate which queues are generic.  We have to define generic queues AFTER
$! all their target queues are defined, or we get errors.
$ queue_type_generic = " " + -
  "F" + - !Batch queue
  "T" + - !Generic batch queue
  "F" + - !Printer queue
  "T" + - !Generic printer queue
  "F" + - !Terminal queue
  "F" + - !Remote terminal queue
  "T" + - !Generic terminal queue
  "F" + - !Server queue
  "T" + - !Generic server queue
  "F" + - !Output queue
  "F" !Logical queue
$! Access control list entries vary in indentation
$!   For V4/V5, the ACE is indented 10 spaces
$!   For V5.5,  the ACE is indented 9 spaces
$ 'if_not_v55 ace_blank_cnt = 10
$ 'if_v55 ace_blank_cnt = 9
$ ace_blanks = f$extract(0,ace_blank_cnt,"          ")
$! For V5.5, the job listing columns were rearranged to put the entry
$! number in the first column (where it won't get confused with the
$! vary between versions.
$ 'if_not_v55 job_hdr = "  Jobname         Username     Entry"
$ 'if_v55 job_hdr = "  Entry  Jobname         Username"
$ job_hdr_len = f$length(job_hdr)
$ 'if_not_v55 job_hdr2 = "  -------         --------     -----"
$ 'if_v55 job_hdr2 = "  -----  -------         --------"
$ job_hdr2_len = f$length(job_hdr2)
$! When the job name is too long for its column, a continuation line is used.
$! Continuation lines start at different places for pre- and post-5.5 listings.
$ 'if_not_v55 cont_blank_cnt = 18 !18 blanks on front for V4.7 or V5.0, or
$ 'if_v55 cont_blank_cnt = 25 !25 blanks on front for V5.5 continuation line
$ cont_blanks = f$extract(0,cont_blank_cnt,"                         ")
$! If there are a lot of qualifiers for a given job, they are listed on several
$! lines.  These lines are indented different numbers of spaces for pre- and
$! post-5.5 listings.  Also, we tell when the continuation lines are done when
$! we hit a line which specifies another file:
$!   For V4,   the filename is indented 6 spaces, without a "File:" prefix
$!   For V5,   the filename is indented 4 spaces, with a "File:" prefix
$!   For V5.5, the filename is indented 9 spaces, with a "File:" prefix
$ 'if_not_v55 leading_blank_cnt = 4
$ 'if_v55 leading_blank_cnt = 9
$ leading_blanks = f$extract(0,leading_blank_cnt,"         ")
$ 'if_v4 file_prefix = "  "  !V4: Just two extra spaces in front of filename
$ 'if_not_v4 file_prefix = "File:" !V5,V55: "File:" in front of filename
$ file_prefix_len = f$length(file_prefix)
$! Error messages for Retained-on-error jobs vary in indentation
$!   For V4/V5, the message is indented 4 spaces
$!   For V5.5,  the message is indented 7 spaces
$ 'if_not_v55 error_blank_cnt = 4
$ 'if_v55 error_blank_cnt = 7
$ error_blanks = f$extract(0,error_blank_cnt,"       ")
$ Q_LOOP_1:
$ parsing_job = "FALSE" !Now parsing a queue
$ i = 0
$ Q_LOOP_1T:
$ i = i + 1
$ next_queue_type_phrase = f$element(i,";",queue_type_phrases) 
$ if next_queue_type_phrase .eqs. ";" then goto SHOW_ERROR !--> No match
$ qtp_len = f$element(i,";",queue_type_phrase_len)
$ if f$extract(0,qtp_len,record) .nes. next_queue_type_phrase -
         then goto Q_LOOP_1T
$ queue_type = f$element(i,";",queue_type_qualifiers)
$! Write ordinary queue information to the COM file directly.  Write generic
$! queues to the GENERIC temporary file instead, which we'll add to the end,
$! after all the other queues are defined
$ q_out = "COM"
$ if f$extract(i,1,queue_type_generic) then q_out = "GENERIC"
$! The default for print queues on the DCL INITIALIZE/QUEUE command is
$! /DEFAULT=(FEED).  We normally need to override this with /DEFAULT=(NOFEED).
$ q_nofeed = "TRUE" !Remember if we need to add /DEFAULT=(NOFEED)
$ if q_out .eqs. "GENERIC" then -
  q_nofeed = "FALSE"  !No /DEFAULT for generic queues
$ if f$locate("BATCH",queue_type) .ne. qtp_len then -
  q_nofeed = "FALSE"  !No /DEFAULT for batch queues
$ trec = f$extract(qtp_len,999,record) !Skip past the queue type phrase
$ qt_len = f$length(queue_type)
$ queue_name = f$element(0,",",trec)
$ write sys$output "  Queue ",queue_name
$! Examine each of the remaining phrases on the queue description line
$ queue_line = "$INITIALIZE/QUEUE" + queue_type
$ queue_start = "/START" !Start queue by default
$ logical_queue = "" !Not a logical queue until proven guilty
$ i = 1
$ Q_LOOP_1A:
$ phrase = f$element(i,",",trec)
$ if phrase .eqs. "," then goto Q_END_1A
$ i = i + 1
$ if f$extract(0,4,phrase) .eqs. " on " then goto Q_1A_ON
$ if f$extract(0,13,phrase) .eqs. " mounted form" then goto Q_1A_FORM
$ if f$extract(0,12,phrase) .eqs. " assigned to" then goto Q_1A_LOGICAL
$ if f$extract(0,7,phrase) .eqs. " paused" then goto Q_NOSTART
$ if f$extract(0,8,phrase) .eqs. " stopped" then goto Q_NOSTART
$ if f$extract(0,9,phrase) .eqs. " stopping" then goto Q_NOSTART
$ if f$extract(0,10,phrase) .eqs. " resetting" then goto Q_NOSTART
$ if f$extract(0,19,phrase) .eqs. " device unavailable" then goto Q_NOSTART
$ if f$extract(0,8,phrase) .eqs. " stalled" then goto Q_NOSTART
$ if f$extract(0,9,phrase) .eqs. " starting" then -
   goto Q_INFO !Just mention this problem to the user -->
$ if f$extract(0,7,phrase) .eqs. " closed" then goto Q_CLOSED
$! Handle phrases added at V5.5:
$!   Ignore "idle" status -- it's normal
$ 'if_v55 if f$extract(0,5,phrase) .eqs. " idle" then goto Q_LOOP_1A
$!   Ignore "busy" status -- it's normal
$ 'if_v55 if f$extract(0,5,phrase) .eqs. " busy" then goto Q_LOOP_1A
$!   Ignore "available" status -- it's normal
$ 'if_v55 if f$extract(0,10,phrase) .eqs. " available" then goto Q_LOOP_1A
$ write sys$output "  ...unrecognized phrase ignored: ",phrase
$ goto Q_LOOP_1A
$ Q_INFO: !For informational status...
$ write sys$output "  ...is in",phrase," state"
$ goto Q_LOOP_1A
$! Don't restart queue if it was in stalled, stopped, paused states
$ Q_NOSTART:
$ message = "and will not be started by FIXQUE_RELOAD.COM"
$!    !Queue will not be restarted
$ if queue_start .eqs. "/NOSTART" then message = "as well"
$ write sys$output "  ...is in",phrase," state ",message
$ message = message - " by FIXQUE_RELOAD.COM"
$ write start "$WRITE SYS$OUTPUT ""Queue ",queue_name," was in",phrase,-
  " state ",message,""""
$ queue_start = "/NOSTART"
$ goto Q_LOOP_1A
$! Don't restart queue if it was in stalled, stopped, paused states
$ Q_CLOSED:
$ write sys$output "  ...is in a closed state and will be reclosed after"
$ write sys$output "     any jobs found have been resubmitted"
$ write start "$WRITE SYS$OUTPUT ""Queue ",queue_name," closed"
$ write start "$SET QUEUE/CLOSE ",queue_name
$ goto Q_LOOP_1A
$ Q_1A_LOGICAL: !Found a logical queue.  Extract the target destination queue
$ phrase = phrase - " assigned to "
$ logical_queue = phrase
$ goto Q_LOOP_1A
$ Q_1A_ON:
$ qualifier = "ON=" + (phrase - " on ")
$ goto Q_1A_QUAL
$ Q_1A_FORM:
$ phrase = phrase - " mounted form "
$ if f$locate(" (stock=",phrase) .ne. f$length(phrase) then -
  phrase = f$element(0," ",phrase)
$ qualifier = "FORM_MOUNTED=" + phrase
$ Q_1A_QUAL:
$ q_len = f$length(qualifier)
$ if (queue_line .eqs. "   " ) .or. -
    (f$length(queue_line) + q_len .le. 77) then -
  goto Q_1A_ADD_QUAL
$ write 'q_out' queue_line,"-"
$ queue_line =  "   " !Indent continuation lines
$ Q_1A_ADD_QUAL:
$ queue_line = queue_line + "/" + qualifier
$ goto Q_LOOP_1A
$ Q_END_1A:
$! Queue descriptions look like the following:
$!         10        20        30        40        50        60        70
$!0123456789.123456789.123456789.123456789.123456789.123456789.123456789.1234567
$!Batch queue SYS$BATCH, on NODNAM::
$!<This is a queue description line.>
$!    /BASE_PRIORITY=3 /JOB_LIMIT=5 /OWNER=[SYS,SYSTEM] /PROTECTION=(S:E,O:D,G:R,W:W)
$!
$! Loop to pick up the rest of the INIT/QUEUE qualifiers
$ queue_qual = ""
$ Q_LOOP_1B:
$ read/end=Q_1B_1 queue record
$ list_record = list_record + 1
$ if record .eqs. "" then goto Q_1B_1 !Blank line -->
$ if f$extract(0,ace_blank_cnt+12,record) .eqs. -
  ace_blanks+"(IDENTIFIER=" then -
  goto Q_1B_1 !There's an ACL on the queue --> stop there
$ if f$extract(0,1,record) .eqs. "<" .and. -
    f$extract(f$length(record)-1,1,record) .eqs. ">" then -
  goto Q_1B_0 !Queue description line -->
$ queue_qual = queue_qual + f$edit(record,"TRIM") !Add line of qualifiers
$ goto Q_LOOP_1B
$ Q_1B_0: !Parse a queue description line
$ qualifier = "DESCRIPTION=""" + f$extract(1,f$length(record)-2,record) + """"
$ q_len = f$length(qualifier)
$ if (queue_line .eqs. "   " ) .or. -
    (f$length(queue_line) + q_len .le. 77) then -
  goto Q_1B0_ADD_QUAL
$ write 'q_out' queue_line,"-"
$ queue_line =  "   " !Indent continuation lines
$ Q_1B0_ADD_QUAL:
$ queue_line = queue_line + "/" + qualifier
$ goto Q_LOOP_1B !Now, go parse the queue qualifier lines -->
$ Q_1B_1:
$ i = 1
$! Loop to check each of the INIT/QUEUE qualifiers
$ Q_LOOP_1C:
$ qualifier = f$edit(f$element(i,"/",queue_qual),"TRIM")
$ if qualifier .eqs. "/" then goto Q_END_1B
$ i = i + 1
$ q_len = f$length(qualifier)
$ if f$locate(" ",qualifier) .ne. q_len then goto Q_1C_QUAL_SPC
$Q_1C_QUAL_NOSPC:
$! If we find a /GENERIC qualifier, remove /GENERIC if already in 'queue_line'
$ if f$extract(0,7,qualifier) .eqs. "GENERIC" then -
  queue_line = queue_line - "/GENERIC"
$ if f$extract(0,7,qualifier) .eqs. "DEFAULT" then goto Q_CHK_NOFEED
$ Q_CHK_NOFEED_DONE:
$ if (queue_line .eqs. "   " ) .or. -
    (f$length(queue_line) + q_len .le. 77) then -
  goto Q_1C_ADD_QUAL
$ write 'q_out' queue_line,"-"
$ queue_line =  "   " !Indent continuation lines
$ Q_1C_ADD_QUAL:
$ queue_line = queue_line + "/" + qualifier
$ goto Q_LOOP_1C
$!
$ Q_1C_QUAL_SPC:
$! Found a space embedded in a qualifier.  Look for special cases of
$! "Lowercase" or "(stock=xxx)" in the middle of things, and pluck them out.
$ if f$locate(" Lowercase",qualifier) .ne. q_len then goto CQ_LOWER
$ Q_1C_CHK_STOCK:
$ t1 = f$locate(" (stock=",qualifier)
$ if t1 .ne. q_len then goto CQ_STOCK
$ goto Q_1C_QUAL_NOSPC
$!
$ CQ_LOWER: !Remove the "Lowercase" phrase
$ qualifier = qualifier - " Lowercase"
$ q_len = f$length(qualifier)
$ goto Q_1C_CHK_STOCK
$!
$ CQ_STOCK:
$!   Example:                           v-T1        T2-v v-Q_LEN
$!                   10        20        30        40        50        60        70
$!          0123456789.123456789.123456789.123456789.123456789.123456789.123456789.1234567
$!          /DEFAULT=(FEED,FORM=PORTRAIT (stock=DEFAULT))
$ t2 = f$locate(")",qualifier) + 1 !Just past "(stock=xxxx)"
$ qualifier = f$extract(0,t1,qualifier) + -
      f$extract(t2,(q_len-t2),qualifier)
$ q_len = f$length(qualifier)
$ goto Q_1C_QUAL_NOSPC
$!
$ Q_CHK_NOFEED:
$ q_nofeed = "FALSE" !Won't need to add /DEFAULT=(NOFEED) at end
$! If we find FEED already in there, no action is needed; otherwise we
$! need to add ,NOFEED to the end of the existing /DEFAULT qualifier list
$ if f$locate("(FEED,",qualifier) .ne. f$length(qualifier) then -
  goto Q_CHK_NOFEED_DONE
$ if f$locate(",FEED,",qualifier) .ne. f$length(qualifier) then -
  goto Q_CHK_NOFEED_DONE
$ qualifier = qualifier - ")" + ",NOFEED)"
$ goto Q_CHK_NOFEED_DONE
$ Q_END_1B:
$ if .not. q_nofeed then goto Q_NOFEED2
$ if (queue_line .eqs. "   " ) .or. -
    (f$length(queue_line) + 16 .le. 77) then -
  goto Q_NOFEED1
$ write 'q_out' queue_line,"-"
$ queue_line = "   " !Indent continuation lines
$ Q_NOFEED1:
$ queue_line = queue_line + "/DEFAULT=(NOFEED)"
$ Q_NOFEED2:
$ if f$length(queue_line) + f$length(queue_name) .le. 77 then goto Q_E1B
$ write 'q_out' queue_line,"-"
$ queue_line = "    "
$ Q_E1B:
$ write 'q_out' queue_line," ",queue_name
$! Set an ACL on the queue if there was one found
$ if f$extract(0,ace_blank_cnt+12,record) .nes. -
  ace_blanks+"(IDENTIFIER=" then goto Q_END_2
$ queue_acl = f$edit(record,"COLLAPSE")
$ Q_1D_LOOP:
$ read/end=Q_1D_1 queue record
$ list_record = list_record + 1
$ if record .eqs. "" then goto Q_1D_1 !Blank line -->
$ if f$extract(0,ace_blank_cnt+12,record) .nes. -
  ace_blanks+"(IDENTIFIER=" then goto SHOW_ERROR
$ queue_acl = queue_acl + f$edit(record,"COLLAPSE") !Add line of aces
$ goto Q_1D_LOOP
$ Q_1D_1:
$ i = 1
$ acl_line = "$SET ACL/OBJECT_TYPE=QUEUE/NEW/ACL=("
$ delimiter = "("  !Don't need a comma before the first ACE
$! Loop for each of the ACEs in the queue ACL
$ Q_LOOP_1D_1:
$ ace = f$edit(f$element(i,"(",queue_acl),"TRIM")
$ if ace .eqs. "(" then goto Q_END_1D
$ i = i + 1
$ a_len = f$length(ace)
$ if (acl_line .eqs. "   " ) .or. -
    (f$length(acl_line) + f$length(delimiter) + a_len .le. 77) then -
  goto Q_1D_1_ADD_QUAL
$ write 'q_out' acl_line,"-"
$ acl_line =  "   " !Indent continuation lines
$ Q_1D_1_ADD_QUAL:
$ acl_line = acl_line + delimiter + ace
$ delimiter = ",("  !Put a comma between ACEs
$ goto Q_LOOP_1D_1
$ Q_END_1D:
$ if f$length(acl_line) + 1 .le. 77 then goto Q_E1D1
$ write 'q_out' acl_line,"-"
$ acl_line = "    "
$ Q_E1D1:
$ acl_line = acl_line + ")" !Close the ACE list
$ if f$length(acl_line) + f$length(queue_name) .le. 77 then goto Q_E1D2
$ write 'q_out' acl_line,"-"
$ acl_line = "    "
$ Q_E1D2:
$ write 'q_out' acl_line," ",queue_name
$ Q_END_2:
$! Create an ASSIGN/QUEUE command if the queue was a logical queue
$ if logical_queue .eqs. "" then goto Q_E2_1
$ write start "$WRITE SYS$OUTPUT ""Assigning logical queue ",-
       queue_name," to ",logical_queue,""""
$ write start "$ASSIGN/QUEUE ",logical_queue," ",queue_name
$ Q_E2_1:
$! Create a START/QUEUE command if the queue was in a restartable state
$ comment = ""
$ if queue_start .nes. "/START" then comment = "!"
$ write start "$"+comment+"WRITE SYS$OUTPUT ""Starting queue ",queue_name,""""
$ write start "$"+comment+"START/QUEUE ",queue_name
$! Here, we'll either see a Jobname header line, a new queue name, or
$! an end-of-file.
$ read/end=Q_EOF queue record
$ list_record = list_record + 1
$ if f$extract(0,1,record) .nes. " " then goto Q_LOOP_1 !Next queue -->
$! Must be the header for jobs in this queue
$ if f$extract(0,job_hdr_len,record) .nes. job_hdr then goto SHOW_ERROR
$ read/end=SHOW_ERROR queue record
$ list_record = list_record + 1
$! Check the 2nd line of the header (with all the dashes for underlines)
$ if f$extract(0,job_hdr2_len,record) .nes. job_hdr2 then goto SHOW_ERROR
$ parsing_job = "TRUE" !Now parsing a job within the queue
$! Figure out whether we'll be PRINTing or SUBMITting any jobs we find
$ batch_queue = "FALSE"
$ if f$locate("BATCH",queue_type) .ne. qt_len then batch_queue = "TRUE"
$ print_submit = "PRINT"
$ if batch_queue then print_submit = "SUBMIT"
$! First line of job description
$ read/end=SHOW_ERROR queue record
$ list_record = list_record + 1
$!          10        20        30        40        50        60        70
$!0123456789.123456789.123456789.123456789.123456789.123456789.123456789.1234567
$!123456789.123456789.123456789.123456789
$!  Jobname         Username     Entry  Blocks  Status
$!  -------         --------     -----  ------  ------
$!  PHONE           J_OHARA        831       3  Pending (queue stopped)
$!  TMP             I_UPCHUCK      799      14  Retained on error
$!  DMAIL           U_KILLER       671       7  Holding until 31-MAR-1989 14:41
$!  XMAIL           M_TEKTRONIX    580      14  Processing
$!  Jobname         Username     Entry          Status
$!  -------         --------     -----          ------
$!  HEGBERT_UPDATE_NOTES
$!                  HEGBERT        202          Holding until  1-APR-1989 01:00
$! For V5.5:
$!          10        20        30        40        50        60        70
$!0123456789.123456789.123456789.123456789.123456789.123456789.123456789.1234567
$!123456789.123456789.123456789.123456789
$!  Entry  Jobname         Username     Blocks  Status
$!  -----  -------         --------     ------  ------
$!    488  QUEUE           PARRIS          160  Holding
$!    490  FIXQUE_QUEUE    PARRIS           89  Holding
$!    786  PUDRIVER        DAVIDSON       1875  Retained on error
$!    486  NMAIL           BICKEN           32  Holding until 18-OCT-1991 13:58
$!  Entry  Jobname         Username             Status
$!  -----  -------         --------             ------
$!    466  FTSV_1878       BRAVES               Executing
$!    476  CLUSTER_ANALYSIS
$!                         KELLER               Pending (queue stopped)
$ Q_LOOP_2:
$ 'if_v55 job_entry = f$edit(f$extract(1,6,record),"TRIM")
$ 'if_v55 if f$type(job_entry) .nes. "INTEGER" then goto SHOW_ERROR
$ 'if_not_v55 job_name = f$edit(f$extract(2,15,record),"TRIM")
$ 'if_v55 job_name = f$edit(f$extract(9,15,record),"TRIM")
$ if job_name .eqs. "" then job_name = " "  !Put a blank in job name if null
$ if f$extract(44,2,record) .eqs. "  " then goto Q_2_2LINE_1 !No cont. line -->
$! The job name was too long to fit inside the usual columns.  Grab the whole
$! job name, and then read the continuation line which follows, which will
$! contain the rest of the information for this job.
$ 'if_not_v55 job_name = f$extract(2,f$length(record)-2,record)
$ 'if_v55 job_name = f$extract(9,f$length(record)-9,record)
$ read/end=SHOW_ERROR queue record
$ list_record = list_record + 1
$ if f$extract(0,cont_blank_cnt,record) .nes. cont_blanks -
  then goto SHOW_ERROR !Not a continuation line --> out of sync
$ Q_2_2LINE_1:
$ if f$locate("/",job_name) .ne. f$length(job_name) then -
  goto SHOW_ERROR  !Found a slash -- must be out of sync
$ job_status = f$extract(46,999,record)
$ 'if_v4 if batch_queue then job_status = f$extract(38,999,record)
$ 'if_not_v55 job_user = f$edit(f$extract(18,12,record),"TRIM")
$ 'if_v55 job_user = f$edit(f$extract(25,12,record),"TRIM")
$ 'if_not_v55 job_entry = f$edit(f$extract(30,6,record),"TRIM")
$ 'if_not_v55 if f$type(job_entry) .nes. "INTEGER" then goto SHOW_ERROR
$ write sys$output "    Job ",job_name,-
      ", user ",job_user,-
      ", status ",f$edit(job_status,"COMPRESS")
$! Keep track of the queue the job should be submitted to (/RESTART=queue will
$! modify this symbol, so the job gets submitted back to the original queue)
$ original_queue_name = queue_name
$ job_line = "$" + print_submit + -
  "/NAME=""" + job_name + """"
$! If job is pending in a queue, that's fine; copy the job across
$ if f$extract(0,7,job_status) .eqs. "Pending" then goto Q_2_COPY
$! Carry across jobs with "Holding" or "Holding until" as-is
$ if f$extract(0,14,job_status) .eqs. "Holding until " then -
  goto Q_2_HOLD_UNTIL
$ if f$extract(0,7,job_status) .eqs. "Holding" then goto Q_2_HOLD
$!
$! The following are special cases, so we're careful to always tell the
$! user what we're doing so appropriate action can be taken later.
$!
$! If job is simply retained on completion, drop it on the floor
$ if f$extract(0,22,job_status) .eqs. "Retained on completion" then -
  goto Q_2_SKIP
$! If job is running or could be retried, copy across but put /HOLD on it:
$!         e.g. Processing, Executing, Printing, Aborting, etc.
$ if f$extract(0,9,job_status) .eqs. "Executing" then goto Q_2_HELD
$ if f$extract(0,8,job_status) .eqs. "Printing" then goto Q_2_HELD
$ if f$extract(0,17,job_status) .eqs. "Retained on error" then -
  goto Q_2_RETAIN_ERR
$ if f$extract(0,10,job_status) .eqs. "Processing" then goto Q_2_HELD
$ if f$extract(0,8,job_status) .eqs. "Aborting" then goto Q_2_HELD
$ if f$extract(0,8,job_status) .eqs. "Starting" then goto Q_2_HELD
$! Anything else we don't recognize, copy across with /HOLD
$ write sys$output "   Unrecognized job status"
$ goto Q_2_HELD
$ Q_2_RETAIN_ERR:
$! Skip the error status line for a Retained-on-error job:
$ read/end=SHOW_ERROR queue record
$ list_record = list_record + 1
$ if f$extract(0,error_blank_cnt,record) .nes. error_blanks then -
  goto SHOW_ERROR
$ if f$extract(error_blank_cnt,1,record) .nes. "%" then goto SHOW_ERROR
$ Q_2_HELD: !Let user know job will be held
$ write sys$output "    ...will be placed in queue with /HOLD"
$ Q_2_HOLD: !Hold job, don't have to tell user
$ qualifier = "HOLD"
$ goto Q_2_1
$ Q_2_HOLD_UNTIL:
$!                                              0123456789.123456789.123456789.
$!                                                            123456789.1234567
$!  NMAIL           N_SILICA       671       7  Holding until 31-MAR-1989 14:41
$ time = f$edit(f$extract(14,11,job_status),"TRIM") + -
  ":" + f$extract(26,5,job_status)
$ qualifier = "AFTER=" + time
$ Q_2_1:
$ if f$length(job_line) + f$length(qualifier) .le. 77 then goto Q_2_3
$ write jobs job_line,"-"
$ job_line = "   " !Indent continuation lines
$ Q_2_3:
$ job_line = job_line + "/" + qualifier
$ Q_2_COPY: !No doubt about it, create the job in the new queue file
$ qualifier = "USER=" + job_user
$ if f$length(job_line) + f$length(qualifier) .le. 77 then goto Q_2_0
$ write jobs job_line,"-"
$ job_line = "   " !Indent continuation lines
$ Q_2_0:
$ job_line = job_line + "/" + qualifier
$!
$! Handle the "Submitted" line and job qualifiers on continuation lines
$!    Submitted 31-MAR-1989 08:00 /KEEP /NOLOG /NOTIFY /PARAM=("TOMORROW+08:00:00","KGB$ROOT:[MAIL]NOTES$NOTEBOOK","SYS$BATCH") 
$!    /NOPRINT /PRIORITY=100 /RESTART=NODNAM_SYS$BATCH
$ Q_3_0:
$ read/end=Q_END_3 queue record
$ list_record = list_record + 1
$! Skip additional error message lines for retained-on-error jobs
$ if f$extract(0,error_blank_cnt+1,record) .eqs. error_blanks+"-" then -
  goto Q_3_0
$ if f$extract(0,leading_blank_cnt,record) .nes. leading_blanks then -
  goto SHOW_ERROR
$ if f$extract(leading_blank_cnt,9,record) .nes. "Submitted" then goto SHOW_ERROR
$! Found a "Submitted" line
$ rec_len = f$length(record)
$ t = f$locate("/",record)
$ if t .eq. rec_len then t = 0
$ job_qual = f$edit(f$extract(t,rec_len-t,record),"TRIM") !1st line of qualifiers
$ Q_LOOP_3: !Loop to pick up the rest of the job qualifiers (this avoids
$!  !problems when a parameter is broken over successive lines
$ read/end=Q_END_3 queue record
$ list_record = list_record + 1
$! Keep picking up qualifier continuation lines until we hit a filename line.
$ if f$extract(0,leading_blank_cnt,record) .nes. leading_blanks then -
  goto SHOW_ERROR
$ if f$extract(leading_blank_cnt,file_prefix_len,record) -
  .eqs. file_prefix then goto Q_END_3
$! If this wasn't a file line, then after the blanks, there should be a
$! non-blank character (continuation lines of more qualifiers for the last file)
$ if f$extract(leading_blank_cnt,1,record) .eqs. " " then goto SHOW_ERROR
$ job_qual = job_qual + f$edit(record,"TRIM") !Add line of qualifiers
$ goto Q_LOOP_3
$ Q_END_3:
$ i = 1
$! Loop to check each of the job qualifiers
$ Q_LOOP_3A:
$ qualifier = f$edit(f$element(i,"/",job_qual),"TRIM")
$ if qualifier .eqs. "/" then goto Q_END_3A  !No more qualifiers -->
$ i = i + 1
$ q_len = f$length(qualifier)
$ if f$extract(0,8,qualifier) .eqs. "RESTART=" then goto Q_3A_RESTART
$ t1 = f$locate(" (stock=",qualifier)
$ if t1 .eq. q_len then goto Q_3A_QUAL_CHKD
$ t2 = f$locate(")",qualifier) + 1 !Just past "(stock=xxxx)"
$ qualifier = f$extract(0,t1,qualifier) + -
      f$extract(t2,(q_len-t2),qualifier)
$ goto Q_3A_QUAL_CHKD
$ Q_3A_RESTART:
$! /RESTART=<queue_name> means job was originally queued to <queue_name>, and
$! would normally be resubmitted to the original queue upon restart.  We'll
$! honor this and send it to the original queue.
$ original_queue_name = qualifier - "RESTART="
$ qualifier = "RESTART"
$! Running under A5.x with a 4.7 queue file, sometimes there is no queue
$! name after the /RESTART= qualifier.  In this case, ignore the "=".
$ if original_queue_name .eqs. "" then original_queue_name = queue_name
$ if original_queue_name .eqs. queue_name then goto Q_3A_QUAL_CHKD
$ write sys$output -
  "    ...has /RESTART=''original_queue_name' and will be entered"
$ write sys$output -
"       back into that queue instead of ''queue_name'"
$ Q_3A_QUAL_CHKD:
$ if (job_line .eqs. "   " ) .or. -
    (f$length(job_line) + q_len .le. 77) then -
  goto Q_3A_ADD_QUAL
$ write jobs job_line,"-"
$ job_line = "   " !Indent continuation lines
$ Q_3A_ADD_QUAL:
$ job_line = job_line + "/" + qualifier
$ goto Q_LOOP_3A
$ Q_END_3A:
$! Now that we know which queue to resubmit the job to, add /QUEUE=name
$ qualifier = "QUEUE=" + original_queue_name
$ if f$length(job_line) + f$length(qualifier) .le. 77 then goto Q_3A1
$ write jobs job_line,"-"
$ job_line = "   " !Indent continuation lines
$ Q_3A1:
$ job_line = job_line + "/" + qualifier
$ write jobs job_line," -"
$!
$! Handle File lines
$!
$!    File: _$255$DUA14:[K_PARRIS.TAPE]DRAFT3.LN3;1 (printing) /DELETE /FEED
$!    File: _$255$DUA14:[K_PARRIS.TAPE]DRAFT3_TOC.LN3;1 /DELETE /FEED
$ if f$extract(0,leading_blank_cnt,record) .nes. leading_blanks then -
  goto SHOW_ERROR
$ if f$extract(leading_blank_cnt,file_prefix_len,record) -
  .nes. file_prefix then goto SHOW_ERROR
$ Q_LOOP_4: !Loop here for each "File:" line
$! Grab the filespec from the "File:" line
$ file_line = "     " + -
  f$element(0," ",f$edit(record,"TRIM") - "File: ")
$ if f$locate("[]",file_line) .eq. f$length(file_line) then goto Q_CONT_4A
$ write sys$output -
"    ...''print_submit' command will fail; file not in directory:"
$ write sys$output " ",file_line
$ not_in_dir = "TRUE" !Remember that we hit one of these
$ goto Q_CONT_4A
$ Q_LOOP_4A: !Loop here for each continuation line after a "File:" line
$ read/end=Q_END_4 queue record
$ list_record = list_record + 1
$! Check for the proper number of blanks and a leading "/" for a qualifier
$! continuation line:
$ if f$extract(0,leading_blank_cnt,record) .nes. leading_blanks then -
  goto Q_END_4AR
$! If this wasn't a file line, then after the blanks, there should be a
$! non-blank character (continuation lines of more qualifiers for the last file)
$ if f$extract(leading_blank_cnt,1,record) .eqs. " " then goto Q_END_4AR
$ Q_CONT_4A:
$ i = 1
$ Q_LOOP_4B: !Loop here to add each file qualifier to symbol 'file_line'
$ qualifier = f$edit(f$element(i,"/",record),"TRIM")
$ if qualifier .eqs. "/" then goto Q_LOOP_4A
$ i = i + 1
$ if (file_line .eqs. "   " ) .or. -
    (f$length(file_line) + f$length(qualifier) .le. 77) then -
  goto Q_4B_ADD_QUAL
$ write jobs file_line,"-"
$ file_line = "   " !Indent continuation lines
$ Q_4B_ADD_QUAL:
$ file_line = file_line + "/" + qualifier
$ goto Q_LOOP_4A
$ Q_END_4AR: !Already have the next record
$ if f$extract(0,leading_blank_cnt,record) .nes. leading_blanks then -
  goto Q_END_4AR_1 !Can't be a file line -->
$! Check for the file prefix; if it's there, this is a file line -->
$ if f$extract(leading_blank_cnt,file_prefix_len,record) -
  .eqs. file_prefix then goto Q_4_NXTFIL
$ Q_END_4AR_1:
$! Line is not another "File:" line, so we're done with this job
$ write jobs file_line !Write the rest of the file qualifiers
$ if record .nes. "" then goto SHOW_ERROR !No blank line at end of job -->
$ read/end=SHOW_ERROR queue record !EOF -> no data after blank line
$ list_record = list_record + 1
$ if f$extract(0,1,record) .nes. " " then goto Q_LOOP_1  !Next queue -->
$ goto Q_LOOP_2  !Go handle the next job in same queue -->
$ Q_4_NXTFIL: !There's another file to be added to the existing job
$ write jobs file_line,",-" !Trailing comma before next job
$ goto Q_LOOP_4  !Go handle the "File:" line -->
$!
$ Q_2_SKIP: !Skip over this job and continue with the next job
$ write sys$output "    ...will be dropped."
$ Q_2_SKIP_1:
$ read/end=Q_EOF queue record
$ list_record = list_record + 1
$ if record .eqs. "" then goto Q_2_SKIP_LOOP !Blank line -->
$! Continuation lines for a job start with four blanks, followed by
$! a slash, Submitted:, File:, etc.  Job names are only preceded by 2 blanks.
$! This probably erroeously skips over a job with a blank name field for pre-5.5:
$ if f$extract(0,leading_blank_cnt,record) .eqs. leading_blanks then -
  goto Q_2_SKIP_LOOP
$! Could be the error message line(s) for a Retained-on-error job:
$! None of the above; must have found either the next queue or next jobname
$ if f$extract(0,1,record) .nes. " " then goto Q_LOOP_1  !Next queue -->
$ goto Q_LOOP_2 !Found start of next job -->
$ Q_2_SKIP_LOOP:
$ goto Q_2_SKIP_1
$!
$ Q_END_4: !End of input file reached while info for a particular job
$ write jobs file_line !Write out the remaining info
$ Q_EOF:
$ close queue
$ write sys$output "Parsing done."
$ close com
$ write generic "$DONE_QUEUES:"
$ close generic
$ append FIXQUE_QUEUE_GENERIC.TEMP FIXQUE_RELOAD.COM
$ write jobs "$DONE_JOBS:"
$ write jobs "$ASK_START:"
$ write jobs "$INQUIRE ANS ""Do you wish to start queues and assign logical queues? [Y]"""
$ write jobs "$ANS=F$EXTRACT(0,1,F$EDIT(ANS,""TRIM,UPCASE""))"
$ write jobs "$IF ANS .EQS. ""N"" THEN GOTO DONE_START"
$ write jobs "$IF ANS .NES. """" .AND. ANS .NES. ""Y"" THEN GOTO ASK_START"
$ write sys$output "Generating START/QUEUE commands"
$! write jobs "$WRITE SYS$OUTPUT ""Starting queues"""
$ close jobs
$ append FIXQUE_QUEUE_JOBS.TEMP FIXQUE_RELOAD.COM
$ write start "$DONE_START:"
$ write start "$WRITE SYS$OUTPUT ""FIXQUE_RELOAD done."""
$ close start
$ append FIXQUE_QUEUE_START.TEMP FIXQUE_RELOAD.COM
$ if .not. rerun then delete/nolog -
    FIXQUE_QUEUE_GENERIC.TEMP;*,-
    FIXQUE_QUEUE_JOBS.TEMP;*,-
    FIXQUE_QUEUE_START.TEMP;*
$ Q_DONE:
$!
$ CLEANUP:
$ close/nolog chars
$ close/nolog forms
$ close/nolog queue
$ close/nolog generic
$ close/nolog jobs
$ close/nolog start
$ close/nolog com
$ if .not. got_error then goto CLEANUP_1
$ write sys$output "***"
$ write sys$output -
"*** Parsing errors were encountered.  The output command procedure"
$ write sys$output -
"*** FIXQUE_RELOAD.COM is incomplete and will need manual editing."
$ write sys$output "***"
$ CLEANUP_1:
$ if .not. not_in_dir then goto CLEANUP_2
$ write sys$output "***"
$ write sys$output -
"*** Job(s) were found which contain files which are not entered in a"
$ write sys$output -
"*** directory.  These could be files created by copying to a spooled"
$ write sys$output -
"*** device.  A job containing such a file cannot be requeued through DCL."
$ write sys$output -
"*** Note: The workaround is to do an ANALYZE/DISK/REPAIR on the"
$ write sys$output -
"***       disk(s) involved to place all such files into the [SYSLOST]"
$ write sys$output -
"***       directory.  Then this procedure must be run again."
$ write sys$output "***"
$ CLEANUP_2:
$ write sys$output "FIXQUE done."
$ write sys$output "Check FIXQUE_RELOAD.COM for accuracy before use."
$ if .not. rerun then purge/nolog fixque_CHARS.list
$ if .not. rerun then purge/nolog fixque_FORMS.list
$ if .not. rerun then purge/nolog fixque_QUEUE.list
$ if .not. rerun then purge/nolog FIXQUE_RELOAD.COM
$ if old_priv .nes. "" then old_priv = f$setprv(old_priv) !Reset privileges
$ exit
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$!
$! Handle unexpected errors
$!
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$ SHOW_ERROR:
$ got_error = "TRUE" !Remember that we got an error
$ write sys$output "FIXQUE encountered an error in parsing"
$ write sys$output "   input file ",list_file
$ write sys$output "   at line ",list_record,", which contained:"
$ write sys$output "   """,record,""""
$! Handle the error differently depending on which type of info we were parsing
$ if list_file .eqs. "FIXQUE_QUEUE.LIST" then goto SE_Q !Queues/Jobs
$ write com "$WRITE SYS$OUTPUT ",-
"""An error in parsing was encountered by FIXQUE.COM at this point..."""
$ if list_file .eqs. "FIXQUE_CHARS.LIST" then goto C_LOOP_1 !Characteristics
$!
$ SF_SKIP: !Skip over this form definition and any qualifiers
$  !and continue with the next form definition
$ if record .eqs. "" then goto F_LOOP_1 !Blank line -->
$ SF_SKIP_LOOP:
$ read/end=F_EOF forms record
$ list_record = list_record + 1
$ if record .eqs. "" then goto F_LOOP_1   !Parse the next form definition -->
$ if f$extract(0,1,record) .nes. " " then goto F_CONT_1 !Next form def.
$ goto SF_SKIP_LOOP !Look for a blank line -->
$!
$ SE_Q: !Decide which file to put the error notification into
$ t = q_out !Either "COM" or "GENERIC"
$ if parsing_job then t = "jobs"
$ write 't' "$WRITE SYS$OUTPUT ",-
"""An error in parsing was encountered by FIXQUE.COM at this point..."""
$ if parsing_job then goto Q_2_SKIP_1 !Skip to the next job
$!
$ SE_SKIP: !Skip over this queue and any jobs and continue with the next queue
$ read/end=Q_EOF queue record
$ list_record = list_record + 1
$ if record .eqs. "" then goto SE_SKIP_LOOP !Blank line -->
$ if f$extract(0,1,record) .nes. " " then goto Q_LOOP_1  !Next queue -->
$ SE_SKIP_LOOP:
$ goto SE_SKIP
