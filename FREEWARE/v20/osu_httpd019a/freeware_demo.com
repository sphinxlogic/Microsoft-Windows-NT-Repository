$ proc = f$environment("PROCEDURE")
$ @'f$parse("DEMO_LOGICALS.COM;",proc)
$ noecho = "TRUE"
$ on error then goto abort
$ on control_y then goto abort
$!
$! determine TCP/IP type.
$!
$ get_tcp:
$ type sys$Input
Please choose the TCP/IP interface you wish to use:
   1 - DEC TCP/IP services for OpenVMS (UCX)
   2 - TGV Multinet
   3 - Wollongong Pathway
   4 - Process Software TCPWare

$ inquire choice "Number of package [1]"
$ if choice .eqs. "" then choice = "1"
$ choice = f$integer(choice)
$ if choice .lt. 1 .or. choice .gt. 4 then goto get_tcp
$ tag = f$element(choice,",","_,ucx,tgv,twg,tpr")
$ define/nolog tserver_tcpshr www_system:tserver_'tag'shr.exe
$ if choice .gt. 0 .and. f$search("tserver_tcpshr") .eqs. ""
$ then
$    write sys$Output "Native vendor TCP/IP image not found, using UCX"
$    define/nolog tserver_tcpshr www_system:tserver_ucxshr.exe
$ endif
$!
$! create network logical and dummy task.
$!
$ type sys$input

Setting network environment for scripts, please enter username and password 
for DECnet access string (password is not echoed).  Hit return to username 
prompt to skip network setup.
$ inquire user "Username"
$ if user .nes. ""
$ then
$    if f$getdvi("SYS$COMMAND","TRM") then noecho=f$getdvi("SYS$COMMAND","TT_NOECHO")
$    if ( .NOT. noecho ) then set term sys$command/noecho
$    inquire pwd "Password"
$    if ( .NOT. noecho ) then set term sys$command/echo
$    noecho = "TRUE"
$    if pwd .nes. "" then pwd = " " + pwd
$    node = f$trnlnm("SYS$NODE") - "::"
$    define demo_node "''node'""''user'''pwd'""::"
$    open/write  dc demo_node::demowwwx.com
$    write dc -
	"$! Special task object created for running freeware http server demo.
$    write dc "$ set output=00:00:05"
$    write dc "$ if f$trnlnm(""www_alphaexe"") .eqs. """""
$    write dc "$ then"
$    setup_proc = f$parse("demo_logicals.com;",f$environment("PROCEDURE"))
$    write dc "$ if f$search(""",setup_proc,""") .nes. """" then goto setup_present"
$    write dc "$ open/read/write net_link sys$net
$    write dc "$ read net_link subfunc"
$    write dc "$ read net_link method"
$    write dc "$ read net_link protocol"
$    write dc "$ read net_link url"
$    write dc "$ crlf = f$fao(""!/"")"
$    write dc "$ write net_link ""<DNETTEXT>"""
$    write dc "$ write net_link ""500 mis-configuration"""
$    write dc "$ write net_link ""Distribution drive not accessible from network process"""
$    write dc "$ write net_link ""</DNETTEXT>"""
$    write dc "$ setup_present:
$    write dc "$  @",f$parse("demo_logicals.com;",f$environment("PROCEDURE"))
$    write dc "$ endif"
$    fwloc = f$trnlnm("FREEWARE$LOC")
$    if fwloc .nes. "" then write dc "$ define freeware$loc ",fwloc
$    write dc "$ @www_src:[000000]wwwexec.com"
$    close dc
$ endif
$ type sys$input

Now starting HTTP server on port 8080, use a Web browser (e.g. Mosaic) to
view the demonstration documents.  Hit Control-Y to stop the server.
-------------------------------------------------------------------------
$ @www_system:http_server run sys$Output'P1' www_system:demo.conf 8080
$ abort:
$ if ( .NOT. noecho ) then set terminal sys$command/echo
