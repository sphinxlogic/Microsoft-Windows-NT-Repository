$patch sys$loadable_images:esdriver
replace/inst 5FB1
'movzbl #4,R0'
'ADDL	R0,R1'
'CMPW	R1,B^32(R3)'
'BLEQU	5FD2'
'SUBL	R0,R1'
'clrl	R0'
'CMPW	R1,B^32(R3)'
'BLEQU	5FD2'
'MOVZWL	B^32(R3),R1'
'MOVW	#838,b^3A(R3)'
exit
'movzwl	b^32(R3),R0'
'cmpl	R1,R0'
'blequ	OK'
'cmpl	R1,#5DC'
'blequ	ovf'
'brw 	54EA'	!'bug	#7E4'
'ovf:	movl R0,R1'
'movw	#838,B^3A(R3)'
'ok:	clrl	R0'
exit
update
exit
