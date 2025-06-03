$!								4-MAY-1995
$!
$! This command procedure runs the http_server.exe image and must reside
$! in the same directory.  It takes the following parameters:
$!
$!	Param.	  Default		Description.
$!	P1	RUN		Mode option:
$!				   RUN		Run server image.
$!
$!      P2  -	HTTP_SERVER_ERR.LOG Log file name.  File will default to same
$!				directory as this procedure.  May include
$!				log_level by appending /{level}
$!
$!      P3  -   HTTP_SAMPLE_HOME.CONF Configuration file name.  This file is
$!				also known as the rules file.  File will 
$!				default to same directory as this procedure.
$!
$!	P4  -    80		TCP port number that server will listen on.
$!
$!	P5  -    		TCP port number that server will listen on
$!				with cache disabled.
$!
$! Note that if http_server.exe is to be installed with privilege, then you 
$! must also install sys$share:cma$lib_shr.exe, sys$share:cma$open_rtl.exe, 
$! and sys$share:cma$rtl.exe.
$!
$ proc = f$parse(f$environment("PROCEDURE"),,,,"NO_CONCEAL")
$ if P1 .eqs. "" THEN P1 = "RUN"
$ IF P1 .nes. "RUN" THEN GOTO ALT_MODE
$! Define DCL symbols that HTTP_SERVER will read as environment variables.
$!
$ HTTP_CACHE_SIZE = 0			! Entries in small document cache.
$ HTTP_CLIENT_LIMIT = 100		! Max. number of concurent clients.
$ HTTP_LOG_LEVEL = 0			! Level of detail in log file.
$ if f$type(HTTP_MANAGE_PORT) .eqs. "" THEN HTTP_MANAGE_PORT = 931
$!
$! Make default for default host name, this will work with both UCX and
$! multinet.
$!
$ if f$type(http_default_host) + f$trnlnm("HTTP_DEFAULT_HOST") .eqs. "" 
$ then
$	hname = f$trnlnm("UCX$INET_HOST")
$	if f$locate(".",hname) .ge. f$length(hname) then -
		hname = hname + "." + f$trnlnm("UCX$INET_DOMAIN")
$ 	if hname .nes. "." then -
		write sys$output "Using default host name of ",hname
$	HTTP_DEFAULT_HOST = hname
$ endif
$ if "''http_default_host'" .eqs. "."   ! OK no host name, try for CMU
$ then
$       hname = f$trnlnm("INTERNET_HOST_NAME")
$	if hname .eqs. ""		! try for TWG
$	then
$	    hname = f$trnlnm("ARPANET_HOST_NAME")
$	    if f$locate(".",hname) .ge. f$length(hname) then -
		hname = hname + "." + f$trnlnm("INET_DOMAIN_NAME")
$	    if hname .eqs. "." then hname = "?hostname?"
$	endif
$       write sys$output "Using default host name of ",hname
$       HTTP_DEFAULT_HOST = hname
$ endif
$ if f$getsyi("CPU") .ge. 128 then http_reentrant_c_rtl == 1
$!
$! Define www_root and www_system logicals if not defined.
$!
$ if f$trnlnm("www_root") .eqs. ""
$ then
$    root_dir = f$parse("1.;",proc,,,"NO_CONCEAL,SYNTAX_ONLY") - "][" - "]1.;"
$    l = f$length(root_dir)
$ trim_last:
$    l = l - 1
$    if f$extract(l,1,root_dir) .nes. "." .and. l .gt. 0 then goto trim_last
$    root_dir = f$extract(0,l,root_dir) + ".]"
$    write sys$output "Defining www_root as ", root_dir
$    define www_root 'root_dir'/trans=(term,conceal)
$ endif
$ if f$trnlnm("www_system") .eqs. ""
$ then
$    sys_dir = f$parse("1.;",proc,,,"NO_CONCEAL,SYNTAX_ONLY") - "][" - "]1.;"
$    sys_dir = sys_dir + "]"
$    write sys$output "Defining www_system as ", sys_dir
$    define www_system 'sys_dir'
$ endif
$!
$! Give warning if no timzone.
$!
$ if f$trnlnm("SYS$TIMEZONE_DIFFERENTIAL") .eqs. ""
$ then
$    type sys$Input
Warning, sys$timezone_differential is undefined, so local time for file
dates will be reported as GMT time of file.  SYS$TIMEZONE_DIFFERENTIAL
should translate to number of seconds offset to add to local time to
get UTC time (.e.g EDT is -18000).
$ endif
$!
$! Setup command line arguments, Make this procedure name the related file
$! to default it to the same directory.
$!
$ log_level = f$element(1,"/",P2)
$ if log_level .nes. "/" .and. log_level .nes. "" then http_log_level = log_level
$ log_file = f$parse(f$element(0,"/",P2),"HTTP_SERVER_ERR.LOG",proc)
$ rules_file = f$parse(P3,"HTTP_SAMPLE_HOME.CONF",proc)
$ port = "80"
$ if P4 .nes. "" then port = P4
$!
$! Run server as foreign command, specifying rules file determined above.
$!
$ httpd = "$ www_system:http_server.exe"
$ httpd 'log_file' 'rules_file' 'port' 'P5' 'P6'
$ exit $status
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! Handle special functions for HTTP_SERVER:
$ ALT_MODE:
$    write sys$output "Invalid P1 parameter, must be RUN.
$    exit 20
