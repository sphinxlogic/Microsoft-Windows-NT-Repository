$!
$! Sift through OPERATOR.LOG files and create a time-sorted, consolidated list
$! of %LAVC console messages in the format of:
$!     dd-mmm-yyyy hh:mm:ss.cc nodexx %LAVC-s-ident, text
$!
$!  Optional P1 parameter:
$!	ALL		Retrieve messages from all accessible operator log files
$!	RECENT		Retrieve messages from most-recent operator log file
$!	YESTERDAY	Retrieve messages from files created since yesterday
$!							K.Parris    Jan. 2000
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$!   http://encompasserve.org/~parris/ or http://www.geocities.com/keithparris/ 
$!
$	proc = f$element(0,";",f$environment("PROCEDURE"))
$	proc_dir = f$parse(proc,,,"DEVICE") + f$parse(proc,,,"DIRECTORY")
$	pid = f$getjpi("","PID")
$	temp_file := sift_lavc_temp_'pid'.temp_file
$!
$	if p1 .eqs. "RECURSIVE"
$	then	!Calling ourselves recursively
$		CALL SUB_WORK 'P2'	!Search operator log file(s)
$		exit
$	endif
$ if p1 .eqs. "TEST" then goto start	!!!
$!
$	close/nolog com
$	open/write com 'temp_file'_COM
$	write com "$ MCR SYSMAN SET ENVIRONMENT/CLUSTER"
$	write com "do @",proc_dir,"SIFT_LAVC RECURSIVE ",p1
$	write com "exit"
$	write com "$ EXIT"
$	close com
$ !!! type/page 'temp_file'_COM	!!!
$	@'temp_file'_COM/out='temp_file'_OPC
$	delete 'temp_file'_COM;*
$ start: !!!
$!
$! What we're sifting here looks like the following:
$!
$! %SYSMAN-I-OUTPUT, command execution on node ETGA04
$! ******************************
$! SYS$SYSROOT:[SYSMGR]OPERATOR.LOG;141
$! %%%%%%%%%%%  OPCOM  26-MAY-1998 14:31:12.82  %%%%%%%%%%%
$! Message from user SYSTEM on ETGA04
$! %LAVC-S-WORKING, ETGA09 - Alpha 8400; Palo Alto
$! ***************
$! %%%%%%%%%%%  OPCOM  26-MAY-1998 14:31:12.82  %%%%%%%%%%%    (from node ETGA07 at 26-MAY-1998 14:31:13.57)
$! Message from user SYSTEM on ETGA07
$! %LAVC-S-WORKING, ETGA09 - Alpha 8400; Palo Alto
$! 
$	close/nolog temp
$	open/read temp 'temp_file'_OPC
$	close/nolog out
$	open/write out 'temp_file'_OUT
$	sysman_node = "??????"
$ loop:
$	read/end=eof temp rec
$	crec = f$edit(rec,"TRIM,COMPRESS")
$! %SYSMAN-I-OUTPUT, command execution on node ETGA04
$	if f$element(0," ",rec) .eqs. "%SYSMAN-I-OUTPUT,"
$	then
$		sysman_node = f$element(5," ",rec)
$ show symbol sysman_node	!!!
$		goto loop
$	endif
$! ******************************
$! ***************
$! 123456789.123456789.123456789.
$	if f$extract(0,15,rec) .eqs. "***************" then goto loop
$! Filespec line from $SEARCH
$	crlen = f$length(crec)
$	if f$locate(":[",crec) .ne. crlen .and. f$locate("]",crec) .ne. crlen then -
		goto loop
$! %%%%%%%%%%%  OPCOM  26-MAY-1998 14:31:12.82  %%%%%%%%%%%
$! %%%%%%%%%%%  OPCOM  26-MAY-1998 14:31:12.82  %%%%%%%%%%%    (from node ETGA07 at 26-MAY-1998 14:31:13.57)
$! 0123456789.123456789.123456789.
$!                     123456789.123456789.123
$	if f$extract(0,18,rec) .eqs. "%%%%%%%%%%%  OPCOM"
$	then
$		timestamp = f$cvtime(f$extract(20,23,rec),"COMPARISON")
$!!! show symbol timestamp	!!!
$		this_node = sysman_node
$! %%%%%%%%%%% OPCOM 26-MAY-1998 14:31:12.82 %%%%%%%%%%% (from node ETGA07 at 26-MAY-1998 14:31:13.57)
$		if f$locate("from node ",crec) .ne. crlen then -
			this_node = f$element(7," ",crec)
$!!! show symbol this_node	!!!
$		goto loop
$	endif
$! Message from user SYSTEM on ETGA04
$! 123456789.123456789.123456789.
$	if f$extract(0,27,rec) .eqs. "Message from user SYSTEM on"
$	then
$		t_node = f$element(5," ",rec)
$		if t_node .nes. this_node
$		then
$			write sys$output "Mismatch between t_node/this_node: ''t_node'/''this_node'"
$		endif
$		goto loop
$	endif
$! %LAVC-S-WORKING, ETGA09 - Alpha 8400; Palo Alto
$	if f$extract(0,5,rec) .eqs. "%LAVC"
$	then
$		write out timestamp," ",this_node," ",rec
$!!!		write sys$output timestamp," ",this_node," ",rec	!!!
$!!! show symbol rec	!!!
$		goto loop
$	endif
$! Anything else
$	write sys$output "Unrecognized line ",rec
$	goto loop
$ eof:
$	close temp
$	delete 'temp_file'_OPC;*
$	close out
$!
$! Sort output in order of time and nodename
$!!	sort/key=(pos:1,size:23)/key=(pos:25,size:6)//dupl -
$	sort/key=(pos:1,size:48)/nodupl -
		 'temp_file'_OUT SIFT_LAVC.SRT
$! /DUP? /NODUP?
$!
$ !!!type/page sift_lavc.srt
$!
$!	if f$search("''temp_file'_%%%") .nes. "" then -
$!_!		delete 'temp_file'_%%%;*
$	exit

$!
$! Search operator log files for %LAVC messages
$!
$ SUB_WORK: SUBROUTINE
$	versions = ";*"
$	if p1 .eqs. "RECENT" then versions = ";0"
$	if p1 .eqs. "YESTERDAY" then versions = ";*/since=yesterday"
$!!!	if p1 .nes. "ALL" then versions = ";*"	!Default is "ALL"
$	oplog = ""
$	if f$trnlnm("OPC$LOGFILE_NAME") .nes. "" then -
		oplog = f$element(0,";",f$search("OPC$LOGFILE_NAME:"))
$	if oplog .eqs. "" then -
		oplog = "SYS$MANAGER:OPERATOR.LOG"
$	search 'oplog''versions' "%LAVC"/window=(2,0)
$	ENDSUBROUTINE
