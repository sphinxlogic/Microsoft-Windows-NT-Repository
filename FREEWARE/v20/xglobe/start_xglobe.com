$ set noon
$ if p1 .eqs. "" then p1 = "256"
$ if p1 .nes. "64" .and. p1 .nes "128" .and p1 .nes "256"
$    then
$       write sys$output ""
$       write sys$output "[1m***[0m Must specify a [1mP1[0m parameter of [1m64[0m, [1m128[0m or [1m256[0m"
$       write sys$output ""
$       exit
$ endif
$ if p2 .nes. "" .and. p2 .nes. "R" .and p2 .nes. "r"
$    then
$       write sys$output ""
$       write sys$output "[1m***[0m Must specify a [1mP2[0m parameter of [1mR[0m or leave it blank"
$       write sys$output ""
$       exit
$ endif
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
$ set def 'f$element(0,"]",workdir)xglobe]'
$ define/nolog globe_bitmap demos:[xglobe]xglobe_'p1''p2'.dat
$ if f$getsyi("cpu") .eq. 128
$    then
$       cpu = "AXP"
$    else
$       cpu = "VAX"
$ endif
$ xglobe :== "$ demos:[xglobe]xglobe_''p1'_''cpu'.exe"
$ define/nolog sys$output nl:
$ define/nolog sys$error nl:
$ if cpu .eqs. "AXP" then xglobe -x 500 -y 300 -d 75 -t "Globe - Decus Spring 93"
$ if cpu .eqs. "VAX" then xglobe -x 500 -y 300 -d 75 -t "Globe - Decus Spring 93"
$ deassign sys$error
$ deassign sys$output
$ set def 'olddir'
$ exit
