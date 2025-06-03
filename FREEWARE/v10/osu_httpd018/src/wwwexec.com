$!
$! This command procedure handles the netserver side of the WWWEXEC decnet
$! object.  See bottom of file for DECnet protocol description.
$ if f$environment("DEPTH") .lt. 2 then set nover
$!
$! Define www_root as procedure's directory if not defined otherwise.
$ if f$trnlnm("WWW_ROOT") .eqs. ""
$ then
$    root_dir = f$parse("1.;",f$environment("PROCEDURE"),,, -
	"NO_CONCEAL,SYNTAX_ONLY") - "][" - "]1.;" + ".]"
$    define www_root 'root_dir'/translation=(terminal,concealed)
$ endif
$ read_net = "read/end=done net_link"
$ write_net = "write net_link"
$ wwwexec_rundown_string == ""
$!
$! Connect to partner (HTTP server) and read input parameters.
$!
$ open/read/write net_link sys$net
$ read_net subfunc		! Sub-function (SEARCH, HTBIN)
$ read_net method		! HTTP request method field
$ read_net protocol		! Protocol field in HTTP request
$ read_net url			! 'file' part of URL in HTTP request.
$!
$! Dispatch on subfunc value.  We expect HTBIN to be most common case, so
$! check for it first.
$!
$ offset = f$locate(subfunc+",","HTBIN,SEARCH,CONVERT,POST,")
$ on warning then goto done		! Make DCL symbol errors abort us.
$ if  offset .LT. 26 then goto do_'subfunc'
$ do_error:
$! Generate error response for unknown sub function.
$ call send_sts "500 server error" "Internal error in server, unsupported function.
$ goto  done_1
$!------------------------------------------------------------------------
$! Script execution.  Parse script name out of URL and search for it in
$! htbin directory, constructing execution symbol in process.
$!
$ do_htbin:
$ write_net "<DNETPATH>"
$ read_net script_path
$ script_name = f$element(0,"/",url-script_path)
$ write_net "<DNETBINDIR>"
$ read_net http_bindir
$!
$! Construct symbol to execute script, either as foreign command or DCL proc.
$!
$ script_exec = "$" + f$search(f$parse(http_bindir+script_name,".com"))
$ if script_exec .eqs. "$" then script_exec = "$" + -
	f$search(f$parse(http_bindir+script_name,".exe"))
$ if f$parse(script_exec-"$",,,"TYPE") .eqs. ".COM" then script_exec[0,1] := "@"
$ if ( script_exec .nes. "$" )
$ then
$    cgi_symbols = "$"+http_bindir+"cgi_symbols.exe"
$    on warning then goto done
$    script_exec "''method'" "''url'" "''protocol'"
$ else
$    call send_sts "404 script not found" -
	"''f$fao("Requested script (!AS) not found in htbin directory (!AS)",-
	script_name,http_bindir)'"
$ endif
$ goto done_1
$!
$!------------------------------------------------------------------------
$! Post command.
$!
$ do_post:
$ write sys$Output "Saving request..."
$ mcr sys$login:request_to_file net_link wwwexec.tmp
$ call send_sts "200 ok" "Request has been saved to file wwwexec.tmp"
$ goto done_1
$!
$!------------------------------------------------------------------------
$! Mime types defined by presentation rules in config file will cause 
$! convert sub-function to be used, search arguments cause search subfunction.
$! The bindir argument is assumed to be the name of the script to execute.
$ do_convert:
$ do_search:
$ write_net "<DNETBINDIR>"
$ read_net script_name
$ script_exec = "$" + f$search(f$parse(script_name,"WWW_ROOT:[000000].com"))
$ if script_exec .eqs. "$" then script_exec = "$" + -
	f$search(f$parse(script_name,"WWW_ROOT:[000000].exe"))
$ if f$parse(script_exec-"$",,,"TYPE") .eqs. ".COM" then script_exec[0,1] := "@"
$!
$ if ( script_exec .nes. "$" )
$ then
$!
$!   Run the script as foriegn command or command procedure.
$!
$    cgi_symbols = "$"+f$parse("cgi_symbols.exe;",f$extract(1,256,script_exec))-";"
$    on warning then goto done
$    script_exec "''method'" "''url'" "''protocol'"
$ else
$    call send_sts "404 presentation script not found" -
	"''f$fao("!AS program (!AS) not found.",subfunc,script_name)
$ endif
$!
$!------------------------------------------------------------------------
$! Common exit point.  Clean up connection.
$!
$ done_1:
$ if wwwexec_rundown_string .nes. "" then write_net wwwexec_rundown_string
$ read_net /time_out=10 line		! let server break connection first.
$ done:
$ deass sys$output
$ close net_link
$ exit $status
$!--------------------------------------------------------------------------
$! Common procedure to send messages to.
$! Parameters are: P1: status, P2: text.
$!
$ send_sts: SUBROUTINE
$ set noon
$ write_net "<DNETTEXT>"
$ write_net P1
$ write_net P2
$ write_net "</DNETTEXT>"
$ endsubroutine
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$! Protocol information:
$!   Script execution involves 3 processes: a client process running a
$!   WWW browser; the HTTP server process, and a DECnet netserver process
$!   which executes this command procedure.  These processes be referred to
$!   as the Client, the Server, and the Scriptserver in the following 
$!   description.
$!
$!   For a script to execute, the Client first connects to the Server
$!   and makes an HTTP request (e.g. POST) that triggers script execution
$!   in the Server.  The Server creates a DECnet logical link to the
$!   WWWEXEC object, which creates the Scriptserver process.
$!
$!   After the logical link is established, the Server always sends the
$!   following 4 data messages to the Scriptserver:
$!
$!	module		This string is a token representing the server
$!			module making the DECnet request, either "SEARCH"
$!			"HTBIN", "POST", or "CONVERT".
$!
$!	method		This string is the METHOD field parsed from the
$!			HTTP request, usually GET.  This field is truncated
$!			to 255 characters before being sent.
$!
$!	protocol	This string is the HTTP protocol (e.g. HTTP/V1.0)
$!			parsed from the request.  If the request is from a
$!			0.9 client, this string is null.  This field is
$!			truncated to 255 characters before being sent.
$!
$!	URL-ident	File identifier parsed from the URL in the request
$!			(ident is portion before ? or # in URL).  Only first
$!			255 characters of this field are sent.
$!
$!   The HTTP server then lets the script execution process control the
$!   session by reading and responding to commands sent over the DECnet link
$!   by the Scriptserver:
$!	 
$!	<DNETARG>	Requests Server to send Scriptserver the search 
$!		        argument parsed from URL of request.  If the argument 
$!			was generated by an HTML form, the response message 
$!			may be quite long. (Not readable by DCL).
$!
$!	<DNETARG2>	Same as <DNETARG> except that argument is truncated
$!			to 255 characters to allow reading by DCL.
$!
$!	<DNETPATH>	Requests Server to send Scriptserver the matching
$!			portion of the translated URL that caused the
$!			the script to be invoked (e.g. "/HTBIN/").  The path
$!			in truncated to 255 characters and converted to upper
$!			case.
$!
$!	<DNETHDR>	Requests Server to send Scriptserver the HTTP request
$!			header lines sent by the Client to the Server.
$!			Last line sent will be a zero-length line.
$!
$!	<DNETINPUT>	Requests Server to read data from Client TCP connection
$!			and relay to the Scriptserver.  Only one DECnet message
$!			is sent.  If more data is still needed, another input
$!			request must by made by the Scriptserver.
$!
$!	<DNETTEXT>	Requests Server to send Client an HTTP protocol
$!			response with a content-type of "text/plain" and
$!			follow with data read from the Scriptserver.
$!			The Scriptserver will first send an HTTP status line
$!			(e.g. 200 sending doc) follow it with text that is
$!			to appear in the user's output window.
$!
$!			Data is sent assuming implied carriage control, the
$!			Server appends a newline (CRLF) to each DECnet message
$!			it recieves over the logical link.  The Scriptserver
$!			marks the end of the text data by sending a line
$!			consisting solely of the string "</DNETTEXT>".
$!			Server will close the connection after processing
$!			this tag.
$!
$!	<DNETRAW>	Requests Server to read DECnet data 'raw' from
$!			scriptserver and send to Client.  The Scriptserver is
$!			responsible for formatting the entire response in
$!			conformance with the HTTP protocol (including carriage
$!			control characters). End of data is flagged by a 
$!			DECnet message consisting of solely of "</DNETRAW>",
$!			which will cause Server to close connection.
$!
$!	<DNETCGI>	Request Server to read DECnet data 'raw' and interpret
$!			data sent by Scriptserver as CGI (Common Gateway 
$!			Interface) script output.  If first line is 
$!			"location: <URL>", a redirect is performed.
$!			End of data is flagged by "</DNETCGI>".
$!
$!	<DNETRECMODE>	Request server to process any subsequent <DNETRAW> or
$!			<DNETCGI> using 'record' mode rather than 'stream'
$!			mode.  In record mode, an implied CRLF is added
$!			to the end of every DECnet record.  The maximum
$!			record length is reduced to 4094 bytes.
$!
$!	<DNETRQURL>	Requests Server to send Scriptserver the actual 
$!			URL (prior to rule file transformations) specified in
$!			the Client's request.  A single DECnet message is sent.
$!
$!	<DNETBINDIR>	Requests Server to send Scriptserver the htbin 
$!			directory string as defined in the rule file.
$!
$!	<DNETHOST>	Requests Server to send Scriptserver the 
$!			http_default_host environment variable (host name 
$!			to use in constructing URLS).
$!
$!	<DNETID>	Request Server to send Scriptserver the server version,
$!			the http_default_host environment variable, the local
$!			port for the connection and the remote port and host
$!			address.  A single message is sent with the items
$!			separated by spaces.  Note that the remote address is
$!			sent as a signed value.
$!
$!	<DNETXLATE>	Requests Server to read a URL from Scriptserver,
$!			translate according to the rules file, and send the
$!			result back to the Scriptserver.
