{************************************************************************
*									*
*  LBRDEF								*
*									*
*  Declarations for LBR$ (Librarian) Utility routines.			*
*									*
************************************************************************}


[ASYNCHRONOUS] FUNCTION LBR$CLOSE (
	Library_Index : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$DELETE_DATA (
	Library_Index : UNSIGNED ;
	%REF Txtrfa : PACKED ARRAY [$L2..$U2:INTEGER] OF UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

{************************************************************************
 ****								     ****
 ****  LBR$DELETE_KEY may take a Key_Name parameter which is one of  ****
 ****  two distinctly different types - ASCII or Binary.  These two  ****
 ****  cases are distinguished by two separately named routines.     ****
 ****								     ****
 ************************************************************************}

[ASYNCHRONOUS,EXTERNAL (LBR$DELETE_KEY)] FUNCTION LBR$DELETE_KEY_A (
	Library_Index : UNSIGNED ;
	Key_Name : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS,EXTERNAL (LBR$DELETE_KEY)] FUNCTION LBR$DELETE_KEY_B (
	Library_Index : UNSIGNED ;
	Key_Name : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$FIND (
	Library_Index : UNSIGNED ;
	%REF Txtrfa : PACKED ARRAY [$L2..$U2:INTEGER] OF UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$FLUSH (
	Library_Index : UNSIGNED ;
	%IMMED Block_Type : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$GET_HEADER (
	Library_Index : UNSIGNED ;
	%REF Retary : PACKED ARRAY [$L2..$U2:INTEGER] OF INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$GET_HELP (
	Library_Index : UNSIGNED ;
	Line_Width : INTEGER := %IMMED 0 ;
	%IMMED [ASYNCHRONOUS,UNBOUND] FUNCTION Routine : UNSIGNED := %IMMED 0 ;
	VAR Data : [VOLATILE] UNSIGNED := %IMMED 0 ;
	Key_1_Key_10 :
	   [CLASS_S,LIST] PACKED ARRAY [$L5..$U5:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$GET_HISTORY (
	Library_Index : UNSIGNED ;
	[ASYNCHRONOUS] PROCEDURE Action_Routine )	{ ## ????? ## }
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$GET_INDEX (
	Library_Index : UNSIGNED ;
	Index_Number  : UNSIGNED ;
	%IMMED [ASYNCHRONOUS,UNBOUND] FUNCTION Routine_Name : UNSIGNED ;
	Match_Desc :
		[CLASS_S] PACKED ARRAY [$L4..$U4:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$GET_RECORD (
	Library_Index : UNSIGNED ;
	%DESCR Inbufdes  : VARYING [$L2] OF CHAR := %IMMED 0 ;
	%DESCR Outbufdes : VARYING [$L3] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$INI_CONTROL (
	VAR Library_Index : [VOLATILE] UNSIGNED ;
	Func : UNSIGNED ;
	_Type : UNSIGNED := %IMMED 0 ;
	%REF Namblk : NAM$TYPE := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

{************************************************************************
 ****								     ****
 ****  LBR$INSERT_KEY may take a Key_Name parameter which is one of  ****
 ****  two distinctly different types - ASCII or Binary.  These two  ****
 ****  cases are distinguished by two separately named routines.     ****
 ****								     ****
 ************************************************************************}

[ASYNCHRONOUS,EXTERNAL (LBR$INSERT_KEY)] FUNCTION LBR$INSERT_KEY_A (
	Library_Index : UNSIGNED ;
	%DESCR Key_Name : VARYING [$L2] OF CHAR ;
	%REF Txtrfa : PACKED ARRAY [$L3..$U3:INTEGER] OF UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS,EXTERNAL (LBR$INSERT_KEY)] FUNCTION LBR$INSERT_KEY_B (
	Library_Index : UNSIGNED ;
	VAR Key_Name : [VOLATILE] UNSIGNED ;
	%REF Txtrfa : PACKED ARRAY [$L3..$U3:INTEGER] OF UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

{************************************************************************
 ****								     ****
 ****  LBR$LOOKUP_KEY may take a Key_Name parameter which is one of  ****
 ****  two distinctly different types - ASCII or Binary.  These two  ****
 ****  cases are distinguished by two separately named routines.     ****
 ****								     ****
 ************************************************************************}

[ASYNCHRONOUS,EXTERNAL (LBR$LOOKUP_KEY)] FUNCTION LBR$LOOKUP_KEY_A (
	Library_Index : UNSIGNED ;
	Key_Name : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	%REF Txtrfa : PACKED ARRAY [$L3..$U3:INTEGER] OF UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS,EXTERNAL (LBR$LOOKUP_KEY)] FUNCTION LBR$LOOKUP_KEY_B (
	Library_Index : UNSIGNED ;
	Key_Name : UNSIGNED ;
	%REF Txtrfa : PACKED ARRAY [$L3..$U3:INTEGER] OF UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$OPEN (
	Library_Index : UNSIGNED ;
	Fns : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 ;
	%REF Create_Options :
		PACKED ARRAY [$L3..$U3:INTEGER] OF UNSIGNED := %IMMED 0 ;
	Dns : [CLASS_S] PACKED ARRAY [$L4..$U4:INTEGER] OF CHAR := %IMMED 0 ;
	Rlfna : NAM$TYPE := %IMMED 0 ;
	%DESCR Rns : VARYING [$L6] OF CHAR := %IMMED 0 ;
	VAR Rnslen : [VOLATILE] INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$OUTPUT_HELP (
	%IMMED [ASYNCHRONOUS,UNBOUND] FUNCTION Output_Routine : UNSIGNED ;
	Output_Width : INTEGER := %IMMED 0 ;
	Line_Desc :
		[CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR := %IMMED 0 ;
	Library_Name :
		[CLASS_S] PACKED ARRAY [$L4..$U4:INTEGER] OF CHAR := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	%IMMED [ASYNCHRONOUS,UNBOUND]
		FUNCTION Input_Routine : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$PUT_END (
	Library_Index : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$PUT_HISTORY (
	Library_Index : UNSIGNED ;
	Record_Desc : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$PUT_RECORD (
	Library_Index : UNSIGNED ;
	Bufdes : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	%REF Txtrfa : PACKED ARRAY [$L3..$U3:INTEGER] OF UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

{************************************************************************
 ****								     ****
 ****  LBR$REPLACE_KEY may take a Key_Name parameter which is one of ****
 ****  two distinctly different types - ASCII or Binary.  These two  ****
 ****  cases are distinguished by two separately named routines.     ****
 ****								     ****
 ************************************************************************}

[ASYNCHRONOUS,EXTERNAL (LBR$REPLACE_KEY)] FUNCTION LBR$REPLACE_KEY_A (
	Library_Index : UNSIGNED ;
	Key_Name : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	%REF Oldrfa : PACKED ARRAY [$L3..$U3:INTEGER] OF UNSIGNED ;
	%REF Newrfa : PACKED ARRAY [$L4..$U4:INTEGER] OF UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS,EXTERNAL (LBR$REPLACE_KEY)] FUNCTION LBR$REPLACE_KEY_B (
	Library_Index : UNSIGNED ;
	Key_Name : UNSIGNED ;
	%REF Oldrfa : PACKED ARRAY [$L3..$U3:INTEGER] OF UNSIGNED ;
	%REF Newrfa : PACKED ARRAY [$L4..$U4:INTEGER] OF UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$RET_RMSSTV : UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$SEARCH (
	Library_Index : UNSIGNED ;
	Index_Number  : UNSIGNED ;
	%REF Rfa_To_Find : PACKED ARRAY [$L3..$U3:INTEGER] OF UNSIGNED ;
	%IMMED [ASYNCHRONOUS,UNBOUND] FUNCTION Routine_Name : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$SET_INDEX (
	Library_Index : UNSIGNED ;
	Index_Number  : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$SET_LOCATE (
	Library_Index : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$SET_MODULE (
	Library_Index : UNSIGNED ;
	%REF Rfa : PACKED ARRAY [$L2..$U2:INTEGER] OF UNSIGNED ;
	%DESCR Bufdesc : VARYING [$L3] OF CHAR := %IMMED 0 ;
	VAR Buflen : [VOLATILE] INTEGER := %IMMED 0 ;
	Updatedesc :
		[CLASS_S] PACKED ARRAY [$L5..$U5:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LBR$SET_MOVE (
	Library_Index : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

