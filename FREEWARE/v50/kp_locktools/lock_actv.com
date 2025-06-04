$!
$! LOCK_ACTV.COM
$!
$! Summarize lock activity in the cluster
$!							K.Parris 12/98
$! Modification history:
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$! http://www.geocities.com/keithparris/ or http://encompasserve.org/~parris/
$	if_debug = "!"	!Set to "!" to disable debugging code, "" to enable
$!!!	if_debug = ""	!Set to "!" to disable debugging code, "" to enable
$	proc = f$environment("procedure")
$	proc_dev = f$getdvi(f$parse(proc,,,"DEVICE"),"ALLDEVNAM")
$	proc_dir = proc_dev + f$parse(proc,,,"DIRECTORY")
$	pid = f$getjpi("","PID")
$	temp_file := lock_actv_'pid'_temp.temp_file
$!!!@@@!!! goto temp_start	!!!@@@!!!
$! Gather lock activity counts from all nodes in the cluster
$	close/nolog com
$	open/write com 'temp_file'_com
$	write com "$ MCR SYSMAN SET ENVIRONMENT/CLUSTER"
$	write com "DO @",proc_dir,"LCKACT.COM"
$	write com "EXIT"
$	write com "$ EXIT"
$	close com
$	show time	!Time-stamp the output
$	write sys$output "Gathering lock rates from all cluster nodes..."
$ 'if_debug' type/page 'temp_file'_com
$	@'temp_file'_com/out='temp_file'_lis
$	delete 'temp_file'_com;*
$ 'if_debug' type/page 'temp_file'_lis
$	show time	!Time-stamp the output again
$	write sys$output "Preprocessing data..."
$!
$! Because the data-collection program reads data structures without holding
$! spinlocks (to avoid contributing to MP_SYNCH time), it is theoretically
$! possible for the data-collection program on a node to ACCVIO and die.  We
$! check for this (to flag the resultant loss of a subset of the data) by
$! pre-scanning the output file to detect any cases where this has actually
$! occurred.  (It will be rare in practice).
$!
$	close/nolog lis
$	open/read lis 'temp_file'_lis
$	node_completed = "TRUE" !Avoid false alarm the first nodename we come across
$ loop0_lis:
$	read/end=eof0_lis lis rec
$	if f$extract(0,44,rec) .eqs. -
		"%SYSMAN-I-OUTPUT, command execution on node "
$	then
$		! See if LCKACT.EXE on the last node completed successfully
$		if .not. node_completed
$		then
$			write sys$output "Node ",node," data collection failed."
$		endif
$		node = f$extract(44,6,rec)
$		node_completed = "FALSE"
$		goto loop0_lis
$	endif
$	if rec .eqs. "Done."
$	then    ! This indicates LCKACT.EXE ran to successful completion
$		node_completed = "TRUE"
$		goto loop0_lis
$	endif
$	goto loop0_lis
$ eof0_lis:
$	close lis
$	if .not. node_completed
$	then
$		write sys$output "Node ",node," data collection failed."
$	endif
$!
$! Output from LCKACT.EXE looks like this:
$!
$! 1) CSID record:
$!       CSID: xxxxxxxx
$!
$! 2) Lock activity record:
$!       nnnnn xxxxxxxx nn xxx...<hex for exactly 32 bytes>...xxx
$!         |      |      |        |
$!Fields:OACT  MSTCSID  RSNLEN RESNAME
$! (Note that since the LCKACT program dumps 32 bytes in the resource name field
$! regardless of how many are significant, this implies some of the trailing
$! bytes (positioned at the left-hand side of the RESNAM variable) are
$! unpredictable.  The RSNLEN field indicates how may bytes are valid.)
$!
$! 3) Completion confirmation record:
$!       Done.
$!
$! We create a temporary file with the lock-master node CSIDs translated to
$! nodenames and the lock-counting nodes' nodenames on the front of each record,
$! and with the resource names' extra bytes trimmed off (so we no longer need to
$! carry along the length field): 
$!       NodeNm nnnnn MstNod xxx...<hex for up to 32 bytes>...xx
$!       123456789012345678901234567890123456789
$!               1         2         3
$	open/read lis 'temp_file'_lis
$	close/nolog ls2
$	open/write ls2 'temp_file'_ls2
$ loop_lis:
$	read/end=eof_lis lis rec
$	if f$locate(f$extract(0,1,rec),"0123456789ABCDEF") .eq. 16 then -
		goto loop_lis	!Not a data record -->
$	if rec .eqs. "Done." then goto loop_lis !Not a data record -->
$	if f$extract(0,6,rec) .eqs. "CSID: "
$	then	!CSID record
$		csid = f$element(1," ",rec)
$		node = f$getsyi("NODENAME",,csid)
$	else
$		oact = f$element(0," ",rec)
$		mstcsid = f$element(1," ",rec)
$		mstnode = f$getsyi("NODENAME",,mstcsid)
$		rlen = f$integer(f$element(2," ",rec))
$		resnam = f$element(3," ",rec)
$		write ls2 f$fao("!6AS",node)," ",oact," ",-
			f$fao("!6AS",mstnode)," ",-
			f$extract(64-(2*rlen),(2*rlen),resnam)
$	endif
$	goto loop_lis
$ eof_lis:
$	close lis
$	close ls2
$	delete 'temp_file'_lis;*
$ 'if_debug' type/page 'temp_file'_ls2
$! Here, the records look like this:
$!       NodeNm Count MstNod Resource_name
$!          |     |      |       |
$!       xxxxxx nnnnn xxxxxx xxxxxxxx...<hex for up to 32 bytes>...xx
$!       123456789012345678901234567890123456789
$!               1         2         3
$!
$	show time	!Time-stamp the output again
$	write sys$output "Sorting by resource name and lock activity..."
$! Sort by resource name and (descending) lock count
$	sort/key=(pos:21,size:64)/key=(pos:8,size:5,desc) -
		'temp_file'_ls2 'temp_file'_srt
$	delete 'temp_file'_ls2;*
$!
$! Now, summarize the lock counts for each resource
$!
$	show time	!Time-stamp the output again
$	write sys$output "Summarizing lock activity cluster-wide ..."
$	close/nolog srt
$	open/read srt 'temp_file'_srt
$	close/nolog sum
$	open/write sum 'temp_file'_sum	!Summary file
$	last_resnam = ""
$ loop_srt:
$	read/end=eof_srt srt rec
$	rec = f$edit(rec,"TRIM,COMPRESS")
$	rnam = f$element(3," ",rec)
$	if rnam .nes. last_resnam
$	then
$		if last_resnam .nes. ""
$		then
$			write sum f$fao("!8ZL",total_act)," ",resnam,-
				" ",mstnode,act_sum
$		endif
$		last_resnam = rnam
$		act_sum = ""	!List of nodenames and their activity counts
$		total_act = 0	!Total lock activity from all nodes
$	endif
$	node = f$element(0," ",rec)
$	oact = f$element(1," ",rec)
$	mstnode = f$element(2," ",rec)
$	resnam = rnam
$	total_act = total_act + f$integer(oact)
$	act_sum = act_sum + " " + node + " " + oact
$	goto loop_srt
$ eof_srt:
$	close srt
$	close sum
$	delete 'temp_file'_srt;*
$! Here, the records look like this:
$!      nnnnnnnn xxxxx...xxxxx MstNode Node1 nnnnn Node2 nnnnn Node3 nnnnn ....
$!          |          |          |      |     |     |     |
$!      TotalAct ResourceName  LckMstr Node1 Count1 Node2 Count2  etc.
$!
$	show time	!Time-stamp the output again
$	write sys$output "Sorting by total lock activity..."
$! Sort by total activity count
$	sort/key=(pos:1,size:8,desc)/duplicates 'temp_file'_sum 'temp_file'_sort
$	delete 'temp_file'_sum;*
$ 'if_debug' type/page 'temp_file'_sort
$!!!@@@!! temp_start:	!!!@@@!!!
$	close/nolog sort
$	open/read sort 'temp_file'_sort
$!	close/nolog rpt
$!	open/write rpt 'temp_file'_rpt
$	show time	!Time-stamp the output
$	write sys$output "Generating report..."
$ loop_rpt:
$	read/end=eof_rpt sort rec
$! Original data is per 8-second interval.  Convert to per-second values.
$	total_act = (f$integer(f$element(0," ",rec)) + 4) / 8
$! Perhaps don't report activity counts under a low-threshold value (i.e. quit
$! after top lock-rate resources, rather than listing them all)    @@@@@
$	resnam = f$element(1," ",rec)
$	mstnode = f$element(2," ",rec)
$! Convert the resource name to its ASCII equivalent, replacing non-printable
$! characters with "." characters
$	res_ascii = ""
$	rlen = f$length(resnam)	!Length of resource name in hex-digit form
$	reslen = rlen / 2	!Length of resource name in actual bytes
$	i = 1
$ ascii_loop:
$	hexbyte = f$extract(rlen-(2*i),2,resnam)
$	hexbyte_'i' = hexbyte	!An 'array' of sorts for the hex byte values
$	byte = %x'hexbyte'
$	byte_'i' = byte		!An 'array' of sorts for the byte values
$	cbyte = "."
$	if byte .ge. 32 .and. byte .lt. 127 then cbyte[0,7] = byte  !Printable
$	res_ascii = res_ascii + cbyte
$	i = i + 1
$	if i .le. reslen then goto ascii_loop
$!
$	write sys$output ""
$	write sys$output resnam," ",res_ascii
$!
$! Try to determine what type of lock this is, and whether it is associated with
$! a particular file, disk, or node
$	if f$extract(0,4,res_ascii) .eqs. "RMS$"
$	then	!RMS lock tree
$		devlocknam = f$edit(f$extract(11,12,res_ascii),"TRIM")	!Disk volume name (usually)
$! @@@@ Here, we really need to search through all mounted Files-11 disk volumes
$! looking for the one that has a device lock name of this value, rather than
$! using this value as the volume name.  However, as a short cut, we can simply
$! check that the volume with this volume name has the same device lock name,
$! and if so, we can skip the search and use this as the volume name for the
$! call to FILE_ID_TO_NAME.COM
$		filn = hexbyte_10 + hexbyte_6 + hexbyte_5	!File number in INDEXF.SYS
$		seq = hexbyte_8 + hexbyte_7	!Sequence number
$		rvn = hexbyte_9
$		write sys$output -
			"    RMS lock tree for file [",-
				%x'filn',",",%x'seq',",",%x'rvn',-
			"] on volume ",devlocknam
$		call file_id_to_name %x'filn' DISK$'devlocknam'
$	endif
$	if f$extract(0,6,res_ascii) .eqs. "F11B$v"
$	then	!XQP Files-11 volume allocation lock
$		volnam = f$edit(f$extract(6,12,res_ascii),"TRIM")	!Disk volume name
$		write sys$output -
			"    Files-11 Volume Allocation lock for volume ",volnam
$	endif
$	if f$extract(0,6,res_ascii) .eqs. "F11B$a"
$! F11B$aVolnam123456filr
$!       123456789012
$! 1234567890123456789012
$	then	!XQP Files-11 File Access Arbitration lock
$		volnam = f$edit(f$extract(6,12,res_ascii),"TRIM")	!Disk volume name
$		filn = hexbyte_21 + hexbyte_20 + hexbyte_19
$		rvn = hexbyte_22
$		write sys$output -
			"    Files-11 File Access Arbitration lock for file [",-
			%x'filn',",*,",%x'rvn',"] on volume ",volnam
$		call file_id_to_name %x'filn' DISK$'volnam'
$	endif
$	if f$extract(0,10,res_ascii) .eqs. "SYS$SYS_ID"
$	then	!System ID Lock
$		volnam = f$extract(6,12,res_ascii)	!Disk volume name
$		scssystemid = hexbyte_12 + hexbyte_11
$		scssystemid = %x'scssystemid'
$		node_area = scssystemid / 1024
$		node_number = scssystemid - (1024 * node_area)
$		write sys$output -
			"    System ID Lock for node with SCSSYSTEMID ",-
			%x'scssystemid'," (DECnet address ",node_area,".",node_number,")"
$	endif
$!
$! Report the total lock count
$	write sys$output f$fao("        Total:!7UL",total_act)
$!
$! Report the lock counts for each of the active nodes
$	i = 3
$ node_loop:
$	node = f$element(i," ",rec)
$	if node .eqs. "" .or. node .eqs. " " then goto node_done
$	i = i + 1
$	oact = (f$integer(f$element(i," ",rec)) + 4) / 8	!Per-second rate
$	i = i + 1
$	mstflag = " "
$	if node .eqs. mstnode then mstflag = "*"  !This is the lock-master node
$	write sys$output f$fao("       !AS!6AS!7UL",mstflag,node,oact)
$	goto node_loop
$ node_done:
$	goto loop_rpt
$!
$ eof_rpt:
$	close sort
$!	close rpt
$!!! rename 'temp_file'_sort lock_actv.sort;
$	delete 'temp_file'_sort;*
$	write sys$output ""
$	write sys$output "       * Lock Master Node for the resource"
$	write sys$output ""
$	write sys$output ""
$	show time
$	write sys$output "All done."
$!
$	exit
$!
$! Given a file ID and disk device name, find out the file name
$!
$!	P1: File number from File ID (decimal, or hex with %X prefix)
$!	P2: Disk device on which this file is located
$!
$!	Return file specification in global symobl 'filespec'
$!
$ file_id_to_name: subroutine
$	old_verify = f$verify('f$trnlnm("file_id_to_name$verify")'+0)
$	if .not. f$getdvi("''p2'","EXISTS")
$	then
$		write sys$output "    Can't find device ''p2'"
$		goto error_exit
$	endif
$	if f$locate(":",p2) .ne. f$length(p2) then p2 = p2 - ":"
$! Initialization
$	debug = "!"	! "!" to disable debug messages; "" to enable messages
$	pid = f$getjpi("","PID")
$	temp_file := file_id_to_name_temp_'pid'.temp_file
$	maxfiles = f$getdvi("''p2'","MAXFILES")
$	cluster = f$getdvi("''p2'","CLUSTER")
$! Look up the file itself
$	call lookup_file 'p1' 'p2'
$	if filename .eqs. "?.?;?" then goto error_exit
$	initial_filename = filename	!Remember this filespec
$! Look up its directory tree
$	filespec = "]" + filename
$ dir_loop:
$	call lookup_file 'dir_file_id' 'p2'
$	if filename .eqs. "?.?;?" then goto error_exit
$	if filename .eqs. "000000.DIR;1" then goto dir_done
$! If we're just backtracking from extension file headers, keep on looking
$	if filename .eqs. initial_filename .and. -
    f$locate(".DIR;1",filename) .eq. f$length(filename) then goto dir_loop
$	dot = "."
$	if f$extract(0,1,filespec) .eqs. "]" then dot = ""
$	filespec = (filename - ".DIR;1") + dot + filespec
$	goto dir_loop
$ dir_done:
$	if f$extract(0,1,filespec) .eqs. "]" then -
		filespec = "000000" + filespec	!File is in the MFD
$	filespec == p2 + ":[" + filespec
$ !!!	write sys$output "    File specification: ",filespec
$ 	goto done
$ error_exit:
$	filespec == ""
$ done:
$	if old_verify then set verify
$	endsubroutine
$!
$! Look up a file by File ID and return its name and the File ID of the
$! directory it is in
$!
$ lookup_file: subroutine
$	x = ((maxfiles+4095) / 4096) + (4 * cluster) + 'p1'
$	dump/file_header/block=(start:'x',count:1)/output='temp_file' -
		'p2':[000000]indexf.sys
$'debug' type/page 'temp_file';
$	define/user sys$error _nla0:
$	define/user sys$output _nla0:
$	search 'temp_file' "File name:","Back link file identification:"-
		/out='temp_file'_1
$	search_severity = $severity	!Save search result: success=1, fail=3
$	delete 'temp_file';*	!Delete temporary file
$	if search_severity .eq. 3 then goto err	!Didn't find the info we need ->
$	close/nolog temp
$'debug' type/page 'temp_file'_1;
$	open/read temp 'temp_file'_1
$! Read the Directory Backlink record
$	read/end=eof temp rec
$	rec = f$edit(rec,"compress,trim")
$	if f$extract(0,9,rec) .eqs. "Back link"
$	then
$	    dir_backlink = f$element(4," ",rec) - "(" - ")"
$'debug' show symbol dir_backlink	!!!
$	    dir_file_id == f$element(0,",",dir_backlink)	!File number
$'debug' show symbol dir_file_id	!!!
$	else
$	    goto err
$	endif
$! Read the File Name record
$	read/end=eof temp rec
$	rec = f$edit(rec,"compress,trim")
$	if f$extract(0,10,rec) .eqs. "File name:"
$	then
$	    filename == f$element(2," ",rec)
$'debug' show symbol filename	!!!
$	else
$	    goto err
$	endif
$	close temp
$	delete 'temp_file'_1;*
$	goto xit	!Normal exit -->
$ eof:	!Premature end-of-file
$	close temp
$ err:	!Error occurred which prevented looking up the filename and/or directory
$	write sys$output "    Error looking up File Number ",p1," on disk ",p2
$	dir_file_id == "[?]"
$	filename == "?.?;?"
$ xit:
$	endsubroutine
