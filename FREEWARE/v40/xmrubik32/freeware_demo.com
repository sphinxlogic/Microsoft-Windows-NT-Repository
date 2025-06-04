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
$ xmrubik :== $'workdir'xmrubik_'cpu'
$ define/nolog sys$error nl:
$ define/nolog sys$output nl:
$ xmrubik
$ deassign sys$error
$ deassign sys$output
$ set def 'olddir'
$ exit
