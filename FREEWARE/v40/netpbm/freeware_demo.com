$ If F$GetSyi ("HW_MODEL") .gt. 1023 
$   Then        ! it's an ALPHA
$       Write Sys$Output "Choosing Alpha hardware..."
$       @ [.NetPBM-1jan96_VMS-ALP]SetUp.com
$   Else        ! it's a VAX
$       Write Sys$Output "Choosing VAX hardware..."
$       @ [.NetPBM-1jan96_VMS-VAX]SetUp.com
$   EndIf
$ Exit
