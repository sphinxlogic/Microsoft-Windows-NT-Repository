$!
$! SHAD_TRACK.COM	Track shadow merge/copy progress over time
$!	P1	Delay between updates (optional; default is 5 minutes)
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$!   http://encompasserve.org/~parris/ or http://www.geocities.com/keithparris/ 
$!
$	set noverify
$	delay = "00:05:00"
$	if p1 .nes. "" then delay = p1
$	proc = f$environment("PROCEDURE")
$	proc_dir = f$parse(proc,,,"DEVICE") + f$parse(proc,,,"DIRECTORY")
$ loop:
$	show time
$	@'proc_dir'shad
$	wait 'delay'
$	goto loop
