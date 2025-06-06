{************************************************************************
*									*
*  NCSDEF								*
*									*
*  Declarations for NCS$ (National Character Set) Utility routines.	*
*									*
************************************************************************}


[ASYNCHRONOUS] FUNCTION NCS$COMPARE (
	Cs_Id : UNSIGNED ;
	String_1 : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	String_2 : [CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR )
		: INTEGER ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION NCS$CONVERT (
	Cf_Id : INTEGER ;
	Source : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	%DESCR Dest : VARYING [$L3] OF CHAR ;
	VAR Ret_Length : $UWORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION NCS$END_CF (
	VAR Cf_Id : INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION NCS$END_CS (
	VAR Cs_Id : INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION NCS$GET_CF (
	VAR Cf_Id : INTEGER ;
	Cfname : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 ;
	Librar : [CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION NCS$GET_CS (
	VAR Cs_Id : INTEGER ;
	Csname : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 ;
	Librar : [CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION NCS$RESTORE_CF (
	VAR Cf_Id : INTEGER ;
	LENGTH  : UNSIGNED := %IMMED 0 ;
	ADDRESS : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION NCS$RESTORE_CS (
	VAR Cs_Id : INTEGER ;
	LENGTH  : UNSIGNED := %IMMED 0 ;
	ADDRESS : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION NCS$SAVE_CF (
	Cf_Id : INTEGER ;
	VAR LENGTH  : UNSIGNED := %IMMED 0 ;
	VAR ADDRESS : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION NCS$SAVE_CS (
	Cs_Id : INTEGER ;
	VAR LENGTH  : UNSIGNED := %IMMED 0 ;
	VAR ADDRESS : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;
