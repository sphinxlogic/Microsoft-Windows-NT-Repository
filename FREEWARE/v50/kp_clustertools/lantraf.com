$!
$! LANTRAF.COM
$!
$!	Show which LAN adapters are enabled for SCS traffice, and the
$!	relative amount of traffic that SCS is sending over each.
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$!   http://encompasserve.org/~parris/ or http://www.geocities.com/keithparris/ 
$!
$	pid = f$getjpi("","PID")
$	filespec := lantraf_'pid'_temp.temp
$	show time
$	call analysis/out='filespec'_list
$	search 'filespec'_list/out='filespec'_list2 -
		"60-07 (SCA) Counters Information",-
		"Octets received",-
		"Seconds since zeroed"
$	delete 'filespec'_list;*
$	device_list = ""
$	open/read list2 'filespec'_list2
$ loop:
$	read/end=eof list2 rec
$	rec = f$edit(rec,"COMPRESS,TRIM")
$	if f$extract(0,20,rec) .eqs. "Seconds since zeroed"
$	then
$		seconds = f$integer(f$element(3," ",rec))
$		goto loop
$	endif
$	if f$element(0," ",rec) .eqs. "--" .and. f$element(3," ",rec) .eqs. "(SCA)"
$	then	!"-- FXB1 60-07 (SCA) Counters Information"
$		device = f$element(1," ",rec)
$		device_list = device_list + "," + device
$ loop2:	read/end=eof list2 rec	!Read counters
$		if f$extract(0,15,rec) .nes. "Octets received" then goto loop2
$		rec = f$edit(rec,"COMPRESS,TRIM")
$		received = f$element(2," ",rec)
$		received = f$extract(0,f$length(received)-3,received)
$		'device'_received = f$integer(received)
$		sent = f$element(2," ",rec)
$		sent = f$extract(0,f$length(sent)-3,sent)
$		'device'_sent = f$integer(sent)
$		'device'_total = 'device'_received + 'device'_sent
$		'device'_rate = 'device'_total / seconds
$		write sys$output device," ",-
			'device'_received," received, ",-
			'device'_sent," sent, ",'device'_total," total Kbytes, ",-
			'device'_rate," Kbytes/second"
$	endif
$	goto loop
$ eof:
$	close list2
$	delete 'filespec'_list2;*
$	exit
$ analysis: subroutine
$	analyze/system
show lan/counters
$	endsuborutine
