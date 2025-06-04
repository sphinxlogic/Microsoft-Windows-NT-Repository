{************************************************************************
*									*
*  SYSDEF								*
*									*
*  Declarations for System Service routines not already in STARLET.	*
*									*
************************************************************************}


[ASYNCHRONOUS] FUNCTION SYS$RMSRUNDWN (
	%DESCR Buf_Addr : VARYING [$L1] OF CHAR ;
	%IMMED Type_Value : $UBYTE )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SYS$SETDDIR (
	New_Dir_Addr :
		[CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR := %IMMED 0 ;
	VAR Length_Addr : [VOLATILE] $UWORD := %IMMED 0 ;
	%STDESCR Cur_Dir_Addr :
		PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SYS$SETDFPROT (
	New_Def_Prot_Addr : $UWORD := %IMMED 0 ;
	VAR Cur_Def_Prot_Addr : [VOLATILE] $UWORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

