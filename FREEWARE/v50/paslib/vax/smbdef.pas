{************************************************************************
*									*
* SMBDEF								*
*									*
* Declarations for SMB$ (Symbiont/JobCTL Interface) Utility routines.	*
*									*
************************************************************************}


[ASYNCHRONOUS] FUNCTION SMB$CHECK_FOR_MESSAGE : UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMB$INITIALIZE (
	Structure_Level : UNSIGNED ;
	%IMMED [ASYNCHRONOUS,UNBOUND] PROCEDURE Ast_Routine := %IMMED 0 ;
	Streams : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMB$READ_MESSAGE (
	VAR Stream : [VOLATILE] UNSIGNED ;
	%DESCR Buffer : VARYING [$L2] OF CHAR ;
	VAR Request : [VOLATILE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMB$READ_MESSAGE_ITEM (
	Message : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	Context : UNSIGNED ;
	VAR Item_Code : [VOLATILE] UNSIGNED ;
	%DESCR Buffer : VARYING [$L4] OF CHAR ;
	VAR SIZE : [VOLATILE] $UWORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMB$SEND_TO_JOBCTL (
	Stream  : UNSIGNED ;
	Request : UNSIGNED := %IMMED 0 ;
	Accounting :
		[CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR := %IMMED 0 ;
	Checkpoint :
		[CLASS_S] PACKED ARRAY [$L4..$U4:INTEGER] OF CHAR := %IMMED 0 ;
	Device_Status : UNSIGNED := %IMMED 0 ;
	%REF Error : PACKED ARRAY [$L6..$U6:INTEGER] OF UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

