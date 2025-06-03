$ set noon
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
$ if f$getsyi("cpu") .eq. 128 
$    then 
$       cpu = "AXP"
$    else
$       cpu = "VAX"
$ endif
$ define/nolog sys$output nl:
$ define/nolog sys$error nl:
$ define/nolog xpool$me "OpenVMS ''cpu'"
$ define/nolog decw$user_defaults demos:[xpool]
$ run demos:[xpool]xpool_'cpu'.exe
$ deassign sys$error
$ deassign sys$output
$ exit
