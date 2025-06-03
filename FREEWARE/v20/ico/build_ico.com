$! Build of ICO for OpenVMS VAX or OpenVMS AXP
$!
$ if f$getsyi("cpu") .eq. 128
$    then
$       cpu = "AXP"
$    else
$       cpu = "VAX"
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
$ set def 'f$element(0,"]",workdir)ico]'
$ write sys$output ""
$ write sys$output "      [1mBuilding ICO for OpenVMS ''cpu' - Dual Architecture VMScluster Demo[0m"
$ write sys$output ""
$ if cpu .eqs. "AXP" then cc :== cc/standard=vaxc
$ define/nolog sys sys$library
$ write sys$output "Compiling ICO.C..."
$ cc/obj=ico_'cpu' ico.c
$ write sys$output "Linking ICO..."
$ write sys$output ""
$ link/exe=ico_'cpu' -
        ico_'cpu',-
        sys$input/option
        sys$share:decw$xlibshr/share
        sys$share:decw$dwtlibshr/share
$ purge/nolog
$ set def 'olddir'
$ exit
