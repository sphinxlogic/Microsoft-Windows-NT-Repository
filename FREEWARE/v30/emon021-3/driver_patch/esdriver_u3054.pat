!
!	generic patch for ETHERNET_CMN_RTN V5.4-3
!	applied to ESDRIVER.	
!	values needed: ECR$RECEIVE_FDT & patch area
!	ESDRIVER V5.4-3
!
$patch sys$common:[sys$ldr]esdriver
!
define ecr$receive_fdt= 2f46
set patch 7555
!
! sanity check
!
verify/inst ecr$receive_fdt = 'movb #2,B^21(r3)'
show patch
!
! the diag buffer is big enough since PRM is enabled (thanks V5.4-3)
!	we could avoid 1 test in the readblk fdt if line mode was
!	forced to 802.3E in prm mode
!
!
verify/inst ecr$receive_fdt+74 = 'movzwl #14,r0'! sanity test
replace/inst ecr$receive_fdt+66
'blbs 	w^10c(r5),ecr$receive_fdt+74'		! useless since the frame is.
exit						! parsed using the LINFMT param
'brb	ecr$receive_fdt+74'			! instead of the received frame
exit						! type
!
!	unfortunately, the CRX$B_R_FMT field does not seem to be initialized
!
define vfy_802e	= ecr$receive_fdt+2afb
define rcv_eth 	= ecr$receive_fdt+2b01
define rcv_802e	= ecr$receive_fdt+2b05
verify/inst rcv_eth = 'movw b^0c(r1),(r0)+'
verify/inst rcv_802e = 'movzwl b^4e(r5),r1'
insert/inst vfy_802e = 'movl B^12(r1),(r0)+'	! now, 802E frame OR prm mode
'blbc	w^10c(r5),.+3'		! br if promiscuous mode
'brw	rcv_802e'		! fall back in normal line
'movl	r0,-(sp)'
'movzbl B^0c(r1),r0'
'ashl	#8,r0,r0'
'movb	B^0d(r1),r0'
'cmpw	r0,#5dc'		! ethernet format ?
'popr	#1'			! restore r0 (does not modify cc bits)
'bgtru	.+3'			! br if ethernet
'brw	rcv_802e'		! fall back in normal line (802/802e will be
				! differentiated by pgm)
'clrq	-(r0)'			! r0 points to the protocol field
				! clear non relevant fields
'brw	rcv_eth'		! and store protocol bytes
exit
!
update
exit
