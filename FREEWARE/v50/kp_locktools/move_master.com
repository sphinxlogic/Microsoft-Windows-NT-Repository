$	old_verify = f$verify('f$trnlnm("MOVE_MASTER$VERIFY")'+0)
$!
$! Move lock mastership of lock tree from another node to this node
$!
$!	P1:	Type of resource: FILE or RESOURCE
$!	P2:	Name of file or resource
$!						K.Parris 04/28/98
$! Assumes each node has a batch queue with a name of the form nodename_BATCH.
$! Assumes each node normally runs with positive non-zero PE1 values (see
$! 'threshold' DCL symbol definition below.
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$! http://www.geocities.com/keithparris/ or http://encompasserve.org/~parris/
$	page_target := node1::system	!Who do you call when things break?
$!
$	proc = f$environment("procedure")
$	proc_dir = f$parse(proc,,,"DEVICE") + f$parse(proc,,,"DIRECTORY")
$!
$	this_node = f$getsyi("NODENAME")
$ show symbol this_node
$	if p1 .nes. "FILE" .and. p1 .nes. "RESOURCE"
$	then
$		write sys$output "P1 must specify FILE or RESOURCE."
$		exit 1+0*f$verify(old_verify)
$	endif
$	p1_lower = f$edit(p1,"LOWERCASE")
$	if p2 .eqs. ""
$	then
$		write sys$output "P2 must specify the name of a ",p1_lower,"."
$		exit 1+0*f$verify(old_verify)
$	endif
$ show time
$	proc = f$environment("procedure")
$	proc_dir = f$parse(proc,,,"DEVICE") + f$parse(proc,,,"DIRECTORY")
$! Determine where the file or resource tree is lock-mastered now
$	@'proc_dir'findmaster_'p1' 'p2'
$! This sets global symbol FINDMASTER_NODE to be the name of the master node
$ show symbol FINDMASTER_NODE
$! Quit if the file doesn't have a lock master (and can thus migrate here if
$! needed), or if it's already mastered on this node
$	if FINDMASTER_NODE .eqs. this_node .or. -
	   FINDMASTER_NODE .eqs. "" then exit
$! Ensure that the current master node will allow outgoing remastering
$! operations, by setting its PE1 parameter to zero.
$! (In theory, we should only set PE1 to 0 if it isn't already, and set it back
$!  to 100 afterward if it was at 100 before, but there's no quick and easy
$!  way to find out the value of a system parameter on a remote node, so we'll
$!  just assume that we enter this procedure with PE1 set to 100 on all nodes,
$!  and try to leave it that way when we exit.)
$	mcr sysman set env/node=('FINDMASTER_NODE')
set profile/priv=all
param use active
param show pe1
param set pe1 0
param write active
param use active
param show pe1
exit
$! Now run the LOTSALOX program to induce lock mastership to move to this node
$!!	item_name = p2 - ":" - "[" - "]" - "<" - ">" - "." - ";"
$!!	job_name = f$extract(0,39,"ForceMaster_''p1'_''item_name'")
$	p2min = f$extract(0,20,p2 - ":" - "[" - "]" - "<" - ">" - "." - ";")
$	job_name = f$extract(0,39,"FM_''p1'_''p2min'")
$	submit/noprint/queue='this_node'_batch/param=("''p2'")-
		/name="''job_name'" -
		/log=sys$login:force_master_'p1'.log-
		'proc_dir'force_master_'p1'	!'proc_dir'force_master_'p1'
$	fm_entry = $entry	!Save the entry number in case we have to bail
$! Ensure that if the lock mastership moved between the time we set PE1 to zero
$! and the time we started inducing it to move to this node, that we enable
$! that node to give it up to us.  We do that by keeping a list of nodes on
$! which we've set PE1 to zero, and ensuring that we set PE1 to 0 and add any
$! additional node(s) to the list if the lock tree shifts to any other node(s).
$	pe1_nodes = "," + FINDMASTER_NODE+ ","	!Remember the node we set PE1 to zero on
$	wait 00:00:08	!It will take a while for the lock master to start remastering
$	seconds = 8	!Keep track of how long we've waited; give up after 10 minutes
$ loop:
$ show time
$! See which node the lock tree is mastered on now
$	@'proc_dir'findmaster_'p1' 'p2'
$ show symbol FINDMASTER_NODE
$! Quit if the file doesn't have a lock master (and thus can migrate here), or
$! if it's already mastered on this node
$	if FINDMASTER_NODE .eqs. this_node .or. -
	   FINDMASTER_NODE .eqs. "" then goto cleanup
$! If the lock tree is now mastered on a different node, we need to set PE1
$! to zero on that node also
$	if f$locate(","+FINDMASTER_NODE+",",pe1_nodes) .eq. f$length(pe1_nodes)
$	then
$! Ensure that the current master node will allow outgoing remastering
$! operations, by setting its PE1 parameter to zero.
$		mcr sysman set env/node=('FINDMASTER_NODE')
set profile/priv=all
param use active
param show pe1
param set pe1 0
param write active
param use active
param show pe1
exit
$		pe1_nodes = pe1_nodes + FINDMASTER_NODE + ","	!Add it to list
$	endif
$	wait 00:00:01
$	seconds = seconds + 1
$!!!	if seconds .le. 90 then goto loop
$!!!	if seconds .le. 300 then goto loop	!In case it takes longer
$	if seconds .le. 600 then goto loop	!In case it takes even longer
$ cleanup:
$ show symbol seconds
$!!	wait 00:00:02	!Give job time to exit by itself after remastering
$!			!before we check on it and kill it here
$	wait 00:00:15	!A bit more time   KP 1/10/99
$	if f$getqui("DISPLAY_ENTRY","JOB_EXECUTING","''fm_entry'")
$	then	!Job is still hanging around for some reason, so kill it
$		delete/entry='fm_entry'
$! Send mail that the job apparently failed to exit as expected
$		mail/subj=-
"REMASTER_''p1' ''p2' to ''this_node' from ''FINDMASTER_NODE' job did not exit."-
		_NLA0: kparris
$!! We don't page this one anymore because MASTER_CONTROL always does, too  KP 6/30/00
$!!		@'proc_dir'pager "''page_target'" -
$!_!"REMASTER_''p1' ''p2' to ''this_node' from ''FINDMASTER_NODE' job did not exit."
$	endif
$! Double-check that we got the lock mastership
$	@'proc_dir'findmaster_'p1' 'p2'
$ show symbol FINDMASTER_NODE
$	if FINDMASTER_NODE .nes. THIS_NODE .and. FINDMASTER_NODE .nes. ""
$	then	!Failed to get the lock mastership onto this node
$! Send mail that we failed to remaster the tree here
$		mail/subj=-
"Failed to remaster ''p1_lower' ''p2' to ''this_node' from ''FINDMASTER_NODE'"-
		_NLA0: kparris
$		p1_lower = f$edit(p1,"LOWERCASE")
$!! We don't page this one anymore because MASTER_CONTROL always does, too  KP 6/30/00
$!!		@'proc_dir'pager "''page_target'" -
$!_!"Failed to remaster ''p1_lower' ''p2' to ''this_node' from ''FINDMASTER_NODE'"
$	endif
$! Disable lock remastering on the node(s) where we freed it up by setting PE1
$! back to the threshold value (a relatively low positive value, e.g. 100)
$	pe1_nodes = pe1_nodes - ","	!Remove leading comma
$	pe1_nodes = f$extract(0,f$length(pe1_nodes)-1,pe1_nodes) !Remove trailing comma
$! Find the threshold value we're currently using to prevent lock remastering
$	x = f$trnlnm("REMASTER$PE1_THRESHOLD")
$	threshold = 100 !Default value of PE1 used to discourage lock remastering
$	if x .nes. "" then threshold = 'x'
$	pid = f$getjpi("","PID")
$	temp_file := move_master_temp_file_'pid'.temp_file
$	close/nolog com
$	open/write com 'temp_file'_com
$	write com "$ mcr sysman set env/node=(''pe1_nodes')"
$	write com "set profile/priv=all"	!Get around 7.2 SYSMAN change
$	write com "param use active"
$	write com "param show pe1"
$	write com "param set pe1 ",threshold
$	write com "param write active"
$	write com "param use active"
$	write com "param show pe1"
$	write com "exit"
$	write com "$ exit"
$	close com
$	@'temp_file'_com
$	delete 'temp_file'_com;*
$!
$	exit 1+0*f$verify(old_verify)
