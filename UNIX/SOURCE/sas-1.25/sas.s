	.file	"sas.c"
	.version	"01.01"
gcc2_compiled.:
__gnu_compiled_c:

.section	.text
.L_text_b:
	.previous

.section	.data
.L_data_b:
	.previous

.section	.rodata
.L_rodata_b:
	.previous

.section	.bss
.L_bss_b:
	.previous

.section	.debug_sfnames
.L_sfnames_b:
	.string	"/cdcopy/SOURCE/sas-1.25/"
	.previous

.section	.line
.L_line_b:
	.4byte	.L_line_e-.L_line_b
	.4byte	.L_text_b
	.previous

.section	.debug_srcinfo
.L_srcinfo_b:
	.4byte	.L_line_b
	.4byte	.L_sfnames_b
	.4byte	.L_text_b
	.4byte	.L_text_e
	.4byte	0xffffffff
	.previous

.section	.debug_pubnames
	.4byte	.L_debug_b
	.previous

.section	.debug_aranges
	.4byte	.L_debug_b
	.previous

.section	.debug
.L_debug_b:
.L_D1:
	.4byte	.L_D1_e-.L_D1
	.2byte	0x11
	.2byte	0x12
	.4byte	.L_D2
	.2byte	0x38
	.string	"sas.c"
	.2byte	0x258
	.string	"GNU C 2.5.6"
	.2byte	0x136
	.4byte	0x1
	.2byte	0x111
	.4byte	.L_text_b
	.2byte	0x121
	.4byte	.L_text_e
	.2byte	0x106
	.4byte	.L_line_b
	.2byte	0x1b8
	.string	"/cdcopy/SOURCE/sas-1.25"
	.2byte	0x8006
	.4byte	.L_sfnames_b
	.2byte	0x8016
	.4byte	.L_srcinfo_b
.L_D1_e:
	.previous

.section	.rodata
.LC0:
	.string	"$Header: /home/roman/src/fas/sas/RCS/sas.c,v 1.25 1992/03/14 03:00:02 roman Exp $"
.data
	.align 4
	.type	 rcsid,@object
	.size	 rcsid,4
rcsid:
	.long .LC0
	.ident	"@(#)sas.c	1.20"
	.ident	"@(#)sas.h	2.09"
	.ident	"@(#)uts-x86at:util/param.h	1.8"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:util/types.h	1.11"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:fs/select.h	1.2"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:fs/s5fs/s5param.h	1.5"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:proc/signal.h	1.5"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:fs/buf.h	1.17"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:fs/dir.h	1.4"
	.ident	"$Header: dir.h 1.2 91/07/12 $"
	.ident	"@(#)uts-x86:fs/s5fs/s5dir.h	1.3"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:proc/user.h	1.12"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:proc/proc.h	1.13"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:mem/immu.h	1.8"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:mem/vmparam.h	1.7"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:proc/siginfo.h	1.3"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:util/weitek.h	1.5"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:svc/resource.h	1.4"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:proc/tss.h	1.6"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:proc/seg.h	1.5"
	.ident	"$Header: $"
	.ident	"@(#)uts-comm:mem/faultcatch.h	1.1.2.4"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:svc/errno.h	1.6"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:io/conf.h	1.5"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:util/sysinfo.h	1.6"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:fs/file.h	1.6"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:io/termio.h	1.6"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:io/termios.h	1.9"
	.ident	"$Header: termios.h 1.2 91/09/05 $"
	.ident	"@(#)uts-x86:io/ttydev.h	1.3"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:io/ioctl.h	1.5"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:io/stream.h	1.10"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:proc/cred.h	1.5"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:fs/vnode.h	1.10"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:svc/time.h	1.6"
	.ident	"$Header: $"
	.ident	"@(#)uts-comm:acc/mac/mac.h	1.15.3.4"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:io/poll.h	1.3"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86at:io/strmdep.h	1.5"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:io/stropts.h	1.4"
	.ident	"$Header: $"
	.ident	"@(#)uts-comm:svc/secsys.h	1.7.4.3"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:io/termiox.h	1.2"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:io/ldterm/euc.h	1.3"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:io/ldterm/eucioctl.h	1.2"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:io/strtty.h	1.4"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:io/stermio.h	1.1"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86:io/xt/jioctl.h	1.4"
	.ident	"$Header: $"
	.ident	"@(#)uts-x86at:util/cmn_err.h	1.6"
	.ident	"$Header: $"

.section	.debug
.L_D3:
	.4byte	.L_D3_e-.L_D3
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D4
	.2byte	0x38
	.string	"outb"
	.2byte	0x208
	.string	""
	.2byte	0x278
	.string	""
	.set	.L_E1807,.L_D3
.L_D3_e:
.L_D5:
	.4byte	.L_D5_e-.L_D5
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D6
	.2byte	0x38
	.string	"port"
	.2byte	0x55
	.2byte	0x4
	.set	.L_E1805,.L_D5
.L_D5_e:
.L_D6:
	.4byte	.L_D6_e-.L_D6
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D7
	.2byte	0x38
	.string	"val"
	.2byte	0x55
	.2byte	0x1
	.set	.L_E1806,.L_D6
.L_D6_e:
.L_D7:
	.4byte	0x4
	.previous

.section	.debug
.L_D4:
	.4byte	.L_D4_e-.L_D4
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D8
	.2byte	0x38
	.string	"inb"
	.2byte	0x208
	.string	""
	.2byte	0x278
	.string	""
	.2byte	0x55
	.2byte	0x3
	.set	.L_E1812,.L_D4
.L_D4_e:
.L_D9:
	.4byte	.L_D9_e-.L_D9
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D10
	.2byte	0x38
	.string	"port"
	.2byte	0x55
	.2byte	0x4
	.set	.L_E1811,.L_D9
.L_D9_e:
.L_D10:
	.4byte	.L_D10_e-.L_D10
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D11
	.2byte	0x38
	.string	"ret"
	.2byte	0x55
	.2byte	0x9
	.set	.L_E1816,.L_D10
.L_D10_e:
.L_D11:
	.4byte	0x4
	.previous

.section	.debug
.L_P0:
.L_D8:
	.4byte	.L_D8_e-.L_D8
	.2byte	0x6
	.2byte	0x12
	.4byte	.L_D12
	.2byte	0x38
	.string	"optim"
	.2byte	0x208
	.string	""
	.set	.L_E1817,.L_D8
.L_D8_e:
.L_D13:
	.4byte	0x4
	.previous
.text
	.align 4
.globl optim
	.type	 optim,@function
optim:

.section	.debug_sfnames
.L_F0:
	.string	"sas.c"
	.previous
.L_LC1:

.section	.line
.L_LE1:
	.4byte	118	/ sas.c:118
	.2byte	0xffff
	.4byte	.L_LC1-.L_text_b
	.previous

.section	.debug_srcinfo
	.4byte	.L_LE1-.L_line_b
	.4byte	.L_F0-.L_sfnames_b
	.previous
	pushl %ebp
	movl %esp,%ebp
.L_b4:
.L_LC2:

.section	.line
	.4byte	119	/ sas.c:119
	.2byte	0xffff
	.4byte	.L_LC2-.L_text_b
	.previous
.L_b4_e:
	leave
	ret
.L_f4_e:
.Lfe1:
	.size	 optim,.Lfe1-optim

.section	.debug_pubnames
	.4byte	.L_P1
	.string	"optim"
	.previous

.section	.debug
.L_P1:
.L_D12:
	.4byte	.L_D12_e-.L_D12
	.2byte	0x6
	.2byte	0x12
	.4byte	.L_D14
	.2byte	0x2b2
	.4byte	.L_E1817
	.2byte	0x111
	.4byte	optim
	.2byte	0x121
	.4byte	.L_f4_e
	.2byte	0x8041
	.4byte	.L_b4
	.2byte	0x8051
	.4byte	.L_b4_e
.L_D12_e:
.L_D15:
	.4byte	0x4
	.previous
.data
	.align 4
	.type	 sas_is_initted,@object
	.size	 sas_is_initted,4
sas_is_initted:
	.long 0
	.align 4
	.type	 event_scheduled,@object
	.size	 event_scheduled,4
event_scheduled:
	.long 0
	.align 4
	.type	 sas_baud,@object
	.size	 sas_baud,64
sas_baud:
	.long 15
	.long 1
	.long 2
	.long 3
	.long 4
	.long 5
	.long 6
	.long 7
	.long 8
	.long 9
	.long 10
	.long 11
	.long 12
	.long 13
	.long 14
	.long 15
	.align 4
	.type	 sas_hbaud,@object
	.size	 sas_hbaud,64
sas_hbaud:
	.long 0
	.long 1
	.long 2
	.long 3
	.long 4
	.long 5
	.long 6
	.long 7
	.long 8
	.long 9
	.long 10
	.long 11
	.long 12
	.long 13
	.long 14
	.long 0
	.align 4
	.type	 sas_speeds,@object
	.size	 sas_speeds,64
sas_speeds:
	.long 2
	.long 2304
	.long 1536
	.long 1047
	.long 857
	.long 768
	.long 576
	.long 384
	.long 192
	.long 96
	.long 64
	.long 48
	.long 24
	.long 12
	.long 6
	.long 3
	.align 4
	.type	 sas_ctimes,@object
	.size	 sas_ctimes,64
sas_ctimes:
	.long 2
	.long 32
	.long 22
	.long 15
	.long 13
	.long 12
	.long 9
	.long 7
	.long 4
	.long 3
	.long 2
	.long 2
	.long 2
	.long 2
	.long 2
	.long 2
	.align 4
	.type	 sas_bsizes,@object
	.size	 sas_bsizes,64
sas_bsizes:
	.long 400
	.long 64
	.long 64
	.long 64
	.long 64
	.long 64
	.long 64
	.long 64
	.long 64
	.long 64
	.long 64
	.long 64
	.long 64
	.long 64
	.long 128
	.long 256
	.align 4
	.type	 sas_open_modes,@object
	.size	 sas_open_modes,64
sas_open_modes:
	.long 49
	.long 241
	.long 113
	.long 369
	.long 17
	.long 209
	.long 81
	.long 337
	.long 14
	.long 206
	.long 78
	.long 334
	.long 6
	.long 198
	.long 70
	.long 326
	.align 4
	.type	 sass_info,@object
	.size	 sass_info,24
sass_info:
	.value 97
	.zero	2
	.long .LC1
	.long 0
	.long 4096
	.long 100
	.long 50
.section	.rodata
.LC1:
	.string	"SAS"
.data
	.align 4
	.type	 sas_rinit,@object
	.size	 sas_rinit,28
sas_rinit:
	.long 0
	.long sas_rsrv
	.long sas_open
	.long sas_close
	.long 0
	.long sass_info
	.long 0
	.align 4
	.type	 sas_winit,@object
	.size	 sas_winit,28
sas_winit:
	.long sas_put
	.long sas_wsrv
	.long 0
	.long 0
	.long 0
	.long sass_info
	.long 0
.globl sasinfo
	.align 4
	.type	 sasinfo,@object
	.size	 sasinfo,16
sasinfo:
	.long sas_rinit
	.long sas_winit
	.long 0
	.long 0
.section	.rodata
.LC2:
	.string	"\nStreams Async Solution 1.25: Unit 0-%d init state is [%s]\n\n"
.text
	.align 4
.globl sasinit
	.type	 sasinit,@function
sasinit:
.L_LC3:

.section	.line
	.4byte	513	/ sas.c:513
	.2byte	0xffff
	.4byte	.L_LC3-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	subl $136,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
.L_b5:
.L_LC4:

.section	.line
	.4byte	514	/ sas.c:514
	.2byte	0xffff
	.4byte	.L_LC4-.L_text_b
	.previous
.L_LC5:

.section	.line
	.4byte	520	/ sas.c:520
	.2byte	0xffff
	.4byte	.L_LC5-.L_text_b
	.previous
	cmpl $0,sas_is_initted
	jne .L5
.L_LC6:

.section	.line
	.4byte	523	/ sas.c:523
	.2byte	0xffff
	.4byte	.L_LC6-.L_text_b
	.previous
	movl $1,sas_is_initted
.L_LC7:

.section	.line
	.4byte	526	/ sas.c:526
	.2byte	0xffff
	.4byte	.L_LC7-.L_text_b
	.previous
	movl $sas_init_seq,%ebx
	cmpl $0,sas_init_seq
	je .L8
	.align 4
.L14:
.L_LC8:

.section	.line
	.4byte	528	/ sas.c:528
	.2byte	0xffff
	.4byte	.L_LC8-.L_text_b
	.previous
	movl (%ebx),%edx
	movl %edx,-128(%ebp)
.L_LC9:

.section	.line
	.4byte	529	/ sas.c:529
	.2byte	0xffff
	.4byte	.L_LC9-.L_text_b
	.previous
	addl $4,%ebx
.L_LC10:

.section	.line
	.4byte	530	/ sas.c:530
	.2byte	0xffff
	.4byte	.L_LC10-.L_text_b
	.previous
	testb $1,1(%ebx)
	je .L10
.L_LC11:

.section	.line
	.4byte	531	/ sas.c:531
	.2byte	0xffff
	.4byte	.L_LC11-.L_text_b
	.previous
.L_B3:
.L_LC12:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC12-.L_text_b
	.previous
.L_LC13:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC13-.L_text_b
	.previous
.L_B4:
.L_LC14:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC14-.L_text_b
	.previous
	movw -128(%ebp),%dx
/APP
	inb	(%dx)
/NO_APP
.L_LC15:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC15-.L_text_b
	.previous
.L_B4_e:
.L_B3_e:
.L_LC16:

.section	.line
	.4byte	531	/ sas.c:531
	.2byte	0xffff
	.4byte	.L_LC16-.L_text_b
	.previous
	jmp .L9
	.align 4
.L10:
.L_LC17:

.section	.line
	.4byte	533	/ sas.c:533
	.2byte	0xffff
	.4byte	.L_LC17-.L_text_b
	.previous
.L_B5:
.L_LC18:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC18-.L_text_b
	.previous
.L_LC19:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC19-.L_text_b
	.previous
	movb (%ebx),%al
	movw -128(%ebp),%dx
/APP
	outb (%dx)
/NO_APP
.L_LC20:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC20-.L_text_b
	.previous
.L_B5_e:
.L_LC21:

.section	.line
	.4byte	526	/ sas.c:526
	.2byte	0xffff
	.4byte	.L_LC21-.L_text_b
	.previous
.L9:
	addl $4,%ebx
	cmpl $0,(%ebx)
	jne .L14
.L8:
.L_LC22:

.section	.line
	.4byte	537	/ sas.c:537
	.2byte	0xffff
	.4byte	.L_LC22-.L_text_b
	.previous
	xorl %edi,%edi
	cmpl %edi,sas_physical_units
	jbe .L16
	movl $sas_info+32,%ebx
	movl $sas_port,-24(%ebp)
	movl $sas_info+65,-28(%ebp)
	movl $sas_info+80,-32(%ebp)
	movl $sas_info+82,-36(%ebp)
	movl $sas_info+68,%esi
	movl $sas_info+92,-40(%ebp)
	movl $sas_info+94,-44(%ebp)
	movl $sas_info+74,-48(%ebp)
	movl $sas_info+72,-52(%ebp)
	movl $sas_info+96,-56(%ebp)
	movl $sas_info+98,-60(%ebp)
	movl $sas_info+100,-64(%ebp)
	movl $sas_info+66,-68(%ebp)
	movl $sas_info+88,-72(%ebp)
	movl $sas_info+90,-76(%ebp)
	movl $sas_mcb,-80(%ebp)
	movl $sas_info+84,-84(%ebp)
	movl $sas_info+86,-88(%ebp)
	movl $sas_info+78,-92(%ebp)
	movl $sas_info+76,-96(%ebp)
	movl $sas_vec,-100(%ebp)
	movl $sas_ctl_val,-104(%ebp)
	movl $sas_ctl_port,-108(%ebp)
	movl $sas_info_ptr,-112(%ebp)
	movl $sas_info,-116(%ebp)
	movl $0,-120(%ebp)
	.align 4
.L113:
.L_LC23:

.section	.line
	.4byte	539	/ sas.c:539
	.2byte	0xffff
	.4byte	.L_LC23-.L_text_b
	.previous
	movl -116(%ebp),%eax
	movl -112(%ebp),%ecx
	movl %eax,(%ecx)
.L_LC24:

.section	.line
	.4byte	540	/ sas.c:540
	.2byte	0xffff
	.4byte	.L_LC24-.L_text_b
	.previous
	movl -120(%ebp),%edx
	movl %eax,sas_info+148(%edx)
	movl %eax,sas_info+132(%edx)
.L_LC25:

.section	.line
	.4byte	541	/ sas.c:541
	.2byte	0xffff
	.4byte	.L_LC25-.L_text_b
	.previous
	movl $0,sas_info+152(%edx)
	movl $0,sas_info+136(%edx)
.L_LC26:

.section	.line
	.4byte	542	/ sas.c:542
	.2byte	0xffff
	.4byte	.L_LC26-.L_text_b
	.previous
	movl $0,sas_info+144(%edx)
	movl $0,sas_info+128(%edx)
.L_LC27:

.section	.line
	.4byte	543	/ sas.c:543
	.2byte	0xffff
	.4byte	.L_LC27-.L_text_b
	.previous
	movl $0,(%ebx)
.L_LC28:

.section	.line
	.4byte	544	/ sas.c:544
	.2byte	0xffff
	.4byte	.L_LC28-.L_text_b
	.previous
	movb $45,-20(%edi,%ebp)
.L_LC29:

.section	.line
	.4byte	545	/ sas.c:545
	.2byte	0xffff
	.4byte	.L_LC29-.L_text_b
	.previous
	movl -24(%ebp),%ecx
	movzwl (%ecx),%ecx
	movl %ecx,-128(%ebp)
	testl %ecx,%ecx
	je .L17
.L_LC30:

.section	.line
	.4byte	548	/ sas.c:548
	.2byte	0xffff
	.4byte	.L_LC30-.L_text_b
	.previous
	movl -100(%ebp),%eax
	cmpl $31,(%eax)
	jbe .L19
.L_LC31:

.section	.line
	.4byte	550	/ sas.c:550
	.2byte	0xffff
	.4byte	.L_LC31-.L_text_b
	.previous
	movb $62,-20(%edi,%ebp)
.L_LC32:

.section	.line
	.4byte	551	/ sas.c:551
	.2byte	0xffff
	.4byte	.L_LC32-.L_text_b
	.previous
	jmp .L17
	.align 4
.L19:
.L_LC33:

.section	.line
	.4byte	555	/ sas.c:555
	.2byte	0xffff
	.4byte	.L_LC33-.L_text_b
	.previous
	movl -108(%ebp),%edx
	cmpl $0,(%edx)
	je .L20
.L_LC34:

.section	.line
	.4byte	557	/ sas.c:557
	.2byte	0xffff
	.4byte	.L_LC34-.L_text_b
	.previous
	movl (%edx),%ecx
	movl %ecx,(%esi)
.L_LC35:

.section	.line
	.4byte	559	/ sas.c:559
	.2byte	0xffff
	.4byte	.L_LC35-.L_text_b
	.previous
	movl -104(%ebp),%eax
	testb $255,1(%eax)
	je .L21
.L_LC36:

.section	.line
	.4byte	560	/ sas.c:560
	.2byte	0xffff
	.4byte	.L_LC36-.L_text_b
	.previous
	orb $4,(%ebx)
	jmp .L20
	.align 4
.L21:
.L_LC37:

.section	.line
	.4byte	562	/ sas.c:562
	.2byte	0xffff
	.4byte	.L_LC37-.L_text_b
	.previous
	orb $2,(%ebx)
.L20:
.L_LC38:

.section	.line
	.4byte	565	/ sas.c:565
	.2byte	0xffff
	.4byte	.L_LC38-.L_text_b
	.previous
	movl -120(%ebp),%edx
	movl $0,sas_info+160(%edx)
.L_LC39:

.section	.line
	.4byte	566	/ sas.c:566
	.2byte	0xffff
	.4byte	.L_LC39-.L_text_b
	.previous
	movl $0,sas_info+164(%edx)
.L_LC40:

.section	.line
	.4byte	567	/ sas.c:567
	.2byte	0xffff
	.4byte	.L_LC40-.L_text_b
	.previous
	movl $0,sas_info+172(%edx)
.L_LC41:

.section	.line
	.4byte	568	/ sas.c:568
	.2byte	0xffff
	.4byte	.L_LC41-.L_text_b
	.previous
	movl $0,sas_info+176(%edx)
.L_LC42:

.section	.line
	.4byte	569	/ sas.c:569
	.2byte	0xffff
	.4byte	.L_LC42-.L_text_b
	.previous
	movl $0,sas_info+180(%edx)
.L_LC43:

.section	.line
	.4byte	570	/ sas.c:570
	.2byte	0xffff
	.4byte	.L_LC43-.L_text_b
	.previous
	movw -128(%ebp),%ax
	movl -52(%ebp),%ecx
	movw %ax,(%ecx)
.L_LC44:

.section	.line
	.4byte	571	/ sas.c:571
	.2byte	0xffff
	.4byte	.L_LC44-.L_text_b
	.previous
	movl (%ebx),%edx
	shrl $2,%edx
	movl %edx,-124(%ebp)
	movw -124(%ebp),%cx
	xorb $1,%cl
	andw $1,%cx
	addw -128(%ebp),%cx
	movl -96(%ebp),%eax
	movw %cx,(%eax)
.L_LC45:

.section	.line
	.4byte	572	/ sas.c:572
	.2byte	0xffff
	.4byte	.L_LC45-.L_text_b
	.previous
	movw -128(%ebp),%dx
	movw %dx,-132(%ebp)
	testb $4,(%ebx)
	jne .L23
	addw $2,-132(%ebp)
.L23:
	movw -132(%ebp),%ax
	movl -32(%ebp),%ecx
	movw %ax,(%ecx)
.L_LC46:

.section	.line
	.4byte	573	/ sas.c:573
	.2byte	0xffff
	.4byte	.L_LC46-.L_text_b
	.previous
	movw -128(%ebp),%dx
	movw %dx,-132(%ebp)
	testb $4,(%ebx)
	jne .L24
	addw $3,-132(%ebp)
.L24:
	movw -132(%ebp),%ax
	movl -84(%ebp),%ecx
	movw %ax,(%ecx)
.L_LC47:

.section	.line
	.4byte	574	/ sas.c:574
	.2byte	0xffff
	.4byte	.L_LC47-.L_text_b
	.previous
	movw -128(%ebp),%dx
	movw %dx,-132(%ebp)
	testb $4,(%ebx)
	jne .L25
	addw $4,-132(%ebp)
.L25:
	movw -132(%ebp),%ax
	movl -72(%ebp),%ecx
	movw %ax,(%ecx)
.L_LC48:

.section	.line
	.4byte	575	/ sas.c:575
	.2byte	0xffff
	.4byte	.L_LC48-.L_text_b
	.previous
	movw -128(%ebp),%dx
	movw %dx,-132(%ebp)
	testb $4,(%ebx)
	jne .L26
	addw $5,-132(%ebp)
.L26:
	movw -132(%ebp),%ax
	movl -40(%ebp),%ecx
	movw %ax,(%ecx)
.L_LC49:

.section	.line
	.4byte	576	/ sas.c:576
	.2byte	0xffff
	.4byte	.L_LC49-.L_text_b
	.previous
	movw -128(%ebp),%dx
	movw %dx,-132(%ebp)
	testb $4,(%ebx)
	jne .L27
	addw $6,-132(%ebp)
.L27:
	movw -132(%ebp),%ax
	movl -56(%ebp),%ecx
	movw %ax,(%ecx)
.L_LC50:

.section	.line
	.4byte	577	/ sas.c:577
	.2byte	0xffff
	.4byte	.L_LC50-.L_text_b
	.previous
	pushl $0
	pushl %edi
	pushl -116(%ebp)
	call sas_make_ctl_val
	movl %eax,-124(%ebp)
	movb -124(%ebp),%dl
	movl -48(%ebp),%ecx
	movb %dl,(%ecx)
.L_LC51:

.section	.line
	.4byte	578	/ sas.c:578
	.2byte	0xffff
	.4byte	.L_LC51-.L_text_b
	.previous
	pushl $1
	pushl %edi
	pushl -116(%ebp)
	call sas_make_ctl_val
	movl %eax,-124(%ebp)
	movb -124(%ebp),%al
	movl -92(%ebp),%edx
	movb %al,(%edx)
.L_LC52:

.section	.line
	.4byte	579	/ sas.c:579
	.2byte	0xffff
	.4byte	.L_LC52-.L_text_b
	.previous
	pushl $2
	pushl %edi
	pushl -116(%ebp)
	call sas_make_ctl_val
	movl %eax,-124(%ebp)
	movb -124(%ebp),%cl
	movl -36(%ebp),%eax
	movb %cl,(%eax)
.L_LC53:

.section	.line
	.4byte	580	/ sas.c:580
	.2byte	0xffff
	.4byte	.L_LC53-.L_text_b
	.previous
	addl $36,%esp
	pushl $3
	pushl %edi
	pushl -116(%ebp)
	call sas_make_ctl_val
	movl %eax,-124(%ebp)
	movb -124(%ebp),%dl
	movl -88(%ebp),%ecx
	movb %dl,(%ecx)
.L_LC54:

.section	.line
	.4byte	581	/ sas.c:581
	.2byte	0xffff
	.4byte	.L_LC54-.L_text_b
	.previous
	pushl $4
	pushl %edi
	pushl -116(%ebp)
	call sas_make_ctl_val
	movl %eax,-124(%ebp)
	movb -124(%ebp),%al
	movl -76(%ebp),%edx
	movb %al,(%edx)
.L_LC55:

.section	.line
	.4byte	582	/ sas.c:582
	.2byte	0xffff
	.4byte	.L_LC55-.L_text_b
	.previous
	pushl $5
	pushl %edi
	pushl -116(%ebp)
	call sas_make_ctl_val
	movl %eax,-124(%ebp)
	movb -124(%ebp),%cl
	movl -44(%ebp),%eax
	movb %cl,(%eax)
.L_LC56:

.section	.line
	.4byte	583	/ sas.c:583
	.2byte	0xffff
	.4byte	.L_LC56-.L_text_b
	.previous
	addl $36,%esp
	pushl $6
	pushl %edi
	pushl -116(%ebp)
	call sas_make_ctl_val
	movl %eax,-124(%ebp)
	movb -124(%ebp),%dl
	movl -60(%ebp),%ecx
	movb %dl,(%ecx)
.L_LC57:

.section	.line
	.4byte	584	/ sas.c:584
	.2byte	0xffff
	.4byte	.L_LC57-.L_text_b
	.previous
	movl -100(%ebp),%eax
	movb (%eax),%al
	movl -28(%ebp),%edx
	movb %al,(%edx)
.L_LC58:

.section	.line
	.4byte	585	/ sas.c:585
	.2byte	0xffff
	.4byte	.L_LC58-.L_text_b
	.previous
	movl -120(%ebp),%edx
	movl %edi,sas_info+120(%edx)
.L_LC59:

.section	.line
	.4byte	587	/ sas.c:587
	.2byte	0xffff
	.4byte	.L_LC59-.L_text_b
	.previous
	movb $0,sas_info+64(%edx)
.L_LC60:

.section	.line
	.4byte	588	/ sas.c:588
	.2byte	0xffff
	.4byte	.L_LC60-.L_text_b
	.previous
	testb $6,(%ebx)
	je .L29
.L_B6:
.L_LC61:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC61-.L_text_b
	.previous
.L_LC62:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC62-.L_text_b
	.previous
	movl -92(%ebp),%eax
	movb (%eax),%al
	movw (%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC63:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC63-.L_text_b
	.previous
.L_B6_e:
.L_LC64:

.section	.line
	.4byte	588	/ sas.c:588
	.2byte	0xffff
	.4byte	.L_LC64-.L_text_b
	.previous
.L29:
.L_B7:
.L_LC65:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC65-.L_text_b
	.previous
	movl -96(%ebp),%edx
	movw (%edx),%dx
	movw %dx,-132(%ebp)
	addl $12,%esp
.L_LC66:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC66-.L_text_b
	.previous
	xorb %al,%al
/APP
	outb %al,%dx
/NO_APP
.L_LC67:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC67-.L_text_b
	.previous
.L_B7_e:
.L_LC68:

.section	.line
	.4byte	591	/ sas.c:591
	.2byte	0xffff
	.4byte	.L_LC68-.L_text_b
	.previous
	testb $4,(%ebx)
	je .L34
.L_B8:
.L_LC69:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC69-.L_text_b
	.previous
.L_LC70:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC70-.L_text_b
	.previous
	movl -92(%ebp),%eax
	movb (%eax),%al
	movw (%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC71:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC71-.L_text_b
	.previous
.L_B8_e:
.L_LC72:

.section	.line
	.4byte	591	/ sas.c:591
	.2byte	0xffff
	.4byte	.L_LC72-.L_text_b
	.previous
.L34:
.L_B9:
.L_LC73:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC73-.L_text_b
	.previous
.L_LC74:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC74-.L_text_b
	.previous
.L_B10:
.L_LC75:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC75-.L_text_b
	.previous
	movw -132(%ebp),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC76:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC76-.L_text_b
	.previous
.L_B10_e:
.L_B9_e:
.L_LC77:

.section	.line
	.4byte	591	/ sas.c:591
	.2byte	0xffff
	.4byte	.L_LC77-.L_text_b
	.previous
	testb %al,%al
	je .L32
.L_LC78:

.section	.line
	.4byte	593	/ sas.c:593
	.2byte	0xffff
	.4byte	.L_LC78-.L_text_b
	.previous
	movb $63,-20(%edi,%ebp)
.L_LC79:

.section	.line
	.4byte	594	/ sas.c:594
	.2byte	0xffff
	.4byte	.L_LC79-.L_text_b
	.previous
	jmp .L17
	.align 4
.L32:
.L_LC80:

.section	.line
	.4byte	598	/ sas.c:598
	.2byte	0xffff
	.4byte	.L_LC80-.L_text_b
	.previous
	movl -24(%ebp),%ecx
	testb $2,2(%ecx)
	jne .L37
	pushl -116(%ebp)
	call sas_test_device
	addb $48,%al
	movb %al,-20(%edi,%ebp)
	addl $4,%esp
	cmpb $48,%al
	jne .L17
.L37:
.L_LC81:

.section	.line
	.4byte	606	/ sas.c:606
	.2byte	0xffff
	.4byte	.L_LC81-.L_text_b
	.previous
	movl -120(%ebp),%eax
	movb $0,sas_info+63(%eax)
.L_LC82:

.section	.line
	.4byte	607	/ sas.c:607
	.2byte	0xffff
	.4byte	.L_LC82-.L_text_b
	.previous
	testb $4,(%ebx)
	je .L39
.L_B11:
.L_LC83:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC83-.L_text_b
	.previous
.L_LC84:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC84-.L_text_b
	.previous
	movl -88(%ebp),%eax
	movb (%eax),%al
	movw (%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC85:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC85-.L_text_b
	.previous
.L_B11_e:
.L_LC86:

.section	.line
	.4byte	607	/ sas.c:607
	.2byte	0xffff
	.4byte	.L_LC86-.L_text_b
	.previous
.L39:
.L_B12:
.L_LC87:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC87-.L_text_b
	.previous
.L_LC88:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC88-.L_text_b
	.previous
	xorb %al,%al
	movl -84(%ebp),%edx
	movw (%edx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC89:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC89-.L_text_b
	.previous
.L_B12_e:
.L_LC90:

.section	.line
	.4byte	608	/ sas.c:608
	.2byte	0xffff
	.4byte	.L_LC90-.L_text_b
	.previous
	movl -80(%ebp),%ecx
	movb (%ecx),%cl
	movl -120(%ebp),%eax
	orb sas_info+52(%eax),%cl
	movb %cl,-132(%ebp)
	movb %cl,sas_info+62(%eax)
.L_LC91:

.section	.line
	.4byte	609	/ sas.c:609
	.2byte	0xffff
	.4byte	.L_LC91-.L_text_b
	.previous
	testb $4,(%ebx)
	je .L43
.L_B13:
.L_LC92:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC92-.L_text_b
	.previous
.L_LC93:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC93-.L_text_b
	.previous
	movl -76(%ebp),%eax
	movb (%eax),%al
	movw (%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC94:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC94-.L_text_b
	.previous
.L_B13_e:
.L_LC95:

.section	.line
	.4byte	609	/ sas.c:609
	.2byte	0xffff
	.4byte	.L_LC95-.L_text_b
	.previous
.L43:
.L_B14:
.L_LC96:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC96-.L_text_b
	.previous
.L_LC97:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC97-.L_text_b
	.previous
	movb -132(%ebp),%al
	movl -72(%ebp),%edx
	movw (%edx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC98:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC98-.L_text_b
	.previous
.L_B14_e:
.L_LC99:

.section	.line
	.4byte	611	/ sas.c:611
	.2byte	0xffff
	.4byte	.L_LC99-.L_text_b
	.previous
	movl -68(%ebp),%ecx
	movb $0,(%ecx)
.L_LC100:

.section	.line
	.4byte	612	/ sas.c:612
	.2byte	0xffff
	.4byte	.L_LC100-.L_text_b
	.previous
	movl -64(%ebp),%eax
	movl $1,(%eax)
.L_LC101:

.section	.line
	.4byte	613	/ sas.c:613
	.2byte	0xffff
	.4byte	.L_LC101-.L_text_b
	.previous
	movb $42,-20(%edi,%ebp)
.L_LC102:

.section	.line
	.4byte	616	/ sas.c:616
	.2byte	0xffff
	.4byte	.L_LC102-.L_text_b
	.previous
	movb (%ebx),%dl
	andb $4,%dl
	movb %dl,-132(%ebp)
	je .L47
.L_B15:
.L_LC103:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC103-.L_text_b
	.previous
.L_LC104:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC104-.L_text_b
	.previous
	movl -36(%ebp),%eax
	movb (%eax),%al
	movw (%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC105:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC105-.L_text_b
	.previous
.L_B15_e:
.L_LC106:

.section	.line
	.4byte	616	/ sas.c:616
	.2byte	0xffff
	.4byte	.L_LC106-.L_text_b
	.previous
.L47:
.L_B16:
.L_LC107:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC107-.L_text_b
	.previous
	movl -32(%ebp),%edx
	movw (%edx),%dx
	movw %dx,-128(%ebp)
.L_LC108:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC108-.L_text_b
	.previous
	movb $-121,%al
/APP
	outb %al,%dx
/NO_APP
.L_LC109:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC109-.L_text_b
	.previous
.L_B16_e:
.L_LC110:

.section	.line
	.4byte	617	/ sas.c:617
	.2byte	0xffff
	.4byte	.L_LC110-.L_text_b
	.previous
	cmpb $0,-132(%ebp)
	je .L52
.L_B17:
.L_LC111:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC111-.L_text_b
	.previous
.L_LC112:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC112-.L_text_b
	.previous
	movl -36(%ebp),%eax
	movb (%eax),%al
	movw (%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC113:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC113-.L_text_b
	.previous
.L_B17_e:
.L_LC114:

.section	.line
	.4byte	617	/ sas.c:617
	.2byte	0xffff
	.4byte	.L_LC114-.L_text_b
	.previous
.L52:
.L_B18:
.L_LC115:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC115-.L_text_b
	.previous
.L_LC116:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC116-.L_text_b
	.previous
.L_B19:
.L_LC117:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC117-.L_text_b
	.previous
	movw -128(%ebp),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC118:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC118-.L_text_b
	.previous
.L_B19_e:
.L_B18_e:
.L_LC119:

.section	.line
	.4byte	617	/ sas.c:617
	.2byte	0xffff
	.4byte	.L_LC119-.L_text_b
	.previous
	notb %al
	testb $192,%al
	jne .L50
.L_LC120:

.section	.line
	.4byte	619	/ sas.c:619
	.2byte	0xffff
	.4byte	.L_LC120-.L_text_b
	.previous
	movl -68(%ebp),%ecx
	movb $2,(%ecx)
.L_LC121:

.section	.line
	.4byte	620	/ sas.c:620
	.2byte	0xffff
	.4byte	.L_LC121-.L_text_b
	.previous
	movl -64(%ebp),%eax
	movl $16,(%eax)
.L_LC122:

.section	.line
	.4byte	621	/ sas.c:621
	.2byte	0xffff
	.4byte	.L_LC122-.L_text_b
	.previous
	movb $70,-20(%edi,%ebp)
.L_LC123:

.section	.line
	.4byte	622	/ sas.c:622
	.2byte	0xffff
	.4byte	.L_LC123-.L_text_b
	.previous
.L_B20:
.L_LC124:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC124-.L_text_b
	.previous
.L_LC125:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC125-.L_text_b
	.previous
.L_B20_e:
.L_LC126:

.section	.line
	.4byte	622	/ sas.c:622
	.2byte	0xffff
	.4byte	.L_LC126-.L_text_b
	.previous
.L_B21:
.L_LC127:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC127-.L_text_b
	.previous
.L_LC128:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC128-.L_text_b
	.previous
.L_B21_e:
.L_LC129:

.section	.line
	.4byte	623	/ sas.c:623
	.2byte	0xffff
	.4byte	.L_LC129-.L_text_b
	.previous
	jmp .L68
	.align 4
.L50:
.L_LC130:

.section	.line
	.4byte	626	/ sas.c:626
	.2byte	0xffff
	.4byte	.L_LC130-.L_text_b
	.previous
	cmpb $0,-132(%ebp)
	je .L61
.L_B22:
.L_LC131:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC131-.L_text_b
	.previous
.L_LC132:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC132-.L_text_b
	.previous
	movl -36(%ebp),%eax
	movb (%eax),%al
	movw (%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC133:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC133-.L_text_b
	.previous
.L_B22_e:
.L_LC134:

.section	.line
	.4byte	626	/ sas.c:626
	.2byte	0xffff
	.4byte	.L_LC134-.L_text_b
	.previous
.L61:
.L_B23:
.L_LC135:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC135-.L_text_b
	.previous
.L_LC136:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC136-.L_text_b
	.previous
	xorb %al,%al
	movw -128(%ebp),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC137:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC137-.L_text_b
	.previous
.L_B23_e:
.L_LC138:

.section	.line
	.4byte	628	/ sas.c:628
	.2byte	0xffff
	.4byte	.L_LC138-.L_text_b
	.previous
	cmpb $0,-132(%ebp)
	je .L65
.L_B24:
.L_LC139:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC139-.L_text_b
	.previous
.L_LC140:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC140-.L_text_b
	.previous
	movl -36(%ebp),%eax
	movb (%eax),%al
	movw (%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC141:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC141-.L_text_b
	.previous
.L_B24_e:
.L_LC142:

.section	.line
	.4byte	628	/ sas.c:628
	.2byte	0xffff
	.4byte	.L_LC142-.L_text_b
	.previous
.L65:
.L_B25:
.L_LC143:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC143-.L_text_b
	.previous
.L_LC144:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC144-.L_text_b
	.previous
	movb $64,%al
	movw -128(%ebp),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC145:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC145-.L_text_b
	.previous
.L_B25_e:
.L_LC146:

.section	.line
	.4byte	629	/ sas.c:629
	.2byte	0xffff
	.4byte	.L_LC146-.L_text_b
	.previous
	cmpb $0,-132(%ebp)
	je .L70
.L_B26:
.L_LC147:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC147-.L_text_b
	.previous
.L_LC148:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC148-.L_text_b
	.previous
	movl -36(%ebp),%eax
	movb (%eax),%al
	movw (%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC149:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC149-.L_text_b
	.previous
.L_B26_e:
.L_LC150:

.section	.line
	.4byte	629	/ sas.c:629
	.2byte	0xffff
	.4byte	.L_LC150-.L_text_b
	.previous
.L70:
.L_B27:
.L_LC151:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC151-.L_text_b
	.previous
.L_LC152:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC152-.L_text_b
	.previous
.L_B28:
.L_LC153:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC153-.L_text_b
	.previous
	movw -128(%ebp),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC154:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC154-.L_text_b
	.previous
.L_B28_e:
.L_B27_e:
.L_LC155:

.section	.line
	.4byte	629	/ sas.c:629
	.2byte	0xffff
	.4byte	.L_LC155-.L_text_b
	.previous
	notb %al
	testb $64,%al
	jne .L68
.L_LC156:

.section	.line
	.4byte	631	/ sas.c:631
	.2byte	0xffff
	.4byte	.L_LC156-.L_text_b
	.previous
	movl -68(%ebp),%ecx
	movb $1,(%ecx)
.L_LC157:

.section	.line
	.4byte	632	/ sas.c:632
	.2byte	0xffff
	.4byte	.L_LC157-.L_text_b
	.previous
	movl -64(%ebp),%eax
	movl $4,(%eax)
.L_LC158:

.section	.line
	.4byte	633	/ sas.c:633
	.2byte	0xffff
	.4byte	.L_LC158-.L_text_b
	.previous
	movb $102,-20(%edi,%ebp)
.L_LC159:

.section	.line
	.4byte	634	/ sas.c:634
	.2byte	0xffff
	.4byte	.L_LC159-.L_text_b
	.previous
	movb (%ebx),%dl
	andb $4,%dl
	movb %dl,-132(%ebp)
	je .L74
.L_B29:
.L_LC160:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC160-.L_text_b
	.previous
.L_LC161:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC161-.L_text_b
	.previous
	movl -36(%ebp),%eax
	movb (%eax),%al
	movw (%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC162:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC162-.L_text_b
	.previous
.L_B29_e:
.L_LC163:

.section	.line
	.4byte	634	/ sas.c:634
	.2byte	0xffff
	.4byte	.L_LC163-.L_text_b
	.previous
.L74:
.L_B30:
.L_LC164:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC164-.L_text_b
	.previous
.L_LC165:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC165-.L_text_b
	.previous
	movb $32,%al
	movl -32(%ebp),%edx
	movw (%edx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC166:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC166-.L_text_b
	.previous
.L_B30_e:
.L_LC167:

.section	.line
	.4byte	635	/ sas.c:635
	.2byte	0xffff
	.4byte	.L_LC167-.L_text_b
	.previous
	cmpb $0,-132(%ebp)
	je .L78
.L_B31:
.L_LC168:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC168-.L_text_b
	.previous
.L_LC169:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC169-.L_text_b
	.previous
	movl -60(%ebp),%eax
	movb (%eax),%al
	movw (%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC170:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC170-.L_text_b
	.previous
.L_B31_e:
.L_LC171:

.section	.line
	.4byte	635	/ sas.c:635
	.2byte	0xffff
	.4byte	.L_LC171-.L_text_b
	.previous
.L78:
.L_B32:
.L_LC172:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC172-.L_text_b
	.previous
.L_LC173:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC173-.L_text_b
	.previous
	movb $12,%al
	movl -56(%ebp),%edx
	movw (%edx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC174:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC174-.L_text_b
	.previous
.L_B32_e:
.L_LC175:

.section	.line
	.4byte	636	/ sas.c:636
	.2byte	0xffff
	.4byte	.L_LC175-.L_text_b
	.previous
	cmpb $0,-132(%ebp)
	je .L82
.L_B33:
.L_LC176:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC176-.L_text_b
	.previous
.L_LC177:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC177-.L_text_b
	.previous
	movl -44(%ebp),%eax
	movb (%eax),%al
	movw (%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC178:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC178-.L_text_b
	.previous
.L_B33_e:
.L_LC179:

.section	.line
	.4byte	636	/ sas.c:636
	.2byte	0xffff
	.4byte	.L_LC179-.L_text_b
	.previous
.L82:
.L_B34:
.L_LC180:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC180-.L_text_b
	.previous
.L_LC181:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC181-.L_text_b
	.previous
	movb $48,%al
	movl -40(%ebp),%edx
	movw (%edx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC182:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC182-.L_text_b
	.previous
.L_B34_e:
.L68:
.L_LC183:

.section	.line
	.4byte	638	/ sas.c:638
	.2byte	0xffff
	.4byte	.L_LC183-.L_text_b
	.previous
	testb $4,(%ebx)
	je .L86
.L_B35:
.L_LC184:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC184-.L_text_b
	.previous
.L_LC185:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC185-.L_text_b
	.previous
	movl -36(%ebp),%eax
	movb (%eax),%al
	movw (%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC186:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC186-.L_text_b
	.previous
.L_B35_e:
.L_LC187:

.section	.line
	.4byte	638	/ sas.c:638
	.2byte	0xffff
	.4byte	.L_LC187-.L_text_b
	.previous
.L86:
.L_B36:
.L_LC188:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC188-.L_text_b
	.previous
.L_LC189:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC189-.L_text_b
	.previous
	xorb %al,%al
	movl -32(%ebp),%edx
	movw (%edx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC190:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC190-.L_text_b
	.previous
.L_B36_e:
.L_LC191:

.section	.line
	.4byte	642	/ sas.c:642
	.2byte	0xffff
	.4byte	.L_LC191-.L_text_b
	.previous
	movl -24(%ebp),%ecx
	testb $1,2(%ecx)
	je .L89
	movl -68(%ebp),%eax
	cmpb $0,(%eax)
	je .L89
.L_LC192:

.section	.line
	.4byte	645	/ sas.c:645
	.2byte	0xffff
	.4byte	.L_LC192-.L_text_b
	.previous
	movb $0,(%eax)
.L_LC193:

.section	.line
	.4byte	646	/ sas.c:646
	.2byte	0xffff
	.4byte	.L_LC193-.L_text_b
	.previous
	movl -64(%ebp),%edx
	movl $1,(%edx)
.L_LC194:

.section	.line
	.4byte	647	/ sas.c:647
	.2byte	0xffff
	.4byte	.L_LC194-.L_text_b
	.previous
	movb $43,-20(%edi,%ebp)
.L89:
.L_LC195:

.section	.line
	.4byte	651	/ sas.c:651
	.2byte	0xffff
	.4byte	.L_LC195-.L_text_b
	.previous
	movb (%ebx),%cl
	andb $4,%cl
	movb %cl,-132(%ebp)
	je .L91
.L_B37:
.L_LC196:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC196-.L_text_b
	.previous
.L_LC197:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC197-.L_text_b
	.previous
	movl -60(%ebp),%eax
	movb (%eax),%al
	movw (%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC198:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC198-.L_text_b
	.previous
.L_B37_e:
.L_LC199:

.section	.line
	.4byte	651	/ sas.c:651
	.2byte	0xffff
	.4byte	.L_LC199-.L_text_b
	.previous
.L91:
.L_B38:
.L_LC200:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC200-.L_text_b
	.previous
.L_LC201:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC201-.L_text_b
	.previous
.L_B39:
.L_LC202:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC202-.L_text_b
	.previous
	movl -56(%ebp),%eax
	movw (%eax),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC203:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC203-.L_text_b
	.previous
.L_B39_e:
.L_B38_e:
.L_LC204:

.section	.line
	.4byte	652	/ sas.c:652
	.2byte	0xffff
	.4byte	.L_LC204-.L_text_b
	.previous
	cmpb $0,-132(%ebp)
	je .L95
.L_B40:
.L_LC205:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC205-.L_text_b
	.previous
.L_LC206:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC206-.L_text_b
	.previous
	movl -48(%ebp),%eax
	movb (%eax),%al
	movw (%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC207:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC207-.L_text_b
	.previous
.L_B40_e:
.L_LC208:

.section	.line
	.4byte	652	/ sas.c:652
	.2byte	0xffff
	.4byte	.L_LC208-.L_text_b
	.previous
.L95:
.L_B41:
.L_LC209:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC209-.L_text_b
	.previous
	movl -52(%ebp),%edx
	movw (%edx),%dx
	movw %dx,-128(%ebp)
.L_LC210:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC210-.L_text_b
	.previous
.L_B42:
.L_LC211:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC211-.L_text_b
	.previous
/APP
	inb %dx,%eax
/NO_APP
.L_LC212:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC212-.L_text_b
	.previous
.L_B42_e:
.L_B41_e:
.L_LC213:

.section	.line
	.4byte	653	/ sas.c:653
	.2byte	0xffff
	.4byte	.L_LC213-.L_text_b
	.previous
	cmpb $0,-132(%ebp)
	je .L99
.L_B43:
.L_LC214:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC214-.L_text_b
	.previous
.L_LC215:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC215-.L_text_b
	.previous
	movl -48(%ebp),%eax
	movb (%eax),%al
	movw (%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC216:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC216-.L_text_b
	.previous
.L_B43_e:
.L_LC217:

.section	.line
	.4byte	653	/ sas.c:653
	.2byte	0xffff
	.4byte	.L_LC217-.L_text_b
	.previous
.L99:
.L_B44:
.L_LC218:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC218-.L_text_b
	.previous
.L_LC219:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC219-.L_text_b
	.previous
.L_B45:
.L_LC220:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC220-.L_text_b
	.previous
	movw -128(%ebp),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC221:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC221-.L_text_b
	.previous
.L_B45_e:
.L_B44_e:
.L_LC222:

.section	.line
	.4byte	654	/ sas.c:654
	.2byte	0xffff
	.4byte	.L_LC222-.L_text_b
	.previous
	cmpb $0,-132(%ebp)
	je .L103
.L_B46:
.L_LC223:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC223-.L_text_b
	.previous
.L_LC224:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC224-.L_text_b
	.previous
	movl -44(%ebp),%eax
	movb (%eax),%al
	movw (%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC225:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC225-.L_text_b
	.previous
.L_B46_e:
.L_LC226:

.section	.line
	.4byte	654	/ sas.c:654
	.2byte	0xffff
	.4byte	.L_LC226-.L_text_b
	.previous
.L103:
.L_B47:
.L_LC227:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC227-.L_text_b
	.previous
.L_LC228:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC228-.L_text_b
	.previous
.L_B48:
.L_LC229:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC229-.L_text_b
	.previous
	movl -40(%ebp),%eax
	movw (%eax),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC230:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC230-.L_text_b
	.previous
.L_B48_e:
.L_B47_e:
.L_LC231:

.section	.line
	.4byte	655	/ sas.c:655
	.2byte	0xffff
	.4byte	.L_LC231-.L_text_b
	.previous
	cmpb $0,-132(%ebp)
	je .L107
.L_B49:
.L_LC232:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC232-.L_text_b
	.previous
.L_LC233:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC233-.L_text_b
	.previous
	movl -36(%ebp),%eax
	movb (%eax),%al
	movw (%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC234:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC234-.L_text_b
	.previous
.L_B49_e:
.L_LC235:

.section	.line
	.4byte	655	/ sas.c:655
	.2byte	0xffff
	.4byte	.L_LC235-.L_text_b
	.previous
.L107:
.L_B50:
.L_LC236:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC236-.L_text_b
	.previous
.L_LC237:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC237-.L_text_b
	.previous
.L_B51:
.L_LC238:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC238-.L_text_b
	.previous
	movl -32(%ebp),%eax
	movw (%eax),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC239:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC239-.L_text_b
	.previous
.L_B51_e:
.L_B50_e:
.L_LC240:

.section	.line
	.4byte	656	/ sas.c:656
	.2byte	0xffff
	.4byte	.L_LC240-.L_text_b
	.previous
	movl -28(%ebp),%edx
	movzbl (%edx),%edx
	movl %edx,-136(%ebp)
	movl sas_int_ack_port(,%edx,4),%ecx
	movl %ecx,-128(%ebp)
	testl %ecx,%ecx
	je .L110
.L_LC241:

.section	.line
	.4byte	657	/ sas.c:657
	.2byte	0xffff
	.4byte	.L_LC241-.L_text_b
	.previous
.L_B52:
.L_LC242:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC242-.L_text_b
	.previous
.L_LC243:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC243-.L_text_b
	.previous
	movb sas_int_ack(,%edx,4),%al
	movw -128(%ebp),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC244:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC244-.L_text_b
	.previous
.L_B52_e:
.L110:
.L_LC245:

.section	.line
	.4byte	660	/ sas.c:660
	.2byte	0xffff
	.4byte	.L_LC245-.L_text_b
	.previous
	movl -24(%ebp),%ecx
	testb $4,2(%ecx)
	je .L112
.L_LC246:

.section	.line
	.4byte	661	/ sas.c:661
	.2byte	0xffff
	.4byte	.L_LC246-.L_text_b
	.previous
	orw $16384,(%ebx)
.L_LC247:

.section	.line
	.4byte	662	/ sas.c:662
	.2byte	0xffff
	.4byte	.L_LC247-.L_text_b
	.previous
	movl $1,sas_speeds+40
.L_LC248:

.section	.line
	.4byte	663	/ sas.c:663
	.2byte	0xffff
	.4byte	.L_LC248-.L_text_b
	.previous
	movl $1,sas_ctimes+40
.L_LC249:

.section	.line
	.4byte	664	/ sas.c:664
	.2byte	0xffff
	.4byte	.L_LC249-.L_text_b
	.previous
	movl $512,sas_bsizes+40
.L112:
.L_LC250:

.section	.line
	.4byte	668	/ sas.c:668
	.2byte	0xffff
	.4byte	.L_LC250-.L_text_b
	.previous
	orb $1,(%ebx)
.L_LC251:

.section	.line
	.4byte	537	/ sas.c:537
	.2byte	0xffff
	.4byte	.L_LC251-.L_text_b
	.previous
.L17:
	addl $204,%ebx
	addl $4,-24(%ebp)
	addl $204,-28(%ebp)
	addl $204,-32(%ebp)
	addl $204,-36(%ebp)
	addl $204,%esi
	addl $204,-40(%ebp)
	addl $204,-44(%ebp)
	addl $204,-48(%ebp)
	addl $204,-52(%ebp)
	addl $204,-56(%ebp)
	addl $204,-60(%ebp)
	addl $204,-64(%ebp)
	addl $204,-68(%ebp)
	addl $204,-72(%ebp)
	addl $204,-76(%ebp)
	addl $4,-80(%ebp)
	addl $204,-84(%ebp)
	addl $204,-88(%ebp)
	addl $204,-92(%ebp)
	addl $204,-96(%ebp)
	addl $4,-100(%ebp)
	addl $4,-104(%ebp)
	addl $4,-108(%ebp)
	addl $4,-112(%ebp)
	addl $204,-116(%ebp)
	addl $204,-120(%ebp)
	incl %edi
	cmpl %edi,sas_physical_units
	ja .L113
.L16:
.L_LC252:

.section	.line
	.4byte	692	/ sas.c:692
	.2byte	0xffff
	.4byte	.L_LC252-.L_text_b
	.previous
	movb $0,-20(%edi,%ebp)
.L_LC253:

.section	.line
	.4byte	693	/ sas.c:693
	.2byte	0xffff
	.4byte	.L_LC253-.L_text_b
	.previous
	leal -20(%ebp),%eax
	pushl %eax
	leal -1(%edi),%eax
	pushl %eax
	pushl $.LC2
	call printf
.L_LC254:

.section	.line
	.4byte	698	/ sas.c:698
	.2byte	0xffff
	.4byte	.L_LC254-.L_text_b
	.previous
.L5:
.L_b5_e:
	leal -148(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
.L_f5_e:
.Lfe2:
	.size	 sasinit,.Lfe2-sasinit

.section	.debug_pubnames
	.4byte	.L_P2
	.string	"sasinit"
	.previous

.section	.debug
.L_P2:
.L_D14:
	.4byte	.L_D14_e-.L_D14
	.2byte	0x6
	.2byte	0x12
	.4byte	.L_D16
	.2byte	0x38
	.string	"sasinit"
	.2byte	0x111
	.4byte	sasinit
	.2byte	0x121
	.4byte	.L_f5_e
	.2byte	0x8041
	.4byte	.L_b5
	.2byte	0x8051
	.4byte	.L_b5_e
.L_D14_e:
.L_D17:
	.4byte	.L_D17_e-.L_D17
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D18
	.2byte	0x38
	.string	"regvar"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l17_e-.L_l17
.L_l17:
.L_l17_e:
.L_D17_e:
.L_D18:
	.4byte	.L_D18_e-.L_D18
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D19
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t18_e-.L_t18
.L_t18:
	.byte	0x1
	.4byte	.L_T816
.L_t18_e:
	.2byte	0x23
	.2byte	.L_l18_e-.L_l18
.L_l18:
.L_l18_e:
.L_D18_e:
.L_D19:
	.4byte	.L_D19_e-.L_D19
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D20
	.2byte	0x38
	.string	"unit"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l19_e-.L_l19
.L_l19:
	.byte	0x1
	.4byte	0x7
.L_l19_e:
.L_D19_e:
.L_D20:
	.4byte	.L_D20_e-.L_D20
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D21
	.2byte	0x38
	.string	"logical_units"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l20_e-.L_l20
.L_l20:
.L_l20_e:
.L_D20_e:
.L_D21:
	.4byte	.L_D21_e-.L_D21
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D22
	.2byte	0x38
	.string	"port"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l21_e-.L_l21
.L_l21:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffff80
	.byte	0x7
.L_l21_e:
.L_D21_e:
.L_D22:
	.4byte	.L_D22_e-.L_D22
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D23
	.2byte	0x38
	.string	"seq_ptr"
	.2byte	0x63
	.2byte	.L_t22_e-.L_t22
.L_t22:
	.byte	0x1
	.2byte	0x9
.L_t22_e:
	.2byte	0x23
	.2byte	.L_l22_e-.L_l22
.L_l22:
	.byte	0x1
	.4byte	0x3
.L_l22_e:
.L_D22_e:
.L_D23:
	.4byte	.L_D23_e-.L_D23
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D24
	.set	.L_T925,.L_D23
	.2byte	0xa3
	.2byte	.L_s23_e-.L_s23
.L_s23:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x10
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s23_e:
.L_D23_e:
.L_D24:
	.4byte	.L_D24_e-.L_D24
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D25
	.2byte	0x38
	.string	"port_stat"
	.2byte	0x72
	.4byte	.L_T925
	.2byte	0x23
	.2byte	.L_l24_e-.L_l24
.L_l24:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffffec
	.byte	0x7
.L_l24_e:
.L_D24_e:
.L_D25:
	.4byte	.L_D25_e-.L_D25
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D26
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B3
	.2byte	0x121
	.4byte	.L_B3_e
.L_D25_e:
.L_D27:
	.4byte	.L_D27_e-.L_D27
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D28
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l27_e-.L_l27
.L_l27:
.L_l27_e:
.L_D27_e:
.L_D28:
	.4byte	.L_D28_e-.L_D28
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D29
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l28_e-.L_l28
.L_l28:
	.byte	0x1
	.4byte	0x6
.L_l28_e:
.L_D28_e:
.L_D29:
	.4byte	0x4
.L_D26:
	.4byte	.L_D26_e-.L_D26
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D30
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B5
	.2byte	0x121
	.4byte	.L_B5_e
.L_D26_e:
.L_D31:
	.4byte	.L_D31_e-.L_D31
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D32
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l31_e-.L_l31
.L_l31:
.L_l31_e:
.L_D31_e:
.L_D32:
	.4byte	.L_D32_e-.L_D32
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D33
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l32_e-.L_l32
.L_l32:
.L_l32_e:
.L_D32_e:
.L_D33:
	.4byte	0x4
.L_D30:
	.4byte	.L_D30_e-.L_D30
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D34
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B6
	.2byte	0x121
	.4byte	.L_B6_e
.L_D30_e:
.L_D35:
	.4byte	.L_D35_e-.L_D35
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D36
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l35_e-.L_l35
.L_l35:
.L_l35_e:
.L_D35_e:
.L_D36:
	.4byte	.L_D36_e-.L_D36
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D37
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l36_e-.L_l36
.L_l36:
.L_l36_e:
.L_D36_e:
.L_D37:
	.4byte	0x4
.L_D34:
	.4byte	.L_D34_e-.L_D34
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D38
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B7
	.2byte	0x121
	.4byte	.L_B7_e
.L_D34_e:
.L_D39:
	.4byte	.L_D39_e-.L_D39
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D40
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l39_e-.L_l39
.L_l39:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffff7c
	.byte	0x7
.L_l39_e:
.L_D39_e:
.L_D40:
	.4byte	.L_D40_e-.L_D40
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D41
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l40_e-.L_l40
.L_l40:
.L_l40_e:
.L_D40_e:
.L_D41:
	.4byte	0x4
.L_D38:
	.4byte	.L_D38_e-.L_D38
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D42
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B8
	.2byte	0x121
	.4byte	.L_B8_e
.L_D38_e:
.L_D43:
	.4byte	.L_D43_e-.L_D43
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D44
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l43_e-.L_l43
.L_l43:
.L_l43_e:
.L_D43_e:
.L_D44:
	.4byte	.L_D44_e-.L_D44
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D45
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l44_e-.L_l44
.L_l44:
.L_l44_e:
.L_D44_e:
.L_D45:
	.4byte	0x4
.L_D42:
	.4byte	.L_D42_e-.L_D42
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D46
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B9
	.2byte	0x121
	.4byte	.L_B9_e
.L_D42_e:
.L_D47:
	.4byte	.L_D47_e-.L_D47
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D48
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l47_e-.L_l47
.L_l47:
.L_l47_e:
.L_D47_e:
.L_D48:
	.4byte	.L_D48_e-.L_D48
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D49
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l48_e-.L_l48
.L_l48:
	.byte	0x1
	.4byte	0x0
.L_l48_e:
.L_D48_e:
.L_D49:
	.4byte	0x4
.L_D46:
	.4byte	.L_D46_e-.L_D46
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D50
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B11
	.2byte	0x121
	.4byte	.L_B11_e
.L_D46_e:
.L_D51:
	.4byte	.L_D51_e-.L_D51
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D52
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l51_e-.L_l51
.L_l51:
.L_l51_e:
.L_D51_e:
.L_D52:
	.4byte	.L_D52_e-.L_D52
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D53
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l52_e-.L_l52
.L_l52:
.L_l52_e:
.L_D52_e:
.L_D53:
	.4byte	0x4
.L_D50:
	.4byte	.L_D50_e-.L_D50
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D54
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B12
	.2byte	0x121
	.4byte	.L_B12_e
.L_D50_e:
.L_D55:
	.4byte	.L_D55_e-.L_D55
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D56
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l55_e-.L_l55
.L_l55:
.L_l55_e:
.L_D55_e:
.L_D56:
	.4byte	.L_D56_e-.L_D56
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D57
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l56_e-.L_l56
.L_l56:
.L_l56_e:
.L_D56_e:
.L_D57:
	.4byte	0x4
.L_D54:
	.4byte	.L_D54_e-.L_D54
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D58
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B13
	.2byte	0x121
	.4byte	.L_B13_e
.L_D54_e:
.L_D59:
	.4byte	.L_D59_e-.L_D59
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D60
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l59_e-.L_l59
.L_l59:
.L_l59_e:
.L_D59_e:
.L_D60:
	.4byte	.L_D60_e-.L_D60
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D61
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l60_e-.L_l60
.L_l60:
.L_l60_e:
.L_D60_e:
.L_D61:
	.4byte	0x4
.L_D58:
	.4byte	.L_D58_e-.L_D58
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D62
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B14
	.2byte	0x121
	.4byte	.L_B14_e
.L_D58_e:
.L_D63:
	.4byte	.L_D63_e-.L_D63
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D64
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l63_e-.L_l63
.L_l63:
.L_l63_e:
.L_D63_e:
.L_D64:
	.4byte	.L_D64_e-.L_D64
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D65
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l64_e-.L_l64
.L_l64:
.L_l64_e:
.L_D64_e:
.L_D65:
	.4byte	0x4
.L_D62:
	.4byte	.L_D62_e-.L_D62
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D66
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B15
	.2byte	0x121
	.4byte	.L_B15_e
.L_D62_e:
.L_D67:
	.4byte	.L_D67_e-.L_D67
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D68
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l67_e-.L_l67
.L_l67:
.L_l67_e:
.L_D67_e:
.L_D68:
	.4byte	.L_D68_e-.L_D68
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D69
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l68_e-.L_l68
.L_l68:
.L_l68_e:
.L_D68_e:
.L_D69:
	.4byte	0x4
.L_D66:
	.4byte	.L_D66_e-.L_D66
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D70
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B16
	.2byte	0x121
	.4byte	.L_B16_e
.L_D66_e:
.L_D71:
	.4byte	.L_D71_e-.L_D71
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D72
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l71_e-.L_l71
.L_l71:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffff80
	.byte	0x7
.L_l71_e:
.L_D71_e:
.L_D72:
	.4byte	.L_D72_e-.L_D72
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D73
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l72_e-.L_l72
.L_l72:
	.4byte	0xffffff87
.L_l72_e:
.L_D72_e:
.L_D73:
	.4byte	0x4
.L_D70:
	.4byte	.L_D70_e-.L_D70
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D74
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B17
	.2byte	0x121
	.4byte	.L_B17_e
.L_D70_e:
.L_D75:
	.4byte	.L_D75_e-.L_D75
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D76
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l75_e-.L_l75
.L_l75:
.L_l75_e:
.L_D75_e:
.L_D76:
	.4byte	.L_D76_e-.L_D76
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D77
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l76_e-.L_l76
.L_l76:
.L_l76_e:
.L_D76_e:
.L_D77:
	.4byte	0x4
.L_D74:
	.4byte	.L_D74_e-.L_D74
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D78
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B18
	.2byte	0x121
	.4byte	.L_B18_e
.L_D74_e:
.L_D79:
	.4byte	.L_D79_e-.L_D79
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D80
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l79_e-.L_l79
.L_l79:
.L_l79_e:
.L_D79_e:
.L_D80:
	.4byte	.L_D80_e-.L_D80
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D81
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l80_e-.L_l80
.L_l80:
	.byte	0x1
	.4byte	0x0
.L_l80_e:
.L_D80_e:
.L_D81:
	.4byte	0x4
.L_D78:
	.4byte	.L_D78_e-.L_D78
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D82
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B20
	.2byte	0x121
	.4byte	.L_B20_e
.L_D78_e:
.L_D83:
	.4byte	.L_D83_e-.L_D83
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D84
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l83_e-.L_l83
.L_l83:
.L_l83_e:
.L_D83_e:
.L_D84:
	.4byte	.L_D84_e-.L_D84
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D85
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l84_e-.L_l84
.L_l84:
.L_l84_e:
.L_D84_e:
.L_D85:
	.4byte	0x4
.L_D82:
	.4byte	.L_D82_e-.L_D82
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D86
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B21
	.2byte	0x121
	.4byte	.L_B21_e
.L_D82_e:
.L_D87:
	.4byte	.L_D87_e-.L_D87
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D88
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l87_e-.L_l87
.L_l87:
.L_l87_e:
.L_D87_e:
.L_D88:
	.4byte	.L_D88_e-.L_D88
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D89
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l88_e-.L_l88
.L_l88:
	.4byte	0x0
.L_l88_e:
.L_D88_e:
.L_D89:
	.4byte	0x4
.L_D86:
	.4byte	.L_D86_e-.L_D86
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D90
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B22
	.2byte	0x121
	.4byte	.L_B22_e
.L_D86_e:
.L_D91:
	.4byte	.L_D91_e-.L_D91
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D92
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l91_e-.L_l91
.L_l91:
.L_l91_e:
.L_D91_e:
.L_D92:
	.4byte	.L_D92_e-.L_D92
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D93
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l92_e-.L_l92
.L_l92:
.L_l92_e:
.L_D92_e:
.L_D93:
	.4byte	0x4
.L_D90:
	.4byte	.L_D90_e-.L_D90
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D94
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B23
	.2byte	0x121
	.4byte	.L_B23_e
.L_D90_e:
.L_D95:
	.4byte	.L_D95_e-.L_D95
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D96
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l95_e-.L_l95
.L_l95:
.L_l95_e:
.L_D95_e:
.L_D96:
	.4byte	.L_D96_e-.L_D96
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D97
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l96_e-.L_l96
.L_l96:
	.4byte	0x0
.L_l96_e:
.L_D96_e:
.L_D97:
	.4byte	0x4
.L_D94:
	.4byte	.L_D94_e-.L_D94
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D98
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B24
	.2byte	0x121
	.4byte	.L_B24_e
.L_D94_e:
.L_D99:
	.4byte	.L_D99_e-.L_D99
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D100
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l99_e-.L_l99
.L_l99:
.L_l99_e:
.L_D99_e:
.L_D100:
	.4byte	.L_D100_e-.L_D100
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D101
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l100_e-.L_l100
.L_l100:
.L_l100_e:
.L_D100_e:
.L_D101:
	.4byte	0x4
.L_D98:
	.4byte	.L_D98_e-.L_D98
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D102
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B25
	.2byte	0x121
	.4byte	.L_B25_e
.L_D98_e:
.L_D103:
	.4byte	.L_D103_e-.L_D103
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D104
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l103_e-.L_l103
.L_l103:
.L_l103_e:
.L_D103_e:
.L_D104:
	.4byte	.L_D104_e-.L_D104
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D105
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l104_e-.L_l104
.L_l104:
	.4byte	0x40
.L_l104_e:
.L_D104_e:
.L_D105:
	.4byte	0x4
.L_D102:
	.4byte	.L_D102_e-.L_D102
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D106
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B26
	.2byte	0x121
	.4byte	.L_B26_e
.L_D102_e:
.L_D107:
	.4byte	.L_D107_e-.L_D107
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D108
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l107_e-.L_l107
.L_l107:
.L_l107_e:
.L_D107_e:
.L_D108:
	.4byte	.L_D108_e-.L_D108
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D109
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l108_e-.L_l108
.L_l108:
.L_l108_e:
.L_D108_e:
.L_D109:
	.4byte	0x4
.L_D106:
	.4byte	.L_D106_e-.L_D106
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D110
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B27
	.2byte	0x121
	.4byte	.L_B27_e
.L_D106_e:
.L_D111:
	.4byte	.L_D111_e-.L_D111
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D112
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l111_e-.L_l111
.L_l111:
.L_l111_e:
.L_D111_e:
.L_D112:
	.4byte	.L_D112_e-.L_D112
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D113
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l112_e-.L_l112
.L_l112:
	.byte	0x1
	.4byte	0x0
.L_l112_e:
.L_D112_e:
.L_D113:
	.4byte	0x4
.L_D110:
	.4byte	.L_D110_e-.L_D110
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D114
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B29
	.2byte	0x121
	.4byte	.L_B29_e
.L_D110_e:
.L_D115:
	.4byte	.L_D115_e-.L_D115
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D116
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l115_e-.L_l115
.L_l115:
.L_l115_e:
.L_D115_e:
.L_D116:
	.4byte	.L_D116_e-.L_D116
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D117
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l116_e-.L_l116
.L_l116:
.L_l116_e:
.L_D116_e:
.L_D117:
	.4byte	0x4
.L_D114:
	.4byte	.L_D114_e-.L_D114
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D118
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B30
	.2byte	0x121
	.4byte	.L_B30_e
.L_D114_e:
.L_D119:
	.4byte	.L_D119_e-.L_D119
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D120
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l119_e-.L_l119
.L_l119:
.L_l119_e:
.L_D119_e:
.L_D120:
	.4byte	.L_D120_e-.L_D120
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D121
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l120_e-.L_l120
.L_l120:
	.4byte	0x20
.L_l120_e:
.L_D120_e:
.L_D121:
	.4byte	0x4
.L_D118:
	.4byte	.L_D118_e-.L_D118
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D122
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B31
	.2byte	0x121
	.4byte	.L_B31_e
.L_D118_e:
.L_D123:
	.4byte	.L_D123_e-.L_D123
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D124
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l123_e-.L_l123
.L_l123:
.L_l123_e:
.L_D123_e:
.L_D124:
	.4byte	.L_D124_e-.L_D124
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D125
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l124_e-.L_l124
.L_l124:
.L_l124_e:
.L_D124_e:
.L_D125:
	.4byte	0x4
.L_D122:
	.4byte	.L_D122_e-.L_D122
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D126
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B32
	.2byte	0x121
	.4byte	.L_B32_e
.L_D122_e:
.L_D127:
	.4byte	.L_D127_e-.L_D127
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D128
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l127_e-.L_l127
.L_l127:
.L_l127_e:
.L_D127_e:
.L_D128:
	.4byte	.L_D128_e-.L_D128
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D129
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l128_e-.L_l128
.L_l128:
	.4byte	0xc
.L_l128_e:
.L_D128_e:
.L_D129:
	.4byte	0x4
.L_D126:
	.4byte	.L_D126_e-.L_D126
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D130
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B33
	.2byte	0x121
	.4byte	.L_B33_e
.L_D126_e:
.L_D131:
	.4byte	.L_D131_e-.L_D131
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D132
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l131_e-.L_l131
.L_l131:
.L_l131_e:
.L_D131_e:
.L_D132:
	.4byte	.L_D132_e-.L_D132
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D133
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l132_e-.L_l132
.L_l132:
.L_l132_e:
.L_D132_e:
.L_D133:
	.4byte	0x4
.L_D130:
	.4byte	.L_D130_e-.L_D130
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D134
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B34
	.2byte	0x121
	.4byte	.L_B34_e
.L_D130_e:
.L_D135:
	.4byte	.L_D135_e-.L_D135
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D136
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l135_e-.L_l135
.L_l135:
.L_l135_e:
.L_D135_e:
.L_D136:
	.4byte	.L_D136_e-.L_D136
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D137
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l136_e-.L_l136
.L_l136:
	.4byte	0x30
.L_l136_e:
.L_D136_e:
.L_D137:
	.4byte	0x4
.L_D134:
	.4byte	.L_D134_e-.L_D134
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D138
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B35
	.2byte	0x121
	.4byte	.L_B35_e
.L_D134_e:
.L_D139:
	.4byte	.L_D139_e-.L_D139
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D140
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l139_e-.L_l139
.L_l139:
.L_l139_e:
.L_D139_e:
.L_D140:
	.4byte	.L_D140_e-.L_D140
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D141
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l140_e-.L_l140
.L_l140:
.L_l140_e:
.L_D140_e:
.L_D141:
	.4byte	0x4
.L_D138:
	.4byte	.L_D138_e-.L_D138
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D142
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B36
	.2byte	0x121
	.4byte	.L_B36_e
.L_D138_e:
.L_D143:
	.4byte	.L_D143_e-.L_D143
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D144
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l143_e-.L_l143
.L_l143:
.L_l143_e:
.L_D143_e:
.L_D144:
	.4byte	.L_D144_e-.L_D144
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D145
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l144_e-.L_l144
.L_l144:
	.4byte	0x0
.L_l144_e:
.L_D144_e:
.L_D145:
	.4byte	0x4
.L_D142:
	.4byte	.L_D142_e-.L_D142
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D146
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B37
	.2byte	0x121
	.4byte	.L_B37_e
.L_D142_e:
.L_D147:
	.4byte	.L_D147_e-.L_D147
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D148
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l147_e-.L_l147
.L_l147:
.L_l147_e:
.L_D147_e:
.L_D148:
	.4byte	.L_D148_e-.L_D148
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D149
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l148_e-.L_l148
.L_l148:
.L_l148_e:
.L_D148_e:
.L_D149:
	.4byte	0x4
.L_D146:
	.4byte	.L_D146_e-.L_D146
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D150
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B38
	.2byte	0x121
	.4byte	.L_B38_e
.L_D146_e:
.L_D151:
	.4byte	.L_D151_e-.L_D151
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D152
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l151_e-.L_l151
.L_l151:
.L_l151_e:
.L_D151_e:
.L_D152:
	.4byte	.L_D152_e-.L_D152
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D153
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l152_e-.L_l152
.L_l152:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffff80
	.byte	0x7
.L_l152_e:
.L_D152_e:
.L_D153:
	.4byte	0x4
.L_D150:
	.4byte	.L_D150_e-.L_D150
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D154
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B40
	.2byte	0x121
	.4byte	.L_B40_e
.L_D150_e:
.L_D155:
	.4byte	.L_D155_e-.L_D155
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D156
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l155_e-.L_l155
.L_l155:
.L_l155_e:
.L_D155_e:
.L_D156:
	.4byte	.L_D156_e-.L_D156
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D157
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l156_e-.L_l156
.L_l156:
.L_l156_e:
.L_D156_e:
.L_D157:
	.4byte	0x4
.L_D154:
	.4byte	.L_D154_e-.L_D154
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D158
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B41
	.2byte	0x121
	.4byte	.L_B41_e
.L_D154_e:
.L_D159:
	.4byte	.L_D159_e-.L_D159
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D160
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l159_e-.L_l159
.L_l159:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffff80
	.byte	0x7
.L_l159_e:
.L_D159_e:
.L_D160:
	.4byte	.L_D160_e-.L_D160
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D161
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l160_e-.L_l160
.L_l160:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffff84
	.byte	0x7
.L_l160_e:
.L_D160_e:
.L_D161:
	.4byte	0x4
.L_D158:
	.4byte	.L_D158_e-.L_D158
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D162
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B43
	.2byte	0x121
	.4byte	.L_B43_e
.L_D158_e:
.L_D163:
	.4byte	.L_D163_e-.L_D163
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D164
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l163_e-.L_l163
.L_l163:
.L_l163_e:
.L_D163_e:
.L_D164:
	.4byte	.L_D164_e-.L_D164
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D165
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l164_e-.L_l164
.L_l164:
.L_l164_e:
.L_D164_e:
.L_D165:
	.4byte	0x4
.L_D162:
	.4byte	.L_D162_e-.L_D162
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D166
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B44
	.2byte	0x121
	.4byte	.L_B44_e
.L_D162_e:
.L_D167:
	.4byte	.L_D167_e-.L_D167
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D168
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l167_e-.L_l167
.L_l167:
.L_l167_e:
.L_D167_e:
.L_D168:
	.4byte	.L_D168_e-.L_D168
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D169
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l168_e-.L_l168
.L_l168:
	.byte	0x1
	.4byte	0x0
.L_l168_e:
.L_D168_e:
.L_D169:
	.4byte	0x4
.L_D166:
	.4byte	.L_D166_e-.L_D166
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D170
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B46
	.2byte	0x121
	.4byte	.L_B46_e
.L_D166_e:
.L_D171:
	.4byte	.L_D171_e-.L_D171
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D172
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l171_e-.L_l171
.L_l171:
.L_l171_e:
.L_D171_e:
.L_D172:
	.4byte	.L_D172_e-.L_D172
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D173
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l172_e-.L_l172
.L_l172:
.L_l172_e:
.L_D172_e:
.L_D173:
	.4byte	0x4
.L_D170:
	.4byte	.L_D170_e-.L_D170
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D174
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B47
	.2byte	0x121
	.4byte	.L_B47_e
.L_D170_e:
.L_D175:
	.4byte	.L_D175_e-.L_D175
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D176
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l175_e-.L_l175
.L_l175:
.L_l175_e:
.L_D175_e:
.L_D176:
	.4byte	.L_D176_e-.L_D176
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D177
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l176_e-.L_l176
.L_l176:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffff80
	.byte	0x7
.L_l176_e:
.L_D176_e:
.L_D177:
	.4byte	0x4
.L_D174:
	.4byte	.L_D174_e-.L_D174
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D178
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B49
	.2byte	0x121
	.4byte	.L_B49_e
.L_D174_e:
.L_D179:
	.4byte	.L_D179_e-.L_D179
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D180
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l179_e-.L_l179
.L_l179:
.L_l179_e:
.L_D179_e:
.L_D180:
	.4byte	.L_D180_e-.L_D180
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D181
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l180_e-.L_l180
.L_l180:
.L_l180_e:
.L_D180_e:
.L_D181:
	.4byte	0x4
.L_D178:
	.4byte	.L_D178_e-.L_D178
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D182
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B50
	.2byte	0x121
	.4byte	.L_B50_e
.L_D178_e:
.L_D183:
	.4byte	.L_D183_e-.L_D183
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D184
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l183_e-.L_l183
.L_l183:
.L_l183_e:
.L_D183_e:
.L_D184:
	.4byte	.L_D184_e-.L_D184
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D185
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l184_e-.L_l184
.L_l184:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffff78
	.byte	0x7
.L_l184_e:
.L_D184_e:
.L_D185:
	.4byte	0x4
.L_D182:
	.4byte	.L_D182_e-.L_D182
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D186
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B52
	.2byte	0x121
	.4byte	.L_B52_e
.L_D182_e:
.L_D187:
	.4byte	.L_D187_e-.L_D187
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D188
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l187_e-.L_l187
.L_l187:
.L_l187_e:
.L_D187_e:
.L_D188:
	.4byte	.L_D188_e-.L_D188
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D189
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l188_e-.L_l188
.L_l188:
.L_l188_e:
.L_D188_e:
.L_D189:
	.4byte	0x4
.L_D186:
	.4byte	0x4
.L_D16:
	.4byte	.L_D16_e-.L_D16
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D190
	.set	.L_T816,.L_D16
	.2byte	0x38
	.string	"sas_info"
	.2byte	0xb6
	.4byte	0xcc
.L_D16_e:
.L_D191:
	.4byte	.L_D191_e-.L_D191
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D192
	.2byte	0x38
	.string	"prev_int_user"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x83
	.2byte	.L_t191_e-.L_t191
.L_t191:
	.byte	0x1
	.4byte	.L_T816
.L_t191_e:
	.2byte	0x23
	.2byte	.L_l191_e-.L_l191
.L_l191:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l191_e:
.L_D191_e:
.L_D192:
	.4byte	.L_D192_e-.L_D192
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D193
	.2byte	0x38
	.string	"next_int_user"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x83
	.2byte	.L_t192_e-.L_t192
.L_t192:
	.byte	0x1
	.4byte	.L_T816
.L_t192_e:
	.2byte	0x23
	.2byte	.L_l192_e-.L_l192
.L_l192:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l192_e:
.L_D192_e:
.L_D193:
	.4byte	.L_D193_e-.L_D193
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D194
	.2byte	0x38
	.string	"timeout_idx"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l193_e-.L_l193
.L_l193:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l193_e:
.L_D193_e:
.L_D194:
	.4byte	.L_D194_e-.L_D194
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D195
	.2byte	0x38
	.string	"iflag"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l194_e-.L_l194
.L_l194:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l194_e:
.L_D194_e:
.L_D195:
	.4byte	.L_D195_e-.L_D195
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D196
	.2byte	0x38
	.string	"scflag"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l195_e-.L_l195
.L_l195:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l195_e:
.L_D195_e:
.L_D196:
	.4byte	.L_D196_e-.L_D196
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D197
	.2byte	0x38
	.string	"cflag"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l196_e-.L_l196
.L_l196:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l196_e:
.L_D196_e:
.L_D197:
	.4byte	.L_D197_e-.L_D197
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D198
	.2byte	0x38
	.string	"oflag"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l197_e-.L_l197
.L_l197:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l197_e:
.L_D197_e:
.L_D198:
	.4byte	.L_D198_e-.L_D198
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D199
	.2byte	0x38
	.string	"lflag"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l198_e-.L_l198
.L_l198:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l198_e:
.L_D198_e:
.L_D199:
	.4byte	.L_D199_e-.L_D199
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D200
	.2byte	0x38
	.string	"device_flags"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x72
	.4byte	.L_T818
	.2byte	0x23
	.2byte	.L_l199_e-.L_l199
.L_l199:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l199_e:
.L_D199_e:
.L_D200:
	.4byte	.L_D200_e-.L_D200
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D201
	.2byte	0x38
	.string	"flow_flags"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x72
	.4byte	.L_T819
	.2byte	0x23
	.2byte	.L_l200_e-.L_l200
.L_l200:
	.byte	0x4
	.4byte	0x24
	.byte	0x7
.L_l200_e:
.L_D200_e:
.L_D201:
	.4byte	.L_D201_e-.L_D201
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D202
	.2byte	0x38
	.string	"event_flags"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x72
	.4byte	.L_T820
	.2byte	0x23
	.2byte	.L_l201_e-.L_l201
.L_l201:
	.byte	0x4
	.4byte	0x28
	.byte	0x7
.L_l201_e:
.L_D201_e:
.L_D202:
	.4byte	.L_D202_e-.L_D202
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D203
	.2byte	0x38
	.string	"o_state"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l202_e-.L_l202
.L_l202:
	.byte	0x4
	.4byte	0x2c
	.byte	0x7
.L_l202_e:
.L_D202_e:
.L_D203:
	.4byte	.L_D203_e-.L_D203
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D204
	.2byte	0x38
	.string	"po_state"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l203_e-.L_l203
.L_l203:
	.byte	0x4
	.4byte	0x30
	.byte	0x7
.L_l203_e:
.L_D203_e:
.L_D204:
	.4byte	.L_D204_e-.L_D204
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D205
	.2byte	0x38
	.string	"modem"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x72
	.4byte	.L_T822
	.2byte	0x23
	.2byte	.L_l204_e-.L_l204
.L_l204:
	.byte	0x4
	.4byte	0x34
	.byte	0x7
.L_l204_e:
.L_D204_e:
.L_D205:
	.4byte	.L_D205_e-.L_D205
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D206
	.2byte	0x38
	.string	"flow"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x72
	.4byte	.L_T824
	.2byte	0x23
	.2byte	.L_l205_e-.L_l205
.L_l205:
	.byte	0x4
	.4byte	0x38
	.byte	0x7
.L_l205_e:
.L_D205_e:
.L_D206:
	.4byte	.L_D206_e-.L_D206
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D207
	.2byte	0x38
	.string	"msr"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l206_e-.L_l206
.L_l206:
	.byte	0x4
	.4byte	0x3c
	.byte	0x7
.L_l206_e:
.L_D206_e:
.L_D207:
	.4byte	.L_D207_e-.L_D207
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D208
	.2byte	0x38
	.string	"new_msr"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l207_e-.L_l207
.L_l207:
	.byte	0x4
	.4byte	0x3d
	.byte	0x7
.L_l207_e:
.L_D207_e:
.L_D208:
	.4byte	.L_D208_e-.L_D208
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D209
	.2byte	0x38
	.string	"mcr"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l208_e-.L_l208
.L_l208:
	.byte	0x4
	.4byte	0x3e
	.byte	0x7
.L_l208_e:
.L_D208_e:
.L_D209:
	.4byte	.L_D209_e-.L_D209
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D210
	.2byte	0x38
	.string	"lcr"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l209_e-.L_l209
.L_l209:
	.byte	0x4
	.4byte	0x3f
	.byte	0x7
.L_l209_e:
.L_D209_e:
.L_D210:
	.4byte	.L_D210_e-.L_D210
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D211
	.2byte	0x38
	.string	"ier"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l210_e-.L_l210
.L_l210:
	.byte	0x4
	.4byte	0x40
	.byte	0x7
.L_l210_e:
.L_D210_e:
.L_D211:
	.4byte	.L_D211_e-.L_D211
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D212
	.2byte	0x38
	.string	"vec"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l211_e-.L_l211
.L_l211:
	.byte	0x4
	.4byte	0x41
	.byte	0x7
.L_l211_e:
.L_D211_e:
.L_D212:
	.4byte	.L_D212_e-.L_D212
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D213
	.2byte	0x38
	.string	"device_type"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l212_e-.L_l212
.L_l212:
	.byte	0x4
	.4byte	0x42
	.byte	0x7
.L_l212_e:
.L_D212_e:
.L_D213:
	.4byte	.L_D213_e-.L_D213
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D214
	.2byte	0x38
	.string	"ctl_port"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l213_e-.L_l213
.L_l213:
	.byte	0x4
	.4byte	0x44
	.byte	0x7
.L_l213_e:
.L_D213_e:
.L_D214:
	.4byte	.L_D214_e-.L_D214
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D215
	.2byte	0x38
	.string	"port_0"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x72
	.4byte	.L_T826
	.2byte	0x23
	.2byte	.L_l214_e-.L_l214
.L_l214:
	.byte	0x4
	.4byte	0x48
	.byte	0x7
.L_l214_e:
.L_D214_e:
.L_D215:
	.4byte	.L_D215_e-.L_D215
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D216
	.2byte	0x38
	.string	"port_1"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x72
	.4byte	.L_T826
	.2byte	0x23
	.2byte	.L_l215_e-.L_l215
.L_l215:
	.byte	0x4
	.4byte	0x4c
	.byte	0x7
.L_l215_e:
.L_D215_e:
.L_D216:
	.4byte	.L_D216_e-.L_D216
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D217
	.2byte	0x38
	.string	"port_2"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x72
	.4byte	.L_T826
	.2byte	0x23
	.2byte	.L_l216_e-.L_l216
.L_l216:
	.byte	0x4
	.4byte	0x50
	.byte	0x7
.L_l216_e:
.L_D216_e:
.L_D217:
	.4byte	.L_D217_e-.L_D217
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D218
	.2byte	0x38
	.string	"port_3"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x72
	.4byte	.L_T826
	.2byte	0x23
	.2byte	.L_l217_e-.L_l217
.L_l217:
	.byte	0x4
	.4byte	0x54
	.byte	0x7
.L_l217_e:
.L_D217_e:
.L_D218:
	.4byte	.L_D218_e-.L_D218
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D219
	.2byte	0x38
	.string	"port_4"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x72
	.4byte	.L_T826
	.2byte	0x23
	.2byte	.L_l218_e-.L_l218
.L_l218:
	.byte	0x4
	.4byte	0x58
	.byte	0x7
.L_l218_e:
.L_D218_e:
.L_D219:
	.4byte	.L_D219_e-.L_D219
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D220
	.2byte	0x38
	.string	"port_5"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x72
	.4byte	.L_T826
	.2byte	0x23
	.2byte	.L_l219_e-.L_l219
.L_l219:
	.byte	0x4
	.4byte	0x5c
	.byte	0x7
.L_l219_e:
.L_D219_e:
.L_D220:
	.4byte	.L_D220_e-.L_D220
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D221
	.2byte	0x38
	.string	"port_6"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x72
	.4byte	.L_T826
	.2byte	0x23
	.2byte	.L_l220_e-.L_l220
.L_l220:
	.byte	0x4
	.4byte	0x60
	.byte	0x7
.L_l220_e:
.L_D220_e:
.L_D221:
	.4byte	.L_D221_e-.L_D221
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D222
	.2byte	0x38
	.string	"xmit_fifo_size"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l221_e-.L_l221
.L_l221:
	.byte	0x4
	.4byte	0x64
	.byte	0x7
.L_l221_e:
.L_D221_e:
.L_D222:
	.4byte	.L_D222_e-.L_D222
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D223
	.2byte	0x38
	.string	"recv_cnt"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l222_e-.L_l222
.L_l222:
	.byte	0x4
	.4byte	0x68
	.byte	0x7
.L_l222_e:
.L_D222_e:
.L_D223:
	.4byte	.L_D223_e-.L_D223
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D224
	.2byte	0x38
	.string	"alloc_size"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l223_e-.L_l223
.L_l223:
	.byte	0x4
	.4byte	0x6c
	.byte	0x7
.L_l223_e:
.L_D223_e:
.L_D224:
	.4byte	.L_D224_e-.L_D224
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D225
	.2byte	0x38
	.string	"sas_curstate"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x63
	.2byte	.L_t224_e-.L_t224
.L_t224:
	.byte	0x1
	.2byte	0x9
.L_t224_e:
	.2byte	0x23
	.2byte	.L_l224_e-.L_l224
.L_l224:
	.byte	0x4
	.4byte	0x70
	.byte	0x7
.L_l224_e:
.L_D224_e:
.L_D225:
	.4byte	.L_D225_e-.L_D225
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D226
	.2byte	0x38
	.string	"sas_timerid"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l225_e-.L_l225
.L_l225:
	.byte	0x4
	.4byte	0x74
	.byte	0x7
.L_l225_e:
.L_D225_e:
.L_D226:
	.4byte	.L_D226_e-.L_D226
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D227
	.2byte	0x38
	.string	"unit"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l226_e-.L_l226
.L_l226:
	.byte	0x4
	.4byte	0x78
	.byte	0x7
.L_l226_e:
.L_D226_e:
.L_D227:
	.4byte	.L_D227_e-.L_D227
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D228
	.2byte	0x38
	.string	"sas_dev"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x72
	.4byte	.L_T830
	.2byte	0x23
	.2byte	.L_l227_e-.L_l227
.L_l227:
	.byte	0x4
	.4byte	0x7c
	.byte	0x7
.L_l227_e:
.L_D227_e:
.L_D228:
	.4byte	.L_D228_e-.L_D228
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D229
	.2byte	0x38
	.string	"rq"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x83
	.2byte	.L_t228_e-.L_t228
.L_t228:
	.byte	0x1
	.4byte	.L_T664
.L_t228_e:
	.2byte	0x23
	.2byte	.L_l228_e-.L_l228
.L_l228:
	.byte	0x4
	.4byte	0x9c
	.byte	0x7
.L_l228_e:
.L_D228_e:
.L_D229:
	.4byte	.L_D229_e-.L_D229
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D230
	.2byte	0x38
	.string	"holdbuf"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x83
	.2byte	.L_t229_e-.L_t229
.L_t229:
	.byte	0x1
	.4byte	.L_T667
.L_t229_e:
	.2byte	0x23
	.2byte	.L_l229_e-.L_l229
.L_l229:
	.byte	0x4
	.4byte	0xa0
	.byte	0x7
.L_l229_e:
.L_D229_e:
.L_D230:
	.4byte	.L_D230_e-.L_D230
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D231
	.2byte	0x38
	.string	"free"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x83
	.2byte	.L_t230_e-.L_t230
.L_t230:
	.byte	0x1
	.4byte	.L_T667
.L_t230_e:
	.2byte	0x23
	.2byte	.L_l230_e-.L_l230
.L_l230:
	.byte	0x4
	.4byte	0xa4
	.byte	0x7
.L_l230_e:
.L_D230_e:
.L_D231:
	.4byte	.L_D231_e-.L_D231
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D232
	.2byte	0x38
	.string	"nfree"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l231_e-.L_l231
.L_l231:
	.byte	0x4
	.4byte	0xa8
	.byte	0x7
.L_l231_e:
.L_D231_e:
.L_D232:
	.4byte	.L_D232_e-.L_D232
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D233
	.2byte	0x38
	.string	"discards"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x83
	.2byte	.L_t232_e-.L_t232
.L_t232:
	.byte	0x1
	.4byte	.L_T667
.L_t232_e:
	.2byte	0x23
	.2byte	.L_l232_e-.L_l232
.L_l232:
	.byte	0x4
	.4byte	0xac
	.byte	0x7
.L_l232_e:
.L_D232_e:
.L_D233:
	.4byte	.L_D233_e-.L_D233
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D234
	.2byte	0x38
	.string	"recv"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x83
	.2byte	.L_t233_e-.L_t233
.L_t233:
	.byte	0x1
	.4byte	.L_T667
.L_t233_e:
	.2byte	0x23
	.2byte	.L_l233_e-.L_l233
.L_l233:
	.byte	0x4
	.4byte	0xb0
	.byte	0x7
.L_l233_e:
.L_D233_e:
.L_D234:
	.4byte	.L_D234_e-.L_D234
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D235
	.2byte	0x38
	.string	"filled"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x83
	.2byte	.L_t234_e-.L_t234
.L_t234:
	.byte	0x1
	.4byte	.L_T667
.L_t234_e:
	.2byte	0x23
	.2byte	.L_l234_e-.L_l234
.L_l234:
	.byte	0x4
	.4byte	0xb4
	.byte	0x7
.L_l234_e:
.L_D234_e:
.L_D235:
	.4byte	.L_D235_e-.L_D235
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D236
	.2byte	0x38
	.string	"cc"
	.2byte	0x142
	.4byte	.L_T816
	.2byte	0x72
	.4byte	.L_T522
	.2byte	0x23
	.2byte	.L_l235_e-.L_l235
.L_l235:
	.byte	0x4
	.4byte	0xb8
	.byte	0x7
.L_l235_e:
.L_D235_e:
.L_D236:
	.4byte	0x4
.L_D190:
	.4byte	.L_D190_e-.L_D190
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D237
	.set	.L_T818,.L_D190
	.2byte	0xb6
	.4byte	0x4
.L_D190_e:
.L_D238:
	.4byte	.L_D238_e-.L_D238
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D239
	.2byte	0x38
	.string	"s"
	.2byte	0x142
	.4byte	.L_T818
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l238_e-.L_l238
.L_l238:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l238_e:
.L_D238_e:
.L_D239:
	.4byte	.L_D239_e-.L_D239
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D240
	.2byte	0x38
	.string	"i"
	.2byte	0x142
	.4byte	.L_T818
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l239_e-.L_l239
.L_l239:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l239_e:
.L_D239_e:
.L_D240:
	.4byte	0x4
.L_D237:
	.4byte	.L_D237_e-.L_D237
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D241
	.set	.L_T819,.L_D237
	.2byte	0xb6
	.4byte	0x4
.L_D237_e:
.L_D242:
	.4byte	.L_D242_e-.L_D242
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D243
	.2byte	0x38
	.string	"s"
	.2byte	0x142
	.4byte	.L_T819
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l242_e-.L_l242
.L_l242:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l242_e:
.L_D242_e:
.L_D243:
	.4byte	.L_D243_e-.L_D243
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D244
	.2byte	0x38
	.string	"i"
	.2byte	0x142
	.4byte	.L_T819
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l243_e-.L_l243
.L_l243:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l243_e:
.L_D243_e:
.L_D244:
	.4byte	0x4
.L_D241:
	.4byte	.L_D241_e-.L_D241
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D245
	.set	.L_T820,.L_D241
	.2byte	0xb6
	.4byte	0x4
.L_D241_e:
.L_D246:
	.4byte	.L_D246_e-.L_D246
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D247
	.2byte	0x38
	.string	"s"
	.2byte	0x142
	.4byte	.L_T820
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l246_e-.L_l246
.L_l246:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l246_e:
.L_D246_e:
.L_D247:
	.4byte	.L_D247_e-.L_D247
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D248
	.2byte	0x38
	.string	"i"
	.2byte	0x142
	.4byte	.L_T820
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l247_e-.L_l247
.L_l247:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l247_e:
.L_D247_e:
.L_D248:
	.4byte	0x4
.L_D245:
	.4byte	.L_D245_e-.L_D245
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D249
	.set	.L_T822,.L_D245
	.2byte	0xb6
	.4byte	0x4
.L_D245_e:
.L_D250:
	.4byte	.L_D250_e-.L_D250
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D251
	.2byte	0x38
	.string	"m"
	.2byte	0x142
	.4byte	.L_T822
	.2byte	0x72
	.4byte	.L_T821
	.2byte	0x23
	.2byte	.L_l250_e-.L_l250
.L_l250:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l250_e:
.L_D250_e:
.L_D251:
	.4byte	.L_D251_e-.L_D251
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D252
	.2byte	0x38
	.string	"l"
	.2byte	0x142
	.4byte	.L_T822
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l251_e-.L_l251
.L_l251:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l251_e:
.L_D251_e:
.L_D252:
	.4byte	0x4
.L_D249:
	.4byte	.L_D249_e-.L_D249
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D253
	.set	.L_T824,.L_D249
	.2byte	0xb6
	.4byte	0x4
.L_D249_e:
.L_D254:
	.4byte	.L_D254_e-.L_D254
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D255
	.2byte	0x38
	.string	"m"
	.2byte	0x142
	.4byte	.L_T824
	.2byte	0x72
	.4byte	.L_T823
	.2byte	0x23
	.2byte	.L_l254_e-.L_l254
.L_l254:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l254_e:
.L_D254_e:
.L_D255:
	.4byte	.L_D255_e-.L_D255
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D256
	.2byte	0x38
	.string	"l"
	.2byte	0x142
	.4byte	.L_T824
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l255_e-.L_l255
.L_l255:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l255_e:
.L_D255_e:
.L_D256:
	.4byte	0x4
.L_D253:
	.4byte	.L_D253_e-.L_D253
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D257
	.set	.L_T826,.L_D253
	.2byte	0xb6
	.4byte	0x4
.L_D253_e:
.L_D258:
	.4byte	.L_D258_e-.L_D258
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D259
	.2byte	0x38
	.string	"addr"
	.2byte	0x142
	.4byte	.L_T826
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l258_e-.L_l258
.L_l258:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l258_e:
.L_D258_e:
.L_D259:
	.4byte	.L_D259_e-.L_D259
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D260
	.2byte	0x38
	.string	"p"
	.2byte	0x142
	.4byte	.L_T826
	.2byte	0x72
	.4byte	.L_T825
	.2byte	0x23
	.2byte	.L_l259_e-.L_l259
.L_l259:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l259_e:
.L_D259_e:
.L_D260:
	.4byte	0x4
.L_D257:
	.4byte	.L_D257_e-.L_D257
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D261
	.set	.L_T815,.L_D257
	.2byte	0x38
	.string	"fptr"
	.2byte	0xb6
	.4byte	0x10
.L_D257_e:
.L_D262:
	.4byte	.L_D262_e-.L_D262
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D263
	.2byte	0x38
	.string	"dev"
	.2byte	0x142
	.4byte	.L_T815
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l262_e-.L_l262
.L_l262:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l262_e:
.L_D262_e:
.L_D263:
	.4byte	.L_D263_e-.L_D263
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D264
	.2byte	0x38
	.string	"sas_state"
	.2byte	0x142
	.4byte	.L_T815
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l263_e-.L_l263
.L_l263:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l263_e:
.L_D263_e:
.L_D264:
	.4byte	.L_D264_e-.L_D264
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D265
	.2byte	0x38
	.string	"fip"
	.2byte	0x142
	.4byte	.L_T815
	.2byte	0x83
	.2byte	.L_t264_e-.L_t264
.L_t264:
	.byte	0x1
	.4byte	.L_T816
.L_t264_e:
	.2byte	0x23
	.2byte	.L_l264_e-.L_l264
.L_l264:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l264_e:
.L_D264_e:
.L_D265:
	.4byte	.L_D265_e-.L_D265
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D266
	.2byte	0x38
	.string	"rq"
	.2byte	0x142
	.4byte	.L_T815
	.2byte	0x83
	.2byte	.L_t265_e-.L_t265
.L_t265:
	.byte	0x1
	.4byte	.L_T664
.L_t265_e:
	.2byte	0x23
	.2byte	.L_l265_e-.L_l265
.L_l265:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l265_e:
.L_D265_e:
.L_D266:
	.4byte	0x4
.L_D261:
	.4byte	.L_D261_e-.L_D261
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D267
	.set	.L_T830,.L_D261
	.2byte	0xa3
	.2byte	.L_s261_e-.L_s261
.L_s261:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x1
	.byte	0x8
	.2byte	0x72
	.4byte	.L_T815
.L_s261_e:
.L_D261_e:
.L_D267:
	.4byte	.L_D267_e-.L_D267
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D268
	.set	.L_T664,.L_D267
	.2byte	0x38
	.string	"queue"
	.2byte	0xb6
	.4byte	0x40
.L_D267_e:
.L_D269:
	.4byte	.L_D269_e-.L_D269
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D270
	.2byte	0x38
	.string	"q_qinfo"
	.2byte	0x142
	.4byte	.L_T664
	.2byte	0x83
	.2byte	.L_t269_e-.L_t269
.L_t269:
	.byte	0x1
	.4byte	.L_T665
.L_t269_e:
	.2byte	0x23
	.2byte	.L_l269_e-.L_l269
.L_l269:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l269_e:
.L_D269_e:
.L_D270:
	.4byte	.L_D270_e-.L_D270
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D271
	.2byte	0x38
	.string	"q_first"
	.2byte	0x142
	.4byte	.L_T664
	.2byte	0x83
	.2byte	.L_t270_e-.L_t270
.L_t270:
	.byte	0x1
	.4byte	.L_T667
.L_t270_e:
	.2byte	0x23
	.2byte	.L_l270_e-.L_l270
.L_l270:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l270_e:
.L_D270_e:
.L_D271:
	.4byte	.L_D271_e-.L_D271
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D272
	.2byte	0x38
	.string	"q_last"
	.2byte	0x142
	.4byte	.L_T664
	.2byte	0x83
	.2byte	.L_t271_e-.L_t271
.L_t271:
	.byte	0x1
	.4byte	.L_T667
.L_t271_e:
	.2byte	0x23
	.2byte	.L_l271_e-.L_l271
.L_l271:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l271_e:
.L_D271_e:
.L_D272:
	.4byte	.L_D272_e-.L_D272
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D273
	.2byte	0x38
	.string	"q_next"
	.2byte	0x142
	.4byte	.L_T664
	.2byte	0x83
	.2byte	.L_t272_e-.L_t272
.L_t272:
	.byte	0x1
	.4byte	.L_T664
.L_t272_e:
	.2byte	0x23
	.2byte	.L_l272_e-.L_l272
.L_l272:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l272_e:
.L_D272_e:
.L_D273:
	.4byte	.L_D273_e-.L_D273
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D274
	.2byte	0x38
	.string	"q_link"
	.2byte	0x142
	.4byte	.L_T664
	.2byte	0x83
	.2byte	.L_t273_e-.L_t273
.L_t273:
	.byte	0x1
	.4byte	.L_T664
.L_t273_e:
	.2byte	0x23
	.2byte	.L_l273_e-.L_l273
.L_l273:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l273_e:
.L_D273_e:
.L_D274:
	.4byte	.L_D274_e-.L_D274
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D275
	.2byte	0x38
	.string	"q_ptr"
	.2byte	0x142
	.4byte	.L_T664
	.2byte	0x63
	.2byte	.L_t274_e-.L_t274
.L_t274:
	.byte	0x1
	.2byte	0x14
.L_t274_e:
	.2byte	0x23
	.2byte	.L_l274_e-.L_l274
.L_l274:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l274_e:
.L_D274_e:
.L_D275:
	.4byte	.L_D275_e-.L_D275
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D276
	.2byte	0x38
	.string	"q_count"
	.2byte	0x142
	.4byte	.L_T664
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l275_e-.L_l275
.L_l275:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l275_e:
.L_D275_e:
.L_D276:
	.4byte	.L_D276_e-.L_D276
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D277
	.2byte	0x38
	.string	"q_flag"
	.2byte	0x142
	.4byte	.L_T664
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l276_e-.L_l276
.L_l276:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l276_e:
.L_D276_e:
.L_D277:
	.4byte	.L_D277_e-.L_D277
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D278
	.2byte	0x38
	.string	"q_minpsz"
	.2byte	0x142
	.4byte	.L_T664
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l277_e-.L_l277
.L_l277:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l277_e:
.L_D277_e:
.L_D278:
	.4byte	.L_D278_e-.L_D278
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D279
	.2byte	0x38
	.string	"q_maxpsz"
	.2byte	0x142
	.4byte	.L_T664
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l278_e-.L_l278
.L_l278:
	.byte	0x4
	.4byte	0x24
	.byte	0x7
.L_l278_e:
.L_D278_e:
.L_D279:
	.4byte	.L_D279_e-.L_D279
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D280
	.2byte	0x38
	.string	"q_hiwat"
	.2byte	0x142
	.4byte	.L_T664
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l279_e-.L_l279
.L_l279:
	.byte	0x4
	.4byte	0x28
	.byte	0x7
.L_l279_e:
.L_D279_e:
.L_D280:
	.4byte	.L_D280_e-.L_D280
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D281
	.2byte	0x38
	.string	"q_lowat"
	.2byte	0x142
	.4byte	.L_T664
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l280_e-.L_l280
.L_l280:
	.byte	0x4
	.4byte	0x2c
	.byte	0x7
.L_l280_e:
.L_D280_e:
.L_D281:
	.4byte	.L_D281_e-.L_D281
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D282
	.2byte	0x38
	.string	"q_bandp"
	.2byte	0x142
	.4byte	.L_T664
	.2byte	0x83
	.2byte	.L_t281_e-.L_t281
.L_t281:
	.byte	0x1
	.4byte	.L_T670
.L_t281_e:
	.2byte	0x23
	.2byte	.L_l281_e-.L_l281
.L_l281:
	.byte	0x4
	.4byte	0x30
	.byte	0x7
.L_l281_e:
.L_D281_e:
.L_D282:
	.4byte	.L_D282_e-.L_D282
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D283
	.2byte	0x38
	.string	"q_nband"
	.2byte	0x142
	.4byte	.L_T664
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l282_e-.L_l282
.L_l282:
	.byte	0x4
	.4byte	0x34
	.byte	0x7
.L_l282_e:
.L_D282_e:
.L_D283:
	.4byte	.L_D283_e-.L_D283
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D284
	.2byte	0x38
	.string	"q_blocked"
	.2byte	0x142
	.4byte	.L_T664
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l283_e-.L_l283
.L_l283:
	.byte	0x4
	.4byte	0x35
	.byte	0x7
.L_l283_e:
.L_D283_e:
.L_D284:
	.4byte	.L_D284_e-.L_D284
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D285
	.2byte	0x38
	.string	"q_pad1"
	.2byte	0x142
	.4byte	.L_T664
	.2byte	0x72
	.4byte	.L_T673
	.2byte	0x23
	.2byte	.L_l284_e-.L_l284
.L_l284:
	.byte	0x4
	.4byte	0x36
	.byte	0x7
.L_l284_e:
.L_D284_e:
.L_D285:
	.4byte	.L_D285_e-.L_D285
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D286
	.2byte	0x38
	.string	"q_pad2"
	.2byte	0x142
	.4byte	.L_T664
	.2byte	0x72
	.4byte	.L_T139
	.2byte	0x23
	.2byte	.L_l285_e-.L_l285
.L_l285:
	.byte	0x4
	.4byte	0x38
	.byte	0x7
.L_l285_e:
.L_D285_e:
.L_D286:
	.4byte	0x4
.L_D268:
	.4byte	.L_D268_e-.L_D268
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D287
	.set	.L_T667,.L_D268
	.2byte	0x38
	.string	"msgb"
	.2byte	0xb6
	.4byte	0x20
.L_D268_e:
.L_D288:
	.4byte	.L_D288_e-.L_D288
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D289
	.2byte	0x38
	.string	"b_next"
	.2byte	0x142
	.4byte	.L_T667
	.2byte	0x83
	.2byte	.L_t288_e-.L_t288
.L_t288:
	.byte	0x1
	.4byte	.L_T667
.L_t288_e:
	.2byte	0x23
	.2byte	.L_l288_e-.L_l288
.L_l288:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l288_e:
.L_D288_e:
.L_D289:
	.4byte	.L_D289_e-.L_D289
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D290
	.2byte	0x38
	.string	"b_prev"
	.2byte	0x142
	.4byte	.L_T667
	.2byte	0x83
	.2byte	.L_t289_e-.L_t289
.L_t289:
	.byte	0x1
	.4byte	.L_T667
.L_t289_e:
	.2byte	0x23
	.2byte	.L_l289_e-.L_l289
.L_l289:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l289_e:
.L_D289_e:
.L_D290:
	.4byte	.L_D290_e-.L_D290
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D291
	.2byte	0x38
	.string	"b_cont"
	.2byte	0x142
	.4byte	.L_T667
	.2byte	0x83
	.2byte	.L_t290_e-.L_t290
.L_t290:
	.byte	0x1
	.4byte	.L_T667
.L_t290_e:
	.2byte	0x23
	.2byte	.L_l290_e-.L_l290
.L_l290:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l290_e:
.L_D290_e:
.L_D291:
	.4byte	.L_D291_e-.L_D291
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D292
	.2byte	0x38
	.string	"b_rptr"
	.2byte	0x142
	.4byte	.L_T667
	.2byte	0x63
	.2byte	.L_t291_e-.L_t291
.L_t291:
	.byte	0x1
	.2byte	0x3
.L_t291_e:
	.2byte	0x23
	.2byte	.L_l291_e-.L_l291
.L_l291:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l291_e:
.L_D291_e:
.L_D292:
	.4byte	.L_D292_e-.L_D292
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D293
	.2byte	0x38
	.string	"b_wptr"
	.2byte	0x142
	.4byte	.L_T667
	.2byte	0x63
	.2byte	.L_t292_e-.L_t292
.L_t292:
	.byte	0x1
	.2byte	0x3
.L_t292_e:
	.2byte	0x23
	.2byte	.L_l292_e-.L_l292
.L_l292:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l292_e:
.L_D292_e:
.L_D293:
	.4byte	.L_D293_e-.L_D293
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D294
	.2byte	0x38
	.string	"b_datap"
	.2byte	0x142
	.4byte	.L_T667
	.2byte	0x83
	.2byte	.L_t293_e-.L_t293
.L_t293:
	.byte	0x1
	.4byte	.L_T695
.L_t293_e:
	.2byte	0x23
	.2byte	.L_l293_e-.L_l293
.L_l293:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l293_e:
.L_D293_e:
.L_D294:
	.4byte	.L_D294_e-.L_D294
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D295
	.2byte	0x38
	.string	"b_band"
	.2byte	0x142
	.4byte	.L_T667
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l294_e-.L_l294
.L_l294:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l294_e:
.L_D294_e:
.L_D295:
	.4byte	.L_D295_e-.L_D295
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D296
	.2byte	0x38
	.string	"b_pad1"
	.2byte	0x142
	.4byte	.L_T667
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l295_e-.L_l295
.L_l295:
	.byte	0x4
	.4byte	0x19
	.byte	0x7
.L_l295_e:
.L_D295_e:
.L_D296:
	.4byte	.L_D296_e-.L_D296
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D297
	.2byte	0x38
	.string	"b_flag"
	.2byte	0x142
	.4byte	.L_T667
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l296_e-.L_l296
.L_l296:
	.byte	0x4
	.4byte	0x1a
	.byte	0x7
.L_l296_e:
.L_D296_e:
.L_D297:
	.4byte	.L_D297_e-.L_D297
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D298
	.2byte	0x38
	.string	"b_pad2"
	.2byte	0x142
	.4byte	.L_T667
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l297_e-.L_l297
.L_l297:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l297_e:
.L_D297_e:
.L_D298:
	.4byte	0x4
.L_D287:
	.4byte	.L_D287_e-.L_D287
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D299
	.set	.L_T522,.L_D287
	.2byte	0xa3
	.2byte	.L_s287_e-.L_s287
.L_s287:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x12
	.byte	0x8
	.2byte	0x55
	.2byte	0x3
.L_s287_e:
.L_D287_e:
.L_D299:
	.4byte	.L_D299_e-.L_D299
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D300
	.set	.L_T821,.L_D299
	.2byte	0xb6
	.4byte	0x4
.L_D299_e:
.L_D301:
	.4byte	.L_D301_e-.L_D301
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D302
	.2byte	0x38
	.string	"di"
	.2byte	0x142
	.4byte	.L_T821
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l301_e-.L_l301
.L_l301:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l301_e:
.L_D301_e:
.L_D302:
	.4byte	.L_D302_e-.L_D302
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D303
	.2byte	0x38
	.string	"eo"
	.2byte	0x142
	.4byte	.L_T821
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l302_e-.L_l302
.L_l302:
	.byte	0x4
	.4byte	0x1
	.byte	0x7
.L_l302_e:
.L_D302_e:
.L_D303:
	.4byte	.L_D303_e-.L_D303
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D304
	.2byte	0x38
	.string	"ei"
	.2byte	0x142
	.4byte	.L_T821
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l303_e-.L_l303
.L_l303:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l303_e:
.L_D303_e:
.L_D304:
	.4byte	.L_D304_e-.L_D304
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D305
	.2byte	0x38
	.string	"ca"
	.2byte	0x142
	.4byte	.L_T821
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l304_e-.L_l304
.L_l304:
	.byte	0x4
	.4byte	0x3
	.byte	0x7
.L_l304_e:
.L_D304_e:
.L_D305:
	.4byte	0x4
.L_D300:
	.4byte	.L_D300_e-.L_D300
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D306
	.set	.L_T823,.L_D300
	.2byte	0xb6
	.4byte	0x4
.L_D300_e:
.L_D307:
	.4byte	.L_D307_e-.L_D307
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D308
	.2byte	0x38
	.string	"ic"
	.2byte	0x142
	.4byte	.L_T823
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l307_e-.L_l307
.L_l307:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l307_e:
.L_D307_e:
.L_D308:
	.4byte	.L_D308_e-.L_D308
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D309
	.2byte	0x38
	.string	"oc"
	.2byte	0x142
	.4byte	.L_T823
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l308_e-.L_l308
.L_l308:
	.byte	0x4
	.4byte	0x1
	.byte	0x7
.L_l308_e:
.L_D308_e:
.L_D309:
	.4byte	.L_D309_e-.L_D309
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D310
	.2byte	0x38
	.string	"oe"
	.2byte	0x142
	.4byte	.L_T823
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l309_e-.L_l309
.L_l309:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l309_e:
.L_D309_e:
.L_D310:
	.4byte	.L_D310_e-.L_D310
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D311
	.2byte	0x38
	.string	"hc"
	.2byte	0x142
	.4byte	.L_T823
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l310_e-.L_l310
.L_l310:
	.byte	0x4
	.4byte	0x3
	.byte	0x7
.L_l310_e:
.L_D310_e:
.L_D311:
	.4byte	0x4
.L_D306:
	.4byte	.L_D306_e-.L_D306
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D312
	.set	.L_T825,.L_D306
	.2byte	0xb6
	.4byte	0x4
.L_D306_e:
.L_D313:
	.4byte	.L_D313_e-.L_D313
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D314
	.2byte	0x38
	.string	"addr"
	.2byte	0x142
	.4byte	.L_T825
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l313_e-.L_l313
.L_l313:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l313_e:
.L_D313_e:
.L_D314:
	.4byte	.L_D314_e-.L_D314
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D315
	.2byte	0x38
	.string	"ctl"
	.2byte	0x142
	.4byte	.L_T825
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l314_e-.L_l314
.L_l314:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l314_e:
.L_D314_e:
.L_D315:
	.4byte	0x4
.L_D312:
	.4byte	.L_D312_e-.L_D312
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D316
	.set	.L_T665,.L_D312
	.2byte	0x38
	.string	"qinit"
	.2byte	0xb6
	.4byte	0x1c
.L_D312_e:
.L_D317:
	.4byte	.L_D317_e-.L_D317
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D318
	.2byte	0x38
	.string	"qi_putp"
	.2byte	0x142
	.4byte	.L_T665
	.2byte	0x83
	.2byte	.L_t317_e-.L_t317
.L_t317:
	.byte	0x1
	.4byte	.L_T38
.L_t317_e:
	.2byte	0x23
	.2byte	.L_l317_e-.L_l317
.L_l317:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l317_e:
.L_D317_e:
.L_D318:
	.4byte	.L_D318_e-.L_D318
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D319
	.2byte	0x38
	.string	"qi_srvp"
	.2byte	0x142
	.4byte	.L_T665
	.2byte	0x83
	.2byte	.L_t318_e-.L_t318
.L_t318:
	.byte	0x1
	.4byte	.L_T38
.L_t318_e:
	.2byte	0x23
	.2byte	.L_l318_e-.L_l318
.L_l318:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l318_e:
.L_D318_e:
.L_D319:
	.4byte	.L_D319_e-.L_D319
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D320
	.2byte	0x38
	.string	"qi_qopen"
	.2byte	0x142
	.4byte	.L_T665
	.2byte	0x83
	.2byte	.L_t319_e-.L_t319
.L_t319:
	.byte	0x1
	.4byte	.L_T38
.L_t319_e:
	.2byte	0x23
	.2byte	.L_l319_e-.L_l319
.L_l319:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l319_e:
.L_D319_e:
.L_D320:
	.4byte	.L_D320_e-.L_D320
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D321
	.2byte	0x38
	.string	"qi_qclose"
	.2byte	0x142
	.4byte	.L_T665
	.2byte	0x83
	.2byte	.L_t320_e-.L_t320
.L_t320:
	.byte	0x1
	.4byte	.L_T38
.L_t320_e:
	.2byte	0x23
	.2byte	.L_l320_e-.L_l320
.L_l320:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l320_e:
.L_D320_e:
.L_D321:
	.4byte	.L_D321_e-.L_D321
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D322
	.2byte	0x38
	.string	"qi_qadmin"
	.2byte	0x142
	.4byte	.L_T665
	.2byte	0x83
	.2byte	.L_t321_e-.L_t321
.L_t321:
	.byte	0x1
	.4byte	.L_T38
.L_t321_e:
	.2byte	0x23
	.2byte	.L_l321_e-.L_l321
.L_l321:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l321_e:
.L_D321_e:
.L_D322:
	.4byte	.L_D322_e-.L_D322
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D323
	.2byte	0x38
	.string	"qi_minfo"
	.2byte	0x142
	.4byte	.L_T665
	.2byte	0x83
	.2byte	.L_t322_e-.L_t322
.L_t322:
	.byte	0x1
	.4byte	.L_T680
.L_t322_e:
	.2byte	0x23
	.2byte	.L_l322_e-.L_l322
.L_l322:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l322_e:
.L_D322_e:
.L_D323:
	.4byte	.L_D323_e-.L_D323
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D324
	.2byte	0x38
	.string	"qi_mstat"
	.2byte	0x142
	.4byte	.L_T665
	.2byte	0x83
	.2byte	.L_t323_e-.L_t323
.L_t323:
	.byte	0x1
	.4byte	.L_T687
.L_t323_e:
	.2byte	0x23
	.2byte	.L_l323_e-.L_l323
.L_l323:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l323_e:
.L_D323_e:
.L_D324:
	.4byte	0x4
.L_D316:
	.4byte	.L_D316_e-.L_D316
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D325
	.set	.L_T670,.L_D316
	.2byte	0x38
	.string	"qband"
	.2byte	0xb6
	.4byte	0x20
.L_D316_e:
.L_D326:
	.4byte	.L_D326_e-.L_D326
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D327
	.2byte	0x38
	.string	"qb_next"
	.2byte	0x142
	.4byte	.L_T670
	.2byte	0x83
	.2byte	.L_t326_e-.L_t326
.L_t326:
	.byte	0x1
	.4byte	.L_T670
.L_t326_e:
	.2byte	0x23
	.2byte	.L_l326_e-.L_l326
.L_l326:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l326_e:
.L_D326_e:
.L_D327:
	.4byte	.L_D327_e-.L_D327
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D328
	.2byte	0x38
	.string	"qb_count"
	.2byte	0x142
	.4byte	.L_T670
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l327_e-.L_l327
.L_l327:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l327_e:
.L_D327_e:
.L_D328:
	.4byte	.L_D328_e-.L_D328
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D329
	.2byte	0x38
	.string	"qb_first"
	.2byte	0x142
	.4byte	.L_T670
	.2byte	0x83
	.2byte	.L_t328_e-.L_t328
.L_t328:
	.byte	0x1
	.4byte	.L_T667
.L_t328_e:
	.2byte	0x23
	.2byte	.L_l328_e-.L_l328
.L_l328:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l328_e:
.L_D328_e:
.L_D329:
	.4byte	.L_D329_e-.L_D329
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D330
	.2byte	0x38
	.string	"qb_last"
	.2byte	0x142
	.4byte	.L_T670
	.2byte	0x83
	.2byte	.L_t329_e-.L_t329
.L_t329:
	.byte	0x1
	.4byte	.L_T667
.L_t329_e:
	.2byte	0x23
	.2byte	.L_l329_e-.L_l329
.L_l329:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l329_e:
.L_D329_e:
.L_D330:
	.4byte	.L_D330_e-.L_D330
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D331
	.2byte	0x38
	.string	"qb_hiwat"
	.2byte	0x142
	.4byte	.L_T670
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l330_e-.L_l330
.L_l330:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l330_e:
.L_D330_e:
.L_D331:
	.4byte	.L_D331_e-.L_D331
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D332
	.2byte	0x38
	.string	"qb_lowat"
	.2byte	0x142
	.4byte	.L_T670
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l331_e-.L_l331
.L_l331:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l331_e:
.L_D331_e:
.L_D332:
	.4byte	.L_D332_e-.L_D332
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D333
	.2byte	0x38
	.string	"qb_flag"
	.2byte	0x142
	.4byte	.L_T670
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l332_e-.L_l332
.L_l332:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l332_e:
.L_D332_e:
.L_D333:
	.4byte	.L_D333_e-.L_D333
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D334
	.2byte	0x38
	.string	"qb_pad1"
	.2byte	0x142
	.4byte	.L_T670
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l333_e-.L_l333
.L_l333:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l333_e:
.L_D333_e:
.L_D334:
	.4byte	0x4
.L_D325:
	.4byte	.L_D325_e-.L_D325
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D335
	.set	.L_T673,.L_D325
	.2byte	0xa3
	.2byte	.L_s325_e-.L_s325
.L_s325:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x1
	.byte	0x8
	.2byte	0x55
	.2byte	0x3
.L_s325_e:
.L_D325_e:
.L_D335:
	.4byte	.L_D335_e-.L_D335
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D336
	.set	.L_T139,.L_D335
	.2byte	0xa3
	.2byte	.L_s335_e-.L_s335
.L_s335:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x1
	.byte	0x8
	.2byte	0x55
	.2byte	0xa
.L_s335_e:
.L_D335_e:
.L_D336:
	.4byte	.L_D336_e-.L_D336
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D337
	.set	.L_T695,.L_D336
	.2byte	0x38
	.string	"datab"
	.2byte	0xb6
	.4byte	0x1c
.L_D336_e:
.L_D338:
	.4byte	.L_D338_e-.L_D338
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D339
	.2byte	0x38
	.string	"db_f"
	.2byte	0x142
	.4byte	.L_T695
	.2byte	0x72
	.4byte	.L_T698
	.2byte	0x23
	.2byte	.L_l338_e-.L_l338
.L_l338:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l338_e:
.L_D338_e:
.L_D339:
	.4byte	.L_D339_e-.L_D339
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D340
	.2byte	0x38
	.string	"db_base"
	.2byte	0x142
	.4byte	.L_T695
	.2byte	0x63
	.2byte	.L_t339_e-.L_t339
.L_t339:
	.byte	0x1
	.2byte	0x3
.L_t339_e:
	.2byte	0x23
	.2byte	.L_l339_e-.L_l339
.L_l339:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l339_e:
.L_D339_e:
.L_D340:
	.4byte	.L_D340_e-.L_D340
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D341
	.2byte	0x38
	.string	"db_lim"
	.2byte	0x142
	.4byte	.L_T695
	.2byte	0x63
	.2byte	.L_t340_e-.L_t340
.L_t340:
	.byte	0x1
	.2byte	0x3
.L_t340_e:
	.2byte	0x23
	.2byte	.L_l340_e-.L_l340
.L_l340:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l340_e:
.L_D340_e:
.L_D341:
	.4byte	.L_D341_e-.L_D341
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D342
	.2byte	0x38
	.string	"db_ref"
	.2byte	0x142
	.4byte	.L_T695
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l341_e-.L_l341
.L_l341:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l341_e:
.L_D341_e:
.L_D342:
	.4byte	.L_D342_e-.L_D342
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D343
	.2byte	0x38
	.string	"db_type"
	.2byte	0x142
	.4byte	.L_T695
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l342_e-.L_l342
.L_l342:
	.byte	0x4
	.4byte	0xd
	.byte	0x7
.L_l342_e:
.L_D342_e:
.L_D343:
	.4byte	.L_D343_e-.L_D343
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D344
	.2byte	0x38
	.string	"db_iswhat"
	.2byte	0x142
	.4byte	.L_T695
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l343_e-.L_l343
.L_l343:
	.byte	0x4
	.4byte	0xe
	.byte	0x7
.L_l343_e:
.L_D343_e:
.L_D344:
	.4byte	.L_D344_e-.L_D344
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D345
	.2byte	0x38
	.string	"db_size"
	.2byte	0x142
	.4byte	.L_T695
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l344_e-.L_l344
.L_l344:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l344_e:
.L_D344_e:
.L_D345:
	.4byte	.L_D345_e-.L_D345
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D346
	.2byte	0x38
	.string	"db_msgaddr"
	.2byte	0x142
	.4byte	.L_T695
	.2byte	0x63
	.2byte	.L_t345_e-.L_t345
.L_t345:
	.byte	0x1
	.2byte	0x1
.L_t345_e:
	.2byte	0x23
	.2byte	.L_l345_e-.L_l345
.L_l345:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l345_e:
.L_D345_e:
.L_D346:
	.4byte	.L_D346_e-.L_D346
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D347
	.2byte	0x38
	.string	"db_filler"
	.2byte	0x142
	.4byte	.L_T695
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l346_e-.L_l346
.L_l346:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l346_e:
.L_D346_e:
.L_D347:
	.4byte	0x4
.L_D337:
	.4byte	.L_D337_e-.L_D337
	.2byte	0x15
	.2byte	0x12
	.4byte	.L_D348
	.set	.L_T38,.L_D337
	.2byte	0x55
	.2byte	0x7
.L_D337_e:
.L_D349:
	.4byte	.L_D349_e-.L_D349
	.2byte	0x18
	.2byte	0x12
	.4byte	.L_D350
.L_D349_e:
.L_D350:
	.4byte	0x4
.L_D348:
	.4byte	.L_D348_e-.L_D348
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D351
	.set	.L_T680,.L_D348
	.2byte	0x38
	.string	"module_info"
	.2byte	0xb6
	.4byte	0x18
.L_D348_e:
.L_D352:
	.4byte	.L_D352_e-.L_D352
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D353
	.2byte	0x38
	.string	"mi_idnum"
	.2byte	0x142
	.4byte	.L_T680
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l352_e-.L_l352
.L_l352:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l352_e:
.L_D352_e:
.L_D353:
	.4byte	.L_D353_e-.L_D353
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D354
	.2byte	0x38
	.string	"mi_idname"
	.2byte	0x142
	.4byte	.L_T680
	.2byte	0x63
	.2byte	.L_t353_e-.L_t353
.L_t353:
	.byte	0x1
	.2byte	0x1
.L_t353_e:
	.2byte	0x23
	.2byte	.L_l353_e-.L_l353
.L_l353:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l353_e:
.L_D353_e:
.L_D354:
	.4byte	.L_D354_e-.L_D354
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D355
	.2byte	0x38
	.string	"mi_minpsz"
	.2byte	0x142
	.4byte	.L_T680
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l354_e-.L_l354
.L_l354:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l354_e:
.L_D354_e:
.L_D355:
	.4byte	.L_D355_e-.L_D355
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D356
	.2byte	0x38
	.string	"mi_maxpsz"
	.2byte	0x142
	.4byte	.L_T680
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l355_e-.L_l355
.L_l355:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l355_e:
.L_D355_e:
.L_D356:
	.4byte	.L_D356_e-.L_D356
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D357
	.2byte	0x38
	.string	"mi_hiwat"
	.2byte	0x142
	.4byte	.L_T680
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l356_e-.L_l356
.L_l356:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l356_e:
.L_D356_e:
.L_D357:
	.4byte	.L_D357_e-.L_D357
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D358
	.2byte	0x38
	.string	"mi_lowat"
	.2byte	0x142
	.4byte	.L_T680
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l357_e-.L_l357
.L_l357:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l357_e:
.L_D357_e:
.L_D358:
	.4byte	0x4
.L_D351:
	.4byte	.L_D351_e-.L_D351
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D359
	.set	.L_T698,.L_D351
	.2byte	0xb6
	.4byte	0x4
.L_D351_e:
.L_D360:
	.4byte	.L_D360_e-.L_D360
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D361
	.2byte	0x38
	.string	"freep"
	.2byte	0x142
	.4byte	.L_T698
	.2byte	0x83
	.2byte	.L_t360_e-.L_t360
.L_t360:
	.byte	0x1
	.4byte	.L_T695
.L_t360_e:
	.2byte	0x23
	.2byte	.L_l360_e-.L_l360
.L_l360:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l360_e:
.L_D360_e:
.L_D361:
	.4byte	.L_D361_e-.L_D361
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D362
	.2byte	0x38
	.string	"frtnp"
	.2byte	0x142
	.4byte	.L_T698
	.2byte	0x83
	.2byte	.L_t361_e-.L_t361
.L_t361:
	.byte	0x1
	.4byte	.L_T693
.L_t361_e:
	.2byte	0x23
	.2byte	.L_l361_e-.L_l361
.L_l361:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l361_e:
.L_D361_e:
.L_D362:
	.4byte	0x4
.L_D359:
	.4byte	.L_D359_e-.L_D359
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D363
	.set	.L_T693,.L_D359
	.2byte	0x38
	.string	"free_rtn"
	.2byte	0xb6
	.4byte	0x8
.L_D359_e:
.L_D364:
	.4byte	.L_D364_e-.L_D364
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D365
	.2byte	0x38
	.string	"free_func"
	.2byte	0x142
	.4byte	.L_T693
	.2byte	0x83
	.2byte	.L_t364_e-.L_t364
.L_t364:
	.byte	0x1
	.4byte	.L_T41
.L_t364_e:
	.2byte	0x23
	.2byte	.L_l364_e-.L_l364
.L_l364:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l364_e:
.L_D364_e:
.L_D365:
	.4byte	.L_D365_e-.L_D365
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D366
	.2byte	0x38
	.string	"free_arg"
	.2byte	0x142
	.4byte	.L_T693
	.2byte	0x63
	.2byte	.L_t365_e-.L_t365
.L_t365:
	.byte	0x1
	.2byte	0x1
.L_t365_e:
	.2byte	0x23
	.2byte	.L_l365_e-.L_l365
.L_l365:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l365_e:
.L_D365_e:
.L_D366:
	.4byte	0x4
.L_D363:
	.4byte	.L_D363_e-.L_D363
	.2byte	0x15
	.2byte	0x12
	.4byte	.L_D367
	.set	.L_T41,.L_D363
.L_D363_e:
.L_D368:
	.4byte	.L_D368_e-.L_D368
	.2byte	0x18
	.2byte	0x12
	.4byte	.L_D369
.L_D368_e:
.L_D369:
	.4byte	0x4
	.previous
	.align 4
.globl sas_open
	.type	 sas_open,@function
sas_open:
.L_LC255:

.section	.line
	.4byte	708	/ sas.c:708
	.2byte	0xffff
	.4byte	.L_LC255-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	subl $12,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 16(%ebp),%edi
.L_b6:
.L_LC256:

.section	.line
	.4byte	709	/ sas.c:709
	.2byte	0xffff
	.4byte	.L_LC256-.L_text_b
	.previous
.L_LC257:

.section	.line
	.4byte	716	/ sas.c:716
	.2byte	0xffff
	.4byte	.L_LC257-.L_text_b
	.previous
	cmpl $0,20(%ebp)
	je .L115
	movb $22,u+4329
	movl $-1,%eax
	jmp .L160
	.align 4
.L115:
.L_LC258:

.section	.line
	.4byte	718	/ sas.c:718
	.2byte	0xffff
	.4byte	.L_LC258-.L_text_b
	.previous
	movl 12(%ebp),%eax
	andl $15,%eax
.L_LC259:

.section	.line
	.4byte	721	/ sas.c:721
	.2byte	0xffff
	.4byte	.L_LC259-.L_text_b
	.previous
	cmpl %eax,sas_physical_units
	jbe .L161
.L_LC260:

.section	.line
	.4byte	723	/ sas.c:723
	.2byte	0xffff
	.4byte	.L_LC260-.L_text_b
	.previous
	movl sas_info_ptr(,%eax,4),%ebx
.L_LC261:

.section	.line
	.4byte	726	/ sas.c:726
	.2byte	0xffff
	.4byte	.L_LC261-.L_text_b
	.previous
	testb $1,32(%ebx)
	jne .L117
.L161:
	movb $6,u+4329
	movl $-1,%eax
	jmp .L160
	.align 4
.L117:
.L_LC262:

.section	.line
	.4byte	728	/ sas.c:728
	.2byte	0xffff
	.4byte	.L_LC262-.L_text_b
	.previous
	movl 12(%ebp),%eax
	sarl $4,%eax
	andl $15,%eax
	movl sas_open_modes(,%eax,4),%eax
	movl %eax,-4(%ebp)
.L_LC263:

.section	.line
	.4byte	729	/ sas.c:729
	.2byte	0xffff
	.4byte	.L_LC263-.L_text_b
	.previous
	xorl %esi,%esi
.L_LC264:

.section	.line
	.4byte	730	/ sas.c:730
	.2byte	0xffff
	.4byte	.L_LC264-.L_text_b
	.previous
	call spltty
	movl %eax,-8(%ebp)
.L_LC265:

.section	.line
	.4byte	736	/ sas.c:736
	.2byte	0xffff
	.4byte	.L_LC265-.L_text_b
	.previous
	leal 32(%ebx),%edx
	movl %edx,-12(%ebp)
.L_LC266:

.section	.line
	.4byte	747	/ sas.c:747
	.2byte	0xffff
	.4byte	.L_LC266-.L_text_b
	.previous
	jmp .L162
	.align 4
	.align 4
.L123:
.L_LC267:

.section	.line
	.4byte	749	/ sas.c:749
	.2byte	0xffff
	.4byte	.L_LC267-.L_text_b
	.previous
	testl %esi,%esi
	je .L124
.L_LC268:

.section	.line
	.4byte	751	/ sas.c:751
	.2byte	0xffff
	.4byte	.L_LC268-.L_text_b
	.previous
	andb $239,32(%ebx)
	pushl -12(%ebp)
	call wakeup
.L_LC269:

.section	.line
	.4byte	752	/ sas.c:752
	.2byte	0xffff
	.4byte	.L_LC269-.L_text_b
	.previous
	xorl %esi,%esi
.L_LC270:

.section	.line
	.4byte	753	/ sas.c:753
	.2byte	0xffff
	.4byte	.L_LC270-.L_text_b
	.previous
	addl $4,%esp
.L124:
.L_LC271:

.section	.line
	.4byte	754	/ sas.c:754
	.2byte	0xffff
	.4byte	.L_LC271-.L_text_b
	.previous
	pushl $28
	leal 44(%ebx),%eax
	pushl %eax
	call sleep
.L_LC272:

.section	.line
	.4byte	755	/ sas.c:755
	.2byte	0xffff
	.4byte	.L_LC272-.L_text_b
	.previous
	addl $8,%esp
.L_LC273:

.section	.line
	.4byte	747	/ sas.c:747
	.2byte	0xffff
	.4byte	.L_LC273-.L_text_b
	.previous
.L162:
	testb $2,-4(%ebp)
	je .L122
	testl $4,%edi
	jne .L122
	testb $1,44(%ebx)
	jne .L123
	testb $2,44(%ebx)
	je .L122
	movl 112(%ebx),%eax
	testb $16,(%eax)
	je .L123
.L122:
.L_LC274:

.section	.line
	.4byte	762	/ sas.c:762
	.2byte	0xffff
	.4byte	.L_LC274-.L_text_b
	.previous
	movl 44(%ebx),%eax
	testb $2,-4(%ebp)
	je .L127
	testb $7,%al
	jne .L128
	jmp .L126
	.align 4
.L127:
	testb $11,%al
	je .L126
.L128:
	testl $1024,%edi
	jne .L129
	movl -4(%ebp),%eax
	xorl 44(%ebx),%eax
	cmpw $0,u+4330
	je .L130
	testw $2041,%ax
	jne .L129
	jmp .L131
	.align 4
.L130:
	testw $1017,%ax
	jne .L129
.L131:
	testl $4,%edi
	je .L126
	movl 112(%ebx),%eax
	testb $2,(%eax)
	je .L126
.L129:
.L_LC275:

.section	.line
	.4byte	772	/ sas.c:772
	.2byte	0xffff
	.4byte	.L_LC275-.L_text_b
	.previous
	testl %esi,%esi
	je .L132
.L_LC276:

.section	.line
	.4byte	773	/ sas.c:773
	.2byte	0xffff
	.4byte	.L_LC276-.L_text_b
	.previous
	andb $239,32(%ebx)
	pushl -12(%ebp)
	call wakeup
	addl $4,%esp
.L132:
.L_LC277:

.section	.line
	.4byte	774	/ sas.c:774
	.2byte	0xffff
	.4byte	.L_LC277-.L_text_b
	.previous
	pushl -8(%ebp)
	call splx
.L_LC278:

.section	.line
	.4byte	775	/ sas.c:775
	.2byte	0xffff
	.4byte	.L_LC278-.L_text_b
	.previous
	movb $16,u+4329
	movl $-1,%eax
	jmp .L160
	.align 4
.L126:
.L_LC279:

.section	.line
	.4byte	783	/ sas.c:783
	.2byte	0xffff
	.4byte	.L_LC279-.L_text_b
	.previous
	testb $3,44(%ebx)
	je .L133
	testl $512,%edi
	je .L133
.L_LC280:

.section	.line
	.4byte	785	/ sas.c:785
	.2byte	0xffff
	.4byte	.L_LC280-.L_text_b
	.previous
	andl $-513,%edi
.L_LC281:

.section	.line
	.4byte	786	/ sas.c:786
	.2byte	0xffff
	.4byte	.L_LC281-.L_text_b
	.previous
	call spl6
.L_LC282:

.section	.line
	.4byte	787	/ sas.c:787
	.2byte	0xffff
	.4byte	.L_LC282-.L_text_b
	.previous
	pushl 8(%ebp)
	call flushq
.L_LC283:

.section	.line
	.4byte	788	/ sas.c:788
	.2byte	0xffff
	.4byte	.L_LC283-.L_text_b
	.previous
	movl 8(%ebp),%eax
	testb $16,28(%eax)
	je .L134
	addl $64,%eax
.L134:
	pushl %eax
	call flushq
.L_LC284:

.section	.line
	.4byte	789	/ sas.c:789
	.2byte	0xffff
	.4byte	.L_LC284-.L_text_b
	.previous
	call spltty
.L_LC285:

.section	.line
	.4byte	790	/ sas.c:790
	.2byte	0xffff
	.4byte	.L_LC285-.L_text_b
	.previous
	addl $8,%esp
.L133:
.L_LC286:

.section	.line
	.4byte	793	/ sas.c:793
	.2byte	0xffff
	.4byte	.L_LC286-.L_text_b
	.previous
	testl %esi,%esi
	jne .L119
.L_LC287:

.section	.line
	.4byte	795	/ sas.c:795
	.2byte	0xffff
	.4byte	.L_LC287-.L_text_b
	.previous
	testb $16,32(%ebx)
	je .L142
.L_LC288:

.section	.line
	.4byte	797	/ sas.c:797
	.2byte	0xffff
	.4byte	.L_LC288-.L_text_b
	.previous
	.align 4
.L139:
	pushl $28
	pushl -12(%ebp)
	call sleep
	addl $8,%esp
	testb $16,32(%ebx)
	jne .L139
	orb $16,32(%ebx)
.L_LC289:

.section	.line
	.4byte	798	/ sas.c:798
	.2byte	0xffff
	.4byte	.L_LC289-.L_text_b
	.previous
	movl $1,%esi
.L_LC290:

.section	.line
	.4byte	804	/ sas.c:804
	.2byte	0xffff
	.4byte	.L_LC290-.L_text_b
	.previous
	jmp .L162
	.align 4
.L_LC291:

.section	.line
	.4byte	807	/ sas.c:807
	.2byte	0xffff
	.4byte	.L_LC291-.L_text_b
	.previous
	.align 4
.L142:
	orb $16,32(%ebx)
.L_LC292:

.section	.line
	.4byte	736	/ sas.c:736
	.2byte	0xffff
	.4byte	.L_LC292-.L_text_b
	.previous
.L119:
.L_LC293:

.section	.line
	.4byte	814	/ sas.c:814
	.2byte	0xffff
	.4byte	.L_LC293-.L_text_b
	.previous
	testl $1024,%edi
	je .L144
.L_LC294:

.section	.line
	.4byte	815	/ sas.c:815
	.2byte	0xffff
	.4byte	.L_LC294-.L_text_b
	.previous
	orl $512,-4(%ebp)
.L144:
.L_LC295:

.section	.line
	.4byte	819	/ sas.c:819
	.2byte	0xffff
	.4byte	.L_LC295-.L_text_b
	.previous
	movl 44(%ebx),%eax
	movl -4(%ebp),%ecx
	testb $2,%cl
	je .L146
	testb $7,%al
	jne .L145
	jmp .L147
	.align 4
.L146:
	testb $3,%al
	jne .L145
.L147:
.L_LC296:

.section	.line
	.4byte	823	/ sas.c:823
	.2byte	0xffff
	.4byte	.L_LC296-.L_text_b
	.previous
	movl -4(%ebp),%edx
	testb $2,%dl
	je .L148
.L_LC297:

.section	.line
	.4byte	824	/ sas.c:824
	.2byte	0xffff
	.4byte	.L_LC297-.L_text_b
	.previous
	leal 128(%ebx),%esi
.L_LC298:

.section	.line
	.4byte	825	/ sas.c:825
	.2byte	0xffff
	.4byte	.L_LC298-.L_text_b
	.previous
	movl 8(%ebp),%ecx
	movl %ecx,136(%ebx)
.L_LC299:

.section	.line
	.4byte	826	/ sas.c:826
	.2byte	0xffff
	.4byte	.L_LC299-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl %edx,124(%ebx)
.L_LC300:

.section	.line
	.4byte	827	/ sas.c:827
	.2byte	0xffff
	.4byte	.L_LC300-.L_text_b
	.previous
	leal 124(%ebx),%edx
.L_LC301:

.section	.line
	.4byte	828	/ sas.c:828
	.2byte	0xffff
	.4byte	.L_LC301-.L_text_b
	.previous
	jmp .L163
	.align 4
.L148:
.L_LC302:

.section	.line
	.4byte	829	/ sas.c:829
	.2byte	0xffff
	.4byte	.L_LC302-.L_text_b
	.previous
	leal 144(%ebx),%esi
.L_LC303:

.section	.line
	.4byte	830	/ sas.c:830
	.2byte	0xffff
	.4byte	.L_LC303-.L_text_b
	.previous
	movl 8(%ebp),%ecx
	movl %ecx,152(%ebx)
.L_LC304:

.section	.line
	.4byte	831	/ sas.c:831
	.2byte	0xffff
	.4byte	.L_LC304-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl %edx,140(%ebx)
.L_LC305:

.section	.line
	.4byte	832	/ sas.c:832
	.2byte	0xffff
	.4byte	.L_LC305-.L_text_b
	.previous
	leal 140(%ebx),%edx
.L163:
	movl %edx,20(%ecx)
.L_LC306:

.section	.line
	.4byte	834	/ sas.c:834
	.2byte	0xffff
	.4byte	.L_LC306-.L_text_b
	.previous
	movl 8(%ebp),%eax
	testb $16,28(%eax)
	je .L150
	addl $64,%eax
.L150:
	movl 8(%ebp),%ecx
	movl 20(%ecx),%ecx
	movl %ecx,20(%eax)
.L_LC307:

.section	.line
	.4byte	836	/ sas.c:836
	.2byte	0xffff
	.4byte	.L_LC307-.L_text_b
	.previous
	movl $0,176(%ebx)
.L_LC308:

.section	.line
	.4byte	837	/ sas.c:837
	.2byte	0xffff
	.4byte	.L_LC308-.L_text_b
	.previous
	movl $0,104(%ebx)
.L_LC309:

.section	.line
	.4byte	839	/ sas.c:839
	.2byte	0xffff
	.4byte	.L_LC309-.L_text_b
	.previous
	movl %esi,112(%ebx)
.L_LC310:

.section	.line
	.4byte	840	/ sas.c:840
	.2byte	0xffff
	.4byte	.L_LC310-.L_text_b
	.previous
	movl 8(%ebp),%edx
	movl %edx,156(%ebx)
.L_LC311:

.section	.line
	.4byte	841	/ sas.c:841
	.2byte	0xffff
	.4byte	.L_LC311-.L_text_b
	.previous
	movl 44(%ebx),%ecx
	movl %ecx,48(%ebx)
.L_LC312:

.section	.line
	.4byte	842	/ sas.c:842
	.2byte	0xffff
	.4byte	.L_LC312-.L_text_b
	.previous
	movl -4(%ebp),%edx
	andb $252,%dl
	movl %edx,44(%ebx)
.L_LC313:

.section	.line
	.4byte	850	/ sas.c:850
	.2byte	0xffff
	.4byte	.L_LC313-.L_text_b
	.previous
	pushl %ebx
	call sas_open_device
.L_LC314:

.section	.line
	.4byte	851	/ sas.c:851
	.2byte	0xffff
	.4byte	.L_LC314-.L_text_b
	.previous
	pushl $1207
	pushl $1
	pushl %ebx
	call sas_param
.L_LC315:

.section	.line
	.4byte	854	/ sas.c:854
	.2byte	0xffff
	.4byte	.L_LC315-.L_text_b
	.previous
	call spl6
.L_LC316:

.section	.line
	.4byte	855	/ sas.c:855
	.2byte	0xffff
	.4byte	.L_LC316-.L_text_b
	.previous
	call spltty
.L_LC317:

.section	.line
	.4byte	856	/ sas.c:856
	.2byte	0xffff
	.4byte	.L_LC317-.L_text_b
	.previous
	addl $16,%esp
.L145:
.L_LC318:

.section	.line
	.4byte	861	/ sas.c:861
	.2byte	0xffff
	.4byte	.L_LC318-.L_text_b
	.previous
	movl -4(%ebp),%ecx
	testb $2,%cl
	je .L151
	testl $4,%edi
	jne .L151
.L_LC319:

.section	.line
	.4byte	865	/ sas.c:865
	.2byte	0xffff
	.4byte	.L_LC319-.L_text_b
	.previous
	jmp .L164
	.align 4
	.align 4
.L154:
.L_LC320:

.section	.line
	.4byte	867	/ sas.c:867
	.2byte	0xffff
	.4byte	.L_LC320-.L_text_b
	.previous
	movl 112(%ebx),%eax
	orb $2,(%eax)
.L_LC321:

.section	.line
	.4byte	868	/ sas.c:868
	.2byte	0xffff
	.4byte	.L_LC321-.L_text_b
	.previous
	andb $239,32(%ebx)
	leal 32(%ebx),%esi
	pushl %esi
	call wakeup
.L_LC322:

.section	.line
	.4byte	869	/ sas.c:869
	.2byte	0xffff
	.4byte	.L_LC322-.L_text_b
	.previous
	pushl $28
	pushl 112(%ebx)
	call sleep
.L_LC323:

.section	.line
	.4byte	870	/ sas.c:870
	.2byte	0xffff
	.4byte	.L_LC323-.L_text_b
	.previous
	addl $12,%esp
	testb $16,32(%ebx)
	je .L156
	.align 4
.L157:
	pushl $28
	pushl %esi
	call sleep
	addl $8,%esp
	testb $16,32(%ebx)
	jne .L157
.L156:
	orb $16,32(%ebx)
.L_LC324:

.section	.line
	.4byte	865	/ sas.c:865
	.2byte	0xffff
	.4byte	.L_LC324-.L_text_b
	.previous
.L164:
	testb $1,44(%ebx)
	jne .L154
	movl 112(%ebx),%eax
	testb $16,(%eax)
	je .L154
.L_LC325:

.section	.line
	.4byte	872	/ sas.c:872
	.2byte	0xffff
	.4byte	.L_LC325-.L_text_b
	.previous
	movl 112(%ebx),%eax
	andl $-3,(%eax)
.L151:
.L_LC326:

.section	.line
	.4byte	876	/ sas.c:876
	.2byte	0xffff
	.4byte	.L_LC326-.L_text_b
	.previous
	testb $3,44(%ebx)
	jne .L159
	pushl $6
	pushl %ebx
	call sas_cmd
	addl $8,%esp
.L159:
.L_LC327:

.section	.line
	.4byte	879	/ sas.c:879
	.2byte	0xffff
	.4byte	.L_LC327-.L_text_b
	.previous
	movl -4(%ebp),%edx
	movl %edx,44(%ebx)
.L_LC328:

.section	.line
	.4byte	880	/ sas.c:880
	.2byte	0xffff
	.4byte	.L_LC328-.L_text_b
	.previous
	movl 112(%ebx),%eax
	orb $4,(%eax)
.L_LC329:

.section	.line
	.4byte	882	/ sas.c:882
	.2byte	0xffff
	.4byte	.L_LC329-.L_text_b
	.previous
	andb $239,32(%ebx)
	leal 32(%ebx),%eax
	pushl %eax
	call wakeup
.L_LC330:

.section	.line
	.4byte	883	/ sas.c:883
	.2byte	0xffff
	.4byte	.L_LC330-.L_text_b
	.previous
	pushl -8(%ebp)
	call splx
.L_LC331:

.section	.line
	.4byte	884	/ sas.c:884
	.2byte	0xffff
	.4byte	.L_LC331-.L_text_b
	.previous
	xorl %eax,%eax
.L160:
.L_LC332:

.section	.line
	.4byte	885	/ sas.c:885
	.2byte	0xffff
	.4byte	.L_LC332-.L_text_b
	.previous
.L_b6_e:
	leal -24(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
.L_f6_e:
.Lfe3:
	.size	 sas_open,.Lfe3-sas_open

.section	.debug_pubnames
	.4byte	.L_P3
	.string	"sas_open"
	.previous

.section	.debug
.L_P3:
.L_D367:
	.4byte	.L_D367_e-.L_D367
	.2byte	0x6
	.2byte	0x12
	.4byte	.L_D370
	.2byte	0x38
	.string	"sas_open"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x111
	.4byte	sas_open
	.2byte	0x121
	.4byte	.L_f6_e
	.2byte	0x8041
	.4byte	.L_b6
	.2byte	0x8051
	.4byte	.L_b6_e
.L_D367_e:
.L_D371:
	.4byte	.L_D371_e-.L_D371
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D372
	.2byte	0x38
	.string	"rq"
	.2byte	0x83
	.2byte	.L_t371_e-.L_t371
.L_t371:
	.byte	0x1
	.4byte	.L_T664
.L_t371_e:
	.2byte	0x23
	.2byte	.L_l371_e-.L_l371
.L_l371:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l371_e:
.L_D371_e:
.L_D372:
	.4byte	.L_D372_e-.L_D372
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D373
	.2byte	0x38
	.string	"dev"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l372_e-.L_l372
.L_l372:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l372_e:
.L_D372_e:
.L_D373:
	.4byte	.L_D373_e-.L_D373
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D374
	.2byte	0x38
	.string	"flag"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l373_e-.L_l373
.L_l373:
	.byte	0x1
	.4byte	0x7
.L_l373_e:
.L_D373_e:
.L_D374:
	.4byte	.L_D374_e-.L_D374
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D375
	.2byte	0x38
	.string	"sflag"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l374_e-.L_l374
.L_l374:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l374_e:
.L_D374_e:
.L_D375:
	.4byte	.L_D375_e-.L_D375
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D376
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t375_e-.L_t375
.L_t375:
	.byte	0x1
	.4byte	.L_T816
.L_t375_e:
	.2byte	0x23
	.2byte	.L_l375_e-.L_l375
.L_l375:
	.byte	0x1
	.4byte	0x3
.L_l375_e:
.L_D375_e:
.L_D376:
	.4byte	.L_D376_e-.L_D376
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D377
	.2byte	0x38
	.string	"ttyp"
	.2byte	0x63
	.2byte	.L_t376_e-.L_t376
.L_t376:
	.byte	0x1
	.2byte	0x9
.L_t376_e:
	.2byte	0x23
	.2byte	.L_l376_e-.L_l376
.L_l376:
	.byte	0x1
	.4byte	0x6
.L_l376_e:
.L_D376_e:
.L_D377:
	.4byte	.L_D377_e-.L_D377
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D378
	.2byte	0x38
	.string	"open_mode"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l377_e-.L_l377
.L_l377:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xfffffffc
	.byte	0x7
.L_l377_e:
.L_D377_e:
.L_D378:
	.4byte	.L_D378_e-.L_D378
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D379
	.2byte	0x38
	.string	"physical_unit"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l378_e-.L_l378
.L_l378:
	.byte	0x1
	.4byte	0x0
.L_l378_e:
.L_D378_e:
.L_D379:
	.4byte	.L_D379_e-.L_D379
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D380
	.2byte	0x38
	.string	"have_lock"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l379_e-.L_l379
.L_l379:
	.byte	0x1
	.4byte	0x6
.L_l379_e:
.L_D379_e:
.L_D380:
	.4byte	.L_D380_e-.L_D380
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D381
	.2byte	0x38
	.string	"old_level"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l380_e-.L_l380
.L_l380:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xfffffff8
	.byte	0x7
.L_l380_e:
.L_D380_e:
.L_D381:
	.4byte	0x4
	.previous
	.align 4
.globl sas_close
	.type	 sas_close,@function
sas_close:
.L_LC333:

.section	.line
	.4byte	895	/ sas.c:895
	.2byte	0xffff
	.4byte	.L_LC333-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	subl $12,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
.L_b7:
.L_LC334:

.section	.line
	.4byte	896	/ sas.c:896
	.2byte	0xffff
	.4byte	.L_LC334-.L_text_b
	.previous
.L_LC335:

.section	.line
	.4byte	903	/ sas.c:903
	.2byte	0xffff
	.4byte	.L_LC335-.L_text_b
	.previous
	cmpl $0,8(%ebp)
	je .L165
.L_LC336:

.section	.line
	.4byte	905	/ sas.c:905
	.2byte	0xffff
	.4byte	.L_LC336-.L_text_b
	.previous
	movl 8(%ebp),%ecx
	movl 20(%ecx),%ecx
	movl %ecx,-4(%ebp)
.L_LC337:

.section	.line
	.4byte	906	/ sas.c:906
	.2byte	0xffff
	.4byte	.L_LC337-.L_text_b
	.previous
	testl %ecx,%ecx
	je .L165
.L_LC338:

.section	.line
	.4byte	908	/ sas.c:908
	.2byte	0xffff
	.4byte	.L_LC338-.L_text_b
	.previous
	movl 8(%ecx),%ebx
.L_LC339:

.section	.line
	.4byte	910	/ sas.c:910
	.2byte	0xffff
	.4byte	.L_LC339-.L_text_b
	.previous
	cmpl $0,116(%ebx)
	je .L168
.L_LC340:

.section	.line
	.4byte	911	/ sas.c:911
	.2byte	0xffff
	.4byte	.L_LC340-.L_text_b
	.previous
	pushl 116(%ebx)
	call untimeout
.L_LC341:

.section	.line
	.4byte	912	/ sas.c:912
	.2byte	0xffff
	.4byte	.L_LC341-.L_text_b
	.previous
	movl $0,116(%ebx)
.L_LC342:

.section	.line
	.4byte	913	/ sas.c:913
	.2byte	0xffff
	.4byte	.L_LC342-.L_text_b
	.previous
	addl $4,%esp
.L168:
.L_LC343:

.section	.line
	.4byte	914	/ sas.c:914
	.2byte	0xffff
	.4byte	.L_LC343-.L_text_b
	.previous
	movl -4(%ebp),%ecx
	movl (%ecx),%eax
	shrl $4,%eax
	andl $15,%eax
	movl sas_open_modes(,%eax,4),%eax
	movl %eax,-8(%ebp)
.L_LC344:

.section	.line
	.4byte	917	/ sas.c:917
	.2byte	0xffff
	.4byte	.L_LC344-.L_text_b
	.previous
	movl %ecx,%esi
	addl $4,%esi
.L_LC345:

.section	.line
	.4byte	919	/ sas.c:919
	.2byte	0xffff
	.4byte	.L_LC345-.L_text_b
	.previous
	call spltty
	movl %eax,-12(%ebp)
.L_LC346:

.section	.line
	.4byte	920	/ sas.c:920
	.2byte	0xffff
	.4byte	.L_LC346-.L_text_b
	.previous
	testb $16,32(%ebx)
	je .L170
	leal 32(%ebx),%edi
	.align 4
.L171:
	pushl $28
	pushl %edi
	call sleep
	addl $8,%esp
	testb $16,32(%ebx)
	jne .L171
.L170:
	orb $16,32(%ebx)
.L_LC347:

.section	.line
	.4byte	923	/ sas.c:923
	.2byte	0xffff
	.4byte	.L_LC347-.L_text_b
	.previous
	testb $4,(%esi)
	je .L181
.L_LC348:

.section	.line
	.4byte	926	/ sas.c:926
	.2byte	0xffff
	.4byte	.L_LC348-.L_text_b
	.previous
	.align 4
.L173:
	movl 8(%ebp),%eax
	testb $16,28(%eax)
	je .L176
	addl $64,%eax
.L176:
	pushl %eax
	call qsize
	addl $4,%esp
	testl %eax,%eax
	jne .L175
	testb $33,(%esi)
	je .L174
.L175:
.L_LC349:

.section	.line
	.4byte	928	/ sas.c:928
	.2byte	0xffff
	.4byte	.L_LC349-.L_text_b
	.previous
	orl $8192,(%esi)
.L_LC350:

.section	.line
	.4byte	929	/ sas.c:929
	.2byte	0xffff
	.4byte	.L_LC350-.L_text_b
	.previous
	pushl $285
	pushl %esi
	call sleep
	addl $8,%esp
	testl %eax,%eax
	je .L173
.L_LC351:

.section	.line
	.4byte	932	/ sas.c:932
	.2byte	0xffff
	.4byte	.L_LC351-.L_text_b
	.previous
	andl $-8193,(%esi)
.L174:
.L_LC352:

.section	.line
	.4byte	939	/ sas.c:939
	.2byte	0xffff
	.4byte	.L_LC352-.L_text_b
	.previous
	orw $256,32(%ebx)
.L_LC353:

.section	.line
	.4byte	942	/ sas.c:942
	.2byte	0xffff
	.4byte	.L_LC353-.L_text_b
	.previous
	testw $2240,32(%ebx)
	je .L181
	leal 32(%ebx),%edi
	.align 4
.L180:
.L_LC354:

.section	.line
	.4byte	943	/ sas.c:943
	.2byte	0xffff
	.4byte	.L_LC354-.L_text_b
	.previous
	pushl $24
	pushl %edi
	call sleep
	addl $8,%esp
.L_LC355:

.section	.line
	.4byte	942	/ sas.c:942
	.2byte	0xffff
	.4byte	.L_LC355-.L_text_b
	.previous
	testw $2240,32(%ebx)
	jne .L180
.L_I7_177:
.L181:
.L_LC356:

.section	.line
	.4byte	949	/ sas.c:949
	.2byte	0xffff
	.4byte	.L_LC356-.L_text_b
	.previous
	call spl6
.L_LC357:

.section	.line
	.4byte	950	/ sas.c:950
	.2byte	0xffff
	.4byte	.L_LC357-.L_text_b
	.previous
	call spltty
.L_LC358:

.section	.line
	.4byte	952	/ sas.c:952
	.2byte	0xffff
	.4byte	.L_LC358-.L_text_b
	.previous
	movl -4(%ebp),%ecx
	movl $0,12(%ecx)
.L_LC359:

.section	.line
	.4byte	954	/ sas.c:954
	.2byte	0xffff
	.4byte	.L_LC359-.L_text_b
	.previous
	movl -8(%ebp),%ecx
	testb $2,%cl
	je .L182
.L_LC360:

.section	.line
	.4byte	957	/ sas.c:957
	.2byte	0xffff
	.4byte	.L_LC360-.L_text_b
	.previous
	andl $-3,(%esi)
.L_LC361:

.section	.line
	.4byte	958	/ sas.c:958
	.2byte	0xffff
	.4byte	.L_LC361-.L_text_b
	.previous
	testb $1,44(%ebx)
	jne .L183
.L_LC362:

.section	.line
	.4byte	960	/ sas.c:960
	.2byte	0xffff
	.4byte	.L_LC362-.L_text_b
	.previous
	pushl %ebx
	call sas_close_device
.L_LC363:

.section	.line
	.4byte	961	/ sas.c:961
	.2byte	0xffff
	.4byte	.L_LC363-.L_text_b
	.previous
	movl $0,44(%ebx)
.L_LC364:

.section	.line
	.4byte	962	/ sas.c:962
	.2byte	0xffff
	.4byte	.L_LC364-.L_text_b
	.previous
	addl $4,%esp
	jmp .L185
	.align 4
.L183:
.L_LC365:

.section	.line
	.4byte	964	/ sas.c:964
	.2byte	0xffff
	.4byte	.L_LC365-.L_text_b
	.previous
	movl $0,48(%ebx)
.L_LC366:

.section	.line
	.4byte	965	/ sas.c:965
	.2byte	0xffff
	.4byte	.L_LC366-.L_text_b
	.previous
	jmp .L185
	.align 4
.L182:
.L_LC367:

.section	.line
	.4byte	968	/ sas.c:968
	.2byte	0xffff
	.4byte	.L_LC367-.L_text_b
	.previous
	pushl %ebx
	call sas_close_device
.L_LC368:

.section	.line
	.4byte	969	/ sas.c:969
	.2byte	0xffff
	.4byte	.L_LC368-.L_text_b
	.previous
	movl $0,44(%ebx)
.L_LC369:

.section	.line
	.4byte	973	/ sas.c:973
	.2byte	0xffff
	.4byte	.L_LC369-.L_text_b
	.previous
	addl $4,%esp
	testb $4,48(%ebx)
	je .L185
.L_LC370:

.section	.line
	.4byte	978	/ sas.c:978
	.2byte	0xffff
	.4byte	.L_LC370-.L_text_b
	.previous
	movl -4(%ebp),%eax
	addl $4,%eax
	leal 128(%ebx),%edx
	cmpl %edx,%eax
	jne .L187
.L_LC371:

.section	.line
	.4byte	979	/ sas.c:979
	.2byte	0xffff
	.4byte	.L_LC371-.L_text_b
	.previous
	leal 144(%ebx),%ecx
	movl %ecx,112(%ebx)
.L_LC372:

.section	.line
	.4byte	980	/ sas.c:980
	.2byte	0xffff
	.4byte	.L_LC372-.L_text_b
	.previous
	movl 152(%ebx),%ecx
.L_LC373:

.section	.line
	.4byte	981	/ sas.c:981
	.2byte	0xffff
	.4byte	.L_LC373-.L_text_b
	.previous
	jmp .L197
	.align 4
.L187:
.L_LC374:

.section	.line
	.4byte	982	/ sas.c:982
	.2byte	0xffff
	.4byte	.L_LC374-.L_text_b
	.previous
	movl %edx,112(%ebx)
.L_LC375:

.section	.line
	.4byte	983	/ sas.c:983
	.2byte	0xffff
	.4byte	.L_LC375-.L_text_b
	.previous
	movl 136(%ebx),%ecx
.L197:
	movl %ecx,156(%ebx)
.L_LC376:

.section	.line
	.4byte	985	/ sas.c:985
	.2byte	0xffff
	.4byte	.L_LC376-.L_text_b
	.previous
	movl 48(%ebx),%ecx
	movl %ecx,44(%ebx)
.L_LC377:

.section	.line
	.4byte	986	/ sas.c:986
	.2byte	0xffff
	.4byte	.L_LC377-.L_text_b
	.previous
	movl $0,48(%ebx)
.L_LC378:

.section	.line
	.4byte	994	/ sas.c:994
	.2byte	0xffff
	.4byte	.L_LC378-.L_text_b
	.previous
	testb $2,33(%ebx)
	jne .L185
.L_LC379:

.section	.line
	.4byte	996	/ sas.c:996
	.2byte	0xffff
	.4byte	.L_LC379-.L_text_b
	.previous
	pushl %ebx
	call sas_open_device
.L_LC380:

.section	.line
	.4byte	998	/ sas.c:998
	.2byte	0xffff
	.4byte	.L_LC380-.L_text_b
	.previous
	pushl $1207
	pushl $1
	pushl %ebx
	call sas_param
.L_LC381:

.section	.line
	.4byte	999	/ sas.c:999
	.2byte	0xffff
	.4byte	.L_LC381-.L_text_b
	.previous
	addl $16,%esp
.L185:
.L_LC382:

.section	.line
	.4byte	1003	/ sas.c:1003
	.2byte	0xffff
	.4byte	.L_LC382-.L_text_b
	.previous
	cmpl $0,44(%ebx)
	jne .L190
.L_LC383:

.section	.line
	.4byte	1004	/ sas.c:1004
	.2byte	0xffff
	.4byte	.L_LC383-.L_text_b
	.previous
	cmpl $0,172(%ebx)
	je .L191
.L_LC384:

.section	.line
	.4byte	1005	/ sas.c:1005
	.2byte	0xffff
	.4byte	.L_LC384-.L_text_b
	.previous
	pushl 172(%ebx)
	call freemsg
.L_LC385:

.section	.line
	.4byte	1006	/ sas.c:1006
	.2byte	0xffff
	.4byte	.L_LC385-.L_text_b
	.previous
	movl $0,172(%ebx)
.L_LC386:

.section	.line
	.4byte	1007	/ sas.c:1007
	.2byte	0xffff
	.4byte	.L_LC386-.L_text_b
	.previous
	addl $4,%esp
.L191:
.L_LC387:

.section	.line
	.4byte	1008	/ sas.c:1008
	.2byte	0xffff
	.4byte	.L_LC387-.L_text_b
	.previous
	cmpl $0,164(%ebx)
	je .L192
.L_LC388:

.section	.line
	.4byte	1009	/ sas.c:1009
	.2byte	0xffff
	.4byte	.L_LC388-.L_text_b
	.previous
	pushl 164(%ebx)
	call freemsg
.L_LC389:

.section	.line
	.4byte	1010	/ sas.c:1010
	.2byte	0xffff
	.4byte	.L_LC389-.L_text_b
	.previous
	movl $0,164(%ebx)
.L_LC390:

.section	.line
	.4byte	1011	/ sas.c:1011
	.2byte	0xffff
	.4byte	.L_LC390-.L_text_b
	.previous
	movl $0,168(%ebx)
.L_LC391:

.section	.line
	.4byte	1012	/ sas.c:1012
	.2byte	0xffff
	.4byte	.L_LC391-.L_text_b
	.previous
	addl $4,%esp
.L192:
.L_LC392:

.section	.line
	.4byte	1013	/ sas.c:1013
	.2byte	0xffff
	.4byte	.L_LC392-.L_text_b
	.previous
	cmpl $0,176(%ebx)
	je .L193
.L_LC393:

.section	.line
	.4byte	1014	/ sas.c:1014
	.2byte	0xffff
	.4byte	.L_LC393-.L_text_b
	.previous
	pushl 176(%ebx)
	call freemsg
.L_LC394:

.section	.line
	.4byte	1015	/ sas.c:1015
	.2byte	0xffff
	.4byte	.L_LC394-.L_text_b
	.previous
	movl $0,176(%ebx)
.L_LC395:

.section	.line
	.4byte	1016	/ sas.c:1016
	.2byte	0xffff
	.4byte	.L_LC395-.L_text_b
	.previous
	movl $0,104(%ebx)
.L_LC396:

.section	.line
	.4byte	1017	/ sas.c:1017
	.2byte	0xffff
	.4byte	.L_LC396-.L_text_b
	.previous
	addl $4,%esp
.L193:
.L_LC397:

.section	.line
	.4byte	1018	/ sas.c:1018
	.2byte	0xffff
	.4byte	.L_LC397-.L_text_b
	.previous
	cmpl $0,180(%ebx)
	je .L194
.L_LC398:

.section	.line
	.4byte	1019	/ sas.c:1019
	.2byte	0xffff
	.4byte	.L_LC398-.L_text_b
	.previous
	pushl 180(%ebx)
	call freemsg
.L_LC399:

.section	.line
	.4byte	1020	/ sas.c:1020
	.2byte	0xffff
	.4byte	.L_LC399-.L_text_b
	.previous
	movl $0,180(%ebx)
.L_LC400:

.section	.line
	.4byte	1021	/ sas.c:1021
	.2byte	0xffff
	.4byte	.L_LC400-.L_text_b
	.previous
	addl $4,%esp
.L194:
.L_LC401:

.section	.line
	.4byte	1022	/ sas.c:1022
	.2byte	0xffff
	.4byte	.L_LC401-.L_text_b
	.previous
	cmpl $0,160(%ebx)
	je .L190
.L_LC402:

.section	.line
	.4byte	1023	/ sas.c:1023
	.2byte	0xffff
	.4byte	.L_LC402-.L_text_b
	.previous
	pushl 160(%ebx)
	call freemsg
.L_LC403:

.section	.line
	.4byte	1024	/ sas.c:1024
	.2byte	0xffff
	.4byte	.L_LC403-.L_text_b
	.previous
	movl $0,160(%ebx)
.L_LC404:

.section	.line
	.4byte	1025	/ sas.c:1025
	.2byte	0xffff
	.4byte	.L_LC404-.L_text_b
	.previous
	addl $4,%esp
.L190:
.L_LC405:

.section	.line
	.4byte	1027	/ sas.c:1027
	.2byte	0xffff
	.4byte	.L_LC405-.L_text_b
	.previous
	movl 112(%ebx),%eax
	andl $-5,(%eax)
.L_LC406:

.section	.line
	.4byte	1029	/ sas.c:1029
	.2byte	0xffff
	.4byte	.L_LC406-.L_text_b
	.previous
	leal 44(%ebx),%eax
	pushl %eax
	call wakeup
.L_LC407:

.section	.line
	.4byte	1031	/ sas.c:1031
	.2byte	0xffff
	.4byte	.L_LC407-.L_text_b
	.previous
	addl $4,%esp
	testb $2,33(%ebx)
	jne .L196
.L_LC408:

.section	.line
	.4byte	1032	/ sas.c:1032
	.2byte	0xffff
	.4byte	.L_LC408-.L_text_b
	.previous
	andb $239,32(%ebx)
	leal 32(%ebx),%eax
	pushl %eax
	call wakeup
	addl $4,%esp
.L196:
.L_LC409:

.section	.line
	.4byte	1034	/ sas.c:1034
	.2byte	0xffff
	.4byte	.L_LC409-.L_text_b
	.previous
	pushl -12(%ebp)
	call splx
.L_LC410:

.section	.line
	.4byte	1035	/ sas.c:1035
	.2byte	0xffff
	.4byte	.L_LC410-.L_text_b
	.previous
	xorl %eax,%eax
.L_LC411:

.section	.line
	.4byte	1036	/ sas.c:1036
	.2byte	0xffff
	.4byte	.L_LC411-.L_text_b
	.previous
.L165:
.L_b7_e:
	leal -24(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
.L_f7_e:
.Lfe4:
	.size	 sas_close,.Lfe4-sas_close

.section	.debug_pubnames
	.4byte	.L_P4
	.string	"sas_close"
	.previous

.section	.debug
.L_P4:
.L_D370:
	.4byte	.L_D370_e-.L_D370
	.2byte	0x6
	.2byte	0x12
	.4byte	.L_D382
	.2byte	0x38
	.string	"sas_close"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x111
	.4byte	sas_close
	.2byte	0x121
	.4byte	.L_f7_e
	.2byte	0x8041
	.4byte	.L_b7
	.2byte	0x8051
	.4byte	.L_b7_e
.L_D370_e:
.L_D383:
	.4byte	.L_D383_e-.L_D383
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D384
	.2byte	0x38
	.string	"rq"
	.2byte	0x83
	.2byte	.L_t383_e-.L_t383
.L_t383:
	.byte	0x1
	.4byte	.L_T664
.L_t383_e:
	.2byte	0x23
	.2byte	.L_l383_e-.L_l383
.L_l383:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l383_e:
.L_D383_e:
.L_D384:
	.4byte	.L_D384_e-.L_D384
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D385
	.2byte	0x38
	.string	"flag"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l384_e-.L_l384
.L_l384:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l384_e:
.L_D384_e:
.L_D385:
	.4byte	.L_D385_e-.L_D385
	.2byte	0xa
	.2byte	0x12
	.4byte	.L_D386
	.2byte	0x38
	.string	"signaled"
	.2byte	0x111
	.4byte	.L_I7_177
.L_D385_e:
.L_D386:
	.4byte	.L_D386_e-.L_D386
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D387
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t386_e-.L_t386
.L_t386:
	.byte	0x1
	.4byte	.L_T816
.L_t386_e:
	.2byte	0x23
	.2byte	.L_l386_e-.L_l386
.L_l386:
	.byte	0x1
	.4byte	0x3
.L_l386_e:
.L_D386_e:
.L_D387:
	.4byte	.L_D387_e-.L_D387
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D388
	.2byte	0x38
	.string	"fp"
	.2byte	0x83
	.2byte	.L_t387_e-.L_t387
.L_t387:
	.byte	0x1
	.4byte	.L_T815
.L_t387_e:
	.2byte	0x23
	.2byte	.L_l387_e-.L_l387
.L_l387:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xfffffffc
	.byte	0x7
.L_l387_e:
.L_D387_e:
.L_D388:
	.4byte	.L_D388_e-.L_D388
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D389
	.2byte	0x38
	.string	"ttyp"
	.2byte	0x63
	.2byte	.L_t388_e-.L_t388
.L_t388:
	.byte	0x1
	.2byte	0x9
.L_t388_e:
	.2byte	0x23
	.2byte	.L_l388_e-.L_l388
.L_l388:
	.byte	0x1
	.4byte	0x6
.L_l388_e:
.L_D388_e:
.L_D389:
	.4byte	.L_D389_e-.L_D389
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D390
	.2byte	0x38
	.string	"open_mode"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l389_e-.L_l389
.L_l389:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xfffffff8
	.byte	0x7
.L_l389_e:
.L_D389_e:
.L_D390:
	.4byte	.L_D390_e-.L_D390
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D391
	.2byte	0x38
	.string	"old_level"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l390_e-.L_l390
.L_l390:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xfffffff4
	.byte	0x7
.L_l390_e:
.L_D390_e:
.L_D391:
	.4byte	.L_D391_e-.L_D391
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D392
	.2byte	0x38
	.string	"dev"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l391_e-.L_l391
.L_l391:
.L_l391_e:
.L_D391_e:
.L_D392:
	.4byte	0x4
	.previous
.section	.rodata
.LC3:
	.string	"SAS: IOCNAK at TCXONC!\n"
.LC4:
	.string	"SAS: EINVAL 1!\n"
.LC5:
	.string	"SAS: bad ioctl 0x%x\n"
.LC6:
	.string	"SAS: This is not iocblk!\n"
.LC7:
	.string	"SAS: Unknown message!!! TYPE 0x%x\n"
.text
	.align 4
.globl sas_put
	.type	 sas_put,@function
sas_put:
.L_LC412:

.section	.line
	.4byte	1042	/ sas.c:1042
	.2byte	0xffff
	.4byte	.L_LC412-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	subl $160,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
.L_b8:
.L_LC413:

.section	.line
	.4byte	1043	/ sas.c:1043
	.2byte	0xffff
	.4byte	.L_LC413-.L_text_b
	.previous
.L_LC414:

.section	.line
	.4byte	1044	/ sas.c:1044
	.2byte	0xffff
	.4byte	.L_LC414-.L_text_b
	.previous
	movl 8(%ebp),%edx
	movl 20(%edx),%ecx
.L_LC415:

.section	.line
	.4byte	1053	/ sas.c:1053
	.2byte	0xffff
	.4byte	.L_LC415-.L_text_b
	.previous
	movl $0,-8(%ebp)
.L_LC416:

.section	.line
	.4byte	1059	/ sas.c:1059
	.2byte	0xffff
	.4byte	.L_LC416-.L_text_b
	.previous
	testl %ecx,%ecx
	jne .L199
.L_LC417:

.section	.line
	.4byte	1060	/ sas.c:1060
	.2byte	0xffff
	.4byte	.L_LC417-.L_text_b
	.previous
.L_B56:
	pushl 12(%ebp)
	pushl %edx
	call sas_error
.L_LC418:

.section	.line
	.4byte	1061	/ sas.c:1061
	.2byte	0xffff
	.4byte	.L_LC418-.L_text_b
	.previous
	jmp .L198
	.align 4
.L_LC419:

.section	.line
	.4byte	1062	/ sas.c:1062
	.2byte	0xffff
	.4byte	.L_LC419-.L_text_b
	.previous
.L_B56_e:
.L199:
.L_LC420:

.section	.line
	.4byte	1063	/ sas.c:1063
	.2byte	0xffff
	.4byte	.L_LC420-.L_text_b
	.previous
	movl 8(%ecx),%ebx
.L_LC421:

.section	.line
	.4byte	1065	/ sas.c:1065
	.2byte	0xffff
	.4byte	.L_LC421-.L_text_b
	.previous
	call spltty
	movl %eax,-4(%ebp)
.L_LC422:

.section	.line
	.4byte	1067	/ sas.c:1067
	.2byte	0xffff
	.4byte	.L_LC422-.L_text_b
	.previous
	movl 12(%ebp),%eax
	movl 20(%eax),%eax
	movl %eax,-16(%ebp)
	movzbl 13(%eax),%ecx
	cmpl $134,%ecx
	je .L201
	jg .L345
	cmpl $13,%ecx
	je .L321
	jg .L346
	testl %ecx,%ecx
	je .L204
	cmpl $12,%ecx
	je .L252
	jmp .L343
	.align 4
.L346:
	cmpl $14,%ecx
	je .L213
	cmpl $133,%ecx
	je .L342
	jmp .L343
	.align 4
.L345:
	cmpl $141,%ecx
	je .L209
	jg .L347
	cmpl $135,%ecx
	je .L340
	cmpl $136,%ecx
	je .L341
	jmp .L343
	.align 4
.L347:
	cmpl $143,%ecx
	je .L326
	cmpl $144,%ecx
	je .L333
	jmp .L343
	.align 4
.L_LC423:

.section	.line
	.4byte	1069	/ sas.c:1069
	.2byte	0xffff
	.4byte	.L_LC423-.L_text_b
	.previous
.L_B57:
.L201:
.L_LC424:

.section	.line
	.4byte	1070	/ sas.c:1070
	.2byte	0xffff
	.4byte	.L_LC424-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 12(%edx),%edx
	testb $2,(%edx)
	je .L202
.L_LC425:

.section	.line
	.4byte	1071	/ sas.c:1071
	.2byte	0xffff
	.4byte	.L_LC425-.L_text_b
	.previous
	pushl $7
	pushl %ebx
	call sas_cmd
.L_LC426:

.section	.line
	.4byte	1072	/ sas.c:1072
	.2byte	0xffff
	.4byte	.L_LC426-.L_text_b
	.previous
	pushl $3
	pushl %ebx
	call sas_cmd
.L_LC427:

.section	.line
	.4byte	1073	/ sas.c:1073
	.2byte	0xffff
	.4byte	.L_LC427-.L_text_b
	.previous
	addl $16,%esp
.L202:
.L_LC428:

.section	.line
	.4byte	1074	/ sas.c:1074
	.2byte	0xffff
	.4byte	.L_LC428-.L_text_b
	.previous
	movl 12(%ebp),%eax
	movl 12(%eax),%eax
	testb $1,(%eax)
	je .L203
.L_LC429:

.section	.line
	.4byte	1075	/ sas.c:1075
	.2byte	0xffff
	.4byte	.L_LC429-.L_text_b
	.previous
	pushl $6
	pushl %ebx
	call sas_cmd
.L_LC430:

.section	.line
	.4byte	1076	/ sas.c:1076
	.2byte	0xffff
	.4byte	.L_LC430-.L_text_b
	.previous
	addl $8,%esp
.L203:
.L_LC431:

.section	.line
	.4byte	1077	/ sas.c:1077
	.2byte	0xffff
	.4byte	.L_LC431-.L_text_b
	.previous
	pushl 12(%ebp)
	pushl 8(%ebp)
	call sas_flush
.L_LC432:

.section	.line
	.4byte	1079	/ sas.c:1079
	.2byte	0xffff
	.4byte	.L_LC432-.L_text_b
	.previous
	addl $8,%esp
	jmp .L200
	.align 4
.L204:
.L_LC433:

.section	.line
	.4byte	1090	/ sas.c:1090
	.2byte	0xffff
	.4byte	.L_LC433-.L_text_b
	.previous
	pushl 8(%ebp)
	call qsize
	addl $4,%esp
	testl %eax,%eax
	jne .L252
.L_LC434:

.section	.line
	.4byte	1091	/ sas.c:1091
	.2byte	0xffff
	.4byte	.L_LC434-.L_text_b
	.previous
.L_B58:
	call splhi
	movl %eax,%esi
.L_LC435:

.section	.line
	.4byte	1092	/ sas.c:1092
	.2byte	0xffff
	.4byte	.L_LC435-.L_text_b
	.previous
	cmpl $0,160(%ebx)
	je .L206
	pushl 160(%ebx)
	call msgdsize
	movl 108(%ebx),%edx
	leal (%edx,%edx,4),%edx
	addl %edx,%edx
	addl $4,%esp
	cmpl %edx,%eax
	jbe .L206
.L_LC436:

.section	.line
	.4byte	1094	/ sas.c:1094
	.2byte	0xffff
	.4byte	.L_LC436-.L_text_b
	.previous
	pushl %esi
	call splx
.L_LC437:

.section	.line
	.4byte	1095	/ sas.c:1095
	.2byte	0xffff
	.4byte	.L_LC437-.L_text_b
	.previous
	pushl 12(%ebp)
	pushl 8(%ebp)
	call putq
.L_LC438:

.section	.line
	.4byte	1096	/ sas.c:1096
	.2byte	0xffff
	.4byte	.L_LC438-.L_text_b
	.previous
	addl $12,%esp
	jmp .L200
	.align 4
.L206:
.L_LC439:

.section	.line
	.4byte	1098	/ sas.c:1098
	.2byte	0xffff
	.4byte	.L_LC439-.L_text_b
	.previous
	pushl 12(%ebp)
	leal 160(%ebx),%eax
	pushl %eax
	call sas_link
.L_LC440:

.section	.line
	.4byte	1099	/ sas.c:1099
	.2byte	0xffff
	.4byte	.L_LC440-.L_text_b
	.previous
	addl $8,%esp
.L_LC441:

.section	.line
	.4byte	1100	/ sas.c:1100
	.2byte	0xffff
	.4byte	.L_LC441-.L_text_b
	.previous
	pushl %esi
	call splx
.L_LC442:

.section	.line
	.4byte	1101	/ sas.c:1101
	.2byte	0xffff
	.4byte	.L_LC442-.L_text_b
	.previous
	movl 112(%ebx),%eax
	orb $32,(%eax)
.L_LC443:

.section	.line
	.4byte	1102	/ sas.c:1102
	.2byte	0xffff
	.4byte	.L_LC443-.L_text_b
	.previous
	pushl %ebx
	call sas_xproc
.L_LC444:

.section	.line
	.4byte	1103	/ sas.c:1103
	.2byte	0xffff
	.4byte	.L_LC444-.L_text_b
	.previous
.L_B58_e:
	addl $8,%esp
	jmp .L200
	.align 4
.L209:
.L_LC445:

.section	.line
	.4byte	1109	/ sas.c:1109
	.2byte	0xffff
	.4byte	.L_LC445-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 12(%edx),%edi
.L_LC446:

.section	.line
	.4byte	1111	/ sas.c:1111
	.2byte	0xffff
	.4byte	.L_LC446-.L_text_b
	.previous
	cmpl $22529,(%edi)
	je .L211
	cmpl $29725,(%edi)
	jne .L252
.L211:
.L_LC447:

.section	.line
	.4byte	1112	/ sas.c:1112
	.2byte	0xffff
	.4byte	.L_LC447-.L_text_b
	.previous
	movl -16(%ebp),%eax
	movb $129,13(%eax)
.L_LC448:

.section	.line
	.4byte	1113	/ sas.c:1113
	.2byte	0xffff
	.4byte	.L_LC448-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 12(%edx),%eax
	addl $40,%eax
	movl %eax,16(%edx)
.L_LC449:

.section	.line
	.4byte	1114	/ sas.c:1114
	.2byte	0xffff
	.4byte	.L_LC449-.L_text_b
	.previous
	movl $0,16(%edi)
.L_LC450:

.section	.line
	.4byte	1115	/ sas.c:1115
	.2byte	0xffff
	.4byte	.L_LC450-.L_text_b
	.previous
	movl $0,12(%edi)
.L_LC451:

.section	.line
	.4byte	1116	/ sas.c:1116
	.2byte	0xffff
	.4byte	.L_LC451-.L_text_b
	.previous
	movl $0,20(%edi)
.L_LC452:

.section	.line
	.4byte	1117	/ sas.c:1117
	.2byte	0xffff
	.4byte	.L_LC452-.L_text_b
	.previous
	pushl %edx
.L_LC453:

.section	.line
	.4byte	1118	/ sas.c:1118
	.2byte	0xffff
	.4byte	.L_LC453-.L_text_b
	.previous
	jmp .L348
	.align 4
.L213:
.L_LC454:

.section	.line
	.4byte	1128	/ sas.c:1128
	.2byte	0xffff
	.4byte	.L_LC454-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 12(%edx),%edi
.L_LC455:

.section	.line
	.4byte	1130	/ sas.c:1130
	.2byte	0xffff
	.4byte	.L_LC455-.L_text_b
	.previous
	movl (%edi),%ecx
	cmpl $21520,%ecx
	jg .L316
	cmpl $21519,%ecx
	jge .L243
	cmpl $21508,%ecx
	jg .L317
	cmpl $21507,%ecx
	jge .L251
	cmpl $21505,%ecx
	je .L289
	jg .L246
	cmpl $17767,%ecx
	je .L309
	jmp .L314
	.align 4
.L317:
	cmpl $21511,%ecx
	je .L215
	jg .L318
	cmpl $21509,%ecx
	je .L252
	cmpl $21510,%ecx
	je .L296
	jmp .L314
	.align 4
.L318:
	cmpl $21517,%ecx
	je .L282
	cmpl $21518,%ecx
	je .L238
	jmp .L314
	.align 4
.L316:
	cmpl $22532,%ecx
	jg .L319
	cmpl $22530,%ecx
	jge .L233
	cmpl $22528,%ecx
	je .L306
	jg .L255
	cmpl $21608,%ecx
	jg .L314
	cmpl $21607,%ecx
	jl .L314
	jmp .L306
	.align 4
.L319:
	cmpl $29724,%ecx
	jg .L320
	cmpl $29722,%ecx
	jge .L233
	cmpl $27141,%ecx
	je .L306
	jmp .L314
	.align 4
.L320:
	cmpl $29725,%ecx
	je .L255
	cmpl $50436,%ecx
	jg .L314
	cmpl $50435,%ecx
	jl .L314
	jmp .L308
	.align 4
.L215:
.L_LC456:

.section	.line
	.4byte	1133	/ sas.c:1133
	.2byte	0xffff
	.4byte	.L_LC456-.L_text_b
	.previous
	cmpl $4,12(%edi)
	je .L216
.L_LC457:

.section	.line
	.4byte	1134	/ sas.c:1134
	.2byte	0xffff
	.4byte	.L_LC457-.L_text_b
	.previous
	movl -16(%ebp),%eax
	movb $130,13(%eax)
.L_LC458:

.section	.line
	.4byte	1135	/ sas.c:1135
	.2byte	0xffff
	.4byte	.L_LC458-.L_text_b
	.previous
	jmp .L349
	.align 4
.L216:
.L_LC459:

.section	.line
	.4byte	1136	/ sas.c:1136
	.2byte	0xffff
	.4byte	.L_LC459-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 8(%edx),%edx
	movl 12(%edx),%eax
	movl (%eax),%ecx
	cmpl $1,%ecx
	je .L221
	jg .L226
	testl %ecx,%ecx
	je .L219
	jmp .L218
	.align 4
.L226:
	cmpl $2,%ecx
	je .L222
	jmp .L218
	.align 4
.L219:
.L_LC460:

.section	.line
	.4byte	1138	/ sas.c:1138
	.2byte	0xffff
	.4byte	.L_LC460-.L_text_b
	.previous
	pushl $6
	pushl %ebx
	call sas_cmd
.L_LC461:

.section	.line
	.4byte	1139	/ sas.c:1139
	.2byte	0xffff
	.4byte	.L_LC461-.L_text_b
	.previous
	pushl $0
	movl 8(%ebp),%ecx
	testb $16,28(%ecx)
	jne .L220
	addl $-64,%ecx
.L220:
	pushl %ecx
	call flushq
.L_LC462:

.section	.line
	.4byte	1140	/ sas.c:1140
	.2byte	0xffff
	.4byte	.L_LC462-.L_text_b
	.previous
	addl $16,%esp
	jmp .L218
	.align 4
.L221:
.L_LC463:

.section	.line
	.4byte	1143	/ sas.c:1143
	.2byte	0xffff
	.4byte	.L_LC463-.L_text_b
	.previous
	pushl $7
	pushl %ebx
	call sas_cmd
.L_LC464:

.section	.line
	.4byte	1144	/ sas.c:1144
	.2byte	0xffff
	.4byte	.L_LC464-.L_text_b
	.previous
	pushl $3
	pushl %ebx
	call sas_cmd
.L_LC465:

.section	.line
	.4byte	1145	/ sas.c:1145
	.2byte	0xffff
	.4byte	.L_LC465-.L_text_b
	.previous
	pushl $0
	pushl 8(%ebp)
	call flushq
.L_LC466:

.section	.line
	.4byte	1146	/ sas.c:1146
	.2byte	0xffff
	.4byte	.L_LC466-.L_text_b
	.previous
	addl $24,%esp
	jmp .L218
	.align 4
.L222:
.L_LC467:

.section	.line
	.4byte	1149	/ sas.c:1149
	.2byte	0xffff
	.4byte	.L_LC467-.L_text_b
	.previous
	pushl $6
	pushl %ebx
	call sas_cmd
.L_LC468:

.section	.line
	.4byte	1150	/ sas.c:1150
	.2byte	0xffff
	.4byte	.L_LC468-.L_text_b
	.previous
	pushl $7
	pushl %ebx
	call sas_cmd
.L_LC469:

.section	.line
	.4byte	1151	/ sas.c:1151
	.2byte	0xffff
	.4byte	.L_LC469-.L_text_b
	.previous
	pushl $3
	pushl %ebx
	call sas_cmd
.L_LC470:

.section	.line
	.4byte	1152	/ sas.c:1152
	.2byte	0xffff
	.4byte	.L_LC470-.L_text_b
	.previous
	pushl $0
	movl 8(%ebp),%ecx
	testb $16,28(%ecx)
	jne .L223
	addl $-64,%ecx
.L223:
	pushl %ecx
	call flushq
.L_LC471:

.section	.line
	.4byte	1153	/ sas.c:1153
	.2byte	0xffff
	.4byte	.L_LC471-.L_text_b
	.previous
	addl $32,%esp
	pushl $0
	pushl 8(%ebp)
	call flushq
.L_LC472:

.section	.line
	.4byte	1154	/ sas.c:1154
	.2byte	0xffff
	.4byte	.L_LC472-.L_text_b
	.previous
	addl $8,%esp
.L218:
.L_LC473:

.section	.line
	.4byte	1156	/ sas.c:1156
	.2byte	0xffff
	.4byte	.L_LC473-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 20(%edx),%edx
	movb $129,13(%edx)
.L_LC474:

.section	.line
	.4byte	1159	/ sas.c:1159
	.2byte	0xffff
	.4byte	.L_LC474-.L_text_b
	.previous
	jmp .L349
	.align 4
.L233:
.L_LC475:

.section	.line
	.4byte	1172	/ sas.c:1172
	.2byte	0xffff
	.4byte	.L_LC475-.L_text_b
	.previous
	cmpl $-1,12(%edi)
	je .L234
.L_LC476:

.section	.line
	.4byte	1173	/ sas.c:1173
	.2byte	0xffff
	.4byte	.L_LC476-.L_text_b
	.previous
	movl 12(%ebp),%eax
	cmpl $0,8(%eax)
	je .L235
.L_LC477:

.section	.line
	.4byte	1174	/ sas.c:1174
	.2byte	0xffff
	.4byte	.L_LC477-.L_text_b
	.previous
	pushl 8(%eax)
	call freemsg
.L_LC478:

.section	.line
	.4byte	1175	/ sas.c:1175
	.2byte	0xffff
	.4byte	.L_LC478-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl $0,8(%edx)
.L_LC479:

.section	.line
	.4byte	1176	/ sas.c:1176
	.2byte	0xffff
	.4byte	.L_LC479-.L_text_b
	.previous
	addl $4,%esp
.L235:
.L_LC480:

.section	.line
	.4byte	1177	/ sas.c:1177
	.2byte	0xffff
	.4byte	.L_LC480-.L_text_b
	.previous
	movl 12(%ebp),%eax
	movl 20(%eax),%eax
	movb $130,13(%eax)
.L_LC481:

.section	.line
	.4byte	1179	/ sas.c:1179
	.2byte	0xffff
	.4byte	.L_LC481-.L_text_b
	.previous
	jmp .L349
	.align 4
.L234:
.L_LC482:

.section	.line
	.4byte	1181	/ sas.c:1181
	.2byte	0xffff
	.4byte	.L_LC482-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 12(%edx),%esi
.L_LC483:

.section	.line
	.4byte	1182	/ sas.c:1182
	.2byte	0xffff
	.4byte	.L_LC483-.L_text_b
	.previous
	movl 8(%edx),%eax
	movl 12(%eax),%edx
	movl (%edx),%eax
	movl %eax,12(%esi)
.L_LC484:

.section	.line
	.4byte	1183	/ sas.c:1183
	.2byte	0xffff
	.4byte	.L_LC484-.L_text_b
	.previous
	movl 12(%ebp),%edx
	pushl 8(%edx)
	call freemsg
.L_LC485:

.section	.line
	.4byte	1184	/ sas.c:1184
	.2byte	0xffff
	.4byte	.L_LC485-.L_text_b
	.previous
	movl 12(%ebp),%eax
	movl $0,8(%eax)
.L_LC486:

.section	.line
	.4byte	1185	/ sas.c:1185
	.2byte	0xffff
	.4byte	.L_LC486-.L_text_b
	.previous
	addl $4,%esp
	cmpb $88,1(%edi)
	jne .L236
.L_LC487:

.section	.line
	.4byte	1186	/ sas.c:1186
	.2byte	0xffff
	.4byte	.L_LC487-.L_text_b
	.previous
	movl $88,24(%esi)
.L_LC488:

.section	.line
	.4byte	1187	/ sas.c:1187
	.2byte	0xffff
	.4byte	.L_LC488-.L_text_b
	.previous
	movl $16,16(%esi)
.L_LC489:

.section	.line
	.4byte	1188	/ sas.c:1188
	.2byte	0xffff
	.4byte	.L_LC489-.L_text_b
	.previous
	jmp .L237
	.align 4
.L236:
.L_LC490:

.section	.line
	.4byte	1189	/ sas.c:1189
	.2byte	0xffff
	.4byte	.L_LC490-.L_text_b
	.previous
	movl $116,24(%esi)
.L_LC491:

.section	.line
	.4byte	1190	/ sas.c:1190
	.2byte	0xffff
	.4byte	.L_LC491-.L_text_b
	.previous
	movl $4,16(%esi)
.L237:
.L_LC492:

.section	.line
	.4byte	1192	/ sas.c:1192
	.2byte	0xffff
	.4byte	.L_LC492-.L_text_b
	.previous
	movl $0,20(%esi)
.L_LC493:

.section	.line
	.4byte	1193	/ sas.c:1193
	.2byte	0xffff
	.4byte	.L_LC493-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 20(%edx),%edx
	movb $139,13(%edx)
.L_LC494:

.section	.line
	.4byte	1194	/ sas.c:1194
	.2byte	0xffff
	.4byte	.L_LC494-.L_text_b
	.previous
	movl 12(%ebp),%eax
	movl 12(%eax),%edx
	addl $44,%edx
	movl %edx,16(%eax)
.L_LC495:

.section	.line
	.4byte	1195	/ sas.c:1195
	.2byte	0xffff
	.4byte	.L_LC495-.L_text_b
	.previous
	pushl %eax
.L_LC496:

.section	.line
	.4byte	1196	/ sas.c:1196
	.2byte	0xffff
	.4byte	.L_LC496-.L_text_b
	.previous
	jmp .L348
	.align 4
.L238:
.L_LC497:

.section	.line
	.4byte	1199	/ sas.c:1199
	.2byte	0xffff
	.4byte	.L_LC497-.L_text_b
	.previous
	cmpl $36,12(%edi)
	je .L239
.L_LC498:

.section	.line
	.4byte	1200	/ sas.c:1200
	.2byte	0xffff
	.4byte	.L_LC498-.L_text_b
	.previous
	movl 12(%ebp),%eax
	movl 20(%eax),%eax
	movb $130,13(%eax)
.L_LC499:

.section	.line
	.4byte	1201	/ sas.c:1201
	.2byte	0xffff
	.4byte	.L_LC499-.L_text_b
	.previous
	jmp .L349
	.align 4
.L239:
.L_LC500:

.section	.line
	.4byte	1202	/ sas.c:1202
	.2byte	0xffff
	.4byte	.L_LC500-.L_text_b
	.previous
	pushl 12(%ebp)
	pushl %ebx
	call sas_tcsets
	addl $8,%esp
	testl %eax,%eax
	jne .L349
.L_LC501:

.section	.line
	.4byte	1203	/ sas.c:1203
	.2byte	0xffff
	.4byte	.L_LC501-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 20(%edx),%edx
	movb $130,13(%edx)
.L_LC502:

.section	.line
	.4byte	1206	/ sas.c:1206
	.2byte	0xffff
	.4byte	.L_LC502-.L_text_b
	.previous
	jmp .L349
	.align 4
.L243:
.L_LC503:

.section	.line
	.4byte	1210	/ sas.c:1210
	.2byte	0xffff
	.4byte	.L_LC503-.L_text_b
	.previous
	cmpl $36,12(%edi)
	je .L252
.L_LC504:

.section	.line
	.4byte	1211	/ sas.c:1211
	.2byte	0xffff
	.4byte	.L_LC504-.L_text_b
	.previous
	movl -16(%ebp),%eax
	movb $130,13(%eax)
.L_LC505:

.section	.line
	.4byte	1213	/ sas.c:1213
	.2byte	0xffff
	.4byte	.L_LC505-.L_text_b
	.previous
	jmp .L349
	.align 4
.L246:
.L_LC506:

.section	.line
	.4byte	1219	/ sas.c:1219
	.2byte	0xffff
	.4byte	.L_LC506-.L_text_b
	.previous
	cmpl $18,12(%edi)
	je .L247
.L_LC507:

.section	.line
	.4byte	1220	/ sas.c:1220
	.2byte	0xffff
	.4byte	.L_LC507-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 20(%edx),%edx
	movb $130,13(%edx)
.L_LC508:

.section	.line
	.4byte	1221	/ sas.c:1221
	.2byte	0xffff
	.4byte	.L_LC508-.L_text_b
	.previous
	jmp .L349
	.align 4
.L247:
.L_LC509:

.section	.line
	.4byte	1222	/ sas.c:1222
	.2byte	0xffff
	.4byte	.L_LC509-.L_text_b
	.previous
	pushl 12(%ebp)
	pushl %ebx
	call sas_tcset
	addl $8,%esp
	testl %eax,%eax
	jne .L349
.L_LC510:

.section	.line
	.4byte	1223	/ sas.c:1223
	.2byte	0xffff
	.4byte	.L_LC510-.L_text_b
	.previous
	movl 12(%ebp),%eax
	movl 20(%eax),%eax
	movb $130,13(%eax)
.L_LC511:

.section	.line
	.4byte	1226	/ sas.c:1226
	.2byte	0xffff
	.4byte	.L_LC511-.L_text_b
	.previous
	jmp .L349
	.align 4
.L251:
.L_LC512:

.section	.line
	.4byte	1230	/ sas.c:1230
	.2byte	0xffff
	.4byte	.L_LC512-.L_text_b
	.previous
	cmpl $18,12(%edi)
	je .L252
.L_LC513:

.section	.line
	.4byte	1231	/ sas.c:1231
	.2byte	0xffff
	.4byte	.L_LC513-.L_text_b
	.previous
	movl -16(%ebp),%edx
	movb $130,13(%edx)
.L_LC514:

.section	.line
	.4byte	1233	/ sas.c:1233
	.2byte	0xffff
	.4byte	.L_LC514-.L_text_b
	.previous
	jmp .L349
	.align 4
.L252:
.L_LC515:

.section	.line
	.4byte	1234	/ sas.c:1234
	.2byte	0xffff
	.4byte	.L_LC515-.L_text_b
	.previous
	pushl 12(%ebp)
	pushl 8(%ebp)
	call putq
.L_LC516:

.section	.line
	.4byte	1235	/ sas.c:1235
	.2byte	0xffff
	.4byte	.L_LC516-.L_text_b
	.previous
	addl $8,%esp
.L_LC517:

.section	.line
	.4byte	1236	/ sas.c:1236
	.2byte	0xffff
	.4byte	.L_LC517-.L_text_b
	.previous
	jmp .L200
	.align 4
.L255:
.L_LC518:

.section	.line
	.4byte	1241	/ sas.c:1241
	.2byte	0xffff
	.4byte	.L_LC518-.L_text_b
	.previous
	cmpl $-1,12(%edi)
	jne .L256
.L_LC519:

.section	.line
	.4byte	1242	/ sas.c:1242
	.2byte	0xffff
	.4byte	.L_LC519-.L_text_b
	.previous
	movl 12(%ebp),%eax
	movl 12(%eax),%esi
.L_LC520:

.section	.line
	.4byte	1243	/ sas.c:1243
	.2byte	0xffff
	.4byte	.L_LC520-.L_text_b
	.previous
	cmpl $22529,(%edi)
	jne .L257
.L_LC521:

.section	.line
	.4byte	1244	/ sas.c:1244
	.2byte	0xffff
	.4byte	.L_LC521-.L_text_b
	.previous
	movl $16,16(%esi)
.L_LC522:

.section	.line
	.4byte	1245	/ sas.c:1245
	.2byte	0xffff
	.4byte	.L_LC522-.L_text_b
	.previous
	jmp .L258
	.align 4
.L257:
.L_LC523:

.section	.line
	.4byte	1246	/ sas.c:1246
	.2byte	0xffff
	.4byte	.L_LC523-.L_text_b
	.previous
	movl $4,16(%esi)
.L258:
.L_LC524:

.section	.line
	.4byte	1248	/ sas.c:1248
	.2byte	0xffff
	.4byte	.L_LC524-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 8(%edx),%edx
	movl 12(%edx),%eax
	movl (%eax),%edx
	movl %edx,12(%esi)
.L_LC525:

.section	.line
	.4byte	1249	/ sas.c:1249
	.2byte	0xffff
	.4byte	.L_LC525-.L_text_b
	.previous
	movl $0,20(%esi)
.L_LC526:

.section	.line
	.4byte	1250	/ sas.c:1250
	.2byte	0xffff
	.4byte	.L_LC526-.L_text_b
	.previous
	movl $1,-8(%ebp)
.L256:
.L_LC527:

.section	.line
	.4byte	1252	/ sas.c:1252
	.2byte	0xffff
	.4byte	.L_LC527-.L_text_b
	.previous
	movl 12(%ebp),%eax
	cmpl $0,8(%eax)
	je .L259
.L_LC528:

.section	.line
	.4byte	1253	/ sas.c:1253
	.2byte	0xffff
	.4byte	.L_LC528-.L_text_b
	.previous
	pushl 8(%eax)
	call freemsg
	addl $4,%esp
.L259:
.L_LC529:

.section	.line
	.4byte	1254	/ sas.c:1254
	.2byte	0xffff
	.4byte	.L_LC529-.L_text_b
	.previous
	pushl $2
	pushl 16(%esi)
	call allocb
	movl %eax,%ecx
	movl 12(%ebp),%edx
	movl %ecx,8(%edx)
	addl $8,%esp
	testl %ecx,%ecx
	jne .L260
.L_LC530:

.section	.line
	.4byte	1256	/ sas.c:1256
	.2byte	0xffff
	.4byte	.L_LC530-.L_text_b
	.previous
	movl 20(%edx),%eax
	movb $130,13(%eax)
.L_LC531:

.section	.line
	.4byte	1257	/ sas.c:1257
	.2byte	0xffff
	.4byte	.L_LC531-.L_text_b
	.previous
	movl $11,16(%edi)
.L_LC532:

.section	.line
	.4byte	1258	/ sas.c:1258
	.2byte	0xffff
	.4byte	.L_LC532-.L_text_b
	.previous
	pushl %edx
.L_LC533:

.section	.line
	.4byte	1259	/ sas.c:1259
	.2byte	0xffff
	.4byte	.L_LC533-.L_text_b
	.previous
	jmp .L348
	.align 4
.L260:
.L_LC534:

.section	.line
	.4byte	1261	/ sas.c:1261
	.2byte	0xffff
	.4byte	.L_LC534-.L_text_b
	.previous
	cmpl $22529,(%edi)
	jne .L261
.L_LC535:

.section	.line
	.4byte	1262	/ sas.c:1262
	.2byte	0xffff
	.4byte	.L_LC535-.L_text_b
	.previous
	movl 12(%ecx),%ecx
.L_LC536:

.section	.line
	.4byte	1263	/ sas.c:1263
	.2byte	0xffff
	.4byte	.L_LC536-.L_text_b
	.previous
	movw $0,14(%ecx)
.L_LC537:

.section	.line
	.4byte	1264	/ sas.c:1264
	.2byte	0xffff
	.4byte	.L_LC537-.L_text_b
	.previous
	movw $0,2(%ecx)
.L_LC538:

.section	.line
	.4byte	1265	/ sas.c:1265
	.2byte	0xffff
	.4byte	.L_LC538-.L_text_b
	.previous
	movl $4,%esi
	.align 4
.L265:
.L_LC539:

.section	.line
	.4byte	1266	/ sas.c:1266
	.2byte	0xffff
	.4byte	.L_LC539-.L_text_b
	.previous
	movw $0,4(%ecx,%esi,2)
.L_LC540:

.section	.line
	.4byte	1265	/ sas.c:1265
	.2byte	0xffff
	.4byte	.L_LC540-.L_text_b
	.previous
	decl %esi
	jns .L265
.L_LC541:

.section	.line
	.4byte	1268	/ sas.c:1268
	.2byte	0xffff
	.4byte	.L_LC541-.L_text_b
	.previous
	movw $0,(%ecx)
.L_LC542:

.section	.line
	.4byte	1269	/ sas.c:1269
	.2byte	0xffff
	.4byte	.L_LC542-.L_text_b
	.previous
	testb $2,36(%ebx)
	je .L266
.L_LC543:

.section	.line
	.4byte	1270	/ sas.c:1270
	.2byte	0xffff
	.4byte	.L_LC543-.L_text_b
	.previous
	cmpb $2,56(%ebx)
	jne .L267
.L_LC544:

.section	.line
	.4byte	1271	/ sas.c:1271
	.2byte	0xffff
	.4byte	.L_LC544-.L_text_b
	.previous
	movw $1,(%ecx)
.L_LC545:

.section	.line
	.4byte	1272	/ sas.c:1272
	.2byte	0xffff
	.4byte	.L_LC545-.L_text_b
	.previous
	jmp .L266
	.align 4
.L267:
.L_LC546:

.section	.line
	.4byte	1273	/ sas.c:1273
	.2byte	0xffff
	.4byte	.L_LC546-.L_text_b
	.previous
	movw $4,(%ecx)
.L266:
.L_LC547:

.section	.line
	.4byte	1276	/ sas.c:1276
	.2byte	0xffff
	.4byte	.L_LC547-.L_text_b
	.previous
	testb $1,36(%ebx)
	je .L269
.L_LC548:

.section	.line
	.4byte	1277	/ sas.c:1277
	.2byte	0xffff
	.4byte	.L_LC548-.L_text_b
	.previous
	cmpb $16,57(%ebx)
	jne .L270
.L_LC549:

.section	.line
	.4byte	1278	/ sas.c:1278
	.2byte	0xffff
	.4byte	.L_LC549-.L_text_b
	.previous
	orb $2,(%ecx)
.L_LC550:

.section	.line
	.4byte	1279	/ sas.c:1279
	.2byte	0xffff
	.4byte	.L_LC550-.L_text_b
	.previous
	jmp .L269
	.align 4
.L270:
.L_LC551:

.section	.line
	.4byte	1280	/ sas.c:1280
	.2byte	0xffff
	.4byte	.L_LC551-.L_text_b
	.previous
	orb $8,(%ecx)
.L269:
.L_LC552:

.section	.line
	.4byte	1283	/ sas.c:1283
	.2byte	0xffff
	.4byte	.L_LC552-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 8(%edx),%edx
	movl %edx,-72(%ebp)
	movl -72(%ebp),%edx
	movl 12(%edx),%eax
	addl $16,%eax
.L_LC553:

.section	.line
	.4byte	1285	/ sas.c:1285
	.2byte	0xffff
	.4byte	.L_LC553-.L_text_b
	.previous
	jmp .L350
	.align 4
.L261:
.L_LC554:

.section	.line
	.4byte	1286	/ sas.c:1286
	.2byte	0xffff
	.4byte	.L_LC554-.L_text_b
	.previous
	movl 12(%ecx),%ecx
.L_LC555:

.section	.line
	.4byte	1287	/ sas.c:1287
	.2byte	0xffff
	.4byte	.L_LC555-.L_text_b
	.previous
	movl $0,(%ecx)
.L_LC556:

.section	.line
	.4byte	1288	/ sas.c:1288
	.2byte	0xffff
	.4byte	.L_LC556-.L_text_b
	.previous
	testb $32,32(%ebx)
	je .L273
.L_LC557:

.section	.line
	.4byte	1289	/ sas.c:1289
	.2byte	0xffff
	.4byte	.L_LC557-.L_text_b
	.previous
	movl $1,(%ecx)
.L273:
.L_LC558:

.section	.line
	.4byte	1291	/ sas.c:1291
	.2byte	0xffff
	.4byte	.L_LC558-.L_text_b
	.previous
	testb $1,62(%ebx)
	je .L274
.L_LC559:

.section	.line
	.4byte	1292	/ sas.c:1292
	.2byte	0xffff
	.4byte	.L_LC559-.L_text_b
	.previous
	orb $2,(%ecx)
.L274:
.L_LC560:

.section	.line
	.4byte	1294	/ sas.c:1294
	.2byte	0xffff
	.4byte	.L_LC560-.L_text_b
	.previous
	testb $2,62(%ebx)
	je .L275
.L_LC561:

.section	.line
	.4byte	1295	/ sas.c:1295
	.2byte	0xffff
	.4byte	.L_LC561-.L_text_b
	.previous
	orb $4,(%ecx)
.L275:
.L_LC562:

.section	.line
	.4byte	1297	/ sas.c:1297
	.2byte	0xffff
	.4byte	.L_LC562-.L_text_b
	.previous
	testb $16,60(%ebx)
	je .L276
.L_LC563:

.section	.line
	.4byte	1298	/ sas.c:1298
	.2byte	0xffff
	.4byte	.L_LC563-.L_text_b
	.previous
	orb $32,(%ecx)
.L276:
.L_LC564:

.section	.line
	.4byte	1300	/ sas.c:1300
	.2byte	0xffff
	.4byte	.L_LC564-.L_text_b
	.previous
	cmpb $0,60(%ebx)
	jge .L277
.L_LC565:

.section	.line
	.4byte	1301	/ sas.c:1301
	.2byte	0xffff
	.4byte	.L_LC565-.L_text_b
	.previous
	orb $64,(%ecx)
.L277:
.L_LC566:

.section	.line
	.4byte	1303	/ sas.c:1303
	.2byte	0xffff
	.4byte	.L_LC566-.L_text_b
	.previous
	testb $64,60(%ebx)
	je .L278
.L_LC567:

.section	.line
	.4byte	1304	/ sas.c:1304
	.2byte	0xffff
	.4byte	.L_LC567-.L_text_b
	.previous
	orb $128,(%ecx)
.L278:
.L_LC568:

.section	.line
	.4byte	1306	/ sas.c:1306
	.2byte	0xffff
	.4byte	.L_LC568-.L_text_b
	.previous
	testb $32,60(%ebx)
	je .L279
.L_LC569:

.section	.line
	.4byte	1307	/ sas.c:1307
	.2byte	0xffff
	.4byte	.L_LC569-.L_text_b
	.previous
	orl $256,(%ecx)
.L279:
.L_LC570:

.section	.line
	.4byte	1309	/ sas.c:1309
	.2byte	0xffff
	.4byte	.L_LC570-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 8(%edx),%edx
	movl %edx,-76(%ebp)
	movl -76(%ebp),%edx
	movl 12(%edx),%eax
	addl $4,%eax
.L350:
	movl %eax,16(%edx)
.L_LC571:

.section	.line
	.4byte	1312	/ sas.c:1312
	.2byte	0xffff
	.4byte	.L_LC571-.L_text_b
	.previous
	cmpl $0,-8(%ebp)
	je .L280
.L_LC572:

.section	.line
	.4byte	1313	/ sas.c:1313
	.2byte	0xffff
	.4byte	.L_LC572-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 20(%edx),%edx
	movb $140,13(%edx)
.L_LC573:

.section	.line
	.4byte	1314	/ sas.c:1314
	.2byte	0xffff
	.4byte	.L_LC573-.L_text_b
	.previous
	movl 12(%ebp),%eax
	movl 12(%eax),%edx
	addl $44,%edx
.L_LC574:

.section	.line
	.4byte	1315	/ sas.c:1315
	.2byte	0xffff
	.4byte	.L_LC574-.L_text_b
	.previous
	jmp .L353
	.align 4
.L280:
.L_LC575:

.section	.line
	.4byte	1316	/ sas.c:1316
	.2byte	0xffff
	.4byte	.L_LC575-.L_text_b
	.previous
	movl 12(%ebp),%eax
	movl 20(%eax),%eax
	movb $129,13(%eax)
.L_LC576:

.section	.line
	.4byte	1317	/ sas.c:1317
	.2byte	0xffff
	.4byte	.L_LC576-.L_text_b
	.previous
	movl $16,12(%edi)
.L_LC577:

.section	.line
	.4byte	1320	/ sas.c:1320
	.2byte	0xffff
	.4byte	.L_LC577-.L_text_b
	.previous
	jmp .L349
	.align 4
.L282:
.L_LC578:

.section	.line
	.4byte	1322	/ sas.c:1322
	.2byte	0xffff
	.4byte	.L_LC578-.L_text_b
	.previous
	movl 12(%ebp),%edx
	cmpl $0,8(%edx)
	je .L283
	pushl 8(%edx)
	call freemsg
	addl $4,%esp
.L283:
.L_LC579:

.section	.line
	.4byte	1323	/ sas.c:1323
	.2byte	0xffff
	.4byte	.L_LC579-.L_text_b
	.previous
	pushl $2
	pushl $36
	call allocb
	movl %eax,%ecx
	movl 12(%ebp),%eax
	movl %ecx,8(%eax)
	addl $8,%esp
	testl %ecx,%ecx
	jne .L284
.L_LC580:

.section	.line
	.4byte	1324	/ sas.c:1324
	.2byte	0xffff
	.4byte	.L_LC580-.L_text_b
	.previous
	movl 20(%eax),%edx
	movb $130,13(%edx)
.L_LC581:

.section	.line
	.4byte	1325	/ sas.c:1325
	.2byte	0xffff
	.4byte	.L_LC581-.L_text_b
	.previous
	movl $11,16(%edi)
.L_LC582:

.section	.line
	.4byte	1326	/ sas.c:1326
	.2byte	0xffff
	.4byte	.L_LC582-.L_text_b
	.previous
	pushl %eax
.L_LC583:

.section	.line
	.4byte	1327	/ sas.c:1327
	.2byte	0xffff
	.4byte	.L_LC583-.L_text_b
	.previous
	jmp .L348
	.align 4
.L284:
.L_LC584:

.section	.line
	.4byte	1329	/ sas.c:1329
	.2byte	0xffff
	.4byte	.L_LC584-.L_text_b
	.previous
	movl 12(%ecx),%ecx
.L_LC585:

.section	.line
	.4byte	1330	/ sas.c:1330
	.2byte	0xffff
	.4byte	.L_LC585-.L_text_b
	.previous
	movl 16(%ebx),%eax
	movl %eax,8(%ecx)
.L_LC586:

.section	.line
	.4byte	1331	/ sas.c:1331
	.2byte	0xffff
	.4byte	.L_LC586-.L_text_b
	.previous
	movl 28(%ebx),%edx
	movl %edx,12(%ecx)
.L_LC587:

.section	.line
	.4byte	1332	/ sas.c:1332
	.2byte	0xffff
	.4byte	.L_LC587-.L_text_b
	.previous
	movl 12(%ebx),%eax
	movl %eax,(%ecx)
.L_LC588:

.section	.line
	.4byte	1333	/ sas.c:1333
	.2byte	0xffff
	.4byte	.L_LC588-.L_text_b
	.previous
	movl 24(%ebx),%edx
	movl %edx,4(%ecx)
.L_LC589:

.section	.line
	.4byte	1334	/ sas.c:1334
	.2byte	0xffff
	.4byte	.L_LC589-.L_text_b
	.previous
	xorl %esi,%esi
	.align 4
.L288:
	movb 184(%esi,%ebx),%al
	movb %al,16(%esi,%ecx)
	incl %esi
	cmpl $18,%esi
	jle .L288
.L_LC590:

.section	.line
	.4byte	1335	/ sas.c:1335
	.2byte	0xffff
	.4byte	.L_LC590-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 20(%edx),%edx
	movb $129,13(%edx)
.L_LC591:

.section	.line
	.4byte	1336	/ sas.c:1336
	.2byte	0xffff
	.4byte	.L_LC591-.L_text_b
	.previous
	movl $36,12(%edi)
.L_LC592:

.section	.line
	.4byte	1337	/ sas.c:1337
	.2byte	0xffff
	.4byte	.L_LC592-.L_text_b
	.previous
	movl 12(%ebp),%eax
	movl 8(%eax),%eax
	movl %eax,-96(%ebp)
	movl -96(%ebp),%eax
.L_LC593:

.section	.line
	.4byte	1340	/ sas.c:1340
	.2byte	0xffff
	.4byte	.L_LC593-.L_text_b
	.previous
	jmp .L351
	.align 4
.L289:
.L_LC594:

.section	.line
	.4byte	1344	/ sas.c:1344
	.2byte	0xffff
	.4byte	.L_LC594-.L_text_b
	.previous
	movl 12(%ebp),%eax
	cmpl $0,8(%eax)
	je .L290
	pushl 8(%eax)
	call freemsg
	addl $4,%esp
.L290:
.L_LC595:

.section	.line
	.4byte	1345	/ sas.c:1345
	.2byte	0xffff
	.4byte	.L_LC595-.L_text_b
	.previous
	pushl $2
	pushl $18
	call allocb
	movl %eax,%ecx
	movl 12(%ebp),%edx
	movl %ecx,8(%edx)
	addl $8,%esp
	testl %ecx,%ecx
	jne .L291
.L_LC596:

.section	.line
	.4byte	1346	/ sas.c:1346
	.2byte	0xffff
	.4byte	.L_LC596-.L_text_b
	.previous
	movl 20(%edx),%eax
	movb $130,13(%eax)
.L_LC597:

.section	.line
	.4byte	1347	/ sas.c:1347
	.2byte	0xffff
	.4byte	.L_LC597-.L_text_b
	.previous
	movl $11,16(%edi)
.L_LC598:

.section	.line
	.4byte	1348	/ sas.c:1348
	.2byte	0xffff
	.4byte	.L_LC598-.L_text_b
	.previous
	pushl %edx
.L_LC599:

.section	.line
	.4byte	1349	/ sas.c:1349
	.2byte	0xffff
	.4byte	.L_LC599-.L_text_b
	.previous
	jmp .L348
	.align 4
.L291:
.L_LC600:

.section	.line
	.4byte	1351	/ sas.c:1351
	.2byte	0xffff
	.4byte	.L_LC600-.L_text_b
	.previous
	movl 12(%ecx),%ecx
.L_LC601:

.section	.line
	.4byte	1352	/ sas.c:1352
	.2byte	0xffff
	.4byte	.L_LC601-.L_text_b
	.previous
	movw 16(%ebx),%dx
	movw %dx,4(%ecx)
.L_LC602:

.section	.line
	.4byte	1353	/ sas.c:1353
	.2byte	0xffff
	.4byte	.L_LC602-.L_text_b
	.previous
	movw 28(%ebx),%ax
	movw %ax,6(%ecx)
.L_LC603:

.section	.line
	.4byte	1354	/ sas.c:1354
	.2byte	0xffff
	.4byte	.L_LC603-.L_text_b
	.previous
	movw 24(%ebx),%dx
	movw %dx,2(%ecx)
.L_LC604:

.section	.line
	.4byte	1355	/ sas.c:1355
	.2byte	0xffff
	.4byte	.L_LC604-.L_text_b
	.previous
	movw 12(%ebx),%ax
	movw %ax,(%ecx)
.L_LC605:

.section	.line
	.4byte	1356	/ sas.c:1356
	.2byte	0xffff
	.4byte	.L_LC605-.L_text_b
	.previous
	xorl %esi,%esi
	.align 4
.L295:
	movb 184(%esi,%ebx),%dl
	movb %dl,9(%esi,%ecx)
	incl %esi
	cmpl $7,%esi
	jle .L295
.L_LC606:

.section	.line
	.4byte	1357	/ sas.c:1357
	.2byte	0xffff
	.4byte	.L_LC606-.L_text_b
	.previous
	movl 12(%ebp),%eax
	movl 20(%eax),%eax
	movb $129,13(%eax)
.L_LC607:

.section	.line
	.4byte	1358	/ sas.c:1358
	.2byte	0xffff
	.4byte	.L_LC607-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 8(%edx),%edx
	movl %edx,-108(%ebp)
	movl -108(%ebp),%edx
	movl 12(%edx),%eax
	addl $18,%eax
	movl %eax,16(%edx)
.L_LC608:

.section	.line
	.4byte	1359	/ sas.c:1359
	.2byte	0xffff
	.4byte	.L_LC608-.L_text_b
	.previous
	movl $18,12(%edi)
.L_LC609:

.section	.line
	.4byte	1362	/ sas.c:1362
	.2byte	0xffff
	.4byte	.L_LC609-.L_text_b
	.previous
	jmp .L349
	.align 4
.L296:
.L_LC610:

.section	.line
	.4byte	1365	/ sas.c:1365
	.2byte	0xffff
	.4byte	.L_LC610-.L_text_b
	.previous
	cmpl $4,12(%edi)
	je .L297
.L_LC611:

.section	.line
	.4byte	1366	/ sas.c:1366
	.2byte	0xffff
	.4byte	.L_LC611-.L_text_b
	.previous
	pushl $.LC3
	call printf
.L_LC612:

.section	.line
	.4byte	1367	/ sas.c:1367
	.2byte	0xffff
	.4byte	.L_LC612-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 20(%edx),%edx
	movb $130,13(%edx)
.L_LC613:

.section	.line
	.4byte	1368	/ sas.c:1368
	.2byte	0xffff
	.4byte	.L_LC613-.L_text_b
	.previous
	pushl 12(%ebp)
	pushl 8(%ebp)
	call qreply
.L_LC614:

.section	.line
	.4byte	1369	/ sas.c:1369
	.2byte	0xffff
	.4byte	.L_LC614-.L_text_b
	.previous
	addl $12,%esp
	jmp .L200
	.align 4
.L297:
.L_LC615:

.section	.line
	.4byte	1371	/ sas.c:1371
	.2byte	0xffff
	.4byte	.L_LC615-.L_text_b
	.previous
	movl 12(%ebp),%eax
	movl 8(%eax),%eax
	movl 12(%eax),%edx
	movl (%edx),%ecx
	testl %ecx,%ecx
	je .L299
	cmpl $1,%ecx
	je .L300
	jmp .L301
	.align 4
.L299:
.L_LC616:

.section	.line
	.4byte	1373	/ sas.c:1373
	.2byte	0xffff
	.4byte	.L_LC616-.L_text_b
	.previous
	pushl $2
.L_LC617:

.section	.line
	.4byte	1374	/ sas.c:1374
	.2byte	0xffff
	.4byte	.L_LC617-.L_text_b
	.previous
	jmp .L352
	.align 4
.L300:
.L_LC618:

.section	.line
	.4byte	1376	/ sas.c:1376
	.2byte	0xffff
	.4byte	.L_LC618-.L_text_b
	.previous
	pushl $3
.L352:
	pushl %ebx
	call sas_cmd
.L_LC619:

.section	.line
	.4byte	1377	/ sas.c:1377
	.2byte	0xffff
	.4byte	.L_LC619-.L_text_b
	.previous
	addl $8,%esp
	jmp .L298
	.align 4
.L301:
.L_LC620:

.section	.line
	.4byte	1379	/ sas.c:1379
	.2byte	0xffff
	.4byte	.L_LC620-.L_text_b
	.previous
	movl $22,16(%edi)
.L_LC621:

.section	.line
	.4byte	1380	/ sas.c:1380
	.2byte	0xffff
	.4byte	.L_LC621-.L_text_b
	.previous
	pushl $.LC4
	call printf
.L_LC622:

.section	.line
	.4byte	1381	/ sas.c:1381
	.2byte	0xffff
	.4byte	.L_LC622-.L_text_b
	.previous
	addl $4,%esp
.L298:
.L_LC623:

.section	.line
	.4byte	1383	/ sas.c:1383
	.2byte	0xffff
	.4byte	.L_LC623-.L_text_b
	.previous
	movl 12(%ebp),%eax
	movl 20(%eax),%eax
	movb $129,13(%eax)
.L_LC624:

.section	.line
	.4byte	1384	/ sas.c:1384
	.2byte	0xffff
	.4byte	.L_LC624-.L_text_b
	.previous
	movl $0,12(%edi)
.L_LC625:

.section	.line
	.4byte	1386	/ sas.c:1386
	.2byte	0xffff
	.4byte	.L_LC625-.L_text_b
	.previous
	jmp .L349
	.align 4
.L306:
.L_LC626:

.section	.line
	.4byte	1393	/ sas.c:1393
	.2byte	0xffff
	.4byte	.L_LC626-.L_text_b
	.previous
	movl $25,16(%edi)
.L_LC627:

.section	.line
	.4byte	1394	/ sas.c:1394
	.2byte	0xffff
	.4byte	.L_LC627-.L_text_b
	.previous
	movl $0,12(%edi)
.L_LC628:

.section	.line
	.4byte	1395	/ sas.c:1395
	.2byte	0xffff
	.4byte	.L_LC628-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 20(%edx),%edx
	movb $130,13(%edx)
.L_LC629:

.section	.line
	.4byte	1397	/ sas.c:1397
	.2byte	0xffff
	.4byte	.L_LC629-.L_text_b
	.previous
	jmp .L349
	.align 4
.L308:
.L_LC630:

.section	.line
	.4byte	1402	/ sas.c:1402
	.2byte	0xffff
	.4byte	.L_LC630-.L_text_b
	.previous
	movl 12(%ebp),%eax
	movl 20(%eax),%eax
	movb $129,13(%eax)
.L_LC631:

.section	.line
	.4byte	1403	/ sas.c:1403
	.2byte	0xffff
	.4byte	.L_LC631-.L_text_b
	.previous
	movl $0,12(%edi)
.L_LC632:

.section	.line
	.4byte	1405	/ sas.c:1405
	.2byte	0xffff
	.4byte	.L_LC632-.L_text_b
	.previous
	jmp .L349
	.align 4
.L309:
.L_LC633:

.section	.line
	.4byte	1409	/ sas.c:1409
	.2byte	0xffff
	.4byte	.L_LC633-.L_text_b
	.previous
	movl 12(%ebp),%edx
	cmpl $0,8(%edx)
	je .L310
	pushl 8(%edx)
	call freemsg
	addl $4,%esp
.L310:
.L_LC634:

.section	.line
	.4byte	1410	/ sas.c:1410
	.2byte	0xffff
	.4byte	.L_LC634-.L_text_b
	.previous
	pushl $2
	pushl $8
	call allocb
	movl %eax,%ecx
	movl 12(%ebp),%eax
	movl %ecx,8(%eax)
	addl $8,%esp
	testl %ecx,%ecx
	jne .L311
.L_LC635:

.section	.line
	.4byte	1411	/ sas.c:1411
	.2byte	0xffff
	.4byte	.L_LC635-.L_text_b
	.previous
	movl 20(%eax),%edx
	movb $130,13(%edx)
.L_LC636:

.section	.line
	.4byte	1412	/ sas.c:1412
	.2byte	0xffff
	.4byte	.L_LC636-.L_text_b
	.previous
	movl $11,16(%edi)
.L_LC637:

.section	.line
	.4byte	1413	/ sas.c:1413
	.2byte	0xffff
	.4byte	.L_LC637-.L_text_b
	.previous
	pushl %eax
.L_LC638:

.section	.line
	.4byte	1414	/ sas.c:1414
	.2byte	0xffff
	.4byte	.L_LC638-.L_text_b
	.previous
	jmp .L348
	.align 4
.L311:
.L_LC639:

.section	.line
	.4byte	1416	/ sas.c:1416
	.2byte	0xffff
	.4byte	.L_LC639-.L_text_b
	.previous
	movl 12(%ecx),%esi
.L_LC640:

.section	.line
	.4byte	1417	/ sas.c:1417
	.2byte	0xffff
	.4byte	.L_LC640-.L_text_b
	.previous
	movl 8(%ebp),%eax
	movl 24(%eax),%eax
	movl %eax,(%esi)
.L_LC641:

.section	.line
	.4byte	1418	/ sas.c:1418
	.2byte	0xffff
	.4byte	.L_LC641-.L_text_b
	.previous
	cmpl $0,160(%ebx)
	je .L312
	pushl 160(%ebx)
	call msgdsize
	movl %eax,4(%esi)
	addl $4,%esp
	jmp .L313
	.align 4
.L312:
.L_LC642:

.section	.line
	.4byte	1419	/ sas.c:1419
	.2byte	0xffff
	.4byte	.L_LC642-.L_text_b
	.previous
	movl $0,4(%esi)
.L313:
.L_LC643:

.section	.line
	.4byte	1420	/ sas.c:1420
	.2byte	0xffff
	.4byte	.L_LC643-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 20(%edx),%edx
	movb $129,13(%edx)
.L_LC644:

.section	.line
	.4byte	1421	/ sas.c:1421
	.2byte	0xffff
	.4byte	.L_LC644-.L_text_b
	.previous
	movl $8,12(%edi)
.L_LC645:

.section	.line
	.4byte	1422	/ sas.c:1422
	.2byte	0xffff
	.4byte	.L_LC645-.L_text_b
	.previous
	movl 12(%ebp),%eax
	movl 8(%eax),%eax
	movl %eax,-140(%ebp)
	movl -140(%ebp),%eax
	movl 12(%eax),%edx
	addl $8,%edx
.L_LC646:

.section	.line
	.4byte	1424	/ sas.c:1424
	.2byte	0xffff
	.4byte	.L_LC646-.L_text_b
	.previous
	jmp .L353
	.align 4
.L314:
.L_LC647:

.section	.line
	.4byte	1427	/ sas.c:1427
	.2byte	0xffff
	.4byte	.L_LC647-.L_text_b
	.previous
	pushl (%edi)
	pushl $.LC5
	call printf
.L_LC648:

.section	.line
	.4byte	1428	/ sas.c:1428
	.2byte	0xffff
	.4byte	.L_LC648-.L_text_b
	.previous
	movl $25,16(%edi)
.L_LC649:

.section	.line
	.4byte	1429	/ sas.c:1429
	.2byte	0xffff
	.4byte	.L_LC649-.L_text_b
	.previous
	movl $0,12(%edi)
.L_LC650:

.section	.line
	.4byte	1430	/ sas.c:1430
	.2byte	0xffff
	.4byte	.L_LC650-.L_text_b
	.previous
	movl 12(%ebp),%eax
	movl 20(%eax),%eax
	movb $130,13(%eax)
.L_LC651:

.section	.line
	.4byte	1431	/ sas.c:1431
	.2byte	0xffff
	.4byte	.L_LC651-.L_text_b
	.previous
	pushl 12(%ebp)
	pushl 8(%ebp)
	call qreply
.L_LC652:

.section	.line
	.4byte	1434	/ sas.c:1434
	.2byte	0xffff
	.4byte	.L_LC652-.L_text_b
	.previous
	jmp .L354
	.align 4
.L321:
.L_LC653:

.section	.line
	.4byte	1438	/ sas.c:1438
	.2byte	0xffff
	.4byte	.L_LC653-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 16(%edx),%eax
	subl 12(%edx),%eax
	cmpl $40,%eax
	jne .L322
.L_LC654:

.section	.line
	.4byte	1439	/ sas.c:1439
	.2byte	0xffff
	.4byte	.L_LC654-.L_text_b
	.previous
	movl 12(%edx),%edi
.L_LC655:

.section	.line
	.4byte	1440	/ sas.c:1440
	.2byte	0xffff
	.4byte	.L_LC655-.L_text_b
	.previous
	cmpl $0,8(%edx)
	je .L323
	pushl 8(%edx)
	call freemsg
	addl $4,%esp
.L323:
.L_LC656:

.section	.line
	.4byte	1441	/ sas.c:1441
	.2byte	0xffff
	.4byte	.L_LC656-.L_text_b
	.previous
	pushl $2
	pushl $36
	call allocb
	movl %eax,-12(%ebp)
	movl 12(%ebp),%edx
	movl %eax,8(%edx)
	addl $8,%esp
	testl %eax,%eax
	jne .L324
.L_LC657:

.section	.line
	.4byte	1442	/ sas.c:1442
	.2byte	0xffff
	.4byte	.L_LC657-.L_text_b
	.previous
	movl 20(%edx),%eax
	movb $130,13(%eax)
.L_LC658:

.section	.line
	.4byte	1443	/ sas.c:1443
	.2byte	0xffff
	.4byte	.L_LC658-.L_text_b
	.previous
	movl $11,16(%edi)
.L_LC659:

.section	.line
	.4byte	1444	/ sas.c:1444
	.2byte	0xffff
	.4byte	.L_LC659-.L_text_b
	.previous
	pushl %edx
.L_LC660:

.section	.line
	.4byte	1445	/ sas.c:1445
	.2byte	0xffff
	.4byte	.L_LC660-.L_text_b
	.previous
	jmp .L348
	.align 4
.L324:
.L_LC661:

.section	.line
	.4byte	1447	/ sas.c:1447
	.2byte	0xffff
	.4byte	.L_LC661-.L_text_b
	.previous
	movl $3,(%edi)
.L_LC662:

.section	.line
	.4byte	1448	/ sas.c:1448
	.2byte	0xffff
	.4byte	.L_LC662-.L_text_b
	.previous
	movl 12(%ebp),%edx
	movl 8(%edx),%edx
	movl 12(%edx),%ecx
.L_LC663:

.section	.line
	.4byte	1449	/ sas.c:1449
	.2byte	0xffff
	.4byte	.L_LC663-.L_text_b
	.previous
	movl $7168,(%ecx)
.L_LC664:

.section	.line
	.4byte	1450	/ sas.c:1450
	.2byte	0xffff
	.4byte	.L_LC664-.L_text_b
	.previous
	movl $0,4(%ecx)
.L_LC665:

.section	.line
	.4byte	1451	/ sas.c:1451
	.2byte	0xffff
	.4byte	.L_LC665-.L_text_b
	.previous
	movl $0,12(%ecx)
.L_LC666:

.section	.line
	.4byte	1452	/ sas.c:1452
	.2byte	0xffff
	.4byte	.L_LC666-.L_text_b
	.previous
	movl 12(%ebp),%eax
	movl 8(%eax),%eax
	movl %eax,-156(%ebp)
	movl -156(%ebp),%eax
.L351:
	movl 12(%eax),%edx
	addl $36,%edx
.L353:
	movl %edx,16(%eax)
.L_LC667:

.section	.line
	.4byte	1455	/ sas.c:1455
	.2byte	0xffff
	.4byte	.L_LC667-.L_text_b
	.previous
.L349:
	pushl 12(%ebp)
.L348:
	pushl 8(%ebp)
	call qreply
.L_LC668:

.section	.line
	.4byte	1456	/ sas.c:1456
	.2byte	0xffff
	.4byte	.L_LC668-.L_text_b
	.previous
	addl $8,%esp
	jmp .L200
	.align 4
.L322:
.L_LC669:

.section	.line
	.4byte	1457	/ sas.c:1457
	.2byte	0xffff
	.4byte	.L_LC669-.L_text_b
	.previous
.L_B59:
	pushl $.LC6
	call printf
.L_LC670:

.section	.line
	.4byte	1458	/ sas.c:1458
	.2byte	0xffff
	.4byte	.L_LC670-.L_text_b
	.previous
	pushl 12(%ebp)
	pushl 8(%ebp)
	call sas_error
.L_LC671:

.section	.line
	.4byte	1459	/ sas.c:1459
	.2byte	0xffff
	.4byte	.L_LC671-.L_text_b
	.previous
.L_B59_e:
	addl $12,%esp
.L_LC672:

.section	.line
	.4byte	1460	/ sas.c:1460
	.2byte	0xffff
	.4byte	.L_LC672-.L_text_b
	.previous
	jmp .L200
	.align 4
.L326:
.L_LC673:

.section	.line
	.4byte	1463	/ sas.c:1463
	.2byte	0xffff
	.4byte	.L_LC673-.L_text_b
	.previous
	testb $2,36(%ebx)
	je .L327
.L_LC674:

.section	.line
	.4byte	1464	/ sas.c:1464
	.2byte	0xffff
	.4byte	.L_LC674-.L_text_b
	.previous
	movb 56(%ebx),%al
	notb %al
	movb %al,%cl
	andb 62(%ebx),%cl
	movb %cl,62(%ebx)
.L_LC675:

.section	.line
	.4byte	1465	/ sas.c:1465
	.2byte	0xffff
	.4byte	.L_LC675-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L329
.L_B60:
.L_LC676:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC676-.L_text_b
	.previous
.L_LC677:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC677-.L_text_b
	.previous
	movb 90(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC678:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC678-.L_text_b
	.previous
.L_B60_e:
.L_LC679:

.section	.line
	.4byte	1465	/ sas.c:1465
	.2byte	0xffff
	.4byte	.L_LC679-.L_text_b
	.previous
.L329:
.L_B61:
.L_LC680:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC680-.L_text_b
	.previous
.L_LC681:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC681-.L_text_b
	.previous
	movb %cl,%al
	movw 88(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC682:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC682-.L_text_b
	.previous
.L_B61_e:
.L_LC683:

.section	.line
	.4byte	1466	/ sas.c:1466
	.2byte	0xffff
	.4byte	.L_LC683-.L_text_b
	.previous
	orb $16,36(%ebx)
.L327:
.L_LC684:

.section	.line
	.4byte	1468	/ sas.c:1468
	.2byte	0xffff
	.4byte	.L_LC684-.L_text_b
	.previous
	testb $16,13(%ebx)
	je .L342
.L_LC685:

.section	.line
	.4byte	1469	/ sas.c:1469
	.2byte	0xffff
	.4byte	.L_LC685-.L_text_b
	.previous
	orw $384,36(%ebx)
.L_LC686:

.section	.line
	.4byte	1470	/ sas.c:1470
	.2byte	0xffff
	.4byte	.L_LC686-.L_text_b
	.previous
	movl 112(%ebx),%eax
	orl $32800,(%eax)
.L_LC687:

.section	.line
	.4byte	1475	/ sas.c:1475
	.2byte	0xffff
	.4byte	.L_LC687-.L_text_b
	.previous
	jmp .L356
	.align 4
.L333:
.L_LC688:

.section	.line
	.4byte	1478	/ sas.c:1478
	.2byte	0xffff
	.4byte	.L_LC688-.L_text_b
	.previous
	testb $16,36(%ebx)
	je .L334
.L_LC689:

.section	.line
	.4byte	1479	/ sas.c:1479
	.2byte	0xffff
	.4byte	.L_LC689-.L_text_b
	.previous
	andb $239,36(%ebx)
.L_LC690:

.section	.line
	.4byte	1480	/ sas.c:1480
	.2byte	0xffff
	.4byte	.L_LC690-.L_text_b
	.previous
	movb 62(%ebx),%cl
	orb 56(%ebx),%cl
	movb %cl,62(%ebx)
.L_LC691:

.section	.line
	.4byte	1481	/ sas.c:1481
	.2byte	0xffff
	.4byte	.L_LC691-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L336
.L_B62:
.L_LC692:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC692-.L_text_b
	.previous
.L_LC693:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC693-.L_text_b
	.previous
	movb 90(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC694:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC694-.L_text_b
	.previous
.L_B62_e:
.L_LC695:

.section	.line
	.4byte	1481	/ sas.c:1481
	.2byte	0xffff
	.4byte	.L_LC695-.L_text_b
	.previous
.L336:
.L_B63:
.L_LC696:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC696-.L_text_b
	.previous
.L_LC697:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC697-.L_text_b
	.previous
	movb %cl,%al
	movw 88(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC698:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC698-.L_text_b
	.previous
.L_B63_e:
.L334:
.L_LC699:

.section	.line
	.4byte	1483	/ sas.c:1483
	.2byte	0xffff
	.4byte	.L_LC699-.L_text_b
	.previous
	cmpb $0,36(%ebx)
	jge .L342
.L_LC700:

.section	.line
	.4byte	1484	/ sas.c:1484
	.2byte	0xffff
	.4byte	.L_LC700-.L_text_b
	.previous
	andb $127,36(%ebx)
.L_LC701:

.section	.line
	.4byte	1485	/ sas.c:1485
	.2byte	0xffff
	.4byte	.L_LC701-.L_text_b
	.previous
	orw $256,36(%ebx)
.L_LC702:

.section	.line
	.4byte	1486	/ sas.c:1486
	.2byte	0xffff
	.4byte	.L_LC702-.L_text_b
	.previous
	movl 112(%ebx),%eax
	orl $16416,(%eax)
.L_LC703:

.section	.line
	.4byte	1487	/ sas.c:1487
	.2byte	0xffff
	.4byte	.L_LC703-.L_text_b
	.previous
.L356:
	pushl %ebx
	call sas_hdx_check
.L_LC704:

.section	.line
	.4byte	1488	/ sas.c:1488
	.2byte	0xffff
	.4byte	.L_LC704-.L_text_b
	.previous
	pushl %ebx
	call sas_xproc
.L_LC705:

.section	.line
	.4byte	1489	/ sas.c:1489
	.2byte	0xffff
	.4byte	.L_LC705-.L_text_b
	.previous
	addl $8,%esp
.L_LC706:

.section	.line
	.4byte	1491	/ sas.c:1491
	.2byte	0xffff
	.4byte	.L_LC706-.L_text_b
	.previous
	jmp .L342
	.align 4
.L340:
.L_LC707:

.section	.line
	.4byte	1494	/ sas.c:1494
	.2byte	0xffff
	.4byte	.L_LC707-.L_text_b
	.previous
	pushl $2
.L_LC708:

.section	.line
	.4byte	1496	/ sas.c:1496
	.2byte	0xffff
	.4byte	.L_LC708-.L_text_b
	.previous
	jmp .L355
	.align 4
.L341:
.L_LC709:

.section	.line
	.4byte	1499	/ sas.c:1499
	.2byte	0xffff
	.4byte	.L_LC709-.L_text_b
	.previous
	pushl $3
.L355:
	pushl %ebx
	call sas_cmd
.L_LC710:

.section	.line
	.4byte	1500	/ sas.c:1500
	.2byte	0xffff
	.4byte	.L_LC710-.L_text_b
	.previous
	pushl 12(%ebp)
	call freemsg
.L_LC711:

.section	.line
	.4byte	1501	/ sas.c:1501
	.2byte	0xffff
	.4byte	.L_LC711-.L_text_b
	.previous
	addl $12,%esp
	jmp .L200
	.align 4
.L342:
.L_LC712:

.section	.line
	.4byte	1504	/ sas.c:1504
	.2byte	0xffff
	.4byte	.L_LC712-.L_text_b
	.previous
	pushl 12(%ebp)
	call freemsg
.L_LC713:

.section	.line
	.4byte	1505	/ sas.c:1505
	.2byte	0xffff
	.4byte	.L_LC713-.L_text_b
	.previous
	addl $4,%esp
	jmp .L200
	.align 4
.L343:
.L_LC714:

.section	.line
	.4byte	1508	/ sas.c:1508
	.2byte	0xffff
	.4byte	.L_LC714-.L_text_b
	.previous
	movl 12(%ebp),%eax
	movl 20(%eax),%eax
	movzbl 13(%eax),%edx
	pushl %edx
	pushl $.LC7
	call printf
.L_LC715:

.section	.line
	.4byte	1509	/ sas.c:1509
	.2byte	0xffff
	.4byte	.L_LC715-.L_text_b
	.previous
	pushl 12(%ebp)
	pushl 8(%ebp)
	call sas_error
.L_LC716:

.section	.line
	.4byte	1510	/ sas.c:1510
	.2byte	0xffff
	.4byte	.L_LC716-.L_text_b
	.previous
.L_B57_e:
.L354:
	addl $16,%esp
.L200:
.L_LC717:

.section	.line
	.4byte	1511	/ sas.c:1511
	.2byte	0xffff
	.4byte	.L_LC717-.L_text_b
	.previous
	pushl -4(%ebp)
	call splx
.L_LC718:

.section	.line
	.4byte	1512	/ sas.c:1512
	.2byte	0xffff
	.4byte	.L_LC718-.L_text_b
	.previous
.L198:
.L_b8_e:
	leal -172(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
.L_f8_e:
.Lfe5:
	.size	 sas_put,.Lfe5-sas_put

.section	.debug_pubnames
	.4byte	.L_P5
	.string	"sas_put"
	.previous

.section	.debug
.L_P5:
.L_D382:
	.4byte	.L_D382_e-.L_D382
	.2byte	0x6
	.2byte	0x12
	.4byte	.L_D393
	.2byte	0x38
	.string	"sas_put"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x111
	.4byte	sas_put
	.2byte	0x121
	.4byte	.L_f8_e
	.2byte	0x8041
	.4byte	.L_b8
	.2byte	0x8051
	.4byte	.L_b8_e
.L_D382_e:
.L_D394:
	.4byte	.L_D394_e-.L_D394
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D395
	.2byte	0x38
	.string	"wq"
	.2byte	0x83
	.2byte	.L_t394_e-.L_t394
.L_t394:
	.byte	0x1
	.4byte	.L_T664
.L_t394_e:
	.2byte	0x23
	.2byte	.L_l394_e-.L_l394
.L_l394:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l394_e:
.L_D394_e:
.L_D395:
	.4byte	.L_D395_e-.L_D395
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D396
	.2byte	0x38
	.string	"bp"
	.2byte	0x83
	.2byte	.L_t395_e-.L_t395
.L_t395:
	.byte	0x1
	.4byte	.L_T667
.L_t395_e:
	.2byte	0x23
	.2byte	.L_l395_e-.L_l395
.L_l395:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l395_e:
.L_D395_e:
.L_D396:
	.4byte	.L_D396_e-.L_D396
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D397
	.2byte	0x38
	.string	"regvar"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l396_e-.L_l396
.L_l396:
.L_l396_e:
.L_D396_e:
.L_D397:
	.4byte	.L_D397_e-.L_D397
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D398
	.2byte	0x38
	.string	"fp"
	.2byte	0x83
	.2byte	.L_t397_e-.L_t397
.L_t397:
	.byte	0x1
	.4byte	.L_T815
.L_t397_e:
	.2byte	0x23
	.2byte	.L_l397_e-.L_l397
.L_l397:
	.byte	0x1
	.4byte	0x1
.L_l397_e:
.L_D397_e:
.L_D398:
	.4byte	.L_D398_e-.L_D398
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D399
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t398_e-.L_t398
.L_t398:
	.byte	0x1
	.4byte	.L_T816
.L_t398_e:
	.2byte	0x23
	.2byte	.L_l398_e-.L_l398
.L_l398:
	.byte	0x1
	.4byte	0x3
.L_l398_e:
.L_D398_e:
.L_D399:
	.4byte	.L_D399_e-.L_D399
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D400
	.2byte	0x38
	.string	"ioss"
	.2byte	0x83
	.2byte	.L_t399_e-.L_t399
.L_t399:
	.byte	0x1
	.4byte	.L_T702
.L_t399_e:
	.2byte	0x23
	.2byte	.L_l399_e-.L_l399
.L_l399:
	.byte	0x1
	.4byte	0x7
.L_l399_e:
.L_D399_e:
.L_D400:
	.4byte	.L_D400_e-.L_D400
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D401
	.2byte	0x38
	.string	"s"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l400_e-.L_l400
.L_l400:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xfffffffc
	.byte	0x7
.L_l400_e:
.L_D400_e:
.L_D401:
	.4byte	.L_D401_e-.L_D401
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D402
	.2byte	0x38
	.string	"x"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l401_e-.L_l401
.L_l401:
	.byte	0x1
	.4byte	0x6
.L_l401_e:
.L_D401_e:
.L_D402:
	.4byte	.L_D402_e-.L_D402
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D403
	.2byte	0x38
	.string	"cpt"
	.2byte	0x83
	.2byte	.L_t402_e-.L_t402
.L_t402:
	.byte	0x1
	.4byte	.L_T705
.L_t402_e:
	.2byte	0x23
	.2byte	.L_l402_e-.L_l402
.L_l402:
	.byte	0x1
	.4byte	0x6
.L_l402_e:
.L_D402_e:
.L_D403:
	.4byte	.L_D403_e-.L_D403
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D404
	.2byte	0x38
	.string	"ppt"
	.2byte	0x83
	.2byte	.L_t403_e-.L_t403
.L_t403:
	.byte	0x1
	.4byte	.L_T709
.L_t403_e:
	.2byte	0x23
	.2byte	.L_l403_e-.L_l403
.L_l403:
.L_l403_e:
.L_D403_e:
.L_D404:
	.4byte	.L_D404_e-.L_D404
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D405
	.2byte	0x38
	.string	"arg"
	.2byte	0x83
	.2byte	.L_t404_e-.L_t404
.L_t404:
	.byte	0x1
	.4byte	.L_T519
.L_t404_e:
	.2byte	0x23
	.2byte	.L_l404_e-.L_l404
.L_l404:
	.byte	0x1
	.4byte	0x1
.L_l404_e:
.L_D404_e:
.L_D405:
	.4byte	.L_D405_e-.L_D405
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D406
	.2byte	0x38
	.string	"tpt"
	.2byte	0x83
	.2byte	.L_t405_e-.L_t405
.L_t405:
	.byte	0x1
	.4byte	.L_T524
.L_t405_e:
	.2byte	0x23
	.2byte	.L_l405_e-.L_l405
.L_l405:
	.byte	0x1
	.4byte	0x1
.L_l405_e:
.L_D405_e:
.L_D406:
	.4byte	.L_D406_e-.L_D406
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D407
	.2byte	0x38
	.string	"xpt"
	.2byte	0x83
	.2byte	.L_t406_e-.L_t406
.L_t406:
	.byte	0x1
	.4byte	.L_T788
.L_t406_e:
	.2byte	0x23
	.2byte	.L_l406_e-.L_l406
.L_l406:
	.byte	0x1
	.4byte	0x1
.L_l406_e:
.L_D406_e:
.L_D407:
	.4byte	.L_D407_e-.L_D407
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D408
	.2byte	0x38
	.string	"xpar"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l407_e-.L_l407
.L_l407:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xfffffff8
	.byte	0x7
.L_l407_e:
.L_D407_e:
.L_D408:
	.4byte	.L_D408_e-.L_D408
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D409
	.2byte	0x38
	.string	"ipt"
	.2byte	0x63
	.2byte	.L_t408_e-.L_t408
.L_t408:
	.byte	0x1
	.2byte	0x7
.L_t408_e:
	.2byte	0x23
	.2byte	.L_l408_e-.L_l408
.L_l408:
	.byte	0x1
	.4byte	0x1
.L_l408_e:
.L_D408_e:
.L_D409:
	.4byte	.L_D409_e-.L_D409
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D410
	.2byte	0x38
	.string	"old_spl"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l409_e-.L_l409
.L_l409:
	.byte	0x1
	.4byte	0x6
.L_l409_e:
.L_D409_e:
.L_D410:
	.4byte	.L_D410_e-.L_D410
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D411
	.2byte	0x38
	.string	"mp"
	.2byte	0x83
	.2byte	.L_t410_e-.L_t410
.L_t410:
	.byte	0x1
	.4byte	.L_T667
.L_t410_e:
	.2byte	0x23
	.2byte	.L_l410_e-.L_l410
.L_l410:
.L_l410_e:
.L_D410_e:
.L_D411:
	.4byte	.L_D411_e-.L_D411
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D412
	.2byte	0x38
	.string	"qt"
	.2byte	0x83
	.2byte	.L_t411_e-.L_t411
.L_t411:
	.byte	0x1
	.4byte	.L_T833
.L_t411_e:
	.2byte	0x23
	.2byte	.L_l411_e-.L_l411
.L_l411:
	.byte	0x1
	.4byte	0x6
.L_l411_e:
.L_D411_e:
.L_D412:
	.4byte	.L_D412_e-.L_D412
	.2byte	0xb
	.2byte	0x12
	.4byte	.L_D413
	.2byte	0x111
	.4byte	.L_B56
	.2byte	0x121
	.4byte	.L_B56_e
.L_D412_e:
.L_D414:
	.4byte	0x4
.L_D413:
	.4byte	.L_D413_e-.L_D413
	.2byte	0xb
	.2byte	0x12
	.4byte	.L_D415
	.2byte	0x111
	.4byte	.L_B57
	.2byte	0x121
	.4byte	.L_B57_e
.L_D413_e:
.L_D416:
	.4byte	.L_D416_e-.L_D416
	.2byte	0xb
	.2byte	0x12
	.4byte	.L_D417
	.2byte	0x111
	.4byte	.L_B58
	.2byte	0x121
	.4byte	.L_B58_e
.L_D416_e:
.L_D418:
	.4byte	0x4
.L_D417:
	.4byte	.L_D417_e-.L_D417
	.2byte	0xb
	.2byte	0x12
	.4byte	.L_D419
	.2byte	0x111
	.4byte	.L_B59
	.2byte	0x121
	.4byte	.L_B59_e
.L_D417_e:
.L_D420:
	.4byte	0x4
.L_D419:
	.4byte	.L_D419_e-.L_D419
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D421
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B60
	.2byte	0x121
	.4byte	.L_B60_e
.L_D419_e:
.L_D422:
	.4byte	.L_D422_e-.L_D422
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D423
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l422_e-.L_l422
.L_l422:
.L_l422_e:
.L_D422_e:
.L_D423:
	.4byte	.L_D423_e-.L_D423
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D424
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l423_e-.L_l423
.L_l423:
.L_l423_e:
.L_D423_e:
.L_D424:
	.4byte	0x4
.L_D421:
	.4byte	.L_D421_e-.L_D421
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D425
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B61
	.2byte	0x121
	.4byte	.L_B61_e
.L_D421_e:
.L_D426:
	.4byte	.L_D426_e-.L_D426
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D427
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l426_e-.L_l426
.L_l426:
.L_l426_e:
.L_D426_e:
.L_D427:
	.4byte	.L_D427_e-.L_D427
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D428
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l427_e-.L_l427
.L_l427:
.L_l427_e:
.L_D427_e:
.L_D428:
	.4byte	0x4
.L_D425:
	.4byte	.L_D425_e-.L_D425
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D429
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B62
	.2byte	0x121
	.4byte	.L_B62_e
.L_D425_e:
.L_D430:
	.4byte	.L_D430_e-.L_D430
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D431
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l430_e-.L_l430
.L_l430:
.L_l430_e:
.L_D430_e:
.L_D431:
	.4byte	.L_D431_e-.L_D431
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D432
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l431_e-.L_l431
.L_l431:
.L_l431_e:
.L_D431_e:
.L_D432:
	.4byte	0x4
.L_D429:
	.4byte	.L_D429_e-.L_D429
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D433
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B63
	.2byte	0x121
	.4byte	.L_B63_e
.L_D429_e:
.L_D434:
	.4byte	.L_D434_e-.L_D434
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D435
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l434_e-.L_l434
.L_l434:
.L_l434_e:
.L_D434_e:
.L_D435:
	.4byte	.L_D435_e-.L_D435
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D436
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l435_e-.L_l435
.L_l435:
.L_l435_e:
.L_D435_e:
.L_D436:
	.4byte	0x4
.L_D433:
	.4byte	0x4
.L_D415:
	.4byte	0x4
.L_D393:
	.4byte	.L_D393_e-.L_D393
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D437
	.set	.L_T702,.L_D393
	.2byte	0x38
	.string	"iocblk"
	.2byte	0xb6
	.4byte	0x28
.L_D393_e:
.L_D438:
	.4byte	.L_D438_e-.L_D438
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D439
	.2byte	0x38
	.string	"ioc_cmd"
	.2byte	0x142
	.4byte	.L_T702
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l438_e-.L_l438
.L_l438:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l438_e:
.L_D438_e:
.L_D439:
	.4byte	.L_D439_e-.L_D439
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D440
	.2byte	0x38
	.string	"ioc_cr"
	.2byte	0x142
	.4byte	.L_T702
	.2byte	0x83
	.2byte	.L_t439_e-.L_t439
.L_t439:
	.byte	0x1
	.4byte	.L_T223
.L_t439_e:
	.2byte	0x23
	.2byte	.L_l439_e-.L_l439
.L_l439:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l439_e:
.L_D439_e:
.L_D440:
	.4byte	.L_D440_e-.L_D440
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D441
	.2byte	0x38
	.string	"ioc_id"
	.2byte	0x142
	.4byte	.L_T702
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l440_e-.L_l440
.L_l440:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l440_e:
.L_D440_e:
.L_D441:
	.4byte	.L_D441_e-.L_D441
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D442
	.2byte	0x38
	.string	"ioc_count"
	.2byte	0x142
	.4byte	.L_T702
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l441_e-.L_l441
.L_l441:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l441_e:
.L_D441_e:
.L_D442:
	.4byte	.L_D442_e-.L_D442
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D443
	.2byte	0x38
	.string	"ioc_error"
	.2byte	0x142
	.4byte	.L_T702
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l442_e-.L_l442
.L_l442:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l442_e:
.L_D442_e:
.L_D443:
	.4byte	.L_D443_e-.L_D443
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D444
	.2byte	0x38
	.string	"ioc_rval"
	.2byte	0x142
	.4byte	.L_T702
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l443_e-.L_l443
.L_l443:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l443_e:
.L_D443_e:
.L_D444:
	.4byte	.L_D444_e-.L_D444
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D445
	.2byte	0x38
	.string	"ioc_filler"
	.2byte	0x142
	.4byte	.L_T702
	.2byte	0x72
	.4byte	.L_T704
	.2byte	0x23
	.2byte	.L_l444_e-.L_l444
.L_l444:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l444_e:
.L_D444_e:
.L_D445:
	.4byte	0x4
.L_D437:
	.4byte	.L_D437_e-.L_D437
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D446
	.set	.L_T705,.L_D437
	.2byte	0x38
	.string	"copyreq"
	.2byte	0xb6
	.4byte	0x2c
.L_D437_e:
.L_D447:
	.4byte	.L_D447_e-.L_D447
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D448
	.2byte	0x38
	.string	"cq_cmd"
	.2byte	0x142
	.4byte	.L_T705
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l447_e-.L_l447
.L_l447:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l447_e:
.L_D447_e:
.L_D448:
	.4byte	.L_D448_e-.L_D448
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D449
	.2byte	0x38
	.string	"cq_cr"
	.2byte	0x142
	.4byte	.L_T705
	.2byte	0x83
	.2byte	.L_t448_e-.L_t448
.L_t448:
	.byte	0x1
	.4byte	.L_T223
.L_t448_e:
	.2byte	0x23
	.2byte	.L_l448_e-.L_l448
.L_l448:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l448_e:
.L_D448_e:
.L_D449:
	.4byte	.L_D449_e-.L_D449
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D450
	.2byte	0x38
	.string	"cq_id"
	.2byte	0x142
	.4byte	.L_T705
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l449_e-.L_l449
.L_l449:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l449_e:
.L_D449_e:
.L_D450:
	.4byte	.L_D450_e-.L_D450
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D451
	.2byte	0x38
	.string	"cq_addr"
	.2byte	0x142
	.4byte	.L_T705
	.2byte	0x63
	.2byte	.L_t450_e-.L_t450
.L_t450:
	.byte	0x1
	.2byte	0x1
.L_t450_e:
	.2byte	0x23
	.2byte	.L_l450_e-.L_l450
.L_l450:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l450_e:
.L_D450_e:
.L_D451:
	.4byte	.L_D451_e-.L_D451
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D452
	.2byte	0x38
	.string	"cq_size"
	.2byte	0x142
	.4byte	.L_T705
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l451_e-.L_l451
.L_l451:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l451_e:
.L_D451_e:
.L_D452:
	.4byte	.L_D452_e-.L_D452
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D453
	.2byte	0x38
	.string	"cq_flag"
	.2byte	0x142
	.4byte	.L_T705
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l452_e-.L_l452
.L_l452:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l452_e:
.L_D452_e:
.L_D453:
	.4byte	.L_D453_e-.L_D453
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D454
	.2byte	0x38
	.string	"cq_private"
	.2byte	0x142
	.4byte	.L_T705
	.2byte	0x83
	.2byte	.L_t453_e-.L_t453
.L_t453:
	.byte	0x1
	.4byte	.L_T667
.L_t453_e:
	.2byte	0x23
	.2byte	.L_l453_e-.L_l453
.L_l453:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l453_e:
.L_D453_e:
.L_D454:
	.4byte	.L_D454_e-.L_D454
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D455
	.2byte	0x38
	.string	"cq_filler"
	.2byte	0x142
	.4byte	.L_T705
	.2byte	0x72
	.4byte	.L_T704
	.2byte	0x23
	.2byte	.L_l454_e-.L_l454
.L_l454:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l454_e:
.L_D454_e:
.L_D455:
	.4byte	0x4
.L_D446:
	.4byte	.L_D446_e-.L_D446
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D456
	.set	.L_T709,.L_D446
	.2byte	0x38
	.string	"copyresp"
	.2byte	0xb6
	.4byte	0x2c
.L_D446_e:
.L_D457:
	.4byte	.L_D457_e-.L_D457
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D458
	.2byte	0x38
	.string	"cp_cmd"
	.2byte	0x142
	.4byte	.L_T709
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l457_e-.L_l457
.L_l457:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l457_e:
.L_D457_e:
.L_D458:
	.4byte	.L_D458_e-.L_D458
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D459
	.2byte	0x38
	.string	"cp_cr"
	.2byte	0x142
	.4byte	.L_T709
	.2byte	0x83
	.2byte	.L_t458_e-.L_t458
.L_t458:
	.byte	0x1
	.4byte	.L_T223
.L_t458_e:
	.2byte	0x23
	.2byte	.L_l458_e-.L_l458
.L_l458:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l458_e:
.L_D458_e:
.L_D459:
	.4byte	.L_D459_e-.L_D459
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D460
	.2byte	0x38
	.string	"cp_id"
	.2byte	0x142
	.4byte	.L_T709
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l459_e-.L_l459
.L_l459:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l459_e:
.L_D459_e:
.L_D460:
	.4byte	.L_D460_e-.L_D460
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D461
	.2byte	0x38
	.string	"cp_rval"
	.2byte	0x142
	.4byte	.L_T709
	.2byte	0x63
	.2byte	.L_t460_e-.L_t460
.L_t460:
	.byte	0x1
	.2byte	0x1
.L_t460_e:
	.2byte	0x23
	.2byte	.L_l460_e-.L_l460
.L_l460:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l460_e:
.L_D460_e:
.L_D461:
	.4byte	.L_D461_e-.L_D461
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D462
	.2byte	0x38
	.string	"cp_pad1"
	.2byte	0x142
	.4byte	.L_T709
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l461_e-.L_l461
.L_l461:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l461_e:
.L_D461_e:
.L_D462:
	.4byte	.L_D462_e-.L_D462
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D463
	.2byte	0x38
	.string	"cp_pad2"
	.2byte	0x142
	.4byte	.L_T709
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l462_e-.L_l462
.L_l462:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l462_e:
.L_D462_e:
.L_D463:
	.4byte	.L_D463_e-.L_D463
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D464
	.2byte	0x38
	.string	"cp_private"
	.2byte	0x142
	.4byte	.L_T709
	.2byte	0x83
	.2byte	.L_t463_e-.L_t463
.L_t463:
	.byte	0x1
	.4byte	.L_T667
.L_t463_e:
	.2byte	0x23
	.2byte	.L_l463_e-.L_l463
.L_l463:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l463_e:
.L_D463_e:
.L_D464:
	.4byte	.L_D464_e-.L_D464
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D465
	.2byte	0x38
	.string	"cp_filler"
	.2byte	0x142
	.4byte	.L_T709
	.2byte	0x72
	.4byte	.L_T704
	.2byte	0x23
	.2byte	.L_l464_e-.L_l464
.L_l464:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l464_e:
.L_D464_e:
.L_D465:
	.4byte	0x4
.L_D456:
	.4byte	.L_D456_e-.L_D456
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D466
	.set	.L_T519,.L_D456
	.2byte	0x38
	.string	"termios"
	.2byte	0xb6
	.4byte	0x24
.L_D456_e:
.L_D467:
	.4byte	.L_D467_e-.L_D467
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D468
	.2byte	0x38
	.string	"c_iflag"
	.2byte	0x142
	.4byte	.L_T519
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l467_e-.L_l467
.L_l467:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l467_e:
.L_D467_e:
.L_D468:
	.4byte	.L_D468_e-.L_D468
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D469
	.2byte	0x38
	.string	"c_oflag"
	.2byte	0x142
	.4byte	.L_T519
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l468_e-.L_l468
.L_l468:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l468_e:
.L_D468_e:
.L_D469:
	.4byte	.L_D469_e-.L_D469
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D470
	.2byte	0x38
	.string	"c_cflag"
	.2byte	0x142
	.4byte	.L_T519
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l469_e-.L_l469
.L_l469:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l469_e:
.L_D469_e:
.L_D470:
	.4byte	.L_D470_e-.L_D470
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D471
	.2byte	0x38
	.string	"c_lflag"
	.2byte	0x142
	.4byte	.L_T519
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l470_e-.L_l470
.L_l470:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l470_e:
.L_D470_e:
.L_D471:
	.4byte	.L_D471_e-.L_D471
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D472
	.2byte	0x38
	.string	"c_cc"
	.2byte	0x142
	.4byte	.L_T519
	.2byte	0x72
	.4byte	.L_T522
	.2byte	0x23
	.2byte	.L_l471_e-.L_l471
.L_l471:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l471_e:
.L_D471_e:
.L_D472:
	.4byte	0x4
.L_D466:
	.4byte	.L_D466_e-.L_D466
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D473
	.set	.L_T524,.L_D466
	.2byte	0x38
	.string	"termio"
	.2byte	0xb6
	.4byte	0x12
.L_D466_e:
.L_D474:
	.4byte	.L_D474_e-.L_D474
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D475
	.2byte	0x38
	.string	"c_iflag"
	.2byte	0x142
	.4byte	.L_T524
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l474_e-.L_l474
.L_l474:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l474_e:
.L_D474_e:
.L_D475:
	.4byte	.L_D475_e-.L_D475
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D476
	.2byte	0x38
	.string	"c_oflag"
	.2byte	0x142
	.4byte	.L_T524
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l475_e-.L_l475
.L_l475:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l475_e:
.L_D475_e:
.L_D476:
	.4byte	.L_D476_e-.L_D476
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D477
	.2byte	0x38
	.string	"c_cflag"
	.2byte	0x142
	.4byte	.L_T524
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l476_e-.L_l476
.L_l476:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l476_e:
.L_D476_e:
.L_D477:
	.4byte	.L_D477_e-.L_D477
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D478
	.2byte	0x38
	.string	"c_lflag"
	.2byte	0x142
	.4byte	.L_T524
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l477_e-.L_l477
.L_l477:
	.byte	0x4
	.4byte	0x6
	.byte	0x7
.L_l477_e:
.L_D477_e:
.L_D478:
	.4byte	.L_D478_e-.L_D478
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D479
	.2byte	0x38
	.string	"c_line"
	.2byte	0x142
	.4byte	.L_T524
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l478_e-.L_l478
.L_l478:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l478_e:
.L_D478_e:
.L_D479:
	.4byte	.L_D479_e-.L_D479
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D480
	.2byte	0x38
	.string	"c_cc"
	.2byte	0x142
	.4byte	.L_T524
	.2byte	0x72
	.4byte	.L_T527
	.2byte	0x23
	.2byte	.L_l479_e-.L_l479
.L_l479:
	.byte	0x4
	.4byte	0x9
	.byte	0x7
.L_l479_e:
.L_D479_e:
.L_D480:
	.4byte	0x4
.L_D473:
	.4byte	.L_D473_e-.L_D473
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D481
	.set	.L_T788,.L_D473
	.2byte	0x38
	.string	"termiox"
	.2byte	0xb6
	.4byte	0x10
.L_D473_e:
.L_D482:
	.4byte	.L_D482_e-.L_D482
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D483
	.2byte	0x38
	.string	"x_hflag"
	.2byte	0x142
	.4byte	.L_T788
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l482_e-.L_l482
.L_l482:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l482_e:
.L_D482_e:
.L_D483:
	.4byte	.L_D483_e-.L_D483
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D484
	.2byte	0x38
	.string	"x_cflag"
	.2byte	0x142
	.4byte	.L_T788
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l483_e-.L_l483
.L_l483:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l483_e:
.L_D483_e:
.L_D484:
	.4byte	.L_D484_e-.L_D484
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D485
	.2byte	0x38
	.string	"x_rflag"
	.2byte	0x142
	.4byte	.L_T788
	.2byte	0x72
	.4byte	.L_T791
	.2byte	0x23
	.2byte	.L_l484_e-.L_l484
.L_l484:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l484_e:
.L_D484_e:
.L_D485:
	.4byte	.L_D485_e-.L_D485
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D486
	.2byte	0x38
	.string	"x_sflag"
	.2byte	0x142
	.4byte	.L_T788
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l485_e-.L_l485
.L_l485:
	.byte	0x4
	.4byte	0xe
	.byte	0x7
.L_l485_e:
.L_D485_e:
.L_D486:
	.4byte	0x4
.L_D481:
	.4byte	.L_D481_e-.L_D481
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D487
	.set	.L_T833,.L_D481
	.2byte	0x38
	.string	"qval"
	.2byte	0xb6
	.4byte	0x8
.L_D481_e:
.L_D488:
	.4byte	.L_D488_e-.L_D488
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D489
	.2byte	0x38
	.string	"qsize"
	.2byte	0x142
	.4byte	.L_T833
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l488_e-.L_l488
.L_l488:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l488_e:
.L_D488_e:
.L_D489:
	.4byte	.L_D489_e-.L_D489
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D490
	.2byte	0x38
	.string	"holdsize"
	.2byte	0x142
	.4byte	.L_T833
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l489_e-.L_l489
.L_l489:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l489_e:
.L_D489_e:
.L_D490:
	.4byte	0x4
.L_D487:
	.4byte	.L_D487_e-.L_D487
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D491
	.set	.L_T223,.L_D487
	.2byte	0x38
	.string	"cred"
	.2byte	0xb6
	.4byte	0x38
.L_D487_e:
.L_D492:
	.4byte	.L_D492_e-.L_D492
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D493
	.2byte	0x38
	.string	"cr_ref"
	.2byte	0x142
	.4byte	.L_T223
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l492_e-.L_l492
.L_l492:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l492_e:
.L_D492_e:
.L_D493:
	.4byte	.L_D493_e-.L_D493
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D494
	.2byte	0x38
	.string	"cr_ngroups"
	.2byte	0x142
	.4byte	.L_T223
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l493_e-.L_l493
.L_l493:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l493_e:
.L_D493_e:
.L_D494:
	.4byte	.L_D494_e-.L_D494
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D495
	.2byte	0x38
	.string	"cr_uid"
	.2byte	0x142
	.4byte	.L_T223
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l494_e-.L_l494
.L_l494:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l494_e:
.L_D494_e:
.L_D495:
	.4byte	.L_D495_e-.L_D495
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D496
	.2byte	0x38
	.string	"cr_gid"
	.2byte	0x142
	.4byte	.L_T223
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l495_e-.L_l495
.L_l495:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l495_e:
.L_D495_e:
.L_D496:
	.4byte	.L_D496_e-.L_D496
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D497
	.2byte	0x38
	.string	"cr_ruid"
	.2byte	0x142
	.4byte	.L_T223
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l496_e-.L_l496
.L_l496:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l496_e:
.L_D496_e:
.L_D497:
	.4byte	.L_D497_e-.L_D497
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D498
	.2byte	0x38
	.string	"cr_rgid"
	.2byte	0x142
	.4byte	.L_T223
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l497_e-.L_l497
.L_l497:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l497_e:
.L_D497_e:
.L_D498:
	.4byte	.L_D498_e-.L_D498
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D499
	.2byte	0x38
	.string	"cr_suid"
	.2byte	0x142
	.4byte	.L_T223
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l498_e-.L_l498
.L_l498:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l498_e:
.L_D498_e:
.L_D499:
	.4byte	.L_D499_e-.L_D499
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D500
	.2byte	0x38
	.string	"cr_sgid"
	.2byte	0x142
	.4byte	.L_T223
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l499_e-.L_l499
.L_l499:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l499_e:
.L_D499_e:
.L_D500:
	.4byte	.L_D500_e-.L_D500
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D501
	.2byte	0x38
	.string	"cr_savpriv"
	.2byte	0x142
	.4byte	.L_T223
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l500_e-.L_l500
.L_l500:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l500_e:
.L_D500_e:
.L_D501:
	.4byte	.L_D501_e-.L_D501
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D502
	.2byte	0x38
	.string	"cr_wkgpriv"
	.2byte	0x142
	.4byte	.L_T223
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l501_e-.L_l501
.L_l501:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l501_e:
.L_D501_e:
.L_D502:
	.4byte	.L_D502_e-.L_D502
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D503
	.2byte	0x38
	.string	"cr_maxpriv"
	.2byte	0x142
	.4byte	.L_T223
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l502_e-.L_l502
.L_l502:
	.byte	0x4
	.4byte	0x24
	.byte	0x7
.L_l502_e:
.L_D502_e:
.L_D503:
	.4byte	.L_D503_e-.L_D503
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D504
	.2byte	0x38
	.string	"cr_lid"
	.2byte	0x142
	.4byte	.L_T223
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l503_e-.L_l503
.L_l503:
	.byte	0x4
	.4byte	0x28
	.byte	0x7
.L_l503_e:
.L_D503_e:
.L_D504:
	.4byte	.L_D504_e-.L_D504
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D505
	.2byte	0x38
	.string	"cr_cmwlid"
	.2byte	0x142
	.4byte	.L_T223
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l504_e-.L_l504
.L_l504:
	.byte	0x4
	.4byte	0x2c
	.byte	0x7
.L_l504_e:
.L_D504_e:
.L_D505:
	.4byte	.L_D505_e-.L_D505
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D506
	.2byte	0x38
	.string	"cr_flags"
	.2byte	0x142
	.4byte	.L_T223
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l505_e-.L_l505
.L_l505:
	.byte	0x4
	.4byte	0x30
	.byte	0x7
.L_l505_e:
.L_D505_e:
.L_D506:
	.4byte	.L_D506_e-.L_D506
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D507
	.2byte	0x38
	.string	"cr_groups"
	.2byte	0x142
	.4byte	.L_T223
	.2byte	0x72
	.4byte	.L_T536
	.2byte	0x23
	.2byte	.L_l506_e-.L_l506
.L_l506:
	.byte	0x4
	.4byte	0x34
	.byte	0x7
.L_l506_e:
.L_D506_e:
.L_D507:
	.4byte	0x4
.L_D491:
	.4byte	.L_D491_e-.L_D491
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D508
	.set	.L_T704,.L_D491
	.2byte	0xa3
	.2byte	.L_s491_e-.L_s491
.L_s491:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x3
	.byte	0x8
	.2byte	0x55
	.2byte	0xa
.L_s491_e:
.L_D491_e:
.L_D508:
	.4byte	.L_D508_e-.L_D508
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D509
	.set	.L_T527,.L_D508
	.2byte	0xa3
	.2byte	.L_s508_e-.L_s508
.L_s508:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x7
	.byte	0x8
	.2byte	0x55
	.2byte	0x3
.L_s508_e:
.L_D508_e:
.L_D509:
	.4byte	.L_D509_e-.L_D509
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D510
	.set	.L_T791,.L_D509
	.2byte	0xa3
	.2byte	.L_s509_e-.L_s509
.L_s509:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x4
	.byte	0x8
	.2byte	0x55
	.2byte	0x6
.L_s509_e:
.L_D509_e:
.L_D510:
	.4byte	.L_D510_e-.L_D510
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D511
	.set	.L_T536,.L_D510
	.2byte	0xa3
	.2byte	.L_s510_e-.L_s510
.L_s510:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x7
.L_s510_e:
.L_D510_e:
	.previous
	.align 4
.globl sas_flush
	.type	 sas_flush,@function
sas_flush:
.L_LC719:

.section	.line
	.4byte	1653	/ sas.c:1653
	.2byte	0xffff
	.4byte	.L_LC719-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	pushl %esi
	pushl %ebx
	movl 8(%ebp),%esi
	movl 12(%ebp),%ebx
.L_b9:
.L_LC720:

.section	.line
	.4byte	1654	/ sas.c:1654
	.2byte	0xffff
	.4byte	.L_LC720-.L_text_b
	.previous
	movl 12(%ebx),%eax
	testb $2,(%eax)
	je .L358
.L_LC721:

.section	.line
	.4byte	1655	/ sas.c:1655
	.2byte	0xffff
	.4byte	.L_LC721-.L_text_b
	.previous
	pushl $0
	pushl %esi
	call flushq
	addl $8,%esp
.L358:
.L_LC722:

.section	.line
	.4byte	1657	/ sas.c:1657
	.2byte	0xffff
	.4byte	.L_LC722-.L_text_b
	.previous
	movl 12(%ebx),%eax
	testb $1,(%eax)
	je .L359
.L_LC723:

.section	.line
	.4byte	1658	/ sas.c:1658
	.2byte	0xffff
	.4byte	.L_LC723-.L_text_b
	.previous
	pushl $0
	movl %esi,%eax
	testb $16,28(%esi)
	jne .L360
	leal -64(%esi),%eax
.L360:
	pushl %eax
	call flushq
.L_LC724:

.section	.line
	.4byte	1659	/ sas.c:1659
	.2byte	0xffff
	.4byte	.L_LC724-.L_text_b
	.previous
	movl 12(%ebx),%eax
	andb $253,(%eax)
.L_LC725:

.section	.line
	.4byte	1660	/ sas.c:1660
	.2byte	0xffff
	.4byte	.L_LC725-.L_text_b
	.previous
	pushl %ebx
	pushl %esi
	call qreply
.L_LC726:

.section	.line
	.4byte	1661	/ sas.c:1661
	.2byte	0xffff
	.4byte	.L_LC726-.L_text_b
	.previous
	jmp .L361
	.align 4
.L359:
.L_LC727:

.section	.line
	.4byte	1662	/ sas.c:1662
	.2byte	0xffff
	.4byte	.L_LC727-.L_text_b
	.previous
	pushl %ebx
	call freemsg
.L361:
.L_LC728:

.section	.line
	.4byte	1664	/ sas.c:1664
	.2byte	0xffff
	.4byte	.L_LC728-.L_text_b
	.previous
.L_b9_e:
	leal -8(%ebp),%esp
	popl %ebx
	popl %esi
	leave
	ret
.L_f9_e:
.Lfe6:
	.size	 sas_flush,.Lfe6-sas_flush

.section	.debug_pubnames
	.4byte	.L_P6
	.string	"sas_flush"
	.previous

.section	.debug
.L_P6:
.L_D511:
	.4byte	.L_D511_e-.L_D511
	.2byte	0x6
	.2byte	0x12
	.4byte	.L_D512
	.2byte	0x38
	.string	"sas_flush"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x111
	.4byte	sas_flush
	.2byte	0x121
	.4byte	.L_f9_e
	.2byte	0x8041
	.4byte	.L_b9
	.2byte	0x8051
	.4byte	.L_b9_e
.L_D511_e:
.L_D513:
	.4byte	.L_D513_e-.L_D513
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D514
	.2byte	0x38
	.string	"wq"
	.2byte	0x83
	.2byte	.L_t513_e-.L_t513
.L_t513:
	.byte	0x1
	.4byte	.L_T664
.L_t513_e:
	.2byte	0x23
	.2byte	.L_l513_e-.L_l513
.L_l513:
	.byte	0x1
	.4byte	0x6
.L_l513_e:
.L_D513_e:
.L_D514:
	.4byte	.L_D514_e-.L_D514
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D515
	.2byte	0x38
	.string	"bp"
	.2byte	0x83
	.2byte	.L_t514_e-.L_t514
.L_t514:
	.byte	0x1
	.4byte	.L_T667
.L_t514_e:
	.2byte	0x23
	.2byte	.L_l514_e-.L_l514
.L_l514:
	.byte	0x1
	.4byte	0x3
.L_l514_e:
.L_D514_e:
.L_D515:
	.4byte	0x4
	.previous
.section	.rodata
.LC8:
	.string	"mega: bad stream message"
.text
	.align 4
	.type	 sas_wsrv,@function
sas_wsrv:
.L_LC729:

.section	.line
	.4byte	1668	/ sas.c:1668
	.2byte	0xffff
	.4byte	.L_LC729-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 8(%ebp),%eax
.L_b10:
.L_LC730:

.section	.line
	.4byte	1669	/ sas.c:1669
	.2byte	0xffff
	.4byte	.L_LC730-.L_text_b
	.previous
.L_LC731:

.section	.line
	.4byte	1678	/ sas.c:1678
	.2byte	0xffff
	.4byte	.L_LC731-.L_text_b
	.previous
	movl 20(%eax),%eax
	movl 8(%eax),%esi
.L_LC732:

.section	.line
	.4byte	1679	/ sas.c:1679
	.2byte	0xffff
	.4byte	.L_LC732-.L_text_b
	.previous
	pushl $0
	pushl %esi
	call sas_buffers
.L_LC733:

.section	.line
	.4byte	1680	/ sas.c:1680
	.2byte	0xffff
	.4byte	.L_LC733-.L_text_b
	.previous
	call splhi
	movl %eax,%edi
.L_LC734:

.section	.line
	.4byte	1681	/ sas.c:1681
	.2byte	0xffff
	.4byte	.L_LC734-.L_text_b
	.previous
	addl $8,%esp
	cmpl $0,160(%esi)
	je .L363
	movl 160(%esi),%eax
	cmpl $0,8(%eax)
	je .L363
	pushl %eax
	call msgdsize
	movl 108(%esi),%edx
	leal (%edx,%edx,4),%edx
	addl %edx,%edx
	addl $4,%esp
	cmpl %edx,%eax
	jbe .L363
.L_LC735:

.section	.line
	.4byte	1684	/ sas.c:1684
	.2byte	0xffff
	.4byte	.L_LC735-.L_text_b
	.previous
.L386:
.L_LC736:

.section	.line
	.4byte	1748	/ sas.c:1748
	.2byte	0xffff
	.4byte	.L_LC736-.L_text_b
	.previous
	pushl %edi
	call splx
.L_LC737:

.section	.line
	.4byte	1749	/ sas.c:1749
	.2byte	0xffff
	.4byte	.L_LC737-.L_text_b
	.previous
	jmp .L362
	.align 4
.L363:
.L_LC738:

.section	.line
	.4byte	1686	/ sas.c:1686
	.2byte	0xffff
	.4byte	.L_LC738-.L_text_b
	.previous
	pushl %edi
	call splx
.L_LC739:

.section	.line
	.4byte	1688	/ sas.c:1688
	.2byte	0xffff
	.4byte	.L_LC739-.L_text_b
	.previous
.L387:
	addl $4,%esp
	.align 4
.L364:
	movl 156(%esi),%eax
	testb $16,28(%eax)
	je .L366
	addl $64,%eax
.L366:
	pushl %eax
	call getq
	movl %eax,%ebx
	addl $4,%esp
	testl %ebx,%ebx
	je .L362
.L_LC740:

.section	.line
	.4byte	1690	/ sas.c:1690
	.2byte	0xffff
	.4byte	.L_LC740-.L_text_b
	.previous
	movl 20(%ebx),%eax
	movzbl 13(%eax),%eax
	cmpl $12,%eax
	je .L370
	jg .L383
	testl %eax,%eax
	je .L368
	jmp .L381
	.align 4
.L383:
	cmpl $14,%eax
	je .L378
	cmpl $141,%eax
	je .L374
	jmp .L381
	.align 4
.L368:
.L_LC741:

.section	.line
	.4byte	1692	/ sas.c:1692
	.2byte	0xffff
	.4byte	.L_LC741-.L_text_b
	.previous
	pushl %ebx
	call msgdsize
	addl $4,%esp
	testl %eax,%eax
	jne .L367
.L_LC742:

.section	.line
	.4byte	1693	/ sas.c:1693
	.2byte	0xffff
	.4byte	.L_LC742-.L_text_b
	.previous
	pushl %ebx
	call freemsg
.L_LC743:

.section	.line
	.4byte	1694	/ sas.c:1694
	.2byte	0xffff
	.4byte	.L_LC743-.L_text_b
	.previous
	jmp .L387
	.align 4
.L370:
.L_LC744:

.section	.line
	.4byte	1699	/ sas.c:1699
	.2byte	0xffff
	.4byte	.L_LC744-.L_text_b
	.previous
	cmpl $0,160(%esi)
	je .L371
.L_LC745:

.section	.line
	.4byte	1702	/ sas.c:1702
	.2byte	0xffff
	.4byte	.L_LC745-.L_text_b
	.previous
	movl 112(%esi),%eax
	orb $64,(%eax)
.L_LC746:

.section	.line
	.4byte	1704	/ sas.c:1704
	.2byte	0xffff
	.4byte	.L_LC746-.L_text_b
	.previous
	jmp .L388
	.align 4
.L371:
.L_LC747:

.section	.line
	.4byte	1700	/ sas.c:1700
	.2byte	0xffff
	.4byte	.L_LC747-.L_text_b
	.previous
	movl 112(%esi),%eax
	andl $-65,(%eax)
.L_LC748:

.section	.line
	.4byte	1701	/ sas.c:1701
	.2byte	0xffff
	.4byte	.L_LC748-.L_text_b
	.previous
.L_LC749:

.section	.line
	.4byte	1706	/ sas.c:1706
	.2byte	0xffff
	.4byte	.L_LC749-.L_text_b
	.previous
	movl 112(%esi),%eax
	orl $4096,(%eax)
.L_LC750:

.section	.line
	.4byte	1707	/ sas.c:1707
	.2byte	0xffff
	.4byte	.L_LC750-.L_text_b
	.previous
	movl 12(%ebx),%eax
	movzbl (%eax),%eax
	pushl %eax
	pushl %esi
	pushl $sas_delay
	call timeout
	movl %eax,116(%esi)
.L_LC751:

.section	.line
	.4byte	1709	/ sas.c:1709
	.2byte	0xffff
	.4byte	.L_LC751-.L_text_b
	.previous
	pushl %ebx
	call freemsg
.L_LC752:

.section	.line
	.4byte	1710	/ sas.c:1710
	.2byte	0xffff
	.4byte	.L_LC752-.L_text_b
	.previous
	addl $16,%esp
	jmp .L364
	.align 4
.L374:
.L_LC753:

.section	.line
	.4byte	1713	/ sas.c:1713
	.2byte	0xffff
	.4byte	.L_LC753-.L_text_b
	.previous
	movl 12(%ebx),%eax
.L_LC754:

.section	.line
	.4byte	1715	/ sas.c:1715
	.2byte	0xffff
	.4byte	.L_LC754-.L_text_b
	.previous
	cmpl $0,12(%eax)
	je .L375
.L_LC755:

.section	.line
	.4byte	1716	/ sas.c:1716
	.2byte	0xffff
	.4byte	.L_LC755-.L_text_b
	.previous
	pushl %ebx
	call freemsg
.L_LC756:

.section	.line
	.4byte	1717	/ sas.c:1717
	.2byte	0xffff
	.4byte	.L_LC756-.L_text_b
	.previous
	jmp .L387
	.align 4
.L375:
.L_LC757:

.section	.line
	.4byte	1719	/ sas.c:1719
	.2byte	0xffff
	.4byte	.L_LC757-.L_text_b
	.previous
	movl $0,16(%eax)
.L_LC758:

.section	.line
	.4byte	1720	/ sas.c:1720
	.2byte	0xffff
	.4byte	.L_LC758-.L_text_b
	.previous
	movl $0,12(%eax)
.L_LC759:

.section	.line
	.4byte	1721	/ sas.c:1721
	.2byte	0xffff
	.4byte	.L_LC759-.L_text_b
	.previous
	movl $0,20(%eax)
.L_LC760:

.section	.line
	.4byte	1722	/ sas.c:1722
	.2byte	0xffff
	.4byte	.L_LC760-.L_text_b
	.previous
	movl 12(%ebx),%ecx
	addl $40,%ecx
	movl %ecx,16(%ebx)
.L_LC761:

.section	.line
	.4byte	1723	/ sas.c:1723
	.2byte	0xffff
	.4byte	.L_LC761-.L_text_b
	.previous
	pushl %ebx
	pushl %esi
	call sas_intioctl
	addl $8,%esp
	testl %eax,%eax
	jl .L388
.L_LC762:

.section	.line
	.4byte	1727	/ sas.c:1727
	.2byte	0xffff
	.4byte	.L_LC762-.L_text_b
	.previous
	pushl 8(%ebx)
	call freemsg
.L_LC763:

.section	.line
	.4byte	1728	/ sas.c:1728
	.2byte	0xffff
	.4byte	.L_LC763-.L_text_b
	.previous
	movl $0,8(%ebx)
.L_LC764:

.section	.line
	.4byte	1729	/ sas.c:1729
	.2byte	0xffff
	.4byte	.L_LC764-.L_text_b
	.previous
	jmp .L387
	.align 4
.L378:
.L_LC765:

.section	.line
	.4byte	1732	/ sas.c:1732
	.2byte	0xffff
	.4byte	.L_LC765-.L_text_b
	.previous
	pushl %ebx
	pushl %esi
	call sas_intioctl
	addl $8,%esp
	testl %eax,%eax
	jge .L364
.L_LC766:

.section	.line
	.4byte	1733	/ sas.c:1733
	.2byte	0xffff
	.4byte	.L_LC766-.L_text_b
	.previous
.L388:
	pushl %ebx
	movl 156(%esi),%eax
	testb $16,28(%eax)
	je .L380
	addl $64,%eax
.L380:
	pushl %eax
	call putbq
.L_LC767:

.section	.line
	.4byte	1734	/ sas.c:1734
	.2byte	0xffff
	.4byte	.L_LC767-.L_text_b
	.previous
	jmp .L362
	.align 4
.L381:
.L_LC768:

.section	.line
	.4byte	1739	/ sas.c:1739
	.2byte	0xffff
	.4byte	.L_LC768-.L_text_b
	.previous
	pushl $.LC8
	pushl $3
	call cmn_err
.L_LC769:

.section	.line
	.4byte	1740	/ sas.c:1740
	.2byte	0xffff
	.4byte	.L_LC769-.L_text_b
	.previous
	pushl %ebx
	call freemsg
.L_LC770:

.section	.line
	.4byte	1741	/ sas.c:1741
	.2byte	0xffff
	.4byte	.L_LC770-.L_text_b
	.previous
	addl $12,%esp
	jmp .L364
	.align 4
.L367:
.L_LC771:

.section	.line
	.4byte	1744	/ sas.c:1744
	.2byte	0xffff
	.4byte	.L_LC771-.L_text_b
	.previous
	call splhi
.L_LC772:

.section	.line
	.4byte	1745	/ sas.c:1745
	.2byte	0xffff
	.4byte	.L_LC772-.L_text_b
	.previous
	pushl %ebx
	leal 160(%esi),%eax
	pushl %eax
	call sas_link
.L_LC773:

.section	.line
	.4byte	1746	/ sas.c:1746
	.2byte	0xffff
	.4byte	.L_LC773-.L_text_b
	.previous
	addl $8,%esp
	cmpl $0,160(%esi)
	je .L384
	pushl 160(%esi)
	call msgdsize
	movl 108(%esi),%edx
	leal (%edx,%edx,4),%edx
	addl %edx,%edx
	addl $4,%esp
	cmpl %edx,%eax
	ja .L386
.L384:
.L_LC774:

.section	.line
	.4byte	1751	/ sas.c:1751
	.2byte	0xffff
	.4byte	.L_LC774-.L_text_b
	.previous
	pushl %edi
	call splx
.L_LC775:

.section	.line
	.4byte	1752	/ sas.c:1752
	.2byte	0xffff
	.4byte	.L_LC775-.L_text_b
	.previous
	movl 112(%esi),%eax
	addl $4,%esp
	testb $32,(%eax)
	jne .L364
.L_LC776:

.section	.line
	.4byte	1753	/ sas.c:1753
	.2byte	0xffff
	.4byte	.L_LC776-.L_text_b
	.previous
	orb $32,(%eax)
.L_LC777:

.section	.line
	.4byte	1754	/ sas.c:1754
	.2byte	0xffff
	.4byte	.L_LC777-.L_text_b
	.previous
	pushl %esi
	call sas_xproc
.L_LC778:

.section	.line
	.4byte	1756	/ sas.c:1756
	.2byte	0xffff
	.4byte	.L_LC778-.L_text_b
	.previous
	jmp .L387
	.align 4
.L_LC779:

.section	.line
	.4byte	1758	/ sas.c:1758
	.2byte	0xffff
	.4byte	.L_LC779-.L_text_b
	.previous
.L362:
.L_b10_e:
	leal -12(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
.L_f10_e:
.Lfe7:
	.size	 sas_wsrv,.Lfe7-sas_wsrv

.section	.debug
.L_D512:
	.4byte	.L_D512_e-.L_D512
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D516
	.2byte	0x38
	.string	"sas_wsrv"
	.2byte	0x278
	.string	""
	.2byte	0x55
	.2byte	0x7
	.2byte	0x111
	.4byte	sas_wsrv
	.2byte	0x121
	.4byte	.L_f10_e
	.2byte	0x8041
	.4byte	.L_b10
	.2byte	0x8051
	.4byte	.L_b10_e
.L_D512_e:
.L_D517:
	.4byte	.L_D517_e-.L_D517
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D518
	.2byte	0x38
	.string	"q"
	.2byte	0x83
	.2byte	.L_t517_e-.L_t517
.L_t517:
	.byte	0x1
	.4byte	.L_T664
.L_t517_e:
	.2byte	0x23
	.2byte	.L_l517_e-.L_l517
.L_l517:
	.byte	0x1
	.4byte	0x0
.L_l517_e:
.L_D517_e:
.L_D518:
	.4byte	.L_D518_e-.L_D518
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D519
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t518_e-.L_t518
.L_t518:
	.byte	0x1
	.4byte	.L_T816
.L_t518_e:
	.2byte	0x23
	.2byte	.L_l518_e-.L_l518
.L_l518:
	.byte	0x1
	.4byte	0x6
.L_l518_e:
.L_D518_e:
.L_D519:
	.4byte	.L_D519_e-.L_D519
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D520
	.2byte	0x38
	.string	"wbp"
	.2byte	0x83
	.2byte	.L_t519_e-.L_t519
.L_t519:
	.byte	0x1
	.4byte	.L_T667
.L_t519_e:
	.2byte	0x23
	.2byte	.L_l519_e-.L_l519
.L_l519:
	.byte	0x1
	.4byte	0x3
.L_l519_e:
.L_D519_e:
.L_D520:
	.4byte	.L_D520_e-.L_D520
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D521
	.2byte	0x38
	.string	"p"
	.2byte	0x63
	.2byte	.L_t520_e-.L_t520
.L_t520:
	.byte	0x1
	.2byte	0x3
.L_t520_e:
	.2byte	0x23
	.2byte	.L_l520_e-.L_l520
.L_l520:
.L_l520_e:
.L_D520_e:
.L_D521:
	.4byte	.L_D521_e-.L_D521
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D522
	.2byte	0x38
	.string	"nc"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l521_e-.L_l521
.L_l521:
.L_l521_e:
.L_D521_e:
.L_D522:
	.4byte	.L_D522_e-.L_D522
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D523
	.2byte	0x38
	.string	"old_spl"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l522_e-.L_l522
.L_l522:
	.byte	0x1
	.4byte	0x7
.L_l522_e:
.L_D522_e:
.L_D523:
	.4byte	.L_D523_e-.L_D523
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D524
	.2byte	0x38
	.string	"cpt"
	.2byte	0x83
	.2byte	.L_t523_e-.L_t523
.L_t523:
	.byte	0x1
	.4byte	.L_T709
.L_t523_e:
	.2byte	0x23
	.2byte	.L_l523_e-.L_l523
.L_l523:
.L_l523_e:
.L_D523_e:
.L_D524:
	.4byte	.L_D524_e-.L_D524
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D525
	.2byte	0x38
	.string	"ipt"
	.2byte	0x83
	.2byte	.L_t524_e-.L_t524
.L_t524:
	.byte	0x1
	.4byte	.L_T702
.L_t524_e:
	.2byte	0x23
	.2byte	.L_l524_e-.L_l524
.L_l524:
	.byte	0x1
	.4byte	0x0
.L_l524_e:
.L_D524_e:
.L_D525:
	.4byte	.L_D525_e-.L_D525
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D526
	.2byte	0x38
	.string	"c"
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l525_e-.L_l525
.L_l525:
.L_l525_e:
.L_D525_e:
.L_D526:
	.4byte	0x4
	.previous
	.align 4
.globl sas_error
	.type	 sas_error,@function
sas_error:
.L_LC780:

.section	.line
	.4byte	1763	/ sas.c:1763
	.2byte	0xffff
	.4byte	.L_LC780-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	movl 12(%ebp),%eax
.L_b11:
.L_LC781:

.section	.line
	.4byte	1764	/ sas.c:1764
	.2byte	0xffff
	.4byte	.L_LC781-.L_text_b
	.previous
	movl 20(%eax),%edx
	movb $138,13(%edx)
.L_LC782:

.section	.line
	.4byte	1765	/ sas.c:1765
	.2byte	0xffff
	.4byte	.L_LC782-.L_text_b
	.previous
	movl 20(%eax),%edx
	movl 4(%edx),%edx
	movl %edx,12(%eax)
.L_LC783:

.section	.line
	.4byte	1766	/ sas.c:1766
	.2byte	0xffff
	.4byte	.L_LC783-.L_text_b
	.previous
	movl %edx,%ecx
	incl %ecx
	movl %ecx,16(%eax)
.L_LC784:

.section	.line
	.4byte	1767	/ sas.c:1767
	.2byte	0xffff
	.4byte	.L_LC784-.L_text_b
	.previous
	movl 12(%eax),%edx
	movb $5,(%edx)
.L_LC785:

.section	.line
	.4byte	1769	/ sas.c:1769
	.2byte	0xffff
	.4byte	.L_LC785-.L_text_b
	.previous
	pushl %eax
	pushl 8(%ebp)
	call qreply
.L_LC786:

.section	.line
	.4byte	1770	/ sas.c:1770
	.2byte	0xffff
	.4byte	.L_LC786-.L_text_b
	.previous
	leave
	ret
.L_b11_e:
.L_f11_e:
.Lfe8:
	.size	 sas_error,.Lfe8-sas_error

.section	.debug_pubnames
	.4byte	.L_P7
	.string	"sas_error"
	.previous

.section	.debug
.L_P7:
.L_D516:
	.4byte	.L_D516_e-.L_D516
	.2byte	0x6
	.2byte	0x12
	.4byte	.L_D527
	.2byte	0x38
	.string	"sas_error"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x111
	.4byte	sas_error
	.2byte	0x121
	.4byte	.L_f11_e
	.2byte	0x8041
	.4byte	.L_b11
	.2byte	0x8051
	.4byte	.L_b11_e
.L_D516_e:
.L_D528:
	.4byte	.L_D528_e-.L_D528
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D529
	.2byte	0x38
	.string	"wq"
	.2byte	0x83
	.2byte	.L_t528_e-.L_t528
.L_t528:
	.byte	0x1
	.4byte	.L_T664
.L_t528_e:
	.2byte	0x23
	.2byte	.L_l528_e-.L_l528
.L_l528:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l528_e:
.L_D528_e:
.L_D529:
	.4byte	.L_D529_e-.L_D529
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D530
	.2byte	0x38
	.string	"bp"
	.2byte	0x83
	.2byte	.L_t529_e-.L_t529
.L_t529:
	.byte	0x1
	.4byte	.L_T667
.L_t529_e:
	.2byte	0x23
	.2byte	.L_l529_e-.L_l529
.L_l529:
	.byte	0x1
	.4byte	0x0
.L_l529_e:
.L_D529_e:
.L_D530:
	.4byte	0x4
	.previous
.section	.rodata
.LC9:
	.string	"SAS: EINVAL 4!\n"
.text
	.align 4
	.type	 sas_intioctl,@function
sas_intioctl:
.L_LC787:

.section	.line
	.4byte	1778	/ sas.c:1778
	.2byte	0xffff
	.4byte	.L_LC787-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	subl $4,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 8(%ebp),%ebx
	movl 12(%ebp),%edi
.L_b12:
.L_LC788:

.section	.line
	.4byte	1779	/ sas.c:1779
	.2byte	0xffff
	.4byte	.L_LC788-.L_text_b
	.previous
.L_LC789:

.section	.line
	.4byte	1784	/ sas.c:1784
	.2byte	0xffff
	.4byte	.L_LC789-.L_text_b
	.previous
	movl 12(%edi),%esi
.L_LC790:

.section	.line
	.4byte	1786	/ sas.c:1786
	.2byte	0xffff
	.4byte	.L_LC790-.L_text_b
	.previous
	movl (%esi),%edx
	movl %edx,-4(%ebp)
	cmpl $21519,%edx
	je .L424
	jg .L438
	cmpl $21508,%edx
	je .L428
	jg .L439
	cmpl $21506,%edx
	je .L433
	cmpl $21507,%edx
	je .L430
	jmp .L436
	.align 4
.L439:
	cmpl $21509,-4(%ebp)
	je .L434
	cmpl $21518,-4(%ebp)
	je .L427
	jmp .L436
	.align 4
.L438:
	cmpl $22531,-4(%ebp)
	je .L417
	jg .L440
	cmpl $21520,-4(%ebp)
	je .L422
	cmpl $22530,-4(%ebp)
	je .L420
	jmp .L436
	.align 4
.L440:
	cmpl $22532,-4(%ebp)
	je .L414
	cmpl $29724,-4(%ebp)
	jg .L436
	cmpl $29722,-4(%ebp)
	jl .L436
.L_LC791:

.section	.line
	.4byte	1788	/ sas.c:1788
	.2byte	0xffff
	.4byte	.L_LC791-.L_text_b
	.previous
.L_B66:
.L_LC792:

.section	.line
	.4byte	1791	/ sas.c:1791
	.2byte	0xffff
	.4byte	.L_LC792-.L_text_b
	.previous
	movl 8(%edi),%eax
	movl 12(%eax),%ecx
.L_LC793:

.section	.line
	.4byte	1792	/ sas.c:1792
	.2byte	0xffff
	.4byte	.L_LC793-.L_text_b
	.previous
	testl $-7,(%ecx)
	jne .L442
.L_LC794:

.section	.line
	.4byte	1796	/ sas.c:1796
	.2byte	0xffff
	.4byte	.L_LC794-.L_text_b
	.previous
	xorl %esi,%esi
.L_LC795:

.section	.line
	.4byte	1797	/ sas.c:1797
	.2byte	0xffff
	.4byte	.L_LC795-.L_text_b
	.previous
	testb $2,(%ecx)
	je .L396
.L_LC796:

.section	.line
	.4byte	1798	/ sas.c:1798
	.2byte	0xffff
	.4byte	.L_LC796-.L_text_b
	.previous
	incl %esi
.L396:
.L_LC797:

.section	.line
	.4byte	1800	/ sas.c:1800
	.2byte	0xffff
	.4byte	.L_LC797-.L_text_b
	.previous
	testb $4,(%ecx)
	je .L397
.L_LC798:

.section	.line
	.4byte	1801	/ sas.c:1801
	.2byte	0xffff
	.4byte	.L_LC798-.L_text_b
	.previous
	orl $1,%esi
.L397:
.L_LC799:

.section	.line
	.4byte	1803	/ sas.c:1803
	.2byte	0xffff
	.4byte	.L_LC799-.L_text_b
	.previous
	movb 62(%ebx),%cl
.L_LC800:

.section	.line
	.4byte	1804	/ sas.c:1804
	.2byte	0xffff
	.4byte	.L_LC800-.L_text_b
	.previous
	cmpl $29723,-4(%ebp)
	je .L399
	jg .L404
	cmpl $29722,-4(%ebp)
	je .L400
	jmp .L398
	.align 4
.L404:
	cmpl $29724,-4(%ebp)
	je .L401
	jmp .L398
	.align 4
.L399:
.L_LC801:

.section	.line
	.4byte	1806	/ sas.c:1806
	.2byte	0xffff
	.4byte	.L_LC801-.L_text_b
	.previous
	movl %esi,%eax
	orb %al,%cl
.L_LC802:

.section	.line
	.4byte	1807	/ sas.c:1807
	.2byte	0xffff
	.4byte	.L_LC802-.L_text_b
	.previous
	jmp .L398
	.align 4
.L400:
.L_LC803:

.section	.line
	.4byte	1809	/ sas.c:1809
	.2byte	0xffff
	.4byte	.L_LC803-.L_text_b
	.previous
	andb $252,%cl
.L_LC804:

.section	.line
	.4byte	1810	/ sas.c:1810
	.2byte	0xffff
	.4byte	.L_LC804-.L_text_b
	.previous
	movl %esi,%edx
	orb %dl,%cl
.L_LC805:

.section	.line
	.4byte	1811	/ sas.c:1811
	.2byte	0xffff
	.4byte	.L_LC805-.L_text_b
	.previous
	jmp .L398
	.align 4
.L401:
.L_LC806:

.section	.line
	.4byte	1813	/ sas.c:1813
	.2byte	0xffff
	.4byte	.L_LC806-.L_text_b
	.previous
	movl %esi,%eax
	notb %al
	andb %al,%cl
.L398:
.L_LC807:

.section	.line
	.4byte	1816	/ sas.c:1816
	.2byte	0xffff
	.4byte	.L_LC807-.L_text_b
	.previous
	cmpb $0,58(%ebx)
	je .L406
	movb 58(%ebx),%al
	testb %al,60(%ebx)
	je .L405
.L406:
.L_LC808:

.section	.line
	.4byte	1818	/ sas.c:1818
	.2byte	0xffff
	.4byte	.L_LC808-.L_text_b
	.previous
	testb $2,36(%ebx)
	je .L405
.L_LC809:

.section	.line
	.4byte	1819	/ sas.c:1819
	.2byte	0xffff
	.4byte	.L_LC809-.L_text_b
	.previous
	testb %cl,56(%ebx)
	je .L408
.L_LC810:

.section	.line
	.4byte	1820	/ sas.c:1820
	.2byte	0xffff
	.4byte	.L_LC810-.L_text_b
	.previous
	andb $239,36(%ebx)
.L_LC811:

.section	.line
	.4byte	1821	/ sas.c:1821
	.2byte	0xffff
	.4byte	.L_LC811-.L_text_b
	.previous
	jmp .L405
	.align 4
.L408:
.L_LC812:

.section	.line
	.4byte	1822	/ sas.c:1822
	.2byte	0xffff
	.4byte	.L_LC812-.L_text_b
	.previous
	orb $16,36(%ebx)
.L405:
.L_LC813:

.section	.line
	.4byte	1826	/ sas.c:1826
	.2byte	0xffff
	.4byte	.L_LC813-.L_text_b
	.previous
	movb %cl,62(%ebx)
.L_LC814:

.section	.line
	.4byte	1827	/ sas.c:1827
	.2byte	0xffff
	.4byte	.L_LC814-.L_text_b
	.previous
	testb $4,32(%ebx)
	je .L411
.L_B67:
.L_LC815:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC815-.L_text_b
	.previous
.L_LC816:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC816-.L_text_b
	.previous
	movb 90(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC817:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC817-.L_text_b
	.previous
.L_B67_e:
.L_LC818:

.section	.line
	.4byte	1827	/ sas.c:1827
	.2byte	0xffff
	.4byte	.L_LC818-.L_text_b
	.previous
.L411:
.L_B68:
.L_LC819:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC819-.L_text_b
	.previous
.L_LC820:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC820-.L_text_b
	.previous
	movb %cl,%al
	movw 88(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC821:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC821-.L_text_b
	.previous
.L_B68_e:
.L_LC822:

.section	.line
	.4byte	1830	/ sas.c:1830
	.2byte	0xffff
	.4byte	.L_LC822-.L_text_b
	.previous
	jmp .L443
	.align 4
.L414:
.L_LC823:

.section	.line
	.4byte	1833	/ sas.c:1833
	.2byte	0xffff
	.4byte	.L_LC823-.L_text_b
	.previous
	pushl %edi
	pushl %ebx
	call sas_tcsetx
	addl $8,%esp
	testl %eax,%eax
	jne .L444
.L_LC824:

.section	.line
	.4byte	1837	/ sas.c:1837
	.2byte	0xffff
	.4byte	.L_LC824-.L_text_b
	.previous
	jmp .L442
	.align 4
.L417:
.L_LC825:

.section	.line
	.4byte	1840	/ sas.c:1840
	.2byte	0xffff
	.4byte	.L_LC825-.L_text_b
	.previous
	cmpl $0,160(%ebx)
	jne .L445
.L_LC826:

.section	.line
	.4byte	1841	/ sas.c:1841
	.2byte	0xffff
	.4byte	.L_LC826-.L_text_b
	.previous
	movl 112(%ebx),%eax
	andl $-65,(%eax)
.L_LC827:

.section	.line
	.4byte	1842	/ sas.c:1842
	.2byte	0xffff
	.4byte	.L_LC827-.L_text_b
	.previous
.L420:
.L_LC828:

.section	.line
	.4byte	1847	/ sas.c:1847
	.2byte	0xffff
	.4byte	.L_LC828-.L_text_b
	.previous
	pushl %edi
	pushl %ebx
	call sas_tcsetx
	addl $8,%esp
	testl %eax,%eax
	jne .L391
.L_LC829:

.section	.line
	.4byte	1848	/ sas.c:1848
	.2byte	0xffff
	.4byte	.L_LC829-.L_text_b
	.previous
.L442:
	movl 20(%edi),%eax
	movb $130,13(%eax)
.L_LC830:

.section	.line
	.4byte	1849	/ sas.c:1849
	.2byte	0xffff
	.4byte	.L_LC830-.L_text_b
	.previous
	jmp .L391
	.align 4
.L422:
.L_LC831:

.section	.line
	.4byte	1851	/ sas.c:1851
	.2byte	0xffff
	.4byte	.L_LC831-.L_text_b
	.previous
	pushl %edi
	pushl %ebx
	call sas_tcsets
.L_LC832:

.section	.line
	.4byte	1853	/ sas.c:1853
	.2byte	0xffff
	.4byte	.L_LC832-.L_text_b
	.previous
	jmp .L446
	.align 4
.L424:
.L_LC833:

.section	.line
	.4byte	1856	/ sas.c:1856
	.2byte	0xffff
	.4byte	.L_LC833-.L_text_b
	.previous
	cmpl $0,160(%ebx)
	jne .L445
.L_LC834:

.section	.line
	.4byte	1857	/ sas.c:1857
	.2byte	0xffff
	.4byte	.L_LC834-.L_text_b
	.previous
	movl 112(%ebx),%eax
	andl $-65,(%eax)
.L_LC835:

.section	.line
	.4byte	1858	/ sas.c:1858
	.2byte	0xffff
	.4byte	.L_LC835-.L_text_b
	.previous
.L427:
.L_LC836:

.section	.line
	.4byte	1863	/ sas.c:1863
	.2byte	0xffff
	.4byte	.L_LC836-.L_text_b
	.previous
	pushl %edi
	pushl %ebx
	call sas_tcsets
.L_LC837:

.section	.line
	.4byte	1864	/ sas.c:1864
	.2byte	0xffff
	.4byte	.L_LC837-.L_text_b
	.previous
	addl $8,%esp
	jmp .L391
	.align 4
.L428:
.L_LC838:

.section	.line
	.4byte	1868	/ sas.c:1868
	.2byte	0xffff
	.4byte	.L_LC838-.L_text_b
	.previous
	pushl %edi
	pushl %ebx
	call sas_tcset
.L446:
	addl $8,%esp
	testl %eax,%eax
	je .L391
.L_LC839:

.section	.line
	.4byte	1869	/ sas.c:1869
	.2byte	0xffff
	.4byte	.L_LC839-.L_text_b
	.previous
.L444:
	pushl $1
	pushl $134
	movl 156(%ebx),%eax
	pushl 12(%eax)
	call putctl1
.L_LC840:

.section	.line
	.4byte	1870	/ sas.c:1870
	.2byte	0xffff
	.4byte	.L_LC840-.L_text_b
	.previous
	addl $12,%esp
.L_LC841:

.section	.line
	.4byte	1871	/ sas.c:1871
	.2byte	0xffff
	.4byte	.L_LC841-.L_text_b
	.previous
	jmp .L391
	.align 4
.L430:
.L_LC842:

.section	.line
	.4byte	1874	/ sas.c:1874
	.2byte	0xffff
	.4byte	.L_LC842-.L_text_b
	.previous
	cmpl $0,160(%ebx)
	je .L431
.L_LC843:

.section	.line
	.4byte	1877	/ sas.c:1877
	.2byte	0xffff
	.4byte	.L_LC843-.L_text_b
	.previous
.L445:
	movl 112(%ebx),%eax
	orb $64,(%eax)
.L_LC844:

.section	.line
	.4byte	1878	/ sas.c:1878
	.2byte	0xffff
	.4byte	.L_LC844-.L_text_b
	.previous
	movl $-1,%eax
	jmp .L441
	.align 4
.L431:
.L_LC845:

.section	.line
	.4byte	1875	/ sas.c:1875
	.2byte	0xffff
	.4byte	.L_LC845-.L_text_b
	.previous
	movl 112(%ebx),%eax
	andl $-65,(%eax)
.L_LC846:

.section	.line
	.4byte	1876	/ sas.c:1876
	.2byte	0xffff
	.4byte	.L_LC846-.L_text_b
	.previous
.L433:
.L_LC847:

.section	.line
	.4byte	1881	/ sas.c:1881
	.2byte	0xffff
	.4byte	.L_LC847-.L_text_b
	.previous
	pushl %edi
	pushl %ebx
	call sas_tcset
.L_LC848:

.section	.line
	.4byte	1882	/ sas.c:1882
	.2byte	0xffff
	.4byte	.L_LC848-.L_text_b
	.previous
	addl $8,%esp
	jmp .L391
	.align 4
.L434:
.L_LC849:

.section	.line
	.4byte	1885	/ sas.c:1885
	.2byte	0xffff
	.4byte	.L_LC849-.L_text_b
	.previous
	movl 8(%edi),%eax
	movl 12(%eax),%eax
	cmpl $0,(%eax)
	jne .L435
	pushl $8
	pushl %ebx
	call sas_cmd
	addl $8,%esp
.L435:
.L_LC850:

.section	.line
	.4byte	1886	/ sas.c:1886
	.2byte	0xffff
	.4byte	.L_LC850-.L_text_b
	.previous
	movl $0,12(%esi)
.L_LC851:

.section	.line
	.4byte	1887	/ sas.c:1887
	.2byte	0xffff
	.4byte	.L_LC851-.L_text_b
	.previous
.L443:
	movl 20(%edi),%eax
	movb $129,13(%eax)
.L_LC852:

.section	.line
	.4byte	1888	/ sas.c:1888
	.2byte	0xffff
	.4byte	.L_LC852-.L_text_b
	.previous
	jmp .L391
	.align 4
.L436:
.L_LC853:

.section	.line
	.4byte	1891	/ sas.c:1891
	.2byte	0xffff
	.4byte	.L_LC853-.L_text_b
	.previous
	movl $22,16(%esi)
.L_LC854:

.section	.line
	.4byte	1892	/ sas.c:1892
	.2byte	0xffff
	.4byte	.L_LC854-.L_text_b
	.previous
	pushl $.LC9
	call printf
.L_LC855:

.section	.line
	.4byte	1893	/ sas.c:1893
	.2byte	0xffff
	.4byte	.L_LC855-.L_text_b
	.previous
	movl $0,12(%esi)
.L_LC856:

.section	.line
	.4byte	1894	/ sas.c:1894
	.2byte	0xffff
	.4byte	.L_LC856-.L_text_b
	.previous
	movl 20(%edi),%eax
	movb $130,13(%eax)
.L_LC857:

.section	.line
	.4byte	1895	/ sas.c:1895
	.2byte	0xffff
	.4byte	.L_LC857-.L_text_b
	.previous
	addl $4,%esp
.L_LC858:

.section	.line
	.4byte	1896	/ sas.c:1896
	.2byte	0xffff
	.4byte	.L_LC858-.L_text_b
	.previous
.L_B66_e:
.L391:
.L_LC859:

.section	.line
	.4byte	1898	/ sas.c:1898
	.2byte	0xffff
	.4byte	.L_LC859-.L_text_b
	.previous
	movl 156(%ebx),%eax
	movl 12(%eax),%eax
	movl (%eax),%edx
	pushl %edi
	pushl %eax
	movl (%edx),%eax
	call *%eax
.L_LC860:

.section	.line
	.4byte	1899	/ sas.c:1899
	.2byte	0xffff
	.4byte	.L_LC860-.L_text_b
	.previous
	xorl %eax,%eax
.L441:
.L_LC861:

.section	.line
	.4byte	1900	/ sas.c:1900
	.2byte	0xffff
	.4byte	.L_LC861-.L_text_b
	.previous
.L_b12_e:
	leal -16(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
.L_f12_e:
.Lfe9:
	.size	 sas_intioctl,.Lfe9-sas_intioctl

.section	.debug
.L_D527:
	.4byte	.L_D527_e-.L_D527
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D531
	.2byte	0x38
	.string	"sas_intioctl"
	.2byte	0x278
	.string	""
	.2byte	0x55
	.2byte	0x7
	.2byte	0x111
	.4byte	sas_intioctl
	.2byte	0x121
	.4byte	.L_f12_e
	.2byte	0x8041
	.4byte	.L_b12
	.2byte	0x8051
	.4byte	.L_b12_e
.L_D527_e:
.L_D532:
	.4byte	.L_D532_e-.L_D532
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D533
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t532_e-.L_t532
.L_t532:
	.byte	0x1
	.4byte	.L_T816
.L_t532_e:
	.2byte	0x23
	.2byte	.L_l532_e-.L_l532
.L_l532:
	.byte	0x1
	.4byte	0x3
.L_l532_e:
.L_D532_e:
.L_D533:
	.4byte	.L_D533_e-.L_D533
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D534
	.2byte	0x38
	.string	"bp"
	.2byte	0x83
	.2byte	.L_t533_e-.L_t533
.L_t533:
	.byte	0x1
	.4byte	.L_T667
.L_t533_e:
	.2byte	0x23
	.2byte	.L_l533_e-.L_l533
.L_l533:
	.byte	0x1
	.4byte	0x7
.L_l533_e:
.L_D533_e:
.L_D534:
	.4byte	.L_D534_e-.L_D534
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D535
	.2byte	0x38
	.string	"regvar"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l534_e-.L_l534
.L_l534:
.L_l534_e:
.L_D534_e:
.L_D535:
	.4byte	.L_D535_e-.L_D535
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D536
	.2byte	0x38
	.string	"ioss"
	.2byte	0x83
	.2byte	.L_t535_e-.L_t535
.L_t535:
	.byte	0x1
	.4byte	.L_T702
.L_t535_e:
	.2byte	0x23
	.2byte	.L_l535_e-.L_l535
.L_l535:
	.byte	0x1
	.4byte	0x6
.L_l535_e:
.L_D535_e:
.L_D536:
	.4byte	.L_D536_e-.L_D536
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D537
	.2byte	0x38
	.string	"stat"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l536_e-.L_l536
.L_l536:
	.byte	0x1
	.4byte	0x6
.L_l536_e:
.L_D536_e:
.L_D537:
	.4byte	.L_D537_e-.L_D537
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D538
	.2byte	0x38
	.string	"ipt"
	.2byte	0x63
	.2byte	.L_t537_e-.L_t537
.L_t537:
	.byte	0x1
	.2byte	0x9
.L_t537_e:
	.2byte	0x23
	.2byte	.L_l537_e-.L_l537
.L_l537:
	.byte	0x1
	.4byte	0x1
.L_l537_e:
.L_D537_e:
.L_D538:
	.4byte	.L_D538_e-.L_D538
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D539
	.2byte	0x38
	.string	"new_mcr"
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l538_e-.L_l538
.L_l538:
	.byte	0x1
	.4byte	0x1
.L_l538_e:
.L_D538_e:
.L_D539:
	.4byte	.L_D539_e-.L_D539
	.2byte	0xb
	.2byte	0x12
	.4byte	.L_D540
	.2byte	0x111
	.4byte	.L_B66
	.2byte	0x121
	.4byte	.L_B66_e
.L_D539_e:
.L_D541:
	.4byte	.L_D541_e-.L_D541
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D542
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B67
	.2byte	0x121
	.4byte	.L_B67_e
.L_D541_e:
.L_D543:
	.4byte	.L_D543_e-.L_D543
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D544
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l543_e-.L_l543
.L_l543:
.L_l543_e:
.L_D543_e:
.L_D544:
	.4byte	.L_D544_e-.L_D544
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D545
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l544_e-.L_l544
.L_l544:
.L_l544_e:
.L_D544_e:
.L_D545:
	.4byte	0x4
.L_D542:
	.4byte	.L_D542_e-.L_D542
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D546
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B68
	.2byte	0x121
	.4byte	.L_B68_e
.L_D542_e:
.L_D547:
	.4byte	.L_D547_e-.L_D547
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D548
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l547_e-.L_l547
.L_l547:
.L_l547_e:
.L_D547_e:
.L_D548:
	.4byte	.L_D548_e-.L_D548
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D549
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l548_e-.L_l548
.L_l548:
.L_l548_e:
.L_D548_e:
.L_D549:
	.4byte	0x4
.L_D546:
	.4byte	0x4
.L_D540:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_cmd,@function
sas_cmd:
.L_LC862:

.section	.line
	.4byte	1909	/ sas.c:1909
	.2byte	0xffff
	.4byte	.L_LC862-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 8(%ebp),%ebx
.L_b13:
.L_LC863:

.section	.line
	.4byte	1910	/ sas.c:1910
	.2byte	0xffff
	.4byte	.L_LC863-.L_text_b
	.previous
.L_LC864:

.section	.line
	.4byte	1911	/ sas.c:1911
	.2byte	0xffff
	.4byte	.L_LC864-.L_text_b
	.previous
	movl 112(%ebx),%esi
.L_LC865:

.section	.line
	.4byte	1914	/ sas.c:1914
	.2byte	0xffff
	.4byte	.L_LC865-.L_text_b
	.previous
	movl 12(%ebp),%ecx
	decl %ecx
	cmpl $7,%ecx
	ja .L448
	jmp *.L506(,%ecx,4)
	.align 4
	.align 4
.L506:
	.long .L449
	.long .L452
	.long .L453
	.long .L454
	.long .L509
	.long .L456
	.long .L477
	.long .L499
	.align 4
.L_LC866:

.section	.line
	.4byte	1916	/ sas.c:1916
	.2byte	0xffff
	.4byte	.L_LC866-.L_text_b
	.previous
.L_B70:
.L449:
.L_LC867:

.section	.line
	.4byte	1917	/ sas.c:1917
	.2byte	0xffff
	.4byte	.L_LC867-.L_text_b
	.previous
	andl $-2,(%esi)
.L_I13_22:
.L450:
.L_LC868:

.section	.line
	.4byte	1919	/ sas.c:1919
	.2byte	0xffff
	.4byte	.L_LC868-.L_text_b
	.previous
	orb $2,40(%ebx)
	cmpl $0,event_scheduled
	jne .L448
	movl $1,event_scheduled
	pushl $8
	pushl $0
	pushl $sas_event
.L_LC869:

.section	.line
	.4byte	1920	/ sas.c:1920
	.2byte	0xffff
	.4byte	.L_LC869-.L_text_b
	.previous
	jmp .L508
	.align 4
.L452:
.L_LC870:

.section	.line
	.4byte	1923	/ sas.c:1923
	.2byte	0xffff
	.4byte	.L_LC870-.L_text_b
	.previous
	orb $64,36(%ebx)
.L_LC871:

.section	.line
	.4byte	1924	/ sas.c:1924
	.2byte	0xffff
	.4byte	.L_LC871-.L_text_b
	.previous
	orl $256,(%esi)
.L_LC872:

.section	.line
	.4byte	1925	/ sas.c:1925
	.2byte	0xffff
	.4byte	.L_LC872-.L_text_b
	.previous
	jmp .L448
	.align 4
.L453:
.L_LC873:

.section	.line
	.4byte	1928	/ sas.c:1928
	.2byte	0xffff
	.4byte	.L_LC873-.L_text_b
	.previous
	andb $191,36(%ebx)
.L_LC874:

.section	.line
	.4byte	1929	/ sas.c:1929
	.2byte	0xffff
	.4byte	.L_LC874-.L_text_b
	.previous
	andl $-257,(%esi)
.L_LC875:

.section	.line
	.4byte	1930	/ sas.c:1930
	.2byte	0xffff
	.4byte	.L_LC875-.L_text_b
	.previous
	pushl %ebx
	call sas_xproc
.L_LC876:

.section	.line
	.4byte	1931	/ sas.c:1931
	.2byte	0xffff
	.4byte	.L_LC876-.L_text_b
	.previous
	jmp .L448
	.align 4
.L454:
.L_LC877:

.section	.line
	.4byte	1934	/ sas.c:1934
	.2byte	0xffff
	.4byte	.L_LC877-.L_text_b
	.previous
	orb $8,(%esi)
.L_LC878:

.section	.line
	.4byte	1935	/ sas.c:1935
	.2byte	0xffff
	.4byte	.L_LC878-.L_text_b
	.previous
	jmp .L448
	.align 4
.L456:
.L_LC879:

.section	.line
	.4byte	1942	/ sas.c:1942
	.2byte	0xffff
	.4byte	.L_LC879-.L_text_b
	.previous
	cmpb $2,66(%ebx)
	jne .L457
.L_LC880:

.section	.line
	.4byte	1943	/ sas.c:1943
	.2byte	0xffff
	.4byte	.L_LC880-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L459
.L_B71:
.L_LC881:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC881-.L_text_b
	.previous
.L_LC882:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC882-.L_text_b
	.previous
	movb 82(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC883:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC883-.L_text_b
	.previous
.L_B71_e:
.L_LC884:

.section	.line
	.4byte	1944	/ sas.c:1944
	.2byte	0xffff
	.4byte	.L_LC884-.L_text_b
	.previous
.L459:
.L_B72:
.L_LC885:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC885-.L_text_b
	.previous
.L_LC886:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC886-.L_text_b
	.previous
	movb $-125,%al
	movw 80(%ebx),%dx
.L_LC887:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC887-.L_text_b
	.previous
.L_B72_e:
.L_LC888:

.section	.line
	.4byte	1944	/ sas.c:1944
	.2byte	0xffff
	.4byte	.L_LC888-.L_text_b
	.previous
	jmp .L510
	.align 4
.L457:
.L_LC889:

.section	.line
	.4byte	1945	/ sas.c:1945
	.2byte	0xffff
	.4byte	.L_LC889-.L_text_b
	.previous
	cmpb $1,66(%ebx)
	jne .L462
.L_LC890:

.section	.line
	.4byte	1947	/ sas.c:1947
	.2byte	0xffff
	.4byte	.L_LC890-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L465
.L_B73:
.L_LC891:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC891-.L_text_b
	.previous
.L_LC892:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC892-.L_text_b
	.previous
	movb 82(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC893:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC893-.L_text_b
	.previous
.L_B73_e:
.L_LC894:

.section	.line
	.4byte	1947	/ sas.c:1947
	.2byte	0xffff
	.4byte	.L_LC894-.L_text_b
	.previous
.L465:
.L_B74:
.L_LC895:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC895-.L_text_b
	.previous
	movw 80(%ebx),%di
.L_LC896:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC896-.L_text_b
	.previous
	movb $32,%al
	movl %edi,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC897:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC897-.L_text_b
	.previous
.L_B74_e:
.L_LC898:

.section	.line
	.4byte	1948	/ sas.c:1948
	.2byte	0xffff
	.4byte	.L_LC898-.L_text_b
	.previous
	movb 32(%ebx),%cl
	andb $4,%cl
	je .L469
.L_B75:
.L_LC899:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC899-.L_text_b
	.previous
.L_LC900:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC900-.L_text_b
	.previous
	movb 94(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC901:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC901-.L_text_b
	.previous
.L_B75_e:
.L_LC902:

.section	.line
	.4byte	1948	/ sas.c:1948
	.2byte	0xffff
	.4byte	.L_LC902-.L_text_b
	.previous
.L469:
.L_B76:
.L_LC903:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC903-.L_text_b
	.previous
.L_LC904:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC904-.L_text_b
	.previous
	movb $48,%al
	movw 92(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC905:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC905-.L_text_b
	.previous
.L_B76_e:
.L_LC906:

.section	.line
	.4byte	1949	/ sas.c:1949
	.2byte	0xffff
	.4byte	.L_LC906-.L_text_b
	.previous
	testb %cl,%cl
	je .L473
.L_B77:
.L_LC907:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC907-.L_text_b
	.previous
.L_LC908:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC908-.L_text_b
	.previous
	movb 82(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC909:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC909-.L_text_b
	.previous
.L_B77_e:
.L_LC910:

.section	.line
	.4byte	1949	/ sas.c:1949
	.2byte	0xffff
	.4byte	.L_LC910-.L_text_b
	.previous
.L473:
.L_B78:
.L_LC911:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC911-.L_text_b
	.previous
.L_LC912:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC912-.L_text_b
	.previous
	xorb %al,%al
	movl %edi,%edx
.L510:
/APP
	outb %al,%dx
/NO_APP
.L_LC913:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC913-.L_text_b
	.previous
.L_B78_e:
.L462:
.L_LC914:

.section	.line
	.4byte	1952	/ sas.c:1952
	.2byte	0xffff
	.4byte	.L_LC914-.L_text_b
	.previous
	call splhi
	movl %eax,%ecx
.L_LC915:

.section	.line
	.4byte	1953	/ sas.c:1953
	.2byte	0xffff
	.4byte	.L_LC915-.L_text_b
	.previous
	cmpl $0,176(%ebx)
	je .L476
.L_LC916:

.section	.line
	.4byte	1954	/ sas.c:1954
	.2byte	0xffff
	.4byte	.L_LC916-.L_text_b
	.previous
	movl 176(%ebx),%eax
	movl 12(%eax),%edx
	movl %edx,16(%eax)
.L_LC917:

.section	.line
	.4byte	1955	/ sas.c:1955
	.2byte	0xffff
	.4byte	.L_LC917-.L_text_b
	.previous
	movl $0,104(%ebx)
.L476:
.L_LC918:

.section	.line
	.4byte	1957	/ sas.c:1957
	.2byte	0xffff
	.4byte	.L_LC918-.L_text_b
	.previous
	pushl %ecx
	call splx
.L_LC919:

.section	.line
	.4byte	1958	/ sas.c:1958
	.2byte	0xffff
	.4byte	.L_LC919-.L_text_b
	.previous
.L509:
	andl $-9,(%esi)
.L_LC920:

.section	.line
	.4byte	1960	/ sas.c:1960
	.2byte	0xffff
	.4byte	.L_LC920-.L_text_b
	.previous
	jmp .L448
	.align 4
.L477:
.L_LC921:

.section	.line
	.4byte	1963	/ sas.c:1963
	.2byte	0xffff
	.4byte	.L_LC921-.L_text_b
	.previous
	cmpb $2,66(%ebx)
	jne .L478
.L_LC922:

.section	.line
	.4byte	1964	/ sas.c:1964
	.2byte	0xffff
	.4byte	.L_LC922-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L480
.L_B79:
.L_LC923:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC923-.L_text_b
	.previous
.L_LC924:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC924-.L_text_b
	.previous
	movb 82(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC925:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC925-.L_text_b
	.previous
.L_B79_e:
.L_LC926:

.section	.line
	.4byte	1965	/ sas.c:1965
	.2byte	0xffff
	.4byte	.L_LC926-.L_text_b
	.previous
.L480:
.L_B80:
.L_LC927:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC927-.L_text_b
	.previous
.L_LC928:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC928-.L_text_b
	.previous
	movb $-123,%al
	movw 80(%ebx),%dx
.L_LC929:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC929-.L_text_b
	.previous
.L_B80_e:
.L_LC930:

.section	.line
	.4byte	1965	/ sas.c:1965
	.2byte	0xffff
	.4byte	.L_LC930-.L_text_b
	.previous
	jmp .L511
	.align 4
.L478:
.L_LC931:

.section	.line
	.4byte	1966	/ sas.c:1966
	.2byte	0xffff
	.4byte	.L_LC931-.L_text_b
	.previous
	cmpb $1,66(%ebx)
	jne .L483
.L_LC932:

.section	.line
	.4byte	1968	/ sas.c:1968
	.2byte	0xffff
	.4byte	.L_LC932-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L486
.L_B81:
.L_LC933:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC933-.L_text_b
	.previous
.L_LC934:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC934-.L_text_b
	.previous
	movb 82(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC935:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC935-.L_text_b
	.previous
.L_B81_e:
.L_LC936:

.section	.line
	.4byte	1968	/ sas.c:1968
	.2byte	0xffff
	.4byte	.L_LC936-.L_text_b
	.previous
.L486:
.L_B82:
.L_LC937:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC937-.L_text_b
	.previous
	movw 80(%ebx),%di
.L_LC938:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC938-.L_text_b
	.previous
	movb $32,%al
	movl %edi,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC939:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC939-.L_text_b
	.previous
.L_B82_e:
.L_LC940:

.section	.line
	.4byte	1969	/ sas.c:1969
	.2byte	0xffff
	.4byte	.L_LC940-.L_text_b
	.previous
	movb 32(%ebx),%cl
	andb $4,%cl
	je .L490
.L_B83:
.L_LC941:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC941-.L_text_b
	.previous
.L_LC942:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC942-.L_text_b
	.previous
	movb 98(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC943:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC943-.L_text_b
	.previous
.L_B83_e:
.L_LC944:

.section	.line
	.4byte	1969	/ sas.c:1969
	.2byte	0xffff
	.4byte	.L_LC944-.L_text_b
	.previous
.L490:
.L_B84:
.L_LC945:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC945-.L_text_b
	.previous
.L_LC946:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC946-.L_text_b
	.previous
	movb $12,%al
	movw 96(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC947:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC947-.L_text_b
	.previous
.L_B84_e:
.L_LC948:

.section	.line
	.4byte	1970	/ sas.c:1970
	.2byte	0xffff
	.4byte	.L_LC948-.L_text_b
	.previous
	testb %cl,%cl
	je .L494
.L_B85:
.L_LC949:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC949-.L_text_b
	.previous
.L_LC950:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC950-.L_text_b
	.previous
	movb 82(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC951:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC951-.L_text_b
	.previous
.L_B85_e:
.L_LC952:

.section	.line
	.4byte	1970	/ sas.c:1970
	.2byte	0xffff
	.4byte	.L_LC952-.L_text_b
	.previous
.L494:
.L_B86:
.L_LC953:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC953-.L_text_b
	.previous
.L_LC954:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC954-.L_text_b
	.previous
	xorb %al,%al
	movl %edi,%edx
.L511:
/APP
	outb %al,%dx
/NO_APP
.L_LC955:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC955-.L_text_b
	.previous
.L_B86_e:
.L483:
.L_LC956:

.section	.line
	.4byte	1974	/ sas.c:1974
	.2byte	0xffff
	.4byte	.L_LC956-.L_text_b
	.previous
	andb $191,36(%ebx)
.L_LC957:

.section	.line
	.4byte	1975	/ sas.c:1975
	.2byte	0xffff
	.4byte	.L_LC957-.L_text_b
	.previous
	andl $-257,(%esi)
.L_LC958:

.section	.line
	.4byte	1977	/ sas.c:1977
	.2byte	0xffff
	.4byte	.L_LC958-.L_text_b
	.previous
	movl 156(%ebx),%ecx
	testb $16,28(%ecx)
	je .L497
	addl $64,%ecx
.L497:
	pushl %ecx
	call flushq
.L_LC959:

.section	.line
	.4byte	1979	/ sas.c:1979
	.2byte	0xffff
	.4byte	.L_LC959-.L_text_b
	.previous
	addl $4,%esp
	testw $2240,32(%ebx)
	jne .L448
.L_LC960:

.section	.line
	.4byte	1982	/ sas.c:1982
	.2byte	0xffff
	.4byte	.L_LC960-.L_text_b
	.previous
	andl $-33,(%esi)
.L_LC961:

.section	.line
	.4byte	1983	/ sas.c:1983
	.2byte	0xffff
	.4byte	.L_LC961-.L_text_b
	.previous
	pushl %ebx
	call sas_hdx_check
.L_LC962:

.section	.line
	.4byte	1984	/ sas.c:1984
	.2byte	0xffff
	.4byte	.L_LC962-.L_text_b
	.previous
	addl $4,%esp
	jmp .L450
	.align 4
.L499:
.L_LC963:

.section	.line
	.4byte	1989	/ sas.c:1989
	.2byte	0xffff
	.4byte	.L_LC963-.L_text_b
	.previous
	orb $128,32(%ebx)
.L_LC964:

.section	.line
	.4byte	1990	/ sas.c:1990
	.2byte	0xffff
	.4byte	.L_LC964-.L_text_b
	.previous
	orb $32,(%esi)
.L_LC965:

.section	.line
	.4byte	1991	/ sas.c:1991
	.2byte	0xffff
	.4byte	.L_LC965-.L_text_b
	.previous
	pushl %ebx
	call sas_hdx_check
.L_LC966:

.section	.line
	.4byte	1992	/ sas.c:1992
	.2byte	0xffff
	.4byte	.L_LC966-.L_text_b
	.previous
	addl $4,%esp
	testw $2112,32(%ebx)
	je .L500
.L_LC967:

.section	.line
	.4byte	1994	/ sas.c:1994
	.2byte	0xffff
	.4byte	.L_LC967-.L_text_b
	.previous
	orw $1024,32(%ebx)
.L_LC968:

.section	.line
	.4byte	1995	/ sas.c:1995
	.2byte	0xffff
	.4byte	.L_LC968-.L_text_b
	.previous
	jmp .L448
	.align 4
.L500:
.L_LC969:

.section	.line
	.4byte	1999	/ sas.c:1999
	.2byte	0xffff
	.4byte	.L_LC969-.L_text_b
	.previous
	movb 63(%ebx),%cl
	orb $64,%cl
	movb %cl,63(%ebx)
.L_LC970:

.section	.line
	.4byte	2000	/ sas.c:2000
	.2byte	0xffff
	.4byte	.L_LC970-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L503
.L_B87:
.L_LC971:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC971-.L_text_b
	.previous
.L_LC972:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC972-.L_text_b
	.previous
	movb 86(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC973:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC973-.L_text_b
	.previous
.L_B87_e:
.L_LC974:

.section	.line
	.4byte	2000	/ sas.c:2000
	.2byte	0xffff
	.4byte	.L_LC974-.L_text_b
	.previous
.L503:
.L_B88:
.L_LC975:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC975-.L_text_b
	.previous
.L_LC976:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC976-.L_text_b
	.previous
	movb %cl,%al
	movw 84(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC977:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC977-.L_text_b
	.previous
.L_B88_e:
.L_LC978:

.section	.line
	.4byte	2001	/ sas.c:2001
	.2byte	0xffff
	.4byte	.L_LC978-.L_text_b
	.previous
	pushl $25
	pushl %ebx
	pushl $sas_timeout
.L508:
	call timeout
.L_LC979:

.section	.line
	.4byte	2006	/ sas.c:2006
	.2byte	0xffff
	.4byte	.L_LC979-.L_text_b
	.previous
.L_B70_e:
.L448:
.L_LC980:

.section	.line
	.4byte	2007	/ sas.c:2007
	.2byte	0xffff
	.4byte	.L_LC980-.L_text_b
	.previous
.L_b13_e:
	leal -12(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
.L_f13_e:
.Lfe10:
	.size	 sas_cmd,.Lfe10-sas_cmd

.section	.debug
.L_D531:
	.4byte	.L_D531_e-.L_D531
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D550
	.2byte	0x38
	.string	"sas_cmd"
	.2byte	0x111
	.4byte	sas_cmd
	.2byte	0x121
	.4byte	.L_f13_e
	.2byte	0x8041
	.4byte	.L_b13
	.2byte	0x8051
	.4byte	.L_b13_e
.L_D531_e:
.L_D551:
	.4byte	.L_D551_e-.L_D551
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D552
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t551_e-.L_t551
.L_t551:
	.byte	0x1
	.4byte	.L_T816
.L_t551_e:
	.2byte	0x23
	.2byte	.L_l551_e-.L_l551
.L_l551:
	.byte	0x1
	.4byte	0x3
.L_l551_e:
.L_D551_e:
.L_D552:
	.4byte	.L_D552_e-.L_D552
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D553
	.2byte	0x38
	.string	"arg2"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l552_e-.L_l552
.L_l552:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l552_e:
.L_D552_e:
.L_D553:
	.4byte	.L_D553_e-.L_D553
	.2byte	0xa
	.2byte	0x12
	.4byte	.L_D554
	.2byte	0x38
	.string	"start_output"
	.2byte	0x111
	.4byte	.L_I13_22
.L_D553_e:
.L_D554:
	.4byte	.L_D554_e-.L_D554
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D555
	.2byte	0x38
	.string	"regvar"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l554_e-.L_l554
.L_l554:
.L_l554_e:
.L_D554_e:
.L_D555:
	.4byte	.L_D555_e-.L_D555
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D556
	.2byte	0x38
	.string	"ttyp"
	.2byte	0x63
	.2byte	.L_t555_e-.L_t555
.L_t555:
	.byte	0x1
	.2byte	0x9
.L_t555_e:
	.2byte	0x23
	.2byte	.L_l555_e-.L_l555
.L_l555:
	.byte	0x1
	.4byte	0x6
.L_l555_e:
.L_D555_e:
.L_D556:
	.4byte	.L_D556_e-.L_D556
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D557
	.2byte	0x38
	.string	"old_spl"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l556_e-.L_l556
.L_l556:
	.byte	0x1
	.4byte	0x1
.L_l556_e:
.L_D556_e:
.L_D557:
	.4byte	.L_D557_e-.L_D557
	.2byte	0xb
	.2byte	0x12
	.4byte	.L_D558
	.2byte	0x111
	.4byte	.L_B70
	.2byte	0x121
	.4byte	.L_B70_e
.L_D557_e:
.L_D559:
	.4byte	.L_D559_e-.L_D559
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D560
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B71
	.2byte	0x121
	.4byte	.L_B71_e
.L_D559_e:
.L_D561:
	.4byte	.L_D561_e-.L_D561
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D562
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l561_e-.L_l561
.L_l561:
.L_l561_e:
.L_D561_e:
.L_D562:
	.4byte	.L_D562_e-.L_D562
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D563
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l562_e-.L_l562
.L_l562:
.L_l562_e:
.L_D562_e:
.L_D563:
	.4byte	0x4
.L_D560:
	.4byte	.L_D560_e-.L_D560
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D564
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B72
	.2byte	0x121
	.4byte	.L_B72_e
.L_D560_e:
.L_D565:
	.4byte	.L_D565_e-.L_D565
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D566
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l565_e-.L_l565
.L_l565:
.L_l565_e:
.L_D565_e:
.L_D566:
	.4byte	.L_D566_e-.L_D566
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D567
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l566_e-.L_l566
.L_l566:
	.4byte	0xffffff83
.L_l566_e:
.L_D566_e:
.L_D567:
	.4byte	0x4
.L_D564:
	.4byte	.L_D564_e-.L_D564
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D568
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B73
	.2byte	0x121
	.4byte	.L_B73_e
.L_D564_e:
.L_D569:
	.4byte	.L_D569_e-.L_D569
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D570
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l569_e-.L_l569
.L_l569:
.L_l569_e:
.L_D569_e:
.L_D570:
	.4byte	.L_D570_e-.L_D570
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D571
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l570_e-.L_l570
.L_l570:
.L_l570_e:
.L_D570_e:
.L_D571:
	.4byte	0x4
.L_D568:
	.4byte	.L_D568_e-.L_D568
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D572
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B74
	.2byte	0x121
	.4byte	.L_B74_e
.L_D568_e:
.L_D573:
	.4byte	.L_D573_e-.L_D573
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D574
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l573_e-.L_l573
.L_l573:
	.byte	0x1
	.4byte	0x7
.L_l573_e:
.L_D573_e:
.L_D574:
	.4byte	.L_D574_e-.L_D574
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D575
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l574_e-.L_l574
.L_l574:
	.4byte	0x20
.L_l574_e:
.L_D574_e:
.L_D575:
	.4byte	0x4
.L_D572:
	.4byte	.L_D572_e-.L_D572
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D576
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B75
	.2byte	0x121
	.4byte	.L_B75_e
.L_D572_e:
.L_D577:
	.4byte	.L_D577_e-.L_D577
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D578
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l577_e-.L_l577
.L_l577:
.L_l577_e:
.L_D577_e:
.L_D578:
	.4byte	.L_D578_e-.L_D578
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D579
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l578_e-.L_l578
.L_l578:
.L_l578_e:
.L_D578_e:
.L_D579:
	.4byte	0x4
.L_D576:
	.4byte	.L_D576_e-.L_D576
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D580
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B76
	.2byte	0x121
	.4byte	.L_B76_e
.L_D576_e:
.L_D581:
	.4byte	.L_D581_e-.L_D581
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D582
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l581_e-.L_l581
.L_l581:
.L_l581_e:
.L_D581_e:
.L_D582:
	.4byte	.L_D582_e-.L_D582
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D583
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l582_e-.L_l582
.L_l582:
	.4byte	0x30
.L_l582_e:
.L_D582_e:
.L_D583:
	.4byte	0x4
.L_D580:
	.4byte	.L_D580_e-.L_D580
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D584
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B77
	.2byte	0x121
	.4byte	.L_B77_e
.L_D580_e:
.L_D585:
	.4byte	.L_D585_e-.L_D585
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D586
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l585_e-.L_l585
.L_l585:
.L_l585_e:
.L_D585_e:
.L_D586:
	.4byte	.L_D586_e-.L_D586
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D587
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l586_e-.L_l586
.L_l586:
.L_l586_e:
.L_D586_e:
.L_D587:
	.4byte	0x4
.L_D584:
	.4byte	.L_D584_e-.L_D584
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D588
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B78
	.2byte	0x121
	.4byte	.L_B78_e
.L_D584_e:
.L_D589:
	.4byte	.L_D589_e-.L_D589
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D590
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l589_e-.L_l589
.L_l589:
.L_l589_e:
.L_D589_e:
.L_D590:
	.4byte	.L_D590_e-.L_D590
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D591
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l590_e-.L_l590
.L_l590:
	.4byte	0x0
.L_l590_e:
.L_D590_e:
.L_D591:
	.4byte	0x4
.L_D588:
	.4byte	.L_D588_e-.L_D588
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D592
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B79
	.2byte	0x121
	.4byte	.L_B79_e
.L_D588_e:
.L_D593:
	.4byte	.L_D593_e-.L_D593
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D594
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l593_e-.L_l593
.L_l593:
.L_l593_e:
.L_D593_e:
.L_D594:
	.4byte	.L_D594_e-.L_D594
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D595
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l594_e-.L_l594
.L_l594:
.L_l594_e:
.L_D594_e:
.L_D595:
	.4byte	0x4
.L_D592:
	.4byte	.L_D592_e-.L_D592
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D596
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B80
	.2byte	0x121
	.4byte	.L_B80_e
.L_D592_e:
.L_D597:
	.4byte	.L_D597_e-.L_D597
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D598
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l597_e-.L_l597
.L_l597:
.L_l597_e:
.L_D597_e:
.L_D598:
	.4byte	.L_D598_e-.L_D598
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D599
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l598_e-.L_l598
.L_l598:
	.4byte	0xffffff85
.L_l598_e:
.L_D598_e:
.L_D599:
	.4byte	0x4
.L_D596:
	.4byte	.L_D596_e-.L_D596
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D600
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B81
	.2byte	0x121
	.4byte	.L_B81_e
.L_D596_e:
.L_D601:
	.4byte	.L_D601_e-.L_D601
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D602
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l601_e-.L_l601
.L_l601:
.L_l601_e:
.L_D601_e:
.L_D602:
	.4byte	.L_D602_e-.L_D602
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D603
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l602_e-.L_l602
.L_l602:
.L_l602_e:
.L_D602_e:
.L_D603:
	.4byte	0x4
.L_D600:
	.4byte	.L_D600_e-.L_D600
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D604
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B82
	.2byte	0x121
	.4byte	.L_B82_e
.L_D600_e:
.L_D605:
	.4byte	.L_D605_e-.L_D605
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D606
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l605_e-.L_l605
.L_l605:
	.byte	0x1
	.4byte	0x7
.L_l605_e:
.L_D605_e:
.L_D606:
	.4byte	.L_D606_e-.L_D606
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D607
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l606_e-.L_l606
.L_l606:
	.4byte	0x20
.L_l606_e:
.L_D606_e:
.L_D607:
	.4byte	0x4
.L_D604:
	.4byte	.L_D604_e-.L_D604
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D608
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B83
	.2byte	0x121
	.4byte	.L_B83_e
.L_D604_e:
.L_D609:
	.4byte	.L_D609_e-.L_D609
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D610
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l609_e-.L_l609
.L_l609:
.L_l609_e:
.L_D609_e:
.L_D610:
	.4byte	.L_D610_e-.L_D610
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D611
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l610_e-.L_l610
.L_l610:
.L_l610_e:
.L_D610_e:
.L_D611:
	.4byte	0x4
.L_D608:
	.4byte	.L_D608_e-.L_D608
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D612
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B84
	.2byte	0x121
	.4byte	.L_B84_e
.L_D608_e:
.L_D613:
	.4byte	.L_D613_e-.L_D613
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D614
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l613_e-.L_l613
.L_l613:
.L_l613_e:
.L_D613_e:
.L_D614:
	.4byte	.L_D614_e-.L_D614
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D615
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l614_e-.L_l614
.L_l614:
	.4byte	0xc
.L_l614_e:
.L_D614_e:
.L_D615:
	.4byte	0x4
.L_D612:
	.4byte	.L_D612_e-.L_D612
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D616
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B85
	.2byte	0x121
	.4byte	.L_B85_e
.L_D612_e:
.L_D617:
	.4byte	.L_D617_e-.L_D617
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D618
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l617_e-.L_l617
.L_l617:
.L_l617_e:
.L_D617_e:
.L_D618:
	.4byte	.L_D618_e-.L_D618
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D619
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l618_e-.L_l618
.L_l618:
.L_l618_e:
.L_D618_e:
.L_D619:
	.4byte	0x4
.L_D616:
	.4byte	.L_D616_e-.L_D616
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D620
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B86
	.2byte	0x121
	.4byte	.L_B86_e
.L_D616_e:
.L_D621:
	.4byte	.L_D621_e-.L_D621
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D622
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l621_e-.L_l621
.L_l621:
.L_l621_e:
.L_D621_e:
.L_D622:
	.4byte	.L_D622_e-.L_D622
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D623
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l622_e-.L_l622
.L_l622:
	.4byte	0x0
.L_l622_e:
.L_D622_e:
.L_D623:
	.4byte	0x4
.L_D620:
	.4byte	.L_D620_e-.L_D620
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D624
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B87
	.2byte	0x121
	.4byte	.L_B87_e
.L_D620_e:
.L_D625:
	.4byte	.L_D625_e-.L_D625
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D626
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l625_e-.L_l625
.L_l625:
.L_l625_e:
.L_D625_e:
.L_D626:
	.4byte	.L_D626_e-.L_D626
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D627
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l626_e-.L_l626
.L_l626:
.L_l626_e:
.L_D626_e:
.L_D627:
	.4byte	0x4
.L_D624:
	.4byte	.L_D624_e-.L_D624
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D628
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B88
	.2byte	0x121
	.4byte	.L_B88_e
.L_D624_e:
.L_D629:
	.4byte	.L_D629_e-.L_D629
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D630
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l629_e-.L_l629
.L_l629:
.L_l629_e:
.L_D629_e:
.L_D630:
	.4byte	.L_D630_e-.L_D630
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D631
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l630_e-.L_l630
.L_l630:
.L_l630_e:
.L_D630_e:
.L_D631:
	.4byte	0x4
.L_D628:
	.4byte	0x4
.L_D558:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_open_device,@function
sas_open_device:
.L_LC981:

.section	.line
	.4byte	2013	/ sas.c:2013
	.2byte	0xffff
	.4byte	.L_LC981-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	subl $4,%esp
	pushl %esi
	pushl %ebx
	movl 8(%ebp),%esi
.L_b14:
.L_LC982:

.section	.line
	.4byte	2014	/ sas.c:2014
	.2byte	0xffff
	.4byte	.L_LC982-.L_text_b
	.previous
.L_LC983:

.section	.line
	.4byte	2017	/ sas.c:2017
	.2byte	0xffff
	.4byte	.L_LC983-.L_text_b
	.previous
	testb $8,32(%esi)
	jne .L514
.L_LC984:

.section	.line
	.4byte	2021	/ sas.c:2021
	.2byte	0xffff
	.4byte	.L_LC984-.L_text_b
	.previous
	andw $16407,32(%esi)
.L_LC985:

.section	.line
	.4byte	2023	/ sas.c:2023
	.2byte	0xffff
	.4byte	.L_LC985-.L_text_b
	.previous
	movw $0,36(%esi)
.L_LC986:

.section	.line
	.4byte	2024	/ sas.c:2024
	.2byte	0xffff
	.4byte	.L_LC986-.L_text_b
	.previous
	movw $0,40(%esi)
.L_LC987:

.section	.line
	.4byte	2025	/ sas.c:2025
	.2byte	0xffff
	.4byte	.L_LC987-.L_text_b
	.previous
	movl $0,20(%esi)
.L_LC988:

.section	.line
	.4byte	2026	/ sas.c:2026
	.2byte	0xffff
	.4byte	.L_LC988-.L_text_b
	.previous
	movl $0,12(%esi)
.L_LC989:

.section	.line
	.4byte	2029	/ sas.c:2029
	.2byte	0xffff
	.4byte	.L_LC989-.L_text_b
	.previous
	movzbl 65(%esi),%eax
	movl sas_first_int_user(,%eax,4),%eax
	movl %eax,4(%esi)
.L_LC990:

.section	.line
	.4byte	2030	/ sas.c:2030
	.2byte	0xffff
	.4byte	.L_LC990-.L_text_b
	.previous
	testl %eax,%eax
	je .L515
.L_LC991:

.section	.line
	.4byte	2031	/ sas.c:2031
	.2byte	0xffff
	.4byte	.L_LC991-.L_text_b
	.previous
	movl 4(%esi),%eax
	movl %esi,(%eax)
.L515:
.L_LC992:

.section	.line
	.4byte	2032	/ sas.c:2032
	.2byte	0xffff
	.4byte	.L_LC992-.L_text_b
	.previous
	movzbl 65(%esi),%eax
	movl %esi,sas_first_int_user(,%eax,4)
.L_LC993:

.section	.line
	.4byte	2033	/ sas.c:2033
	.2byte	0xffff
	.4byte	.L_LC993-.L_text_b
	.previous
	movl $0,(%esi)
.L_LC994:

.section	.line
	.4byte	2035	/ sas.c:2035
	.2byte	0xffff
	.4byte	.L_LC994-.L_text_b
	.previous
	movb $0,63(%esi)
.L_LC995:

.section	.line
	.4byte	2036	/ sas.c:2036
	.2byte	0xffff
	.4byte	.L_LC995-.L_text_b
	.previous
	testb $6,32(%esi)
	je .L517
.L_B90:
.L_LC996:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC996-.L_text_b
	.previous
.L_LC997:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC997-.L_text_b
	.previous
	movb 86(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC998:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC998-.L_text_b
	.previous
.L_B90_e:
.L_LC999:

.section	.line
	.4byte	2036	/ sas.c:2036
	.2byte	0xffff
	.4byte	.L_LC999-.L_text_b
	.previous
.L517:
.L_B91:
.L_LC1000:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1000-.L_text_b
	.previous
.L_LC1001:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1001-.L_text_b
	.previous
	xorb %al,%al
	movw 84(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1002:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1002-.L_text_b
	.previous
.L_B91_e:
.L_LC1003:

.section	.line
	.4byte	2039	/ sas.c:2039
	.2byte	0xffff
	.4byte	.L_LC1003-.L_text_b
	.previous
	cmpb $2,66(%esi)
	jne .L520
.L_LC1004:

.section	.line
	.4byte	2040	/ sas.c:2040
	.2byte	0xffff
	.4byte	.L_LC1004-.L_text_b
	.previous
	testb $4,32(%esi)
	je .L522
.L_B92:
.L_LC1005:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1005-.L_text_b
	.previous
.L_LC1006:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1006-.L_text_b
	.previous
	movb 82(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1007:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1007-.L_text_b
	.previous
.L_B92_e:
.L_LC1008:

.section	.line
	.4byte	2040	/ sas.c:2040
	.2byte	0xffff
	.4byte	.L_LC1008-.L_text_b
	.previous
.L522:
.L_B93:
.L_LC1009:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1009-.L_text_b
	.previous
.L_LC1010:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1010-.L_text_b
	.previous
	xorb %al,%al
	movw 80(%esi),%dx
.L_LC1011:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1011-.L_text_b
	.previous
.L_B93_e:
.L_LC1012:

.section	.line
	.4byte	2040	/ sas.c:2040
	.2byte	0xffff
	.4byte	.L_LC1012-.L_text_b
	.previous
	jmp .L611
	.align 4
.L520:
.L_LC1013:

.section	.line
	.4byte	2041	/ sas.c:2041
	.2byte	0xffff
	.4byte	.L_LC1013-.L_text_b
	.previous
	cmpb $1,66(%esi)
	jne .L525
.L_LC1014:

.section	.line
	.4byte	2043	/ sas.c:2043
	.2byte	0xffff
	.4byte	.L_LC1014-.L_text_b
	.previous
	movb 32(%esi),%bl
	andb $4,%bl
	je .L528
.L_B94:
.L_LC1015:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1015-.L_text_b
	.previous
.L_LC1016:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1016-.L_text_b
	.previous
	movb 82(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1017:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1017-.L_text_b
	.previous
.L_B94_e:
.L_LC1018:

.section	.line
	.4byte	2043	/ sas.c:2043
	.2byte	0xffff
	.4byte	.L_LC1018-.L_text_b
	.previous
.L528:
.L_B95:
.L_LC1019:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1019-.L_text_b
	.previous
	movw 80(%esi),%cx
.L_LC1020:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1020-.L_text_b
	.previous
	movb $32,%al
	movl %ecx,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC1021:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1021-.L_text_b
	.previous
.L_B95_e:
.L_LC1022:

.section	.line
	.4byte	2044	/ sas.c:2044
	.2byte	0xffff
	.4byte	.L_LC1022-.L_text_b
	.previous
	testb %bl,%bl
	je .L532
.L_B96:
.L_LC1023:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1023-.L_text_b
	.previous
.L_LC1024:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1024-.L_text_b
	.previous
	movb 98(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1025:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1025-.L_text_b
	.previous
.L_B96_e:
.L_LC1026:

.section	.line
	.4byte	2044	/ sas.c:2044
	.2byte	0xffff
	.4byte	.L_LC1026-.L_text_b
	.previous
.L532:
.L_B97:
.L_LC1027:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1027-.L_text_b
	.previous
.L_LC1028:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1028-.L_text_b
	.previous
	movb $12,%al
	movw 96(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1029:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1029-.L_text_b
	.previous
.L_B97_e:
.L_LC1030:

.section	.line
	.4byte	2045	/ sas.c:2045
	.2byte	0xffff
	.4byte	.L_LC1030-.L_text_b
	.previous
	testb %bl,%bl
	je .L536
.L_B98:
.L_LC1031:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1031-.L_text_b
	.previous
.L_LC1032:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1032-.L_text_b
	.previous
	movb 94(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1033:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1033-.L_text_b
	.previous
.L_B98_e:
.L_LC1034:

.section	.line
	.4byte	2045	/ sas.c:2045
	.2byte	0xffff
	.4byte	.L_LC1034-.L_text_b
	.previous
.L536:
.L_B99:
.L_LC1035:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1035-.L_text_b
	.previous
.L_LC1036:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1036-.L_text_b
	.previous
	movb $48,%al
	movw 92(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1037:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1037-.L_text_b
	.previous
.L_B99_e:
.L_LC1038:

.section	.line
	.4byte	2046	/ sas.c:2046
	.2byte	0xffff
	.4byte	.L_LC1038-.L_text_b
	.previous
	testb %bl,%bl
	je .L540
.L_B100:
.L_LC1039:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1039-.L_text_b
	.previous
.L_LC1040:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1040-.L_text_b
	.previous
	movb 82(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1041:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1041-.L_text_b
	.previous
.L_B100_e:
.L_LC1042:

.section	.line
	.4byte	2046	/ sas.c:2046
	.2byte	0xffff
	.4byte	.L_LC1042-.L_text_b
	.previous
.L540:
.L_B101:
.L_LC1043:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1043-.L_text_b
	.previous
.L_LC1044:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1044-.L_text_b
	.previous
	movb $64,%al
	movl %ecx,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC1045:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1045-.L_text_b
	.previous
.L_B101_e:
.L_LC1046:

.section	.line
	.4byte	2047	/ sas.c:2047
	.2byte	0xffff
	.4byte	.L_LC1046-.L_text_b
	.previous
	testb %bl,%bl
	je .L544
.L_B102:
.L_LC1047:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1047-.L_text_b
	.previous
.L_LC1048:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1048-.L_text_b
	.previous
	movb 90(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1049:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1049-.L_text_b
	.previous
.L_B102_e:
.L_LC1050:

.section	.line
	.4byte	2047	/ sas.c:2047
	.2byte	0xffff
	.4byte	.L_LC1050-.L_text_b
	.previous
.L544:
.L_B103:
.L_LC1051:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1051-.L_text_b
	.previous
.L_LC1052:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1052-.L_text_b
	.previous
	xorb %al,%al
	movw 88(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1053:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1053-.L_text_b
	.previous
.L_B103_e:
.L_LC1054:

.section	.line
	.4byte	2048	/ sas.c:2048
	.2byte	0xffff
	.4byte	.L_LC1054-.L_text_b
	.previous
	testb %bl,%bl
	je .L548
.L_B104:
.L_LC1055:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1055-.L_text_b
	.previous
.L_LC1056:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1056-.L_text_b
	.previous
	movb 82(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1057:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1057-.L_text_b
	.previous
.L_B104_e:
.L_LC1058:

.section	.line
	.4byte	2048	/ sas.c:2048
	.2byte	0xffff
	.4byte	.L_LC1058-.L_text_b
	.previous
.L548:
.L_B105:
.L_LC1059:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1059-.L_text_b
	.previous
.L_LC1060:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1060-.L_text_b
	.previous
	xorb %al,%al
	movl %ecx,%edx
.L611:
/APP
	outb %al,%dx
/NO_APP
.L_LC1061:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1061-.L_text_b
	.previous
.L_B105_e:
.L525:
.L_LC1062:

.section	.line
	.4byte	2052	/ sas.c:2052
	.2byte	0xffff
	.4byte	.L_LC1062-.L_text_b
	.previous
	movb 32(%esi),%al
	andb $4,%al
	movb %al,-4(%ebp)
	je .L552
.L_B106:
.L_LC1063:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1063-.L_text_b
	.previous
.L_LC1064:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1064-.L_text_b
	.previous
	movb 98(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1065:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1065-.L_text_b
	.previous
.L_B106_e:
.L_LC1066:

.section	.line
	.4byte	2052	/ sas.c:2052
	.2byte	0xffff
	.4byte	.L_LC1066-.L_text_b
	.previous
.L552:
.L_B107:
.L_LC1067:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1067-.L_text_b
	.previous
.L_LC1068:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1068-.L_text_b
	.previous
.L_B108:
.L_LC1069:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1069-.L_text_b
	.previous
	movw 96(%esi),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1070:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1070-.L_text_b
	.previous
.L_B108_e:
.L_B107_e:
.L_LC1071:

.section	.line
	.4byte	2053	/ sas.c:2053
	.2byte	0xffff
	.4byte	.L_LC1071-.L_text_b
	.previous
	cmpb $0,-4(%ebp)
	je .L556
.L_B109:
.L_LC1072:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1072-.L_text_b
	.previous
.L_LC1073:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1073-.L_text_b
	.previous
	movb 74(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1074:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1074-.L_text_b
	.previous
.L_B109_e:
.L_LC1075:

.section	.line
	.4byte	2053	/ sas.c:2053
	.2byte	0xffff
	.4byte	.L_LC1075-.L_text_b
	.previous
.L556:
.L_B110:
.L_LC1076:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1076-.L_text_b
	.previous
	movw 72(%esi),%cx
.L_LC1077:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1077-.L_text_b
	.previous
.L_B111:
.L_LC1078:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1078-.L_text_b
	.previous
	movl %ecx,%edx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1079:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1079-.L_text_b
	.previous
.L_B111_e:
.L_B110_e:
.L_LC1080:

.section	.line
	.4byte	2054	/ sas.c:2054
	.2byte	0xffff
	.4byte	.L_LC1080-.L_text_b
	.previous
	cmpb $0,-4(%ebp)
	je .L560
.L_B112:
.L_LC1081:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1081-.L_text_b
	.previous
.L_LC1082:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1082-.L_text_b
	.previous
	movb 74(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1083:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1083-.L_text_b
	.previous
.L_B112_e:
.L_LC1084:

.section	.line
	.4byte	2054	/ sas.c:2054
	.2byte	0xffff
	.4byte	.L_LC1084-.L_text_b
	.previous
.L560:
.L_B113:
.L_LC1085:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1085-.L_text_b
	.previous
.L_LC1086:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1086-.L_text_b
	.previous
.L_B114:
.L_LC1087:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1087-.L_text_b
	.previous
	movl %ecx,%edx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1088:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1088-.L_text_b
	.previous
.L_B114_e:
.L_B113_e:
.L_LC1089:

.section	.line
	.4byte	2055	/ sas.c:2055
	.2byte	0xffff
	.4byte	.L_LC1089-.L_text_b
	.previous
	cmpb $0,-4(%ebp)
	je .L564
.L_B115:
.L_LC1090:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1090-.L_text_b
	.previous
.L_LC1091:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1091-.L_text_b
	.previous
	movb 94(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1092:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1092-.L_text_b
	.previous
.L_B115_e:
.L_LC1093:

.section	.line
	.4byte	2055	/ sas.c:2055
	.2byte	0xffff
	.4byte	.L_LC1093-.L_text_b
	.previous
.L564:
.L_B116:
.L_LC1094:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1094-.L_text_b
	.previous
.L_LC1095:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1095-.L_text_b
	.previous
.L_B117:
.L_LC1096:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1096-.L_text_b
	.previous
	movw 92(%esi),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1097:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1097-.L_text_b
	.previous
.L_B117_e:
.L_B116_e:
.L_LC1098:

.section	.line
	.4byte	2056	/ sas.c:2056
	.2byte	0xffff
	.4byte	.L_LC1098-.L_text_b
	.previous
	cmpb $0,-4(%ebp)
	je .L568
.L_B118:
.L_LC1099:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1099-.L_text_b
	.previous
.L_LC1100:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1100-.L_text_b
	.previous
	movb 82(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1101:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1101-.L_text_b
	.previous
.L_B118_e:
.L_LC1102:

.section	.line
	.4byte	2056	/ sas.c:2056
	.2byte	0xffff
	.4byte	.L_LC1102-.L_text_b
	.previous
.L568:
.L_B119:
.L_LC1103:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1103-.L_text_b
	.previous
	movw 80(%esi),%cx
.L_LC1104:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1104-.L_text_b
	.previous
.L_B120:
.L_LC1105:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1105-.L_text_b
	.previous
	movl %ecx,%edx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1106:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1106-.L_text_b
	.previous
.L_B120_e:
.L_B119_e:
.L_LC1107:

.section	.line
	.4byte	2059	/ sas.c:2059
	.2byte	0xffff
	.4byte	.L_LC1107-.L_text_b
	.previous
	cmpb $2,66(%esi)
	jne .L571
.L_LC1108:

.section	.line
	.4byte	2060	/ sas.c:2060
	.2byte	0xffff
	.4byte	.L_LC1108-.L_text_b
	.previous
	cmpb $0,-4(%ebp)
	je .L573
.L_B121:
.L_LC1109:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1109-.L_text_b
	.previous
.L_LC1110:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1110-.L_text_b
	.previous
	movb 82(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1111:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1111-.L_text_b
	.previous
.L_B121_e:
.L_LC1112:

.section	.line
	.4byte	2060	/ sas.c:2060
	.2byte	0xffff
	.4byte	.L_LC1112-.L_text_b
	.previous
.L573:
.L_B122:
.L_LC1113:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1113-.L_text_b
	.previous
.L_LC1114:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1114-.L_text_b
	.previous
	movb $-127,%al
.L_LC1115:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1115-.L_text_b
	.previous
.L_B122_e:
.L_LC1116:

.section	.line
	.4byte	2060	/ sas.c:2060
	.2byte	0xffff
	.4byte	.L_LC1116-.L_text_b
	.previous
	jmp .L612
	.align 4
.L571:
.L_LC1117:

.section	.line
	.4byte	2061	/ sas.c:2061
	.2byte	0xffff
	.4byte	.L_LC1117-.L_text_b
	.previous
	cmpb $1,66(%esi)
	jne .L576
.L_LC1118:

.section	.line
	.4byte	2063	/ sas.c:2063
	.2byte	0xffff
	.4byte	.L_LC1118-.L_text_b
	.previous
	cmpb $0,-4(%ebp)
	je .L579
.L_B123:
.L_LC1119:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1119-.L_text_b
	.previous
.L_LC1120:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1120-.L_text_b
	.previous
	movb 82(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1121:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1121-.L_text_b
	.previous
.L_B123_e:
.L_LC1122:

.section	.line
	.4byte	2063	/ sas.c:2063
	.2byte	0xffff
	.4byte	.L_LC1122-.L_text_b
	.previous
.L579:
.L_B124:
.L_LC1123:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1123-.L_text_b
	.previous
.L_LC1124:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1124-.L_text_b
	.previous
	movb $64,%al
	movl %ecx,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC1125:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1125-.L_text_b
	.previous
.L_B124_e:
.L_LC1126:

.section	.line
	.4byte	2064	/ sas.c:2064
	.2byte	0xffff
	.4byte	.L_LC1126-.L_text_b
	.previous
	cmpb $0,-4(%ebp)
	je .L583
.L_B125:
.L_LC1127:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1127-.L_text_b
	.previous
.L_LC1128:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1128-.L_text_b
	.previous
	movb 90(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1129:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1129-.L_text_b
	.previous
.L_B125_e:
.L_LC1130:

.section	.line
	.4byte	2064	/ sas.c:2064
	.2byte	0xffff
	.4byte	.L_LC1130-.L_text_b
	.previous
.L583:
.L_B126:
.L_LC1131:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1131-.L_text_b
	.previous
.L_LC1132:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1132-.L_text_b
	.previous
	movb $8,%al
	movw 88(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1133:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1133-.L_text_b
	.previous
.L_B126_e:
.L_LC1134:

.section	.line
	.4byte	2065	/ sas.c:2065
	.2byte	0xffff
	.4byte	.L_LC1134-.L_text_b
	.previous
	cmpb $0,-4(%ebp)
	je .L587
.L_B127:
.L_LC1135:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1135-.L_text_b
	.previous
.L_LC1136:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1136-.L_text_b
	.previous
	movb 82(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1137:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1137-.L_text_b
	.previous
.L_B127_e:
.L_LC1138:

.section	.line
	.4byte	2065	/ sas.c:2065
	.2byte	0xffff
	.4byte	.L_LC1138-.L_text_b
	.previous
.L587:
.L_B128:
.L_LC1139:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1139-.L_text_b
	.previous
.L_LC1140:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1140-.L_text_b
	.previous
	xorb %al,%al
.L612:
	movl %ecx,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC1141:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1141-.L_text_b
	.previous
.L_B128_e:
.L576:
.L_LC1142:

.section	.line
	.4byte	2068	/ sas.c:2068
	.2byte	0xffff
	.4byte	.L_LC1142-.L_text_b
	.previous
	testb $4,32(%esi)
	je .L591
.L_B129:
.L_LC1143:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1143-.L_text_b
	.previous
.L_LC1144:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1144-.L_text_b
	.previous
	movb 98(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1145:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1145-.L_text_b
	.previous
.L_B129_e:
.L_LC1146:

.section	.line
	.4byte	2071	/ sas.c:2071
	.2byte	0xffff
	.4byte	.L_LC1146-.L_text_b
	.previous
.L591:
.L_B130:
.L_LC1147:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1147-.L_text_b
	.previous
.L_LC1148:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1148-.L_text_b
	.previous
.L_B131:
.L_LC1149:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1149-.L_text_b
	.previous
	movw 96(%esi),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1150:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1150-.L_text_b
	.previous
.L_B131_e:
.L_B130_e:
.L_LC1151:

.section	.line
	.4byte	2071	/ sas.c:2071
	.2byte	0xffff
	.4byte	.L_LC1151-.L_text_b
	.previous
	andb $176,%al
	movb %al,61(%esi)
	movb %al,60(%esi)
.L_LC1152:

.section	.line
	.4byte	2073	/ sas.c:2073
	.2byte	0xffff
	.4byte	.L_LC1152-.L_text_b
	.previous
	movb $7,64(%esi)
.L_LC1153:

.section	.line
	.4byte	2074	/ sas.c:2074
	.2byte	0xffff
	.4byte	.L_LC1153-.L_text_b
	.previous
	testb $4,32(%esi)
	je .L595
.L_B132:
.L_LC1154:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1154-.L_text_b
	.previous
.L_LC1155:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1155-.L_text_b
	.previous
	movb 78(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1156:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1156-.L_text_b
	.previous
.L_B132_e:
.L_LC1157:

.section	.line
	.4byte	2074	/ sas.c:2074
	.2byte	0xffff
	.4byte	.L_LC1157-.L_text_b
	.previous
.L595:
.L_B133:
.L_LC1158:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1158-.L_text_b
	.previous
.L_LC1159:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1159-.L_text_b
	.previous
	movb $7,%al
	movw 76(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1160:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1160-.L_text_b
	.previous
.L_B133_e:
.L_LC1161:

.section	.line
	.4byte	2079	/ sas.c:2079
	.2byte	0xffff
	.4byte	.L_LC1161-.L_text_b
	.previous
	andb $248,36(%esi)
.L_LC1162:

.section	.line
	.4byte	2082	/ sas.c:2082
	.2byte	0xffff
	.4byte	.L_LC1162-.L_text_b
	.previous
	testw $448,44(%esi)
	je .L598
.L_LC1163:

.section	.line
	.4byte	2085	/ sas.c:2085
	.2byte	0xffff
	.4byte	.L_LC1163-.L_text_b
	.previous
	testb $64,44(%esi)
	je .L599
.L_LC1164:

.section	.line
	.4byte	2086	/ sas.c:2086
	.2byte	0xffff
	.4byte	.L_LC1164-.L_text_b
	.previous
	orb $1,36(%esi)
.L599:
.L_LC1165:

.section	.line
	.4byte	2087	/ sas.c:2087
	.2byte	0xffff
	.4byte	.L_LC1165-.L_text_b
	.previous
	cmpb $0,44(%esi)
	jge .L600
.L_LC1166:

.section	.line
	.4byte	2088	/ sas.c:2088
	.2byte	0xffff
	.4byte	.L_LC1166-.L_text_b
	.previous
	orb $2,36(%esi)
.L600:
.L_LC1167:

.section	.line
	.4byte	2089	/ sas.c:2089
	.2byte	0xffff
	.4byte	.L_LC1167-.L_text_b
	.previous
	testb $1,45(%esi)
	je .L598
.L_LC1168:

.section	.line
	.4byte	2090	/ sas.c:2090
	.2byte	0xffff
	.4byte	.L_LC1168-.L_text_b
	.previous
	orb $4,36(%esi)
.L598:
.L_LC1169:

.section	.line
	.4byte	2092	/ sas.c:2092
	.2byte	0xffff
	.4byte	.L_LC1169-.L_text_b
	.previous
	movl 120(%esi),%eax
	movl sas_modem(,%eax,4),%eax
	movl %eax,52(%esi)
.L_LC1170:

.section	.line
	.4byte	2093	/ sas.c:2093
	.2byte	0xffff
	.4byte	.L_LC1170-.L_text_b
	.previous
	movl 120(%esi),%eax
	movl sas_flow(,%eax,4),%eax
	movl %eax,56(%esi)
.L_LC1171:

.section	.line
	.4byte	2096	/ sas.c:2096
	.2byte	0xffff
	.4byte	.L_LC1171-.L_text_b
	.previous
	andb $3,52(%esi)
.L_LC1172:

.section	.line
	.4byte	2097	/ sas.c:2097
	.2byte	0xffff
	.4byte	.L_LC1172-.L_text_b
	.previous
	andb $3,53(%esi)
.L_LC1173:

.section	.line
	.4byte	2098	/ sas.c:2098
	.2byte	0xffff
	.4byte	.L_LC1173-.L_text_b
	.previous
	andb $3,54(%esi)
.L_LC1174:

.section	.line
	.4byte	2099	/ sas.c:2099
	.2byte	0xffff
	.4byte	.L_LC1174-.L_text_b
	.previous
	andb $240,55(%esi)
.L_LC1175:

.section	.line
	.4byte	2100	/ sas.c:2100
	.2byte	0xffff
	.4byte	.L_LC1175-.L_text_b
	.previous
	andb $3,56(%esi)
.L_LC1176:

.section	.line
	.4byte	2101	/ sas.c:2101
	.2byte	0xffff
	.4byte	.L_LC1176-.L_text_b
	.previous
	andb $240,57(%esi)
.L_LC1177:

.section	.line
	.4byte	2102	/ sas.c:2102
	.2byte	0xffff
	.4byte	.L_LC1177-.L_text_b
	.previous
	andb $240,58(%esi)
.L_LC1178:

.section	.line
	.4byte	2103	/ sas.c:2103
	.2byte	0xffff
	.4byte	.L_LC1178-.L_text_b
	.previous
	andb $3,59(%esi)
.L_I14_774:
.L514:
.L_LC1179:

.section	.line
	.4byte	2107	/ sas.c:2107
	.2byte	0xffff
	.4byte	.L_LC1179-.L_text_b
	.previous
	movb 52(%esi),%al
	orb 54(%esi),%al
	orb 53(%esi),%al
	orb 56(%esi),%al
	orb 59(%esi),%al
	notb %al
	andb %al,62(%esi)
.L_LC1180:

.section	.line
	.4byte	2113	/ sas.c:2113
	.2byte	0xffff
	.4byte	.L_LC1180-.L_text_b
	.previous
	movzbl 62(%esi),%ecx
	testb $4,44(%esi)
	je .L602
	movzbl 54(%esi),%eax
	jmp .L613
	.align 4
.L602:
	movzbl 53(%esi),%eax
.L613:
	orl %eax,%ecx
	movb %cl,62(%esi)
.L_LC1181:

.section	.line
	.4byte	2117	/ sas.c:2117
	.2byte	0xffff
	.4byte	.L_LC1181-.L_text_b
	.previous
	cmpb $0,44(%esi)
	jge .L604
.L_LC1182:

.section	.line
	.4byte	2118	/ sas.c:2118
	.2byte	0xffff
	.4byte	.L_LC1182-.L_text_b
	.previous
	movb 56(%esi),%al
	orb %al,62(%esi)
	jmp .L605
	.align 4
.L604:
.L_LC1183:

.section	.line
	.4byte	2119	/ sas.c:2119
	.2byte	0xffff
	.4byte	.L_LC1183-.L_text_b
	.previous
	testb $1,45(%esi)
	jne .L605
.L_LC1184:

.section	.line
	.4byte	2121	/ sas.c:2121
	.2byte	0xffff
	.4byte	.L_LC1184-.L_text_b
	.previous
	orb $32,36(%esi)
.L_LC1185:

.section	.line
	.4byte	2122	/ sas.c:2122
	.2byte	0xffff
	.4byte	.L_LC1185-.L_text_b
	.previous
	movb 59(%esi),%dl
	orb %dl,62(%esi)
.L605:
.L_LC1186:

.section	.line
	.4byte	2125	/ sas.c:2125
	.2byte	0xffff
	.4byte	.L_LC1186-.L_text_b
	.previous
	testb $4,32(%esi)
	je .L608
.L_B134:
.L_LC1187:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1187-.L_text_b
	.previous
.L_LC1188:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1188-.L_text_b
	.previous
	movb 90(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1189:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1189-.L_text_b
	.previous
.L_B134_e:
.L_LC1190:

.section	.line
	.4byte	2125	/ sas.c:2125
	.2byte	0xffff
	.4byte	.L_LC1190-.L_text_b
	.previous
.L608:
.L_B135:
.L_LC1191:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1191-.L_text_b
	.previous
.L_LC1192:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1192-.L_text_b
	.previous
	movb 62(%esi),%al
	movw 88(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1193:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1193-.L_text_b
	.previous
.L_B135_e:
.L_LC1194:

.section	.line
	.4byte	2127	/ sas.c:2127
	.2byte	0xffff
	.4byte	.L_LC1194-.L_text_b
	.previous
	orb $40,32(%esi)
.L_LC1195:

.section	.line
	.4byte	2128	/ sas.c:2128
	.2byte	0xffff
	.4byte	.L_LC1195-.L_text_b
	.previous
.L_b14_e:
	leal -12(%ebp),%esp
	popl %ebx
	popl %esi
	leave
	ret
.L_f14_e:
.Lfe11:
	.size	 sas_open_device,.Lfe11-sas_open_device

.section	.debug
.L_D550:
	.4byte	.L_D550_e-.L_D550
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D632
	.2byte	0x38
	.string	"sas_open_device"
	.2byte	0x111
	.4byte	sas_open_device
	.2byte	0x121
	.4byte	.L_f14_e
	.2byte	0x8041
	.4byte	.L_b14
	.2byte	0x8051
	.4byte	.L_b14_e
.L_D550_e:
.L_D633:
	.4byte	.L_D633_e-.L_D633
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D634
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t633_e-.L_t633
.L_t633:
	.byte	0x1
	.4byte	.L_T816
.L_t633_e:
	.2byte	0x23
	.2byte	.L_l633_e-.L_l633
.L_l633:
	.byte	0x1
	.4byte	0x6
.L_l633_e:
.L_D633_e:
.L_D634:
	.4byte	.L_D634_e-.L_D634
	.2byte	0xa
	.2byte	0x12
	.4byte	.L_D635
	.2byte	0x38
	.string	"setmcr"
	.2byte	0x111
	.4byte	.L_I14_774
.L_D634_e:
.L_D635:
	.4byte	.L_D635_e-.L_D635
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D636
	.2byte	0x38
	.string	"regvar"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l635_e-.L_l635
.L_l635:
.L_l635_e:
.L_D635_e:
.L_D636:
	.4byte	.L_D636_e-.L_D636
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D637
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B90
	.2byte	0x121
	.4byte	.L_B90_e
.L_D636_e:
.L_D638:
	.4byte	.L_D638_e-.L_D638
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D639
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l638_e-.L_l638
.L_l638:
.L_l638_e:
.L_D638_e:
.L_D639:
	.4byte	.L_D639_e-.L_D639
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D640
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l639_e-.L_l639
.L_l639:
.L_l639_e:
.L_D639_e:
.L_D640:
	.4byte	0x4
.L_D637:
	.4byte	.L_D637_e-.L_D637
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D641
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B91
	.2byte	0x121
	.4byte	.L_B91_e
.L_D637_e:
.L_D642:
	.4byte	.L_D642_e-.L_D642
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D643
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l642_e-.L_l642
.L_l642:
.L_l642_e:
.L_D642_e:
.L_D643:
	.4byte	.L_D643_e-.L_D643
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D644
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l643_e-.L_l643
.L_l643:
.L_l643_e:
.L_D643_e:
.L_D644:
	.4byte	0x4
.L_D641:
	.4byte	.L_D641_e-.L_D641
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D645
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B92
	.2byte	0x121
	.4byte	.L_B92_e
.L_D641_e:
.L_D646:
	.4byte	.L_D646_e-.L_D646
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D647
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l646_e-.L_l646
.L_l646:
.L_l646_e:
.L_D646_e:
.L_D647:
	.4byte	.L_D647_e-.L_D647
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D648
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l647_e-.L_l647
.L_l647:
.L_l647_e:
.L_D647_e:
.L_D648:
	.4byte	0x4
.L_D645:
	.4byte	.L_D645_e-.L_D645
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D649
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B93
	.2byte	0x121
	.4byte	.L_B93_e
.L_D645_e:
.L_D650:
	.4byte	.L_D650_e-.L_D650
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D651
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l650_e-.L_l650
.L_l650:
.L_l650_e:
.L_D650_e:
.L_D651:
	.4byte	.L_D651_e-.L_D651
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D652
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l651_e-.L_l651
.L_l651:
	.4byte	0x0
.L_l651_e:
.L_D651_e:
.L_D652:
	.4byte	0x4
.L_D649:
	.4byte	.L_D649_e-.L_D649
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D653
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B94
	.2byte	0x121
	.4byte	.L_B94_e
.L_D649_e:
.L_D654:
	.4byte	.L_D654_e-.L_D654
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D655
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l654_e-.L_l654
.L_l654:
.L_l654_e:
.L_D654_e:
.L_D655:
	.4byte	.L_D655_e-.L_D655
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D656
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l655_e-.L_l655
.L_l655:
.L_l655_e:
.L_D655_e:
.L_D656:
	.4byte	0x4
.L_D653:
	.4byte	.L_D653_e-.L_D653
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D657
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B95
	.2byte	0x121
	.4byte	.L_B95_e
.L_D653_e:
.L_D658:
	.4byte	.L_D658_e-.L_D658
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D659
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l658_e-.L_l658
.L_l658:
	.byte	0x1
	.4byte	0x1
.L_l658_e:
.L_D658_e:
.L_D659:
	.4byte	.L_D659_e-.L_D659
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D660
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l659_e-.L_l659
.L_l659:
	.4byte	0x20
.L_l659_e:
.L_D659_e:
.L_D660:
	.4byte	0x4
.L_D657:
	.4byte	.L_D657_e-.L_D657
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D661
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B96
	.2byte	0x121
	.4byte	.L_B96_e
.L_D657_e:
.L_D662:
	.4byte	.L_D662_e-.L_D662
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D663
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l662_e-.L_l662
.L_l662:
.L_l662_e:
.L_D662_e:
.L_D663:
	.4byte	.L_D663_e-.L_D663
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D664
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l663_e-.L_l663
.L_l663:
.L_l663_e:
.L_D663_e:
.L_D664:
	.4byte	0x4
.L_D661:
	.4byte	.L_D661_e-.L_D661
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D665
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B97
	.2byte	0x121
	.4byte	.L_B97_e
.L_D661_e:
.L_D666:
	.4byte	.L_D666_e-.L_D666
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D667
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l666_e-.L_l666
.L_l666:
.L_l666_e:
.L_D666_e:
.L_D667:
	.4byte	.L_D667_e-.L_D667
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D668
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l667_e-.L_l667
.L_l667:
	.4byte	0xc
.L_l667_e:
.L_D667_e:
.L_D668:
	.4byte	0x4
.L_D665:
	.4byte	.L_D665_e-.L_D665
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D669
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B98
	.2byte	0x121
	.4byte	.L_B98_e
.L_D665_e:
.L_D670:
	.4byte	.L_D670_e-.L_D670
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D671
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l670_e-.L_l670
.L_l670:
.L_l670_e:
.L_D670_e:
.L_D671:
	.4byte	.L_D671_e-.L_D671
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D672
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l671_e-.L_l671
.L_l671:
.L_l671_e:
.L_D671_e:
.L_D672:
	.4byte	0x4
.L_D669:
	.4byte	.L_D669_e-.L_D669
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D673
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B99
	.2byte	0x121
	.4byte	.L_B99_e
.L_D669_e:
.L_D674:
	.4byte	.L_D674_e-.L_D674
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D675
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l674_e-.L_l674
.L_l674:
.L_l674_e:
.L_D674_e:
.L_D675:
	.4byte	.L_D675_e-.L_D675
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D676
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l675_e-.L_l675
.L_l675:
	.4byte	0x30
.L_l675_e:
.L_D675_e:
.L_D676:
	.4byte	0x4
.L_D673:
	.4byte	.L_D673_e-.L_D673
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D677
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B100
	.2byte	0x121
	.4byte	.L_B100_e
.L_D673_e:
.L_D678:
	.4byte	.L_D678_e-.L_D678
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D679
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l678_e-.L_l678
.L_l678:
.L_l678_e:
.L_D678_e:
.L_D679:
	.4byte	.L_D679_e-.L_D679
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D680
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l679_e-.L_l679
.L_l679:
.L_l679_e:
.L_D679_e:
.L_D680:
	.4byte	0x4
.L_D677:
	.4byte	.L_D677_e-.L_D677
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D681
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B101
	.2byte	0x121
	.4byte	.L_B101_e
.L_D677_e:
.L_D682:
	.4byte	.L_D682_e-.L_D682
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D683
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l682_e-.L_l682
.L_l682:
.L_l682_e:
.L_D682_e:
.L_D683:
	.4byte	.L_D683_e-.L_D683
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D684
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l683_e-.L_l683
.L_l683:
	.4byte	0x40
.L_l683_e:
.L_D683_e:
.L_D684:
	.4byte	0x4
.L_D681:
	.4byte	.L_D681_e-.L_D681
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D685
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B102
	.2byte	0x121
	.4byte	.L_B102_e
.L_D681_e:
.L_D686:
	.4byte	.L_D686_e-.L_D686
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D687
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l686_e-.L_l686
.L_l686:
.L_l686_e:
.L_D686_e:
.L_D687:
	.4byte	.L_D687_e-.L_D687
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D688
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l687_e-.L_l687
.L_l687:
.L_l687_e:
.L_D687_e:
.L_D688:
	.4byte	0x4
.L_D685:
	.4byte	.L_D685_e-.L_D685
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D689
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B103
	.2byte	0x121
	.4byte	.L_B103_e
.L_D685_e:
.L_D690:
	.4byte	.L_D690_e-.L_D690
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D691
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l690_e-.L_l690
.L_l690:
.L_l690_e:
.L_D690_e:
.L_D691:
	.4byte	.L_D691_e-.L_D691
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D692
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l691_e-.L_l691
.L_l691:
	.4byte	0x0
.L_l691_e:
.L_D691_e:
.L_D692:
	.4byte	0x4
.L_D689:
	.4byte	.L_D689_e-.L_D689
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D693
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B104
	.2byte	0x121
	.4byte	.L_B104_e
.L_D689_e:
.L_D694:
	.4byte	.L_D694_e-.L_D694
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D695
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l694_e-.L_l694
.L_l694:
.L_l694_e:
.L_D694_e:
.L_D695:
	.4byte	.L_D695_e-.L_D695
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D696
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l695_e-.L_l695
.L_l695:
.L_l695_e:
.L_D695_e:
.L_D696:
	.4byte	0x4
.L_D693:
	.4byte	.L_D693_e-.L_D693
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D697
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B105
	.2byte	0x121
	.4byte	.L_B105_e
.L_D693_e:
.L_D698:
	.4byte	.L_D698_e-.L_D698
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D699
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l698_e-.L_l698
.L_l698:
.L_l698_e:
.L_D698_e:
.L_D699:
	.4byte	.L_D699_e-.L_D699
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D700
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l699_e-.L_l699
.L_l699:
	.4byte	0x0
.L_l699_e:
.L_D699_e:
.L_D700:
	.4byte	0x4
.L_D697:
	.4byte	.L_D697_e-.L_D697
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D701
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B106
	.2byte	0x121
	.4byte	.L_B106_e
.L_D697_e:
.L_D702:
	.4byte	.L_D702_e-.L_D702
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D703
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l702_e-.L_l702
.L_l702:
.L_l702_e:
.L_D702_e:
.L_D703:
	.4byte	.L_D703_e-.L_D703
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D704
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l703_e-.L_l703
.L_l703:
.L_l703_e:
.L_D703_e:
.L_D704:
	.4byte	0x4
.L_D701:
	.4byte	.L_D701_e-.L_D701
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D705
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B107
	.2byte	0x121
	.4byte	.L_B107_e
.L_D701_e:
.L_D706:
	.4byte	.L_D706_e-.L_D706
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D707
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l706_e-.L_l706
.L_l706:
.L_l706_e:
.L_D706_e:
.L_D707:
	.4byte	.L_D707_e-.L_D707
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D708
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l707_e-.L_l707
.L_l707:
	.byte	0x1
	.4byte	0x1
.L_l707_e:
.L_D707_e:
.L_D708:
	.4byte	0x4
.L_D705:
	.4byte	.L_D705_e-.L_D705
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D709
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B109
	.2byte	0x121
	.4byte	.L_B109_e
.L_D705_e:
.L_D710:
	.4byte	.L_D710_e-.L_D710
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D711
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l710_e-.L_l710
.L_l710:
.L_l710_e:
.L_D710_e:
.L_D711:
	.4byte	.L_D711_e-.L_D711
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D712
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l711_e-.L_l711
.L_l711:
.L_l711_e:
.L_D711_e:
.L_D712:
	.4byte	0x4
.L_D709:
	.4byte	.L_D709_e-.L_D709
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D713
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B110
	.2byte	0x121
	.4byte	.L_B110_e
.L_D709_e:
.L_D714:
	.4byte	.L_D714_e-.L_D714
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D715
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l714_e-.L_l714
.L_l714:
	.byte	0x1
	.4byte	0x1
.L_l714_e:
.L_D714_e:
.L_D715:
	.4byte	.L_D715_e-.L_D715
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D716
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l715_e-.L_l715
.L_l715:
	.byte	0x1
	.4byte	0x3
.L_l715_e:
.L_D715_e:
.L_D716:
	.4byte	0x4
.L_D713:
	.4byte	.L_D713_e-.L_D713
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D717
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B112
	.2byte	0x121
	.4byte	.L_B112_e
.L_D713_e:
.L_D718:
	.4byte	.L_D718_e-.L_D718
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D719
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l718_e-.L_l718
.L_l718:
.L_l718_e:
.L_D718_e:
.L_D719:
	.4byte	.L_D719_e-.L_D719
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D720
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l719_e-.L_l719
.L_l719:
.L_l719_e:
.L_D719_e:
.L_D720:
	.4byte	0x4
.L_D717:
	.4byte	.L_D717_e-.L_D717
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D721
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B113
	.2byte	0x121
	.4byte	.L_B113_e
.L_D717_e:
.L_D722:
	.4byte	.L_D722_e-.L_D722
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D723
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l722_e-.L_l722
.L_l722:
.L_l722_e:
.L_D722_e:
.L_D723:
	.4byte	.L_D723_e-.L_D723
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D724
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l723_e-.L_l723
.L_l723:
	.byte	0x1
	.4byte	0x1
.L_l723_e:
.L_D723_e:
.L_D724:
	.4byte	0x4
.L_D721:
	.4byte	.L_D721_e-.L_D721
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D725
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B115
	.2byte	0x121
	.4byte	.L_B115_e
.L_D721_e:
.L_D726:
	.4byte	.L_D726_e-.L_D726
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D727
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l726_e-.L_l726
.L_l726:
.L_l726_e:
.L_D726_e:
.L_D727:
	.4byte	.L_D727_e-.L_D727
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D728
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l727_e-.L_l727
.L_l727:
.L_l727_e:
.L_D727_e:
.L_D728:
	.4byte	0x4
.L_D725:
	.4byte	.L_D725_e-.L_D725
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D729
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B116
	.2byte	0x121
	.4byte	.L_B116_e
.L_D725_e:
.L_D730:
	.4byte	.L_D730_e-.L_D730
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D731
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l730_e-.L_l730
.L_l730:
.L_l730_e:
.L_D730_e:
.L_D731:
	.4byte	.L_D731_e-.L_D731
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D732
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l731_e-.L_l731
.L_l731:
	.byte	0x1
	.4byte	0x1
.L_l731_e:
.L_D731_e:
.L_D732:
	.4byte	0x4
.L_D729:
	.4byte	.L_D729_e-.L_D729
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D733
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B118
	.2byte	0x121
	.4byte	.L_B118_e
.L_D729_e:
.L_D734:
	.4byte	.L_D734_e-.L_D734
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D735
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l734_e-.L_l734
.L_l734:
.L_l734_e:
.L_D734_e:
.L_D735:
	.4byte	.L_D735_e-.L_D735
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D736
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l735_e-.L_l735
.L_l735:
.L_l735_e:
.L_D735_e:
.L_D736:
	.4byte	0x4
.L_D733:
	.4byte	.L_D733_e-.L_D733
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D737
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B119
	.2byte	0x121
	.4byte	.L_B119_e
.L_D733_e:
.L_D738:
	.4byte	.L_D738_e-.L_D738
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D739
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l738_e-.L_l738
.L_l738:
	.byte	0x1
	.4byte	0x1
.L_l738_e:
.L_D738_e:
.L_D739:
	.4byte	.L_D739_e-.L_D739
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D740
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l739_e-.L_l739
.L_l739:
	.byte	0x1
	.4byte	0x3
.L_l739_e:
.L_D739_e:
.L_D740:
	.4byte	0x4
.L_D737:
	.4byte	.L_D737_e-.L_D737
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D741
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B121
	.2byte	0x121
	.4byte	.L_B121_e
.L_D737_e:
.L_D742:
	.4byte	.L_D742_e-.L_D742
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D743
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l742_e-.L_l742
.L_l742:
.L_l742_e:
.L_D742_e:
.L_D743:
	.4byte	.L_D743_e-.L_D743
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D744
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l743_e-.L_l743
.L_l743:
.L_l743_e:
.L_D743_e:
.L_D744:
	.4byte	0x4
.L_D741:
	.4byte	.L_D741_e-.L_D741
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D745
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B122
	.2byte	0x121
	.4byte	.L_B122_e
.L_D741_e:
.L_D746:
	.4byte	.L_D746_e-.L_D746
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D747
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l746_e-.L_l746
.L_l746:
.L_l746_e:
.L_D746_e:
.L_D747:
	.4byte	.L_D747_e-.L_D747
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D748
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l747_e-.L_l747
.L_l747:
	.4byte	0xffffff81
.L_l747_e:
.L_D747_e:
.L_D748:
	.4byte	0x4
.L_D745:
	.4byte	.L_D745_e-.L_D745
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D749
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B123
	.2byte	0x121
	.4byte	.L_B123_e
.L_D745_e:
.L_D750:
	.4byte	.L_D750_e-.L_D750
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D751
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l750_e-.L_l750
.L_l750:
.L_l750_e:
.L_D750_e:
.L_D751:
	.4byte	.L_D751_e-.L_D751
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D752
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l751_e-.L_l751
.L_l751:
.L_l751_e:
.L_D751_e:
.L_D752:
	.4byte	0x4
.L_D749:
	.4byte	.L_D749_e-.L_D749
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D753
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B124
	.2byte	0x121
	.4byte	.L_B124_e
.L_D749_e:
.L_D754:
	.4byte	.L_D754_e-.L_D754
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D755
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l754_e-.L_l754
.L_l754:
.L_l754_e:
.L_D754_e:
.L_D755:
	.4byte	.L_D755_e-.L_D755
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D756
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l755_e-.L_l755
.L_l755:
	.4byte	0x40
.L_l755_e:
.L_D755_e:
.L_D756:
	.4byte	0x4
.L_D753:
	.4byte	.L_D753_e-.L_D753
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D757
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B125
	.2byte	0x121
	.4byte	.L_B125_e
.L_D753_e:
.L_D758:
	.4byte	.L_D758_e-.L_D758
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D759
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l758_e-.L_l758
.L_l758:
.L_l758_e:
.L_D758_e:
.L_D759:
	.4byte	.L_D759_e-.L_D759
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D760
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l759_e-.L_l759
.L_l759:
.L_l759_e:
.L_D759_e:
.L_D760:
	.4byte	0x4
.L_D757:
	.4byte	.L_D757_e-.L_D757
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D761
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B126
	.2byte	0x121
	.4byte	.L_B126_e
.L_D757_e:
.L_D762:
	.4byte	.L_D762_e-.L_D762
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D763
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l762_e-.L_l762
.L_l762:
.L_l762_e:
.L_D762_e:
.L_D763:
	.4byte	.L_D763_e-.L_D763
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D764
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l763_e-.L_l763
.L_l763:
	.4byte	0x8
.L_l763_e:
.L_D763_e:
.L_D764:
	.4byte	0x4
.L_D761:
	.4byte	.L_D761_e-.L_D761
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D765
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B127
	.2byte	0x121
	.4byte	.L_B127_e
.L_D761_e:
.L_D766:
	.4byte	.L_D766_e-.L_D766
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D767
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l766_e-.L_l766
.L_l766:
.L_l766_e:
.L_D766_e:
.L_D767:
	.4byte	.L_D767_e-.L_D767
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D768
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l767_e-.L_l767
.L_l767:
.L_l767_e:
.L_D767_e:
.L_D768:
	.4byte	0x4
.L_D765:
	.4byte	.L_D765_e-.L_D765
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D769
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B128
	.2byte	0x121
	.4byte	.L_B128_e
.L_D765_e:
.L_D770:
	.4byte	.L_D770_e-.L_D770
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D771
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l770_e-.L_l770
.L_l770:
.L_l770_e:
.L_D770_e:
.L_D771:
	.4byte	.L_D771_e-.L_D771
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D772
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l771_e-.L_l771
.L_l771:
	.4byte	0x0
.L_l771_e:
.L_D771_e:
.L_D772:
	.4byte	0x4
.L_D769:
	.4byte	.L_D769_e-.L_D769
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D773
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B129
	.2byte	0x121
	.4byte	.L_B129_e
.L_D769_e:
.L_D774:
	.4byte	.L_D774_e-.L_D774
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D775
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l774_e-.L_l774
.L_l774:
.L_l774_e:
.L_D774_e:
.L_D775:
	.4byte	.L_D775_e-.L_D775
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D776
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l775_e-.L_l775
.L_l775:
.L_l775_e:
.L_D775_e:
.L_D776:
	.4byte	0x4
.L_D773:
	.4byte	.L_D773_e-.L_D773
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D777
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B130
	.2byte	0x121
	.4byte	.L_B130_e
.L_D773_e:
.L_D778:
	.4byte	.L_D778_e-.L_D778
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D779
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l778_e-.L_l778
.L_l778:
.L_l778_e:
.L_D778_e:
.L_D779:
	.4byte	.L_D779_e-.L_D779
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D780
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l779_e-.L_l779
.L_l779:
	.byte	0x1
	.4byte	0x0
.L_l779_e:
.L_D779_e:
.L_D780:
	.4byte	0x4
.L_D777:
	.4byte	.L_D777_e-.L_D777
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D781
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B132
	.2byte	0x121
	.4byte	.L_B132_e
.L_D777_e:
.L_D782:
	.4byte	.L_D782_e-.L_D782
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D783
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l782_e-.L_l782
.L_l782:
.L_l782_e:
.L_D782_e:
.L_D783:
	.4byte	.L_D783_e-.L_D783
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D784
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l783_e-.L_l783
.L_l783:
.L_l783_e:
.L_D783_e:
.L_D784:
	.4byte	0x4
.L_D781:
	.4byte	.L_D781_e-.L_D781
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D785
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B133
	.2byte	0x121
	.4byte	.L_B133_e
.L_D781_e:
.L_D786:
	.4byte	.L_D786_e-.L_D786
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D787
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l786_e-.L_l786
.L_l786:
.L_l786_e:
.L_D786_e:
.L_D787:
	.4byte	.L_D787_e-.L_D787
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D788
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l787_e-.L_l787
.L_l787:
.L_l787_e:
.L_D787_e:
.L_D788:
	.4byte	0x4
.L_D785:
	.4byte	.L_D785_e-.L_D785
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D789
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B134
	.2byte	0x121
	.4byte	.L_B134_e
.L_D785_e:
.L_D790:
	.4byte	.L_D790_e-.L_D790
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D791
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l790_e-.L_l790
.L_l790:
.L_l790_e:
.L_D790_e:
.L_D791:
	.4byte	.L_D791_e-.L_D791
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D792
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l791_e-.L_l791
.L_l791:
.L_l791_e:
.L_D791_e:
.L_D792:
	.4byte	0x4
.L_D789:
	.4byte	.L_D789_e-.L_D789
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D793
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B135
	.2byte	0x121
	.4byte	.L_B135_e
.L_D789_e:
.L_D794:
	.4byte	.L_D794_e-.L_D794
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D795
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l794_e-.L_l794
.L_l794:
.L_l794_e:
.L_D794_e:
.L_D795:
	.4byte	.L_D795_e-.L_D795
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D796
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l795_e-.L_l795
.L_l795:
.L_l795_e:
.L_D795_e:
.L_D796:
	.4byte	0x4
.L_D793:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_close_device,@function
sas_close_device:
.L_LC1196:

.section	.line
	.4byte	2134	/ sas.c:2134
	.2byte	0xffff
	.4byte	.L_LC1196-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	subl $8,%esp
	pushl %esi
	pushl %ebx
	movl 8(%ebp),%esi
.L_b15:
.L_LC1197:

.section	.line
	.4byte	2135	/ sas.c:2135
	.2byte	0xffff
	.4byte	.L_LC1197-.L_text_b
	.previous
.L_LC1198:

.section	.line
	.4byte	2137	/ sas.c:2137
	.2byte	0xffff
	.4byte	.L_LC1198-.L_text_b
	.previous
	movb $0,64(%esi)
.L_LC1199:

.section	.line
	.4byte	2138	/ sas.c:2138
	.2byte	0xffff
	.4byte	.L_LC1199-.L_text_b
	.previous
	testb $6,32(%esi)
	je .L616
.L_B137:
.L_LC1200:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1200-.L_text_b
	.previous
.L_LC1201:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1201-.L_text_b
	.previous
	movb 78(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1202:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1202-.L_text_b
	.previous
.L_B137_e:
.L_LC1203:

.section	.line
	.4byte	2138	/ sas.c:2138
	.2byte	0xffff
	.4byte	.L_LC1203-.L_text_b
	.previous
.L616:
.L_B138:
.L_LC1204:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1204-.L_text_b
	.previous
.L_LC1205:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1205-.L_text_b
	.previous
	xorb %al,%al
	movw 76(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1206:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1206-.L_text_b
	.previous
.L_B138_e:
.L_LC1207:

.section	.line
	.4byte	2141	/ sas.c:2141
	.2byte	0xffff
	.4byte	.L_LC1207-.L_text_b
	.previous
	movzbl 62(%esi),%ecx
	cmpb $0,44(%esi)
	jge .L619
	movzbl 56(%esi),%eax
	jmp .L664
	.align 4
.L619:
	movzbl 59(%esi),%eax
.L664:
	notl %eax
	andl %eax,%ecx
	movb %cl,62(%esi)
.L_LC1208:

.section	.line
	.4byte	2144	/ sas.c:2144
	.2byte	0xffff
	.4byte	.L_LC1208-.L_text_b
	.previous
	movb 32(%esi),%al
	andb $4,%al
	movb %al,-4(%ebp)
	je .L622
.L_B139:
.L_LC1209:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1209-.L_text_b
	.previous
.L_LC1210:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1210-.L_text_b
	.previous
	movb 90(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1211:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1211-.L_text_b
	.previous
.L_B139_e:
.L_LC1212:

.section	.line
	.4byte	2144	/ sas.c:2144
	.2byte	0xffff
	.4byte	.L_LC1212-.L_text_b
	.previous
.L622:
.L_B140:
.L_LC1213:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1213-.L_text_b
	.previous
	movw 88(%esi),%bx
.L_LC1214:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1214-.L_text_b
	.previous
	movb 62(%esi),%al
	movl %ebx,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC1215:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1215-.L_text_b
	.previous
.L_B140_e:
.L_LC1216:

.section	.line
	.4byte	2147	/ sas.c:2147
	.2byte	0xffff
	.4byte	.L_LC1216-.L_text_b
	.previous
	cmpb $2,66(%esi)
	jne .L625
.L_LC1217:

.section	.line
	.4byte	2148	/ sas.c:2148
	.2byte	0xffff
	.4byte	.L_LC1217-.L_text_b
	.previous
	cmpb $0,-4(%ebp)
	je .L627
.L_B141:
.L_LC1218:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1218-.L_text_b
	.previous
.L_LC1219:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1219-.L_text_b
	.previous
	movb 82(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1220:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1220-.L_text_b
	.previous
.L_B141_e:
.L_LC1221:

.section	.line
	.4byte	2148	/ sas.c:2148
	.2byte	0xffff
	.4byte	.L_LC1221-.L_text_b
	.previous
.L627:
.L_B142:
.L_LC1222:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1222-.L_text_b
	.previous
.L_LC1223:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1223-.L_text_b
	.previous
	xorb %al,%al
	movw 80(%esi),%dx
.L_LC1224:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1224-.L_text_b
	.previous
.L_B142_e:
.L_LC1225:

.section	.line
	.4byte	2148	/ sas.c:2148
	.2byte	0xffff
	.4byte	.L_LC1225-.L_text_b
	.previous
	jmp .L665
	.align 4
.L625:
.L_LC1226:

.section	.line
	.4byte	2149	/ sas.c:2149
	.2byte	0xffff
	.4byte	.L_LC1226-.L_text_b
	.previous
	cmpb $1,66(%esi)
	jne .L630
.L_LC1227:

.section	.line
	.4byte	2151	/ sas.c:2151
	.2byte	0xffff
	.4byte	.L_LC1227-.L_text_b
	.previous
	cmpb $0,-4(%ebp)
	je .L633
.L_B143:
.L_LC1228:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1228-.L_text_b
	.previous
.L_LC1229:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1229-.L_text_b
	.previous
	movb 82(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1230:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1230-.L_text_b
	.previous
.L_B143_e:
.L_LC1231:

.section	.line
	.4byte	2151	/ sas.c:2151
	.2byte	0xffff
	.4byte	.L_LC1231-.L_text_b
	.previous
.L633:
.L_B144:
.L_LC1232:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1232-.L_text_b
	.previous
	movw 80(%esi),%cx
.L_LC1233:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1233-.L_text_b
	.previous
	movb $32,%al
	movl %ecx,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC1234:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1234-.L_text_b
	.previous
.L_B144_e:
.L_LC1235:

.section	.line
	.4byte	2152	/ sas.c:2152
	.2byte	0xffff
	.4byte	.L_LC1235-.L_text_b
	.previous
	cmpb $0,-4(%ebp)
	je .L637
.L_B145:
.L_LC1236:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1236-.L_text_b
	.previous
.L_LC1237:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1237-.L_text_b
	.previous
	movb 98(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1238:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1238-.L_text_b
	.previous
.L_B145_e:
.L_LC1239:

.section	.line
	.4byte	2152	/ sas.c:2152
	.2byte	0xffff
	.4byte	.L_LC1239-.L_text_b
	.previous
.L637:
.L_B146:
.L_LC1240:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1240-.L_text_b
	.previous
.L_LC1241:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1241-.L_text_b
	.previous
	movb $12,%al
	movw 96(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1242:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1242-.L_text_b
	.previous
.L_B146_e:
.L_LC1243:

.section	.line
	.4byte	2153	/ sas.c:2153
	.2byte	0xffff
	.4byte	.L_LC1243-.L_text_b
	.previous
	cmpb $0,-4(%ebp)
	je .L641
.L_B147:
.L_LC1244:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1244-.L_text_b
	.previous
.L_LC1245:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1245-.L_text_b
	.previous
	movb 94(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1246:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1246-.L_text_b
	.previous
.L_B147_e:
.L_LC1247:

.section	.line
	.4byte	2153	/ sas.c:2153
	.2byte	0xffff
	.4byte	.L_LC1247-.L_text_b
	.previous
.L641:
.L_B148:
.L_LC1248:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1248-.L_text_b
	.previous
.L_LC1249:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1249-.L_text_b
	.previous
	movb $48,%al
	movw 92(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1250:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1250-.L_text_b
	.previous
.L_B148_e:
.L_LC1251:

.section	.line
	.4byte	2154	/ sas.c:2154
	.2byte	0xffff
	.4byte	.L_LC1251-.L_text_b
	.previous
	cmpb $0,-4(%ebp)
	je .L645
.L_B149:
.L_LC1252:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1252-.L_text_b
	.previous
.L_LC1253:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1253-.L_text_b
	.previous
	movb 82(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1254:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1254-.L_text_b
	.previous
.L_B149_e:
.L_LC1255:

.section	.line
	.4byte	2154	/ sas.c:2154
	.2byte	0xffff
	.4byte	.L_LC1255-.L_text_b
	.previous
.L645:
.L_B150:
.L_LC1256:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1256-.L_text_b
	.previous
.L_LC1257:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1257-.L_text_b
	.previous
	movb $64,%al
	movl %ecx,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC1258:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1258-.L_text_b
	.previous
.L_B150_e:
.L_LC1259:

.section	.line
	.4byte	2155	/ sas.c:2155
	.2byte	0xffff
	.4byte	.L_LC1259-.L_text_b
	.previous
	cmpb $0,-4(%ebp)
	je .L649
.L_B151:
.L_LC1260:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1260-.L_text_b
	.previous
.L_LC1261:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1261-.L_text_b
	.previous
	movb 90(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1262:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1262-.L_text_b
	.previous
.L_B151_e:
.L_LC1263:

.section	.line
	.4byte	2155	/ sas.c:2155
	.2byte	0xffff
	.4byte	.L_LC1263-.L_text_b
	.previous
.L649:
.L_B152:
.L_LC1264:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1264-.L_text_b
	.previous
.L_LC1265:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1265-.L_text_b
	.previous
	xorb %al,%al
	movl %ebx,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC1266:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1266-.L_text_b
	.previous
.L_B152_e:
.L_LC1267:

.section	.line
	.4byte	2156	/ sas.c:2156
	.2byte	0xffff
	.4byte	.L_LC1267-.L_text_b
	.previous
	cmpb $0,-4(%ebp)
	je .L653
.L_B153:
.L_LC1268:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1268-.L_text_b
	.previous
.L_LC1269:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1269-.L_text_b
	.previous
	movb 82(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1270:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1270-.L_text_b
	.previous
.L_B153_e:
.L_LC1271:

.section	.line
	.4byte	2156	/ sas.c:2156
	.2byte	0xffff
	.4byte	.L_LC1271-.L_text_b
	.previous
.L653:
.L_B154:
.L_LC1272:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1272-.L_text_b
	.previous
.L_LC1273:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1273-.L_text_b
	.previous
	xorb %al,%al
	movl %ecx,%edx
.L665:
/APP
	outb %al,%dx
/NO_APP
.L_LC1274:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1274-.L_text_b
	.previous
.L_B154_e:
.L630:
.L_LC1275:

.section	.line
	.4byte	2160	/ sas.c:2160
	.2byte	0xffff
	.4byte	.L_LC1275-.L_text_b
	.previous
	movb 63(%esi),%cl
	andb $191,%cl
	movb %cl,63(%esi)
.L_LC1276:

.section	.line
	.4byte	2161	/ sas.c:2161
	.2byte	0xffff
	.4byte	.L_LC1276-.L_text_b
	.previous
	testb $4,32(%esi)
	je .L657
.L_B155:
.L_LC1277:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1277-.L_text_b
	.previous
.L_LC1278:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1278-.L_text_b
	.previous
	movb 86(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1279:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1279-.L_text_b
	.previous
.L_B155_e:
.L_LC1280:

.section	.line
	.4byte	2161	/ sas.c:2161
	.2byte	0xffff
	.4byte	.L_LC1280-.L_text_b
	.previous
.L657:
.L_B156:
.L_LC1281:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1281-.L_text_b
	.previous
.L_LC1282:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1282-.L_text_b
	.previous
	movb %cl,%al
	movw 84(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1283:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1283-.L_text_b
	.previous
.L_B156_e:
.L_LC1284:

.section	.line
	.4byte	2164	/ sas.c:2164
	.2byte	0xffff
	.4byte	.L_LC1284-.L_text_b
	.previous
	andb $247,32(%esi)
.L_LC1285:

.section	.line
	.4byte	2165	/ sas.c:2165
	.2byte	0xffff
	.4byte	.L_LC1285-.L_text_b
	.previous
	movw $0,40(%esi)
.L_LC1286:

.section	.line
	.4byte	2168	/ sas.c:2168
	.2byte	0xffff
	.4byte	.L_LC1286-.L_text_b
	.previous
	cmpl $0,(%esi)
	je .L660
.L_LC1287:

.section	.line
	.4byte	2169	/ sas.c:2169
	.2byte	0xffff
	.4byte	.L_LC1287-.L_text_b
	.previous
	movl 4(%esi),%edx
	movl (%esi),%eax
	movl %edx,4(%eax)
	jmp .L661
	.align 4
.L660:
.L_LC1288:

.section	.line
	.4byte	2171	/ sas.c:2171
	.2byte	0xffff
	.4byte	.L_LC1288-.L_text_b
	.previous
	movzbl 65(%esi),%eax
	movl 4(%esi),%edx
	movl %edx,sas_first_int_user(,%eax,4)
.L661:
.L_LC1289:

.section	.line
	.4byte	2172	/ sas.c:2172
	.2byte	0xffff
	.4byte	.L_LC1289-.L_text_b
	.previous
	cmpl $0,4(%esi)
	je .L662
.L_LC1290:

.section	.line
	.4byte	2173	/ sas.c:2173
	.2byte	0xffff
	.4byte	.L_LC1290-.L_text_b
	.previous
	movl (%esi),%edx
	movl 4(%esi),%eax
	movl %edx,(%eax)
.L662:
.L_LC1291:

.section	.line
	.4byte	2175	/ sas.c:2175
	.2byte	0xffff
	.4byte	.L_LC1291-.L_text_b
	.previous
	testb $4,21(%esi)
	je .L663
.L_LC1292:

.section	.line
	.4byte	2178	/ sas.c:2178
	.2byte	0xffff
	.4byte	.L_LC1292-.L_text_b
	.previous
	orw $512,32(%esi)
.L_LC1293:

.section	.line
	.4byte	2179	/ sas.c:2179
	.2byte	0xffff
	.4byte	.L_LC1293-.L_text_b
	.previous
	pushl $50
	pushl %esi
	pushl $sas_hangup
	call timeout
.L663:
.L_LC1294:

.section	.line
	.4byte	2181	/ sas.c:2181
	.2byte	0xffff
	.4byte	.L_LC1294-.L_text_b
	.previous
.L_b15_e:
	leal -16(%ebp),%esp
	popl %ebx
	popl %esi
	leave
	ret
.L_f15_e:
.Lfe12:
	.size	 sas_close_device,.Lfe12-sas_close_device

.section	.debug
.L_D632:
	.4byte	.L_D632_e-.L_D632
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D797
	.2byte	0x38
	.string	"sas_close_device"
	.2byte	0x111
	.4byte	sas_close_device
	.2byte	0x121
	.4byte	.L_f15_e
	.2byte	0x8041
	.4byte	.L_b15
	.2byte	0x8051
	.4byte	.L_b15_e
.L_D632_e:
.L_D798:
	.4byte	.L_D798_e-.L_D798
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D799
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t798_e-.L_t798
.L_t798:
	.byte	0x1
	.4byte	.L_T816
.L_t798_e:
	.2byte	0x23
	.2byte	.L_l798_e-.L_l798
.L_l798:
	.byte	0x1
	.4byte	0x6
.L_l798_e:
.L_D798_e:
.L_D799:
	.4byte	.L_D799_e-.L_D799
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D800
	.2byte	0x38
	.string	"regvar"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l799_e-.L_l799
.L_l799:
.L_l799_e:
.L_D799_e:
.L_D800:
	.4byte	.L_D800_e-.L_D800
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D801
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B137
	.2byte	0x121
	.4byte	.L_B137_e
.L_D800_e:
.L_D802:
	.4byte	.L_D802_e-.L_D802
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D803
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l802_e-.L_l802
.L_l802:
.L_l802_e:
.L_D802_e:
.L_D803:
	.4byte	.L_D803_e-.L_D803
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D804
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l803_e-.L_l803
.L_l803:
.L_l803_e:
.L_D803_e:
.L_D804:
	.4byte	0x4
.L_D801:
	.4byte	.L_D801_e-.L_D801
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D805
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B138
	.2byte	0x121
	.4byte	.L_B138_e
.L_D801_e:
.L_D806:
	.4byte	.L_D806_e-.L_D806
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D807
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l806_e-.L_l806
.L_l806:
.L_l806_e:
.L_D806_e:
.L_D807:
	.4byte	.L_D807_e-.L_D807
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D808
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l807_e-.L_l807
.L_l807:
.L_l807_e:
.L_D807_e:
.L_D808:
	.4byte	0x4
.L_D805:
	.4byte	.L_D805_e-.L_D805
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D809
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B139
	.2byte	0x121
	.4byte	.L_B139_e
.L_D805_e:
.L_D810:
	.4byte	.L_D810_e-.L_D810
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D811
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l810_e-.L_l810
.L_l810:
.L_l810_e:
.L_D810_e:
.L_D811:
	.4byte	.L_D811_e-.L_D811
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D812
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l811_e-.L_l811
.L_l811:
.L_l811_e:
.L_D811_e:
.L_D812:
	.4byte	0x4
.L_D809:
	.4byte	.L_D809_e-.L_D809
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D813
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B140
	.2byte	0x121
	.4byte	.L_B140_e
.L_D809_e:
.L_D814:
	.4byte	.L_D814_e-.L_D814
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D815
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l814_e-.L_l814
.L_l814:
	.byte	0x1
	.4byte	0x3
.L_l814_e:
.L_D814_e:
.L_D815:
	.4byte	.L_D815_e-.L_D815
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D816
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l815_e-.L_l815
.L_l815:
.L_l815_e:
.L_D815_e:
.L_D816:
	.4byte	0x4
.L_D813:
	.4byte	.L_D813_e-.L_D813
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D817
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B141
	.2byte	0x121
	.4byte	.L_B141_e
.L_D813_e:
.L_D818:
	.4byte	.L_D818_e-.L_D818
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D819
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l818_e-.L_l818
.L_l818:
.L_l818_e:
.L_D818_e:
.L_D819:
	.4byte	.L_D819_e-.L_D819
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D820
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l819_e-.L_l819
.L_l819:
.L_l819_e:
.L_D819_e:
.L_D820:
	.4byte	0x4
.L_D817:
	.4byte	.L_D817_e-.L_D817
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D821
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B142
	.2byte	0x121
	.4byte	.L_B142_e
.L_D817_e:
.L_D822:
	.4byte	.L_D822_e-.L_D822
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D823
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l822_e-.L_l822
.L_l822:
.L_l822_e:
.L_D822_e:
.L_D823:
	.4byte	.L_D823_e-.L_D823
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D824
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l823_e-.L_l823
.L_l823:
	.4byte	0x0
.L_l823_e:
.L_D823_e:
.L_D824:
	.4byte	0x4
.L_D821:
	.4byte	.L_D821_e-.L_D821
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D825
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B143
	.2byte	0x121
	.4byte	.L_B143_e
.L_D821_e:
.L_D826:
	.4byte	.L_D826_e-.L_D826
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D827
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l826_e-.L_l826
.L_l826:
.L_l826_e:
.L_D826_e:
.L_D827:
	.4byte	.L_D827_e-.L_D827
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D828
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l827_e-.L_l827
.L_l827:
.L_l827_e:
.L_D827_e:
.L_D828:
	.4byte	0x4
.L_D825:
	.4byte	.L_D825_e-.L_D825
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D829
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B144
	.2byte	0x121
	.4byte	.L_B144_e
.L_D825_e:
.L_D830:
	.4byte	.L_D830_e-.L_D830
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D831
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l830_e-.L_l830
.L_l830:
	.byte	0x1
	.4byte	0x1
.L_l830_e:
.L_D830_e:
.L_D831:
	.4byte	.L_D831_e-.L_D831
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D832
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l831_e-.L_l831
.L_l831:
	.4byte	0x20
.L_l831_e:
.L_D831_e:
.L_D832:
	.4byte	0x4
.L_D829:
	.4byte	.L_D829_e-.L_D829
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D833
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B145
	.2byte	0x121
	.4byte	.L_B145_e
.L_D829_e:
.L_D834:
	.4byte	.L_D834_e-.L_D834
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D835
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l834_e-.L_l834
.L_l834:
.L_l834_e:
.L_D834_e:
.L_D835:
	.4byte	.L_D835_e-.L_D835
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D836
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l835_e-.L_l835
.L_l835:
.L_l835_e:
.L_D835_e:
.L_D836:
	.4byte	0x4
.L_D833:
	.4byte	.L_D833_e-.L_D833
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D837
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B146
	.2byte	0x121
	.4byte	.L_B146_e
.L_D833_e:
.L_D838:
	.4byte	.L_D838_e-.L_D838
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D839
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l838_e-.L_l838
.L_l838:
.L_l838_e:
.L_D838_e:
.L_D839:
	.4byte	.L_D839_e-.L_D839
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D840
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l839_e-.L_l839
.L_l839:
	.4byte	0xc
.L_l839_e:
.L_D839_e:
.L_D840:
	.4byte	0x4
.L_D837:
	.4byte	.L_D837_e-.L_D837
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D841
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B147
	.2byte	0x121
	.4byte	.L_B147_e
.L_D837_e:
.L_D842:
	.4byte	.L_D842_e-.L_D842
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D843
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l842_e-.L_l842
.L_l842:
.L_l842_e:
.L_D842_e:
.L_D843:
	.4byte	.L_D843_e-.L_D843
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D844
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l843_e-.L_l843
.L_l843:
.L_l843_e:
.L_D843_e:
.L_D844:
	.4byte	0x4
.L_D841:
	.4byte	.L_D841_e-.L_D841
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D845
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B148
	.2byte	0x121
	.4byte	.L_B148_e
.L_D841_e:
.L_D846:
	.4byte	.L_D846_e-.L_D846
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D847
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l846_e-.L_l846
.L_l846:
.L_l846_e:
.L_D846_e:
.L_D847:
	.4byte	.L_D847_e-.L_D847
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D848
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l847_e-.L_l847
.L_l847:
	.4byte	0x30
.L_l847_e:
.L_D847_e:
.L_D848:
	.4byte	0x4
.L_D845:
	.4byte	.L_D845_e-.L_D845
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D849
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B149
	.2byte	0x121
	.4byte	.L_B149_e
.L_D845_e:
.L_D850:
	.4byte	.L_D850_e-.L_D850
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D851
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l850_e-.L_l850
.L_l850:
.L_l850_e:
.L_D850_e:
.L_D851:
	.4byte	.L_D851_e-.L_D851
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D852
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l851_e-.L_l851
.L_l851:
.L_l851_e:
.L_D851_e:
.L_D852:
	.4byte	0x4
.L_D849:
	.4byte	.L_D849_e-.L_D849
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D853
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B150
	.2byte	0x121
	.4byte	.L_B150_e
.L_D849_e:
.L_D854:
	.4byte	.L_D854_e-.L_D854
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D855
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l854_e-.L_l854
.L_l854:
.L_l854_e:
.L_D854_e:
.L_D855:
	.4byte	.L_D855_e-.L_D855
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D856
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l855_e-.L_l855
.L_l855:
	.4byte	0x40
.L_l855_e:
.L_D855_e:
.L_D856:
	.4byte	0x4
.L_D853:
	.4byte	.L_D853_e-.L_D853
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D857
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B151
	.2byte	0x121
	.4byte	.L_B151_e
.L_D853_e:
.L_D858:
	.4byte	.L_D858_e-.L_D858
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D859
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l858_e-.L_l858
.L_l858:
.L_l858_e:
.L_D858_e:
.L_D859:
	.4byte	.L_D859_e-.L_D859
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D860
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l859_e-.L_l859
.L_l859:
.L_l859_e:
.L_D859_e:
.L_D860:
	.4byte	0x4
.L_D857:
	.4byte	.L_D857_e-.L_D857
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D861
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B152
	.2byte	0x121
	.4byte	.L_B152_e
.L_D857_e:
.L_D862:
	.4byte	.L_D862_e-.L_D862
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D863
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l862_e-.L_l862
.L_l862:
.L_l862_e:
.L_D862_e:
.L_D863:
	.4byte	.L_D863_e-.L_D863
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D864
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l863_e-.L_l863
.L_l863:
	.4byte	0x0
.L_l863_e:
.L_D863_e:
.L_D864:
	.4byte	0x4
.L_D861:
	.4byte	.L_D861_e-.L_D861
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D865
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B153
	.2byte	0x121
	.4byte	.L_B153_e
.L_D861_e:
.L_D866:
	.4byte	.L_D866_e-.L_D866
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D867
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l866_e-.L_l866
.L_l866:
.L_l866_e:
.L_D866_e:
.L_D867:
	.4byte	.L_D867_e-.L_D867
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D868
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l867_e-.L_l867
.L_l867:
.L_l867_e:
.L_D867_e:
.L_D868:
	.4byte	0x4
.L_D865:
	.4byte	.L_D865_e-.L_D865
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D869
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B154
	.2byte	0x121
	.4byte	.L_B154_e
.L_D865_e:
.L_D870:
	.4byte	.L_D870_e-.L_D870
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D871
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l870_e-.L_l870
.L_l870:
.L_l870_e:
.L_D870_e:
.L_D871:
	.4byte	.L_D871_e-.L_D871
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D872
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l871_e-.L_l871
.L_l871:
	.4byte	0x0
.L_l871_e:
.L_D871_e:
.L_D872:
	.4byte	0x4
.L_D869:
	.4byte	.L_D869_e-.L_D869
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D873
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B155
	.2byte	0x121
	.4byte	.L_B155_e
.L_D869_e:
.L_D874:
	.4byte	.L_D874_e-.L_D874
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D875
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l874_e-.L_l874
.L_l874:
.L_l874_e:
.L_D874_e:
.L_D875:
	.4byte	.L_D875_e-.L_D875
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D876
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l875_e-.L_l875
.L_l875:
.L_l875_e:
.L_D875_e:
.L_D876:
	.4byte	0x4
.L_D873:
	.4byte	.L_D873_e-.L_D873
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D877
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B156
	.2byte	0x121
	.4byte	.L_B156_e
.L_D873_e:
.L_D878:
	.4byte	.L_D878_e-.L_D878
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D879
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l878_e-.L_l878
.L_l878:
.L_l878_e:
.L_D878_e:
.L_D879:
	.4byte	.L_D879_e-.L_D879
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D880
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l879_e-.L_l879
.L_l879:
.L_l879_e:
.L_D879_e:
.L_D880:
	.4byte	0x4
.L_D877:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_make_ctl_val,@function
sas_make_ctl_val:
.L_LC1295:

.section	.line
	.4byte	2189	/ sas.c:2189
	.2byte	0xffff
	.4byte	.L_LC1295-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	pushl %ebx
	movl 8(%ebp),%edx
	movl 12(%ebp),%eax
	movl 16(%ebp),%ebx
.L_b16:
.L_LC1296:

.section	.line
	.4byte	2190	/ sas.c:2190
	.2byte	0xffff
	.4byte	.L_LC1296-.L_text_b
	.previous
.L_LC1297:

.section	.line
	.4byte	2193	/ sas.c:2193
	.2byte	0xffff
	.4byte	.L_LC1297-.L_text_b
	.previous
	testb $2,32(%edx)
	je .L667
.L_LC1298:

.section	.line
	.4byte	2194	/ sas.c:2194
	.2byte	0xffff
	.4byte	.L_LC1298-.L_text_b
	.previous
	movl sas_ctl_val(,%eax,4),%eax
	jmp .L675
	.align 4
.L667:
.L_LC1299:

.section	.line
	.4byte	2196	/ sas.c:2196
	.2byte	0xffff
	.4byte	.L_LC1299-.L_text_b
	.previous
	testb $4,32(%edx)
	je .L668
.L_LC1300:

.section	.line
	.4byte	2198	/ sas.c:2198
	.2byte	0xffff
	.4byte	.L_LC1300-.L_text_b
	.previous
	xorl %ecx,%ecx
	movl sas_ctl_val(,%eax,4),%edx
	movl %edx,%eax
	sall $8,%eax
	.align 4
.L674:
.L_LC1301:

.section	.line
	.4byte	2201	/ sas.c:2201
	.2byte	0xffff
	.4byte	.L_LC1301-.L_text_b
	.previous
	testb $1,%dh
	je .L672
.L_LC1302:

.section	.line
	.4byte	2203	/ sas.c:2203
	.2byte	0xffff
	.4byte	.L_LC1302-.L_text_b
	.previous
	testb $1,%bl
	je .L673
.L_LC1303:

.section	.line
	.4byte	2204	/ sas.c:2204
	.2byte	0xffff
	.4byte	.L_LC1303-.L_text_b
	.previous
	xorb $1,%ah
.L673:
.L_LC1304:

.section	.line
	.4byte	2205	/ sas.c:2205
	.2byte	0xffff
	.4byte	.L_LC1304-.L_text_b
	.previous
	shrl $1,%ebx
.L672:
.L_LC1305:

.section	.line
	.4byte	2207	/ sas.c:2207
	.2byte	0xffff
	.4byte	.L_LC1305-.L_text_b
	.previous
	shrl $1,%edx
.L_LC1306:

.section	.line
	.4byte	2208	/ sas.c:2208
	.2byte	0xffff
	.4byte	.L_LC1306-.L_text_b
	.previous
	shrl $1,%eax
.L_LC1307:

.section	.line
	.4byte	2199	/ sas.c:2199
	.2byte	0xffff
	.4byte	.L_LC1307-.L_text_b
	.previous
	incl %ecx
	cmpl $7,%ecx
	jbe .L674
.L_LC1308:

.section	.line
	.4byte	2210	/ sas.c:2210
	.2byte	0xffff
	.4byte	.L_LC1308-.L_text_b
	.previous
	jmp .L675
	.align 4
.L668:
.L_LC1309:

.section	.line
	.4byte	2212	/ sas.c:2212
	.2byte	0xffff
	.4byte	.L_LC1309-.L_text_b
	.previous
	xorl %eax,%eax
.L675:
.L_LC1310:

.section	.line
	.4byte	2213	/ sas.c:2213
	.2byte	0xffff
	.4byte	.L_LC1310-.L_text_b
	.previous
.L_b16_e:
	movl -4(%ebp),%ebx
	leave
	ret
.L_f16_e:
.Lfe13:
	.size	 sas_make_ctl_val,.Lfe13-sas_make_ctl_val

.section	.debug
.L_D797:
	.4byte	.L_D797_e-.L_D797
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D881
	.2byte	0x38
	.string	"sas_make_ctl_val"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x111
	.4byte	sas_make_ctl_val
	.2byte	0x121
	.4byte	.L_f16_e
	.2byte	0x8041
	.4byte	.L_b16
	.2byte	0x8051
	.4byte	.L_b16_e
.L_D797_e:
.L_D882:
	.4byte	.L_D882_e-.L_D882
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D883
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t882_e-.L_t882
.L_t882:
	.byte	0x1
	.4byte	.L_T816
.L_t882_e:
	.2byte	0x23
	.2byte	.L_l882_e-.L_l882
.L_l882:
	.byte	0x1
	.4byte	0x2
.L_l882_e:
.L_D882_e:
.L_D883:
	.4byte	.L_D883_e-.L_D883
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D884
	.2byte	0x38
	.string	"unit"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l883_e-.L_l883
.L_l883:
	.byte	0x1
	.4byte	0x0
.L_l883_e:
.L_D883_e:
.L_D884:
	.4byte	.L_D884_e-.L_D884
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D885
	.2byte	0x38
	.string	"num"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l884_e-.L_l884
.L_l884:
	.byte	0x1
	.4byte	0x3
.L_l884_e:
.L_D884_e:
.L_D885:
	.4byte	.L_D885_e-.L_D885
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D886
	.2byte	0x38
	.string	"mask"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l885_e-.L_l885
.L_l885:
	.byte	0x1
	.4byte	0x2
.L_l885_e:
.L_D885_e:
.L_D886:
	.4byte	.L_D886_e-.L_D886
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D887
	.2byte	0x38
	.string	"val"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l886_e-.L_l886
.L_l886:
	.byte	0x1
	.4byte	0x0
.L_l886_e:
.L_D886_e:
.L_D887:
	.4byte	.L_D887_e-.L_D887
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D888
	.2byte	0x38
	.string	"i"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l887_e-.L_l887
.L_l887:
	.byte	0x1
	.4byte	0x1
.L_l887_e:
.L_D887_e:
.L_D888:
	.4byte	0x4
	.previous
.data
	.align 4
	.type	 lcrval.32,@object
	.size	 lcrval.32,12
lcrval.32:
	.long 3
	.long 11
	.long 27
.section	.rodata
.LC10:
	.string	"\377U\252%\244"
	.string	""
	.string	""
.LC11:
	.string	"\005b\n\237\006X\t\257\017\361"
	.string	"\017"
	.string	""
	.string	""
.text
	.align 4
	.type	 sas_test_device,@function
sas_test_device:
.L_LC1311:

.section	.line
	.4byte	2219	/ sas.c:2219
	.2byte	0xffff
	.4byte	.L_LC1311-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	subl $40,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 8(%ebp),%ecx
.L_b17:
.L_LC1312:

.section	.line
	.4byte	2220	/ sas.c:2220
	.2byte	0xffff
	.4byte	.L_LC1312-.L_text_b
	.previous
.L_LC1313:

.section	.line
	.4byte	2232	/ sas.c:2232
	.2byte	0xffff
	.4byte	.L_LC1313-.L_text_b
	.previous
	testb $6,32(%ecx)
	je .L678
.L_B159:
.L_LC1314:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1314-.L_text_b
	.previous
.L_LC1315:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1315-.L_text_b
	.previous
	movb 82(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1316:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1316-.L_text_b
	.previous
.L_B159_e:
.L_LC1317:

.section	.line
	.4byte	2232	/ sas.c:2232
	.2byte	0xffff
	.4byte	.L_LC1317-.L_text_b
	.previous
.L678:
.L_B160:
.L_LC1318:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1318-.L_text_b
	.previous
	movw 80(%ecx),%di
.L_LC1319:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1319-.L_text_b
	.previous
	xorb %al,%al
	movl %edi,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC1320:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1320-.L_text_b
	.previous
.L_B160_e:
.L_LC1321:

.section	.line
	.4byte	2233	/ sas.c:2233
	.2byte	0xffff
	.4byte	.L_LC1321-.L_text_b
	.previous
	movb 32(%ecx),%bl
	andb $4,%bl
	je .L682
.L_B161:
.L_LC1322:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1322-.L_text_b
	.previous
.L_LC1323:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1323-.L_text_b
	.previous
	movb 82(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1324:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1324-.L_text_b
	.previous
.L_B161_e:
.L_LC1325:

.section	.line
	.4byte	2233	/ sas.c:2233
	.2byte	0xffff
	.4byte	.L_LC1325-.L_text_b
	.previous
.L682:
.L_B162:
.L_LC1326:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1326-.L_text_b
	.previous
.L_LC1327:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1327-.L_text_b
	.previous
	movb $64,%al
	movl %edi,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC1328:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1328-.L_text_b
	.previous
.L_B162_e:
.L_LC1329:

.section	.line
	.4byte	2234	/ sas.c:2234
	.2byte	0xffff
	.4byte	.L_LC1329-.L_text_b
	.previous
	testb %bl,%bl
	je .L686
.L_B163:
.L_LC1330:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1330-.L_text_b
	.previous
.L_LC1331:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1331-.L_text_b
	.previous
	movb 90(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1332:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1332-.L_text_b
	.previous
.L_B163_e:
.L_LC1333:

.section	.line
	.4byte	2234	/ sas.c:2234
	.2byte	0xffff
	.4byte	.L_LC1333-.L_text_b
	.previous
.L686:
.L_B164:
.L_LC1334:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1334-.L_text_b
	.previous
	movw 88(%ecx),%si
	movw %si,-36(%ebp)
.L_LC1335:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1335-.L_text_b
	.previous
	xorb %al,%al
	movl %esi,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC1336:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1336-.L_text_b
	.previous
.L_B164_e:
.L_LC1337:

.section	.line
	.4byte	2235	/ sas.c:2235
	.2byte	0xffff
	.4byte	.L_LC1337-.L_text_b
	.previous
	testb %bl,%bl
	je .L690
.L_B165:
.L_LC1338:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1338-.L_text_b
	.previous
.L_LC1339:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1339-.L_text_b
	.previous
	movb 82(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1340:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1340-.L_text_b
	.previous
.L_B165_e:
.L_LC1341:

.section	.line
	.4byte	2235	/ sas.c:2235
	.2byte	0xffff
	.4byte	.L_LC1341-.L_text_b
	.previous
.L690:
.L_B166:
.L_LC1342:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1342-.L_text_b
	.previous
.L_LC1343:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1343-.L_text_b
	.previous
	xorb %al,%al
	movl %edi,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC1344:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1344-.L_text_b
	.previous
.L_B166_e:
.L_LC1345:

.section	.line
	.4byte	2238	/ sas.c:2238
	.2byte	0xffff
	.4byte	.L_LC1345-.L_text_b
	.previous
	testb %bl,%bl
	je .L694
.L_B167:
.L_LC1346:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1346-.L_text_b
	.previous
.L_LC1347:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1347-.L_text_b
	.previous
	movb 86(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1348:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1348-.L_text_b
	.previous
.L_B167_e:
.L_LC1349:

.section	.line
	.4byte	2238	/ sas.c:2238
	.2byte	0xffff
	.4byte	.L_LC1349-.L_text_b
	.previous
.L694:
.L_B168:
.L_LC1350:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1350-.L_text_b
	.previous
	movw 84(%ecx),%di
.L_LC1351:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1351-.L_text_b
	.previous
	movb $-128,%al
	movl %edi,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC1352:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1352-.L_text_b
	.previous
.L_B168_e:
.L_LC1353:

.section	.line
	.4byte	2239	/ sas.c:2239
	.2byte	0xffff
	.4byte	.L_LC1353-.L_text_b
	.previous
	testb %bl,%bl
	je .L698
.L_B169:
.L_LC1354:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1354-.L_text_b
	.previous
.L_LC1355:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1355-.L_text_b
	.previous
	movb 74(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1356:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1356-.L_text_b
	.previous
.L_B169_e:
.L_LC1357:

.section	.line
	.4byte	2239	/ sas.c:2239
	.2byte	0xffff
	.4byte	.L_LC1357-.L_text_b
	.previous
.L698:
.L_B170:
.L_LC1358:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1358-.L_text_b
	.previous
.L_LC1359:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1359-.L_text_b
	.previous
	movb sas_speeds,%al
	movw 72(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1360:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1360-.L_text_b
	.previous
.L_B170_e:
.L_LC1361:

.section	.line
	.4byte	2240	/ sas.c:2240
	.2byte	0xffff
	.4byte	.L_LC1361-.L_text_b
	.previous
	testb %bl,%bl
	je .L702
.L_B171:
.L_LC1362:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1362-.L_text_b
	.previous
.L_LC1363:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1363-.L_text_b
	.previous
	movb 78(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1364:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1364-.L_text_b
	.previous
.L_B171_e:
.L_LC1365:

.section	.line
	.4byte	2240	/ sas.c:2240
	.2byte	0xffff
	.4byte	.L_LC1365-.L_text_b
	.previous
.L702:
.L_B172:
.L_LC1366:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1366-.L_text_b
	.previous
	movl sas_speeds,%eax
	shrl $8,%eax
.L_LC1367:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1367-.L_text_b
	.previous
	movw 76(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1368:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1368-.L_text_b
	.previous
.L_B172_e:
.L_LC1369:

.section	.line
	.4byte	2241	/ sas.c:2241
	.2byte	0xffff
	.4byte	.L_LC1369-.L_text_b
	.previous
	testb %bl,%bl
	je .L706
.L_B173:
.L_LC1370:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1370-.L_text_b
	.previous
.L_LC1371:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1371-.L_text_b
	.previous
	movb 86(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1372:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1372-.L_text_b
	.previous
.L_B173_e:
.L_LC1373:

.section	.line
	.4byte	2241	/ sas.c:2241
	.2byte	0xffff
	.4byte	.L_LC1373-.L_text_b
	.previous
.L706:
.L_B174:
.L_LC1374:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1374-.L_text_b
	.previous
.L_LC1375:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1375-.L_text_b
	.previous
	xorb %al,%al
	movl %edi,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC1376:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1376-.L_text_b
	.previous
.L_B174_e:
.L_LC1377:

.section	.line
	.4byte	2244	/ sas.c:2244
	.2byte	0xffff
	.4byte	.L_LC1377-.L_text_b
	.previous
	testb %bl,%bl
	je .L710
.L_B175:
.L_LC1378:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1378-.L_text_b
	.previous
.L_LC1379:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1379-.L_text_b
	.previous
	movb 90(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1380:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1380-.L_text_b
	.previous
.L_B175_e:
.L_LC1381:

.section	.line
	.4byte	2244	/ sas.c:2244
	.2byte	0xffff
	.4byte	.L_LC1381-.L_text_b
	.previous
.L710:
.L_B176:
.L_LC1382:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1382-.L_text_b
	.previous
.L_LC1383:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1383-.L_text_b
	.previous
	movb $16,%al
	movw -36(%ebp),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1384:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1384-.L_text_b
	.previous
.L_B176_e:
.L_LC1385:

.section	.line
	.4byte	2246	/ sas.c:2246
	.2byte	0xffff
	.4byte	.L_LC1385-.L_text_b
	.previous
	movl $0,-4(%ebp)
.L_LC1386:

.section	.line
	.4byte	2249	/ sas.c:2249
	.2byte	0xffff
	.4byte	.L_LC1386-.L_text_b
	.previous
	movl $20000,-32(%ebp)
.L_LC1387:

.section	.line
	.4byte	2251	/ sas.c:2251
	.2byte	0xffff
	.4byte	.L_LC1387-.L_text_b
	.previous
	movb %bl,-12(%ebp)
	.align 4
.L720:
	cmpb $0,-12(%ebp)
	je .L717
.L_B177:
.L_LC1388:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1388-.L_text_b
	.previous
.L_LC1389:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1389-.L_text_b
	.previous
	movb 94(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1390:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1390-.L_text_b
	.previous
.L_B177_e:
.L_LC1391:

.section	.line
	.4byte	2252	/ sas.c:2252
	.2byte	0xffff
	.4byte	.L_LC1391-.L_text_b
	.previous
.L717:
.L_B178:
.L_LC1392:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1392-.L_text_b
	.previous
	movw 92(%ecx),%bx
.L_LC1393:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1393-.L_text_b
	.previous
.L_B179:
.L_LC1394:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1394-.L_text_b
	.previous
	movl %ebx,%edx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1395:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1395-.L_text_b
	.previous
.L_B179_e:
.L_B178_e:
.L_LC1396:

.section	.line
	.4byte	2252	/ sas.c:2252
	.2byte	0xffff
	.4byte	.L_LC1396-.L_text_b
	.previous
	notb %al
	testb $96,%al
	je .L714
	decl -32(%ebp)
.L_LC1397:

.section	.line
	.4byte	2251	/ sas.c:2251
	.2byte	0xffff
	.4byte	.L_LC1397-.L_text_b
	.previous
	jne .L720
.L714:
.L_LC1398:

.section	.line
	.4byte	2255	/ sas.c:2255
	.2byte	0xffff
	.4byte	.L_LC1398-.L_text_b
	.previous
	cmpl $0,-32(%ebp)
	jne .L721
.L_LC1399:

.section	.line
	.4byte	2256	/ sas.c:2256
	.2byte	0xffff
	.4byte	.L_LC1399-.L_text_b
	.previous
	movl $1,-4(%ebp)
.L721:
.L_LC1400:

.section	.line
	.4byte	2258	/ sas.c:2258
	.2byte	0xffff
	.4byte	.L_LC1400-.L_text_b
	.previous
	cmpl $0,-4(%ebp)
	jne .L836
.L_LC1401:

.section	.line
	.4byte	2261	/ sas.c:2261
	.2byte	0xffff
	.4byte	.L_LC1401-.L_text_b
	.previous
	movb 32(%ecx),%al
	andb $4,%al
	movb %al,-28(%ebp)
	je .L724
.L_B180:
.L_LC1402:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1402-.L_text_b
	.previous
.L_LC1403:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1403-.L_text_b
	.previous
	movb 74(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1404:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1404-.L_text_b
	.previous
.L_B180_e:
.L_LC1405:

.section	.line
	.4byte	2261	/ sas.c:2261
	.2byte	0xffff
	.4byte	.L_LC1405-.L_text_b
	.previous
.L724:
.L_B181:
.L_LC1406:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1406-.L_text_b
	.previous
	movw 72(%ecx),%di
.L_LC1407:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1407-.L_text_b
	.previous
.L_B182:
.L_LC1408:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1408-.L_text_b
	.previous
	movl %edi,%edx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1409:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1409-.L_text_b
	.previous
.L_B182_e:
.L_B181_e:
.L_LC1410:

.section	.line
	.4byte	2262	/ sas.c:2262
	.2byte	0xffff
	.4byte	.L_LC1410-.L_text_b
	.previous
	cmpb $0,-28(%ebp)
	je .L728
.L_B183:
.L_LC1411:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1411-.L_text_b
	.previous
.L_LC1412:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1412-.L_text_b
	.previous
	movb 74(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1413:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1413-.L_text_b
	.previous
.L_B183_e:
.L_LC1414:

.section	.line
	.4byte	2262	/ sas.c:2262
	.2byte	0xffff
	.4byte	.L_LC1414-.L_text_b
	.previous
.L728:
.L_B184:
.L_LC1415:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1415-.L_text_b
	.previous
.L_LC1416:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1416-.L_text_b
	.previous
.L_B185:
.L_LC1417:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1417-.L_text_b
	.previous
	movl %edi,%edx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1418:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1418-.L_text_b
	.previous
.L_B185_e:
.L_B184_e:
.L_LC1419:

.section	.line
	.4byte	2263	/ sas.c:2263
	.2byte	0xffff
	.4byte	.L_LC1419-.L_text_b
	.previous
	cmpb $0,-28(%ebp)
	je .L732
.L_B186:
.L_LC1420:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1420-.L_text_b
	.previous
.L_LC1421:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1421-.L_text_b
	.previous
	movb 94(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1422:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1422-.L_text_b
	.previous
.L_B186_e:
.L_LC1423:

.section	.line
	.4byte	2263	/ sas.c:2263
	.2byte	0xffff
	.4byte	.L_LC1423-.L_text_b
	.previous
.L732:
.L_B187:
.L_LC1424:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1424-.L_text_b
	.previous
.L_LC1425:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1425-.L_text_b
	.previous
.L_B188:
.L_LC1426:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1426-.L_text_b
	.previous
	movl %ebx,%edx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1427:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1427-.L_text_b
	.previous
.L_B188_e:
.L_B187_e:
.L_LC1428:

.section	.line
	.4byte	2268	/ sas.c:2268
	.2byte	0xffff
	.4byte	.L_LC1428-.L_text_b
	.previous
	movl $20000,-32(%ebp)
.L_LC1429:

.section	.line
	.4byte	2270	/ sas.c:2270
	.2byte	0xffff
	.4byte	.L_LC1429-.L_text_b
	.previous
	movb -28(%ebp),%bl
	.align 4
.L742:
	testb %bl,%bl
	je .L739
.L_B189:
.L_LC1430:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1430-.L_text_b
	.previous
.L_LC1431:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1431-.L_text_b
	.previous
	movb 94(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1432:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1432-.L_text_b
	.previous
.L_B189_e:
.L_LC1433:

.section	.line
	.4byte	2271	/ sas.c:2271
	.2byte	0xffff
	.4byte	.L_LC1433-.L_text_b
	.previous
.L739:
.L_B190:
.L_LC1434:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1434-.L_text_b
	.previous
.L_LC1435:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1435-.L_text_b
	.previous
.L_B191:
.L_LC1436:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1436-.L_text_b
	.previous
	movw 92(%ecx),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1437:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1437-.L_text_b
	.previous
.L_B191_e:
.L_B190_e:
.L_LC1438:

.section	.line
	.4byte	2271	/ sas.c:2271
	.2byte	0xffff
	.4byte	.L_LC1438-.L_text_b
	.previous
	testb $1,%al
	jne .L736
	decl -32(%ebp)
.L_LC1439:

.section	.line
	.4byte	2270	/ sas.c:2270
	.2byte	0xffff
	.4byte	.L_LC1439-.L_text_b
	.previous
	jne .L742
.L736:
.L_LC1440:

.section	.line
	.4byte	2274	/ sas.c:2274
	.2byte	0xffff
	.4byte	.L_LC1440-.L_text_b
	.previous
	cmpl $0,-32(%ebp)
	je .L743
.L_LC1441:

.section	.line
	.4byte	2275	/ sas.c:2275
	.2byte	0xffff
	.4byte	.L_LC1441-.L_text_b
	.previous
	testb %bl,%bl
	je .L745
.L_B192:
.L_LC1442:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1442-.L_text_b
	.previous
.L_LC1443:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1443-.L_text_b
	.previous
	movb 74(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1444:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1444-.L_text_b
	.previous
.L_B192_e:
.L_LC1445:

.section	.line
	.4byte	2275	/ sas.c:2275
	.2byte	0xffff
	.4byte	.L_LC1445-.L_text_b
	.previous
.L745:
.L_B193:
.L_LC1446:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1446-.L_text_b
	.previous
.L_LC1447:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1447-.L_text_b
	.previous
.L_B194:
.L_LC1448:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1448-.L_text_b
	.previous
	movw 72(%ecx),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1449:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1449-.L_text_b
	.previous
.L_B194_e:
.L_B193_e:
.L743:
.L_LC1450:

.section	.line
	.4byte	2277	/ sas.c:2277
	.2byte	0xffff
	.4byte	.L_LC1450-.L_text_b
	.previous
	movl $100,-8(%ebp)
	movb 32(%ecx),%al
	andb $4,%al
	movb %al,-16(%ebp)
	.align 4
.L835:
.L_LC1451:

.section	.line
	.4byte	2279	/ sas.c:2279
	.2byte	0xffff
	.4byte	.L_LC1451-.L_text_b
	.previous
	movl $lcrval.32,-24(%ebp)
	.align 4
.L833:
.L_LC1452:

.section	.line
	.4byte	2282	/ sas.c:2282
	.2byte	0xffff
	.4byte	.L_LC1452-.L_text_b
	.previous
	movl $.LC10,%edi
.L_LC1453:

.section	.line
	.4byte	2285	/ sas.c:2285
	.2byte	0xffff
	.4byte	.L_LC1453-.L_text_b
	.previous
	cmpb $0,-16(%ebp)
	je .L756
.L_B195:
.L_LC1454:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1454-.L_text_b
	.previous
.L_LC1455:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1455-.L_text_b
	.previous
	movb 94(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1456:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1456-.L_text_b
	.previous
.L_B195_e:
.L_LC1457:

.section	.line
	.4byte	2286	/ sas.c:2286
	.2byte	0xffff
	.4byte	.L_LC1457-.L_text_b
	.previous
.L756:
.L_B196:
.L_LC1458:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1458-.L_text_b
	.previous
.L_LC1459:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1459-.L_text_b
	.previous
.L_B197:
.L_LC1460:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1460-.L_text_b
	.previous
	movw 92(%ecx),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1461:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1461-.L_text_b
	.previous
.L_B197_e:
.L_B196_e:
.L_LC1462:

.section	.line
	.4byte	2286	/ sas.c:2286
	.2byte	0xffff
	.4byte	.L_LC1462-.L_text_b
	.previous
	cmpb $96,%al
	jne .L877
.L_LC1463:

.section	.line
	.4byte	2295	/ sas.c:2295
	.2byte	0xffff
	.4byte	.L_LC1463-.L_text_b
	.previous
	cmpb $0,-16(%ebp)
	je .L760
.L_B198:
.L_LC1464:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1464-.L_text_b
	.previous
.L_LC1465:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1465-.L_text_b
	.previous
	movb 86(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1466:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1466-.L_text_b
	.previous
.L_B198_e:
.L_LC1467:

.section	.line
	.4byte	2295	/ sas.c:2295
	.2byte	0xffff
	.4byte	.L_LC1467-.L_text_b
	.previous
.L760:
.L_B199:
.L_LC1468:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1468-.L_text_b
	.previous
.L_LC1469:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1469-.L_text_b
	.previous
	movl -24(%ebp),%eax
	movb (%eax),%al
	movw 84(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1470:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1470-.L_text_b
	.previous
.L_B199_e:
.L_LC1471:

.section	.line
	.4byte	2298	/ sas.c:2298
	.2byte	0xffff
	.4byte	.L_LC1471-.L_text_b
	.previous
	cmpb $0,-16(%ebp)
	je .L764
.L_B200:
.L_LC1472:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1472-.L_text_b
	.previous
.L_LC1473:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1473-.L_text_b
	.previous
	movb 74(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1474:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1474-.L_text_b
	.previous
.L_B200_e:
.L_LC1475:

.section	.line
	.4byte	2298	/ sas.c:2298
	.2byte	0xffff
	.4byte	.L_LC1475-.L_text_b
	.previous
.L764:
.L_B201:
.L_LC1476:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1476-.L_text_b
	.previous
.L_LC1477:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1477-.L_text_b
	.previous
	movb .LC10,%al
	movw 72(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1478:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1478-.L_text_b
	.previous
.L_B201_e:
.L_LC1479:

.section	.line
	.4byte	2303	/ sas.c:2303
	.2byte	0xffff
	.4byte	.L_LC1479-.L_text_b
	.previous
	movl $20000,-32(%ebp)
.L_LC1480:

.section	.line
	.4byte	2307	/ sas.c:2307
	.2byte	0xffff
	.4byte	.L_LC1480-.L_text_b
	.previous
	movb 32(%ecx),%bl
	andb $4,%bl
	.align 4
.L774:
	testb %bl,%bl
	je .L771
.L_B202:
.L_LC1481:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1481-.L_text_b
	.previous
.L_LC1482:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1482-.L_text_b
	.previous
	movb 94(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1483:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1483-.L_text_b
	.previous
.L_B202_e:
.L_LC1484:

.section	.line
	.4byte	2308	/ sas.c:2308
	.2byte	0xffff
	.4byte	.L_LC1484-.L_text_b
	.previous
.L771:
.L_B203:
.L_LC1485:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1485-.L_text_b
	.previous
.L_LC1486:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1486-.L_text_b
	.previous
.L_B204:
.L_LC1487:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1487-.L_text_b
	.previous
	movw 92(%ecx),%dx
/APP
	inb %dx,%eax
/NO_APP
	movl %eax,-40(%ebp)
.L_LC1488:

.section	.line
	.4byte	113	/ sas.c:113
	.2byte	0xffff
	.4byte	.L_LC1488-.L_text_b
	.previous
	movb -40(%ebp),%al
	movb %al,-28(%ebp)
.L_LC1489:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1489-.L_text_b
	.previous
.L_B204_e:
.L_B203_e:
.L_LC1490:

.section	.line
	.4byte	2308	/ sas.c:2308
	.2byte	0xffff
	.4byte	.L_LC1490-.L_text_b
	.previous
	movzbl -28(%ebp),%edx
	movl %edx,-28(%ebp)
	testb $32,%dl
	jne .L768
	decl -32(%ebp)
.L_LC1491:

.section	.line
	.4byte	2307	/ sas.c:2307
	.2byte	0xffff
	.4byte	.L_LC1491-.L_text_b
	.previous
	jne .L774
.L768:
.L_LC1492:

.section	.line
	.4byte	2311	/ sas.c:2311
	.2byte	0xffff
	.4byte	.L_LC1492-.L_text_b
	.previous
	cmpl $0,-32(%ebp)
	jne .L775
.L_LC1493:

.section	.line
	.4byte	2313	/ sas.c:2313
	.2byte	0xffff
	.4byte	.L_LC1493-.L_text_b
	.previous
	movl $3,-4(%ebp)
.L_LC1494:

.section	.line
	.4byte	2314	/ sas.c:2314
	.2byte	0xffff
	.4byte	.L_LC1494-.L_text_b
	.previous
	jmp .L836
	.align 4
.L775:
.L_LC1495:

.section	.line
	.4byte	2318	/ sas.c:2318
	.2byte	0xffff
	.4byte	.L_LC1495-.L_text_b
	.previous
	cmpl $32,-28(%ebp)
	jne .L877
	.align 4
.L777:
.L_LC1496:

.section	.line
	.4byte	2326	/ sas.c:2326
	.2byte	0xffff
	.4byte	.L_LC1496-.L_text_b
	.previous
	cmpb $0,(%edi)
	je .L780
.L_LC1497:

.section	.line
	.4byte	2329	/ sas.c:2329
	.2byte	0xffff
	.4byte	.L_LC1497-.L_text_b
	.previous
	movb 32(%ecx),%bl
	andb $4,%bl
	je .L782
.L_B205:
.L_LC1498:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1498-.L_text_b
	.previous
.L_LC1499:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1499-.L_text_b
	.previous
	movb 74(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1500:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1500-.L_text_b
	.previous
.L_B205_e:
.L_LC1501:

.section	.line
	.4byte	2329	/ sas.c:2329
	.2byte	0xffff
	.4byte	.L_LC1501-.L_text_b
	.previous
.L782:
.L_B206:
.L_LC1502:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1502-.L_text_b
	.previous
.L_LC1503:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1503-.L_text_b
	.previous
	movb 1(%edi),%al
	movw 72(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1504:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1504-.L_text_b
	.previous
.L_B206_e:
.L_LC1505:

.section	.line
	.4byte	2332	/ sas.c:2332
	.2byte	0xffff
	.4byte	.L_LC1505-.L_text_b
	.previous
	testb %bl,%bl
	je .L787
.L_B207:
.L_LC1506:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1506-.L_text_b
	.previous
.L_LC1507:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1507-.L_text_b
	.previous
	movb 94(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1508:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1508-.L_text_b
	.previous
.L_B207_e:
.L_LC1509:

.section	.line
	.4byte	2332	/ sas.c:2332
	.2byte	0xffff
	.4byte	.L_LC1509-.L_text_b
	.previous
.L787:
.L_B208:
.L_LC1510:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1510-.L_text_b
	.previous
.L_LC1511:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1511-.L_text_b
	.previous
.L_B209:
.L_LC1512:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1512-.L_text_b
	.previous
	movw 92(%ecx),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1513:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1513-.L_text_b
	.previous
.L_B209_e:
.L_B208_e:
.L_LC1514:

.section	.line
	.4byte	2332	/ sas.c:2332
	.2byte	0xffff
	.4byte	.L_LC1514-.L_text_b
	.previous
	testb %al,%al
	je .L780
.L_LC1515:

.section	.line
	.4byte	2334	/ sas.c:2334
	.2byte	0xffff
	.4byte	.L_LC1515-.L_text_b
	.previous
.L877:
	movl $2,-4(%ebp)
.L_LC1516:

.section	.line
	.4byte	2335	/ sas.c:2335
	.2byte	0xffff
	.4byte	.L_LC1516-.L_text_b
	.previous
	jmp .L836
	.align 4
.L780:
.L_LC1517:

.section	.line
	.4byte	2340	/ sas.c:2340
	.2byte	0xffff
	.4byte	.L_LC1517-.L_text_b
	.previous
	movl $20000,-32(%ebp)
.L_LC1518:

.section	.line
	.4byte	2344	/ sas.c:2344
	.2byte	0xffff
	.4byte	.L_LC1518-.L_text_b
	.previous
	movb 32(%ecx),%bl
	andb $4,%bl
	.align 4
.L797:
	testb %bl,%bl
	je .L794
.L_B210:
.L_LC1519:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1519-.L_text_b
	.previous
.L_LC1520:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1520-.L_text_b
	.previous
	movb 94(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1521:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1521-.L_text_b
	.previous
.L_B210_e:
.L_LC1522:

.section	.line
	.4byte	2345	/ sas.c:2345
	.2byte	0xffff
	.4byte	.L_LC1522-.L_text_b
	.previous
.L794:
.L_B211:
.L_LC1523:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1523-.L_text_b
	.previous
	movw 92(%ecx),%si
	movw %si,-36(%ebp)
.L_LC1524:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1524-.L_text_b
	.previous
.L_B212:
.L_LC1525:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1525-.L_text_b
	.previous
	movl %esi,%edx
/APP
	inb %dx,%eax
/NO_APP
	movl %eax,-28(%ebp)
.L_LC1526:

.section	.line
	.4byte	113	/ sas.c:113
	.2byte	0xffff
	.4byte	.L_LC1526-.L_text_b
	.previous
	movb -28(%ebp),%al
	movb %al,-28(%ebp)
.L_LC1527:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1527-.L_text_b
	.previous
.L_B212_e:
.L_B211_e:
.L_LC1528:

.section	.line
	.4byte	2345	/ sas.c:2345
	.2byte	0xffff
	.4byte	.L_LC1528-.L_text_b
	.previous
	movzbl -28(%ebp),%edx
	movl %edx,-28(%ebp)
	testb $1,%dl
	jne .L791
	decl -32(%ebp)
.L_LC1529:

.section	.line
	.4byte	2344	/ sas.c:2344
	.2byte	0xffff
	.4byte	.L_LC1529-.L_text_b
	.previous
	jne .L797
.L791:
.L_LC1530:

.section	.line
	.4byte	2348	/ sas.c:2348
	.2byte	0xffff
	.4byte	.L_LC1530-.L_text_b
	.previous
	cmpl $0,-32(%ebp)
	jne .L798
.L_LC1531:

.section	.line
	.4byte	2350	/ sas.c:2350
	.2byte	0xffff
	.4byte	.L_LC1531-.L_text_b
	.previous
	movl $4,-4(%ebp)
.L_LC1532:

.section	.line
	.4byte	2351	/ sas.c:2351
	.2byte	0xffff
	.4byte	.L_LC1532-.L_text_b
	.previous
	jmp .L836
	.align 4
.L798:
.L_LC1533:

.section	.line
	.4byte	2355	/ sas.c:2355
	.2byte	0xffff
	.4byte	.L_LC1533-.L_text_b
	.previous
	movl -28(%ebp),%eax
	andl $31,%eax
	cmpl $1,%eax
	jne .L878
.L_LC1534:

.section	.line
	.4byte	2362	/ sas.c:2362
	.2byte	0xffff
	.4byte	.L_LC1534-.L_text_b
	.previous
	testb %bl,%bl
	je .L802
.L_B213:
.L_LC1535:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1535-.L_text_b
	.previous
.L_LC1536:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1536-.L_text_b
	.previous
	movb 74(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1537:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1537-.L_text_b
	.previous
.L_B213_e:
.L_LC1538:

.section	.line
	.4byte	2362	/ sas.c:2362
	.2byte	0xffff
	.4byte	.L_LC1538-.L_text_b
	.previous
.L802:
.L_B214:
.L_LC1539:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1539-.L_text_b
	.previous
.L_LC1540:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1540-.L_text_b
	.previous
.L_B215:
.L_LC1541:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1541-.L_text_b
	.previous
	movw 72(%ecx),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1542:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1542-.L_text_b
	.previous
.L_B215_e:
.L_B214_e:
.L_LC1543:

.section	.line
	.4byte	2362	/ sas.c:2362
	.2byte	0xffff
	.4byte	.L_LC1543-.L_text_b
	.previous
	cmpb %al,(%edi)
	je .L800
.L_LC1544:

.section	.line
	.4byte	2364	/ sas.c:2364
	.2byte	0xffff
	.4byte	.L_LC1544-.L_text_b
	.previous
	movl $6,-4(%ebp)
.L_LC1545:

.section	.line
	.4byte	2365	/ sas.c:2365
	.2byte	0xffff
	.4byte	.L_LC1545-.L_text_b
	.previous
	jmp .L836
	.align 4
.L800:
.L_LC1546:

.section	.line
	.4byte	2369	/ sas.c:2369
	.2byte	0xffff
	.4byte	.L_LC1546-.L_text_b
	.previous
	testb %bl,%bl
	je .L807
.L_B216:
.L_LC1547:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1547-.L_text_b
	.previous
.L_LC1548:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1548-.L_text_b
	.previous
	movb 94(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1549:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1549-.L_text_b
	.previous
.L_B216_e:
.L_LC1550:

.section	.line
	.4byte	2370	/ sas.c:2370
	.2byte	0xffff
	.4byte	.L_LC1550-.L_text_b
	.previous
.L807:
.L_B217:
.L_LC1551:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1551-.L_text_b
	.previous
.L_LC1552:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1552-.L_text_b
	.previous
.L_B218:
.L_LC1553:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1553-.L_text_b
	.previous
	movw -36(%ebp),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1554:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1554-.L_text_b
	.previous
.L_B218_e:
.L_B217_e:
.L_LC1555:

.section	.line
	.4byte	2370	/ sas.c:2370
	.2byte	0xffff
	.4byte	.L_LC1555-.L_text_b
	.previous
	testb $31,%al
	je .L805
.L_LC1556:

.section	.line
	.4byte	2372	/ sas.c:2372
	.2byte	0xffff
	.4byte	.L_LC1556-.L_text_b
	.previous
.L878:
	movl $5,-4(%ebp)
.L_LC1557:

.section	.line
	.4byte	2373	/ sas.c:2373
	.2byte	0xffff
	.4byte	.L_LC1557-.L_text_b
	.previous
	jmp .L836
	.align 4
.L805:
.L_LC1558:

.section	.line
	.4byte	2379	/ sas.c:2379
	.2byte	0xffff
	.4byte	.L_LC1558-.L_text_b
	.previous
	cmpb $0,(%edi)
	je .L810
.L_LC1559:

.section	.line
	.4byte	2381	/ sas.c:2381
	.2byte	0xffff
	.4byte	.L_LC1559-.L_text_b
	.previous
	movl $20000,-32(%ebp)
.L_LC1560:

.section	.line
	.4byte	2385	/ sas.c:2385
	.2byte	0xffff
	.4byte	.L_LC1560-.L_text_b
	.previous
	movb 32(%ecx),%bl
	andb $4,%bl
	.align 4
.L818:
	testb %bl,%bl
	je .L815
.L_B219:
.L_LC1561:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1561-.L_text_b
	.previous
.L_LC1562:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1562-.L_text_b
	.previous
	movb 94(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1563:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1563-.L_text_b
	.previous
.L_B219_e:
.L_LC1564:

.section	.line
	.4byte	2386	/ sas.c:2386
	.2byte	0xffff
	.4byte	.L_LC1564-.L_text_b
	.previous
.L815:
.L_B220:
.L_LC1565:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1565-.L_text_b
	.previous
.L_LC1566:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1566-.L_text_b
	.previous
.L_B221:
.L_LC1567:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1567-.L_text_b
	.previous
	movw 92(%ecx),%dx
/APP
	inb %dx,%eax
/NO_APP
	movl %eax,-40(%ebp)
.L_LC1568:

.section	.line
	.4byte	113	/ sas.c:113
	.2byte	0xffff
	.4byte	.L_LC1568-.L_text_b
	.previous
	movb -40(%ebp),%al
	movb %al,-28(%ebp)
.L_LC1569:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1569-.L_text_b
	.previous
.L_B221_e:
.L_B220_e:
.L_LC1570:

.section	.line
	.4byte	2386	/ sas.c:2386
	.2byte	0xffff
	.4byte	.L_LC1570-.L_text_b
	.previous
	movzbl -28(%ebp),%edx
	movl %edx,-28(%ebp)
	testb $32,%dl
	jne .L812
	decl -32(%ebp)
.L_LC1571:

.section	.line
	.4byte	2385	/ sas.c:2385
	.2byte	0xffff
	.4byte	.L_LC1571-.L_text_b
	.previous
	jne .L818
.L812:
.L_LC1572:

.section	.line
	.4byte	2389	/ sas.c:2389
	.2byte	0xffff
	.4byte	.L_LC1572-.L_text_b
	.previous
	cmpl $0,-32(%ebp)
	je .L879
.L_LC1573:

.section	.line
	.4byte	2396	/ sas.c:2396
	.2byte	0xffff
	.4byte	.L_LC1573-.L_text_b
	.previous
	cmpl $32,-28(%ebp)
	je .L779
.L_LC1574:

.section	.line
	.4byte	2398	/ sas.c:2398
	.2byte	0xffff
	.4byte	.L_LC1574-.L_text_b
	.previous
	movl $8,-4(%ebp)
.L_LC1575:

.section	.line
	.4byte	2399	/ sas.c:2399
	.2byte	0xffff
	.4byte	.L_LC1575-.L_text_b
	.previous
	jmp .L836
	.align 4
.L810:
.L_LC1576:

.section	.line
	.4byte	2404	/ sas.c:2404
	.2byte	0xffff
	.4byte	.L_LC1576-.L_text_b
	.previous
	movl $20000,-32(%ebp)
.L_LC1577:

.section	.line
	.4byte	2408	/ sas.c:2408
	.2byte	0xffff
	.4byte	.L_LC1577-.L_text_b
	.previous
	movb 32(%ecx),%bl
	andb $4,%bl
	.align 4
.L829:
	testb %bl,%bl
	je .L826
.L_B222:
.L_LC1578:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1578-.L_text_b
	.previous
.L_LC1579:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1579-.L_text_b
	.previous
	movb 94(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1580:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1580-.L_text_b
	.previous
.L_B222_e:
.L_LC1581:

.section	.line
	.4byte	2409	/ sas.c:2409
	.2byte	0xffff
	.4byte	.L_LC1581-.L_text_b
	.previous
.L826:
.L_B223:
.L_LC1582:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1582-.L_text_b
	.previous
.L_LC1583:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1583-.L_text_b
	.previous
.L_B224:
.L_LC1584:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1584-.L_text_b
	.previous
	movw 92(%ecx),%dx
/APP
	inb %dx,%eax
/NO_APP
	movl %eax,-40(%ebp)
.L_LC1585:

.section	.line
	.4byte	113	/ sas.c:113
	.2byte	0xffff
	.4byte	.L_LC1585-.L_text_b
	.previous
	movb -40(%ebp),%al
	movb %al,-28(%ebp)
.L_LC1586:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1586-.L_text_b
	.previous
.L_B224_e:
.L_B223_e:
.L_LC1587:

.section	.line
	.4byte	2409	/ sas.c:2409
	.2byte	0xffff
	.4byte	.L_LC1587-.L_text_b
	.previous
	movzbl -28(%ebp),%edx
	movl %edx,-28(%ebp)
	testb $64,%dl
	jne .L823
	decl -32(%ebp)
.L_LC1588:

.section	.line
	.4byte	2408	/ sas.c:2408
	.2byte	0xffff
	.4byte	.L_LC1588-.L_text_b
	.previous
	jne .L829
.L823:
.L_LC1589:

.section	.line
	.4byte	2412	/ sas.c:2412
	.2byte	0xffff
	.4byte	.L_LC1589-.L_text_b
	.previous
	cmpl $0,-32(%ebp)
	jne .L830
.L_LC1590:

.section	.line
	.4byte	2414	/ sas.c:2414
	.2byte	0xffff
	.4byte	.L_LC1590-.L_text_b
	.previous
.L879:
	movl $7,-4(%ebp)
.L_LC1591:

.section	.line
	.4byte	2415	/ sas.c:2415
	.2byte	0xffff
	.4byte	.L_LC1591-.L_text_b
	.previous
	jmp .L836
	.align 4
.L830:
.L_LC1592:

.section	.line
	.4byte	2419	/ sas.c:2419
	.2byte	0xffff
	.4byte	.L_LC1592-.L_text_b
	.previous
	cmpl $96,-28(%ebp)
	je .L779
.L_LC1593:

.section	.line
	.4byte	2422	/ sas.c:2422
	.2byte	0xffff
	.4byte	.L_LC1593-.L_text_b
	.previous
	movl $8,-4(%ebp)
.L_LC1594:

.section	.line
	.4byte	2423	/ sas.c:2423
	.2byte	0xffff
	.4byte	.L_LC1594-.L_text_b
	.previous
	jmp .L836
	.align 4
.L_LC1595:

.section	.line
	.4byte	2426	/ sas.c:2426
	.2byte	0xffff
	.4byte	.L_LC1595-.L_text_b
	.previous
.L779:
	movw (%edi),%ax
	incl %edi
	testw %ax,%ax
	jne .L777
.L_LC1596:

.section	.line
	.4byte	2428	/ sas.c:2428
	.2byte	0xffff
	.4byte	.L_LC1596-.L_text_b
	.previous
	cmpl $0,-4(%ebp)
	jne .L836
.L_LC1597:

.section	.line
	.4byte	2279	/ sas.c:2279
	.2byte	0xffff
	.4byte	.L_LC1597-.L_text_b
	.previous
	addl $4,-24(%ebp)
	cmpl $lcrval.32+8,-24(%ebp)
	jbe .L833
.L_LC1598:

.section	.line
	.4byte	2277	/ sas.c:2277
	.2byte	0xffff
	.4byte	.L_LC1598-.L_text_b
	.previous
	decl -8(%ebp)
	jne .L835
.L_LC1599:

.section	.line
	.4byte	2440	/ sas.c:2440
	.2byte	0xffff
	.4byte	.L_LC1599-.L_text_b
	.previous
	testb $4,32(%ecx)
	je .L838
.L_B225:
.L_LC1600:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1600-.L_text_b
	.previous
.L_LC1601:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1601-.L_text_b
	.previous
	movb 98(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1602:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1602-.L_text_b
	.previous
.L_B225_e:
.L_LC1603:

.section	.line
	.4byte	2440	/ sas.c:2440
	.2byte	0xffff
	.4byte	.L_LC1603-.L_text_b
	.previous
.L838:
.L_B226:
.L_LC1604:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1604-.L_text_b
	.previous
.L_LC1605:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1605-.L_text_b
	.previous
.L_B227:
.L_LC1606:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1606-.L_text_b
	.previous
	movw 96(%ecx),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1607:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1607-.L_text_b
	.previous
.L_B227_e:
.L_B226_e:
.L_LC1608:

.section	.line
	.4byte	2442	/ sas.c:2442
	.2byte	0xffff
	.4byte	.L_LC1608-.L_text_b
	.previous
	movl $1000,-8(%ebp)
	.align 4
.L863:
.L_LC1609:

.section	.line
	.4byte	2445	/ sas.c:2445
	.2byte	0xffff
	.4byte	.L_LC1609-.L_text_b
	.previous
	movl $.LC11,%edi
.L_LC1610:

.section	.line
	.4byte	2447	/ sas.c:2447
	.2byte	0xffff
	.4byte	.L_LC1610-.L_text_b
	.previous
	cmpw $0,.LC11
	je .L845
	movb 32(%ecx),%al
	andb $4,%al
	movb %al,-20(%ebp)
	.align 4
.L861:
.L_LC1611:

.section	.line
	.4byte	2450	/ sas.c:2450
	.2byte	0xffff
	.4byte	.L_LC1611-.L_text_b
	.previous
	cmpb $0,-20(%ebp)
	je .L848
.L_B228:
.L_LC1612:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1612-.L_text_b
	.previous
.L_LC1613:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1613-.L_text_b
	.previous
	movb 90(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1614:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1614-.L_text_b
	.previous
.L_B228_e:
.L_LC1615:

.section	.line
	.4byte	2450	/ sas.c:2450
	.2byte	0xffff
	.4byte	.L_LC1615-.L_text_b
	.previous
.L848:
.L_B229:
.L_LC1616:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1616-.L_text_b
	.previous
	movb (%edi),%al
	orb $16,%al
.L_LC1617:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1617-.L_text_b
	.previous
	movw 88(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1618:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1618-.L_text_b
	.previous
.L_B229_e:
.L_LC1619:

.section	.line
	.4byte	2451	/ sas.c:2451
	.2byte	0xffff
	.4byte	.L_LC1619-.L_text_b
	.previous
	cmpb $0,-20(%ebp)
	je .L853
.L_B230:
.L_LC1620:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1620-.L_text_b
	.previous
.L_LC1621:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1621-.L_text_b
	.previous
	movb 98(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1622:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1622-.L_text_b
	.previous
.L_B230_e:
.L_LC1623:

.section	.line
	.4byte	2451	/ sas.c:2451
	.2byte	0xffff
	.4byte	.L_LC1623-.L_text_b
	.previous
.L853:
.L_B231:
.L_LC1624:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1624-.L_text_b
	.previous
	movw 96(%ecx),%si
	movw %si,-36(%ebp)
.L_LC1625:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1625-.L_text_b
	.previous
.L_B232:
.L_LC1626:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1626-.L_text_b
	.previous
	movl %esi,%edx
/APP
	inb %dx,%eax
/NO_APP
	movl %eax,%ebx
.L_LC1627:

.section	.line
	.4byte	113	/ sas.c:113
	.2byte	0xffff
	.4byte	.L_LC1627-.L_text_b
	.previous
	movb %bl,-28(%ebp)
.L_LC1628:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1628-.L_text_b
	.previous
.L_B232_e:
.L_B231_e:
.L_LC1629:

.section	.line
	.4byte	2451	/ sas.c:2451
	.2byte	0xffff
	.4byte	.L_LC1629-.L_text_b
	.previous
	movzbl -28(%ebp),%ebx
	movb -28(%ebp),%al
	cmpb %al,1(%edi)
	jne .L880
.L_LC1630:

.section	.line
	.4byte	2456	/ sas.c:2456
	.2byte	0xffff
	.4byte	.L_LC1630-.L_text_b
	.previous
	cmpb $0,-20(%ebp)
	je .L858
.L_B233:
.L_LC1631:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1631-.L_text_b
	.previous
.L_LC1632:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1632-.L_text_b
	.previous
	movb 98(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1633:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1633-.L_text_b
	.previous
.L_B233_e:
.L_LC1634:

.section	.line
	.4byte	2457	/ sas.c:2457
	.2byte	0xffff
	.4byte	.L_LC1634-.L_text_b
	.previous
.L858:
.L_B234:
.L_LC1635:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1635-.L_text_b
	.previous
.L_LC1636:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1636-.L_text_b
	.previous
.L_B235:
.L_LC1637:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1637-.L_text_b
	.previous
	movw -36(%ebp),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1638:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1638-.L_text_b
	.previous
.L_B235_e:
.L_B234_e:
.L_LC1639:

.section	.line
	.4byte	2457	/ sas.c:2457
	.2byte	0xffff
	.4byte	.L_LC1639-.L_text_b
	.previous
	andl $255,%eax
	movl %eax,-32(%ebp)
	movl %ebx,%eax
	andb $240,%al
	cmpl %eax,-32(%ebp)
	je .L846
.L_LC1640:

.section	.line
	.4byte	2459	/ sas.c:2459
	.2byte	0xffff
	.4byte	.L_LC1640-.L_text_b
	.previous
.L880:
	movl $9,-4(%ebp)
.L_LC1641:

.section	.line
	.4byte	2460	/ sas.c:2460
	.2byte	0xffff
	.4byte	.L_LC1641-.L_text_b
	.previous
	jmp .L836
	.align 4
.L_LC1642:

.section	.line
	.4byte	2447	/ sas.c:2447
	.2byte	0xffff
	.4byte	.L_LC1642-.L_text_b
	.previous
.L846:
	addl $2,%edi
	cmpw $0,(%edi)
	jne .L861
.L845:
.L_LC1643:

.section	.line
	.4byte	2464	/ sas.c:2464
	.2byte	0xffff
	.4byte	.L_LC1643-.L_text_b
	.previous
	cmpl $0,-4(%ebp)
	jne .L836
.L_LC1644:

.section	.line
	.4byte	2442	/ sas.c:2442
	.2byte	0xffff
	.4byte	.L_LC1644-.L_text_b
	.previous
	decl -8(%ebp)
	jne .L863
.L836:
.L_LC1645:

.section	.line
	.4byte	2470	/ sas.c:2470
	.2byte	0xffff
	.4byte	.L_LC1645-.L_text_b
	.previous
	testb $4,32(%ecx)
	je .L865
.L_B236:
.L_LC1646:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1646-.L_text_b
	.previous
.L_LC1647:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1647-.L_text_b
	.previous
	movb 90(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1648:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1648-.L_text_b
	.previous
.L_B236_e:
.L_LC1649:

.section	.line
	.4byte	2470	/ sas.c:2470
	.2byte	0xffff
	.4byte	.L_LC1649-.L_text_b
	.previous
.L865:
.L_B237:
.L_LC1650:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1650-.L_text_b
	.previous
.L_LC1651:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1651-.L_text_b
	.previous
	xorb %al,%al
	movw 88(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1652:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1652-.L_text_b
	.previous
.L_B237_e:
.L_LC1653:

.section	.line
	.4byte	2472	/ sas.c:2472
	.2byte	0xffff
	.4byte	.L_LC1653-.L_text_b
	.previous
	movl -4(%ebp),%eax
.L_LC1654:

.section	.line
	.4byte	2473	/ sas.c:2473
	.2byte	0xffff
	.4byte	.L_LC1654-.L_text_b
	.previous
.L_b17_e:
	leal -52(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
.L_f17_e:
.Lfe14:
	.size	 sas_test_device,.Lfe14-sas_test_device

.section	.debug
.L_D881:
	.4byte	.L_D881_e-.L_D881
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D889
	.2byte	0x38
	.string	"sas_test_device"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x111
	.4byte	sas_test_device
	.2byte	0x121
	.4byte	.L_f17_e
	.2byte	0x8041
	.4byte	.L_b17
	.2byte	0x8051
	.4byte	.L_b17_e
.L_D881_e:
.L_D890:
	.4byte	.L_D890_e-.L_D890
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D891
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t890_e-.L_t890
.L_t890:
	.byte	0x1
	.4byte	.L_T816
.L_t890_e:
	.2byte	0x23
	.2byte	.L_l890_e-.L_l890
.L_l890:
	.byte	0x1
	.4byte	0x1
.L_l890_e:
.L_D890_e:
.L_D891:
	.4byte	.L_D891_e-.L_D891
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D892
	.2byte	0x38
	.string	"regvar"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l891_e-.L_l891
.L_l891:
.L_l891_e:
.L_D891_e:
.L_D892:
	.4byte	.L_D892_e-.L_D892
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D893
	.2byte	0x38
	.string	"cptr"
	.2byte	0x63
	.2byte	.L_t892_e-.L_t892
.L_t892:
	.byte	0x1
	.2byte	0x3
.L_t892_e:
	.2byte	0x23
	.2byte	.L_l892_e-.L_l892
.L_l892:
	.byte	0x1
	.4byte	0x7
.L_l892_e:
.L_D892_e:
.L_D893:
	.4byte	.L_D893_e-.L_D893
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D894
	.2byte	0x38
	.string	"done"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l893_e-.L_l893
.L_l893:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xfffffffc
	.byte	0x7
.L_l893_e:
.L_D893_e:
.L_D894:
	.4byte	.L_D894_e-.L_D894
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D895
	.2byte	0x38
	.string	"delay_count"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l894_e-.L_l894
.L_l894:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffffe0
	.byte	0x7
.L_l894_e:
.L_D894_e:
.L_D895:
	.4byte	.L_D895_e-.L_D895
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D896
	.2byte	0x38
	.string	"rep_count"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l895_e-.L_l895
.L_l895:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xfffffff8
	.byte	0x7
.L_l895_e:
.L_D895_e:
.L_D896:
	.4byte	.L_D896_e-.L_D896
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D897
	.2byte	0x38
	.string	"i"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l896_e-.L_l896
.L_l896:
.L_l896_e:
.L_D896_e:
.L_D897:
	.4byte	.L_D897_e-.L_D897
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D898
	.2byte	0x38
	.string	"lsr"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l897_e-.L_l897
.L_l897:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffffe4
	.byte	0x7
.L_l897_e:
.L_D897_e:
.L_D898:
	.4byte	.L_D898_e-.L_D898
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D899
	.set	.L_T900,.L_D898
	.2byte	0xa3
	.2byte	.L_s898_e-.L_s898
.L_s898:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x2
	.byte	0x8
	.2byte	0x55
	.2byte	0x9
.L_s898_e:
.L_D898_e:
.L_D899:
	.4byte	.L_D899_e-.L_D899
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D900
	.2byte	0x38
	.string	"lcrval"
	.2byte	0x72
	.4byte	.L_T900
	.2byte	0x23
	.2byte	.L_l899_e-.L_l899
.L_l899:
	.byte	0x3
	.4byte	lcrval.32
.L_l899_e:
.L_D899_e:
.L_D900:
	.4byte	.L_D900_e-.L_D900
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D901
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B159
	.2byte	0x121
	.4byte	.L_B159_e
.L_D900_e:
.L_D902:
	.4byte	.L_D902_e-.L_D902
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D903
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l902_e-.L_l902
.L_l902:
.L_l902_e:
.L_D902_e:
.L_D903:
	.4byte	.L_D903_e-.L_D903
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D904
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l903_e-.L_l903
.L_l903:
.L_l903_e:
.L_D903_e:
.L_D904:
	.4byte	0x4
.L_D901:
	.4byte	.L_D901_e-.L_D901
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D905
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B160
	.2byte	0x121
	.4byte	.L_B160_e
.L_D901_e:
.L_D906:
	.4byte	.L_D906_e-.L_D906
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D907
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l906_e-.L_l906
.L_l906:
	.byte	0x1
	.4byte	0x7
.L_l906_e:
.L_D906_e:
.L_D907:
	.4byte	.L_D907_e-.L_D907
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D908
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l907_e-.L_l907
.L_l907:
	.4byte	0x0
.L_l907_e:
.L_D907_e:
.L_D908:
	.4byte	0x4
.L_D905:
	.4byte	.L_D905_e-.L_D905
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D909
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B161
	.2byte	0x121
	.4byte	.L_B161_e
.L_D905_e:
.L_D910:
	.4byte	.L_D910_e-.L_D910
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D911
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l910_e-.L_l910
.L_l910:
.L_l910_e:
.L_D910_e:
.L_D911:
	.4byte	.L_D911_e-.L_D911
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D912
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l911_e-.L_l911
.L_l911:
.L_l911_e:
.L_D911_e:
.L_D912:
	.4byte	0x4
.L_D909:
	.4byte	.L_D909_e-.L_D909
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D913
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B162
	.2byte	0x121
	.4byte	.L_B162_e
.L_D909_e:
.L_D914:
	.4byte	.L_D914_e-.L_D914
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D915
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l914_e-.L_l914
.L_l914:
.L_l914_e:
.L_D914_e:
.L_D915:
	.4byte	.L_D915_e-.L_D915
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D916
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l915_e-.L_l915
.L_l915:
	.4byte	0x40
.L_l915_e:
.L_D915_e:
.L_D916:
	.4byte	0x4
.L_D913:
	.4byte	.L_D913_e-.L_D913
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D917
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B163
	.2byte	0x121
	.4byte	.L_B163_e
.L_D913_e:
.L_D918:
	.4byte	.L_D918_e-.L_D918
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D919
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l918_e-.L_l918
.L_l918:
.L_l918_e:
.L_D918_e:
.L_D919:
	.4byte	.L_D919_e-.L_D919
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D920
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l919_e-.L_l919
.L_l919:
.L_l919_e:
.L_D919_e:
.L_D920:
	.4byte	0x4
.L_D917:
	.4byte	.L_D917_e-.L_D917
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D921
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B164
	.2byte	0x121
	.4byte	.L_B164_e
.L_D917_e:
.L_D922:
	.4byte	.L_D922_e-.L_D922
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D923
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l922_e-.L_l922
.L_l922:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffffdc
	.byte	0x7
.L_l922_e:
.L_D922_e:
.L_D923:
	.4byte	.L_D923_e-.L_D923
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D924
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l923_e-.L_l923
.L_l923:
	.4byte	0x0
.L_l923_e:
.L_D923_e:
.L_D924:
	.4byte	0x4
.L_D921:
	.4byte	.L_D921_e-.L_D921
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D925
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B165
	.2byte	0x121
	.4byte	.L_B165_e
.L_D921_e:
.L_D926:
	.4byte	.L_D926_e-.L_D926
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D927
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l926_e-.L_l926
.L_l926:
.L_l926_e:
.L_D926_e:
.L_D927:
	.4byte	.L_D927_e-.L_D927
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D928
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l927_e-.L_l927
.L_l927:
.L_l927_e:
.L_D927_e:
.L_D928:
	.4byte	0x4
.L_D925:
	.4byte	.L_D925_e-.L_D925
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D929
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B166
	.2byte	0x121
	.4byte	.L_B166_e
.L_D925_e:
.L_D930:
	.4byte	.L_D930_e-.L_D930
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D931
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l930_e-.L_l930
.L_l930:
.L_l930_e:
.L_D930_e:
.L_D931:
	.4byte	.L_D931_e-.L_D931
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D932
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l931_e-.L_l931
.L_l931:
	.4byte	0x0
.L_l931_e:
.L_D931_e:
.L_D932:
	.4byte	0x4
.L_D929:
	.4byte	.L_D929_e-.L_D929
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D933
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B167
	.2byte	0x121
	.4byte	.L_B167_e
.L_D929_e:
.L_D934:
	.4byte	.L_D934_e-.L_D934
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D935
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l934_e-.L_l934
.L_l934:
.L_l934_e:
.L_D934_e:
.L_D935:
	.4byte	.L_D935_e-.L_D935
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D936
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l935_e-.L_l935
.L_l935:
.L_l935_e:
.L_D935_e:
.L_D936:
	.4byte	0x4
.L_D933:
	.4byte	.L_D933_e-.L_D933
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D937
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B168
	.2byte	0x121
	.4byte	.L_B168_e
.L_D933_e:
.L_D938:
	.4byte	.L_D938_e-.L_D938
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D939
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l938_e-.L_l938
.L_l938:
	.byte	0x1
	.4byte	0x7
.L_l938_e:
.L_D938_e:
.L_D939:
	.4byte	.L_D939_e-.L_D939
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D940
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l939_e-.L_l939
.L_l939:
	.4byte	0xffffff80
.L_l939_e:
.L_D939_e:
.L_D940:
	.4byte	0x4
.L_D937:
	.4byte	.L_D937_e-.L_D937
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D941
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B169
	.2byte	0x121
	.4byte	.L_B169_e
.L_D937_e:
.L_D942:
	.4byte	.L_D942_e-.L_D942
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D943
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l942_e-.L_l942
.L_l942:
.L_l942_e:
.L_D942_e:
.L_D943:
	.4byte	.L_D943_e-.L_D943
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D944
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l943_e-.L_l943
.L_l943:
.L_l943_e:
.L_D943_e:
.L_D944:
	.4byte	0x4
.L_D941:
	.4byte	.L_D941_e-.L_D941
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D945
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B170
	.2byte	0x121
	.4byte	.L_B170_e
.L_D941_e:
.L_D946:
	.4byte	.L_D946_e-.L_D946
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D947
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l946_e-.L_l946
.L_l946:
.L_l946_e:
.L_D946_e:
.L_D947:
	.4byte	.L_D947_e-.L_D947
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D948
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l947_e-.L_l947
.L_l947:
.L_l947_e:
.L_D947_e:
.L_D948:
	.4byte	0x4
.L_D945:
	.4byte	.L_D945_e-.L_D945
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D949
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B171
	.2byte	0x121
	.4byte	.L_B171_e
.L_D945_e:
.L_D950:
	.4byte	.L_D950_e-.L_D950
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D951
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l950_e-.L_l950
.L_l950:
.L_l950_e:
.L_D950_e:
.L_D951:
	.4byte	.L_D951_e-.L_D951
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D952
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l951_e-.L_l951
.L_l951:
.L_l951_e:
.L_D951_e:
.L_D952:
	.4byte	0x4
.L_D949:
	.4byte	.L_D949_e-.L_D949
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D953
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B172
	.2byte	0x121
	.4byte	.L_B172_e
.L_D949_e:
.L_D954:
	.4byte	.L_D954_e-.L_D954
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D955
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l954_e-.L_l954
.L_l954:
.L_l954_e:
.L_D954_e:
.L_D955:
	.4byte	.L_D955_e-.L_D955
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D956
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l955_e-.L_l955
.L_l955:
.L_l955_e:
.L_D955_e:
.L_D956:
	.4byte	0x4
.L_D953:
	.4byte	.L_D953_e-.L_D953
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D957
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B173
	.2byte	0x121
	.4byte	.L_B173_e
.L_D953_e:
.L_D958:
	.4byte	.L_D958_e-.L_D958
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D959
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l958_e-.L_l958
.L_l958:
.L_l958_e:
.L_D958_e:
.L_D959:
	.4byte	.L_D959_e-.L_D959
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D960
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l959_e-.L_l959
.L_l959:
.L_l959_e:
.L_D959_e:
.L_D960:
	.4byte	0x4
.L_D957:
	.4byte	.L_D957_e-.L_D957
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D961
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B174
	.2byte	0x121
	.4byte	.L_B174_e
.L_D957_e:
.L_D962:
	.4byte	.L_D962_e-.L_D962
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D963
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l962_e-.L_l962
.L_l962:
.L_l962_e:
.L_D962_e:
.L_D963:
	.4byte	.L_D963_e-.L_D963
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D964
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l963_e-.L_l963
.L_l963:
	.4byte	0x0
.L_l963_e:
.L_D963_e:
.L_D964:
	.4byte	0x4
.L_D961:
	.4byte	.L_D961_e-.L_D961
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D965
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B175
	.2byte	0x121
	.4byte	.L_B175_e
.L_D961_e:
.L_D966:
	.4byte	.L_D966_e-.L_D966
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D967
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l966_e-.L_l966
.L_l966:
.L_l966_e:
.L_D966_e:
.L_D967:
	.4byte	.L_D967_e-.L_D967
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D968
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l967_e-.L_l967
.L_l967:
.L_l967_e:
.L_D967_e:
.L_D968:
	.4byte	0x4
.L_D965:
	.4byte	.L_D965_e-.L_D965
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D969
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B176
	.2byte	0x121
	.4byte	.L_B176_e
.L_D965_e:
.L_D970:
	.4byte	.L_D970_e-.L_D970
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D971
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l970_e-.L_l970
.L_l970:
.L_l970_e:
.L_D970_e:
.L_D971:
	.4byte	.L_D971_e-.L_D971
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D972
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l971_e-.L_l971
.L_l971:
	.4byte	0x10
.L_l971_e:
.L_D971_e:
.L_D972:
	.4byte	0x4
.L_D969:
	.4byte	.L_D969_e-.L_D969
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D973
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B177
	.2byte	0x121
	.4byte	.L_B177_e
.L_D969_e:
.L_D974:
	.4byte	.L_D974_e-.L_D974
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D975
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l974_e-.L_l974
.L_l974:
.L_l974_e:
.L_D974_e:
.L_D975:
	.4byte	.L_D975_e-.L_D975
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D976
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l975_e-.L_l975
.L_l975:
.L_l975_e:
.L_D975_e:
.L_D976:
	.4byte	0x4
.L_D973:
	.4byte	.L_D973_e-.L_D973
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D977
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B178
	.2byte	0x121
	.4byte	.L_B178_e
.L_D973_e:
.L_D978:
	.4byte	.L_D978_e-.L_D978
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D979
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l978_e-.L_l978
.L_l978:
	.byte	0x1
	.4byte	0x3
.L_l978_e:
.L_D978_e:
.L_D979:
	.4byte	.L_D979_e-.L_D979
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D980
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l979_e-.L_l979
.L_l979:
	.byte	0x1
	.4byte	0x0
.L_l979_e:
.L_D979_e:
.L_D980:
	.4byte	0x4
.L_D977:
	.4byte	.L_D977_e-.L_D977
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D981
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B180
	.2byte	0x121
	.4byte	.L_B180_e
.L_D977_e:
.L_D982:
	.4byte	.L_D982_e-.L_D982
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D983
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l982_e-.L_l982
.L_l982:
.L_l982_e:
.L_D982_e:
.L_D983:
	.4byte	.L_D983_e-.L_D983
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D984
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l983_e-.L_l983
.L_l983:
.L_l983_e:
.L_D983_e:
.L_D984:
	.4byte	0x4
.L_D981:
	.4byte	.L_D981_e-.L_D981
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D985
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B181
	.2byte	0x121
	.4byte	.L_B181_e
.L_D981_e:
.L_D986:
	.4byte	.L_D986_e-.L_D986
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D987
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l986_e-.L_l986
.L_l986:
	.byte	0x1
	.4byte	0x7
.L_l986_e:
.L_D986_e:
.L_D987:
	.4byte	.L_D987_e-.L_D987
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D988
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l987_e-.L_l987
.L_l987:
	.byte	0x1
	.4byte	0x6
.L_l987_e:
.L_D987_e:
.L_D988:
	.4byte	0x4
.L_D985:
	.4byte	.L_D985_e-.L_D985
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D989
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B183
	.2byte	0x121
	.4byte	.L_B183_e
.L_D985_e:
.L_D990:
	.4byte	.L_D990_e-.L_D990
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D991
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l990_e-.L_l990
.L_l990:
.L_l990_e:
.L_D990_e:
.L_D991:
	.4byte	.L_D991_e-.L_D991
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D992
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l991_e-.L_l991
.L_l991:
.L_l991_e:
.L_D991_e:
.L_D992:
	.4byte	0x4
.L_D989:
	.4byte	.L_D989_e-.L_D989
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D993
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B184
	.2byte	0x121
	.4byte	.L_B184_e
.L_D989_e:
.L_D994:
	.4byte	.L_D994_e-.L_D994
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D995
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l994_e-.L_l994
.L_l994:
.L_l994_e:
.L_D994_e:
.L_D995:
	.4byte	.L_D995_e-.L_D995
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D996
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l995_e-.L_l995
.L_l995:
	.byte	0x1
	.4byte	0x6
.L_l995_e:
.L_D995_e:
.L_D996:
	.4byte	0x4
.L_D993:
	.4byte	.L_D993_e-.L_D993
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D997
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B186
	.2byte	0x121
	.4byte	.L_B186_e
.L_D993_e:
.L_D998:
	.4byte	.L_D998_e-.L_D998
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D999
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l998_e-.L_l998
.L_l998:
.L_l998_e:
.L_D998_e:
.L_D999:
	.4byte	.L_D999_e-.L_D999
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1000
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l999_e-.L_l999
.L_l999:
.L_l999_e:
.L_D999_e:
.L_D1000:
	.4byte	0x4
.L_D997:
	.4byte	.L_D997_e-.L_D997
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1001
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B187
	.2byte	0x121
	.4byte	.L_B187_e
.L_D997_e:
.L_D1002:
	.4byte	.L_D1002_e-.L_D1002
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1003
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1002_e-.L_l1002
.L_l1002:
.L_l1002_e:
.L_D1002_e:
.L_D1003:
	.4byte	.L_D1003_e-.L_D1003
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1004
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1003_e-.L_l1003
.L_l1003:
	.byte	0x1
	.4byte	0x3
.L_l1003_e:
.L_D1003_e:
.L_D1004:
	.4byte	0x4
.L_D1001:
	.4byte	.L_D1001_e-.L_D1001
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1005
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B189
	.2byte	0x121
	.4byte	.L_B189_e
.L_D1001_e:
.L_D1006:
	.4byte	.L_D1006_e-.L_D1006
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1007
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1006_e-.L_l1006
.L_l1006:
.L_l1006_e:
.L_D1006_e:
.L_D1007:
	.4byte	.L_D1007_e-.L_D1007
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1008
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1007_e-.L_l1007
.L_l1007:
.L_l1007_e:
.L_D1007_e:
.L_D1008:
	.4byte	0x4
.L_D1005:
	.4byte	.L_D1005_e-.L_D1005
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1009
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B190
	.2byte	0x121
	.4byte	.L_B190_e
.L_D1005_e:
.L_D1010:
	.4byte	.L_D1010_e-.L_D1010
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1011
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1010_e-.L_l1010
.L_l1010:
.L_l1010_e:
.L_D1010_e:
.L_D1011:
	.4byte	.L_D1011_e-.L_D1011
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1012
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1011_e-.L_l1011
.L_l1011:
	.byte	0x1
	.4byte	0x0
.L_l1011_e:
.L_D1011_e:
.L_D1012:
	.4byte	0x4
.L_D1009:
	.4byte	.L_D1009_e-.L_D1009
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1013
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B192
	.2byte	0x121
	.4byte	.L_B192_e
.L_D1009_e:
.L_D1014:
	.4byte	.L_D1014_e-.L_D1014
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1015
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1014_e-.L_l1014
.L_l1014:
.L_l1014_e:
.L_D1014_e:
.L_D1015:
	.4byte	.L_D1015_e-.L_D1015
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1016
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1015_e-.L_l1015
.L_l1015:
.L_l1015_e:
.L_D1015_e:
.L_D1016:
	.4byte	0x4
.L_D1013:
	.4byte	.L_D1013_e-.L_D1013
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1017
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B193
	.2byte	0x121
	.4byte	.L_B193_e
.L_D1013_e:
.L_D1018:
	.4byte	.L_D1018_e-.L_D1018
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1019
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1018_e-.L_l1018
.L_l1018:
.L_l1018_e:
.L_D1018_e:
.L_D1019:
	.4byte	.L_D1019_e-.L_D1019
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1020
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1019_e-.L_l1019
.L_l1019:
	.byte	0x1
	.4byte	0x0
.L_l1019_e:
.L_D1019_e:
.L_D1020:
	.4byte	0x4
.L_D1017:
	.4byte	.L_D1017_e-.L_D1017
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1021
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B195
	.2byte	0x121
	.4byte	.L_B195_e
.L_D1017_e:
.L_D1022:
	.4byte	.L_D1022_e-.L_D1022
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1023
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1022_e-.L_l1022
.L_l1022:
.L_l1022_e:
.L_D1022_e:
.L_D1023:
	.4byte	.L_D1023_e-.L_D1023
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1024
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1023_e-.L_l1023
.L_l1023:
.L_l1023_e:
.L_D1023_e:
.L_D1024:
	.4byte	0x4
.L_D1021:
	.4byte	.L_D1021_e-.L_D1021
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1025
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B196
	.2byte	0x121
	.4byte	.L_B196_e
.L_D1021_e:
.L_D1026:
	.4byte	.L_D1026_e-.L_D1026
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1027
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1026_e-.L_l1026
.L_l1026:
.L_l1026_e:
.L_D1026_e:
.L_D1027:
	.4byte	.L_D1027_e-.L_D1027
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1028
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1027_e-.L_l1027
.L_l1027:
	.byte	0x1
	.4byte	0x0
.L_l1027_e:
.L_D1027_e:
.L_D1028:
	.4byte	0x4
.L_D1025:
	.4byte	.L_D1025_e-.L_D1025
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1029
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B198
	.2byte	0x121
	.4byte	.L_B198_e
.L_D1025_e:
.L_D1030:
	.4byte	.L_D1030_e-.L_D1030
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1031
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1030_e-.L_l1030
.L_l1030:
.L_l1030_e:
.L_D1030_e:
.L_D1031:
	.4byte	.L_D1031_e-.L_D1031
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1032
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1031_e-.L_l1031
.L_l1031:
.L_l1031_e:
.L_D1031_e:
.L_D1032:
	.4byte	0x4
.L_D1029:
	.4byte	.L_D1029_e-.L_D1029
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1033
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B199
	.2byte	0x121
	.4byte	.L_B199_e
.L_D1029_e:
.L_D1034:
	.4byte	.L_D1034_e-.L_D1034
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1035
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1034_e-.L_l1034
.L_l1034:
.L_l1034_e:
.L_D1034_e:
.L_D1035:
	.4byte	.L_D1035_e-.L_D1035
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1036
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1035_e-.L_l1035
.L_l1035:
.L_l1035_e:
.L_D1035_e:
.L_D1036:
	.4byte	0x4
.L_D1033:
	.4byte	.L_D1033_e-.L_D1033
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1037
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B200
	.2byte	0x121
	.4byte	.L_B200_e
.L_D1033_e:
.L_D1038:
	.4byte	.L_D1038_e-.L_D1038
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1039
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1038_e-.L_l1038
.L_l1038:
.L_l1038_e:
.L_D1038_e:
.L_D1039:
	.4byte	.L_D1039_e-.L_D1039
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1040
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1039_e-.L_l1039
.L_l1039:
.L_l1039_e:
.L_D1039_e:
.L_D1040:
	.4byte	0x4
.L_D1037:
	.4byte	.L_D1037_e-.L_D1037
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1041
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B201
	.2byte	0x121
	.4byte	.L_B201_e
.L_D1037_e:
.L_D1042:
	.4byte	.L_D1042_e-.L_D1042
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1043
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1042_e-.L_l1042
.L_l1042:
.L_l1042_e:
.L_D1042_e:
.L_D1043:
	.4byte	.L_D1043_e-.L_D1043
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1044
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1043_e-.L_l1043
.L_l1043:
.L_l1043_e:
.L_D1043_e:
.L_D1044:
	.4byte	0x4
.L_D1041:
	.4byte	.L_D1041_e-.L_D1041
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1045
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B202
	.2byte	0x121
	.4byte	.L_B202_e
.L_D1041_e:
.L_D1046:
	.4byte	.L_D1046_e-.L_D1046
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1047
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1046_e-.L_l1046
.L_l1046:
.L_l1046_e:
.L_D1046_e:
.L_D1047:
	.4byte	.L_D1047_e-.L_D1047
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1048
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1047_e-.L_l1047
.L_l1047:
.L_l1047_e:
.L_D1047_e:
.L_D1048:
	.4byte	0x4
.L_D1045:
	.4byte	.L_D1045_e-.L_D1045
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1049
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B203
	.2byte	0x121
	.4byte	.L_B203_e
.L_D1045_e:
.L_D1050:
	.4byte	.L_D1050_e-.L_D1050
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1051
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1050_e-.L_l1050
.L_l1050:
.L_l1050_e:
.L_D1050_e:
.L_D1051:
	.4byte	.L_D1051_e-.L_D1051
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1052
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1051_e-.L_l1051
.L_l1051:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffffd8
	.byte	0x7
.L_l1051_e:
.L_D1051_e:
.L_D1052:
	.4byte	0x4
.L_D1049:
	.4byte	.L_D1049_e-.L_D1049
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1053
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B205
	.2byte	0x121
	.4byte	.L_B205_e
.L_D1049_e:
.L_D1054:
	.4byte	.L_D1054_e-.L_D1054
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1055
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1054_e-.L_l1054
.L_l1054:
.L_l1054_e:
.L_D1054_e:
.L_D1055:
	.4byte	.L_D1055_e-.L_D1055
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1056
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1055_e-.L_l1055
.L_l1055:
.L_l1055_e:
.L_D1055_e:
.L_D1056:
	.4byte	0x4
.L_D1053:
	.4byte	.L_D1053_e-.L_D1053
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1057
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B206
	.2byte	0x121
	.4byte	.L_B206_e
.L_D1053_e:
.L_D1058:
	.4byte	.L_D1058_e-.L_D1058
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1059
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1058_e-.L_l1058
.L_l1058:
.L_l1058_e:
.L_D1058_e:
.L_D1059:
	.4byte	.L_D1059_e-.L_D1059
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1060
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1059_e-.L_l1059
.L_l1059:
.L_l1059_e:
.L_D1059_e:
.L_D1060:
	.4byte	0x4
.L_D1057:
	.4byte	.L_D1057_e-.L_D1057
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1061
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B207
	.2byte	0x121
	.4byte	.L_B207_e
.L_D1057_e:
.L_D1062:
	.4byte	.L_D1062_e-.L_D1062
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1063
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1062_e-.L_l1062
.L_l1062:
.L_l1062_e:
.L_D1062_e:
.L_D1063:
	.4byte	.L_D1063_e-.L_D1063
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1064
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1063_e-.L_l1063
.L_l1063:
.L_l1063_e:
.L_D1063_e:
.L_D1064:
	.4byte	0x4
.L_D1061:
	.4byte	.L_D1061_e-.L_D1061
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1065
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B208
	.2byte	0x121
	.4byte	.L_B208_e
.L_D1061_e:
.L_D1066:
	.4byte	.L_D1066_e-.L_D1066
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1067
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1066_e-.L_l1066
.L_l1066:
.L_l1066_e:
.L_D1066_e:
.L_D1067:
	.4byte	.L_D1067_e-.L_D1067
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1068
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1067_e-.L_l1067
.L_l1067:
	.byte	0x1
	.4byte	0x0
.L_l1067_e:
.L_D1067_e:
.L_D1068:
	.4byte	0x4
.L_D1065:
	.4byte	.L_D1065_e-.L_D1065
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1069
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B210
	.2byte	0x121
	.4byte	.L_B210_e
.L_D1065_e:
.L_D1070:
	.4byte	.L_D1070_e-.L_D1070
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1071
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1070_e-.L_l1070
.L_l1070:
.L_l1070_e:
.L_D1070_e:
.L_D1071:
	.4byte	.L_D1071_e-.L_D1071
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1072
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1071_e-.L_l1071
.L_l1071:
.L_l1071_e:
.L_D1071_e:
.L_D1072:
	.4byte	0x4
.L_D1069:
	.4byte	.L_D1069_e-.L_D1069
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1073
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B211
	.2byte	0x121
	.4byte	.L_B211_e
.L_D1069_e:
.L_D1074:
	.4byte	.L_D1074_e-.L_D1074
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1075
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1074_e-.L_l1074
.L_l1074:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffffdc
	.byte	0x7
.L_l1074_e:
.L_D1074_e:
.L_D1075:
	.4byte	.L_D1075_e-.L_D1075
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1076
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1075_e-.L_l1075
.L_l1075:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffffe4
	.byte	0x7
.L_l1075_e:
.L_D1075_e:
.L_D1076:
	.4byte	0x4
.L_D1073:
	.4byte	.L_D1073_e-.L_D1073
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1077
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B213
	.2byte	0x121
	.4byte	.L_B213_e
.L_D1073_e:
.L_D1078:
	.4byte	.L_D1078_e-.L_D1078
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1079
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1078_e-.L_l1078
.L_l1078:
.L_l1078_e:
.L_D1078_e:
.L_D1079:
	.4byte	.L_D1079_e-.L_D1079
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1080
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1079_e-.L_l1079
.L_l1079:
.L_l1079_e:
.L_D1079_e:
.L_D1080:
	.4byte	0x4
.L_D1077:
	.4byte	.L_D1077_e-.L_D1077
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1081
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B214
	.2byte	0x121
	.4byte	.L_B214_e
.L_D1077_e:
.L_D1082:
	.4byte	.L_D1082_e-.L_D1082
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1083
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1082_e-.L_l1082
.L_l1082:
.L_l1082_e:
.L_D1082_e:
.L_D1083:
	.4byte	.L_D1083_e-.L_D1083
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1084
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1083_e-.L_l1083
.L_l1083:
	.byte	0x1
	.4byte	0x0
.L_l1083_e:
.L_D1083_e:
.L_D1084:
	.4byte	0x4
.L_D1081:
	.4byte	.L_D1081_e-.L_D1081
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1085
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B216
	.2byte	0x121
	.4byte	.L_B216_e
.L_D1081_e:
.L_D1086:
	.4byte	.L_D1086_e-.L_D1086
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1087
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1086_e-.L_l1086
.L_l1086:
.L_l1086_e:
.L_D1086_e:
.L_D1087:
	.4byte	.L_D1087_e-.L_D1087
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1088
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1087_e-.L_l1087
.L_l1087:
.L_l1087_e:
.L_D1087_e:
.L_D1088:
	.4byte	0x4
.L_D1085:
	.4byte	.L_D1085_e-.L_D1085
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1089
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B217
	.2byte	0x121
	.4byte	.L_B217_e
.L_D1085_e:
.L_D1090:
	.4byte	.L_D1090_e-.L_D1090
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1091
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1090_e-.L_l1090
.L_l1090:
.L_l1090_e:
.L_D1090_e:
.L_D1091:
	.4byte	.L_D1091_e-.L_D1091
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1092
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1091_e-.L_l1091
.L_l1091:
	.byte	0x1
	.4byte	0x0
.L_l1091_e:
.L_D1091_e:
.L_D1092:
	.4byte	0x4
.L_D1089:
	.4byte	.L_D1089_e-.L_D1089
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1093
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B219
	.2byte	0x121
	.4byte	.L_B219_e
.L_D1089_e:
.L_D1094:
	.4byte	.L_D1094_e-.L_D1094
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1095
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1094_e-.L_l1094
.L_l1094:
.L_l1094_e:
.L_D1094_e:
.L_D1095:
	.4byte	.L_D1095_e-.L_D1095
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1096
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1095_e-.L_l1095
.L_l1095:
.L_l1095_e:
.L_D1095_e:
.L_D1096:
	.4byte	0x4
.L_D1093:
	.4byte	.L_D1093_e-.L_D1093
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1097
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B220
	.2byte	0x121
	.4byte	.L_B220_e
.L_D1093_e:
.L_D1098:
	.4byte	.L_D1098_e-.L_D1098
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1099
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1098_e-.L_l1098
.L_l1098:
.L_l1098_e:
.L_D1098_e:
.L_D1099:
	.4byte	.L_D1099_e-.L_D1099
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1100
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1099_e-.L_l1099
.L_l1099:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffffd8
	.byte	0x7
.L_l1099_e:
.L_D1099_e:
.L_D1100:
	.4byte	0x4
.L_D1097:
	.4byte	.L_D1097_e-.L_D1097
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1101
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B222
	.2byte	0x121
	.4byte	.L_B222_e
.L_D1097_e:
.L_D1102:
	.4byte	.L_D1102_e-.L_D1102
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1103
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1102_e-.L_l1102
.L_l1102:
.L_l1102_e:
.L_D1102_e:
.L_D1103:
	.4byte	.L_D1103_e-.L_D1103
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1104
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1103_e-.L_l1103
.L_l1103:
.L_l1103_e:
.L_D1103_e:
.L_D1104:
	.4byte	0x4
.L_D1101:
	.4byte	.L_D1101_e-.L_D1101
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1105
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B223
	.2byte	0x121
	.4byte	.L_B223_e
.L_D1101_e:
.L_D1106:
	.4byte	.L_D1106_e-.L_D1106
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1107
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1106_e-.L_l1106
.L_l1106:
.L_l1106_e:
.L_D1106_e:
.L_D1107:
	.4byte	.L_D1107_e-.L_D1107
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1108
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1107_e-.L_l1107
.L_l1107:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffffd8
	.byte	0x7
.L_l1107_e:
.L_D1107_e:
.L_D1108:
	.4byte	0x4
.L_D1105:
	.4byte	.L_D1105_e-.L_D1105
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1109
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B225
	.2byte	0x121
	.4byte	.L_B225_e
.L_D1105_e:
.L_D1110:
	.4byte	.L_D1110_e-.L_D1110
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1111
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1110_e-.L_l1110
.L_l1110:
.L_l1110_e:
.L_D1110_e:
.L_D1111:
	.4byte	.L_D1111_e-.L_D1111
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1112
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1111_e-.L_l1111
.L_l1111:
.L_l1111_e:
.L_D1111_e:
.L_D1112:
	.4byte	0x4
.L_D1109:
	.4byte	.L_D1109_e-.L_D1109
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1113
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B226
	.2byte	0x121
	.4byte	.L_B226_e
.L_D1109_e:
.L_D1114:
	.4byte	.L_D1114_e-.L_D1114
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1115
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1114_e-.L_l1114
.L_l1114:
.L_l1114_e:
.L_D1114_e:
.L_D1115:
	.4byte	.L_D1115_e-.L_D1115
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1116
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1115_e-.L_l1115
.L_l1115:
	.byte	0x1
	.4byte	0x0
.L_l1115_e:
.L_D1115_e:
.L_D1116:
	.4byte	0x4
.L_D1113:
	.4byte	.L_D1113_e-.L_D1113
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1117
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B228
	.2byte	0x121
	.4byte	.L_B228_e
.L_D1113_e:
.L_D1118:
	.4byte	.L_D1118_e-.L_D1118
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1119
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1118_e-.L_l1118
.L_l1118:
.L_l1118_e:
.L_D1118_e:
.L_D1119:
	.4byte	.L_D1119_e-.L_D1119
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1120
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1119_e-.L_l1119
.L_l1119:
.L_l1119_e:
.L_D1119_e:
.L_D1120:
	.4byte	0x4
.L_D1117:
	.4byte	.L_D1117_e-.L_D1117
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1121
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B229
	.2byte	0x121
	.4byte	.L_B229_e
.L_D1117_e:
.L_D1122:
	.4byte	.L_D1122_e-.L_D1122
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1123
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1122_e-.L_l1122
.L_l1122:
.L_l1122_e:
.L_D1122_e:
.L_D1123:
	.4byte	.L_D1123_e-.L_D1123
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1124
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1123_e-.L_l1123
.L_l1123:
	.byte	0x1
	.4byte	0x0
.L_l1123_e:
.L_D1123_e:
.L_D1124:
	.4byte	0x4
.L_D1121:
	.4byte	.L_D1121_e-.L_D1121
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1125
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B230
	.2byte	0x121
	.4byte	.L_B230_e
.L_D1121_e:
.L_D1126:
	.4byte	.L_D1126_e-.L_D1126
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1127
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1126_e-.L_l1126
.L_l1126:
.L_l1126_e:
.L_D1126_e:
.L_D1127:
	.4byte	.L_D1127_e-.L_D1127
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1128
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1127_e-.L_l1127
.L_l1127:
.L_l1127_e:
.L_D1127_e:
.L_D1128:
	.4byte	0x4
.L_D1125:
	.4byte	.L_D1125_e-.L_D1125
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1129
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B231
	.2byte	0x121
	.4byte	.L_B231_e
.L_D1125_e:
.L_D1130:
	.4byte	.L_D1130_e-.L_D1130
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1131
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1130_e-.L_l1130
.L_l1130:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffffdc
	.byte	0x7
.L_l1130_e:
.L_D1130_e:
.L_D1131:
	.4byte	.L_D1131_e-.L_D1131
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1132
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1131_e-.L_l1131
.L_l1131:
	.byte	0x1
	.4byte	0x3
.L_l1131_e:
.L_D1131_e:
.L_D1132:
	.4byte	0x4
.L_D1129:
	.4byte	.L_D1129_e-.L_D1129
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1133
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B233
	.2byte	0x121
	.4byte	.L_B233_e
.L_D1129_e:
.L_D1134:
	.4byte	.L_D1134_e-.L_D1134
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1135
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1134_e-.L_l1134
.L_l1134:
.L_l1134_e:
.L_D1134_e:
.L_D1135:
	.4byte	.L_D1135_e-.L_D1135
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1136
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1135_e-.L_l1135
.L_l1135:
.L_l1135_e:
.L_D1135_e:
.L_D1136:
	.4byte	0x4
.L_D1133:
	.4byte	.L_D1133_e-.L_D1133
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1137
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B234
	.2byte	0x121
	.4byte	.L_B234_e
.L_D1133_e:
.L_D1138:
	.4byte	.L_D1138_e-.L_D1138
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1139
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1138_e-.L_l1138
.L_l1138:
.L_l1138_e:
.L_D1138_e:
.L_D1139:
	.4byte	.L_D1139_e-.L_D1139
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1140
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1139_e-.L_l1139
.L_l1139:
	.byte	0x1
	.4byte	0x0
.L_l1139_e:
.L_D1139_e:
.L_D1140:
	.4byte	0x4
.L_D1137:
	.4byte	.L_D1137_e-.L_D1137
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1141
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B236
	.2byte	0x121
	.4byte	.L_B236_e
.L_D1137_e:
.L_D1142:
	.4byte	.L_D1142_e-.L_D1142
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1143
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1142_e-.L_l1142
.L_l1142:
.L_l1142_e:
.L_D1142_e:
.L_D1143:
	.4byte	.L_D1143_e-.L_D1143
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1144
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1143_e-.L_l1143
.L_l1143:
.L_l1143_e:
.L_D1143_e:
.L_D1144:
	.4byte	0x4
.L_D1141:
	.4byte	.L_D1141_e-.L_D1141
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1145
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B237
	.2byte	0x121
	.4byte	.L_B237_e
.L_D1141_e:
.L_D1146:
	.4byte	.L_D1146_e-.L_D1146
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1147
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1146_e-.L_l1146
.L_l1146:
.L_l1146_e:
.L_D1146_e:
.L_D1147:
	.4byte	.L_D1147_e-.L_D1147
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1148
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l1147_e-.L_l1147
.L_l1147:
	.4byte	0x0
.L_l1147_e:
.L_D1147_e:
.L_D1148:
	.4byte	0x4
.L_D1145:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_hangup,@function
sas_hangup:
.L_LC1655:

.section	.line
	.4byte	2480	/ sas.c:2480
	.2byte	0xffff
	.4byte	.L_LC1655-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	pushl %ebx
	movl 8(%ebp),%ebx
.L_b18:
.L_LC1656:

.section	.line
	.4byte	2481	/ sas.c:2481
	.2byte	0xffff
	.4byte	.L_LC1656-.L_text_b
	.previous
.L_LC1657:

.section	.line
	.4byte	2484	/ sas.c:2484
	.2byte	0xffff
	.4byte	.L_LC1657-.L_text_b
	.previous
	testb $2,33(%ebx)
	je .L882
.L_LC1658:

.section	.line
	.4byte	2487	/ sas.c:2487
	.2byte	0xffff
	.4byte	.L_LC1658-.L_text_b
	.previous
	movb 54(%ebx),%al
	orb 53(%ebx),%al
	notb %al
	andb %al,62(%ebx)
.L_LC1659:

.section	.line
	.4byte	2489	/ sas.c:2489
	.2byte	0xffff
	.4byte	.L_LC1659-.L_text_b
	.previous
	movb 62(%ebx),%cl
	orb 52(%ebx),%cl
	movb %cl,62(%ebx)
.L_LC1660:

.section	.line
	.4byte	2490	/ sas.c:2490
	.2byte	0xffff
	.4byte	.L_LC1660-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L884
.L_B239:
.L_LC1661:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1661-.L_text_b
	.previous
.L_LC1662:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1662-.L_text_b
	.previous
	movb 90(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1663:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1663-.L_text_b
	.previous
.L_B239_e:
.L_LC1664:

.section	.line
	.4byte	2490	/ sas.c:2490
	.2byte	0xffff
	.4byte	.L_LC1664-.L_text_b
	.previous
.L884:
.L_B240:
.L_LC1665:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1665-.L_text_b
	.previous
.L_LC1666:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1666-.L_text_b
	.previous
	movb %cl,%al
	movw 88(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1667:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1667-.L_text_b
	.previous
.L_B240_e:
.L_LC1668:

.section	.line
	.4byte	2491	/ sas.c:2491
	.2byte	0xffff
	.4byte	.L_LC1668-.L_text_b
	.previous
	andw $64991,32(%ebx)
.L_LC1669:

.section	.line
	.4byte	2492	/ sas.c:2492
	.2byte	0xffff
	.4byte	.L_LC1669-.L_text_b
	.previous
	pushl $100
	pushl %ebx
	pushl $sas_hangup
	call timeout
.L_LC1670:

.section	.line
	.4byte	2493	/ sas.c:2493
	.2byte	0xffff
	.4byte	.L_LC1670-.L_text_b
	.previous
	jmp .L887
	.align 4
.L882:
.L_LC1671:

.section	.line
	.4byte	2499	/ sas.c:2499
	.2byte	0xffff
	.4byte	.L_LC1671-.L_text_b
	.previous
	testb $4,44(%ebx)
	je .L888
.L_LC1672:

.section	.line
	.4byte	2502	/ sas.c:2502
	.2byte	0xffff
	.4byte	.L_LC1672-.L_text_b
	.previous
	call spl6
.L_LC1673:

.section	.line
	.4byte	2503	/ sas.c:2503
	.2byte	0xffff
	.4byte	.L_LC1673-.L_text_b
	.previous
	pushl %eax
	call splx
.L_LC1674:

.section	.line
	.4byte	2505	/ sas.c:2505
	.2byte	0xffff
	.4byte	.L_LC1674-.L_text_b
	.previous
	pushl %ebx
	call sas_open_device
.L_LC1675:

.section	.line
	.4byte	2506	/ sas.c:2506
	.2byte	0xffff
	.4byte	.L_LC1675-.L_text_b
	.previous
	pushl $1207
	pushl $1
	pushl %ebx
	call sas_param
.L_LC1676:

.section	.line
	.4byte	2509	/ sas.c:2509
	.2byte	0xffff
	.4byte	.L_LC1676-.L_text_b
	.previous
	call spl6
.L_LC1677:

.section	.line
	.4byte	2510	/ sas.c:2510
	.2byte	0xffff
	.4byte	.L_LC1677-.L_text_b
	.previous
	pushl %eax
	call splx
.L_LC1678:

.section	.line
	.4byte	2511	/ sas.c:2511
	.2byte	0xffff
	.4byte	.L_LC1678-.L_text_b
	.previous
	addl $24,%esp
.L888:
.L_LC1679:

.section	.line
	.4byte	2512	/ sas.c:2512
	.2byte	0xffff
	.4byte	.L_LC1679-.L_text_b
	.previous
	andb $239,32(%ebx)
	leal 32(%ebx),%eax
	pushl %eax
	call wakeup
.L887:
.L_LC1680:

.section	.line
	.4byte	2514	/ sas.c:2514
	.2byte	0xffff
	.4byte	.L_LC1680-.L_text_b
	.previous
.L_b18_e:
	movl -4(%ebp),%ebx
	leave
	ret
.L_f18_e:
.Lfe15:
	.size	 sas_hangup,.Lfe15-sas_hangup

.section	.debug
.L_D889:
	.4byte	.L_D889_e-.L_D889
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D1149
	.2byte	0x38
	.string	"sas_hangup"
	.2byte	0x111
	.4byte	sas_hangup
	.2byte	0x121
	.4byte	.L_f18_e
	.2byte	0x8041
	.4byte	.L_b18
	.2byte	0x8051
	.4byte	.L_b18_e
.L_D889_e:
.L_D1150:
	.4byte	.L_D1150_e-.L_D1150
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1151
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t1150_e-.L_t1150
.L_t1150:
	.byte	0x1
	.4byte	.L_T816
.L_t1150_e:
	.2byte	0x23
	.2byte	.L_l1150_e-.L_l1150
.L_l1150:
	.byte	0x1
	.4byte	0x3
.L_l1150_e:
.L_D1150_e:
.L_D1151:
	.4byte	.L_D1151_e-.L_D1151
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1152
	.2byte	0x38
	.string	"old_level"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1151_e-.L_l1151
.L_l1151:
	.byte	0x1
	.4byte	0x0
.L_l1151_e:
.L_D1151_e:
.L_D1152:
	.4byte	.L_D1152_e-.L_D1152
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1153
	.2byte	0x38
	.string	"regvar"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1152_e-.L_l1152
.L_l1152:
.L_l1152_e:
.L_D1152_e:
.L_D1153:
	.4byte	.L_D1153_e-.L_D1153
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1154
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B239
	.2byte	0x121
	.4byte	.L_B239_e
.L_D1153_e:
.L_D1155:
	.4byte	.L_D1155_e-.L_D1155
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1156
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1155_e-.L_l1155
.L_l1155:
.L_l1155_e:
.L_D1155_e:
.L_D1156:
	.4byte	.L_D1156_e-.L_D1156
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1157
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1156_e-.L_l1156
.L_l1156:
.L_l1156_e:
.L_D1156_e:
.L_D1157:
	.4byte	0x4
.L_D1154:
	.4byte	.L_D1154_e-.L_D1154
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1158
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B240
	.2byte	0x121
	.4byte	.L_B240_e
.L_D1154_e:
.L_D1159:
	.4byte	.L_D1159_e-.L_D1159
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1160
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1159_e-.L_l1159
.L_l1159:
.L_l1159_e:
.L_D1159_e:
.L_D1160:
	.4byte	.L_D1160_e-.L_D1160
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1161
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1160_e-.L_l1160
.L_l1160:
.L_l1160_e:
.L_D1160_e:
.L_D1161:
	.4byte	0x4
.L_D1158:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_timeout,@function
sas_timeout:
.L_LC1681:

.section	.line
	.4byte	2520	/ sas.c:2520
	.2byte	0xffff
	.4byte	.L_LC1681-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	subl $4,%esp
	pushl %ebx
	movl 8(%ebp),%ebx
.L_b19:
.L_LC1682:

.section	.line
	.4byte	2521	/ sas.c:2521
	.2byte	0xffff
	.4byte	.L_LC1682-.L_text_b
	.previous
.L_LC1683:

.section	.line
	.4byte	2524	/ sas.c:2524
	.2byte	0xffff
	.4byte	.L_LC1683-.L_text_b
	.previous
	testb $4,33(%ebx)
	je .L890
.L_LC1684:

.section	.line
	.4byte	2527	/ sas.c:2527
	.2byte	0xffff
	.4byte	.L_LC1684-.L_text_b
	.previous
	movb 63(%ebx),%cl
	orb $64,%cl
	movb %cl,63(%ebx)
.L_LC1685:

.section	.line
	.4byte	2528	/ sas.c:2528
	.2byte	0xffff
	.4byte	.L_LC1685-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L892
.L_B242:
.L_LC1686:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1686-.L_text_b
	.previous
.L_LC1687:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1687-.L_text_b
	.previous
	movb 86(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1688:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1688-.L_text_b
	.previous
.L_B242_e:
.L_LC1689:

.section	.line
	.4byte	2528	/ sas.c:2528
	.2byte	0xffff
	.4byte	.L_LC1689-.L_text_b
	.previous
.L892:
.L_B243:
.L_LC1690:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1690-.L_text_b
	.previous
.L_LC1691:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1691-.L_text_b
	.previous
	movb %cl,%al
	movw 84(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1692:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1692-.L_text_b
	.previous
.L_B243_e:
.L_LC1693:

.section	.line
	.4byte	2529	/ sas.c:2529
	.2byte	0xffff
	.4byte	.L_LC1693-.L_text_b
	.previous
	andw $62463,32(%ebx)
.L_LC1694:

.section	.line
	.4byte	2530	/ sas.c:2530
	.2byte	0xffff
	.4byte	.L_LC1694-.L_text_b
	.previous
	pushl $25
	pushl %ebx
	pushl $sas_timeout
	call timeout
.L_LC1695:

.section	.line
	.4byte	2531	/ sas.c:2531
	.2byte	0xffff
	.4byte	.L_LC1695-.L_text_b
	.previous
	jmp .L889
	.align 4
.L890:
.L_LC1696:

.section	.line
	.4byte	2535	/ sas.c:2535
	.2byte	0xffff
	.4byte	.L_LC1696-.L_text_b
	.previous
	cmpb $0,32(%ebx)
	jge .L895
	testb $64,63(%ebx)
	je .L895
.L_LC1697:

.section	.line
	.4byte	2538	/ sas.c:2538
	.2byte	0xffff
	.4byte	.L_LC1697-.L_text_b
	.previous
	movb 63(%ebx),%cl
	andb $191,%cl
	movb %cl,63(%ebx)
.L_LC1698:

.section	.line
	.4byte	2539	/ sas.c:2539
	.2byte	0xffff
	.4byte	.L_LC1698-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L897
.L_B244:
.L_LC1699:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1699-.L_text_b
	.previous
.L_LC1700:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1700-.L_text_b
	.previous
	movb 86(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1701:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1701-.L_text_b
	.previous
.L_B244_e:
.L_LC1702:

.section	.line
	.4byte	2539	/ sas.c:2539
	.2byte	0xffff
	.4byte	.L_LC1702-.L_text_b
	.previous
.L897:
.L_B245:
.L_LC1703:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1703-.L_text_b
	.previous
.L_LC1704:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1704-.L_text_b
	.previous
	movb %cl,%al
	movw 84(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1705:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1705-.L_text_b
	.previous
.L_B245_e:
.L_LC1706:

.section	.line
	.4byte	2540	/ sas.c:2540
	.2byte	0xffff
	.4byte	.L_LC1706-.L_text_b
	.previous
	orw $2048,32(%ebx)
.L_LC1707:

.section	.line
	.4byte	2541	/ sas.c:2541
	.2byte	0xffff
	.4byte	.L_LC1707-.L_text_b
	.previous
	movl 20(%ebx),%eax
	andl $15,%eax
	pushl sas_ctimes(,%eax,4)
	pushl %ebx
	pushl $sas_timeout
	call timeout
	movl %eax,8(%ebx)
.L_LC1708:

.section	.line
	.4byte	2543	/ sas.c:2543
	.2byte	0xffff
	.4byte	.L_LC1708-.L_text_b
	.previous
	jmp .L889
	.align 4
.L895:
.L_LC1709:

.section	.line
	.4byte	2546	/ sas.c:2546
	.2byte	0xffff
	.4byte	.L_LC1709-.L_text_b
	.previous
	andw $63359,32(%ebx)
.L_LC1710:

.section	.line
	.4byte	2548	/ sas.c:2548
	.2byte	0xffff
	.4byte	.L_LC1710-.L_text_b
	.previous
	movl 156(%ebx),%ecx
	testb $16,28(%ecx)
	je .L901
	addl $64,%ecx
.L901:
	pushl %ecx
	call qsize
	addl $4,%esp
	testl %eax,%eax
	jne .L900
	testb $64,32(%ebx)
	jne .L900
.L_LC1711:

.section	.line
	.4byte	2550	/ sas.c:2550
	.2byte	0xffff
	.4byte	.L_LC1711-.L_text_b
	.previous
	movl 112(%ebx),%eax
	andl $-33,(%eax)
.L_LC1712:

.section	.line
	.4byte	2551	/ sas.c:2551
	.2byte	0xffff
	.4byte	.L_LC1712-.L_text_b
	.previous
	pushl %ebx
	call sas_hdx_check
.L_LC1713:

.section	.line
	.4byte	2552	/ sas.c:2552
	.2byte	0xffff
	.4byte	.L_LC1713-.L_text_b
	.previous
	addl $4,%esp
.L900:
.L_LC1714:

.section	.line
	.4byte	2554	/ sas.c:2554
	.2byte	0xffff
	.4byte	.L_LC1714-.L_text_b
	.previous
	orb $2,40(%ebx)
	cmpl $0,event_scheduled
	jne .L902
	movl $1,event_scheduled
	pushl $8
	pushl $0
	pushl $sas_event
	call timeout
	addl $12,%esp
.L902:
.L_LC1715:

.section	.line
	.4byte	2556	/ sas.c:2556
	.2byte	0xffff
	.4byte	.L_LC1715-.L_text_b
	.previous
	leal 32(%ebx),%eax
	pushl %eax
	call wakeup
.L_LC1716:

.section	.line
	.4byte	2557	/ sas.c:2557
	.2byte	0xffff
	.4byte	.L_LC1716-.L_text_b
	.previous
.L889:
.L_b19_e:
	movl -8(%ebp),%ebx
	leave
	ret
.L_f19_e:
.Lfe16:
	.size	 sas_timeout,.Lfe16-sas_timeout

.section	.debug
.L_D1149:
	.4byte	.L_D1149_e-.L_D1149
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D1162
	.2byte	0x38
	.string	"sas_timeout"
	.2byte	0x111
	.4byte	sas_timeout
	.2byte	0x121
	.4byte	.L_f19_e
	.2byte	0x8041
	.4byte	.L_b19
	.2byte	0x8051
	.4byte	.L_b19_e
.L_D1149_e:
.L_D1163:
	.4byte	.L_D1163_e-.L_D1163
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1164
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t1163_e-.L_t1163
.L_t1163:
	.byte	0x1
	.4byte	.L_T816
.L_t1163_e:
	.2byte	0x23
	.2byte	.L_l1163_e-.L_l1163
.L_l1163:
	.byte	0x1
	.4byte	0x3
.L_l1163_e:
.L_D1163_e:
.L_D1164:
	.4byte	.L_D1164_e-.L_D1164
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1165
	.2byte	0x38
	.string	"regvar"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1164_e-.L_l1164
.L_l1164:
.L_l1164_e:
.L_D1164_e:
.L_D1165:
	.4byte	.L_D1165_e-.L_D1165
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1166
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B242
	.2byte	0x121
	.4byte	.L_B242_e
.L_D1165_e:
.L_D1167:
	.4byte	.L_D1167_e-.L_D1167
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1168
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1167_e-.L_l1167
.L_l1167:
.L_l1167_e:
.L_D1167_e:
.L_D1168:
	.4byte	.L_D1168_e-.L_D1168
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1169
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1168_e-.L_l1168
.L_l1168:
.L_l1168_e:
.L_D1168_e:
.L_D1169:
	.4byte	0x4
.L_D1166:
	.4byte	.L_D1166_e-.L_D1166
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1170
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B243
	.2byte	0x121
	.4byte	.L_B243_e
.L_D1166_e:
.L_D1171:
	.4byte	.L_D1171_e-.L_D1171
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1172
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1171_e-.L_l1171
.L_l1171:
.L_l1171_e:
.L_D1171_e:
.L_D1172:
	.4byte	.L_D1172_e-.L_D1172
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1173
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1172_e-.L_l1172
.L_l1172:
.L_l1172_e:
.L_D1172_e:
.L_D1173:
	.4byte	0x4
.L_D1170:
	.4byte	.L_D1170_e-.L_D1170
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1174
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B244
	.2byte	0x121
	.4byte	.L_B244_e
.L_D1170_e:
.L_D1175:
	.4byte	.L_D1175_e-.L_D1175
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1176
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1175_e-.L_l1175
.L_l1175:
.L_l1175_e:
.L_D1175_e:
.L_D1176:
	.4byte	.L_D1176_e-.L_D1176
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1177
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1176_e-.L_l1176
.L_l1176:
.L_l1176_e:
.L_D1176_e:
.L_D1177:
	.4byte	0x4
.L_D1174:
	.4byte	.L_D1174_e-.L_D1174
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1178
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B245
	.2byte	0x121
	.4byte	.L_B245_e
.L_D1174_e:
.L_D1179:
	.4byte	.L_D1179_e-.L_D1179
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1180
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1179_e-.L_l1179
.L_l1179:
.L_l1179_e:
.L_D1179_e:
.L_D1180:
	.4byte	.L_D1180_e-.L_D1180
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1181
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1180_e-.L_l1180
.L_l1180:
.L_l1180_e:
.L_D1180_e:
.L_D1181:
	.4byte	0x4
.L_D1178:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_hdx_check,@function
sas_hdx_check:
.L_LC1717:

.section	.line
	.4byte	2563	/ sas.c:2563
	.2byte	0xffff
	.4byte	.L_LC1717-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	pushl %ebx
	movl 8(%ebp),%ebx
.L_b20:
.L_LC1718:

.section	.line
	.4byte	2564	/ sas.c:2564
	.2byte	0xffff
	.4byte	.L_LC1718-.L_text_b
	.previous
.L_LC1719:

.section	.line
	.4byte	2567	/ sas.c:2567
	.2byte	0xffff
	.4byte	.L_LC1719-.L_text_b
	.previous
	testb $2,36(%ebx)
	jne .L903
.L_LC1720:

.section	.line
	.4byte	2578	/ sas.c:2578
	.2byte	0xffff
	.4byte	.L_LC1720-.L_text_b
	.previous
	testb $32,36(%ebx)
	je .L905
.L_LC1721:

.section	.line
	.4byte	2583	/ sas.c:2583
	.2byte	0xffff
	.4byte	.L_LC1721-.L_text_b
	.previous
	testb $4,36(%ebx)
	je .L903
	movl 112(%ebx),%eax
	testb $32,(%eax)
	jne .L903
.L_LC1722:

.section	.line
	.4byte	2586	/ sas.c:2586
	.2byte	0xffff
	.4byte	.L_LC1722-.L_text_b
	.previous
	movb 59(%ebx),%al
	notb %al
	movb %al,%cl
	andb 62(%ebx),%cl
	movb %cl,62(%ebx)
.L_LC1723:

.section	.line
	.4byte	2587	/ sas.c:2587
	.2byte	0xffff
	.4byte	.L_LC1723-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L908
.L_B247:
.L_LC1724:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1724-.L_text_b
	.previous
.L_LC1725:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1725-.L_text_b
	.previous
	movb 90(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1726:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1726-.L_text_b
	.previous
.L_B247_e:
.L_LC1727:

.section	.line
	.4byte	2587	/ sas.c:2587
	.2byte	0xffff
	.4byte	.L_LC1727-.L_text_b
	.previous
.L908:
.L_B248:
.L_LC1728:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1728-.L_text_b
	.previous
.L_LC1729:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1729-.L_text_b
	.previous
	movb %cl,%al
	movw 88(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1730:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1730-.L_text_b
	.previous
.L_B248_e:
.L_LC1731:

.section	.line
	.4byte	2588	/ sas.c:2588
	.2byte	0xffff
	.4byte	.L_LC1731-.L_text_b
	.previous
	andb $223,36(%ebx)
.L_LC1732:

.section	.line
	.4byte	2590	/ sas.c:2590
	.2byte	0xffff
	.4byte	.L_LC1732-.L_text_b
	.previous
	jmp .L903
	.align 4
.L905:
.L_LC1733:

.section	.line
	.4byte	2597	/ sas.c:2597
	.2byte	0xffff
	.4byte	.L_LC1733-.L_text_b
	.previous
	testb $4,36(%ebx)
	je .L913
	movl 112(%ebx),%eax
	testb $32,(%eax)
	je .L903
.L913:
.L_LC1734:

.section	.line
	.4byte	2600	/ sas.c:2600
	.2byte	0xffff
	.4byte	.L_LC1734-.L_text_b
	.previous
	movb 62(%ebx),%cl
	orb 59(%ebx),%cl
	movb %cl,62(%ebx)
.L_LC1735:

.section	.line
	.4byte	2601	/ sas.c:2601
	.2byte	0xffff
	.4byte	.L_LC1735-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L915
.L_B249:
.L_LC1736:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1736-.L_text_b
	.previous
.L_LC1737:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1737-.L_text_b
	.previous
	movb 90(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1738:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1738-.L_text_b
	.previous
.L_B249_e:
.L_LC1739:

.section	.line
	.4byte	2601	/ sas.c:2601
	.2byte	0xffff
	.4byte	.L_LC1739-.L_text_b
	.previous
.L915:
.L_B250:
.L_LC1740:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1740-.L_text_b
	.previous
.L_LC1741:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1741-.L_text_b
	.previous
	movb %cl,%al
	movw 88(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1742:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1742-.L_text_b
	.previous
.L_B250_e:
.L_LC1743:

.section	.line
	.4byte	2602	/ sas.c:2602
	.2byte	0xffff
	.4byte	.L_LC1743-.L_text_b
	.previous
	orb $32,36(%ebx)
.L_LC1744:

.section	.line
	.4byte	2605	/ sas.c:2605
	.2byte	0xffff
	.4byte	.L_LC1744-.L_text_b
	.previous
.L903:
.L_b20_e:
	movl -4(%ebp),%ebx
	leave
	ret
.L_f20_e:
.Lfe17:
	.size	 sas_hdx_check,.Lfe17-sas_hdx_check

.section	.debug
.L_D1162:
	.4byte	.L_D1162_e-.L_D1162
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D1182
	.2byte	0x38
	.string	"sas_hdx_check"
	.2byte	0x111
	.4byte	sas_hdx_check
	.2byte	0x121
	.4byte	.L_f20_e
	.2byte	0x8041
	.4byte	.L_b20
	.2byte	0x8051
	.4byte	.L_b20_e
.L_D1162_e:
.L_D1183:
	.4byte	.L_D1183_e-.L_D1183
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1184
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t1183_e-.L_t1183
.L_t1183:
	.byte	0x1
	.4byte	.L_T816
.L_t1183_e:
	.2byte	0x23
	.2byte	.L_l1183_e-.L_l1183
.L_l1183:
	.byte	0x1
	.4byte	0x3
.L_l1183_e:
.L_D1183_e:
.L_D1184:
	.4byte	.L_D1184_e-.L_D1184
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1185
	.2byte	0x38
	.string	"regvar"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1184_e-.L_l1184
.L_l1184:
.L_l1184_e:
.L_D1184_e:
.L_D1185:
	.4byte	.L_D1185_e-.L_D1185
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1186
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B247
	.2byte	0x121
	.4byte	.L_B247_e
.L_D1185_e:
.L_D1187:
	.4byte	.L_D1187_e-.L_D1187
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1188
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1187_e-.L_l1187
.L_l1187:
.L_l1187_e:
.L_D1187_e:
.L_D1188:
	.4byte	.L_D1188_e-.L_D1188
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1189
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1188_e-.L_l1188
.L_l1188:
.L_l1188_e:
.L_D1188_e:
.L_D1189:
	.4byte	0x4
.L_D1186:
	.4byte	.L_D1186_e-.L_D1186
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1190
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B248
	.2byte	0x121
	.4byte	.L_B248_e
.L_D1186_e:
.L_D1191:
	.4byte	.L_D1191_e-.L_D1191
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1192
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1191_e-.L_l1191
.L_l1191:
.L_l1191_e:
.L_D1191_e:
.L_D1192:
	.4byte	.L_D1192_e-.L_D1192
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1193
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1192_e-.L_l1192
.L_l1192:
.L_l1192_e:
.L_D1192_e:
.L_D1193:
	.4byte	0x4
.L_D1190:
	.4byte	.L_D1190_e-.L_D1190
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1194
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B249
	.2byte	0x121
	.4byte	.L_B249_e
.L_D1190_e:
.L_D1195:
	.4byte	.L_D1195_e-.L_D1195
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1196
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1195_e-.L_l1195
.L_l1195:
.L_l1195_e:
.L_D1195_e:
.L_D1196:
	.4byte	.L_D1196_e-.L_D1196
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1197
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1196_e-.L_l1196
.L_l1196:
.L_l1196_e:
.L_D1196_e:
.L_D1197:
	.4byte	0x4
.L_D1194:
	.4byte	.L_D1194_e-.L_D1194
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1198
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B250
	.2byte	0x121
	.4byte	.L_B250_e
.L_D1194_e:
.L_D1199:
	.4byte	.L_D1199_e-.L_D1199
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1200
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1199_e-.L_l1199
.L_l1199:
.L_l1199_e:
.L_D1199_e:
.L_D1200:
	.4byte	.L_D1200_e-.L_D1200
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1201
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1200_e-.L_l1200
.L_l1200:
.L_l1200_e:
.L_D1200_e:
.L_D1201:
	.4byte	0x4
.L_D1198:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_xproc,@function
sas_xproc:
.L_LC1745:

.section	.line
	.4byte	2611	/ sas.c:2611
	.2byte	0xffff
	.4byte	.L_LC1745-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	subl $4,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 8(%ebp),%esi
.L_b21:
.L_LC1746:

.section	.line
	.4byte	2612	/ sas.c:2612
	.2byte	0xffff
	.4byte	.L_LC1746-.L_text_b
	.previous
.L_LC1747:

.section	.line
	.4byte	2613	/ sas.c:2613
	.2byte	0xffff
	.4byte	.L_LC1747-.L_text_b
	.previous
	movl 160(%esi),%ebx
.L_LC1748:

.section	.line
	.4byte	2617	/ sas.c:2617
	.2byte	0xffff
	.4byte	.L_LC1748-.L_text_b
	.previous
	testw $448,32(%esi)
	jne .L918
	testb $8,36(%esi)
	jne .L918
.L_LC1749:

.section	.line
	.4byte	2621	/ sas.c:2621
	.2byte	0xffff
	.4byte	.L_LC1749-.L_text_b
	.previous
	movl 100(%esi),%edi
.L_LC1750:

.section	.line
	.4byte	2624	/ sas.c:2624
	.2byte	0xffff
	.4byte	.L_LC1750-.L_text_b
	.previous
	testb $1,37(%esi)
	je .L921
.L_LC1751:

.section	.line
	.4byte	2631	/ sas.c:2631
	.2byte	0xffff
	.4byte	.L_LC1751-.L_text_b
	.previous
	testb $6,32(%esi)
	je .L923
.L_B252:
.L_LC1752:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1752-.L_text_b
	.previous
.L_LC1753:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1753-.L_text_b
	.previous
	movb 74(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1754:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1754-.L_text_b
	.previous
.L_B252_e:
.L_LC1755:

.section	.line
	.4byte	2633	/ sas.c:2633
	.2byte	0xffff
	.4byte	.L_LC1755-.L_text_b
	.previous
.L923:
.L_B253:
.L_LC1756:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1756-.L_text_b
	.previous
	cmpb $0,36(%esi)
	jge .L925
	movb 193(%esi),%cl
	jmp .L926
	.align 4
.L925:
	movb 192(%esi),%cl
.L926:
.L_LC1757:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1757-.L_text_b
	.previous
	movb %cl,%al
	movw 72(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1758:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1758-.L_text_b
	.previous
.L_B253_e:
.L_LC1759:

.section	.line
	.4byte	2635	/ sas.c:2635
	.2byte	0xffff
	.4byte	.L_LC1759-.L_text_b
	.previous
	movl 112(%esi),%eax
	andl $-49153,(%eax)
.L_LC1760:

.section	.line
	.4byte	2636	/ sas.c:2636
	.2byte	0xffff
	.4byte	.L_LC1760-.L_text_b
	.previous
	orb $64,32(%esi)
.L_LC1761:

.section	.line
	.4byte	2637	/ sas.c:2637
	.2byte	0xffff
	.4byte	.L_LC1761-.L_text_b
	.previous
	andw $65279,36(%esi)
.L_LC1762:

.section	.line
	.4byte	2639	/ sas.c:2639
	.2byte	0xffff
	.4byte	.L_LC1762-.L_text_b
	.previous
	testb $8,33(%esi)
	je .L928
.L_LC1763:

.section	.line
	.4byte	2641	/ sas.c:2641
	.2byte	0xffff
	.4byte	.L_LC1763-.L_text_b
	.previous
	andw $63487,32(%esi)
.L_LC1764:

.section	.line
	.4byte	2642	/ sas.c:2642
	.2byte	0xffff
	.4byte	.L_LC1764-.L_text_b
	.previous
	pushl 8(%esi)
	call untimeout
.L_LC1765:

.section	.line
	.4byte	2643	/ sas.c:2643
	.2byte	0xffff
	.4byte	.L_LC1765-.L_text_b
	.previous
	addl $4,%esp
.L928:
.L_LC1766:

.section	.line
	.4byte	2644	/ sas.c:2644
	.2byte	0xffff
	.4byte	.L_LC1766-.L_text_b
	.previous
	decl %edi
.L921:
.L_LC1767:

.section	.line
	.4byte	2648	/ sas.c:2648
	.2byte	0xffff
	.4byte	.L_LC1767-.L_text_b
	.previous
	testb $64,36(%esi)
	jne .L918
.L_LC1768:

.section	.line
	.4byte	2653	/ sas.c:2653
	.2byte	0xffff
	.4byte	.L_LC1768-.L_text_b
	.previous
	jmp .L930
	.align 4
	.align 4
.L936:
.L_LC1769:

.section	.line
	.4byte	2654	/ sas.c:2654
	.2byte	0xffff
	.4byte	.L_LC1769-.L_text_b
	.previous
	movl 8(%ebx),%edx
	movl %edx,160(%esi)
.L_LC1770:

.section	.line
	.4byte	2655	/ sas.c:2655
	.2byte	0xffff
	.4byte	.L_LC1770-.L_text_b
	.previous
	movl $0,8(%ebx)
.L_LC1771:

.section	.line
	.4byte	2656	/ sas.c:2656
	.2byte	0xffff
	.4byte	.L_LC1771-.L_text_b
	.previous
	pushl %ebx
	leal 172(%esi),%eax
	pushl %eax
	call sas_link
.L_LC1772:

.section	.line
	.4byte	2658	/ sas.c:2658
	.2byte	0xffff
	.4byte	.L_LC1772-.L_text_b
	.previous
	movl 160(%esi),%ebx
	addl $8,%esp
	testl %ebx,%ebx
	je .L948
.L_LC1773:

.section	.line
	.4byte	2661	/ sas.c:2661
	.2byte	0xffff
	.4byte	.L_LC1773-.L_text_b
	.previous
	cmpl $0,8(%ebx)
	jne .L930
.L_LC1774:

.section	.line
	.4byte	2662	/ sas.c:2662
	.2byte	0xffff
	.4byte	.L_LC1774-.L_text_b
	.previous
	pushl $1
	movl 156(%esi),%ecx
	testb $16,28(%ecx)
	je .L935
	addl $64,%ecx
.L935:
	pushl %ecx
	pushl $sas_qenable
	call timeout
	addl $12,%esp
.L_LC1775:

.section	.line
	.4byte	2664	/ sas.c:2664
	.2byte	0xffff
	.4byte	.L_LC1775-.L_text_b
	.previous
.L930:
	testl %ebx,%ebx
	je .L948
	movl 16(%ebx),%ecx
	subl 12(%ebx),%ecx
	je .L936
.L_LC1776:

.section	.line
	.4byte	2666	/ sas.c:2666
	.2byte	0xffff
	.4byte	.L_LC1776-.L_text_b
	.previous
	testl %ebx,%ebx
	jne .L937
.L948:
.L_LC1777:

.section	.line
	.4byte	2667	/ sas.c:2667
	.2byte	0xffff
	.4byte	.L_LC1777-.L_text_b
	.previous
	movl 112(%esi),%ecx
	testb $32,1(%ecx)
	je .L938
.L_LC1778:

.section	.line
	.4byte	2668	/ sas.c:2668
	.2byte	0xffff
	.4byte	.L_LC1778-.L_text_b
	.previous
	andl $-8225,(%ecx)
.L_LC1779:

.section	.line
	.4byte	2669	/ sas.c:2669
	.2byte	0xffff
	.4byte	.L_LC1779-.L_text_b
	.previous
	pushl 112(%esi)
	call wakeup
.L_LC1780:

.section	.line
	.4byte	2670	/ sas.c:2670
	.2byte	0xffff
	.4byte	.L_LC1780-.L_text_b
	.previous
	addl $4,%esp
.L938:
.L_LC1781:

.section	.line
	.4byte	2671	/ sas.c:2671
	.2byte	0xffff
	.4byte	.L_LC1781-.L_text_b
	.previous
	movl 112(%esi),%eax
	andl $-33,(%eax)
.L_LC1782:

.section	.line
	.4byte	2672	/ sas.c:2672
	.2byte	0xffff
	.4byte	.L_LC1782-.L_text_b
	.previous
	orb $2,40(%esi)
	cmpl $0,event_scheduled
	jne .L918
	movl $1,event_scheduled
	pushl $8
	pushl $0
	pushl $sas_event
	call timeout
.L_LC1783:

.section	.line
	.4byte	2673	/ sas.c:2673
	.2byte	0xffff
	.4byte	.L_LC1783-.L_text_b
	.previous
	jmp .L918
	.align 4
.L937:
.L_LC1784:

.section	.line
	.4byte	2676	/ sas.c:2676
	.2byte	0xffff
	.4byte	.L_LC1784-.L_text_b
	.previous
	cmpl %edi,%ecx
	jae .L940
.L_LC1785:

.section	.line
	.4byte	2677	/ sas.c:2677
	.2byte	0xffff
	.4byte	.L_LC1785-.L_text_b
	.previous
	movl %ecx,%edi
.L940:
.L_LC1786:

.section	.line
	.4byte	2679	/ sas.c:2679
	.2byte	0xffff
	.4byte	.L_LC1786-.L_text_b
	.previous
	addl %edi,sysinfo+152
.L_LC1787:

.section	.line
	.4byte	2681	/ sas.c:2681
	.2byte	0xffff
	.4byte	.L_LC1787-.L_text_b
	.previous
	testb $6,32(%esi)
	je .L942
.L_B254:
.L_LC1788:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1788-.L_text_b
	.previous
.L_LC1789:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1789-.L_text_b
	.previous
	movb 74(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1790:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1790-.L_text_b
	.previous
.L_B254_e:
.L942:
.L_LC1791:

.section	.line
	.4byte	2683	/ sas.c:2683
	.2byte	0xffff
	.4byte	.L_LC1791-.L_text_b
	.previous
	cmpl $1,%edi
	jne .L944
.L_LC1792:

.section	.line
	.4byte	2684	/ sas.c:2684
	.2byte	0xffff
	.4byte	.L_LC1792-.L_text_b
	.previous
.L_B255:
.L_LC1793:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1793-.L_text_b
	.previous
	movl 12(%ebx),%eax
	movw 72(%esi),%dx
	movb (%eax),%al
	incl 12(%ebx)
.L_LC1794:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1794-.L_text_b
	.previous
/APP
	outb %al,%dx
/NO_APP
.L_LC1795:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1795-.L_text_b
	.previous
.L_B255_e:
.L_LC1796:

.section	.line
	.4byte	2685	/ sas.c:2685
	.2byte	0xffff
	.4byte	.L_LC1796-.L_text_b
	.previous
	jmp .L946
	.align 4
.L944:
.L_LC1797:

.section	.line
	.4byte	2686	/ sas.c:2686
	.2byte	0xffff
	.4byte	.L_LC1797-.L_text_b
	.previous
.L_B256:
	pushl %edi
	pushl 12(%ebx)
	pushl 72(%esi)
	call repoutsb
.L_LC1798:

.section	.line
	.4byte	2687	/ sas.c:2687
	.2byte	0xffff
	.4byte	.L_LC1798-.L_text_b
	.previous
	addl %edi,12(%ebx)
.L_LC1799:

.section	.line
	.4byte	2688	/ sas.c:2688
	.2byte	0xffff
	.4byte	.L_LC1799-.L_text_b
	.previous
.L_B256_e:
	addl $12,%esp
.L946:
.L_LC1800:

.section	.line
	.4byte	2691	/ sas.c:2691
	.2byte	0xffff
	.4byte	.L_LC1800-.L_text_b
	.previous
	orb $64,32(%esi)
.L_LC1801:

.section	.line
	.4byte	2693	/ sas.c:2693
	.2byte	0xffff
	.4byte	.L_LC1801-.L_text_b
	.previous
	testb $8,33(%esi)
	je .L918
.L_LC1802:

.section	.line
	.4byte	2695	/ sas.c:2695
	.2byte	0xffff
	.4byte	.L_LC1802-.L_text_b
	.previous
	andw $63487,32(%esi)
.L_LC1803:

.section	.line
	.4byte	2696	/ sas.c:2696
	.2byte	0xffff
	.4byte	.L_LC1803-.L_text_b
	.previous
	pushl 8(%esi)
	call untimeout
.L_LC1804:

.section	.line
	.4byte	2698	/ sas.c:2698
	.2byte	0xffff
	.4byte	.L_LC1804-.L_text_b
	.previous
.L918:
.L_b21_e:
	leal -16(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
.L_f21_e:
.Lfe18:
	.size	 sas_xproc,.Lfe18-sas_xproc

.section	.debug
.L_D1182:
	.4byte	.L_D1182_e-.L_D1182
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D1202
	.2byte	0x38
	.string	"sas_xproc"
	.2byte	0x111
	.4byte	sas_xproc
	.2byte	0x121
	.4byte	.L_f21_e
	.2byte	0x8041
	.4byte	.L_b21
	.2byte	0x8051
	.4byte	.L_b21_e
.L_D1182_e:
.L_D1203:
	.4byte	.L_D1203_e-.L_D1203
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1204
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t1203_e-.L_t1203
.L_t1203:
	.byte	0x1
	.4byte	.L_T816
.L_t1203_e:
	.2byte	0x23
	.2byte	.L_l1203_e-.L_l1203
.L_l1203:
	.byte	0x1
	.4byte	0x6
.L_l1203_e:
.L_D1203_e:
.L_D1204:
	.4byte	.L_D1204_e-.L_D1204
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1205
	.2byte	0x38
	.string	"regvar"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1204_e-.L_l1204
.L_l1204:
.L_l1204_e:
.L_D1204_e:
.L_D1205:
	.4byte	.L_D1205_e-.L_D1205
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1206
	.2byte	0x38
	.string	"mp"
	.2byte	0x83
	.2byte	.L_t1205_e-.L_t1205
.L_t1205:
	.byte	0x1
	.4byte	.L_T667
.L_t1205_e:
	.2byte	0x23
	.2byte	.L_l1205_e-.L_l1205
.L_l1205:
	.byte	0x1
	.4byte	0x3
.L_l1205_e:
.L_D1205_e:
.L_D1206:
	.4byte	.L_D1206_e-.L_D1206
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1207
	.2byte	0x38
	.string	"num_to_output"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1206_e-.L_l1206
.L_l1206:
	.byte	0x1
	.4byte	0x7
.L_l1206_e:
.L_D1206_e:
.L_D1207:
	.4byte	.L_D1207_e-.L_D1207
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1208
	.2byte	0x38
	.string	"nbytes"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1207_e-.L_l1207
.L_l1207:
	.byte	0x1
	.4byte	0x1
.L_l1207_e:
.L_D1207_e:
.L_D1208:
	.4byte	.L_D1208_e-.L_D1208
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1209
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B252
	.2byte	0x121
	.4byte	.L_B252_e
.L_D1208_e:
.L_D1210:
	.4byte	.L_D1210_e-.L_D1210
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1211
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1210_e-.L_l1210
.L_l1210:
.L_l1210_e:
.L_D1210_e:
.L_D1211:
	.4byte	.L_D1211_e-.L_D1211
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1212
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1211_e-.L_l1211
.L_l1211:
.L_l1211_e:
.L_D1211_e:
.L_D1212:
	.4byte	0x4
.L_D1209:
	.4byte	.L_D1209_e-.L_D1209
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1213
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B253
	.2byte	0x121
	.4byte	.L_B253_e
.L_D1209_e:
.L_D1214:
	.4byte	.L_D1214_e-.L_D1214
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1215
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1214_e-.L_l1214
.L_l1214:
.L_l1214_e:
.L_D1214_e:
.L_D1215:
	.4byte	.L_D1215_e-.L_D1215
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1216
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1215_e-.L_l1215
.L_l1215:
.L_l1215_e:
.L_D1215_e:
.L_D1216:
	.4byte	0x4
.L_D1213:
	.4byte	.L_D1213_e-.L_D1213
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1217
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B254
	.2byte	0x121
	.4byte	.L_B254_e
.L_D1213_e:
.L_D1218:
	.4byte	.L_D1218_e-.L_D1218
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1219
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1218_e-.L_l1218
.L_l1218:
.L_l1218_e:
.L_D1218_e:
.L_D1219:
	.4byte	.L_D1219_e-.L_D1219
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1220
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1219_e-.L_l1219
.L_l1219:
.L_l1219_e:
.L_D1219_e:
.L_D1220:
	.4byte	0x4
.L_D1217:
	.4byte	.L_D1217_e-.L_D1217
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1221
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B255
	.2byte	0x121
	.4byte	.L_B255_e
.L_D1217_e:
.L_D1222:
	.4byte	.L_D1222_e-.L_D1222
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1223
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1222_e-.L_l1222
.L_l1222:
	.byte	0x1
	.4byte	0x2
.L_l1222_e:
.L_D1222_e:
.L_D1223:
	.4byte	.L_D1223_e-.L_D1223
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1224
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1223_e-.L_l1223
.L_l1223:
	.byte	0x1
	.4byte	0x0
.L_l1223_e:
.L_D1223_e:
.L_D1224:
	.4byte	0x4
.L_D1221:
	.4byte	.L_D1221_e-.L_D1221
	.2byte	0xb
	.2byte	0x12
	.4byte	.L_D1225
	.2byte	0x111
	.4byte	.L_B256
	.2byte	0x121
	.4byte	.L_B256_e
.L_D1221_e:
.L_D1226:
	.4byte	0x4
.L_D1225:
	.4byte	0x4
	.previous
	.align 4
.globl sasintr
	.type	 sasintr,@function
sasintr:
.L_LC1805:

.section	.line
	.4byte	2706	/ sas.c:2706
	.2byte	0xffff
	.4byte	.L_LC1805-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	subl $24,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
.L_b22:
.L_LC1806:

.section	.line
	.4byte	2707	/ sas.c:2707
	.2byte	0xffff
	.4byte	.L_LC1806-.L_text_b
	.previous
.L_LC1807:

.section	.line
	.4byte	2714	/ sas.c:2714
	.2byte	0xffff
	.4byte	.L_LC1807-.L_text_b
	.previous
	xorl %edi,%edi
	.align 4
.L950:
.L_LC1808:

.section	.line
	.4byte	2722	/ sas.c:2722
	.2byte	0xffff
	.4byte	.L_LC1808-.L_text_b
	.previous
	movl $1,-4(%ebp)
.L_LC1809:

.section	.line
	.4byte	2724	/ sas.c:2724
	.2byte	0xffff
	.4byte	.L_LC1809-.L_text_b
	.previous
	movl 8(%ebp),%ecx
	movl sas_first_int_user(,%ecx,4),%ebx
.L_LC1810:

.section	.line
	.4byte	2728	/ sas.c:2728
	.2byte	0xffff
	.4byte	.L_LC1810-.L_text_b
	.previous
	testl %ebx,%ebx
	je .L952
	.align 4
.L_I22_35:
.L956:
.L_LC1811:

.section	.line
	.4byte	2735	/ sas.c:2735
	.2byte	0xffff
	.4byte	.L_LC1811-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L959
.L_B258:
.L_LC1812:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1812-.L_text_b
	.previous
.L_LC1813:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1813-.L_text_b
	.previous
	movb 82(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1814:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1814-.L_text_b
	.previous
.L_B258_e:
.L_LC1815:

.section	.line
	.4byte	2736	/ sas.c:2736
	.2byte	0xffff
	.4byte	.L_LC1815-.L_text_b
	.previous
.L959:
.L_B259:
.L_LC1816:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1816-.L_text_b
	.previous
.L_LC1817:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1817-.L_text_b
	.previous
.L_B260:
.L_LC1818:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1818-.L_text_b
	.previous
	movw 80(%ebx),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1819:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1819-.L_text_b
	.previous
.L_B260_e:
.L_B259_e:
.L_LC1820:

.section	.line
	.4byte	2736	/ sas.c:2736
	.2byte	0xffff
	.4byte	.L_LC1820-.L_text_b
	.previous
	testb $1,%al
	je .L968
.L_LC1821:

.section	.line
	.4byte	2739	/ sas.c:2739
	.2byte	0xffff
	.4byte	.L_LC1821-.L_text_b
	.previous
	testb $16,33(%ebx)
	je .L955
.L_LC1822:

.section	.line
	.4byte	2741	/ sas.c:2741
	.2byte	0xffff
	.4byte	.L_LC1822-.L_text_b
	.previous
	andw $61439,32(%ebx)
.L_LC1823:

.section	.line
	.4byte	2743	/ sas.c:2743
	.2byte	0xffff
	.4byte	.L_LC1823-.L_text_b
	.previous
	testb $4,32(%ebx)
	je .L964
.L_B261:
.L_LC1824:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1824-.L_text_b
	.previous
.L_LC1825:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1825-.L_text_b
	.previous
	movb 82(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1826:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1826-.L_text_b
	.previous
.L_B261_e:
.L_LC1827:

.section	.line
	.4byte	2744	/ sas.c:2744
	.2byte	0xffff
	.4byte	.L_LC1827-.L_text_b
	.previous
.L964:
.L_B262:
.L_LC1828:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1828-.L_text_b
	.previous
.L_LC1829:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1829-.L_text_b
	.previous
	movb $-127,%al
	movw 80(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1830:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1830-.L_text_b
	.previous
.L_B262_e:
.L_LC1831:

.section	.line
	.4byte	2750	/ sas.c:2750
	.2byte	0xffff
	.4byte	.L_LC1831-.L_text_b
	.previous
	jmp .L955
	.align 4
.L_I22_133:
.L968:
.L_LC1832:

.section	.line
	.4byte	2755	/ sas.c:2755
	.2byte	0xffff
	.4byte	.L_LC1832-.L_text_b
	.previous
	testb $16,33(%ebx)
	je .L969
.L_LC1833:

.section	.line
	.4byte	2757	/ sas.c:2757
	.2byte	0xffff
	.4byte	.L_LC1833-.L_text_b
	.previous
	andw $61439,32(%ebx)
.L_LC1834:

.section	.line
	.4byte	2758	/ sas.c:2758
	.2byte	0xffff
	.4byte	.L_LC1834-.L_text_b
	.previous
	testb $4,32(%ebx)
	je .L971
.L_B263:
.L_LC1835:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1835-.L_text_b
	.previous
.L_LC1836:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1836-.L_text_b
	.previous
	movb 82(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1837:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1837-.L_text_b
	.previous
.L_B263_e:
.L_LC1838:

.section	.line
	.4byte	2759	/ sas.c:2759
	.2byte	0xffff
	.4byte	.L_LC1838-.L_text_b
	.previous
.L971:
.L_B264:
.L_LC1839:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1839-.L_text_b
	.previous
.L_LC1840:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1840-.L_text_b
	.previous
	movb $-127,%al
	movw 80(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1841:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1841-.L_text_b
	.previous
.L_B264_e:
.L969:
.L_LC1842:

.section	.line
	.4byte	2762	/ sas.c:2762
	.2byte	0xffff
	.4byte	.L_LC1842-.L_text_b
	.previous
	movl $0,-4(%ebp)
.L_LC1843:

.section	.line
	.4byte	2763	/ sas.c:2763
	.2byte	0xffff
	.4byte	.L_LC1843-.L_text_b
	.previous
	movl 104(%ebx),%esi
	.align 4
.L974:
.L_LC1844:

.section	.line
	.4byte	2768	/ sas.c:2768
	.2byte	0xffff
	.4byte	.L_LC1844-.L_text_b
	.previous
	testb $4,32(%ebx)
	je .L979
.L_B265:
.L_LC1845:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1845-.L_text_b
	.previous
.L_LC1846:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1846-.L_text_b
	.previous
	movb 94(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1847:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1847-.L_text_b
	.previous
.L_B265_e:
.L_LC1848:

.section	.line
	.4byte	2769	/ sas.c:2769
	.2byte	0xffff
	.4byte	.L_LC1848-.L_text_b
	.previous
.L979:
.L_B266:
.L_LC1849:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1849-.L_text_b
	.previous
.L_LC1850:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1850-.L_text_b
	.previous
.L_B267:
.L_LC1851:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1851-.L_text_b
	.previous
	movw 92(%ebx),%dx
/APP
	inb %dx,%eax
/NO_APP
	movl %eax,-24(%ebp)
.L_LC1852:

.section	.line
	.4byte	113	/ sas.c:113
	.2byte	0xffff
	.4byte	.L_LC1852-.L_text_b
	.previous
	movb -24(%ebp),%cl
	movb %cl,-16(%ebp)
.L_LC1853:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1853-.L_text_b
	.previous
.L_B267_e:
.L_B266_e:
.L_LC1854:

.section	.line
	.4byte	2769	/ sas.c:2769
	.2byte	0xffff
	.4byte	.L_LC1854-.L_text_b
	.previous
	movzbl -16(%ebp),%eax
	movl %eax,-12(%ebp)
	testb $31,%al
	je .L977
.L_LC1855:

.section	.line
	.4byte	2771	/ sas.c:2771
	.2byte	0xffff
	.4byte	.L_LC1855-.L_text_b
	.previous
	testl %edi,%edi
	jne .L982
	cmpb $2,66(%ebx)
	jne .L982
.L_LC1856:

.section	.line
	.4byte	2781	/ sas.c:2781
	.2byte	0xffff
	.4byte	.L_LC1856-.L_text_b
	.previous
	movl %ebx,%edi
.L_LC1857:

.section	.line
	.4byte	2782	/ sas.c:2782
	.2byte	0xffff
	.4byte	.L_LC1857-.L_text_b
	.previous
	movl 8(%ebp),%edx
	movl sas_first_int_user(,%edx,4),%ebx
.L_LC1858:

.section	.line
	.4byte	2783	/ sas.c:2783
	.2byte	0xffff
	.4byte	.L_LC1858-.L_text_b
	.previous
	testl %ebx,%ebx
	je .L984
	.align 4
.L991:
.L_LC1859:

.section	.line
	.4byte	2785	/ sas.c:2785
	.2byte	0xffff
	.4byte	.L_LC1859-.L_text_b
	.previous
	cmpb $2,66(%ebx)
	jne .L985
	cmpl %edi,%ebx
	je .L985
.L_LC1860:

.section	.line
	.4byte	2789	/ sas.c:2789
	.2byte	0xffff
	.4byte	.L_LC1860-.L_text_b
	.previous
	orw $4096,32(%ebx)
.L_LC1861:

.section	.line
	.4byte	2791	/ sas.c:2791
	.2byte	0xffff
	.4byte	.L_LC1861-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L988
.L_B268:
.L_LC1862:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1862-.L_text_b
	.previous
.L_LC1863:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1863-.L_text_b
	.previous
	movb 82(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1864:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1864-.L_text_b
	.previous
.L_B268_e:
.L_LC1865:

.section	.line
	.4byte	2793	/ sas.c:2793
	.2byte	0xffff
	.4byte	.L_LC1865-.L_text_b
	.previous
.L988:
.L_B269:
.L_LC1866:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1866-.L_text_b
	.previous
.L_LC1867:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1867-.L_text_b
	.previous
	movb $1,%al
	movw 80(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1868:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1868-.L_text_b
	.previous
.L_B269_e:
.L_LC1869:

.section	.line
	.4byte	2783	/ sas.c:2783
	.2byte	0xffff
	.4byte	.L_LC1869-.L_text_b
	.previous
.L985:
	movl 4(%ebx),%ebx
	testl %ebx,%ebx
	jne .L991
.L984:
.L_LC1870:

.section	.line
	.4byte	2796	/ sas.c:2796
	.2byte	0xffff
	.4byte	.L_LC1870-.L_text_b
	.previous
	movl %edi,%ebx
.L_LC1871:

.section	.line
	.4byte	2797	/ sas.c:2797
	.2byte	0xffff
	.4byte	.L_LC1871-.L_text_b
	.previous
	movl $1,%edi
.L982:
.L_LC1872:

.section	.line
	.4byte	2799	/ sas.c:2799
	.2byte	0xffff
	.4byte	.L_LC1872-.L_text_b
	.previous
	pushl -12(%ebp)
	pushl %ebx
	call sas_rproc
	movl %eax,-12(%ebp)
.L_LC1873:

.section	.line
	.4byte	2800	/ sas.c:2800
	.2byte	0xffff
	.4byte	.L_LC1873-.L_text_b
	.previous
	incl sysinfo+132
.L_LC1874:

.section	.line
	.4byte	2801	/ sas.c:2801
	.2byte	0xffff
	.4byte	.L_LC1874-.L_text_b
	.previous
	addl $8,%esp
.L977:
.L_LC1875:

.section	.line
	.4byte	2804	/ sas.c:2804
	.2byte	0xffff
	.4byte	.L_LC1875-.L_text_b
	.previous
	movl -12(%ebp),%ecx
	testb $32,%cl
	je .L992
	testb $64,32(%ebx)
	je .L992
.L_LC1876:

.section	.line
	.4byte	2807	/ sas.c:2807
	.2byte	0xffff
	.4byte	.L_LC1876-.L_text_b
	.previous
	andb $191,32(%ebx)
.L_LC1877:

.section	.line
	.4byte	2808	/ sas.c:2808
	.2byte	0xffff
	.4byte	.L_LC1877-.L_text_b
	.previous
	pushl %ebx
	call sas_xproc
.L_LC1878:

.section	.line
	.4byte	2809	/ sas.c:2809
	.2byte	0xffff
	.4byte	.L_LC1878-.L_text_b
	.previous
	addl $4,%esp
	testb $64,32(%ebx)
	jne .L993
.L_LC1879:

.section	.line
	.4byte	2812	/ sas.c:2812
	.2byte	0xffff
	.4byte	.L_LC1879-.L_text_b
	.previous
	orw $2048,32(%ebx)
.L_LC1880:

.section	.line
	.4byte	2814	/ sas.c:2814
	.2byte	0xffff
	.4byte	.L_LC1880-.L_text_b
	.previous
	movl 20(%ebx),%eax
	andl $15,%eax
	pushl sas_ctimes(,%eax,4)
	pushl %ebx
	pushl $sas_timeout
	call timeout
	movl %eax,8(%ebx)
.L_LC1881:

.section	.line
	.4byte	2819	/ sas.c:2819
	.2byte	0xffff
	.4byte	.L_LC1881-.L_text_b
	.previous
	addl $12,%esp
.L993:
.L_LC1882:

.section	.line
	.4byte	2820	/ sas.c:2820
	.2byte	0xffff
	.4byte	.L_LC1882-.L_text_b
	.previous
	incl sysinfo+136
.L992:
.L_LC1883:

.section	.line
	.4byte	2826	/ sas.c:2826
	.2byte	0xffff
	.4byte	.L_LC1883-.L_text_b
	.previous
	testb $32,33(%ebx)
	je .L976
	testb $4,32(%ebx)
	je .L996
.L_B270:
.L_LC1884:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1884-.L_text_b
	.previous
.L_LC1885:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1885-.L_text_b
	.previous
	movb 98(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1886:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1886-.L_text_b
	.previous
.L_B270_e:
.L_LC1887:

.section	.line
	.4byte	2829	/ sas.c:2829
	.2byte	0xffff
	.4byte	.L_LC1887-.L_text_b
	.previous
.L996:
.L_B271:
.L_LC1888:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1888-.L_text_b
	.previous
.L_LC1889:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1889-.L_text_b
	.previous
.L_B272:
.L_LC1890:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1890-.L_text_b
	.previous
	movw 96(%ebx),%dx
/APP
	inb %dx,%eax
/NO_APP
	movl %eax,-24(%ebp)
.L_LC1891:

.section	.line
	.4byte	113	/ sas.c:113
	.2byte	0xffff
	.4byte	.L_LC1891-.L_text_b
	.previous
	movb -24(%ebp),%cl
	movb %cl,-16(%ebp)
.L_LC1892:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1892-.L_text_b
	.previous
.L_B272_e:
.L_B271_e:
.L_LC1893:

.section	.line
	.4byte	2829	/ sas.c:2829
	.2byte	0xffff
	.4byte	.L_LC1893-.L_text_b
	.previous
	movzbl -16(%ebp),%eax
	movl %eax,-12(%ebp)
	testb $15,%al
	je .L976
.L_LC1894:

.section	.line
	.4byte	2839	/ sas.c:2839
	.2byte	0xffff
	.4byte	.L_LC1894-.L_text_b
	.previous
	movb -12(%ebp),%al
	andb 61(%ebx),%al
	testb $15,%al
	je .L999
.L_LC1895:

.section	.line
	.4byte	2842	/ sas.c:2842
	.2byte	0xffff
	.4byte	.L_LC1895-.L_text_b
	.previous
	pushl %ebx
	call sas_msi_disable
.L_LC1896:

.section	.line
	.4byte	2843	/ sas.c:2843
	.2byte	0xffff
	.4byte	.L_LC1896-.L_text_b
	.previous
	incl sas_msi_noise
.L_LC1897:

.section	.line
	.4byte	2844	/ sas.c:2844
	.2byte	0xffff
	.4byte	.L_LC1897-.L_text_b
	.previous
	addl $4,%esp
.L999:
.L_LC1898:

.section	.line
	.4byte	2849	/ sas.c:2849
	.2byte	0xffff
	.4byte	.L_LC1898-.L_text_b
	.previous
	movl -12(%ebp),%edx
	andb $191,%dl
	movb 61(%ebx),%cl
	andl $64,%ecx
	orl %ecx,%edx
	movl %edx,-12(%ebp)
.L_LC1899:

.section	.line
	.4byte	2852	/ sas.c:2852
	.2byte	0xffff
	.4byte	.L_LC1899-.L_text_b
	.previous
	testb $4,%dl
	je .L1000
.L_LC1900:

.section	.line
	.4byte	2853	/ sas.c:2853
	.2byte	0xffff
	.4byte	.L_LC1900-.L_text_b
	.previous
	orb $64,%dl
	movl %edx,-12(%ebp)
.L1000:
.L_LC1901:

.section	.line
	.4byte	2854	/ sas.c:2854
	.2byte	0xffff
	.4byte	.L_LC1901-.L_text_b
	.previous
	movb -12(%ebp),%al
	xorb 61(%ebx),%al
	testb $240,%al
	je .L1001
.L_LC1902:

.section	.line
	.4byte	2858	/ sas.c:2858
	.2byte	0xffff
	.4byte	.L_LC1902-.L_text_b
	.previous
	pushl -12(%ebp)
	pushl %ebx
	call sas_fproc
.L_LC1903:

.section	.line
	.4byte	2859	/ sas.c:2859
	.2byte	0xffff
	.4byte	.L_LC1903-.L_text_b
	.previous
	orb $8,40(%ebx)
	addl $8,%esp
.L_LC1904:

.section	.line
	.4byte	2860	/ sas.c:2860
	.2byte	0xffff
	.4byte	.L_LC1904-.L_text_b
	.previous
	jmp .L1022
	.align 4
.L1001:
.L_LC1905:

.section	.line
	.4byte	2862	/ sas.c:2862
	.2byte	0xffff
	.4byte	.L_LC1905-.L_text_b
	.previous
	orb $32,40(%ebx)
.L_LC1906:

.section	.line
	.4byte	2863	/ sas.c:2863
	.2byte	0xffff
	.4byte	.L_LC1906-.L_text_b
	.previous
.L1022:
	cmpl $0,event_scheduled
	jne .L1003
	movl $1,event_scheduled
	pushl $8
	pushl $0
	pushl $sas_event
	call timeout
	addl $12,%esp
.L1003:
.L_LC1907:

.section	.line
	.4byte	2867	/ sas.c:2867
	.2byte	0xffff
	.4byte	.L_LC1907-.L_text_b
	.previous
	movb 61(%ebx),%al
	andb $15,%al
	orb -12(%ebp),%al
	movb %al,61(%ebx)
.L_LC1908:

.section	.line
	.4byte	2870	/ sas.c:2870
	.2byte	0xffff
	.4byte	.L_LC1908-.L_text_b
	.previous
	incl sysinfo+140
.L976:
.L_LC1909:

.section	.line
	.4byte	2873	/ sas.c:2873
	.2byte	0xffff
	.4byte	.L_LC1909-.L_text_b
	.previous
	testb $4,32(%ebx)
	je .L1006
.L_B273:
.L_LC1910:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1910-.L_text_b
	.previous
.L_LC1911:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1911-.L_text_b
	.previous
	movb 82(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1912:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1912-.L_text_b
	.previous
.L_B273_e:
.L_LC1913:

.section	.line
	.4byte	2873	/ sas.c:2873
	.2byte	0xffff
	.4byte	.L_LC1913-.L_text_b
	.previous
.L1006:
.L_B274:
.L_LC1914:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1914-.L_text_b
	.previous
.L_LC1915:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1915-.L_text_b
	.previous
.L_B275:
.L_LC1916:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1916-.L_text_b
	.previous
	movw 80(%ebx),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1917:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1917-.L_text_b
	.previous
.L_B275_e:
.L_B274_e:
.L_LC1918:

.section	.line
	.4byte	2873	/ sas.c:2873
	.2byte	0xffff
	.4byte	.L_LC1918-.L_text_b
	.previous
	testb $1,%al
	je .L974
.L_LC1919:

.section	.line
	.4byte	2877	/ sas.c:2877
	.2byte	0xffff
	.4byte	.L_LC1919-.L_text_b
	.previous
	cmpl %esi,104(%ebx)
	je .L1009
.L_LC1920:

.section	.line
	.4byte	2880	/ sas.c:2880
	.2byte	0xffff
	.4byte	.L_LC1920-.L_text_b
	.previous
	testl %esi,%esi
	jne .L1009
	testb $2,37(%ebx)
	jne .L1009
.L_LC1921:

.section	.line
	.4byte	2882	/ sas.c:2882
	.2byte	0xffff
	.4byte	.L_LC1921-.L_text_b
	.previous
	orb $1,40(%ebx)
	cmpl $0,event_scheduled
	jne .L1009
	movl $1,event_scheduled
	pushl $8
	pushl $0
	pushl $sas_event
	call timeout
	addl $12,%esp
.L1009:
.L_LC1922:

.section	.line
	.4byte	2887	/ sas.c:2887
	.2byte	0xffff
	.4byte	.L_LC1922-.L_text_b
	.previous
	cmpl $0,168(%ebx)
	jne .L955
	cmpl $0,176(%ebx)
	je .L955
.L_LC1923:

.section	.line
	.4byte	2889	/ sas.c:2889
	.2byte	0xffff
	.4byte	.L_LC1923-.L_text_b
	.previous
	movl 36(%ebx),%eax
	andl $18,%eax
	cmpl $2,%eax
	jne .L1013
.L_LC1924:

.section	.line
	.4byte	2891	/ sas.c:2891
	.2byte	0xffff
	.4byte	.L_LC1924-.L_text_b
	.previous
	movb 56(%ebx),%cl
	notb %cl
	andb 62(%ebx),%cl
	movb %cl,-24(%ebp)
	movb %cl,62(%ebx)
.L_LC1925:

.section	.line
	.4byte	2892	/ sas.c:2892
	.2byte	0xffff
	.4byte	.L_LC1925-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L1015
.L_B276:
.L_LC1926:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1926-.L_text_b
	.previous
.L_LC1927:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1927-.L_text_b
	.previous
	movb 90(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1928:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1928-.L_text_b
	.previous
.L_B276_e:
.L_LC1929:

.section	.line
	.4byte	2892	/ sas.c:2892
	.2byte	0xffff
	.4byte	.L_LC1929-.L_text_b
	.previous
.L1015:
.L_B277:
.L_LC1930:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1930-.L_text_b
	.previous
.L_LC1931:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1931-.L_text_b
	.previous
	movb -24(%ebp),%al
	movw 88(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1932:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1932-.L_text_b
	.previous
.L_B277_e:
.L_LC1933:

.section	.line
	.4byte	2893	/ sas.c:2893
	.2byte	0xffff
	.4byte	.L_LC1933-.L_text_b
	.previous
	orb $16,36(%ebx)
.L1013:
.L_LC1934:

.section	.line
	.4byte	2895	/ sas.c:2895
	.2byte	0xffff
	.4byte	.L_LC1934-.L_text_b
	.previous
	testb $16,13(%ebx)
	je .L955
	cmpb $0,36(%ebx)
	jl .L955
.L_LC1935:

.section	.line
	.4byte	2897	/ sas.c:2897
	.2byte	0xffff
	.4byte	.L_LC1935-.L_text_b
	.previous
	orw $384,36(%ebx)
.L_LC1936:

.section	.line
	.4byte	2898	/ sas.c:2898
	.2byte	0xffff
	.4byte	.L_LC1936-.L_text_b
	.previous
	movl 112(%ebx),%eax
	orl $32800,(%eax)
.L_LC1937:

.section	.line
	.4byte	2899	/ sas.c:2899
	.2byte	0xffff
	.4byte	.L_LC1937-.L_text_b
	.previous
	pushl %ebx
	call sas_hdx_check
.L_LC1938:

.section	.line
	.4byte	2900	/ sas.c:2900
	.2byte	0xffff
	.4byte	.L_LC1938-.L_text_b
	.previous
	pushl %ebx
	call sas_xproc
.L_LC1939:

.section	.line
	.4byte	2901	/ sas.c:2901
	.2byte	0xffff
	.4byte	.L_LC1939-.L_text_b
	.previous
	addl $8,%esp
.L_LC1940:

.section	.line
	.4byte	2729	/ sas.c:2729
	.2byte	0xffff
	.4byte	.L_LC1940-.L_text_b
	.previous
.L955:
	movl 4(%ebx),%ebx
	testl %ebx,%ebx
	jne .L956
.L_LC1941:

.section	.line
	.4byte	2904	/ sas.c:2904
	.2byte	0xffff
	.4byte	.L_LC1941-.L_text_b
	.previous
.L952:
	cmpl $0,-4(%ebp)
	je .L950
.L_LC1942:

.section	.line
	.4byte	2909	/ sas.c:2909
	.2byte	0xffff
	.4byte	.L_LC1942-.L_text_b
	.previous
	movl 8(%ebp),%ecx
	movl sas_int_ack_port(,%ecx,4),%ebx
	testl %ebx,%ebx
	je .L1020
.L_LC1943:

.section	.line
	.4byte	2910	/ sas.c:2910
	.2byte	0xffff
	.4byte	.L_LC1943-.L_text_b
	.previous
.L_B278:
.L_LC1944:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1944-.L_text_b
	.previous
.L_LC1945:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1945-.L_text_b
	.previous
	movb sas_int_ack(,%ecx,4),%al
	movl %ebx,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC1946:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1946-.L_text_b
	.previous
.L_B278_e:
.L1020:
.L_LC1947:

.section	.line
	.4byte	2912	/ sas.c:2912
	.2byte	0xffff
	.4byte	.L_LC1947-.L_text_b
	.previous
	xorl %eax,%eax
.L_LC1948:

.section	.line
	.4byte	2913	/ sas.c:2913
	.2byte	0xffff
	.4byte	.L_LC1948-.L_text_b
	.previous
.L_b22_e:
	leal -36(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
.L_f22_e:
.Lfe19:
	.size	 sasintr,.Lfe19-sasintr

.section	.debug_pubnames
	.4byte	.L_P8
	.string	"sasintr"
	.previous

.section	.debug
.L_P8:
.L_D1202:
	.4byte	.L_D1202_e-.L_D1202
	.2byte	0x6
	.2byte	0x12
	.4byte	.L_D1227
	.2byte	0x38
	.string	"sasintr"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x111
	.4byte	sasintr
	.2byte	0x121
	.4byte	.L_f22_e
	.2byte	0x8041
	.4byte	.L_b22
	.2byte	0x8051
	.4byte	.L_b22_e
.L_D1202_e:
.L_D1228:
	.4byte	.L_D1228_e-.L_D1228
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1229
	.2byte	0x38
	.string	"vect"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1228_e-.L_l1228
.L_l1228:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l1228_e:
.L_D1228_e:
.L_D1229:
	.4byte	.L_D1229_e-.L_D1229
	.2byte	0xa
	.2byte	0x12
	.4byte	.L_D1230
	.2byte	0x38
	.string	"fastloop"
	.2byte	0x111
	.4byte	.L_I22_35
.L_D1229_e:
.L_D1230:
	.4byte	.L_D1230_e-.L_D1230
	.2byte	0xa
	.2byte	0x12
	.4byte	.L_D1231
	.2byte	0x38
	.string	"goit"
	.2byte	0x111
	.4byte	.L_I22_133
.L_D1230_e:
.L_D1231:
	.4byte	.L_D1231_e-.L_D1231
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1232
	.2byte	0x38
	.string	"regvar"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1231_e-.L_l1231
.L_l1231:
.L_l1231_e:
.L_D1231_e:
.L_D1232:
	.4byte	.L_D1232_e-.L_D1232
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1233
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t1232_e-.L_t1232
.L_t1232:
	.byte	0x1
	.4byte	.L_T816
.L_t1232_e:
	.2byte	0x23
	.2byte	.L_l1232_e-.L_l1232
.L_l1232:
	.byte	0x1
	.4byte	0x3
.L_l1232_e:
.L_D1232_e:
.L_D1233:
	.4byte	.L_D1233_e-.L_D1233
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1234
	.2byte	0x38
	.string	"status"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1233_e-.L_l1233
.L_l1233:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xfffffff4
	.byte	0x7
.L_l1233_e:
.L_D1233_e:
.L_D1234:
	.4byte	.L_D1234_e-.L_D1234
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1235
	.2byte	0x38
	.string	"old_fip"
	.2byte	0x83
	.2byte	.L_t1234_e-.L_t1234
.L_t1234:
	.byte	0x1
	.4byte	.L_T816
.L_t1234_e:
	.2byte	0x23
	.2byte	.L_l1234_e-.L_l1234
.L_l1234:
	.byte	0x1
	.4byte	0x7
.L_l1234_e:
.L_D1234_e:
.L_D1235:
	.4byte	.L_D1235_e-.L_D1235
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1236
	.2byte	0x38
	.string	"done"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1235_e-.L_l1235
.L_l1235:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xfffffffc
	.byte	0x7
.L_l1235_e:
.L_D1235_e:
.L_D1236:
	.4byte	.L_D1236_e-.L_D1236
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1237
	.2byte	0x38
	.string	"drop_mode"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1236_e-.L_l1236
.L_l1236:
	.byte	0x1
	.4byte	0x7
.L_l1236_e:
.L_D1236_e:
.L_D1237:
	.4byte	.L_D1237_e-.L_D1237
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1238
	.2byte	0x38
	.string	"port"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1237_e-.L_l1237
.L_l1237:
	.byte	0x1
	.4byte	0x3
.L_l1237_e:
.L_D1237_e:
.L_D1238:
	.4byte	.L_D1238_e-.L_D1238
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1239
	.2byte	0x38
	.string	"old_recv_count"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1238_e-.L_l1238
.L_l1238:
	.byte	0x1
	.4byte	0x6
.L_l1238_e:
.L_D1238_e:
.L_D1239:
	.4byte	.L_D1239_e-.L_D1239
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1240
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B258
	.2byte	0x121
	.4byte	.L_B258_e
.L_D1239_e:
.L_D1241:
	.4byte	.L_D1241_e-.L_D1241
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1242
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1241_e-.L_l1241
.L_l1241:
.L_l1241_e:
.L_D1241_e:
.L_D1242:
	.4byte	.L_D1242_e-.L_D1242
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1243
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1242_e-.L_l1242
.L_l1242:
.L_l1242_e:
.L_D1242_e:
.L_D1243:
	.4byte	0x4
.L_D1240:
	.4byte	.L_D1240_e-.L_D1240
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1244
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B259
	.2byte	0x121
	.4byte	.L_B259_e
.L_D1240_e:
.L_D1245:
	.4byte	.L_D1245_e-.L_D1245
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1246
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1245_e-.L_l1245
.L_l1245:
.L_l1245_e:
.L_D1245_e:
.L_D1246:
	.4byte	.L_D1246_e-.L_D1246
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1247
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1246_e-.L_l1246
.L_l1246:
	.byte	0x1
	.4byte	0x0
.L_l1246_e:
.L_D1246_e:
.L_D1247:
	.4byte	0x4
.L_D1244:
	.4byte	.L_D1244_e-.L_D1244
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1248
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B261
	.2byte	0x121
	.4byte	.L_B261_e
.L_D1244_e:
.L_D1249:
	.4byte	.L_D1249_e-.L_D1249
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1250
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1249_e-.L_l1249
.L_l1249:
.L_l1249_e:
.L_D1249_e:
.L_D1250:
	.4byte	.L_D1250_e-.L_D1250
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1251
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1250_e-.L_l1250
.L_l1250:
.L_l1250_e:
.L_D1250_e:
.L_D1251:
	.4byte	0x4
.L_D1248:
	.4byte	.L_D1248_e-.L_D1248
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1252
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B262
	.2byte	0x121
	.4byte	.L_B262_e
.L_D1248_e:
.L_D1253:
	.4byte	.L_D1253_e-.L_D1253
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1254
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1253_e-.L_l1253
.L_l1253:
.L_l1253_e:
.L_D1253_e:
.L_D1254:
	.4byte	.L_D1254_e-.L_D1254
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1255
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l1254_e-.L_l1254
.L_l1254:
	.4byte	0xffffff81
.L_l1254_e:
.L_D1254_e:
.L_D1255:
	.4byte	0x4
.L_D1252:
	.4byte	.L_D1252_e-.L_D1252
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1256
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B263
	.2byte	0x121
	.4byte	.L_B263_e
.L_D1252_e:
.L_D1257:
	.4byte	.L_D1257_e-.L_D1257
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1258
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1257_e-.L_l1257
.L_l1257:
.L_l1257_e:
.L_D1257_e:
.L_D1258:
	.4byte	.L_D1258_e-.L_D1258
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1259
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1258_e-.L_l1258
.L_l1258:
.L_l1258_e:
.L_D1258_e:
.L_D1259:
	.4byte	0x4
.L_D1256:
	.4byte	.L_D1256_e-.L_D1256
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1260
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B264
	.2byte	0x121
	.4byte	.L_B264_e
.L_D1256_e:
.L_D1261:
	.4byte	.L_D1261_e-.L_D1261
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1262
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1261_e-.L_l1261
.L_l1261:
.L_l1261_e:
.L_D1261_e:
.L_D1262:
	.4byte	.L_D1262_e-.L_D1262
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1263
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l1262_e-.L_l1262
.L_l1262:
	.4byte	0xffffff81
.L_l1262_e:
.L_D1262_e:
.L_D1263:
	.4byte	0x4
.L_D1260:
	.4byte	.L_D1260_e-.L_D1260
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1264
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B265
	.2byte	0x121
	.4byte	.L_B265_e
.L_D1260_e:
.L_D1265:
	.4byte	.L_D1265_e-.L_D1265
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1266
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1265_e-.L_l1265
.L_l1265:
.L_l1265_e:
.L_D1265_e:
.L_D1266:
	.4byte	.L_D1266_e-.L_D1266
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1267
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1266_e-.L_l1266
.L_l1266:
.L_l1266_e:
.L_D1266_e:
.L_D1267:
	.4byte	0x4
.L_D1264:
	.4byte	.L_D1264_e-.L_D1264
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1268
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B266
	.2byte	0x121
	.4byte	.L_B266_e
.L_D1264_e:
.L_D1269:
	.4byte	.L_D1269_e-.L_D1269
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1270
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1269_e-.L_l1269
.L_l1269:
.L_l1269_e:
.L_D1269_e:
.L_D1270:
	.4byte	.L_D1270_e-.L_D1270
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1271
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1270_e-.L_l1270
.L_l1270:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffffe8
	.byte	0x7
.L_l1270_e:
.L_D1270_e:
.L_D1271:
	.4byte	0x4
.L_D1268:
	.4byte	.L_D1268_e-.L_D1268
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1272
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B268
	.2byte	0x121
	.4byte	.L_B268_e
.L_D1268_e:
.L_D1273:
	.4byte	.L_D1273_e-.L_D1273
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1274
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1273_e-.L_l1273
.L_l1273:
.L_l1273_e:
.L_D1273_e:
.L_D1274:
	.4byte	.L_D1274_e-.L_D1274
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1275
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1274_e-.L_l1274
.L_l1274:
.L_l1274_e:
.L_D1274_e:
.L_D1275:
	.4byte	0x4
.L_D1272:
	.4byte	.L_D1272_e-.L_D1272
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1276
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B269
	.2byte	0x121
	.4byte	.L_B269_e
.L_D1272_e:
.L_D1277:
	.4byte	.L_D1277_e-.L_D1277
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1278
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1277_e-.L_l1277
.L_l1277:
.L_l1277_e:
.L_D1277_e:
.L_D1278:
	.4byte	.L_D1278_e-.L_D1278
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1279
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l1278_e-.L_l1278
.L_l1278:
	.4byte	0x1
.L_l1278_e:
.L_D1278_e:
.L_D1279:
	.4byte	0x4
.L_D1276:
	.4byte	.L_D1276_e-.L_D1276
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1280
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B270
	.2byte	0x121
	.4byte	.L_B270_e
.L_D1276_e:
.L_D1281:
	.4byte	.L_D1281_e-.L_D1281
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1282
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1281_e-.L_l1281
.L_l1281:
.L_l1281_e:
.L_D1281_e:
.L_D1282:
	.4byte	.L_D1282_e-.L_D1282
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1283
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1282_e-.L_l1282
.L_l1282:
.L_l1282_e:
.L_D1282_e:
.L_D1283:
	.4byte	0x4
.L_D1280:
	.4byte	.L_D1280_e-.L_D1280
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1284
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B271
	.2byte	0x121
	.4byte	.L_B271_e
.L_D1280_e:
.L_D1285:
	.4byte	.L_D1285_e-.L_D1285
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1286
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1285_e-.L_l1285
.L_l1285:
.L_l1285_e:
.L_D1285_e:
.L_D1286:
	.4byte	.L_D1286_e-.L_D1286
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1287
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1286_e-.L_l1286
.L_l1286:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xffffffe8
	.byte	0x7
.L_l1286_e:
.L_D1286_e:
.L_D1287:
	.4byte	0x4
.L_D1284:
	.4byte	.L_D1284_e-.L_D1284
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1288
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B273
	.2byte	0x121
	.4byte	.L_B273_e
.L_D1284_e:
.L_D1289:
	.4byte	.L_D1289_e-.L_D1289
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1290
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1289_e-.L_l1289
.L_l1289:
.L_l1289_e:
.L_D1289_e:
.L_D1290:
	.4byte	.L_D1290_e-.L_D1290
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1291
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1290_e-.L_l1290
.L_l1290:
.L_l1290_e:
.L_D1290_e:
.L_D1291:
	.4byte	0x4
.L_D1288:
	.4byte	.L_D1288_e-.L_D1288
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1292
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B274
	.2byte	0x121
	.4byte	.L_B274_e
.L_D1288_e:
.L_D1293:
	.4byte	.L_D1293_e-.L_D1293
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1294
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1293_e-.L_l1293
.L_l1293:
.L_l1293_e:
.L_D1293_e:
.L_D1294:
	.4byte	.L_D1294_e-.L_D1294
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1295
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1294_e-.L_l1294
.L_l1294:
	.byte	0x1
	.4byte	0x0
.L_l1294_e:
.L_D1294_e:
.L_D1295:
	.4byte	0x4
.L_D1292:
	.4byte	.L_D1292_e-.L_D1292
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1296
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B276
	.2byte	0x121
	.4byte	.L_B276_e
.L_D1292_e:
.L_D1297:
	.4byte	.L_D1297_e-.L_D1297
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1298
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1297_e-.L_l1297
.L_l1297:
.L_l1297_e:
.L_D1297_e:
.L_D1298:
	.4byte	.L_D1298_e-.L_D1298
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1299
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1298_e-.L_l1298
.L_l1298:
.L_l1298_e:
.L_D1298_e:
.L_D1299:
	.4byte	0x4
.L_D1296:
	.4byte	.L_D1296_e-.L_D1296
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1300
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B277
	.2byte	0x121
	.4byte	.L_B277_e
.L_D1296_e:
.L_D1301:
	.4byte	.L_D1301_e-.L_D1301
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1302
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1301_e-.L_l1301
.L_l1301:
.L_l1301_e:
.L_D1301_e:
.L_D1302:
	.4byte	.L_D1302_e-.L_D1302
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1303
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1302_e-.L_l1302
.L_l1302:
.L_l1302_e:
.L_D1302_e:
.L_D1303:
	.4byte	0x4
.L_D1300:
	.4byte	.L_D1300_e-.L_D1300
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1304
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B278
	.2byte	0x121
	.4byte	.L_B278_e
.L_D1300_e:
.L_D1305:
	.4byte	.L_D1305_e-.L_D1305
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1306
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1305_e-.L_l1305
.L_l1305:
.L_l1305_e:
.L_D1305_e:
.L_D1306:
	.4byte	.L_D1306_e-.L_D1306
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1307
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1306_e-.L_l1306
.L_l1306:
.L_l1306_e:
.L_D1306_e:
.L_D1307:
	.4byte	0x4
.L_D1304:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_rproc,@function
sas_rproc:
.L_LC1949:

.section	.line
	.4byte	2922	/ sas.c:2922
	.2byte	0xffff
	.4byte	.L_LC1949-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	subl $20,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 8(%ebp),%esi
	movl 12(%ebp),%ecx
.L_b23:
.L_LC1950:

.section	.line
	.4byte	2923	/ sas.c:2923
	.2byte	0xffff
	.4byte	.L_LC1950-.L_text_b
	.previous
.L_LC1951:

.section	.line
	.4byte	2924	/ sas.c:2924
	.2byte	0xffff
	.4byte	.L_LC1951-.L_text_b
	.previous
	movl 112(%esi),%eax
	movl %eax,-8(%ebp)
.L_LC1952:

.section	.line
	.4byte	2929	/ sas.c:2929
	.2byte	0xffff
	.4byte	.L_LC1952-.L_text_b
	.previous
	testb $2,32(%esi)
	je .L1027
.L_B280:
.L_LC1953:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1953-.L_text_b
	.previous
.L_LC1954:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1954-.L_text_b
	.previous
	movb 74(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1955:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1955-.L_text_b
	.previous
.L_B280_e:
	.align 4
.L1027:
.L_LC1956:

.section	.line
	.4byte	2936	/ sas.c:2936
	.2byte	0xffff
	.4byte	.L_LC1956-.L_text_b
	.previous
	testb $1,%cl
	je .L1030
	testb $4,32(%esi)
	je .L1033
.L_B281:
.L_LC1957:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1957-.L_text_b
	.previous
.L_LC1958:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1958-.L_text_b
	.previous
	movb 74(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1959:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1959-.L_text_b
	.previous
.L_B281_e:
.L_LC1960:

.section	.line
	.4byte	2938	/ sas.c:2938
	.2byte	0xffff
	.4byte	.L_LC1960-.L_text_b
	.previous
.L1033:
.L_B282:
.L_LC1961:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC1961-.L_text_b
	.previous
.L_LC1962:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC1962-.L_text_b
	.previous
.L_B283:
.L_LC1963:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC1963-.L_text_b
	.previous
	movw 72(%esi),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC1964:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC1964-.L_text_b
	.previous
.L_B283_e:
.L_B282_e:
.L_LC1965:

.section	.line
	.4byte	2938	/ sas.c:2938
	.2byte	0xffff
	.4byte	.L_LC1965-.L_text_b
	.previous
	movzbl %al,%edi
	jmp .L1031
	.align 4
.L1030:
	xorl %edi,%edi
.L1031:
.L_LC1966:

.section	.line
	.4byte	2943	/ sas.c:2943
	.2byte	0xffff
	.4byte	.L_LC1966-.L_text_b
	.previous
	testb $2,%cl
	je .L1036
.L_LC1967:

.section	.line
	.4byte	2944	/ sas.c:2944
	.2byte	0xffff
	.4byte	.L_LC1967-.L_text_b
	.previous
	movzbl 66(%esi),%eax
	incl sas_overrun(,%eax,4)
.L1036:
.L_LC1968:

.section	.line
	.4byte	2947	/ sas.c:2947
	.2byte	0xffff
	.4byte	.L_LC1968-.L_text_b
	.previous
	cmpb $0,20(%esi)
	jge .L1038
	movl -8(%ebp),%eax
	movl (%eax),%edx
	andl $20,%edx
	cmpl $20,%edx
	je .L1037
.L1038:
.L_LC1969:

.section	.line
	.4byte	2951	/ sas.c:2951
	.2byte	0xffff
	.4byte	.L_LC1969-.L_text_b
	.previous
	cmpb $2,66(%esi)
	jne .L1039
.L_LC1970:

.section	.line
	.4byte	2952	/ sas.c:2952
	.2byte	0xffff
	.4byte	.L_LC1970-.L_text_b
	.previous
	testb $4,32(%esi)
	je .L1041
.L_B284:
.L_LC1971:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1971-.L_text_b
	.previous
.L_LC1972:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1972-.L_text_b
	.previous
	movb 82(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1973:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1973-.L_text_b
	.previous
.L_B284_e:
.L_LC1974:

.section	.line
	.4byte	2953	/ sas.c:2953
	.2byte	0xffff
	.4byte	.L_LC1974-.L_text_b
	.previous
.L1041:
.L_B285:
.L_LC1975:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1975-.L_text_b
	.previous
.L_LC1976:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1976-.L_text_b
	.previous
	movb $-125,%al
	movw 80(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1977:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1977-.L_text_b
	.previous
.L_B285_e:
.L_LC1978:

.section	.line
	.4byte	2953	/ sas.c:2953
	.2byte	0xffff
	.4byte	.L_LC1978-.L_text_b
	.previous
	jmp .L1029
	.align 4
.L1039:
.L_LC1979:

.section	.line
	.4byte	2954	/ sas.c:2954
	.2byte	0xffff
	.4byte	.L_LC1979-.L_text_b
	.previous
	cmpb $1,66(%esi)
	jne .L1029
.L_LC1980:

.section	.line
	.4byte	2956	/ sas.c:2956
	.2byte	0xffff
	.4byte	.L_LC1980-.L_text_b
	.previous
	movb 32(%esi),%cl
	andb $4,%cl
	je .L1047
.L_B286:
.L_LC1981:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1981-.L_text_b
	.previous
.L_LC1982:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1982-.L_text_b
	.previous
	movb 82(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1983:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1983-.L_text_b
	.previous
.L_B286_e:
.L_LC1984:

.section	.line
	.4byte	2956	/ sas.c:2956
	.2byte	0xffff
	.4byte	.L_LC1984-.L_text_b
	.previous
.L1047:
.L_B287:
.L_LC1985:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1985-.L_text_b
	.previous
	movw 80(%esi),%bx
.L_LC1986:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1986-.L_text_b
	.previous
	movb $32,%al
	movl %ebx,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC1987:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1987-.L_text_b
	.previous
.L_B287_e:
.L_LC1988:

.section	.line
	.4byte	2957	/ sas.c:2957
	.2byte	0xffff
	.4byte	.L_LC1988-.L_text_b
	.previous
	testb %cl,%cl
	je .L1051
.L_B288:
.L_LC1989:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1989-.L_text_b
	.previous
.L_LC1990:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1990-.L_text_b
	.previous
	movb 94(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1991:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1991-.L_text_b
	.previous
.L_B288_e:
.L_LC1992:

.section	.line
	.4byte	2957	/ sas.c:2957
	.2byte	0xffff
	.4byte	.L_LC1992-.L_text_b
	.previous
.L1051:
.L_B289:
.L_LC1993:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1993-.L_text_b
	.previous
.L_LC1994:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1994-.L_text_b
	.previous
	movb $48,%al
	movw 92(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1995:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1995-.L_text_b
	.previous
.L_B289_e:
.L_LC1996:

.section	.line
	.4byte	2958	/ sas.c:2958
	.2byte	0xffff
	.4byte	.L_LC1996-.L_text_b
	.previous
	testb %cl,%cl
	je .L1055
.L_B290:
.L_LC1997:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC1997-.L_text_b
	.previous
.L_LC1998:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC1998-.L_text_b
	.previous
	movb 82(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC1999:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC1999-.L_text_b
	.previous
.L_B290_e:
.L_LC2000:

.section	.line
	.4byte	2958	/ sas.c:2958
	.2byte	0xffff
	.4byte	.L_LC2000-.L_text_b
	.previous
.L1055:
.L_B291:
.L_LC2001:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2001-.L_text_b
	.previous
.L_LC2002:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2002-.L_text_b
	.previous
	xorb %al,%al
	movl %ebx,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC2003:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2003-.L_text_b
	.previous
.L_B291_e:
.L_LC2004:

.section	.line
	.4byte	2960	/ sas.c:2960
	.2byte	0xffff
	.4byte	.L_LC2004-.L_text_b
	.previous
	jmp .L1029
	.align 4
.L1037:
.L_LC2005:

.section	.line
	.4byte	2963	/ sas.c:2963
	.2byte	0xffff
	.4byte	.L_LC2005-.L_text_b
	.previous
	xorl %ebx,%ebx
.L_LC2006:

.section	.line
	.4byte	2966	/ sas.c:2966
	.2byte	0xffff
	.4byte	.L_LC2006-.L_text_b
	.previous
	testb $4,%cl
	je .L1058
	testb $16,12(%esi)
	jne .L1058
.L_LC2007:

.section	.line
	.4byte	2968	/ sas.c:2968
	.2byte	0xffff
	.4byte	.L_LC2007-.L_text_b
	.previous
	andb $251,%cl
.L1058:
.L_LC2008:

.section	.line
	.4byte	2971	/ sas.c:2971
	.2byte	0xffff
	.4byte	.L_LC2008-.L_text_b
	.previous
	testb $28,%cl
	je .L1059
.L_LC2009:

.section	.line
	.4byte	2994	/ sas.c:2994
	.2byte	0xffff
	.4byte	.L_LC2009-.L_text_b
	.previous
	testb $16,%cl
	je .L1060
.L_LC2010:

.section	.line
	.4byte	2996	/ sas.c:2996
	.2byte	0xffff
	.4byte	.L_LC2010-.L_text_b
	.previous
	testb $1,12(%esi)
	jne .L1070
.L_LC2011:

.section	.line
	.4byte	2997	/ sas.c:2997
	.2byte	0xffff
	.4byte	.L_LC2011-.L_text_b
	.previous
	testb $2,12(%esi)
	je .L1062
.L_LC2012:

.section	.line
	.4byte	3000	/ sas.c:3000
	.2byte	0xffff
	.4byte	.L_LC2012-.L_text_b
	.previous
	orb $4,40(%esi)
	cmpl $0,event_scheduled
	jne .L1070
	movl $1,event_scheduled
	pushl $8
	pushl $0
	pushl $sas_event
	call timeout
	addl $12,%esp
.L_LC2013:

.section	.line
	.4byte	3001	/ sas.c:3001
	.2byte	0xffff
	.4byte	.L_LC2013-.L_text_b
	.previous
	jmp .L1070
	.align 4
.L1062:
.L_LC2014:

.section	.line
	.4byte	3004	/ sas.c:3004
	.2byte	0xffff
	.4byte	.L_LC2014-.L_text_b
	.previous
	movb $0,-4(%ebx,%ebp)
.L_LC2015:

.section	.line
	.4byte	3005	/ sas.c:3005
	.2byte	0xffff
	.4byte	.L_LC2015-.L_text_b
	.previous
	incl %ebx
.L_LC2016:

.section	.line
	.4byte	3006	/ sas.c:3006
	.2byte	0xffff
	.4byte	.L_LC2016-.L_text_b
	.previous
	testb $8,12(%esi)
	je .L1070
.L_LC2017:

.section	.line
	.4byte	3008	/ sas.c:3008
	.2byte	0xffff
	.4byte	.L_LC2017-.L_text_b
	.previous
	movb $0,-4(%ebx,%ebp)
.L_LC2018:

.section	.line
	.4byte	3014	/ sas.c:3014
	.2byte	0xffff
	.4byte	.L_LC2018-.L_text_b
	.previous
	jmp .L1099
	.align 4
.L1060:
.L_LC2019:

.section	.line
	.4byte	3015	/ sas.c:3015
	.2byte	0xffff
	.4byte	.L_LC2019-.L_text_b
	.previous
	testb $4,12(%esi)
	jne .L1070
.L_LC2020:

.section	.line
	.4byte	3016	/ sas.c:3016
	.2byte	0xffff
	.4byte	.L_LC2020-.L_text_b
	.previous
	testb $8,12(%esi)
	je .L1068
.L_LC2021:

.section	.line
	.4byte	3018	/ sas.c:3018
	.2byte	0xffff
	.4byte	.L_LC2021-.L_text_b
	.previous
	movl %edi,%eax
	movb %al,-4(%ebx,%ebp)
.L_LC2022:

.section	.line
	.4byte	3019	/ sas.c:3019
	.2byte	0xffff
	.4byte	.L_LC2022-.L_text_b
	.previous
	incl %ebx
.L_LC2023:

.section	.line
	.4byte	3020	/ sas.c:3020
	.2byte	0xffff
	.4byte	.L_LC2023-.L_text_b
	.previous
	movb $0,-4(%ebx,%ebp)
.L_LC2024:

.section	.line
	.4byte	3024	/ sas.c:3024
	.2byte	0xffff
	.4byte	.L_LC2024-.L_text_b
	.previous
	jmp .L1099
	.align 4
.L1068:
.L_LC2025:

.section	.line
	.4byte	3027	/ sas.c:3027
	.2byte	0xffff
	.4byte	.L_LC2025-.L_text_b
	.previous
	movb $0,-4(%ebx,%ebp)
.L_LC2026:

.section	.line
	.4byte	3028	/ sas.c:3028
	.2byte	0xffff
	.4byte	.L_LC2026-.L_text_b
	.previous
	incl %ebx
.L_LC2027:

.section	.line
	.4byte	3030	/ sas.c:3030
	.2byte	0xffff
	.4byte	.L_LC2027-.L_text_b
	.previous
	jmp .L1070
	.align 4
.L1059:
.L_LC2028:

.section	.line
	.4byte	3033	/ sas.c:3033
	.2byte	0xffff
	.4byte	.L_LC2028-.L_text_b
	.previous
	testb $1,%cl
	je .L1070
.L_LC2029:

.section	.line
	.4byte	3035	/ sas.c:3035
	.2byte	0xffff
	.4byte	.L_LC2029-.L_text_b
	.previous
	testb $4,13(%esi)
	je .L1080
.L_LC2030:

.section	.line
	.4byte	3038	/ sas.c:3038
	.2byte	0xffff
	.4byte	.L_LC2030-.L_text_b
	.previous
	testb $64,36(%esi)
	je .L1073
.L_LC2031:

.section	.line
	.4byte	3043	/ sas.c:3043
	.2byte	0xffff
	.4byte	.L_LC2031-.L_text_b
	.previous
	movzbl 192(%esi),%eax
	cmpl %eax,%edi
	je .L1075
	testb $8,13(%esi)
	je .L1076
.L1075:
.L_LC2032:

.section	.line
	.4byte	3047	/ sas.c:3047
	.2byte	0xffff
	.4byte	.L_LC2032-.L_text_b
	.previous
	andb $191,36(%esi)
.L_LC2033:

.section	.line
	.4byte	3049	/ sas.c:3049
	.2byte	0xffff
	.4byte	.L_LC2033-.L_text_b
	.previous
	movl -8(%ebp),%edx
	andl $-257,(%edx)
.L_LC2034:

.section	.line
	.4byte	3051	/ sas.c:3051
	.2byte	0xffff
	.4byte	.L_LC2034-.L_text_b
	.previous
	pushl %esi
	call sas_xproc
.L_LC2035:

.section	.line
	.4byte	3052	/ sas.c:3052
	.2byte	0xffff
	.4byte	.L_LC2035-.L_text_b
	.previous
	addl $4,%esp
.L_LC2036:

.section	.line
	.4byte	3053	/ sas.c:3053
	.2byte	0xffff
	.4byte	.L_LC2036-.L_text_b
	.previous
	jmp .L1076
	.align 4
.L1073:
.L_LC2037:

.section	.line
	.4byte	3059	/ sas.c:3059
	.2byte	0xffff
	.4byte	.L_LC2037-.L_text_b
	.previous
	movzbl 193(%esi),%eax
	cmpl %eax,%edi
	jne .L1076
.L_LC2038:

.section	.line
	.4byte	3062	/ sas.c:3062
	.2byte	0xffff
	.4byte	.L_LC2038-.L_text_b
	.previous
	orb $64,36(%esi)
.L_LC2039:

.section	.line
	.4byte	3064	/ sas.c:3064
	.2byte	0xffff
	.4byte	.L_LC2039-.L_text_b
	.previous
	movl -8(%ebp),%eax
	orl $256,(%eax)
.L1076:
.L_LC2040:

.section	.line
	.4byte	3074	/ sas.c:3074
	.2byte	0xffff
	.4byte	.L_LC2040-.L_text_b
	.previous
	movzbl 192(%esi),%eax
	cmpl %eax,%edi
	je .L1029
	movzbl 193(%esi),%eax
	cmpl %eax,%edi
	je .L1029
.L_I23_442:
.L1080:
.L_LC2041:

.section	.line
	.4byte	3080	/ sas.c:3080
	.2byte	0xffff
	.4byte	.L_LC2041-.L_text_b
	.previous
	cmpl $255,%edi
	jne .L1081
	testb $8,12(%esi)
	je .L1081
.L_LC2042:

.section	.line
	.4byte	3082	/ sas.c:3082
	.2byte	0xffff
	.4byte	.L_LC2042-.L_text_b
	.previous
	movb $255,-4(%ebx,%ebp)
.L_LC2043:

.section	.line
	.4byte	3083	/ sas.c:3083
	.2byte	0xffff
	.4byte	.L_LC2043-.L_text_b
	.previous
.L1099:
	incl %ebx
.L_LC2044:

.section	.line
	.4byte	3084	/ sas.c:3084
	.2byte	0xffff
	.4byte	.L_LC2044-.L_text_b
	.previous
	movb $255,-4(%ebx,%ebp)
.L_LC2045:

.section	.line
	.4byte	3085	/ sas.c:3085
	.2byte	0xffff
	.4byte	.L_LC2045-.L_text_b
	.previous
	incl %ebx
.L_LC2046:

.section	.line
	.4byte	3086	/ sas.c:3086
	.2byte	0xffff
	.4byte	.L_LC2046-.L_text_b
	.previous
	jmp .L1070
	.align 4
.L1081:
.L_LC2047:

.section	.line
	.4byte	3092	/ sas.c:3092
	.2byte	0xffff
	.4byte	.L_LC2047-.L_text_b
	.previous
	cmpl $2047,104(%esi)
	ja .L1029
.L_LC2048:

.section	.line
	.4byte	3094	/ sas.c:3094
	.2byte	0xffff
	.4byte	.L_LC2048-.L_text_b
	.previous
	cmpl $0,176(%esi)
	jne .L1084
.L_LC2049:

.section	.line
	.4byte	3095	/ sas.c:3095
	.2byte	0xffff
	.4byte	.L_LC2049-.L_text_b
	.previous
	movl 164(%esi),%eax
	movl %eax,176(%esi)
	testl %eax,%eax
	je .L1029
.L_LC2050:

.section	.line
	.4byte	3099	/ sas.c:3099
	.2byte	0xffff
	.4byte	.L_LC2050-.L_text_b
	.previous
	decl 168(%esi)
.L_LC2051:

.section	.line
	.4byte	3100	/ sas.c:3100
	.2byte	0xffff
	.4byte	.L_LC2051-.L_text_b
	.previous
	movl 176(%esi),%eax
	movl 8(%eax),%eax
	movl %eax,164(%esi)
.L_LC2052:

.section	.line
	.4byte	3101	/ sas.c:3101
	.2byte	0xffff
	.4byte	.L_LC2052-.L_text_b
	.previous
	movl 176(%esi),%eax
	movl $0,8(%eax)
.L_LC2053:

.section	.line
	.4byte	3102	/ sas.c:3102
	.2byte	0xffff
	.4byte	.L_LC2053-.L_text_b
	.previous
	movl $0,104(%esi)
.L1084:
.L_LC2054:

.section	.line
	.4byte	3104	/ sas.c:3104
	.2byte	0xffff
	.4byte	.L_LC2054-.L_text_b
	.previous
	incl 104(%esi)
.L_LC2055:

.section	.line
	.4byte	3105	/ sas.c:3105
	.2byte	0xffff
	.4byte	.L_LC2055-.L_text_b
	.previous
	movl 176(%esi),%edx
	movl %edx,-12(%ebp)
	movl 16(%edx),%eax
	movl %edi,%edx
	movb %dl,(%eax)
	movl -12(%ebp),%eax
	incl 16(%eax)
.L_LC2056:

.section	.line
	.4byte	3106	/ sas.c:3106
	.2byte	0xffff
	.4byte	.L_LC2056-.L_text_b
	.previous
	movl 104(%esi),%edx
	cmpl %edx,108(%esi)
	jne .L1029
.L_LC2057:

.section	.line
	.4byte	3107	/ sas.c:3107
	.2byte	0xffff
	.4byte	.L_LC2057-.L_text_b
	.previous
	pushl 176(%esi)
	leal 180(%esi),%eax
	pushl %eax
	call sas_link
.L_LC2058:

.section	.line
	.4byte	3109	/ sas.c:3109
	.2byte	0xffff
	.4byte	.L_LC2058-.L_text_b
	.previous
	movl $0,176(%esi)
.L_LC2059:

.section	.line
	.4byte	3110	/ sas.c:3110
	.2byte	0xffff
	.4byte	.L_LC2059-.L_text_b
	.previous
	movl $0,104(%esi)
.L_LC2060:

.section	.line
	.4byte	3111	/ sas.c:3111
	.2byte	0xffff
	.4byte	.L_LC2060-.L_text_b
	.previous
	addl $8,%esp
.L_LC2061:

.section	.line
	.4byte	3113	/ sas.c:3113
	.2byte	0xffff
	.4byte	.L_LC2061-.L_text_b
	.previous
	jmp .L1029
	.align 4
.L1070:
.L_LC2062:

.section	.line
	.4byte	3116	/ sas.c:3116
	.2byte	0xffff
	.4byte	.L_LC2062-.L_text_b
	.previous
	testl %ebx,%ebx
	je .L1029
.L_LC2063:

.section	.line
	.4byte	3118	/ sas.c:3118
	.2byte	0xffff
	.4byte	.L_LC2063-.L_text_b
	.previous
	movl %ebx,%eax
	addl 104(%esi),%eax
	cmpl $2048,%eax
	jbe .L1088
.L_LC2064:

.section	.line
	.4byte	3119	/ sas.c:3119
	.2byte	0xffff
	.4byte	.L_LC2064-.L_text_b
	.previous
	pushl 176(%esi)
	leal 180(%esi),%eax
	pushl %eax
	call sas_link
.L_LC2065:

.section	.line
	.4byte	3120	/ sas.c:3120
	.2byte	0xffff
	.4byte	.L_LC2065-.L_text_b
	.previous
	movl $0,176(%esi)
.L_LC2066:

.section	.line
	.4byte	3121	/ sas.c:3121
	.2byte	0xffff
	.4byte	.L_LC2066-.L_text_b
	.previous
	movl $0,104(%esi)
.L_LC2067:

.section	.line
	.4byte	3122	/ sas.c:3122
	.2byte	0xffff
	.4byte	.L_LC2067-.L_text_b
	.previous
	addl $8,%esp
	cmpl $0,168(%esi)
	je .L1029
.L_LC2068:

.section	.line
	.4byte	3124	/ sas.c:3124
	.2byte	0xffff
	.4byte	.L_LC2068-.L_text_b
	.previous
	decl 168(%esi)
.L_LC2069:

.section	.line
	.4byte	3125	/ sas.c:3125
	.2byte	0xffff
	.4byte	.L_LC2069-.L_text_b
	.previous
	movl 164(%esi),%eax
	movl %eax,176(%esi)
.L_LC2070:

.section	.line
	.4byte	3126	/ sas.c:3126
	.2byte	0xffff
	.4byte	.L_LC2070-.L_text_b
	.previous
	movl 176(%esi),%edx
	movl 8(%edx),%edx
	movl %edx,164(%esi)
.L_LC2071:

.section	.line
	.4byte	3127	/ sas.c:3127
	.2byte	0xffff
	.4byte	.L_LC2071-.L_text_b
	.previous
	movl 176(%esi),%eax
	movl $0,8(%eax)
.L1088:
.L_LC2072:

.section	.line
	.4byte	3130	/ sas.c:3130
	.2byte	0xffff
	.4byte	.L_LC2072-.L_text_b
	.previous
	cmpl $0,176(%esi)
	jne .L1090
.L_LC2073:

.section	.line
	.4byte	3131	/ sas.c:3131
	.2byte	0xffff
	.4byte	.L_LC2073-.L_text_b
	.previous
	movl 164(%esi),%eax
	movl %eax,176(%esi)
	testl %eax,%eax
	je .L1029
.L_LC2074:

.section	.line
	.4byte	3134	/ sas.c:3134
	.2byte	0xffff
	.4byte	.L_LC2074-.L_text_b
	.previous
	decl 168(%esi)
.L_LC2075:

.section	.line
	.4byte	3135	/ sas.c:3135
	.2byte	0xffff
	.4byte	.L_LC2075-.L_text_b
	.previous
	movl 176(%esi),%eax
	movl 8(%eax),%eax
	movl %eax,164(%esi)
.L_LC2076:

.section	.line
	.4byte	3136	/ sas.c:3136
	.2byte	0xffff
	.4byte	.L_LC2076-.L_text_b
	.previous
	movl 176(%esi),%eax
	movl $0,8(%eax)
.L1090:
.L_LC2077:

.section	.line
	.4byte	3138	/ sas.c:3138
	.2byte	0xffff
	.4byte	.L_LC2077-.L_text_b
	.previous
	addl %ebx,104(%esi)
	.align 4
.L1092:
.L_LC2078:

.section	.line
	.4byte	3143	/ sas.c:3143
	.2byte	0xffff
	.4byte	.L_LC2078-.L_text_b
	.previous
	movl 176(%esi),%edx
	movl %edx,-12(%ebp)
	movl 16(%edx),%eax
	movb -5(%ebx,%ebp),%dl
	movb %dl,(%eax)
	movl -12(%ebp),%eax
	incl 16(%eax)
.L_LC2079:

.section	.line
	.4byte	3144	/ sas.c:3144
	.2byte	0xffff
	.4byte	.L_LC2079-.L_text_b
	.previous
	decl %ebx
	jne .L1092
.L_LC2080:

.section	.line
	.4byte	3145	/ sas.c:3145
	.2byte	0xffff
	.4byte	.L_LC2080-.L_text_b
	.previous
.L1029:
	testb $4,32(%esi)
	je .L1096
.L_B292:
.L_LC2081:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2081-.L_text_b
	.previous
.L_LC2082:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2082-.L_text_b
	.previous
	movb 94(%esi),%al
	movw 68(%esi),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC2083:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2083-.L_text_b
	.previous
.L_B292_e:
.L_LC2084:

.section	.line
	.4byte	3145	/ sas.c:3145
	.2byte	0xffff
	.4byte	.L_LC2084-.L_text_b
	.previous
.L1096:
.L_B293:
.L_LC2085:

.section	.line
	.4byte	109	/ sas.c:109
	.2byte	0xffff
	.4byte	.L_LC2085-.L_text_b
	.previous
.L_LC2086:

.section	.line
	.4byte	111	/ sas.c:111
	.2byte	0xffff
	.4byte	.L_LC2086-.L_text_b
	.previous
.L_B294:
.L_LC2087:

.section	.line
	.4byte	112	/ sas.c:112
	.2byte	0xffff
	.4byte	.L_LC2087-.L_text_b
	.previous
	movw 92(%esi),%dx
/APP
	inb %dx,%eax
/NO_APP
.L_LC2088:

.section	.line
	.4byte	114	/ sas.c:114
	.2byte	0xffff
	.4byte	.L_LC2088-.L_text_b
	.previous
.L_B294_e:
.L_B293_e:
.L_LC2089:

.section	.line
	.4byte	3145	/ sas.c:3145
	.2byte	0xffff
	.4byte	.L_LC2089-.L_text_b
	.previous
	movzbl %al,%ecx
	testb $31,%cl
	jne .L1027
.L_LC2090:

.section	.line
	.4byte	3147	/ sas.c:3147
	.2byte	0xffff
	.4byte	.L_LC2090-.L_text_b
	.previous
	movl %ecx,%eax
.L_LC2091:

.section	.line
	.4byte	3148	/ sas.c:3148
	.2byte	0xffff
	.4byte	.L_LC2091-.L_text_b
	.previous
.L_b23_e:
	leal -32(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
.L_f23_e:
.Lfe20:
	.size	 sas_rproc,.Lfe20-sas_rproc

.section	.debug
.L_D1227:
	.4byte	.L_D1227_e-.L_D1227
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D1308
	.2byte	0x38
	.string	"sas_rproc"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x111
	.4byte	sas_rproc
	.2byte	0x121
	.4byte	.L_f23_e
	.2byte	0x8041
	.4byte	.L_b23
	.2byte	0x8051
	.4byte	.L_b23_e
.L_D1227_e:
.L_D1309:
	.4byte	.L_D1309_e-.L_D1309
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1310
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t1309_e-.L_t1309
.L_t1309:
	.byte	0x1
	.4byte	.L_T816
.L_t1309_e:
	.2byte	0x23
	.2byte	.L_l1309_e-.L_l1309
.L_l1309:
	.byte	0x1
	.4byte	0x6
.L_l1309_e:
.L_D1309_e:
.L_D1310:
	.4byte	.L_D1310_e-.L_D1310
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1311
	.2byte	0x38
	.string	"line_status"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1310_e-.L_l1310
.L_l1310:
	.byte	0x1
	.4byte	0x1
.L_l1310_e:
.L_D1310_e:
.L_D1311:
	.4byte	.L_D1311_e-.L_D1311
	.2byte	0xa
	.2byte	0x12
	.4byte	.L_D1312
	.2byte	0x38
	.string	"valid_char"
	.2byte	0x111
	.4byte	.L_I23_442
.L_D1311_e:
.L_D1312:
	.4byte	.L_D1312_e-.L_D1312
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1313
	.2byte	0x38
	.string	"regvar"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1312_e-.L_l1312
.L_l1312:
.L_l1312_e:
.L_D1312_e:
.L_D1313:
	.4byte	.L_D1313_e-.L_D1313
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1314
	.2byte	0x38
	.string	"ttyp"
	.2byte	0x63
	.2byte	.L_t1313_e-.L_t1313
.L_t1313:
	.byte	0x1
	.2byte	0x9
.L_t1313_e:
	.2byte	0x23
	.2byte	.L_l1313_e-.L_l1313
.L_l1313:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xfffffff8
	.byte	0x7
.L_l1313_e:
.L_D1313_e:
.L_D1314:
	.4byte	.L_D1314_e-.L_D1314
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1315
	.2byte	0x38
	.string	"charac"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1314_e-.L_l1314
.L_l1314:
	.byte	0x1
	.4byte	0x7
.L_l1314_e:
.L_D1314_e:
.L_D1315:
	.4byte	.L_D1315_e-.L_D1315
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1316
	.2byte	0x38
	.string	"csize"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1315_e-.L_l1315
.L_l1315:
	.byte	0x1
	.4byte	0x3
.L_l1315_e:
.L_D1315_e:
.L_D1316:
	.4byte	.L_D1316_e-.L_D1316
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D1317
	.set	.L_T1014,.L_D1316
	.2byte	0xa3
	.2byte	.L_s1316_e-.L_s1316
.L_s1316:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x3
	.byte	0x8
	.2byte	0x55
	.2byte	0x3
.L_s1316_e:
.L_D1316_e:
.L_D1317:
	.4byte	.L_D1317_e-.L_D1317
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1318
	.2byte	0x38
	.string	"metta"
	.2byte	0x72
	.4byte	.L_T1014
	.2byte	0x23
	.2byte	.L_l1317_e-.L_l1317
.L_l1317:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xfffffffc
	.byte	0x7
.L_l1317_e:
.L_D1317_e:
.L_D1318:
	.4byte	.L_D1318_e-.L_D1318
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1319
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B280
	.2byte	0x121
	.4byte	.L_B280_e
.L_D1318_e:
.L_D1320:
	.4byte	.L_D1320_e-.L_D1320
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1321
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1320_e-.L_l1320
.L_l1320:
.L_l1320_e:
.L_D1320_e:
.L_D1321:
	.4byte	.L_D1321_e-.L_D1321
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1322
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1321_e-.L_l1321
.L_l1321:
.L_l1321_e:
.L_D1321_e:
.L_D1322:
	.4byte	0x4
.L_D1319:
	.4byte	.L_D1319_e-.L_D1319
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1323
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B281
	.2byte	0x121
	.4byte	.L_B281_e
.L_D1319_e:
.L_D1324:
	.4byte	.L_D1324_e-.L_D1324
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1325
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1324_e-.L_l1324
.L_l1324:
.L_l1324_e:
.L_D1324_e:
.L_D1325:
	.4byte	.L_D1325_e-.L_D1325
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1326
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1325_e-.L_l1325
.L_l1325:
.L_l1325_e:
.L_D1325_e:
.L_D1326:
	.4byte	0x4
.L_D1323:
	.4byte	.L_D1323_e-.L_D1323
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1327
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B282
	.2byte	0x121
	.4byte	.L_B282_e
.L_D1323_e:
.L_D1328:
	.4byte	.L_D1328_e-.L_D1328
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1329
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1328_e-.L_l1328
.L_l1328:
.L_l1328_e:
.L_D1328_e:
.L_D1329:
	.4byte	.L_D1329_e-.L_D1329
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1330
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1329_e-.L_l1329
.L_l1329:
	.byte	0x1
	.4byte	0x0
.L_l1329_e:
.L_D1329_e:
.L_D1330:
	.4byte	0x4
.L_D1327:
	.4byte	.L_D1327_e-.L_D1327
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1331
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B284
	.2byte	0x121
	.4byte	.L_B284_e
.L_D1327_e:
.L_D1332:
	.4byte	.L_D1332_e-.L_D1332
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1333
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1332_e-.L_l1332
.L_l1332:
.L_l1332_e:
.L_D1332_e:
.L_D1333:
	.4byte	.L_D1333_e-.L_D1333
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1334
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1333_e-.L_l1333
.L_l1333:
.L_l1333_e:
.L_D1333_e:
.L_D1334:
	.4byte	0x4
.L_D1331:
	.4byte	.L_D1331_e-.L_D1331
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1335
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B285
	.2byte	0x121
	.4byte	.L_B285_e
.L_D1331_e:
.L_D1336:
	.4byte	.L_D1336_e-.L_D1336
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1337
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1336_e-.L_l1336
.L_l1336:
.L_l1336_e:
.L_D1336_e:
.L_D1337:
	.4byte	.L_D1337_e-.L_D1337
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1338
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l1337_e-.L_l1337
.L_l1337:
	.4byte	0xffffff83
.L_l1337_e:
.L_D1337_e:
.L_D1338:
	.4byte	0x4
.L_D1335:
	.4byte	.L_D1335_e-.L_D1335
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1339
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B286
	.2byte	0x121
	.4byte	.L_B286_e
.L_D1335_e:
.L_D1340:
	.4byte	.L_D1340_e-.L_D1340
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1341
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1340_e-.L_l1340
.L_l1340:
.L_l1340_e:
.L_D1340_e:
.L_D1341:
	.4byte	.L_D1341_e-.L_D1341
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1342
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1341_e-.L_l1341
.L_l1341:
.L_l1341_e:
.L_D1341_e:
.L_D1342:
	.4byte	0x4
.L_D1339:
	.4byte	.L_D1339_e-.L_D1339
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1343
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B287
	.2byte	0x121
	.4byte	.L_B287_e
.L_D1339_e:
.L_D1344:
	.4byte	.L_D1344_e-.L_D1344
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1345
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1344_e-.L_l1344
.L_l1344:
	.byte	0x1
	.4byte	0x3
.L_l1344_e:
.L_D1344_e:
.L_D1345:
	.4byte	.L_D1345_e-.L_D1345
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1346
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l1345_e-.L_l1345
.L_l1345:
	.4byte	0x20
.L_l1345_e:
.L_D1345_e:
.L_D1346:
	.4byte	0x4
.L_D1343:
	.4byte	.L_D1343_e-.L_D1343
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1347
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B288
	.2byte	0x121
	.4byte	.L_B288_e
.L_D1343_e:
.L_D1348:
	.4byte	.L_D1348_e-.L_D1348
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1349
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1348_e-.L_l1348
.L_l1348:
.L_l1348_e:
.L_D1348_e:
.L_D1349:
	.4byte	.L_D1349_e-.L_D1349
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1350
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1349_e-.L_l1349
.L_l1349:
.L_l1349_e:
.L_D1349_e:
.L_D1350:
	.4byte	0x4
.L_D1347:
	.4byte	.L_D1347_e-.L_D1347
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1351
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B289
	.2byte	0x121
	.4byte	.L_B289_e
.L_D1347_e:
.L_D1352:
	.4byte	.L_D1352_e-.L_D1352
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1353
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1352_e-.L_l1352
.L_l1352:
.L_l1352_e:
.L_D1352_e:
.L_D1353:
	.4byte	.L_D1353_e-.L_D1353
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1354
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l1353_e-.L_l1353
.L_l1353:
	.4byte	0x30
.L_l1353_e:
.L_D1353_e:
.L_D1354:
	.4byte	0x4
.L_D1351:
	.4byte	.L_D1351_e-.L_D1351
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1355
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B290
	.2byte	0x121
	.4byte	.L_B290_e
.L_D1351_e:
.L_D1356:
	.4byte	.L_D1356_e-.L_D1356
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1357
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1356_e-.L_l1356
.L_l1356:
.L_l1356_e:
.L_D1356_e:
.L_D1357:
	.4byte	.L_D1357_e-.L_D1357
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1358
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1357_e-.L_l1357
.L_l1357:
.L_l1357_e:
.L_D1357_e:
.L_D1358:
	.4byte	0x4
.L_D1355:
	.4byte	.L_D1355_e-.L_D1355
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1359
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B291
	.2byte	0x121
	.4byte	.L_B291_e
.L_D1355_e:
.L_D1360:
	.4byte	.L_D1360_e-.L_D1360
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1361
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1360_e-.L_l1360
.L_l1360:
.L_l1360_e:
.L_D1360_e:
.L_D1361:
	.4byte	.L_D1361_e-.L_D1361
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1362
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x1c4
	.4byte	.L_l1361_e-.L_l1361
.L_l1361:
	.4byte	0x0
.L_l1361_e:
.L_D1361_e:
.L_D1362:
	.4byte	0x4
.L_D1359:
	.4byte	.L_D1359_e-.L_D1359
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1363
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B292
	.2byte	0x121
	.4byte	.L_B292_e
.L_D1359_e:
.L_D1364:
	.4byte	.L_D1364_e-.L_D1364
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1365
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1364_e-.L_l1364
.L_l1364:
.L_l1364_e:
.L_D1364_e:
.L_D1365:
	.4byte	.L_D1365_e-.L_D1365
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1366
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1365_e-.L_l1365
.L_l1365:
.L_l1365_e:
.L_D1365_e:
.L_D1366:
	.4byte	0x4
.L_D1363:
	.4byte	.L_D1363_e-.L_D1363
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1367
	.2byte	0x2b2
	.4byte	.L_E1812
	.2byte	0x111
	.4byte	.L_B293
	.2byte	0x121
	.4byte	.L_B293_e
.L_D1363_e:
.L_D1368:
	.4byte	.L_D1368_e-.L_D1368
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1369
	.2byte	0x2b2
	.4byte	.L_E1811
	.2byte	0x23
	.2byte	.L_l1368_e-.L_l1368
.L_l1368:
.L_l1368_e:
.L_D1368_e:
.L_D1369:
	.4byte	.L_D1369_e-.L_D1369
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1370
	.2byte	0x2b2
	.4byte	.L_E1816
	.2byte	0x23
	.2byte	.L_l1369_e-.L_l1369
.L_l1369:
	.byte	0x1
	.4byte	0x0
.L_l1369_e:
.L_D1369_e:
.L_D1370:
	.4byte	0x4
.L_D1367:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_event,@function
sas_event:
.L_LC2092:

.section	.line
	.4byte	3157	/ sas.c:3157
	.2byte	0xffff
	.4byte	.L_LC2092-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	pushl %edi
	pushl %esi
	pushl %ebx
.L_b24:
.L_LC2093:

.section	.line
	.4byte	3158	/ sas.c:3158
	.2byte	0xffff
	.4byte	.L_LC2093-.L_text_b
	.previous
.L_LC2094:

.section	.line
	.4byte	3163	/ sas.c:3163
	.2byte	0xffff
	.4byte	.L_LC2094-.L_text_b
	.previous
	call spl6
.L_LC2095:

.section	.line
	.4byte	3164	/ sas.c:3164
	.2byte	0xffff
	.4byte	.L_LC2095-.L_text_b
	.previous
	pushl %eax
	call splx
.L_LC2096:

.section	.line
	.4byte	3167	/ sas.c:3167
	.2byte	0xffff
	.4byte	.L_LC2096-.L_text_b
	.previous
	movl $sas_info,%esi
	xorl %edi,%edi
	addl $4,%esp
	cmpl %edi,sas_physical_units
	jbe .L1102
	movl $sas_info+36,%ebx
	.align 4
.L_I24_31:
.L1104:
.L_LC2097:

.section	.line
	.4byte	3174	/ sas.c:3174
	.2byte	0xffff
	.4byte	.L_LC2097-.L_text_b
	.previous
	cmpl $0,4(%ebx)
	je .L1123
.L_LC2098:

.section	.line
	.4byte	3184	/ sas.c:3184
	.2byte	0xffff
	.4byte	.L_LC2098-.L_text_b
	.previous
	testb $32,4(%ebx)
	je .L1107
.L_LC2099:

.section	.line
	.4byte	3186	/ sas.c:3186
	.2byte	0xffff
	.4byte	.L_LC2099-.L_text_b
	.previous
	andb $223,4(%ebx)
.L_LC2100:

.section	.line
	.4byte	3187	/ sas.c:3187
	.2byte	0xffff
	.4byte	.L_LC2100-.L_text_b
	.previous
	andb $240,25(%ebx)
.L1107:
.L_LC2101:

.section	.line
	.4byte	3191	/ sas.c:3191
	.2byte	0xffff
	.4byte	.L_LC2101-.L_text_b
	.previous
	testb $8,4(%ebx)
	je .L1108
.L_LC2102:

.section	.line
	.4byte	3193	/ sas.c:3193
	.2byte	0xffff
	.4byte	.L_LC2102-.L_text_b
	.previous
	andb $247,4(%ebx)
.L_LC2103:

.section	.line
	.4byte	3194	/ sas.c:3194
	.2byte	0xffff
	.4byte	.L_LC2103-.L_text_b
	.previous
	andb $240,25(%ebx)
.L_LC2104:

.section	.line
	.4byte	3195	/ sas.c:3195
	.2byte	0xffff
	.4byte	.L_LC2104-.L_text_b
	.previous
	pushl %esi
	call sas_mproc
.L_LC2105:

.section	.line
	.4byte	3196	/ sas.c:3196
	.2byte	0xffff
	.4byte	.L_LC2105-.L_text_b
	.previous
	addl $4,%esp
.L1108:
.L_LC2106:

.section	.line
	.4byte	3199	/ sas.c:3199
	.2byte	0xffff
	.4byte	.L_LC2106-.L_text_b
	.previous
	testb $4,4(%ebx)
	je .L1109
.L_LC2107:

.section	.line
	.4byte	3201	/ sas.c:3201
	.2byte	0xffff
	.4byte	.L_LC2107-.L_text_b
	.previous
	andb $251,4(%ebx)
.L_LC2108:

.section	.line
	.4byte	3202	/ sas.c:3202
	.2byte	0xffff
	.4byte	.L_LC2108-.L_text_b
	.previous
	movl 76(%ebx),%eax
	testb $4,(%eax)
	je .L1109
.L_LC2109:

.section	.line
	.4byte	3203	/ sas.c:3203
	.2byte	0xffff
	.4byte	.L_LC2109-.L_text_b
	.previous
	pushl $8
	movl 120(%ebx),%eax
	pushl 12(%eax)
	call putctl
	addl $8,%esp
.L1109:
.L_LC2110:

.section	.line
	.4byte	3207	/ sas.c:3207
	.2byte	0xffff
	.4byte	.L_LC2110-.L_text_b
	.previous
	testb $1,4(%ebx)
	je .L1111
.L_LC2111:

.section	.line
	.4byte	3209	/ sas.c:3209
	.2byte	0xffff
	.4byte	.L_LC2111-.L_text_b
	.previous
	andb $254,4(%ebx)
.L_LC2112:

.section	.line
	.4byte	3210	/ sas.c:3210
	.2byte	0xffff
	.4byte	.L_LC2112-.L_text_b
	.previous
	testb $2,1(%ebx)
	jne .L1111
.L_LC2113:

.section	.line
	.4byte	3213	/ sas.c:3213
	.2byte	0xffff
	.4byte	.L_LC2113-.L_text_b
	.previous
	pushl 120(%ebx)
	call qenable
.L_LC2114:

.section	.line
	.4byte	3214	/ sas.c:3214
	.2byte	0xffff
	.4byte	.L_LC2114-.L_text_b
	.previous
	addl $4,%esp
.L1111:
.L_LC2115:

.section	.line
	.4byte	3218	/ sas.c:3218
	.2byte	0xffff
	.4byte	.L_LC2115-.L_text_b
	.previous
	testb $2,4(%ebx)
	je .L1113
.L_LC2116:

.section	.line
	.4byte	3220	/ sas.c:3220
	.2byte	0xffff
	.4byte	.L_LC2116-.L_text_b
	.previous
	andb $253,4(%ebx)
.L_LC2117:

.section	.line
	.4byte	3221	/ sas.c:3221
	.2byte	0xffff
	.4byte	.L_LC2117-.L_text_b
	.previous
	pushl %esi
	call sas_hdx_check
.L_LC2118:

.section	.line
	.4byte	3223	/ sas.c:3223
	.2byte	0xffff
	.4byte	.L_LC2118-.L_text_b
	.previous
	movl 120(%ebx),%eax
	testb $16,28(%eax)
	je .L1114
	addl $64,%eax
.L1114:
	pushl %eax
	call qenable
.L_LC2119:

.section	.line
	.4byte	3224	/ sas.c:3224
	.2byte	0xffff
	.4byte	.L_LC2119-.L_text_b
	.previous
	addl $8,%esp
.L1113:
.L_LC2120:

.section	.line
	.4byte	3241	/ sas.c:3241
	.2byte	0xffff
	.4byte	.L_LC2120-.L_text_b
	.previous
	movl -4(%ebx),%eax
	andl $8200,%eax
	cmpl $8,%eax
	jne .L1115
.L_LC2121:

.section	.line
	.4byte	3249	/ sas.c:3249
	.2byte	0xffff
	.4byte	.L_LC2121-.L_text_b
	.previous
	testb $8,-15(%ebx)
	je .L1117
	testb $1,(%ebx)
	je .L1115
.L1117:
.L_LC2122:

.section	.line
	.4byte	3252	/ sas.c:3252
	.2byte	0xffff
	.4byte	.L_LC2122-.L_text_b
	.previous
	pushl %esi
	call sas_msi_enable
	addl $4,%esp
.L1115:
.L_LC2123:

.section	.line
	.4byte	3256	/ sas.c:3256
	.2byte	0xffff
	.4byte	.L_LC2123-.L_text_b
	.previous
	call spl6
.L_LC2124:

.section	.line
	.4byte	3257	/ sas.c:3257
	.2byte	0xffff
	.4byte	.L_LC2124-.L_text_b
	.previous
	pushl %eax
	call splx
.L_LC2125:

.section	.line
	.4byte	3168	/ sas.c:3168
	.2byte	0xffff
	.4byte	.L_LC2125-.L_text_b
	.previous
	addl $4,%esp
.L1123:
	addl $204,%ebx
	addl $204,%esi
	incl %edi
	cmpl %edi,sas_physical_units
	ja .L1104
.L1102:
.L_LC2126:

.section	.line
	.4byte	3260	/ sas.c:3260
	.2byte	0xffff
	.4byte	.L_LC2126-.L_text_b
	.previous
	movl $0,event_scheduled
.L_LC2127:

.section	.line
	.4byte	3264	/ sas.c:3264
	.2byte	0xffff
	.4byte	.L_LC2127-.L_text_b
	.previous
	movl $sas_info,%esi
.L_LC2128:

.section	.line
	.4byte	3265	/ sas.c:3265
	.2byte	0xffff
	.4byte	.L_LC2128-.L_text_b
	.previous
	xorl %edi,%edi
.L_I24_220:
.L1119:
.L_LC2129:

.section	.line
	.4byte	3268	/ sas.c:3268
	.2byte	0xffff
	.4byte	.L_LC2129-.L_text_b
	.previous
	cmpl $0,40(%esi)
	jne .L1120
.L_LC2130:

.section	.line
	.4byte	3271	/ sas.c:3271
	.2byte	0xffff
	.4byte	.L_LC2130-.L_text_b
	.previous
	addl $204,%esi
.L_LC2131:

.section	.line
	.4byte	3272	/ sas.c:3272
	.2byte	0xffff
	.4byte	.L_LC2131-.L_text_b
	.previous
	incl %edi
	cmpl %edi,sas_physical_units
	ja .L1119
.L_LC2132:

.section	.line
	.4byte	3274	/ sas.c:3274
	.2byte	0xffff
	.4byte	.L_LC2132-.L_text_b
	.previous
	jmp .L1122
	.align 4
.L1120:
.L_LC2133:

.section	.line
	.4byte	3280	/ sas.c:3280
	.2byte	0xffff
	.4byte	.L_LC2133-.L_text_b
	.previous
	movl $1,event_scheduled
.L_LC2134:

.section	.line
	.4byte	3281	/ sas.c:3281
	.2byte	0xffff
	.4byte	.L_LC2134-.L_text_b
	.previous
	pushl $8
	pushl $0
	pushl $sas_event
	call timeout
.L1122:
.L_LC2135:

.section	.line
	.4byte	3284	/ sas.c:3284
	.2byte	0xffff
	.4byte	.L_LC2135-.L_text_b
	.previous
.L_b24_e:
	leal -12(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
.L_f24_e:
.Lfe21:
	.size	 sas_event,.Lfe21-sas_event

.section	.debug
.L_D1308:
	.4byte	.L_D1308_e-.L_D1308
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D1371
	.2byte	0x38
	.string	"sas_event"
	.2byte	0x111
	.4byte	sas_event
	.2byte	0x121
	.4byte	.L_f24_e
	.2byte	0x8041
	.4byte	.L_b24
	.2byte	0x8051
	.4byte	.L_b24_e
.L_D1308_e:
.L_D1372:
	.4byte	.L_D1372_e-.L_D1372
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1373
	.2byte	0x38
	.string	"dummy"
	.2byte	0x63
	.2byte	.L_t1372_e-.L_t1372
.L_t1372:
	.byte	0x1
	.2byte	0x14
.L_t1372_e:
	.2byte	0x23
	.2byte	.L_l1372_e-.L_l1372
.L_l1372:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l1372_e:
.L_D1372_e:
.L_D1373:
	.4byte	.L_D1373_e-.L_D1373
	.2byte	0xa
	.2byte	0x12
	.4byte	.L_D1374
	.2byte	0x38
	.string	"fastloop2"
	.2byte	0x111
	.4byte	.L_I24_31
.L_D1373_e:
.L_D1374:
	.4byte	.L_D1374_e-.L_D1374
	.2byte	0xa
	.2byte	0x12
	.4byte	.L_D1375
	.2byte	0x38
	.string	"fastloop3"
	.2byte	0x111
	.4byte	.L_I24_220
.L_D1374_e:
.L_D1375:
	.4byte	.L_D1375_e-.L_D1375
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1376
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t1375_e-.L_t1375
.L_t1375:
	.byte	0x1
	.4byte	.L_T816
.L_t1375_e:
	.2byte	0x23
	.2byte	.L_l1375_e-.L_l1375
.L_l1375:
	.byte	0x1
	.4byte	0x6
.L_l1375_e:
.L_D1375_e:
.L_D1376:
	.4byte	.L_D1376_e-.L_D1376
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1377
	.2byte	0x38
	.string	"unit"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1376_e-.L_l1376
.L_l1376:
	.byte	0x1
	.4byte	0x7
.L_l1376_e:
.L_D1376_e:
.L_D1377:
	.4byte	.L_D1377_e-.L_D1377
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1378
	.2byte	0x38
	.string	"old_level"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1377_e-.L_l1377
.L_l1377:
	.byte	0x1
	.4byte	0x0
.L_l1377_e:
.L_D1377_e:
.L_D1378:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_mproc,@function
sas_mproc:
.L_LC2136:

.section	.line
	.4byte	3290	/ sas.c:3290
	.2byte	0xffff
	.4byte	.L_LC2136-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 8(%ebp),%ebx
.L_b25:
.L_LC2137:

.section	.line
	.4byte	3291	/ sas.c:3291
	.2byte	0xffff
	.4byte	.L_LC2137-.L_text_b
	.previous
	movl 112(%ebx),%ecx
.L_LC2138:

.section	.line
	.4byte	3296	/ sas.c:3296
	.2byte	0xffff
	.4byte	.L_LC2138-.L_text_b
	.previous
	movzbl 61(%ebx),%esi
.L_LC2139:

.section	.line
	.4byte	3297	/ sas.c:3297
	.2byte	0xffff
	.4byte	.L_LC2139-.L_text_b
	.previous
	andb $191,61(%ebx)
.L_LC2140:

.section	.line
	.4byte	3303	/ sas.c:3303
	.2byte	0xffff
	.4byte	.L_LC2140-.L_text_b
	.previous
	testb $8,21(%ebx)
	jne .L1125
.L_LC2141:

.section	.line
	.4byte	3305	/ sas.c:3305
	.2byte	0xffff
	.4byte	.L_LC2141-.L_text_b
	.previous
	movl %esi,%eax
	notl %eax
	movzbl 55(%ebx),%edi
	testl %eax,%edi
	jne .L1126
.L_LC2142:

.section	.line
	.4byte	3307	/ sas.c:3307
	.2byte	0xffff
	.4byte	.L_LC2142-.L_text_b
	.previous
	orb $16,(%ecx)
.L_LC2143:

.section	.line
	.4byte	3309	/ sas.c:3309
	.2byte	0xffff
	.4byte	.L_LC2143-.L_text_b
	.previous
	testb $2,(%ecx)
	je .L1125
	movzbl 60(%ebx),%eax
	notl %eax
	movzbl 55(%ebx),%edx
	testl %eax,%edx
	je .L1125
.L_LC2144:

.section	.line
	.4byte	3311	/ sas.c:3311
	.2byte	0xffff
	.4byte	.L_LC2144-.L_text_b
	.previous
	pushl %ecx
	call wakeup
.L_LC2145:

.section	.line
	.4byte	3312	/ sas.c:3312
	.2byte	0xffff
	.4byte	.L_LC2145-.L_text_b
	.previous
	jmp .L1125
	.align 4
.L1126:
.L_LC2146:

.section	.line
	.4byte	3315	/ sas.c:3315
	.2byte	0xffff
	.4byte	.L_LC2146-.L_text_b
	.previous
	movzbl 60(%ebx),%eax
	notl %eax
	testl %eax,%edi
	jne .L1125
.L_LC2147:

.section	.line
	.4byte	3317	/ sas.c:3317
	.2byte	0xffff
	.4byte	.L_LC2147-.L_text_b
	.previous
	andl $-17,(%ecx)
.L_LC2148:

.section	.line
	.4byte	3318	/ sas.c:3318
	.2byte	0xffff
	.4byte	.L_LC2148-.L_text_b
	.previous
	testb $4,(%ecx)
	je .L1125
.L_LC2149:

.section	.line
	.4byte	3319	/ sas.c:3319
	.2byte	0xffff
	.4byte	.L_LC2149-.L_text_b
	.previous
	pushl $2
	pushl $134
	movl 156(%ebx),%eax
	pushl 12(%eax)
	call putctl1
.L_LC2150:

.section	.line
	.4byte	3320	/ sas.c:3320
	.2byte	0xffff
	.4byte	.L_LC2150-.L_text_b
	.previous
	pushl $137
	movl 156(%ebx),%eax
	pushl 12(%eax)
	call putctl1
.L1125:
.L_LC2151:

.section	.line
	.4byte	3348	/ sas.c:3348
	.2byte	0xffff
	.4byte	.L_LC2151-.L_text_b
	.previous
	movl %esi,%edx
	andb $191,%dl
	movb %dl,60(%ebx)
.L_LC2152:

.section	.line
	.4byte	3349	/ sas.c:3349
	.2byte	0xffff
	.4byte	.L_LC2152-.L_text_b
	.previous
.L_b25_e:
	leal -12(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
.L_f25_e:
.Lfe22:
	.size	 sas_mproc,.Lfe22-sas_mproc

.section	.debug
.L_D1371:
	.4byte	.L_D1371_e-.L_D1371
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D1379
	.2byte	0x38
	.string	"sas_mproc"
	.2byte	0x111
	.4byte	sas_mproc
	.2byte	0x121
	.4byte	.L_f25_e
	.2byte	0x8041
	.4byte	.L_b25
	.2byte	0x8051
	.4byte	.L_b25_e
.L_D1371_e:
.L_D1380:
	.4byte	.L_D1380_e-.L_D1380
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1381
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t1380_e-.L_t1380
.L_t1380:
	.byte	0x1
	.4byte	.L_T816
.L_t1380_e:
	.2byte	0x23
	.2byte	.L_l1380_e-.L_l1380
.L_l1380:
	.byte	0x1
	.4byte	0x3
.L_l1380_e:
.L_D1380_e:
.L_D1381:
	.4byte	.L_D1381_e-.L_D1381
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1382
	.2byte	0x38
	.string	"ttyp"
	.2byte	0x63
	.2byte	.L_t1381_e-.L_t1381
.L_t1381:
	.byte	0x1
	.2byte	0x9
.L_t1381_e:
	.2byte	0x23
	.2byte	.L_l1381_e-.L_l1381
.L_l1381:
	.byte	0x1
	.4byte	0x1
.L_l1381_e:
.L_D1381_e:
.L_D1382:
	.4byte	.L_D1382_e-.L_D1382
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1383
	.2byte	0x38
	.string	"mdm_status"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1382_e-.L_l1382
.L_l1382:
	.byte	0x1
	.4byte	0x6
.L_l1382_e:
.L_D1382_e:
.L_D1383:
	.4byte	.L_D1383_e-.L_D1383
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1384
	.2byte	0x38
	.string	"vpix_status"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1383_e-.L_l1383
.L_l1383:
.L_l1383_e:
.L_D1383_e:
.L_D1384:
	.4byte	.L_D1384_e-.L_D1384
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1385
	.2byte	0x38
	.string	"old_level"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1384_e-.L_l1384
.L_l1384:
.L_l1384_e:
.L_D1384_e:
.L_D1385:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_fproc,@function
sas_fproc:
.L_LC2153:

.section	.line
	.4byte	3355	/ sas.c:3355
	.2byte	0xffff
	.4byte	.L_LC2153-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	movl 8(%ebp),%edx
.L_b26:
.L_LC2154:

.section	.line
	.4byte	3359	/ sas.c:3359
	.2byte	0xffff
	.4byte	.L_LC2154-.L_text_b
	.previous
	movl 12(%ebp),%ecx
	notl %ecx
	movzbl 57(%edx),%eax
	testl %ecx,%eax
	je .L1133
	movzbl 58(%edx),%eax
	testl %ecx,%eax
	jne .L1133
	testb $1,36(%edx)
	jne .L1132
.L1133:
.L_LC2155:

.section	.line
	.4byte	3363	/ sas.c:3363
	.2byte	0xffff
	.4byte	.L_LC2155-.L_text_b
	.previous
	testb $8,36(%edx)
	je .L1135
.L_LC2156:

.section	.line
	.4byte	3365	/ sas.c:3365
	.2byte	0xffff
	.4byte	.L_LC2156-.L_text_b
	.previous
	andb $247,36(%edx)
.L_LC2157:

.section	.line
	.4byte	3366	/ sas.c:3366
	.2byte	0xffff
	.4byte	.L_LC2157-.L_text_b
	.previous
	pushl %edx
	call sas_xproc
.L_LC2158:

.section	.line
	.4byte	3368	/ sas.c:3368
	.2byte	0xffff
	.4byte	.L_LC2158-.L_text_b
	.previous
	leave
	ret
	.align 4
.L1132:
.L_LC2159:

.section	.line
	.4byte	3370	/ sas.c:3370
	.2byte	0xffff
	.4byte	.L_LC2159-.L_text_b
	.previous
	orb $8,36(%edx)
.L1135:
.L_LC2160:

.section	.line
	.4byte	3371	/ sas.c:3371
	.2byte	0xffff
	.4byte	.L_LC2160-.L_text_b
	.previous
	leave
	ret
.L_b26_e:
.L_f26_e:
.Lfe23:
	.size	 sas_fproc,.Lfe23-sas_fproc

.section	.debug
.L_D1379:
	.4byte	.L_D1379_e-.L_D1379
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D1386
	.2byte	0x38
	.string	"sas_fproc"
	.2byte	0x111
	.4byte	sas_fproc
	.2byte	0x121
	.4byte	.L_f26_e
	.2byte	0x8041
	.4byte	.L_b26
	.2byte	0x8051
	.4byte	.L_b26_e
.L_D1379_e:
.L_D1387:
	.4byte	.L_D1387_e-.L_D1387
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1388
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t1387_e-.L_t1387
.L_t1387:
	.byte	0x1
	.4byte	.L_T816
.L_t1387_e:
	.2byte	0x23
	.2byte	.L_l1387_e-.L_l1387
.L_l1387:
	.byte	0x1
	.4byte	0x2
.L_l1387_e:
.L_D1387_e:
.L_D1388:
	.4byte	.L_D1388_e-.L_D1388
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1389
	.2byte	0x38
	.string	"mdm_status"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1388_e-.L_l1388
.L_l1388:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l1388_e:
.L_D1388_e:
.L_D1389:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_msi_disable,@function
sas_msi_disable:
.L_LC2161:

.section	.line
	.4byte	3377	/ sas.c:3377
	.2byte	0xffff
	.4byte	.L_LC2161-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	pushl %ebx
	movl 8(%ebp),%ecx
.L_b27:
.L_LC2162:

.section	.line
	.4byte	3378	/ sas.c:3378
	.2byte	0xffff
	.4byte	.L_LC2162-.L_text_b
	.previous
.L_LC2163:

.section	.line
	.4byte	3380	/ sas.c:3380
	.2byte	0xffff
	.4byte	.L_LC2163-.L_text_b
	.previous
	movb 64(%ecx),%bl
	andb $247,%bl
	movb %bl,64(%ecx)
.L_LC2164:

.section	.line
	.4byte	3381	/ sas.c:3381
	.2byte	0xffff
	.4byte	.L_LC2164-.L_text_b
	.previous
	testb $6,32(%ecx)
	je .L1138
.L_B298:
.L_LC2165:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2165-.L_text_b
	.previous
.L_LC2166:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2166-.L_text_b
	.previous
	movb 78(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC2167:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2167-.L_text_b
	.previous
.L_B298_e:
.L_LC2168:

.section	.line
	.4byte	3381	/ sas.c:3381
	.2byte	0xffff
	.4byte	.L_LC2168-.L_text_b
	.previous
.L1138:
.L_B299:
.L_LC2169:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2169-.L_text_b
	.previous
.L_LC2170:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2170-.L_text_b
	.previous
	movb %bl,%al
	movw 76(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC2171:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2171-.L_text_b
	.previous
.L_B299_e:
.L_LC2172:

.section	.line
	.4byte	3382	/ sas.c:3382
	.2byte	0xffff
	.4byte	.L_LC2172-.L_text_b
	.previous
	andw $57343,32(%ecx)
.L_LC2173:

.section	.line
	.4byte	3383	/ sas.c:3383
	.2byte	0xffff
	.4byte	.L_LC2173-.L_text_b
	.previous
.L_b27_e:
	movl -4(%ebp),%ebx
	leave
	ret
.L_f27_e:
.Lfe24:
	.size	 sas_msi_disable,.Lfe24-sas_msi_disable

.section	.debug
.L_D1386:
	.4byte	.L_D1386_e-.L_D1386
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D1390
	.2byte	0x38
	.string	"sas_msi_disable"
	.2byte	0x111
	.4byte	sas_msi_disable
	.2byte	0x121
	.4byte	.L_f27_e
	.2byte	0x8041
	.4byte	.L_b27
	.2byte	0x8051
	.4byte	.L_b27_e
.L_D1386_e:
.L_D1391:
	.4byte	.L_D1391_e-.L_D1391
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1392
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t1391_e-.L_t1391
.L_t1391:
	.byte	0x1
	.4byte	.L_T816
.L_t1391_e:
	.2byte	0x23
	.2byte	.L_l1391_e-.L_l1391
.L_l1391:
	.byte	0x1
	.4byte	0x1
.L_l1391_e:
.L_D1391_e:
.L_D1392:
	.4byte	.L_D1392_e-.L_D1392
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1393
	.2byte	0x38
	.string	"regvar"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1392_e-.L_l1392
.L_l1392:
.L_l1392_e:
.L_D1392_e:
.L_D1393:
	.4byte	.L_D1393_e-.L_D1393
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1394
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B298
	.2byte	0x121
	.4byte	.L_B298_e
.L_D1393_e:
.L_D1395:
	.4byte	.L_D1395_e-.L_D1395
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1396
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1395_e-.L_l1395
.L_l1395:
.L_l1395_e:
.L_D1395_e:
.L_D1396:
	.4byte	.L_D1396_e-.L_D1396
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1397
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1396_e-.L_l1396
.L_l1396:
.L_l1396_e:
.L_D1396_e:
.L_D1397:
	.4byte	0x4
.L_D1394:
	.4byte	.L_D1394_e-.L_D1394
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1398
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B299
	.2byte	0x121
	.4byte	.L_B299_e
.L_D1394_e:
.L_D1399:
	.4byte	.L_D1399_e-.L_D1399
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1400
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1399_e-.L_l1399
.L_l1399:
.L_l1399_e:
.L_D1399_e:
.L_D1400:
	.4byte	.L_D1400_e-.L_D1400
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1401
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1400_e-.L_l1400
.L_l1400:
.L_l1400_e:
.L_D1400_e:
.L_D1401:
	.4byte	0x4
.L_D1398:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_msi_enable,@function
sas_msi_enable:
.L_LC2174:

.section	.line
	.4byte	3389	/ sas.c:3389
	.2byte	0xffff
	.4byte	.L_LC2174-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	pushl %ebx
	movl 8(%ebp),%ecx
.L_b28:
.L_LC2175:

.section	.line
	.4byte	3390	/ sas.c:3390
	.2byte	0xffff
	.4byte	.L_LC2175-.L_text_b
	.previous
.L_LC2176:

.section	.line
	.4byte	3392	/ sas.c:3392
	.2byte	0xffff
	.4byte	.L_LC2176-.L_text_b
	.previous
	orw $8192,32(%ecx)
.L_LC2177:

.section	.line
	.4byte	3393	/ sas.c:3393
	.2byte	0xffff
	.4byte	.L_LC2177-.L_text_b
	.previous
	movb 64(%ecx),%bl
	orb $8,%bl
	movb %bl,64(%ecx)
.L_LC2178:

.section	.line
	.4byte	3394	/ sas.c:3394
	.2byte	0xffff
	.4byte	.L_LC2178-.L_text_b
	.previous
	testb $6,32(%ecx)
	je .L1143
.L_B301:
.L_LC2179:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2179-.L_text_b
	.previous
.L_LC2180:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2180-.L_text_b
	.previous
	movb 78(%ecx),%al
	movw 68(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC2181:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2181-.L_text_b
	.previous
.L_B301_e:
.L_LC2182:

.section	.line
	.4byte	3394	/ sas.c:3394
	.2byte	0xffff
	.4byte	.L_LC2182-.L_text_b
	.previous
.L1143:
.L_B302:
.L_LC2183:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2183-.L_text_b
	.previous
.L_LC2184:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2184-.L_text_b
	.previous
	movb %bl,%al
	movw 76(%ecx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC2185:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2185-.L_text_b
	.previous
.L_B302_e:
.L_LC2186:

.section	.line
	.4byte	3395	/ sas.c:3395
	.2byte	0xffff
	.4byte	.L_LC2186-.L_text_b
	.previous
.L_b28_e:
	movl -4(%ebp),%ebx
	leave
	ret
.L_f28_e:
.Lfe25:
	.size	 sas_msi_enable,.Lfe25-sas_msi_enable

.section	.debug
.L_D1390:
	.4byte	.L_D1390_e-.L_D1390
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D1402
	.2byte	0x38
	.string	"sas_msi_enable"
	.2byte	0x111
	.4byte	sas_msi_enable
	.2byte	0x121
	.4byte	.L_f28_e
	.2byte	0x8041
	.4byte	.L_b28
	.2byte	0x8051
	.4byte	.L_b28_e
.L_D1390_e:
.L_D1403:
	.4byte	.L_D1403_e-.L_D1403
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1404
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t1403_e-.L_t1403
.L_t1403:
	.byte	0x1
	.4byte	.L_T816
.L_t1403_e:
	.2byte	0x23
	.2byte	.L_l1403_e-.L_l1403
.L_l1403:
	.byte	0x1
	.4byte	0x1
.L_l1403_e:
.L_D1403_e:
.L_D1404:
	.4byte	.L_D1404_e-.L_D1404
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1405
	.2byte	0x38
	.string	"regvar"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1404_e-.L_l1404
.L_l1404:
.L_l1404_e:
.L_D1404_e:
.L_D1405:
	.4byte	.L_D1405_e-.L_D1405
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1406
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B301
	.2byte	0x121
	.4byte	.L_B301_e
.L_D1405_e:
.L_D1407:
	.4byte	.L_D1407_e-.L_D1407
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1408
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1407_e-.L_l1407
.L_l1407:
.L_l1407_e:
.L_D1407_e:
.L_D1408:
	.4byte	.L_D1408_e-.L_D1408
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1409
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1408_e-.L_l1408
.L_l1408:
.L_l1408_e:
.L_D1408_e:
.L_D1409:
	.4byte	0x4
.L_D1406:
	.4byte	.L_D1406_e-.L_D1406
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1410
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B302
	.2byte	0x121
	.4byte	.L_B302_e
.L_D1406_e:
.L_D1411:
	.4byte	.L_D1411_e-.L_D1411
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1412
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1411_e-.L_l1411
.L_l1411:
.L_l1411_e:
.L_D1411_e:
.L_D1412:
	.4byte	.L_D1412_e-.L_D1412
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1413
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1412_e-.L_l1412
.L_l1412:
.L_l1412_e:
.L_D1412_e:
.L_D1413:
	.4byte	0x4
.L_D1410:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_param,@function
sas_param:
.L_LC2187:

.section	.line
	.4byte	3403	/ sas.c:3403
	.2byte	0xffff
	.4byte	.L_LC2187-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	subl $8,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 8(%ebp),%ebx
	movl 16(%ebp),%esi
.L_b29:
.L_LC2188:

.section	.line
	.4byte	3404	/ sas.c:3404
	.2byte	0xffff
	.4byte	.L_LC2188-.L_text_b
	.previous
.L_LC2189:

.section	.line
	.4byte	3421	/ sas.c:3421
	.2byte	0xffff
	.4byte	.L_LC2189-.L_text_b
	.previous
	movl %esi,16(%ebx)
.L_LC2190:

.section	.line
	.4byte	3425	/ sas.c:3425
	.2byte	0xffff
	.4byte	.L_LC2190-.L_text_b
	.previous
	movl %esi,%ecx
	andb $240,%cl
	testl $15,%esi
	jne .L1147
	movl 20(%ebx),%eax
	andl $15,%eax
	orl %eax,%ecx
	jmp .L1148
	.align 4
.L1147:
	testb $64,33(%ebx)
	je .L1149
	movl %esi,%eax
	andl $15,%eax
	orl sas_hbaud(,%eax,4),%ecx
	jmp .L1148
	.align 4
.L1149:
	movl %esi,%eax
	andl $15,%eax
	orl sas_baud(,%eax,4),%ecx
.L1148:
.L_LC2191:

.section	.line
	.4byte	3433	/ sas.c:3433
	.2byte	0xffff
	.4byte	.L_LC2191-.L_text_b
	.previous
	cmpl $0,12(%ebp)
	jne .L1151
	movl 20(%ebx),%edi
	andl $-16,%edi
	testb $32,32(%ebx)
	je .L1152
	movl 20(%ebx),%eax
	andl $15,%eax
	orw sas_baud(,%eax,4),%di
	movl %edi,%eax
	xorw %si,%ax
	jmp .L1208
	.align 4
.L1152:
	movl %esi,%eax
	xorw %di,%ax
.L1208:
	testw $895,%ax
	jne .L1151
.L_LC2192:

.section	.line
	.4byte	3440	/ sas.c:3440
	.2byte	0xffff
	.4byte	.L_LC2192-.L_text_b
	.previous
	movl %ecx,%esi
.L_LC2193:

.section	.line
	.4byte	3441	/ sas.c:3441
	.2byte	0xffff
	.4byte	.L_LC2193-.L_text_b
	.previous
	jmp .L1154
	.align 4
.L1151:
.L_LC2194:

.section	.line
	.4byte	3445	/ sas.c:3445
	.2byte	0xffff
	.4byte	.L_LC2194-.L_text_b
	.previous
	testl $15,%esi
	jne .L1155
.L_LC2195:

.section	.line
	.4byte	3447	/ sas.c:3447
	.2byte	0xffff
	.4byte	.L_LC2195-.L_text_b
	.previous
	testb $32,32(%ebx)
	je .L1163
.L_LC2196:

.section	.line
	.4byte	3449	/ sas.c:3449
	.2byte	0xffff
	.4byte	.L_LC2196-.L_text_b
	.previous
	movzbl 62(%ebx),%esi
	testb $4,44(%ebx)
	je .L1157
	movzbl 54(%ebx),%eax
	jmp .L1209
	.align 4
.L1157:
	movzbl 53(%ebx),%eax
.L1209:
	notl %eax
	andl %eax,%esi
	movl %esi,%eax
	movb %al,62(%ebx)
.L_LC2197:

.section	.line
	.4byte	3452	/ sas.c:3452
	.2byte	0xffff
	.4byte	.L_LC2197-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L1160
.L_B304:
.L_LC2198:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2198-.L_text_b
	.previous
.L_LC2199:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2199-.L_text_b
	.previous
	movb 90(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC2200:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2200-.L_text_b
	.previous
.L_B304_e:
.L_LC2201:

.section	.line
	.4byte	3452	/ sas.c:3452
	.2byte	0xffff
	.4byte	.L_LC2201-.L_text_b
	.previous
.L1160:
.L_B305:
.L_LC2202:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2202-.L_text_b
	.previous
.L_LC2203:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2203-.L_text_b
	.previous
	movb 62(%ebx),%al
	movw 88(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC2204:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2204-.L_text_b
	.previous
.L_B305_e:
.L_LC2205:

.section	.line
	.4byte	3453	/ sas.c:3453
	.2byte	0xffff
	.4byte	.L_LC2205-.L_text_b
	.previous
	andb $223,32(%ebx)
.L_LC2206:

.section	.line
	.4byte	3455	/ sas.c:3455
	.2byte	0xffff
	.4byte	.L_LC2206-.L_text_b
	.previous
	jmp .L1163
	.align 4
.L1155:
.L_LC2207:

.section	.line
	.4byte	3458	/ sas.c:3458
	.2byte	0xffff
	.4byte	.L_LC2207-.L_text_b
	.previous
	testb $32,32(%ebx)
	jne .L1163
.L_LC2208:

.section	.line
	.4byte	3460	/ sas.c:3460
	.2byte	0xffff
	.4byte	.L_LC2208-.L_text_b
	.previous
	movzbl 62(%ebx),%esi
	testb $4,44(%ebx)
	je .L1165
	movzbl 54(%ebx),%eax
	jmp .L1210
	.align 4
.L1165:
	movzbl 53(%ebx),%eax
.L1210:
	orl %eax,%esi
	movl %esi,%eax
	movb %al,62(%ebx)
.L_LC2209:

.section	.line
	.4byte	3463	/ sas.c:3463
	.2byte	0xffff
	.4byte	.L_LC2209-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L1168
.L_B306:
.L_LC2210:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2210-.L_text_b
	.previous
.L_LC2211:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2211-.L_text_b
	.previous
	movb 90(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC2212:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2212-.L_text_b
	.previous
.L_B306_e:
.L_LC2213:

.section	.line
	.4byte	3463	/ sas.c:3463
	.2byte	0xffff
	.4byte	.L_LC2213-.L_text_b
	.previous
.L1168:
.L_B307:
.L_LC2214:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2214-.L_text_b
	.previous
.L_LC2215:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2215-.L_text_b
	.previous
	movb 62(%ebx),%al
	movw 88(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC2216:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2216-.L_text_b
	.previous
.L_B307_e:
.L_LC2217:

.section	.line
	.4byte	3464	/ sas.c:3464
	.2byte	0xffff
	.4byte	.L_LC2217-.L_text_b
	.previous
	orb $32,32(%ebx)
.L1163:
.L_LC2218:

.section	.line
	.4byte	3468	/ sas.c:3468
	.2byte	0xffff
	.4byte	.L_LC2218-.L_text_b
	.previous
	movl %ecx,%esi
.L_LC2219:

.section	.line
	.4byte	3471	/ sas.c:3471
	.2byte	0xffff
	.4byte	.L_LC2219-.L_text_b
	.previous
	andb $64,63(%ebx)
.L_LC2220:

.section	.line
	.4byte	3474	/ sas.c:3474
	.2byte	0xffff
	.4byte	.L_LC2220-.L_text_b
	.previous
	andl $48,%ecx
	cmpl $16,%ecx
	je .L1173
	ja .L1177
	testl %ecx,%ecx
	je .L1172
	jmp .L1175
	.align 4
.L1177:
	cmpl $32,%ecx
	je .L1174
	jmp .L1175
	.align 4
.L1172:
.L_LC2221:

.section	.line
	.4byte	3477	/ sas.c:3477
	.2byte	0xffff
	.4byte	.L_LC2221-.L_text_b
	.previous
	movb 63(%ebx),%al
	movb %al,63(%ebx)
.L_LC2222:

.section	.line
	.4byte	3478	/ sas.c:3478
	.2byte	0xffff
	.4byte	.L_LC2222-.L_text_b
	.previous
	jmp .L1171
	.align 4
.L1173:
.L_LC2223:

.section	.line
	.4byte	3481	/ sas.c:3481
	.2byte	0xffff
	.4byte	.L_LC2223-.L_text_b
	.previous
	orb $1,63(%ebx)
.L_LC2224:

.section	.line
	.4byte	3482	/ sas.c:3482
	.2byte	0xffff
	.4byte	.L_LC2224-.L_text_b
	.previous
	jmp .L1171
	.align 4
.L1174:
.L_LC2225:

.section	.line
	.4byte	3485	/ sas.c:3485
	.2byte	0xffff
	.4byte	.L_LC2225-.L_text_b
	.previous
	orb $2,63(%ebx)
.L_LC2226:

.section	.line
	.4byte	3486	/ sas.c:3486
	.2byte	0xffff
	.4byte	.L_LC2226-.L_text_b
	.previous
	jmp .L1171
	.align 4
.L1175:
.L_LC2227:

.section	.line
	.4byte	3489	/ sas.c:3489
	.2byte	0xffff
	.4byte	.L_LC2227-.L_text_b
	.previous
	orb $3,63(%ebx)
.L1171:
.L_LC2228:

.section	.line
	.4byte	3494	/ sas.c:3494
	.2byte	0xffff
	.4byte	.L_LC2228-.L_text_b
	.previous
	testl $64,%esi
	je .L1178
.L_LC2229:

.section	.line
	.4byte	3495	/ sas.c:3495
	.2byte	0xffff
	.4byte	.L_LC2229-.L_text_b
	.previous
	orb $4,63(%ebx)
.L1178:
.L_LC2230:

.section	.line
	.4byte	3498	/ sas.c:3498
	.2byte	0xffff
	.4byte	.L_LC2230-.L_text_b
	.previous
	testl $256,%esi
	je .L1179
.L_LC2231:

.section	.line
	.4byte	3500	/ sas.c:3500
	.2byte	0xffff
	.4byte	.L_LC2231-.L_text_b
	.previous
	orb $8,63(%ebx)
.L_LC2232:

.section	.line
	.4byte	3502	/ sas.c:3502
	.2byte	0xffff
	.4byte	.L_LC2232-.L_text_b
	.previous
	testl $512,%esi
	jne .L1179
.L_LC2233:

.section	.line
	.4byte	3503	/ sas.c:3503
	.2byte	0xffff
	.4byte	.L_LC2233-.L_text_b
	.previous
	orb $16,63(%ebx)
.L1179:
.L_LC2234:

.section	.line
	.4byte	3507	/ sas.c:3507
	.2byte	0xffff
	.4byte	.L_LC2234-.L_text_b
	.previous
	movl %esi,%edx
	andl $15,%edx
	movl %edx,-4(%ebp)
	movl sas_speeds(,%edx,4),%ecx
.L_LC2235:

.section	.line
	.4byte	3509	/ sas.c:3509
	.2byte	0xffff
	.4byte	.L_LC2235-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L1182
.L_B308:
.L_LC2236:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2236-.L_text_b
	.previous
.L_LC2237:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2237-.L_text_b
	.previous
	movb 86(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC2238:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2238-.L_text_b
	.previous
.L_B308_e:
.L_LC2239:

.section	.line
	.4byte	3509	/ sas.c:3509
	.2byte	0xffff
	.4byte	.L_LC2239-.L_text_b
	.previous
.L1182:
.L_B309:
.L_LC2240:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2240-.L_text_b
	.previous
	movw 84(%ebx),%di
	movb 63(%ebx),%al
	orb $128,%al
.L_LC2241:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2241-.L_text_b
	.previous
	movl %edi,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC2242:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2242-.L_text_b
	.previous
.L_B309_e:
.L_LC2243:

.section	.line
	.4byte	3510	/ sas.c:3510
	.2byte	0xffff
	.4byte	.L_LC2243-.L_text_b
	.previous
	movb 32(%ebx),%al
	andb $4,%al
	movb %al,-8(%ebp)
	je .L1186
.L_B310:
.L_LC2244:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2244-.L_text_b
	.previous
.L_LC2245:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2245-.L_text_b
	.previous
	movb 74(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC2246:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2246-.L_text_b
	.previous
.L_B310_e:
.L_LC2247:

.section	.line
	.4byte	3510	/ sas.c:3510
	.2byte	0xffff
	.4byte	.L_LC2247-.L_text_b
	.previous
.L1186:
.L_B311:
.L_LC2248:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2248-.L_text_b
	.previous
.L_LC2249:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2249-.L_text_b
	.previous
	movb %cl,%al
	movw 72(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC2250:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2250-.L_text_b
	.previous
.L_B311_e:
.L_LC2251:

.section	.line
	.4byte	3511	/ sas.c:3511
	.2byte	0xffff
	.4byte	.L_LC2251-.L_text_b
	.previous
	cmpb $0,-8(%ebp)
	je .L1190
.L_B312:
.L_LC2252:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2252-.L_text_b
	.previous
.L_LC2253:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2253-.L_text_b
	.previous
	movb 78(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC2254:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2254-.L_text_b
	.previous
.L_B312_e:
.L_LC2255:

.section	.line
	.4byte	3511	/ sas.c:3511
	.2byte	0xffff
	.4byte	.L_LC2255-.L_text_b
	.previous
.L1190:
.L_B313:
.L_LC2256:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2256-.L_text_b
	.previous
	movl %ecx,%eax
	shrl $8,%eax
.L_LC2257:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2257-.L_text_b
	.previous
	movw 76(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC2258:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2258-.L_text_b
	.previous
.L_B313_e:
.L_LC2259:

.section	.line
	.4byte	3512	/ sas.c:3512
	.2byte	0xffff
	.4byte	.L_LC2259-.L_text_b
	.previous
	cmpb $0,-8(%ebp)
	je .L1194
.L_B314:
.L_LC2260:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2260-.L_text_b
	.previous
.L_LC2261:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2261-.L_text_b
	.previous
	movb 86(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC2262:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2262-.L_text_b
	.previous
.L_B314_e:
.L_LC2263:

.section	.line
	.4byte	3512	/ sas.c:3512
	.2byte	0xffff
	.4byte	.L_LC2263-.L_text_b
	.previous
.L1194:
.L_B315:
.L_LC2264:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2264-.L_text_b
	.previous
.L_LC2265:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2265-.L_text_b
	.previous
	movb 63(%ebx),%al
	movl %edi,%edx
/APP
	outb %al,%dx
/NO_APP
.L_LC2266:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2266-.L_text_b
	.previous
.L_B315_e:
.L_LC2267:

.section	.line
	.4byte	3513	/ sas.c:3513
	.2byte	0xffff
	.4byte	.L_LC2267-.L_text_b
	.previous
	call splhi
	movl %eax,%edi
.L_LC2268:

.section	.line
	.4byte	3514	/ sas.c:3514
	.2byte	0xffff
	.4byte	.L_LC2268-.L_text_b
	.previous
	movl -4(%ebp),%eax
	movl sas_bsizes(,%eax,4),%eax
	movl %eax,108(%ebx)
.L_LC2269:

.section	.line
	.4byte	3515	/ sas.c:3515
	.2byte	0xffff
	.4byte	.L_LC2269-.L_text_b
	.previous
	cmpl $0,168(%ebx)
	je .L1197
.L_LC2270:

.section	.line
	.4byte	3516	/ sas.c:3516
	.2byte	0xffff
	.4byte	.L_LC2270-.L_text_b
	.previous
	movl $0,168(%ebx)
.L_LC2271:

.section	.line
	.4byte	3517	/ sas.c:3517
	.2byte	0xffff
	.4byte	.L_LC2271-.L_text_b
	.previous
	pushl 164(%ebx)
	leal 172(%ebx),%eax
	pushl %eax
	call sas_link
.L_LC2272:

.section	.line
	.4byte	3518	/ sas.c:3518
	.2byte	0xffff
	.4byte	.L_LC2272-.L_text_b
	.previous
	movl $0,164(%ebx)
.L_LC2273:

.section	.line
	.4byte	3519	/ sas.c:3519
	.2byte	0xffff
	.4byte	.L_LC2273-.L_text_b
	.previous
	addl $8,%esp
.L1197:
.L_LC2274:

.section	.line
	.4byte	3520	/ sas.c:3520
	.2byte	0xffff
	.4byte	.L_LC2274-.L_text_b
	.previous
	pushl %edi
	call splx
.L_LC2275:

.section	.line
	.4byte	3521	/ sas.c:3521
	.2byte	0xffff
	.4byte	.L_LC2275-.L_text_b
	.previous
	pushl $1
	pushl %ebx
	call sas_buffers
	addl $12,%esp
.L_I29_451:
.L1154:
.L_LC2276:

.section	.line
	.4byte	3525	/ sas.c:3525
	.2byte	0xffff
	.4byte	.L_LC2276-.L_text_b
	.previous
	testb $32,44(%ebx)
	je .L1198
.L_LC2277:

.section	.line
	.4byte	3526	/ sas.c:3526
	.2byte	0xffff
	.4byte	.L_LC2277-.L_text_b
	.previous
	orl $2048,%esi
.L1198:
.L_LC2278:

.section	.line
	.4byte	3535	/ sas.c:3535
	.2byte	0xffff
	.4byte	.L_LC2278-.L_text_b
	.previous
	testl $2048,%esi
	je .L1199
	testb $1,36(%ebx)
	jne .L1199
.L_LC2279:

.section	.line
	.4byte	3538	/ sas.c:3538
	.2byte	0xffff
	.4byte	.L_LC2279-.L_text_b
	.previous
	testb $32,33(%ebx)
	je .L1201
.L_LC2280:

.section	.line
	.4byte	3539	/ sas.c:3539
	.2byte	0xffff
	.4byte	.L_LC2280-.L_text_b
	.previous
	pushl %ebx
	call sas_msi_disable
.L_LC2281:

.section	.line
	.4byte	3540	/ sas.c:3540
	.2byte	0xffff
	.4byte	.L_LC2281-.L_text_b
	.previous
	jmp .L1211
	.align 4
.L1199:
.L_LC2282:

.section	.line
	.4byte	3543	/ sas.c:3543
	.2byte	0xffff
	.4byte	.L_LC2282-.L_text_b
	.previous
	testb $32,33(%ebx)
	jne .L1201
.L_LC2283:

.section	.line
	.4byte	3544	/ sas.c:3544
	.2byte	0xffff
	.4byte	.L_LC2283-.L_text_b
	.previous
	pushl %ebx
	call sas_msi_enable
.L1211:
	addl $4,%esp
.L1201:
.L_LC2284:

.section	.line
	.4byte	3550	/ sas.c:3550
	.2byte	0xffff
	.4byte	.L_LC2284-.L_text_b
	.previous
	movzbl 60(%ebx),%eax
	notl %eax
	movzbl 55(%ebx),%edx
	testl %eax,%edx
	je .L1204
	testl $2048,%esi
	jne .L1204
	testb $16,44(%ebx)
	je .L1203
	testb $3,44(%ebx)
	je .L1204
	movl 112(%ebx),%eax
	testb $16,(%eax)
	je .L1203
.L1204:
.L_LC2285:

.section	.line
	.4byte	3555	/ sas.c:3555
	.2byte	0xffff
	.4byte	.L_LC2285-.L_text_b
	.previous
	movl 112(%ebx),%eax
	orb $16,(%eax)
	jmp .L1205
	.align 4
.L1203:
.L_LC2286:

.section	.line
	.4byte	3557	/ sas.c:3557
	.2byte	0xffff
	.4byte	.L_LC2286-.L_text_b
	.previous
	movl 112(%ebx),%eax
	andl $-17,(%eax)
.L1205:
.L_LC2287:

.section	.line
	.4byte	3561	/ sas.c:3561
	.2byte	0xffff
	.4byte	.L_LC2287-.L_text_b
	.previous
	testw %si,%si
	jge .L1206
.L_LC2288:

.section	.line
	.4byte	3562	/ sas.c:3562
	.2byte	0xffff
	.4byte	.L_LC2288-.L_text_b
	.previous
	orl $1024,44(%ebx)
	jmp .L1207
	.align 4
.L1206:
.L_LC2289:

.section	.line
	.4byte	3564	/ sas.c:3564
	.2byte	0xffff
	.4byte	.L_LC2289-.L_text_b
	.previous
	andl $-1025,44(%ebx)
.L1207:
.L_LC2290:

.section	.line
	.4byte	3567	/ sas.c:3567
	.2byte	0xffff
	.4byte	.L_LC2290-.L_text_b
	.previous
	movl %esi,20(%ebx)
.L_LC2291:

.section	.line
	.4byte	3569	/ sas.c:3569
	.2byte	0xffff
	.4byte	.L_LC2291-.L_text_b
	.previous
	andw $65279,32(%ebx)
.L_LC2292:

.section	.line
	.4byte	3572	/ sas.c:3572
	.2byte	0xffff
	.4byte	.L_LC2292-.L_text_b
	.previous
	pushl %ebx
	call sas_hdx_check
.L_LC2293:

.section	.line
	.4byte	3573	/ sas.c:3573
	.2byte	0xffff
	.4byte	.L_LC2293-.L_text_b
	.previous
	movzbl 61(%ebx),%eax
	pushl %eax
	pushl %ebx
	call sas_fproc
.L_LC2294:

.section	.line
	.4byte	3576	/ sas.c:3576
	.2byte	0xffff
	.4byte	.L_LC2294-.L_text_b
	.previous
	pushl %ebx
	call sas_xproc
.L_LC2295:

.section	.line
	.4byte	3577	/ sas.c:3577
	.2byte	0xffff
	.4byte	.L_LC2295-.L_text_b
	.previous
.L_b29_e:
	leal -20(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
.L_f29_e:
.Lfe26:
	.size	 sas_param,.Lfe26-sas_param

.section	.debug
.L_D1402:
	.4byte	.L_D1402_e-.L_D1402
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D1414
	.2byte	0x38
	.string	"sas_param"
	.2byte	0x111
	.4byte	sas_param
	.2byte	0x121
	.4byte	.L_f29_e
	.2byte	0x8041
	.4byte	.L_b29
	.2byte	0x8051
	.4byte	.L_b29_e
.L_D1402_e:
.L_D1415:
	.4byte	.L_D1415_e-.L_D1415
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1416
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t1415_e-.L_t1415
.L_t1415:
	.byte	0x1
	.4byte	.L_T816
.L_t1415_e:
	.2byte	0x23
	.2byte	.L_l1415_e-.L_l1415
.L_l1415:
	.byte	0x1
	.4byte	0x3
.L_l1415_e:
.L_D1415_e:
.L_D1416:
	.4byte	.L_D1416_e-.L_D1416
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1417
	.2byte	0x38
	.string	"init_type"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1416_e-.L_l1416
.L_l1416:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l1416_e:
.L_D1416_e:
.L_D1417:
	.4byte	.L_D1417_e-.L_D1417
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1418
	.2byte	0x38
	.string	"cflag"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1417_e-.L_l1417
.L_l1417:
	.byte	0x1
	.4byte	0x6
.L_l1417_e:
.L_D1417_e:
.L_D1418:
	.4byte	.L_D1418_e-.L_D1418
	.2byte	0xa
	.2byte	0x12
	.4byte	.L_D1419
	.2byte	0x38
	.string	"setflags"
	.2byte	0x111
	.4byte	.L_I29_451
.L_D1418_e:
.L_D1419:
	.4byte	.L_D1419_e-.L_D1419
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1420
	.2byte	0x38
	.string	"regvar"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1419_e-.L_l1419
.L_l1419:
.L_l1419_e:
.L_D1419_e:
.L_D1420:
	.4byte	.L_D1420_e-.L_D1420
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1421
	.2byte	0x38
	.string	"fake_cflag"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1420_e-.L_l1420
.L_l1420:
	.byte	0x1
	.4byte	0x1
.L_l1420_e:
.L_D1420_e:
.L_D1421:
	.4byte	.L_D1421_e-.L_D1421
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1422
	.2byte	0x38
	.string	"divisor"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1421_e-.L_l1421
.L_l1421:
	.byte	0x1
	.4byte	0x1
.L_l1421_e:
.L_D1421_e:
.L_D1422:
	.4byte	.L_D1422_e-.L_D1422
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1423
	.2byte	0x38
	.string	"old_spl"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1422_e-.L_l1422
.L_l1422:
	.byte	0x1
	.4byte	0x7
.L_l1422_e:
.L_D1422_e:
.L_D1423:
	.4byte	.L_D1423_e-.L_D1423
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1424
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B304
	.2byte	0x121
	.4byte	.L_B304_e
.L_D1423_e:
.L_D1425:
	.4byte	.L_D1425_e-.L_D1425
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1426
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1425_e-.L_l1425
.L_l1425:
.L_l1425_e:
.L_D1425_e:
.L_D1426:
	.4byte	.L_D1426_e-.L_D1426
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1427
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1426_e-.L_l1426
.L_l1426:
.L_l1426_e:
.L_D1426_e:
.L_D1427:
	.4byte	0x4
.L_D1424:
	.4byte	.L_D1424_e-.L_D1424
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1428
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B305
	.2byte	0x121
	.4byte	.L_B305_e
.L_D1424_e:
.L_D1429:
	.4byte	.L_D1429_e-.L_D1429
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1430
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1429_e-.L_l1429
.L_l1429:
.L_l1429_e:
.L_D1429_e:
.L_D1430:
	.4byte	.L_D1430_e-.L_D1430
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1431
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1430_e-.L_l1430
.L_l1430:
.L_l1430_e:
.L_D1430_e:
.L_D1431:
	.4byte	0x4
.L_D1428:
	.4byte	.L_D1428_e-.L_D1428
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1432
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B306
	.2byte	0x121
	.4byte	.L_B306_e
.L_D1428_e:
.L_D1433:
	.4byte	.L_D1433_e-.L_D1433
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1434
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1433_e-.L_l1433
.L_l1433:
.L_l1433_e:
.L_D1433_e:
.L_D1434:
	.4byte	.L_D1434_e-.L_D1434
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1435
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1434_e-.L_l1434
.L_l1434:
.L_l1434_e:
.L_D1434_e:
.L_D1435:
	.4byte	0x4
.L_D1432:
	.4byte	.L_D1432_e-.L_D1432
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1436
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B307
	.2byte	0x121
	.4byte	.L_B307_e
.L_D1432_e:
.L_D1437:
	.4byte	.L_D1437_e-.L_D1437
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1438
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1437_e-.L_l1437
.L_l1437:
.L_l1437_e:
.L_D1437_e:
.L_D1438:
	.4byte	.L_D1438_e-.L_D1438
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1439
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1438_e-.L_l1438
.L_l1438:
.L_l1438_e:
.L_D1438_e:
.L_D1439:
	.4byte	0x4
.L_D1436:
	.4byte	.L_D1436_e-.L_D1436
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1440
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B308
	.2byte	0x121
	.4byte	.L_B308_e
.L_D1436_e:
.L_D1441:
	.4byte	.L_D1441_e-.L_D1441
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1442
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1441_e-.L_l1441
.L_l1441:
.L_l1441_e:
.L_D1441_e:
.L_D1442:
	.4byte	.L_D1442_e-.L_D1442
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1443
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1442_e-.L_l1442
.L_l1442:
.L_l1442_e:
.L_D1442_e:
.L_D1443:
	.4byte	0x4
.L_D1440:
	.4byte	.L_D1440_e-.L_D1440
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1444
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B309
	.2byte	0x121
	.4byte	.L_B309_e
.L_D1440_e:
.L_D1445:
	.4byte	.L_D1445_e-.L_D1445
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1446
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1445_e-.L_l1445
.L_l1445:
	.byte	0x1
	.4byte	0x7
.L_l1445_e:
.L_D1445_e:
.L_D1446:
	.4byte	.L_D1446_e-.L_D1446
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1447
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1446_e-.L_l1446
.L_l1446:
	.byte	0x1
	.4byte	0x0
.L_l1446_e:
.L_D1446_e:
.L_D1447:
	.4byte	0x4
.L_D1444:
	.4byte	.L_D1444_e-.L_D1444
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1448
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B310
	.2byte	0x121
	.4byte	.L_B310_e
.L_D1444_e:
.L_D1449:
	.4byte	.L_D1449_e-.L_D1449
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1450
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1449_e-.L_l1449
.L_l1449:
.L_l1449_e:
.L_D1449_e:
.L_D1450:
	.4byte	.L_D1450_e-.L_D1450
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1451
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1450_e-.L_l1450
.L_l1450:
.L_l1450_e:
.L_D1450_e:
.L_D1451:
	.4byte	0x4
.L_D1448:
	.4byte	.L_D1448_e-.L_D1448
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1452
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B311
	.2byte	0x121
	.4byte	.L_B311_e
.L_D1448_e:
.L_D1453:
	.4byte	.L_D1453_e-.L_D1453
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1454
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1453_e-.L_l1453
.L_l1453:
.L_l1453_e:
.L_D1453_e:
.L_D1454:
	.4byte	.L_D1454_e-.L_D1454
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1455
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1454_e-.L_l1454
.L_l1454:
.L_l1454_e:
.L_D1454_e:
.L_D1455:
	.4byte	0x4
.L_D1452:
	.4byte	.L_D1452_e-.L_D1452
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1456
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B312
	.2byte	0x121
	.4byte	.L_B312_e
.L_D1452_e:
.L_D1457:
	.4byte	.L_D1457_e-.L_D1457
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1458
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1457_e-.L_l1457
.L_l1457:
.L_l1457_e:
.L_D1457_e:
.L_D1458:
	.4byte	.L_D1458_e-.L_D1458
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1459
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1458_e-.L_l1458
.L_l1458:
.L_l1458_e:
.L_D1458_e:
.L_D1459:
	.4byte	0x4
.L_D1456:
	.4byte	.L_D1456_e-.L_D1456
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1460
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B313
	.2byte	0x121
	.4byte	.L_B313_e
.L_D1456_e:
.L_D1461:
	.4byte	.L_D1461_e-.L_D1461
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1462
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1461_e-.L_l1461
.L_l1461:
.L_l1461_e:
.L_D1461_e:
.L_D1462:
	.4byte	.L_D1462_e-.L_D1462
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1463
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1462_e-.L_l1462
.L_l1462:
.L_l1462_e:
.L_D1462_e:
.L_D1463:
	.4byte	0x4
.L_D1460:
	.4byte	.L_D1460_e-.L_D1460
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1464
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B314
	.2byte	0x121
	.4byte	.L_B314_e
.L_D1460_e:
.L_D1465:
	.4byte	.L_D1465_e-.L_D1465
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1466
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1465_e-.L_l1465
.L_l1465:
.L_l1465_e:
.L_D1465_e:
.L_D1466:
	.4byte	.L_D1466_e-.L_D1466
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1467
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1466_e-.L_l1466
.L_l1466:
.L_l1466_e:
.L_D1466_e:
.L_D1467:
	.4byte	0x4
.L_D1464:
	.4byte	.L_D1464_e-.L_D1464
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1468
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B315
	.2byte	0x121
	.4byte	.L_B315_e
.L_D1464_e:
.L_D1469:
	.4byte	.L_D1469_e-.L_D1469
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1470
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1469_e-.L_l1469
.L_l1469:
.L_l1469_e:
.L_D1469_e:
.L_D1470:
	.4byte	.L_D1470_e-.L_D1470
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1471
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1470_e-.L_l1470
.L_l1470:
.L_l1470_e:
.L_D1470_e:
.L_D1471:
	.4byte	0x4
.L_D1468:
	.4byte	0x4
	.previous
	.align 4
.globl sas_rsrv
	.type	 sas_rsrv,@function
sas_rsrv:
.L_LC2296:

.section	.line
	.4byte	3583	/ sas.c:3583
	.2byte	0xffff
	.4byte	.L_LC2296-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	subl $4,%esp
	pushl %esi
	pushl %ebx
	movl 8(%ebp),%eax
.L_b30:
.L_LC2297:

.section	.line
	.4byte	3584	/ sas.c:3584
	.2byte	0xffff
	.4byte	.L_LC2297-.L_text_b
	.previous
.L_LC2298:

.section	.line
	.4byte	3591	/ sas.c:3591
	.2byte	0xffff
	.4byte	.L_LC2298-.L_text_b
	.previous
	movl 20(%eax),%eax
	movl 8(%eax),%ebx
.L_LC2299:

.section	.line
	.4byte	3592	/ sas.c:3592
	.2byte	0xffff
	.4byte	.L_LC2299-.L_text_b
	.previous
	pushl $1
	pushl %ebx
	call sas_buffers
.L_LC2300:

.section	.line
	.4byte	3593	/ sas.c:3593
	.2byte	0xffff
	.4byte	.L_LC2300-.L_text_b
	.previous
	call spl6
	movl %eax,%esi
.L_LC2301:

.section	.line
	.4byte	3596	/ sas.c:3596
	.2byte	0xffff
	.4byte	.L_LC2301-.L_text_b
	.previous
	addl $8,%esp
	cmpl $0,104(%ebx)
	jne .L1213
	cmpl $0,180(%ebx)
	je .L1228
.L1213:
.L_LC2302:

.section	.line
	.4byte	3602	/ sas.c:3602
	.2byte	0xffff
	.4byte	.L_LC2302-.L_text_b
	.previous
	movl 156(%ebx),%eax
	pushl 12(%eax)
	call canput
	addl $4,%esp
	testl %eax,%eax
	jne .L1214
.L_LC2303:

.section	.line
	.4byte	3603	/ sas.c:3603
	.2byte	0xffff
	.4byte	.L_LC2303-.L_text_b
	.previous
	movl 36(%ebx),%eax
	andl $18,%eax
	cmpl $2,%eax
	jne .L1215
.L_LC2304:

.section	.line
	.4byte	3605	/ sas.c:3605
	.2byte	0xffff
	.4byte	.L_LC2304-.L_text_b
	.previous
	movb 56(%ebx),%al
	notb %al
	movb %al,%cl
	andb 62(%ebx),%cl
	movb %cl,62(%ebx)
.L_LC2305:

.section	.line
	.4byte	3606	/ sas.c:3606
	.2byte	0xffff
	.4byte	.L_LC2305-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L1217
.L_B317:
.L_LC2306:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2306-.L_text_b
	.previous
.L_LC2307:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2307-.L_text_b
	.previous
	movb 90(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC2308:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2308-.L_text_b
	.previous
.L_B317_e:
.L_LC2309:

.section	.line
	.4byte	3606	/ sas.c:3606
	.2byte	0xffff
	.4byte	.L_LC2309-.L_text_b
	.previous
.L1217:
.L_B318:
.L_LC2310:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2310-.L_text_b
	.previous
.L_LC2311:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2311-.L_text_b
	.previous
	movb %cl,%al
	movw 88(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC2312:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2312-.L_text_b
	.previous
.L_B318_e:
.L_LC2313:

.section	.line
	.4byte	3607	/ sas.c:3607
	.2byte	0xffff
	.4byte	.L_LC2313-.L_text_b
	.previous
	orb $16,36(%ebx)
.L1215:
.L_LC2314:

.section	.line
	.4byte	3609	/ sas.c:3609
	.2byte	0xffff
	.4byte	.L_LC2314-.L_text_b
	.previous
	testb $16,13(%ebx)
	je .L1228
	cmpb $0,36(%ebx)
	jl .L1228
.L_LC2315:

.section	.line
	.4byte	3611	/ sas.c:3611
	.2byte	0xffff
	.4byte	.L_LC2315-.L_text_b
	.previous
	orw $384,36(%ebx)
.L_LC2316:

.section	.line
	.4byte	3612	/ sas.c:3612
	.2byte	0xffff
	.4byte	.L_LC2316-.L_text_b
	.previous
	movl 112(%ebx),%eax
	orl $32800,(%eax)
.L_LC2317:

.section	.line
	.4byte	3613	/ sas.c:3613
	.2byte	0xffff
	.4byte	.L_LC2317-.L_text_b
	.previous
	pushl %ebx
	call sas_hdx_check
.L_LC2318:

.section	.line
	.4byte	3614	/ sas.c:3614
	.2byte	0xffff
	.4byte	.L_LC2318-.L_text_b
	.previous
	pushl %ebx
	call sas_xproc
.L_LC2319:

.section	.line
	.4byte	3615	/ sas.c:3615
	.2byte	0xffff
	.4byte	.L_LC2319-.L_text_b
	.previous
	addl $8,%esp
.L_LC2320:

.section	.line
	.4byte	3617	/ sas.c:3617
	.2byte	0xffff
	.4byte	.L_LC2320-.L_text_b
	.previous
	jmp .L1228
	.align 4
.L1214:
.L_LC2321:

.section	.line
	.4byte	3620	/ sas.c:3620
	.2byte	0xffff
	.4byte	.L_LC2321-.L_text_b
	.previous
	testb $16,36(%ebx)
	je .L1221
.L_LC2322:

.section	.line
	.4byte	3621	/ sas.c:3621
	.2byte	0xffff
	.4byte	.L_LC2322-.L_text_b
	.previous
	andb $239,36(%ebx)
.L_LC2323:

.section	.line
	.4byte	3622	/ sas.c:3622
	.2byte	0xffff
	.4byte	.L_LC2323-.L_text_b
	.previous
	movb 62(%ebx),%cl
	orb 56(%ebx),%cl
	movb %cl,62(%ebx)
.L_LC2324:

.section	.line
	.4byte	3623	/ sas.c:3623
	.2byte	0xffff
	.4byte	.L_LC2324-.L_text_b
	.previous
	testb $6,32(%ebx)
	je .L1223
.L_B319:
.L_LC2325:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2325-.L_text_b
	.previous
.L_LC2326:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2326-.L_text_b
	.previous
	movb 90(%ebx),%al
	movw 68(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC2327:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2327-.L_text_b
	.previous
.L_B319_e:
.L_LC2328:

.section	.line
	.4byte	3623	/ sas.c:3623
	.2byte	0xffff
	.4byte	.L_LC2328-.L_text_b
	.previous
.L1223:
.L_B320:
.L_LC2329:

.section	.line
	.4byte	103	/ sas.c:103
	.2byte	0xffff
	.4byte	.L_LC2329-.L_text_b
	.previous
.L_LC2330:

.section	.line
	.4byte	105	/ sas.c:105
	.2byte	0xffff
	.4byte	.L_LC2330-.L_text_b
	.previous
	movb %cl,%al
	movw 88(%ebx),%dx
/APP
	outb %al,%dx
/NO_APP
.L_LC2331:

.section	.line
	.4byte	106	/ sas.c:106
	.2byte	0xffff
	.4byte	.L_LC2331-.L_text_b
	.previous
.L_B320_e:
.L1221:
.L_LC2332:

.section	.line
	.4byte	3625	/ sas.c:3625
	.2byte	0xffff
	.4byte	.L_LC2332-.L_text_b
	.previous
	cmpb $0,36(%ebx)
	jge .L1226
.L_LC2333:

.section	.line
	.4byte	3626	/ sas.c:3626
	.2byte	0xffff
	.4byte	.L_LC2333-.L_text_b
	.previous
	andb $127,36(%ebx)
.L_LC2334:

.section	.line
	.4byte	3627	/ sas.c:3627
	.2byte	0xffff
	.4byte	.L_LC2334-.L_text_b
	.previous
	orw $256,36(%ebx)
.L_LC2335:

.section	.line
	.4byte	3628	/ sas.c:3628
	.2byte	0xffff
	.4byte	.L_LC2335-.L_text_b
	.previous
	movl 112(%ebx),%eax
	orl $16416,(%eax)
.L_LC2336:

.section	.line
	.4byte	3629	/ sas.c:3629
	.2byte	0xffff
	.4byte	.L_LC2336-.L_text_b
	.previous
	pushl %ebx
	call sas_hdx_check
.L_LC2337:

.section	.line
	.4byte	3630	/ sas.c:3630
	.2byte	0xffff
	.4byte	.L_LC2337-.L_text_b
	.previous
	pushl %ebx
	call sas_xproc
.L_LC2338:

.section	.line
	.4byte	3631	/ sas.c:3631
	.2byte	0xffff
	.4byte	.L_LC2338-.L_text_b
	.previous
	addl $8,%esp
.L1226:
.L_LC2339:

.section	.line
	.4byte	3642	/ sas.c:3642
	.2byte	0xffff
	.4byte	.L_LC2339-.L_text_b
	.previous
	movl $0,-4(%ebp)
.L_LC2340:

.section	.line
	.4byte	3643	/ sas.c:3643
	.2byte	0xffff
	.4byte	.L_LC2340-.L_text_b
	.previous
	call splhi
.L_LC2341:

.section	.line
	.4byte	3644	/ sas.c:3644
	.2byte	0xffff
	.4byte	.L_LC2341-.L_text_b
	.previous
	movl 180(%ebx),%eax
	movl %eax,-4(%ebp)
.L_LC2342:

.section	.line
	.4byte	3645	/ sas.c:3645
	.2byte	0xffff
	.4byte	.L_LC2342-.L_text_b
	.previous
	movl $0,180(%ebx)
.L_LC2343:

.section	.line
	.4byte	3646	/ sas.c:3646
	.2byte	0xffff
	.4byte	.L_LC2343-.L_text_b
	.previous
	cmpl $0,104(%ebx)
	je .L1227
.L_LC2344:

.section	.line
	.4byte	3647	/ sas.c:3647
	.2byte	0xffff
	.4byte	.L_LC2344-.L_text_b
	.previous
	pushl 176(%ebx)
	leal -4(%ebp),%eax
	pushl %eax
	call sas_link
.L_LC2345:

.section	.line
	.4byte	3648	/ sas.c:3648
	.2byte	0xffff
	.4byte	.L_LC2345-.L_text_b
	.previous
	movl $0,176(%ebx)
.L_LC2346:

.section	.line
	.4byte	3649	/ sas.c:3649
	.2byte	0xffff
	.4byte	.L_LC2346-.L_text_b
	.previous
	movl $0,104(%ebx)
.L_LC2347:

.section	.line
	.4byte	3650	/ sas.c:3650
	.2byte	0xffff
	.4byte	.L_LC2347-.L_text_b
	.previous
	addl $8,%esp
.L1227:
.L_LC2348:

.section	.line
	.4byte	3651	/ sas.c:3651
	.2byte	0xffff
	.4byte	.L_LC2348-.L_text_b
	.previous
	call spl6
.L_LC2349:

.section	.line
	.4byte	3653	/ sas.c:3653
	.2byte	0xffff
	.4byte	.L_LC2349-.L_text_b
	.previous
	cmpl $0,-4(%ebp)
	je .L1228
.L_LC2350:

.section	.line
	.4byte	3654	/ sas.c:3654
	.2byte	0xffff
	.4byte	.L_LC2350-.L_text_b
	.previous
	pushl -4(%ebp)
	call msgdsize
	addl %eax,sysinfo+144
.L_LC2351:

.section	.line
	.4byte	3655	/ sas.c:3655
	.2byte	0xffff
	.4byte	.L_LC2351-.L_text_b
	.previous
	movl 156(%ebx),%eax
	movl 12(%eax),%eax
	movl (%eax),%edx
	pushl -4(%ebp)
	pushl %eax
	movl (%edx),%eax
	call *%eax
.L_LC2352:

.section	.line
	.4byte	3656	/ sas.c:3656
	.2byte	0xffff
	.4byte	.L_LC2352-.L_text_b
	.previous
	addl $12,%esp
.L1228:
.L_LC2353:

.section	.line
	.4byte	3657	/ sas.c:3657
	.2byte	0xffff
	.4byte	.L_LC2353-.L_text_b
	.previous
	pushl %esi
	call splx
.L_LC2354:

.section	.line
	.4byte	3658	/ sas.c:3658
	.2byte	0xffff
	.4byte	.L_LC2354-.L_text_b
	.previous
.L_b30_e:
	leal -12(%ebp),%esp
	popl %ebx
	popl %esi
	leave
	ret
.L_f30_e:
.Lfe27:
	.size	 sas_rsrv,.Lfe27-sas_rsrv

.section	.debug_pubnames
	.4byte	.L_P9
	.string	"sas_rsrv"
	.previous

.section	.debug
.L_P9:
.L_D1414:
	.4byte	.L_D1414_e-.L_D1414
	.2byte	0x6
	.2byte	0x12
	.4byte	.L_D1472
	.2byte	0x38
	.string	"sas_rsrv"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x111
	.4byte	sas_rsrv
	.2byte	0x121
	.4byte	.L_f30_e
	.2byte	0x8041
	.4byte	.L_b30
	.2byte	0x8051
	.4byte	.L_b30_e
.L_D1414_e:
.L_D1473:
	.4byte	.L_D1473_e-.L_D1473
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1474
	.2byte	0x38
	.string	"q"
	.2byte	0x83
	.2byte	.L_t1473_e-.L_t1473
.L_t1473:
	.byte	0x1
	.4byte	.L_T664
.L_t1473_e:
	.2byte	0x23
	.2byte	.L_l1473_e-.L_l1473
.L_l1473:
	.byte	0x1
	.4byte	0x0
.L_l1473_e:
.L_D1473_e:
.L_D1474:
	.4byte	.L_D1474_e-.L_D1474
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1475
	.2byte	0x38
	.string	"regvar"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1474_e-.L_l1474
.L_l1474:
.L_l1474_e:
.L_D1474_e:
.L_D1475:
	.4byte	.L_D1475_e-.L_D1475
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1476
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t1475_e-.L_t1475
.L_t1475:
	.byte	0x1
	.4byte	.L_T816
.L_t1475_e:
	.2byte	0x23
	.2byte	.L_l1475_e-.L_l1475
.L_l1475:
	.byte	0x1
	.4byte	0x3
.L_l1475_e:
.L_D1475_e:
.L_D1476:
	.4byte	.L_D1476_e-.L_D1476
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1477
	.2byte	0x38
	.string	"num_to_xfer"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1476_e-.L_l1476
.L_l1476:
.L_l1476_e:
.L_D1476_e:
.L_D1477:
	.4byte	.L_D1477_e-.L_D1477
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1478
	.2byte	0x38
	.string	"mp"
	.2byte	0x83
	.2byte	.L_t1477_e-.L_t1477
.L_t1477:
	.byte	0x1
	.4byte	.L_T667
.L_t1477_e:
	.2byte	0x23
	.2byte	.L_l1477_e-.L_l1477
.L_l1477:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xfffffffc
	.byte	0x7
.L_l1477_e:
.L_D1477_e:
.L_D1478:
	.4byte	.L_D1478_e-.L_D1478
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1479
	.2byte	0x38
	.string	"num_save"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1478_e-.L_l1478
.L_l1478:
.L_l1478_e:
.L_D1478_e:
.L_D1479:
	.4byte	.L_D1479_e-.L_D1479
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1480
	.2byte	0x38
	.string	"old_level"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1479_e-.L_l1479
.L_l1479:
	.byte	0x1
	.4byte	0x6
.L_l1479_e:
.L_D1479_e:
.L_D1480:
	.4byte	.L_D1480_e-.L_D1480
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1481
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B317
	.2byte	0x121
	.4byte	.L_B317_e
.L_D1480_e:
.L_D1482:
	.4byte	.L_D1482_e-.L_D1482
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1483
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1482_e-.L_l1482
.L_l1482:
.L_l1482_e:
.L_D1482_e:
.L_D1483:
	.4byte	.L_D1483_e-.L_D1483
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1484
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1483_e-.L_l1483
.L_l1483:
.L_l1483_e:
.L_D1483_e:
.L_D1484:
	.4byte	0x4
.L_D1481:
	.4byte	.L_D1481_e-.L_D1481
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1485
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B318
	.2byte	0x121
	.4byte	.L_B318_e
.L_D1481_e:
.L_D1486:
	.4byte	.L_D1486_e-.L_D1486
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1487
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1486_e-.L_l1486
.L_l1486:
.L_l1486_e:
.L_D1486_e:
.L_D1487:
	.4byte	.L_D1487_e-.L_D1487
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1488
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1487_e-.L_l1487
.L_l1487:
.L_l1487_e:
.L_D1487_e:
.L_D1488:
	.4byte	0x4
.L_D1485:
	.4byte	.L_D1485_e-.L_D1485
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1489
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B319
	.2byte	0x121
	.4byte	.L_B319_e
.L_D1485_e:
.L_D1490:
	.4byte	.L_D1490_e-.L_D1490
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1491
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1490_e-.L_l1490
.L_l1490:
.L_l1490_e:
.L_D1490_e:
.L_D1491:
	.4byte	.L_D1491_e-.L_D1491
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1492
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1491_e-.L_l1491
.L_l1491:
.L_l1491_e:
.L_D1491_e:
.L_D1492:
	.4byte	0x4
.L_D1489:
	.4byte	.L_D1489_e-.L_D1489
	.2byte	0x1d
	.2byte	0x12
	.4byte	.L_D1493
	.2byte	0x2b2
	.4byte	.L_E1807
	.2byte	0x111
	.4byte	.L_B320
	.2byte	0x121
	.4byte	.L_B320_e
.L_D1489_e:
.L_D1494:
	.4byte	.L_D1494_e-.L_D1494
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1495
	.2byte	0x2b2
	.4byte	.L_E1805
	.2byte	0x23
	.2byte	.L_l1494_e-.L_l1494
.L_l1494:
.L_l1494_e:
.L_D1494_e:
.L_D1495:
	.4byte	.L_D1495_e-.L_D1495
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1496
	.2byte	0x2b2
	.4byte	.L_E1806
	.2byte	0x23
	.2byte	.L_l1495_e-.L_l1495
.L_l1495:
.L_l1495_e:
.L_D1495_e:
.L_D1496:
	.4byte	0x4
.L_D1493:
	.4byte	0x4
	.previous
	.align 4
.globl sas_tcsetx
	.type	 sas_tcsetx,@function
sas_tcsetx:
.L_LC2355:

.section	.line
	.4byte	3663	/ sas.c:3663
	.2byte	0xffff
	.4byte	.L_LC2355-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	pushl %esi
	pushl %ebx
	movl 8(%ebp),%ecx
	movl 12(%ebp),%ebx
.L_b31:
.L_LC2356:

.section	.line
	.4byte	3664	/ sas.c:3664
	.2byte	0xffff
	.4byte	.L_LC2356-.L_text_b
	.previous
.L_LC2357:

.section	.line
	.4byte	3667	/ sas.c:3667
	.2byte	0xffff
	.4byte	.L_LC2357-.L_text_b
	.previous
	movl 12(%ebx),%esi
.L_LC2358:

.section	.line
	.4byte	3668	/ sas.c:3668
	.2byte	0xffff
	.4byte	.L_LC2358-.L_text_b
	.previous
	movl 8(%ebx),%eax
	movl 12(%eax),%edx
.L_LC2359:

.section	.line
	.4byte	3670	/ sas.c:3670
	.2byte	0xffff
	.4byte	.L_LC2359-.L_text_b
	.previous
	cmpw $0,2(%edx)
	jne .L1243
.L_LC2360:

.section	.line
	.4byte	3673	/ sas.c:3673
	.2byte	0xffff
	.4byte	.L_LC2360-.L_text_b
	.previous
	movw (%edx),%ax
	andw $5,%ax
	cmpw $5,%ax
	je .L1243
.L_LC2361:

.section	.line
	.4byte	3676	/ sas.c:3676
	.2byte	0xffff
	.4byte	.L_LC2361-.L_text_b
	.previous
	movw (%edx),%ax
	andw $10,%ax
	cmpw $10,%ax
	je .L1243
.L_LC2362:

.section	.line
	.4byte	3679	/ sas.c:3679
	.2byte	0xffff
	.4byte	.L_LC2362-.L_text_b
	.previous
	testb $16,(%edx)
	je .L1233
.L_LC2363:

.section	.line
	.4byte	3680	/ sas.c:3680
	.2byte	0xffff
	.4byte	.L_LC2363-.L_text_b
	.previous
.L1243:
	xorl %eax,%eax
	jmp .L1242
	.align 4
.L1233:
.L_LC2364:

.section	.line
	.4byte	3687	/ sas.c:3687
	.2byte	0xffff
	.4byte	.L_LC2364-.L_text_b
	.previous
	testb $5,(%edx)
	je .L1234
.L_LC2365:

.section	.line
	.4byte	3688	/ sas.c:3688
	.2byte	0xffff
	.4byte	.L_LC2365-.L_text_b
	.previous
	testb $1,(%edx)
	je .L1235
.L_LC2366:

.section	.line
	.4byte	3689	/ sas.c:3689
	.2byte	0xffff
	.4byte	.L_LC2366-.L_text_b
	.previous
	movb $2,56(%ecx)
.L_LC2367:

.section	.line
	.4byte	3690	/ sas.c:3690
	.2byte	0xffff
	.4byte	.L_LC2367-.L_text_b
	.previous
	movb $1,53(%ecx)
.L_LC2368:

.section	.line
	.4byte	3691	/ sas.c:3691
	.2byte	0xffff
	.4byte	.L_LC2368-.L_text_b
	.previous
	movb $1,54(%ecx)
.L1235:
.L_LC2369:

.section	.line
	.4byte	3693	/ sas.c:3693
	.2byte	0xffff
	.4byte	.L_LC2369-.L_text_b
	.previous
	testb $4,(%edx)
	je .L1236
.L_LC2370:

.section	.line
	.4byte	3694	/ sas.c:3694
	.2byte	0xffff
	.4byte	.L_LC2370-.L_text_b
	.previous
	movb $1,56(%ecx)
.L_LC2371:

.section	.line
	.4byte	3695	/ sas.c:3695
	.2byte	0xffff
	.4byte	.L_LC2371-.L_text_b
	.previous
	movb $2,53(%ecx)
.L_LC2372:

.section	.line
	.4byte	3696	/ sas.c:3696
	.2byte	0xffff
	.4byte	.L_LC2372-.L_text_b
	.previous
	movb $2,54(%ecx)
.L1236:
.L_LC2373:

.section	.line
	.4byte	3698	/ sas.c:3698
	.2byte	0xffff
	.4byte	.L_LC2373-.L_text_b
	.previous
	orb $2,36(%ecx)
.L_LC2374:

.section	.line
	.4byte	3699	/ sas.c:3699
	.2byte	0xffff
	.4byte	.L_LC2374-.L_text_b
	.previous
	jmp .L1237
	.align 4
.L1234:
.L_LC2375:

.section	.line
	.4byte	3700	/ sas.c:3700
	.2byte	0xffff
	.4byte	.L_LC2375-.L_text_b
	.previous
	andb $253,36(%ecx)
.L1237:
.L_LC2376:

.section	.line
	.4byte	3702	/ sas.c:3702
	.2byte	0xffff
	.4byte	.L_LC2376-.L_text_b
	.previous
	testb $10,(%edx)
	je .L1238
.L_LC2377:

.section	.line
	.4byte	3703	/ sas.c:3703
	.2byte	0xffff
	.4byte	.L_LC2377-.L_text_b
	.previous
	testb $2,(%edx)
	je .L1239
.L_LC2378:

.section	.line
	.4byte	3704	/ sas.c:3704
	.2byte	0xffff
	.4byte	.L_LC2378-.L_text_b
	.previous
	movb $16,57(%ecx)
.L_LC2379:

.section	.line
	.4byte	3705	/ sas.c:3705
	.2byte	0xffff
	.4byte	.L_LC2379-.L_text_b
	.previous
	movb $128,55(%ecx)
.L1239:
.L_LC2380:

.section	.line
	.4byte	3707	/ sas.c:3707
	.2byte	0xffff
	.4byte	.L_LC2380-.L_text_b
	.previous
	testb $8,(%edx)
	je .L1240
.L_LC2381:

.section	.line
	.4byte	3708	/ sas.c:3708
	.2byte	0xffff
	.4byte	.L_LC2381-.L_text_b
	.previous
	movb $128,57(%ecx)
.L_LC2382:

.section	.line
	.4byte	3709	/ sas.c:3709
	.2byte	0xffff
	.4byte	.L_LC2382-.L_text_b
	.previous
	movb $16,55(%ecx)
.L1240:
.L_LC2383:

.section	.line
	.4byte	3711	/ sas.c:3711
	.2byte	0xffff
	.4byte	.L_LC2383-.L_text_b
	.previous
	orb $1,36(%ecx)
.L_LC2384:

.section	.line
	.4byte	3712	/ sas.c:3712
	.2byte	0xffff
	.4byte	.L_LC2384-.L_text_b
	.previous
	jmp .L1241
	.align 4
.L1238:
.L_LC2385:

.section	.line
	.4byte	3713	/ sas.c:3713
	.2byte	0xffff
	.4byte	.L_LC2385-.L_text_b
	.previous
	andb $254,36(%ecx)
.L1241:
.L_LC2386:

.section	.line
	.4byte	3716	/ sas.c:3716
	.2byte	0xffff
	.4byte	.L_LC2386-.L_text_b
	.previous
	movl $0,12(%esi)
.L_LC2387:

.section	.line
	.4byte	3717	/ sas.c:3717
	.2byte	0xffff
	.4byte	.L_LC2387-.L_text_b
	.previous
	movl 20(%ebx),%eax
	movb $129,13(%eax)
.L_LC2388:

.section	.line
	.4byte	3718	/ sas.c:3718
	.2byte	0xffff
	.4byte	.L_LC2388-.L_text_b
	.previous
	movl $1,%eax
.L1242:
.L_LC2389:

.section	.line
	.4byte	3719	/ sas.c:3719
	.2byte	0xffff
	.4byte	.L_LC2389-.L_text_b
	.previous
.L_b31_e:
	leal -8(%ebp),%esp
	popl %ebx
	popl %esi
	leave
	ret
.L_f31_e:
.Lfe28:
	.size	 sas_tcsetx,.Lfe28-sas_tcsetx

.section	.debug_pubnames
	.4byte	.L_P10
	.string	"sas_tcsetx"
	.previous

.section	.debug
.L_P10:
.L_D1472:
	.4byte	.L_D1472_e-.L_D1472
	.2byte	0x6
	.2byte	0x12
	.4byte	.L_D1497
	.2byte	0x38
	.string	"sas_tcsetx"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x111
	.4byte	sas_tcsetx
	.2byte	0x121
	.4byte	.L_f31_e
	.2byte	0x8041
	.4byte	.L_b31
	.2byte	0x8051
	.4byte	.L_b31_e
.L_D1472_e:
.L_D1498:
	.4byte	.L_D1498_e-.L_D1498
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1499
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t1498_e-.L_t1498
.L_t1498:
	.byte	0x1
	.4byte	.L_T816
.L_t1498_e:
	.2byte	0x23
	.2byte	.L_l1498_e-.L_l1498
.L_l1498:
	.byte	0x1
	.4byte	0x1
.L_l1498_e:
.L_D1498_e:
.L_D1499:
	.4byte	.L_D1499_e-.L_D1499
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1500
	.2byte	0x38
	.string	"bp"
	.2byte	0x83
	.2byte	.L_t1499_e-.L_t1499
.L_t1499:
	.byte	0x1
	.4byte	.L_T667
.L_t1499_e:
	.2byte	0x23
	.2byte	.L_l1499_e-.L_l1499
.L_l1499:
	.byte	0x1
	.4byte	0x3
.L_l1499_e:
.L_D1499_e:
.L_D1500:
	.4byte	.L_D1500_e-.L_D1500
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1501
	.2byte	0x38
	.string	"xp"
	.2byte	0x83
	.2byte	.L_t1500_e-.L_t1500
.L_t1500:
	.byte	0x1
	.4byte	.L_T788
.L_t1500_e:
	.2byte	0x23
	.2byte	.L_l1500_e-.L_l1500
.L_l1500:
	.byte	0x1
	.4byte	0x2
.L_l1500_e:
.L_D1500_e:
.L_D1501:
	.4byte	.L_D1501_e-.L_D1501
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1502
	.2byte	0x38
	.string	"ioss"
	.2byte	0x83
	.2byte	.L_t1501_e-.L_t1501
.L_t1501:
	.byte	0x1
	.4byte	.L_T702
.L_t1501_e:
	.2byte	0x23
	.2byte	.L_l1501_e-.L_l1501
.L_l1501:
	.byte	0x1
	.4byte	0x6
.L_l1501_e:
.L_D1501_e:
.L_D1502:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_tcsets,@function
sas_tcsets:
.L_LC2390:

.section	.line
	.4byte	3725	/ sas.c:3725
	.2byte	0xffff
	.4byte	.L_LC2390-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	subl $4,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 8(%ebp),%esi
	movl 12(%ebp),%edi
.L_b32:
.L_LC2391:

.section	.line
	.4byte	3726	/ sas.c:3726
	.2byte	0xffff
	.4byte	.L_LC2391-.L_text_b
	.previous
.L_LC2392:

.section	.line
	.4byte	3731	/ sas.c:3731
	.2byte	0xffff
	.4byte	.L_LC2392-.L_text_b
	.previous
	movl 12(%edi),%ecx
	movl %ecx,-4(%ebp)
.L_LC2393:

.section	.line
	.4byte	3732	/ sas.c:3732
	.2byte	0xffff
	.4byte	.L_LC2393-.L_text_b
	.previous
	movl 8(%edi),%eax
	movl 12(%eax),%ebx
.L_LC2394:

.section	.line
	.4byte	3734	/ sas.c:3734
	.2byte	0xffff
	.4byte	.L_LC2394-.L_text_b
	.previous
	movl 8(%ebx),%edx
.L_LC2395:

.section	.line
	.4byte	3735	/ sas.c:3735
	.2byte	0xffff
	.4byte	.L_LC2395-.L_text_b
	.previous
	xorl %eax,%eax
	.align 4
.L1248:
	movb 16(%eax,%ebx),%cl
	movb %cl,184(%eax,%esi)
	incl %eax
	cmpl $18,%eax
	jle .L1248
.L_LC2396:

.section	.line
	.4byte	3737	/ sas.c:3737
	.2byte	0xffff
	.4byte	.L_LC2396-.L_text_b
	.previous
	pushl %edx
	pushl $0
	pushl %esi
	call sas_param
.L_LC2397:

.section	.line
	.4byte	3739	/ sas.c:3739
	.2byte	0xffff
	.4byte	.L_LC2397-.L_text_b
	.previous
	movl 16(%esi),%ecx
	movl %ecx,8(%ebx)
.L_LC2398:

.section	.line
	.4byte	3741	/ sas.c:3741
	.2byte	0xffff
	.4byte	.L_LC2398-.L_text_b
	.previous
	movl -4(%ebp),%ecx
	movl $0,12(%ecx)
.L_LC2399:

.section	.line
	.4byte	3742	/ sas.c:3742
	.2byte	0xffff
	.4byte	.L_LC2399-.L_text_b
	.previous
	movl 20(%edi),%eax
	movb $129,13(%eax)
.L_LC2400:

.section	.line
	.4byte	3744	/ sas.c:3744
	.2byte	0xffff
	.4byte	.L_LC2400-.L_text_b
	.previous
	movl 12(%ebx),%ecx
	movl %ecx,28(%esi)
.L_LC2401:

.section	.line
	.4byte	3745	/ sas.c:3745
	.2byte	0xffff
	.4byte	.L_LC2401-.L_text_b
	.previous
	movl 4(%ebx),%ecx
	movl %ecx,24(%esi)
.L_LC2402:

.section	.line
	.4byte	3746	/ sas.c:3746
	.2byte	0xffff
	.4byte	.L_LC2402-.L_text_b
	.previous
	movl (%ebx),%ebx
	movl %ebx,12(%esi)
.L_LC2403:

.section	.line
	.4byte	3748	/ sas.c:3748
	.2byte	0xffff
	.4byte	.L_LC2403-.L_text_b
	.previous
	movl $1,%eax
.L_LC2404:

.section	.line
	.4byte	3749	/ sas.c:3749
	.2byte	0xffff
	.4byte	.L_LC2404-.L_text_b
	.previous
.L_b32_e:
	leal -16(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
.L_f32_e:
.Lfe29:
	.size	 sas_tcsets,.Lfe29-sas_tcsets

.section	.debug
.L_D1497:
	.4byte	.L_D1497_e-.L_D1497
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D1503
	.2byte	0x38
	.string	"sas_tcsets"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x111
	.4byte	sas_tcsets
	.2byte	0x121
	.4byte	.L_f32_e
	.2byte	0x8041
	.4byte	.L_b32
	.2byte	0x8051
	.4byte	.L_b32_e
.L_D1497_e:
.L_D1504:
	.4byte	.L_D1504_e-.L_D1504
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1505
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t1504_e-.L_t1504
.L_t1504:
	.byte	0x1
	.4byte	.L_T816
.L_t1504_e:
	.2byte	0x23
	.2byte	.L_l1504_e-.L_l1504
.L_l1504:
	.byte	0x1
	.4byte	0x6
.L_l1504_e:
.L_D1504_e:
.L_D1505:
	.4byte	.L_D1505_e-.L_D1505
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1506
	.2byte	0x38
	.string	"bp"
	.2byte	0x83
	.2byte	.L_t1505_e-.L_t1505
.L_t1505:
	.byte	0x1
	.4byte	.L_T667
.L_t1505_e:
	.2byte	0x23
	.2byte	.L_l1505_e-.L_l1505
.L_l1505:
	.byte	0x1
	.4byte	0x7
.L_l1505_e:
.L_D1505_e:
.L_D1506:
	.4byte	.L_D1506_e-.L_D1506
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1507
	.2byte	0x38
	.string	"ioss"
	.2byte	0x83
	.2byte	.L_t1506_e-.L_t1506
.L_t1506:
	.byte	0x1
	.4byte	.L_T702
.L_t1506_e:
	.2byte	0x23
	.2byte	.L_l1506_e-.L_l1506
.L_l1506:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xfffffffc
	.byte	0x7
.L_l1506_e:
.L_D1506_e:
.L_D1507:
	.4byte	.L_D1507_e-.L_D1507
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1508
	.2byte	0x38
	.string	"tp"
	.2byte	0x83
	.2byte	.L_t1507_e-.L_t1507
.L_t1507:
	.byte	0x1
	.4byte	.L_T519
.L_t1507_e:
	.2byte	0x23
	.2byte	.L_l1507_e-.L_l1507
.L_l1507:
	.byte	0x1
	.4byte	0x3
.L_l1507_e:
.L_D1507_e:
.L_D1508:
	.4byte	.L_D1508_e-.L_D1508
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1509
	.2byte	0x38
	.string	"cflag"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1508_e-.L_l1508
.L_l1508:
	.byte	0x1
	.4byte	0x2
.L_l1508_e:
.L_D1508_e:
.L_D1509:
	.4byte	.L_D1509_e-.L_D1509
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1510
	.2byte	0x38
	.string	"x"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1509_e-.L_l1509
.L_l1509:
	.byte	0x1
	.4byte	0x0
.L_l1509_e:
.L_D1509_e:
.L_D1510:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_tcset,@function
sas_tcset:
.L_LC2405:

.section	.line
	.4byte	3756	/ sas.c:3756
	.2byte	0xffff
	.4byte	.L_LC2405-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	subl $4,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 8(%ebp),%esi
	movl 12(%ebp),%edi
.L_b33:
.L_LC2406:

.section	.line
	.4byte	3757	/ sas.c:3757
	.2byte	0xffff
	.4byte	.L_LC2406-.L_text_b
	.previous
.L_LC2407:

.section	.line
	.4byte	3762	/ sas.c:3762
	.2byte	0xffff
	.4byte	.L_LC2407-.L_text_b
	.previous
	movl 12(%edi),%ecx
	movl %ecx,-4(%ebp)
.L_LC2408:

.section	.line
	.4byte	3763	/ sas.c:3763
	.2byte	0xffff
	.4byte	.L_LC2408-.L_text_b
	.previous
	movl 8(%edi),%eax
	movl 12(%eax),%ebx
.L_LC2409:

.section	.line
	.4byte	3765	/ sas.c:3765
	.2byte	0xffff
	.4byte	.L_LC2409-.L_text_b
	.previous
	movzwl 4(%ebx),%edx
.L_LC2410:

.section	.line
	.4byte	3767	/ sas.c:3767
	.2byte	0xffff
	.4byte	.L_LC2410-.L_text_b
	.previous
	xorl %eax,%eax
	.align 4
.L1253:
	movb 9(%eax,%ebx),%cl
	movb %cl,184(%eax,%esi)
	incl %eax
	cmpl $7,%eax
	jle .L1253
.L_LC2411:

.section	.line
	.4byte	3769	/ sas.c:3769
	.2byte	0xffff
	.4byte	.L_LC2411-.L_text_b
	.previous
	movb $19,193(%esi)
.L_LC2412:

.section	.line
	.4byte	3770	/ sas.c:3770
	.2byte	0xffff
	.4byte	.L_LC2412-.L_text_b
	.previous
	movb $17,192(%esi)
.L_LC2413:

.section	.line
	.4byte	3772	/ sas.c:3772
	.2byte	0xffff
	.4byte	.L_LC2413-.L_text_b
	.previous
	pushl %edx
	pushl $0
	pushl %esi
	call sas_param
.L_LC2414:

.section	.line
	.4byte	3774	/ sas.c:3774
	.2byte	0xffff
	.4byte	.L_LC2414-.L_text_b
	.previous
	movw 16(%esi),%cx
	movw %cx,4(%ebx)
.L_LC2415:

.section	.line
	.4byte	3776	/ sas.c:3776
	.2byte	0xffff
	.4byte	.L_LC2415-.L_text_b
	.previous
	movl -4(%ebp),%ecx
	movl $0,12(%ecx)
.L_LC2416:

.section	.line
	.4byte	3777	/ sas.c:3777
	.2byte	0xffff
	.4byte	.L_LC2416-.L_text_b
	.previous
	movl 20(%edi),%eax
	movb $129,13(%eax)
.L_LC2417:

.section	.line
	.4byte	3779	/ sas.c:3779
	.2byte	0xffff
	.4byte	.L_LC2417-.L_text_b
	.previous
	movzwl 6(%ebx),%ecx
	movl %ecx,28(%esi)
.L_LC2418:

.section	.line
	.4byte	3780	/ sas.c:3780
	.2byte	0xffff
	.4byte	.L_LC2418-.L_text_b
	.previous
	movzwl 2(%ebx),%ecx
	movl %ecx,24(%esi)
.L_LC2419:

.section	.line
	.4byte	3781	/ sas.c:3781
	.2byte	0xffff
	.4byte	.L_LC2419-.L_text_b
	.previous
	movzwl (%ebx),%ebx
	movl %ebx,12(%esi)
.L_LC2420:

.section	.line
	.4byte	3783	/ sas.c:3783
	.2byte	0xffff
	.4byte	.L_LC2420-.L_text_b
	.previous
	movl $1,%eax
.L_LC2421:

.section	.line
	.4byte	3784	/ sas.c:3784
	.2byte	0xffff
	.4byte	.L_LC2421-.L_text_b
	.previous
.L_b33_e:
	leal -16(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
.L_f33_e:
.Lfe30:
	.size	 sas_tcset,.Lfe30-sas_tcset

.section	.debug
.L_D1503:
	.4byte	.L_D1503_e-.L_D1503
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D1511
	.2byte	0x38
	.string	"sas_tcset"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x111
	.4byte	sas_tcset
	.2byte	0x121
	.4byte	.L_f33_e
	.2byte	0x8041
	.4byte	.L_b33
	.2byte	0x8051
	.4byte	.L_b33_e
.L_D1503_e:
.L_D1512:
	.4byte	.L_D1512_e-.L_D1512
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1513
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t1512_e-.L_t1512
.L_t1512:
	.byte	0x1
	.4byte	.L_T816
.L_t1512_e:
	.2byte	0x23
	.2byte	.L_l1512_e-.L_l1512
.L_l1512:
	.byte	0x1
	.4byte	0x6
.L_l1512_e:
.L_D1512_e:
.L_D1513:
	.4byte	.L_D1513_e-.L_D1513
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1514
	.2byte	0x38
	.string	"bp"
	.2byte	0x83
	.2byte	.L_t1513_e-.L_t1513
.L_t1513:
	.byte	0x1
	.4byte	.L_T667
.L_t1513_e:
	.2byte	0x23
	.2byte	.L_l1513_e-.L_l1513
.L_l1513:
	.byte	0x1
	.4byte	0x7
.L_l1513_e:
.L_D1513_e:
.L_D1514:
	.4byte	.L_D1514_e-.L_D1514
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1515
	.2byte	0x38
	.string	"ioss"
	.2byte	0x83
	.2byte	.L_t1514_e-.L_t1514
.L_t1514:
	.byte	0x1
	.4byte	.L_T702
.L_t1514_e:
	.2byte	0x23
	.2byte	.L_l1514_e-.L_l1514
.L_l1514:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xfffffffc
	.byte	0x7
.L_l1514_e:
.L_D1514_e:
.L_D1515:
	.4byte	.L_D1515_e-.L_D1515
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1516
	.2byte	0x38
	.string	"tp"
	.2byte	0x83
	.2byte	.L_t1515_e-.L_t1515
.L_t1515:
	.byte	0x1
	.4byte	.L_T524
.L_t1515_e:
	.2byte	0x23
	.2byte	.L_l1515_e-.L_l1515
.L_l1515:
	.byte	0x1
	.4byte	0x3
.L_l1515_e:
.L_D1515_e:
.L_D1516:
	.4byte	.L_D1516_e-.L_D1516
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1517
	.2byte	0x38
	.string	"cflag"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1516_e-.L_l1516
.L_l1516:
	.byte	0x1
	.4byte	0x2
.L_l1516_e:
.L_D1516_e:
.L_D1517:
	.4byte	.L_D1517_e-.L_D1517
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1518
	.2byte	0x38
	.string	"x"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1517_e-.L_l1517
.L_l1517:
	.byte	0x1
	.4byte	0x0
.L_l1517_e:
.L_D1517_e:
.L_D1518:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_delay,@function
sas_delay:
.L_LC2422:

.section	.line
	.4byte	3789	/ sas.c:3789
	.2byte	0xffff
	.4byte	.L_LC2422-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	pushl %esi
	pushl %ebx
	movl 8(%ebp),%ebx
.L_b34:
.L_LC2423:

.section	.line
	.4byte	3790	/ sas.c:3790
	.2byte	0xffff
	.4byte	.L_LC2423-.L_text_b
	.previous
.L_LC2424:

.section	.line
	.4byte	3792	/ sas.c:3792
	.2byte	0xffff
	.4byte	.L_LC2424-.L_text_b
	.previous
	call spltty
	movl %eax,%esi
.L_LC2425:

.section	.line
	.4byte	3793	/ sas.c:3793
	.2byte	0xffff
	.4byte	.L_LC2425-.L_text_b
	.previous
	movl 112(%ebx),%eax
	andl $-4097,(%eax)
.L_LC2426:

.section	.line
	.4byte	3794	/ sas.c:3794
	.2byte	0xffff
	.4byte	.L_LC2426-.L_text_b
	.previous
	movl $0,116(%ebx)
.L_LC2427:

.section	.line
	.4byte	3795	/ sas.c:3795
	.2byte	0xffff
	.4byte	.L_LC2427-.L_text_b
	.previous
	movl 156(%ebx),%eax
	testb $16,28(%eax)
	je .L1255
	addl $64,%eax
.L1255:
	pushl %eax
	call qenable
.L_LC2428:

.section	.line
	.4byte	3796	/ sas.c:3796
	.2byte	0xffff
	.4byte	.L_LC2428-.L_text_b
	.previous
	pushl %esi
	call splx
.L_LC2429:

.section	.line
	.4byte	3797	/ sas.c:3797
	.2byte	0xffff
	.4byte	.L_LC2429-.L_text_b
	.previous
.L_b34_e:
	leal -8(%ebp),%esp
	popl %ebx
	popl %esi
	leave
	ret
.L_f34_e:
.Lfe31:
	.size	 sas_delay,.Lfe31-sas_delay

.section	.debug
.L_D1511:
	.4byte	.L_D1511_e-.L_D1511
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D1519
	.2byte	0x38
	.string	"sas_delay"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x111
	.4byte	sas_delay
	.2byte	0x121
	.4byte	.L_f34_e
	.2byte	0x8041
	.4byte	.L_b34
	.2byte	0x8051
	.4byte	.L_b34_e
.L_D1511_e:
.L_D1520:
	.4byte	.L_D1520_e-.L_D1520
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1521
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t1520_e-.L_t1520
.L_t1520:
	.byte	0x1
	.4byte	.L_T816
.L_t1520_e:
	.2byte	0x23
	.2byte	.L_l1520_e-.L_l1520
.L_l1520:
	.byte	0x1
	.4byte	0x3
.L_l1520_e:
.L_D1520_e:
.L_D1521:
	.4byte	.L_D1521_e-.L_D1521
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1522
	.2byte	0x38
	.string	"oldpri"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1521_e-.L_l1521
.L_l1521:
	.byte	0x1
	.4byte	0x6
.L_l1521_e:
.L_D1521_e:
.L_D1522:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_qenable,@function
sas_qenable:
.L_LC2430:

.section	.line
	.4byte	3801	/ sas.c:3801
	.2byte	0xffff
	.4byte	.L_LC2430-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
.L_b35:
.L_LC2431:

.section	.line
	.4byte	3802	/ sas.c:3802
	.2byte	0xffff
	.4byte	.L_LC2431-.L_text_b
	.previous
	pushl 8(%ebp)
	call qenable
.L_LC2432:

.section	.line
	.4byte	3803	/ sas.c:3803
	.2byte	0xffff
	.4byte	.L_LC2432-.L_text_b
	.previous
	leave
	ret
.L_b35_e:
.L_f35_e:
.Lfe32:
	.size	 sas_qenable,.Lfe32-sas_qenable

.section	.debug
.L_D1519:
	.4byte	.L_D1519_e-.L_D1519
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D1523
	.2byte	0x38
	.string	"sas_qenable"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x111
	.4byte	sas_qenable
	.2byte	0x121
	.4byte	.L_f35_e
	.2byte	0x8041
	.4byte	.L_b35
	.2byte	0x8051
	.4byte	.L_b35_e
.L_D1519_e:
.L_D1524:
	.4byte	.L_D1524_e-.L_D1524
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1525
	.2byte	0x38
	.string	"q"
	.2byte	0x83
	.2byte	.L_t1524_e-.L_t1524
.L_t1524:
	.byte	0x1
	.4byte	.L_T664
.L_t1524_e:
	.2byte	0x23
	.2byte	.L_l1524_e-.L_l1524
.L_l1524:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l1524_e:
.L_D1524_e:
.L_D1525:
	.4byte	0x4
	.previous
.section	.rodata
.LC12:
	.string	"SAS alloc error\n"
.text
	.align 4
	.type	 sas_buffers,@function
sas_buffers:
.L_LC2433:

.section	.line
	.4byte	3808	/ sas.c:3808
	.2byte	0xffff
	.4byte	.L_LC2433-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	subl $4,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 8(%ebp),%edi
.L_b36:
.L_LC2434:

.section	.line
	.4byte	3809	/ sas.c:3809
	.2byte	0xffff
	.4byte	.L_LC2434-.L_text_b
	.previous
.L_LC2435:

.section	.line
	.4byte	3812	/ sas.c:3812
	.2byte	0xffff
	.4byte	.L_LC2435-.L_text_b
	.previous
	call splhi
	movl %eax,-4(%ebp)
.L_LC2436:

.section	.line
	.4byte	3813	/ sas.c:3813
	.2byte	0xffff
	.4byte	.L_LC2436-.L_text_b
	.previous
	cmpl $0,172(%edi)
	je .L1258
.L_LC2437:

.section	.line
	.4byte	3814	/ sas.c:3814
	.2byte	0xffff
	.4byte	.L_LC2437-.L_text_b
	.previous
	movl 172(%edi),%esi
.L_LC2438:

.section	.line
	.4byte	3815	/ sas.c:3815
	.2byte	0xffff
	.4byte	.L_LC2438-.L_text_b
	.previous
	movl $0,172(%edi)
.L_LC2439:

.section	.line
	.4byte	3816	/ sas.c:3816
	.2byte	0xffff
	.4byte	.L_LC2439-.L_text_b
	.previous
	jmp .L1259
	.align 4
.L1258:
.L_LC2440:

.section	.line
	.4byte	3817	/ sas.c:3817
	.2byte	0xffff
	.4byte	.L_LC2440-.L_text_b
	.previous
	xorl %esi,%esi
.L1259:
.L_LC2441:

.section	.line
	.4byte	3819	/ sas.c:3819
	.2byte	0xffff
	.4byte	.L_LC2441-.L_text_b
	.previous
	pushl -4(%ebp)
	call splx
.L_LC2442:

.section	.line
	.4byte	3820	/ sas.c:3820
	.2byte	0xffff
	.4byte	.L_LC2442-.L_text_b
	.previous
	addl $4,%esp
	cmpl $0,12(%ebp)
	je .L1260
.L_LC2443:

.section	.line
	.4byte	3821	/ sas.c:3821
	.2byte	0xffff
	.4byte	.L_LC2443-.L_text_b
	.previous
	cmpl $3,168(%edi)
	jg .L1260
	.align 4
.L1272:
.L_LC2444:

.section	.line
	.4byte	3822	/ sas.c:3822
	.2byte	0xffff
	.4byte	.L_LC2444-.L_text_b
	.previous
	xorl %ebx,%ebx
.L_LC2445:

.section	.line
	.4byte	3823	/ sas.c:3823
	.2byte	0xffff
	.4byte	.L_LC2445-.L_text_b
	.previous
.L1275:
	testl %esi,%esi
	je .L1264
.L_LC2446:

.section	.line
	.4byte	3824	/ sas.c:3824
	.2byte	0xffff
	.4byte	.L_LC2446-.L_text_b
	.previous
	movl 20(%esi),%edx
	movl 16(%edx),%eax
	cmpl %eax,108(%edi)
	ja .L1266
	cmpb $1,12(%edx)
	je .L1265
.L1266:
.L_LC2447:

.section	.line
	.4byte	3827	/ sas.c:3827
	.2byte	0xffff
	.4byte	.L_LC2447-.L_text_b
	.previous
	movl 8(%esi),%ebx
.L_LC2448:

.section	.line
	.4byte	3828	/ sas.c:3828
	.2byte	0xffff
	.4byte	.L_LC2448-.L_text_b
	.previous
	movl $0,8(%esi)
.L_LC2449:

.section	.line
	.4byte	3829	/ sas.c:3829
	.2byte	0xffff
	.4byte	.L_LC2449-.L_text_b
	.previous
	pushl %esi
	call freeb
.L_LC2450:

.section	.line
	.4byte	3830	/ sas.c:3830
	.2byte	0xffff
	.4byte	.L_LC2450-.L_text_b
	.previous
	movl %ebx,%esi
.L_LC2451:

.section	.line
	.4byte	3831	/ sas.c:3831
	.2byte	0xffff
	.4byte	.L_LC2451-.L_text_b
	.previous
	addl $4,%esp
	jmp .L1275
	.align 4
.L1265:
.L_LC2452:

.section	.line
	.4byte	3833	/ sas.c:3833
	.2byte	0xffff
	.4byte	.L_LC2452-.L_text_b
	.previous
	movl %esi,%ebx
.L_LC2453:

.section	.line
	.4byte	3834	/ sas.c:3834
	.2byte	0xffff
	.4byte	.L_LC2453-.L_text_b
	.previous
	movl 8(%esi),%esi
.L_LC2454:

.section	.line
	.4byte	3835	/ sas.c:3835
	.2byte	0xffff
	.4byte	.L_LC2454-.L_text_b
	.previous
	movl 20(%ebx),%eax
	movl 4(%eax),%eax
	movl %eax,16(%ebx)
	movl %eax,12(%ebx)
.L_LC2455:

.section	.line
	.4byte	3836	/ sas.c:3836
	.2byte	0xffff
	.4byte	.L_LC2455-.L_text_b
	.previous
	movl $0,8(%ebx)
.L1264:
.L_LC2456:

.section	.line
	.4byte	3839	/ sas.c:3839
	.2byte	0xffff
	.4byte	.L_LC2456-.L_text_b
	.previous
	testl %ebx,%ebx
	jne .L1274
.L_LC2457:

.section	.line
	.4byte	3840	/ sas.c:3840
	.2byte	0xffff
	.4byte	.L_LC2457-.L_text_b
	.previous
	pushl $2
	pushl 108(%edi)
	call allocb
	movl %eax,%ebx
	addl $8,%esp
	testl %ebx,%ebx
	jne .L1274
.L_LC2458:

.section	.line
	.4byte	3841	/ sas.c:3841
	.2byte	0xffff
	.4byte	.L_LC2458-.L_text_b
	.previous
	pushl $.LC12
	call printf
.L_LC2459:

.section	.line
	.4byte	3842	/ sas.c:3842
	.2byte	0xffff
	.4byte	.L_LC2459-.L_text_b
	.previous
	addl $4,%esp
.L_LC2460:

.section	.line
	.4byte	3844	/ sas.c:3844
	.2byte	0xffff
	.4byte	.L_LC2460-.L_text_b
	.previous
	testl %ebx,%ebx
	je .L1260
.L1274:
.L_LC2461:

.section	.line
	.4byte	3845	/ sas.c:3845
	.2byte	0xffff
	.4byte	.L_LC2461-.L_text_b
	.previous
	call splhi
.L_LC2462:

.section	.line
	.4byte	3846	/ sas.c:3846
	.2byte	0xffff
	.4byte	.L_LC2462-.L_text_b
	.previous
	movl 164(%edi),%ecx
	movl %ecx,8(%ebx)
.L_LC2463:

.section	.line
	.4byte	3847	/ sas.c:3847
	.2byte	0xffff
	.4byte	.L_LC2463-.L_text_b
	.previous
	movl %ebx,164(%edi)
.L_LC2464:

.section	.line
	.4byte	3848	/ sas.c:3848
	.2byte	0xffff
	.4byte	.L_LC2464-.L_text_b
	.previous
	incl 168(%edi)
.L_LC2465:

.section	.line
	.4byte	3849	/ sas.c:3849
	.2byte	0xffff
	.4byte	.L_LC2465-.L_text_b
	.previous
	pushl -4(%ebp)
	call splx
.L_LC2466:

.section	.line
	.4byte	3850	/ sas.c:3850
	.2byte	0xffff
	.4byte	.L_LC2466-.L_text_b
	.previous
	addl $4,%esp
.L_LC2467:

.section	.line
	.4byte	3853	/ sas.c:3853
	.2byte	0xffff
	.4byte	.L_LC2467-.L_text_b
	.previous
	cmpl $3,168(%edi)
	jle .L1272
.L1260:
.L_LC2468:

.section	.line
	.4byte	3855	/ sas.c:3855
	.2byte	0xffff
	.4byte	.L_LC2468-.L_text_b
	.previous
	testl %esi,%esi
	je .L1273
.L_LC2469:

.section	.line
	.4byte	3856	/ sas.c:3856
	.2byte	0xffff
	.4byte	.L_LC2469-.L_text_b
	.previous
	pushl %esi
	call freemsg
.L1273:
.L_LC2470:

.section	.line
	.4byte	3857	/ sas.c:3857
	.2byte	0xffff
	.4byte	.L_LC2470-.L_text_b
	.previous
.L_b36_e:
	leal -16(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
.L_f36_e:
.Lfe33:
	.size	 sas_buffers,.Lfe33-sas_buffers

.section	.debug
.L_D1523:
	.4byte	.L_D1523_e-.L_D1523
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D1526
	.2byte	0x38
	.string	"sas_buffers"
	.2byte	0x278
	.string	""
	.2byte	0x55
	.2byte	0x7
	.2byte	0x111
	.4byte	sas_buffers
	.2byte	0x121
	.4byte	.L_f36_e
	.2byte	0x8041
	.4byte	.L_b36
	.2byte	0x8051
	.4byte	.L_b36_e
.L_D1523_e:
.L_D1527:
	.4byte	.L_D1527_e-.L_D1527
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1528
	.2byte	0x38
	.string	"fip"
	.2byte	0x83
	.2byte	.L_t1527_e-.L_t1527
.L_t1527:
	.byte	0x1
	.4byte	.L_T816
.L_t1527_e:
	.2byte	0x23
	.2byte	.L_l1527_e-.L_l1527
.L_l1527:
	.byte	0x1
	.4byte	0x7
.L_l1527_e:
.L_D1527_e:
.L_D1528:
	.4byte	.L_D1528_e-.L_D1528
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1529
	.2byte	0x38
	.string	"flag"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1528_e-.L_l1528
.L_l1528:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l1528_e:
.L_D1528_e:
.L_D1529:
	.4byte	.L_D1529_e-.L_D1529
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1530
	.2byte	0x38
	.string	"mp"
	.2byte	0x83
	.2byte	.L_t1529_e-.L_t1529
.L_t1529:
	.byte	0x1
	.4byte	.L_T667
.L_t1529_e:
	.2byte	0x23
	.2byte	.L_l1529_e-.L_l1529
.L_l1529:
	.byte	0x1
	.4byte	0x3
.L_l1529_e:
.L_D1529_e:
.L_D1530:
	.4byte	.L_D1530_e-.L_D1530
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1531
	.2byte	0x38
	.string	"discard"
	.2byte	0x83
	.2byte	.L_t1530_e-.L_t1530
.L_t1530:
	.byte	0x1
	.4byte	.L_T667
.L_t1530_e:
	.2byte	0x23
	.2byte	.L_l1530_e-.L_l1530
.L_l1530:
	.byte	0x1
	.4byte	0x6
.L_l1530_e:
.L_D1530_e:
.L_D1531:
	.4byte	.L_D1531_e-.L_D1531
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1532
	.2byte	0x38
	.string	"old_spl"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1531_e-.L_l1531
.L_l1531:
	.byte	0x2
	.4byte	0x5
	.byte	0x4
	.4byte	0xfffffffc
	.byte	0x7
.L_l1531_e:
.L_D1531_e:
.L_D1532:
	.4byte	0x4
	.previous
	.align 4
	.type	 sas_link,@function
sas_link:
.L_LC2471:

.section	.line
	.4byte	3861	/ sas.c:3861
	.2byte	0xffff
	.4byte	.L_LC2471-.L_text_b
	.previous
	pushl %ebp
	movl %esp,%ebp
	movl 8(%ebp),%eax
	movl 12(%ebp),%edx
.L_b37:
.L_LC2472:

.section	.line
	.4byte	3862	/ sas.c:3862
	.2byte	0xffff
	.4byte	.L_LC2472-.L_text_b
	.previous
.L_LC2473:

.section	.line
	.4byte	3863	/ sas.c:3863
	.2byte	0xffff
	.4byte	.L_LC2473-.L_text_b
	.previous
	cmpl $0,(%eax)
	jne .L1277
.L_LC2474:

.section	.line
	.4byte	3864	/ sas.c:3864
	.2byte	0xffff
	.4byte	.L_LC2474-.L_text_b
	.previous
	movl %edx,(%eax)
.L_LC2475:

.section	.line
	.4byte	3865	/ sas.c:3865
	.2byte	0xffff
	.4byte	.L_LC2475-.L_text_b
	.previous
	leave
	ret
	.align 4
.L1277:
.L_LC2476:

.section	.line
	.4byte	3866	/ sas.c:3866
	.2byte	0xffff
	.4byte	.L_LC2476-.L_text_b
	.previous
	movl (%eax),%eax
.L_LC2477:

.section	.line
	.4byte	3867	/ sas.c:3867
	.2byte	0xffff
	.4byte	.L_LC2477-.L_text_b
	.previous
	cmpl $0,8(%eax)
	je .L1280
	.align 4
.L1281:
.L_LC2478:

.section	.line
	.4byte	3868	/ sas.c:3868
	.2byte	0xffff
	.4byte	.L_LC2478-.L_text_b
	.previous
	movl 8(%eax),%eax
	cmpl $0,8(%eax)
	jne .L1281
.L1280:
.L_LC2479:

.section	.line
	.4byte	3869	/ sas.c:3869
	.2byte	0xffff
	.4byte	.L_LC2479-.L_text_b
	.previous
	movl %edx,8(%eax)
.L_LC2480:

.section	.line
	.4byte	3871	/ sas.c:3871
	.2byte	0xffff
	.4byte	.L_LC2480-.L_text_b
	.previous
	leave
	ret
.L_b37_e:
.L_f37_e:
.Lfe34:
	.size	 sas_link,.Lfe34-sas_link

.section	.debug
.L_D1526:
	.4byte	.L_D1526_e-.L_D1526
	.2byte	0x14
	.2byte	0x12
	.4byte	.L_D1533
	.2byte	0x38
	.string	"sas_link"
	.2byte	0x111
	.4byte	sas_link
	.2byte	0x121
	.4byte	.L_f37_e
	.2byte	0x8041
	.4byte	.L_b37
	.2byte	0x8051
	.4byte	.L_b37_e
.L_D1526_e:
.L_D1534:
	.4byte	.L_D1534_e-.L_D1534
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1535
	.2byte	0x38
	.string	"mp"
	.2byte	0x83
	.2byte	.L_t1534_e-.L_t1534
.L_t1534:
	.byte	0x1
	.byte	0x1
	.4byte	.L_T667
.L_t1534_e:
	.2byte	0x23
	.2byte	.L_l1534_e-.L_l1534
.L_l1534:
	.byte	0x1
	.4byte	0x0
.L_l1534_e:
.L_D1534_e:
.L_D1535:
	.4byte	.L_D1535_e-.L_D1535
	.2byte	0x5
	.2byte	0x12
	.4byte	.L_D1536
	.2byte	0x38
	.string	"bp"
	.2byte	0x83
	.2byte	.L_t1535_e-.L_t1535
.L_t1535:
	.byte	0x1
	.4byte	.L_T667
.L_t1535_e:
	.2byte	0x23
	.2byte	.L_l1535_e-.L_l1535
.L_l1535:
	.byte	0x1
	.4byte	0x2
.L_l1535_e:
.L_D1535_e:
.L_D1536:
	.4byte	.L_D1536_e-.L_D1536
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1537
	.2byte	0x38
	.string	"lp"
	.2byte	0x83
	.2byte	.L_t1536_e-.L_t1536
.L_t1536:
	.byte	0x1
	.4byte	.L_T667
.L_t1536_e:
	.2byte	0x23
	.2byte	.L_l1536_e-.L_l1536
.L_l1536:
	.byte	0x1
	.4byte	0x0
.L_l1536_e:
.L_D1536_e:
.L_D1537:
	.4byte	0x4
	.previous

.section	.debug
.L_D1533:
	.4byte	.L_D1533_e-.L_D1533
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D1538
	.2byte	0x38
	.string	"rcsid"
	.2byte	0x63
	.2byte	.L_t1533_e-.L_t1533
.L_t1533:
	.byte	0x1
	.2byte	0x1
.L_t1533_e:
	.2byte	0x23
	.2byte	.L_l1533_e-.L_l1533
.L_l1533:
	.byte	0x3
	.4byte	rcsid
.L_l1533_e:
.L_D1533_e:
	.previous

.section	.debug
.L_D1538:
	.4byte	.L_D1538_e-.L_D1538
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1539
	.2byte	0x38
	.string	"addr_t"
	.2byte	0x63
	.2byte	.L_t1538_e-.L_t1538
.L_t1538:
	.byte	0x1
	.2byte	0x1
.L_t1538_e:
.L_D1538_e:
	.previous

.section	.debug
.L_D1539:
	.4byte	.L_D1539_e-.L_D1539
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1540
	.2byte	0x38
	.string	"caddr_t"
	.2byte	0x63
	.2byte	.L_t1539_e-.L_t1539
.L_t1539:
	.byte	0x1
	.2byte	0x1
.L_t1539_e:
.L_D1539_e:
	.previous

.section	.debug
.L_D1540:
	.4byte	.L_D1540_e-.L_D1540
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1541
	.2byte	0x38
	.string	"daddr_t"
	.2byte	0x55
	.2byte	0x7
.L_D1540_e:
	.previous

.section	.debug
.L_D1541:
	.4byte	.L_D1541_e-.L_D1541
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1542
	.2byte	0x38
	.string	"faddr_t"
	.2byte	0x63
	.2byte	.L_t1541_e-.L_t1541
.L_t1541:
	.byte	0x1
	.2byte	0x1
.L_t1541_e:
.L_D1541_e:
	.previous

.section	.debug
.L_D1542:
	.4byte	.L_D1542_e-.L_D1542
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1543
	.2byte	0x38
	.string	"off_t"
	.2byte	0x55
	.2byte	0x7
.L_D1542_e:
	.previous

.section	.debug
.L_D1543:
	.4byte	.L_D1543_e-.L_D1543
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1544
	.2byte	0x38
	.string	"cnt_t"
	.2byte	0x55
	.2byte	0x4
.L_D1543_e:
	.previous

.section	.debug
.L_D1544:
	.4byte	.L_D1544_e-.L_D1544
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1545
	.2byte	0x38
	.string	"paddr_t"
	.2byte	0x55
	.2byte	0x9
.L_D1544_e:
	.previous

.section	.debug
.L_D1545:
	.4byte	.L_D1545_e-.L_D1545
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1546
	.2byte	0x38
	.string	"use_t"
	.2byte	0x55
	.2byte	0x3
.L_D1545_e:
	.previous

.section	.debug
.L_D1546:
	.4byte	.L_D1546_e-.L_D1546
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1547
	.2byte	0x38
	.string	"sysid_t"
	.2byte	0x55
	.2byte	0x4
.L_D1546_e:
	.previous

.section	.debug
.L_D1547:
	.4byte	.L_D1547_e-.L_D1547
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1548
	.2byte	0x38
	.string	"index_t"
	.2byte	0x55
	.2byte	0x4
.L_D1547_e:
	.previous

.section	.debug
.L_D1548:
	.4byte	.L_D1548_e-.L_D1548
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1549
	.2byte	0x38
	.string	"lock_t"
	.2byte	0x55
	.2byte	0x4
.L_D1548_e:
	.previous

.section	.debug
.L_D1549:
	.4byte	.L_D1549_e-.L_D1549
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1550
	.2byte	0x38
	.string	"sel_t"
	.2byte	0x55
	.2byte	0x6
.L_D1549_e:
	.previous

.section	.debug
.L_D1550:
	.4byte	.L_D1550_e-.L_D1550
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1551
	.2byte	0x38
	.string	"k_sigset_t"
	.2byte	0x55
	.2byte	0x9
.L_D1550_e:
	.previous

.section	.debug
.L_D1551:
	.4byte	.L_D1551_e-.L_D1551
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1552
	.2byte	0x38
	.string	"k_fltset_t"
	.2byte	0x55
	.2byte	0x9
.L_D1551_e:
	.previous

.section	.debug
.L_D1552:
	.4byte	.L_D1552_e-.L_D1552
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1553
	.set	.L_T83,.L_D1552
	.2byte	0x38
	.string	"_label"
	.2byte	0xb6
	.4byte	0x18
.L_D1552_e:
.L_D1554:
	.4byte	.L_D1554_e-.L_D1554
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1555
	.2byte	0x38
	.string	"val"
	.2byte	0x142
	.4byte	.L_T83
	.2byte	0x72
	.4byte	.L_T85
	.2byte	0x23
	.2byte	.L_l1554_e-.L_l1554
.L_l1554:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1554_e:
.L_D1554_e:
.L_D1555:
	.4byte	0x4
.L_D1553:
	.4byte	.L_D1553_e-.L_D1553
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D1556
	.set	.L_T85,.L_D1553
	.2byte	0xa3
	.2byte	.L_s1553_e-.L_s1553
.L_s1553:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x5
	.byte	0x8
	.2byte	0x55
	.2byte	0x7
.L_s1553_e:
.L_D1553_e:
	.previous

.section	.debug
.L_D1556:
	.4byte	.L_D1556_e-.L_D1556
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1557
	.2byte	0x38
	.string	"label_t"
	.2byte	0x72
	.4byte	.L_T83
.L_D1556_e:
	.previous

.section	.debug
.L_D1557:
	.4byte	.L_D1557_e-.L_D1557
	.2byte	0x4
	.2byte	0x12
	.4byte	.L_D1558
	.set	.L_T87,.L_D1557
	.2byte	0x38
	.string	"boolean"
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xf4
	.4byte	.L_e1557_e-.L_e1557
.L_e1557:
	.4byte	0x1
	.string	"B_TRUE"
	.4byte	0x0
	.string	"B_FALSE"
.L_e1557_e:
.L_D1557_e:
	.previous

.section	.debug
.L_D1558:
	.4byte	.L_D1558_e-.L_D1558
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1559
	.2byte	0x38
	.string	"boolean_t"
	.2byte	0x72
	.4byte	.L_T87
.L_D1558_e:
	.previous

.section	.debug
.L_D1559:
	.4byte	.L_D1559_e-.L_D1559
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1560
	.2byte	0x38
	.string	"uchar_t"
	.2byte	0x55
	.2byte	0x3
.L_D1559_e:
	.previous

.section	.debug
.L_D1560:
	.4byte	.L_D1560_e-.L_D1560
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1561
	.2byte	0x38
	.string	"ushort_t"
	.2byte	0x55
	.2byte	0x6
.L_D1560_e:
	.previous

.section	.debug
.L_D1561:
	.4byte	.L_D1561_e-.L_D1561
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1562
	.2byte	0x38
	.string	"uint_t"
	.2byte	0x55
	.2byte	0x9
.L_D1561_e:
	.previous

.section	.debug
.L_D1562:
	.4byte	.L_D1562_e-.L_D1562
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1563
	.2byte	0x38
	.string	"ulong_t"
	.2byte	0x55
	.2byte	0x9
.L_D1562_e:
	.previous

.section	.debug
.L_D1563:
	.4byte	.L_D1563_e-.L_D1563
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1564
	.2byte	0x38
	.string	"id_t"
	.2byte	0x55
	.2byte	0x7
.L_D1563_e:
	.previous

.section	.debug
.L_D1564:
	.4byte	.L_D1564_e-.L_D1564
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1565
	.2byte	0x38
	.string	"pvec_t"
	.2byte	0x55
	.2byte	0x9
.L_D1564_e:
	.previous

.section	.debug
.L_D1565:
	.4byte	.L_D1565_e-.L_D1565
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1566
	.2byte	0x38
	.string	"lid_t"
	.2byte	0x55
	.2byte	0x9
.L_D1565_e:
	.previous

.section	.debug
.L_D1566:
	.4byte	.L_D1566_e-.L_D1566
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1567
	.2byte	0x38
	.string	"level_t"
	.2byte	0x55
	.2byte	0x9
.L_D1566_e:
	.previous

.section	.debug
.L_D1567:
	.4byte	.L_D1567_e-.L_D1567
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D1568
	.set	.L_T99,.L_D1567
	.2byte	0xa3
	.2byte	.L_s1567_e-.L_s1567
.L_s1567:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x7
	.byte	0x8
	.2byte	0x55
	.2byte	0xc
.L_s1567_e:
.L_D1567_e:
.L_D1568:
	.4byte	.L_D1568_e-.L_D1568
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1569
	.2byte	0x38
	.string	"adtemask_t"
	.2byte	0x72
	.4byte	.L_T99
.L_D1568_e:
	.previous

.section	.debug
.L_D1569:
	.4byte	.L_D1569_e-.L_D1569
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1570
	.2byte	0x38
	.string	"major_t"
	.2byte	0x55
	.2byte	0x9
.L_D1569_e:
	.previous

.section	.debug
.L_D1570:
	.4byte	.L_D1570_e-.L_D1570
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1571
	.2byte	0x38
	.string	"minor_t"
	.2byte	0x55
	.2byte	0x9
.L_D1570_e:
	.previous

.section	.debug
.L_D1571:
	.4byte	.L_D1571_e-.L_D1571
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1572
	.2byte	0x38
	.string	"o_mode_t"
	.2byte	0x55
	.2byte	0x6
.L_D1571_e:
	.previous

.section	.debug
.L_D1572:
	.4byte	.L_D1572_e-.L_D1572
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1573
	.2byte	0x38
	.string	"o_dev_t"
	.2byte	0x55
	.2byte	0x4
.L_D1572_e:
	.previous

.section	.debug
.L_D1573:
	.4byte	.L_D1573_e-.L_D1573
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1574
	.2byte	0x38
	.string	"o_uid_t"
	.2byte	0x55
	.2byte	0x6
.L_D1573_e:
	.previous

.section	.debug
.L_D1574:
	.4byte	.L_D1574_e-.L_D1574
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1575
	.2byte	0x38
	.string	"o_gid_t"
	.2byte	0x55
	.2byte	0x6
.L_D1574_e:
	.previous

.section	.debug
.L_D1575:
	.4byte	.L_D1575_e-.L_D1575
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1576
	.2byte	0x38
	.string	"o_nlink_t"
	.2byte	0x55
	.2byte	0x4
.L_D1575_e:
	.previous

.section	.debug
.L_D1576:
	.4byte	.L_D1576_e-.L_D1576
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1577
	.2byte	0x38
	.string	"o_pid_t"
	.2byte	0x55
	.2byte	0x4
.L_D1576_e:
	.previous

.section	.debug
.L_D1577:
	.4byte	.L_D1577_e-.L_D1577
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1578
	.2byte	0x38
	.string	"o_ino_t"
	.2byte	0x55
	.2byte	0x6
.L_D1577_e:
	.previous

.section	.debug
.L_D1578:
	.4byte	.L_D1578_e-.L_D1578
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1579
	.2byte	0x38
	.string	"key_t"
	.2byte	0x55
	.2byte	0x7
.L_D1578_e:
	.previous

.section	.debug
.L_D1579:
	.4byte	.L_D1579_e-.L_D1579
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1580
	.2byte	0x38
	.string	"mode_t"
	.2byte	0x55
	.2byte	0x9
.L_D1579_e:
	.previous

.section	.debug
.L_D1580:
	.4byte	.L_D1580_e-.L_D1580
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1581
	.2byte	0x38
	.string	"uid_t"
	.2byte	0x55
	.2byte	0x7
.L_D1580_e:
	.previous

.section	.debug
.L_D1581:
	.4byte	.L_D1581_e-.L_D1581
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1582
	.2byte	0x38
	.string	"gid_t"
	.2byte	0x55
	.2byte	0x7
.L_D1581_e:
	.previous

.section	.debug
.L_D1582:
	.4byte	.L_D1582_e-.L_D1582
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1583
	.2byte	0x38
	.string	"nlink_t"
	.2byte	0x55
	.2byte	0x9
.L_D1582_e:
	.previous

.section	.debug
.L_D1583:
	.4byte	.L_D1583_e-.L_D1583
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1584
	.2byte	0x38
	.string	"dev_t"
	.2byte	0x55
	.2byte	0x9
.L_D1583_e:
	.previous

.section	.debug
.L_D1584:
	.4byte	.L_D1584_e-.L_D1584
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1585
	.2byte	0x38
	.string	"ino_t"
	.2byte	0x55
	.2byte	0x9
.L_D1584_e:
	.previous

.section	.debug
.L_D1585:
	.4byte	.L_D1585_e-.L_D1585
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1586
	.2byte	0x38
	.string	"pid_t"
	.2byte	0x55
	.2byte	0x7
.L_D1585_e:
	.previous

.section	.debug
.L_D1586:
	.4byte	.L_D1586_e-.L_D1586
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1587
	.2byte	0x38
	.string	"size_t"
	.2byte	0x55
	.2byte	0x9
.L_D1586_e:
	.previous

.section	.debug
.L_D1587:
	.4byte	.L_D1587_e-.L_D1587
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1588
	.2byte	0x38
	.string	"ssize_t"
	.2byte	0x55
	.2byte	0x7
.L_D1587_e:
	.previous

.section	.debug
.L_D1588:
	.4byte	.L_D1588_e-.L_D1588
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1589
	.2byte	0x38
	.string	"time_t"
	.2byte	0x55
	.2byte	0x7
.L_D1588_e:
	.previous

.section	.debug
.L_D1589:
	.4byte	.L_D1589_e-.L_D1589
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1590
	.2byte	0x38
	.string	"clock_t"
	.2byte	0x55
	.2byte	0x7
.L_D1589_e:
	.previous

.section	.debug
.L_D1590:
	.4byte	.L_D1590_e-.L_D1590
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1591
	.set	.L_T124,.L_D1590
	.2byte	0xb6
	.4byte	0x4
.L_D1590_e:
.L_D1592:
	.4byte	.L_D1592_e-.L_D1592
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1593
	.2byte	0x38
	.string	"r"
	.2byte	0x142
	.4byte	.L_T124
	.2byte	0x72
	.4byte	.L_T123
	.2byte	0x23
	.2byte	.L_l1592_e-.L_l1592
.L_l1592:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1592_e:
.L_D1592_e:
.L_D1593:
	.4byte	0x4
.L_D1591:
	.4byte	.L_D1591_e-.L_D1591
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D1594
	.set	.L_T123,.L_D1591
	.2byte	0xa3
	.2byte	.L_s1591_e-.L_s1591
.L_s1591:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x7
.L_s1591_e:
.L_D1591_e:
	.previous

.section	.debug
.L_D1594:
	.4byte	.L_D1594_e-.L_D1594
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1595
	.2byte	0x38
	.string	"physadr"
	.2byte	0x83
	.2byte	.L_t1594_e-.L_t1594
.L_t1594:
	.byte	0x1
	.4byte	.L_T124
.L_t1594_e:
.L_D1594_e:
	.previous

.section	.debug
.L_D1595:
	.4byte	.L_D1595_e-.L_D1595
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1596
	.2byte	0x38
	.string	"unchar"
	.2byte	0x55
	.2byte	0x3
.L_D1595_e:
	.previous

.section	.debug
.L_D1596:
	.4byte	.L_D1596_e-.L_D1596
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1597
	.2byte	0x38
	.string	"ushort"
	.2byte	0x55
	.2byte	0x6
.L_D1596_e:
	.previous

.section	.debug
.L_D1597:
	.4byte	.L_D1597_e-.L_D1597
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1598
	.2byte	0x38
	.string	"uint"
	.2byte	0x55
	.2byte	0x9
.L_D1597_e:
	.previous

.section	.debug
.L_D1598:
	.4byte	.L_D1598_e-.L_D1598
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1599
	.2byte	0x38
	.string	"ulong"
	.2byte	0x55
	.2byte	0x9
.L_D1598_e:
	.previous

.section	.debug
.L_D1599:
	.4byte	.L_D1599_e-.L_D1599
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1600
	.2byte	0x38
	.string	"spl_t"
	.2byte	0x55
	.2byte	0x7
.L_D1599_e:
	.previous

.section	.debug
.L_D1600:
	.4byte	.L_D1600_e-.L_D1600
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1601
	.2byte	0x38
	.string	"hostid_t"
	.2byte	0x55
	.2byte	0x7
.L_D1600_e:
	.previous

.section	.debug
.L_D1601:
	.4byte	.L_D1601_e-.L_D1601
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1602
	.2byte	0x38
	.string	"u_char"
	.2byte	0x55
	.2byte	0x3
.L_D1601_e:
	.previous

.section	.debug
.L_D1602:
	.4byte	.L_D1602_e-.L_D1602
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1603
	.2byte	0x38
	.string	"u_short"
	.2byte	0x55
	.2byte	0x6
.L_D1602_e:
	.previous

.section	.debug
.L_D1603:
	.4byte	.L_D1603_e-.L_D1603
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1604
	.2byte	0x38
	.string	"u_int"
	.2byte	0x55
	.2byte	0x9
.L_D1603_e:
	.previous

.section	.debug
.L_D1604:
	.4byte	.L_D1604_e-.L_D1604
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1605
	.2byte	0x38
	.string	"u_long"
	.2byte	0x55
	.2byte	0x9
.L_D1604_e:
	.previous

.section	.debug
.L_D1605:
	.4byte	.L_D1605_e-.L_D1605
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1606
	.set	.L_T137,.L_D1605
	.2byte	0x38
	.string	"_quad"
	.2byte	0xb6
	.4byte	0x8
.L_D1605_e:
.L_D1607:
	.4byte	.L_D1607_e-.L_D1607
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1608
	.2byte	0x38
	.string	"val"
	.2byte	0x142
	.4byte	.L_T137
	.2byte	0x72
	.4byte	.L_T139
	.2byte	0x23
	.2byte	.L_l1607_e-.L_l1607
.L_l1607:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1607_e:
.L_D1607_e:
.L_D1608:
	.4byte	0x4
	.previous

.section	.debug
.L_D1606:
	.4byte	.L_D1606_e-.L_D1606
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1609
	.2byte	0x38
	.string	"quad"
	.2byte	0x72
	.4byte	.L_T137
.L_D1606_e:
	.previous

.section	.debug
.L_D1609:
	.4byte	.L_D1609_e-.L_D1609
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1610
	.2byte	0x38
	.string	"fd_mask"
	.2byte	0x55
	.2byte	0x7
.L_D1609_e:
	.previous

.section	.debug
.L_D1610:
	.4byte	.L_D1610_e-.L_D1610
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1611
	.set	.L_T142,.L_D1610
	.2byte	0x38
	.string	"fd_set"
	.2byte	0xb6
	.4byte	0x80
.L_D1610_e:
.L_D1612:
	.4byte	.L_D1612_e-.L_D1612
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1613
	.2byte	0x38
	.string	"fds_bits"
	.2byte	0x142
	.4byte	.L_T142
	.2byte	0x72
	.4byte	.L_T145
	.2byte	0x23
	.2byte	.L_l1612_e-.L_l1612
.L_l1612:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1612_e:
.L_D1612_e:
.L_D1613:
	.4byte	0x4
.L_D1611:
	.4byte	.L_D1611_e-.L_D1611
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D1614
	.set	.L_T145,.L_D1611
	.2byte	0xa3
	.2byte	.L_s1611_e-.L_s1611
.L_s1611:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x1f
	.byte	0x8
	.2byte	0x55
	.2byte	0x7
.L_s1611_e:
.L_D1611_e:
	.previous

.section	.debug
.L_D1614:
	.4byte	.L_D1614_e-.L_D1614
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1615
	.2byte	0x38
	.string	"fd_set"
	.2byte	0x72
	.4byte	.L_T142
.L_D1614_e:
	.previous

.section	.debug
.L_D1615:
	.4byte	.L_D1615_e-.L_D1615
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1616
	.set	.L_T149,.L_D1615
	.2byte	0xb6
	.4byte	0x10
.L_D1615_e:
.L_D1617:
	.4byte	.L_D1617_e-.L_D1617
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1618
	.2byte	0x38
	.string	"sa_sigbits"
	.2byte	0x142
	.4byte	.L_T149
	.2byte	0x72
	.4byte	.L_T148
	.2byte	0x23
	.2byte	.L_l1617_e-.L_l1617
.L_l1617:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1617_e:
.L_D1617_e:
.L_D1618:
	.4byte	0x4
.L_D1616:
	.4byte	.L_D1616_e-.L_D1616
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D1619
	.set	.L_T148,.L_D1616
	.2byte	0xa3
	.2byte	.L_s1616_e-.L_s1616
.L_s1616:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x3
	.byte	0x8
	.2byte	0x55
	.2byte	0xc
.L_s1616_e:
.L_D1616_e:
	.previous

.section	.debug
.L_D1619:
	.4byte	.L_D1619_e-.L_D1619
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1620
	.2byte	0x38
	.string	"sigset_t"
	.2byte	0x72
	.4byte	.L_T149
.L_D1619_e:
	.previous

.section	.debug
.L_D1620:
	.4byte	.L_D1620_e-.L_D1620
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1621
	.set	.L_T151,.L_D1620
	.2byte	0x38
	.string	"sigaction"
	.2byte	0xb6
	.4byte	0x20
.L_D1620_e:
.L_D1622:
	.4byte	.L_D1622_e-.L_D1622
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1623
	.2byte	0x38
	.string	"sa_flags"
	.2byte	0x142
	.4byte	.L_T151
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1622_e-.L_l1622
.L_l1622:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1622_e:
.L_D1622_e:
.L_D1623:
	.4byte	.L_D1623_e-.L_D1623
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1624
	.2byte	0x38
	.string	"sa_handler"
	.2byte	0x142
	.4byte	.L_T151
	.2byte	0x83
	.2byte	.L_t1623_e-.L_t1623
.L_t1623:
	.byte	0x1
	.4byte	.L_T41
.L_t1623_e:
	.2byte	0x23
	.2byte	.L_l1623_e-.L_l1623
.L_l1623:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l1623_e:
.L_D1623_e:
.L_D1624:
	.4byte	.L_D1624_e-.L_D1624
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1625
	.2byte	0x38
	.string	"sa_mask"
	.2byte	0x142
	.4byte	.L_T151
	.2byte	0x72
	.4byte	.L_T149
	.2byte	0x23
	.2byte	.L_l1624_e-.L_l1624
.L_l1624:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l1624_e:
.L_D1624_e:
.L_D1625:
	.4byte	.L_D1625_e-.L_D1625
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1626
	.2byte	0x38
	.string	"sa_resv"
	.2byte	0x142
	.4byte	.L_T151
	.2byte	0x72
	.4byte	.L_T154
	.2byte	0x23
	.2byte	.L_l1625_e-.L_l1625
.L_l1625:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l1625_e:
.L_D1625_e:
.L_D1626:
	.4byte	0x4
.L_D1621:
	.4byte	.L_D1621_e-.L_D1621
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D1627
	.set	.L_T154,.L_D1621
	.2byte	0xa3
	.2byte	.L_s1621_e-.L_s1621
.L_s1621:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x1
	.byte	0x8
	.2byte	0x55
	.2byte	0x7
.L_s1621_e:
.L_D1621_e:
	.previous

.section	.debug
.L_D1627:
	.4byte	.L_D1627_e-.L_D1627
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1628
	.set	.L_T155,.L_D1627
	.2byte	0x38
	.string	"sigaltstack"
	.2byte	0xb6
	.4byte	0xc
.L_D1627_e:
.L_D1629:
	.4byte	.L_D1629_e-.L_D1629
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1630
	.2byte	0x38
	.string	"ss_sp"
	.2byte	0x142
	.4byte	.L_T155
	.2byte	0x63
	.2byte	.L_t1629_e-.L_t1629
.L_t1629:
	.byte	0x1
	.2byte	0x1
.L_t1629_e:
	.2byte	0x23
	.2byte	.L_l1629_e-.L_l1629
.L_l1629:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1629_e:
.L_D1629_e:
.L_D1630:
	.4byte	.L_D1630_e-.L_D1630
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1631
	.2byte	0x38
	.string	"ss_size"
	.2byte	0x142
	.4byte	.L_T155
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1630_e-.L_l1630
.L_l1630:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l1630_e:
.L_D1630_e:
.L_D1631:
	.4byte	.L_D1631_e-.L_D1631
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1632
	.2byte	0x38
	.string	"ss_flags"
	.2byte	0x142
	.4byte	.L_T155
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1631_e-.L_l1631
.L_l1631:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l1631_e:
.L_D1631_e:
.L_D1632:
	.4byte	0x4
	.previous

.section	.debug
.L_D1628:
	.4byte	.L_D1628_e-.L_D1628
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1633
	.2byte	0x38
	.string	"stack_t"
	.2byte	0x72
	.4byte	.L_T155
.L_D1628_e:
	.previous

.section	.debug
.L_D1633:
	.4byte	.L_D1633_e-.L_D1633
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1634
	.set	.L_T157,.L_D1633
	.2byte	0xb6
	.4byte	0xc
.L_D1633_e:
.L_D1635:
	.4byte	.L_D1635_e-.L_D1635
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1636
	.2byte	0x38
	.string	"sig"
	.2byte	0x142
	.4byte	.L_T157
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1635_e-.L_l1635
.L_l1635:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1635_e:
.L_D1635_e:
.L_D1636:
	.4byte	.L_D1636_e-.L_D1636
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1637
	.2byte	0x38
	.string	"perm"
	.2byte	0x142
	.4byte	.L_T157
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1636_e-.L_l1636
.L_l1636:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l1636_e:
.L_D1636_e:
.L_D1637:
	.4byte	.L_D1637_e-.L_D1637
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1638
	.2byte	0x38
	.string	"checkperm"
	.2byte	0x142
	.4byte	.L_T157
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1637_e-.L_l1637
.L_l1637:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l1637_e:
.L_D1637_e:
.L_D1638:
	.4byte	0x4
	.previous

.section	.debug
.L_D1634:
	.4byte	.L_D1634_e-.L_D1634
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D1639
	.2byte	0x38
	.string	"sigsend_t"
	.2byte	0x72
	.4byte	.L_T157
.L_D1634_e:
	.previous

.section	.debug
.L_D1639:
	.4byte	.L_D1639_e-.L_D1639
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1640
	.set	.L_T161,.L_D1639
	.2byte	0x38
	.string	"buf"
	.2byte	0xb6
	.4byte	0x64
.L_D1639_e:
.L_D1641:
	.4byte	.L_D1641_e-.L_D1641
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1642
	.2byte	0x38
	.string	"b_flags"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1641_e-.L_l1641
.L_l1641:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1641_e:
.L_D1641_e:
.L_D1642:
	.4byte	.L_D1642_e-.L_D1642
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1643
	.2byte	0x38
	.string	"b_forw"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x83
	.2byte	.L_t1642_e-.L_t1642
.L_t1642:
	.byte	0x1
	.4byte	.L_T161
.L_t1642_e:
	.2byte	0x23
	.2byte	.L_l1642_e-.L_l1642
.L_l1642:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l1642_e:
.L_D1642_e:
.L_D1643:
	.4byte	.L_D1643_e-.L_D1643
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1644
	.2byte	0x38
	.string	"b_back"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x83
	.2byte	.L_t1643_e-.L_t1643
.L_t1643:
	.byte	0x1
	.4byte	.L_T161
.L_t1643_e:
	.2byte	0x23
	.2byte	.L_l1643_e-.L_l1643
.L_l1643:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l1643_e:
.L_D1643_e:
.L_D1644:
	.4byte	.L_D1644_e-.L_D1644
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1645
	.2byte	0x38
	.string	"av_forw"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x83
	.2byte	.L_t1644_e-.L_t1644
.L_t1644:
	.byte	0x1
	.4byte	.L_T161
.L_t1644_e:
	.2byte	0x23
	.2byte	.L_l1644_e-.L_l1644
.L_l1644:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l1644_e:
.L_D1644_e:
.L_D1645:
	.4byte	.L_D1645_e-.L_D1645
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1646
	.2byte	0x38
	.string	"av_back"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x83
	.2byte	.L_t1645_e-.L_t1645
.L_t1645:
	.byte	0x1
	.4byte	.L_T161
.L_t1645_e:
	.2byte	0x23
	.2byte	.L_l1645_e-.L_l1645
.L_l1645:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l1645_e:
.L_D1645_e:
.L_D1646:
	.4byte	.L_D1646_e-.L_D1646
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1647
	.2byte	0x38
	.string	"b_dev"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l1646_e-.L_l1646
.L_l1646:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l1646_e:
.L_D1646_e:
.L_D1647:
	.4byte	.L_D1647_e-.L_D1647
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1648
	.2byte	0x38
	.string	"b_bcount"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1647_e-.L_l1647
.L_l1647:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l1647_e:
.L_D1647_e:
.L_D1648:
	.4byte	.L_D1648_e-.L_D1648
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1649
	.2byte	0x38
	.string	"b_un"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x72
	.4byte	.L_T164
	.2byte	0x23
	.2byte	.L_l1648_e-.L_l1648
.L_l1648:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l1648_e:
.L_D1648_e:
.L_D1649:
	.4byte	.L_D1649_e-.L_D1649
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1650
	.2byte	0x38
	.string	"b_blkno"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1649_e-.L_l1649
.L_l1649:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l1649_e:
.L_D1649_e:
.L_D1650:
	.4byte	.L_D1650_e-.L_D1650
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1651
	.2byte	0x38
	.string	"b_oerror"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l1650_e-.L_l1650
.L_l1650:
	.byte	0x4
	.4byte	0x24
	.byte	0x7
.L_l1650_e:
.L_D1650_e:
.L_D1651:
	.4byte	.L_D1651_e-.L_D1651
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1652
	.2byte	0x38
	.string	"b_res"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l1651_e-.L_l1651
.L_l1651:
	.byte	0x4
	.4byte	0x25
	.byte	0x7
.L_l1651_e:
.L_D1651_e:
.L_D1652:
	.4byte	.L_D1652_e-.L_D1652
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1653
	.2byte	0x38
	.string	"b_cylin"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1652_e-.L_l1652
.L_l1652:
	.byte	0x4
	.4byte	0x26
	.byte	0x7
.L_l1652_e:
.L_D1652_e:
.L_D1653:
	.4byte	.L_D1653_e-.L_D1653
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1654
	.2byte	0x38
	.string	"b_resid"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1653_e-.L_l1653
.L_l1653:
	.byte	0x4
	.4byte	0x28
	.byte	0x7
.L_l1653_e:
.L_D1653_e:
.L_D1654:
	.4byte	.L_D1654_e-.L_D1654
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1655
	.2byte	0x38
	.string	"b_sector"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1654_e-.L_l1654
.L_l1654:
	.byte	0x4
	.4byte	0x2c
	.byte	0x7
.L_l1654_e:
.L_D1654_e:
.L_D1655:
	.4byte	.L_D1655_e-.L_D1655
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1656
	.2byte	0x38
	.string	"b_start"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1655_e-.L_l1655
.L_l1655:
	.byte	0x4
	.4byte	0x30
	.byte	0x7
.L_l1655_e:
.L_D1655_e:
.L_D1656:
	.4byte	.L_D1656_e-.L_D1656
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1657
	.2byte	0x38
	.string	"b_proc"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x83
	.2byte	.L_t1656_e-.L_t1656
.L_t1656:
	.byte	0x1
	.4byte	.L_T165
.L_t1656_e:
	.2byte	0x23
	.2byte	.L_l1656_e-.L_l1656
.L_l1656:
	.byte	0x4
	.4byte	0x34
	.byte	0x7
.L_l1656_e:
.L_D1656_e:
.L_D1657:
	.4byte	.L_D1657_e-.L_D1657
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1658
	.2byte	0x38
	.string	"b_pages"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x83
	.2byte	.L_t1657_e-.L_t1657
.L_t1657:
	.byte	0x1
	.4byte	.L_T167
.L_t1657_e:
	.2byte	0x23
	.2byte	.L_l1657_e-.L_l1657
.L_l1657:
	.byte	0x4
	.4byte	0x38
	.byte	0x7
.L_l1657_e:
.L_D1657_e:
.L_D1658:
	.4byte	.L_D1658_e-.L_D1658
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1659
	.2byte	0x38
	.string	"b_reltime"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1658_e-.L_l1658
.L_l1658:
	.byte	0x4
	.4byte	0x3c
	.byte	0x7
.L_l1658_e:
.L_D1658_e:
.L_D1659:
	.4byte	.L_D1659_e-.L_D1659
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1660
	.2byte	0x38
	.string	"b_bufsize"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l1659_e-.L_l1659
.L_l1659:
	.byte	0x4
	.4byte	0x40
	.byte	0x7
.L_l1659_e:
.L_D1659_e:
.L_D1660:
	.4byte	.L_D1660_e-.L_D1660
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1661
	.2byte	0x38
	.string	"b_iodone"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x83
	.2byte	.L_t1660_e-.L_t1660
.L_t1660:
	.byte	0x1
	.4byte	.L_T41
.L_t1660_e:
	.2byte	0x23
	.2byte	.L_l1660_e-.L_l1660
.L_l1660:
	.byte	0x4
	.4byte	0x44
	.byte	0x7
.L_l1660_e:
.L_D1660_e:
.L_D1661:
	.4byte	.L_D1661_e-.L_D1661
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1662
	.2byte	0x38
	.string	"b_vp"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x83
	.2byte	.L_t1661_e-.L_t1661
.L_t1661:
	.byte	0x1
	.4byte	.L_T170
.L_t1661_e:
	.2byte	0x23
	.2byte	.L_l1661_e-.L_l1661
.L_l1661:
	.byte	0x4
	.4byte	0x48
	.byte	0x7
.L_l1661_e:
.L_D1661_e:
.L_D1662:
	.4byte	.L_D1662_e-.L_D1662
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1663
	.2byte	0x38
	.string	"b_chain"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x83
	.2byte	.L_t1662_e-.L_t1662
.L_t1662:
	.byte	0x1
	.4byte	.L_T161
.L_t1662_e:
	.2byte	0x23
	.2byte	.L_l1662_e-.L_l1662
.L_l1662:
	.byte	0x4
	.4byte	0x4c
	.byte	0x7
.L_l1662_e:
.L_D1662_e:
.L_D1663:
	.4byte	.L_D1663_e-.L_D1663
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1664
	.2byte	0x38
	.string	"b_reqcnt"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1663_e-.L_l1663
.L_l1663:
	.byte	0x4
	.4byte	0x50
	.byte	0x7
.L_l1663_e:
.L_D1663_e:
.L_D1664:
	.4byte	.L_D1664_e-.L_D1664
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1665
	.2byte	0x38
	.string	"b_error"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1664_e-.L_l1664
.L_l1664:
	.byte	0x4
	.4byte	0x54
	.byte	0x7
.L_l1664_e:
.L_D1664_e:
.L_D1665:
	.4byte	.L_D1665_e-.L_D1665
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1666
	.2byte	0x38
	.string	"b_edev"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1665_e-.L_l1665
.L_l1665:
	.byte	0x4
	.4byte	0x58
	.byte	0x7
.L_l1665_e:
.L_D1665_e:
.L_D1666:
	.4byte	.L_D1666_e-.L_D1666
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1667
	.2byte	0x38
	.string	"b_private"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x63
	.2byte	.L_t1666_e-.L_t1666
.L_t1666:
	.byte	0x1
	.2byte	0x1
.L_t1666_e:
	.2byte	0x23
	.2byte	.L_l1666_e-.L_l1666
.L_l1666:
	.byte	0x4
	.4byte	0x5c
	.byte	0x7
.L_l1666_e:
.L_D1666_e:
.L_D1667:
	.4byte	.L_D1667_e-.L_D1667
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1668
	.2byte	0x38
	.string	"b_writestrat"
	.2byte	0x142
	.4byte	.L_T161
	.2byte	0x83
	.2byte	.L_t1667_e-.L_t1667
.L_t1667:
	.byte	0x1
	.4byte	.L_T41
.L_t1667_e:
	.2byte	0x23
	.2byte	.L_l1667_e-.L_l1667
.L_l1667:
	.byte	0x4
	.4byte	0x60
	.byte	0x7
.L_l1667_e:
.L_D1667_e:
.L_D1668:
	.4byte	0x4
.L_D1640:
	.4byte	.L_D1640_e-.L_D1640
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D1669
	.set	.L_T164,.L_D1640
	.2byte	0xb6
	.4byte	0x4
.L_D1640_e:
.L_D1670:
	.4byte	.L_D1670_e-.L_D1670
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1671
	.2byte	0x38
	.string	"b_addr"
	.2byte	0x142
	.4byte	.L_T164
	.2byte	0x63
	.2byte	.L_t1670_e-.L_t1670
.L_t1670:
	.byte	0x1
	.2byte	0x1
.L_t1670_e:
	.2byte	0x23
	.2byte	.L_l1670_e-.L_l1670
.L_l1670:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1670_e:
.L_D1670_e:
.L_D1671:
	.4byte	.L_D1671_e-.L_D1671
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1672
	.2byte	0x38
	.string	"b_words"
	.2byte	0x142
	.4byte	.L_T164
	.2byte	0x63
	.2byte	.L_t1671_e-.L_t1671
.L_t1671:
	.byte	0x1
	.2byte	0x7
.L_t1671_e:
	.2byte	0x23
	.2byte	.L_l1671_e-.L_l1671
.L_l1671:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1671_e:
.L_D1671_e:
.L_D1672:
	.4byte	.L_D1672_e-.L_D1672
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1673
	.2byte	0x38
	.string	"b_daddr"
	.2byte	0x142
	.4byte	.L_T164
	.2byte	0x63
	.2byte	.L_t1672_e-.L_t1672
.L_t1672:
	.byte	0x1
	.2byte	0xa
.L_t1672_e:
	.2byte	0x23
	.2byte	.L_l1672_e-.L_l1672
.L_l1672:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1672_e:
.L_D1672_e:
.L_D1673:
	.4byte	0x4
.L_D1669:
	.4byte	.L_D1669_e-.L_D1669
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1674
	.set	.L_T165,.L_D1669
	.2byte	0x38
	.string	"proc"
	.2byte	0xb6
	.4byte	0x18c
.L_D1669_e:
.L_D1675:
	.4byte	.L_D1675_e-.L_D1675
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1676
	.2byte	0x38
	.string	"p_stat"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l1675_e-.L_l1675
.L_l1675:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1675_e:
.L_D1675_e:
.L_D1676:
	.4byte	.L_D1676_e-.L_D1676
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1677
	.2byte	0x38
	.string	"p_cpu"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l1676_e-.L_l1676
.L_l1676:
	.byte	0x4
	.4byte	0x1
	.byte	0x7
.L_l1676_e:
.L_D1676_e:
.L_D1677:
	.4byte	.L_D1677_e-.L_D1677
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1678
	.2byte	0x38
	.string	"p_curflt"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l1677_e-.L_l1677
.L_l1677:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l1677_e:
.L_D1677_e:
.L_D1678:
	.4byte	.L_D1678_e-.L_D1678
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1679
	.2byte	0x38
	.string	"p_swlocks"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l1678_e-.L_l1678
.L_l1678:
	.byte	0x4
	.4byte	0x3
	.byte	0x7
.L_l1678_e:
.L_D1678_e:
.L_D1679:
	.4byte	.L_D1679_e-.L_D1679
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1680
	.2byte	0x38
	.string	"p_flag"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1679_e-.L_l1679
.L_l1679:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l1679_e:
.L_D1679_e:
.L_D1680:
	.4byte	.L_D1680_e-.L_D1680
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1681
	.2byte	0x38
	.string	"p_uid"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1680_e-.L_l1680
.L_l1680:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l1680_e:
.L_D1680_e:
.L_D1681:
	.4byte	.L_D1681_e-.L_D1681
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1682
	.2byte	0x38
	.string	"p_oppid"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l1681_e-.L_l1681
.L_l1681:
	.byte	0x4
	.4byte	0xa
	.byte	0x7
.L_l1681_e:
.L_D1681_e:
.L_D1682:
	.4byte	.L_D1682_e-.L_D1682
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1683
	.2byte	0x38
	.string	"p_opgrp"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l1682_e-.L_l1682
.L_l1682:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l1682_e:
.L_D1682_e:
.L_D1683:
	.4byte	.L_D1683_e-.L_D1683
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1684
	.2byte	0x38
	.string	"p_opid"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l1683_e-.L_l1683
.L_l1683:
	.byte	0x4
	.4byte	0xe
	.byte	0x7
.L_l1683_e:
.L_D1683_e:
.L_D1684:
	.4byte	.L_D1684_e-.L_D1684
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1685
	.2byte	0x38
	.string	"p_cred"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1684_e-.L_t1684
.L_t1684:
	.byte	0x1
	.4byte	.L_T223
.L_t1684_e:
	.2byte	0x23
	.2byte	.L_l1684_e-.L_l1684
.L_l1684:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l1684_e:
.L_D1684_e:
.L_D1685:
	.4byte	.L_D1685_e-.L_D1685
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1686
	.2byte	0x38
	.string	"p_sessp"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1685_e-.L_t1685
.L_t1685:
	.byte	0x1
	.4byte	.L_T225
.L_t1685_e:
	.2byte	0x23
	.2byte	.L_l1685_e-.L_l1685
.L_l1685:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l1685_e:
.L_D1685_e:
.L_D1686:
	.4byte	.L_D1686_e-.L_D1686
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1687
	.2byte	0x38
	.string	"p_pri"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1686_e-.L_l1686
.L_l1686:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l1686_e:
.L_D1686_e:
.L_D1687:
	.4byte	.L_D1687_e-.L_D1687
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1688
	.2byte	0x38
	.string	"p_unw"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x72
	.4byte	.L_T227
	.2byte	0x23
	.2byte	.L_l1687_e-.L_l1687
.L_l1687:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l1687_e:
.L_D1687_e:
.L_D1688:
	.4byte	.L_D1688_e-.L_D1688
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1689
	.2byte	0x38
	.string	"p_link"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1688_e-.L_t1688
.L_t1688:
	.byte	0x1
	.4byte	.L_T165
.L_t1688_e:
	.2byte	0x23
	.2byte	.L_l1688_e-.L_l1688
.L_l1688:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l1688_e:
.L_D1688_e:
.L_D1689:
	.4byte	.L_D1689_e-.L_D1689
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1690
	.2byte	0x38
	.string	"p_parent"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1689_e-.L_t1689
.L_t1689:
	.byte	0x1
	.4byte	.L_T165
.L_t1689_e:
	.2byte	0x23
	.2byte	.L_l1689_e-.L_l1689
.L_l1689:
	.byte	0x4
	.4byte	0x24
	.byte	0x7
.L_l1689_e:
.L_D1689_e:
.L_D1690:
	.4byte	.L_D1690_e-.L_D1690
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1691
	.2byte	0x38
	.string	"p_child"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1690_e-.L_t1690
.L_t1690:
	.byte	0x1
	.4byte	.L_T165
.L_t1690_e:
	.2byte	0x23
	.2byte	.L_l1690_e-.L_l1690
.L_l1690:
	.byte	0x4
	.4byte	0x28
	.byte	0x7
.L_l1690_e:
.L_D1690_e:
.L_D1691:
	.4byte	.L_D1691_e-.L_D1691
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1692
	.2byte	0x38
	.string	"p_sibling"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1691_e-.L_t1691
.L_t1691:
	.byte	0x1
	.4byte	.L_T165
.L_t1691_e:
	.2byte	0x23
	.2byte	.L_l1691_e-.L_l1691
.L_l1691:
	.byte	0x4
	.4byte	0x2c
	.byte	0x7
.L_l1691_e:
.L_D1691_e:
.L_D1692:
	.4byte	.L_D1692_e-.L_D1692
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1693
	.2byte	0x38
	.string	"p_swtch_in"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1692_e-.L_t1692
.L_t1692:
	.byte	0x1
	.4byte	.L_T228
.L_t1692_e:
	.2byte	0x23
	.2byte	.L_l1692_e-.L_l1692
.L_l1692:
	.byte	0x4
	.4byte	0x30
	.byte	0x7
.L_l1692_e:
.L_D1692_e:
.L_D1693:
	.4byte	.L_D1693_e-.L_D1693
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1694
	.2byte	0x38
	.string	"p_brkbase"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x63
	.2byte	.L_t1693_e-.L_t1693
.L_t1693:
	.byte	0x1
	.2byte	0x1
.L_t1693_e:
	.2byte	0x23
	.2byte	.L_l1693_e-.L_l1693
.L_l1693:
	.byte	0x4
	.4byte	0x34
	.byte	0x7
.L_l1693_e:
.L_D1693_e:
.L_D1694:
	.4byte	.L_D1694_e-.L_D1694
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1695
	.2byte	0x38
	.string	"p_brksize"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1694_e-.L_l1694
.L_l1694:
	.byte	0x4
	.4byte	0x38
	.byte	0x7
.L_l1694_e:
.L_D1694_e:
.L_D1695:
	.4byte	.L_D1695_e-.L_D1695
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1696
	.2byte	0x38
	.string	"p_stkbase"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x63
	.2byte	.L_t1695_e-.L_t1695
.L_t1695:
	.byte	0x1
	.2byte	0x1
.L_t1695_e:
	.2byte	0x23
	.2byte	.L_l1695_e-.L_l1695
.L_l1695:
	.byte	0x4
	.4byte	0x3c
	.byte	0x7
.L_l1695_e:
.L_D1695_e:
.L_D1696:
	.4byte	.L_D1696_e-.L_D1696
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1697
	.2byte	0x38
	.string	"p_stksize"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1696_e-.L_l1696
.L_l1696:
	.byte	0x4
	.4byte	0x40
	.byte	0x7
.L_l1696_e:
.L_D1696_e:
.L_D1697:
	.4byte	.L_D1697_e-.L_D1697
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1698
	.2byte	0x38
	.string	"p_swrss"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1697_e-.L_l1697
.L_l1697:
	.byte	0x4
	.4byte	0x44
	.byte	0x7
.L_l1697_e:
.L_D1697_e:
.L_D1698:
	.4byte	.L_D1698_e-.L_D1698
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1699
	.2byte	0x38
	.string	"p_utime"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1698_e-.L_l1698
.L_l1698:
	.byte	0x4
	.4byte	0x48
	.byte	0x7
.L_l1698_e:
.L_D1698_e:
.L_D1699:
	.4byte	.L_D1699_e-.L_D1699
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1700
	.2byte	0x38
	.string	"p_stime"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1699_e-.L_l1699
.L_l1699:
	.byte	0x4
	.4byte	0x4c
	.byte	0x7
.L_l1699_e:
.L_D1699_e:
.L_D1700:
	.4byte	.L_D1700_e-.L_D1700
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1701
	.2byte	0x38
	.string	"p_usize"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1700_e-.L_l1700
.L_l1700:
	.byte	0x4
	.4byte	0x50
	.byte	0x7
.L_l1700_e:
.L_D1700_e:
.L_D1701:
	.4byte	.L_D1701_e-.L_D1701
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1702
	.2byte	0x38
	.string	"p_user_license"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1701_e-.L_l1701
.L_l1701:
	.byte	0x4
	.4byte	0x52
	.byte	0x7
.L_l1701_e:
.L_D1701_e:
.L_D1702:
	.4byte	.L_D1702_e-.L_D1702
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1703
	.2byte	0x38
	.string	"p_ldt"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x63
	.2byte	.L_t1702_e-.L_t1702
.L_t1702:
	.byte	0x1
	.2byte	0x1
.L_t1702_e:
	.2byte	0x23
	.2byte	.L_l1702_e-.L_l1702
.L_l1702:
	.byte	0x4
	.4byte	0x54
	.byte	0x7
.L_l1702_e:
.L_D1702_e:
.L_D1703:
	.4byte	.L_D1703_e-.L_D1703
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1704
	.2byte	0x38
	.string	"p_vm86p"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1703_e-.L_t1703
.L_t1703:
	.byte	0x1
	.4byte	.L_T230
.L_t1703_e:
	.2byte	0x23
	.2byte	.L_l1703_e-.L_l1703
.L_l1703:
	.byte	0x4
	.4byte	0x58
	.byte	0x7
.L_l1703_e:
.L_D1703_e:
.L_D1704:
	.4byte	.L_D1704_e-.L_D1704
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1705
	.2byte	0x38
	.string	"p_as"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1704_e-.L_t1704
.L_t1704:
	.byte	0x1
	.4byte	.L_T232
.L_t1704_e:
	.2byte	0x23
	.2byte	.L_l1704_e-.L_l1704
.L_l1704:
	.byte	0x4
	.4byte	0x5c
	.byte	0x7
.L_l1704_e:
.L_D1704_e:
.L_D1705:
	.4byte	.L_D1705_e-.L_D1705
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1706
	.2byte	0x38
	.string	"p_wdata"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1705_e-.L_l1705
.L_l1705:
	.byte	0x4
	.4byte	0x60
	.byte	0x7
.L_l1705_e:
.L_D1705_e:
.L_D1706:
	.4byte	.L_D1706_e-.L_D1706
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1707
	.2byte	0x38
	.string	"p_wcode"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l1706_e-.L_l1706
.L_l1706:
	.byte	0x4
	.4byte	0x64
	.byte	0x7
.L_l1706_e:
.L_D1706_e:
.L_D1707:
	.4byte	.L_D1707_e-.L_D1707
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1708
	.2byte	0x38
	.string	"p_pollflag"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l1707_e-.L_l1707
.L_l1707:
	.byte	0x4
	.4byte	0x65
	.byte	0x7
.L_l1707_e:
.L_D1707_e:
.L_D1708:
	.4byte	.L_D1708_e-.L_D1708
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1709
	.2byte	0x38
	.string	"p_cursig"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l1708_e-.L_l1708
.L_l1708:
	.byte	0x4
	.4byte	0x66
	.byte	0x7
.L_l1708_e:
.L_D1708_e:
.L_D1709:
	.4byte	.L_D1709_e-.L_D1709
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1710
	.2byte	0x38
	.string	"p_epid"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1709_e-.L_l1709
.L_l1709:
	.byte	0x4
	.4byte	0x68
	.byte	0x7
.L_l1709_e:
.L_D1709_e:
.L_D1710:
	.4byte	.L_D1710_e-.L_D1710
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1711
	.2byte	0x38
	.string	"p_sysid"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l1710_e-.L_l1710
.L_l1710:
	.byte	0x4
	.4byte	0x6c
	.byte	0x7
.L_l1710_e:
.L_D1710_e:
.L_D1711:
	.4byte	.L_D1711_e-.L_D1711
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1712
	.2byte	0x38
	.string	"p_rlink"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1711_e-.L_t1711
.L_t1711:
	.byte	0x1
	.4byte	.L_T165
.L_t1711_e:
	.2byte	0x23
	.2byte	.L_l1711_e-.L_l1711
.L_l1711:
	.byte	0x4
	.4byte	0x70
	.byte	0x7
.L_l1711_e:
.L_D1711_e:
.L_D1712:
	.4byte	.L_D1712_e-.L_D1712
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1713
	.2byte	0x38
	.string	"p_srwchan"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1712_e-.L_l1712
.L_l1712:
	.byte	0x4
	.4byte	0x74
	.byte	0x7
.L_l1712_e:
.L_D1712_e:
.L_D1713:
	.4byte	.L_D1713_e-.L_D1713
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1714
	.2byte	0x38
	.string	"p_trace"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1713_e-.L_t1713
.L_t1713:
	.byte	0x1
	.4byte	.L_T170
.L_t1713_e:
	.2byte	0x23
	.2byte	.L_l1713_e-.L_l1713
.L_l1713:
	.byte	0x4
	.4byte	0x78
	.byte	0x7
.L_l1713_e:
.L_D1713_e:
.L_D1714:
	.4byte	.L_D1714_e-.L_D1714
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1715
	.2byte	0x38
	.string	"p_sig"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1714_e-.L_l1714
.L_l1714:
	.byte	0x4
	.4byte	0x7c
	.byte	0x7
.L_l1714_e:
.L_D1714_e:
.L_D1715:
	.4byte	.L_D1715_e-.L_D1715
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1716
	.2byte	0x38
	.string	"p_sigmask"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1715_e-.L_l1715
.L_l1715:
	.byte	0x4
	.4byte	0x80
	.byte	0x7
.L_l1715_e:
.L_D1715_e:
.L_D1716:
	.4byte	.L_D1716_e-.L_D1716
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1717
	.2byte	0x38
	.string	"p_hold"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1716_e-.L_l1716
.L_l1716:
	.byte	0x4
	.4byte	0x84
	.byte	0x7
.L_l1716_e:
.L_D1716_e:
.L_D1717:
	.4byte	.L_D1717_e-.L_D1717
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1718
	.2byte	0x38
	.string	"p_ignore"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1717_e-.L_l1717
.L_l1717:
	.byte	0x4
	.4byte	0x88
	.byte	0x7
.L_l1717_e:
.L_D1717_e:
.L_D1718:
	.4byte	.L_D1718_e-.L_D1718
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1719
	.2byte	0x38
	.string	"p_siginfo"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1718_e-.L_l1718
.L_l1718:
	.byte	0x4
	.4byte	0x8c
	.byte	0x7
.L_l1718_e:
.L_D1718_e:
.L_D1719:
	.4byte	.L_D1719_e-.L_D1719
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1720
	.2byte	0x38
	.string	"p_v86"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1719_e-.L_t1719
.L_t1719:
	.byte	0x1
	.4byte	.L_T234
.L_t1719_e:
	.2byte	0x23
	.2byte	.L_l1719_e-.L_l1719
.L_l1719:
	.byte	0x4
	.4byte	0x90
	.byte	0x7
.L_l1719_e:
.L_D1719_e:
.L_D1720:
	.4byte	.L_D1720_e-.L_D1720
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1721
	.2byte	0x38
	.string	"p_raiocnt"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1720_e-.L_l1720
.L_l1720:
	.byte	0x4
	.4byte	0x94
	.byte	0x7
.L_l1720_e:
.L_D1720_e:
.L_D1721:
	.4byte	.L_D1721_e-.L_D1721
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1722
	.2byte	0x38
	.string	"p_filler1"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x72
	.4byte	.L_T237
	.2byte	0x23
	.2byte	.L_l1721_e-.L_l1721
.L_l1721:
	.byte	0x4
	.4byte	0x96
	.byte	0x7
.L_l1721_e:
.L_D1721_e:
.L_D1722:
	.4byte	.L_D1722_e-.L_D1722
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1723
	.2byte	0x38
	.string	"p_whystop"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1722_e-.L_l1722
.L_l1722:
	.byte	0x4
	.4byte	0xdc
	.byte	0x7
.L_l1722_e:
.L_D1722_e:
.L_D1723:
	.4byte	.L_D1723_e-.L_D1723
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1724
	.2byte	0x38
	.string	"p_whatstop"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1723_e-.L_l1723
.L_l1723:
	.byte	0x4
	.4byte	0xde
	.byte	0x7
.L_l1723_e:
.L_D1723_e:
.L_D1724:
	.4byte	.L_D1724_e-.L_D1724
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1725
	.2byte	0x38
	.string	"p_ubptbl"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1724_e-.L_t1724
.L_t1724:
	.byte	0x1
	.4byte	.L_T210
.L_t1724_e:
	.2byte	0x23
	.2byte	.L_l1724_e-.L_l1724
.L_l1724:
	.byte	0x4
	.4byte	0xe0
	.byte	0x7
.L_l1724_e:
.L_D1724_e:
.L_D1725:
	.4byte	.L_D1725_e-.L_D1725
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1726
	.2byte	0x38
	.string	"p_filler2"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x72
	.4byte	.L_T239
	.2byte	0x23
	.2byte	.L_l1725_e-.L_l1725
.L_l1725:
	.byte	0x4
	.4byte	0xe4
	.byte	0x7
.L_l1725_e:
.L_D1725_e:
.L_D1726:
	.4byte	.L_D1726_e-.L_D1726
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1727
	.2byte	0x38
	.string	"p_alarmid"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1726_e-.L_l1726
.L_l1726:
	.byte	0x4
	.4byte	0x118
	.byte	0x7
.L_l1726_e:
.L_D1726_e:
.L_D1727:
	.4byte	.L_D1727_e-.L_D1727
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1728
	.2byte	0x38
	.string	"p_alarmtime"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1727_e-.L_l1727
.L_l1727:
	.byte	0x4
	.4byte	0x11c
	.byte	0x7
.L_l1727_e:
.L_D1727_e:
.L_D1728:
	.4byte	.L_D1728_e-.L_D1728
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1729
	.2byte	0x38
	.string	"p_inoutage"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1728_e-.L_l1728
.L_l1728:
	.byte	0x4
	.4byte	0x120
	.byte	0x7
.L_l1728_e:
.L_D1728_e:
.L_D1729:
	.4byte	.L_D1729_e-.L_D1729
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1730
	.2byte	0x38
	.string	"p_slptime"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1729_e-.L_l1729
.L_l1729:
	.byte	0x4
	.4byte	0x124
	.byte	0x7
.L_l1729_e:
.L_D1729_e:
.L_D1730:
	.4byte	.L_D1730_e-.L_D1730
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1731
	.2byte	0x38
	.string	"p_fltmask"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1730_e-.L_l1730
.L_l1730:
	.byte	0x4
	.4byte	0x128
	.byte	0x7
.L_l1730_e:
.L_D1730_e:
.L_D1731:
	.4byte	.L_D1731_e-.L_D1731
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1732
	.2byte	0x38
	.string	"p_evpdp"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1731_e-.L_t1731
.L_t1731:
	.byte	0x1
	.4byte	.L_T240
.L_t1731_e:
	.2byte	0x23
	.2byte	.L_l1731_e-.L_l1731
.L_l1731:
	.byte	0x4
	.4byte	0x12c
	.byte	0x7
.L_l1731_e:
.L_D1731_e:
.L_D1732:
	.4byte	.L_D1732_e-.L_D1732
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1733
	.2byte	0x38
	.string	"p_italarm"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x72
	.4byte	.L_T246
	.2byte	0x23
	.2byte	.L_l1732_e-.L_l1732
.L_l1732:
	.byte	0x4
	.4byte	0x130
	.byte	0x7
.L_l1732_e:
.L_D1732_e:
.L_D1733:
	.4byte	.L_D1733_e-.L_D1733
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1734
	.2byte	0x38
	.string	"p_pglink"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1733_e-.L_t1733
.L_t1733:
	.byte	0x1
	.4byte	.L_T165
.L_t1733_e:
	.2byte	0x23
	.2byte	.L_l1733_e-.L_l1733
.L_l1733:
	.byte	0x4
	.4byte	0x138
	.byte	0x7
.L_l1733_e:
.L_D1733_e:
.L_D1734:
	.4byte	.L_D1734_e-.L_D1734
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1735
	.2byte	0x38
	.string	"p_cutime"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1734_e-.L_l1734
.L_l1734:
	.byte	0x4
	.4byte	0x13c
	.byte	0x7
.L_l1734_e:
.L_D1734_e:
.L_D1735:
	.4byte	.L_D1735_e-.L_D1735
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1736
	.2byte	0x38
	.string	"p_cstime"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1735_e-.L_l1735
.L_l1735:
	.byte	0x4
	.4byte	0x140
	.byte	0x7
.L_l1735_e:
.L_D1735_e:
.L_D1736:
	.4byte	.L_D1736_e-.L_D1736
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1737
	.2byte	0x38
	.string	"p_aiocount"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l1736_e-.L_l1736
.L_l1736:
	.byte	0x4
	.4byte	0x144
	.byte	0x7
.L_l1736_e:
.L_D1736_e:
.L_D1737:
	.4byte	.L_D1737_e-.L_D1737
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1738
	.2byte	0x38
	.string	"p_aiowcnt"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l1737_e-.L_l1737
.L_l1737:
	.byte	0x4
	.4byte	0x146
	.byte	0x7
.L_l1737_e:
.L_D1737_e:
.L_D1738:
	.4byte	.L_D1738_e-.L_D1738
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1739
	.2byte	0x38
	.string	"p_cid"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1738_e-.L_l1738
.L_l1738:
	.byte	0x4
	.4byte	0x148
	.byte	0x7
.L_l1738_e:
.L_D1738_e:
.L_D1739:
	.4byte	.L_D1739_e-.L_D1739
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1740
	.2byte	0x38
	.string	"p_clproc"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x63
	.2byte	.L_t1739_e-.L_t1739
.L_t1739:
	.byte	0x1
	.2byte	0x14
.L_t1739_e:
	.2byte	0x23
	.2byte	.L_l1739_e-.L_l1739
.L_l1739:
	.byte	0x4
	.4byte	0x14c
	.byte	0x7
.L_l1739_e:
.L_D1739_e:
.L_D1740:
	.4byte	.L_D1740_e-.L_D1740
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1741
	.2byte	0x38
	.string	"p_clfuncs"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1740_e-.L_t1740
.L_t1740:
	.byte	0x1
	.4byte	.L_T247
.L_t1740_e:
	.2byte	0x23
	.2byte	.L_l1740_e-.L_l1740
.L_l1740:
	.byte	0x4
	.4byte	0x150
	.byte	0x7
.L_l1740_e:
.L_D1740_e:
.L_D1741:
	.4byte	.L_D1741_e-.L_D1741
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1742
	.2byte	0x38
	.string	"p_sigqueue"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1741_e-.L_t1741
.L_t1741:
	.byte	0x1
	.4byte	.L_T249
.L_t1741_e:
	.2byte	0x23
	.2byte	.L_l1741_e-.L_l1741
.L_l1741:
	.byte	0x4
	.4byte	0x154
	.byte	0x7
.L_l1741_e:
.L_D1741_e:
.L_D1742:
	.4byte	.L_D1742_e-.L_D1742
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1743
	.2byte	0x38
	.string	"p_curinfo"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1742_e-.L_t1742
.L_t1742:
	.byte	0x1
	.4byte	.L_T249
.L_t1742_e:
	.2byte	0x23
	.2byte	.L_l1742_e-.L_l1742
.L_l1742:
	.byte	0x4
	.4byte	0x158
	.byte	0x7
.L_l1742_e:
.L_D1742_e:
.L_D1743:
	.4byte	.L_D1743_e-.L_D1743
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1744
	.2byte	0x38
	.string	"p_segacct"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x63
	.2byte	.L_t1743_e-.L_t1743
.L_t1743:
	.byte	0x1
	.byte	0x1
	.2byte	0x1
.L_t1743_e:
	.2byte	0x23
	.2byte	.L_l1743_e-.L_l1743
.L_l1743:
	.byte	0x4
	.4byte	0x15c
	.byte	0x7
.L_l1743_e:
.L_D1743_e:
.L_D1744:
	.4byte	.L_D1744_e-.L_D1744
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1745
	.2byte	0x38
	.string	"p_segu"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1744_e-.L_t1744
.L_t1744:
	.byte	0x1
	.4byte	.L_T252
.L_t1744_e:
	.2byte	0x23
	.2byte	.L_l1744_e-.L_l1744
.L_l1744:
	.byte	0x4
	.4byte	0x160
	.byte	0x7
.L_l1744_e:
.L_D1744_e:
.L_D1745:
	.4byte	.L_D1745_e-.L_D1745
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1746
	.2byte	0x38
	.string	"p_exec"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1745_e-.L_t1745
.L_t1745:
	.byte	0x1
	.4byte	.L_T170
.L_t1745_e:
	.2byte	0x23
	.2byte	.L_l1745_e-.L_l1745
.L_l1745:
	.byte	0x4
	.4byte	0x164
	.byte	0x7
.L_l1745_e:
.L_D1745_e:
.L_D1746:
	.4byte	.L_D1746_e-.L_D1746
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1747
	.2byte	0x38
	.string	"p_ppid"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1746_e-.L_l1746
.L_l1746:
	.byte	0x4
	.4byte	0x168
	.byte	0x7
.L_l1746_e:
.L_D1746_e:
.L_D1747:
	.4byte	.L_D1747_e-.L_D1747
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1748
	.2byte	0x38
	.string	"p_pidp"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1747_e-.L_t1747
.L_t1747:
	.byte	0x1
	.4byte	.L_T254
.L_t1747_e:
	.2byte	0x23
	.2byte	.L_l1747_e-.L_l1747
.L_l1747:
	.byte	0x4
	.4byte	0x16c
	.byte	0x7
.L_l1747_e:
.L_D1747_e:
.L_D1748:
	.4byte	.L_D1748_e-.L_D1748
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1749
	.2byte	0x38
	.string	"p_pgidp"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1748_e-.L_t1748
.L_t1748:
	.byte	0x1
	.4byte	.L_T254
.L_t1748_e:
	.2byte	0x23
	.2byte	.L_l1748_e-.L_l1748
.L_l1748:
	.byte	0x4
	.4byte	0x170
	.byte	0x7
.L_l1748_e:
.L_D1748_e:
.L_D1749:
	.4byte	.L_D1749_e-.L_D1749
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1750
	.2byte	0x38
	.string	"p_sdp"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1749_e-.L_t1749
.L_t1749:
	.byte	0x1
	.4byte	.L_T256
.L_t1749_e:
	.2byte	0x23
	.2byte	.L_l1749_e-.L_l1749
.L_l1749:
	.byte	0x4
	.4byte	0x174
	.byte	0x7
.L_l1749_e:
.L_D1749_e:
.L_D1750:
	.4byte	.L_D1750_e-.L_D1750
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1751
	.2byte	0x38
	.string	"p_next"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1750_e-.L_t1750
.L_t1750:
	.byte	0x1
	.4byte	.L_T165
.L_t1750_e:
	.2byte	0x23
	.2byte	.L_l1750_e-.L_l1750
.L_l1750:
	.byte	0x4
	.4byte	0x178
	.byte	0x7
.L_l1750_e:
.L_D1750_e:
.L_D1751:
	.4byte	.L_D1751_e-.L_D1751
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1752
	.2byte	0x38
	.string	"p_nextofkin"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1751_e-.L_t1751
.L_t1751:
	.byte	0x1
	.4byte	.L_T165
.L_t1751_e:
	.2byte	0x23
	.2byte	.L_l1751_e-.L_l1751
.L_l1751:
	.byte	0x4
	.4byte	0x17c
	.byte	0x7
.L_l1751_e:
.L_D1751_e:
.L_D1752:
	.4byte	.L_D1752_e-.L_D1752
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1753
	.2byte	0x38
	.string	"p_orphan"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1752_e-.L_t1752
.L_t1752:
	.byte	0x1
	.4byte	.L_T165
.L_t1752_e:
	.2byte	0x23
	.2byte	.L_l1752_e-.L_l1752
.L_l1752:
	.byte	0x4
	.4byte	0x180
	.byte	0x7
.L_l1752_e:
.L_D1752_e:
.L_D1753:
	.4byte	.L_D1753_e-.L_D1753
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1754
	.2byte	0x38
	.string	"p_nextorph"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1753_e-.L_t1753
.L_t1753:
	.byte	0x1
	.4byte	.L_T165
.L_t1753_e:
	.2byte	0x23
	.2byte	.L_l1753_e-.L_l1753
.L_l1753:
	.byte	0x4
	.4byte	0x184
	.byte	0x7
.L_l1753_e:
.L_D1753_e:
.L_D1754:
	.4byte	.L_D1754_e-.L_D1754
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1755
	.2byte	0x38
	.string	"p_aprocp"
	.2byte	0x142
	.4byte	.L_T165
	.2byte	0x83
	.2byte	.L_t1754_e-.L_t1754
.L_t1754:
	.byte	0x1
	.4byte	.L_T258
.L_t1754_e:
	.2byte	0x23
	.2byte	.L_l1754_e-.L_l1754
.L_l1754:
	.byte	0x4
	.4byte	0x188
	.byte	0x7
.L_l1754_e:
.L_D1754_e:
.L_D1755:
	.4byte	0x4
.L_D1674:
	.4byte	.L_D1674_e-.L_D1674
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1756
	.set	.L_T167,.L_D1674
	.2byte	0x38
	.string	"page"
.L_D1674_e:
.L_D1756:
	.4byte	.L_D1756_e-.L_D1756
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1757
	.set	.L_T170,.L_D1756
	.2byte	0x38
	.string	"vnode"
	.2byte	0xb6
	.4byte	0x40
.L_D1756_e:
.L_D1758:
	.4byte	.L_D1758_e-.L_D1758
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1759
	.2byte	0x38
	.string	"v_flag"
	.2byte	0x142
	.4byte	.L_T170
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1758_e-.L_l1758
.L_l1758:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1758_e:
.L_D1758_e:
.L_D1759:
	.4byte	.L_D1759_e-.L_D1759
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1760
	.2byte	0x38
	.string	"v_count"
	.2byte	0x142
	.4byte	.L_T170
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1759_e-.L_l1759
.L_l1759:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l1759_e:
.L_D1759_e:
.L_D1760:
	.4byte	.L_D1760_e-.L_D1760
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1761
	.2byte	0x38
	.string	"v_vfsmountedhere"
	.2byte	0x142
	.4byte	.L_T170
	.2byte	0x83
	.2byte	.L_t1760_e-.L_t1760
.L_t1760:
	.byte	0x1
	.4byte	.L_T571
.L_t1760_e:
	.2byte	0x23
	.2byte	.L_l1760_e-.L_l1760
.L_l1760:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l1760_e:
.L_D1760_e:
.L_D1761:
	.4byte	.L_D1761_e-.L_D1761
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1762
	.2byte	0x38
	.string	"v_op"
	.2byte	0x142
	.4byte	.L_T170
	.2byte	0x83
	.2byte	.L_t1761_e-.L_t1761
.L_t1761:
	.byte	0x1
	.4byte	.L_T573
.L_t1761_e:
	.2byte	0x23
	.2byte	.L_l1761_e-.L_l1761
.L_l1761:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l1761_e:
.L_D1761_e:
.L_D1762:
	.4byte	.L_D1762_e-.L_D1762
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1763
	.2byte	0x38
	.string	"v_vfsp"
	.2byte	0x142
	.4byte	.L_T170
	.2byte	0x83
	.2byte	.L_t1762_e-.L_t1762
.L_t1762:
	.byte	0x1
	.4byte	.L_T571
.L_t1762_e:
	.2byte	0x23
	.2byte	.L_l1762_e-.L_l1762
.L_l1762:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l1762_e:
.L_D1762_e:
.L_D1763:
	.4byte	.L_D1763_e-.L_D1763
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1764
	.2byte	0x38
	.string	"v_stream"
	.2byte	0x142
	.4byte	.L_T170
	.2byte	0x83
	.2byte	.L_t1763_e-.L_t1763
.L_t1763:
	.byte	0x1
	.4byte	.L_T575
.L_t1763_e:
	.2byte	0x23
	.2byte	.L_l1763_e-.L_l1763
.L_l1763:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l1763_e:
.L_D1763_e:
.L_D1764:
	.4byte	.L_D1764_e-.L_D1764
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1765
	.2byte	0x38
	.string	"v_pages"
	.2byte	0x142
	.4byte	.L_T170
	.2byte	0x83
	.2byte	.L_t1764_e-.L_t1764
.L_t1764:
	.byte	0x1
	.4byte	.L_T167
.L_t1764_e:
	.2byte	0x23
	.2byte	.L_l1764_e-.L_l1764
.L_l1764:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l1764_e:
.L_D1764_e:
.L_D1765:
	.4byte	.L_D1765_e-.L_D1765
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1766
	.2byte	0x38
	.string	"v_type"
	.2byte	0x142
	.4byte	.L_T170
	.2byte	0x72
	.4byte	.L_T569
	.2byte	0x23
	.2byte	.L_l1765_e-.L_l1765
.L_l1765:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l1765_e:
.L_D1765_e:
.L_D1766:
	.4byte	.L_D1766_e-.L_D1766
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1767
	.2byte	0x38
	.string	"v_rdev"
	.2byte	0x142
	.4byte	.L_T170
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1766_e-.L_l1766
.L_l1766:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l1766_e:
.L_D1766_e:
.L_D1767:
	.4byte	.L_D1767_e-.L_D1767
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1768
	.2byte	0x38
	.string	"v_data"
	.2byte	0x142
	.4byte	.L_T170
	.2byte	0x63
	.2byte	.L_t1767_e-.L_t1767
.L_t1767:
	.byte	0x1
	.2byte	0x1
.L_t1767_e:
	.2byte	0x23
	.2byte	.L_l1767_e-.L_l1767
.L_l1767:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l1767_e:
.L_D1767_e:
.L_D1768:
	.4byte	.L_D1768_e-.L_D1768
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1769
	.2byte	0x38
	.string	"v_filocks"
	.2byte	0x142
	.4byte	.L_T170
	.2byte	0x83
	.2byte	.L_t1768_e-.L_t1768
.L_t1768:
	.byte	0x1
	.4byte	.L_T577
.L_t1768_e:
	.2byte	0x23
	.2byte	.L_l1768_e-.L_l1768
.L_l1768:
	.byte	0x4
	.4byte	0x24
	.byte	0x7
.L_l1768_e:
.L_D1768_e:
.L_D1769:
	.4byte	.L_D1769_e-.L_D1769
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1770
	.2byte	0x38
	.string	"v_lid"
	.2byte	0x142
	.4byte	.L_T170
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1769_e-.L_l1769
.L_l1769:
	.byte	0x4
	.4byte	0x28
	.byte	0x7
.L_l1769_e:
.L_D1769_e:
.L_D1770:
	.4byte	.L_D1770_e-.L_D1770
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1771
	.2byte	0x38
	.string	"v_cmwlid"
	.2byte	0x142
	.4byte	.L_T170
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1770_e-.L_l1770
.L_l1770:
	.byte	0x4
	.4byte	0x2c
	.byte	0x7
.L_l1770_e:
.L_D1770_e:
.L_D1771:
	.4byte	.L_D1771_e-.L_D1771
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1772
	.2byte	0x38
	.string	"v_macflag"
	.2byte	0x142
	.4byte	.L_T170
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1771_e-.L_l1771
.L_l1771:
	.byte	0x4
	.4byte	0x30
	.byte	0x7
.L_l1771_e:
.L_D1771_e:
.L_D1772:
	.4byte	.L_D1772_e-.L_D1772
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1773
	.2byte	0x38
	.string	"v_filler"
	.2byte	0x142
	.4byte	.L_T170
	.2byte	0x72
	.4byte	.L_T580
	.2byte	0x23
	.2byte	.L_l1772_e-.L_l1772
.L_l1772:
	.byte	0x4
	.4byte	0x34
	.byte	0x7
.L_l1772_e:
.L_D1772_e:
.L_D1773:
	.4byte	0x4
.L_D1757:
	.4byte	.L_D1757_e-.L_D1757
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1774
	.set	.L_T225,.L_D1757
	.2byte	0x38
	.string	"sess"
.L_D1757_e:
.L_D1774:
	.4byte	.L_D1774_e-.L_D1774
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D1775
	.set	.L_T227,.L_D1774
	.2byte	0xb6
	.4byte	0x4
.L_D1774_e:
.L_D1776:
	.4byte	.L_D1776_e-.L_D1776
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1777
	.2byte	0x38
	.string	"p_cad"
	.2byte	0x142
	.4byte	.L_T227
	.2byte	0x63
	.2byte	.L_t1776_e-.L_t1776
.L_t1776:
	.byte	0x1
	.2byte	0x1
.L_t1776_e:
	.2byte	0x23
	.2byte	.L_l1776_e-.L_l1776
.L_l1776:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1776_e:
.L_D1776_e:
.L_D1777:
	.4byte	.L_D1777_e-.L_D1777
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1778
	.2byte	0x38
	.string	"p_int"
	.2byte	0x142
	.4byte	.L_T227
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1777_e-.L_l1777
.L_l1777:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1777_e:
.L_D1777_e:
.L_D1778:
	.4byte	0x4
.L_D1775:
	.4byte	.L_D1775_e-.L_D1775
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1779
	.set	.L_T228,.L_D1775
	.2byte	0x38
	.string	"swnotify"
.L_D1775_e:
.L_D1779:
	.4byte	.L_D1779_e-.L_D1779
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1780
	.set	.L_T230,.L_D1779
	.2byte	0x38
	.string	"vm86"
.L_D1779_e:
.L_D1780:
	.4byte	.L_D1780_e-.L_D1780
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1781
	.set	.L_T232,.L_D1780
	.2byte	0x38
	.string	"as"
.L_D1780_e:
.L_D1781:
	.4byte	.L_D1781_e-.L_D1781
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1782
	.set	.L_T234,.L_D1781
	.2byte	0x38
	.string	"v86dat"
.L_D1781_e:
.L_D1782:
	.4byte	.L_D1782_e-.L_D1782
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D1783
	.set	.L_T237,.L_D1782
	.2byte	0xa3
	.2byte	.L_s1782_e-.L_s1782
.L_s1782:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x45
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s1782_e:
.L_D1782_e:
.L_D1783:
	.4byte	.L_D1783_e-.L_D1783
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D1784
	.set	.L_T210,.L_D1783
	.2byte	0x38
	.string	"pte"
	.2byte	0xb6
	.4byte	0x4
.L_D1783_e:
.L_D1785:
	.4byte	.L_D1785_e-.L_D1785
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1786
	.2byte	0x38
	.string	"pgm"
	.2byte	0x142
	.4byte	.L_T210
	.2byte	0x72
	.4byte	.L_T211
	.2byte	0x23
	.2byte	.L_l1785_e-.L_l1785
.L_l1785:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1785_e:
.L_D1785_e:
.L_D1786:
	.4byte	.L_D1786_e-.L_D1786
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1787
	.2byte	0x38
	.string	"pg_pte"
	.2byte	0x142
	.4byte	.L_T210
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1786_e-.L_l1786
.L_l1786:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1786_e:
.L_D1786_e:
.L_D1787:
	.4byte	0x4
.L_D1784:
	.4byte	.L_D1784_e-.L_D1784
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D1788
	.set	.L_T239,.L_D1784
	.2byte	0xa3
	.2byte	.L_s1784_e-.L_s1784
.L_s1784:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x33
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s1784_e:
.L_D1784_e:
.L_D1788:
	.4byte	.L_D1788_e-.L_D1788
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1789
	.set	.L_T240,.L_D1788
	.2byte	0x38
	.string	"evpd"
.L_D1788_e:
.L_D1789:
	.4byte	.L_D1789_e-.L_D1789
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1790
	.set	.L_T242,.L_D1789
	.2byte	0x38
	.string	"timer"
.L_D1789_e:
.L_D1790:
	.4byte	.L_D1790_e-.L_D1790
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D1791
	.set	.L_T246,.L_D1790
	.2byte	0xa3
	.2byte	.L_s1790_e-.L_s1790
.L_s1790:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x1
	.byte	0x8
	.2byte	0x83
	.2byte	.L_t1790_e-.L_t1790
.L_t1790:
	.byte	0x1
	.4byte	.L_T242
.L_t1790_e:
.L_s1790_e:
.L_D1790_e:
.L_D1791:
	.4byte	.L_D1791_e-.L_D1791
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1792
	.set	.L_T247,.L_D1791
	.2byte	0x38
	.string	"classfuncs"
.L_D1791_e:
.L_D1792:
	.4byte	.L_D1792_e-.L_D1792
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1793
	.set	.L_T249,.L_D1792
	.2byte	0x38
	.string	"sigqueue"
	.2byte	0xb6
	.4byte	0x20
.L_D1792_e:
.L_D1794:
	.4byte	.L_D1794_e-.L_D1794
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1795
	.2byte	0x38
	.string	"sq_next"
	.2byte	0x142
	.4byte	.L_T249
	.2byte	0x83
	.2byte	.L_t1794_e-.L_t1794
.L_t1794:
	.byte	0x1
	.4byte	.L_T249
.L_t1794_e:
	.2byte	0x23
	.2byte	.L_l1794_e-.L_l1794
.L_l1794:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1794_e:
.L_D1794_e:
.L_D1795:
	.4byte	.L_D1795_e-.L_D1795
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1796
	.2byte	0x38
	.string	"sq_info"
	.2byte	0x142
	.4byte	.L_T249
	.2byte	0x72
	.4byte	.L_T307
	.2byte	0x23
	.2byte	.L_l1795_e-.L_l1795
.L_l1795:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l1795_e:
.L_D1795_e:
.L_D1796:
	.4byte	0x4
.L_D1793:
	.4byte	.L_D1793_e-.L_D1793
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1797
	.set	.L_T252,.L_D1793
	.2byte	0x38
	.string	"seguser"
	.2byte	0xb6
	.4byte	0x1518
.L_D1793_e:
.L_D1798:
	.4byte	.L_D1798_e-.L_D1798
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1799
	.2byte	0x38
	.string	"segusr"
	.2byte	0x142
	.4byte	.L_T252
	.2byte	0x72
	.4byte	.L_T425
	.2byte	0x23
	.2byte	.L_l1798_e-.L_l1798
.L_l1798:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1798_e:
.L_D1798_e:
.L_D1799:
	.4byte	0x4
.L_D1797:
	.4byte	.L_D1797_e-.L_D1797
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1800
	.set	.L_T254,.L_D1797
	.2byte	0x38
	.string	"pid"
	.2byte	0xb6
	.4byte	0x10
.L_D1797_e:
.L_D1801:
	.4byte	.L_D1801_e-.L_D1801
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1802
	.2byte	0x38
	.string	"pid_prinactive"
	.2byte	0x142
	.4byte	.L_T254
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0x1f
	.2byte	0x23
	.2byte	.L_l1801_e-.L_l1801
.L_l1801:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1801_e:
.L_D1801_e:
.L_D1802:
	.4byte	.L_D1802_e-.L_D1802
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1803
	.2byte	0x38
	.string	"pid_pgorphaned"
	.2byte	0x142
	.4byte	.L_T254
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0x1e
	.2byte	0x23
	.2byte	.L_l1802_e-.L_l1802
.L_l1802:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1802_e:
.L_D1802_e:
.L_D1803:
	.4byte	.L_D1803_e-.L_D1803
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1804
	.2byte	0x38
	.string	"pid_ref"
	.2byte	0x142
	.4byte	.L_T254
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x6
	.2byte	0xc5
	.2byte	0x18
	.2byte	0x23
	.2byte	.L_l1803_e-.L_l1803
.L_l1803:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1803_e:
.L_D1803_e:
.L_D1804:
	.4byte	.L_D1804_e-.L_D1804
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1805
	.2byte	0x38
	.string	"pid_prslot"
	.2byte	0x142
	.4byte	.L_T254
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x18
	.2byte	0xc5
	.2byte	0x0
	.2byte	0x23
	.2byte	.L_l1804_e-.L_l1804
.L_l1804:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1804_e:
.L_D1804_e:
.L_D1805:
	.4byte	.L_D1805_e-.L_D1805
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1806
	.2byte	0x38
	.string	"pid_id"
	.2byte	0x142
	.4byte	.L_T254
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1805_e-.L_l1805
.L_l1805:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l1805_e:
.L_D1805_e:
.L_D1806:
	.4byte	.L_D1806_e-.L_D1806
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1807
	.2byte	0x38
	.string	"pid_pglink"
	.2byte	0x142
	.4byte	.L_T254
	.2byte	0x83
	.2byte	.L_t1806_e-.L_t1806
.L_t1806:
	.byte	0x1
	.4byte	.L_T165
.L_t1806_e:
	.2byte	0x23
	.2byte	.L_l1806_e-.L_l1806
.L_l1806:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l1806_e:
.L_D1806_e:
.L_D1807:
	.4byte	.L_D1807_e-.L_D1807
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1808
	.2byte	0x38
	.string	"pid_link"
	.2byte	0x142
	.4byte	.L_T254
	.2byte	0x83
	.2byte	.L_t1807_e-.L_t1807
.L_t1807:
	.byte	0x1
	.4byte	.L_T254
.L_t1807_e:
	.2byte	0x23
	.2byte	.L_l1807_e-.L_l1807
.L_l1807:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l1807_e:
.L_D1807_e:
.L_D1808:
	.4byte	0x4
.L_D1800:
	.4byte	.L_D1800_e-.L_D1800
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1809
	.set	.L_T256,.L_D1800
	.2byte	0x38
	.string	"sd"
.L_D1800_e:
.L_D1809:
	.4byte	.L_D1809_e-.L_D1809
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1810
	.set	.L_T258,.L_D1809
	.2byte	0x38
	.string	"aproc"
.L_D1809_e:
.L_D1810:
	.4byte	.L_D1810_e-.L_D1810
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1811
	.set	.L_T571,.L_D1810
	.2byte	0x38
	.string	"vfs"
.L_D1810_e:
.L_D1811:
	.4byte	.L_D1811_e-.L_D1811
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1812
	.set	.L_T573,.L_D1811
	.2byte	0x38
	.string	"vnodeops"
	.2byte	0xb6
	.4byte	0x110
.L_D1811_e:
.L_D1813:
	.4byte	.L_D1813_e-.L_D1813
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1814
	.2byte	0x38
	.string	"vop_open"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1813_e-.L_t1813
.L_t1813:
	.byte	0x1
	.4byte	.L_T38
.L_t1813_e:
	.2byte	0x23
	.2byte	.L_l1813_e-.L_l1813
.L_l1813:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1813_e:
.L_D1813_e:
.L_D1814:
	.4byte	.L_D1814_e-.L_D1814
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1815
	.2byte	0x38
	.string	"vop_close"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1814_e-.L_t1814
.L_t1814:
	.byte	0x1
	.4byte	.L_T38
.L_t1814_e:
	.2byte	0x23
	.2byte	.L_l1814_e-.L_l1814
.L_l1814:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l1814_e:
.L_D1814_e:
.L_D1815:
	.4byte	.L_D1815_e-.L_D1815
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1816
	.2byte	0x38
	.string	"vop_read"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1815_e-.L_t1815
.L_t1815:
	.byte	0x1
	.4byte	.L_T38
.L_t1815_e:
	.2byte	0x23
	.2byte	.L_l1815_e-.L_l1815
.L_l1815:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l1815_e:
.L_D1815_e:
.L_D1816:
	.4byte	.L_D1816_e-.L_D1816
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1817
	.2byte	0x38
	.string	"vop_write"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1816_e-.L_t1816
.L_t1816:
	.byte	0x1
	.4byte	.L_T38
.L_t1816_e:
	.2byte	0x23
	.2byte	.L_l1816_e-.L_l1816
.L_l1816:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l1816_e:
.L_D1816_e:
.L_D1817:
	.4byte	.L_D1817_e-.L_D1817
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1818
	.2byte	0x38
	.string	"vop_ioctl"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1817_e-.L_t1817
.L_t1817:
	.byte	0x1
	.4byte	.L_T38
.L_t1817_e:
	.2byte	0x23
	.2byte	.L_l1817_e-.L_l1817
.L_l1817:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l1817_e:
.L_D1817_e:
.L_D1818:
	.4byte	.L_D1818_e-.L_D1818
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1819
	.2byte	0x38
	.string	"vop_setfl"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1818_e-.L_t1818
.L_t1818:
	.byte	0x1
	.4byte	.L_T38
.L_t1818_e:
	.2byte	0x23
	.2byte	.L_l1818_e-.L_l1818
.L_l1818:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l1818_e:
.L_D1818_e:
.L_D1819:
	.4byte	.L_D1819_e-.L_D1819
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1820
	.2byte	0x38
	.string	"vop_getattr"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1819_e-.L_t1819
.L_t1819:
	.byte	0x1
	.4byte	.L_T38
.L_t1819_e:
	.2byte	0x23
	.2byte	.L_l1819_e-.L_l1819
.L_l1819:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l1819_e:
.L_D1819_e:
.L_D1820:
	.4byte	.L_D1820_e-.L_D1820
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1821
	.2byte	0x38
	.string	"vop_setattr"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1820_e-.L_t1820
.L_t1820:
	.byte	0x1
	.4byte	.L_T38
.L_t1820_e:
	.2byte	0x23
	.2byte	.L_l1820_e-.L_l1820
.L_l1820:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l1820_e:
.L_D1820_e:
.L_D1821:
	.4byte	.L_D1821_e-.L_D1821
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1822
	.2byte	0x38
	.string	"vop_access"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1821_e-.L_t1821
.L_t1821:
	.byte	0x1
	.4byte	.L_T38
.L_t1821_e:
	.2byte	0x23
	.2byte	.L_l1821_e-.L_l1821
.L_l1821:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l1821_e:
.L_D1821_e:
.L_D1822:
	.4byte	.L_D1822_e-.L_D1822
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1823
	.2byte	0x38
	.string	"vop_lookup"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1822_e-.L_t1822
.L_t1822:
	.byte	0x1
	.4byte	.L_T38
.L_t1822_e:
	.2byte	0x23
	.2byte	.L_l1822_e-.L_l1822
.L_l1822:
	.byte	0x4
	.4byte	0x24
	.byte	0x7
.L_l1822_e:
.L_D1822_e:
.L_D1823:
	.4byte	.L_D1823_e-.L_D1823
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1824
	.2byte	0x38
	.string	"vop_create"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1823_e-.L_t1823
.L_t1823:
	.byte	0x1
	.4byte	.L_T38
.L_t1823_e:
	.2byte	0x23
	.2byte	.L_l1823_e-.L_l1823
.L_l1823:
	.byte	0x4
	.4byte	0x28
	.byte	0x7
.L_l1823_e:
.L_D1823_e:
.L_D1824:
	.4byte	.L_D1824_e-.L_D1824
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1825
	.2byte	0x38
	.string	"vop_remove"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1824_e-.L_t1824
.L_t1824:
	.byte	0x1
	.4byte	.L_T38
.L_t1824_e:
	.2byte	0x23
	.2byte	.L_l1824_e-.L_l1824
.L_l1824:
	.byte	0x4
	.4byte	0x2c
	.byte	0x7
.L_l1824_e:
.L_D1824_e:
.L_D1825:
	.4byte	.L_D1825_e-.L_D1825
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1826
	.2byte	0x38
	.string	"vop_link"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1825_e-.L_t1825
.L_t1825:
	.byte	0x1
	.4byte	.L_T38
.L_t1825_e:
	.2byte	0x23
	.2byte	.L_l1825_e-.L_l1825
.L_l1825:
	.byte	0x4
	.4byte	0x30
	.byte	0x7
.L_l1825_e:
.L_D1825_e:
.L_D1826:
	.4byte	.L_D1826_e-.L_D1826
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1827
	.2byte	0x38
	.string	"vop_rename"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1826_e-.L_t1826
.L_t1826:
	.byte	0x1
	.4byte	.L_T38
.L_t1826_e:
	.2byte	0x23
	.2byte	.L_l1826_e-.L_l1826
.L_l1826:
	.byte	0x4
	.4byte	0x34
	.byte	0x7
.L_l1826_e:
.L_D1826_e:
.L_D1827:
	.4byte	.L_D1827_e-.L_D1827
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1828
	.2byte	0x38
	.string	"vop_mkdir"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1827_e-.L_t1827
.L_t1827:
	.byte	0x1
	.4byte	.L_T38
.L_t1827_e:
	.2byte	0x23
	.2byte	.L_l1827_e-.L_l1827
.L_l1827:
	.byte	0x4
	.4byte	0x38
	.byte	0x7
.L_l1827_e:
.L_D1827_e:
.L_D1828:
	.4byte	.L_D1828_e-.L_D1828
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1829
	.2byte	0x38
	.string	"vop_rmdir"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1828_e-.L_t1828
.L_t1828:
	.byte	0x1
	.4byte	.L_T38
.L_t1828_e:
	.2byte	0x23
	.2byte	.L_l1828_e-.L_l1828
.L_l1828:
	.byte	0x4
	.4byte	0x3c
	.byte	0x7
.L_l1828_e:
.L_D1828_e:
.L_D1829:
	.4byte	.L_D1829_e-.L_D1829
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1830
	.2byte	0x38
	.string	"vop_readdir"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1829_e-.L_t1829
.L_t1829:
	.byte	0x1
	.4byte	.L_T38
.L_t1829_e:
	.2byte	0x23
	.2byte	.L_l1829_e-.L_l1829
.L_l1829:
	.byte	0x4
	.4byte	0x40
	.byte	0x7
.L_l1829_e:
.L_D1829_e:
.L_D1830:
	.4byte	.L_D1830_e-.L_D1830
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1831
	.2byte	0x38
	.string	"vop_symlink"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1830_e-.L_t1830
.L_t1830:
	.byte	0x1
	.4byte	.L_T38
.L_t1830_e:
	.2byte	0x23
	.2byte	.L_l1830_e-.L_l1830
.L_l1830:
	.byte	0x4
	.4byte	0x44
	.byte	0x7
.L_l1830_e:
.L_D1830_e:
.L_D1831:
	.4byte	.L_D1831_e-.L_D1831
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1832
	.2byte	0x38
	.string	"vop_readlink"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1831_e-.L_t1831
.L_t1831:
	.byte	0x1
	.4byte	.L_T38
.L_t1831_e:
	.2byte	0x23
	.2byte	.L_l1831_e-.L_l1831
.L_l1831:
	.byte	0x4
	.4byte	0x48
	.byte	0x7
.L_l1831_e:
.L_D1831_e:
.L_D1832:
	.4byte	.L_D1832_e-.L_D1832
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1833
	.2byte	0x38
	.string	"vop_fsync"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1832_e-.L_t1832
.L_t1832:
	.byte	0x1
	.4byte	.L_T38
.L_t1832_e:
	.2byte	0x23
	.2byte	.L_l1832_e-.L_l1832
.L_l1832:
	.byte	0x4
	.4byte	0x4c
	.byte	0x7
.L_l1832_e:
.L_D1832_e:
.L_D1833:
	.4byte	.L_D1833_e-.L_D1833
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1834
	.2byte	0x38
	.string	"vop_inactive"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1833_e-.L_t1833
.L_t1833:
	.byte	0x1
	.4byte	.L_T41
.L_t1833_e:
	.2byte	0x23
	.2byte	.L_l1833_e-.L_l1833
.L_l1833:
	.byte	0x4
	.4byte	0x50
	.byte	0x7
.L_l1833_e:
.L_D1833_e:
.L_D1834:
	.4byte	.L_D1834_e-.L_D1834
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1835
	.2byte	0x38
	.string	"vop_fid"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1834_e-.L_t1834
.L_t1834:
	.byte	0x1
	.4byte	.L_T38
.L_t1834_e:
	.2byte	0x23
	.2byte	.L_l1834_e-.L_l1834
.L_l1834:
	.byte	0x4
	.4byte	0x54
	.byte	0x7
.L_l1834_e:
.L_D1834_e:
.L_D1835:
	.4byte	.L_D1835_e-.L_D1835
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1836
	.2byte	0x38
	.string	"vop_rwlock"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1835_e-.L_t1835
.L_t1835:
	.byte	0x1
	.4byte	.L_T41
.L_t1835_e:
	.2byte	0x23
	.2byte	.L_l1835_e-.L_l1835
.L_l1835:
	.byte	0x4
	.4byte	0x58
	.byte	0x7
.L_l1835_e:
.L_D1835_e:
.L_D1836:
	.4byte	.L_D1836_e-.L_D1836
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1837
	.2byte	0x38
	.string	"vop_rwunlock"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1836_e-.L_t1836
.L_t1836:
	.byte	0x1
	.4byte	.L_T41
.L_t1836_e:
	.2byte	0x23
	.2byte	.L_l1836_e-.L_l1836
.L_l1836:
	.byte	0x4
	.4byte	0x5c
	.byte	0x7
.L_l1836_e:
.L_D1836_e:
.L_D1837:
	.4byte	.L_D1837_e-.L_D1837
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1838
	.2byte	0x38
	.string	"vop_seek"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1837_e-.L_t1837
.L_t1837:
	.byte	0x1
	.4byte	.L_T38
.L_t1837_e:
	.2byte	0x23
	.2byte	.L_l1837_e-.L_l1837
.L_l1837:
	.byte	0x4
	.4byte	0x60
	.byte	0x7
.L_l1837_e:
.L_D1837_e:
.L_D1838:
	.4byte	.L_D1838_e-.L_D1838
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1839
	.2byte	0x38
	.string	"vop_cmp"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1838_e-.L_t1838
.L_t1838:
	.byte	0x1
	.4byte	.L_T38
.L_t1838_e:
	.2byte	0x23
	.2byte	.L_l1838_e-.L_l1838
.L_l1838:
	.byte	0x4
	.4byte	0x64
	.byte	0x7
.L_l1838_e:
.L_D1838_e:
.L_D1839:
	.4byte	.L_D1839_e-.L_D1839
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1840
	.2byte	0x38
	.string	"vop_frlock"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1839_e-.L_t1839
.L_t1839:
	.byte	0x1
	.4byte	.L_T38
.L_t1839_e:
	.2byte	0x23
	.2byte	.L_l1839_e-.L_l1839
.L_l1839:
	.byte	0x4
	.4byte	0x68
	.byte	0x7
.L_l1839_e:
.L_D1839_e:
.L_D1840:
	.4byte	.L_D1840_e-.L_D1840
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1841
	.2byte	0x38
	.string	"vop_space"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1840_e-.L_t1840
.L_t1840:
	.byte	0x1
	.4byte	.L_T38
.L_t1840_e:
	.2byte	0x23
	.2byte	.L_l1840_e-.L_l1840
.L_l1840:
	.byte	0x4
	.4byte	0x6c
	.byte	0x7
.L_l1840_e:
.L_D1840_e:
.L_D1841:
	.4byte	.L_D1841_e-.L_D1841
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1842
	.2byte	0x38
	.string	"vop_realvp"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1841_e-.L_t1841
.L_t1841:
	.byte	0x1
	.4byte	.L_T38
.L_t1841_e:
	.2byte	0x23
	.2byte	.L_l1841_e-.L_l1841
.L_l1841:
	.byte	0x4
	.4byte	0x70
	.byte	0x7
.L_l1841_e:
.L_D1841_e:
.L_D1842:
	.4byte	.L_D1842_e-.L_D1842
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1843
	.2byte	0x38
	.string	"vop_getpage"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1842_e-.L_t1842
.L_t1842:
	.byte	0x1
	.4byte	.L_T38
.L_t1842_e:
	.2byte	0x23
	.2byte	.L_l1842_e-.L_l1842
.L_l1842:
	.byte	0x4
	.4byte	0x74
	.byte	0x7
.L_l1842_e:
.L_D1842_e:
.L_D1843:
	.4byte	.L_D1843_e-.L_D1843
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1844
	.2byte	0x38
	.string	"vop_putpage"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1843_e-.L_t1843
.L_t1843:
	.byte	0x1
	.4byte	.L_T38
.L_t1843_e:
	.2byte	0x23
	.2byte	.L_l1843_e-.L_l1843
.L_l1843:
	.byte	0x4
	.4byte	0x78
	.byte	0x7
.L_l1843_e:
.L_D1843_e:
.L_D1844:
	.4byte	.L_D1844_e-.L_D1844
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1845
	.2byte	0x38
	.string	"vop_map"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1844_e-.L_t1844
.L_t1844:
	.byte	0x1
	.4byte	.L_T38
.L_t1844_e:
	.2byte	0x23
	.2byte	.L_l1844_e-.L_l1844
.L_l1844:
	.byte	0x4
	.4byte	0x7c
	.byte	0x7
.L_l1844_e:
.L_D1844_e:
.L_D1845:
	.4byte	.L_D1845_e-.L_D1845
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1846
	.2byte	0x38
	.string	"vop_addmap"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1845_e-.L_t1845
.L_t1845:
	.byte	0x1
	.4byte	.L_T38
.L_t1845_e:
	.2byte	0x23
	.2byte	.L_l1845_e-.L_l1845
.L_l1845:
	.byte	0x4
	.4byte	0x80
	.byte	0x7
.L_l1845_e:
.L_D1845_e:
.L_D1846:
	.4byte	.L_D1846_e-.L_D1846
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1847
	.2byte	0x38
	.string	"vop_delmap"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1846_e-.L_t1846
.L_t1846:
	.byte	0x1
	.4byte	.L_T38
.L_t1846_e:
	.2byte	0x23
	.2byte	.L_l1846_e-.L_l1846
.L_l1846:
	.byte	0x4
	.4byte	0x84
	.byte	0x7
.L_l1846_e:
.L_D1846_e:
.L_D1847:
	.4byte	.L_D1847_e-.L_D1847
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1848
	.2byte	0x38
	.string	"vop_poll"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1847_e-.L_t1847
.L_t1847:
	.byte	0x1
	.4byte	.L_T38
.L_t1847_e:
	.2byte	0x23
	.2byte	.L_l1847_e-.L_l1847
.L_l1847:
	.byte	0x4
	.4byte	0x88
	.byte	0x7
.L_l1847_e:
.L_D1847_e:
.L_D1848:
	.4byte	.L_D1848_e-.L_D1848
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1849
	.2byte	0x38
	.string	"vop_dump"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1848_e-.L_t1848
.L_t1848:
	.byte	0x1
	.4byte	.L_T38
.L_t1848_e:
	.2byte	0x23
	.2byte	.L_l1848_e-.L_l1848
.L_l1848:
	.byte	0x4
	.4byte	0x8c
	.byte	0x7
.L_l1848_e:
.L_D1848_e:
.L_D1849:
	.4byte	.L_D1849_e-.L_D1849
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1850
	.2byte	0x38
	.string	"vop_pathconf"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1849_e-.L_t1849
.L_t1849:
	.byte	0x1
	.4byte	.L_T38
.L_t1849_e:
	.2byte	0x23
	.2byte	.L_l1849_e-.L_l1849
.L_l1849:
	.byte	0x4
	.4byte	0x90
	.byte	0x7
.L_l1849_e:
.L_D1849_e:
.L_D1850:
	.4byte	.L_D1850_e-.L_D1850
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1851
	.2byte	0x38
	.string	"vop_allocstore"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1850_e-.L_t1850
.L_t1850:
	.byte	0x1
	.4byte	.L_T38
.L_t1850_e:
	.2byte	0x23
	.2byte	.L_l1850_e-.L_l1850
.L_l1850:
	.byte	0x4
	.4byte	0x94
	.byte	0x7
.L_l1850_e:
.L_D1850_e:
.L_D1851:
	.4byte	.L_D1851_e-.L_D1851
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1852
	.2byte	0x38
	.string	"vop_getacl"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1851_e-.L_t1851
.L_t1851:
	.byte	0x1
	.4byte	.L_T38
.L_t1851_e:
	.2byte	0x23
	.2byte	.L_l1851_e-.L_l1851
.L_l1851:
	.byte	0x4
	.4byte	0x98
	.byte	0x7
.L_l1851_e:
.L_D1851_e:
.L_D1852:
	.4byte	.L_D1852_e-.L_D1852
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1853
	.2byte	0x38
	.string	"vop_getaclcnt"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1852_e-.L_t1852
.L_t1852:
	.byte	0x1
	.4byte	.L_T38
.L_t1852_e:
	.2byte	0x23
	.2byte	.L_l1852_e-.L_l1852
.L_l1852:
	.byte	0x4
	.4byte	0x9c
	.byte	0x7
.L_l1852_e:
.L_D1852_e:
.L_D1853:
	.4byte	.L_D1853_e-.L_D1853
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1854
	.2byte	0x38
	.string	"vop_setacl"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1853_e-.L_t1853
.L_t1853:
	.byte	0x1
	.4byte	.L_T38
.L_t1853_e:
	.2byte	0x23
	.2byte	.L_l1853_e-.L_l1853
.L_l1853:
	.byte	0x4
	.4byte	0xa0
	.byte	0x7
.L_l1853_e:
.L_D1853_e:
.L_D1854:
	.4byte	.L_D1854_e-.L_D1854
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1855
	.2byte	0x38
	.string	"vop_setlevel"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1854_e-.L_t1854
.L_t1854:
	.byte	0x1
	.4byte	.L_T38
.L_t1854_e:
	.2byte	0x23
	.2byte	.L_l1854_e-.L_l1854
.L_l1854:
	.byte	0x4
	.4byte	0xa4
	.byte	0x7
.L_l1854_e:
.L_D1854_e:
.L_D1855:
	.4byte	.L_D1855_e-.L_D1855
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1856
	.2byte	0x38
	.string	"vop_getdvstat"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1855_e-.L_t1855
.L_t1855:
	.byte	0x1
	.4byte	.L_T38
.L_t1855_e:
	.2byte	0x23
	.2byte	.L_l1855_e-.L_l1855
.L_l1855:
	.byte	0x4
	.4byte	0xa8
	.byte	0x7
.L_l1855_e:
.L_D1855_e:
.L_D1856:
	.4byte	.L_D1856_e-.L_D1856
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1857
	.2byte	0x38
	.string	"vop_setdvstat"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1856_e-.L_t1856
.L_t1856:
	.byte	0x1
	.4byte	.L_T38
.L_t1856_e:
	.2byte	0x23
	.2byte	.L_l1856_e-.L_l1856
.L_l1856:
	.byte	0x4
	.4byte	0xac
	.byte	0x7
.L_l1856_e:
.L_D1856_e:
.L_D1857:
	.4byte	.L_D1857_e-.L_D1857
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1858
	.2byte	0x38
	.string	"vop_makemld"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x83
	.2byte	.L_t1857_e-.L_t1857
.L_t1857:
	.byte	0x1
	.4byte	.L_T38
.L_t1857_e:
	.2byte	0x23
	.2byte	.L_l1857_e-.L_l1857
.L_l1857:
	.byte	0x4
	.4byte	0xb0
	.byte	0x7
.L_l1857_e:
.L_D1857_e:
.L_D1858:
	.4byte	.L_D1858_e-.L_D1858
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1859
	.2byte	0x38
	.string	"vop_filler"
	.2byte	0x142
	.4byte	.L_T573
	.2byte	0x72
	.4byte	.L_T630
	.2byte	0x23
	.2byte	.L_l1858_e-.L_l1858
.L_l1858:
	.byte	0x4
	.4byte	0xb4
	.byte	0x7
.L_l1858_e:
.L_D1858_e:
.L_D1859:
	.4byte	0x4
.L_D1812:
	.4byte	.L_D1812_e-.L_D1812
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1860
	.set	.L_T575,.L_D1812
	.2byte	0x38
	.string	"stdata"
.L_D1812_e:
.L_D1860:
	.4byte	.L_D1860_e-.L_D1860
	.2byte	0x4
	.2byte	0x12
	.4byte	.L_D1861
	.set	.L_T569,.L_D1860
	.2byte	0x38
	.string	"vtype"
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xf4
	.4byte	.L_e1860_e-.L_e1860
.L_e1860:
	.4byte	0x8
	.string	"VBAD"
	.4byte	0x7
	.string	"VXNAM"
	.4byte	0x6
	.string	"VFIFO"
	.4byte	0x5
	.string	"VLNK"
	.4byte	0x4
	.string	"VCHR"
	.4byte	0x3
	.string	"VBLK"
	.4byte	0x2
	.string	"VDIR"
	.4byte	0x1
	.string	"VREG"
	.4byte	0x0
	.string	"VNON"
.L_e1860_e:
.L_D1860_e:
.L_D1861:
	.4byte	.L_D1861_e-.L_D1861
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1862
	.set	.L_T577,.L_D1861
	.2byte	0x38
	.string	"filock"
.L_D1861_e:
.L_D1862:
	.4byte	.L_D1862_e-.L_D1862
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D1863
	.set	.L_T580,.L_D1862
	.2byte	0xa3
	.2byte	.L_s1862_e-.L_s1862
.L_s1862:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x2
	.byte	0x8
	.2byte	0x55
	.2byte	0xa
.L_s1862_e:
.L_D1862_e:
.L_D1863:
	.4byte	.L_D1863_e-.L_D1863
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1864
	.set	.L_T211,.L_D1863
	.2byte	0xb6
	.4byte	0x4
.L_D1863_e:
.L_D1865:
	.4byte	.L_D1865_e-.L_D1865
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1866
	.2byte	0x38
	.string	"pg_v"
	.2byte	0x142
	.4byte	.L_T211
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0x1f
	.2byte	0x23
	.2byte	.L_l1865_e-.L_l1865
.L_l1865:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1865_e:
.L_D1865_e:
.L_D1866:
	.4byte	.L_D1866_e-.L_D1866
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1867
	.2byte	0x38
	.string	"pg_rw"
	.2byte	0x142
	.4byte	.L_T211
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0x1e
	.2byte	0x23
	.2byte	.L_l1866_e-.L_l1866
.L_l1866:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1866_e:
.L_D1866_e:
.L_D1867:
	.4byte	.L_D1867_e-.L_D1867
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1868
	.2byte	0x38
	.string	"pg_us"
	.2byte	0x142
	.4byte	.L_T211
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0x1d
	.2byte	0x23
	.2byte	.L_l1867_e-.L_l1867
.L_l1867:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1867_e:
.L_D1867_e:
.L_D1868:
	.4byte	.L_D1868_e-.L_D1868
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1869
	.2byte	0x38
	.string	"pg_ref"
	.2byte	0x142
	.4byte	.L_T211
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0x1a
	.2byte	0x23
	.2byte	.L_l1868_e-.L_l1868
.L_l1868:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1868_e:
.L_D1868_e:
.L_D1869:
	.4byte	.L_D1869_e-.L_D1869
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1870
	.2byte	0x38
	.string	"pg_mod"
	.2byte	0x142
	.4byte	.L_T211
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0x19
	.2byte	0x23
	.2byte	.L_l1869_e-.L_l1869
.L_l1869:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1869_e:
.L_D1869_e:
.L_D1870:
	.4byte	.L_D1870_e-.L_D1870
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1871
	.2byte	0x38
	.string	"pg_tag"
	.2byte	0x142
	.4byte	.L_T211
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x3
	.2byte	0xc5
	.2byte	0x14
	.2byte	0x23
	.2byte	.L_l1870_e-.L_l1870
.L_l1870:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1870_e:
.L_D1870_e:
.L_D1871:
	.4byte	.L_D1871_e-.L_D1871
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1872
	.2byte	0x38
	.string	"pg_pfn"
	.2byte	0x142
	.4byte	.L_T211
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x14
	.2byte	0xc5
	.2byte	0x0
	.2byte	0x23
	.2byte	.L_l1871_e-.L_l1871
.L_l1871:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1871_e:
.L_D1871_e:
.L_D1872:
	.4byte	0x4
.L_D1864:
	.4byte	.L_D1864_e-.L_D1864
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1873
	.set	.L_T307,.L_D1864
	.2byte	0x38
	.string	"k_siginfo"
	.2byte	0xb6
	.4byte	0x1c
.L_D1864_e:
.L_D1874:
	.4byte	.L_D1874_e-.L_D1874
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1875
	.2byte	0x38
	.string	"si_signo"
	.2byte	0x142
	.4byte	.L_T307
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1874_e-.L_l1874
.L_l1874:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1874_e:
.L_D1874_e:
.L_D1875:
	.4byte	.L_D1875_e-.L_D1875
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1876
	.2byte	0x38
	.string	"si_code"
	.2byte	0x142
	.4byte	.L_T307
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1875_e-.L_l1875
.L_l1875:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l1875_e:
.L_D1875_e:
.L_D1876:
	.4byte	.L_D1876_e-.L_D1876
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1877
	.2byte	0x38
	.string	"si_errno"
	.2byte	0x142
	.4byte	.L_T307
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1876_e-.L_l1876
.L_l1876:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l1876_e:
.L_D1876_e:
.L_D1877:
	.4byte	.L_D1877_e-.L_D1877
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1878
	.2byte	0x38
	.string	"_data"
	.2byte	0x142
	.4byte	.L_T307
	.2byte	0x72
	.4byte	.L_T314
	.2byte	0x23
	.2byte	.L_l1877_e-.L_l1877
.L_l1877:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l1877_e:
.L_D1877_e:
.L_D1878:
	.4byte	0x4
.L_D1873:
	.4byte	.L_D1873_e-.L_D1873
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D1879
	.set	.L_T425,.L_D1873
	.2byte	0xb6
	.4byte	0x1518
.L_D1873_e:
.L_D1880:
	.4byte	.L_D1880_e-.L_D1880
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1881
	.2byte	0x38
	.string	"segu_stack"
	.2byte	0x142
	.4byte	.L_T425
	.2byte	0x72
	.4byte	.L_T368
	.2byte	0x23
	.2byte	.L_l1880_e-.L_l1880
.L_l1880:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1880_e:
.L_D1880_e:
.L_D1881:
	.4byte	.L_D1881_e-.L_D1881
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1882
	.2byte	0x38
	.string	"segu_u"
	.2byte	0x142
	.4byte	.L_T425
	.2byte	0x72
	.4byte	.L_T366
	.2byte	0x23
	.2byte	.L_l1881_e-.L_l1881
.L_l1881:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1881_e:
.L_D1881_e:
.L_D1882:
	.4byte	0x4
.L_D1879:
	.4byte	.L_D1879_e-.L_D1879
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D1883
	.set	.L_T630,.L_D1879
	.2byte	0xa3
	.2byte	.L_s1879_e-.L_s1879
.L_s1879:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x16
	.byte	0x8
	.2byte	0x83
	.2byte	.L_t1879_e-.L_t1879
.L_t1879:
	.byte	0x1
	.4byte	.L_T38
.L_t1879_e:
.L_s1879_e:
.L_D1879_e:
.L_D1883:
	.4byte	.L_D1883_e-.L_D1883
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D1884
	.set	.L_T314,.L_D1883
	.2byte	0xb6
	.4byte	0x10
.L_D1883_e:
.L_D1885:
	.4byte	.L_D1885_e-.L_D1885
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1886
	.2byte	0x38
	.string	"_proc"
	.2byte	0x142
	.4byte	.L_T314
	.2byte	0x72
	.4byte	.L_T311
	.2byte	0x23
	.2byte	.L_l1885_e-.L_l1885
.L_l1885:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1885_e:
.L_D1885_e:
.L_D1886:
	.4byte	.L_D1886_e-.L_D1886
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1887
	.2byte	0x38
	.string	"_fault"
	.2byte	0x142
	.4byte	.L_T314
	.2byte	0x72
	.4byte	.L_T312
	.2byte	0x23
	.2byte	.L_l1886_e-.L_l1886
.L_l1886:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1886_e:
.L_D1886_e:
.L_D1887:
	.4byte	.L_D1887_e-.L_D1887
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1888
	.2byte	0x38
	.string	"_file"
	.2byte	0x142
	.4byte	.L_T314
	.2byte	0x72
	.4byte	.L_T313
	.2byte	0x23
	.2byte	.L_l1887_e-.L_l1887
.L_l1887:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1887_e:
.L_D1887_e:
.L_D1888:
	.4byte	0x4
.L_D1884:
	.4byte	.L_D1884_e-.L_D1884
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D1889
	.set	.L_T368,.L_D1884
	.2byte	0xa3
	.2byte	.L_s1884_e-.L_s1884
.L_s1884:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0xe7f
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s1884_e:
.L_D1884_e:
.L_D1889:
	.4byte	.L_D1889_e-.L_D1889
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D1890
	.set	.L_T366,.L_D1889
	.2byte	0x38
	.string	"user"
	.2byte	0xb6
	.4byte	0x1518
.L_D1889_e:
.L_D1891:
	.4byte	.L_D1891_e-.L_D1891
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1892
	.2byte	0x38
	.string	"u_stack"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T368
	.2byte	0x23
	.2byte	.L_l1891_e-.L_l1891
.L_l1891:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l1891_e:
.L_D1891_e:
.L_D1892:
	.4byte	.L_D1892_e-.L_D1892
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1893
	.2byte	0x38
	.string	"u_stack_filler_1"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T370
	.2byte	0x23
	.2byte	.L_l1892_e-.L_l1892
.L_l1892:
	.byte	0x4
	.4byte	0xe80
	.byte	0x7
.L_l1892_e:
.L_D1892_e:
.L_D1893:
	.4byte	.L_D1893_e-.L_D1893
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1894
	.2byte	0x38
	.string	"u_fpvalid"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l1893_e-.L_l1893
.L_l1893:
	.byte	0x4
	.4byte	0xe82
	.byte	0x7
.L_l1893_e:
.L_D1893_e:
.L_D1894:
	.4byte	.L_D1894_e-.L_D1894
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1895
	.2byte	0x38
	.string	"u_weitek"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l1894_e-.L_l1894
.L_l1894:
	.byte	0x4
	.4byte	0xe83
	.byte	0x7
.L_l1894_e:
.L_D1894_e:
.L_D1895:
	.4byte	.L_D1895_e-.L_D1895
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1896
	.2byte	0x38
	.string	"u_fps"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T379
	.2byte	0x23
	.2byte	.L_l1895_e-.L_l1895
.L_l1895:
	.byte	0x4
	.4byte	0xe84
	.byte	0x7
.L_l1895_e:
.L_D1895_e:
.L_D1896:
	.4byte	.L_D1896_e-.L_D1896
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1897
	.2byte	0x38
	.string	"u_weitek_reg"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T381
	.2byte	0x23
	.2byte	.L_l1896_e-.L_l1896
.L_l1896:
	.byte	0x4
	.4byte	0xf7c
	.byte	0x7
.L_l1896_e:
.L_D1896_e:
.L_D1897:
	.4byte	.L_D1897_e-.L_D1897
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1898
	.2byte	0x38
	.string	"u_tss"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x83
	.2byte	.L_t1897_e-.L_t1897
.L_t1897:
	.byte	0x1
	.4byte	.L_T340
.L_t1897_e:
	.2byte	0x23
	.2byte	.L_l1897_e-.L_l1897
.L_l1897:
	.byte	0x4
	.4byte	0x1000
	.byte	0x7
.L_l1897_e:
.L_D1897_e:
.L_D1898:
	.4byte	.L_D1898_e-.L_D1898
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1899
	.2byte	0x38
	.string	"u_sztss"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1898_e-.L_l1898
.L_l1898:
	.byte	0x4
	.4byte	0x1004
	.byte	0x7
.L_l1898_e:
.L_D1898_e:
.L_D1899:
	.4byte	.L_D1899_e-.L_D1899
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1900
	.2byte	0x38
	.string	"u_sigfault"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l1899_e-.L_l1899
.L_l1899:
	.byte	0x4
	.4byte	0x1006
	.byte	0x7
.L_l1899_e:
.L_D1899_e:
.L_D1900:
	.4byte	.L_D1900_e-.L_D1900
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1901
	.2byte	0x38
	.string	"u_usigfailed"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l1900_e-.L_l1900
.L_l1900:
	.byte	0x4
	.4byte	0x1007
	.byte	0x7
.L_l1900_e:
.L_D1900_e:
.L_D1901:
	.4byte	.L_D1901_e-.L_D1901
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1902
	.2byte	0x38
	.string	"u_sub"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1901_e-.L_l1901
.L_l1901:
	.byte	0x4
	.4byte	0x1008
	.byte	0x7
.L_l1901_e:
.L_D1901_e:
.L_D1902:
	.4byte	.L_D1902_e-.L_D1902
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1903
	.2byte	0x38
	.string	"u_filler1"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T384
	.2byte	0x23
	.2byte	.L_l1902_e-.L_l1902
.L_l1902:
	.byte	0x4
	.4byte	0x100c
	.byte	0x7
.L_l1902_e:
.L_D1902_e:
.L_D1903:
	.4byte	.L_D1903_e-.L_D1903
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1904
	.2byte	0x38
	.string	"u_386b1"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l1903_e-.L_l1903
.L_l1903:
	.byte	0x4
	.4byte	0x1014
	.byte	0x7
.L_l1903_e:
.L_D1903_e:
.L_D1904:
	.4byte	.L_D1904_e-.L_D1904
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1905
	.2byte	0x38
	.string	"u_raioaddr"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x63
	.2byte	.L_t1904_e-.L_t1904
.L_t1904:
	.byte	0x1
	.2byte	0x1
.L_t1904_e:
	.2byte	0x23
	.2byte	.L_l1904_e-.L_l1904
.L_l1904:
	.byte	0x4
	.4byte	0x1018
	.byte	0x7
.L_l1904_e:
.L_D1904_e:
.L_D1905:
	.4byte	.L_D1905_e-.L_D1905
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1906
	.2byte	0x38
	.string	"u_raiosize"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1905_e-.L_l1905
.L_l1905:
	.byte	0x4
	.4byte	0x101c
	.byte	0x7
.L_l1905_e:
.L_D1905_e:
.L_D1906:
	.4byte	.L_D1906_e-.L_D1906
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1907
	.2byte	0x38
	.string	"u_escbug"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x63
	.2byte	.L_t1906_e-.L_t1906
.L_t1906:
	.byte	0x1
	.2byte	0x1
.L_t1906_e:
	.2byte	0x23
	.2byte	.L_l1906_e-.L_l1906
.L_l1906:
	.byte	0x4
	.4byte	0x1020
	.byte	0x7
.L_l1906_e:
.L_D1906_e:
.L_D1907:
	.4byte	.L_D1907_e-.L_D1907
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1908
	.2byte	0x38
	.string	"u_tss_desc"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T341
	.2byte	0x23
	.2byte	.L_l1907_e-.L_l1907
.L_l1907:
	.byte	0x4
	.4byte	0x1024
	.byte	0x7
.L_l1907_e:
.L_D1907_e:
.L_D1908:
	.4byte	.L_D1908_e-.L_D1908
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1909
	.2byte	0x38
	.string	"u_ldt_desc"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T341
	.2byte	0x23
	.2byte	.L_l1908_e-.L_l1908
.L_l1908:
	.byte	0x4
	.4byte	0x102c
	.byte	0x7
.L_l1908_e:
.L_D1908_e:
.L_D1909:
	.4byte	.L_D1909_e-.L_D1909
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1910
	.2byte	0x38
	.string	"u_filler2"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T386
	.2byte	0x23
	.2byte	.L_l1909_e-.L_l1909
.L_l1909:
	.byte	0x4
	.4byte	0x1034
	.byte	0x7
.L_l1909_e:
.L_D1909_e:
.L_D1910:
	.4byte	.L_D1910_e-.L_D1910
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1911
	.2byte	0x38
	.string	"u_nshmseg"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l1910_e-.L_l1910
.L_l1910:
	.byte	0x4
	.4byte	0x1038
	.byte	0x7
.L_l1910_e:
.L_D1910_e:
.L_D1911:
	.4byte	.L_D1911_e-.L_D1911
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1912
	.2byte	0x38
	.string	"rfs_pad0"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1911_e-.L_l1911
.L_l1911:
	.byte	0x4
	.4byte	0x103a
	.byte	0x7
.L_l1911_e:
.L_D1911_e:
.L_D1912:
	.4byte	.L_D1912_e-.L_D1912
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1913
	.2byte	0x38
	.string	"rfs_pad1"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1912_e-.L_l1912
.L_l1912:
	.byte	0x4
	.4byte	0x103c
	.byte	0x7
.L_l1912_e:
.L_D1912_e:
.L_D1913:
	.4byte	.L_D1913_e-.L_D1913
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1914
	.2byte	0x38
	.string	"u_syscall"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1913_e-.L_l1913
.L_l1913:
	.byte	0x4
	.4byte	0x1040
	.byte	0x7
.L_l1913_e:
.L_D1913_e:
.L_D1914:
	.4byte	.L_D1914_e-.L_D1914
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1915
	.2byte	0x38
	.string	"u_srchan"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x83
	.2byte	.L_t1914_e-.L_t1914
.L_t1914:
	.byte	0x1
	.4byte	.L_T387
.L_t1914_e:
	.2byte	0x23
	.2byte	.L_l1914_e-.L_l1914
.L_l1914:
	.byte	0x4
	.4byte	0x1044
	.byte	0x7
.L_l1914_e:
.L_D1914_e:
.L_D1915:
	.4byte	.L_D1915_e-.L_D1915
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1916
	.2byte	0x38
	.string	"rfs_pad2"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l1915_e-.L_l1915
.L_l1915:
	.byte	0x4
	.4byte	0x1048
	.byte	0x7
.L_l1915_e:
.L_D1915_e:
.L_D1916:
	.4byte	.L_D1916_e-.L_D1916
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1917
	.2byte	0x38
	.string	"u_userstack"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1916_e-.L_l1916
.L_l1916:
	.byte	0x4
	.4byte	0x104c
	.byte	0x7
.L_l1916_e:
.L_D1916_e:
.L_D1917:
	.4byte	.L_D1917_e-.L_D1917
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1918
	.2byte	0x38
	.string	"u_bsize"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l1917_e-.L_l1917
.L_l1917:
	.byte	0x4
	.4byte	0x1050
	.byte	0x7
.L_l1917_e:
.L_D1917_e:
.L_D1918:
	.4byte	.L_D1918_e-.L_D1918
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1919
	.2byte	0x38
	.string	"u_execsw"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x83
	.2byte	.L_t1918_e-.L_t1918
.L_t1918:
	.byte	0x1
	.4byte	.L_T389
.L_t1918_e:
	.2byte	0x23
	.2byte	.L_l1918_e-.L_l1918
.L_l1918:
	.byte	0x4
	.4byte	0x1054
	.byte	0x7
.L_l1918_e:
.L_D1918_e:
.L_D1919:
	.4byte	.L_D1919_e-.L_D1919
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1920
	.2byte	0x38
	.string	"u_ageinterval"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1919_e-.L_l1919
.L_l1919:
	.byte	0x4
	.4byte	0x1058
	.byte	0x7
.L_l1919_e:
.L_D1919_e:
.L_D1920:
	.4byte	.L_D1920_e-.L_D1920
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1921
	.2byte	0x38
	.string	"u_psargs"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T392
	.2byte	0x23
	.2byte	.L_l1920_e-.L_l1920
.L_l1920:
	.byte	0x4
	.4byte	0x105c
	.byte	0x7
.L_l1920_e:
.L_D1920_e:
.L_D1921:
	.4byte	.L_D1921_e-.L_D1921
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1922
	.2byte	0x38
	.string	"u_tracepc"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x63
	.2byte	.L_t1921_e-.L_t1921
.L_t1921:
	.byte	0x1
	.2byte	0x1
.L_t1921_e:
	.2byte	0x23
	.2byte	.L_l1921_e-.L_l1921
.L_l1921:
	.byte	0x4
	.4byte	0x10ac
	.byte	0x7
.L_l1921_e:
.L_D1921_e:
.L_D1922:
	.4byte	.L_D1922_e-.L_D1922
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1923
	.2byte	0x38
	.string	"u_arg"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T394
	.2byte	0x23
	.2byte	.L_l1922_e-.L_l1922
.L_l1922:
	.byte	0x4
	.4byte	0x10b0
	.byte	0x7
.L_l1922_e:
.L_D1922_e:
.L_D1923:
	.4byte	.L_D1923_e-.L_D1923
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1924
	.2byte	0x38
	.string	"u_qsav"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T83
	.2byte	0x23
	.2byte	.L_l1923_e-.L_l1923
.L_l1923:
	.byte	0x4
	.4byte	0x10d0
	.byte	0x7
.L_l1923_e:
.L_D1923_e:
.L_D1924:
	.4byte	.L_D1924_e-.L_D1924
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1925
	.2byte	0x38
	.string	"u_segflg"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l1924_e-.L_l1924
.L_l1924:
	.byte	0x4
	.4byte	0x10e8
	.byte	0x7
.L_l1924_e:
.L_D1924_e:
.L_D1925:
	.4byte	.L_D1925_e-.L_D1925
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1926
	.2byte	0x38
	.string	"u_error"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l1925_e-.L_l1925
.L_l1925:
	.byte	0x4
	.4byte	0x10e9
	.byte	0x7
.L_l1925_e:
.L_D1925_e:
.L_D1926:
	.4byte	.L_D1926_e-.L_D1926
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1927
	.2byte	0x38
	.string	"u_uid"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1926_e-.L_l1926
.L_l1926:
	.byte	0x4
	.4byte	0x10ea
	.byte	0x7
.L_l1926_e:
.L_D1926_e:
.L_D1927:
	.4byte	.L_D1927_e-.L_D1927
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1928
	.2byte	0x38
	.string	"u_gid"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1927_e-.L_l1927
.L_l1927:
	.byte	0x4
	.4byte	0x10ec
	.byte	0x7
.L_l1927_e:
.L_D1927_e:
.L_D1928:
	.4byte	.L_D1928_e-.L_D1928
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1929
	.2byte	0x38
	.string	"u_ruid"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1928_e-.L_l1928
.L_l1928:
	.byte	0x4
	.4byte	0x10ee
	.byte	0x7
.L_l1928_e:
.L_D1928_e:
.L_D1929:
	.4byte	.L_D1929_e-.L_D1929
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1930
	.2byte	0x38
	.string	"u_rgid"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1929_e-.L_l1929
.L_l1929:
	.byte	0x4
	.4byte	0x10f0
	.byte	0x7
.L_l1929_e:
.L_D1929_e:
.L_D1930:
	.4byte	.L_D1930_e-.L_D1930
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1931
	.2byte	0x38
	.string	"u_procp"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x83
	.2byte	.L_t1930_e-.L_t1930
.L_t1930:
	.byte	0x1
	.4byte	.L_T165
.L_t1930_e:
	.2byte	0x23
	.2byte	.L_l1930_e-.L_l1930
.L_l1930:
	.byte	0x4
	.4byte	0x10f4
	.byte	0x7
.L_l1930_e:
.L_D1930_e:
.L_D1931:
	.4byte	.L_D1931_e-.L_D1931
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1932
	.2byte	0x38
	.string	"u_ap"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x63
	.2byte	.L_t1931_e-.L_t1931
.L_t1931:
	.byte	0x1
	.2byte	0x7
.L_t1931_e:
	.2byte	0x23
	.2byte	.L_l1931_e-.L_l1931
.L_l1931:
	.byte	0x4
	.4byte	0x10f8
	.byte	0x7
.L_l1931_e:
.L_D1931_e:
.L_D1932:
	.4byte	.L_D1932_e-.L_D1932
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1933
	.2byte	0x38
	.string	"u_r"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T396
	.2byte	0x23
	.2byte	.L_l1932_e-.L_l1932
.L_l1932:
	.byte	0x4
	.4byte	0x10fc
	.byte	0x7
.L_l1932_e:
.L_D1932_e:
.L_D1933:
	.4byte	.L_D1933_e-.L_D1933
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1934
	.2byte	0x38
	.string	"u_base"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x63
	.2byte	.L_t1933_e-.L_t1933
.L_t1933:
	.byte	0x1
	.2byte	0x1
.L_t1933_e:
	.2byte	0x23
	.2byte	.L_l1933_e-.L_l1933
.L_l1933:
	.byte	0x4
	.4byte	0x1104
	.byte	0x7
.L_l1933_e:
.L_D1933_e:
.L_D1934:
	.4byte	.L_D1934_e-.L_D1934
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1935
	.2byte	0x38
	.string	"u_count"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1934_e-.L_l1934
.L_l1934:
	.byte	0x4
	.4byte	0x1108
	.byte	0x7
.L_l1934_e:
.L_D1934_e:
.L_D1935:
	.4byte	.L_D1935_e-.L_D1935
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1936
	.2byte	0x38
	.string	"u_offset"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1935_e-.L_l1935
.L_l1935:
	.byte	0x4
	.4byte	0x110c
	.byte	0x7
.L_l1935_e:
.L_D1935_e:
.L_D1936:
	.4byte	.L_D1936_e-.L_D1936
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1937
	.2byte	0x38
	.string	"u_fmode"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l1936_e-.L_l1936
.L_l1936:
	.byte	0x4
	.4byte	0x1110
	.byte	0x7
.L_l1936_e:
.L_D1936_e:
.L_D1937:
	.4byte	.L_D1937_e-.L_D1937
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1938
	.2byte	0x38
	.string	"u_pbsize"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1937_e-.L_l1937
.L_l1937:
	.byte	0x4
	.4byte	0x1112
	.byte	0x7
.L_l1937_e:
.L_D1937_e:
.L_D1938:
	.4byte	.L_D1938_e-.L_D1938
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1939
	.2byte	0x38
	.string	"u_pboff"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1938_e-.L_l1938
.L_l1938:
	.byte	0x4
	.4byte	0x1114
	.byte	0x7
.L_l1938_e:
.L_D1938_e:
.L_D1939:
	.4byte	.L_D1939_e-.L_D1939
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1940
	.2byte	0x38
	.string	"u_errcnt"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l1939_e-.L_l1939
.L_l1939:
	.byte	0x4
	.4byte	0x1116
	.byte	0x7
.L_l1939_e:
.L_D1939_e:
.L_D1940:
	.4byte	.L_D1940_e-.L_D1940
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1941
	.2byte	0x38
	.string	"u_rablock"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1940_e-.L_l1940
.L_l1940:
	.byte	0x4
	.4byte	0x1118
	.byte	0x7
.L_l1940_e:
.L_D1940_e:
.L_D1941:
	.4byte	.L_D1941_e-.L_D1941
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1942
	.2byte	0x38
	.string	"u_sysabort"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1941_e-.L_l1941
.L_l1941:
	.byte	0x4
	.4byte	0x111c
	.byte	0x7
.L_l1941_e:
.L_D1941_e:
.L_D1942:
	.4byte	.L_D1942_e-.L_D1942
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1943
	.2byte	0x38
	.string	"u_entrymask"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T364
	.2byte	0x23
	.2byte	.L_l1942_e-.L_l1942
.L_l1942:
	.byte	0x4
	.4byte	0x1120
	.byte	0x7
.L_l1942_e:
.L_D1942_e:
.L_D1943:
	.4byte	.L_D1943_e-.L_D1943
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1944
	.2byte	0x38
	.string	"u_exitmask"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T364
	.2byte	0x23
	.2byte	.L_l1943_e-.L_l1943
.L_l1943:
	.byte	0x4
	.4byte	0x1160
	.byte	0x7
.L_l1943_e:
.L_D1943_e:
.L_D1944:
	.4byte	.L_D1944_e-.L_D1944
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1945
	.2byte	0x38
	.string	"u_cdir"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x83
	.2byte	.L_t1944_e-.L_t1944
.L_t1944:
	.byte	0x1
	.4byte	.L_T170
.L_t1944_e:
	.2byte	0x23
	.2byte	.L_l1944_e-.L_l1944
.L_l1944:
	.byte	0x4
	.4byte	0x11a0
	.byte	0x7
.L_l1944_e:
.L_D1944_e:
.L_D1945:
	.4byte	.L_D1945_e-.L_D1945
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1946
	.2byte	0x38
	.string	"u_rdir"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x83
	.2byte	.L_t1945_e-.L_t1945
.L_t1945:
	.byte	0x1
	.4byte	.L_T170
.L_t1945_e:
	.2byte	0x23
	.2byte	.L_l1945_e-.L_l1945
.L_l1945:
	.byte	0x4
	.4byte	0x11a4
	.byte	0x7
.L_l1945_e:
.L_D1945_e:
.L_D1946:
	.4byte	.L_D1946_e-.L_D1946
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1947
	.2byte	0x38
	.string	"u_rlimit"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T398
	.2byte	0x23
	.2byte	.L_l1946_e-.L_l1946
.L_l1946:
	.byte	0x4
	.4byte	0x11a8
	.byte	0x7
.L_l1946_e:
.L_D1946_e:
.L_D1947:
	.4byte	.L_D1947_e-.L_D1947
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1948
	.2byte	0x38
	.string	"u_tsize"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1947_e-.L_l1947
.L_l1947:
	.byte	0x4
	.4byte	0x11e0
	.byte	0x7
.L_l1947_e:
.L_D1947_e:
.L_D1948:
	.4byte	.L_D1948_e-.L_D1948
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1949
	.2byte	0x38
	.string	"u_dsize"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1948_e-.L_l1948
.L_l1948:
	.byte	0x4
	.4byte	0x11e4
	.byte	0x7
.L_l1948_e:
.L_D1948_e:
.L_D1949:
	.4byte	.L_D1949_e-.L_D1949
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1950
	.2byte	0x38
	.string	"u_ssize"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1949_e-.L_l1949
.L_l1949:
	.byte	0x4
	.4byte	0x11e8
	.byte	0x7
.L_l1949_e:
.L_D1949_e:
.L_D1950:
	.4byte	.L_D1950_e-.L_D1950
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1951
	.2byte	0x38
	.string	"u_siginfo"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T307
	.2byte	0x23
	.2byte	.L_l1950_e-.L_l1950
.L_l1950:
	.byte	0x4
	.4byte	0x11ec
	.byte	0x7
.L_l1950_e:
.L_D1950_e:
.L_D1951:
	.4byte	.L_D1951_e-.L_D1951
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1952
	.2byte	0x38
	.string	"u_systrap"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1951_e-.L_l1951
.L_l1951:
	.byte	0x4
	.4byte	0x1208
	.byte	0x7
.L_l1951_e:
.L_D1951_e:
.L_D1952:
	.4byte	.L_D1952_e-.L_D1952
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1953
	.2byte	0x38
	.string	"u_execid"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1952_e-.L_l1952
.L_l1952:
	.byte	0x4
	.4byte	0x120c
	.byte	0x7
.L_l1952_e:
.L_D1952_e:
.L_D1953:
	.4byte	.L_D1953_e-.L_D1953
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1954
	.2byte	0x38
	.string	"u_execsz"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l1953_e-.L_l1953
.L_l1953:
	.byte	0x4
	.4byte	0x1210
	.byte	0x7
.L_l1953_e:
.L_D1953_e:
.L_D1954:
	.4byte	.L_D1954_e-.L_D1954
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1955
	.2byte	0x38
	.string	"u_ttyp"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x63
	.2byte	.L_t1954_e-.L_t1954
.L_t1954:
	.byte	0x1
	.2byte	0x4
.L_t1954_e:
	.2byte	0x23
	.2byte	.L_l1954_e-.L_l1954
.L_l1954:
	.byte	0x4
	.4byte	0x1214
	.byte	0x7
.L_l1954_e:
.L_D1954_e:
.L_D1955:
	.4byte	.L_D1955_e-.L_D1955
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1956
	.2byte	0x38
	.string	"u_ttyd"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l1955_e-.L_l1955
.L_l1955:
	.byte	0x4
	.4byte	0x1218
	.byte	0x7
.L_l1955_e:
.L_D1955_e:
.L_D1956:
	.4byte	.L_D1956_e-.L_D1956
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1957
	.2byte	0x38
	.string	"u_sigevpend"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l1956_e-.L_l1956
.L_l1956:
	.byte	0x4
	.4byte	0x121a
	.byte	0x7
.L_l1956_e:
.L_D1956_e:
.L_D1957:
	.4byte	.L_D1957_e-.L_D1957
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1958
	.2byte	0x38
	.string	"u_uservirt"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1957_e-.L_l1957
.L_l1957:
	.byte	0x4
	.4byte	0x121c
	.byte	0x7
.L_l1957_e:
.L_D1957_e:
.L_D1958:
	.4byte	.L_D1958_e-.L_D1958
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1959
	.2byte	0x38
	.string	"u_procvirt"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1958_e-.L_l1958
.L_l1958:
	.byte	0x4
	.4byte	0x1220
	.byte	0x7
.L_l1958_e:
.L_D1958_e:
.L_D1959:
	.4byte	.L_D1959_e-.L_D1959
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1960
	.2byte	0x38
	.string	"u_ar0"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x63
	.2byte	.L_t1959_e-.L_t1959
.L_t1959:
	.byte	0x1
	.2byte	0x7
.L_t1959_e:
	.2byte	0x23
	.2byte	.L_l1959_e-.L_l1959
.L_l1959:
	.byte	0x4
	.4byte	0x1224
	.byte	0x7
.L_l1959_e:
.L_D1959_e:
.L_D1960:
	.4byte	.L_D1960_e-.L_D1960
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1961
	.2byte	0x38
	.string	"u_sigflag"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1960_e-.L_l1960
.L_l1960:
	.byte	0x4
	.4byte	0x1228
	.byte	0x7
.L_l1960_e:
.L_D1960_e:
.L_D1961:
	.4byte	.L_D1961_e-.L_D1961
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1962
	.2byte	0x38
	.string	"u_oldcontext"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x83
	.2byte	.L_t1961_e-.L_t1961
.L_t1961:
	.byte	0x1
	.4byte	.L_T400
.L_t1961_e:
	.2byte	0x23
	.2byte	.L_l1961_e-.L_l1961
.L_l1961:
	.byte	0x4
	.4byte	0x122c
	.byte	0x7
.L_l1961_e:
.L_D1961_e:
.L_D1962:
	.4byte	.L_D1962_e-.L_D1962
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1963
	.2byte	0x38
	.string	"u_sigaltstack"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T155
	.2byte	0x23
	.2byte	.L_l1962_e-.L_l1962
.L_l1962:
	.byte	0x4
	.4byte	0x1230
	.byte	0x7
.L_l1962_e:
.L_D1962_e:
.L_D1963:
	.4byte	.L_D1963_e-.L_D1963
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1964
	.2byte	0x38
	.string	"u_signodefer"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1963_e-.L_l1963
.L_l1963:
	.byte	0x4
	.4byte	0x123c
	.byte	0x7
.L_l1963_e:
.L_D1963_e:
.L_D1964:
	.4byte	.L_D1964_e-.L_D1964
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1965
	.2byte	0x38
	.string	"u_sigonstack"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1964_e-.L_l1964
.L_l1964:
	.byte	0x4
	.4byte	0x1240
	.byte	0x7
.L_l1964_e:
.L_D1964_e:
.L_D1965:
	.4byte	.L_D1965_e-.L_D1965
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1966
	.2byte	0x38
	.string	"u_sigresethand"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1965_e-.L_l1965
.L_l1965:
	.byte	0x4
	.4byte	0x1244
	.byte	0x7
.L_l1965_e:
.L_D1965_e:
.L_D1966:
	.4byte	.L_D1966_e-.L_D1966
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1967
	.2byte	0x38
	.string	"u_sigrestart"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1966_e-.L_l1966
.L_l1966:
	.byte	0x4
	.4byte	0x1248
	.byte	0x7
.L_l1966_e:
.L_D1966_e:
.L_D1967:
	.4byte	.L_D1967_e-.L_D1967
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1968
	.2byte	0x38
	.string	"u_sigoldmask"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1967_e-.L_l1967
.L_l1967:
	.byte	0x4
	.4byte	0x124c
	.byte	0x7
.L_l1967_e:
.L_D1967_e:
.L_D1968:
	.4byte	.L_D1968_e-.L_D1968
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1969
	.2byte	0x38
	.string	"u_sigmask"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T403
	.2byte	0x23
	.2byte	.L_l1968_e-.L_l1968
.L_l1968:
	.byte	0x4
	.4byte	0x1250
	.byte	0x7
.L_l1968_e:
.L_D1968_e:
.L_D1969:
	.4byte	.L_D1969_e-.L_D1969
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1970
	.2byte	0x38
	.string	"u_signal"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T407
	.2byte	0x23
	.2byte	.L_l1969_e-.L_l1969
.L_l1969:
	.byte	0x4
	.4byte	0x12d0
	.byte	0x7
.L_l1969_e:
.L_D1969_e:
.L_D1970:
	.4byte	.L_D1970_e-.L_D1970
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1971
	.2byte	0x38
	.string	"u_sigreturn"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x83
	.2byte	.L_t1970_e-.L_t1970
.L_t1970:
	.byte	0x1
	.4byte	.L_T41
.L_t1970_e:
	.2byte	0x23
	.2byte	.L_l1970_e-.L_l1970
.L_l1970:
	.byte	0x4
	.4byte	0x1350
	.byte	0x7
.L_l1970_e:
.L_D1970_e:
.L_D1971:
	.4byte	.L_D1971_e-.L_D1971
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1972
	.2byte	0x38
	.string	"u_prof"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T409
	.2byte	0x23
	.2byte	.L_l1971_e-.L_l1971
.L_l1971:
	.byte	0x4
	.4byte	0x1354
	.byte	0x7
.L_l1971_e:
.L_D1971_e:
.L_D1972:
	.4byte	.L_D1972_e-.L_D1972
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1973
	.2byte	0x38
	.string	"u_renv"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1972_e-.L_l1972
.L_l1972:
	.byte	0x4
	.4byte	0x1364
	.byte	0x7
.L_l1972_e:
.L_D1972_e:
.L_D1973:
	.4byte	.L_D1973_e-.L_D1973
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1974
	.2byte	0x38
	.string	"u_exdata"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T411
	.2byte	0x23
	.2byte	.L_l1973_e-.L_l1973
.L_l1973:
	.byte	0x4
	.4byte	0x1368
	.byte	0x7
.L_l1973_e:
.L_D1973_e:
.L_D1974:
	.4byte	.L_D1974_e-.L_D1974
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1975
	.2byte	0x38
	.string	"u_comm"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T208
	.2byte	0x23
	.2byte	.L_l1974_e-.L_l1974
.L_l1974:
	.byte	0x4
	.4byte	0x13a0
	.byte	0x7
.L_l1974_e:
.L_D1974_e:
.L_D1975:
	.4byte	.L_D1975_e-.L_D1975
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1976
	.2byte	0x38
	.string	"u_start"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1975_e-.L_l1975
.L_l1975:
	.byte	0x4
	.4byte	0x13b0
	.byte	0x7
.L_l1975_e:
.L_D1975_e:
.L_D1976:
	.4byte	.L_D1976_e-.L_D1976
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1977
	.2byte	0x38
	.string	"u_ticks"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1976_e-.L_l1976
.L_l1976:
	.byte	0x4
	.4byte	0x13b4
	.byte	0x7
.L_l1976_e:
.L_D1976_e:
.L_D1977:
	.4byte	.L_D1977_e-.L_D1977
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1978
	.2byte	0x38
	.string	"u_mem"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l1977_e-.L_l1977
.L_l1977:
	.byte	0x4
	.4byte	0x13b8
	.byte	0x7
.L_l1977_e:
.L_D1977_e:
.L_D1978:
	.4byte	.L_D1978_e-.L_D1978
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1979
	.2byte	0x38
	.string	"u_ior"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l1978_e-.L_l1978
.L_l1978:
	.byte	0x4
	.4byte	0x13bc
	.byte	0x7
.L_l1978_e:
.L_D1978_e:
.L_D1979:
	.4byte	.L_D1979_e-.L_D1979
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1980
	.2byte	0x38
	.string	"u_iow"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l1979_e-.L_l1979
.L_l1979:
	.byte	0x4
	.4byte	0x13c0
	.byte	0x7
.L_l1979_e:
.L_D1979_e:
.L_D1980:
	.4byte	.L_D1980_e-.L_D1980
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1981
	.2byte	0x38
	.string	"u_iosw"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l1980_e-.L_l1980
.L_l1980:
	.byte	0x4
	.4byte	0x13c4
	.byte	0x7
.L_l1980_e:
.L_D1980_e:
.L_D1981:
	.4byte	.L_D1981_e-.L_D1981
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1982
	.2byte	0x38
	.string	"u_ioch"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l1981_e-.L_l1981
.L_l1981:
	.byte	0x4
	.4byte	0x13c8
	.byte	0x7
.L_l1981_e:
.L_D1981_e:
.L_D1982:
	.4byte	.L_D1982_e-.L_D1982
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1983
	.2byte	0x38
	.string	"u_acflag"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l1982_e-.L_l1982
.L_l1982:
	.byte	0x4
	.4byte	0x13cc
	.byte	0x7
.L_l1982_e:
.L_D1982_e:
.L_D1983:
	.4byte	.L_D1983_e-.L_D1983
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1984
	.2byte	0x38
	.string	"u_cmask"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1983_e-.L_l1983
.L_l1983:
	.byte	0x4
	.4byte	0x13d0
	.byte	0x7
.L_l1983_e:
.L_D1983_e:
.L_D1984:
	.4byte	.L_D1984_e-.L_D1984
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1985
	.2byte	0x38
	.string	"u_oldsig"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1984_e-.L_l1984
.L_l1984:
	.byte	0x4
	.4byte	0x13d4
	.byte	0x7
.L_l1984_e:
.L_D1984_e:
.L_D1985:
	.4byte	.L_D1985_e-.L_D1985
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1986
	.2byte	0x38
	.string	"u_fault_catch"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T349
	.2byte	0x23
	.2byte	.L_l1985_e-.L_l1985
.L_l1985:
	.byte	0x4
	.4byte	0x13d8
	.byte	0x7
.L_l1985_e:
.L_D1985_e:
.L_D1986:
	.4byte	.L_D1986_e-.L_D1986
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1987
	.2byte	0x38
	.string	"u_shmbase"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x63
	.2byte	.L_t1986_e-.L_t1986
.L_t1986:
	.byte	0x1
	.2byte	0x1
.L_t1986_e:
	.2byte	0x23
	.2byte	.L_l1986_e-.L_l1986
.L_l1986:
	.byte	0x4
	.4byte	0x13fc
	.byte	0x7
.L_l1986_e:
.L_D1986_e:
.L_D1987:
	.4byte	.L_D1987_e-.L_D1987
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1988
	.2byte	0x38
	.string	"u_shmend"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x63
	.2byte	.L_t1987_e-.L_t1987
.L_t1987:
	.byte	0x1
	.2byte	0x1
.L_t1987_e:
	.2byte	0x23
	.2byte	.L_l1987_e-.L_l1987
.L_l1987:
	.byte	0x4
	.4byte	0x1400
	.byte	0x7
.L_l1987_e:
.L_D1987_e:
.L_D1988:
	.4byte	.L_D1988_e-.L_D1988
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1989
	.2byte	0x38
	.string	"u_covchan"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1988_e-.L_l1988
.L_l1988:
	.byte	0x4
	.4byte	0x1404
	.byte	0x7
.L_l1988_e:
.L_D1988_e:
.L_D1989:
	.4byte	.L_D1989_e-.L_D1989
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1990
	.2byte	0x38
	.string	"u_filler3"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T415
	.2byte	0x23
	.2byte	.L_l1989_e-.L_l1989
.L_l1989:
	.byte	0x4
	.4byte	0x1406
	.byte	0x7
.L_l1989_e:
.L_D1989_e:
.L_D1990:
	.4byte	.L_D1990_e-.L_D1990
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1991
	.2byte	0x38
	.string	"u_renv2"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l1990_e-.L_l1990
.L_l1990:
	.byte	0x4
	.4byte	0x1454
	.byte	0x7
.L_l1990_e:
.L_D1990_e:
.L_D1991:
	.4byte	.L_D1991_e-.L_D1991
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1992
	.2byte	0x38
	.string	"u_fpintgate"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T154
	.2byte	0x23
	.2byte	.L_l1991_e-.L_l1991
.L_l1991:
	.byte	0x4
	.4byte	0x1458
	.byte	0x7
.L_l1991_e:
.L_D1991_e:
.L_D1992:
	.4byte	.L_D1992_e-.L_D1992
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1993
	.2byte	0x38
	.string	"u_callgatep"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x63
	.2byte	.L_t1992_e-.L_t1992
.L_t1992:
	.byte	0x1
	.2byte	0x7
.L_t1992_e:
	.2byte	0x23
	.2byte	.L_l1992_e-.L_l1992
.L_l1992:
	.byte	0x4
	.4byte	0x1460
	.byte	0x7
.L_l1992_e:
.L_D1992_e:
.L_D1993:
	.4byte	.L_D1993_e-.L_D1993
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1994
	.2byte	0x38
	.string	"u_callgate"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T154
	.2byte	0x23
	.2byte	.L_l1993_e-.L_l1993
.L_l1993:
	.byte	0x4
	.4byte	0x1464
	.byte	0x7
.L_l1993_e:
.L_D1993_e:
.L_D1994:
	.4byte	.L_D1994_e-.L_D1994
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1995
	.2byte	0x38
	.string	"u_ldtmodified"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l1994_e-.L_l1994
.L_l1994:
	.byte	0x4
	.4byte	0x146c
	.byte	0x7
.L_l1994_e:
.L_D1994_e:
.L_D1995:
	.4byte	.L_D1995_e-.L_D1995
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1996
	.2byte	0x38
	.string	"u_ldtlimit"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l1995_e-.L_l1995
.L_l1995:
	.byte	0x4
	.4byte	0x1470
	.byte	0x7
.L_l1995_e:
.L_D1995_e:
.L_D1996:
	.4byte	.L_D1996_e-.L_D1996
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1997
	.2byte	0x38
	.string	"u_debugpend"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l1996_e-.L_l1996
.L_l1996:
	.byte	0x4
	.4byte	0x1472
	.byte	0x7
.L_l1996_e:
.L_D1996_e:
.L_D1997:
	.4byte	.L_D1997_e-.L_D1997
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1998
	.2byte	0x38
	.string	"u_debugon"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l1997_e-.L_l1997
.L_l1997:
	.byte	0x4
	.4byte	0x1473
	.byte	0x7
.L_l1997_e:
.L_D1997_e:
.L_D1998:
	.4byte	.L_D1998_e-.L_D1998
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D1999
	.2byte	0x38
	.string	"u_debugreg"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T394
	.2byte	0x23
	.2byte	.L_l1998_e-.L_l1998
.L_l1998:
	.byte	0x4
	.4byte	0x1474
	.byte	0x7
.L_l1998_e:
.L_D1998_e:
.L_D1999:
	.4byte	.L_D1999_e-.L_D1999
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2000
	.2byte	0x38
	.string	"u_lock"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l1999_e-.L_l1999
.L_l1999:
	.byte	0x4
	.4byte	0x1494
	.byte	0x7
.L_l1999_e:
.L_D1999_e:
.L_D2000:
	.4byte	.L_D2000_e-.L_D2000
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2001
	.2byte	0x38
	.string	"u_nofiles"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2000_e-.L_l2000
.L_l2000:
	.byte	0x4
	.4byte	0x1498
	.byte	0x7
.L_l2000_e:
.L_D2000_e:
.L_D2001:
	.4byte	.L_D2001_e-.L_D2001
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2002
	.2byte	0x38
	.string	"u_flist"
	.2byte	0x142
	.4byte	.L_T366
	.2byte	0x72
	.4byte	.L_T353
	.2byte	0x23
	.2byte	.L_l2001_e-.L_l2001
.L_l2001:
	.byte	0x4
	.4byte	0x149c
	.byte	0x7
.L_l2001_e:
.L_D2001_e:
.L_D2002:
	.4byte	0x4
.L_D1890:
	.4byte	.L_D1890_e-.L_D1890
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2003
	.set	.L_T311,.L_D1890
	.2byte	0xb6
	.4byte	0x10
.L_D1890_e:
.L_D2004:
	.4byte	.L_D2004_e-.L_D2004
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2005
	.2byte	0x38
	.string	"_pid"
	.2byte	0x142
	.4byte	.L_T311
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2004_e-.L_l2004
.L_l2004:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2004_e:
.L_D2004_e:
.L_D2005:
	.4byte	.L_D2005_e-.L_D2005
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2006
	.2byte	0x38
	.string	"_pdata"
	.2byte	0x142
	.4byte	.L_T311
	.2byte	0x72
	.4byte	.L_T310
	.2byte	0x23
	.2byte	.L_l2005_e-.L_l2005
.L_l2005:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2005_e:
.L_D2005_e:
.L_D2006:
	.4byte	0x4
.L_D2003:
	.4byte	.L_D2003_e-.L_D2003
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2007
	.set	.L_T312,.L_D2003
	.2byte	0xb6
	.4byte	0x4
.L_D2003_e:
.L_D2008:
	.4byte	.L_D2008_e-.L_D2008
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2009
	.2byte	0x38
	.string	"_addr"
	.2byte	0x142
	.4byte	.L_T312
	.2byte	0x63
	.2byte	.L_t2008_e-.L_t2008
.L_t2008:
	.byte	0x1
	.2byte	0x1
.L_t2008_e:
	.2byte	0x23
	.2byte	.L_l2008_e-.L_l2008
.L_l2008:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2008_e:
.L_D2008_e:
.L_D2009:
	.4byte	0x4
.L_D2007:
	.4byte	.L_D2007_e-.L_D2007
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2010
	.set	.L_T313,.L_D2007
	.2byte	0xb6
	.4byte	0x8
.L_D2007_e:
.L_D2011:
	.4byte	.L_D2011_e-.L_D2011
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2012
	.2byte	0x38
	.string	"_fd"
	.2byte	0x142
	.4byte	.L_T313
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2011_e-.L_l2011
.L_l2011:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2011_e:
.L_D2011_e:
.L_D2012:
	.4byte	.L_D2012_e-.L_D2012
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2013
	.2byte	0x38
	.string	"_band"
	.2byte	0x142
	.4byte	.L_T313
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2012_e-.L_l2012
.L_l2012:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2012_e:
.L_D2012_e:
.L_D2013:
	.4byte	0x4
.L_D2010:
	.4byte	.L_D2010_e-.L_D2010
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2014
	.set	.L_T370,.L_D2010
	.2byte	0xa3
	.2byte	.L_s2010_e-.L_s2010
.L_s2010:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x1
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s2010_e:
.L_D2010_e:
.L_D2014:
	.4byte	.L_D2014_e-.L_D2014
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D2015
	.set	.L_T379,.L_D2014
	.2byte	0xb6
	.4byte	0xf8
.L_D2014_e:
.L_D2016:
	.4byte	.L_D2016_e-.L_D2016
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2017
	.2byte	0x38
	.string	"u_fpstate"
	.2byte	0x142
	.4byte	.L_T379
	.2byte	0x72
	.4byte	.L_T371
	.2byte	0x23
	.2byte	.L_l2016_e-.L_l2016
.L_l2016:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2016_e:
.L_D2016_e:
.L_D2017:
	.4byte	.L_D2017_e-.L_D2017
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2018
	.2byte	0x38
	.string	"fp_emul"
	.2byte	0x142
	.4byte	.L_T379
	.2byte	0x72
	.4byte	.L_T374
	.2byte	0x23
	.2byte	.L_l2017_e-.L_l2017
.L_l2017:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2017_e:
.L_D2017_e:
.L_D2018:
	.4byte	0x4
.L_D2015:
	.4byte	.L_D2015_e-.L_D2015
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2019
	.set	.L_T381,.L_D2015
	.2byte	0xa3
	.2byte	.L_s2015_e-.L_s2015
.L_s2015:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x20
	.byte	0x8
	.2byte	0x55
	.2byte	0xa
.L_s2015_e:
.L_D2015_e:
.L_D2019:
	.4byte	.L_D2019_e-.L_D2019
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2020
	.set	.L_T340,.L_D2019
	.2byte	0x38
	.string	"tss386"
	.2byte	0xb6
	.4byte	0x68
.L_D2019_e:
.L_D2021:
	.4byte	.L_D2021_e-.L_D2021
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2022
	.2byte	0x38
	.string	"t_link"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2021_e-.L_l2021
.L_l2021:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2021_e:
.L_D2021_e:
.L_D2022:
	.4byte	.L_D2022_e-.L_D2022
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2023
	.2byte	0x38
	.string	"t_esp0"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2022_e-.L_l2022
.L_l2022:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2022_e:
.L_D2022_e:
.L_D2023:
	.4byte	.L_D2023_e-.L_D2023
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2024
	.2byte	0x38
	.string	"t_ss0"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2023_e-.L_l2023
.L_l2023:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2023_e:
.L_D2023_e:
.L_D2024:
	.4byte	.L_D2024_e-.L_D2024
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2025
	.2byte	0x38
	.string	"t_esp1"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2024_e-.L_l2024
.L_l2024:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2024_e:
.L_D2024_e:
.L_D2025:
	.4byte	.L_D2025_e-.L_D2025
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2026
	.2byte	0x38
	.string	"t_ss1"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2025_e-.L_l2025
.L_l2025:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l2025_e:
.L_D2025_e:
.L_D2026:
	.4byte	.L_D2026_e-.L_D2026
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2027
	.2byte	0x38
	.string	"t_esp2"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2026_e-.L_l2026
.L_l2026:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l2026_e:
.L_D2026_e:
.L_D2027:
	.4byte	.L_D2027_e-.L_D2027
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2028
	.2byte	0x38
	.string	"t_ss2"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2027_e-.L_l2027
.L_l2027:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l2027_e:
.L_D2027_e:
.L_D2028:
	.4byte	.L_D2028_e-.L_D2028
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2029
	.2byte	0x38
	.string	"t_cr3"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2028_e-.L_l2028
.L_l2028:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l2028_e:
.L_D2028_e:
.L_D2029:
	.4byte	.L_D2029_e-.L_D2029
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2030
	.2byte	0x38
	.string	"t_eip"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2029_e-.L_l2029
.L_l2029:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l2029_e:
.L_D2029_e:
.L_D2030:
	.4byte	.L_D2030_e-.L_D2030
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2031
	.2byte	0x38
	.string	"t_eflags"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2030_e-.L_l2030
.L_l2030:
	.byte	0x4
	.4byte	0x24
	.byte	0x7
.L_l2030_e:
.L_D2030_e:
.L_D2031:
	.4byte	.L_D2031_e-.L_D2031
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2032
	.2byte	0x38
	.string	"t_eax"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2031_e-.L_l2031
.L_l2031:
	.byte	0x4
	.4byte	0x28
	.byte	0x7
.L_l2031_e:
.L_D2031_e:
.L_D2032:
	.4byte	.L_D2032_e-.L_D2032
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2033
	.2byte	0x38
	.string	"t_ecx"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2032_e-.L_l2032
.L_l2032:
	.byte	0x4
	.4byte	0x2c
	.byte	0x7
.L_l2032_e:
.L_D2032_e:
.L_D2033:
	.4byte	.L_D2033_e-.L_D2033
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2034
	.2byte	0x38
	.string	"t_edx"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2033_e-.L_l2033
.L_l2033:
	.byte	0x4
	.4byte	0x30
	.byte	0x7
.L_l2033_e:
.L_D2033_e:
.L_D2034:
	.4byte	.L_D2034_e-.L_D2034
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2035
	.2byte	0x38
	.string	"t_ebx"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2034_e-.L_l2034
.L_l2034:
	.byte	0x4
	.4byte	0x34
	.byte	0x7
.L_l2034_e:
.L_D2034_e:
.L_D2035:
	.4byte	.L_D2035_e-.L_D2035
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2036
	.2byte	0x38
	.string	"t_esp"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2035_e-.L_l2035
.L_l2035:
	.byte	0x4
	.4byte	0x38
	.byte	0x7
.L_l2035_e:
.L_D2035_e:
.L_D2036:
	.4byte	.L_D2036_e-.L_D2036
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2037
	.2byte	0x38
	.string	"t_ebp"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2036_e-.L_l2036
.L_l2036:
	.byte	0x4
	.4byte	0x3c
	.byte	0x7
.L_l2036_e:
.L_D2036_e:
.L_D2037:
	.4byte	.L_D2037_e-.L_D2037
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2038
	.2byte	0x38
	.string	"t_esi"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2037_e-.L_l2037
.L_l2037:
	.byte	0x4
	.4byte	0x40
	.byte	0x7
.L_l2037_e:
.L_D2037_e:
.L_D2038:
	.4byte	.L_D2038_e-.L_D2038
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2039
	.2byte	0x38
	.string	"t_edi"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2038_e-.L_l2038
.L_l2038:
	.byte	0x4
	.4byte	0x44
	.byte	0x7
.L_l2038_e:
.L_D2038_e:
.L_D2039:
	.4byte	.L_D2039_e-.L_D2039
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2040
	.2byte	0x38
	.string	"t_es"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2039_e-.L_l2039
.L_l2039:
	.byte	0x4
	.4byte	0x48
	.byte	0x7
.L_l2039_e:
.L_D2039_e:
.L_D2040:
	.4byte	.L_D2040_e-.L_D2040
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2041
	.2byte	0x38
	.string	"t_cs"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2040_e-.L_l2040
.L_l2040:
	.byte	0x4
	.4byte	0x4c
	.byte	0x7
.L_l2040_e:
.L_D2040_e:
.L_D2041:
	.4byte	.L_D2041_e-.L_D2041
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2042
	.2byte	0x38
	.string	"t_ss"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2041_e-.L_l2041
.L_l2041:
	.byte	0x4
	.4byte	0x50
	.byte	0x7
.L_l2041_e:
.L_D2041_e:
.L_D2042:
	.4byte	.L_D2042_e-.L_D2042
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2043
	.2byte	0x38
	.string	"t_ds"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2042_e-.L_l2042
.L_l2042:
	.byte	0x4
	.4byte	0x54
	.byte	0x7
.L_l2042_e:
.L_D2042_e:
.L_D2043:
	.4byte	.L_D2043_e-.L_D2043
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2044
	.2byte	0x38
	.string	"t_fs"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2043_e-.L_l2043
.L_l2043:
	.byte	0x4
	.4byte	0x58
	.byte	0x7
.L_l2043_e:
.L_D2043_e:
.L_D2044:
	.4byte	.L_D2044_e-.L_D2044
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2045
	.2byte	0x38
	.string	"t_gs"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2044_e-.L_l2044
.L_l2044:
	.byte	0x4
	.4byte	0x5c
	.byte	0x7
.L_l2044_e:
.L_D2044_e:
.L_D2045:
	.4byte	.L_D2045_e-.L_D2045
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2046
	.2byte	0x38
	.string	"t_ldt"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2045_e-.L_l2045
.L_l2045:
	.byte	0x4
	.4byte	0x60
	.byte	0x7
.L_l2045_e:
.L_D2045_e:
.L_D2046:
	.4byte	.L_D2046_e-.L_D2046
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2047
	.2byte	0x38
	.string	"t_bitmapbase"
	.2byte	0x142
	.4byte	.L_T340
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2046_e-.L_l2046
.L_l2046:
	.byte	0x4
	.4byte	0x64
	.byte	0x7
.L_l2046_e:
.L_D2046_e:
.L_D2047:
	.4byte	0x4
.L_D2020:
	.4byte	.L_D2020_e-.L_D2020
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2048
	.set	.L_T384,.L_D2020
	.2byte	0xa3
	.2byte	.L_s2020_e-.L_s2020
.L_s2020:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x7
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s2020_e:
.L_D2020_e:
.L_D2048:
	.4byte	.L_D2048_e-.L_D2048
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2049
	.set	.L_T341,.L_D2048
	.2byte	0x38
	.string	"seg_desc"
	.2byte	0xb6
	.4byte	0x8
.L_D2048_e:
.L_D2050:
	.4byte	.L_D2050_e-.L_D2050
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2051
	.2byte	0x38
	.string	"s_base"
	.2byte	0x142
	.4byte	.L_T341
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2050_e-.L_l2050
.L_l2050:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2050_e:
.L_D2050_e:
.L_D2051:
	.4byte	.L_D2051_e-.L_D2051
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2052
	.2byte	0x38
	.string	"s_limacc"
	.2byte	0x142
	.4byte	.L_T341
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2051_e-.L_l2051
.L_l2051:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2051_e:
.L_D2051_e:
.L_D2052:
	.4byte	0x4
.L_D2049:
	.4byte	.L_D2049_e-.L_D2049
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2053
	.set	.L_T386,.L_D2049
	.2byte	0xa3
	.2byte	.L_s2049_e-.L_s2049
.L_s2049:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x3
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s2049_e:
.L_D2049_e:
.L_D2053:
	.4byte	.L_D2053_e-.L_D2053
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2054
	.set	.L_T387,.L_D2053
	.2byte	0x38
	.string	"sndd"
.L_D2053_e:
.L_D2054:
	.4byte	.L_D2054_e-.L_D2054
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2055
	.set	.L_T389,.L_D2054
	.2byte	0x38
	.string	"execsw"
.L_D2054_e:
.L_D2055:
	.4byte	.L_D2055_e-.L_D2055
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2056
	.set	.L_T392,.L_D2055
	.2byte	0xa3
	.2byte	.L_s2055_e-.L_s2055
.L_s2055:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x4f
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s2055_e:
.L_D2055_e:
.L_D2056:
	.4byte	.L_D2056_e-.L_D2056
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2057
	.set	.L_T394,.L_D2056
	.2byte	0xa3
	.2byte	.L_s2056_e-.L_s2056
.L_s2056:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x7
	.byte	0x8
	.2byte	0x55
	.2byte	0x7
.L_s2056_e:
.L_D2056_e:
.L_D2057:
	.4byte	.L_D2057_e-.L_D2057
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D2058
	.set	.L_T396,.L_D2057
	.2byte	0xb6
	.4byte	0x8
.L_D2057_e:
.L_D2059:
	.4byte	.L_D2059_e-.L_D2059
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2060
	.2byte	0x38
	.string	"r_reg"
	.2byte	0x142
	.4byte	.L_T396
	.2byte	0x72
	.4byte	.L_T395
	.2byte	0x23
	.2byte	.L_l2059_e-.L_l2059
.L_l2059:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2059_e:
.L_D2059_e:
.L_D2060:
	.4byte	.L_D2060_e-.L_D2060
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2061
	.2byte	0x38
	.string	"r_off"
	.2byte	0x142
	.4byte	.L_T396
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2060_e-.L_l2060
.L_l2060:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2060_e:
.L_D2060_e:
.L_D2061:
	.4byte	.L_D2061_e-.L_D2061
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2062
	.2byte	0x38
	.string	"r_time"
	.2byte	0x142
	.4byte	.L_T396
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2061_e-.L_l2061
.L_l2061:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2061_e:
.L_D2061_e:
.L_D2062:
	.4byte	0x4
.L_D2058:
	.4byte	.L_D2058_e-.L_D2058
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2063
	.set	.L_T364,.L_D2058
	.2byte	0xb6
	.4byte	0x40
.L_D2058_e:
.L_D2064:
	.4byte	.L_D2064_e-.L_D2064
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2065
	.2byte	0x38
	.string	"word"
	.2byte	0x142
	.4byte	.L_T364
	.2byte	0x72
	.4byte	.L_T363
	.2byte	0x23
	.2byte	.L_l2064_e-.L_l2064
.L_l2064:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2064_e:
.L_D2064_e:
.L_D2065:
	.4byte	0x4
.L_D2063:
	.4byte	.L_D2063_e-.L_D2063
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2066
	.set	.L_T334,.L_D2063
	.2byte	0x38
	.string	"rlimit"
	.2byte	0xb6
	.4byte	0x8
.L_D2063_e:
.L_D2067:
	.4byte	.L_D2067_e-.L_D2067
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2068
	.2byte	0x38
	.string	"rlim_cur"
	.2byte	0x142
	.4byte	.L_T334
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2067_e-.L_l2067
.L_l2067:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2067_e:
.L_D2067_e:
.L_D2068:
	.4byte	.L_D2068_e-.L_D2068
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2069
	.2byte	0x38
	.string	"rlim_max"
	.2byte	0x142
	.4byte	.L_T334
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2068_e-.L_l2068
.L_l2068:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2068_e:
.L_D2068_e:
.L_D2069:
	.4byte	0x4
.L_D2066:
	.4byte	.L_D2066_e-.L_D2066
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2070
	.set	.L_T398,.L_D2066
	.2byte	0xa3
	.2byte	.L_s2066_e-.L_s2066
.L_s2066:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x6
	.byte	0x8
	.2byte	0x72
	.4byte	.L_T334
.L_s2066_e:
.L_D2066_e:
.L_D2070:
	.4byte	.L_D2070_e-.L_D2070
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2071
	.set	.L_T400,.L_D2070
	.2byte	0x38
	.string	"ucontext"
.L_D2070_e:
.L_D2071:
	.4byte	.L_D2071_e-.L_D2071
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2072
	.set	.L_T403,.L_D2071
	.2byte	0xa3
	.2byte	.L_s2071_e-.L_s2071
.L_s2071:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x1f
	.byte	0x8
	.2byte	0x55
	.2byte	0x9
.L_s2071_e:
.L_D2071_e:
.L_D2072:
	.4byte	.L_D2072_e-.L_D2072
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2073
	.set	.L_T407,.L_D2072
	.2byte	0xa3
	.2byte	.L_s2072_e-.L_s2072
.L_s2072:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x1f
	.byte	0x8
	.2byte	0x83
	.2byte	.L_t2072_e-.L_t2072
.L_t2072:
	.byte	0x1
	.4byte	.L_T41
.L_t2072_e:
.L_s2072_e:
.L_D2072_e:
.L_D2073:
	.4byte	.L_D2073_e-.L_D2073
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2074
	.set	.L_T409,.L_D2073
	.2byte	0x38
	.string	"prof"
	.2byte	0xb6
	.4byte	0x10
.L_D2073_e:
.L_D2075:
	.4byte	.L_D2075_e-.L_D2075
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2076
	.2byte	0x38
	.string	"pr_base"
	.2byte	0x142
	.4byte	.L_T409
	.2byte	0x63
	.2byte	.L_t2075_e-.L_t2075
.L_t2075:
	.byte	0x1
	.2byte	0x6
.L_t2075_e:
	.2byte	0x23
	.2byte	.L_l2075_e-.L_l2075
.L_l2075:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2075_e:
.L_D2075_e:
.L_D2076:
	.4byte	.L_D2076_e-.L_D2076
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2077
	.2byte	0x38
	.string	"pr_size"
	.2byte	0x142
	.4byte	.L_T409
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2076_e-.L_l2076
.L_l2076:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2076_e:
.L_D2076_e:
.L_D2077:
	.4byte	.L_D2077_e-.L_D2077
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2078
	.2byte	0x38
	.string	"pr_off"
	.2byte	0x142
	.4byte	.L_T409
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2077_e-.L_l2077
.L_l2077:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2077_e:
.L_D2077_e:
.L_D2078:
	.4byte	.L_D2078_e-.L_D2078
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2079
	.2byte	0x38
	.string	"pr_scale"
	.2byte	0x142
	.4byte	.L_T409
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2078_e-.L_l2078
.L_l2078:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2078_e:
.L_D2078_e:
.L_D2079:
	.4byte	0x4
.L_D2074:
	.4byte	.L_D2074_e-.L_D2074
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2080
	.set	.L_T411,.L_D2074
	.2byte	0x38
	.string	"exdata"
	.2byte	0xb6
	.4byte	0x38
.L_D2074_e:
.L_D2081:
	.4byte	.L_D2081_e-.L_D2081
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2082
	.2byte	0x38
	.string	"vp"
	.2byte	0x142
	.4byte	.L_T411
	.2byte	0x83
	.2byte	.L_t2081_e-.L_t2081
.L_t2081:
	.byte	0x1
	.4byte	.L_T170
.L_t2081_e:
	.2byte	0x23
	.2byte	.L_l2081_e-.L_l2081
.L_l2081:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2081_e:
.L_D2081_e:
.L_D2082:
	.4byte	.L_D2082_e-.L_D2082
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2083
	.2byte	0x38
	.string	"ux_tsize"
	.2byte	0x142
	.4byte	.L_T411
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2082_e-.L_l2082
.L_l2082:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2082_e:
.L_D2082_e:
.L_D2083:
	.4byte	.L_D2083_e-.L_D2083
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2084
	.2byte	0x38
	.string	"ux_dsize"
	.2byte	0x142
	.4byte	.L_T411
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2083_e-.L_l2083
.L_l2083:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2083_e:
.L_D2083_e:
.L_D2084:
	.4byte	.L_D2084_e-.L_D2084
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2085
	.2byte	0x38
	.string	"ux_bsize"
	.2byte	0x142
	.4byte	.L_T411
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2084_e-.L_l2084
.L_l2084:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2084_e:
.L_D2084_e:
.L_D2085:
	.4byte	.L_D2085_e-.L_D2085
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2086
	.2byte	0x38
	.string	"ux_lsize"
	.2byte	0x142
	.4byte	.L_T411
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2085_e-.L_l2085
.L_l2085:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l2085_e:
.L_D2085_e:
.L_D2086:
	.4byte	.L_D2086_e-.L_D2086
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2087
	.2byte	0x38
	.string	"ux_nshlibs"
	.2byte	0x142
	.4byte	.L_T411
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2086_e-.L_l2086
.L_l2086:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l2086_e:
.L_D2086_e:
.L_D2087:
	.4byte	.L_D2087_e-.L_D2087
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2088
	.2byte	0x38
	.string	"ux_mag"
	.2byte	0x142
	.4byte	.L_T411
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2087_e-.L_l2087
.L_l2087:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l2087_e:
.L_D2087_e:
.L_D2088:
	.4byte	.L_D2088_e-.L_D2088
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2089
	.2byte	0x38
	.string	"ux_toffset"
	.2byte	0x142
	.4byte	.L_T411
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2088_e-.L_l2088
.L_l2088:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l2088_e:
.L_D2088_e:
.L_D2089:
	.4byte	.L_D2089_e-.L_D2089
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2090
	.2byte	0x38
	.string	"ux_doffset"
	.2byte	0x142
	.4byte	.L_T411
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2089_e-.L_l2089
.L_l2089:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l2089_e:
.L_D2089_e:
.L_D2090:
	.4byte	.L_D2090_e-.L_D2090
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2091
	.2byte	0x38
	.string	"ux_loffset"
	.2byte	0x142
	.4byte	.L_T411
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2090_e-.L_l2090
.L_l2090:
	.byte	0x4
	.4byte	0x24
	.byte	0x7
.L_l2090_e:
.L_D2090_e:
.L_D2091:
	.4byte	.L_D2091_e-.L_D2091
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2092
	.2byte	0x38
	.string	"ux_txtorg"
	.2byte	0x142
	.4byte	.L_T411
	.2byte	0x63
	.2byte	.L_t2091_e-.L_t2091
.L_t2091:
	.byte	0x1
	.2byte	0x1
.L_t2091_e:
	.2byte	0x23
	.2byte	.L_l2091_e-.L_l2091
.L_l2091:
	.byte	0x4
	.4byte	0x28
	.byte	0x7
.L_l2091_e:
.L_D2091_e:
.L_D2092:
	.4byte	.L_D2092_e-.L_D2092
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2093
	.2byte	0x38
	.string	"ux_datorg"
	.2byte	0x142
	.4byte	.L_T411
	.2byte	0x63
	.2byte	.L_t2092_e-.L_t2092
.L_t2092:
	.byte	0x1
	.2byte	0x1
.L_t2092_e:
	.2byte	0x23
	.2byte	.L_l2092_e-.L_l2092
.L_l2092:
	.byte	0x4
	.4byte	0x2c
	.byte	0x7
.L_l2092_e:
.L_D2092_e:
.L_D2093:
	.4byte	.L_D2093_e-.L_D2093
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2094
	.2byte	0x38
	.string	"ux_entloc"
	.2byte	0x142
	.4byte	.L_T411
	.2byte	0x63
	.2byte	.L_t2093_e-.L_t2093
.L_t2093:
	.byte	0x1
	.2byte	0x1
.L_t2093_e:
	.2byte	0x23
	.2byte	.L_l2093_e-.L_l2093
.L_l2093:
	.byte	0x4
	.4byte	0x30
	.byte	0x7
.L_l2093_e:
.L_D2093_e:
.L_D2094:
	.4byte	.L_D2094_e-.L_D2094
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2095
	.2byte	0x38
	.string	"ux_renv"
	.2byte	0x142
	.4byte	.L_T411
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2094_e-.L_l2094
.L_l2094:
	.byte	0x4
	.4byte	0x34
	.byte	0x7
.L_l2094_e:
.L_D2094_e:
.L_D2095:
	.4byte	0x4
.L_D2080:
	.4byte	.L_D2080_e-.L_D2080
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2096
	.set	.L_T208,.L_D2080
	.2byte	0xa3
	.2byte	.L_s2080_e-.L_s2080
.L_s2080:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0xd
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s2080_e:
.L_D2080_e:
.L_D2096:
	.4byte	.L_D2096_e-.L_D2096
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2097
	.set	.L_T349,.L_D2096
	.2byte	0x38
	.string	"fault_catch"
	.2byte	0xb6
	.4byte	0x24
.L_D2096_e:
.L_D2098:
	.4byte	.L_D2098_e-.L_D2098
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2099
	.2byte	0x38
	.string	"fc_flags"
	.2byte	0x142
	.4byte	.L_T349
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2098_e-.L_l2098
.L_l2098:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2098_e:
.L_D2098_e:
.L_D2099:
	.4byte	.L_D2099_e-.L_D2099
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2100
	.2byte	0x38
	.string	"fc_errno"
	.2byte	0x142
	.4byte	.L_T349
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2099_e-.L_l2099
.L_l2099:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2099_e:
.L_D2099_e:
.L_D2100:
	.4byte	.L_D2100_e-.L_D2100
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2101
	.2byte	0x38
	.string	"fc_func"
	.2byte	0x142
	.4byte	.L_T349
	.2byte	0x83
	.2byte	.L_t2100_e-.L_t2100
.L_t2100:
	.byte	0x1
	.4byte	.L_T41
.L_t2100_e:
	.2byte	0x23
	.2byte	.L_l2100_e-.L_l2100
.L_l2100:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2100_e:
.L_D2100_e:
.L_D2101:
	.4byte	.L_D2101_e-.L_D2101
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2102
	.2byte	0x38
	.string	"fc_jmp"
	.2byte	0x142
	.4byte	.L_T349
	.2byte	0x72
	.4byte	.L_T83
	.2byte	0x23
	.2byte	.L_l2101_e-.L_l2101
.L_l2101:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2101_e:
.L_D2101_e:
.L_D2102:
	.4byte	0x4
.L_D2097:
	.4byte	.L_D2097_e-.L_D2097
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2103
	.set	.L_T415,.L_D2097
	.2byte	0xa3
	.2byte	.L_s2097_e-.L_s2097
.L_s2097:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x4d
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s2097_e:
.L_D2097_e:
.L_D2103:
	.4byte	.L_D2103_e-.L_D2103
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2104
	.set	.L_T353,.L_D2103
	.2byte	0x38
	.string	"ufchunk"
	.2byte	0xb6
	.4byte	0x7c
.L_D2103_e:
.L_D2105:
	.4byte	.L_D2105_e-.L_D2105
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2106
	.2byte	0x38
	.string	"uf_ofile"
	.2byte	0x142
	.4byte	.L_T353
	.2byte	0x72
	.4byte	.L_T358
	.2byte	0x23
	.2byte	.L_l2105_e-.L_l2105
.L_l2105:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2105_e:
.L_D2105_e:
.L_D2106:
	.4byte	.L_D2106_e-.L_D2106
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2107
	.2byte	0x38
	.string	"uf_pofile"
	.2byte	0x142
	.4byte	.L_T353
	.2byte	0x72
	.4byte	.L_T360
	.2byte	0x23
	.2byte	.L_l2106_e-.L_l2106
.L_l2106:
	.byte	0x4
	.4byte	0x60
	.byte	0x7
.L_l2106_e:
.L_D2106_e:
.L_D2107:
	.4byte	.L_D2107_e-.L_D2107
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2108
	.2byte	0x38
	.string	"uf_next"
	.2byte	0x142
	.4byte	.L_T353
	.2byte	0x83
	.2byte	.L_t2107_e-.L_t2107
.L_t2107:
	.byte	0x1
	.4byte	.L_T353
.L_t2107_e:
	.2byte	0x23
	.2byte	.L_l2107_e-.L_l2107
.L_l2107:
	.byte	0x4
	.4byte	0x78
	.byte	0x7
.L_l2107_e:
.L_D2107_e:
.L_D2108:
	.4byte	0x4
.L_D2104:
	.4byte	.L_D2104_e-.L_D2104
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D2109
	.set	.L_T310,.L_D2104
	.2byte	0xb6
	.4byte	0xc
.L_D2104_e:
.L_D2110:
	.4byte	.L_D2110_e-.L_D2110
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2111
	.2byte	0x38
	.string	"_kill"
	.2byte	0x142
	.4byte	.L_T310
	.2byte	0x72
	.4byte	.L_T308
	.2byte	0x23
	.2byte	.L_l2110_e-.L_l2110
.L_l2110:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2110_e:
.L_D2110_e:
.L_D2111:
	.4byte	.L_D2111_e-.L_D2111
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2112
	.2byte	0x38
	.string	"_cld"
	.2byte	0x142
	.4byte	.L_T310
	.2byte	0x72
	.4byte	.L_T309
	.2byte	0x23
	.2byte	.L_l2111_e-.L_l2111
.L_l2111:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2111_e:
.L_D2111_e:
.L_D2112:
	.4byte	0x4
.L_D2109:
	.4byte	.L_D2109_e-.L_D2109
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2113
	.set	.L_T371,.L_D2109
	.2byte	0x38
	.string	"fpstate"
	.2byte	0xb6
	.4byte	0x70
.L_D2109_e:
.L_D2114:
	.4byte	.L_D2114_e-.L_D2114
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2115
	.2byte	0x38
	.string	"state"
	.2byte	0x142
	.4byte	.L_T371
	.2byte	0x72
	.4byte	.L_T373
	.2byte	0x23
	.2byte	.L_l2114_e-.L_l2114
.L_l2114:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2114_e:
.L_D2114_e:
.L_D2115:
	.4byte	.L_D2115_e-.L_D2115
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2116
	.2byte	0x38
	.string	"status"
	.2byte	0x142
	.4byte	.L_T371
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2115_e-.L_l2115
.L_l2115:
	.byte	0x4
	.4byte	0x6c
	.byte	0x7
.L_l2115_e:
.L_D2115_e:
.L_D2116:
	.4byte	0x4
.L_D2113:
	.4byte	.L_D2113_e-.L_D2113
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2117
	.set	.L_T374,.L_D2113
	.2byte	0x38
	.string	"fp_emul"
	.2byte	0xb6
	.4byte	0xf8
.L_D2113_e:
.L_D2118:
	.4byte	.L_D2118_e-.L_D2118
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2119
	.2byte	0x38
	.string	"fp_emul"
	.2byte	0x142
	.4byte	.L_T374
	.2byte	0x72
	.4byte	.L_T376
	.2byte	0x23
	.2byte	.L_l2118_e-.L_l2118
.L_l2118:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2118_e:
.L_D2118_e:
.L_D2119:
	.4byte	.L_D2119_e-.L_D2119
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2120
	.2byte	0x38
	.string	"fp_epad"
	.2byte	0x142
	.4byte	.L_T374
	.2byte	0x72
	.4byte	.L_T370
	.2byte	0x23
	.2byte	.L_l2119_e-.L_l2119
.L_l2119:
	.byte	0x4
	.4byte	0xf6
	.byte	0x7
.L_l2119_e:
.L_D2119_e:
.L_D2120:
	.4byte	0x4
.L_D2117:
	.4byte	.L_D2117_e-.L_D2117
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2121
	.set	.L_T395,.L_D2117
	.2byte	0xb6
	.4byte	0x8
.L_D2117_e:
.L_D2122:
	.4byte	.L_D2122_e-.L_D2122
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2123
	.2byte	0x38
	.string	"r_v1"
	.2byte	0x142
	.4byte	.L_T395
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2122_e-.L_l2122
.L_l2122:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2122_e:
.L_D2122_e:
.L_D2123:
	.4byte	.L_D2123_e-.L_D2123
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2124
	.2byte	0x38
	.string	"r_v2"
	.2byte	0x142
	.4byte	.L_T395
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2123_e-.L_l2123
.L_l2123:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2123_e:
.L_D2123_e:
.L_D2124:
	.4byte	0x4
.L_D2121:
	.4byte	.L_D2121_e-.L_D2121
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2125
	.set	.L_T363,.L_D2121
	.2byte	0xa3
	.2byte	.L_s2121_e-.L_s2121
.L_s2121:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0xf
	.byte	0x8
	.2byte	0x55
	.2byte	0xa
.L_s2121_e:
.L_D2121_e:
.L_D2125:
	.4byte	.L_D2125_e-.L_D2125
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2126
	.set	.L_T354,.L_D2125
	.2byte	0x38
	.string	"file"
	.2byte	0xb6
	.4byte	0x24
.L_D2125_e:
.L_D2127:
	.4byte	.L_D2127_e-.L_D2127
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2128
	.2byte	0x38
	.string	"f_next"
	.2byte	0x142
	.4byte	.L_T354
	.2byte	0x83
	.2byte	.L_t2127_e-.L_t2127
.L_t2127:
	.byte	0x1
	.4byte	.L_T354
.L_t2127_e:
	.2byte	0x23
	.2byte	.L_l2127_e-.L_l2127
.L_l2127:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2127_e:
.L_D2127_e:
.L_D2128:
	.4byte	.L_D2128_e-.L_D2128
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2129
	.2byte	0x38
	.string	"f_prev"
	.2byte	0x142
	.4byte	.L_T354
	.2byte	0x83
	.2byte	.L_t2128_e-.L_t2128
.L_t2128:
	.byte	0x1
	.4byte	.L_T354
.L_t2128_e:
	.2byte	0x23
	.2byte	.L_l2128_e-.L_l2128
.L_l2128:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2128_e:
.L_D2128_e:
.L_D2129:
	.4byte	.L_D2129_e-.L_D2129
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2130
	.2byte	0x38
	.string	"f_flag"
	.2byte	0x142
	.4byte	.L_T354
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2129_e-.L_l2129
.L_l2129:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2129_e:
.L_D2129_e:
.L_D2130:
	.4byte	.L_D2130_e-.L_D2130
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2131
	.2byte	0x38
	.string	"f_count"
	.2byte	0x142
	.4byte	.L_T354
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2130_e-.L_l2130
.L_l2130:
	.byte	0x4
	.4byte	0xa
	.byte	0x7
.L_l2130_e:
.L_D2130_e:
.L_D2131:
	.4byte	.L_D2131_e-.L_D2131
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2132
	.2byte	0x38
	.string	"f_vnode"
	.2byte	0x142
	.4byte	.L_T354
	.2byte	0x83
	.2byte	.L_t2131_e-.L_t2131
.L_t2131:
	.byte	0x1
	.4byte	.L_T170
.L_t2131_e:
	.2byte	0x23
	.2byte	.L_l2131_e-.L_l2131
.L_l2131:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2131_e:
.L_D2131_e:
.L_D2132:
	.4byte	.L_D2132_e-.L_D2132
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2133
	.2byte	0x38
	.string	"f_offset"
	.2byte	0x142
	.4byte	.L_T354
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2132_e-.L_l2132
.L_l2132:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l2132_e:
.L_D2132_e:
.L_D2133:
	.4byte	.L_D2133_e-.L_D2133
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2134
	.2byte	0x38
	.string	"f_cred"
	.2byte	0x142
	.4byte	.L_T354
	.2byte	0x83
	.2byte	.L_t2133_e-.L_t2133
.L_t2133:
	.byte	0x1
	.4byte	.L_T223
.L_t2133_e:
	.2byte	0x23
	.2byte	.L_l2133_e-.L_l2133
.L_l2133:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l2133_e:
.L_D2133_e:
.L_D2134:
	.4byte	.L_D2134_e-.L_D2134
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2135
	.2byte	0x38
	.string	"f_aiof"
	.2byte	0x142
	.4byte	.L_T354
	.2byte	0x63
	.2byte	.L_t2134_e-.L_t2134
.L_t2134:
	.byte	0x1
	.byte	0x1
	.2byte	0x1
.L_t2134_e:
	.2byte	0x23
	.2byte	.L_l2134_e-.L_l2134
.L_l2134:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l2134_e:
.L_D2134_e:
.L_D2135:
	.4byte	.L_D2135_e-.L_D2135
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2136
	.2byte	0x38
	.string	"f_aiob"
	.2byte	0x142
	.4byte	.L_T354
	.2byte	0x63
	.2byte	.L_t2135_e-.L_t2135
.L_t2135:
	.byte	0x1
	.byte	0x1
	.2byte	0x1
.L_t2135_e:
	.2byte	0x23
	.2byte	.L_l2135_e-.L_l2135
.L_l2135:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l2135_e:
.L_D2135_e:
.L_D2136:
	.4byte	.L_D2136_e-.L_D2136
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2137
	.2byte	0x38
	.string	"f_slnk"
	.2byte	0x142
	.4byte	.L_T354
	.2byte	0x83
	.2byte	.L_t2136_e-.L_t2136
.L_t2136:
	.byte	0x1
	.4byte	.L_T354
.L_t2136_e:
	.2byte	0x23
	.2byte	.L_l2136_e-.L_l2136
.L_l2136:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l2136_e:
.L_D2136_e:
.L_D2137:
	.4byte	0x4
.L_D2126:
	.4byte	.L_D2126_e-.L_D2126
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2138
	.set	.L_T358,.L_D2126
	.2byte	0xa3
	.2byte	.L_s2126_e-.L_s2126
.L_s2126:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x17
	.byte	0x8
	.2byte	0x83
	.2byte	.L_t2126_e-.L_t2126
.L_t2126:
	.byte	0x1
	.4byte	.L_T354
.L_t2126_e:
.L_s2126_e:
.L_D2126_e:
.L_D2138:
	.4byte	.L_D2138_e-.L_D2138
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2139
	.set	.L_T360,.L_D2138
	.2byte	0xa3
	.2byte	.L_s2138_e-.L_s2138
.L_s2138:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x17
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s2138_e:
.L_D2138_e:
.L_D2139:
	.4byte	.L_D2139_e-.L_D2139
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2140
	.set	.L_T308,.L_D2139
	.2byte	0xb6
	.4byte	0x4
.L_D2139_e:
.L_D2141:
	.4byte	.L_D2141_e-.L_D2141
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2142
	.2byte	0x38
	.string	"_uid"
	.2byte	0x142
	.4byte	.L_T308
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2141_e-.L_l2141
.L_l2141:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2141_e:
.L_D2141_e:
.L_D2142:
	.4byte	0x4
.L_D2140:
	.4byte	.L_D2140_e-.L_D2140
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2143
	.set	.L_T309,.L_D2140
	.2byte	0xb6
	.4byte	0xc
.L_D2140_e:
.L_D2144:
	.4byte	.L_D2144_e-.L_D2144
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2145
	.2byte	0x38
	.string	"_utime"
	.2byte	0x142
	.4byte	.L_T309
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2144_e-.L_l2144
.L_l2144:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2144_e:
.L_D2144_e:
.L_D2145:
	.4byte	.L_D2145_e-.L_D2145
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2146
	.2byte	0x38
	.string	"_status"
	.2byte	0x142
	.4byte	.L_T309
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2145_e-.L_l2145
.L_l2145:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2145_e:
.L_D2145_e:
.L_D2146:
	.4byte	.L_D2146_e-.L_D2146
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2147
	.2byte	0x38
	.string	"_stime"
	.2byte	0x142
	.4byte	.L_T309
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2146_e-.L_l2146
.L_l2146:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2146_e:
.L_D2146_e:
.L_D2147:
	.4byte	0x4
.L_D2143:
	.4byte	.L_D2143_e-.L_D2143
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2148
	.set	.L_T373,.L_D2143
	.2byte	0xa3
	.2byte	.L_s2143_e-.L_s2143
.L_s2143:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x1a
	.byte	0x8
	.2byte	0x55
	.2byte	0x7
.L_s2143_e:
.L_D2143_e:
.L_D2148:
	.4byte	.L_D2148_e-.L_D2148
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2149
	.set	.L_T376,.L_D2148
	.2byte	0xa3
	.2byte	.L_s2148_e-.L_s2148
.L_s2148:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0xf5
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s2148_e:
.L_D2148_e:
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2149:
	.4byte	.L_D2149_e-.L_D2149
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2150
	.2byte	0x38
	.string	"buf_t"
	.2byte	0x72
	.4byte	.L_T161
.L_D2149_e:
	.previous

.section	.debug
.L_D2150:
	.4byte	.L_D2150_e-.L_D2150
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2151
	.set	.L_T174,.L_D2150
	.2byte	0x38
	.string	"bufhd"
	.2byte	0xb6
	.4byte	0xc
.L_D2150_e:
.L_D2152:
	.4byte	.L_D2152_e-.L_D2152
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2153
	.2byte	0x38
	.string	"b_flags"
	.2byte	0x142
	.4byte	.L_T174
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2152_e-.L_l2152
.L_l2152:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2152_e:
.L_D2152_e:
.L_D2153:
	.4byte	.L_D2153_e-.L_D2153
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2154
	.2byte	0x38
	.string	"b_forw"
	.2byte	0x142
	.4byte	.L_T174
	.2byte	0x83
	.2byte	.L_t2153_e-.L_t2153
.L_t2153:
	.byte	0x1
	.4byte	.L_T161
.L_t2153_e:
	.2byte	0x23
	.2byte	.L_l2153_e-.L_l2153
.L_l2153:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2153_e:
.L_D2153_e:
.L_D2154:
	.4byte	.L_D2154_e-.L_D2154
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2155
	.2byte	0x38
	.string	"b_back"
	.2byte	0x142
	.4byte	.L_T174
	.2byte	0x83
	.2byte	.L_t2154_e-.L_t2154
.L_t2154:
	.byte	0x1
	.4byte	.L_T161
.L_t2154_e:
	.2byte	0x23
	.2byte	.L_l2154_e-.L_l2154
.L_l2154:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2154_e:
.L_D2154_e:
.L_D2155:
	.4byte	0x4
	.previous

.section	.debug
.L_D2151:
	.4byte	.L_D2151_e-.L_D2151
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2156
	.set	.L_T175,.L_D2151
	.2byte	0x38
	.string	"diskhd"
	.2byte	0xb6
	.4byte	0x18
.L_D2151_e:
.L_D2157:
	.4byte	.L_D2157_e-.L_D2157
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2158
	.2byte	0x38
	.string	"b_flags"
	.2byte	0x142
	.4byte	.L_T175
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2157_e-.L_l2157
.L_l2157:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2157_e:
.L_D2157_e:
.L_D2158:
	.4byte	.L_D2158_e-.L_D2158
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2159
	.2byte	0x38
	.string	"b_forw"
	.2byte	0x142
	.4byte	.L_T175
	.2byte	0x83
	.2byte	.L_t2158_e-.L_t2158
.L_t2158:
	.byte	0x1
	.4byte	.L_T161
.L_t2158_e:
	.2byte	0x23
	.2byte	.L_l2158_e-.L_l2158
.L_l2158:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2158_e:
.L_D2158_e:
.L_D2159:
	.4byte	.L_D2159_e-.L_D2159
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2160
	.2byte	0x38
	.string	"b_back"
	.2byte	0x142
	.4byte	.L_T175
	.2byte	0x83
	.2byte	.L_t2159_e-.L_t2159
.L_t2159:
	.byte	0x1
	.4byte	.L_T161
.L_t2159_e:
	.2byte	0x23
	.2byte	.L_l2159_e-.L_l2159
.L_l2159:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2159_e:
.L_D2159_e:
.L_D2160:
	.4byte	.L_D2160_e-.L_D2160
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2161
	.2byte	0x38
	.string	"av_forw"
	.2byte	0x142
	.4byte	.L_T175
	.2byte	0x83
	.2byte	.L_t2160_e-.L_t2160
.L_t2160:
	.byte	0x1
	.4byte	.L_T161
.L_t2160_e:
	.2byte	0x23
	.2byte	.L_l2160_e-.L_l2160
.L_l2160:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2160_e:
.L_D2160_e:
.L_D2161:
	.4byte	.L_D2161_e-.L_D2161
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2162
	.2byte	0x38
	.string	"av_back"
	.2byte	0x142
	.4byte	.L_T175
	.2byte	0x83
	.2byte	.L_t2161_e-.L_t2161
.L_t2161:
	.byte	0x1
	.4byte	.L_T161
.L_t2161_e:
	.2byte	0x23
	.2byte	.L_l2161_e-.L_l2161
.L_l2161:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l2161_e:
.L_D2161_e:
.L_D2162:
	.4byte	.L_D2162_e-.L_D2162
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2163
	.2byte	0x38
	.string	"b_bcount"
	.2byte	0x142
	.4byte	.L_T175
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2162_e-.L_l2162
.L_l2162:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l2162_e:
.L_D2162_e:
.L_D2163:
	.4byte	0x4
	.previous

.section	.debug
.L_D2156:
	.4byte	.L_D2156_e-.L_D2156
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2164
	.set	.L_T176,.L_D2156
	.2byte	0x38
	.string	"pfree"
	.2byte	0xb6
	.4byte	0x8
.L_D2156_e:
.L_D2165:
	.4byte	.L_D2165_e-.L_D2165
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2166
	.2byte	0x38
	.string	"b_flags"
	.2byte	0x142
	.4byte	.L_T176
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2165_e-.L_l2165
.L_l2165:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2165_e:
.L_D2165_e:
.L_D2166:
	.4byte	.L_D2166_e-.L_D2166
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2167
	.2byte	0x38
	.string	"av_forw"
	.2byte	0x142
	.4byte	.L_T176
	.2byte	0x83
	.2byte	.L_t2166_e-.L_t2166
.L_t2166:
	.byte	0x1
	.4byte	.L_T161
.L_t2166_e:
	.2byte	0x23
	.2byte	.L_l2166_e-.L_l2166
.L_l2166:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2166_e:
.L_D2166_e:
.L_D2167:
	.4byte	0x4
	.previous

.section	.debug
.L_D2164:
	.4byte	.L_D2164_e-.L_D2164
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2168
	.set	.L_T177,.L_D2164
	.2byte	0x38
	.string	"hbuf"
	.2byte	0xb6
	.4byte	0x10
.L_D2164_e:
.L_D2169:
	.4byte	.L_D2169_e-.L_D2169
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2170
	.2byte	0x38
	.string	"b_flags"
	.2byte	0x142
	.4byte	.L_T177
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2169_e-.L_l2169
.L_l2169:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2169_e:
.L_D2169_e:
.L_D2170:
	.4byte	.L_D2170_e-.L_D2170
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2171
	.2byte	0x38
	.string	"b_forw"
	.2byte	0x142
	.4byte	.L_T177
	.2byte	0x83
	.2byte	.L_t2170_e-.L_t2170
.L_t2170:
	.byte	0x1
	.4byte	.L_T161
.L_t2170_e:
	.2byte	0x23
	.2byte	.L_l2170_e-.L_l2170
.L_l2170:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2170_e:
.L_D2170_e:
.L_D2171:
	.4byte	.L_D2171_e-.L_D2171
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2172
	.2byte	0x38
	.string	"b_back"
	.2byte	0x142
	.4byte	.L_T177
	.2byte	0x83
	.2byte	.L_t2171_e-.L_t2171
.L_t2171:
	.byte	0x1
	.4byte	.L_T161
.L_t2171_e:
	.2byte	0x23
	.2byte	.L_l2171_e-.L_l2171
.L_l2171:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2171_e:
.L_D2171_e:
.L_D2172:
	.4byte	.L_D2172_e-.L_D2172
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2173
	.2byte	0x38
	.string	"b_pad"
	.2byte	0x142
	.4byte	.L_T177
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2172_e-.L_l2172
.L_l2172:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2172_e:
.L_D2172_e:
.L_D2173:
	.4byte	0x4
	.previous

.section	.debug
.L_D2168:
	.4byte	.L_D2168_e-.L_D2168
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2174
	.set	.L_T206,.L_D2168
	.2byte	0x38
	.string	"direct"
	.2byte	0xb6
	.4byte	0x10
.L_D2168_e:
.L_D2175:
	.4byte	.L_D2175_e-.L_D2175
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2176
	.2byte	0x38
	.string	"d_ino"
	.2byte	0x142
	.4byte	.L_T206
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2175_e-.L_l2175
.L_l2175:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2175_e:
.L_D2175_e:
.L_D2176:
	.4byte	.L_D2176_e-.L_D2176
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2177
	.2byte	0x38
	.string	"d_name"
	.2byte	0x142
	.4byte	.L_T206
	.2byte	0x72
	.4byte	.L_T208
	.2byte	0x23
	.2byte	.L_l2176_e-.L_l2176
.L_l2176:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l2176_e:
.L_D2176_e:
.L_D2177:
	.4byte	0x4
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2174:
	.4byte	.L_D2174_e-.L_D2174
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2178
	.2byte	0x38
	.string	"pte_t"
	.2byte	0x72
	.4byte	.L_T210
.L_D2174_e:
	.previous

.section	.debug
.L_D2178:
	.4byte	.L_D2178_e-.L_D2178
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2179
	.set	.L_T213,.L_D2178
	.2byte	0x38
	.string	"ptbl"
	.2byte	0xb6
	.4byte	0x1000
.L_D2178_e:
.L_D2180:
	.4byte	.L_D2180_e-.L_D2180
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2181
	.2byte	0x38
	.string	"page"
	.2byte	0x142
	.4byte	.L_T213
	.2byte	0x72
	.4byte	.L_T215
	.2byte	0x23
	.2byte	.L_l2180_e-.L_l2180
.L_l2180:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2180_e:
.L_D2180_e:
.L_D2181:
	.4byte	0x4
.L_D2179:
	.4byte	.L_D2179_e-.L_D2179
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2182
	.set	.L_T215,.L_D2179
	.2byte	0xa3
	.2byte	.L_s2179_e-.L_s2179
.L_s2179:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x3ff
	.byte	0x8
	.2byte	0x55
	.2byte	0x7
.L_s2179_e:
.L_D2179_e:
	.previous

.section	.debug
.L_D2182:
	.4byte	.L_D2182_e-.L_D2182
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2183
	.2byte	0x38
	.string	"ptbl_t"
	.2byte	0x72
	.4byte	.L_T213
.L_D2182_e:
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2183:
	.4byte	.L_D2183_e-.L_D2183
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2184
	.2byte	0x38
	.string	"proc_t"
	.2byte	0x72
	.4byte	.L_T165
.L_D2183_e:
	.previous

.section	.debug
.L_D2184:
	.4byte	.L_D2184_e-.L_D2184
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2185
	.set	.L_T296,.L_D2184
	.2byte	0x38
	.string	"siginfo"
	.2byte	0xb6
	.4byte	0x80
.L_D2184_e:
.L_D2186:
	.4byte	.L_D2186_e-.L_D2186
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2187
	.2byte	0x38
	.string	"si_signo"
	.2byte	0x142
	.4byte	.L_T296
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2186_e-.L_l2186
.L_l2186:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2186_e:
.L_D2186_e:
.L_D2187:
	.4byte	.L_D2187_e-.L_D2187
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2188
	.2byte	0x38
	.string	"si_code"
	.2byte	0x142
	.4byte	.L_T296
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2187_e-.L_l2187
.L_l2187:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2187_e:
.L_D2187_e:
.L_D2188:
	.4byte	.L_D2188_e-.L_D2188
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2189
	.2byte	0x38
	.string	"si_errno"
	.2byte	0x142
	.4byte	.L_T296
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2188_e-.L_l2188
.L_l2188:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2188_e:
.L_D2188_e:
.L_D2189:
	.4byte	.L_D2189_e-.L_D2189
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2190
	.2byte	0x38
	.string	"_data"
	.2byte	0x142
	.4byte	.L_T296
	.2byte	0x72
	.4byte	.L_T305
	.2byte	0x23
	.2byte	.L_l2189_e-.L_l2189
.L_l2189:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2189_e:
.L_D2189_e:
.L_D2190:
	.4byte	0x4
.L_D2185:
	.4byte	.L_D2185_e-.L_D2185
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D2191
	.set	.L_T305,.L_D2185
	.2byte	0xb6
	.4byte	0x74
.L_D2185_e:
.L_D2192:
	.4byte	.L_D2192_e-.L_D2192
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2193
	.2byte	0x38
	.string	"_pad"
	.2byte	0x142
	.4byte	.L_T305
	.2byte	0x72
	.4byte	.L_T298
	.2byte	0x23
	.2byte	.L_l2192_e-.L_l2192
.L_l2192:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2192_e:
.L_D2192_e:
.L_D2193:
	.4byte	.L_D2193_e-.L_D2193
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2194
	.2byte	0x38
	.string	"_proc"
	.2byte	0x142
	.4byte	.L_T305
	.2byte	0x72
	.4byte	.L_T302
	.2byte	0x23
	.2byte	.L_l2193_e-.L_l2193
.L_l2193:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2193_e:
.L_D2193_e:
.L_D2194:
	.4byte	.L_D2194_e-.L_D2194
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2195
	.2byte	0x38
	.string	"_fault"
	.2byte	0x142
	.4byte	.L_T305
	.2byte	0x72
	.4byte	.L_T303
	.2byte	0x23
	.2byte	.L_l2194_e-.L_l2194
.L_l2194:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2194_e:
.L_D2194_e:
.L_D2195:
	.4byte	.L_D2195_e-.L_D2195
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2196
	.2byte	0x38
	.string	"_file"
	.2byte	0x142
	.4byte	.L_T305
	.2byte	0x72
	.4byte	.L_T304
	.2byte	0x23
	.2byte	.L_l2195_e-.L_l2195
.L_l2195:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2195_e:
.L_D2195_e:
.L_D2196:
	.4byte	0x4
.L_D2191:
	.4byte	.L_D2191_e-.L_D2191
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2197
	.set	.L_T298,.L_D2191
	.2byte	0xa3
	.2byte	.L_s2191_e-.L_s2191
.L_s2191:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x1c
	.byte	0x8
	.2byte	0x55
	.2byte	0x7
.L_s2191_e:
.L_D2191_e:
.L_D2197:
	.4byte	.L_D2197_e-.L_D2197
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2198
	.set	.L_T302,.L_D2197
	.2byte	0xb6
	.4byte	0x10
.L_D2197_e:
.L_D2199:
	.4byte	.L_D2199_e-.L_D2199
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2200
	.2byte	0x38
	.string	"_pid"
	.2byte	0x142
	.4byte	.L_T302
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2199_e-.L_l2199
.L_l2199:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2199_e:
.L_D2199_e:
.L_D2200:
	.4byte	.L_D2200_e-.L_D2200
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2201
	.2byte	0x38
	.string	"_pdata"
	.2byte	0x142
	.4byte	.L_T302
	.2byte	0x72
	.4byte	.L_T301
	.2byte	0x23
	.2byte	.L_l2200_e-.L_l2200
.L_l2200:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2200_e:
.L_D2200_e:
.L_D2201:
	.4byte	0x4
.L_D2198:
	.4byte	.L_D2198_e-.L_D2198
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2202
	.set	.L_T303,.L_D2198
	.2byte	0xb6
	.4byte	0x4
.L_D2198_e:
.L_D2203:
	.4byte	.L_D2203_e-.L_D2203
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2204
	.2byte	0x38
	.string	"_addr"
	.2byte	0x142
	.4byte	.L_T303
	.2byte	0x63
	.2byte	.L_t2203_e-.L_t2203
.L_t2203:
	.byte	0x1
	.2byte	0x1
.L_t2203_e:
	.2byte	0x23
	.2byte	.L_l2203_e-.L_l2203
.L_l2203:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2203_e:
.L_D2203_e:
.L_D2204:
	.4byte	0x4
.L_D2202:
	.4byte	.L_D2202_e-.L_D2202
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2205
	.set	.L_T304,.L_D2202
	.2byte	0xb6
	.4byte	0x8
.L_D2202_e:
.L_D2206:
	.4byte	.L_D2206_e-.L_D2206
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2207
	.2byte	0x38
	.string	"_fd"
	.2byte	0x142
	.4byte	.L_T304
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2206_e-.L_l2206
.L_l2206:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2206_e:
.L_D2206_e:
.L_D2207:
	.4byte	.L_D2207_e-.L_D2207
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2208
	.2byte	0x38
	.string	"_band"
	.2byte	0x142
	.4byte	.L_T304
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2207_e-.L_l2207
.L_l2207:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2207_e:
.L_D2207_e:
.L_D2208:
	.4byte	0x4
.L_D2205:
	.4byte	.L_D2205_e-.L_D2205
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D2209
	.set	.L_T301,.L_D2205
	.2byte	0xb6
	.4byte	0xc
.L_D2205_e:
.L_D2210:
	.4byte	.L_D2210_e-.L_D2210
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2211
	.2byte	0x38
	.string	"_kill"
	.2byte	0x142
	.4byte	.L_T301
	.2byte	0x72
	.4byte	.L_T299
	.2byte	0x23
	.2byte	.L_l2210_e-.L_l2210
.L_l2210:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2210_e:
.L_D2210_e:
.L_D2211:
	.4byte	.L_D2211_e-.L_D2211
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2212
	.2byte	0x38
	.string	"_cld"
	.2byte	0x142
	.4byte	.L_T301
	.2byte	0x72
	.4byte	.L_T300
	.2byte	0x23
	.2byte	.L_l2211_e-.L_l2211
.L_l2211:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2211_e:
.L_D2211_e:
.L_D2212:
	.4byte	0x4
.L_D2209:
	.4byte	.L_D2209_e-.L_D2209
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2213
	.set	.L_T299,.L_D2209
	.2byte	0xb6
	.4byte	0x4
.L_D2209_e:
.L_D2214:
	.4byte	.L_D2214_e-.L_D2214
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2215
	.2byte	0x38
	.string	"_uid"
	.2byte	0x142
	.4byte	.L_T299
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2214_e-.L_l2214
.L_l2214:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2214_e:
.L_D2214_e:
.L_D2215:
	.4byte	0x4
.L_D2213:
	.4byte	.L_D2213_e-.L_D2213
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2216
	.set	.L_T300,.L_D2213
	.2byte	0xb6
	.4byte	0xc
.L_D2213_e:
.L_D2217:
	.4byte	.L_D2217_e-.L_D2217
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2218
	.2byte	0x38
	.string	"_utime"
	.2byte	0x142
	.4byte	.L_T300
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2217_e-.L_l2217
.L_l2217:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2217_e:
.L_D2217_e:
.L_D2218:
	.4byte	.L_D2218_e-.L_D2218
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2219
	.2byte	0x38
	.string	"_status"
	.2byte	0x142
	.4byte	.L_T300
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2218_e-.L_l2218
.L_l2218:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2218_e:
.L_D2218_e:
.L_D2219:
	.4byte	.L_D2219_e-.L_D2219
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2220
	.2byte	0x38
	.string	"_stime"
	.2byte	0x142
	.4byte	.L_T300
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2219_e-.L_l2219
.L_l2219:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2219_e:
.L_D2219_e:
.L_D2220:
	.4byte	0x4
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2216:
	.4byte	.L_D2216_e-.L_D2216
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2221
	.2byte	0x38
	.string	"siginfo_t"
	.2byte	0x72
	.4byte	.L_T296
.L_D2216_e:
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2221:
	.4byte	.L_D2221_e-.L_D2221
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2222
	.2byte	0x38
	.string	"k_siginfo_t"
	.2byte	0x72
	.4byte	.L_T307
.L_D2221_e:
	.previous

.section	.debug
.L_D2222:
	.4byte	.L_D2222_e-.L_D2222
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2223
	.2byte	0x38
	.string	"sigqueue_t"
	.2byte	0x72
	.4byte	.L_T249
.L_D2222_e:
	.previous

.section	.debug
.L_D2223:
	.4byte	.L_D2223_e-.L_D2223
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2224
	.set	.L_T330,.L_D2223
	.2byte	0x38
	.string	"ctxt_type"
	.2byte	0xb6
	.4byte	0x84
.L_D2223_e:
.L_D2225:
	.4byte	.L_D2225_e-.L_D2225
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2226
	.2byte	0x38
	.string	"REGISTERS"
	.2byte	0x142
	.4byte	.L_T330
	.2byte	0x72
	.4byte	.L_T332
	.2byte	0x23
	.2byte	.L_l2225_e-.L_l2225
.L_l2225:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2225_e:
.L_D2225_e:
.L_D2226:
	.4byte	.L_D2226_e-.L_D2226
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2227
	.2byte	0x38
	.string	"PCR"
	.2byte	0x142
	.4byte	.L_T330
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2226_e-.L_l2226
.L_l2226:
	.byte	0x4
	.4byte	0x80
	.byte	0x7
.L_l2226_e:
.L_D2226_e:
.L_D2227:
	.4byte	0x4
.L_D2224:
	.4byte	.L_D2224_e-.L_D2224
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2228
	.set	.L_T332,.L_D2224
	.2byte	0xa3
	.2byte	.L_s2224_e-.L_s2224
.L_s2224:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x1f
	.byte	0x8
	.2byte	0x55
	.2byte	0xa
.L_s2224_e:
.L_D2224_e:
	.previous

.section	.debug
.L_D2228:
	.4byte	.L_D2228_e-.L_D2228
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2229
	.2byte	0x38
	.string	"rlim_t"
	.2byte	0x55
	.2byte	0x9
.L_D2228_e:
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2229:
	.4byte	.L_D2229_e-.L_D2229
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2230
	.set	.L_T338,.L_D2229
	.2byte	0x38
	.string	"flags"
	.2byte	0xb6
	.4byte	0x4
.L_D2229_e:
.L_D2231:
	.4byte	.L_D2231_e-.L_D2231
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2232
	.2byte	0x38
	.string	"fl_cf"
	.2byte	0x142
	.4byte	.L_T338
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0x1f
	.2byte	0x23
	.2byte	.L_l2231_e-.L_l2231
.L_l2231:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2231_e:
.L_D2231_e:
.L_D2232:
	.4byte	.L_D2232_e-.L_D2232
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2233
	.2byte	0x38
	.string	"fl_pf"
	.2byte	0x142
	.4byte	.L_T338
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0x1d
	.2byte	0x23
	.2byte	.L_l2232_e-.L_l2232
.L_l2232:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2232_e:
.L_D2232_e:
.L_D2233:
	.4byte	.L_D2233_e-.L_D2233
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2234
	.2byte	0x38
	.string	"fl_af"
	.2byte	0x142
	.4byte	.L_T338
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0x1b
	.2byte	0x23
	.2byte	.L_l2233_e-.L_l2233
.L_l2233:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2233_e:
.L_D2233_e:
.L_D2234:
	.4byte	.L_D2234_e-.L_D2234
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2235
	.2byte	0x38
	.string	"fl_zf"
	.2byte	0x142
	.4byte	.L_T338
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0x19
	.2byte	0x23
	.2byte	.L_l2234_e-.L_l2234
.L_l2234:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2234_e:
.L_D2234_e:
.L_D2235:
	.4byte	.L_D2235_e-.L_D2235
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2236
	.2byte	0x38
	.string	"fl_sf"
	.2byte	0x142
	.4byte	.L_T338
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0x18
	.2byte	0x23
	.2byte	.L_l2235_e-.L_l2235
.L_l2235:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2235_e:
.L_D2235_e:
.L_D2236:
	.4byte	.L_D2236_e-.L_D2236
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2237
	.2byte	0x38
	.string	"fl_tf"
	.2byte	0x142
	.4byte	.L_T338
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0x17
	.2byte	0x23
	.2byte	.L_l2236_e-.L_l2236
.L_l2236:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2236_e:
.L_D2236_e:
.L_D2237:
	.4byte	.L_D2237_e-.L_D2237
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2238
	.2byte	0x38
	.string	"fl_if"
	.2byte	0x142
	.4byte	.L_T338
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0x16
	.2byte	0x23
	.2byte	.L_l2237_e-.L_l2237
.L_l2237:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2237_e:
.L_D2237_e:
.L_D2238:
	.4byte	.L_D2238_e-.L_D2238
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2239
	.2byte	0x38
	.string	"fl_df"
	.2byte	0x142
	.4byte	.L_T338
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0x15
	.2byte	0x23
	.2byte	.L_l2238_e-.L_l2238
.L_l2238:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2238_e:
.L_D2238_e:
.L_D2239:
	.4byte	.L_D2239_e-.L_D2239
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2240
	.2byte	0x38
	.string	"fl_of"
	.2byte	0x142
	.4byte	.L_T338
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0x14
	.2byte	0x23
	.2byte	.L_l2239_e-.L_l2239
.L_l2239:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2239_e:
.L_D2239_e:
.L_D2240:
	.4byte	.L_D2240_e-.L_D2240
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2241
	.2byte	0x38
	.string	"fl_iopl"
	.2byte	0x142
	.4byte	.L_T338
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x2
	.2byte	0xc5
	.2byte	0x12
	.2byte	0x23
	.2byte	.L_l2240_e-.L_l2240
.L_l2240:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2240_e:
.L_D2240_e:
.L_D2241:
	.4byte	.L_D2241_e-.L_D2241
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2242
	.2byte	0x38
	.string	"fl_nt"
	.2byte	0x142
	.4byte	.L_T338
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0x11
	.2byte	0x23
	.2byte	.L_l2241_e-.L_l2241
.L_l2241:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2241_e:
.L_D2241_e:
.L_D2242:
	.4byte	.L_D2242_e-.L_D2242
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2243
	.2byte	0x38
	.string	"fl_rf"
	.2byte	0x142
	.4byte	.L_T338
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0xf
	.2byte	0x23
	.2byte	.L_l2242_e-.L_l2242
.L_l2242:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2242_e:
.L_D2242_e:
.L_D2243:
	.4byte	.L_D2243_e-.L_D2243
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2244
	.2byte	0x38
	.string	"fl_vm"
	.2byte	0x142
	.4byte	.L_T338
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0xe
	.2byte	0x23
	.2byte	.L_l2243_e-.L_l2243
.L_l2243:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2243_e:
.L_D2243_e:
.L_D2244:
	.4byte	.L_D2244_e-.L_D2244
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2245
	.2byte	0x38
	.string	"fl_ac"
	.2byte	0x142
	.4byte	.L_T338
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0xd
	.2byte	0x23
	.2byte	.L_l2244_e-.L_l2244
.L_l2244:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2244_e:
.L_D2244_e:
.L_D2245:
	.4byte	.L_D2245_e-.L_D2245
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2246
	.2byte	0x38
	.string	"fl_vif"
	.2byte	0x142
	.4byte	.L_T338
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2245_e-.L_l2245
.L_l2245:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2245_e:
.L_D2245_e:
.L_D2246:
	.4byte	.L_D2246_e-.L_D2246
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2247
	.2byte	0x38
	.string	"fl_vip"
	.2byte	0x142
	.4byte	.L_T338
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0xb
	.2byte	0x23
	.2byte	.L_l2246_e-.L_l2246
.L_l2246:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2246_e:
.L_D2246_e:
.L_D2247:
	.4byte	.L_D2247_e-.L_D2247
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2248
	.2byte	0x38
	.string	"fl_id"
	.2byte	0x142
	.4byte	.L_T338
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x1
	.2byte	0xc5
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2247_e-.L_l2247
.L_l2247:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2247_e:
.L_D2247_e:
.L_D2248:
	.4byte	.L_D2248_e-.L_D2248
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2249
	.2byte	0x38
	.string	"fl_res"
	.2byte	0x142
	.4byte	.L_T338
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0xa
	.2byte	0xc5
	.2byte	0x0
	.2byte	0x23
	.2byte	.L_l2248_e-.L_l2248
.L_l2248:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2248_e:
.L_D2248_e:
.L_D2249:
	.4byte	0x4
	.previous

.section	.debug
.L_D2230:
	.4byte	.L_D2230_e-.L_D2230
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2250
	.2byte	0x38
	.string	"flags_t"
	.2byte	0x72
	.4byte	.L_T338
.L_D2230_e:
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2250:
	.4byte	.L_D2250_e-.L_D2250
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2251
	.set	.L_T342,.L_D2250
	.2byte	0x38
	.string	"dscr"
	.2byte	0xb6
	.4byte	0x8
.L_D2250_e:
.L_D2252:
	.4byte	.L_D2252_e-.L_D2252
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2253
	.2byte	0x38
	.string	"a_lim0015"
	.2byte	0x142
	.4byte	.L_T342
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x10
	.2byte	0xc5
	.2byte	0x10
	.2byte	0x23
	.2byte	.L_l2252_e-.L_l2252
.L_l2252:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2252_e:
.L_D2252_e:
.L_D2253:
	.4byte	.L_D2253_e-.L_D2253
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2254
	.2byte	0x38
	.string	"a_base0015"
	.2byte	0x142
	.4byte	.L_T342
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x10
	.2byte	0xc5
	.2byte	0x0
	.2byte	0x23
	.2byte	.L_l2253_e-.L_l2253
.L_l2253:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2253_e:
.L_D2253_e:
.L_D2254:
	.4byte	.L_D2254_e-.L_D2254
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2255
	.2byte	0x38
	.string	"a_base1623"
	.2byte	0x142
	.4byte	.L_T342
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x8
	.2byte	0xc5
	.2byte	0x18
	.2byte	0x23
	.2byte	.L_l2254_e-.L_l2254
.L_l2254:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2254_e:
.L_D2254_e:
.L_D2255:
	.4byte	.L_D2255_e-.L_D2255
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2256
	.2byte	0x38
	.string	"a_acc0007"
	.2byte	0x142
	.4byte	.L_T342
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x8
	.2byte	0xc5
	.2byte	0x10
	.2byte	0x23
	.2byte	.L_l2255_e-.L_l2255
.L_l2255:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2255_e:
.L_D2255_e:
.L_D2256:
	.4byte	.L_D2256_e-.L_D2256
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2257
	.2byte	0x38
	.string	"a_lim1619"
	.2byte	0x142
	.4byte	.L_T342
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x4
	.2byte	0xc5
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2256_e-.L_l2256
.L_l2256:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2256_e:
.L_D2256_e:
.L_D2257:
	.4byte	.L_D2257_e-.L_D2257
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2258
	.2byte	0x38
	.string	"a_acc0811"
	.2byte	0x142
	.4byte	.L_T342
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x4
	.2byte	0xc5
	.2byte	0x8
	.2byte	0x23
	.2byte	.L_l2257_e-.L_l2257
.L_l2257:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2257_e:
.L_D2257_e:
.L_D2258:
	.4byte	.L_D2258_e-.L_D2258
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2259
	.2byte	0x38
	.string	"a_base2431"
	.2byte	0x142
	.4byte	.L_T342
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x8
	.2byte	0xc5
	.2byte	0x0
	.2byte	0x23
	.2byte	.L_l2258_e-.L_l2258
.L_l2258:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2258_e:
.L_D2258_e:
.L_D2259:
	.4byte	0x4
	.previous

.section	.debug
.L_D2251:
	.4byte	.L_D2251_e-.L_D2251
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2260
	.set	.L_T343,.L_D2251
	.2byte	0x38
	.string	"i_seg_desc"
	.2byte	0xb6
	.4byte	0x8
.L_D2251_e:
.L_D2261:
	.4byte	.L_D2261_e-.L_D2261
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2262
	.2byte	0x38
	.string	"s_b0015"
	.2byte	0x142
	.4byte	.L_T343
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2261_e-.L_l2261
.L_l2261:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2261_e:
.L_D2261_e:
.L_D2262:
	.4byte	.L_D2262_e-.L_D2262
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2263
	.2byte	0x38
	.string	"s_b1623"
	.2byte	0x142
	.4byte	.L_T343
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l2262_e-.L_l2262
.L_l2262:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l2262_e:
.L_D2262_e:
.L_D2263:
	.4byte	.L_D2263_e-.L_D2263
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2264
	.2byte	0x38
	.string	"s_b2431"
	.2byte	0x142
	.4byte	.L_T343
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l2263_e-.L_l2263
.L_l2263:
	.byte	0x4
	.4byte	0x3
	.byte	0x7
.L_l2263_e:
.L_D2263_e:
.L_D2264:
	.4byte	.L_D2264_e-.L_D2264
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2265
	.2byte	0x38
	.string	"s_lim0015"
	.2byte	0x142
	.4byte	.L_T343
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2264_e-.L_l2264
.L_l2264:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2264_e:
.L_D2264_e:
.L_D2265:
	.4byte	.L_D2265_e-.L_D2265
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2266
	.2byte	0x38
	.string	"s_acc2"
	.2byte	0x142
	.4byte	.L_T343
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l2265_e-.L_l2265
.L_l2265:
	.byte	0x4
	.4byte	0x6
	.byte	0x7
.L_l2265_e:
.L_D2265_e:
.L_D2266:
	.4byte	.L_D2266_e-.L_D2266
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2267
	.2byte	0x38
	.string	"s_acc1"
	.2byte	0x142
	.4byte	.L_T343
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l2266_e-.L_l2266
.L_l2266:
	.byte	0x4
	.4byte	0x7
	.byte	0x7
.L_l2266_e:
.L_D2266_e:
.L_D2267:
	.4byte	0x4
	.previous

.section	.debug
.L_D2260:
	.4byte	.L_D2260_e-.L_D2260
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2268
	.set	.L_T344,.L_D2260
	.2byte	0x38
	.string	"r_seg_desc"
	.2byte	0xb6
	.4byte	0x8
.L_D2260_e:
.L_D2269:
	.4byte	.L_D2269_e-.L_D2269
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2270
	.2byte	0x38
	.string	"s_lim0015"
	.2byte	0x142
	.4byte	.L_T344
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2269_e-.L_l2269
.L_l2269:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2269_e:
.L_D2269_e:
.L_D2270:
	.4byte	.L_D2270_e-.L_D2270
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2271
	.2byte	0x38
	.string	"s_b0015"
	.2byte	0x142
	.4byte	.L_T344
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2270_e-.L_l2270
.L_l2270:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l2270_e:
.L_D2270_e:
.L_D2271:
	.4byte	.L_D2271_e-.L_D2271
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2272
	.2byte	0x38
	.string	"s_b1623"
	.2byte	0x142
	.4byte	.L_T344
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l2271_e-.L_l2271
.L_l2271:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2271_e:
.L_D2271_e:
.L_D2272:
	.4byte	.L_D2272_e-.L_D2272
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2273
	.2byte	0x38
	.string	"s_acc1"
	.2byte	0x142
	.4byte	.L_T344
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l2272_e-.L_l2272
.L_l2272:
	.byte	0x4
	.4byte	0x5
	.byte	0x7
.L_l2272_e:
.L_D2272_e:
.L_D2273:
	.4byte	.L_D2273_e-.L_D2273
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2274
	.2byte	0x38
	.string	"s_acc2"
	.2byte	0x142
	.4byte	.L_T344
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l2273_e-.L_l2273
.L_l2273:
	.byte	0x4
	.4byte	0x6
	.byte	0x7
.L_l2273_e:
.L_D2273_e:
.L_D2274:
	.4byte	.L_D2274_e-.L_D2274
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2275
	.2byte	0x38
	.string	"s_b2431"
	.2byte	0x142
	.4byte	.L_T344
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l2274_e-.L_l2274
.L_l2274:
	.byte	0x4
	.4byte	0x7
	.byte	0x7
.L_l2274_e:
.L_D2274_e:
.L_D2275:
	.4byte	0x4
	.previous

.section	.debug
.L_D2268:
	.4byte	.L_D2268_e-.L_D2268
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2276
	.set	.L_T345,.L_D2268
	.2byte	0x38
	.string	"gate_desc"
	.2byte	0xb6
	.4byte	0x8
.L_D2268_e:
.L_D2277:
	.4byte	.L_D2277_e-.L_D2277
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2278
	.2byte	0x38
	.string	"g_off"
	.2byte	0x142
	.4byte	.L_T345
	.2byte	0x55
	.2byte	0xc
	.2byte	0x23
	.2byte	.L_l2277_e-.L_l2277
.L_l2277:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2277_e:
.L_D2277_e:
.L_D2278:
	.4byte	.L_D2278_e-.L_D2278
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2279
	.2byte	0x38
	.string	"g_sel"
	.2byte	0x142
	.4byte	.L_T345
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2278_e-.L_l2278
.L_l2278:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2278_e:
.L_D2278_e:
.L_D2279:
	.4byte	.L_D2279_e-.L_D2279
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2280
	.2byte	0x38
	.string	"g_wcount"
	.2byte	0x142
	.4byte	.L_T345
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l2279_e-.L_l2279
.L_l2279:
	.byte	0x4
	.4byte	0x6
	.byte	0x7
.L_l2279_e:
.L_D2279_e:
.L_D2280:
	.4byte	.L_D2280_e-.L_D2280
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2281
	.2byte	0x38
	.string	"g_type"
	.2byte	0x142
	.4byte	.L_T345
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l2280_e-.L_l2280
.L_l2280:
	.byte	0x4
	.4byte	0x7
	.byte	0x7
.L_l2280_e:
.L_D2280_e:
.L_D2281:
	.4byte	0x4
	.previous

.section	.debug
.L_D2276:
	.4byte	.L_D2276_e-.L_D2276
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2282
	.set	.L_T346,.L_D2276
	.2byte	0x38
	.string	"i_gate_desc"
	.2byte	0xb6
	.4byte	0x8
.L_D2276_e:
.L_D2283:
	.4byte	.L_D2283_e-.L_D2283
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2284
	.2byte	0x38
	.string	"g_off0015"
	.2byte	0x142
	.4byte	.L_T346
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2283_e-.L_l2283
.L_l2283:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2283_e:
.L_D2283_e:
.L_D2284:
	.4byte	.L_D2284_e-.L_D2284
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2285
	.2byte	0x38
	.string	"g_off1631"
	.2byte	0x142
	.4byte	.L_T346
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2284_e-.L_l2284
.L_l2284:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l2284_e:
.L_D2284_e:
.L_D2285:
	.4byte	.L_D2285_e-.L_D2285
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2286
	.2byte	0x38
	.string	"g_sel"
	.2byte	0x142
	.4byte	.L_T346
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2285_e-.L_l2285
.L_l2285:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2285_e:
.L_D2285_e:
.L_D2286:
	.4byte	.L_D2286_e-.L_D2286
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2287
	.2byte	0x38
	.string	"g_wcount"
	.2byte	0x142
	.4byte	.L_T346
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l2286_e-.L_l2286
.L_l2286:
	.byte	0x4
	.4byte	0x6
	.byte	0x7
.L_l2286_e:
.L_D2286_e:
.L_D2287:
	.4byte	.L_D2287_e-.L_D2287
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2288
	.2byte	0x38
	.string	"g_type"
	.2byte	0x142
	.4byte	.L_T346
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l2287_e-.L_l2287
.L_l2287:
	.byte	0x4
	.4byte	0x7
	.byte	0x7
.L_l2287_e:
.L_D2287_e:
.L_D2288:
	.4byte	0x4
	.previous

.section	.debug
.L_D2282:
	.4byte	.L_D2282_e-.L_D2282
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2289
	.set	.L_T347,.L_D2282
	.2byte	0x38
	.string	"r_gate_desc"
	.2byte	0xb6
	.4byte	0x8
.L_D2282_e:
.L_D2290:
	.4byte	.L_D2290_e-.L_D2290
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2291
	.2byte	0x38
	.string	"g_off0015"
	.2byte	0x142
	.4byte	.L_T347
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2290_e-.L_l2290
.L_l2290:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2290_e:
.L_D2290_e:
.L_D2291:
	.4byte	.L_D2291_e-.L_D2291
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2292
	.2byte	0x38
	.string	"g_sel"
	.2byte	0x142
	.4byte	.L_T347
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2291_e-.L_l2291
.L_l2291:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l2291_e:
.L_D2291_e:
.L_D2292:
	.4byte	.L_D2292_e-.L_D2292
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2293
	.2byte	0x38
	.string	"g_wcount"
	.2byte	0x142
	.4byte	.L_T347
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l2292_e-.L_l2292
.L_l2292:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2292_e:
.L_D2292_e:
.L_D2293:
	.4byte	.L_D2293_e-.L_D2293
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2294
	.2byte	0x38
	.string	"g_type"
	.2byte	0x142
	.4byte	.L_T347
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l2293_e-.L_l2293
.L_l2293:
	.byte	0x4
	.4byte	0x5
	.byte	0x7
.L_l2293_e:
.L_D2293_e:
.L_D2294:
	.4byte	.L_D2294_e-.L_D2294
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2295
	.2byte	0x38
	.string	"g_off1631"
	.2byte	0x142
	.4byte	.L_T347
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2294_e-.L_l2294
.L_l2294:
	.byte	0x4
	.4byte	0x6
	.byte	0x7
.L_l2294_e:
.L_D2294_e:
.L_D2295:
	.4byte	0x4
	.previous

.section	.debug
.L_D2289:
	.4byte	.L_D2289_e-.L_D2289
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2296
	.set	.L_T348,.L_D2289
	.2byte	0x38
	.string	"gdscr"
	.2byte	0xb6
	.4byte	0x8
.L_D2289_e:
.L_D2297:
	.4byte	.L_D2297_e-.L_D2297
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2298
	.2byte	0x38
	.string	"gd_off0015"
	.2byte	0x142
	.4byte	.L_T348
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x10
	.2byte	0xc5
	.2byte	0x10
	.2byte	0x23
	.2byte	.L_l2297_e-.L_l2297
.L_l2297:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2297_e:
.L_D2297_e:
.L_D2298:
	.4byte	.L_D2298_e-.L_D2298
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2299
	.2byte	0x38
	.string	"gd_selector"
	.2byte	0x142
	.4byte	.L_T348
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x10
	.2byte	0xc5
	.2byte	0x0
	.2byte	0x23
	.2byte	.L_l2298_e-.L_l2298
.L_l2298:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2298_e:
.L_D2298_e:
.L_D2299:
	.4byte	.L_D2299_e-.L_D2299
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2300
	.2byte	0x38
	.string	"gd_unused"
	.2byte	0x142
	.4byte	.L_T348
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x8
	.2byte	0xc5
	.2byte	0x18
	.2byte	0x23
	.2byte	.L_l2299_e-.L_l2299
.L_l2299:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2299_e:
.L_D2299_e:
.L_D2300:
	.4byte	.L_D2300_e-.L_D2300
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2301
	.2byte	0x38
	.string	"gd_acc0007"
	.2byte	0x142
	.4byte	.L_T348
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x8
	.2byte	0xc5
	.2byte	0x10
	.2byte	0x23
	.2byte	.L_l2300_e-.L_l2300
.L_l2300:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2300_e:
.L_D2300_e:
.L_D2301:
	.4byte	.L_D2301_e-.L_D2301
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2302
	.2byte	0x38
	.string	"gd_off1631"
	.2byte	0x142
	.4byte	.L_T348
	.2byte	0x55
	.2byte	0x9
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xd6
	.4byte	0x10
	.2byte	0xc5
	.2byte	0x0
	.2byte	0x23
	.2byte	.L_l2301_e-.L_l2301
.L_l2301:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2301_e:
.L_D2301_e:
.L_D2302:
	.4byte	0x4
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2296:
	.4byte	.L_D2296_e-.L_D2296
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2303
	.2byte	0x38
	.string	"fault_catch_t"
	.2byte	0x72
	.4byte	.L_T349
.L_D2296_e:
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2303:
	.4byte	.L_D2303_e-.L_D2303
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2304
	.2byte	0x38
	.string	"k_sysset_t"
	.2byte	0x72
	.4byte	.L_T364
.L_D2303_e:
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2304:
	.4byte	.L_D2304_e-.L_D2304
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2305
	.2byte	0x38
	.string	"user_t"
	.2byte	0x72
	.4byte	.L_T366
.L_D2304_e:
	.previous

.section	.debug
.L_P11:
.L_D2305:
	.4byte	.L_D2305_e-.L_D2305
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D2306
	.2byte	0x38
	.string	"u"
	.2byte	0x72
	.4byte	.L_T366
.L_D2305_e:
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2306:
	.4byte	.L_D2306_e-.L_D2306
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2307
	.set	.L_T428,.L_D2306
	.2byte	0x38
	.string	"bdevsw"
	.2byte	0xb6
	.4byte	0x20
.L_D2306_e:
.L_D2308:
	.4byte	.L_D2308_e-.L_D2308
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2309
	.2byte	0x38
	.string	"d_open"
	.2byte	0x142
	.4byte	.L_T428
	.2byte	0x83
	.2byte	.L_t2308_e-.L_t2308
.L_t2308:
	.byte	0x1
	.4byte	.L_T38
.L_t2308_e:
	.2byte	0x23
	.2byte	.L_l2308_e-.L_l2308
.L_l2308:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2308_e:
.L_D2308_e:
.L_D2309:
	.4byte	.L_D2309_e-.L_D2309
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2310
	.2byte	0x38
	.string	"d_close"
	.2byte	0x142
	.4byte	.L_T428
	.2byte	0x83
	.2byte	.L_t2309_e-.L_t2309
.L_t2309:
	.byte	0x1
	.4byte	.L_T38
.L_t2309_e:
	.2byte	0x23
	.2byte	.L_l2309_e-.L_l2309
.L_l2309:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2309_e:
.L_D2309_e:
.L_D2310:
	.4byte	.L_D2310_e-.L_D2310
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2311
	.2byte	0x38
	.string	"d_strategy"
	.2byte	0x142
	.4byte	.L_T428
	.2byte	0x83
	.2byte	.L_t2310_e-.L_t2310
.L_t2310:
	.byte	0x1
	.4byte	.L_T38
.L_t2310_e:
	.2byte	0x23
	.2byte	.L_l2310_e-.L_l2310
.L_l2310:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2310_e:
.L_D2310_e:
.L_D2311:
	.4byte	.L_D2311_e-.L_D2311
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2312
	.2byte	0x38
	.string	"d_print"
	.2byte	0x142
	.4byte	.L_T428
	.2byte	0x83
	.2byte	.L_t2311_e-.L_t2311
.L_t2311:
	.byte	0x1
	.4byte	.L_T38
.L_t2311_e:
	.2byte	0x23
	.2byte	.L_l2311_e-.L_l2311
.L_l2311:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2311_e:
.L_D2311_e:
.L_D2312:
	.4byte	.L_D2312_e-.L_D2312
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2313
	.2byte	0x38
	.string	"d_size"
	.2byte	0x142
	.4byte	.L_T428
	.2byte	0x83
	.2byte	.L_t2312_e-.L_t2312
.L_t2312:
	.byte	0x1
	.4byte	.L_T38
.L_t2312_e:
	.2byte	0x23
	.2byte	.L_l2312_e-.L_l2312
.L_l2312:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l2312_e:
.L_D2312_e:
.L_D2313:
	.4byte	.L_D2313_e-.L_D2313
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2314
	.2byte	0x38
	.string	"d_name"
	.2byte	0x142
	.4byte	.L_T428
	.2byte	0x63
	.2byte	.L_t2313_e-.L_t2313
.L_t2313:
	.byte	0x1
	.2byte	0x1
.L_t2313_e:
	.2byte	0x23
	.2byte	.L_l2313_e-.L_l2313
.L_l2313:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l2313_e:
.L_D2313_e:
.L_D2314:
	.4byte	.L_D2314_e-.L_D2314
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2315
	.2byte	0x38
	.string	"d_tab"
	.2byte	0x142
	.4byte	.L_T428
	.2byte	0x83
	.2byte	.L_t2314_e-.L_t2314
.L_t2314:
	.byte	0x1
	.4byte	.L_T435
.L_t2314_e:
	.2byte	0x23
	.2byte	.L_l2314_e-.L_l2314
.L_l2314:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l2314_e:
.L_D2314_e:
.L_D2315:
	.4byte	.L_D2315_e-.L_D2315
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2316
	.2byte	0x38
	.string	"d_flag"
	.2byte	0x142
	.4byte	.L_T428
	.2byte	0x63
	.2byte	.L_t2315_e-.L_t2315
.L_t2315:
	.byte	0x1
	.2byte	0x7
.L_t2315_e:
	.2byte	0x23
	.2byte	.L_l2315_e-.L_l2315
.L_l2315:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l2315_e:
.L_D2315_e:
.L_D2316:
	.4byte	0x4
.L_D2307:
	.4byte	.L_D2307_e-.L_D2307
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2317
	.set	.L_T435,.L_D2307
	.2byte	0x38
	.string	"iobuf"
.L_D2307_e:
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2317:
	.4byte	.L_D2317_e-.L_D2317
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2318
	.set	.L_T440,.L_D2317
	.2byte	0x38
	.string	"cdevsw"
	.2byte	0xb6
	.4byte	0x30
.L_D2317_e:
.L_D2319:
	.4byte	.L_D2319_e-.L_D2319
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2320
	.2byte	0x38
	.string	"d_open"
	.2byte	0x142
	.4byte	.L_T440
	.2byte	0x83
	.2byte	.L_t2319_e-.L_t2319
.L_t2319:
	.byte	0x1
	.4byte	.L_T38
.L_t2319_e:
	.2byte	0x23
	.2byte	.L_l2319_e-.L_l2319
.L_l2319:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2319_e:
.L_D2319_e:
.L_D2320:
	.4byte	.L_D2320_e-.L_D2320
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2321
	.2byte	0x38
	.string	"d_close"
	.2byte	0x142
	.4byte	.L_T440
	.2byte	0x83
	.2byte	.L_t2320_e-.L_t2320
.L_t2320:
	.byte	0x1
	.4byte	.L_T38
.L_t2320_e:
	.2byte	0x23
	.2byte	.L_l2320_e-.L_l2320
.L_l2320:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2320_e:
.L_D2320_e:
.L_D2321:
	.4byte	.L_D2321_e-.L_D2321
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2322
	.2byte	0x38
	.string	"d_read"
	.2byte	0x142
	.4byte	.L_T440
	.2byte	0x83
	.2byte	.L_t2321_e-.L_t2321
.L_t2321:
	.byte	0x1
	.4byte	.L_T38
.L_t2321_e:
	.2byte	0x23
	.2byte	.L_l2321_e-.L_l2321
.L_l2321:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2321_e:
.L_D2321_e:
.L_D2322:
	.4byte	.L_D2322_e-.L_D2322
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2323
	.2byte	0x38
	.string	"d_write"
	.2byte	0x142
	.4byte	.L_T440
	.2byte	0x83
	.2byte	.L_t2322_e-.L_t2322
.L_t2322:
	.byte	0x1
	.4byte	.L_T38
.L_t2322_e:
	.2byte	0x23
	.2byte	.L_l2322_e-.L_l2322
.L_l2322:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2322_e:
.L_D2322_e:
.L_D2323:
	.4byte	.L_D2323_e-.L_D2323
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2324
	.2byte	0x38
	.string	"d_ioctl"
	.2byte	0x142
	.4byte	.L_T440
	.2byte	0x83
	.2byte	.L_t2323_e-.L_t2323
.L_t2323:
	.byte	0x1
	.4byte	.L_T38
.L_t2323_e:
	.2byte	0x23
	.2byte	.L_l2323_e-.L_l2323
.L_l2323:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l2323_e:
.L_D2323_e:
.L_D2324:
	.4byte	.L_D2324_e-.L_D2324
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2325
	.2byte	0x38
	.string	"d_mmap"
	.2byte	0x142
	.4byte	.L_T440
	.2byte	0x83
	.2byte	.L_t2324_e-.L_t2324
.L_t2324:
	.byte	0x1
	.4byte	.L_T38
.L_t2324_e:
	.2byte	0x23
	.2byte	.L_l2324_e-.L_l2324
.L_l2324:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l2324_e:
.L_D2324_e:
.L_D2325:
	.4byte	.L_D2325_e-.L_D2325
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2326
	.2byte	0x38
	.string	"d_segmap"
	.2byte	0x142
	.4byte	.L_T440
	.2byte	0x83
	.2byte	.L_t2325_e-.L_t2325
.L_t2325:
	.byte	0x1
	.4byte	.L_T38
.L_t2325_e:
	.2byte	0x23
	.2byte	.L_l2325_e-.L_l2325
.L_l2325:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l2325_e:
.L_D2325_e:
.L_D2326:
	.4byte	.L_D2326_e-.L_D2326
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2327
	.2byte	0x38
	.string	"d_poll"
	.2byte	0x142
	.4byte	.L_T440
	.2byte	0x83
	.2byte	.L_t2326_e-.L_t2326
.L_t2326:
	.byte	0x1
	.4byte	.L_T38
.L_t2326_e:
	.2byte	0x23
	.2byte	.L_l2326_e-.L_l2326
.L_l2326:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l2326_e:
.L_D2326_e:
.L_D2327:
	.4byte	.L_D2327_e-.L_D2327
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2328
	.2byte	0x38
	.string	"d_ttys"
	.2byte	0x142
	.4byte	.L_T440
	.2byte	0x83
	.2byte	.L_t2327_e-.L_t2327
.L_t2327:
	.byte	0x1
	.4byte	.L_T449
.L_t2327_e:
	.2byte	0x23
	.2byte	.L_l2327_e-.L_l2327
.L_l2327:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l2327_e:
.L_D2327_e:
.L_D2328:
	.4byte	.L_D2328_e-.L_D2328
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2329
	.2byte	0x38
	.string	"d_str"
	.2byte	0x142
	.4byte	.L_T440
	.2byte	0x83
	.2byte	.L_t2328_e-.L_t2328
.L_t2328:
	.byte	0x1
	.4byte	.L_T451
.L_t2328_e:
	.2byte	0x23
	.2byte	.L_l2328_e-.L_l2328
.L_l2328:
	.byte	0x4
	.4byte	0x24
	.byte	0x7
.L_l2328_e:
.L_D2328_e:
.L_D2329:
	.4byte	.L_D2329_e-.L_D2329
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2330
	.2byte	0x38
	.string	"d_name"
	.2byte	0x142
	.4byte	.L_T440
	.2byte	0x63
	.2byte	.L_t2329_e-.L_t2329
.L_t2329:
	.byte	0x1
	.2byte	0x1
.L_t2329_e:
	.2byte	0x23
	.2byte	.L_l2329_e-.L_l2329
.L_l2329:
	.byte	0x4
	.4byte	0x28
	.byte	0x7
.L_l2329_e:
.L_D2329_e:
.L_D2330:
	.4byte	.L_D2330_e-.L_D2330
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2331
	.2byte	0x38
	.string	"d_flag"
	.2byte	0x142
	.4byte	.L_T440
	.2byte	0x63
	.2byte	.L_t2330_e-.L_t2330
.L_t2330:
	.byte	0x1
	.2byte	0x7
.L_t2330_e:
	.2byte	0x23
	.2byte	.L_l2330_e-.L_l2330
.L_l2330:
	.byte	0x4
	.4byte	0x2c
	.byte	0x7
.L_l2330_e:
.L_D2330_e:
.L_D2331:
	.4byte	0x4
.L_D2318:
	.4byte	.L_D2318_e-.L_D2318
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2332
	.set	.L_T449,.L_D2318
	.2byte	0x38
	.string	"tty"
.L_D2318_e:
.L_D2332:
	.4byte	.L_D2332_e-.L_D2332
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2333
	.set	.L_T451,.L_D2332
	.2byte	0x38
	.string	"streamtab"
	.2byte	0xb6
	.4byte	0x10
.L_D2332_e:
.L_D2334:
	.4byte	.L_D2334_e-.L_D2334
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2335
	.2byte	0x38
	.string	"st_rdinit"
	.2byte	0x142
	.4byte	.L_T451
	.2byte	0x83
	.2byte	.L_t2334_e-.L_t2334
.L_t2334:
	.byte	0x1
	.4byte	.L_T665
.L_t2334_e:
	.2byte	0x23
	.2byte	.L_l2334_e-.L_l2334
.L_l2334:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2334_e:
.L_D2334_e:
.L_D2335:
	.4byte	.L_D2335_e-.L_D2335
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2336
	.2byte	0x38
	.string	"st_wrinit"
	.2byte	0x142
	.4byte	.L_T451
	.2byte	0x83
	.2byte	.L_t2335_e-.L_t2335
.L_t2335:
	.byte	0x1
	.4byte	.L_T665
.L_t2335_e:
	.2byte	0x23
	.2byte	.L_l2335_e-.L_l2335
.L_l2335:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2335_e:
.L_D2335_e:
.L_D2336:
	.4byte	.L_D2336_e-.L_D2336
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2337
	.2byte	0x38
	.string	"st_muxrinit"
	.2byte	0x142
	.4byte	.L_T451
	.2byte	0x83
	.2byte	.L_t2336_e-.L_t2336
.L_t2336:
	.byte	0x1
	.4byte	.L_T665
.L_t2336_e:
	.2byte	0x23
	.2byte	.L_l2336_e-.L_l2336
.L_l2336:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2336_e:
.L_D2336_e:
.L_D2337:
	.4byte	.L_D2337_e-.L_D2337
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2338
	.2byte	0x38
	.string	"st_muxwinit"
	.2byte	0x142
	.4byte	.L_T451
	.2byte	0x83
	.2byte	.L_t2337_e-.L_t2337
.L_t2337:
	.byte	0x1
	.4byte	.L_T665
.L_t2337_e:
	.2byte	0x23
	.2byte	.L_l2337_e-.L_l2337
.L_l2337:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2337_e:
.L_D2337_e:
.L_D2338:
	.4byte	0x4
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2333:
	.4byte	.L_D2333_e-.L_D2333
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2339
	.set	.L_T456,.L_D2333
	.2byte	0x38
	.string	"conssw"
	.2byte	0xb6
	.4byte	0x10
.L_D2333_e:
.L_D2340:
	.4byte	.L_D2340_e-.L_D2340
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2341
	.2byte	0x38
	.string	"co"
	.2byte	0x142
	.4byte	.L_T456
	.2byte	0x83
	.2byte	.L_t2340_e-.L_t2340
.L_t2340:
	.byte	0x1
	.4byte	.L_T38
.L_t2340_e:
	.2byte	0x23
	.2byte	.L_l2340_e-.L_l2340
.L_l2340:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2340_e:
.L_D2340_e:
.L_D2341:
	.4byte	.L_D2341_e-.L_D2341
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2342
	.2byte	0x38
	.string	"co_dev"
	.2byte	0x142
	.4byte	.L_T456
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2341_e-.L_l2341
.L_l2341:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2341_e:
.L_D2341_e:
.L_D2342:
	.4byte	.L_D2342_e-.L_D2342
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2343
	.2byte	0x38
	.string	"ci"
	.2byte	0x142
	.4byte	.L_T456
	.2byte	0x83
	.2byte	.L_t2342_e-.L_t2342
.L_t2342:
	.byte	0x1
	.4byte	.L_T38
.L_t2342_e:
	.2byte	0x23
	.2byte	.L_l2342_e-.L_l2342
.L_l2342:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2342_e:
.L_D2342_e:
.L_D2343:
	.4byte	.L_D2343_e-.L_D2343
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2344
	.2byte	0x38
	.string	"co_mode"
	.2byte	0x142
	.4byte	.L_T456
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2343_e-.L_l2343
.L_l2343:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2343_e:
.L_D2343_e:
.L_D2344:
	.4byte	0x4
	.previous

.section	.debug
.L_D2339:
	.4byte	.L_D2339_e-.L_D2339
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2345
	.set	.L_T459,.L_D2339
	.2byte	0x38
	.string	"fmodsw"
	.2byte	0xb6
	.4byte	0x14
.L_D2339_e:
.L_D2346:
	.4byte	.L_D2346_e-.L_D2346
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2347
	.2byte	0x38
	.string	"f_name"
	.2byte	0x142
	.4byte	.L_T459
	.2byte	0x72
	.4byte	.L_T461
	.2byte	0x23
	.2byte	.L_l2346_e-.L_l2346
.L_l2346:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2346_e:
.L_D2346_e:
.L_D2347:
	.4byte	.L_D2347_e-.L_D2347
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2348
	.2byte	0x38
	.string	"f_str"
	.2byte	0x142
	.4byte	.L_T459
	.2byte	0x83
	.2byte	.L_t2347_e-.L_t2347
.L_t2347:
	.byte	0x1
	.4byte	.L_T451
.L_t2347_e:
	.2byte	0x23
	.2byte	.L_l2347_e-.L_l2347
.L_l2347:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2347_e:
.L_D2347_e:
.L_D2348:
	.4byte	.L_D2348_e-.L_D2348
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2349
	.2byte	0x38
	.string	"f_flag"
	.2byte	0x142
	.4byte	.L_T459
	.2byte	0x63
	.2byte	.L_t2348_e-.L_t2348
.L_t2348:
	.byte	0x1
	.2byte	0x7
.L_t2348_e:
	.2byte	0x23
	.2byte	.L_l2348_e-.L_l2348
.L_l2348:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l2348_e:
.L_D2348_e:
.L_D2349:
	.4byte	0x4
.L_D2345:
	.4byte	.L_D2345_e-.L_D2345
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2350
	.set	.L_T461,.L_D2345
	.2byte	0xa3
	.2byte	.L_s2345_e-.L_s2345
.L_s2345:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x8
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s2345_e:
.L_D2345_e:
	.previous

.section	.debug
.L_D2350:
	.4byte	.L_D2350_e-.L_D2350
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2351
	.set	.L_T464,.L_D2350
	.2byte	0x38
	.string	"linesw"
	.2byte	0xb6
	.4byte	0x20
.L_D2350_e:
.L_D2352:
	.4byte	.L_D2352_e-.L_D2352
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2353
	.2byte	0x38
	.string	"l_open"
	.2byte	0x142
	.4byte	.L_T464
	.2byte	0x83
	.2byte	.L_t2352_e-.L_t2352
.L_t2352:
	.byte	0x1
	.4byte	.L_T38
.L_t2352_e:
	.2byte	0x23
	.2byte	.L_l2352_e-.L_l2352
.L_l2352:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2352_e:
.L_D2352_e:
.L_D2353:
	.4byte	.L_D2353_e-.L_D2353
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2354
	.2byte	0x38
	.string	"l_close"
	.2byte	0x142
	.4byte	.L_T464
	.2byte	0x83
	.2byte	.L_t2353_e-.L_t2353
.L_t2353:
	.byte	0x1
	.4byte	.L_T38
.L_t2353_e:
	.2byte	0x23
	.2byte	.L_l2353_e-.L_l2353
.L_l2353:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2353_e:
.L_D2353_e:
.L_D2354:
	.4byte	.L_D2354_e-.L_D2354
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2355
	.2byte	0x38
	.string	"l_read"
	.2byte	0x142
	.4byte	.L_T464
	.2byte	0x83
	.2byte	.L_t2354_e-.L_t2354
.L_t2354:
	.byte	0x1
	.4byte	.L_T38
.L_t2354_e:
	.2byte	0x23
	.2byte	.L_l2354_e-.L_l2354
.L_l2354:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2354_e:
.L_D2354_e:
.L_D2355:
	.4byte	.L_D2355_e-.L_D2355
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2356
	.2byte	0x38
	.string	"l_write"
	.2byte	0x142
	.4byte	.L_T464
	.2byte	0x83
	.2byte	.L_t2355_e-.L_t2355
.L_t2355:
	.byte	0x1
	.4byte	.L_T38
.L_t2355_e:
	.2byte	0x23
	.2byte	.L_l2355_e-.L_l2355
.L_l2355:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2355_e:
.L_D2355_e:
.L_D2356:
	.4byte	.L_D2356_e-.L_D2356
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2357
	.2byte	0x38
	.string	"l_ioctl"
	.2byte	0x142
	.4byte	.L_T464
	.2byte	0x83
	.2byte	.L_t2356_e-.L_t2356
.L_t2356:
	.byte	0x1
	.4byte	.L_T38
.L_t2356_e:
	.2byte	0x23
	.2byte	.L_l2356_e-.L_l2356
.L_l2356:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l2356_e:
.L_D2356_e:
.L_D2357:
	.4byte	.L_D2357_e-.L_D2357
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2358
	.2byte	0x38
	.string	"l_input"
	.2byte	0x142
	.4byte	.L_T464
	.2byte	0x83
	.2byte	.L_t2357_e-.L_t2357
.L_t2357:
	.byte	0x1
	.4byte	.L_T38
.L_t2357_e:
	.2byte	0x23
	.2byte	.L_l2357_e-.L_l2357
.L_l2357:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l2357_e:
.L_D2357_e:
.L_D2358:
	.4byte	.L_D2358_e-.L_D2358
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2359
	.2byte	0x38
	.string	"l_output"
	.2byte	0x142
	.4byte	.L_T464
	.2byte	0x83
	.2byte	.L_t2358_e-.L_t2358
.L_t2358:
	.byte	0x1
	.4byte	.L_T38
.L_t2358_e:
	.2byte	0x23
	.2byte	.L_l2358_e-.L_l2358
.L_l2358:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l2358_e:
.L_D2358_e:
.L_D2359:
	.4byte	.L_D2359_e-.L_D2359
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2360
	.2byte	0x38
	.string	"l_mdmint"
	.2byte	0x142
	.4byte	.L_T464
	.2byte	0x83
	.2byte	.L_t2359_e-.L_t2359
.L_t2359:
	.byte	0x1
	.4byte	.L_T38
.L_t2359_e:
	.2byte	0x23
	.2byte	.L_l2359_e-.L_l2359
.L_l2359:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l2359_e:
.L_D2359_e:
.L_D2360:
	.4byte	0x4
	.previous

.section	.debug
.L_D2351:
	.4byte	.L_D2351_e-.L_D2351
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2361
	.set	.L_T475,.L_D2351
	.2byte	0x38
	.string	"sysinfo"
	.2byte	0xb6
	.4byte	0xd4
.L_D2351_e:
.L_D2362:
	.4byte	.L_D2362_e-.L_D2362
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2363
	.2byte	0x38
	.string	"cpu"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x72
	.4byte	.L_T478
	.2byte	0x23
	.2byte	.L_l2362_e-.L_l2362
.L_l2362:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2362_e:
.L_D2362_e:
.L_D2363:
	.4byte	.L_D2363_e-.L_D2363
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2364
	.2byte	0x38
	.string	"wait"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x72
	.4byte	.L_T480
	.2byte	0x23
	.2byte	.L_l2363_e-.L_l2363
.L_l2363:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l2363_e:
.L_D2363_e:
.L_D2364:
	.4byte	.L_D2364_e-.L_D2364
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2365
	.2byte	0x38
	.string	"bread"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2364_e-.L_l2364
.L_l2364:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l2364_e:
.L_D2364_e:
.L_D2365:
	.4byte	.L_D2365_e-.L_D2365
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2366
	.2byte	0x38
	.string	"bwrite"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2365_e-.L_l2365
.L_l2365:
	.byte	0x4
	.4byte	0x24
	.byte	0x7
.L_l2365_e:
.L_D2365_e:
.L_D2366:
	.4byte	.L_D2366_e-.L_D2366
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2367
	.2byte	0x38
	.string	"lread"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2366_e-.L_l2366
.L_l2366:
	.byte	0x4
	.4byte	0x28
	.byte	0x7
.L_l2366_e:
.L_D2366_e:
.L_D2367:
	.4byte	.L_D2367_e-.L_D2367
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2368
	.2byte	0x38
	.string	"lwrite"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2367_e-.L_l2367
.L_l2367:
	.byte	0x4
	.4byte	0x2c
	.byte	0x7
.L_l2367_e:
.L_D2367_e:
.L_D2368:
	.4byte	.L_D2368_e-.L_D2368
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2369
	.2byte	0x38
	.string	"phread"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2368_e-.L_l2368
.L_l2368:
	.byte	0x4
	.4byte	0x30
	.byte	0x7
.L_l2368_e:
.L_D2368_e:
.L_D2369:
	.4byte	.L_D2369_e-.L_D2369
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2370
	.2byte	0x38
	.string	"phwrite"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2369_e-.L_l2369
.L_l2369:
	.byte	0x4
	.4byte	0x34
	.byte	0x7
.L_l2369_e:
.L_D2369_e:
.L_D2370:
	.4byte	.L_D2370_e-.L_D2370
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2371
	.2byte	0x38
	.string	"swapin"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2370_e-.L_l2370
.L_l2370:
	.byte	0x4
	.4byte	0x38
	.byte	0x7
.L_l2370_e:
.L_D2370_e:
.L_D2371:
	.4byte	.L_D2371_e-.L_D2371
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2372
	.2byte	0x38
	.string	"swapout"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2371_e-.L_l2371
.L_l2371:
	.byte	0x4
	.4byte	0x3c
	.byte	0x7
.L_l2371_e:
.L_D2371_e:
.L_D2372:
	.4byte	.L_D2372_e-.L_D2372
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2373
	.2byte	0x38
	.string	"bswapin"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2372_e-.L_l2372
.L_l2372:
	.byte	0x4
	.4byte	0x40
	.byte	0x7
.L_l2372_e:
.L_D2372_e:
.L_D2373:
	.4byte	.L_D2373_e-.L_D2373
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2374
	.2byte	0x38
	.string	"bswapout"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2373_e-.L_l2373
.L_l2373:
	.byte	0x4
	.4byte	0x44
	.byte	0x7
.L_l2373_e:
.L_D2373_e:
.L_D2374:
	.4byte	.L_D2374_e-.L_D2374
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2375
	.2byte	0x38
	.string	"pswitch"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2374_e-.L_l2374
.L_l2374:
	.byte	0x4
	.4byte	0x48
	.byte	0x7
.L_l2374_e:
.L_D2374_e:
.L_D2375:
	.4byte	.L_D2375_e-.L_D2375
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2376
	.2byte	0x38
	.string	"syscall"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2375_e-.L_l2375
.L_l2375:
	.byte	0x4
	.4byte	0x4c
	.byte	0x7
.L_l2375_e:
.L_D2375_e:
.L_D2376:
	.4byte	.L_D2376_e-.L_D2376
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2377
	.2byte	0x38
	.string	"sysread"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2376_e-.L_l2376
.L_l2376:
	.byte	0x4
	.4byte	0x50
	.byte	0x7
.L_l2376_e:
.L_D2376_e:
.L_D2377:
	.4byte	.L_D2377_e-.L_D2377
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2378
	.2byte	0x38
	.string	"syswrite"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2377_e-.L_l2377
.L_l2377:
	.byte	0x4
	.4byte	0x54
	.byte	0x7
.L_l2377_e:
.L_D2377_e:
.L_D2378:
	.4byte	.L_D2378_e-.L_D2378
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2379
	.2byte	0x38
	.string	"sysfork"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2378_e-.L_l2378
.L_l2378:
	.byte	0x4
	.4byte	0x58
	.byte	0x7
.L_l2378_e:
.L_D2378_e:
.L_D2379:
	.4byte	.L_D2379_e-.L_D2379
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2380
	.2byte	0x38
	.string	"sysexec"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2379_e-.L_l2379
.L_l2379:
	.byte	0x4
	.4byte	0x5c
	.byte	0x7
.L_l2379_e:
.L_D2379_e:
.L_D2380:
	.4byte	.L_D2380_e-.L_D2380
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2381
	.2byte	0x38
	.string	"runque"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2380_e-.L_l2380
.L_l2380:
	.byte	0x4
	.4byte	0x60
	.byte	0x7
.L_l2380_e:
.L_D2380_e:
.L_D2381:
	.4byte	.L_D2381_e-.L_D2381
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2382
	.2byte	0x38
	.string	"runocc"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2381_e-.L_l2381
.L_l2381:
	.byte	0x4
	.4byte	0x64
	.byte	0x7
.L_l2381_e:
.L_D2381_e:
.L_D2382:
	.4byte	.L_D2382_e-.L_D2382
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2383
	.2byte	0x38
	.string	"swpque"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2382_e-.L_l2382
.L_l2382:
	.byte	0x4
	.4byte	0x68
	.byte	0x7
.L_l2382_e:
.L_D2382_e:
.L_D2383:
	.4byte	.L_D2383_e-.L_D2383
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2384
	.2byte	0x38
	.string	"swpocc"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2383_e-.L_l2383
.L_l2383:
	.byte	0x4
	.4byte	0x6c
	.byte	0x7
.L_l2383_e:
.L_D2383_e:
.L_D2384:
	.4byte	.L_D2384_e-.L_D2384
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2385
	.2byte	0x38
	.string	"iget"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2384_e-.L_l2384
.L_l2384:
	.byte	0x4
	.4byte	0x70
	.byte	0x7
.L_l2384_e:
.L_D2384_e:
.L_D2385:
	.4byte	.L_D2385_e-.L_D2385
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2386
	.2byte	0x38
	.string	"namei"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2385_e-.L_l2385
.L_l2385:
	.byte	0x4
	.4byte	0x74
	.byte	0x7
.L_l2385_e:
.L_D2385_e:
.L_D2386:
	.4byte	.L_D2386_e-.L_D2386
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2387
	.2byte	0x38
	.string	"dirblk"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2386_e-.L_l2386
.L_l2386:
	.byte	0x4
	.4byte	0x78
	.byte	0x7
.L_l2386_e:
.L_D2386_e:
.L_D2387:
	.4byte	.L_D2387_e-.L_D2387
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2388
	.2byte	0x38
	.string	"readch"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2387_e-.L_l2387
.L_l2387:
	.byte	0x4
	.4byte	0x7c
	.byte	0x7
.L_l2387_e:
.L_D2387_e:
.L_D2388:
	.4byte	.L_D2388_e-.L_D2388
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2389
	.2byte	0x38
	.string	"writech"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2388_e-.L_l2388
.L_l2388:
	.byte	0x4
	.4byte	0x80
	.byte	0x7
.L_l2388_e:
.L_D2388_e:
.L_D2389:
	.4byte	.L_D2389_e-.L_D2389
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2390
	.2byte	0x38
	.string	"rcvint"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2389_e-.L_l2389
.L_l2389:
	.byte	0x4
	.4byte	0x84
	.byte	0x7
.L_l2389_e:
.L_D2389_e:
.L_D2390:
	.4byte	.L_D2390_e-.L_D2390
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2391
	.2byte	0x38
	.string	"xmtint"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2390_e-.L_l2390
.L_l2390:
	.byte	0x4
	.4byte	0x88
	.byte	0x7
.L_l2390_e:
.L_D2390_e:
.L_D2391:
	.4byte	.L_D2391_e-.L_D2391
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2392
	.2byte	0x38
	.string	"mdmint"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2391_e-.L_l2391
.L_l2391:
	.byte	0x4
	.4byte	0x8c
	.byte	0x7
.L_l2391_e:
.L_D2391_e:
.L_D2392:
	.4byte	.L_D2392_e-.L_D2392
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2393
	.2byte	0x38
	.string	"rawch"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2392_e-.L_l2392
.L_l2392:
	.byte	0x4
	.4byte	0x90
	.byte	0x7
.L_l2392_e:
.L_D2392_e:
.L_D2393:
	.4byte	.L_D2393_e-.L_D2393
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2394
	.2byte	0x38
	.string	"canch"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2393_e-.L_l2393
.L_l2393:
	.byte	0x4
	.4byte	0x94
	.byte	0x7
.L_l2393_e:
.L_D2393_e:
.L_D2394:
	.4byte	.L_D2394_e-.L_D2394
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2395
	.2byte	0x38
	.string	"outch"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2394_e-.L_l2394
.L_l2394:
	.byte	0x4
	.4byte	0x98
	.byte	0x7
.L_l2394_e:
.L_D2394_e:
.L_D2395:
	.4byte	.L_D2395_e-.L_D2395
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2396
	.2byte	0x38
	.string	"msg"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2395_e-.L_l2395
.L_l2395:
	.byte	0x4
	.4byte	0x9c
	.byte	0x7
.L_l2395_e:
.L_D2395_e:
.L_D2396:
	.4byte	.L_D2396_e-.L_D2396
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2397
	.2byte	0x38
	.string	"sema"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2396_e-.L_l2396
.L_l2396:
	.byte	0x4
	.4byte	0xa0
	.byte	0x7
.L_l2396_e:
.L_D2396_e:
.L_D2397:
	.4byte	.L_D2397_e-.L_D2397
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2398
	.2byte	0x38
	.string	"pnpfault"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2397_e-.L_l2397
.L_l2397:
	.byte	0x4
	.4byte	0xa4
	.byte	0x7
.L_l2397_e:
.L_D2397_e:
.L_D2398:
	.4byte	.L_D2398_e-.L_D2398
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2399
	.2byte	0x38
	.string	"wrtfault"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2398_e-.L_l2398
.L_l2398:
	.byte	0x4
	.4byte	0xa8
	.byte	0x7
.L_l2398_e:
.L_D2398_e:
.L_D2399:
	.4byte	.L_D2399_e-.L_D2399
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2400
	.2byte	0x38
	.string	"s5ipage"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2399_e-.L_l2399
.L_l2399:
	.byte	0x4
	.4byte	0xac
	.byte	0x7
.L_l2399_e:
.L_D2399_e:
.L_D2400:
	.4byte	.L_D2400_e-.L_D2400
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2401
	.2byte	0x38
	.string	"s5inopage"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2400_e-.L_l2400
.L_l2400:
	.byte	0x4
	.4byte	0xb0
	.byte	0x7
.L_l2400_e:
.L_D2400_e:
.L_D2401:
	.4byte	.L_D2401_e-.L_D2401
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2402
	.2byte	0x38
	.string	"ufsipage"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2401_e-.L_l2401
.L_l2401:
	.byte	0x4
	.4byte	0xb4
	.byte	0x7
.L_l2401_e:
.L_D2401_e:
.L_D2402:
	.4byte	.L_D2402_e-.L_D2402
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2403
	.2byte	0x38
	.string	"ufsinopage"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2402_e-.L_l2402
.L_l2402:
	.byte	0x4
	.4byte	0xb8
	.byte	0x7
.L_l2402_e:
.L_D2402_e:
.L_D2403:
	.4byte	.L_D2403_e-.L_D2403
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2404
	.2byte	0x38
	.string	"sfsipage"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2403_e-.L_l2403
.L_l2403:
	.byte	0x4
	.4byte	0xbc
	.byte	0x7
.L_l2403_e:
.L_D2403_e:
.L_D2404:
	.4byte	.L_D2404_e-.L_D2404
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2405
	.2byte	0x38
	.string	"sfsinopage"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2404_e-.L_l2404
.L_l2404:
	.byte	0x4
	.4byte	0xc0
	.byte	0x7
.L_l2404_e:
.L_D2404_e:
.L_D2405:
	.4byte	.L_D2405_e-.L_D2405
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2406
	.2byte	0x38
	.string	"xxipage"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2405_e-.L_l2405
.L_l2405:
	.byte	0x4
	.4byte	0xc4
	.byte	0x7
.L_l2405_e:
.L_D2405_e:
.L_D2406:
	.4byte	.L_D2406_e-.L_D2406
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2407
	.2byte	0x38
	.string	"xxinopage"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2406_e-.L_l2406
.L_l2406:
	.byte	0x4
	.4byte	0xc8
	.byte	0x7
.L_l2406_e:
.L_D2406_e:
.L_D2407:
	.4byte	.L_D2407_e-.L_D2407
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2408
	.2byte	0x38
	.string	"vxfsipage"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2407_e-.L_l2407
.L_l2407:
	.byte	0x4
	.4byte	0xcc
	.byte	0x7
.L_l2407_e:
.L_D2407_e:
.L_D2408:
	.4byte	.L_D2408_e-.L_D2408
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2409
	.2byte	0x38
	.string	"vxfsinopage"
	.2byte	0x142
	.4byte	.L_T475
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2408_e-.L_l2408
.L_l2408:
	.byte	0x4
	.4byte	0xd0
	.byte	0x7
.L_l2408_e:
.L_D2408_e:
.L_D2409:
	.4byte	0x4
.L_D2361:
	.4byte	.L_D2361_e-.L_D2361
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2410
	.set	.L_T478,.L_D2361
	.2byte	0xa3
	.2byte	.L_s2361_e-.L_s2361
.L_s2361:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x4
	.byte	0x8
	.2byte	0x55
	.2byte	0x7
.L_s2361_e:
.L_D2361_e:
.L_D2410:
	.4byte	.L_D2410_e-.L_D2410
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2411
	.set	.L_T480,.L_D2410
	.2byte	0xa3
	.2byte	.L_s2410_e-.L_s2410
.L_s2410:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x2
	.byte	0x8
	.2byte	0x55
	.2byte	0x7
.L_s2410_e:
.L_D2410_e:
	.previous

.section	.debug
.L_D2411:
	.4byte	.L_D2411_e-.L_D2411
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2412
	.set	.L_T481,.L_D2411
	.2byte	0x38
	.string	"syswait"
	.2byte	0xb6
	.4byte	0x6
.L_D2411_e:
.L_D2413:
	.4byte	.L_D2413_e-.L_D2413
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2414
	.2byte	0x38
	.string	"iowait"
	.2byte	0x142
	.4byte	.L_T481
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2413_e-.L_l2413
.L_l2413:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2413_e:
.L_D2413_e:
.L_D2414:
	.4byte	.L_D2414_e-.L_D2414
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2415
	.2byte	0x38
	.string	"swap"
	.2byte	0x142
	.4byte	.L_T481
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2414_e-.L_l2414
.L_l2414:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l2414_e:
.L_D2414_e:
.L_D2415:
	.4byte	.L_D2415_e-.L_D2415
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2416
	.2byte	0x38
	.string	"physio"
	.2byte	0x142
	.4byte	.L_T481
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2415_e-.L_l2415
.L_l2415:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2415_e:
.L_D2415_e:
.L_D2416:
	.4byte	0x4
	.previous

.section	.debug
.L_D2412:
	.4byte	.L_D2412_e-.L_D2412
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2417
	.set	.L_T482,.L_D2412
	.2byte	0x38
	.string	"minfo"
	.2byte	0xb6
	.4byte	0x5c
.L_D2412_e:
.L_D2418:
	.4byte	.L_D2418_e-.L_D2418
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2419
	.2byte	0x38
	.string	"mi_freemem"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x72
	.4byte	.L_T484
	.2byte	0x23
	.2byte	.L_l2418_e-.L_l2418
.L_l2418:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2418_e:
.L_D2418_e:
.L_D2419:
	.4byte	.L_D2419_e-.L_D2419
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2420
	.2byte	0x38
	.string	"freeswap"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2419_e-.L_l2419
.L_l2419:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2419_e:
.L_D2419_e:
.L_D2420:
	.4byte	.L_D2420_e-.L_D2420
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2421
	.2byte	0x38
	.string	"vfault"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2420_e-.L_l2420
.L_l2420:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2420_e:
.L_D2420_e:
.L_D2421:
	.4byte	.L_D2421_e-.L_D2421
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2422
	.2byte	0x38
	.string	"demand"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2421_e-.L_l2421
.L_l2421:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l2421_e:
.L_D2421_e:
.L_D2422:
	.4byte	.L_D2422_e-.L_D2422
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2423
	.2byte	0x38
	.string	"swap"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2422_e-.L_l2422
.L_l2422:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l2422_e:
.L_D2422_e:
.L_D2423:
	.4byte	.L_D2423_e-.L_D2423
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2424
	.2byte	0x38
	.string	"cache"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2423_e-.L_l2423
.L_l2423:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l2423_e:
.L_D2423_e:
.L_D2424:
	.4byte	.L_D2424_e-.L_D2424
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2425
	.2byte	0x38
	.string	"file"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2424_e-.L_l2424
.L_l2424:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l2424_e:
.L_D2424_e:
.L_D2425:
	.4byte	.L_D2425_e-.L_D2425
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2426
	.2byte	0x38
	.string	"pfault"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2425_e-.L_l2425
.L_l2425:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l2425_e:
.L_D2425_e:
.L_D2426:
	.4byte	.L_D2426_e-.L_D2426
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2427
	.2byte	0x38
	.string	"cw"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2426_e-.L_l2426
.L_l2426:
	.byte	0x4
	.4byte	0x24
	.byte	0x7
.L_l2426_e:
.L_D2426_e:
.L_D2427:
	.4byte	.L_D2427_e-.L_D2427
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2428
	.2byte	0x38
	.string	"steal"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2427_e-.L_l2427
.L_l2427:
	.byte	0x4
	.4byte	0x28
	.byte	0x7
.L_l2427_e:
.L_D2427_e:
.L_D2428:
	.4byte	.L_D2428_e-.L_D2428
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2429
	.2byte	0x38
	.string	"freedpgs"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2428_e-.L_l2428
.L_l2428:
	.byte	0x4
	.4byte	0x2c
	.byte	0x7
.L_l2428_e:
.L_D2428_e:
.L_D2429:
	.4byte	.L_D2429_e-.L_D2429
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2430
	.2byte	0x38
	.string	"vfpg"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2429_e-.L_l2429
.L_l2429:
	.byte	0x4
	.4byte	0x30
	.byte	0x7
.L_l2429_e:
.L_D2429_e:
.L_D2430:
	.4byte	.L_D2430_e-.L_D2430
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2431
	.2byte	0x38
	.string	"sfpg"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2430_e-.L_l2430
.L_l2430:
	.byte	0x4
	.4byte	0x34
	.byte	0x7
.L_l2430_e:
.L_D2430_e:
.L_D2431:
	.4byte	.L_D2431_e-.L_D2431
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2432
	.2byte	0x38
	.string	"vspg"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2431_e-.L_l2431
.L_l2431:
	.byte	0x4
	.4byte	0x38
	.byte	0x7
.L_l2431_e:
.L_D2431_e:
.L_D2432:
	.4byte	.L_D2432_e-.L_D2432
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2433
	.2byte	0x38
	.string	"sspg"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2432_e-.L_l2432
.L_l2432:
	.byte	0x4
	.4byte	0x3c
	.byte	0x7
.L_l2432_e:
.L_D2432_e:
.L_D2433:
	.4byte	.L_D2433_e-.L_D2433
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2434
	.2byte	0x38
	.string	"unmodsw"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2433_e-.L_l2433
.L_l2433:
	.byte	0x4
	.4byte	0x40
	.byte	0x7
.L_l2433_e:
.L_D2433_e:
.L_D2434:
	.4byte	.L_D2434_e-.L_D2434
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2435
	.2byte	0x38
	.string	"unmodfl"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2434_e-.L_l2434
.L_l2434:
	.byte	0x4
	.4byte	0x44
	.byte	0x7
.L_l2434_e:
.L_D2434_e:
.L_D2435:
	.4byte	.L_D2435_e-.L_D2435
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2436
	.2byte	0x38
	.string	"psoutok"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2435_e-.L_l2435
.L_l2435:
	.byte	0x4
	.4byte	0x48
	.byte	0x7
.L_l2435_e:
.L_D2435_e:
.L_D2436:
	.4byte	.L_D2436_e-.L_D2436
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2437
	.2byte	0x38
	.string	"psinfail"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2436_e-.L_l2436
.L_l2436:
	.byte	0x4
	.4byte	0x4c
	.byte	0x7
.L_l2436_e:
.L_D2436_e:
.L_D2437:
	.4byte	.L_D2437_e-.L_D2437
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2438
	.2byte	0x38
	.string	"psinok"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2437_e-.L_l2437
.L_l2437:
	.byte	0x4
	.4byte	0x50
	.byte	0x7
.L_l2437_e:
.L_D2437_e:
.L_D2438:
	.4byte	.L_D2438_e-.L_D2438
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2439
	.2byte	0x38
	.string	"rsout"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2438_e-.L_l2438
.L_l2438:
	.byte	0x4
	.4byte	0x54
	.byte	0x7
.L_l2438_e:
.L_D2438_e:
.L_D2439:
	.4byte	.L_D2439_e-.L_D2439
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2440
	.2byte	0x38
	.string	"rsin"
	.2byte	0x142
	.4byte	.L_T482
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2439_e-.L_l2439
.L_l2439:
	.byte	0x4
	.4byte	0x58
	.byte	0x7
.L_l2439_e:
.L_D2439_e:
.L_D2440:
	.4byte	0x4
.L_D2417:
	.4byte	.L_D2417_e-.L_D2417
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2441
	.set	.L_T484,.L_D2417
	.2byte	0xa3
	.2byte	.L_s2417_e-.L_s2417
.L_s2417:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x1
	.byte	0x8
	.2byte	0x55
	.2byte	0xc
.L_s2417_e:
.L_D2417_e:
	.previous

.section	.debug
.L_D2441:
	.4byte	.L_D2441_e-.L_D2441
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2442
	.set	.L_T485,.L_D2441
	.2byte	0x38
	.string	"fsinfo"
	.2byte	0xb6
	.4byte	0x30
.L_D2441_e:
.L_D2443:
	.4byte	.L_D2443_e-.L_D2443
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2444
	.2byte	0x38
	.string	"fsireadch"
	.2byte	0x142
	.4byte	.L_T485
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2443_e-.L_l2443
.L_l2443:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2443_e:
.L_D2443_e:
.L_D2444:
	.4byte	.L_D2444_e-.L_D2444
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2445
	.2byte	0x38
	.string	"fsiwritech"
	.2byte	0x142
	.4byte	.L_T485
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2444_e-.L_l2444
.L_l2444:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2444_e:
.L_D2444_e:
.L_D2445:
	.4byte	.L_D2445_e-.L_D2445
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2446
	.2byte	0x38
	.string	"fsivop_open"
	.2byte	0x142
	.4byte	.L_T485
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2445_e-.L_l2445
.L_l2445:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2445_e:
.L_D2445_e:
.L_D2446:
	.4byte	.L_D2446_e-.L_D2446
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2447
	.2byte	0x38
	.string	"fsivop_close"
	.2byte	0x142
	.4byte	.L_T485
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2446_e-.L_l2446
.L_l2446:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2446_e:
.L_D2446_e:
.L_D2447:
	.4byte	.L_D2447_e-.L_D2447
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2448
	.2byte	0x38
	.string	"fsivop_read"
	.2byte	0x142
	.4byte	.L_T485
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2447_e-.L_l2447
.L_l2447:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l2447_e:
.L_D2447_e:
.L_D2448:
	.4byte	.L_D2448_e-.L_D2448
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2449
	.2byte	0x38
	.string	"fsivop_write"
	.2byte	0x142
	.4byte	.L_T485
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2448_e-.L_l2448
.L_l2448:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l2448_e:
.L_D2448_e:
.L_D2449:
	.4byte	.L_D2449_e-.L_D2449
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2450
	.2byte	0x38
	.string	"fsivop_lookup"
	.2byte	0x142
	.4byte	.L_T485
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2449_e-.L_l2449
.L_l2449:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l2449_e:
.L_D2449_e:
.L_D2450:
	.4byte	.L_D2450_e-.L_D2450
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2451
	.2byte	0x38
	.string	"fsivop_create"
	.2byte	0x142
	.4byte	.L_T485
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2450_e-.L_l2450
.L_l2450:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l2450_e:
.L_D2450_e:
.L_D2451:
	.4byte	.L_D2451_e-.L_D2451
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2452
	.2byte	0x38
	.string	"fsivop_readdir"
	.2byte	0x142
	.4byte	.L_T485
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2451_e-.L_l2451
.L_l2451:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l2451_e:
.L_D2451_e:
.L_D2452:
	.4byte	.L_D2452_e-.L_D2452
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2453
	.2byte	0x38
	.string	"fsivop_getpage"
	.2byte	0x142
	.4byte	.L_T485
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2452_e-.L_l2452
.L_l2452:
	.byte	0x4
	.4byte	0x24
	.byte	0x7
.L_l2452_e:
.L_D2452_e:
.L_D2453:
	.4byte	.L_D2453_e-.L_D2453
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2454
	.2byte	0x38
	.string	"fsivop_putpage"
	.2byte	0x142
	.4byte	.L_T485
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2453_e-.L_l2453
.L_l2453:
	.byte	0x4
	.4byte	0x28
	.byte	0x7
.L_l2453_e:
.L_D2453_e:
.L_D2454:
	.4byte	.L_D2454_e-.L_D2454
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2455
	.2byte	0x38
	.string	"fsivop_other"
	.2byte	0x142
	.4byte	.L_T485
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2454_e-.L_l2454
.L_l2454:
	.byte	0x4
	.4byte	0x2c
	.byte	0x7
.L_l2454_e:
.L_D2454_e:
.L_D2455:
	.4byte	0x4
	.previous

.section	.debug
.L_D2442:
	.4byte	.L_D2442_e-.L_D2442
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2456
	.2byte	0x38
	.string	"fsinfo_t"
	.2byte	0x72
	.4byte	.L_T485
.L_D2442_e:
	.previous

.section	.debug
.L_D2456:
	.4byte	.L_D2456_e-.L_D2456
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2457
	.set	.L_T487,.L_D2456
	.2byte	0x38
	.string	"vminfo"
	.2byte	0xb6
	.4byte	0x40
.L_D2456_e:
.L_D2458:
	.4byte	.L_D2458_e-.L_D2458
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2459
	.2byte	0x38
	.string	"v_pgrec"
	.2byte	0x142
	.4byte	.L_T487
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2458_e-.L_l2458
.L_l2458:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2458_e:
.L_D2458_e:
.L_D2459:
	.4byte	.L_D2459_e-.L_D2459
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2460
	.2byte	0x38
	.string	"v_xsfrec"
	.2byte	0x142
	.4byte	.L_T487
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2459_e-.L_l2459
.L_l2459:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2459_e:
.L_D2459_e:
.L_D2460:
	.4byte	.L_D2460_e-.L_D2460
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2461
	.2byte	0x38
	.string	"v_xifrec"
	.2byte	0x142
	.4byte	.L_T487
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2460_e-.L_l2460
.L_l2460:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2460_e:
.L_D2460_e:
.L_D2461:
	.4byte	.L_D2461_e-.L_D2461
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2462
	.2byte	0x38
	.string	"v_pgin"
	.2byte	0x142
	.4byte	.L_T487
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2461_e-.L_l2461
.L_l2461:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2461_e:
.L_D2461_e:
.L_D2462:
	.4byte	.L_D2462_e-.L_D2462
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2463
	.2byte	0x38
	.string	"v_pgpgin"
	.2byte	0x142
	.4byte	.L_T487
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2462_e-.L_l2462
.L_l2462:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l2462_e:
.L_D2462_e:
.L_D2463:
	.4byte	.L_D2463_e-.L_D2463
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2464
	.2byte	0x38
	.string	"v_pgout"
	.2byte	0x142
	.4byte	.L_T487
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2463_e-.L_l2463
.L_l2463:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l2463_e:
.L_D2463_e:
.L_D2464:
	.4byte	.L_D2464_e-.L_D2464
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2465
	.2byte	0x38
	.string	"v_pgpgout"
	.2byte	0x142
	.4byte	.L_T487
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2464_e-.L_l2464
.L_l2464:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l2464_e:
.L_D2464_e:
.L_D2465:
	.4byte	.L_D2465_e-.L_D2465
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2466
	.2byte	0x38
	.string	"v_swpout"
	.2byte	0x142
	.4byte	.L_T487
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2465_e-.L_l2465
.L_l2465:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l2465_e:
.L_D2465_e:
.L_D2466:
	.4byte	.L_D2466_e-.L_D2466
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2467
	.2byte	0x38
	.string	"v_pswpout"
	.2byte	0x142
	.4byte	.L_T487
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2466_e-.L_l2466
.L_l2466:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l2466_e:
.L_D2466_e:
.L_D2467:
	.4byte	.L_D2467_e-.L_D2467
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2468
	.2byte	0x38
	.string	"v_swpin"
	.2byte	0x142
	.4byte	.L_T487
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2467_e-.L_l2467
.L_l2467:
	.byte	0x4
	.4byte	0x24
	.byte	0x7
.L_l2467_e:
.L_D2467_e:
.L_D2468:
	.4byte	.L_D2468_e-.L_D2468
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2469
	.2byte	0x38
	.string	"v_pswpin"
	.2byte	0x142
	.4byte	.L_T487
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2468_e-.L_l2468
.L_l2468:
	.byte	0x4
	.4byte	0x28
	.byte	0x7
.L_l2468_e:
.L_D2468_e:
.L_D2469:
	.4byte	.L_D2469_e-.L_D2469
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2470
	.2byte	0x38
	.string	"v_dfree"
	.2byte	0x142
	.4byte	.L_T487
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2469_e-.L_l2469
.L_l2469:
	.byte	0x4
	.4byte	0x2c
	.byte	0x7
.L_l2469_e:
.L_D2469_e:
.L_D2470:
	.4byte	.L_D2470_e-.L_D2470
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2471
	.2byte	0x38
	.string	"v_scan"
	.2byte	0x142
	.4byte	.L_T487
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2470_e-.L_l2470
.L_l2470:
	.byte	0x4
	.4byte	0x30
	.byte	0x7
.L_l2470_e:
.L_D2470_e:
.L_D2471:
	.4byte	.L_D2471_e-.L_D2471
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2472
	.2byte	0x38
	.string	"v_pfault"
	.2byte	0x142
	.4byte	.L_T487
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2471_e-.L_l2471
.L_l2471:
	.byte	0x4
	.4byte	0x34
	.byte	0x7
.L_l2471_e:
.L_D2471_e:
.L_D2472:
	.4byte	.L_D2472_e-.L_D2472
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2473
	.2byte	0x38
	.string	"v_vfault"
	.2byte	0x142
	.4byte	.L_T487
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2472_e-.L_l2472
.L_l2472:
	.byte	0x4
	.4byte	0x38
	.byte	0x7
.L_l2472_e:
.L_D2472_e:
.L_D2473:
	.4byte	.L_D2473_e-.L_D2473
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2474
	.2byte	0x38
	.string	"v_sftlock"
	.2byte	0x142
	.4byte	.L_T487
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2473_e-.L_l2473
.L_l2473:
	.byte	0x4
	.4byte	0x3c
	.byte	0x7
.L_l2473_e:
.L_D2473_e:
.L_D2474:
	.4byte	0x4
	.previous

.section	.debug
.L_D2457:
	.4byte	.L_D2457_e-.L_D2457
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2475
	.set	.L_T488,.L_D2457
	.2byte	0x38
	.string	"syserr"
	.2byte	0xb6
	.4byte	0x10
.L_D2457_e:
.L_D2476:
	.4byte	.L_D2476_e-.L_D2476
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2477
	.2byte	0x38
	.string	"inodeovf"
	.2byte	0x142
	.4byte	.L_T488
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2476_e-.L_l2476
.L_l2476:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2476_e:
.L_D2476_e:
.L_D2477:
	.4byte	.L_D2477_e-.L_D2477
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2478
	.2byte	0x38
	.string	"fileovf"
	.2byte	0x142
	.4byte	.L_T488
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2477_e-.L_l2477
.L_l2477:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2477_e:
.L_D2477_e:
.L_D2478:
	.4byte	.L_D2478_e-.L_D2478
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2479
	.2byte	0x38
	.string	"textovf"
	.2byte	0x142
	.4byte	.L_T488
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2478_e-.L_l2478
.L_l2478:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2478_e:
.L_D2478_e:
.L_D2479:
	.4byte	.L_D2479_e-.L_D2479
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2480
	.2byte	0x38
	.string	"procovf"
	.2byte	0x142
	.4byte	.L_T488
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2479_e-.L_l2479
.L_l2479:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2479_e:
.L_D2479_e:
.L_D2480:
	.4byte	0x4
	.previous

.section	.debug
.L_D2475:
	.4byte	.L_D2475_e-.L_D2475
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2481
	.set	.L_T489,.L_D2475
	.2byte	0x38
	.string	"shlbinfo"
	.2byte	0xb6
	.4byte	0x10
.L_D2475_e:
.L_D2482:
	.4byte	.L_D2482_e-.L_D2482
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2483
	.2byte	0x38
	.string	"shlbs"
	.2byte	0x142
	.4byte	.L_T489
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2482_e-.L_l2482
.L_l2482:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2482_e:
.L_D2482_e:
.L_D2483:
	.4byte	.L_D2483_e-.L_D2483
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2484
	.2byte	0x38
	.string	"shlblnks"
	.2byte	0x142
	.4byte	.L_T489
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2483_e-.L_l2483
.L_l2483:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2483_e:
.L_D2483_e:
.L_D2484:
	.4byte	.L_D2484_e-.L_D2484
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2485
	.2byte	0x38
	.string	"shlbovf"
	.2byte	0x142
	.4byte	.L_T489
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2484_e-.L_l2484
.L_l2484:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2484_e:
.L_D2484_e:
.L_D2485:
	.4byte	.L_D2485_e-.L_D2485
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2486
	.2byte	0x38
	.string	"shlbatts"
	.2byte	0x142
	.4byte	.L_T489
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2485_e-.L_l2485
.L_l2485:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2485_e:
.L_D2485_e:
.L_D2486:
	.4byte	0x4
	.previous

.section	.debug
.L_D2481:
	.4byte	.L_D2481_e-.L_D2481
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2487
	.set	.L_T490,.L_D2481
	.2byte	0x38
	.string	"bpbinfo"
	.2byte	0xb6
	.4byte	0x10
.L_D2481_e:
.L_D2488:
	.4byte	.L_D2488_e-.L_D2488
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2489
	.2byte	0x38
	.string	"usr"
	.2byte	0x142
	.4byte	.L_T490
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2488_e-.L_l2488
.L_l2488:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2488_e:
.L_D2488_e:
.L_D2489:
	.4byte	.L_D2489_e-.L_D2489
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2490
	.2byte	0x38
	.string	"sys"
	.2byte	0x142
	.4byte	.L_T490
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2489_e-.L_l2489
.L_l2489:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2489_e:
.L_D2489_e:
.L_D2490:
	.4byte	.L_D2490_e-.L_D2490
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2491
	.2byte	0x38
	.string	"idle"
	.2byte	0x142
	.4byte	.L_T490
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2490_e-.L_l2490
.L_l2490:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2490_e:
.L_D2490_e:
.L_D2491:
	.4byte	.L_D2491_e-.L_D2491
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2492
	.2byte	0x38
	.string	"syscall"
	.2byte	0x142
	.4byte	.L_T490
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2491_e-.L_l2491
.L_l2491:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2491_e:
.L_D2491_e:
.L_D2492:
	.4byte	0x4
	.previous

.section	.debug
.L_D2487:
	.4byte	.L_D2487_e-.L_D2487
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2493
	.set	.L_T491,.L_D2487
	.2byte	0x38
	.string	"kmeminfo"
	.2byte	0xb6
	.4byte	0x24
.L_D2487_e:
.L_D2494:
	.4byte	.L_D2494_e-.L_D2494
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2495
	.2byte	0x38
	.string	"km_mem"
	.2byte	0x142
	.4byte	.L_T491
	.2byte	0x72
	.4byte	.L_T494
	.2byte	0x23
	.2byte	.L_l2494_e-.L_l2494
.L_l2494:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2494_e:
.L_D2494_e:
.L_D2495:
	.4byte	.L_D2495_e-.L_D2495
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2496
	.2byte	0x38
	.string	"km_alloc"
	.2byte	0x142
	.4byte	.L_T491
	.2byte	0x72
	.4byte	.L_T494
	.2byte	0x23
	.2byte	.L_l2495_e-.L_l2495
.L_l2495:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2495_e:
.L_D2495_e:
.L_D2496:
	.4byte	.L_D2496_e-.L_D2496
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2497
	.2byte	0x38
	.string	"km_fail"
	.2byte	0x142
	.4byte	.L_T491
	.2byte	0x72
	.4byte	.L_T494
	.2byte	0x23
	.2byte	.L_l2496_e-.L_l2496
.L_l2496:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l2496_e:
.L_D2496_e:
.L_D2497:
	.4byte	0x4
.L_D2493:
	.4byte	.L_D2493_e-.L_D2493
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2498
	.set	.L_T494,.L_D2493
	.2byte	0xa3
	.2byte	.L_s2493_e-.L_s2493
.L_s2493:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x2
	.byte	0x8
	.2byte	0x55
	.2byte	0x9
.L_s2493_e:
.L_D2493_e:
	.previous

.section	.debug
.L_P12:
.L_D2498:
	.4byte	.L_D2498_e-.L_D2498
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D2499
	.2byte	0x38
	.string	"sysinfo"
	.2byte	0x72
	.4byte	.L_T475
.L_D2498_e:
	.previous

.section	.debug
.L_D2499:
	.4byte	.L_D2499_e-.L_D2499
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2500
	.2byte	0x38
	.string	"file_t"
	.2byte	0x72
	.4byte	.L_T354
.L_D2499_e:
	.previous

.section	.debug
.L_D2500:
	.4byte	.L_D2500_e-.L_D2500
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2501
	.2byte	0x38
	.string	"tcflag_t"
	.2byte	0x55
	.2byte	0x9
.L_D2500_e:
	.previous

.section	.debug
.L_D2501:
	.4byte	.L_D2501_e-.L_D2501
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2502
	.2byte	0x38
	.string	"cc_t"
	.2byte	0x55
	.2byte	0x3
.L_D2501_e:
	.previous

.section	.debug
.L_D2502:
	.4byte	.L_D2502_e-.L_D2502
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2503
	.2byte	0x38
	.string	"speed_t"
	.2byte	0x55
	.2byte	0x9
.L_D2502_e:
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2503:
	.4byte	.L_D2503_e-.L_D2503
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2504
	.set	.L_T523,.L_D2503
	.2byte	0x38
	.string	"winsize"
	.2byte	0xb6
	.4byte	0x8
.L_D2503_e:
.L_D2505:
	.4byte	.L_D2505_e-.L_D2505
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2506
	.2byte	0x38
	.string	"ws_row"
	.2byte	0x142
	.4byte	.L_T523
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2505_e-.L_l2505
.L_l2505:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2505_e:
.L_D2505_e:
.L_D2506:
	.4byte	.L_D2506_e-.L_D2506
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2507
	.2byte	0x38
	.string	"ws_col"
	.2byte	0x142
	.4byte	.L_T523
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2506_e-.L_l2506
.L_l2506:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l2506_e:
.L_D2506_e:
.L_D2507:
	.4byte	.L_D2507_e-.L_D2507
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2508
	.2byte	0x38
	.string	"ws_xpixel"
	.2byte	0x142
	.4byte	.L_T523
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2507_e-.L_l2507
.L_l2507:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2507_e:
.L_D2507_e:
.L_D2508:
	.4byte	.L_D2508_e-.L_D2508
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2509
	.2byte	0x38
	.string	"ws_ypixel"
	.2byte	0x142
	.4byte	.L_T523
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2508_e-.L_l2508
.L_l2508:
	.byte	0x4
	.4byte	0x6
	.byte	0x7
.L_l2508_e:
.L_D2508_e:
.L_D2509:
	.4byte	0x4
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2504:
	.4byte	.L_D2504_e-.L_D2504
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2510
	.set	.L_T528,.L_D2504
	.2byte	0x38
	.string	"termcb"
	.2byte	0xb6
	.4byte	0x6
.L_D2504_e:
.L_D2511:
	.4byte	.L_D2511_e-.L_D2511
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2512
	.2byte	0x38
	.string	"st_flgs"
	.2byte	0x142
	.4byte	.L_T528
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2511_e-.L_l2511
.L_l2511:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2511_e:
.L_D2511_e:
.L_D2512:
	.4byte	.L_D2512_e-.L_D2512
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2513
	.2byte	0x38
	.string	"st_termt"
	.2byte	0x142
	.4byte	.L_T528
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2512_e-.L_l2512
.L_l2512:
	.byte	0x4
	.4byte	0x1
	.byte	0x7
.L_l2512_e:
.L_D2512_e:
.L_D2513:
	.4byte	.L_D2513_e-.L_D2513
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2514
	.2byte	0x38
	.string	"st_crow"
	.2byte	0x142
	.4byte	.L_T528
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2513_e-.L_l2513
.L_l2513:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l2513_e:
.L_D2513_e:
.L_D2514:
	.4byte	.L_D2514_e-.L_D2514
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2515
	.2byte	0x38
	.string	"st_ccol"
	.2byte	0x142
	.4byte	.L_T528
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2514_e-.L_l2514
.L_l2514:
	.byte	0x4
	.4byte	0x3
	.byte	0x7
.L_l2514_e:
.L_D2514_e:
.L_D2515:
	.4byte	.L_D2515_e-.L_D2515
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2516
	.2byte	0x38
	.string	"st_vrow"
	.2byte	0x142
	.4byte	.L_T528
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2515_e-.L_l2515
.L_l2515:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2515_e:
.L_D2515_e:
.L_D2516:
	.4byte	.L_D2516_e-.L_D2516
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2517
	.2byte	0x38
	.string	"st_lrow"
	.2byte	0x142
	.4byte	.L_T528
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2516_e-.L_l2516
.L_l2516:
	.byte	0x4
	.4byte	0x5
	.byte	0x7
.L_l2516_e:
.L_D2516_e:
.L_D2517:
	.4byte	0x4
	.previous

.section	.debug
.L_D2510:
	.4byte	.L_D2510_e-.L_D2510
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2518
	.set	.L_T529,.L_D2510
	.2byte	0x38
	.string	"termss"
	.2byte	0xb6
	.4byte	0x2
.L_D2510_e:
.L_D2519:
	.4byte	.L_D2519_e-.L_D2519
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2520
	.2byte	0x38
	.string	"ss_start"
	.2byte	0x142
	.4byte	.L_T529
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2519_e-.L_l2519
.L_l2519:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2519_e:
.L_D2519_e:
.L_D2520:
	.4byte	.L_D2520_e-.L_D2520
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2521
	.2byte	0x38
	.string	"ss_stop"
	.2byte	0x142
	.4byte	.L_T529
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2520_e-.L_l2520
.L_l2520:
	.byte	0x4
	.4byte	0x1
	.byte	0x7
.L_l2520_e:
.L_D2520_e:
.L_D2521:
	.4byte	0x4
	.previous

.section	.debug
.L_D2518:
	.4byte	.L_D2518_e-.L_D2518
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D2522
	.set	.L_T530,.L_D2518
	.2byte	0x38
	.string	"ioctl_arg"
	.2byte	0xb6
	.4byte	0x4
.L_D2518_e:
.L_D2523:
	.4byte	.L_D2523_e-.L_D2523
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2524
	.2byte	0x38
	.string	"stparg"
	.2byte	0x142
	.4byte	.L_T530
	.2byte	0x83
	.2byte	.L_t2523_e-.L_t2523
.L_t2523:
	.byte	0x1
	.4byte	.L_T524
.L_t2523_e:
	.2byte	0x23
	.2byte	.L_l2523_e-.L_l2523
.L_l2523:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2523_e:
.L_D2523_e:
.L_D2524:
	.4byte	.L_D2524_e-.L_D2524
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2525
	.2byte	0x38
	.string	"sparg"
	.2byte	0x142
	.4byte	.L_T530
	.2byte	0x83
	.2byte	.L_t2524_e-.L_t2524
.L_t2524:
	.byte	0x1
	.4byte	.L_T532
.L_t2524_e:
	.2byte	0x23
	.2byte	.L_l2524_e-.L_l2524
.L_l2524:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2524_e:
.L_D2524_e:
.L_D2525:
	.4byte	.L_D2525_e-.L_D2525
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2526
	.2byte	0x38
	.string	"cparg"
	.2byte	0x142
	.4byte	.L_T530
	.2byte	0x63
	.2byte	.L_t2525_e-.L_t2525
.L_t2525:
	.byte	0x1
	.2byte	0x1
.L_t2525_e:
	.2byte	0x23
	.2byte	.L_l2525_e-.L_l2525
.L_l2525:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2525_e:
.L_D2525_e:
.L_D2526:
	.4byte	.L_D2526_e-.L_D2526
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2527
	.2byte	0x38
	.string	"carg"
	.2byte	0x142
	.4byte	.L_T530
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2526_e-.L_l2526
.L_l2526:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2526_e:
.L_D2526_e:
.L_D2527:
	.4byte	.L_D2527_e-.L_D2527
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2528
	.2byte	0x38
	.string	"iparg"
	.2byte	0x142
	.4byte	.L_T530
	.2byte	0x63
	.2byte	.L_t2527_e-.L_t2527
.L_t2527:
	.byte	0x1
	.2byte	0x7
.L_t2527_e:
	.2byte	0x23
	.2byte	.L_l2527_e-.L_l2527
.L_l2527:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2527_e:
.L_D2527_e:
.L_D2528:
	.4byte	.L_D2528_e-.L_D2528
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2529
	.2byte	0x38
	.string	"iarg"
	.2byte	0x142
	.4byte	.L_T530
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2528_e-.L_l2528
.L_l2528:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2528_e:
.L_D2528_e:
.L_D2529:
	.4byte	.L_D2529_e-.L_D2529
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2530
	.2byte	0x38
	.string	"lparg"
	.2byte	0x142
	.4byte	.L_T530
	.2byte	0x63
	.2byte	.L_t2529_e-.L_t2529
.L_t2529:
	.byte	0x1
	.2byte	0xa
.L_t2529_e:
	.2byte	0x23
	.2byte	.L_l2529_e-.L_l2529
.L_l2529:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2529_e:
.L_D2529_e:
.L_D2530:
	.4byte	.L_D2530_e-.L_D2530
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2531
	.2byte	0x38
	.string	"larg"
	.2byte	0x142
	.4byte	.L_T530
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2530_e-.L_l2530
.L_l2530:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2530_e:
.L_D2530_e:
.L_D2531:
	.4byte	0x4
.L_D2522:
	.4byte	.L_D2522_e-.L_D2522
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2532
	.set	.L_T532,.L_D2522
	.2byte	0x38
	.string	"Generic"
.L_D2522_e:
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2532:
	.4byte	.L_D2532_e-.L_D2532
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2533
	.2byte	0x38
	.string	"cred_t"
	.2byte	0x72
	.4byte	.L_T223
.L_D2532_e:
	.previous

.section	.debug
.L_D2533:
	.4byte	.L_D2533_e-.L_D2533
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2534
	.set	.L_T549,.L_D2533
	.2byte	0x38
	.string	"timeval"
	.2byte	0xb6
	.4byte	0x8
.L_D2533_e:
.L_D2535:
	.4byte	.L_D2535_e-.L_D2535
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2536
	.2byte	0x38
	.string	"tv_sec"
	.2byte	0x142
	.4byte	.L_T549
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2535_e-.L_l2535
.L_l2535:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2535_e:
.L_D2535_e:
.L_D2536:
	.4byte	.L_D2536_e-.L_D2536
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2537
	.2byte	0x38
	.string	"tv_usec"
	.2byte	0x142
	.4byte	.L_T549
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2536_e-.L_l2536
.L_l2536:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2536_e:
.L_D2536_e:
.L_D2537:
	.4byte	0x4
	.previous

.section	.debug
.L_D2534:
	.4byte	.L_D2534_e-.L_D2534
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2538
	.set	.L_T550,.L_D2534
	.2byte	0x38
	.string	"timezone"
	.2byte	0xb6
	.4byte	0x8
.L_D2534_e:
.L_D2539:
	.4byte	.L_D2539_e-.L_D2539
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2540
	.2byte	0x38
	.string	"tz_minuteswest"
	.2byte	0x142
	.4byte	.L_T550
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2539_e-.L_l2539
.L_l2539:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2539_e:
.L_D2539_e:
.L_D2540:
	.4byte	.L_D2540_e-.L_D2540
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2541
	.2byte	0x38
	.string	"tz_dsttime"
	.2byte	0x142
	.4byte	.L_T550
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2540_e-.L_l2540
.L_l2540:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2540_e:
.L_D2540_e:
.L_D2541:
	.4byte	0x4
	.previous

.section	.debug
.L_D2538:
	.4byte	.L_D2538_e-.L_D2538
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2542
	.set	.L_T551,.L_D2538
	.2byte	0x38
	.string	"itimerval"
	.2byte	0xb6
	.4byte	0x10
.L_D2538_e:
.L_D2543:
	.4byte	.L_D2543_e-.L_D2543
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2544
	.2byte	0x38
	.string	"it_interval"
	.2byte	0x142
	.4byte	.L_T551
	.2byte	0x72
	.4byte	.L_T549
	.2byte	0x23
	.2byte	.L_l2543_e-.L_l2543
.L_l2543:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2543_e:
.L_D2543_e:
.L_D2544:
	.4byte	.L_D2544_e-.L_D2544
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2545
	.2byte	0x38
	.string	"it_value"
	.2byte	0x142
	.4byte	.L_T551
	.2byte	0x72
	.4byte	.L_T549
	.2byte	0x23
	.2byte	.L_l2544_e-.L_l2544
.L_l2544:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2544_e:
.L_D2544_e:
.L_D2545:
	.4byte	0x4
	.previous

.section	.debug
.L_D2542:
	.4byte	.L_D2542_e-.L_D2542
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2546
	.set	.L_T552,.L_D2542
	.2byte	0xb6
	.4byte	0x8
.L_D2542_e:
.L_D2547:
	.4byte	.L_D2547_e-.L_D2547
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2548
	.2byte	0x38
	.string	"tv_sec"
	.2byte	0x142
	.4byte	.L_T552
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2547_e-.L_l2547
.L_l2547:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2547_e:
.L_D2547_e:
.L_D2548:
	.4byte	.L_D2548_e-.L_D2548
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2549
	.2byte	0x38
	.string	"tv_nsec"
	.2byte	0x142
	.4byte	.L_T552
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2548_e-.L_l2548
.L_l2548:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2548_e:
.L_D2548_e:
.L_D2549:
	.4byte	0x4
	.previous

.section	.debug
.L_D2546:
	.4byte	.L_D2546_e-.L_D2546
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2550
	.2byte	0x38
	.string	"timestruc_t"
	.2byte	0x72
	.4byte	.L_T552
.L_D2546_e:
	.previous

.section	.debug
.L_D2550:
	.4byte	.L_D2550_e-.L_D2550
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2551
	.set	.L_T554,.L_D2550
	.2byte	0x38
	.string	"mac_level"
	.2byte	0xb6
	.4byte	0xc8
.L_D2550_e:
.L_D2552:
	.4byte	.L_D2552_e-.L_D2552
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2553
	.2byte	0x38
	.string	"lvl_cat"
	.2byte	0x142
	.4byte	.L_T554
	.2byte	0x72
	.4byte	.L_T557
	.2byte	0x23
	.2byte	.L_l2552_e-.L_l2552
.L_l2552:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2552_e:
.L_D2552_e:
.L_D2553:
	.4byte	.L_D2553_e-.L_D2553
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2554
	.2byte	0x38
	.string	"lvl_class"
	.2byte	0x142
	.4byte	.L_T554
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2553_e-.L_l2553
.L_l2553:
	.byte	0x4
	.4byte	0x80
	.byte	0x7
.L_l2553_e:
.L_D2553_e:
.L_D2554:
	.4byte	.L_D2554_e-.L_D2554
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2555
	.2byte	0x38
	.string	"lvl_valid"
	.2byte	0x142
	.4byte	.L_T554
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l2554_e-.L_l2554
.L_l2554:
	.byte	0x4
	.4byte	0x82
	.byte	0x7
.L_l2554_e:
.L_D2554_e:
.L_D2555:
	.4byte	.L_D2555_e-.L_D2555
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2556
	.2byte	0x38
	.string	"lvl_catsig"
	.2byte	0x142
	.4byte	.L_T554
	.2byte	0x72
	.4byte	.L_T560
	.2byte	0x23
	.2byte	.L_l2555_e-.L_l2555
.L_l2555:
	.byte	0x4
	.4byte	0x84
	.byte	0x7
.L_l2555_e:
.L_D2555_e:
.L_D2556:
	.4byte	0x4
.L_D2551:
	.4byte	.L_D2551_e-.L_D2551
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2557
	.set	.L_T557,.L_D2551
	.2byte	0xa3
	.2byte	.L_s2551_e-.L_s2551
.L_s2551:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x1f
	.byte	0x8
	.2byte	0x55
	.2byte	0x9
.L_s2551_e:
.L_D2551_e:
.L_D2557:
	.4byte	.L_D2557_e-.L_D2557
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2558
	.set	.L_T560,.L_D2557
	.2byte	0xa3
	.2byte	.L_s2557_e-.L_s2557
.L_s2557:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x20
	.byte	0x8
	.2byte	0x55
	.2byte	0x6
.L_s2557_e:
.L_D2557_e:
	.previous

.section	.debug
.L_D2558:
	.4byte	.L_D2558_e-.L_D2558
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2559
	.set	.L_T561,.L_D2558
	.2byte	0x38
	.string	"mac_cachent"
	.2byte	0xb6
	.4byte	0xd8
.L_D2558_e:
.L_D2560:
	.4byte	.L_D2560_e-.L_D2560
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2561
	.2byte	0x38
	.string	"ca_lid"
	.2byte	0x142
	.4byte	.L_T561
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2560_e-.L_l2560
.L_l2560:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2560_e:
.L_D2560_e:
.L_D2561:
	.4byte	.L_D2561_e-.L_D2561
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2562
	.2byte	0x38
	.string	"ca_lastref"
	.2byte	0x142
	.4byte	.L_T561
	.2byte	0x72
	.4byte	.L_T552
	.2byte	0x23
	.2byte	.L_l2561_e-.L_l2561
.L_l2561:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2561_e:
.L_D2561_e:
.L_D2562:
	.4byte	.L_D2562_e-.L_D2562
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2563
	.2byte	0x38
	.string	"ca_count"
	.2byte	0x142
	.4byte	.L_T561
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2562_e-.L_l2562
.L_l2562:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2562_e:
.L_D2562_e:
.L_D2563:
	.4byte	.L_D2563_e-.L_D2563
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2564
	.2byte	0x38
	.string	"ca_level"
	.2byte	0x142
	.4byte	.L_T561
	.2byte	0x72
	.4byte	.L_T554
	.2byte	0x23
	.2byte	.L_l2563_e-.L_l2563
.L_l2563:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l2563_e:
.L_D2563_e:
.L_D2564:
	.4byte	0x4
	.previous

.section	.debug
.L_D2559:
	.4byte	.L_D2559_e-.L_D2559
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2565
	.set	.L_T562,.L_D2559
	.2byte	0x38
	.string	"devstat"
	.2byte	0xb6
	.4byte	0x10
.L_D2559_e:
.L_D2566:
	.4byte	.L_D2566_e-.L_D2566
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2567
	.2byte	0x38
	.string	"dev_relflag"
	.2byte	0x142
	.4byte	.L_T562
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2566_e-.L_l2566
.L_l2566:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2566_e:
.L_D2566_e:
.L_D2567:
	.4byte	.L_D2567_e-.L_D2567
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2568
	.2byte	0x38
	.string	"dev_mode"
	.2byte	0x142
	.4byte	.L_T562
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2567_e-.L_l2567
.L_l2567:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l2567_e:
.L_D2567_e:
.L_D2568:
	.4byte	.L_D2568_e-.L_D2568
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2569
	.2byte	0x38
	.string	"dev_hilevel"
	.2byte	0x142
	.4byte	.L_T562
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2568_e-.L_l2568
.L_l2568:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2568_e:
.L_D2568_e:
.L_D2569:
	.4byte	.L_D2569_e-.L_D2569
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2570
	.2byte	0x38
	.string	"dev_lolevel"
	.2byte	0x142
	.4byte	.L_T562
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2569_e-.L_l2569
.L_l2569:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2569_e:
.L_D2569_e:
.L_D2570:
	.4byte	.L_D2570_e-.L_D2570
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2571
	.2byte	0x38
	.string	"dev_state"
	.2byte	0x142
	.4byte	.L_T562
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2570_e-.L_l2570
.L_l2570:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2570_e:
.L_D2570_e:
.L_D2571:
	.4byte	.L_D2571_e-.L_D2571
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2572
	.2byte	0x38
	.string	"dev_usecount"
	.2byte	0x142
	.4byte	.L_T562
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2571_e-.L_l2571
.L_l2571:
	.byte	0x4
	.4byte	0xe
	.byte	0x7
.L_l2571_e:
.L_D2571_e:
.L_D2572:
	.4byte	0x4
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2565:
	.4byte	.L_D2565_e-.L_D2565
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2573
	.2byte	0x38
	.string	"vtype_t"
	.2byte	0x72
	.4byte	.L_T569
.L_D2565_e:
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2573:
	.4byte	.L_D2573_e-.L_D2573
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2574
	.2byte	0x38
	.string	"vnode_t"
	.2byte	0x72
	.4byte	.L_T170
.L_D2573_e:
	.previous

.section	.debug
.L_D2574:
	.4byte	.L_D2574_e-.L_D2574
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2575
	.2byte	0x38
	.string	"vnodeops_t"
	.2byte	0x72
	.4byte	.L_T573
.L_D2574_e:
	.previous

.section	.debug
.L_D2575:
	.4byte	.L_D2575_e-.L_D2575
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2576
	.set	.L_T632,.L_D2575
	.2byte	0x38
	.string	"vattr"
	.2byte	0xb6
	.4byte	0x70
.L_D2575_e:
.L_D2577:
	.4byte	.L_D2577_e-.L_D2577
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2578
	.2byte	0x38
	.string	"va_mask"
	.2byte	0x142
	.4byte	.L_T632
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2577_e-.L_l2577
.L_l2577:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2577_e:
.L_D2577_e:
.L_D2578:
	.4byte	.L_D2578_e-.L_D2578
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2579
	.2byte	0x38
	.string	"va_type"
	.2byte	0x142
	.4byte	.L_T632
	.2byte	0x72
	.4byte	.L_T569
	.2byte	0x23
	.2byte	.L_l2578_e-.L_l2578
.L_l2578:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2578_e:
.L_D2578_e:
.L_D2579:
	.4byte	.L_D2579_e-.L_D2579
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2580
	.2byte	0x38
	.string	"va_mode"
	.2byte	0x142
	.4byte	.L_T632
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2579_e-.L_l2579
.L_l2579:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2579_e:
.L_D2579_e:
.L_D2580:
	.4byte	.L_D2580_e-.L_D2580
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2581
	.2byte	0x38
	.string	"va_uid"
	.2byte	0x142
	.4byte	.L_T632
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2580_e-.L_l2580
.L_l2580:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2580_e:
.L_D2580_e:
.L_D2581:
	.4byte	.L_D2581_e-.L_D2581
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2582
	.2byte	0x38
	.string	"va_gid"
	.2byte	0x142
	.4byte	.L_T632
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2581_e-.L_l2581
.L_l2581:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l2581_e:
.L_D2581_e:
.L_D2582:
	.4byte	.L_D2582_e-.L_D2582
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2583
	.2byte	0x38
	.string	"va_fsid"
	.2byte	0x142
	.4byte	.L_T632
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2582_e-.L_l2582
.L_l2582:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l2582_e:
.L_D2582_e:
.L_D2583:
	.4byte	.L_D2583_e-.L_D2583
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2584
	.2byte	0x38
	.string	"va_nodeid"
	.2byte	0x142
	.4byte	.L_T632
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2583_e-.L_l2583
.L_l2583:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l2583_e:
.L_D2583_e:
.L_D2584:
	.4byte	.L_D2584_e-.L_D2584
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2585
	.2byte	0x38
	.string	"va_nlink"
	.2byte	0x142
	.4byte	.L_T632
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2584_e-.L_l2584
.L_l2584:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l2584_e:
.L_D2584_e:
.L_D2585:
	.4byte	.L_D2585_e-.L_D2585
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2586
	.2byte	0x38
	.string	"va_size0"
	.2byte	0x142
	.4byte	.L_T632
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2585_e-.L_l2585
.L_l2585:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l2585_e:
.L_D2585_e:
.L_D2586:
	.4byte	.L_D2586_e-.L_D2586
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2587
	.2byte	0x38
	.string	"va_size"
	.2byte	0x142
	.4byte	.L_T632
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2586_e-.L_l2586
.L_l2586:
	.byte	0x4
	.4byte	0x24
	.byte	0x7
.L_l2586_e:
.L_D2586_e:
.L_D2587:
	.4byte	.L_D2587_e-.L_D2587
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2588
	.2byte	0x38
	.string	"va_atime"
	.2byte	0x142
	.4byte	.L_T632
	.2byte	0x72
	.4byte	.L_T552
	.2byte	0x23
	.2byte	.L_l2587_e-.L_l2587
.L_l2587:
	.byte	0x4
	.4byte	0x28
	.byte	0x7
.L_l2587_e:
.L_D2587_e:
.L_D2588:
	.4byte	.L_D2588_e-.L_D2588
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2589
	.2byte	0x38
	.string	"va_mtime"
	.2byte	0x142
	.4byte	.L_T632
	.2byte	0x72
	.4byte	.L_T552
	.2byte	0x23
	.2byte	.L_l2588_e-.L_l2588
.L_l2588:
	.byte	0x4
	.4byte	0x30
	.byte	0x7
.L_l2588_e:
.L_D2588_e:
.L_D2589:
	.4byte	.L_D2589_e-.L_D2589
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2590
	.2byte	0x38
	.string	"va_ctime"
	.2byte	0x142
	.4byte	.L_T632
	.2byte	0x72
	.4byte	.L_T552
	.2byte	0x23
	.2byte	.L_l2589_e-.L_l2589
.L_l2589:
	.byte	0x4
	.4byte	0x38
	.byte	0x7
.L_l2589_e:
.L_D2589_e:
.L_D2590:
	.4byte	.L_D2590_e-.L_D2590
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2591
	.2byte	0x38
	.string	"va_rdev"
	.2byte	0x142
	.4byte	.L_T632
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2590_e-.L_l2590
.L_l2590:
	.byte	0x4
	.4byte	0x40
	.byte	0x7
.L_l2590_e:
.L_D2590_e:
.L_D2591:
	.4byte	.L_D2591_e-.L_D2591
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2592
	.2byte	0x38
	.string	"va_blksize"
	.2byte	0x142
	.4byte	.L_T632
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2591_e-.L_l2591
.L_l2591:
	.byte	0x4
	.4byte	0x44
	.byte	0x7
.L_l2591_e:
.L_D2591_e:
.L_D2592:
	.4byte	.L_D2592_e-.L_D2592
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2593
	.2byte	0x38
	.string	"va_nblocks"
	.2byte	0x142
	.4byte	.L_T632
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2592_e-.L_l2592
.L_l2592:
	.byte	0x4
	.4byte	0x48
	.byte	0x7
.L_l2592_e:
.L_D2592_e:
.L_D2593:
	.4byte	.L_D2593_e-.L_D2593
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2594
	.2byte	0x38
	.string	"va_vcode"
	.2byte	0x142
	.4byte	.L_T632
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2593_e-.L_l2593
.L_l2593:
	.byte	0x4
	.4byte	0x4c
	.byte	0x7
.L_l2593_e:
.L_D2593_e:
.L_D2594:
	.4byte	.L_D2594_e-.L_D2594
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2595
	.2byte	0x38
	.string	"va_filler"
	.2byte	0x142
	.4byte	.L_T632
	.2byte	0x72
	.4byte	.L_T634
	.2byte	0x23
	.2byte	.L_l2594_e-.L_l2594
.L_l2594:
	.byte	0x4
	.4byte	0x50
	.byte	0x7
.L_l2594_e:
.L_D2594_e:
.L_D2595:
	.4byte	0x4
.L_D2576:
	.4byte	.L_D2576_e-.L_D2576
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2596
	.set	.L_T634,.L_D2576
	.2byte	0xa3
	.2byte	.L_s2576_e-.L_s2576
.L_s2576:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x7
	.byte	0x8
	.2byte	0x55
	.2byte	0xa
.L_s2576_e:
.L_D2576_e:
	.previous

.section	.debug
.L_D2596:
	.4byte	.L_D2596_e-.L_D2596
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2597
	.2byte	0x38
	.string	"vattr_t"
	.2byte	0x72
	.4byte	.L_T632
.L_D2596_e:
	.previous

.section	.debug
.L_D2597:
	.4byte	.L_D2597_e-.L_D2597
	.2byte	0x4
	.2byte	0x12
	.4byte	.L_D2598
	.set	.L_T644,.L_D2597
	.2byte	0x38
	.string	"rm"
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xf4
	.4byte	.L_e2597_e-.L_e2597
.L_e2597:
	.4byte	0x1
	.string	"RMDIRECTORY"
	.4byte	0x0
	.string	"RMFILE"
.L_e2597_e:
.L_D2597_e:
	.previous

.section	.debug
.L_D2598:
	.4byte	.L_D2598_e-.L_D2598
	.2byte	0x4
	.2byte	0x12
	.4byte	.L_D2599
	.set	.L_T645,.L_D2598
	.2byte	0x38
	.string	"symfollow"
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xf4
	.4byte	.L_e2598_e-.L_e2598
.L_e2598:
	.4byte	0x1
	.string	"FOLLOW"
	.4byte	0x0
	.string	"NO_FOLLOW"
.L_e2598_e:
.L_D2598_e:
	.previous

.section	.debug
.L_D2599:
	.4byte	.L_D2599_e-.L_D2599
	.2byte	0x4
	.2byte	0x12
	.4byte	.L_D2600
	.set	.L_T646,.L_D2599
	.2byte	0x38
	.string	"vcexcl"
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xf4
	.4byte	.L_e2599_e-.L_e2599
.L_e2599:
	.4byte	0x1
	.string	"EXCL"
	.4byte	0x0
	.string	"NONEXCL"
.L_e2599_e:
.L_D2599_e:
	.previous

.section	.debug
.L_D2600:
	.4byte	.L_D2600_e-.L_D2600
	.2byte	0x4
	.2byte	0x12
	.4byte	.L_D2601
	.set	.L_T647,.L_D2600
	.2byte	0x38
	.string	"create"
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xf4
	.4byte	.L_e2600_e-.L_e2600
.L_e2600:
	.4byte	0x4
	.string	"CRMKMLD"
	.4byte	0x3
	.string	"CRCORE"
	.4byte	0x2
	.string	"CRMKDIR"
	.4byte	0x1
	.string	"CRMKNOD"
	.4byte	0x0
	.string	"CRCREAT"
.L_e2600_e:
.L_D2600_e:
	.previous

.section	.debug
.L_D2601:
	.4byte	.L_D2601_e-.L_D2601
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2602
	.2byte	0x38
	.string	"rm_t"
	.2byte	0x72
	.4byte	.L_T644
.L_D2601_e:
	.previous

.section	.debug
.L_D2602:
	.4byte	.L_D2602_e-.L_D2602
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2603
	.2byte	0x38
	.string	"symfollow_t"
	.2byte	0x72
	.4byte	.L_T645
.L_D2602_e:
	.previous

.section	.debug
.L_D2603:
	.4byte	.L_D2603_e-.L_D2603
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2604
	.2byte	0x38
	.string	"vcexcl_t"
	.2byte	0x72
	.4byte	.L_T646
.L_D2603_e:
	.previous

.section	.debug
.L_D2604:
	.4byte	.L_D2604_e-.L_D2604
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2605
	.2byte	0x38
	.string	"create_t"
	.2byte	0x72
	.4byte	.L_T647
.L_D2604_e:
	.previous

.section	.debug
.L_D2605:
	.4byte	.L_D2605_e-.L_D2605
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2606
	.set	.L_T654,.L_D2605
	.2byte	0x38
	.string	"pollfd"
	.2byte	0xb6
	.4byte	0x8
.L_D2605_e:
.L_D2607:
	.4byte	.L_D2607_e-.L_D2607
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2608
	.2byte	0x38
	.string	"fd"
	.2byte	0x142
	.4byte	.L_T654
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2607_e-.L_l2607
.L_l2607:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2607_e:
.L_D2607_e:
.L_D2608:
	.4byte	.L_D2608_e-.L_D2608
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2609
	.2byte	0x38
	.string	"events"
	.2byte	0x142
	.4byte	.L_T654
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2608_e-.L_l2608
.L_l2608:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2608_e:
.L_D2608_e:
.L_D2609:
	.4byte	.L_D2609_e-.L_D2609
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2610
	.2byte	0x38
	.string	"revents"
	.2byte	0x142
	.4byte	.L_T654
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2609_e-.L_l2609
.L_l2609:
	.byte	0x4
	.4byte	0x6
	.byte	0x7
.L_l2609_e:
.L_D2609_e:
.L_D2610:
	.4byte	0x4
	.previous

.section	.debug
.L_D2606:
	.4byte	.L_D2606_e-.L_D2606
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2611
	.set	.L_T655,.L_D2606
	.2byte	0x38
	.string	"pollhead"
	.2byte	0xb6
	.4byte	0x20
.L_D2606_e:
.L_D2612:
	.4byte	.L_D2612_e-.L_D2612
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2613
	.2byte	0x38
	.string	"ph_list"
	.2byte	0x142
	.4byte	.L_T655
	.2byte	0x83
	.2byte	.L_t2612_e-.L_t2612
.L_t2612:
	.byte	0x1
	.4byte	.L_T656
.L_t2612_e:
	.2byte	0x23
	.2byte	.L_l2612_e-.L_l2612
.L_l2612:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2612_e:
.L_D2612_e:
.L_D2613:
	.4byte	.L_D2613_e-.L_D2613
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2614
	.2byte	0x38
	.string	"ph_dummy"
	.2byte	0x142
	.4byte	.L_T655
	.2byte	0x83
	.2byte	.L_t2613_e-.L_t2613
.L_t2613:
	.byte	0x1
	.4byte	.L_T656
.L_t2613_e:
	.2byte	0x23
	.2byte	.L_l2613_e-.L_l2613
.L_l2613:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2613_e:
.L_D2613_e:
.L_D2614:
	.4byte	.L_D2614_e-.L_D2614
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2615
	.2byte	0x38
	.string	"ph_events"
	.2byte	0x142
	.4byte	.L_T655
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2614_e-.L_l2614
.L_l2614:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2614_e:
.L_D2614_e:
.L_D2615:
	.4byte	.L_D2615_e-.L_D2615
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2616
	.2byte	0x38
	.string	"ph_filler"
	.2byte	0x142
	.4byte	.L_T655
	.2byte	0x72
	.4byte	.L_T659
	.2byte	0x23
	.2byte	.L_l2615_e-.L_l2615
.L_l2615:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2615_e:
.L_D2615_e:
.L_D2616:
	.4byte	0x4
.L_D2611:
	.4byte	.L_D2611_e-.L_D2611
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2617
	.set	.L_T656,.L_D2611
	.2byte	0x38
	.string	"polldat"
	.2byte	0xb6
	.4byte	0x1c
.L_D2611_e:
.L_D2618:
	.4byte	.L_D2618_e-.L_D2618
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2619
	.2byte	0x38
	.string	"pd_next"
	.2byte	0x142
	.4byte	.L_T656
	.2byte	0x83
	.2byte	.L_t2618_e-.L_t2618
.L_t2618:
	.byte	0x1
	.4byte	.L_T656
.L_t2618_e:
	.2byte	0x23
	.2byte	.L_l2618_e-.L_l2618
.L_l2618:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2618_e:
.L_D2618_e:
.L_D2619:
	.4byte	.L_D2619_e-.L_D2619
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2620
	.2byte	0x38
	.string	"pd_prev"
	.2byte	0x142
	.4byte	.L_T656
	.2byte	0x83
	.2byte	.L_t2619_e-.L_t2619
.L_t2619:
	.byte	0x1
	.4byte	.L_T656
.L_t2619_e:
	.2byte	0x23
	.2byte	.L_l2619_e-.L_l2619
.L_l2619:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2619_e:
.L_D2619_e:
.L_D2620:
	.4byte	.L_D2620_e-.L_D2620
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2621
	.2byte	0x38
	.string	"pd_chain"
	.2byte	0x142
	.4byte	.L_T656
	.2byte	0x83
	.2byte	.L_t2620_e-.L_t2620
.L_t2620:
	.byte	0x1
	.4byte	.L_T656
.L_t2620_e:
	.2byte	0x23
	.2byte	.L_l2620_e-.L_l2620
.L_l2620:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2620_e:
.L_D2620_e:
.L_D2621:
	.4byte	.L_D2621_e-.L_D2621
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2622
	.2byte	0x38
	.string	"pd_events"
	.2byte	0x142
	.4byte	.L_T656
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2621_e-.L_l2621
.L_l2621:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2621_e:
.L_D2621_e:
.L_D2622:
	.4byte	.L_D2622_e-.L_D2622
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2623
	.2byte	0x38
	.string	"pd_headp"
	.2byte	0x142
	.4byte	.L_T656
	.2byte	0x83
	.2byte	.L_t2622_e-.L_t2622
.L_t2622:
	.byte	0x1
	.4byte	.L_T655
.L_t2622_e:
	.2byte	0x23
	.2byte	.L_l2622_e-.L_l2622
.L_l2622:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l2622_e:
.L_D2622_e:
.L_D2623:
	.4byte	.L_D2623_e-.L_D2623
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2624
	.2byte	0x38
	.string	"pd_fn"
	.2byte	0x142
	.4byte	.L_T656
	.2byte	0x83
	.2byte	.L_t2623_e-.L_t2623
.L_t2623:
	.byte	0x1
	.4byte	.L_T41
.L_t2623_e:
	.2byte	0x23
	.2byte	.L_l2623_e-.L_l2623
.L_l2623:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l2623_e:
.L_D2623_e:
.L_D2624:
	.4byte	.L_D2624_e-.L_D2624
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2625
	.2byte	0x38
	.string	"pd_arg"
	.2byte	0x142
	.4byte	.L_T656
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2624_e-.L_l2624
.L_l2624:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l2624_e:
.L_D2624_e:
.L_D2625:
	.4byte	0x4
.L_D2617:
	.4byte	.L_D2617_e-.L_D2617
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2626
	.set	.L_T659,.L_D2617
	.2byte	0xa3
	.2byte	.L_s2617_e-.L_s2617
.L_s2617:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x4
	.byte	0x8
	.2byte	0x55
	.2byte	0xa
.L_s2617_e:
.L_D2617_e:
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2626:
	.4byte	.L_D2626_e-.L_D2626
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2627
	.2byte	0x38
	.string	"queue_t"
	.2byte	0x72
	.4byte	.L_T664
.L_D2626_e:
	.previous

.section	.debug
.L_D2627:
	.4byte	.L_D2627_e-.L_D2627
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2628
	.2byte	0x38
	.string	"qband_t"
	.2byte	0x72
	.4byte	.L_T670
.L_D2627_e:
	.previous

.section	.debug
.L_D2628:
	.4byte	.L_D2628_e-.L_D2628
	.2byte	0x4
	.2byte	0x12
	.4byte	.L_D2629
	.set	.L_T678,.L_D2628
	.2byte	0x38
	.string	"qfields"
	.2byte	0xb6
	.4byte	0x4
	.2byte	0xf4
	.4byte	.L_e2628_e-.L_e2628
.L_e2628:
	.4byte	0x8
	.string	"QBAD"
	.4byte	0x7
	.string	"QFLAG"
	.4byte	0x6
	.string	"QLAST"
	.4byte	0x5
	.string	"QFIRST"
	.4byte	0x4
	.string	"QCOUNT"
	.4byte	0x3
	.string	"QMINPSZ"
	.4byte	0x2
	.string	"QMAXPSZ"
	.4byte	0x1
	.string	"QLOWAT"
	.4byte	0x0
	.string	"QHIWAT"
.L_e2628_e:
.L_D2628_e:
	.previous

.section	.debug
.L_D2629:
	.4byte	.L_D2629_e-.L_D2629
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2630
	.2byte	0x38
	.string	"qfields_t"
	.2byte	0x72
	.4byte	.L_T678
.L_D2629_e:
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2630:
	.4byte	.L_D2630_e-.L_D2630
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2631
	.set	.L_T687,.L_D2630
	.2byte	0x38
	.string	"module_stat"
.L_D2630_e:
	.previous

.section	.debug
.L_D2631:
	.4byte	.L_D2631_e-.L_D2631
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2632
	.set	.L_T689,.L_D2631
	.2byte	0x38
	.string	"linkblk"
	.2byte	0xb6
	.4byte	0x20
.L_D2631_e:
.L_D2633:
	.4byte	.L_D2633_e-.L_D2633
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2634
	.2byte	0x38
	.string	"l_qtop"
	.2byte	0x142
	.4byte	.L_T689
	.2byte	0x83
	.2byte	.L_t2633_e-.L_t2633
.L_t2633:
	.byte	0x1
	.4byte	.L_T664
.L_t2633_e:
	.2byte	0x23
	.2byte	.L_l2633_e-.L_l2633
.L_l2633:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2633_e:
.L_D2633_e:
.L_D2634:
	.4byte	.L_D2634_e-.L_D2634
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2635
	.2byte	0x38
	.string	"l_qbot"
	.2byte	0x142
	.4byte	.L_T689
	.2byte	0x83
	.2byte	.L_t2634_e-.L_t2634
.L_t2634:
	.byte	0x1
	.4byte	.L_T664
.L_t2634_e:
	.2byte	0x23
	.2byte	.L_l2634_e-.L_l2634
.L_l2634:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2634_e:
.L_D2634_e:
.L_D2635:
	.4byte	.L_D2635_e-.L_D2635
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2636
	.2byte	0x38
	.string	"l_index"
	.2byte	0x142
	.4byte	.L_T689
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2635_e-.L_l2635
.L_l2635:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2635_e:
.L_D2635_e:
.L_D2636:
	.4byte	.L_D2636_e-.L_D2636
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2637
	.2byte	0x38
	.string	"l_pad"
	.2byte	0x142
	.4byte	.L_T689
	.2byte	0x72
	.4byte	.L_T659
	.2byte	0x23
	.2byte	.L_l2636_e-.L_l2636
.L_l2636:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2636_e:
.L_D2636_e:
.L_D2637:
	.4byte	0x4
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2632:
	.4byte	.L_D2632_e-.L_D2632
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2638
	.2byte	0x38
	.string	"mblk_t"
	.2byte	0x72
	.4byte	.L_T667
.L_D2632_e:
	.previous

.section	.debug
.L_D2638:
	.4byte	.L_D2638_e-.L_D2638
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2639
	.2byte	0x38
	.string	"dblk_t"
	.2byte	0x72
	.4byte	.L_T695
.L_D2638_e:
	.previous

.section	.debug
.L_D2639:
	.4byte	.L_D2639_e-.L_D2639
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2640
	.2byte	0x38
	.string	"frtn_t"
	.2byte	0x72
	.4byte	.L_T693
.L_D2639_e:
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2640:
	.4byte	.L_D2640_e-.L_D2640
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2641
	.set	.L_T712,.L_D2640
	.2byte	0x38
	.string	"stroptions"
	.2byte	0xb6
	.4byte	0x1c
.L_D2640_e:
.L_D2642:
	.4byte	.L_D2642_e-.L_D2642
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2643
	.2byte	0x38
	.string	"so_flags"
	.2byte	0x142
	.4byte	.L_T712
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2642_e-.L_l2642
.L_l2642:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2642_e:
.L_D2642_e:
.L_D2643:
	.4byte	.L_D2643_e-.L_D2643
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2644
	.2byte	0x38
	.string	"so_readopt"
	.2byte	0x142
	.4byte	.L_T712
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2643_e-.L_l2643
.L_l2643:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2643_e:
.L_D2643_e:
.L_D2644:
	.4byte	.L_D2644_e-.L_D2644
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2645
	.2byte	0x38
	.string	"so_wroff"
	.2byte	0x142
	.4byte	.L_T712
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2644_e-.L_l2644
.L_l2644:
	.byte	0x4
	.4byte	0x6
	.byte	0x7
.L_l2644_e:
.L_D2644_e:
.L_D2645:
	.4byte	.L_D2645_e-.L_D2645
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2646
	.2byte	0x38
	.string	"so_minpsz"
	.2byte	0x142
	.4byte	.L_T712
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2645_e-.L_l2645
.L_l2645:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2645_e:
.L_D2645_e:
.L_D2646:
	.4byte	.L_D2646_e-.L_D2646
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2647
	.2byte	0x38
	.string	"so_maxpsz"
	.2byte	0x142
	.4byte	.L_T712
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2646_e-.L_l2646
.L_l2646:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2646_e:
.L_D2646_e:
.L_D2647:
	.4byte	.L_D2647_e-.L_D2647
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2648
	.2byte	0x38
	.string	"so_hiwat"
	.2byte	0x142
	.4byte	.L_T712
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2647_e-.L_l2647
.L_l2647:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l2647_e:
.L_D2647_e:
.L_D2648:
	.4byte	.L_D2648_e-.L_D2648
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2649
	.2byte	0x38
	.string	"so_lowat"
	.2byte	0x142
	.4byte	.L_T712
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2648_e-.L_l2648
.L_l2648:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l2648_e:
.L_D2648_e:
.L_D2649:
	.4byte	.L_D2649_e-.L_D2649
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2650
	.2byte	0x38
	.string	"so_band"
	.2byte	0x142
	.4byte	.L_T712
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l2649_e-.L_l2649
.L_l2649:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l2649_e:
.L_D2649_e:
.L_D2650:
	.4byte	0x4
	.previous

.section	.debug
.L_D2641:
	.4byte	.L_D2641_e-.L_D2641
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2651
	.set	.L_T713,.L_D2641
	.2byte	0x38
	.string	"str_evmsg"
	.2byte	0xb6
	.4byte	0x30
.L_D2641_e:
.L_D2652:
	.4byte	.L_D2652_e-.L_D2652
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2653
	.2byte	0x38
	.string	"sv_event"
	.2byte	0x142
	.4byte	.L_T713
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2652_e-.L_l2652
.L_l2652:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2652_e:
.L_D2652_e:
.L_D2653:
	.4byte	.L_D2653_e-.L_D2653
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2654
	.2byte	0x38
	.string	"sv_vp"
	.2byte	0x142
	.4byte	.L_T713
	.2byte	0x83
	.2byte	.L_t2653_e-.L_t2653
.L_t2653:
	.byte	0x1
	.4byte	.L_T170
.L_t2653_e:
	.2byte	0x23
	.2byte	.L_l2653_e-.L_l2653
.L_l2653:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2653_e:
.L_D2653_e:
.L_D2654:
	.4byte	.L_D2654_e-.L_D2654
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2655
	.2byte	0x38
	.string	"sv_eid"
	.2byte	0x142
	.4byte	.L_T713
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2654_e-.L_l2654
.L_l2654:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2654_e:
.L_D2654_e:
.L_D2655:
	.4byte	.L_D2655_e-.L_D2655
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2656
	.2byte	0x38
	.string	"sv_evpri"
	.2byte	0x142
	.4byte	.L_T713
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2655_e-.L_l2655
.L_l2655:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2655_e:
.L_D2655_e:
.L_D2656:
	.4byte	.L_D2656_e-.L_D2656
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2657
	.2byte	0x38
	.string	"sv_flags"
	.2byte	0x142
	.4byte	.L_T713
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2656_e-.L_l2656
.L_l2656:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l2656_e:
.L_D2656_e:
.L_D2657:
	.4byte	.L_D2657_e-.L_D2657
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2658
	.2byte	0x38
	.string	"sv_uid"
	.2byte	0x142
	.4byte	.L_T713
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2657_e-.L_l2657
.L_l2657:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l2657_e:
.L_D2657_e:
.L_D2658:
	.4byte	.L_D2658_e-.L_D2658
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2659
	.2byte	0x38
	.string	"sv_pid"
	.2byte	0x142
	.4byte	.L_T713
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2658_e-.L_l2658
.L_l2658:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l2658_e:
.L_D2658_e:
.L_D2659:
	.4byte	.L_D2659_e-.L_D2659
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2660
	.2byte	0x38
	.string	"sv_hostid"
	.2byte	0x142
	.4byte	.L_T713
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2659_e-.L_l2659
.L_l2659:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l2659_e:
.L_D2659_e:
.L_D2660:
	.4byte	.L_D2660_e-.L_D2660
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2661
	.2byte	0x38
	.string	"sv_pad"
	.2byte	0x142
	.4byte	.L_T713
	.2byte	0x72
	.4byte	.L_T704
	.2byte	0x23
	.2byte	.L_l2660_e-.L_l2660
.L_l2660:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l2660_e:
.L_D2660_e:
.L_D2661:
	.4byte	0x4
	.previous

.section	.debug
.L_D2651:
	.4byte	.L_D2651_e-.L_D2651
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2662
	.set	.L_T755,.L_D2651
	.2byte	0x38
	.string	"strstat"
.L_D2651_e:
	.previous

.section	.debug
.L_D2662:
	.4byte	.L_D2662_e-.L_D2662
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2663
	.set	.L_T756,.L_D2662
	.2byte	0x38
	.string	"v86blk"
	.2byte	0xb6
	.4byte	0x18
.L_D2662_e:
.L_D2664:
	.4byte	.L_D2664_e-.L_D2664
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2665
	.2byte	0x38
	.string	"v86_u_procp"
	.2byte	0x142
	.4byte	.L_T756
	.2byte	0x83
	.2byte	.L_t2664_e-.L_t2664
.L_t2664:
	.byte	0x1
	.4byte	.L_T165
.L_t2664_e:
	.2byte	0x23
	.2byte	.L_l2664_e-.L_l2664
.L_l2664:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2664_e:
.L_D2664_e:
.L_D2665:
	.4byte	.L_D2665_e-.L_D2665
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2666
	.2byte	0x38
	.string	"v86_u_renv"
	.2byte	0x142
	.4byte	.L_T756
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2665_e-.L_l2665
.L_l2665:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2665_e:
.L_D2665_e:
.L_D2666:
	.4byte	.L_D2666_e-.L_D2666
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2667
	.2byte	0x38
	.string	"v86_p_pid"
	.2byte	0x142
	.4byte	.L_T756
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2666_e-.L_l2666
.L_l2666:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2666_e:
.L_D2666_e:
.L_D2667:
	.4byte	.L_D2667_e-.L_D2667
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2668
	.2byte	0x38
	.string	"v86_p_ppid"
	.2byte	0x142
	.4byte	.L_T756
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2667_e-.L_l2667
.L_l2667:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2667_e:
.L_D2667_e:
.L_D2668:
	.4byte	.L_D2668_e-.L_D2668
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2669
	.2byte	0x38
	.string	"v86_p_cred"
	.2byte	0x142
	.4byte	.L_T756
	.2byte	0x83
	.2byte	.L_t2668_e-.L_t2668
.L_t2668:
	.byte	0x1
	.4byte	.L_T223
.L_t2668_e:
	.2byte	0x23
	.2byte	.L_l2668_e-.L_l2668
.L_l2668:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l2668_e:
.L_D2668_e:
.L_D2669:
	.4byte	.L_D2669_e-.L_D2669
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2670
	.2byte	0x38
	.string	"v86_p_v86"
	.2byte	0x142
	.4byte	.L_T756
	.2byte	0x83
	.2byte	.L_t2669_e-.L_t2669
.L_t2669:
	.byte	0x1
	.4byte	.L_T234
.L_t2669_e:
	.2byte	0x23
	.2byte	.L_l2669_e-.L_l2669
.L_l2669:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l2669_e:
.L_D2669_e:
.L_D2670:
	.4byte	0x4
	.previous

.section	.debug
.L_D2663:
	.4byte	.L_D2663_e-.L_D2663
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2671
	.set	.L_T757,.L_D2663
	.2byte	0x38
	.string	"obj_attr"
	.2byte	0xb6
	.4byte	0x18
.L_D2663_e:
.L_D2672:
	.4byte	.L_D2672_e-.L_D2672
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2673
	.2byte	0x38
	.string	"uid"
	.2byte	0x142
	.4byte	.L_T757
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2672_e-.L_l2672
.L_l2672:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2672_e:
.L_D2672_e:
.L_D2673:
	.4byte	.L_D2673_e-.L_D2673
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2674
	.2byte	0x38
	.string	"gid"
	.2byte	0x142
	.4byte	.L_T757
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2673_e-.L_l2673
.L_l2673:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2673_e:
.L_D2673_e:
.L_D2674:
	.4byte	.L_D2674_e-.L_D2674
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2675
	.2byte	0x38
	.string	"mode"
	.2byte	0x142
	.4byte	.L_T757
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2674_e-.L_l2674
.L_l2674:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2674_e:
.L_D2674_e:
.L_D2675:
	.4byte	.L_D2675_e-.L_D2675
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2676
	.2byte	0x38
	.string	"lid"
	.2byte	0x142
	.4byte	.L_T757
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2675_e-.L_l2675
.L_l2675:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2675_e:
.L_D2675_e:
.L_D2676:
	.4byte	.L_D2676_e-.L_D2676
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2677
	.2byte	0x38
	.string	"filler"
	.2byte	0x142
	.4byte	.L_T757
	.2byte	0x72
	.4byte	.L_T384
	.2byte	0x23
	.2byte	.L_l2676_e-.L_l2676
.L_l2676:
	.byte	0x4
	.4byte	0x10
	.byte	0x7
.L_l2676_e:
.L_D2676_e:
.L_D2677:
	.4byte	0x4
	.previous

.section	.debug
.L_D2671:
	.4byte	.L_D2671_e-.L_D2671
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2678
	.set	.L_T760,.L_D2671
	.2byte	0x38
	.string	"sub_attr"
	.2byte	0xb6
	.4byte	0x1
.L_D2671_e:
.L_D2679:
	.4byte	.L_D2679_e-.L_D2679
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2680
	.2byte	0x38
	.string	"kernel_info"
	.2byte	0x142
	.4byte	.L_T760
	.2byte	0x72
	.4byte	.L_T762
	.2byte	0x23
	.2byte	.L_l2679_e-.L_l2679
.L_l2679:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2679_e:
.L_D2679_e:
.L_D2680:
	.4byte	0x4
.L_D2678:
	.4byte	.L_D2678_e-.L_D2678
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2681
	.set	.L_T762,.L_D2678
	.2byte	0xa3
	.2byte	.L_s2678_e-.L_s2678
.L_s2678:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x1
.L_s2678_e:
.L_D2678_e:
	.previous

.section	.debug
.L_D2681:
	.4byte	.L_D2681_e-.L_D2681
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2682
	.set	.L_T763,.L_D2681
	.2byte	0x38
	.string	"strioctl"
	.2byte	0xb6
	.4byte	0x10
.L_D2681_e:
.L_D2683:
	.4byte	.L_D2683_e-.L_D2683
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2684
	.2byte	0x38
	.string	"ic_cmd"
	.2byte	0x142
	.4byte	.L_T763
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2683_e-.L_l2683
.L_l2683:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2683_e:
.L_D2683_e:
.L_D2684:
	.4byte	.L_D2684_e-.L_D2684
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2685
	.2byte	0x38
	.string	"ic_timout"
	.2byte	0x142
	.4byte	.L_T763
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2684_e-.L_l2684
.L_l2684:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2684_e:
.L_D2684_e:
.L_D2685:
	.4byte	.L_D2685_e-.L_D2685
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2686
	.2byte	0x38
	.string	"ic_len"
	.2byte	0x142
	.4byte	.L_T763
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2685_e-.L_l2685
.L_l2685:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2685_e:
.L_D2685_e:
.L_D2686:
	.4byte	.L_D2686_e-.L_D2686
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2687
	.2byte	0x38
	.string	"ic_dp"
	.2byte	0x142
	.4byte	.L_T763
	.2byte	0x63
	.2byte	.L_t2686_e-.L_t2686
.L_t2686:
	.byte	0x1
	.2byte	0x1
.L_t2686_e:
	.2byte	0x23
	.2byte	.L_l2686_e-.L_l2686
.L_l2686:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2686_e:
.L_D2686_e:
.L_D2687:
	.4byte	0x4
	.previous

.section	.debug
.L_D2682:
	.4byte	.L_D2682_e-.L_D2682
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2688
	.set	.L_T764,.L_D2682
	.2byte	0x38
	.string	"strbuf"
	.2byte	0xb6
	.4byte	0xc
.L_D2682_e:
.L_D2689:
	.4byte	.L_D2689_e-.L_D2689
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2690
	.2byte	0x38
	.string	"maxlen"
	.2byte	0x142
	.4byte	.L_T764
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2689_e-.L_l2689
.L_l2689:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2689_e:
.L_D2689_e:
.L_D2690:
	.4byte	.L_D2690_e-.L_D2690
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2691
	.2byte	0x38
	.string	"len"
	.2byte	0x142
	.4byte	.L_T764
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2690_e-.L_l2690
.L_l2690:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2690_e:
.L_D2690_e:
.L_D2691:
	.4byte	.L_D2691_e-.L_D2691
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2692
	.2byte	0x38
	.string	"buf"
	.2byte	0x142
	.4byte	.L_T764
	.2byte	0x63
	.2byte	.L_t2691_e-.L_t2691
.L_t2691:
	.byte	0x1
	.2byte	0x1
.L_t2691_e:
	.2byte	0x23
	.2byte	.L_l2691_e-.L_l2691
.L_l2691:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2691_e:
.L_D2691_e:
.L_D2692:
	.4byte	0x4
	.previous

.section	.debug
.L_D2688:
	.4byte	.L_D2688_e-.L_D2688
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2693
	.set	.L_T765,.L_D2688
	.2byte	0x38
	.string	"strpeek"
	.2byte	0xb6
	.4byte	0x1c
.L_D2688_e:
.L_D2694:
	.4byte	.L_D2694_e-.L_D2694
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2695
	.2byte	0x38
	.string	"ctlbuf"
	.2byte	0x142
	.4byte	.L_T765
	.2byte	0x72
	.4byte	.L_T764
	.2byte	0x23
	.2byte	.L_l2694_e-.L_l2694
.L_l2694:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2694_e:
.L_D2694_e:
.L_D2695:
	.4byte	.L_D2695_e-.L_D2695
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2696
	.2byte	0x38
	.string	"databuf"
	.2byte	0x142
	.4byte	.L_T765
	.2byte	0x72
	.4byte	.L_T764
	.2byte	0x23
	.2byte	.L_l2695_e-.L_l2695
.L_l2695:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2695_e:
.L_D2695_e:
.L_D2696:
	.4byte	.L_D2696_e-.L_D2696
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2697
	.2byte	0x38
	.string	"flags"
	.2byte	0x142
	.4byte	.L_T765
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2696_e-.L_l2696
.L_l2696:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l2696_e:
.L_D2696_e:
.L_D2697:
	.4byte	0x4
	.previous

.section	.debug
.L_D2693:
	.4byte	.L_D2693_e-.L_D2693
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2698
	.set	.L_T766,.L_D2693
	.2byte	0x38
	.string	"strfdinsert"
	.2byte	0xb6
	.4byte	0x24
.L_D2693_e:
.L_D2699:
	.4byte	.L_D2699_e-.L_D2699
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2700
	.2byte	0x38
	.string	"ctlbuf"
	.2byte	0x142
	.4byte	.L_T766
	.2byte	0x72
	.4byte	.L_T764
	.2byte	0x23
	.2byte	.L_l2699_e-.L_l2699
.L_l2699:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2699_e:
.L_D2699_e:
.L_D2700:
	.4byte	.L_D2700_e-.L_D2700
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2701
	.2byte	0x38
	.string	"databuf"
	.2byte	0x142
	.4byte	.L_T766
	.2byte	0x72
	.4byte	.L_T764
	.2byte	0x23
	.2byte	.L_l2700_e-.L_l2700
.L_l2700:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2700_e:
.L_D2700_e:
.L_D2701:
	.4byte	.L_D2701_e-.L_D2701
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2702
	.2byte	0x38
	.string	"flags"
	.2byte	0x142
	.4byte	.L_T766
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2701_e-.L_l2701
.L_l2701:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l2701_e:
.L_D2701_e:
.L_D2702:
	.4byte	.L_D2702_e-.L_D2702
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2703
	.2byte	0x38
	.string	"fildes"
	.2byte	0x142
	.4byte	.L_T766
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2702_e-.L_l2702
.L_l2702:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l2702_e:
.L_D2702_e:
.L_D2703:
	.4byte	.L_D2703_e-.L_D2703
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2704
	.2byte	0x38
	.string	"offset"
	.2byte	0x142
	.4byte	.L_T766
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2703_e-.L_l2703
.L_l2703:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l2703_e:
.L_D2703_e:
.L_D2704:
	.4byte	0x4
	.previous

.section	.debug
.L_D2698:
	.4byte	.L_D2698_e-.L_D2698
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2705
	.set	.L_T767,.L_D2698
	.2byte	0x38
	.string	"o_strrecvfd"
	.2byte	0xb6
	.4byte	0x10
.L_D2698_e:
.L_D2706:
	.4byte	.L_D2706_e-.L_D2706
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2707
	.2byte	0x38
	.string	"f"
	.2byte	0x142
	.4byte	.L_T767
	.2byte	0x72
	.4byte	.L_T768
	.2byte	0x23
	.2byte	.L_l2706_e-.L_l2706
.L_l2706:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2706_e:
.L_D2706_e:
.L_D2707:
	.4byte	.L_D2707_e-.L_D2707
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2708
	.2byte	0x38
	.string	"uid"
	.2byte	0x142
	.4byte	.L_T767
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2707_e-.L_l2707
.L_l2707:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2707_e:
.L_D2707_e:
.L_D2708:
	.4byte	.L_D2708_e-.L_D2708
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2709
	.2byte	0x38
	.string	"gid"
	.2byte	0x142
	.4byte	.L_T767
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2708_e-.L_l2708
.L_l2708:
	.byte	0x4
	.4byte	0x6
	.byte	0x7
.L_l2708_e:
.L_D2708_e:
.L_D2709:
	.4byte	.L_D2709_e-.L_D2709
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2710
	.2byte	0x38
	.string	"fill"
	.2byte	0x142
	.4byte	.L_T767
	.2byte	0x72
	.4byte	.L_T384
	.2byte	0x23
	.2byte	.L_l2709_e-.L_l2709
.L_l2709:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2709_e:
.L_D2709_e:
.L_D2710:
	.4byte	0x4
.L_D2705:
	.4byte	.L_D2705_e-.L_D2705
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D2711
	.set	.L_T768,.L_D2705
	.2byte	0xb6
	.4byte	0x4
.L_D2705_e:
.L_D2712:
	.4byte	.L_D2712_e-.L_D2712
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2713
	.2byte	0x38
	.string	"fp"
	.2byte	0x142
	.4byte	.L_T768
	.2byte	0x83
	.2byte	.L_t2712_e-.L_t2712
.L_t2712:
	.byte	0x1
	.4byte	.L_T354
.L_t2712_e:
	.2byte	0x23
	.2byte	.L_l2712_e-.L_l2712
.L_l2712:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2712_e:
.L_D2712_e:
.L_D2713:
	.4byte	.L_D2713_e-.L_D2713
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2714
	.2byte	0x38
	.string	"fd"
	.2byte	0x142
	.4byte	.L_T768
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2713_e-.L_l2713
.L_l2713:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2713_e:
.L_D2713_e:
.L_D2714:
	.4byte	0x4
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2711:
	.4byte	.L_D2711_e-.L_D2711
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2715
	.set	.L_T771,.L_D2711
	.2byte	0x38
	.string	"e_strrecvfd"
	.2byte	0xb6
	.4byte	0x14
.L_D2711_e:
.L_D2716:
	.4byte	.L_D2716_e-.L_D2716
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2717
	.2byte	0x38
	.string	"f"
	.2byte	0x142
	.4byte	.L_T771
	.2byte	0x72
	.4byte	.L_T772
	.2byte	0x23
	.2byte	.L_l2716_e-.L_l2716
.L_l2716:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2716_e:
.L_D2716_e:
.L_D2717:
	.4byte	.L_D2717_e-.L_D2717
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2718
	.2byte	0x38
	.string	"uid"
	.2byte	0x142
	.4byte	.L_T771
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2717_e-.L_l2717
.L_l2717:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2717_e:
.L_D2717_e:
.L_D2718:
	.4byte	.L_D2718_e-.L_D2718
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2719
	.2byte	0x38
	.string	"gid"
	.2byte	0x142
	.4byte	.L_T771
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2718_e-.L_l2718
.L_l2718:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2718_e:
.L_D2718_e:
.L_D2719:
	.4byte	.L_D2719_e-.L_D2719
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2720
	.2byte	0x38
	.string	"fill"
	.2byte	0x142
	.4byte	.L_T771
	.2byte	0x72
	.4byte	.L_T384
	.2byte	0x23
	.2byte	.L_l2719_e-.L_l2719
.L_l2719:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2719_e:
.L_D2719_e:
.L_D2720:
	.4byte	0x4
.L_D2715:
	.4byte	.L_D2715_e-.L_D2715
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D2721
	.set	.L_T772,.L_D2715
	.2byte	0xb6
	.4byte	0x4
.L_D2715_e:
.L_D2722:
	.4byte	.L_D2722_e-.L_D2722
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2723
	.2byte	0x38
	.string	"fp"
	.2byte	0x142
	.4byte	.L_T772
	.2byte	0x83
	.2byte	.L_t2722_e-.L_t2722
.L_t2722:
	.byte	0x1
	.4byte	.L_T354
.L_t2722_e:
	.2byte	0x23
	.2byte	.L_l2722_e-.L_l2722
.L_l2722:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2722_e:
.L_D2722_e:
.L_D2723:
	.4byte	.L_D2723_e-.L_D2723
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2724
	.2byte	0x38
	.string	"fd"
	.2byte	0x142
	.4byte	.L_T772
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2723_e-.L_l2723
.L_l2723:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2723_e:
.L_D2723_e:
.L_D2724:
	.4byte	0x4
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2721:
	.4byte	.L_D2721_e-.L_D2721
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2725
	.set	.L_T775,.L_D2721
	.2byte	0x38
	.string	"strrecvfd"
	.2byte	0xb6
	.4byte	0x14
.L_D2721_e:
.L_D2726:
	.4byte	.L_D2726_e-.L_D2726
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2727
	.2byte	0x38
	.string	"f"
	.2byte	0x142
	.4byte	.L_T775
	.2byte	0x72
	.4byte	.L_T776
	.2byte	0x23
	.2byte	.L_l2726_e-.L_l2726
.L_l2726:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2726_e:
.L_D2726_e:
.L_D2727:
	.4byte	.L_D2727_e-.L_D2727
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2728
	.2byte	0x38
	.string	"uid"
	.2byte	0x142
	.4byte	.L_T775
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2727_e-.L_l2727
.L_l2727:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2727_e:
.L_D2727_e:
.L_D2728:
	.4byte	.L_D2728_e-.L_D2728
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2729
	.2byte	0x38
	.string	"gid"
	.2byte	0x142
	.4byte	.L_T775
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2728_e-.L_l2728
.L_l2728:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2728_e:
.L_D2728_e:
.L_D2729:
	.4byte	.L_D2729_e-.L_D2729
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2730
	.2byte	0x38
	.string	"fill"
	.2byte	0x142
	.4byte	.L_T775
	.2byte	0x72
	.4byte	.L_T384
	.2byte	0x23
	.2byte	.L_l2729_e-.L_l2729
.L_l2729:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2729_e:
.L_D2729_e:
.L_D2730:
	.4byte	0x4
.L_D2725:
	.4byte	.L_D2725_e-.L_D2725
	.2byte	0x17
	.2byte	0x12
	.4byte	.L_D2731
	.set	.L_T776,.L_D2725
	.2byte	0xb6
	.4byte	0x4
.L_D2725_e:
.L_D2732:
	.4byte	.L_D2732_e-.L_D2732
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2733
	.2byte	0x38
	.string	"fp"
	.2byte	0x142
	.4byte	.L_T776
	.2byte	0x83
	.2byte	.L_t2732_e-.L_t2732
.L_t2732:
	.byte	0x1
	.4byte	.L_T354
.L_t2732_e:
	.2byte	0x23
	.2byte	.L_l2732_e-.L_l2732
.L_l2732:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2732_e:
.L_D2732_e:
.L_D2733:
	.4byte	.L_D2733_e-.L_D2733
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2734
	.2byte	0x38
	.string	"fd"
	.2byte	0x142
	.4byte	.L_T776
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2733_e-.L_l2733
.L_l2733:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2733_e:
.L_D2733_e:
.L_D2734:
	.4byte	0x4
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2731:
	.4byte	.L_D2731_e-.L_D2731
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2735
	.set	.L_T779,.L_D2731
	.2byte	0x38
	.string	"adtrecvfd"
	.2byte	0xb6
	.4byte	0x10
.L_D2731_e:
.L_D2736:
	.4byte	.L_D2736_e-.L_D2736
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2737
	.2byte	0x38
	.string	"adtrfd_sendpid"
	.2byte	0x142
	.4byte	.L_T779
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2736_e-.L_l2736
.L_l2736:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2736_e:
.L_D2736_e:
.L_D2737:
	.4byte	.L_D2737_e-.L_D2737
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2738
	.2byte	0x38
	.string	"adtrfd_sendfd"
	.2byte	0x142
	.4byte	.L_T779
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2737_e-.L_l2737
.L_l2737:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2737_e:
.L_D2737_e:
.L_D2738:
	.4byte	.L_D2738_e-.L_D2738
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2739
	.2byte	0x38
	.string	"adtrfd_recvpid"
	.2byte	0x142
	.4byte	.L_T779
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2738_e-.L_l2738
.L_l2738:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2738_e:
.L_D2738_e:
.L_D2739:
	.4byte	.L_D2739_e-.L_D2739
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2740
	.2byte	0x38
	.string	"adtrfd_recvfd"
	.2byte	0x142
	.4byte	.L_T779
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2739_e-.L_l2739
.L_l2739:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2739_e:
.L_D2739_e:
.L_D2740:
	.4byte	0x4
	.previous

.section	.debug
.L_D2735:
	.4byte	.L_D2735_e-.L_D2735
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2741
	.set	.L_T780,.L_D2735
	.2byte	0x38
	.string	"adt_strrecvfd"
	.2byte	0xb6
	.4byte	0x24
.L_D2735_e:
.L_D2742:
	.4byte	.L_D2742_e-.L_D2742
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2743
	.2byte	0x38
	.string	"adt_strrecvfd"
	.2byte	0x142
	.4byte	.L_T780
	.2byte	0x72
	.4byte	.L_T775
	.2byte	0x23
	.2byte	.L_l2742_e-.L_l2742
.L_l2742:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2742_e:
.L_D2742_e:
.L_D2743:
	.4byte	.L_D2743_e-.L_D2743
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2744
	.2byte	0x38
	.string	"adt_adtrecvfd"
	.2byte	0x142
	.4byte	.L_T780
	.2byte	0x72
	.4byte	.L_T779
	.2byte	0x23
	.2byte	.L_l2743_e-.L_l2743
.L_l2743:
	.byte	0x4
	.4byte	0x14
	.byte	0x7
.L_l2743_e:
.L_D2743_e:
.L_D2744:
	.4byte	0x4
	.previous

.section	.debug
.L_D2741:
	.4byte	.L_D2741_e-.L_D2741
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2745
	.set	.L_T781,.L_D2741
	.2byte	0x38
	.string	"s_strrecvfd"
	.2byte	0xb6
	.4byte	0x10
.L_D2741_e:
.L_D2746:
	.4byte	.L_D2746_e-.L_D2746
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2747
	.2byte	0x38
	.string	"fd"
	.2byte	0x142
	.4byte	.L_T781
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2746_e-.L_l2746
.L_l2746:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2746_e:
.L_D2746_e:
.L_D2747:
	.4byte	.L_D2747_e-.L_D2747
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2748
	.2byte	0x38
	.string	"uid"
	.2byte	0x142
	.4byte	.L_T781
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2747_e-.L_l2747
.L_l2747:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2747_e:
.L_D2747_e:
.L_D2748:
	.4byte	.L_D2748_e-.L_D2748
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2749
	.2byte	0x38
	.string	"gid"
	.2byte	0x142
	.4byte	.L_T781
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2748_e-.L_l2748
.L_l2748:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2748_e:
.L_D2748_e:
.L_D2749:
	.4byte	.L_D2749_e-.L_D2749
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2750
	.2byte	0x38
	.string	"s_attrs"
	.2byte	0x142
	.4byte	.L_T781
	.2byte	0x72
	.4byte	.L_T760
	.2byte	0x23
	.2byte	.L_l2749_e-.L_l2749
.L_l2749:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2749_e:
.L_D2749_e:
.L_D2750:
	.4byte	0x4
	.previous

.section	.debug
.L_D2745:
	.4byte	.L_D2745_e-.L_D2745
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2751
	.set	.L_T782,.L_D2745
	.2byte	0x38
	.string	"str_mlist"
	.2byte	0xb6
	.4byte	0x9
.L_D2745_e:
.L_D2752:
	.4byte	.L_D2752_e-.L_D2752
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2753
	.2byte	0x38
	.string	"l_name"
	.2byte	0x142
	.4byte	.L_T782
	.2byte	0x72
	.4byte	.L_T461
	.2byte	0x23
	.2byte	.L_l2752_e-.L_l2752
.L_l2752:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2752_e:
.L_D2752_e:
.L_D2753:
	.4byte	0x4
	.previous

.section	.debug
.L_D2751:
	.4byte	.L_D2751_e-.L_D2751
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2754
	.set	.L_T785,.L_D2751
	.2byte	0x38
	.string	"str_list"
	.2byte	0xb6
	.4byte	0x8
.L_D2751_e:
.L_D2755:
	.4byte	.L_D2755_e-.L_D2755
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2756
	.2byte	0x38
	.string	"sl_nmods"
	.2byte	0x142
	.4byte	.L_T785
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2755_e-.L_l2755
.L_l2755:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2755_e:
.L_D2755_e:
.L_D2756:
	.4byte	.L_D2756_e-.L_D2756
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2757
	.2byte	0x38
	.string	"sl_modlist"
	.2byte	0x142
	.4byte	.L_T785
	.2byte	0x83
	.2byte	.L_t2756_e-.L_t2756
.L_t2756:
	.byte	0x1
	.4byte	.L_T782
.L_t2756_e:
	.2byte	0x23
	.2byte	.L_l2756_e-.L_l2756
.L_l2756:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2756_e:
.L_D2756_e:
.L_D2757:
	.4byte	0x4
	.previous

.section	.debug
.L_D2754:
	.4byte	.L_D2754_e-.L_D2754
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2758
	.set	.L_T787,.L_D2754
	.2byte	0x38
	.string	"bandinfo"
	.2byte	0xb6
	.4byte	0x8
.L_D2754_e:
.L_D2759:
	.4byte	.L_D2759_e-.L_D2759
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2760
	.2byte	0x38
	.string	"bi_pri"
	.2byte	0x142
	.4byte	.L_T787
	.2byte	0x55
	.2byte	0x3
	.2byte	0x23
	.2byte	.L_l2759_e-.L_l2759
.L_l2759:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2759_e:
.L_D2759_e:
.L_D2760:
	.4byte	.L_D2760_e-.L_D2760
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2761
	.2byte	0x38
	.string	"bi_flag"
	.2byte	0x142
	.4byte	.L_T787
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2760_e-.L_l2760
.L_l2760:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2760_e:
.L_D2760_e:
.L_D2761:
	.4byte	0x4
	.previous

.section	.debug
	.previous

.section	.debug
.L_D2758:
	.4byte	.L_D2758_e-.L_D2758
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2762
	.set	.L_T792,.L_D2758
	.2byte	0xb6
	.4byte	0x10
.L_D2758_e:
.L_D2763:
	.4byte	.L_D2763_e-.L_D2763
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2764
	.2byte	0x38
	.string	"_eucw1"
	.2byte	0x142
	.4byte	.L_T792
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2763_e-.L_l2763
.L_l2763:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2763_e:
.L_D2763_e:
.L_D2764:
	.4byte	.L_D2764_e-.L_D2764
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2765
	.2byte	0x38
	.string	"_eucw2"
	.2byte	0x142
	.4byte	.L_T792
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2764_e-.L_l2764
.L_l2764:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l2764_e:
.L_D2764_e:
.L_D2765:
	.4byte	.L_D2765_e-.L_D2765
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2766
	.2byte	0x38
	.string	"_eucw3"
	.2byte	0x142
	.4byte	.L_T792
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2765_e-.L_l2765
.L_l2765:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2765_e:
.L_D2765_e:
.L_D2766:
	.4byte	.L_D2766_e-.L_D2766
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2767
	.2byte	0x38
	.string	"_scrw1"
	.2byte	0x142
	.4byte	.L_T792
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2766_e-.L_l2766
.L_l2766:
	.byte	0x4
	.4byte	0x6
	.byte	0x7
.L_l2766_e:
.L_D2766_e:
.L_D2767:
	.4byte	.L_D2767_e-.L_D2767
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2768
	.2byte	0x38
	.string	"_scrw2"
	.2byte	0x142
	.4byte	.L_T792
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2767_e-.L_l2767
.L_l2767:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2767_e:
.L_D2767_e:
.L_D2768:
	.4byte	.L_D2768_e-.L_D2768
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2769
	.2byte	0x38
	.string	"_scrw3"
	.2byte	0x142
	.4byte	.L_T792
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2768_e-.L_l2768
.L_l2768:
	.byte	0x4
	.4byte	0xa
	.byte	0x7
.L_l2768_e:
.L_D2768_e:
.L_D2769:
	.4byte	.L_D2769_e-.L_D2769
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2770
	.2byte	0x38
	.string	"_pcw"
	.2byte	0x142
	.4byte	.L_T792
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2769_e-.L_l2769
.L_l2769:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2769_e:
.L_D2769_e:
.L_D2770:
	.4byte	.L_D2770_e-.L_D2770
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2771
	.2byte	0x38
	.string	"_multibyte"
	.2byte	0x142
	.4byte	.L_T792
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2770_e-.L_l2770
.L_l2770:
	.byte	0x4
	.4byte	0xe
	.byte	0x7
.L_l2770_e:
.L_D2770_e:
.L_D2771:
	.4byte	0x4
	.previous

.section	.debug
.L_D2762:
	.4byte	.L_D2762_e-.L_D2762
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2772
	.2byte	0x38
	.string	"eucwidth_t"
	.2byte	0x72
	.4byte	.L_T792
.L_D2762_e:
	.previous

.section	.debug
.L_D2772:
	.4byte	.L_D2772_e-.L_D2772
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2773
	.set	.L_T794,.L_D2772
	.2byte	0x38
	.string	"eucioc"
	.2byte	0xb6
	.4byte	0x8
.L_D2772_e:
.L_D2774:
	.4byte	.L_D2774_e-.L_D2774
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2775
	.2byte	0x38
	.string	"eucw"
	.2byte	0x142
	.4byte	.L_T794
	.2byte	0x72
	.4byte	.L_T796
	.2byte	0x23
	.2byte	.L_l2774_e-.L_l2774
.L_l2774:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2774_e:
.L_D2774_e:
.L_D2775:
	.4byte	.L_D2775_e-.L_D2775
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2776
	.2byte	0x38
	.string	"scrw"
	.2byte	0x142
	.4byte	.L_T794
	.2byte	0x72
	.4byte	.L_T796
	.2byte	0x23
	.2byte	.L_l2775_e-.L_l2775
.L_l2775:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2775_e:
.L_D2775_e:
.L_D2776:
	.4byte	0x4
.L_D2773:
	.4byte	.L_D2773_e-.L_D2773
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2777
	.set	.L_T796,.L_D2773
	.2byte	0xa3
	.2byte	.L_s2773_e-.L_s2773
.L_s2773:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x3
	.byte	0x8
	.2byte	0x55
	.2byte	0x3
.L_s2773_e:
.L_D2773_e:
	.previous

.section	.debug
.L_D2777:
	.4byte	.L_D2777_e-.L_D2777
	.2byte	0x16
	.2byte	0x12
	.4byte	.L_D2778
	.2byte	0x38
	.string	"eucioc_t"
	.2byte	0x72
	.4byte	.L_T794
.L_D2777_e:
	.previous

.section	.debug
.L_D2778:
	.4byte	.L_D2778_e-.L_D2778
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2779
	.set	.L_T800,.L_D2778
	.2byte	0x38
	.string	"t_buf"
	.2byte	0xb6
	.4byte	0xc
.L_D2778_e:
.L_D2780:
	.4byte	.L_D2780_e-.L_D2780
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2781
	.2byte	0x38
	.string	"bu_bp"
	.2byte	0x142
	.4byte	.L_T800
	.2byte	0x83
	.2byte	.L_t2780_e-.L_t2780
.L_t2780:
	.byte	0x1
	.4byte	.L_T667
.L_t2780_e:
	.2byte	0x23
	.2byte	.L_l2780_e-.L_l2780
.L_l2780:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2780_e:
.L_D2780_e:
.L_D2781:
	.4byte	.L_D2781_e-.L_D2781
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2782
	.2byte	0x38
	.string	"bu_ptr"
	.2byte	0x142
	.4byte	.L_T800
	.2byte	0x63
	.2byte	.L_t2781_e-.L_t2781
.L_t2781:
	.byte	0x1
	.2byte	0x3
.L_t2781_e:
	.2byte	0x23
	.2byte	.L_l2781_e-.L_l2781
.L_l2781:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2781_e:
.L_D2781_e:
.L_D2782:
	.4byte	.L_D2782_e-.L_D2782
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2783
	.2byte	0x38
	.string	"bu_cnt"
	.2byte	0x142
	.4byte	.L_T800
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2782_e-.L_l2782
.L_l2782:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2782_e:
.L_D2782_e:
.L_D2783:
	.4byte	0x4
	.previous

.section	.debug
.L_D2779:
	.4byte	.L_D2779_e-.L_D2779
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2784
	.set	.L_T801,.L_D2779
	.2byte	0x38
	.string	"strtty"
	.2byte	0xb6
	.4byte	0x50
.L_D2779_e:
.L_D2785:
	.4byte	.L_D2785_e-.L_D2785
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2786
	.2byte	0x38
	.string	"t_in"
	.2byte	0x142
	.4byte	.L_T801
	.2byte	0x72
	.4byte	.L_T800
	.2byte	0x23
	.2byte	.L_l2785_e-.L_l2785
.L_l2785:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2785_e:
.L_D2785_e:
.L_D2786:
	.4byte	.L_D2786_e-.L_D2786
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2787
	.2byte	0x38
	.string	"t_out"
	.2byte	0x142
	.4byte	.L_T801
	.2byte	0x72
	.4byte	.L_T800
	.2byte	0x23
	.2byte	.L_l2786_e-.L_l2786
.L_l2786:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2786_e:
.L_D2786_e:
.L_D2787:
	.4byte	.L_D2787_e-.L_D2787
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2788
	.2byte	0x38
	.string	"t_rdqp"
	.2byte	0x142
	.4byte	.L_T801
	.2byte	0x83
	.2byte	.L_t2787_e-.L_t2787
.L_t2787:
	.byte	0x1
	.4byte	.L_T664
.L_t2787_e:
	.2byte	0x23
	.2byte	.L_l2787_e-.L_l2787
.L_l2787:
	.byte	0x4
	.4byte	0x18
	.byte	0x7
.L_l2787_e:
.L_D2787_e:
.L_D2788:
	.4byte	.L_D2788_e-.L_D2788
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2789
	.2byte	0x38
	.string	"t_ioctlp"
	.2byte	0x142
	.4byte	.L_T801
	.2byte	0x83
	.2byte	.L_t2788_e-.L_t2788
.L_t2788:
	.byte	0x1
	.4byte	.L_T667
.L_t2788_e:
	.2byte	0x23
	.2byte	.L_l2788_e-.L_l2788
.L_l2788:
	.byte	0x4
	.4byte	0x1c
	.byte	0x7
.L_l2788_e:
.L_D2788_e:
.L_D2789:
	.4byte	.L_D2789_e-.L_D2789
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2790
	.2byte	0x38
	.string	"t_lbuf"
	.2byte	0x142
	.4byte	.L_T801
	.2byte	0x83
	.2byte	.L_t2789_e-.L_t2789
.L_t2789:
	.byte	0x1
	.4byte	.L_T667
.L_t2789_e:
	.2byte	0x23
	.2byte	.L_l2789_e-.L_l2789
.L_l2789:
	.byte	0x4
	.4byte	0x20
	.byte	0x7
.L_l2789_e:
.L_D2789_e:
.L_D2790:
	.4byte	.L_D2790_e-.L_D2790
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2791
	.2byte	0x38
	.string	"t_dev"
	.2byte	0x142
	.4byte	.L_T801
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2790_e-.L_l2790
.L_l2790:
	.byte	0x4
	.4byte	0x24
	.byte	0x7
.L_l2790_e:
.L_D2790_e:
.L_D2791:
	.4byte	.L_D2791_e-.L_D2791
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2792
	.2byte	0x38
	.string	"t_iflag"
	.2byte	0x142
	.4byte	.L_T801
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2791_e-.L_l2791
.L_l2791:
	.byte	0x4
	.4byte	0x28
	.byte	0x7
.L_l2791_e:
.L_D2791_e:
.L_D2792:
	.4byte	.L_D2792_e-.L_D2792
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2793
	.2byte	0x38
	.string	"t_oflag"
	.2byte	0x142
	.4byte	.L_T801
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2792_e-.L_l2792
.L_l2792:
	.byte	0x4
	.4byte	0x2c
	.byte	0x7
.L_l2792_e:
.L_D2792_e:
.L_D2793:
	.4byte	.L_D2793_e-.L_D2793
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2794
	.2byte	0x38
	.string	"t_cflag"
	.2byte	0x142
	.4byte	.L_T801
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2793_e-.L_l2793
.L_l2793:
	.byte	0x4
	.4byte	0x30
	.byte	0x7
.L_l2793_e:
.L_D2793_e:
.L_D2794:
	.4byte	.L_D2794_e-.L_D2794
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2795
	.2byte	0x38
	.string	"t_lflag"
	.2byte	0x142
	.4byte	.L_T801
	.2byte	0x55
	.2byte	0xa
	.2byte	0x23
	.2byte	.L_l2794_e-.L_l2794
.L_l2794:
	.byte	0x4
	.4byte	0x34
	.byte	0x7
.L_l2794_e:
.L_D2794_e:
.L_D2795:
	.4byte	.L_D2795_e-.L_D2795
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2796
	.2byte	0x38
	.string	"t_state"
	.2byte	0x142
	.4byte	.L_T801
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2795_e-.L_l2795
.L_l2795:
	.byte	0x4
	.4byte	0x38
	.byte	0x7
.L_l2795_e:
.L_D2795_e:
.L_D2796:
	.4byte	.L_D2796_e-.L_D2796
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2797
	.2byte	0x38
	.string	"t_line"
	.2byte	0x142
	.4byte	.L_T801
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2796_e-.L_l2796
.L_l2796:
	.byte	0x4
	.4byte	0x3a
	.byte	0x7
.L_l2796_e:
.L_D2796_e:
.L_D2797:
	.4byte	.L_D2797_e-.L_D2797
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2798
	.2byte	0x38
	.string	"t_dstat"
	.2byte	0x142
	.4byte	.L_T801
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2797_e-.L_l2797
.L_l2797:
	.byte	0x4
	.4byte	0x3b
	.byte	0x7
.L_l2797_e:
.L_D2797_e:
.L_D2798:
	.4byte	.L_D2798_e-.L_D2798
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2799
	.2byte	0x38
	.string	"t_cc"
	.2byte	0x142
	.4byte	.L_T801
	.2byte	0x72
	.4byte	.L_T803
	.2byte	0x23
	.2byte	.L_l2798_e-.L_l2798
.L_l2798:
	.byte	0x4
	.4byte	0x3c
	.byte	0x7
.L_l2798_e:
.L_D2798_e:
.L_D2799:
	.4byte	0x4
.L_D2784:
	.4byte	.L_D2784_e-.L_D2784
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2800
	.set	.L_T803,.L_D2784
	.2byte	0xa3
	.2byte	.L_s2784_e-.L_s2784
.L_s2784:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x12
	.byte	0x8
	.2byte	0x55
	.2byte	0x3
.L_s2784_e:
.L_D2784_e:
	.previous

.section	.debug
.L_D2800:
	.4byte	.L_D2800_e-.L_D2800
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2801
	.set	.L_T804,.L_D2800
	.2byte	0x38
	.string	"stio"
	.2byte	0xb6
	.4byte	0x10
.L_D2800_e:
.L_D2802:
	.4byte	.L_D2802_e-.L_D2802
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2803
	.2byte	0x38
	.string	"ttyid"
	.2byte	0x142
	.4byte	.L_T804
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2802_e-.L_l2802
.L_l2802:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2802_e:
.L_D2802_e:
.L_D2803:
	.4byte	.L_D2803_e-.L_D2803
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2804
	.2byte	0x38
	.string	"row"
	.2byte	0x142
	.4byte	.L_T804
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2803_e-.L_l2803
.L_l2803:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l2803_e:
.L_D2803_e:
.L_D2804:
	.4byte	.L_D2804_e-.L_D2804
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2805
	.2byte	0x38
	.string	"col"
	.2byte	0x142
	.4byte	.L_T804
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2804_e-.L_l2804
.L_l2804:
	.byte	0x4
	.4byte	0x3
	.byte	0x7
.L_l2804_e:
.L_D2804_e:
.L_D2805:
	.4byte	.L_D2805_e-.L_D2805
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2806
	.2byte	0x38
	.string	"orow"
	.2byte	0x142
	.4byte	.L_T804
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2805_e-.L_l2805
.L_l2805:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2805_e:
.L_D2805_e:
.L_D2806:
	.4byte	.L_D2806_e-.L_D2806
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2807
	.2byte	0x38
	.string	"ocol"
	.2byte	0x142
	.4byte	.L_T804
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2806_e-.L_l2806
.L_l2806:
	.byte	0x4
	.4byte	0x5
	.byte	0x7
.L_l2806_e:
.L_D2806_e:
.L_D2807:
	.4byte	.L_D2807_e-.L_D2807
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2808
	.2byte	0x38
	.string	"tab"
	.2byte	0x142
	.4byte	.L_T804
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2807_e-.L_l2807
.L_l2807:
	.byte	0x4
	.4byte	0x6
	.byte	0x7
.L_l2807_e:
.L_D2807_e:
.L_D2808:
	.4byte	.L_D2808_e-.L_D2808
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2809
	.2byte	0x38
	.string	"aid"
	.2byte	0x142
	.4byte	.L_T804
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2808_e-.L_l2808
.L_l2808:
	.byte	0x4
	.4byte	0x7
	.byte	0x7
.L_l2808_e:
.L_D2808_e:
.L_D2809:
	.4byte	.L_D2809_e-.L_D2809
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2810
	.2byte	0x38
	.string	"ss1"
	.2byte	0x142
	.4byte	.L_T804
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2809_e-.L_l2809
.L_l2809:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2809_e:
.L_D2809_e:
.L_D2810:
	.4byte	.L_D2810_e-.L_D2810
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2811
	.2byte	0x38
	.string	"ss2"
	.2byte	0x142
	.4byte	.L_T804
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2810_e-.L_l2810
.L_l2810:
	.byte	0x4
	.4byte	0x9
	.byte	0x7
.L_l2810_e:
.L_D2810_e:
.L_D2811:
	.4byte	.L_D2811_e-.L_D2811
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2812
	.2byte	0x38
	.string	"imode"
	.2byte	0x142
	.4byte	.L_T804
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2811_e-.L_l2811
.L_l2811:
	.byte	0x4
	.4byte	0xa
	.byte	0x7
.L_l2811_e:
.L_D2811_e:
.L_D2812:
	.4byte	.L_D2812_e-.L_D2812
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2813
	.2byte	0x38
	.string	"lmode"
	.2byte	0x142
	.4byte	.L_T804
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2812_e-.L_l2812
.L_l2812:
	.byte	0x4
	.4byte	0xc
	.byte	0x7
.L_l2812_e:
.L_D2812_e:
.L_D2813:
	.4byte	.L_D2813_e-.L_D2813
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2814
	.2byte	0x38
	.string	"omode"
	.2byte	0x142
	.4byte	.L_T804
	.2byte	0x55
	.2byte	0x6
	.2byte	0x23
	.2byte	.L_l2813_e-.L_l2813
.L_l2813:
	.byte	0x4
	.4byte	0xe
	.byte	0x7
.L_l2813_e:
.L_D2813_e:
.L_D2814:
	.4byte	0x4
	.previous

.section	.debug
.L_D2801:
	.4byte	.L_D2801_e-.L_D2801
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2815
	.set	.L_T805,.L_D2801
	.2byte	0x38
	.string	"sttsv"
	.2byte	0xb6
	.4byte	0xc
.L_D2801_e:
.L_D2816:
	.4byte	.L_D2816_e-.L_D2816
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2817
	.2byte	0x38
	.string	"st_major"
	.2byte	0x142
	.4byte	.L_T805
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2816_e-.L_l2816
.L_l2816:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2816_e:
.L_D2816_e:
.L_D2817:
	.4byte	.L_D2817_e-.L_D2817
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2818
	.2byte	0x38
	.string	"st_pcdnum"
	.2byte	0x142
	.4byte	.L_T805
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2817_e-.L_l2817
.L_l2817:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l2817_e:
.L_D2817_e:
.L_D2818:
	.4byte	.L_D2818_e-.L_D2818
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2819
	.2byte	0x38
	.string	"st_devaddr"
	.2byte	0x142
	.4byte	.L_T805
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2818_e-.L_l2818
.L_l2818:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2818_e:
.L_D2818_e:
.L_D2819:
	.4byte	.L_D2819_e-.L_D2819
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2820
	.2byte	0x38
	.string	"st_csidev"
	.2byte	0x142
	.4byte	.L_T805
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2819_e-.L_l2819
.L_l2819:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2819_e:
.L_D2819_e:
.L_D2820:
	.4byte	0x4
	.previous

.section	.debug
.L_D2815:
	.4byte	.L_D2815_e-.L_D2815
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2821
	.set	.L_T806,.L_D2815
	.2byte	0x38
	.string	"stcntrs"
	.2byte	0xb6
	.4byte	0x6
.L_D2815_e:
.L_D2822:
	.4byte	.L_D2822_e-.L_D2822
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2823
	.2byte	0x38
	.string	"st_lrc"
	.2byte	0x142
	.4byte	.L_T806
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2822_e-.L_l2822
.L_l2822:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2822_e:
.L_D2822_e:
.L_D2823:
	.4byte	.L_D2823_e-.L_D2823
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2824
	.2byte	0x38
	.string	"st_xnaks"
	.2byte	0x142
	.4byte	.L_T806
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2823_e-.L_l2823
.L_l2823:
	.byte	0x4
	.4byte	0x1
	.byte	0x7
.L_l2823_e:
.L_D2823_e:
.L_D2824:
	.4byte	.L_D2824_e-.L_D2824
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2825
	.2byte	0x38
	.string	"st_rnaks"
	.2byte	0x142
	.4byte	.L_T806
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2824_e-.L_l2824
.L_l2824:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l2824_e:
.L_D2824_e:
.L_D2825:
	.4byte	.L_D2825_e-.L_D2825
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2826
	.2byte	0x38
	.string	"st_xwaks"
	.2byte	0x142
	.4byte	.L_T806
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2825_e-.L_l2825
.L_l2825:
	.byte	0x4
	.4byte	0x3
	.byte	0x7
.L_l2825_e:
.L_D2825_e:
.L_D2826:
	.4byte	.L_D2826_e-.L_D2826
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2827
	.2byte	0x38
	.string	"st_rwaks"
	.2byte	0x142
	.4byte	.L_T806
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2826_e-.L_l2826
.L_l2826:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2826_e:
.L_D2826_e:
.L_D2827:
	.4byte	.L_D2827_e-.L_D2827
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2828
	.2byte	0x38
	.string	"st_scc"
	.2byte	0x142
	.4byte	.L_T806
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2827_e-.L_l2827
.L_l2827:
	.byte	0x4
	.4byte	0x5
	.byte	0x7
.L_l2827_e:
.L_D2827_e:
.L_D2828:
	.4byte	0x4
	.previous

.section	.debug
.L_D2821:
	.4byte	.L_D2821_e-.L_D2821
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2829
	.set	.L_T807,.L_D2821
	.2byte	0x38
	.string	"jwinsize"
	.2byte	0xb6
	.4byte	0x6
.L_D2821_e:
.L_D2830:
	.4byte	.L_D2830_e-.L_D2830
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2831
	.2byte	0x38
	.string	"bytesx"
	.2byte	0x142
	.4byte	.L_T807
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2830_e-.L_l2830
.L_l2830:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2830_e:
.L_D2830_e:
.L_D2831:
	.4byte	.L_D2831_e-.L_D2831
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2832
	.2byte	0x38
	.string	"bytesy"
	.2byte	0x142
	.4byte	.L_T807
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2831_e-.L_l2831
.L_l2831:
	.byte	0x4
	.4byte	0x1
	.byte	0x7
.L_l2831_e:
.L_D2831_e:
.L_D2832:
	.4byte	.L_D2832_e-.L_D2832
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2833
	.2byte	0x38
	.string	"bitsx"
	.2byte	0x142
	.4byte	.L_T807
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2832_e-.L_l2832
.L_l2832:
	.byte	0x4
	.4byte	0x2
	.byte	0x7
.L_l2832_e:
.L_D2832_e:
.L_D2833:
	.4byte	.L_D2833_e-.L_D2833
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2834
	.2byte	0x38
	.string	"bitsy"
	.2byte	0x142
	.4byte	.L_T807
	.2byte	0x55
	.2byte	0x4
	.2byte	0x23
	.2byte	.L_l2833_e-.L_l2833
.L_l2833:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2833_e:
.L_D2833_e:
.L_D2834:
	.4byte	0x4
	.previous

.section	.debug
.L_D2829:
	.4byte	.L_D2829_e-.L_D2829
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2835
	.set	.L_T808,.L_D2829
	.2byte	0x38
	.string	"jerqmesg"
	.2byte	0xb6
	.4byte	0x2
.L_D2829_e:
.L_D2836:
	.4byte	.L_D2836_e-.L_D2836
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2837
	.2byte	0x38
	.string	"cmd"
	.2byte	0x142
	.4byte	.L_T808
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2836_e-.L_l2836
.L_l2836:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2836_e:
.L_D2836_e:
.L_D2837:
	.4byte	.L_D2837_e-.L_D2837
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2838
	.2byte	0x38
	.string	"chan"
	.2byte	0x142
	.4byte	.L_T808
	.2byte	0x55
	.2byte	0x1
	.2byte	0x23
	.2byte	.L_l2837_e-.L_l2837
.L_l2837:
	.byte	0x4
	.4byte	0x1
	.byte	0x7
.L_l2837_e:
.L_D2837_e:
.L_D2838:
	.4byte	0x4
	.previous

.section	.debug
.L_D2835:
	.4byte	.L_D2835_e-.L_D2835
	.2byte	0x13
	.2byte	0x12
	.4byte	.L_D2839
	.set	.L_T809,.L_D2835
	.2byte	0x38
	.string	"bagent"
	.2byte	0xb6
	.4byte	0xc
.L_D2835_e:
.L_D2840:
	.4byte	.L_D2840_e-.L_D2840
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2841
	.2byte	0x38
	.string	"size"
	.2byte	0x142
	.4byte	.L_T809
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2840_e-.L_l2840
.L_l2840:
	.byte	0x4
	.4byte	0x0
	.byte	0x7
.L_l2840_e:
.L_D2840_e:
.L_D2841:
	.4byte	.L_D2841_e-.L_D2841
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2842
	.2byte	0x38
	.string	"src"
	.2byte	0x142
	.4byte	.L_T809
	.2byte	0x63
	.2byte	.L_t2841_e-.L_t2841
.L_t2841:
	.byte	0x1
	.2byte	0x1
.L_t2841_e:
	.2byte	0x23
	.2byte	.L_l2841_e-.L_l2841
.L_l2841:
	.byte	0x4
	.4byte	0x4
	.byte	0x7
.L_l2841_e:
.L_D2841_e:
.L_D2842:
	.4byte	.L_D2842_e-.L_D2842
	.2byte	0xd
	.2byte	0x12
	.4byte	.L_D2843
	.2byte	0x38
	.string	"dest"
	.2byte	0x142
	.4byte	.L_T809
	.2byte	0x63
	.2byte	.L_t2842_e-.L_t2842
.L_t2842:
	.byte	0x1
	.2byte	0x1
.L_t2842_e:
	.2byte	0x23
	.2byte	.L_l2842_e-.L_l2842
.L_l2842:
	.byte	0x4
	.4byte	0x8
	.byte	0x7
.L_l2842_e:
.L_D2842_e:
.L_D2843:
	.4byte	0x4
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
	.previous

.section	.debug
.L_P13:
.L_D2839:
	.4byte	.L_D2839_e-.L_D2839
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D2844
	.2byte	0x38
	.string	"sas_physical_units"
	.2byte	0x55
	.2byte	0x9
.L_D2839_e:
	.previous

.section	.debug
.L_D2844:
	.4byte	.L_D2844_e-.L_D2844
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2845
	.set	.L_T880,.L_D2844
	.2byte	0xa3
	.2byte	.L_s2844_e-.L_s2844
.L_s2844:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x9
.L_s2844_e:
.L_D2844_e:
.L_P14:
.L_D2845:
	.4byte	.L_D2845_e-.L_D2845
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D2846
	.2byte	0x38
	.string	"sas_port"
	.2byte	0x72
	.4byte	.L_T880
.L_D2845_e:
	.previous

.section	.debug
.L_D2846:
	.4byte	.L_D2846_e-.L_D2846
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2847
	.set	.L_T882,.L_D2846
	.2byte	0xa3
	.2byte	.L_s2846_e-.L_s2846
.L_s2846:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x9
.L_s2846_e:
.L_D2846_e:
.L_P15:
.L_D2847:
	.4byte	.L_D2847_e-.L_D2847
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D2848
	.2byte	0x38
	.string	"sas_vec"
	.2byte	0x72
	.4byte	.L_T882
.L_D2847_e:
	.previous

.section	.debug
.L_D2848:
	.4byte	.L_D2848_e-.L_D2848
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2849
	.set	.L_T883,.L_D2848
	.2byte	0xa3
	.2byte	.L_s2848_e-.L_s2848
.L_s2848:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x9
.L_s2848_e:
.L_D2848_e:
.L_P16:
.L_D2849:
	.4byte	.L_D2849_e-.L_D2849
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D2850
	.2byte	0x38
	.string	"sas_init_seq"
	.2byte	0x72
	.4byte	.L_T883
.L_D2849_e:
	.previous

.section	.debug
.L_D2850:
	.4byte	.L_D2850_e-.L_D2850
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2851
	.set	.L_T884,.L_D2850
	.2byte	0xa3
	.2byte	.L_s2850_e-.L_s2850
.L_s2850:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x9
.L_s2850_e:
.L_D2850_e:
.L_P17:
.L_D2851:
	.4byte	.L_D2851_e-.L_D2851
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D2852
	.2byte	0x38
	.string	"sas_mcb"
	.2byte	0x72
	.4byte	.L_T884
.L_D2851_e:
	.previous

.section	.debug
.L_D2852:
	.4byte	.L_D2852_e-.L_D2852
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2853
	.set	.L_T885,.L_D2852
	.2byte	0xa3
	.2byte	.L_s2852_e-.L_s2852
.L_s2852:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x9
.L_s2852_e:
.L_D2852_e:
.L_P18:
.L_D2853:
	.4byte	.L_D2853_e-.L_D2853
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D2854
	.2byte	0x38
	.string	"sas_modem"
	.2byte	0x72
	.4byte	.L_T885
.L_D2853_e:
	.previous

.section	.debug
.L_D2854:
	.4byte	.L_D2854_e-.L_D2854
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2855
	.set	.L_T886,.L_D2854
	.2byte	0xa3
	.2byte	.L_s2854_e-.L_s2854
.L_s2854:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x9
.L_s2854_e:
.L_D2854_e:
.L_P19:
.L_D2855:
	.4byte	.L_D2855_e-.L_D2855
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D2856
	.2byte	0x38
	.string	"sas_flow"
	.2byte	0x72
	.4byte	.L_T886
.L_D2855_e:
	.previous

.section	.debug
.L_D2856:
	.4byte	.L_D2856_e-.L_D2856
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2857
	.set	.L_T887,.L_D2856
	.2byte	0xa3
	.2byte	.L_s2856_e-.L_s2856
.L_s2856:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x9
.L_s2856_e:
.L_D2856_e:
.L_P20:
.L_D2857:
	.4byte	.L_D2857_e-.L_D2857
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D2858
	.2byte	0x38
	.string	"sas_ctl_port"
	.2byte	0x72
	.4byte	.L_T887
.L_D2857_e:
	.previous

.section	.debug
.L_D2858:
	.4byte	.L_D2858_e-.L_D2858
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2859
	.set	.L_T888,.L_D2858
	.2byte	0xa3
	.2byte	.L_s2858_e-.L_s2858
.L_s2858:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x9
.L_s2858_e:
.L_D2858_e:
.L_P21:
.L_D2859:
	.4byte	.L_D2859_e-.L_D2859
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D2860
	.2byte	0x38
	.string	"sas_ctl_val"
	.2byte	0x72
	.4byte	.L_T888
.L_D2859_e:
	.previous

.section	.debug
.L_D2860:
	.4byte	.L_D2860_e-.L_D2860
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2861
	.set	.L_T889,.L_D2860
	.2byte	0xa3
	.2byte	.L_s2860_e-.L_s2860
.L_s2860:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x9
.L_s2860_e:
.L_D2860_e:
.L_P22:
.L_D2861:
	.4byte	.L_D2861_e-.L_D2861
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D2862
	.2byte	0x38
	.string	"sas_int_ack_port"
	.2byte	0x72
	.4byte	.L_T889
.L_D2861_e:
	.previous

.section	.debug
.L_D2862:
	.4byte	.L_D2862_e-.L_D2862
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2863
	.set	.L_T890,.L_D2862
	.2byte	0xa3
	.2byte	.L_s2862_e-.L_s2862
.L_s2862:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x0
	.byte	0x8
	.2byte	0x55
	.2byte	0x9
.L_s2862_e:
.L_D2862_e:
.L_P23:
.L_D2863:
	.4byte	.L_D2863_e-.L_D2863
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D2864
	.2byte	0x38
	.string	"sas_int_ack"
	.2byte	0x72
	.4byte	.L_T890
.L_D2863_e:
	.previous

.section	.debug
.L_D2864:
	.4byte	.L_D2864_e-.L_D2864
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2865
	.set	.L_T891,.L_D2864
	.2byte	0xa3
	.2byte	.L_s2864_e-.L_s2864
.L_s2864:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x0
	.byte	0x8
	.2byte	0x72
	.4byte	.L_T816
.L_s2864_e:
.L_D2864_e:
.L_P24:
.L_D2865:
	.4byte	.L_D2865_e-.L_D2865
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D2866
	.2byte	0x38
	.string	"sas_info"
	.2byte	0x72
	.4byte	.L_T891
.L_D2865_e:
	.previous

.section	.debug
.L_D2866:
	.4byte	.L_D2866_e-.L_D2866
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2867
	.set	.L_T893,.L_D2866
	.2byte	0xa3
	.2byte	.L_s2866_e-.L_s2866
.L_s2866:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x0
	.byte	0x8
	.2byte	0x83
	.2byte	.L_t2866_e-.L_t2866
.L_t2866:
	.byte	0x1
	.4byte	.L_T816
.L_t2866_e:
.L_s2866_e:
.L_D2866_e:
.L_P25:
.L_D2867:
	.4byte	.L_D2867_e-.L_D2867
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D2868
	.2byte	0x38
	.string	"sas_info_ptr"
	.2byte	0x72
	.4byte	.L_T893
.L_D2867_e:
	.previous

.section	.debug
.L_D2868:
	.4byte	.L_D2868_e-.L_D2868
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D2869
	.2byte	0x38
	.string	"sas_is_initted"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2868_e-.L_l2868
.L_l2868:
	.byte	0x3
	.4byte	sas_is_initted
.L_l2868_e:
.L_D2868_e:
	.previous

.section	.debug
.L_D2869:
	.4byte	.L_D2869_e-.L_D2869
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D2870
	.2byte	0x38
	.string	"event_scheduled"
	.2byte	0x55
	.2byte	0x7
	.2byte	0x23
	.2byte	.L_l2869_e-.L_l2869
.L_l2869:
	.byte	0x3
	.4byte	event_scheduled
.L_l2869_e:
.L_D2869_e:
	.previous
	.local	sas_first_int_user
	.comm	sas_first_int_user,128,4

.section	.debug
.L_D2870:
	.4byte	.L_D2870_e-.L_D2870
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2871
	.set	.L_T898,.L_D2870
	.2byte	0xa3
	.2byte	.L_s2870_e-.L_s2870
.L_s2870:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0x1f
	.byte	0x8
	.2byte	0x83
	.2byte	.L_t2870_e-.L_t2870
.L_t2870:
	.byte	0x1
	.4byte	.L_T816
.L_t2870_e:
.L_s2870_e:
.L_D2870_e:
.L_D2871:
	.4byte	.L_D2871_e-.L_D2871
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D2872
	.2byte	0x38
	.string	"sas_first_int_user"
	.2byte	0x72
	.4byte	.L_T898
	.2byte	0x23
	.2byte	.L_l2871_e-.L_l2871
.L_l2871:
	.byte	0x3
	.4byte	sas_first_int_user
.L_l2871_e:
.L_D2871_e:
	.previous
	.local	sas_overrun
	.comm	sas_overrun,12,4

.section	.debug
.L_D2872:
	.4byte	.L_D2872_e-.L_D2872
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D2873
	.2byte	0x38
	.string	"sas_overrun"
	.2byte	0x72
	.4byte	.L_T900
	.2byte	0x23
	.2byte	.L_l2872_e-.L_l2872
.L_l2872:
	.byte	0x3
	.4byte	sas_overrun
.L_l2872_e:
.L_D2872_e:
	.previous
	.local	sas_msi_noise
	.comm	sas_msi_noise,4,4

.section	.debug
.L_D2873:
	.4byte	.L_D2873_e-.L_D2873
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D2874
	.2byte	0x38
	.string	"sas_msi_noise"
	.2byte	0x55
	.2byte	0x9
	.2byte	0x23
	.2byte	.L_l2873_e-.L_l2873
.L_l2873:
	.byte	0x3
	.4byte	sas_msi_noise
.L_l2873_e:
.L_D2873_e:
	.previous

.section	.debug
.L_D2874:
	.4byte	.L_D2874_e-.L_D2874
	.2byte	0x1
	.2byte	0x12
	.4byte	.L_D2875
	.set	.L_T902,.L_D2874
	.2byte	0xa3
	.2byte	.L_s2874_e-.L_s2874
.L_s2874:
	.byte	0x0
	.2byte	0x7
	.4byte	0x0
	.4byte	0xf
	.byte	0x8
	.2byte	0x55
	.2byte	0x9
.L_s2874_e:
.L_D2874_e:
.L_D2875:
	.4byte	.L_D2875_e-.L_D2875
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D2876
	.2byte	0x38
	.string	"sas_baud"
	.2byte	0x72
	.4byte	.L_T902
	.2byte	0x23
	.2byte	.L_l2875_e-.L_l2875
.L_l2875:
	.byte	0x3
	.4byte	sas_baud
.L_l2875_e:
.L_D2875_e:
	.previous

.section	.debug
.L_D2876:
	.4byte	.L_D2876_e-.L_D2876
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D2877
	.2byte	0x38
	.string	"sas_hbaud"
	.2byte	0x72
	.4byte	.L_T902
	.2byte	0x23
	.2byte	.L_l2876_e-.L_l2876
.L_l2876:
	.byte	0x3
	.4byte	sas_hbaud
.L_l2876_e:
.L_D2876_e:
	.previous

.section	.debug
.L_D2877:
	.4byte	.L_D2877_e-.L_D2877
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D2878
	.2byte	0x38
	.string	"sas_speeds"
	.2byte	0x72
	.4byte	.L_T902
	.2byte	0x23
	.2byte	.L_l2877_e-.L_l2877
.L_l2877:
	.byte	0x3
	.4byte	sas_speeds
.L_l2877_e:
.L_D2877_e:
	.previous

.section	.debug
.L_D2878:
	.4byte	.L_D2878_e-.L_D2878
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D2879
	.2byte	0x38
	.string	"sas_ctimes"
	.2byte	0x72
	.4byte	.L_T902
	.2byte	0x23
	.2byte	.L_l2878_e-.L_l2878
.L_l2878:
	.byte	0x3
	.4byte	sas_ctimes
.L_l2878_e:
.L_D2878_e:
	.previous

.section	.debug
.L_D2879:
	.4byte	.L_D2879_e-.L_D2879
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D2880
	.2byte	0x38
	.string	"sas_bsizes"
	.2byte	0x72
	.4byte	.L_T902
	.2byte	0x23
	.2byte	.L_l2879_e-.L_l2879
.L_l2879:
	.byte	0x3
	.4byte	sas_bsizes
.L_l2879_e:
.L_D2879_e:
	.previous

.section	.debug
.L_D2880:
	.4byte	.L_D2880_e-.L_D2880
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D2881
	.2byte	0x38
	.string	"sas_open_modes"
	.2byte	0x72
	.4byte	.L_T902
	.2byte	0x23
	.2byte	.L_l2880_e-.L_l2880
.L_l2880:
	.byte	0x3
	.4byte	sas_open_modes
.L_l2880_e:
.L_D2880_e:
	.previous

.section	.debug
.L_D2881:
	.4byte	.L_D2881_e-.L_D2881
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D2882
	.2byte	0x38
	.string	"sass_info"
	.2byte	0x72
	.4byte	.L_T680
	.2byte	0x23
	.2byte	.L_l2881_e-.L_l2881
.L_l2881:
	.byte	0x3
	.4byte	sass_info
.L_l2881_e:
.L_D2881_e:
	.previous

.section	.debug
.L_D2882:
	.4byte	.L_D2882_e-.L_D2882
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D2883
	.2byte	0x38
	.string	"sas_rinit"
	.2byte	0x72
	.4byte	.L_T665
	.2byte	0x23
	.2byte	.L_l2882_e-.L_l2882
.L_l2882:
	.byte	0x3
	.4byte	sas_rinit
.L_l2882_e:
.L_D2882_e:
	.previous

.section	.debug
.L_D2883:
	.4byte	.L_D2883_e-.L_D2883
	.2byte	0xc
	.2byte	0x12
	.4byte	.L_D2884
	.2byte	0x38
	.string	"sas_winit"
	.2byte	0x72
	.4byte	.L_T665
	.2byte	0x23
	.2byte	.L_l2883_e-.L_l2883
.L_l2883:
	.byte	0x3
	.4byte	sas_winit
.L_l2883_e:
.L_D2883_e:
	.previous

.section	.debug_pubnames
	.4byte	.L_P26
	.string	"sasinfo"
	.previous

.section	.debug
.L_P26:
.L_D2884:
	.4byte	.L_D2884_e-.L_D2884
	.2byte	0x7
	.2byte	0x12
	.4byte	.L_D2885
	.2byte	0x38
	.string	"sasinfo"
	.2byte	0x72
	.4byte	.L_T451
	.2byte	0x23
	.2byte	.L_l2884_e-.L_l2884
.L_l2884:
	.byte	0x3
	.4byte	sasinfo
.L_l2884_e:
.L_D2884_e:
	.previous

.section	.debug
.L_D2885:
	.4byte	.L_D2885_e-.L_D2885
	.align 4
.L_D2885_e:
.L_D2:
	.previous

.section	.text
.L_text_e:
	.previous

.section	.data
.L_data_e:
	.previous

.section	.rodata
.L_rodata_e:
	.previous

.section	.bss
.L_bss_e:
	.previous

.section	.line
.L_line_last:
	.4byte	0x0
	.2byte	0xffff
	.4byte	.L_text_e-.L_text_b
.L_line_e:
	.previous

.section	.debug_srcinfo
	.4byte	.L_line_last-.L_line_b
	.4byte	0xffffffff
	.previous

.section	.debug_pubnames
	.4byte	0x0
	.string	""
	.previous

.section	.debug_aranges
	.4byte	.L_text_b
	.4byte	.L_text_e-.L_text_b
	.4byte	.L_data_b
	.4byte	.L_data_e-.L_data_b
	.4byte	.L_rodata_b
	.4byte	.L_rodata_e-.L_rodata_b
	.4byte	.L_bss_b
	.4byte	.L_bss_e-.L_bss_b
	.4byte	0x0
	.4byte	0x0
	.previous
	.ident	"GCC: (GNU) 2.5.6"
