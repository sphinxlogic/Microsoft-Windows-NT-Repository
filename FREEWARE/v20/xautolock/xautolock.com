$	If (F$Getsyi("ARCH_NAME") .EQS. "VAX") Then -
		Define	DECC$LIBRARY_INCLUDE SYS$LIBRARY
$	CC/DECC/STAND=VAXC	XAUTOLOCK
$	link/notrace xautolot3,sys$input/opt
sys$share:decw$xtshr/share
sys$share:decw$xlibshr/share
sys$share:decw$dwtlibshr/share
$	exit
