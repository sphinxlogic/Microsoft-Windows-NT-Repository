$! Build of XPOOL for OpenVMS VAX or OpenVMS AXP 
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
$ set def 'f$element(0,"]",workdir)xpool]'
$ if cpu .eqs. "AXP" then cc :== "cc/standard=vaxc"
$ write sys$output ""
$ write sys$output "     [1mBuilding XPOOL for OpenVMS ''cpu' - Dual Architecture VMScluster Demo[0m"
$ write sys$output ""
$ write sys$output "Compiling XPOOL.C..."
$ cc/object=xpool_'cpu' xpool.c
$ write sys$output "Compiling XPOOL_XLIB.C..."
$ cc/object=xpool_xlib_'cpu' xpool_xlib.c 
$ write sys$output "Compiling XPOOL_DWT.C..."
$ cc/object=xpool_dwt_'cpu' xpool_dwt.c  
$ write sys$output "Compiling XPOOL_XRM.C..."
$ cc/object=xpool_xrm_'cpu' xpool_xrm.c 
$ write sys$output "Compiling XPOOL_NET.C..."
$ cc/object=xpool_net_'cpu' xpool_net.c
$ write sys$output "Compiling XPOOL_ENQ.C..."
$ cc/object=xpool_enq_'cpu' xpool_enq.C
$ write sys$output "Linking XPOOL..."
$ write sys$output ""
$ if cpu .eqs. "AXP
$    then
$       link/exe=xpool_'cpu' sys$input/option
                xpool_axp
                xpool_xlib_axp
                xpool_dwt_axp
                xpool_xrm_axp
                xpool_net_axp
	        xpool_enq_axp
                sys$share:decw$dwtlibshr/share
                sys$share:decw$xlibshr/share
                sys$share:decw$xtshr/share
$    else
$       link/exe=xpool_'cpu' sys$input/option
                xpool_vax
                xpool_xlib_vax
                xpool_dwt_vax
                xpool_xrm_vax
                xpool_net_vax
                xpool_enq_vax
                sys$share:decw$dwtlibshr/share
                sys$share:decw$xlibshr/share
$ endif
$ purge/nolog
$ set def 'olddir'
$ exit
