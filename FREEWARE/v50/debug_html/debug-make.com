$!debug-make.com. nic clews. Latest edit February 2001.
$! produces htm debug files. assumes remaining command procedures are
$! in this directory, and if P1 is not passed, that the html files to
$! be tested are in the current default directory
$
$ set symbol/scope=(noglobal,nolocal)
$ set noon
$ on control_y then exit
$ thisdir=f$parse(f$envir("procedure"),,,"device")+ -
	f$parse(f$envir("procedure"),,,"directory")
$
$ if p1.eqs."" then p1=f$environment("default")
$ if p2.eqs."" then p2="index.html"	! which files do not link this?
$ if p3.eqs."" then p3="index.html"	! which files are not linked by this?
$
$ set nover
$ if f$search("''p1'debug.htm").eqs."" then -
	copy/log 'thisdir'debug.htm 'p1'
$
$ write sys$output "making check references and check links ",f$time()
$@'thisdir'check-ref 'p1'
$ write sys$output "making links to all html files ",f$time()
$@'thisdir'link-html 'p1'
$ write sys$output "making links to all files ",f$time()
$@'thisdir'link-all 'p1'
$ write sys$output "making alphabetic list of image links ",f$time()
$@'thisdir'image-links 'p1'
$ write sys$output "making unused/ unreferenced gif page ",f$time()
$@'thisdir'unused-gif 'p1'
$ write sys$output "making unused/ unreferenced jpg page ",f$time()
$@'thisdir'unused-jpg 'p1'
$ write sys$output "making unused/ unreferenced * page ",f$time()
$@'thisdir'unused-any 'p1'
$ write sys$output "making internal references file ",f$time()
$@'thisdir'references 'p1'
$ write sys$output "making html not containing ''p2' file ",f$time()
$@'thisdir'notcontained 'p1' 'p2'
$ write sys$output "making html not referenced by ''p3' file ",f$time()
$@'thisdir'notreferenced 'p1' 'p3'
$ write sys$output "done at ",f$time()
$ exit 1
