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
$ define ACM_LIBRARY 'workdir'
$ acm :== $acm_library:acm_'cpu'
$ acms:== $acm_library:acms_'cpu'
$! define/nolog sys$error nl:
$! define/nolog sys$output nl:
$ spawn/nowait acms
$ wait 00:00:05
$ acm
$! deassign sys$error
$! deassign sys$output
$ set def 'olddir'
$ exit
