$!
$! This procedure is run by a privileged process to install the server
$! program and start the server as a network process using proxies.
$! For the most secure operation this procedure should reside in the
$! SYS$MANAGER directory, it must be protected world readable however so
$! the WWW server account can execute the network process portion.
$!
$! Parameters:
$!    P1		Proxied username to run daemon under
$!    P2		Error log file. (Passed to HTTP_SERVER.COM as P2)
$!    P3		Rule file (Passed to HTTP_SERVER.COM as P3)
$!    P4		Primary port (Passed to HTTP_SERVER.COM as P4)
$!    P5		Nocache port.
$!    P6		Equivalence name for WWW_SYSTEM (def: www_root:[system])
$!
$! Network proxy requirements:
$!    Assuming system startup runs under SYSTEM account and server account is 
$!    HTTP_SERVER and local node name is MYNODE, the following commands
$!    define the required proxy access.
$!
$!	UAF> add/proxy mynode::http_server http_server/default
$!	UAF> add/proxy mynode::system http_server
$!
$ if f$mode() .eqs. "NETWORK" then goto daemon		! we are DECnet object.
$!
$ if P1 .EQS. ""
$ then
$    write sys$output "Server's username must be supplied as P1"
$    exit
$ endif
$ if .not. f$privilege("SYSNAM,SYSPRV,NETMBX,CMKRNL")
$ then
$    write sys$output "This procedure requires SYSNAM, SYSPRV, and CMKRNL."
$    exit
$ endif
$!
$! Define logical for taskname and create the network process that will
$! become the daemon.  Daemon will respond with login directory.
$!
$ proc = f$environment("PROCEDURE")
$ define/sys www_init 'proc'
$ open/write/read net 0"''P1'"::"0=WWW_INIT"
$ on error then goto cleanup
$ on control_y then goto cleanup
$ write sys$output "Daemon process output file set to http_startup.log"
$ read net login_dir
$ deass/sys www_init
$!
$! define system-wide logicals for www_root and www_system if not defined
$! already.
$!
$ if f$trnlnm("www_root","LNM$SYSTEM") .eqs. ""
$ then
$    root_dir = f$parse("1.;",login_dir,,,"NO_CONCEAL,SYNTAX_ONLY") - 
		- "][" - "]1.;"
$    root_dir = root_dir + ".]"
$    write sys$output "Defining www_root as ", root_dir
$    define/sys/exec www_root 'root_dir'/trans=(term,conceal)
$ endif
$ if P6 .eqs. "" then P6 = "WWW_ROOT:[SYSTEM]"
$ define/sys/exec www_system 'P6'
$!
$! Install images, http_server installed with needed privileges.
$! search directory for exe's that look like potential dynamic services
$! and install those as well.
$!
$ privs = "SYSPRV"
$ if f$getdvi("IP:","EXISTS") then privs = privs + ",PHY_IO"	! CMU present.
$ install replace www_system:http_server.exe/privilege=('privs')
$ next_tserver:
$    tserver_image = f$search("www_system:tserver_*tcp.exe;")
$    if tserver_image .eqs. "" then goto msts_done
$    tserver_image = "www_system:" + f$parse(tserver_image,,,"NAME") + ".exe"
$    install replace 'tserver_image'
$    goto next_tserver
$ tserver_done:
$!
$ next_mst:
$    mst_image = f$search("www_system:*_mst.exe;")
$    if mst_image .eqs. "" then goto msts_done
$    mst_image = "www_system:" + f$parse(mst_image,,,"NAME") + ".exe"
$    install replace 'mst_image'
$    goto next_mst
$ msts_done:
$!
$! Define exec mode logical for the privielged ports used.
$!
$ if P4 .eqs. "" then p4 = "80"
$ GOSUB setup_port_list
$!
$!	Install directory browser scriptserver.  Define exec mode
$!	logical required to make scriptserver a persistent object.
$!
$ image = "www_system:http_dirserv.exe"
$ privs = "SYSNAM"
$ if f$search(image) .nes. "" then install replace 'image'/privilege=('privs')
$ define/sys/exec www_dirserv_object "WWWDIR"
$ define/sys/exec www_dirserv_access 'f$trnlnm("SYS$NODE")''P1'
$!
$! Install required decthreads images if not installed already.
$!
$ if .not. f$file("sys$share:cma$lib_shr.exe","known") then -
	    install replace sys$share:cma$lib_shr.exe
$!
$ if .not. f$file("sys$share:cma$open_rtl.exe","known") then -
	    install replace sys$share:cma$open_rtl.exe
$!
$ if .not. f$file("sys$share:cma$rtl.exe","known") then -
	    install replace sys$share:cma$rtl.exe
$!
$ if .not. f$file("sys$share:cma$open_lib_shr.exe","known") then -
	        install replace sys$share:cma$open_lib_shr.exe
$!
$! The daemon process has been patiently waiting on us to install the
$! image(s), send it the control arguments so it can proceed.
$! Send parameters as comma-delimited list (easier to parse).
$!
$ write net P2,",",P3,",",P4,",",P5
$!
$! Make script jobs permanent.
$!
$ define/sys/exec netserver$servers_'p1' 2
$ cleanup:
$ close net
$ exit
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$ daemon:
$! Branch to here when this procedure is a DECnet TASK object invoked by
$! another account.
$!
$! Accept connect, tell server our default directory, then read arguments.
$! Once we close link, DECnet ignores us until we exit (which we won't).
$!
$ open/write/share/err=accept netlog http_startup.log
$ deassign sys$Output		! stop sending to netserver.log
$ define sys$output netlog	! direct output to http_startup.log
$ accept:
$ open/read/write net sys$net
$ write net f$environment("default")
$ read net args
$ close net
$ P2 = f$element(0,",",args)
$ if P2 .eqs. "," then P2 = ""
$ P3 = f$element(1,",",args)
$ if P3 .eqs. "," then P3 = ""
$ P4 = f$element(2,",",args)
$ if P4 .eqs. "," then P4 = "80"
$ P5 = f$element(3,",",args)
$ if P5 .eqs. "," then P5 = ""
$!
$! Set process name as a way to ensure only 1 daemon per port.
$!
$ set process/name="WWW server ''P4'"
$!
$! Run the http_server program using passed in parameters. Ensure we
$! use logicals defined in system table.
$!
$ if f$trnlnm("WWW_ROOT","LNM$PROCESS") .nes. "" then deassign WWW_ROOT
$ if f$trnlnm("WWW_SYSTEM","LNM$PROCESS") .nes. "" then deassign WWW_SYSTEM
$ @www_system:http_server run "''P2'" "''P3'" "''P4'" "''P5'"
$ exit $status
$!
$! Define or redefine WWW_SERVER_PORTS to include
$!  f$trnlnm(name,table,index,mode,case,item)
$!
$ SETUP_PORT_LIST:
$ lname = "WWW_SERVER_PORTS"
$ plist = ""
$ max_index = f$trnlnm(lname,,,,,"MAX_INDEX")
$ if f$type(max_index) .nes. "INTEGER" then max_index = -1
$ i = 0
$ next_equiv:
$ if i .gt. max_index then goto define_list
$     equiv = f$trnlnm(lname,,i)
$     if i .gt. 0 then plist = plist + ","
$     plist = plist + equiv
$     pused_'f$integer(equiv)' = 1
$     i = i + 1
$     goto next_equiv
$!
$ define_list:
$ if p4 .nes. "" .and. f$integer(p4) .lt. 1024 .and. f$type(pused_'p4') .eqs. ""
$ then
$    max_index = max_index + 1
$    pused_'p4' = 1
$    if plist .nes. "" then plist = plist + ","
$    plist = plist + p4
$ endif
$ if p5 .nes. "" .and. f$integer(p5) .lt. 1024 .and. f$type(pused_'p5') .eqs. ""
$ then
$    max_index = max_index + 1
$    pused_'p5' = 1
$    if plist .nes. "" then plist = plist + ","
$    plist = plist + p5
$ endif
$ if plist .nes. "" then define/sys/exec www_server_ports 'plist'
$ if plist .eqs. "" then define/sys/exec www_server_ports 80
$ return
