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
$ set def 'f$element(0,"]",workdir)colormap]'
$ if f$getsyi("cpu") .eq. 128
$    then
$       cpu = "AXP"
$    else
$       cpu = "VAX"
$ endif
$ colormap :== "$demos:[colormap]colormap_''cpu'"
$ colormap -geom +500+400
$ set def 'olddir'
$ exit
