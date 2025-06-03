$! This script demonstrates the use of the CGI_SYMBOLS program for creating
$! DCL symbols similar to that produced by the CERN server.  We assume the
$! cgi_symbols DCL symbol has been assigned to invoke the cgi_symbols as a
$! foreign command.  Each variable in the CGI environment will create a DCL 
$! symbol of the same name prefixed with "WWW_".
$!
$! Author: David Jones
$! Date:   27-SEP-1994
$! Revised:23-NOV-1994
$!
$ CGI_SYMBOLS
$!
$! Send back CGI response.  Note that newlines (<CR><LF>) must be
$! explicitly sent.
$!
$ set noon
$ crlf = f$fao("!/")
$ put = "write net_link"
$ put "content-type: text/plain", crlf, crlf	 ! CGI header
$ if P1 .eqs. "HEAD" then exit
$!
$ put "This script demonstrates the use of the cgi_symbols program to",crlf
$ put "facilitate writing HTTP server scripts in DCL rather than a programming",crlf
$ put "language.",crlf,crlf
$ put "Once your command procedure runs program cgi_symbols, your connection is",crlf
$ put "in a special 'CGI' mode where the server expects you to write a CGI",crlf
$ put "response.  Your first line must be either a content-type header or",crlf
$ put "a location header followed by 2 newline sequences.  The wwexec.com",crlf
$ put "procedure will take care of closing down the connection with the",crlf
$ put "server so you should NOT write </DNETCGI>.",crlf,crlf
$ put "The environment variables available via the cgi_info() call in cgilib",crlf
$ put "are defined as DCL symbols with a prefix of 'WWW_'.  For example, the",crlf
$ put "script name is given by WWW_SCRIPT_NAME and is currently ""''WWW_SCRIPT_NAME'"".",crlf
$ put "The WWW_PATH_INFO value is ""''WWW_PATH_INFO'"".",crlf
$ exit
