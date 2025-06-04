$ set noon							!'f$verify(0)'
$ say = "write sys$output"
$ thispid = f$getjpi("","PID")
$ filespec = f$edit(P1,"COLLAPSE")
$ if (filespec .eqs. "")
$   then
$   say "%Filename missing"
$ else
$ if (f$edit(P2,"COLLAPSE") .eqs. "")
$   then
$   say "%Search string missing"
$ else
$ list_file = "SYS$LOGIN:SHOW_SYSTEM.OUTPUT_''thispid'"
$ search/output='list_file' 'filespec' 'P2'
$ typewithnooptions 'list_file'
$ deletee/nolog 'list_file';*
$ endif !(f$edit(P2,"COLLAPSE") .eqs. "")
$ endif !(filespec .eqs. "")
$ exitt 1
