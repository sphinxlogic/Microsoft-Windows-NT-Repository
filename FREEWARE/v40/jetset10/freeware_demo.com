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
$ jetset :== $'workdir'jetset_'cpu'
$ define/nolog sys$error nl:
$ define/nolog sys$output nl:
$ jetset
$ deassign sys$error
$ deassign sys$output
$ set def 'olddir'
$ exit
