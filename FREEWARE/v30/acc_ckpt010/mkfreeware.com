$!
$	open/read v version.dat
$	read v version
$	close v
$!
$	if f$search ("[-]DIRSTARTUP.COM").nes."" then @[-]dirstartup
$!
$ backup [...]	$1$dua7:[anonymous.pub.freeware_cd]acc_ckpt'version'.bck/save/prot=w:r-
	/block=2048/log
