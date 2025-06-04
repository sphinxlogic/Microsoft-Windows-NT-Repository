$! Build of XMag for OpenVMS VAX or OpenVMS AXP
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
$ set def 'f$element(0,"]",workdir)xmag]'
$ write sys$output ""
$ write sys$output "    [1mBuilding XMAG for OpenVMS ''cpu' - Dual Architecture VMScluster Demo[0m"
$ write sys$output ""
$ if cpu .eqs. "AXP" then cc :== cc/standard=vaxc
$ define/nolog sys sys$library
$ write sys$output "Compiling XMAG.C..."
$ cc/obj=xmag_'cpu' xmag.c
$ write sys$output "Linking XMAG..."
$ write sys$output ""
$ if cpu .eqs. "AXP"
$    then
$       link/exe=xmag_'cpu' -
                xmag_'cpu',-
                sys$input/option
                sys$share:decw$xlibshr/share
$ endif
$ if cpu .eqs. "VAX"
$    then                                    
$       link/exe=xmag_'cpu' -                
                xmag_'cpu',-                 
                sys$input/option             
                sys$share:decw$xlibshr/share
                sys$share:vaxcrtl/share
$ endif
$ purge/nolog
$ set def 'olddir'
$ exit
