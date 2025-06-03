$	If (F$Getsyi("ARCH_NAME") .EQS. "VAX") Then -
		Define	DECC$LIBRARY_INCLUDE SYS$LIBRARY
$
$	CC/DECC/Pref=ALL DTIME
$	CC/DECC	Phase
$	CC/DECC/Incl=SYS$DISK:[]		Moon
$	CC/DECC/Pref=ALL XPHOON
$
$	Link Xphoon,Dtime,Moon,Phase,Sys$Input/Opt
Sys$Share:DECW$XLIBSHR/SHARE
$
$	Exit
