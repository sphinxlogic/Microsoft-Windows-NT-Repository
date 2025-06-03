!
!	generic patch for module CSMACD X-15
!	applied to ESDRIVER X-16
!	values needed: LAN$setup_pte addr (from .map driver file)
!
$patch sys$common:[sys$ldr]esdriver
!
define lan$setup_pte =	2d5d	! map
!
define lan$receive_fdt= lan$setup_pte+ 202
define lan$patch_desc = lan$setup_pte+45bf
set patch	lan$patch_desc
!
set eco 97
!
!	parse frame against it's format and not the line format
!
!
! sanity check
!
verify/inst lan$receive_fdt = 'movb #2,B^21(r3)'
show patch
!
! the diag buffer is big enough since PRM is enabled (thanks V5.4-3)
!	we could avoid 1 test in the readblk fdt if line mode was
!	forced to 802.3E in prm mode
!
!
verify/inst lan$receive_fdt+74 = 'movzwl #14,r0'! sanity test
replace/inst lan$receive_fdt+66
'blbs 	w^10c(r5),lan$receive_fdt+74'		! useless since the frame is.
exit						! parsed using the LINFMT param
'brb	lan$receive_fdt+74'			! instead of the received frame
exit						! type
!
!	unfortunately, the CRX$B_R_FMT field does not seem to be initialized
!
define vfy_802e	= lan$receive_fdt+2b09	!afb
define rcv_eth 	= vfy_802e+6
define rcv_802e	= rcv_eth+4
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
update
!
set eco 98
!
!	if promiscuous mode, clear padding and set 802E mode
!
define ilan$val_combo = lan$receive_fdt+26e
replace/inst	ilan$val_combo+5
'brw ilan$val_combo+8f'
exit
'nop'
exit
insert/inst ilan$val_combo+5
'nop'
'movb	#1,W^113(r5)'		! UCB$B_NI_PAD(r5) = NMA$C_STATE_OFF
'clrb	W^0F5(r5)'		! UCB$B_NI_FMT(r5) = NMA$C_FMT_802E
'brw	ilan$val_combo+8f'
exit
update
!
exit
