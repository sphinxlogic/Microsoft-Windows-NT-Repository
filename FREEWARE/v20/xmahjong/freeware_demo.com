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
$ xmahjongg :== $'workdir'xmahjongg_'cpu'
$ define/nolog sys$error nl:
$ define/nolog sys$output nl:
$ define/user xmahjong_dir 'workdir'
$ xmahjongg
$ deassign sys$error
$ deassign sys$output
$ set def 'olddir'
$ exit
