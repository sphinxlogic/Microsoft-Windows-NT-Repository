$ set noon
$ olddir = f$environment("default")
$ workdir = f$trnlnm("demos")
$ if workdir .eqs. ""
$    then
$       write sys$output ""
$       write sys$output "Logical name [1mDEMOS[0m must be defined, eg: "
$       write sys$output ""
$       write sys$output "$ DEFINE/SYSTEM/EXECUTIVE DEMOS WORK:[SMITH.]"
$       write sys$output ""
$       write sys$output "[1mNOTE:[0m Be sure to include the ""."" following the last directory name" 
$       write sys$output ""
$       exit
$ endif
$ set def 'f$element(0,"]",workdir)xswarm]'
$ if f$getsyi("cpu") .eq. 128
$    then
$       cpu = "AXP"
$    else
$       cpu = "VAX"
$ endif
$ xswarm :== "$demos:[xswarm]xswarm_''cpu'"
$ define/nolog sys$error nl:
$ define/nolog sys$output nl:
$ xswarm -g 800x800-625+150 -c red "-S" 40 -d 500 "-B" 20 -t "XSwarm Demo - Decus Spring 1993"
$ deassign sys$error
$ deassign sys$output
$ set def 'olddir'
$ exit
