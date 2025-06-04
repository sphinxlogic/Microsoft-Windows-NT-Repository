$! File: [.NETLIB]SETUP.COM
$!
$! Defines logical names for building SOCKETSHR with NETLIB
$!
$! 03-Jun-1997	?. ?		Timestamp found on file
$! 01-Dec-1998	J. Malmberg	GCC Build support.
$! 13-Mar-1999	J. Malmberg	DECC Build support.
$! 17-Mar-1999	J. Malmberg	Fix typos in DECC build support
$!
$!=====================================================================
$!
$ dir = f$environment("PROCEDURE")
$ dir = f$parse(dir,,,"DEVICE") + f$parse(dir,,,"DIRECTORY")
$
$ HAVEDECC = f$search("sys$system:decc$compiler.exe") .nes. ""
$ DEFAULTC = f$trnlnm("DECC$CC_DEFAULT")
$ if (DEFAULTC .eqs. "/VAXC") .or. .not. HAVEDECC
$ then
$   if f$trnlnm("GNU_CC_VERSION") .nes. ""
$   then
$	__GCC__ == 1
$   else
$	define/nolog vaxc$include sys$disk:[],sys$disk:[-],sys$library
$	define/nolog sys sys$disk:[],sys$library
$	define/nolog netinet sys$library
$   endif
$ else
$   if (DEFAULTC .EQS. "/VAXC") .or. .not. HAVEDECC
$   then
$	define/nolog decc$system_include sys$disk:[],sys$disk:[-]
$   else
$	__DECC__ == 1
$   endif
$!  define/nolog decc$system_include sys$disk:[],sys$disk:[-],sys$library
$   if f$trnlnm("SYS")     .nes. "" then $ deassign sys
$   if f$trnlnm("NETINET") .nes. "" then $ deassign netinet
$ endif
$! This would conflict with our use of 'TRACE'
$ del /sym/glo traceroute
$!
$ define/nolog socketshr 'dir'socketshr_netlib1.exe
