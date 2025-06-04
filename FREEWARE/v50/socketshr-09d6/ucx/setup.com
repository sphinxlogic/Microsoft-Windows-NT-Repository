$	disk = f$trnlnm("SYS$DISK")
$	dir = f$environment("PROCEDURE")
$	dir = f$parse(dir,,,"DEVICE") + f$parse(dir,,,"DIRECTORY")
$	pdir = dir - "]" + ".-]"
$	pdir = f$parse(pdir,,,"DEVICE") + f$parse(pdir,,,"DIRECTORY")
$
$ HAVEDECC = f$search("sys$system:decc$compiler.exe") .nes. ""
$ DEFAULTC = f$trnlnm("DECC$CC_DEFAULT")
$ if (DEFAULTC .eqs. "/VAXC") .or. .not. HAVEDECC
$   then
$	define/nolog vaxc$include 'pdir',sys$library
$	define/nolog sys sys$library
$	define/nolog netinet sys$library
$   else
$	define/nolog decc$system_include 'pdir',sys$library
$       if f$trnlnm("SYS")     .nes. "" then $ deassign sys
$       if f$trnlnm("NETINET") .nes. "" then $ deassign netinet
$!	define/nolog sys sys$library
$!	define/nolog netinet sys$library
$ endif
$	define/nolog socketshr 'dir'socketshr_ucx.exe
$
$! This would conflict with our use of 'TRACE'
$ del /sym/glo traceroute
