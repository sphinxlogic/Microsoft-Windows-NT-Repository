$ v=f$verify(0)
$!
$!	DO.COM
$!
$!	Written by Gregg Wonderly 30-Oct-1986
$!
$! write sys$output "$ ''P1' ''P2' ''P3' ''P4' ''P5' ''P6' ''P7' ''P8'"
$ write sys$output "$ ",P1," ",P2," ",P3," ",P4," ",P5," ",P6," ",P7," ",P8
$ 'P1' 'P2' 'P3' 'P4' 'P5' 'P6' 'P7' 'P8'
$ EXIT $status+('f$verify(v)'*0)
