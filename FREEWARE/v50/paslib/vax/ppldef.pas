{************************************************************************
*									*
*  PPLDEF								*
*									*
*  Declarations for PPL$ (Parallel Processing) Utility routines.	*
*									*
************************************************************************}


[ASYNCHRONOUS] FUNCTION PPL$ADJUST_QUORUM (
	Barrier_Id : UNSIGNED ;
	Amount : $WORD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$AWAIT_EVENT (
	Event_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$CREATE_BARRIER (
	VAR Barrier_Id : [VOLATILE] UNSIGNED ;
	Barrier_Name :
		[CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 ;
	Quorum : $WORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$CREATE_EVENT (
	VAR Event_Id : [VOLATILE] UNSIGNED ;
	Event_Name :
		[CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$CREATE_SEMAPHORE (
	VAR Semaphore_Id : [VOLATILE] UNSIGNED ;
	Semaphore_Name :
		[CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 ;
	Semaphore_Maximum : $WORD := %IMMED 0 ;
	Semaphore_Initial : $WORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$CREATE_SHARED_MEMORY (
	Section_Name :
		[CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%REF Memory_Area : ARRAY [$L2..$U2:INTEGER] OF UNSIGNED ;
	%IMMED Flags : Mask_Longword := %IMMED 0 ;
	FILE_NAME :
		[CLASS_S] PACKED ARRAY [$L4..$U4:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$CREATE_SPIN_LOCK (
	VAR Lock_Id : [VOLATILE] UNSIGNED ;
	Lock_Name :
		[CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$CREATE_VM_ZONE (
	VAR Zone_Id : [VOLATILE] UNSIGNED ;
	Algorithm : INTEGER := %IMMED 0 ;
	Algorithm_Argument : INTEGER := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	Extend_Size : INTEGER := %IMMED 0 ;
	Initial_Size : INTEGER := %IMMED 0 ;
	Block_Size : INTEGER := %IMMED 0 ;
	Alignment : INTEGER := %IMMED 0 ;
	Page_Limit : INTEGER := %IMMED 0 ;
	Smallest_Block_Size : INTEGER := %IMMED 0 ;
	Zone_Name :
		[CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$DECREMENT_SEMAPHORE (
	Semaphore_Id : UNSIGNED ;
	Flags : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$DELETE_SHARED_MEMORY (
	Section_Name : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%REF Memory_Area : ARRAY [$L2..$U2:INTEGER] OF UNSIGNED ;
	%IMMED Flags : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$ENABLE_EVENT_AST (
	Event_Id : UNSIGNED ;
	[ASYNCHRONOUS] PROCEDURE Astadr ;
	%IMMED Astprm : [UNSAFE] UNSIGNED := %IMMED 0 )		{ ## ?????? ## }
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$ENABLE_EVENT_SIGNAL (
	Event_Id : UNSIGNED ;
	%IMMED Signal_Value : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$FIND_SYNCH_ELEMENT_ID (
	VAR Element_Id : [VOLATILE] UNSIGNED ;
	Element_Name : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$FLUSH_SHARED_MEMORY (
	Section_Name : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%REF Memory_Area : ARRAY [$L2..$U2:INTEGER] OF UNSIGNED := %IMMED 0 ;
	%IMMED Flags : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$GET_INDEX (
	VAR Participant_Index : [VOLATILE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$INCREMENT_SEMAPHORE (
	Semaphore_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$INDEX_TO_PID (
	Participant_Index : UNSIGNED ;
	VAR Pid : [VOLATILE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$INITIALIZE (
	SIZE : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$PID_TO_INDEX (
	Pid : UNSIGNED ;
	VAR Participant_Index : [VOLATILE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$READ_BARRIER (
	Barrier_Id  : UNSIGNED ;
	VAR Quorum  : [VOLATILE] $WORD ;
	VAR Waiters : [VOLATILE] $WORD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$READ_EVENT (
	Event_Id : UNSIGNED ;
	VAR Occurred : [VOLATILE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$READ_SEMAPHORE (
	Semaphore_Id : UNSIGNED ;
	VAR Semaphore_Value : [VOLATILE] $WORD := %IMMED 0 ;
	VAR Semaphore_Maximum : [VOLATILE] $WORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$RELEASE_SPIN_LOCK (
	Lock_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$SEIZE_SPIN_LOCK (
	Lock_Id : UNSIGNED ;
	Flags : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$SET_QUORUM (
	Barrier_Id  : UNSIGNED ;
	Quorum : $WORD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$SPAWN (
	Copies : UNSIGNED ;
	Program_Name :
		[CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 ;
	%REF Children_Ids : ARRAY [$L3..$U3:INTEGER] OF UNSIGNED := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	Std_Input_File :
		[CLASS_S] PACKED ARRAY [$L5..$U5:INTEGER] OF CHAR := %IMMED 0 ;
	Std_Output_File :
		[CLASS_S] PACKED ARRAY [$L6..$U6:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$STOP (
	Participant_Index : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$TERMINATE (
	Flags : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$TRIGGER_EVENT (
	Event_Id : UNSIGNED ;
	%IMMED Event_Param : [UNSAFE] UNSIGNED := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$UNIQUE_NAME (
	Name_String : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%DESCR Resultant_String : VARYING [$L2] OF CHAR ;
	VAR Resultant_Length : [VOLATILE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION PPL$WAIT_AT_BARRIER (
	Barrier_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

