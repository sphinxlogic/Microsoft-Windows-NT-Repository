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
$ net3dclient :== $'workdir'net3dclient_'cpu'
$! define/nolog sys$error nl:
$! define/nolog sys$output nl:
$ define/user net3d_dir 'workdir'
$ net3dclient map.i tank.i
$! deassign sys$error
$! deassign sys$output
$ set def 'olddir'
$ exit
