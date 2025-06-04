$!
$! SHADOW$TRACK.COM
$!
$!	Track shadowset membership and notify someone when things change.
$!
$!								V1.2
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$! http://www.geocities.com/keithparris/ or http://encompasserve.org/~parris/
$!
$	old_verify = f$verify('f$trnlnm("SHADOW$TRACK_VERIFY")'+0)
$	node = f$getsyi("NODENAME")
$	if p1 .eqs. "SUBMIT" then goto resubmit
$!  *** ATTENTION ***
$! Change the following line to direct VMSmail notification to the desired user(s):
$	target := system	!Username to be notified by mail of problems
$!!	page_target := xxxxx	!E-mail address to send a page to
$!
$	pid = f$getjpi("","PID")
$	temp_file := shadow$track_temp_file_'pid'.temp_file
$	close/nolog temp
$! Instead of putting shadowset membership here in this procedure, and having to
$! maintain it, we'll just pick up what we see now and track from here on out.
$	first_time := TRUE
$!
$ check_shadowsets:
$	@SHADOW$CONFIG GLOBAL_SYMBOLS	!Gather current shadowset info
$!
$! See if anything has changed
$!
$	i = SHADOW$CONFIG_SET_COUNT
$ shad_loop:
$	if i .le. 0 then goto shad_end
$	dsa = SHADOW$CONFIG_SET_'i'_DEVICE_NAME - ":"
$!!! write sys$output ""
$!!! write sys$output "Checking device ",dsa	!!!
$!!! write sys$output ""
$!
$! Build a list of the members currently in the shadowset, to make it easier
$! later to use the F$LOCATE lexical function to look for members
$	j = SHADOW$CONFIG_SET_'i'_MEMBER_COUNT
$	current_mbr_list = ","	!Leading comma (will also have trailing comma)
$ build_current_list:
$	if j .le. 0 then goto build_current_end
$	current_mbr_list = current_mbr_list + -
		SHADOW$CONFIG_SET_'i'_MEMBER_'j'_DEVICE_NAME + ","
$	j = j - 1
$	goto build_current_list
$ build_current_end:
$!!! show symbol current_mbr_list	!!!
$! Create a version of the current-member list without leading/trailing commas
$	cml = -	!Remove leading and trailing commas from current list
    f$extract(0,f$length(current_mbr_list)-1,current_mbr_list) - ","
$!
$! Check this DSAn: device against our DCL-symbol database of device info
$	if .not. F$GETDVI(dsa,"MNT")
$	then
$! Here's a shadow set that is not mounted.
$! If we weren't tracking it before, ignore it now.
$		if f$type(SHADOW$TRACK_'dsa'_MEMBER_COUNT) .eqs. "" then -
			goto shad_next
$! We we tracking this shadow set before, but now it's no longer in use.
$		if .not. first_time
$		then
$		    write sys$output -
	"Shadowset ''dsa' is no longer mounted on ''node'::"
$		    write sys$output -
	"Membership data will no longer be tracked for ",dsa," starting now."
$		    call mail -
-!	"Shadowset ''dsa' removed from set of tracked shadowsets on ''node'::"-
	"Shadowset ''dsa' removed from set of tracked shadowsets"
$		endif
$! Remove this shadowset from our DCL symbol database and don't track it
$! any longer
$		delete/symbol/global SHADOW$TRACK_'dsa'_LABEL
$		delete/symbol/global SHADOW$TRACK_'dsa'_MEMBER_COUNT
$		delete/symbol/global SHADOW$TRACK_'dsa'_MEMBER_LIST
$		goto shad_next
$	endif
$!
$	if f$type(SHADOW$TRACK_'dsa'_LABEL) .eqs. ""
$	then	!We have no data on this particular shadowset
$!		!Ignore it if it's not mounted
$		if .not. f$getdvi(dsa,"MNT") then goto shad_next
$!		!Ignore it if there are no members
$		if SHADOW$CONFIG_SET_'i'_MEMBER_COUNT .le. 0 then goto shad_next
$		if .not. first_time
$		then
$		    write sys$output -
	"No previous membership data available for shadowset ",dsa,";"
$		    write sys$output -
	"membership data will start being tracked for ",dsa," starting now."
$		    write sys$output "Current set of members for ",dsa,-
			" is <",cml,">"
$		    lbel = f$getdvi(dsa,"VOLNAM")	!Volume name
$		    call mail -
-!	"Shadowset ''dsa': (''cml') added to set of tracked shadowsets on ''node'::"-
	"Shadowset ''dsa': < ''lbel' > (''cml') added to set of tracked shadowsets"
$!!			sys$input "''target'"
$		endif
$!! No previous membership data was available in SHADOW$TRACK.COM for this
$!! shadowset, but membership data will start being tracked for this unit starting
$!! now.
$! Add this shadowset to our DCL symbol database and track it from now on
$		goto shad_update
$	endif
$	if SHADOW$TRACK_'dsa'_LABEL .nes. SHADOW$CONFIG_SET_'i'_LABEL
$	then
$		old_lbl = SHADOW$TRACK_'dsa'_LABEL
$		new_lbl = SHADOW$CONFIG_SET_'i'_LABEL
$		if .not. first_time
$		then
$		    write sys$output "Label on shadowset ",dsa,-
			" has changed from '",old_lbl,"' to '",new_lbl,"'"
$		    call mail -
-!"Label on shadowset ''dsa' has changed from ""''old_lbl'"" to ""''new_lbl'"" on ''node'::"-
"Label on shadowset ''dsa' has changed from <''old_lbl'> to <''new_lbl'>"
$		endif
$	endif
$! See if the number of members in the shadowset has changed
$	difference = SHADOW$CONFIG_SET_'i'_MEMBER_COUNT -
	           - SHADOW$TRACK_'dsa'_MEMBER_COUNT
$	if difference .ne. 0
$	then	!Number of members has indeed changed
$		if difference .gt. 0
$		then
$		    if .not. first_time
$		    then
$			write sys$output "Member count for shadowset ",dsa,-
-!				" has gone up by ",difference," on ''node'::"
				" has gone up by ",difference
$!!! Don't bother to notify of membership count changes by mail, as they are
$!!! always followed by specific information on which member was added or
$!!! removed anyway...
$!!!			call mail -
$!-!!!	"Member count for shadowset ''dsa' has gone up by ''difference' on ''node'::"
$		    endif
$		else
$			difference = -difference
$		    if .not. first_time
$		    then
$			write sys$output "Member count for shadowset ",dsa,-
-!				" has gone down by ",difference," on ''node'::"
				" has gone down by ",difference
$!!!			call mail -
$!-!!!	"Member count for shadowset ''dsa' has gone down by ''difference' on ''node'::"
$		    endif
$		endif
$	endif
$!
$! Check each member list to see if what, if anything, has changed
$!
$! Here's the list of the members formerly in the shadowset
$	former_mbr_list = "," + SHADOW$TRACK_'dsa'_MEMBER_LIST + ","
$!!! write sys$output "Former member list: ",former_mbr_list	!!!
$!
$! Check each of former members against the current members' list
$	k = SHADOW$TRACK_'dsa'_MEMBER_COUNT
$!!! write sys$output "Former-member list has ''k' elements."	!!!
$ check_former:
$!!!!	mbr = SHADOW$TRACK_'dsa'_MEMBER_'k'_DEVICE_NAME
$	mbr = f$element(k,",",former_mbr_list)
$!!! write sys$output "Checking former member (''k')",mbr		!!!
$	if f$locate(","+mbr+",",current_mbr_list) .eq. -
	               f$length(current_mbr_list)
$	then
$	    if .not. first_time
$	    then
$		write sys$output "Disk ",mbr," is no longer in shadowset ",dsa
$		labl = SHADOW$CONFIG_SET_'i'_LABEL
$		call mail -
-!"Device ''mbr' is no longer in shadowset ''dsa'(''labl') on ''node'::"-
-!"''mbr' left shadowset ''dsa': < ''labl' > on ''node'::"-
"Disk ''mbr' left shadowset ''dsa': <''labl'> ''dsa':/shad=''mbr' ''labl'"
$! If automatic repair is enabled, re-mount the failed volume
$		if f$trnlnm("SHADOW$TRACK_DO_FIX")
$		then
$		    mount/system/noassist/norebuild -
			'dsa'/shadow=('mbr') 'labl'
$		endif
$	    endif
$	endif
$	k = k - 1
$	if k .gt. 0 then goto check_former
$!
$! Check each of the current members against the former-members list
$!
$	j = SHADOW$CONFIG_SET_'i'_MEMBER_COUNT
$!!! show symbol former_mbr_list	!!!
$ check_current:
$	mbr = SHADOW$CONFIG_SET_'i'_MEMBER_'j'_DEVICE_NAME
$!!! write sys$output "Checking current member ",mbr
$	if f$locate(","+mbr+",",former_mbr_list) .eq. f$length(former_mbr_list)
$	then
$	    if .not. first_time
$	    then
$		write sys$output "Disk ",mbr,-
-!			" has been added to shadowset ",dsa," on ''node'::"
			" has been added to shadowset ",dsa
$		call mail -
-!	"Device ''mbr' has been added to shadowset ''dsa' on ''node'::"-
	"Disk ''mbr' has been added to shadowset ''dsa'"
$	    endif
$	endif
$	j = j - 1
$	if j .gt. 0 then goto check_current
$!
$! Update our DCL-symbol database to reflect the current membership so we
$! don't re-report changes
$!
$ shad_update:
$	SHADOW$TRACK_'dsa'_LABEL == SHADOW$CONFIG_SET_'i'_LABEL
$	SHADOW$TRACK_'dsa'_MEMBER_COUNT == SHADOW$CONFIG_SET_'i'_MEMBER_COUNT
$	SHADOW$TRACK_'dsa'_MEMBER_LIST == cml
$ shad_next:
$	i = i - 1
$	goto shad_loop
$!
$ shad_end:
$!!! exit
$	wait 00:05:00	!Wait a while
$	first_time := FALSE	!Not the initial pass anymore...
$	goto check_shadowsets
$!
$! Submit this job to run in a batch queue
$!
$ resubmit:
$	this_proc = f$environment("PROCEDURE")
$	this_proc = f$element(0,";",this_proc)	!Strip version #
$!  *** ATTENTION ***
$! Preferably, submit this to a cluster-wide generic batch queue, so it
$! can fail over to another node if the one it is running on fails.
$	submit/noprint/restart 'this_proc'	!/queue=cluster_batch
$	exit 1+0*'f$verify(old_verify)'
$!
$! Do VMSmail and pager notification.  P1 = message text to send
$!	Symbol 'target' contains VMSmail address(es) (subject contains text)
$!	Symbol 'page_target' contains pager addresses (body contains text)
$!
$ mail: subroutine
$	MAIL/NOSELF/SUBJECT="''p1'" _NLA0: "''target'"
$! PAGING DISABLED
$	endsubroutine	! PAGING DISABLED
$! PAGING DISABLED
$	open/write temp 'temp_file'
$! Airtouch pager requires the message to be in the body, as the subject line is ignored
$	write temp "$ MAIL/NOSELF SYS$INPUT " + -
"''page_target'"		!e.g. ""9991234567@alphapage.airtouch.com"""
$	write temp p1," at ",f$time()
$	write temp "$ EXIT"
$	close temp
$!!DISABLED!!	@'temp_file'
$	delete 'temp_file';*
$	endsubroutine
