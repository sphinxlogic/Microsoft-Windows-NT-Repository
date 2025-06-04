$ set noon
$ olddir = f$environment("default")
$ mydisk = f$trnlmn("SYS$DISK")
$ workdir  = mydisk+f$directory()
$ if f$getsyi("cpu") .eq. 128
$    then
$       cpu = "AXP"
$    else
$       cpu = "VAX"
$ endif
$ x11perf :== $'workdir'x11perf_'cpu'
$! define/nolog sys$error nl:
$! define/nolog sys$output nl:
$ x11perf -repeat 2 -seg100 -line100 -dline100 -circle100
$! deassign sys$error
$! deassign sys$output
$ set def 'olddir'
$ exit
