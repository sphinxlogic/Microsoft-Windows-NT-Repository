$!freeware_demo.com for HTML VALIDATOR
$ set symbol/scope=(noglobal,nolocal)
$
$ on control_y then goto exit
$ on error then goto exit
$ thisdir=f$parse(f$environ("procedure"),,,"device")+ -
	f$parse(f$environ("procedure"),,,"directory")
$
$ write sys$output "This procedure will create several files with the"
$ write sys$output "extension .htm to distinguish from the .html files."
$ write sys$output ""
$ write sys$output "You can read the AAAREADME file, and you can abort"
$ write sys$output "before or during execution."
$ write sys$output ""
$ inquire/nopunc p1 " * Do you wish to view the AAAREADME file? [NO] "
$ if p1 then type/page 'thisdir'aaareadme.txt
$again:
$ write sys$output ""
$ inquire/nopunc p1 " * Do you wish to run the procedures? [NO] "
$ if .not.p1 then goto exit
$ write sys$output ""
$ inquire/punc p2 "Directory of HTML files" 
$ p1=f$parse(p2,,,"device")+f$parse(p2,,,"directory")
$ if p1.eqs.""
$	then
$	write sys$output p1," is not valid."
$	goto again
$	endif
$ if f$search("''p1'*.html").eqs.""
$	then
$	write sys$output "Files called *.HTML cannot be found in ''p1'"
$	write sys$output "required for the demo. Extract the files and"
$	write sys$output "change to suit your environment."
$	goto again
$	endif
$ @'thisdir'debug-make 'p1'
$	
$ write sys$output "Load debug.htm into your browser when viewing the "
$ write sys$output "files in ''p2'. Hope you find it useful."
$ inquire/nopunc p1 " Press RETURN to return to Freeware"
$exit:
$ exit 1
