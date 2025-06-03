$! This script demonstrates the use of the CGI_SYMBOLS program for creating
$! DCL symbols similar to that produced by the CERN server.  We assume the
$! cgi_symbols program is in the same directory as this script.  Each
$! variable in the CGI environment will create a DCL symbol of the same
$! name prefixed with "WWW_".
$!
$! Author: David Jones
$! Date:   27-SEP-1994
$! Revised: 23-NOV-1994
$!
$ write net_link "<DNETRECMODE>"	! Set implied carriage control.
$ CGI_SYMBOLS "WWW_" "FORM_"
$!
$! Send back CGI response.  Note that newlines (<CR><LF>) must be
$! explicitly sent.
$!
$ set noon
$ put = "write net_link"
$ put "content-type: text/plain" 	! CGI header
$ put "status: 200 my own damn status"
$ put "extra-header: just testing"
$ put ""
$ if www_request_method .eqs. "HEAD" then exit
$!
$ create net_link:
This script demonstrates the use of the cgi_symbols program to
facilitate writing HTTP server scripts in DCL rather than a 
programming language.  The WWWEXEC procedure that runs your 
script defines the foreign command CGI_SYMBOLS to establish the
CGI DCL environment.

Once your command procedure executes CGI_SYMBOLS, your connection 
is in a special 'CGI' mode where the server expects you to write 
a CGI response.  Your first line must be either a content-type 
header or a location header followed by 2 newline sequences.  
The wwexec.com procedure will take care of closing down the 
connection with the server so you should NOT write </DNETCGI>.

The environment variables available via the cgi_info() call in 
cgilib are defined as DCL symbols with a prefix of 'WWW_'.  If you 
invoke CGI_SYMBOLS with command line arguments, the first argument 
overrides the symbol prefix used.  A second argument, if supplied 
will cause CGI_SYMBOLS to assume the request is a POST action 
submitted by a form and create a set of DCL symbols containing the 
form data.

Standard CGI symbols (show symbol www_*):
$ set nover
$ define sys$output net_link:
$ show symbol www_*
$ put ""
$ put "Current form symbols (form*):"
$ show symbol form*
$ deass sys$output
$ exit
