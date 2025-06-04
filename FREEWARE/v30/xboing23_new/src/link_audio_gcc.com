$! We fisrt test the version of DECW/Motif ; if 1.2 we need to link with new
$! X11R5 libraries
$@sys$update:decw$get_image_version sys$share:decw$xlibshr.exe decw$version
$ if f$extract(4,3,decw$version).eqs."1.2"
$ then
$! DECW/Motif 1.2 : link with X11R5 libraries
$     link/exe=xboing23.exe main,vms_audio_gcc_12.opt/opt
$ else
$! Else, link with X11R4 libraries
$     link/exe=xboing23.exe main,vms_audio_gcc.opt/opt
$ endif
