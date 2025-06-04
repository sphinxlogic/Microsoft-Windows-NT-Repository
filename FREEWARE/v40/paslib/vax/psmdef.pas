{************************************************************************
*									*
* PSMDEF								*
*									*
* Declarations for PSM$ (Print Symbiont Modification) Utility routines.	*
*									*
************************************************************************}


[ASYNCHRONOUS] FUNCTION PSM$PRINT (
	Streams : UNSIGNED := %IMMED 0 ;
	Bufsiz  : UNSIGNED := %IMMED 0 ;
	Worksiz : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PSM$READ_ITEM_DX (
	Request_Id : [UNSAFE] UNSIGNED ;
	VAR Item   : [VOLATILE] UNSIGNED ;	{ Documentation WRONG ??!! }
	%DESCR Buffer : VARYING [$L3] OF CHAR )	{       "         "   !!!! }
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PSM$REPLACE (
	Code : UNSIGNED ;
	%IMMED [ASYNCHRONOUS,UNBOUND] FUNCTION Routine : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PSM$REPORT (
	Request_Id : UNSIGNED ;
	VAR STATUS : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

