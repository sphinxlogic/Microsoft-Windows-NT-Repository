{************************************************************************
*									*
*  SORDEF								*
*									*
*  Declarations for SOR$ (Sort) Utility routines.			*
*									*
************************************************************************}


[ASYNCHRONOUS] FUNCTION SOR$BEGIN_MERGE (
	%REF Key_Buffer : PACKED ARRAY [$L1..$U1:INTEGER] OF $UWORD := %IMMED 0;
	Lrl : $UWORD := %IMMED 0 ;
	Options : Mask_Longword := %IMMED 0 ;
	Merge_Order : $UBYTE := %IMMED 0 ;
	[ASYNCHRONOUS] FUNCTION User_Compare : INTEGER  := %IMMED 0 ;
	[ASYNCHRONOUS] FUNCTION User_Equal   : UNSIGNED := %IMMED 0 ;
	[ASYNCHRONOUS] FUNCTION User_Input   : UNSIGNED := %IMMED 0 ;
	VAR Context : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SOR$BEGIN_SORT (
	%REF Key_Buffer : PACKED ARRAY [$L1..$U1:INTEGER] OF $UWORD := %IMMED 0;
	Lrl : $UWORD := %IMMED 0 ;
	Options : Mask_Longword := %IMMED 0 ;
	File_Alloc : UNSIGNED := %IMMED 0 ;
	[ASYNCHRONOUS] FUNCTION User_Compare : INTEGER  := %IMMED 0 ;
	[ASYNCHRONOUS] FUNCTION User_Equal   : UNSIGNED := %IMMED 0 ;
	Sort_Process : $UBYTE := %IMMED 0 ;
	Work_Files : $UBYTE := %IMMED 0 ;
	VAR Context : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SOR$DTYPE (
	VAR Context : [VOLATILE] UNSIGNED := %IMMED 0 ;
	VAR Dtype_Code : [VOLATILE] $UWORD ;
	Usage : UNSIGNED ;
	P1 : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SOR$END_SORT (
	VAR Context : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SOR$PASS_FILES (
	Inp_Desc :
		[CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR := %IMMED 0 ;
	Out_Desc :
		[CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 ;
	Org : $UBYTE := %IMMED 0 ;
	Rfm : $UBYTE := %IMMED 0 ;
	Bks : $UBYTE := %IMMED 0 ;
	Bls : $UWORD := %IMMED 0 ;
	Mrs : $UWORD := %IMMED 0 ;
	Alq : UNSIGNED := %IMMED 0 ;
	Fop : Mask_Longword := %IMMED 0 ;
	Fsz : $UBYTE := %IMMED 0 ;
	VAR Context : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SOR$RELEASE_REC (
	Desc : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	VAR Context : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SOR$RETURN_REC (
	%DESCR Desc : VARYING [$L1] OF CHAR ;
	VAR LENGTH  : [VOLATILE] $UWORD := %IMMED 0 ;
	VAR Context : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SOR$SORT_MERGE (
	VAR Context : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SOR$SPEC_FILE (
	Spec_File :
		[CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR := %IMMED 0 ;
	Spec_Buffer :
		[CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 ;
	VAR Context : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SOR$STAT (
	Code : $UWORD ;
	VAR Result  : [VOLATILE] UNSIGNED ;
	VAR Context : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

