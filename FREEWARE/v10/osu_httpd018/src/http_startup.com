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
$! Install images with needed privileges.
$!
$ save_default = f$environment("DEFAULT")
$ set default 'login_dir'
$ image = f$parse("HTTP_SERVER.EXE;",login_dir) - ";" - "]["
$ p_dir = f$locate(".000000]",image)
$ if p_dir .lt. f$length(image) then image = -
		f$extract(0,p_dir,image) + f$extract(p_dir+7,250,image)
$ privs = "SYSPRV"
$ install replace 'image'/privilege=('privs')
$!
$!	Install directory browser scriptserver.  Define exec mode
$!	logical required to make scriptserver a persistent object.
$!
$ image = f$parse("http_dirserv.EXE;",image) - ";" - "]["
$ p_dir = f$locate(".000000]",image)
$ if p_dir .lt. f$length(image) then image = -
		f$extract(0,p_dir,image) + f$extract(p_dir+7,250,image)
$ privs = "SYSNAM"
$ if f$search(image) .nes. "" then install replace 'image'/privilege=('privs')
$ define/sys/exec www_dirserv_object "WWWDIR"
$ define/sys/exec www_dirserv_access 'f$trnlnm("SYS$NODE")''P1'
$ set default 'save_default'
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
$ if f$getsyi("CPU") .ge. 128		! test for alpha
$ then
$	if .not. f$file("sys$share:cma$open_lib_shr.exe","known") then -
	        install replace sys$share:cma$open_lib_shr.exe
$ endif
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
$! Run the http_server program using passed in parameters.
$!
$ @http_server run "''P2'" "''P3'" "''P4'" "''P5"
$ exit $status
