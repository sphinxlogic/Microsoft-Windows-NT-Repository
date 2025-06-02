Date: Mon, 5 Feb 90 09:54:26 EST
From: "Jimmy Law" <PHYJLAW%UOGUELPH.bitnet@ugw.utcs.utoronto.ca>
Subject: AT drive problems

Re vol90/13 on AT floppy drive problem - queries from E Mueller and R Schuerger
reply.  I had the same problem when I up the crystal speed on my True Blue
AT from 6 MHz to 10 MHz.  The solution is to increase the timing loop to
wait for the Floppy controller.  Since you can't change the BIOS, only way is
to tap into the disk interrupt vector.
The following code did it for me:

DEBUG DUMP OF DISK.COM

1635:0100  EB 4C 90 00 00 00 00 3D-01 90 74 05 2E FF 2E 03   .L.....=..t.....
1635:0110  01 9C 2E FF 1E 03 01 72-11 B3 06 31 C9 F6 06 3E   .......r...3...>
1635:0120  00 80 75 06 E2 F7 FE CB-75 F3 CA 02 00 1E B8 15   ..u.....u.......
1635:0130  35 CD 21 2E 8C 06 05 01-2E 89 1E 03 01 BA 07 01   5.!.............
1635:0140  B8 15 25 CD 21 1F E8 60-00 BA 2D 01 CD 27 1E B8   ..%.!..`..-..'..
1635:0150  15 35 CD 21 1F 89 DF BE-07 01 B9 26 00 F3 A6 75   .5.!.......&...u
1635:0160  CC BA 69 01 B4 09 CD 21-C3 0D 0A 20 41 54 2D 46   ..i....!... AT-F
1635:0170  6C 6F 70 70 79 20 50 61-74 63 68 20 66 6F 72 20   loppy Patch for
1635:0180  38 20 4D 68 7A 0D 0A 20-41 6C 72 65 61 64 79 20   8 Mhz.. Already
1635:0190  69 6E 73 74 61 6C 6C 65-64 20 38 36 2F 30 35 2F   installed 86/05/
1635:01A0  33 31 20 20 20 20 0D 0A-24 BA B1 01 B4 09 CD 21   31    ..$......!
1635:01B0  C3 0D 0A 20 41 54 2D 46-6C 6F 70 70 79 20 50 61   ... AT-Floppy Pa
1635:01C0  74 63 68 20 66 6F 72 20-38 20 4D 68 7A 0D 0A 20   tch for 8 Mhz..
1635:01D0  28 63 29 20 4A 20 4C 61-77 20 55 6F 47 20 50 68   (c) J Law UoG Ph
1635:01E0  79 73 69 63 73 20 38 36-2F 30 35 2F 33 31 0D 0A   ysics 86/05/31..
1635:01F0  24 4B                                             $K

To recreate diskf.com:

a) Place the code below in a file : diskf
b) execute :  debug <diskf
c) this will give you the TSR program diskf.com
d) place it in your autoexec.bat
e) I have it working on my true Blue-AT souped up to 10MHz from 6MHz
f) The longer timing loop means there will be no" disk not ready" message
   but you will sometime notice a slightly longer wait for Floppy disk access.
g) Does not affect Hard disk access.

ENJOY!
 ----cut here-----place in file diskf----
  ndiskf2.com
  rCX
  F1
  a100
  JMP     014E
  NOP
  ADD     [BX+SI],AL
  ADD     [BX+SI],AL
  CMP     AX,9001
  JZ      0111
  CS:
  JMP     FAR [0103]
  PUSHF
  CS:
  CALL    FAR [0103]
  JB      012A
  MOV     BL,06
  XOR     CX,CX
  TEST    BYTE PTR [003E],80
  JNZ     012A
  LOOP    011D
  DEC     BL
  JNZ     011D
  RETF    0002
  PUSH    DS
  MOV     AX,3515
  INT     21
  CS:
  MOV     [0105],ES
  CS:
  MOV     [0103],BX
  MOV     DX,0107
  MOV     AX,2515
  INT     21
  POP     DS
  CALL    01A9
  MOV     DX,012D
  INT     27
  PUSH    DS
  MOV     AX,3515
  INT     21
  POP     DS
  MOV     DI,BX
  MOV     SI,0107
  MOV     CX,0026
  REPZ
  CMPSB
  JNZ     012D
  MOV     DX,0169
  MOV     AH,09
  INT     21
  RET
  OR      AX,200A
  INC     CX
  PUSH    SP
  SUB     AX,6C46
  DB      6F
  JO      01E4
  JNS     0196
  PUSH    AX
  DB      61
  JZ      01DD
  DB      68
  AND     [BP+6F],AH
  JB      01A0
  CMP     [BX+SI],AH
  DEC     BP
  DB      68
  JPE     0193
  OR      AH,[BX+SI]
  INC     CX
  DB      6C
  JB      01F1
  DB      61
  DB      64
  JNS     01B0
  DB      69
  DB      6E
  JNB     0208
  DB      61
  DB      6C
  DB      6C
  DB      65
  DB      64
  AND     [BX+SI],BH
  SS:
  DAS
  XOR     [DI],DH
  DAS
  XOR     SI,[BX+DI]
  AND     [BX+SI],AH
  AND     [BX+SI],AH
  OR      AX,240A
  MOV     DX,01B1
  MOV     AH,09
  INT     21
  RET
  OR      AX,200A
  INC     CX
  PUSH    SP
  SUB     AX,6C46
  DB      6F
  JO      022C
  JNS     01DE
  PUSH    AX
  DB      61
  JZ      0225
  DB      68
  AND     [BP+6F],AH
  JB      01E8
  CMP     [BX+SI],AH
  DEC     BP
  DB      68
  JPE     01DB
  OR      AH,[BX+SI]
  SUB     [BP+DI+29],AH
  AND     [BP+SI+20],CL
  DEC     SP
  DB      61
  JA      01FA
  PUSH    BP
  DB      6F
  INC     DI
  AND     [BX+SI+68],DL
  JNS     0255
  DB      69
  DB      63
  JNB     0206
  CMP     [302F],DH
  XOR     AX,332F
  XOR     [DI],CX
  OR      AH,[SI]
  DEC     BX
                               <--Make sure blank line here(delete this comment)
w
q
------cut here---------------

------------------------------

