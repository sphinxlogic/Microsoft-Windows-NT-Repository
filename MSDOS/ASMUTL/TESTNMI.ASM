comment *	TEST_NMI     Copyright (c) 1984 by Lawrence B. Afrin

		TEST_NMI is a demonstration program that shows how
		an NMI interrupt is somehow (?) invoked when NMI
		interrupts are enabled after having been disabled on
		the IBM PC.  This source code should be run through
		the assembler, the linker, and finally EXE2BIN.  Then,
		when it is run, the user will see a capital letter A
		show up in the first column of roughly the fifth line
		of the monochrome video screen.  This shows that the
		NMI interrupt routine was invoked.  This program was
		developed on one of the original (1981) models of the
		IBM PC.  I do not know whether this "bug" exists on
		later models.  -- LBA, 12/30/84
*

biosseg	segment	at 0
	org	2h*4		; locate the address of the NMI interrupt
int_2	label	dword		; vector
biosseg	ends

cseg	segment
	assume	cs:cseg
	org	100h		; this will be a .COM file

doit	proc	near
	jmp	start		; jump around new NMI interrupt handler

new_nmi:	push	bx		; set up to address monochrome video
	push	es		; screen memory at segment B000 hex
	mov	bx,0b000h
	mov	es,bx
	mov	bx,80*2*5	; set up to address line 5, column 1
	mov	byte ptr es:[bx],'A'	; display an 'A' there
	pop	es		; exit from this interrupt "handler"
	pop	bx
	iret

	assume	ds:biosseg	; code to install new NMI handler:

start:	mov	ax,biosseg	; set up to address interrupt vector area
	mov	ds,ax

; NOTE: The following two instructions are the real kicker to this
; program.  As noted above in the header comments, it is the re-
; enabling of the NMI interrupts FOLLOWING THE DISABLING OF THEM
; that causes the spurious NMI interrupt to be signalled.  If we were
; to remove the following two instructions, then the re-enabling
; of an already enabled NMI interrupt would have no effect.

	mov	al,00		; temporarily mask off NMI interrupts
	out	0a0h,al
	mov	word ptr int_2,offset new_nmi	; set up address of new NMI
	mov	int_2[2],cs			; interrupt handler routine

; NOTE: This is where the funny business happens.  The IBM PC Technical
; Reference Manual says that to enable NMI interrupts, an 80 hex should
; be output to port A0 hex.  The manual doesn't give any indication that
; this operation provokes the system into signalling an NMI interrupt,
; but that is indeed what appears to happen when the following OUT
; instruction is executed.

	mov	al,80h	; turn NMI interrupts back on
	out	0a0h,al

	mov	dx,offset start	; if we get here (fat chance),
	int	27h			; exit and leave new NMI handler
					; resident

doit	endp

cseg	ends

	end	doit

