$!
$	open/read v version.dat
$	read v version
$	close v
$	if f$search ("[-]dirstartup.com").nes."" then @[-]dirstartup
$!
$ backup [.alpha]*.*;,[.vax]*.*;,[.src]*.*;,[]*.com,.doc,*.txt -
	$1$dua7:[anonymous.pub.freeware_cd]syspql'version'.bck/prot=w:R-
	/sav/log/block=4096
