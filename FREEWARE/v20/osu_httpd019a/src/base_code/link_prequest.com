$!
$! This procedure links the PRIVREQUEST program from the object files.
$!
$! Parameters:
$!	P1		Keyword for TCP package to use: 
$!				UCXTCP, CMUTCP, TWGTCP, MULTINET, TCPWARE
$!	P2		Option additional link flags (e.g. /TRACE).
$!
$! Author:	David Jones
$! Date:	20-AUG-1994
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! First validate P1.
$ say = "WRITE SYS$OUTPUT"
$ if P1 .eqs. ""
$ then
$    P1 = "UCXTCP"
$    if f$trnlnm("TWG$TCP") .nes. "" then P1 = "TWGTCP"
$    if f$trnlnm("MULTINET") .nes. "" then P1 = "MULTINET"
$    say "Using ",P1," for TCP package for PRIVREQUEST.EXE"
$ endif
$ tcp_list = "/UCXTCP/CMUTCP/TWGTCP/MULTINET/TCPWARE/"
$ if f$locate("/''P1'/",TCP_LIST) .GE. f$length(tcp_list)
$ then
$    say "Invalid P1 (tcp type), valid values: ",-
	"UCXTCP MULTINET CMUTCP TWGTCP TCPWARE"
$    exit
$ endif
$ create TMPRQ_LINK.OPT
$ open/append lopt TMPRQ_LINK.OPT
$ on error then goto cleanup
$ on control_y then goto cleanup
$!
$! Determine our configuration.
$!
$ is_axp = (f$getsyi("CPU") .ge. 128)
$ is_vaxc = (.not.is_axp .and. (f$trnlnm("DECC$CC_DEFAULT") .nes. "/DECC"))
$ is_vmsv5 = ("V5." .eqs. f$extract(0,3,f$getsyi("VERSION")))
$ write lopt "!This file generated by LINK_PREQUEST.COM, AXP flag: ", is_axp, -
	", VAXC flag: ", is_vaxc, ", TCP: ", P1
$!
$! Add any speical lines needed for TCP package.
$!
$ if P1 .eqs. "MULTINET" then write lopt -
"multinet_root:[multinet]multinet_socket_library/share"
$ if P1 .eqs. "TWGTCP" then write lopt "twg$tcp:[netdist.lib]twglib/lib"
$ if P1 .eqs. "TCPWARE" then write lopt "tcpip_socklib/share"
$ if P1 .eqs. "UCXTCP" .and. is_vaxc then write lopt "sys$library:ucx$ipc/lib"
$ if P1 .eqs. "CMUTCP" then write lopt "sys$library:libcmu/lib"
$!
$! Add special stuff for VAXC
$!
$ if is_vaxc then write lopt "sys$share:vaxcrtl/share"
$ if is_vaxc then write lopt "PSECT_ATTR=$CHAR_STRING_CONSTANTS,NOWRT"
$!
$ close lopt
$ on error then exit $status
$ purge tmprq_link.opt
$!
$ link 'P2' PRIVREQUEST,TMPRQ_LINK.OPT/OPT
$ exit $status
$!
$ cleanup:
$ close lopt
