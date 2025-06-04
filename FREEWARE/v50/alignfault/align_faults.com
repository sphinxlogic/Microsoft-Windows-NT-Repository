$	!
$	! ALIGN_FAULTS.COM
$	!
$	! (See README.TXT for instructions)
$	!
$	original_file = "SYSTEM_ALIGNMENT_FAULTS.TXT"
$	sort_file = "SYSTEM_FAULTS.SRT"
$	address_file = "ADDRESSES.RESOLVED"
$	output_file = "ALIGNMENT_FAULTS.RPT"
$	close/nolog afile
$	close/nolog ifile
$	close/nolog ofile
$	create 'sort_file'
$	open/append ofile 'sort_file'
$	write ofile "$PC              VA               PID      PS       Cnt   User     Offset   Routine:Image"
$	open/read ifile 'original_file'
$ loop_1:
$	read/end=end_loop_1 ifile irec
$	if f$extract(0,5,irec) .nes. "PC = " then goto loop_1
$ loop_1a:
$	xPC = f$extract(5,16,irec)
$	xVA = f$extract(29,16,irec)
$	xPID = f$extract(53,8,irec)
$	xPS = f$extract(68,8,irec)
$	xCnt = f$extract(84,f$length(irec)-83,irec)
$	read/end=end_loop_1 ifile irec2
$	xUser = f$extract(13,8,irec2)
$	xImage = f$edit(f$extract(41,f$length(irec2),irec2),"trim")
$	if xImage .nes. "" then xImage = f$parse(xImage,,,"name")
$	xImage = f$fao("!12AS", xImage)
$	write ofile f$fao("!AS !AS !AS !AS !AS !AS", xPC, xVA, xPID, xPS, xCnt, xUser)
$	read/end=end_loop_1 ifile irec
$	goto loop_1a
$ end_loop_1:
$	close ofile
$	close ifile
$	sort 'sort_file' *
$	open/read ifile 'sort_file'
$	open/read afile 'address_file'
$	create 'output_file'
$	open/append ofile 'output_file'
$	read/end=end_loop_2 ifile irec
$	write ofile irec
$	gosub get_address
$	gosub get_fault
$
$ loop_2:
$	if curPC .lts. start_address .or. curPC .ges. end_address
$	then gosub get_address
$	     goto loop_2
$	endif
$	write ofile f$fao("!AS !8XL !AS", irec, %x'curPC' - %x'start_address', Routine)
$	gosub get_fault
$	goto loop_2
$ end_loop_2:
$	close ifile
$	close afile
$	close ofile
$	sort/key=(pos:53,size:5,descend) 'output_file' *
$	exit
$
$ get_address:
$	read/end=end_loop_2 afile arec
$	arec = f$edit(arec,"compress")
$	Routine		= f$element(0," ",arec)
$	start_address	= f$element(1," ",arec)
$	end_address	= f$element(2," ",arec)
$	if f$extract (0,1,start_address) .lts. "8"
$	then start_address = "00000000" + start_address
$	else start_address = "FFFFFFFF" + start_address
$	endif
$	if f$extract (0,1,end_address) .lts. "8"
$	then end_address = "00000000" + end_address
$	else end_address = "FFFFFFFF" + end_address
$	endif
$	return
$ get_fault:
$	read/end=end_loop_2 ifile irec
$	curPC = f$extract (0,16,irec)
$	return
