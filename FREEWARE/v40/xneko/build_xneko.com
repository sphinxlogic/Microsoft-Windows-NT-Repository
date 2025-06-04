$! Build of XNEKO for OpenVMS VAX or OpenVMS AXP
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
$ set def 'f$element(0,"]",workdir)xneko]'
$ if cpu .eqs. "AXP" then cc :== "cc/standard=vaxc/prefix=all"
$ write sys$output ""
$ write sys$output "      [1mBuilding XNEKO for OpenVMS ''cpu' - Dual Architecture VMScluster Demo[0m"
$ write sys$output ""
$ write sys$output "Compiling XNEKO.C..."
$ cc/object=xneko_'cpu' xneko
$ cc/object=smpausewindow_'cpu' smpausewindow
$ write sys$output "Linking XNEKO..."
$ if cpu .eqs. "AXP
$    then
$       link/exe=xneko_'cpu' sys$input/option
                smpausewindow_axp
                xneko_axp
                sys$share:decw$xlibshr/share
$    else
$       link/exe=xneko_'cpu' sys$input/option
                smpausewindow_vax
                xneko_vax
                sys$share:decw$xlibshr/share
                sys$share:vaxcrtl/share
$ endif        
$ purge/nolog
$ set def 'olddir'
$ exit
