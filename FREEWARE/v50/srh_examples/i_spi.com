$!
$! I_Spi.Com	17-Feb-1988
$!
$!	Command procedure to compile and link the test of the
$!	undocumented EXE$GETSPI call used by VMS MONITOR.  The
$!	$GETSPI appeared starting in VMS V4.4.  It changed on
$!	V5.0.  Presence in any versions above V5.4-1 is unknown.
$!	AND IT IS UNSUPPORTED ON ANY RELEASE...
$!
$!	Send comments to: Stephen (XDELTA::) Hoffman.
$!
$!
$ DBG_CC = "/Debug/NoOptimiz"
$! DBG_CC = "/NoDebug/Optimiz"
$ DBG_LINK = "/Debug"
$! DBG_LINK = "/NoDebug"
$ Cc 'DBG_cc' 	I_Spi
$!
$ Link		I_Spi,Sys$Input/Option 'DBG_Link'
Sys$Share:VAXCRTL/Share
Sys$Share:SPISHR/Share
$!
