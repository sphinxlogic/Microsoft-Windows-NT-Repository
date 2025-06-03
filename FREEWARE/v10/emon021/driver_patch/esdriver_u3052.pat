!
!	generic patch for ETHERNET_CMN_RTN
!	applied to ESDRIVER.	
!	values needed: ECR$RECEIVE_FDT & driver specific patch area
!	ATTENTION: il y a 2 varietes d'ESDRIVER...
!
$patch sys$loadable_images:Esdriver
define ecr$receive_fdt= 2358
set patch_area 5251
! sanity check
verify/inst ecr$receive_fdt = 'clrw b^30(r3)'
verify/inst ecr$receive_fdt+63 = 'movzwl b^4(ap),r1'
! verify the diag buffer is big enough, and if so, clear the 802E specific area
insert/inst ecr$receive_fdt+60 = 'movzwl #14,r0'
'movl b^4c(r3),r1'
'cmpw #20,B^8(r1)'
'bgtru ecr$receive_fdt+63'
'clrl B^1a(r1)'
'clrw B^1e(r1)'
exit
update
exit
