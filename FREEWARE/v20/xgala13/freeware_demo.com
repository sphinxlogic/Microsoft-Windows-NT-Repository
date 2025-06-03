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
$ xgala :== $'workdir'xgala_'cpu'
$ define/nolog sys$error nl:
$ define/nolog sys$output nl:
$ define/user XGALA_DIR 'workdir'
$ xgala
$ deassign sys$error
$ deassign sys$output
$ set def 'olddir'
$ exit
