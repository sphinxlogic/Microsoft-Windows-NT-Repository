{************************************************************************
*									*
*  CONVDEF								*
*									*
*  Declarations for CONV$ (Convert/Reclaim) Utility routines.		*
*									*
*  These routines cannot be called from AST level.			*
*									*
************************************************************************}


FUNCTION CONV$CONVERT (
	%REF Status_Block_Address :
		PACKED ARRAY [$L1..$U1:INTEGER] OF UNSIGNED := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

FUNCTION CONV$PASS_FILES (
	Input_Filespec  : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	Output_Filespec : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	Fdl_Filespec :
		[CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR := %IMMED 0 ;
	Exception_Filespec :
		[CLASS_S] PACKED ARRAY [$L4..$U4:INTEGER] OF CHAR := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

FUNCTION CONV$PASS_OPTIONS (
	%REF Parameter_List_Address :
		PACKED ARRAY [$L1..$U1:INTEGER] OF UNSIGNED := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

FUNCTION CONV$RECLAIM (
	Input_Filespec : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%REF Statistics_Blk :
		PACKED ARRAY [$L2..$U2:INTEGER] OF UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

