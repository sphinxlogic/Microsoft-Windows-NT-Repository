Date: 4 December 89, 09:53:09 EDT
From: Antonio@garcon.cso.uiuc.edu, Quezada-Duarte@garcon.cso.uiuc.edu,
Subject: Disabling CTRL-ALT-DEL

   Marcelo H. Ang Jr in his letter dated Nov 11th 1989 asks for a program
  to disable CTRl+ALT+DEL, well.., here it is, i've used it on my
  TANDY 1000A with no problems and in PS/2's 25 and 50 and had no
  problems at all either.

  To use it just type, from the dos prompt:

  MASM NOCAD; (of course you need MASM)
  LINK NOCAD; (and of course you need a linker)
  NOCAD

    If you type NOCAD once it has been loaded, it will tell you so.
    I've used it with DOS 3.30, but it works just as well under DOS
    2.00 and above.
    If you have any comment of any kind please let me know.
    And have a nice new decade everybody.

cut here: ************************************************************
page 255,132                           ;just to get a nice list file
comment   :
         Program to prevent CTRL+ALT+DEL from restarting the system
         WARNING: Once loaded, you only have three choices:
             1) Turn power off
             2) Use a reset button (not all the machines have one)
             3) Generate INT 19h

         WARNING: If you have a program that uses INT 0CDh, change
                  this value in the equates line below to the number
                  of a not used INT. This method is used because
                  there are too many programs that hook INT 9 Vector and
                  we can't be sure it always points to the end of our
                  notice (and start of our ISR).

         NOTE: For memory references i use parentheses instead of
               square brackets because of ASCII-EBCDIC translations.
               It works the same under Microsoft's MASM 4.0

         NOTE: NOCAD won't work if you press CTRL+ALT+DEL from
               a program that hooked to INT 9 before NOCAD
               (example: SideKick). Solution: Load NOCAD before
               everything else.

  Author     Antonio Quezada-Duarte
             Monterrey, Nuevo Leon, MEXICO
             Monterrey's Technologic Institute
             ID No 296641
             Bitnet Address   AL296641@TECMTYVM
                              BL296641@TECMTYVM

             Feel free to share this with anyone while
             leaving the copyright notice intact.

             If you have any comment please let me know,thanks.   :



ctrl equ  0100b                        ;bit map for ctrl key
alt  equ  1000b                        ;bit map for alt key
free_vector equ 0CDh                   ;vector used to prevent double loading
                                       ;change it if your system uses INT 0CDh
   nocad segment byte 'CODE'
    assume cs:nocad, ds:msgs, es:nothing, ss:stack
      Copyright db 'Antonio Quezada-Duarte ITESM ISC 296641 Monterrey '
                db 'Nuevo Leon MEXICO'
      Cright_Len equ $-Offset Copyright
    new_int_9h proc near
      push ax
      push ds                          ;save registers
      xor ax,ax
      mov ds,ax                        ;point DS to BIOS data area
                                       ;well, actually BIOS data area
                                       ;starts at 0040:0000, but
                                       ; XOR AX,AX is faster than MOV AX,40h
      mov al,ds:(417h)                 ;get keyboard flags
      and al,ctrl+alt                  ;clear non relevant bits
      cmp al,ctrl+alt                  ;compare to our map
      jne go_ahead                     ;NO CTRL+ALT keys pressed
      and byte ptr ds:(417h),not alt   ;CTRL+ALT pressed
                                       ;clear ALT key bit to simulate
                                       ;ALT key is not pressed
go_ahead:
      pushf                            ;old ISR returns with IRET
      COMMENT :
         The Following code stands for

             CALL OLD_INT_9
                Where OLD_INT_9 is a FAR PROC
                this is faster than having the address of OLD_INT_9
                stored in memory and doing a
                CALL CS:(OLD_INT_9)
                                               :
      DB 9Ah
         OLD_INT_9_OFS DW 0
         OLD_INT_9_SEG DW 0            ;call old INT 9 ISR

      pop ds
      pop ax                           ;restore registers
      iret                             ;return to caller
    new_int_9h endp
       begin proc near
          push es                      ;save psp base address
          mov dx,seg msgs
          mov ds,dx
          mov dx,offset msg_0
          mov ah,9
          int 21h
          mov ax,3500h + free_vector
          int 21h
          mov di,bx                    ;ES:DI ===> start of INT 0CDh ISR
          mov si,offset copyright
          mov ax,cs
          mov ds,ax                    ;DS:SI ===> THIS code copyright notice
          mov cx,cright_len
          cld
          repe cmpsb                   ;compare
          je loaded                    ;if equal then already loaded
          mov ax,2500h + free_vector
          mov dx,cs
          mov ds,dx
          mov dx,offset copyright
          int 21h                      ;point free_vector INT vector to
                                       ;our copyright notice
          mov ax,3509h
          int 21h                      ;get pointer to INT 9 ISR
          mov cs:old_int_9_ofs,bx
          mov cs:old_int_9_seg,es      ;put it IN the new INT 9 ISR
          mov ax,2509h
          mov dx,offset new_int_9h
          push cs
          pop ds
          int 21h                      ;point INT 9 vector to our ISR
          mov dx,seg msgs
          mov ds,dx
          mov dx,offset msg_1
          mov ah,9
          int 21h                      ;print loaded msg
          pop ds                       ;get saved psp base address
          mov es,ds:(2Ch)
          mov ah,49h
          int 21h                      ;free environment's memory
                                       ;assume no error
          mov dx,offset begin          ;everything up to BEGIN
          add dx,10Fh                  ;and all the bytes needed to
          mov cl,4                     ;make a full paragraph ...
          shr dx,cl
          mov ax,3100h                 ;... stay resident
          int 21h                      ;and return exit code = 0
loaded:   pop ax                       ;get psp address out of stack
                                       ;any register will do
          mov dx,seg msgs
          mov ds,dx                    ;point DS to our data area
          mov dx,offset msg_2
          mov ah,9
          int 21h                      ;print already loaded msg
          mov ax,4C01h
          int 21h                      ;terminate with exit code = 1
       begin endp
       nocad ends
       msgs segment word 'DATA'
          msg_0 db 10,13,'NOCAD: Prevent CTRL+ALT+DEL from restarting the '
                db 'system',10,13,'Author: Antonio Quezada-Duarte',10,13,'$'
          msg_1 db 10,13,'NOCAD Loaded OK.',10,13,'$'
          msg_2 db 10,13,'NOCAD Already Loaded.',10,13,'$'
       msgs ends
       stack segment para stack 'STACK'
          dw 1024 dup (?)
       stack ends
       end  begin

------------------------------

