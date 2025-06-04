$! Build of YADMAN for OpenVMS VAX or OpenVMS AXP
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
$ set def 'f$element(0,"]",workdir)yadman]'
$ write sys$output ""
$ write sys$output "    [1mBuilding YADMAN for OpenVMS ''cpu' - Dual Architecture VMScluster Demo[0m"
$ write sys$output ""
$ if cpu .eqs. "AXP" then cc :== cc/standard=vaxc/float=d_float
$ write sys$output "Compiling YADMAN.C..."
$ cc/obj=yadman_'cpu' yadman.c
$ write sys$output "Compiling YADMAN.UIL..."
$ uil yadman.uil
$ write sys$output "Linking YADMAN..."
$ write sys$output ""
$ link/nodebug/exe=yadman_'cpu' -
        yadman_'cpu',-
        sys$input/option
        sys$share:decw$xlibshr/share
        sys$share:decw$dwtlibshr/share
        sys$share:decw$xtshr/share
$ purge/nolog
$ set def 'olddir'
$ exit

