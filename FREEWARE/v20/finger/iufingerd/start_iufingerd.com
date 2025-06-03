$ on warning   then exit
$ on control_y then exit
$!
$ infile  := sys$nowhere:[nobody]iufingerd.com
$ outfile := sys$nowhere:[nobody]iufingerd.log
$!
$ run/detached         -
     /uic=[SYSTEM]     -
     /privilege=(setprv) -
     /input='infile'   -
     /output='outfile' -
     /error='outfile'  -
     /time=0           -
     SYS$SYSTEM:LOGINOUT.EXE
$!
$ exit
