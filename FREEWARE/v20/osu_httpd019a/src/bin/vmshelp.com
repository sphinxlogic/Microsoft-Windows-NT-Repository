$!
$! Deomonstrate use of scripts.
$!
$ write sys$Output "P1: ", P1,", P2: ", P2
$   libqual = ""
$ libname = f$element(3,"/",P2)
$ if libname .nes. "" .and. libname .nes. "/"
$ then
$   libqual = "/library=" + libname
$ endif
$!
$ write net_link "<DNETARG2>"
$ read/end=done net_link search_arg
$ if search_arg .eqs. ""
$ then
$!    Send html document that explains how to do search.
$!    Server will do internal redirect so URL client uses stays the same.
$!
$    crlf = f$fao("!/")
$    write net_link "<DNETCGI>"
$    write net_link "location: /www/vmshelp.html",crlf,crlf
$    write net_link "</DNETCGI>"
$    exit
$ endif
$!
$ search_arg = f$extract(1,255,search_arg)
$ write net_link "<DNETTEXT>"
$ write net_link "200 Sending document"
$ num = 0
$ help_list = ""
$ help_list_raw = ""
$ next_keyword:
$    keyword = f$element(num,"+",search_arg)
$    if keyword .eqs. "+" then goto decode_escape
$    help_list_raw = help_list_raw + keyword + " "
$    num = num + 1
$    goto next_keyword
$ decode_escape:
$    percent = f$locat("%",help_list_raw)
$    help_list = help_list + f$extract(0,percent,help_list_raw)
$    if percent .ge. f$length(help_list_raw) then goto do_help
$    help_list[f$length(help_list)*8,8] = -
		%x0'f$extract(percent+1,2,help_list_raw)'
$    help_list_raw = f$extract(percent+3,f$length(help_list_raw),help_list_raw)
$    goto decode_escape
$!!!!!!!!!!!!!
$ do_help:
$ write sys$output "help_list = ", help_list
$ save_verify = 'f$verify(0)'
$ define sys$Output net_link:
$ type sys$input
VMSHELP output:
$ if help_list .eqs. "? " then help_list = ""
$ helpcmd = "help" + libqual + " " + help_list
$ helpcmd
$ deass sys$output
$ save_verify = f$verify(save_verify)
$ write net_link "</DNETTEXT>"
$ read/end=done net_link line	! let server break connection first.
$!
$ done:
$ exit
