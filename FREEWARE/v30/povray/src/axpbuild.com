$ if P1 .nes. "" then goto 'P1'
$ define sys sys$include:
$ options = "/nolist/DEFINE=(XWINDOWS,ALPHA)/optimize/notrace/list"
$ write sys$output "Compile for ALPHA"
$ cc 'options' BEZIER
$ cc 'options' BLOB
$ cc 'options' BOUND
$ cc 'options' BOXES
$ cc 'options' CAMERA
$ cc 'options' COLOUR
$ cc 'options' CONES
$ cc 'options' CSG
$ cc 'options' DISCS
$ cc 'options' DUMP
$ cc 'options' EXPRESS
$ cc 'options' GIF
$ cc 'options' GIFDECOD
$ cc 'options' HFIELD
$ cc 'options' IFF
$ cc 'options' IMAGE
$ cc 'options' LIGHTING
$ cc 'options' MATRICES
$ cc 'options' NORMAL
$ cc 'options' OBJECTS
$ cc 'options' PARSE
$ cc 'options' PIGMENT
$ cc 'options' PLANES
$ cc 'options' POINT
$ cc 'options' POLY
$ cc 'options' POVRAY
$ cc 'options' QUADRICS
$ cc 'options' RAW
$ cc 'options' RAY
$ cc 'options' RENDER
$ cc 'options' SPHERES
$ cc 'options' TARGA
$ cc 'options' TEXTURE
$ cc 'options' TOKENIZE
$ cc 'options' TRIANGLE
$ cc 'options' TXTTEST
$ cc 'options' VAX                                                
$ cc 'options' VECT
$ cc 'options' XWIND
$
$ lib/replace/log povray *.obj
$ del *.obj;*
$
$link:
$ write sys$output "Link"
$ link /nomap       -
       /notrace     -
       /native_only -
       /exe=povray  -
        POVRAY/include=( -
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
		VAX            , -
		VECT           , -
		XWIND) ,-
		POVRAY/OPT
$ exit
