{************************************************************************
*									*
*  ACLDEF								*
*									*
*  Declarations for ACLEDIT$ Utility routine.				*
*									*
************************************************************************}


[ASYNCHRONOUS] FUNCTION ACLEDIT$EDIT (
	%DESCR Item_List : [UNSAFE] ARRAY [$L1..$U1:INTEGER] OF $UBYTE )
		: UNSIGNED ; EXTERNAL ;

