Comment "

   Resident program to report intermittant disk I/O errors, before DOS
   reports them after trying 5 times.  Error handling remains with DOS.

   Errors reported:
        No Response     No response from disk
        Failed Seek     Could not locate data
        NEC Error       Controller error
        Bad CRC Seen    Circular Redundancy Check error
        DMA overrun     CPU too busy to allow data on bus (unusual)
        Impos Sector    NEC tried to read non existent sector
        No Addr Mark    No address mark on disk
        W Protected     Write protected disk
        Err Unknown     Severe problem; NEC does not know what happened

   Note: some copy protected disks will show disk errors, which are part of
   the scheme.

   Author Steve Holzner, as published in PC Magazine Vol. 4 No. 12, p. 263.

   To produce a .com file
      [m]asm dskwatch
      link   dskwatch   (Ignore missing stack segment warning.)
      exe2bin dskwatch.exe dskwatch.com

   Program may be invoked in autoexec.bat for a permanent watchdog.  Once
   started, can only be removed by re-booting.
"

INTERRUPTS      SEGMENT AT 0H  ;This is where the disk interrupt
        ORG     13H*4          ;holds the address of its service routine
DISK_INT        LABEL   DWORD
INTERRUPTS      ENDS
 
SCREEN  SEGMENT AT 0B000H     ;A dummy segment to use as the Extra Segment
SCREEN  ENDS                  ; so we can write to the screen
 
CODE_SEG        SEGMENT
        ASSUME  CS:CODE_SEG
        ORG     100H          ;ORG = 100H to make this into a .COM file
FIRST:  JMP     LOAD_WATCH    ;First time through jump to initialize routine
 
        MSG_PART_1      DB    'Disk Error: ' ;Here are the error messages
        MSG_PART_2      DB    'No response Failed Seek NEC Error   '
                        DB    'Bad CRC SeenDMA Overrun Impos Sector'
                        DB    'No Addr MarkW. ProtectedErr Unknown '
        FIRST_POSITION  DW    ?              ;Position of 1st char on screen
        FLAGS           DW    ?
        SCREEN_SEG_OFFSET     DW      0      ;0 for mono, 8000H for graphics
        OLD_DISK_INT          DD      ?      ;Location of old disk interrupt
        RET_ADDR              LABEL DWORD    ;Used in fooling around with
        RET_ADDR_WORD         DW 2 DUP(?)    ; stack.
 
DISK_WATCH      PROC    FAR          ;The Disk interrupt will now come here
        ASSUME  CS:CODE_SEG
        PUSHF                   ;First, call old disk interrupt
        CALL    OLD_DISK_INT
        PUSHF                   ;Save the flags in memory location "FLAGS"
        POP     FLAGS           ; (cunning name)
        JC      ERROR           ;If there was an error, carry flag will have
        JMP     FIN             ; been set by Disk interrupt
ERROR:  PUSH    AX              ;AH has the status of the error
        PUSH    CX              ;Push all used registers for politeness
        PUSH    DX
        PUSH    DI
        PUSH    SI
        PUSH    ES
        LEA     SI,MSG_PART_1   ;Always print "Disk Error: " part.
        ASSUME  ES:SCREEN             ;Use screen as extra segment
        MOV     DX,SCREEN
        MOV     ES,DX
        MOV     DI,SCREEN_SEG_OFFSET  ;DI will be pointer to screen position
        ADD     DI,FIRST_POSITION     ;Add to point to desired area on screen
        CALL    WRITE_TO_SCREEN  ;This writes 12 characters from [SI] to [DI]
        MOV     DH,80H                ;Initialize for later comparisons
        MOV     CX,7                  ;Loop seven times
E_LOOP: CMP     AH,DH                 ;Are error code and DH the same?
        JE      E_FOUND               ;If yes, Error has been found
        ADD     SI,12                 ;Point to next error message
        SHR     DH,1                  ;Divide DH by 2
        LOOP    E_LOOP                ;Keep going until matched    DH=0
        CMP     AH,3                  ;Error code not even number; 3 perhaps?
        JE      E_FOUND               ;If yes, have found the error
        ADD     SI,12                 ;Err unknown; unknown error returned
E_FOUND:CALL    WRITE_TO_SCREEN       ;Write the error message to screen
        POP     ES                    ;Having done Pushes, here are the Pops
        POP     SI
        POP     DI
        POP     DX
        POP     CX
        POP     AX
FIN:    POP     RET_ADDR_WORD         ;Fooling with the stack.  We want to
        POP     RET_ADDR_WORD[2]      ;preserve the flags but the old flags
        ADD     SP,2                  ;are still on the stack.  First remove
        PUSH    FLAGS                 ;return address, then flags. Fill flags
        POPF                          ;from "FLAGS", return to correct addr.
        JMP     RET_ADDR
DISK_WATCH      ENDP
 
WRITE_TO_SCREEN PROC    NEAR      ;Puts 12 characters on the screen
        MOV     CX,12             ;Loop 12 times
W_LOOP: MOVS    ES:BYTE PTR[DI],CS:[SI] ;Move to the screen
        MOV     AL,7              ;Move screen attribute into screen buffer
        MOV     ES:[DI],AL
        INC     DI                ;Point to next byte in screenbuffer
        LOOP    W_LOOP            ;Keep going until done
        RET                       ;Exeunt
WRITE_TO_SCREEN ENDP
 
LOAD_WATCH      PROC    NEAR      ;This procedure initializes everything
        ASSUME  DS:INTERRUPTS     ;The data segment will be the Interrupt area
        MOV     AX,INTERRUPTS
        MOV     DS,AX
 
        MOV     AX,DISK_INT         ;Get the old interrupt service routine
        MOV     OLD_DISK_INT,AX     ;address and put it into our location
        MOV     AX,DISK_INT[2]      ;OLD_DISK_INT so we can call it.
        MOV     OLD_DISK_INT[2],AX
 
        MOV     DISK_INT,OFFSET DISK_WATCH  ;Now load the address of Dsk Watch
        MOV     DISK_INT[2],CS              ; routine into the Disk interrupt
 
        MOV     AH,15                  ;Ask for service 15 of INT 10H
        INT     10H                    ;This tells us how display is set
        SUB     AH,25                  ;Move to twenty-five placed before edge
        SHL     AH,1                   ;Mult by two (char & attribute bytes)
        MOV     BYTE PTR FIRST_POSITION,AH      ;Set screen cursor
        TEST    AL,4                   ;Is it a monochrome display?
        JNZ     EXIT                   ;Yes - jump out
        MOV     SCREEN_SEG_OFFSET,8000H ;No - set up for graphics display
EXIT:   MOV     DX,OFFSET LOAD_WATCH   ;Set up everything but this program to
        INT     27H                    ;stay resident and attach itself to DOS
LOAD_WATCH      ENDP
        CODE_SEG        ENDS
        END     FIRST   ;END "FIRST" so 8088 will go to FIRST first
