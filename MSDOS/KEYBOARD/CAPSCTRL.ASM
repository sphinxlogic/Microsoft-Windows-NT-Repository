From:  Hess@MIT-Multics.ARPA
Subject:  capsctrl.asm
  
;         CapsCtrl.asm
;
;         This tiny tsr makes the "caps-lock" key act like the "Ctrl" key on
;         the IBM 101-key keyboards.
;         To get a real caps-lock, type shift+caps-lock.
;
;         Warning: this one MUST be loaded before any other TSR's that replace
;         the keyboard BIOS call!
;
code_seg  segment
          assume  CS:code_seg
          org     100H

old_int   label dword
begin:    jmp       short init
          dw 0
upcode    db 80H+3AH

; Int 15H points here:
bint:     cmp       AH,4FH              ; is this the "bios" keyboard interrupt?
          jnz       bint0               ; no, act normal.
          cmp       AL,3AH              ; is it the "caps-lock" key
          je        bint1
          cmp       AL,80H+3AH          ; is it the "release" key?
          jne       bint0
          xchg      al,upcode
          jmp       short bint2
bint0:    jmp       [old_int]
bint1:    push      ES
          push      AX
          xor       AX,AX
          mov       ES,AX
          test      byte ptr ES:[417H],1011B      ; see if Alt or Shift
          pop       AX
          pop       ES
          jnz       bint0
          mov       AL,1DH              ; turn into ctrl key
          mov       upcode,80H+1DH
bint2:    stc                                     ; tell "bios" to use this new code
          iret

;--- end of TSR portion ---

     assume CS:code_seg,DS:code_seg
init:     xor       AX,AX
          mov       ES,AX
          mov       AX,ES:[54H]; copy old int pointer
          mov       word ptr old_int,AX
          mov       AX,ES:[56H]
          mov       word ptr old_int[2],AX
          cli
          mov       AX,offset bint
          mov       ES:[54H],AX
          mov       AX,CS
          mov       ES:[56H],AX
          sti
          mov       DX,offset init
          int       27H

code_seg  ends

     end begin
