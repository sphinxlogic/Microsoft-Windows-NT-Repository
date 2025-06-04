$!
$! LCKQUE.COM
$!
$! Summarize lock queues in the cluster
$!							K.Parris 01/2000
$! Modification history:
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$! http://www.geocities.com/keithparris/ or http://encompasserve.org/~parris/
$	if_debug = "!"	!Set to "!" to disable debugging code, "" to enable
$!!!	if_debug = ""	!Set to "!" to disable debugging code, "" to enable
$	proc = f$environment("procedure")
$	proc_dev = f$parse(proc,,,"DEVICE")
$	proc_dir = proc_dev + f$parse(proc,,,"DIRECTORY")
$	pid = f$getjpi("","PID")
$	temp_file := LCKQUE_'pid'_temp.temp_file
$!!!'if_debug' temp_file := LCKQUE_temp.temp_file	!!! for testing
$	date_time = f$time()	!Grab a timestamp
$	date = f$cvtime(date_time,"COMPARISON","DATE") - "-" - "-"
$	time = f$cvtime(date_time,"COMPARISON","TIME")
$	hhmm = f$element(0,":",time) + f$element(1,":",time)	!HoursMinutes
$	output_file := LCKQUE_'date'_'hhmm'.RPT
$	output_dir = proc_dir	!!!:= DISK$APP3:[KPARRIS.LCKACT]
$!!!	show symbol date_time	!Time-stamp the output
$!!!	show symbol output_file	!Show output file specification
$	spaces = -	!64 blank spaces
   "                                                                "
$!   1234567890123456789012345678901234567890123456789012345678901234
$!            1         2         3         4         5         6
$	spaces = spaces + spaces	!128 spaces
$	spaces = spaces + spaces	!256 spaces
$!!!'if_debug' goto temp_start	!!!@@@!!!
$! Gather lock queue lengths from all nodes in the cluster
$	close/nolog com
$	open/write com 'temp_file'_com
$	write com "$ SET NOON"
$	write com "$ MCR SYSMAN SET ENVIRONMENT/CLUSTER"
$!!!	write com "SET TIMEOUT 0:0:3"	!Give up after 3 seconds
$	write com "SET TIMEOUT 0:0:10"	!Give up after 10 seconds
$	write com "DO @",proc_dir,"LCKQUE_RUN"
$	write com "EXIT"
$	write com "$ EXIT 1"
$	close com
$!!! show time	!Time-stamp the output
$	write sys$output "Gathering lock queues from all cluster nodes..."
$ 'if_debug' type/page 'temp_file'_com
$	@'temp_file'_com/out='temp_file'_lis
$	delete 'temp_file'_com;*
$ 'if_debug' type/page 'temp_file'_lis
$ temp_start:	!!! for testing
$!!!	write sys$output ""
$!!! show time	!Time-stamp the output again
$	write sys$output "Post-processing data..."
$!
$! Output from LCKQUE.EXE looks like this:
$!    Lock queue record:
$!         nnn nnnnnnnnnn nnnnnnnnnn nnn xxx...<hex for exactly 32 bytes>...xxx
$!          |      |          |       |       |
$!Fields: Depth ConvQueue WaitQueue RsnLen ResNamX
$! where ConvQueue is the number of lock blocks hanging off the conversion
$! queue waiting for a conversion request to be granted, and WaitQueue is the
$! numbe of new lock request lock blocks hanging off the wait queue.
$! If the resource is not a root resource, one or more parent resource name
$! records immediately precedes the resource name with queue lengths:
$!    Parent resource name record:
$!         nnn (Parent resource:)    nnn xxx...<hex for exactly 32 bytes>...xxx
$!          |      |          |       |       |
$!Fields: Depth ConvQueue WaitQueue RsnLen ResNamX
$! One parent resource record is output for each level above the resource on
$! which we see queues (to fully specify the resource hierarchy).  The root
$! resource name is output first.
$!
$! (Note that since the LCKQUE program dumps 32 bytes in the resource name field
$! regardless of how many are significant, this implies some of the trailing
$! bytes (positioned at the left-hand side of the ResNam variable) are
$! unpredictable.  The RsnLen field indicates how may bytes are valid.)
$!
$!
$!!The following section is not correct at present:
$!! We create an output file with the nodes' nodenames on the front of each
$!! record, and with the resource names for each level in the hierarchy included.
$!! and with the resource names' extra hex bytes
$!! trimmed off (so we no longer need to carry along the length fields):
$!!     NodeNm nnnnnnnnnn nnnnnnnnnn xxx...<hex for up to 32 bytes for each level>...xxx/xxx...xxx
$!!        |        |          |       |
$!!   Nodename  ConvQueue  WaitQueue  Resource name hierarchy: root/leaf1/leaf2
$!!     123456789012345678901234567890123456789
$!!              1         2         3
$!!
$	close/nolog lis
$	open/read lis 'temp_file'_lis
$	close/nolog ls2
$	open/write ls2 'output_file'
$	write ls2 "Time: ",f$extract(0,2,hhmm),":",f$extract(2,2,hhmm)
$	node_completed = "TRUE"	!Avoid false alarm the first nodename we come across
$ loop_lis:
$	read/end=eof_lis lis rec
$	if f$extract(0,44,rec) .eqs. -
		"%SYSMAN-I-OUTPUT, command execution on node "
$	then
$		! See if LCKQUE on the last node completed successfully
$		if .not. node_completed
$		then
$			write ls2 "Node ",node," data collection failed."
$		endif
$		node = f$extract(44,6,rec)
$		node_heading = "FALSE"	!Haven't yet output nodename heading
$		node_completed = "FALSE"
$		goto loop_lis
$	endif
$	if rec .eqs. "LCKQUE done."
$	then	! This indicates LCKQUE.EXE ran to successful completion
$		node_completed = "TRUE"
$		goto loop_lis
$	endif
$!	if f$locate(f$extract(0,1,rec),"0123456789 ") .eq. 11 then -
$!_!		goto loop_lis	!Not a data record -->
$	if f$length(rec) .lt. 25 then goto loop_lis	!Not a data record -->
$! Check Depth field to weed out SYSMAN messages from log
$	if f$locate(f$extract(2,1,rec),"0123456789") .eq. 10 then -
		goto loop_lis	!Not a data record -->
$!!!'if_debug' show symbol rec	!!! for testing
$	crec = f$edit(rec,"COMPRESS,TRIM")
$	depth = f$element(0," ",crec)
$	ConvQueue = f$element(1," ",crec)
$	WaitQueue = f$element(2," ",crec)
$	RsnLen = f$integer(f$element(3," ",crec)) !Actual resource name length (bytes)
$	ResNamX = f$element(4," ",crec)	!Resource name (fixed 32 bytes)
$	hxrlen = RsnLen*2		!Length of resource name in hex-digit form
$	resnam = f$extract(64-hxrlen,hxrlen,ResNamX)	!Trim leading junk off
$!
$! Convert the resource name to its ASCII equivalent, replacing non-printable
$! characters with "." characters
$	res_ascii = ""
$	i = 1
$ ascii_loop:
$	hexbyte = f$extract(hxrlen-(2*i),2,resnam)
$	hexbyte_'i' = hexbyte	!An 'array' of sorts for the hex byte values
$	byte = %x'hexbyte'
$	byte_'i' = byte		!An 'array' of sorts for the byte values
$	cbyte = "."
$	if byte .ge. 32 .and. byte .lt. 127 then cbyte[0,7] = byte  !Printable
$	res_ascii = res_ascii + cbyte
$	i = i + 1
$	if i .le. RsnLen then goto ascii_loop
$!
$! Filter out dead-man locks that are always waiting and are thus of no interest
$!!! goto past_filter  !!!Uncomment to see every lock queue, even standing ones
$	if depth .eq. 0 .and. ConvQueue .nes. "(Parent"
$	then	!Have a root lock here
$		if f$extract(0,5,res_ascii) .eqs. "VCC$v" then goto loop_lis
$		if f$extract(0,8,res_ascii) .eqs. "CACHE$cm" then goto loop_lis
$		if f$extract(0,4,res_ascii) .eqs. "$DSA" .and. -
            f$extract(RsnLen-8,8,res_ascii) .eqs. "_$WATCHR" then goto loop_lis
$		if f$extract(0,7,res_ascii) .eqs. "IOGEN$_" then goto loop_lis
$		if f$extract(0,7,res_ascii) .eqs. "AUDRSV$" then goto loop_lis
$		if f$extract(0,7,res_ascii) .eqs. "PSCDNS_" then goto loop_lis
$	endif
$	if (depth .eq. 0 .and. f$extract(0,9,res_ascii) .eqs. "QMAN$MSR_") -
      .or. (depth .eq. 1 .and. res_ascii .eqs. "QMAN$JBC_ALIVE_01") -
		then goto loop_lis
$	if (depth .eq. 0 .and. f$extract(0,11,res_ascii) .eqs. "CLU$NETACP_") -
      .or. (depth .eq. 1 .and. f$extract(0,4,res_ascii) .eqs. "IDL_") -
		then goto loop_lis
$ past_filter:
$! End of filter section
$!
$! Output a nodename heading if we haven't already
$	if .not. node_heading
$	then
$		write ls2 ""	!Blank line
$		write ls2 "-----  ",node,":  -----"	!Nodename
$		node_heading = "TRUE"	!Now we've output a heading
$	endif
$!
$! Display the resource name in ASCII and hexadecimal
$	if depth .eq. 0 then write ls2 ""
$	write ls2 f$extract(0,depth,spaces),-
		"'",res_ascii,"' ",resnam
$!		resnam," ",res_ascii
$!
$	if ConvQueue .eqs. "(Parent" .and. WaitQueue .eqs. "Resource:)"
$	then	!Parent lock
$!		write ls2 f$extract(0,depth,spaces),-
$!_!			f$extract(64-hxrlen,hxrlen,resnam) + " /"
$		RsnLen_'depth' = RsnLen	!Remember parent locks by level
$		resnam_'depth' = resnam
$		res_ascii_'depth' = res_ascii
$	else	!Lock with queue(s)
$		if depth .eq. 1		!Child lock of a root lock
$		then
$		    if f$extract(0,4,res_ascii_0) .eqs. "RMS$"
$		    then	!Child of an RMS File Lock
$			if RsnLen .eq. 4 then -
$				write ls2 f$extract(0,depth,spaces),-
			"  RMS bucket lock, on bucket starting at VBN ",-
					%x'resnam'
$			if RsnLen .eq. 8 then -
$				write ls2 f$extract(0,depth,spaces),-
			"  RMS record lock, on RFA %x",-
					resnam
$		    endif
$	    	    if f$extract(0,6,res_ascii_0) .eqs. "F11B$v"
$		    then	!Child of a Files-11 Volume Allocation Lock
$	    		if f$extract(0,6,res_ascii) .eqs. "F11B$s"
$		        then	!XQP Files-11 File Serialization lock:
$!					 F11B$s{lockbasis}
$!					       1234
$!					 1234567890
$			    volnam = f$edit(f$extract(6,12,res_ascii_0),"TRIM")	!Disk volume name
$			    filn = hexbyte_9 + hexbyte_8 + hexbyte_7
$			    rvn = hexbyte_10
$			    write ls2 -
			"    Files-11 File Serialization lock for file [",-
			%x'filn',",*,",%x'rvn',"] on volume ",volnam
$			    call file_id_to_name %x'filn' DISK$'volnam'
$			    write ls2 "    File specification: ",filespec
$			endif
$		    endif
$		    if f$extract(0,10,res_ascii_0) .eqs. "SYS$SYS_ID"
$		    then	!Child of a System ID Lock
$			if f$extract(0,4,res_ascii) .eqs. "RMS$"
$			then
$			    devlocknam = f$edit(f$extract(11,12,res_ascii),"TRIM")	!Disk volume name (usually)
$! @@@@ Here, we really need to search through all mounted Files-11 disk volumes
$! looking for the one that has a device lock name of this value, rather than
$! using this value as the volume name.  However, as a short cut, we can simply
$! check that the volume with this volume name has the same device lock name,
$! and if so, we can skip the search and use this as the volume name for the
$! call to FILE_ID_TO_NAME.COM
$			    filn = hexbyte_10 + hexbyte_6 + hexbyte_5	!File number in INDEXF.SYS
$			    seq = hexbyte_8 + hexbyte_7	!Sequence number
$			    rvn = hexbyte_9
$			    write ls2 f$extract(0,depth,spaces),-
				"  RMS Global Buffer Section Lock for file [",-
				%x'filn',",",%x'seq',",",%x'rvn',-
				"] on volume ",devlocknam
$			    call file_id_to_name %x'filn' DISK$'devlocknam'
$			    write ls2 "    File specification: ",filespec
$		        endif
$		    endif
$		endif	!End of decoding for 1-parent locks
$		write ls2 f$extract(0,depth,spaces),-
-!			f$extract(64-hxrlen,hxrlen,resnam)
	f$fao(" Convert queue: !AS, Wait queue: !AS",ConvQueue,WaitQueue) !," ",-
$!			ResNamX
$	endif
$!
$!
$!
$	if depth .eq. 0
$	then	!Root lock
$! Try to determine what type of lock this is, and whether it is associated with
$! a particular file, disk, or node
$	    if f$extract(0,4,res_ascii) .eqs. "RMS$"
$	    then	!RMS lock tree
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
$		write ls2 -
			"    RMS lock tree for file [",-
				%x'filn',",",%x'seq',",",%x'rvn',-
			"] on volume ",devlocknam
$		call file_id_to_name %x'filn' DISK$'devlocknam'
$		write ls2 "    File specification: ",filespec
$	    endif
$	    if f$extract(0,6,res_ascii) .eqs. "F11B$v"
$	    then	!XQP Files-11 volume allocation lock
$		volnam = f$edit(f$extract(6,12,res_ascii),"TRIM")	!Disk volume name
$		write ls2 -
			"    Files-11 Volume Allocation lock for volume ",volnam
$	    endif
$	    if f$extract(0,6,res_ascii) .eqs. "F11B$a"
$! F11B$aVolnam123456filr
$!       123456789012
$! 1234567890123456789012
$	    then	!XQP Files-11 File Access Arbitration lock
$		volnam = f$edit(f$extract(6,12,res_ascii),"TRIM")	!Disk volume name
$		filn = hexbyte_21 + hexbyte_20 + hexbyte_19
$		rvn = hexbyte_22
$		write ls2 -
			"    Files-11 File Access Arbitration lock for file [",-
			%x'filn',",*,",%x'rvn',"] on volume ",volnam
$		call file_id_to_name %x'filn' DISK$'volnam'
$		write ls2 "    File specification: ",filespec
$	    endif
$	    if f$extract(0,10,res_ascii) .eqs. "SYS$SYS_ID"
$	    then	!System ID Lock
$		volnam = f$extract(6,12,res_ascii)	!Disk volume name
$		scssystemid = hexbyte_12 + hexbyte_11
$		scssystemid = %x'scssystemid'
$		node_area = scssystemid / 1024
$		node_number = scssystemid - (1024 * node_area)
$		write ls2 -
			"    System ID Lock for node with SCSSYSTEMID ",-
			%x'scssystemid'," (DECnet address ",node_area,".",node_number,")"
$	    endif
$	endif	!End of decoding for root locks
$!
$	goto loop_lis
$!
$!
$!
$ eof_lis:
$	if .not. node_completed
$	then
$		write ls2 "Node ",node," data collection failed."
$	endif
$	close lis
$	close ls2
$	delete 'temp_file'_lis;*	!!!! Save output
$ type 'output_file'
$!
$!
$!!! show time
$	write sys$output -
"================================== All done. =================================="
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
