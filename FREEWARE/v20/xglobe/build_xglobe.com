$! Build of XGLOBE for OpenVMS VAX or OpenVMS AXP
$!
$ if p1 .eqs. "" then p1 = "256"
$ if p1 .nes. "64" .and. p1 .nes "128" .and p1 .nes "256"
$    then
$       write sys$output ""
$       write sys$output "[1m***[0m Must specify a [1mP1[0m parameter of [1m64[0m, [1m128[0m or [1m256[0m"
$       write sys$output ""
$       exit
$ endif
$ if p1 .eqs. 256 then size = "LARGE"
$ if p1 .eqs. 128 then size = "MEDIUM"
$ if p1 .eqs. 64  then size = "SMALL"
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
$ set def 'f$element(0,"]",workdir)xglobe]'
$ if cpu .eqs. "AXP" then cc :== "cc/standard=vaxc/prefix=all"
$ write sys$output ""
$ write sys$output " [1mBuilding XGLOBE (''p1'x''p1') for OpenVMS ''cpu' - Dual Architecture VMScluster Demo[0m"
$ write sys$output ""
$ write sys$output "Compiling XGLOBE.C..."
$ cc/obj=xglobe_'p1'_'cpu'/define='size' xglobe.c
$ write sys$output "Linking XGLOBE..."
$ write sys$output ""
$ link/exe=xglobe_'p1'_'cpu' xglobe_'p1'_'cpu',sys$input/option
        sys$share:decw$xtshr/share
        sys$share:decw$xlibshr/share
	sys$share:decw$dwtlibshr/share
$ purge/nolog
$ set def 'olddir'
$ exit
