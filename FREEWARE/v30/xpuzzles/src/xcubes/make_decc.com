$ set verify
$ cc="CC/DECC/STANDAR=VAXC"
$ if p1.eqs."DEBUG" .or. p2.eqs."DEBUG"
$ then
$ cc=="CC/DECC/DEB/NOOPT/STANDAR=VAXC"
$   link=="LINK/DEB"
$ endif
$ if p1.nes."LINK"
$ then
$! define sys sys$library
$!
$ cc rngs.c
$ cc Cubes.c
$ cc CubesU.c
$ cc xcubes.c
$ endif
$!
$! We fisrt test the version of DECW/Motif ; if 1.2 we need to link with new
$! X11R5 libraries
$@sys$update:decw$get_image_version sys$share:decw$xlibshr.exe decw$version
$ if f$extract(4,3,decw$version).eqs."1.2"
$ then
$! DECW/Motif 1.2 : link with X11R5 libraries
$ link/map xcubes,Cubes,CubesU,rngs,-
       sys$input/opt
sys$share:decw$dxmlibshr12/share
sys$share:decw$xlibshr/share
$ else
$! Else, link with X11R4 libraries
$ link/map xcubes,Cubes,CubesU,rngs,-
       sys$input/opt
sys$share:decw$dxmlibshr/share
sys$share:decw$xlibshr/share
$ endif
$ set noverify
$ exit
