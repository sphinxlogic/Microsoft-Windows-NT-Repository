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
$!	- multiple rejection filters
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
$!    $ POSTMAN DELETE_ON_ERROR		! Delete message files if DELIVER bombs.
$!    $ POSTMAN DEBUG [filename]	! Debug DELIVER, alternate logfile name.
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
$!..............................................................................
$!  History:
$! v1.0	07-Oct-1998	JER	Original version.
$! v1.1	12-Oct-1998	JER	Allow logging of rejected messages.
$! v1.2	14-Oct-1998	JER	Checks for non-Postman generated file.
$! v1.3	15-Oct-1998	JER	For a single forward (ONLY), use MAIL.
$! v1.4	30-Oct-1998	JER	Allow multiple parameters to this procedure, and
$!				message files on error to stay or be deleted.
$!******************************************************************************
$
$ on warning    then  goto  END
$ on control_y  then  goto  END
$
$ say 	:= write SYS$OUTPUT
$ ask	:= read  SYS$COMMAND /error=END /end=END /prompt=
$ put   := write deliver /error=ERROR_EXIT /end=ERROR_EXIT
$ putt  := write tmpfyl  /error=ERROR_EXIT /end=ERROR_EXIT
$ blurb := type  SYS$INPUT
$ z	:= type /page NLA0:
$
$ version = "1.4 (30-Oct-1998)"	! Keep the VERSION information up-to-date!!!
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
$ vms_version = f$getsyi ("VERSION")
$
$ tmp := SYS$SCRATCH:POSTMAN_'pid'.TMP
$ postie := SYS_SYSTEM:POSTMAN		! SITE-specific name of this procedure !
$ reject_log := SYS$LOGIN:MAIL.REJECT_LOG
$
$ forward  = ""
$ actor    = ""
$ debugger = "DELIVER.LOG"	! Must be only filename - no directory, etc.
$ vacating = 0
$ delivering = 0
$ forwarding = 0
$ filter_cnt = 0
$ deactivate = 0
$ tilded = 0
$ active = 0
$ fwcnt  = 0
$ debug  = 0
$ commit = 0
$ log_rejects   = 0
$ need_vacation = 0
$ delete_files  = 0
$
$ retain_mail = 1		! Default is to deliver to self.
$
$CHECK_PARAMS:
$ p1 = f$edit (p1,"UPCASE,COLLAPSE,UNCOMMENT")
$ if p1 .eqs. ""  then  goto  FIRE_UP
$
$ if p1 .eqs. "TRACE"  then  set verify		! Watch what's happening here.
$
$ if p1 .eqs. "ALOHA"				! Take me away P&O !!!
$ then
$   mail 'p2' "''f$element(0,"""",from-"IN%""")'" -
	/subject="Re: ''subject'" /noself
$   commit = 1		! Fudge it.
$   goto  END		! Escape!
$ endif
$
$ if p1 .eqs. "SIEVE"				! Log the filtered message.
$ then
$   close /nolog tmpfyl
$   if f$search (reject_log) .eqs. ""  then  create 'reject_log'
$   open /append /error=ERROR_EXIT /end=ERROR_EXIT tmpfyl 'reject_log'
$   putt ""
$   putt "Date:	", f$cvtime()
$   putt "From:	", f$element(0,"""",from-"IN%""")
$   putt "Subj:	", subject
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

  - a vacation message;
  - multiple forwarding addresses;
  - multiple filters to discard messages based on "From" and "Subject" lines.

               ***    Postman uses PMDF's DELIVER system.   ***
               *** If you wish to control DELIVER yourself, ***
               ***    you should NOT use this utility.      ***

Enter Control-Z or Control-C or Control-Y at any time to abort.
("Control" is the "Ctrl" key - use it like "Shift" with "Z", "C" or "Y")

Checking your current configuration ...
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
$   forward = f$edit (forward-"IN%"""-""""-""""-""""-""""-"""","COMPRESS,TRIM")
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
$   search SYS$LOGIN:MAIL.DELIVERY'actor' /nooutput /nowarning " by postman v"
$   status = $status
$   set on
$   if status .ne. ss$_normal
$   then
$     say ""
$     say "WARNING: You have a PMDF MAIL.DELIVERY''actor' file which was NOT"
$     say "generated by Postman!  Using Postman may cause some of your"
$     say "configuration to be lost.",bell,bell
$     say ""
$     ask "Do you want to continue? (Yes/No) [No] " ans
$     ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$     if ans .eqs. ""  then  ans = "No"
$     if .not. ans  then  goto  END
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
$   set noon
$   search SYS$LOGIN:MAIL.DELIVERY'actor' "* * * a d" /nooutput /nowarning
$   status = $status
$   set on
$   retain_mail = status .eq. ss$_normal
$
$   if f$search (tmp) .nes. ""  then  delete /nolog 'tmp';*
$
$!  Filters? ...
$
$   set noon
$   search SYS$LOGIN:MAIL.DELIVERY'actor' " y q " /output='tmp' /nowarning
$   status = $status
$   set on
$   if status .eq. ss$_normal
$   then
$     close /nolog tmpfyl
$     open /read /error=ERROR_EXIT /end=ERROR_EXIT tmpfyl 'tmp'
$FILTER_READ:
$     read /error=FILTER_DONE /end=FILTER_DONE tmpfyl rec
$     filter_cnt = filter_cnt + 1
$     filter'filter_cnt'_from = f$extract (0,f$locate(" * ",rec)-1,rec) - """"
$     filter'filter_cnt'_subj = rec - """" - filter'filter_cnt'_from - -
		""" * " - " y q " - """" - """"
$     goto  FILTER_READ
$FILTER_DONE:
$     close /nolog tmpfyl
$   endif
$
$!  Are rejected messages being logged? ...
$
$   set noon
$   search SYS$LOGIN:MAIL.DELIVERY'actor' "''postie'  SIEVE" -
	/nooutput /nowarning
$   status = $status
$   set on
$   log_rejects = status .eq. ss$_normal
$
$   if f$search (tmp) .nes. ""  then  delete /nolog 'tmp';*
$
$!  Vacation? ...
$
$   set noon
$   search SYS$LOGIN:MAIL.DELIVERY'actor' "''postie'  ALOHA" -
	/output='tmp' /nowarning
$   status = $status
$   set on
$   vacating = status .eq. ss$_normal
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

A vacation message can be used to automatically notify a person who has
sent you mail that you are on vacation, or simply to indicate to the
sender that you have received the mail.

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
$     goto  CHECK_FWD
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
$!..............................................................................
$CHECK_FWD:
$ say ""
$ say divider
$ blurb
FORWARDING
----------

Forwarding allows you to send a copy of a message to another person.
You may establish as many forwarding addresses as you wish - each one
will receive a copy of the message.

$ if fwcnt .eq. 0
$ then
$   say "You are not forwarding your mail."
$   gosub  ADD_FORWARDS
$ else
$   if active .or. (fwcnt .gt. 0)
$   then
$     default = "Yes"
$     say "You already have the following ACTIVE forwarding addresses defined."
$   else
$     default = "No"
$     say "You already have the following forwarding addresses defined,"
$     say "BUT they are NOT currently active."
$   endif
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
$     goto  CHECK_FILTER
$   endif
$
$   say ""
$   ask "Do you want to change your forwards? (Yes/No) [No] " ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  ans = "No"
$   if .not. ans  then  goto  GET_FWD
$
$   blurb

Indicate if you wish to keep each of these forwards.
Enter "Quit" to discard ALL current forwards.
Enter "All"  to retain  ALL forwards not yet discarded.
$   inx = 1
$FW_CHECK:
$   fwd = fw'inx'
$   say ""
$   ask "''fwd' ? (Yes/No/Quit/All) [Yes] " ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  ans = "Yes"
$   ans = f$extract (0,1,ans)
$   if ans .eqs. "Q"
$   then
$     fwcnt = 0
$     say ""
$     say "All current forwards will be discarded."
$     goto  GET_FWD
$   endif
$   if ans .eqs. "A"
$   then
$     say ""
$     say "All forwards not yet discarded will be retained."
$     goto  GET_FWD
$   endif
$   if .not. ans  then  fw'inx' = "###"
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
$   if retain_mail .and. active
$   then
$     default = "Yes"
$   else
$     default = "No"
$   endif
$   say ""
$   say "You are forwarding your mail."
$   say ""
$   msg = "Do you want to keep a copy of each forwarded message? " + -
	  "(Yes/No) [" + default + "] "
$   ask "''msg'" ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  ans = default
$   if .not. ans  then  retain_mail = 0
$ endif
$
$!..............................................................................
$CHECK_FILTER:
$ say ""
$ say divider
$ blurb
FILTERING
---------

Filtering allows you to discard an incoming message completely as if the
message had never been received.

$ if filter_cnt .eq. 0
$ then
$   say "You have no filters."
$   gosub  ADD_FILTERS
$ else
$   if active
$   then
$     default = "Yes"
$     say "You already have the following ACTIVE filters defined."
$   else
$     default = "No"
$     say "You already have the following filters defined,"
$     say "BUT they are NOT currently active."
$   endif
$
$   cnt  = 1
$FILTER_SHOW:			! Display all current filters.
$   say ""
$   say "From: ",filter'cnt'_from
$   say "Subj: ",filter'cnt'_subj
$   cnt = cnt + 1
$   if cnt .le. filter_cnt  then  goto  FILTER_SHOW
$
$   say ""
$   ask "Do you want to filter mail (at all)? (Yes/No) [''default'] " ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  ans = default
$   if .not. ans
$   then
$     filter_cnt = 0
$     goto  MAKE_DELIVER_FILE
$   endif
$
$   say ""
$   ask "Do you want to change your filters? (Yes/No) [No] " ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  ans = "No"
$   if .not. ans  then  goto  FILTER_LOG
$
$   blurb

Indicate if you wish to keep each of these filters.
Enter "Quit" to discard ALL current filters.
Enter "All"  to retain  ALL filters not yet discarded.
$
$   inx  = 1
$   subj = 1
$FILTER_CHECK:
$   if subj
$   then
$     ftype = "From"
$     subj  = 0
$     say ""
$   else
$     ftype = "Subj"
$     subj  = 1
$   endif
$   filter = filter'inx'_'ftype'
$   ask "''ftype': ''filter' ? (Yes/No/Quit/All) [Yes] " ans
$   ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$   if ans .eqs. ""  then  ans = "Yes"
$   ans = f$extract (0,1,ans)
$   if ans .eqs. "Q"
$   then
$     filter_cnt = 0
$     say ""
$     say "All current filters will be discarded."
$     goto  GET_FILTER
$   endif
$   if ans .eqs. "A"
$   then
$     say ""
$     say "All filters not yet discarded will be retained."
$     goto  GET_FILTER
$   endif
$   if .not. ans  then  filter'inx'_'ftype' = "*"
$   if .not. subj then  goto  FILTER_CHECK
$   inx = inx + 1
$   if inx .le. filter_cnt  then  goto  FILTER_CHECK
$GET_FILTER:
$   gosub  ADD_FILTERS
$
$!  Re-write the filter list to eliminate holes ...
$
$   inx = 0
$   cnt = 1
$FILTER_LOOPER:
$   if (filter'cnt'_from .nes. "*") .or. (filter'cnt'_subj .nes. "*")
$   then
$     inx = inx + 1
$     filter'inx'_from = filter'cnt'_from
$     filter'inx'_subj = filter'cnt'_subj
$   endif
$   cnt = cnt + 1
$   if cnt .le. filter_cnt  then  goto  FILTER_LOOPER
$   filter_cnt = inx
$ endif
$
$!  If we are filtering, do we want to log rejected messages?
$
$FILTER_LOG:
$ if filter_cnt .gt. 0
$ then
$   say ""
$   say "You can log information about rejected messages to a logfile in your"
$   say "home directory (''f$trnlnm("SYS$LOGIN")').  The information logged is"
$   say "the date and the ""From:"" and ""Subject:"" lines of the rejected"
$   say "messages."
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
     (filter_cnt .eq. 0) .and. -
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
$ put "!  Created by: ''user'
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
$ if filter_cnt .gt. 0
$ then
$   put "!"
$   put "!  Filtering ..."
$   put "!"
$   active = 1
$   inx = 1
$FILTER_WRITE:
$   filter1 = filter'inx'_from
$   filter2 = filter'inx'_subj
$   if log_rejects  then -
    put """''filter1'"" * ""''filter2'"" y e ""@''postie'  SIEVE"""
$   put """''filter1'"" * ""''filter2'"" y q "
$   inx = inx + 1
$   if inx .le. filter_cnt  then  goto  FILTER_WRITE
$   say ""
$   say "Your filtering is now in place!"
$ endif
$
$!  Vacation message ...
$
$ if need_vacation
$ then
$   put "!"
$   put "!  Vacation message ..."
$   put "!"
$   put "* * * a e ""@''postie'  ALOHA  ''vacation_file'"""
$   say ""
$   say "Your vacation message is now in place!"
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
$   put "* * * a d"
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
$
$ EXIT 1 + 0 * f$verify (verify)
$
$!==============================================================================
$ADD_FORWARDS:
$ say ""
$ ask "Do you wish to add any forwards? (Yes/No) [No] " ans
$ ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$ if ans .eqs. ""  then  ans = "No"
$ if ans
$ then
$   blurb

*** CAUTION: Your forwarding addresses should be *valid* mail addresses.    ***
*** If they are not valid, you may inadvertently set up a "mail loop" where ***
*** rejected ("bounced") mail messages are re-forwarded to the same invalid ***
*** address when the notification of the rejection is sent to you.          ***

Enter the forwarding addresses, ONE per line, a blank line to end:
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
$ ask "Do you wish to add any filters? (Yes/No) [No] " ans
$ ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$ if ans .eqs. ""  then  ans = "No"
$ if ans
$ then
$   blurb

You can filter based on the "From:" line or the "Subject:" line or both.
You can use the asterisk ("*") as a wildcard to indicate zero or more
unknown characters.  Examples:

  - to match either "dot", "dont" or "donut", you could specify "do*t"
  - to filter all mail with "free offer" in the subject, specify "*free offer*"
  - to filter all mail from "hotmail.com", specify "*@hotmail.com"

To specify filters, you must specify both the "From:" line and the "Subject:"
line.  Leave the line BLANK if you do not wish to filter that particular type
of line.  Leave BOTH lines BLANK to terminate input of filters.
$
$   inx  = filter_cnt + 1
$   subj = 1
$FILTER_GET:
$   if subj
$   then
$     ftype = "From"
$     subj  = 0
$     filter'inx'_subj = ""
$     say ""
$   else
$     ftype = "Subj"
$     subj  = 1
$   endif
$   ask "''ftype': " ans
$   ans = f$edit (ans,"TRIM,UNCOMMENT")
$   if ans .eqs. ""
$   then
$     filter'inx'_'ftype' = "*"
$   else
$     ans = ans - "IN%" - "in%" - "In%" - "iN%" - -	! Remove any PMDF junk !
		  """" - """" - """" - """" - """" - """" - """" - """"
$     filter'inx'_'ftype' = ans
$   endif
$   if (filter'inx'_from .nes. "*") .or. (filter'inx'_subj .nes. "*")
$   then
$     inx = inx + subj
$     goto  FILTER_GET
$   endif
$   filter_cnt = inx - 1
$ endif
$
$ RETURN
$
$!------------------------------------------------------------------------------
$GET_MESSAGE_FILE:
$ blurb

You must specify the vacation message you wish to send.

You can either create the message here and now, or you
can specify the name of a text file which contains the
message you wish to be sent.

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
$     ask "Do you want to use this file anyway? (Yes/No) [No] " ans
$     ans = f$edit (ans,"COLLAPSE,UNCOMMENT,UPCASE")
$     if ans .eqs. ""  then  ans = "No"
$     if .not. ans  then  goto  MAKE_VACATION
$   endif
$   vacation_file = fyl
$
$   if ans
$   then
$     define /user sys$input sys$command
$     edit /tpu 'vacation_file'
$   else
$     blurb

Please enter your message below - you may use as many lines you wish.
You should press "Return" at the end of each line.  When you are
finished, please press Control-Z (i.e., the "Ctrl" key and the "Z" key
at the same time).

$     define /user sys$input sys$command
$     create 'vacation_file'
$   endif
$ endif
$
$ say ""
$ say "Your vacation message file has been created."
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
