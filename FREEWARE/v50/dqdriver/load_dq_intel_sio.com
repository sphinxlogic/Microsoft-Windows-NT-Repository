$!
$!  Copyright 2000  Compaq Computer Corporation
$!
$! To manually load the DQ (IDE ATAPI CD-ROM) driver on a system with 
$! the Intel SIO (Intel SIO 82378), you can use the following command:
$!
$!      $ MC SYSMAN
$!      SYSMAN> IO CONNECT DQA0: /DRIVER=SYS$DQDRIVER -
$!                               /CSR=%X1F0 /Adap=x /Node=n /Vector=y
$!
$! This command assumes (or requires):
$!
$!      Adapter is the ISA adapter, found from the TR number of
$!              the ISA/PCI adapter from SYSMAN IO SHOW BUS.
$!
$!      Node is the slot number into which the board was plugged.
$!
$!      CSR is %X1F0, this is the standard address for a primary IDE
$!
$!      Vector is IRQ*4; IRQs are usually 14 (0x0e) and 15 (0x0F),
$!              so vectors are usually 56 and 60.
$!
$
$
$ START:
$  HW_MODEL = F$GetSyI( "HW_MODEL" )
$  SYSTEM = ""
$  IF ( (HW_MODEL .GE. 1554) .AND. (HW_MODEL .LE. 1557) ) THEN SYSTEM = "MX5"
$  IF ( (HW_MODEL .GE. 1702) .AND. (HW_MODEL .LE. 1715) ) THEN SYSTEM = "MX5"
$  IF ( (HW_MODEL .GE. 1790) .AND. (HW_MODEL .LE. 1792) ) THEN SYSTEM = "GL"
$  IF (SYSTEM .GTS. "") THEN GOTO GET_P1
$  WRITE SYS$OUTPUT "%This CPU HW_MODEL is not supported by this command file:"
$  WRITE SYS$OUTPUT "  HW_MODEL: ", HW_MODEL
$  WRITE SYS$OUTPUT "  HW_NAME:  ", F$GetSyI("HW_NAME")
$  WRITE SYS$OUTPUT ""
$  EXIT 01
$
$ 
$ GET_P1:
$  DRIVE = ""
$  IF (P1 .GTS. "") THEN GOTO PARSE_P1
$  INQUIRE P1 "Which drive to load [DQA0:]"
$  IF (P1 .EQS. "") THEN P1="DQA0:"
$ PARSE_P1:
$  P1 = P1 - ":"
$  IF (P1 .EQS. "DQA0") THEN DRIVE = P1
$  IF (P1 .EQS. "DQA1") THEN DRIVE = P1
$  IF (P1 .EQS. "DQB0") THEN DRIVE = P1
$  IF (P1 .EQS. "DQB1") THEN DRIVE = P1
$  IF (DRIVE .GTS. "") THEN GOTO DISPATCHER
$  WRITE SYS$OUTPUT "%Unknown drive"
$  EXIT 01
$
$
$ DISPATCHER:
$  DESTINATION = "LOAD_" + SYSTEM + "_" + DRIVE
$  GOTO 'DESTINATION'
$  STOP
$
$
$!
$ LOAD_MX5_DQA0:
$  MCR SYSMAN
IO CONNECT DQA0 /DRIVER=SYS$DQDRIVER -
                /ADAPTER=4 -
                /NODE=6 -
                /CSR=%X1F0 -
                /VECTOR=56
EXIT
$  EXIT 01
$
$
$ LOAD_MX5_DQA1:
$  MCR SYSMAN
IO CONNECT DQA1 /DRIVER=SYS$DQDRIVER -
                /ADAPTER=4 -
                /NODE=6 -
                /CSR=%X1F0 -
                /VECTOR=56
EXIT
$  EXIT 01
$
$
$ LOAD_MX5_DQB0:
$  MCR SYSMAN
IO CONNECT DQB0 /DRIVER=SYS$DQDRIVER -
                /ADAPTER=4 -
                /NODE=6 -
                /CSR=%X170 -
                /VECTOR=60
EXIT
$  EXIT 01
$
$
$ LOAD_MX5_DQB1:
$  MCR SYSMAN
IO CONNECT DQB1 /DRIVER=SYS$DQDRIVER -
                /ADAPTER=4 -
                /NODE=6 -
                /CSR=%X170 -
                /VECTOR=60
EXIT
$  EXIT 01
$
$
$!
$ LOAD_GL_DQA0:
$  MCR SYSMAN
IO CONNECT DQA0 /DRIVER=SYS$DQDRIVER -
                /ADAPTER=3 -
                /NODE=%X39 -
                /CSR=%X1F0 -
                /VECTOR=56
EXIT
$  EXIT 01
$
$
$ LOAD_GL_DQA1:
$  MCR SYSMAN
IO CONNECT DQA1 /DRIVER=SYS$DQDRIVER -
                /ADAPTER=3 -
                /NODE=%X39 -
                /CSR=%X1F0 -
                /VECTOR=56
EXIT
$  EXIT 01
$
$
$ LOAD_GL_DQB0:
$  MCR SYSMAN
IO CONNECT DQB0 /DRIVER=SYS$DQDRIVER -
                /ADAPTER=3 -
                /NODE=%X3A -
                /CSR=%X170 -
                /VECTOR=60
EXIT
$  EXIT 01
$
$
$ LOAD_GL_DQB1:
$  MCR SYSMAN
IO CONNECT DQB1 /DRIVER=SYS$DQDRIVER -
                /ADAPTER=3 -
                /NODE=%X3A -
                /CSR=%X170 -
                /VECTOR=60
EXIT
$  EXIT 01
$
$
