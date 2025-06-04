$! Build of ColorMap for OpenVMS VAX or OpenVMS AXP
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
$ set def 'f$element(0,"]",workdir)colormap]'
$ write sys$output ""
$ write sys$output "    [1mBuilding ColorMap for OpenVMS ''cpu' - Dual Architecture VMScluster Demo[0m"
$ write sys$output ""
$ if cpu .eqs. "AXP" then cc :== cc/standard=vaxc
$ define/nolog sys sys$library
$ write sys$output "Compiling COLORMAP.C..."
$ cc/obj=colormap_'cpu' colormap.c
$ write sys$output "Compiling COLORMAP.UIL..."
$ uil colormap.uil
$ write sys$output "Linking COLORMAP..."
$ write sys$output ""
$ link/exe=colormap_'cpu' -
        colormap_'cpu',-
        sys$input/option
        sys$share:decw$xlibshr/share
        sys$share:decw$dwtlibshr/share
        sys$share:decw$xtshr.exe/share
$ purge/nolog
$ set def 'olddir'
$ exit
