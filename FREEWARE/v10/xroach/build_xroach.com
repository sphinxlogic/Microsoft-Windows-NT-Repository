$! Build of XROACH for OpenVMS VAX or OpenVMS AXP
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
$ set def 'f$element(0,"]",workdir)xroach]'
$ if cpu .eqs. "AXP" then cc :== "cc/standard=vaxc/prefix=all"
$ write sys$output ""
$ write sys$output "     [1mBuilding XROACH for OpenVMS ''cpu' - Dual Architecture VMScluster Demo[0m"
$ write sys$output ""
$ write sys$output "Compiling XROACH.C..."
$ cc/object=xroach_'cpu' xroach
$ write sys$output "Linking XROACH..."
$ if cpu .eqs. "AXP"
$    then
$       link/exe=xroach_'cpu' sys$input/option
                xroach_axp
                sys$share:decw$xlibshr/share
$    else
$       link/exe=xroach_'cpu' sys$input/option
                xroach_vax
                sys$share:vaxcrtl/share
                sys$share:decw$xlibshr/share
$ endif
$ purge/nolog
$ set def 'olddir'
$ exit
