$!
$! Name - VMSMAKE.COM
$!
$! ccs version:	1.3
$!
$! ccsid:	@(#)VMSMAKE.COM	1.3 - 8/12/92 12:39:05
$! from: 	ccs/s.VMSMAKE.COM
$! date: 	8/12/92 14:15:36
$!
$! This is an UNTESTED command file for building xgrabsc under Vax/VMS
$!
$!
$ define x11 decw$include
$ cc xgrabsc
$ link xgrabsc,sys$input/opt
sys$library:decw$xlibshr/share
sys$library:vaxcrtl/share
$!
$! the link for xgrab is questionable - it probably needs an Athena
$! widget library
$!
$ cc xgrab
$ link xgrab,sys$input/opt
sys$library:decw$xlibshr/share
sys$library:vaxcrtl/share
$ exit
