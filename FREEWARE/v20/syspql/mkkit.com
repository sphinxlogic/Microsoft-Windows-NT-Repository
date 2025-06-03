$!
$	open/read v version.dat
$	read v version
$	close v
$!
$	if f$search ("[-]DIRSTARTUP.COM").nes."" then @[-]dirstartup
$!
$ backup [.alpha]*.*;,[.vax]*.*;,[.src]*.*;,[]*.com,.doc,.dat -
	[]syspql'version'.bck	/sav/log/block=4096
