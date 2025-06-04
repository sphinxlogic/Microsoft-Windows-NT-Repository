$!
$! Show the preferred paths being used by PEDRIVER to each remote node
$!
$!						V1.0 4/96
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$! http://www.geocities.com/keithparris/ or http://encompasserve.org/~parris/
$!
$! First, find out the names of all the nodes in the cluster
$	this_node = f$getsyi("NODENAME")	!This is where we're running
$	nodecount = 0
$	context = ""
$ loop:
$	csid = f$csid(context)	!Get the CSID of the next cluster member
$	if csid .nes. ""
$	then
$	    nodecount = nodecount + 1	!1 more node
$	    nodename_'nodecount' = f$getsyi("NODENAME",,csid)
$	    goto loop
$	endif
$! Make temporary filenames unique by PID
$	this_PID = f$getjpi("","PID")
$	this_node = f$getsyi("NODENAME")	!This is where we're running
$! Next, use SDA to find out the channel addresses of the preferred paths
$! to each node
$	close/nolog com
$	open/write com show_pref_paths_temp_'this_PID'.com
$	write com "$ DEFINE/USER SYS$OUTPUT NLA0:"
$	write com "$ DEFINE/USER SYS$ERROR NLA0:"
$	write com "$ ANALYZE/SYSTEM"
$	write com "SET OUTPUT NLA0:"
$	write com "SHOW PORTS"	!To set symbols for use later
$	write com "SET OUTPUT show_pref_paths_temp_''this_PID'.LIST"
$	i = 0
$ loop2:
$	i = i + 1
$!(Don't bother to look up a preferred path to this node itself)
$	if nodename_'i' .nes. this_node then-
		write com "SHOW PORT/ADDRESS=PE_PDT/VC/NODE=",nodename_'i'
$	if i .lt. nodecount then goto loop2
$	write com "EXIT"
$	write com "$ EXIT"
$	close com
$	@show_pref_paths_temp_'this_PID'.com
$	delete show_pref_paths_temp_'this_PID'.com;*	!!!
$	search show_pref_paths_temp_'this_PID'.list "Preferred Channel "-
		/out=show_pref_paths_temp_'this_PID'.channels
$	delete show_pref_paths_temp_'this_PID'.list;*	!!!
$	close/nolog chan
$! Now use SDA again to find out the "bus" (LAN adapter) which is currently
$! used in the preferred path to each node
$	open/write com show_pref_paths_temp_'this_PID'.com2
$	write com "$ DEFINE/USER SYS$OUTPUT NLA0:"
$	write com "$ DEFINE/USER SYS$ERROR NLA0:"
$	write com "$ ANALYZE/SYSTEM"
$	write com "SET OUTPUT show_pref_paths_temp_''this_PID'.LIST2"
$	open/read chan show_pref_paths_temp_'this_PID'.channels
$ loop3:
$	read/end=eof3 chan rec
$	rec = f$edit(rec,"COMPRESS,TRIM")
$	t = f$locate("Preferred Channel ",rec)
$	chan_addr = f$extract(t+f$length("Preferred Channel "),8,rec)
$	if chan_addr .nes. "00000000" then -
		write com "SHOW PORT/CHANNEL=",chan_addr
$	goto loop3
$ eof3:
$	close chan
$	delete show_pref_paths_temp_'this_PID'.channels;*	!!!
$	close com
$	@show_pref_paths_temp_'this_PID'.com2
$	delete show_pref_paths_temp_'this_PID'.com2;*	!!!
$! Show the user what we found
$	search show_pref_paths_temp_'this_PID'.list2 -
		"PEDRIVER Channel","Lcl Device:","Rmt Device:"
$	delete show_pref_paths_temp_'this_PID'.list2;*	!!!
$!
$	exit
