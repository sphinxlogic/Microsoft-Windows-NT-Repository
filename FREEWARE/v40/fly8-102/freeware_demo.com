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
$ fly :== "$''workdir'fly8_''cpu' """-F FLY8_DIR:""""
$ define/nolog sys$error nl:
$ define/nolog sys$output nl:
$ set def sys$login    ! Needed to create FLY.LOG file
$ define/user FLY8_DIR 'workdir'
$ fly -z5
$ deassign sys$error
$ deassign sys$output
$ set def 'olddir'
$ exit
