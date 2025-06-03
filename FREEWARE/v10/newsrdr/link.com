$ V = 'F$VERIFY(0)
$!
$! LINK.COM
$!   Creates NEWSRDR.EXE from object code.
$!
$! Copyright © 1993, 1994  MadGoat Software.  All rights reserved.
$!
$ IF F$GETSYI("HW_MODEL") .GE. 1024
$ THEN
$   OLB = "ALPHA_OLB"
$   OBJ = "ALPHA_OBJ"
$   QUAL = "/NONATIVE_ONLY"
$   AXP = 1
$ ELSE
$   OLB = "OLB"
$   OBJ = "OBJ"
$   QUAL = ""
$   AXP = 0
$ ENDIF
$!
$ IF AXP
$ THEN
$   VER = F$EDIT(F$GETSYI("VERSION"),"TRIM")
$   IF F$EXTRACT(0,4,VER) .EQS. "V1.5"
$   THEN
$   	TYPE SYS$INPUT:

>>> PLEASE NOTE:

    This system is running OpenVMS AXP V1.5.  If you have not done so
    already, get CSC patch #378 (DEC C RTL patch) from Digital customer
    support.  This patch is required for NEWSRDR to run properly; without
    it, NEWSRDR will ACCVIO whenever a condition is signalled.

    If you have DSNlink, you can get this patch through DSNLINK VTX.

<<<

$   ENDIF
$ ENDIF
$!
$!  Only ask if they aren't running CMU-Tek...
$!
$ OPTION = " "
$ IF F$GETDVI("IP0:", "EXISTS")
$ THEN
$   CMU = F$GETDVI("IP0:","DEVNAM") .EQS. "_IP0:"
$ ELSE
$   CMU = 0
$ ENDIF
$ IF F$TRNLNM("NETLIB_DIR") .NES. "" .AND. .NOT. CMU
$ THEN
$   TYPE SYS$INPUT:

    NETLIB appears to be installed on your system.  You may link NEWSRDR
    against NETLIB, or use the socket library interface appropriate to
    your TCP/IP package.

    NOTE: If you want to use the socket library interface, you may need
    to recompile SERVER_SOCKET.C.  Look at the comments at the beginning
    of SERVER_SOCKET.C for information on compiling that module.

    Enter N to link against NETLIB, S to link against the socket library,
    or Q to quit this procedure.

$!
$GETOPT:
$   READ/END=BYEBYE/ERR=BYEBYE SYS$COMMAND OPTION -
    	/PROMPT="[N]ETLIB, [S]ocket, [Q]uit: "
$   OPTION = F$EDIT(OPTION,"TRIM,UPCASE")
$   IF OPTION .EQS. "" THEN GOTO GETOPT
$   OPTION = F$EXTRACT(0,1,OPTION)
$   IF OPTION .EQS. "Q" THEN GOTO BYEBYE
$   IF OPTION .NES. "N" .AND. OPTION .NES. "S" THEN GOTO GETOPT
$ ENDIF
$!
$ CREATE SYS$SCRATCH:NEWSRDR.OPT
$ OPEN/APPEND X SYS$SCRATCH:NEWSRDR.OPT
$ WRITE X "PSECT_ATTR=$$$COPYRIGHT,NOWRT"
$ WRITE X "COLLECT=CLUSTER1,$$$COPYRIGHT"
$ IF CMU .OR. (OPTION .EQS. "N") THEN GOTO USE_NETLIB
$ SS = "SERVER_SOCKET"
$!
$! MultiNet?
$!
$ IF F$TRNLNM("MULTINET_SOCKET_LIBRARY") .NES. ""
$ THEN
$   WRITE X "MULTINET_SOCKET_LIBRARY/SHARE"
$   GOTO SOCKET_COMMON
$ ENDIF
$!
$! UCX?
$!
$ IF F$SEARCH("SYS$LIBRARY:UCX$IPC_SHR.EXE") .NES. "" 
$ THEN
$   IF AXP
$   THEN
$   	SS = "SERVER_SOCKET_UCX"
$   ELSE
$   	WRITE X "SYS$LIBRARY:UCX$IPC.OLB/LIB"
$   ENDIF
$   GOTO SOCKET_COMMON
$ ENDIF
$!
$! TCPware (>= V3.0) ?
$!
$ IF F$TRNLNM ("TCPWARE") .NES. ""
$ THEN
$   img = F$PARSE ("TCPWARE_SOCKLIB_SHR","SYS$SHARE:.EXE")
$   WRITE X "''img'/SHARE"
$   GOTO SOCKET_COMMON
$ ENDIF
$!
$! TCPware?
$!
$ IF F$TRNLNM("TCPIP_ROOT") .NES. ""
$ THEN
$   WRITE X "TCPIP_ROOT:SOCKLIB.OLB/LIB"
$   GOTO SOCKET_COMMON
$ ENDIF
$!
$! WIN/TCP?
$!
$ IF F$TRNLNM("TWG$COMMON") .NES. ""
$ THEN
$   IF AXP
$   THEN
$   	WRITE X "SYS$SHARE:TWGLIB.EXE/SHARE"
$   ELSE
$   	WRITE X "TWG$COMMON:[NETDIST.LIB]TWGLIB.OLB/LIB"
$   ENDIF
$   GOTO SOCKET_COMMON
$ ENDIF
$!
$ IF F$TRNLNM("NETLIB_DIR") .NES."" THEN GOTO Use_NETLIB
$!
$ WRITE SYS$OUTPUT "Could not identify socket library to be used."
$ WRITE SYS$OUTPUT "Please edit LINK.COM appropriately and try again."
$ CLOSE X
$ DELETE SYS$SCRATCH:NEWSRDR.OPT;*
$ EXIT 1
$!
$! Common for all
$!
$SOCKET_COMMON:
$ IF .NOT. AXP THEN WRITE X "SYS$SHARE:VAXCRTL/SHARE"
$ CLOSE X
$ SET VERIFY
$ LINK/EXEC=NEWSRDR.EXE/NOTRACE'QUAL' NEWSRDR.'OLB'/INCLUDE=NEWSRDR/LIB,-
   	'SS'.'OBJ',SYS$SCRATCH:NEWSRDR.OPT/OPT
$!'F$VERIFY(0)
$!
$ DELETE SYS$SCRATCH:NEWSRDR.OPT;*
$!
$ EXIT 1+0*F$VERIFY(V)
$!
$!------- The following is for linking with NETLIB.
$!
$Use_NETLIB:
$!
$ WRITE X "NETLIB_DIR:NETLIB_SHRXFR/SHARE"
$ IF .NOT. AXP THEN WRITE X "SYS$SHARE:VAXCRTL/SHARE"
$ CLOSE X
$ SET VERIFY
$ LINK/EXEC=NEWSRDR.EXE/NOTRACE'QUAL' NEWSRDR.'OLB'/INCLUDE=NEWSRDR/LIB,-
    	SERVER_NETLIB.'OBJ',SYS$SCRATCH:NEWSRDR.OPT/OPT
$!'F$VERIFY(0)
$!
$ DELETE SYS$SCRATCH:NEWSRDR.OPT;*
$!
$BYEBYE:
$ EXIT 1+0*F$VERIFY(V)
