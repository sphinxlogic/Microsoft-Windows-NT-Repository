$!
$! This script simply converts the requested search argument to the user
$! html directory.
$!
$ write sys$Output "P1: ", P1,", P2: ", P2
$   libqual = ""
$ libname = f$element(3,"/",P2)
$ if libname .nes. "" .and. libname .nes. "/"
$ then
$   libqual = "/library=" + libname
$ endif
$!
$    crlf = f$fao("!/")
$ write net_link "<DNETARG2>"
$ read/end=done net_link search_arg
$ if search_arg .eqs. ""
$ then
$!    Send html document that explains how to do search.
$!    Server will do internal redirect so URL client uses stays the same.
$!
$    write net_link "<DNETCGI>"
$    write net_link "location: /www/userdir.html",crlf,crlf
$ else
$!    Send CGI relocation to specified user.  Presence of http: will cause
$!    Server to do external redirect so URL client uses stays the same.
$!
$    search_arg = f$extract(1,255,search_arg)
$ do_decode_slash:
$    slash = f$locate("%2F",search_arg)
$    if slash .eq. f$length(search_arg) then goto do_redirect
$    search_arg = f$extract(0,slash,search_arg)+"/"+f$extr(slash+3,255,search_arg)
$    goto do_decode_slash
$!
$ do_redirect:
$    if f$locate("/",search_arg) .ge. f$length(search_arg) then -
		search_arg = search_arg + "/"
$    write net_link "<DNETID>"
$    read net_link connect_info
$    port = ":" + f$element(2," ",connect_info)
$    if port .eqs. ":80" .or. port .eqs. ": " then port = ""
$    write net_link "<DNETHOST>"
$    read net_link host_name
$    write net_link "<DNETCGI>"
$    on error then continue
$    write net_link "location: http://",host_name,port,"/~",search_arg,crlf,crlf
$ endif
$ write_abort:
$    write net_link "</DNETCGI>"
$!
$ exit
