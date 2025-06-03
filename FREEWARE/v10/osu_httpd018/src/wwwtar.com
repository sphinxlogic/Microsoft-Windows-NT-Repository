$! This command procedure manages the WWWTAR network object.  It runs
$! the TARGATE.EXE scriptserver program as a NETSERVER task.  If the
$! WWW_TARSERV_OBJECT logical has been defined properly, we run TARGATE as 
$! a persistent decnet object.
$!
$ if f$environment("DEPTH") .lt. 2 then set nover
$!
$! Define www_root as procedure's directory if not defined otherwise.
$ if f$trnlnm("WWW_ROOT") .eqs. ""
$ then
$    root_dir = f$parse("1.;",f$environment("PROCEDURE"),,, -
	"NO_CONCEAL,SYNTAX_ONLY") - "][" - "]1.;" + ".]"
$    define www_root 'root_dir'/translation=(terminal,concealed)
$ endif
$ define tar_archive www_root:[000000]
$!
$! Some aspects of TARGATE are configured via environment variables.
$!
$ HTGT_CLIENT_LIMIT = 16	! max. number of concurrent clients.
$ HTGT_LOG_LEVEL = 10		! Logging level in trace file.
$ HTGT_REENTRANT_C_RTL = 0	! set true if using DECC
$!
$! Verify that we have targate.exe.
$!
$ tarserv == "WWW_ROOT:[000000]TARGATE.EXE"
$ if f$search(tarserv) .eqs. "" then goto missing_file
$!
$! Start scriptserver.
$ set default www_root:[000000]
$ mcr 'tarserv' http_tarserv.log http_sample_home.conf
$ exit $status
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$ missing_file:
$ set noon
$ read_net = "read/end=done net_link"
$ write_net = "write net_link"
$!
$! Connect to partner (HTTP server) and read input parameters.
$!
$ open/read/write net_link sys$net
$ read_net subfunc		! Sub-function (SEARCH, HTBIN)
$ read_net method		! HTTP request method field
$ read_net protocol		! Protocol field in HTTP request
$ read_net url			! 'file' part of URL in HTTP request.
$!
$! Return error message.
$!
$ write_net "<DNETTEXT>"
$ write_net "500 Internal error, missing directory server"
$ write_net "A directory could not be returned because the targazer
$ write_net "program (TARGATE.EXE) is missing.
$ write_net "</DNETTEXT>"
$ close net_link
