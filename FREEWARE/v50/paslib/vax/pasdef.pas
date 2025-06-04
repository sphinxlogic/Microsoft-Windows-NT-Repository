{************************************************************************
*									*
*  PASDEF								*
*									*
*  Declarations for miscellaneous Pascal routines.			*
*									*
*  Declarations for PAS$MARK2 and PAS$RELEASE2 are not included as	*
*  their use is not recommended in Pascal.				*
*									*
************************************************************************}


[HIDDEN] TYPE

Unsafe_File = [UNSAFE] FILE OF CHAR ;
Ptr_To_Fab  = ^FAB$TYPE ;
Ptr_To_Rab  = ^RAB$TYPE ;


[ASYNCHRONOUS] FUNCTION PAS$FAB (
	VAR F : Unsafe_File )
		: Ptr_To_Fab ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PAS$RAB (
	VAR F : Unsafe_File )
		: Ptr_To_Rab ; EXTERNAL ;

