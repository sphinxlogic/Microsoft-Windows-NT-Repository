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
$ set def 'f$element(0,"]",workdir)sunclock]'
$ if f$getsyi("cpu") .eq. 128
$    then
$       cpu = "AXP"
$    else
$       cpu = "VAX"
$ endif
$ sunclock :== "$demos:[sunclock]sunclock_''cpu'"
$ define/nolog sys$error nl:
$ define/nolog sys$output nl:
$ sunclock -zone "EDT" -offset 4 -wtitle "SunClock Demo - Decus Spring 1993" -ititle "SunClock" -geometry +300+300
$ deassign sys$error
$ deassign sys$output
$ set def 'olddir'
$ exit
