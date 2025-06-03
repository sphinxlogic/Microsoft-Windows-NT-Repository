$! 'f$verify(0)'
$!			HelpGate.COM
$!
$!	01-May-1994	F.Macrides (macrides@sci.wfeb.edu)
$!			Example htbin script interface to the VMS
$!			help library gateway (VMSHelpGate.c).
$!			For use with CERN v216-1betavms httpd.
$!			Should be placed where your htbin scripts
$!			have been mapped.
$!
$!	VMSHelpGate.c is my modification of Jean-Francois Groff's (CERN, 1992)
$!	v1 WWWLib gateway.  I have upgraded it to HTTP/1.0 and for use via
$!	a CGI interface, with parameters passed via symbols.  Read its header
$!	for more information about its history and characteristics, and how
$!	to set up your httpd configuration file for this service.
$!
$!	If properly configured, it is evoked via URL's of the following form:
$!
$!		http://<host>[:port]/HELP[/@library][/topic[/subtopic...]]
$!
$!	or anchors in your httpd's HTML files as in the following example:
$!
$!		<A href="/HELP">VMS HELP</A>
$!		<A href="/HELP/@MULTINET:MULTINET">MultiNet HELP</A>
$!
$!	The symbol WWW_PATH_INFO, with the partial URL for the requested
$!	library and module, will have been set up by the httpd.  This
$!	script sets up the symbols:
$!
$!	WWW_HOST_ACRONYM (optional) 	A brief acronym for your site, to use
$!					 as a prefix in HTML titles.
$!	WWW_COPYRIGHT_STRING (optional)	A copyright notice for the libraries
$!					 of copyrighted software.
$!
$!	Set up the foreign command definition for VMSHelpGate, below,
$!	or define it in the HTTP Server's SpawnInit.com.
$!
$!----------------------------------------------------------------------------
$ VMSHelpGate == "$" + f$parse("VMSHELPGATE.EXE;",f$environment("PROCEDURE"))
$ write net_link "<DNETPATH>"
$ read net_link htbin_path
$ www_path_info = f$edit(p2 - htbin_path,"UPCASE")	! close enough
$ path_info_length = f$length(www_path_info)
$!-------------------------------------------------
$! Change this to an acronym appropriate for your site.
$!
$ WWW_HOST_ACRONYM = "KCGL1"
$!
$! Set up the copyrights for foreign software, and default to DEC copyright.
$!
$ IF f$locate("@GCGHELP", WWW_PATH_INFO) .lt. path_info_length
$ Then
$   WWW_COPYRIGHT_STRING = -
    "Copyright 1993, <b>Genetics Computer Group, Inc.</b> All rights reserved."
$!
$ Else IF f$locate("@MULTINET", WWW_PATH_INFO) .lt. path_info_length
$ Then
$   WWW_COPYRIGHT_STRING = "Copyright <b>TGV, Inc.</b> All rights reserved."
$!
$ Else IF f$locate("@CSWING", WWW_PATH_INFO) .lt. path_info_length
$ Then
$   WWW_COPYRIGHT_STRING = -
	"Copyright <b>Harry Flowers and Foteos Macrides</b> <A " + -
	"HREF=""gopher://sci.wfeb.edu/11%5b_FileServ._CSwing%5d"" " + -
	"TITLE=""C Swing"">FreeWare</A>."
$!
$ ELSE  ! Default
$   WWW_COPYRIGHT_STRING = -
	"Copyright <b>Digital Equipment Corp.</b> All rights reserved."
$!
$ EndIF ! GCG
$ EndIF ! MultiNet
$ EndIF ! C Swing
$!
$ VMSHelpGate "''p1'" "''p2'" "''P3'"
$exit
