$!
$! 	Compile and link PQUOTA.MAR
$!
$ IF F$GETSYI("ARCH_TYPE") .EQ. 2 THEN GOTO AXP
$!
$! 	We are on a VAX
$!
$ MACRO PQUOTA + SYS$LIBRARY:LIB/LIB
$ LINK  PQUOTA, SYS$SYSTEM:SYS.STB/SELECTIVE/NOTRACE
$!
$ GOTO EXIT
$!
$! 	We are on an AXP
$!
$ AXP:
$!
$ MACRO/MIGRATION/NOTIE/WARN=NOINFO/OBJ=PQUOTA -
        ARCH_DEFS + -
        PQUOTA	  + -
	SYS$LIBRARY:STARLET/LIB + -
        SYS$LIBRARY:LIB/LIB
$!
$ LINK/ALPHA/NATIVE_ONLY/SYSEXE=SELECTIVE  PQUOTA
$!
$ EXIT:
$!
