$! This command procedure manages the WWWDIR network object.  It runs
$! the HTTP_DIRSERV.EXE scriptserver program as a NETSERVER task.  If the
$! WWW_DIRSERV_OBJECT logical has been defined properly, we run HTTP_DIRSERV as 
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
$!
$! Some aspects of HTTP_DIRSERV are configured via environment variables.
$!
$ HTDS_CLIENT_LIMIT = 16	! max. number of concurrent clients.
$ HTDS_LOG_LEVEL = 10		! Logging level in trace file.
$ HTDS_REENTRANT_C_RTL = 0	! set true if using DECC
$!
$! Verify that we have http_dirserv.exe.
$!
$ dirserv == f$trnlnm("WWW_ROOT") - ".]" + "]HTTP_DIRSERV.EXE"
$ if f$search(dirserv) .eqs. "" then goto missing_file
$!
$! Start scriptserver.
$ set default www_root:[000000]
$ mcr 'dirserv' http_dirserv.log
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
$ write_net "A directory could not be returned because the directory browser
$ write_net "program (HTTP_DIRSERV.EXE) is missing.
$ write_net "</DNETTEXT>"
$ close net_link
