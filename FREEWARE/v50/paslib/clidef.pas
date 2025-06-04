{************************************************************************
*									*
*  CLIDEF								*
*									*
*  Declarations for CLI$ (Command Line Interface) Utility routines.	*
*									*
************************************************************************}


[ASYNCHRONOUS] FUNCTION CLI$DCL_PARSE (
	Command_String :
		[CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR := %IMMED 0 ;
	%IMMED [ASYNCHRONOUS,UNBOUND] PROCEDURE Table ;
	%IMMED [ASYNCHRONOUS,UNBOUND]
		FUNCTION Param_Routine  : INTEGER := %IMMED 0 ;
	%IMMED [ASYNCHRONOUS,UNBOUND]
		FUNCTION Prompt_Routine : INTEGER := %IMMED 0 ;
	Prompt_String :
		[CLASS_S] PACKED ARRAY [$L5..$U5:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION CLI$DISPATCH (
	%IMMED Userarg : [UNSAFE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION CLI$GET_VALUE (
	Entity_Desc : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%DESCR Retdesc : VARYING [$L2] OF CHAR ;
	VAR Retlength : [VOLATILE] $UWORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION CLI$PRESENT (
	Entity_Desc : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

