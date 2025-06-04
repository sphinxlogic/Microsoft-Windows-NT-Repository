$	old_verify = f$verify('f$trnlnm("watch_rbld$verify")'+0)
$! WATCH_RBLD.COM
$!
$! Procedure to monitor rebuild activity in the cluster
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$! http://www.geocities.com/keithparris/ or http://encompasserve.org/~parris/
$	pid = f$getjpi("","PID")
$	temp_file := watch_rbld_temp_'pid'.temp_file
$	close/nolog lis
$ loop:
$	call get_rbld_cnts/out='temp_file'
$!!! type/page 'temp_file'
$	open/read lis 'temp_file'
$	node = "_NULL_"
$	timestamped = "FALSE"	!Haven't output a timestamp
$ loop2:
$	read/end=eof2 lis rec
$	if f$extract(0,43,rec) .eqs. "%SYSMAN-I-OUTPUT, command execution on node"
$	then
$		node = f$extract(44,6,rec)
$		type = "RCVD"	!"rcvd" is first each time, followed by "sent"
$		goto loop2
$	endif
$	if f$extract(0,3,rec) .eqs. "Hex"
$	then
$		rec = f$edit(rec,"COMPRESS,TRIM")
$		varname = "RBLD_" + node + "_" + type
$		new_value = f$integer(f$element(5," ",rec))
$		if f$type('varname') .eqs. ""
$		then	!First time around; fake a no-change situation
$			last_'varname' = new_value
$		else	!Save previous value
$			last_'varname' = 'varname'
$!! Ignore cases of counter wrap-around:
$!!			if 'varname' .lt. last_'varname' then -
$!!_!				last_'varname' = new_value
$! For now, wrap-around will just appear as an increase by a negative value...
$		endif
$		'varname' = new_value
$		if last_'varname' .ne. 'varname'
$		then
$			diff = 'varname'-last_'varname'
$			if diff .gt. 100 .or. diff .lt. -100
$			then
$				if .not. timestamped
$				then
$				    write sys$output f$time()
$				    timestamped = "TRUE"
$				endif
$				write sys$output -
	"  ",(varname-"RBLD_")," went up by ",diff	!," at ",f$time()
$			endif
$		endif
$		if type .eqs. "RCVD"
$		then
$			type = "SENT"
$		else
$			type = "RCVD"
$		endif
$		goto loop2
$	endif
$	goto loop2
$ eof2:
$	close lis
$	delete 'temp_file';*
$!!! show symbol rbld_*
$	wait 00:00:30	!Wait a bit
$	goto loop
$!	exit 1+0*'f$verify(old_verify)'
$!
$! Get the rebuild packet counts from all nodes
$ get_rbld_cnts: subroutine
$	mcr sysman set environment/cluster
do @rbld
$	endsubroutine


$! RBLD.COM contains:
$ analyze/system
eval @pms$gl_rm_rbld_rcvd
eval @pms$gl_rm_rbld_sent
$ exit


! Example of output we're parsing:

123456789.123456789.123456789.123456789.123456789.123456789.
%SYSMAN-I-OUTPUT, command execution on node ETGA06
OpenVMS (TM) Alpha System analyzer
Hex = 01C764CA   Decimal = 29844682
Hex = 01F5092B   Decimal = 32835883
