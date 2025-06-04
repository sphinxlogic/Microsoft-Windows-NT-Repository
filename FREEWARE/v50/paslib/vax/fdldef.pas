{************************************************************************
*									*
*  FDLDEF								*
*									*
*  Declarations for FDL$ (File Definition Language) Utility routines.	*
*									*
*  These routines cannot be called from AST level.			*
*									*
************************************************************************}


FUNCTION FDL$CREATE (
	Fdl_Desc : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	Filename :
		[CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 ;
	Default_Name :
		[CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR := %IMMED 0 ;
	%STDESCR Result_Name :
		PACKED ARRAY [$L4..$U4:INTEGER] OF CHAR := %IMMED 0 ;
	%REF Fid_Block :
		PACKED ARRAY [$L5..$U5:INTEGER] OF UNSIGNED := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	VAR Stmnt_Num : [VOLATILE] UNSIGNED := %IMMED 0 ;
	VAR Retlen : [VOLATILE] UNSIGNED := %IMMED 0 ;
	VAR Sts : [VOLATILE] UNSIGNED := %IMMED 0 ;
	VAR Stv : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

FUNCTION FDL$GENERATE (
	Flags : Mask_Longword ;
	Fab_Pointer : [UNSAFE] UNSIGNED ;
	Rab_Pointer : [UNSAFE] UNSIGNED ;
	Fdl_File_Dst :
		[CLASS_S] PACKED ARRAY [$L4..$U4:INTEGER] OF CHAR := %IMMED 0 ;
	%STDESCR Fdl_File_Resnam :
		PACKED ARRAY [$L5..$U5:INTEGER] OF CHAR := %IMMED 0 ;
	%STDESCR Fdl_Str_Dst :
		PACKED ARRAY [$L6..$U6:INTEGER] OF CHAR := %IMMED 0 ;
	VAR Bad_Blk_Addr : [VOLATILE,UNSAFE] UNSIGNED := %IMMED 0 ;
	VAR Retlen : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

FUNCTION FDL$PARSE (
	Fdl_Spec : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	VAR Fdl_Fab_Pointer : [VOLATILE,UNSAFE] UNSIGNED ;
	VAR Fdl_Rab_Pointer : [VOLATILE,UNSAFE] UNSIGNED ;
	Flags : Mask_Longword := %IMMED 0 ;
	Dflt_Fdl_Spc :
		[CLASS_S] PACKED ARRAY [$L5..$U5:INTEGER] OF CHAR := %IMMED 0 ;
	VAR Stmnt_Num : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

FUNCTION FDL$RELEASE (
	Fab_Pointer : [UNSAFE] UNSIGNED := %IMMED 0 ;
	Rab_Pointer : [UNSAFE] UNSIGNED := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	VAR Badblk_Addr : [VOLATILE,UNSAFE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

