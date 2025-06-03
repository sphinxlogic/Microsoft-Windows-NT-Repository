$ define sys sys$include:
$ opt = "/nolist/opt/define=(XWINDOWS)"
$ if P1 .nes. "" then goto 'P1'
$ cc'opt' BEZIER
$ cc'opt' BLOB
$ cc'opt' BOUND
$ cc'opt' BOXES
$ cc'opt' CAMERA
$ cc'opt' COLOUR
$ cc'opt' CONES 
$ cc'opt' CSG
$ cc'opt' DISCS
$ cc'opt' DUMP
$ cc'opt' EXPRESS
$ cc'opt' GIF
$ cc'opt' GIFDECOD
$ cc'opt' HFIELD
$ cc'opt' IFF
$ cc'opt' IMAGE
$ cc'opt' LIGHTING
$ cc'opt' MATRICES
$ cc'opt' NORMAL  
$ cc'opt' OBJECTS
$ cc'opt' PARSE
$ cc'opt' PIGMENT
$ cc'opt' PLANES
$ cc'opt' POINT
$ cc'opt' POLY
$ cc'opt' POVRAY
$ cc'opt' QUADRICS
$ cc'opt' RAW
$ cc'opt' RAY
$ cc'opt' RENDER
$ cc'opt' SPHERES
$ cc'opt' TARGA
$ cc'opt' TEXTURE
$ cc'opt' TOKENIZE
$ cc'opt' TRIANGLE
$ cc'opt' TXTTEST
$ cc'opt' VAX
$ cc'opt' VECT
$ cc'opt' XWIND
$LINK:
$ link/nomap/exe=povray -
POVRAY         , -
BEZIER         , -
BLOB           , -
BOUND          , -
BOXES          , -
CAMERA         , -
COLOUR         , -
CONES          , -
CSG            , -
DISCS          , -
DUMP           , -
EXPRESS        , -
GIF            , -
GIFDECOD       , -
HFIELD         , -
IFF            , -
IMAGE          , -
LIGHTING       , -
MATRICES       , -
NORMAL         , -
OBJECTS        , -
PARSE          , -
PIGMENT        , -
PLANES         , -
POINT          , -
POLY           , -
QUADRICS       , -
RAW            , -
RAY            , -
RENDER         , -
SPHERES        , -
TARGA          , -
TEXTURE        , -
TOKENIZE       , -
TRIANGLE       , -
TXTTEST        , -
XWIND          , -
VAX            , -
VECT, sys$input/opt
sys$share:vaxcrtl/share
sys$share:decw$xlibshr/share
sys$share:decw$xtshr/share
sys$share:decw$dwtshr/share
sys$share:decw$dwtlibshr/share
sys$share:decw$xmlibshr/share
sys$share:decw$xextlibshr/share
sys$share:decw$dxmlibshr/share
sys$share:decw$terminalshr/share
$ exit
