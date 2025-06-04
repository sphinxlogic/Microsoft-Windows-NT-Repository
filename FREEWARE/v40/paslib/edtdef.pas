{************************************************************************
*									*
*  EDTDEF								*
*									*
*  Declarations for EDT$ (Callable EDT) Utility routines.		*
*									*
************************************************************************}


FUNCTION EDT$EDIT (
	In_File : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	Out_File :
		[CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 ;
	Com_File :
		[CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR := %IMMED 0 ;
	Jou_File :
		[CLASS_S] PACKED ARRAY [$L4..$U4:INTEGER] OF CHAR := %IMMED 0 ;
	Options : Mask_Longword := %IMMED 0 ;
	[ASYNCHRONOUS] FUNCTION Fileio : UNSIGNED := %IMMED 0 ;
	[ASYNCHRONOUS] FUNCTION Workio : UNSIGNED := %IMMED 0 ;
	[ASYNCHRONOUS] FUNCTION Xlate  : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

FUNCTION EDT$FILEIO (
	Code : UNSIGNED ;
	Stream : UNSIGNED ;
	%DESCR _Record : VARYING [$L3] OF CHAR ;
	%DESCR Rhb     : VARYING [$L4] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

FUNCTION EDT$WORKIO (
	Code : UNSIGNED ;
	Recordno : INTEGER ;
	%STDESCR _Record : PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

FUNCTION EDT$XLATE (
	%DESCR String : VARYING [$L1] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

