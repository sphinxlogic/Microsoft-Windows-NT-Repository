From: ppa@hpldola.HP.COM (Paul P. Austgen)
Newsgroups: comp.sys.ibm.pc
Subject: Re: One more time....
Date: 2 Feb 89 21:30:05 GMT

I'm sure there are programs to do just exactly what you want, but this
one allows you to change any keys to anything else that you want to.
If you have access to a BB, try to find kbmap.  If yoiu have the .COM
file, you can edit the table with DEBUG and you don't have to
reassemble.  I find it especially useful to map keys on old games for
the XT over to my AT cursor keys.  I have included a source listing
below:


;*******************************************
;*                                         *
;*  KBMAP -- A program to translate scan   *
;*           codes on the PC/AT keyboard.  *
;*                                         *
;*        Written by: Alan D. Jones        *
;*                                         *
;*        Released to the public domain    *
;*        on Feb 27,1988.                  *
;*                                         *
;*******************************************

;This program must be converted to a "COM"
; file after linking.


.286c

XCODE	segment para public 'CODE'

	assume	cs:XCODE,ds:NOTHING,es:NOTHING,ss:NOTHING

	org	100h

start:	jmp	load

;This is a complete scan code translation table.
;The scan code received from the keyboard is used
; as an index into this table, and the byte found
; at "scantbl+<scan code>" is substituted and
; passed to the keyboard interrupt handler.
;CAUTION: if this table is modified, be sure to
; swap or substitute MATCHING PAIRS! Both the "make"
; code and the "break" code ( = make code + 80h )
; must be changed.

;<ctrl> key:       make = 1Dh, break = 9Dh
;<caps lock> key:  make = 3Ah, break = 0BAh
;The table as it stands exchanges the <ctrl> and
; <caps lock> key functions.

	org	104h

scantbl	DB 000H,001H,002H,003H,004H,005H,006H,007H
	DB 008H,009H,00AH,00BH,00CH,00DH,00EH,00FH
	DB 010H,011H,012H,013H,014H,015H,016H,017H
	DB 018H,019H,01AH,01BH,01CH,03AH,01EH,01FH
	DB 020H,021H,022H,023H,024H,025H,026H,027H
	DB 028H,029H,02AH,02BH,02CH,02DH,02EH,02FH
	DB 030H,031H,032H,033H,034H,035H,036H,037H
	DB 038H,039H,01DH,03BH,03CH,03DH,03EH,03FH
	DB 040H,041H,042H,043H,044H,045H,046H,047H
	DB 048H,049H,04AH,04BH,04CH,04DH,04EH,04FH
	DB 050H,051H,052H,053H,054H,055H,056H,057H
	DB 058H,059H,05AH,05BH,05CH,05DH,05EH,05FH
	DB 060H,061H,062H,063H,064H,065H,066H,067H
	DB 068H,069H,06AH,06BH,06CH,06DH,06EH,06FH
	DB 070H,071H,072H,073H,074H,075H,076H,077H
	DB 078H,079H,07AH,07BH,07CH,07DH,07EH,07FH
	DB 080H,081H,082H,083H,084H,085H,086H,087H
	DB 088H,089H,08AH,08BH,08CH,08DH,08EH,08FH
	DB 090H,091H,092H,093H,094H,095H,096H,097H
	DB 098H,099H,09AH,09BH,09CH,0BAH,09EH,09FH
	DB 0A0H,0A1H,0A2H,0A3H,0A4H,0A5H,0A6H,0A7H
	DB 0A8H,0A9H,0AAH,0ABH,0ACH,0ADH,0AEH,0AFH
	DB 0B0H,0B1H,0B2H,0B3H,0B4H,0B5H,0B6H,0B7H
	DB 0B8H,0B9H,09DH,0BBH,0BCH,0BDH,0BEH,0BFH
	DB 0C0H,0C1H,0C2H,0C3H,0C4H,0C5H,0C6H,0C7H
	DB 0C8H,0C9H,0CAH,0CBH,0CCH,0CDH,0CEH,0CFH
	DB 0D0H,0D1H,0D2H,0D3H,0D4H,0D5H,0D6H,0D7H
	DB 0D8H,0D9H,0DAH,0DBH,0DCH,0DDH,0DEH,0DFH
	DB 0E0H,0E1H,0E2H,0E3H,0E4H,0E5H,0E6H,0E7H
	DB 0E8H,0E9H,0EAH,0EBH,0ECH,0EDH,0EEH,0EFH
	DB 0F0H,0F1H,0F2H,0F3H,0F4H,0F5H,0F6H,0F7H
	DB 0F8H,0F9H,0FAH,0FBH,0FCH,0FDH,0FEH,0FFH
	
old15		dd	0

;-------------------------------------------

;Interrupt 15h function 4Fh is called (on MOST AT bios's)
; by the keyboard interrupt handler, with the scan code
; passed in the AL register. A program may trap int 15h
; function 4Fh and examine or alter the scan code BEFORE
; it is processed by the handler. If int 15h returns with
; carry set, processing proceeds. If it returns with carry
; clear, processing is skipped and the bios routine ignores
; the key press.

int15		proc	far

		pushf

;is this the scan code call?

		cmp	ah,4Fh
		je	scancode

;no, act as if we were not here

		popf
		jmp	cs:old15

;it is the scan code call, run it thru the translator

scancode:	popf
		push	ds
		push	bx
		push	cs
		pop	ds
		lea	bx,scantbl
		xlatb
		pop	bx
		pop	ds

;set carry so scan code is processed

		stc
		iret

int15		endp

;-------------------------------------------

;Initialization code -- not needed after program
; becomes memory-resident.

cutoff		label	byte

msg1		db	'This program only runs '
		db	'on PC/AT type machines.'
		db	0Dh,0Ah,24h

load:

;Make sure this is a PC/AT

		push	sp
		pop	ax
		cmp	ax,sp
		jne	wrongmachine
		mov	ax,0F000h
		mov	es,ax
		cmp	byte ptr es:[0FFFEh],0FCh
		jne	wrongmachine


;Get old interrupt vector

		mov	ax,3515h
		int	21h
		mov	word ptr old15,bx
		mov	word ptr old15+2,es

;Install new interrupt vector

		mov	ax,2515h
		mov	dx,offset int15
		int	21h

;Terminate and stay resident

		mov	ax,3100h
		lea	dx,cutoff
		add	dx,0Fh
		shr	dx,4
		int	21h

;Display "wrong machine" error message and exit

wrongmachine:	lea	dx,msg1
		mov	ah,9
		int	21h
		mov	ax,4C01h
		int	21h

XCODE	ends
	end	start
