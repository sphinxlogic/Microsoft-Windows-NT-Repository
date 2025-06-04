$!
$!************************************************************************
$!
$!
$ MACRO/MIGRATION/NOTIE/noop/lis=SYSPDUMP/mach /OBJ=SYSPDUMP -
        ARCH_DEFS + -
	IMGDMPDEF + -
        SYSPDUMP + -
        SYS$LIBRARY:STARLET/LIB + -
        SYS$LIBRARY:LIB/LIB
$!
$!
$ IF F$SEARCH("sys.olb") .EQS. "" THEN LIBR/ALPHA/CREA/OBJ SYS.OLB
$!
$ LIBR/ALPHA/REPLA SYS.OLB SYSPDUMP
$!
$!************************************************************************
$!
$ IF F$SEARCH("chf_subs.mar") .EQS. "" THEN GOTO 100
$!
$ MACRO/MIGRATION/NOTIE/noop/lis=CHF_SUBS/mach /OBJ=CHF_SUBS -
	        ARCH_DEFS + -
		IMGDMPDEF + -
	        CHF_SUBS  + -
	        SYS$LIBRARY:STARLET/LIB + -
	        SYS$LIBRARY:LIB/LIB
$!
$ 100:
$!
$ LIBR/ALPHA/REPLA SYS.OLB CHF_SUBS
$!
$!************************************************************************
$!
$ IF F$SEARCH("doinit.mar") .EQS. "" THEN GOTO 200
$!
$ MACRO/MIGRATION/NOTIE/lis=DOINIT/mach /OBJ=DOINIT -
	        ARCH_DEFS + -
	        DOINIT + -
	        SYS$LIBRARY:STARLET/LIB + -
	        SYS$LIBRARY:LIB/LIB
$!
$ 200:
$!
$ LIBR/ALPHA/REPLA SYS.OLB DOINIT
$!
$!************************************************************************
$!
$ IF F$SEARCH("sys$system:macro64.exe") .EQS. "" THEN GOTO 300
$!
$ MACRO/ALPHA/lis=DO_PDUMP/obj=DO_PDUMP/noop ARCH_DEFS.MAR + DO_PDUMP.M64
$!
$ 300:
$!
$ LIBR/ALPHA/REPLA SYS.OLB DO_PDUMP
$!
$!************************************************************************
$!
$!
$ LINK/ALPHA/NATIVE_ONLY/BPAGE=14/SECTION/REPLACE/VMS_EXEC/NODEMAND_ZERO -
	/NOTRACEBACK/SHARE=SYSPDUMP/MAP=SYSPDUMP/FULL/CROSS -
	/SYMBOL=SYSPDUMP SYSPDUMP/OPTION
$!
$!
$ copy SYSPDUMP.EXE,.stb SYS$COMMON:[SYS$LDR]
$!
