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
$ xblackjack :== $'workdir'xblackjack_'cpu'
$ define/nolog sys$error nl:
$ define/nolog sys$output nl:
$ xblackjack
$ deassign sys$error
$ deassign sys$output
$ set def 'olddir'
$ exit
