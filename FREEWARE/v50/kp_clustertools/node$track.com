$!
$! NODE$TRACK.COM
$!
$! Watch each node in the cluster and notify me when a node leaves or joins
$!					V1.2 7/21/97, 9/23/98, 2/2001
$!
$! On initial entry, P1 & P2 may be blank.  P1 may be "SUBMIT" to submit the
$! initial monitoring job.  Otherwise,
$!	P1: Count of nodes last seen
$!	P2: List of names of nodes last seen, separated by commas (no spaces)
$!
$! Modification history:
$! 10/??/98 K.Parris  Have job resubmit itself at midnight to avoid excessive
$!                    logfile growth over long uptime
$! 10/14/98 K.Parris  Pass node-count and list of nodenames across the midnight
$!                    resubmits that we do to keep limits on the logfile size
$!                    and detect the difference between this and a restart
$!                    due to a node failure by the value of the $RESTART symbol
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$! http://www.geocities.com/keithparris/ or http://encompasserve.org/~parris/
$!
$	old_verify = f$verify('f$trnlnm("NODE$TRACK$VERIFY")'+0)
$	if p1 .eqs. "SUBMIT"
$	then	!On initial start, clear arguments
$		p1 = ""
$		p2 = ""
$		goto submit_it
$	endif
$	if $RESTART
$	then	!On auto-restart by queue manager after node failure, clear args
$		p1 = ""
$		p2 = ""
$	endif
$! *** ATTENTION ***
$! Change the following line to direct VMSmail notification to the desired user
$       mail_target := system      !Username to be notified by mail
$! Change the following line to direct mail notification to the desired users'
$! alphanumeric pagers:
$!!!PAGING DISABLED!!!       page_target := xxxx
$!
$	pid = f$getjpi("","PID")
$	temp_file := node$track_temp_'pid'.temp_file
$	close/nolog msg
$!
$! First, find out the names of all the nodes in the cluster
$	last_nodecount = 0
$	if p1 .nes. "" then last_nodecount = f$integer(p1)
$	last_nodelist = ""
$	if p2 .nes. "" .and. last_nodecount .ne. 0 then last_nodelist = p2
$!
$	last_hour = f$cvtime(,,"HOUR")
$ main_loop:
$	nodecount = 0
$	nodelist = ""
$	context = ""
$ loop:
$	csid = f$csid(context)	!Get the CSID of the next cluster member
$!!! show sym id	!!!
$	if csid .nes. ""
$	then
$	    nodename = f$getsyi("NODENAME",,csid)
$	    nodelist = nodelist + "," + nodename
$	    nodecount = nodecount + 1	!Count 1 more node
$	    goto loop
$	endif
$	nodelist = nodelist + ","	!Add trailing comma
$!!! show sym last_nodecount	!!!
$!!! show sym last_nodelist	!!!
$!!! show sym nodecount	!!!
$!!! show sym nodelist	!!!
$	if last_nodelist .eqs. ""
$	then	!First time through loop, notify me that we're starting over
$		call notify "(Re)Starting with ''nodecount' nodes"
$	endif
$!
$! Check the list and see which nodes are new or have left
$!
$! First, check that all nodes which were here before are still here (i.e.
$! detect any nodes which have left the cluster)
$!!! write sys$output "Checking for nodes which have left"	!!!
$	if last_nodecount .eq. 0 then goto end2	!Don't check for nodes leaving
$!						!or joining before we have the
$!						!initial list to compare against
$	i = 1
$ loop1:
$!!! show sym i	!!!
$	node = f$element(i,",",last_nodelist)
$!!! show sym node	!!!
$	if f$locate(","+node+",",nodelist) .eq. f$length(nodelist)
$	then	!Node was in old list, but is not in the new list
$		call notify "''node' left"
$	endif
$	i = i + 1
$	if i .le. last_nodecount then goto loop1
$! Next, check that all nodes present now were here before (i.e. detect any
$! nodes which have joined the cluster)
$!!! write sys$output "Checking for nodes which have joined"	!!!
$	i = 1
$ loop2:
$!!! show sym i	!!!
$	node = f$element(i,",",nodelist)
$!!! show sym node	!!!
$	if f$locate(","+node+",",last_nodelist) .eq. f$length(last_nodelist)
$	then	!Node wasn't in the old list
$		call notify "''node' joined"
$	endif
$	i = i + 1
$	if i .le. nodecount then goto loop2
$!
$ end2:
$	last_nodecount = nodecount	!Save this list of nodes to check next
$	last_nodelist = nodelist	!time around
$!
$	wait 00:00:30		!Wait half a minute
$!
$	hour = f$cvtime(,,"HOUR")	!Check the time
$! Resubmit this job at midnight to start a new logfile and thus prevent
$! the logfile from growing excessively large
$	if last_hour .gt. hour
$	then
$		p1 = nodecount
$		p2 = nodelist
$		goto submit_it
$	endif
$	last_hour = hour
$!
$	goto main_loop		!Check again
$!
$! Do VMSmail and pager notification.  P1 = message text to send
$!	Symbol 'mail_target' contains VMSmail address(es) (subject contains text)
$!!!!	Symbol 'page_target' contains pager addresses (body contains text)
$!
$ notify: subroutine
$	MAIL/NOSELF/SUBJECT="''p1'" _NLA0: 'mail_target'
$!!! PAGING DISABLED !!!
$	endsubroutine	!!! PAGING DISABLED !!!
$!!! PAGING DISABLED !!!
$	open/write temp 'temp_file'
$! Pager requires the message to be in the body, as the subject line is ignored
$	write temp "$ MAIL/NOSELF SYS$INPUT ""''page_target'"""
$	write temp p1," at ",f$time()
$	write temp "$ EXIT"
$	close temp
$!!! PAGING DISABLED !!!	@'temp_file'
$	delete 'temp_file';*
$	endsubroutine
$!
$ submit_it:
$	proc0 = f$element(0,";",f$environment("procedure"))
$	submit/queue=cluster_batch/noprint/restart-
		/param=("''p1'","''p2'") 'proc0'
$	exit 1+0*f$verify(old_verify)
