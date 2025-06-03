$!
$! CGI script to bounce the current path.
$ write net_link "<DNETRECMODE>"
$ cgi_symbols
$ www_query_string = "?''www_query_string'"
$ if www_query_string .eqs. "?" then www_query_string = ""
$ if f$type(www_path_info) .eqs. "" then www_path_info = ""
$ if www_path_info .eqs. ""
$ then
$    write net_link "content-type: text/plain"
$    write net_link ""
$    write net_link "Missing path info"
$ else
$    write net_link "location: ", www_path_info,www_query_string
$    write net_link ""
$ endif
$ exit
