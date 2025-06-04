$! Ascii Art viewer launcher
$! Usage:
$! $ @asciiview filename [width [height]]
$!
$ IF P1 .EQS. "" THEN GOTO nofich
$ write sys$output "%ASCVIEW-I-CONVPNM, Converting image to PNM format"
$ mconvert 'P1' aviewtemp.pnm
$ write sys$output "%ASCVIEW-I-LAUNCH, Launching Ascii Art Viewer"
$ if P2 .EQS. "" 
$   then
$     aview aviewtemp.pnm
$   else
$     if P3 .EQS. "" 
$      then
$        aview -width 'P2' -height 32  aviewtemp.pnm
$      else
$        aview -width 'P2' -height 'P3' aviewtemp.pnm
$     endif
$  endif
$ delete/noconf aviewtemp.pnm;
$ fini:
$ exit
$ nofich:
$ write sys$output "%ASCVIEW-F-NOFILE, No image file specified"
$ exit
