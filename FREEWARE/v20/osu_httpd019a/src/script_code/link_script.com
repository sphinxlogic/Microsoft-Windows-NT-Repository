$!
$! This procedure link standard CGI scripts.
$!
$! Parameters:
$!	P1		Object modules to link.
$!	P2		Keyword for TCP package to use or blank: 
$!				UCXTCP, CMUTCP, TWGTCP, MULTINET, TCPWARE
$!	P3		Option additional link flags (e.g. /TRACE).
$!
$! Author:	David Jones
$! Date:	30-AUG-194
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! First validate P2.
$ say = "WRITE SYS$OUTPUT"
$ if p2 .nes. ""
$ then
$   tcp_list = "/UCXTCP/CMUTCP/TWGTCP/MULTINET/TCPWARE/"
$    if f$locate("/''P2'/",TCP_LIST) .GE. f$length(tcp_list)
$    then
$       say "Invalid P2 (tcp type), valid values: ",-
	   "UCXTCP MULTINET CMUTCP TWGTCP TCPWARE"
$       exit
$    endif
$ endif
$ create TMP_LINK.OPT
$ open/append lopt TMP_LINK.OPT
$ on error then goto cleanup
$ on control_y then goto cleanup
$!
$! Determine our configuration.
$!
$ is_axp = (f$getsyi("CPU") .ge. 128)
$ is_vaxc = (.not.is_axp .and. (f$trnlnm("DECC$CC_DEFAULT") .nes. "/DECC"))
$ is_vmsv5 = ("V5." .eqs. f$extract(0,3,f$getsyi("VERSION")))
$ write lopt "!This file generated by LINK_SERVER.COM, AXP flag: ", is_axp, -
	", VAXC flag: ", is_vaxc, ", TCP: ", P2
$!
$! Add any special lines needed for TCP package.(None needed for UCX or CMU)
$!
$ if p1 .nes. ""
$ then
$ if P2 .eqs. "MULTINET" then write lopt -
"multinet_root:[multinet]multinet_socket_library/share"
$ if P2 .eqs. "TWGTCP" then write lopt "twg$tcp:[netdist.lib]twglib/lib"
$ if P2 .eqs. "TCPWARE" then write lopt "tcpip_socklib/share"
$ if P2 .eqs. "UCXTCP" .and. is_vaxc then write lopt "sys$share:ucx$ipc/lib"
$ if P2 .eqs. "CMUTCP" then write lopt "sys$share:libcmu/lib"
$ endif
$!
$! Add special stuff for VAXC
$!
$ if is_vaxc then write lopt "sys$share:vaxcrtl/share"
$ if is_vaxc then write lopt "PSECT_ATTR=$CHAR_STRING_CONSTANTS,NOWRT"
$!
$ close lopt
$ on error then exit $status
$ purge tmp_link.opt
$!
$ link 'P3' 'P1',TMP_LINK.OPT/OPT
$ exit $status
$!
$ cleanup:
$ close lopt
