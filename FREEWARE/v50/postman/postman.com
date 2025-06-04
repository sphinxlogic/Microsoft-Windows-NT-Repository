$ verify = 'f$verify (0)'
$!******************************************************************************
$!  SYS_SYSTEM:POSTMAN.COM	- This *SITE-specific* name is used below.
$!
$!  Handle various aspects of mail delivery.
$!
$!  *** This procedure *REQUIRES* Innosoft's PMDF DELIVER !!! ***
$!
$!  Currently, the procedure supports the establishment, changing
$!  and removal of:
$!	- a vacation message
$!	- multiple forwards
$!	- multiple reject and accept filters
$!	- accept filters which deliver to a folder
$!
$!  This procedure is also called as the vacation notification procedure,
$!  delivering a specified text file to the sender of received mail.
$!  In that case, the following assumptions apply:
$!    - the file specified exists and is a valid text file;
$!    - the user has an appropriate MAIL.DELIVERY file;
$!    - the user's mail is forwarded to IN%"~username".
$!
$!  This procedure is also called to log rejected message information.
$!
$!  *** NOTE ***  This procedure's file PROTECTION should be EXECUTE ONLY
$!		  for "normal" users, so that TRACE doesn't work for them.
$!
$!  Invocation:
$!    $ POSTMAN :== @SYS_SYSTEM:POSTMAN	! ... or whatever the procedure's called
$!    $ POSTMAN				! "User mode" - normal user use.
$!    $ POSTMAN BRIEF			! Avoid lengthy explanatory text.
$!    $ POSTMAN DELETE_ON_ERROR		! Delete message files if DELIVER bombs.
$!    $ POSTMAN DEBUG [filename]	! Debug DELIVER, alternate logfile name.
$!    $ POSTMAN FOLDERS			! Enable folders.
$!    $ POSTMAN TRACE			! Verify this procedure - see NOTE above
$!    $ POSTMAN ALOHA  filename		! Vacation message delivery.
$!    $ POSTMAN SIEVE			! Log rejected message information.
$!
$!  Note that, with the exception of ALOHA and SIEVE, multiple parameters may
$!  be specified in any order.  If DEBUG is specified with an empty file
$!  specification, it *must* either be the last parameter or it must be followed
$!  by "".
$!..............................................................................
$!  Author:	Jonathan Ridler,
$!		Information Technology Services,
$!		The University of Melbourne,
$!		Victoria, Australia.
$!
$!  Email:	jonathan@unimelb.edu.au
$!
$!  (c) The University of Melbourne 1999
$!
$!  This procedure may be freely distributed and modified, provided that this
$!  copyright information remains intact.  The procedure is freeware and may
$!  not be sold under any circumstances, whether in its original form or
$!  modified.
$!..............................................................................
$!  History:
$! v1.0	07-Oct-1998	JER	Original version.
$! v1.1	12-Oct-1998	JER	Allow logging of rejected messages.
$! v1.2	14-Oct-1998	JER	Checks for non-Postman generated file.
$! v1.3	15-Oct-1998	JER	For a single forward (ONLY), use MAIL.
$! v1.4	30-Oct-1998	JER	Allow multiple parameters to this procedure, and
$!				message files on error to stay or be deleted.
$! v1.5	04-Nov-1998	JER	Cater for forward with IN% and no quotes at all.
$! v2.0	23-Nov-1998	JER	Implement Reject, Accept, Folder and To filters.
$! v2.1	25-Nov-1998	JER	Minor enhancements.
$! v2.2	22-Dec-1998	JER	Minor fixes to vacation handling.
$! v2.3	10-Feb-1999	JER	Implement vacation sender db and overall confirm
$! v2.4	17-Feb-1999	JER	Allow accept-filtered messages to be fully
$!				processed, and various minor fixes and cleanups.
$! v2.5	11-Mar-1999	JER	Avoid sending vacation message to mailing lists.
$! v2.6	30-Mar-1999	JER	Add precedence "junk" to "bulk" and "list".
$! v2.7	24-Jun-1999	JER	Implement BRIEF option.
$! v2.8	02-Jul-1999	JER	More checks for mailing lists.
$! v2.9	06-Oct-1999	JER	Avoid external symbol definitions except PMDF's.
$!******************************************************************************
$
$ on warning    then  goto  END
$ on control_y  then  goto  END
$
$!  Avoid external symbol definitions ...
$
$ set := set
$ set symbol /scope=(nolocal,noglobal)
$
$ say 	:= write SYS$OUTPUT
$ ask	:= read  SYS$COMMAND /error=END /end=END /prompt=
$ put   := write deliver /error=ERROR_EXIT /end=ERROR_EXIT
$ putt  := write tmpfyl  /error=ERROR_EXIT /end=ERROR_EXIT
$ blurb := type  SYS$INPUT
$ z	:= type /page NLA0:
$ hold  := 'ask' """Press RETURN to continue ... """ ans
$
$ version = "2.9 (06-Oct-1999)"  ! Keep the VERSION information up-to-date!!!
$ divider = "---------------------------------------------------------------"
$
$ bell[0,8]  = %X07
$ ss$_normal = 1
$
$!  No PMDF?  No can do!!!
$
$ if f$trnlnm ("PMDF_ROOT") .eqs. ""
$ then
$   say ""
$   say "*** This utility *REQUIRES* Innosoft's PMDF DELIVER !!! ***",bell,bell
$   say ""
$   commit = 1		! Fudge it.
$   goto  END		! Escape!
$ endif
$
$ pid  = f$getjpi ("","PID")
$ user = f$edit (f$getjpi ("","USERNAME"),"COLLAPSE")
$ node = f$getsyi ("NODENAME")
$ vms_version  = f$getsyi ("VERSION")
$ pmdf_version = f$trnlnm ("PMDF_VERSION")
$
$ tmp := SYS$SCRATCH:POSTMAN_'pid'.TMP
$ postie := SYS_SYSTEM:POSTMAN		! SITE-specific name of this procedure !
$ reject_log  := SYS$LOGIN:MAIL.REJECT_LOG
$ vac_db_file := SYS$LOGIN:VACATION.RECEIPT_DB
$
$ options = "|ALOHA|BRIEF|DEBUG|DELETE_ON_ERROR|FOLDERS|SIEVE|TRACE|"
$
$ forward  = ""
$ actor    = ""
$ debugger = "DELIVER.LOG"	! Must be only filename - no directory, etc.
$ vacating = 0
$ delivering = 0
$ forwarding = 0
$ deactivate = 0
$ use_vac_db = 0
$ folders = 0
$ tilded  = 0
$ active  = 0
$ fwcnt   = 0
$ debug   = 0
$ commit  = 0
$ full    = 1
$ diskver = "???"
$ log_rejects   = 0
$ need_vacation = 0
$ delete_files  = 0
$ reject_filter_cnt = 0
$ accept_filter_cnt = 0
$
$ retain_mail = 1		! Default is to deliver to self.
$
$CHECK_PARAMS:
$ p1 = f$edit (p1,"UPCASE,COLLAPSE,UNCOMMENT")
$ if p1 .eqs. ""  then  goto  FIRE_UP
$
$ if f$locate ("|"+p1+"|",options) .eqs. f$length (options)
$ then
$   say "*** Invalid option: ''p1' ***",bell
$   commit = 1		! Fudge it.
$   goto  END
$ endif
$
$ if p1 .eqs. "TRACE"  then  set verify		! Watch what's happening here.
$
$ if p1 .eqs. "FOLDERS"  then  folders = 1	! Enable folders.
$
$ if p1 .eqs. "BRIEF"  then  full = 0		! Don't ramble on.
$
$ if p1 .eqs. "ALOHA"				! Take me away P&O !!!
$ then
$   set symbol /scope=(nolocal,global)
$   send_msg = 1
$   sender = f$edit (f$element (0,"""",from-"IN%"""),"lowercase")
$   if vac_db	! Global symbol set by DELIVER.
$   then
$     if f$search (vac_db_file) .eqs. ""  then  create 'vac_db_file'
$     search 'vac_db_file' "''sender'" /nooutput /nowarning
$     if $status .eq. ss$_normal
$     then
$	send_msg = 0
$     else
$	close /nolog vdb
$	open /append vdb 'vac_db_file' /error=ERROR_EXIT
$	write vdb /error=ERROR_EXIT sender
$	close /nolog vdb
$     endif
$   endif
$
$   if send_msg
$   then
$     if f$extract (0,3,f$edit(subject,"COLLAPSE,UPCASE")) .nes. "RE:"  then -
	subject = "Re: " + subject
$     mail 'p2' "''sender'" /subject="''subject'" /noself
$   endif
$   commit = 1		! Fudge it.
$   set symbol /scope=(nolocal,noglobal)
$   goto  END		! Escape!
$ endif
$
$ if p1 .eqs. "SIEVE"				! Log the filtered message.
$ then
$   if f$search (reject_log) .eqs. ""  then  create 'reject_log'
$   close /nolog tmpfyl
$   open /append /error=ERROR_EXIT /end=ERROR_EXIT tmpfyl 'reject_log'
$   set symbol /scope=(nolocal,global)
$   putt ""
$   putt "Date:	", f$cvtime()
$   putt "From:	", f$element(0,"""",from-"IN%""")
$   putt "To:	", f$element(0,"""",to-"IN%""")
$   putt "Subj:	", subject
$   set symbol /scope=(nolocal,noglobal)
$   close /nolog tmpfyl
$   commit = 1		! Fudge it.
$   goto  END		! Escape!
$ endif
$
$ if p1 .eqs. "DEBUG"				! Watch what DELIVER does.
$ then
$   debug = 1
$   p2 = f$edit (p2,"UPCASE,COLLAPSE,UNCOMMENT")
$   if p2 .nes. ""  then  debugger = p2		! Either DEBUG must be the last
$   gosub SHUFFLE_PARAMS			!  or P2 *must* exist or be ""
$ endif
$
$ if p1 .eqs. "DELETE_ON_ERROR"  then -	! Delete message files if DELIVER bombs?
	delete_files = 1
$
$ gosub SHUFFLE_PARAMS
$ goto  CHECK_PARAMS
$
$!..............................................................................
$FIRE_UP:
$ say ""
$ say "Postman - Version ''version'"
$ blurb

Postman allows you to manage incoming electronic mail.
You can establish, change or remove:

  - a vacation message
  - multiple forwarding addresses
  - multiple filters to accept or discard messages based on
    "From", "To" and "Subject" lines
$
$ if folders  then  say "  - automatically file messages to a given mail folder"
$ blurb

               ***    Postman uses PMDF's DELIVER system.   ***
               *** If you wish to control DELIVER yourself, ***
               ***    you should NOT use this utility.      ***

Enter Control-Z or Control-C or Control-Y at any time to abort.
("Control" is the "Ctrl" key - use it like "Shift" with "Z", "C" or "Y")

$ hold
$ say ""
$ say divider
$ say "Checking your current configuration ..."
$
$!..............................................................................
$!  Check for a forward, and if it exists, check to see if it's for DELIVER.
$
$ if f$search (tmp) .nes. ""  then  delete /nolog 'tmp';*
$
$ define /user sys$output 'tmp'
$ mail
show forward
exit
$
$ search 'tmp';-1 forward /output='tmp'
$ close /nolog tmpfyl
$ open /read /error=ERROR_EXIT /end=ERROR_EXIT tmpfyl 'tmp'
$ read /error=ERROR_EXIT /end=ERROR_EXIT tmpfyl rec
$ close /nolog tmpfyl
$ delete /nolog 'tmp';*
$
$ forwarding = rec .nes. "You have not set a forwarding address."
$ if forwarding
$ then
$   forward = f$edit (f$extract (32,f$length(rec)-33,rec),"UPCASE")
$   forward = forward-"IN%"-""""-""""-""""-""""-""""-"""" ! Remove any PMDF junk
$   forward = f$edit (forward,"COMPRESS,TRIM,UPCASE")
$   tilded  = f$locate ("~''user'",forward) .ne. f$length (forward)
$   if .not. tilded
$   then		! Not = "~user" therefore it's a real forward.
$     fwcnt = 1
$     fw1 = forward
$   endif
$ endif
$
$ if f$search (tmp) .nes. ""  then  delete /nolog 'tmp';*
$
$!  If we already have a DELIVER file (active or inactive), check it for
$!  forwards, filters and a vacation message.
$
$ delivering = f$search ("SYS$LOGIN:MAIL.DELIVERY") .nes. ""
$ active = delivering
$ if .not. delivering  then -
	delivering = f$search ("SYS$LOGIN:MAIL.DELIVERY_INACTIVE") .nes. ""
$ if .not. active  then  actor := _INACTIVE
$
$ if delivering
$ then
$
$!  Check for a non-Postman generated MAIL.DELIVERY* file ...
$
$   set noon
$   search SYS$LOGIN:MAIL.DELIVERY'actor' /output='tmp' /nowarn " by Postman v"
$   status = $status
$   set on
$   if status .eq. ss$_normal
$   then
$     close /nolog tmpfyl
$     open /read /error=ERROR_EXIT /end=ERROR_EXIT tmpfyl 'tmp'
$     read /error=VER_DONE /end=VER_DONE tmpfyl rec
$     diskver = f$extract (f$locate (" by Postman v",rec)+13,20,rec)
$     diskver = f$element (0," ",diskver)
$VER_DONE:
$     close /nolog tmpfyl
$   else
$     say ""
$     say "** WARNING: You have a PMDF MAIL.DELIVERY''actor' file which was NOT"
$     say "** generated by Postman!  Using Postman will cause your current"
$     say "** configuration to be lost.",bell,bell,bell
$     say ""
$     ask "Do you want to continue? (Yes/No) [No] " ans
$     ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$     if ans .eqs. ""  then  ans = "No"
$     if ans
$     then
$	goto  VACATE		! Don't look for any of our stuff!
$     else
$	goto  END
$     endif
$   endif
$
$!  Forwards? ...
$
$   set noon
$   search SYS$LOGIN:MAIL.DELIVERY'actor' "* * * a p " /output='tmp' /nowarning
$   status = $status
$   set on
$   if status .eq. ss$_normal
$   then
$     close /nolog tmpfyl
$     open /read /error=ERROR_EXIT /end=ERROR_EXIT tmpfyl 'tmp'
$FW_READ:
$     read /error=FW_DONE /end=FW_DONE tmpfyl rec
$     fwcnt = fwcnt + 1
$     fw'fwcnt' = rec - "* * * a p "
$     goto  FW_READ
$FW_DONE:
$     close /nolog tmpfyl
$   endif
$
$!  Retain mail? ...
$
$   set noon
$   search SYS$LOGIN:MAIL.DELIVERY'actor' "* * * o d" /nooutput /nowarning
$   retain_mail = $status .eq. ss$_normal
$   set on
$
$   if f$search (tmp) .nes. ""  then  delete /nolog 'tmp';*
$
$!  Filters? ...
$
$!  Reject filters and logging of rejected messages ...
$
$   set noon
$   search SYS$LOGIN:MAIL.DELIVERY'actor' "''postie'  SIEVE" -
	/nooutput /nowarning
$   log_rejects = $status .eq. ss$_normal
$
$   if diskver .lts. "2.4"
$   then
$     search SYS$LOGIN:MAIL.DELIVERY'actor' " y q " /output='tmp' /nowarning
$     status = $status
$   else
$
$!  Search for PMDF v5.1+ form first, then, if none, the pre-v5.1 form ...
$
$     search SYS$LOGIN:MAIL.DELIVERY'actor' " o q " /output='tmp' /nowarning
$     status = $status
$     if status .ne. ss$_normal
$     then
$	search SYS$LOGIN:MAIL.DELIVERY'actor' " a q " /output='tmp' /nowarning
$	status = $status
$     endif
$   endif
$   set on
$
$   if status .eq. ss$_normal
$   then
$     close /nolog tmpfyl
$     open /read /error=ERROR_EXIT /end=ERROR_EXIT tmpfyl 'tmp'
$FILTER_READ1:
$     read /error=FILTER_DONE1 /end=FILTER_DONE1 tmpfyl rec
$     reject_filter_cnt = reject_filter_cnt + 1
$     reject_filter'reject_filter_cnt'_from = f$element (1,"""",rec)
$     if diskver .lts. "2.0"
$     then
$	reject_filter'reject_filter_cnt'_to   = "*"
$	reject_filter'reject_filter_cnt'_subj = f$element (3,"""",rec)
$     else
$	reject_filter'reject_filter_cnt'_to   = f$element (3,"""",rec)
$	reject_filter'reject_filter_cnt'_subj = f$element (5,"""",rec)
$     endif
$     goto  FILTER_READ1
$FILTER_DONE1:
$     close /nolog tmpfyl
$   endif
$
$!  Accept (and folder) filters ...
$
$   set noon
$   search SYS$LOGIN:MAIL.DELIVERY'actor' " y d " /output='tmp' /nowarning
$   status = $status
$   set on
$
$   if status .eq. ss$_normal
$   then
$     close /nolog tmpfyl
$     open /read /error=ERROR_EXIT /end=ERROR_EXIT tmpfyl 'tmp'
$FILTER_READ2:
$     read /error=FILTER_DONE2 /end=FILTER_DONE2 tmpfyl rec
$     accept_filter_cnt = accept_filter_cnt + 1
$     accept_filter'accept_filter_cnt'_from = f$element (1,"""",rec)
$     if diskver .lts. "2.0"
$     then
$	accept_filter'accept_filter_cnt'_to   = "*"
$	accept_filter'accept_filter_cnt'_subj = f$element (3,"""",rec)
$     else
$	accept_filter'accept_filter_cnt'_to   = f$element (3,"""",rec)
$	accept_filter'accept_filter_cnt'_subj = f$element (5,"""",rec)
$     endif
$     accept_filter'accept_filter_cnt'_dest = f$extract -
                                        (f$locate (" y d ",rec)+5,200,rec)
$     if accept_filter'accept_filter_cnt'_dest .nes. ""  then  folders = 1
$     goto  FILTER_READ2
$FILTER_DONE2:
$     close /nolog tmpfyl
$   endif
$
$   if f$search (tmp) .nes. ""  then  delete /nolog 'tmp';*
$
$!  Vacation? ...
$
$   set noon
$   search SYS$LOGIN:MAIL.DELIVERY'actor' "''postie'  ALOHA" -
	/output='tmp' /nowarning
$   vacating = $status .eq. ss$_normal
$   set on
$
$   if vacating
$   then
$     close /nolog tmpfyl
$     open /read /error=ERROR_EXIT /end=ERROR_EXIT tmpfyl 'tmp'
$     read /error=ERROR_EXIT /end=ERROR_EXIT tmpfyl rec
$     close /nolog tmpfyl
$     delete /nolog 'tmp';*
$     rec = f$edit (rec-""""-""""-""""-""""-""""-"""","COMPRESS,TRIM")
$     vacation_file = f$element (2," ",f$element (1,"@",rec)) - """"
$     if f$search (vacation_file) .eqs. ""  then  vacating = 0	! File not found
$     set noon
$     search SYS$LOGIN:MAIL.DELIVERY'actor' "vac_db == 1" /nooutput /nowarning
$     use_vac_db = $status .eq. ss$_normal
$     set on
$   endif
$ endif
$
$ say ""
$ if delivering
$ then
$   if active
$   then
$     say "Your current configuration is ACTIVE."
$   else
$     say "Your current configuration is NOT ACTIVE."
$   endif
$ else
$   if fwcnt .eq. 1
$   then
$     say "You are currently forwarding your mail."
$   else
$     say "You do not have a current configuration."
$   endif
$ endif
$
$!..............................................................................
$VACATE:
$ say ""
$ say divider
$ blurb
VACATION
--------

$ if full
$ then
$   blurb
A vacation message can be used to automatically notify a person who has
sent you mail that you are on vacation, or simply to indicate to the
sender that you have received the mail.

$ endif
$
$ if vacating
$ then
$   say "You already have a vacation message file.  Its name is:"
$   say ""
$   say "  ",vacation_file
$   say ""
$   say "It contains:"
$   say ""
$   wait 00:00:02.00		! In case a big file scrolls off screen
$   type 'vacation_file'
$   say ""
$   if active
$   then
$     notta = "ACTIVE"
$     default = "Yes"
$   else
$     notta = "NOT active"
$     default = "No"
$   endif
$   say "The vacation message is currently ",notta,"."
$   say ""
$   msg = "Do you want to have a vacation message (at all)? (Yes/No) [" + -
	  default + "] "
$   ask "''msg'" ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  ans = default
$   if ans  then  need_vacation = 1
$   if .not. need_vacation  then  goto  CHECK_FWD
$   say ""
$   ask "Do you want to continue to use this message file? (Yes/No) [Yes] " ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  ans = "Yes"
$   if ans
$   then
$     goto  CHECK_RETAIN
$   else
$     gosub  GET_MESSAGE_FILE
$   endif
$ else
$   say "You do not currently have a vacation message."
$   say ""
$   ask "Do you want to have a vacation message? (Yes/No) [No] " ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  ans = "No"
$   if ans  then  need_vacation = 1
$   if need_vacation
$   then
$     gosub  GET_MESSAGE_FILE
$   else
$     goto  CHECK_FWD
$   endif
$ endif
$
$CHECK_RETAIN:
$ if .not. retain_mail
$ then
$   if full
$   then
$     blurb

Your current configuration does NOT retain a copy of incoming mail.  This
is very unwise if you have a vacation message, since you will NOT receive
the incoming message, even though the vacation message will be sent to the
sender.  Unless you intend also to forward your messages, you are strongly
urged to retain mail when you use a vacation message.

$   endif
$
$   ask "Do you want to retain incoming mail messages? (Yes/No) [Yes] " ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  ans = "Yes"
$   retain_mail = ans
$ endif
$
$ if full
$ then
$   blurb

You can track the sender addresses of incoming messages so that each unique
sender address only receives the vacation message once during the current
activation of the vacation message.  If you do not track the sender addresses,
the vacation message is sent each and every time a message is received.
$ endif
$
$ if use_vac_db
$ then
$   default = "Yes"
$ else
$   default = "No"
$ endif
$
$ say ""
$ msg = "Do you want to send the vacation message only once per sender? " + -
	"(Yes/No) "
$ ask "''msg' [''default'] " ans
$ ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$ if ans .eqs. ""  then  ans = default
$ use_vac_db = ans
$
$!..............................................................................
$CHECK_FWD:
$ say ""
$ say divider
$ blurb
FORWARDING
----------

$ if full
$ then
$   blurb
Forwarding allows you to send a copy of a message to another person.
You may establish as many forwarding addresses as you wish - each one
will receive a copy of the message.

$ endif
$
$ if need_vacation .and. .not. retain_mail
$ then
$   blurb
*** You have a vacation message but you are NOT retaining a copy   ***
*** of incoming messages.  You are strongly urged either to retain ***
*** a copy of any incoming message, or use a forward.              ***

$ endif
$
$ if fwcnt .eq. 0
$ then
$   say "You are not forwarding your mail."
$   gosub  ADD_FORWARDS
$ else
$   if active .or. (fwcnt .eq. 1)
$   then
$     default = "Yes"
$     say "You already have the following ACTIVE forwarding addresses defined."
$   else
$     default = "No"
$     say "You already have the following forwarding addresses defined,"
$     say "BUT they are NOT currently active."
$   endif
$   if need_vacation .and. .not. retain_mail  then  default = "Yes"
$
$   say ""
$   cnt = 1
$FW_SHOW:			! Display all current forwards.
$   say fw'cnt'
$   cnt = cnt + 1
$   if cnt .le. fwcnt  then  goto  FW_SHOW
$
$   say ""
$   ask "Do you want to forward mail (at all)? (Yes/No) [''default'] " ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  ans = default
$   if .not. ans
$   then
$     fwcnt = 0
$     goto  CHECK_FILTERS
$   endif
$
$   say ""
$   ask "Do you want to change your existing forwards? (Yes/No) [No] " ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  ans = "No"
$   if .not. ans  then  goto  GET_FWD
$
$   if full
$   then
$     blurb

Indicate what you want to do with each of these forwards.
Enter one of the following (a single letter abbreviation is sufficient):

  . "Keep" to leave the current forward unchanged.
  . "Modify" to modify the current forward.
  . "Delete" to delete the current forward.
  . "Zap" to discard ALL current forwards.
  . "OK" to retain ALL forwards not yet discarded.

$   endif
$   inx = 1
$FW_CHECK:
$   fwd = fw'inx'
$   say ""
$   ask "''fwd' ? (Keep/Modify/Delete/Zap/OK) [Keep] " ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  ans = "Keep"
$   ans = f$extract (0,1,ans)
$   if ans .eqs. "M"
$   then
$     say ""
$     ask "Address? " ans
$     ans = f$edit (ans,"TRIM,UNCOMMENT")
$     if ans .eqs. ""  then  goto  FW_CHECK
$     ans = ans - "IN%" - "in%" - "In%" - "iN%" - -	! Remove any PMDF junk !
		  """" - """" - """" - """" - """" - """" - """" - """"
$     fw'inx' = ans
$     ans = "M"
$   endif
$   if ans .eqs. "D"  then  fw'inx' = "###"
$   if ans .eqs. "Z"
$   then
$     fwcnt = 0
$     say ""
$     say "All current forwards will be discarded."
$     goto  GET_FWD
$   endif
$   if ans .eqs. "O"
$   then
$     say ""
$     say "All forwards not yet discarded will be retained."
$     goto  GET_FWD
$   endif
$   if (ans .nes. "K") .and. (ans .nes. "M") .and. (ans .nes. "D")  then -
	goto  FW_CHECK
$
$   inx = inx + 1
$   if inx .le. fwcnt  then  goto  FW_CHECK
$
$GET_FWD:
$   gosub  ADD_FORWARDS
$
$!  Re-write the forward list to eliminate holes ...
$
$   inx = 0
$   cnt = 1
$FW_LOOPER:
$   if fw'cnt' .nes. "###"
$   then
$     inx = inx + 1
$     fw'inx' = fw'cnt'
$   endif
$   cnt = cnt + 1
$   if cnt .le. fwcnt  then  goto  FW_LOOPER
$   fwcnt = inx
$ endif
$
$CHECK_KEEP_COPY:
$ if fwcnt .gt. 0
$ then
$   say ""
$   if retain_mail .or. need_vacation
$   then
$     default = "Yes"
$     if full
$     then
$	blurb
You are forwarding your mail.  You also are currently retaining a copy of
incoming messages.  (If you have a vacation message, this is strongly
advisable.)  Please confirm that you wish to retain a copy of incoming
messages.
$     endif
$   else
$     default = "No"
$     say "You are forwarding your mail."
$   endif
$   if full  then  say ""
$   msg = "Do you want to retain a copy of each incoming message? " + -
	  "(Yes/No) [" + default + "] "
$   ask "''msg'" ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  ans = default
$   retain_mail = ans
$   if .not. ans
$   then
$     blurb

*** WARNING!!!  You are forwarding without retaining a copy of incoming  ***
*** messages.  This is dangerous!  You are very likely to lose messages! ***
*** If you wish to correct this, exit Postman NOW and then re-run it.    ***
$     say bell,bell
$     hold
$   endif
$ endif
$
$!..............................................................................
$CHECK_FILTERS:
$ say ""
$ say divider
$ blurb
FILTERING
---------

$ if full
$ then
$   blurb
There are two types of filters: Accept and Reject.  Both work together to
allow you to automatically select incoming messages to receive or to discard
completely as if the message had never been received.  Accept filters are
applied before reject filters.  Since reject filters cause messages to be
discarded, by also specifying accept filters, you can allow specific subsets
of messages which might normally be rejected to be received.  For example,
you may want all mail from "hotmail.com" to be rejected, except if it comes
from "hilary@hotmail.com".  To do this, implement a reject filter on the
"From" line which is "*@hotmail.com", and an accept filter on the "From"
line which is "hilary@hotmail.com".  (Quotation marks are not necessary.)

You do not need to use accept filters if you are not using reject filters.
By default, all mail is accepted unless reject filters exist.
$ endif
$
$ if folders .and. full
$ then
$   blurb

You can, however, use accept filters to automatically file a message in a
given mail folder or mail file.  Simply specify the folder name and an
optional mail file name (space separated) at the "Dest" prompt.
$ endif
$
$ if pmdf_version .lts. "5.1"
$ then
$   blurb

NOTE:  You are running a version of PMDF earlier than v5.1.  Because Postman
       needs a feature introduced in PMDF v5.1 to facilitate the full
       processing of Accept-filtered messages, you should note that messages
       which are Accept-filtered will *only* be delivered to you locally -
       they will not be forwarded nor will a vacation message be sent if these
       have been requested.
$ endif
$
$ if full
$ then
$   say ""
$   hold
$ endif
$
$ filtyp = "Reject"
$
$DISPLAY_FILTERS:
$ say ""
$ say "''filtyp' Filters"
$ say "--------------"
$ say ""
$
$ if 'filtyp'_filter_cnt .eq. 0
$ then
$   say "You have no ''filtyp' filters."
$   gosub  ADD_FILTERS
$   if filtyp .eqs. "Reject"
$   then
$     filtyp = "Accept"
$     goto  DISPLAY_FILTERS
$   endif
$ else
$   if active
$   then
$     default = "Yes"
$     say "You already have the following ACTIVE ''filtyp' filters defined."
$   else
$     default = "No"
$     say "You already have the following ''filtyp' filters defined,"
$     say "BUT they are NOT currently active."
$   endif
$
$   cnt = 1
$FILTER_SHOW:			! Display all current filters.
$   say ""
$   say "From: ",'filtyp'_filter'cnt'_from
$   say "To:   ",'filtyp'_filter'cnt'_to
$   say "Subj: ",'filtyp'_filter'cnt'_subj
$   if (filtyp .eqs. "Accept") .and. folders  then -
	say "Dest: ",'filtyp'_filter'cnt'_dest
$   cnt = cnt + 1
$   if cnt .le. 'filtyp'_filter_cnt  then  goto  FILTER_SHOW
$
$   say ""
$   msg = "Do you want to ''filtyp' filter mail (at all)? (Yes/No) " + -
	  "[''default'] "
$   ask "''msg'" ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  ans = default
$   if .not. ans
$   then
$     'filtyp'_filter_cnt = 0
$     if filtyp .eqs. "Reject"
$     then
$	filtyp = "Accept"
$	goto  DISPLAY_FILTERS
$     else
$	goto  MAKE_DELIVER_FILE
$     endif
$   endif
$
$CHANGE_FILTERS:
$   say ""
$   msg = "Do you want to change your existing ''filtyp' filters? " + -
	  "(Yes/No) [No] "
$   ask "''msg'" ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  ans = "No"
$   if .not. ans  then  goto  GET_NEW_FILTERS
$
$   if full
$   then
$     blurb

Indicate what you want to do with each of these groups of filters.
Enter one of the following (a single letter abbreviation is sufficient):

  . "Keep" to leave the current filter line unchanged.
  . "Modify" to modify the current filter line.
  . "Clear" to clear the current filter line.
  . "Delete" to delete the current filter group.
  . "Next" to skip to the next filter group.
  . "Zap" to delete ALL current filters.
  . "OK" to retain all filters not yet cleared or deleted.
$   endif
$   inx   = 1
$   fline = "From"
$   pad = ""
$FILTER_CHECK:
$   if (fline .eqs. "From") .or. (ans .eqs. "M")  then  say ""
$   filter = 'filtyp'_filter'inx'_'fline'
$   msg = "''fline':''pad' ''filter' ? " + -
	  "(Keep/Modify/Clear/Delete/Next/Zap/OK) [Keep] "
$   ask "''msg'" ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  ans = "Keep"
$   ans = f$extract (0,1,ans)
$   if ans .eqs. "M"
$   then
$     say ""
$     ask "''fline': ? " ans
$     ans = f$edit (ans,"TRIM,UNCOMMENT")
$     if ans .eqs. ""  then  ans = "*"
$     'filtyp'_filter'inx'_'fline' = ans
$     ans = "M"
$   endif
$   if ans .eqs. "C"  then  'filtyp'_filter'inx'_'fline' = "*"
$   if ans .eqs. "D"
$   then
$     'filtyp'_filter'inx'_from = "*"
$     'filtyp'_filter'inx'_to   = "*"
$     'filtyp'_filter'inx'_subj = "*"
$     if filtyp .eqs. "Accept"  then  'filtyp'_filter'inx'_dest = ""
$   endif
$   if (ans .eqs. "N") .or. (ans .eqs. "D")
$   then
$     if inx .lt. 'filtyp'_filter_cnt
$     then
$	fline = "From"
$	pad = ""
$	inx = inx + 1
$	goto  FILTER_CHECK
$     else
$	goto  GET_NEW_FILTERS
$     endif
$   endif
$   if ans .eqs. "Z"
$   then
$     'filtyp'_filter_cnt = 0
$     say ""
$     say "All current ''filtyp' filters will be discarded."
$     goto  GET_NEW_FILTERS
$   endif
$   if ans .eqs. "O"
$   then
$     say ""
$     say "All ''filtyp' filters not yet discarded will be retained."
$     goto  GET_NEW_FILTERS
$   endif
$   if (ans .nes. "K") .and. (ans .nes. "M") .and. (ans .nes. "C")
$   then
$     if fline .nes. "From"  then  say ""
$     goto  FILTER_CHECK
$   endif
$
$   if fline .eqs. "From"
$   then
$     fline = "To"
$     pad = "  "
$     goto  FILTER_CHECK
$   endif
$   if fline .eqs. "To"
$   then
$     fline = "Subj"
$     pad = ""
$     goto  FILTER_CHECK
$   endif
$   if fline .eqs. "Subj"
$   then
$     if (filtyp .eqs. "Accept") .and. folders
$     then
$	fline = "Dest"
$	goto  FILTER_CHECK
$     else
$	fline = "From"
$     endif
$   endif
$   if fline .eqs. "Dest"
$   then
$     if 'filtyp'_filter'inx'_dest .eqs. "*" then 'filtyp'_filter'inx'_dest = ""
$     fline = "From"
$   endif
$
$   inx = inx + 1
$   if inx .le. 'filtyp'_filter_cnt  then  goto  FILTER_CHECK
$
$GET_NEW_FILTERS:
$   gosub  ADD_FILTERS
$
$   if filtyp .eqs. "Reject"
$   then
$     filtyp = "Accept"
$     goto  DISPLAY_FILTERS
$   endif
$
$!  Re-write the filter lists to eliminate holes ...
$
$   inx = 0
$   cnt = 1
$FILTER_LOOPER:
$   if ('filtyp'_filter'cnt'_from .nes. "*") .or. -
       ('filtyp'_filter'cnt'_to   .nes. "*") .or. -
       ('filtyp'_filter'cnt'_subj .nes. "*")
$   then
$     inx = inx + 1
$     'filtyp'_filter'inx'_from = 'filtyp'_filter'cnt'_from
$     'filtyp'_filter'inx'_to   = 'filtyp'_filter'cnt'_to
$     'filtyp'_filter'inx'_subj = 'filtyp'_filter'cnt'_subj
$     if filtyp .eqs. "Accept"  then -
	'filtyp'_filter'inx'_dest = 'filtyp'_filter'cnt'_dest
$   endif
$   cnt = cnt + 1
$   if cnt .le. 'filtyp'_filter_cnt  then  goto  FILTER_LOOPER
$   'filtyp'_filter_cnt = inx
$
$   if filtyp .eqs. "Reject"
$   then
$     filtyp = "Accept"
$     say ""
$     goto  DISPLAY_FILTERS
$   endif
$ endif
$
$!  If we are filtering, do we want to log rejected messages?
$
$FILTER_LOG:
$ if reject_filter_cnt .gt. 0
$ then
$   say ""
$   say "Rejected Message Logging"
$   say "------------------------"
$   if full
$   then
$     say ""
$     say "You can log information about rejected messages to a logfile in"
$     say "your home directory (''f$trnlnm("SYS$LOGIN")'). ", -
	  "The information logged"
$     say "is the date and the ""From:"", ""To:"" and ""Subject:"" lines of the"
$     say "rejected messages."
$   endif
$   if log_rejects
$   then
$     default = "Yes"
$     blurb

You are currently logging rejected message information.  The logfile is

$     say "    ''f$trnlnm("SYS$LOGIN")'MAIL.REJECT_LOG"
$   else
$     default = "No"
$     say ""
$     say "You are NOT currently logging rejected message information."
$   endif
$   say ""
$   msg = "Do you want to log rejected message information? (Yes/No) [" + -
	  default + "] "
$   ask "''msg'" ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  ans = default
$   log_rejects = ans
$ endif
$
$!..............................................................................
$!  Create the new DELIVER file.  Always, keep only ONE version of the active
$!  or inactive DELIVER file.
$
$MAKE_DELIVER_FILE:
$
$ say ""
$ say divider,bell
$
$ default = "Yes"
$ if full
$ then
$   blurb

Your selections above may cause your mail handling configuration to change.
You may choose to implement your selection, or, if not, end Postman without
making any changes.

$ endif
$
$ if need_vacation .and. (.not. retain_mail) .and. (fwcnt .eq. 0)
$ then
$   default = "No"
$   blurb
*** WARNING!  You have a vacation message, but you are NOT retaining    ***
*** incoming mail, and you do NOT have any forwards.  This means that   ***
*** your incoming mail will be discarded completely!  You should re-run ***
*** Postman to ensure your messages are delivered to you or forwarded.  ***
$   say bell, bell
$ endif
$
$ if default .and. .not. full  then  say ""
$ msg = "Implement changes which may result from your selection? (Yes/No)"
$ ask "''msg' [''default'] " ans
$ ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$ if ans .eqs. ""  then  ans = default
$ if .not. ans  then  goto  END
$
$ say ""
$ say divider
$ say "Implementing required mail handling ..."
$
$ commit = 1
$
$ if f$search (tmp) .nes. ""  then  delete /nolog 'tmp';*
$
$!  If no forwards, filters or vacation, then de-activate any active DELIVER.
$
$ if ((fwcnt .eq. 0) .or. ((fwcnt .eq. 1) .and. .not. retain_mail)) .and. -
     (reject_filter_cnt .eq. 0) .and. (accept_filter_cnt .eq. 0) .and. -
     .not. need_vacation
$ then
$   deactivate = 1
$   if forwarding
$   then
$     close /nolog tmpfyl
$     open /write /error=ERROR_EXIT /end=ERROR_EXIT tmpfyl 'tmp'
$     putt "$ define /user SYS$OUTPUT NLA0:"
$     putt "$ mail"
$     if fwcnt .eq. 0
$     then
$	putt "set noforward"
$     else
$	if f$locate ("@",fw1) .eq. f$length (fw1)
$	then
$	  putt "set forward ''fw1'"	! Not an Internet address.
$	else
$	  if vms_version .lts. "V7.0"	! Quotes are handled differently!
$	  then
$	    putt "set forward in%""""""''fw1'"""""""
$	  else
$	    putt "set forward in%""''fw1'"""
$	  endif
$	endif
$     endif
$     putt "exit"
$     putt "$ EXIT 1"
$     close /nolog tmpfyl
$     @'tmp'
$     delete /nolog 'tmp';*
$   endif
$   if delivering .and. vacating
$   then
$     say ""
$     say "The vacation message will no longer be sent."
$   endif
$   if fwcnt .eq. 0
$   then
$     say ""
$     say "Forwarding, filtering and vacation message not required."
$   else
$     say ""
$     say "Your forwarding is now in place!"
$   endif
$ endif
$
$ if deactivate
$ then
$   if active
$   then
$     if fwcnt .eq. 1
$     then
$       say ""
$       say "The remainder of your existing configuration which was active ", -
	    "is now INACTIVE."
$     else
$       say ""
$       say "Your configuration which was active is now INACTIVE."
$     endif
$   endif
$   if delivering
$   then
$     if f$search ("SYS$LOGIN:MAIL.DELIVERY") .nes. ""  then -
	  purge /nolog SYS$LOGIN:MAIL.DELIVERY
$     if active  then -
	  rename /nolog SYS$LOGIN:MAIL.DELIVERY SYS$LOGIN:MAIL.DELIVERY_INACTIVE
$     if f$search ("SYS$LOGIN:MAIL.DELIVERY_INACTIVE") .nes. ""  then -
	  purge /nolog SYS$LOGIN:MAIL.DELIVERY_INACTIVE
$   endif
$   if f$search (vac_db_file) .nes. ""  then  delete /nolog 'vac_db_file';*
$   goto  END
$ endif
$
$!  Create the new DELIVER file ...
$
$ close /nolog deliver
$ open /write /error=ERROR_EXIT /end=ERROR_EXIT deliver SYS$LOGIN:MAIL.DELIVERY
$
$!  Some general remarks for anyone reading the MAIL.DELIVERY file ...
$
$ put "!  This file was AUTOMATICALLY generated by Postman v''version'"
$ put "!  Created by ''user' on ''node' (running PMDF v''pmdf_version')"
$ put "!  Creation time: ''f$extract(0,19,f$cvtime())'"
$ put "!  Do NOT edit or delete this file!"
$ put "!  Use the Postman utility to change or"
$ put "!     to activate or de-activate this file."
$
$!  We may want to debug ... !
$
$ if debug
$ then
$   put "!"
$   put "!  Debugger ..."
$   put "!"
$   put "* * * a l ''debugger'"
$   say ""
$   say "DELIVER debugging has been enabled."
$ endif
$
$!  Prevent any non-fatal errors in the DELIVER job from stopping it
$!  processing to the end.
$
$ put "!"
$ put "!  DELIVER error handler ..."
$ put "!"
$ put "* * * a e ""!---------------------------------------"""
$ put "* * * a e ""! Establishing Postman ERROR handler ..."""
$ put "* * * a e ""on severe_error  then  goto error_exit"""
$ put "* * * a e ""goto continue"""
$ put "* * * a e ""error_exit:	! Postman ERROR handler ..."""
$ if delete_files
$ then
$   put "* * * a e ""delete 'message_file';*"""
$   put "* * * a e ""delete 'message_header';*"""
$ endif
$ put "* * * a e ""exit 1"""
$ put "* * * a e ""continue:"""
$ put "* * * a e ""!---------------------------------------"""
$
$!  Include a "handler" to discard bounced messages and other messages
$!  which have an empty RETURN-PATH address.  Deliver a copy to the user
$!  for information.
$
$ put "!"
$ put "!  Bounce handlers ..."
$ put "!"
$ put """*mailer-daemon@*"" * * t d"
$ put """*mailer-daemon@*"" * * t q"
$ put """*postmaster@*"" * * t d"
$ put """*postmaster@*"" * * t q"
$ put "* * * a 1 ""Return-path"""
$ put """<>"" * * t d"
$ put """<>"" * * t q"
$ put "* * * a 1 ""From"""
$
$!  Filtering ...
$
$ filtyp = "Accept"
$FILTRATE:
$ if 'filtyp'_filter_cnt .gt. 0
$ then
$   put "!"
$   put "!  ''filtyp' Filtering ..."
$   put "!"
$   active = 1
$   inx = 1
$   dest = ""
$FILTER_WRITE:
$   filter1 = 'filtyp'_filter'inx'_from
$   filter2 = 'filtyp'_filter'inx'_to
$   filter3 = 'filtyp'_filter'inx'_subj
$   if filtyp .eqs. "Accept"
$   then
$     dest = 'filtyp'_filter'inx'_dest
$     put """''filter1'"" ""''filter2'"" ""''filter3'"" y d ''dest'"
$   else
$
$!  For Reject filtering, use a feature of PMDF 5.1+ (the asterisk on the
$!  directive) to facilitate Accept filtering passing on by.  Otherwise,
$!  Accept filters must terminate here.
$
$     if pmdf_version .lts. "5.1"
$     then
$       if log_rejects  then -
	   put """''filter1'"" ""''filter2'"" ""''filter3'"" o e """ + -
		"@''postie'  SIEVE"""
$	put """''filter1'"" ""''filter2'"" ""''filter3'"" a q "
$     else
$       if log_rejects  then -
	   put """''filter1'"" ""''filter2'"" ""''filter3'"" o* e """ + -
		"@''postie'  SIEVE"""
$	put """''filter1'"" ""''filter2'"" ""''filter3'"" o q "
$     endif
$   endif
$   inx = inx + 1
$   if inx .le. 'filtyp'_filter_cnt  then  goto  FILTER_WRITE
$   say ""
$   say "Your ''filtyp' filtering is now in place!"
$ endif
$ if filtyp .eqs. "Accept"
$ then
$   filtyp = "Reject"
$   goto  FILTRATE
$ endif
$
$!  Vacation message ...
$
$ if need_vacation
$ then
$   active = 1
$   put "!"
$   put "!  Vacation message - by default, respond ...
$   put "!"
$   put "* * * a e ""respond = 1"""
$   if pmdf_version .ges. "5.1"
$   then
$     put "!"
$     put "!  Check if it's from a mailing list ..."
$     put "!"
$     put "* * * a 1 ""Precedence"""
$     put """bulk"" * * t* e ""respond = 0"""
$     put """list"" * * t* e ""respond = 0"""
$     put """junk"" * * t* e ""respond = 0"""
$     put "* * * a 1 ""Content-return"""
$     put """prohibited"" * * t* e ""respond = 0"""
$     put "* * * a 1 ""X-Listname"""
$     put """%*"" * * t* e ""respond = 0"""
$     put "* * * a 1 ""X-Listserver"""
$     put """%*"" * * t* e ""respond = 0"""
$     put "* * * a 1 ""From"""
$     put "!"
$     put "!  Respond if required ..."
$     put "!"
$   endif
$   if use_vac_db
$   then
$     put "* * * a e ""vac_db == 1"""		! Global variable for DELIVER.
$   else
$     put "* * * a e ""vac_db == 0"""		! Global variable for DELIVER.
$   endif
$   put "* * * a e ""if respond  then  @''postie'  ALOHA  ''vacation_file'"""
$   say ""
$   say "Your vacation message is now in place!"
$ else
$   if f$search (vac_db_file) .nes. ""  then  delete /nolog 'vac_db_file';*
$ endif
$
$!  Forwarding ...
$
$ if fwcnt .gt. 0
$ then
$   put "!"
$   put "!  Forwarding ..."
$   put "!"
$   active = 1
$   inx = 1
$FW_WRITE:
$   fwd = fw'inx'
$   put "* * * a p ''fwd'"
$   inx = inx + 1
$   if inx .le. fwcnt  then  goto  FW_WRITE
$   say ""
$   say "Your forwarding is now in place!"
$ endif
$
$ if retain_mail
$ then
$   put "!"
$   put "!  Local delivery ..."
$   put "!"
$   put "* * * o d"
$ endif
$
$ close /nolog deliver
$
$!  Ensure OpenVMS MAIL uses DELIVER too ...
$
$ if .not. tilded
$ then
$   close /nolog tmpfyl
$   open /write /error=ERROR_EXIT /end=ERROR_EXIT tmpfyl 'tmp'
$   putt "$ define /user SYS$OUTPUT NLA0:"
$   putt "$ mail"
$   if vms_version .lts. "V7.0"		! Quotes are handled differently!
$   then
$     putt "set forward in%""""""~''user'"""""""
$   else
$     putt "set forward in%""~''user'"""
$   endif
$   putt "exit"
$   putt "$ EXIT 1"
$   close /nolog tmpfyl
$   @'tmp'
$   delete /nolog 'tmp';*
$ endif
$
$ if f$search (tmp) .nes. ""  then  delete /nolog 'tmp';*
$
$ if delivering
$ then
$   if f$search ("SYS$LOGIN:MAIL.DELIVERY") .nes. ""
$   then
$     purge /nolog SYS$LOGIN:MAIL.DELIVERY
$     if .not. active  then  delete /nolog SYS$LOGIN:MAIL.DELIVERY;*
$   endif
$   if f$search ("SYS$LOGIN:MAIL.DELIVERY_INACTIVE") .nes. ""
$   then
$     purge /nolog SYS$LOGIN:MAIL.DELIVERY_INACTIVE
$     if active  then  delete /nolog SYS$LOGIN:MAIL.DELIVERY_INACTIVE;*
$   endif
$ endif
$
$ goto  END
$
$!------------------------------------------------------------------------------
$ERROR_EXIT:
$ status = $status
$ say ""
$ say "*** An error (''status') has occured!  Please notify ITS! ***",bell,bell
$
$END:
$ if .not. commit
$ then
$   say ""
$   say "No changes made to your current configuration."
$ endif
$
$ close /nolog tmpfyl
$ close /nolog deliver
$ close /nolog vdb
$
$ if f$search (tmp) .nes. ""  then  delete /nolog 'tmp';*
$
$ EXIT 1 + 0 * f$verify (verify)
$
$!==============================================================================
$ADD_FORWARDS:
$ if need_vacation .and. (.not. retain_mail) .and. (fwcnt .eq. 0)
$ then
$   default = "Yes"
$ else
$   default = "No"
$ endif
$
$ say ""
$ ask "Do you wish to add any forwards? (Yes/No) [''default'] " ans
$ ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$ if ans .eqs. ""  then  ans = default
$ if ans
$ then
$   if full
$   then
$     blurb

*** CAUTION: Your forwarding addresses should be *valid* mail addresses.    ***
*** If they are not valid, you may inadvertently set up a "mail loop" where ***
*** rejected ("bounced") mail messages are re-forwarded to the same invalid ***
*** address when the notification of the rejection is sent to you.          ***

Enter the forwarding addresses, ONE per line, a blank line to end:
$   endif
$   inx = fwcnt + 1
$FW_GET:
$   say ""
$   ask "Address? " ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT")
$   if ans .eqs. ""
$   then
$     inx = inx - 1
$   else
$     ans = ans - "IN%" - "in%" - "In%" - "iN%" - -	! Remove any PMDF junk !
		  """" - """" - """" - """" - """" - """" - """" - """"
$     fw'inx' = ans
$     inx = inx + 1
$     goto  FW_GET
$   endif
$   fwcnt = inx
$   if fwcnt .gt. 0  then  forwarding = 1
$ endif
$
$ RETURN
$
$!------------------------------------------------------------------------------
$ADD_FILTERS:
$ say ""
$ ask "Do you wish to add any ''filtyp' filters? (Yes/No) [No] " ans
$ ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$ if ans .eqs. ""  then  ans = "No"
$ if ans
$ then
$   if full
$   then
$     blurb

You can filter based on any or all of the "From:" line, the "To:" line or
the "Subject:" line.  You can use the percent sign ("%") as a wildcard to
indicate one single unknown character, and the asterisk ("*") as a wildcard
to indicate zero or more unknown characters.  Filters are not case sensitive.
Examples:

  - to match either "dot", "dont" or "donut", you could specify "do*t"
  - to filter all mail with "free offer" in the subject, specify "*free offer*"
  - to filter all mail from "hotmail.com", specify "*@hotmail.com"

To specify filters, you must specify all of the lines, ("From:", "To:" and
"Subject:"), but you may leave the line BLANK if you do not wish to filter
that particular type of line.  Leave ALL lines BLANK to terminate input of
filters.

$   endif
$
$   if folders .and. (filtyp .eqs. "Accept") .and. full
$   then
$     blurb
You can also use accept filters to automatically file a message in a
given mail folder or mail file.  Simply specify the folder name and an
optional mail file name (space separated) at the "Dest" prompt.

$   endif
$   inx   = 'filtyp'_filter_cnt + 1
$   fline = "From"
$   pad = ""
$FILTER_GET:
$   'filtyp'_filter'inx'_'fline' = ""
$   ask "''fline':''pad' " ans
$   ans = f$edit (ans,"TRIM,UNCOMMENT")
$   if ans .eqs. ""
$   then
$     'filtyp'_filter'inx'_'fline' = "*"
$   else
$     ans = ans - "IN%" - "in%" - "In%" - "iN%" - -	! Remove any PMDF junk !
		  """" - """" - """" - """" - """" - """" - """" - """"
$     'filtyp'_filter'inx'_'fline' = ans
$   endif
$
$   if fline .eqs. "From"
$   then
$     fline = "To"
$     pad = "  "
$     goto  FILTER_GET
$   endif
$   if fline .eqs. "To"
$   then
$     fline = "Subj"
$     pad = ""
$     goto  FILTER_GET
$   endif
$   if fline .eqs. "Subj"
$   then
$     if (filtyp .eqs. "Accept") .and. folders
$     then
$	fline = "Dest"
$	goto  FILTER_GET
$     else
$	fline = "From"
$	'filtyp'_filter'inx'_dest = ""
$     endif
$   endif
$   if fline .eqs. "Dest"
$   then
$     if 'filtyp'_filter'inx'_dest .eqs. "*" then 'filtyp'_filter'inx'_dest = ""
$     fline = "From"
$   endif
$
$   if ('filtyp'_filter'inx'_from .nes. "*") .or. -
       ('filtyp'_filter'inx'_to   .nes. "*") .or. -
       ('filtyp'_filter'inx'_subj .nes. "*")
$   then
$     inx = inx + 1
$     say ""
$     goto  FILTER_GET
$   endif
$   'filtyp'_filter_cnt = inx - 1
$ endif
$
$ RETURN
$
$!------------------------------------------------------------------------------
$GET_MESSAGE_FILE:
$ if full
$ then
$   blurb

You must specify the vacation message you wish to send.

You can either create the message here and now, or you
can specify the name of a text file which contains the
message you wish to be sent.
$ endif
$
$ say ""
$ ask "Do you wish to use an existing text file? (Yes/No) [Yes] " ans
$ ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$ if ans .eqs. ""  then  ans = "Yes"
$ if ans
$ then
$GET_FILENAME:
$   say ""
$   ask "Filename to use? " ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  goto  GET_FILENAME
$   vacation_file = f$search (ans)
$   if vacation_file .eqs. ""
$   then
$     say ""
$     say "The file does NOT exist!",bell
$     goto  GET_MESSAGE_FILE
$   else
$     say ""
$     say "The vacation message file is:
$     say ""
$     say "  ",vacation_file
$     say ""
$     say "It contains:"
$     say ""
$     wait 00:00:02.00		! In case a big file scrolls off screen
$     type 'vacation_file'
$     say ""
$     ask "Do you want to use this file? (Yes/No) [Yes] " ans
$     ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$     if ans .eqs. ""  then  ans = "Yes"
$     if .not. ans  then  goto  GET_MESSAGE_FILE
$     vacation_file = f$element (0,";",vacation_file)
$     fyl = vacation_file
$   endif
$ else
$   say ""
$   msg = "Do you want to use the EVE editor to create your file? " + -
	  "(Yes/No) [Yes] "
$   ask "''msg'" ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  ans = "Yes"
$
$   inx = 1
$MAKE_VACATION:
$   vacation_file := SYS$LOGIN:VACATION_MESSAGE_'inx'.TXT
$   if f$search (vacation_file) .nes. ""
$   then
$     inx = inx + 1
$     goto  MAKE_VACATION
$   endif
$
$   say ""
$   say "Enter the name of the new message file or accept the default."
$   say ""
$   ask "Filename? [ ''vacation_file' ] " fyl
$   fyl = f$edit (fyl,"COLLAPSE,UNCOMMENT,UPCASE")
$   if fyl .eqs. ""  then  fyl = vacation_file
$   if f$parse (fyl) .eqs. ""
$   then
$     say ""
$     say "Invalid filename format - please try again ...",bell
$     goto  MAKE_VACATION
$   endif
$   if f$search (fyl) .nes. ""
$   then
$     say ""
$     say "A file of this name already exists."
$     say ""
$     say "It contains:"
$     say ""
$     wait 00:00:02.00		! In case a big file scrolls off screen
$     type 'fyl'
$     say ""
$     ask "Do you want to use (and edit) this file anyway? (Yes/No) [No] " ans
$     ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$     if ans .eqs. ""  then  ans = "No"
$     if .not. ans  then  goto  MAKE_VACATION
$   endif
$   vacation_file = fyl
$   fyl = f$search (fyl)
$
$   if ans
$   then
$     define /user sys$input sys$command
$     edit /tpu 'vacation_file'
$   else
$     blurb

Please enter your message below - you may use as many lines you wish.
You should press the "Return" key at the end of each line.  When you are
finished, please press Control-Z (i.e., the "Ctrl" key and the "Z" key
at the same time).

$     define /user sys$input sys$command
$     create 'vacation_file'
$   endif
$ endif
$
$ if fyl .eqs. ""
$ then
$   say ""
$   say "Your vacation message file has been created."
$ endif
$
$ vacation_file = f$element (0,";",f$search (vacation_file))
$
$ RETURN
$!------------------------------------------------------------------------------
$SHUFFLE_PARAMS:
$
$ p1 = p2
$ p2 = p3
$ p3 = p4
$ p4 = p5
$ p5 = p6
$ p6 = p7
$ p7 = p8
$ p8 = ""
$
$ RETURN
$!------------------------------------------------------------------------------
