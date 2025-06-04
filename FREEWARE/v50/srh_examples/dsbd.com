$!$ ccopt = "/NODEBUG/OPTIMIZE"	    !>
$!$ linkopt = "/NODEBUG/NOTRACE"    !>
$!$ ccopt = "/DEBUG/NOOPTIMIZE"	    !+
$!$ linkopt = "/DEBUG"		    !+
$ ccopt = "/NODEBUG/OPTIMIZE"	    !=
$ linkopt = "/NODEBUG/TRACE"	    !=
$!
$ CC	'ccopt'	    DSBD
$ Link	'linkopt'   DSBD,Sys$Input/Opt
SYS$SHARE:VAXCRTL/Share
$!
$ If F$Search("DSBD.OBJ") .nes. "" Then DELETE DSBD.OBJ.*
$!
