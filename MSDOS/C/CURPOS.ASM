; CURPOS:  Move the cursor to specified location (depends on mode)
; J. J. Gillogly
; Uses Lattice C calling conventions
;
PGROUP	GROUP	PROG
PROG	SEGMENT	BYTE PUBLIC 'PROG'
	PUBLIC	CURPOS
	ASSUME	CS:PGROUP
;
; name		curpos -- set cursor location
;
; synopsis	curpos(x, y)
;		int x,y;	x and y coordinates of destination
;
; description	use video interrupt to go to (x,y) on screen
;
CURPOS	PROC	NEAR
	PUSH	BP	; LATTICE return conventions
	MOV	BP,SP	; LOOK FOR ARGS
;
	MOV	DL,[BP+4]	; FIRST ARG INTO DL, NEW ROW (Y)
	MOV	DH,[BP+6]	; 2ND ARG INTO DH, NEW COLUMN (X)
	MOV	AH,2		; BIOS FUNCTION: SET CURSOR POSITION
	MOV	BH,0		; DISPLAY PAGE, 0 FOR GRAPHICS MODE
	INT	10H		; EXECUTE THE VIDEO INTERRUPT
;
	POP	BP		; RESTORE LATTICE INFO
	RET
CURPOS	ENDP
;
PROG	ENDS
	END
